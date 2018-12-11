/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

void dwc_usb3_task_schedule(struct tasklet_struct *tasklet)
{
	tasklet_schedule(tasklet);
}

/**
 * Helper routine for dwc_wait_pme_thread()
 */
static void dwc_wait_for_link_up(dwc_usb3_pcd_t *pcd)
{
	unsigned long flags;
	u32 state;
	int count = 5;

	while (count-- > 0) {
		msleep(20);
		spin_lock_irqsave(&pcd->lock, flags);
		state = dwc_usb3_pcd_get_link_state(pcd);
		pcd->link_state = state;
		if (state == DWC_LINK_STATE_U0) {
			dwc_debug(pcd->usb3_dev,
				  "Exiting from hibernation 2\n");
			pcd->usb3_dev->hiber_wait_connect = 0;

			/* Perform the steps in Section 9.1.3 "Initialization on
			 * Connect Done" in [DWSS].
			 */
			dwc_usb3_handle_connect_done_intr(pcd);
			dwc_exit_hibernation_after_connect(pcd, 0);
			count = 0;
		}

		spin_unlock_irqrestore(&pcd->lock, flags);
	}
}

/**
 * Kernel thread that monitors the usb3_dev->hibernate variable. usb3_dev->
 * hibernate is set to 1 or 2 from the interrupt handler when the core is
 * requesting to enter hibernation. This thread checks whether it is safe to
 * do so (usb3_dev->hiber_cnt == 0) and then sets usb3_dev->hibernate to 3 and
 * puts the core into hibernation. usb3_dev->hibernate >= 3 tells the rest of
 * the driver that it cannot access the hardware.
 *
 * This thread also handles some housekeeping work that must be done after the
 * core has exited from hibernation.
 *
 * Note that this is NOT the way a "real" device would enter hibernation. This
 * code is ONLY for testing hibernation on the Synopsys HAPS platform.
 */
int dwc_wait_pme_thread(void *data)
{
	dwc_usb3_pcd_t *pcd = (dwc_usb3_pcd_t *)data;
	dwc_usb3_device_t *dev = pcd->usb3_dev;
	unsigned long flags;
	u32 state, temp;
	int i;
	int cnt = 0;

	dwc_debug(dev, "%s(%p)\n", __func__, data);

	/* Allow the thread to be killed by a signal, but set the signal mask
	 * to block everything but INT, TERM, KILL, and USR1. */
	allow_signal(SIGINT);
	allow_signal(SIGTERM);
	allow_signal(SIGKILL);
	allow_signal(SIGUSR1);

	set_user_nice(current, -10);

	/* Allow the thread to be frozen */
	set_freezable();

	for (;;) {
		spin_lock_irqsave(&pcd->lock, flags);
		state = dev->hibernate;

		if (dev->hiber_cnt == 0) {
			if (state == DWC_HIBER_ENTER_NOSAVE ||
			    state == DWC_HIBER_ENTER_SAVE) {
				dwc_enter_hibernation(pcd,
						state == DWC_HIBER_ENTER_SAVE);
				state = DWC_HIBER_SLEEPING;
			}
		}

		if (state != DWC_HIBER_SLEEPING && dev->snpsid >= 0x5533230a &&
							dev->hiber_wait_u0) {
			/* Handle waiting for U0 after requesting link state
			 * RECOVERY, because we don't have the link state
			 * change event enabled. We also do this in
			 * dwc_usb3_irq() in case an event comes first.
			 */
			temp = dwc_usb3_pcd_get_link_state(pcd);
			pcd->link_state = temp;

			if (cnt-- == 0) {
				cnt = 500;
				dwc_debug1(pcd->usb3_dev,
					   "thread WAIT_U0 state=%d\n", temp);
			}

			if (temp == DWC_LINK_STATE_U0) {
				dwc_debug0(dev,
					   "thread WAIT_U0 setting speed\n");
				pcd->speed = dwc_usb3_get_device_speed(pcd);
				if (pcd->remote_wakeup_enable)
					dwc_usb3_pcd_remote_wake(pcd, 0);
				dev->hiber_wait_u0 = 0;
			}
		}

		if (state == DWC_HIBER_WAIT_LINK_UP) {
			dev->hibernate = DWC_HIBER_AWAKE;
			state = DWC_HIBER_AWAKE;
			spin_unlock_irqrestore(&pcd->lock, flags);
			dwc_wait_for_link_up(pcd);
			spin_lock_irqsave(&pcd->lock, flags);
		}

		if (state == DWC_HIBER_SS_DIS_QUIRK) {
			for (i = 0; i < 500; i++) {
				if (dev->hibernate != DWC_HIBER_SS_DIS_QUIRK) {
					dwc_info(dev,
						 "breaking loop after %d ms\n",
						 i);
					break;
				}

				spin_unlock_irqrestore(&pcd->lock, flags);
				msleep(1);
				spin_lock_irqsave(&pcd->lock, flags);
			}

			if (dev->hibernate == DWC_HIBER_SS_DIS_QUIRK) {
				temp = dwc_usb3_pcd_get_link_state(pcd);
				pcd->link_state = temp;
				if (temp == DWC_LINK_STATE_SS_DIS)
					dwc_enter_hibernation(pcd, 0);
				else
					dev->hibernate = DWC_HIBER_AWAKE;
			}
		}

		spin_unlock_irqrestore(&pcd->lock, flags);
		if (kthread_should_stop())
			break;
		msleep(1);
	}

	return 0;
}

