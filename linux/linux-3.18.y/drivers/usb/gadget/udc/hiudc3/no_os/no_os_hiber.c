/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

/**
 * Helper routine for dwc_wait_pme_thread()
 */
static void dwc_wait_for_link_up(dwc_usb3_pcd_t *pcd)
{
	unsigned long flags;
	u32 temp;
	int count = 5;

	while (count-- > 0) {
		dwc_msleep(pcd->usb3_dev, 20);
		dwc_acquire_spinlock_irq(pcd->usb3_dev, &pcd->lock, flags);
		temp = dwc_usb3_pcd_get_link_state(pcd);
		if (temp == 0) {
			dwc_info0(pcd->usb3_dev, "Exiting from hibernation 2\n");
			pcd->usb3_dev->hiber_wait_connect = 0;

			/* Perform the steps in Section 9.1.3 "Initialization on
			 * Connect Done" in [DWSS].
			 */
			dwc_usb3_handle_connect_done_intr(pcd);
			dwc_exit_hibernation_after_connect(pcd, 0);
			count = 0;
		}

		dwc_release_spinlock_irq(pcd->usb3_dev, &pcd->lock, flags);
	}
}

/**
 * Routine that monitors the dev->hibernate variable and the 'debug'
 * register in the FPGA. dev->hibernate is set to 1 or 2 from the interrupt
 * handler when the core is requesting to enter hibernation. This routine
 * checks whether it is safe to do so (dev->hiber_cnt == 0) and then sets
 * dev->hibernate to 3 and puts the core into hibernation. dev->hibernate >= 3
 * tells the rest of the driver that it cannot access the hardware.
 *
 * When the core is in hibernation, this routine should be called periodically
 * to read the 'debug' register to see if the core is requesting to exit
 * hibernation. If so, it brings the core out of hibernation.
 *
 * Note that this is NOT the way a "real" device would enter and exit
 * hibernation. This code is ONLY for testing hibernation on the Synopsys
 * HAPS platform.
 */
int dwc_usb3_wait_pme(dwc_usb3_device_t *dev)
{
	dwc_usb3_pcd_t *pcd = &dev->pcd;
	unsigned long flags;
	u32 temp;
	int state, i;

	dwc_acquire_spinlock_irq(dev, &pcd->lock, flags);
	temp = dev->hibernate;

	if (dev->hiber_cnt == 0) {
		if (temp == DWC_HIBER_ENTER_NOSAVE || temp == DWC_HIBER_ENTER_SAVE) {
			dwc_enter_hibernation(pcd, temp == DWC_HIBER_ENTER_SAVE);
			temp = DWC_HIBER_SLEEPING;
		}
	}

	if (temp == DWC_HIBER_WAIT_LINK_UP) {
		dev->hibernate = DWC_HIBER_AWAKE;
		temp = DWC_HIBER_AWAKE;
		dwc_release_spinlock_irq(dev, &pcd->lock, flags);
		dwc_wait_for_link_up(pcd);
		dwc_acquire_spinlock_irq(dev, &pcd->lock, flags);
	}

	if (temp == DWC_HIBER_WAIT_U0) {
		state = dwc_usb3_pcd_get_link_state(pcd);
		if (state == 0) {
			dev->hibernate = DWC_HIBER_AWAKE;
			temp = DWC_HIBER_AWAKE;
		}
	}

	if (temp == DWC_HIBER_SS_DIS_QUIRK) {
		for (i = 0; i < 500; i++) {
			temp = dev->hibernate;
			if (temp != DWC_HIBER_SS_DIS_QUIRK) {
				dwc_debug1(dev, "breaking loop after %d ms\n", i);
				break;
			}

			dwc_release_spinlock_irq(dev, &pcd->lock, flags);
			dwc_msleep(dev, 1);
			dwc_acquire_spinlock_irq(dev, &pcd->lock, flags);
		}

		if (temp == DWC_HIBER_SS_DIS_QUIRK) {
			if (dwc_usb3_pcd_get_link_state(pcd) == DWC_LINK_STATE_SS_DIS) {
				dwc_enter_hibernation(pcd, 0);
				temp = DWC_HIBER_SLEEPING;
			} else {
				dev->hibernate = DWC_HIBER_AWAKE;
				temp = DWC_HIBER_AWAKE;
			}
		}
	}

	dwc_release_spinlock_irq(dev, &pcd->lock, flags);
	dwc_msleep(dev, 1);

	return temp;
}

int dwc_usb3_handle_pme_intr(dwc_usb3_device_t *dev)
{
	int ret;

	dwc_debug1(dev, "%s()\n", __func__);

	dwc_debug0(dev, "calling dwc_exit_hibernation()\n");
	ret = dwc_exit_hibernation(&dev->pcd, 1);
	if (ret)
		dwc_info1(dev, "dwc_exit_hibernation() returned %d\n", ret);

	return 1;
}

void dwc_usb3_power_ctl(dwc_usb3_device_t *dev, int on)
{
}