/**
 * Fake PME interrupt handler, called from dwc_usb3_irq() in cil_intr.c if
 * the core is in hibernation and any interrupt is received. The custom PCIe
 * "gasket" on the HAPS platform generates an interrupt when the core requests
 * to exit hibernation.
 *
 * This function reads the Debug register at offset 16 in the gasket, to make
 * sure the core is requesting to exit hibernation. If so, it brings the core
 * out of hibernation.
 *
 * Note that this is NOT the way a "real" device would exit hibernation. This
 * code is ONLY for testing hibernation on the Synopsys HAPS platform.
 */
int dwc_usb3_handle_pme_intr(dwc_usb3_device_t *dev)
{
	u32 temp, test1, test2;
	u8 __iomem *base_addr;
	int ret;

	dwc_debug(dev, "%s()\n", __func__);

	if (dev->gasket_base)
		base_addr = (u8 __iomem *)dev->gasket_base;
	else
		base_addr = (u8 __iomem *)dev->base + dev->gasket_ofs;

	if ((dev->hwparams3 & DWC_HWP3_SSPHY_IFC_BITS) == 0) {
		test1 = 0x40;
		test2 = 0x80;
	} else {
		test1 = 0x44;
		test2 = 0x88;
	}

	temp = dwc_rd32(dev, (volatile u32 __iomem *)(base_addr + 16));
	if ((temp & test1) != 0) {
		dwc_debug(dev, "calling dwc_exit_hibernation() Debug=%01x\n",
			  temp);
		ret = dwc_exit_hibernation(&dev->pcd, (temp & test2) != 0);
		if (ret)
			dwc_debug(dev, "dwc_exit_hibernation() returned %d\n",
				  ret);
	}

	return 1;
}

/**
 * Routine to enable and disable power to the core. The custom PCIe "gasket" on
 * the HAPS platform provides this capability through the R1 register at offset
 * 4.
 *
 * Note that this is NOT the way a "real" device would control the power. This
 * code is ONLY for testing hibernation on the Synopsys HAPS platform.
 */
void dwc_usb3_power_ctl(dwc_usb3_device_t *dev, int on)
{
	u32 temp, test;
	u8 __iomem *base_addr;
	int cnt;

	dwc_debug(dev, "%s(%1x)\n", __func__, on);

	if (dev->gasket_base)
		base_addr = (u8 __iomem *)dev->gasket_base;
	else
		base_addr = (u8 __iomem *)dev->base + dev->gasket_ofs;

	if ((dev->hwparams3 & DWC_HWP3_SSPHY_IFC_BITS) == 0)
		test = 0x30;
	else
		test = 0x33;

	if (on) {
		temp = dwc_rd32(dev, (volatile u32 __iomem *)(base_addr + 4));
		dwc_debug(dev, "R1=%01x before write\n", temp);
		temp &= ~0x3000;
		dwc_wr32(dev, (volatile u32 __iomem *)(base_addr + 4), temp);
		temp = dwc_rd32(dev, (volatile u32 __iomem *)(base_addr + 4));
		dwc_debug(dev, "R1=%01x after write\n", temp);

		/* Wait until both PMUs confirm that they have entered D0 */
		dwc_debug(dev, "Asked for D0 state, waiting for response\n");
		cnt = 0;
		do {
			udelay(1);
			temp = dwc_rd32(dev, (volatile u32 __iomem *)
					(base_addr + 16));
			if (++cnt > 10000000) {
				cnt = 0;
				dwc_debug(dev, "%01x\n", temp);
				//break;
			}
		} while ((temp & test) != 0);
	} else {
		temp = dwc_rd32(dev, (volatile u32 __iomem *)(base_addr + 4));
		dwc_debug(dev, "R1=%01x before write\n", temp);
		temp |= 0x3000;
		dwc_wr32(dev, (volatile u32 __iomem *)(base_addr + 4), temp);
		temp = dwc_rd32(dev, (volatile u32 __iomem *)(base_addr + 4));
		dwc_debug(dev, "R1=%01x after write\n", temp);

		/* Wait until both PMUs confirm that they have entered D3 */
		dwc_debug(dev, "Asked for D3 state, waiting for response\n");

		cnt = 0;
		do {
			udelay(1);
			temp = dwc_rd32(dev, (volatile u32 __iomem *)
					(base_addr + 16));
			if (++cnt > 10000000) {
				cnt = 0;
				dwc_debug(dev, "%01x\n", temp);
				//break;
			}
		} while ((temp & test) != test);
	}
}
