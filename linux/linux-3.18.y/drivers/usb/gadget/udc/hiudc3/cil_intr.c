/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/cil_intr.c $
 * $Revision: #39 $
 * $Date: 2014/11/11 $
 * $Change: 2664766 $
 *
 * Synopsys SS USB3 Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

/** @file
 *
 * The Core Interface Layer provides basic services for accessing and
 * managing the DWC_usb3 hardware. These services are used by both the
 * Peripheral Controller Driver and the On The Go Driver.
 *
 * This file contains the common interrupt handling functions.
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

/**
 * This routine enables the Event Buffer interrupt.
 */
static void ena_eventbuf_intr(dwc_usb3_device_t *dev, int bufno)
{
	u32 eventsiz;

	eventsiz =
	      dwc_rd32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz);
	eventsiz &= ~DWC_EVENTSIZ_INT_MSK_BIT;
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz,
		 eventsiz);
}

/**
 * This routine disables the Event Buffer interrupt.
 */
static void dis_eventbuf_intr(dwc_usb3_device_t *dev, int bufno)
{
	u32 eventsiz;

	eventsiz =
	      dwc_rd32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz);
	eventsiz |= DWC_EVENTSIZ_INT_MSK_BIT;
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz,
		 eventsiz);
}

/**
 * This routine disables the Event Buffer interrupt and flushes any pending
 * events from the buffer.
 */
void dwc_usb3_dis_flush_eventbuf_intr(dwc_usb3_device_t *dev, int bufno)
{
	u32 cnt;

	if (dev->hibernate >= DWC_HIBER_SLEEPING)
		return;

	dis_eventbuf_intr(dev, bufno);
	cnt = dwc_rd32(dev, &dev->core_global_regs->geventbuf[bufno].geventcnt);
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventcnt, cnt);
}

/**
 * This routine reads the current Event Buffer count.
 */
static int get_eventbuf_count(dwc_usb3_device_t *dev, int bufno)
{
	u32 cnt;

	cnt = dwc_rd32(dev, &dev->core_global_regs->geventbuf[bufno].geventcnt);
	return cnt & DWC_EVENTCNT_CNT_BITS;
}

/**
 * This routine writes the Event Buffer count.
 */
static void update_eventbuf_count(dwc_usb3_device_t *dev, int bufno, int cnt)
{
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventcnt, cnt);
}

/**
 * This routine fetches the next event from the Event Buffer.
 */
static u32 get_eventbuf_event(dwc_usb3_device_t *dev, int bufno, int size)
{
	u32 event;

	event = *dev->event_ptr[bufno]++;
	if (dev->event_ptr[bufno] >= dev->event_buf[bufno] + size)
		dev->event_ptr[bufno] = dev->event_buf[bufno];
	return event;
}

/**
 * This routine initializes an Event Buffer.
 */
void dwc_usb3_init_eventbuf(dwc_usb3_device_t *dev, int bufno,
			    u32 *addr, unsigned int size, dwc_dma_t dma_addr)
{
	dwc_debug4(dev, "Event buf %d addr 0x%08lx phys 0x%08lx size %d\n",
		   bufno, (unsigned long)addr, (unsigned long)dma_addr, size);
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventadr_lo,
		 dma_addr & 0xffffffffU);
#ifdef DWC_64_BIT_ARCH
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventadr_hi,
		 dma_addr >> 32U & 0xffffffffU);
#else
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventadr_hi, 0);
#endif
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventsiz,
		 size << 2);
	dwc_wr32(dev, &dev->core_global_regs->geventbuf[bufno].geventcnt, 0);
}

/**
 * This routine initializes the commmon interrupts.
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
void dwc_usb3_enable_common_interrupts(dwc_usb3_device_t *dev)
{
	/* Clear any pending interrupts */
	dwc_usb3_dis_flush_eventbuf_intr(dev, 0);

	ena_eventbuf_intr(dev, 0);
}

/**
 * This routine enables the Device mode interrupts.
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
void dwc_usb3_enable_device_interrupts(dwc_usb3_device_t *dev)
{
	u32 devten;

	dwc_debug1(dev, "%s()\n", __func__);

	/* Enable global interrupts */
	dwc_usb3_enable_common_interrupts(dev);

	devten = DWC_DEVTEN_DISCONN_BIT | DWC_DEVTEN_CONNDONE_BIT |
		 DWC_DEVTEN_USBRESET_BIT | DWC_DEVTEN_HIBER_REQ_BIT |
		 DWC_DEVTEN_WKUP_BIT;

	if (dev->snpsid >= 0x5533230a)
		devten |= DWC_DEVTEN_U3_L2L1_SUSP_BIT;
	else
		devten |= DWC_DEVTEN_ULST_CHNG_BIT;

	/* Enable device interrupts */
	dwc_wr32(dev, &dev->pcd.dev_global_regs->devten, devten);

	dwc_debug2(dev, "%s() devten=%0x\n", __func__,
		   dwc_rd32(dev, &dev->pcd.dev_global_regs->devten));
}

/**
 * This routine handles all interrupt events. It is called by the
 * dwc_usb3_irq() interrupt handler routine, and by the enter_hibernation()
 * routine after clearing the Run/Stop bit and waiting for the Halted bit to
 * be set.
 *
 * @param dev   Programming view of DWC_usb3 controller.
 * return       1 if an interrupt event was seen, 0 if not.
 */
int dwc_usb3_handle_event(dwc_usb3_device_t *dev)
{
	dwc_usb3_pcd_t *pcd = &dev->pcd;
	u32 event;
	int count, intr, physep, i;
	int ret = 0;
	static int msg_cnt;

	count = get_eventbuf_count(dev, 0);
	if (count)
		dwc_debug1(dev, "Interrupt count %d\n", count);

	if ((count & DWC_EVENTCNT_CNT_BITS) ==
					(0xffffffff & DWC_EVENTCNT_CNT_BITS) ||
	    count >= DWC_EVENT_BUF_SIZE * 4) {
		if (msg_cnt > 100) {
			msg_cnt = 100;
			dwc_warn0(dev, "Too many bad events!!\n");
		} else {
			msg_cnt++;
			dwc_warn1(dev,
				"Bad event count 0x%01x in dwc_usb3_irq() !!\n",
				count);
		}

		dis_eventbuf_intr(dev, 0);
		update_eventbuf_count(dev, 0, count);
		count = 0;
	}

	if (!count)
		goto out;
	ret = 1;

#if defined(CONFIG_IPMATE) || defined(COSIM) || defined(VIRTIO_MODEL)
	dis_eventbuf_intr(dev, 0);
#endif

	for (i = 0; i < count; i += 4) {
		dwc_debug1(dev, "Event addr 0x%08lx\n",
			   (unsigned long)dev->event_ptr[0]);
		event = get_eventbuf_event(dev, 0, DWC_EVENT_BUF_SIZE);
		update_eventbuf_count(dev, 0, 4);
		if (event == 0) {
			dwc_print0(dev, "## Null event! ##\n");

			/* Ignore null events */
			continue;
		}

		//dwc_debug1(dev, "Interrupt event 0x%08x\n", event);
		if (event & DWC_EVENT_NON_EP_BIT) {
			//dwc_debug0(dev, "Non-EP interrupt event\n");
			intr = event & DWC_EVENT_INTTYPE_BITS;

			if (intr ==
			    DWC_EVENT_DEV_INT << DWC_EVENT_INTTYPE_SHIFT) {
				dwc_debug1(dev,
					   "## Device interrupt 0x%08x ##\n",
					   event);
				ret = dwc_usb3_handle_dev_intr(pcd, event);
				if (ret) {
					ret = 2;
					goto out;
				}
				ret = 1;
			} else {
				dwc_debug1(dev, "## Core interrupt 0x%08x ##\n",
					   event);

				/* @todo Handle non-Device interrupts
				 * (OTG, CarKit, I2C)
				 */
			}
		} else {
			physep = event >> DWC_DEPEVT_EPNUM_SHIFT &
				DWC_DEPEVT_EPNUM_BITS >> DWC_DEPEVT_EPNUM_SHIFT;
			dwc_debug2(dev,
				   "## Physical EP%d interrupt 0x%08x ##\n",
				   physep, event);
			dwc_debug2(dev, "[EP%d] %s\n", physep >> 1 & 0xf,
				   physep & 1 ? "IN" : "OUT");
			dwc_usb3_handle_ep_intr(pcd, physep, event);
		}
	}

#if defined(CONFIG_IPMATE) || defined(COSIM) || defined(VIRTIO_MODEL)
	ena_eventbuf_intr(dev, 0);
#endif
out:
	return ret;
}

#ifdef SSIC

static int ssic_read_attr(dwc_usb3_device_t *dev, u32 aid, u32 *aval)
{
	int i;
	u32 sevt = 0;
	u32 sctl = 0;
	u32 data = 0;

	dwc_debug(dev, "%s: aid=0x%0x\n", __func__, aid);

	sctl |= DWC_SCTL_GO_ACC_BIT;
	sctl |= (aid << DWC_SCTL_AID_SHIFT);

	dwc_debug(dev, "%s: writing sctl=0x%08x\n", __func__, sctl);
	dwc_wr32(dev, &dev->ssic_regs->sctl[0], sctl);

	i = 100000;
	do {
		sevt = dwc_rd32(dev, &dev->ssic_regs->sevt[0]);
		if (sevt & DWC_SEVT_LACC_CMPLT_BIT)
			break;
		dwc_udelay(dev, 1);
	} while (--i > 0);

	if (i == 0) {
		dwc_error(dev, "%s: lacc timeout\n", __func__);
		return -1;
	}

	data = (sevt & DWC_SEVT_RDATA_RCVD_BITS) >> DWC_SEVT_RDATA_RCVD_SHIFT;
	if (aval)
		*aval = (u8)data;

	dwc_debug(dev, "%s: rdata=%02x\n", __func__, data);
	return 0;
}

static int ssic_write_attr(dwc_usb3_device_t *dev, u32 aid, u32 aval)
{
	int i;
	u32 sctl = 0;
	u32 sevt = 0;

	dwc_debug(dev, "%s: aid=0x%0x aval=%0x\n", __func__, aid, aval);
	sctl |= (DWC_SCTL_GO_ACC_BIT | DWC_SCTL_RD_WR_N_BIT);
	sctl |= (aid << DWC_SCTL_AID_SHIFT);
	sctl |= (aval << DWC_SCTL_ADATA_SHIFT);
	dwc_debug(dev, "%s: writing sctl=0x%08x\n", __func__, sctl);
	dwc_wr32(dev, &dev->ssic_regs->sctl[0], sctl);

	i = 100000;
	do {
		sevt = dwc_rd32(dev, &dev->ssic_regs->sevt[0]);
		if (sevt & DWC_SEVT_LACC_CMPLT_BIT)
			break;
		dwc_udelay(dev, 1);
	} while (--i > 0);

	if (i == 0 ) {
		dwc_error(dev, "%s: lacc timeout\n", __func__);
		return -1;
	}

	return 0;
}

static int ssic_rrap_response(dwc_usb3_device_t *dev, int write, u32 aid, u32 aeid, u32 aval)
{
	int i;
	u32 sctl = 0;
	u32 sevt = 0;

	dwc_debug(dev, "%s: aid=0x%0x, aeid=%0x, aval=%0x, write=%d\n",
		  __func__, aid, aeid, aval, write);

	sctl |= (DWC_SCTL_GO_ACC_BIT | DWC_SCTL_RACC_BIT);
	if (write)
		sctl |= (DWC_SCTL_RD_WR_N_BIT);

	sctl |= ((aid << DWC_SCTL_AID_SHIFT) & DWC_SCTL_AID_BITS);
	sctl |= ((aeid << DWC_SCTL_EAID_SHIFT) & DWC_SCTL_EAID_BITS);
	sctl |= ((aval << DWC_SCTL_ADATA_SHIFT) & DWC_SCTL_ADATA_BITS);

	dwc_debug(dev, "%s: writing sctl=0x%08x\n", __func__, sctl);
	dwc_wr32(dev, &dev->ssic_regs->sctl[0], sctl);

	i = 100000;
	do {
		sevt = dwc_rd32(dev, &dev->ssic_regs->sevt[0]);
		if (sevt & DWC_SEVT_RCMD_RES_SENT_BIT)
			break;
		dwc_udelay(dev, 1);
	} while(--i > 0);

	if (i == 0) {
		dwc_error(dev, "%s: racc response send timeout\n", __func__);
		return -1;
	}

	dwc_debug(dev, "%s: racc response sent result=0x%0x\n", __func__,
		(sevt & DWC_SEVT_RACC_RESULT_BITS) >> DWC_SEVT_RACC_RESULT_SHIFT);

	return 0;
}

static int ssic_cfg_done(dwc_usb3_device_t *dev)
{
	u32 temp;
	int usec = 100000;
	dwc_debug1(dev, "%s\n", __func__);

	temp = dwc_rd32(dev, &dev->ssic_regs->sctl[0]);
	temp |= DWC_SCTL_CFG_DONE_BIT;
	dwc_wr32(dev, &dev->ssic_regs->sctl[0], temp);

	do {
		u32 result = dwc_rd32(dev, &dev->core_global_regs->gusb3rmmictl[0]);
		if (((result & DWC_RMMICTL_MPHY_STATE_BITS) >>
		     DWC_RMMICTL_MPHY_STATE_SHIFT) != DWC_MPHY_STATE_HIBERN8) {
			break;
		}

		dwc_udelay(dev, 1);
		usec -= 1;
	} while (usec > 0);

	if (!usec) {
		dwc_error(dev, "%s: cfg done timed out\n", __func__);
		return -1;
	}

	return 0;
}

static int dwc_usb3_handle_ssic_event(dwc_usb3_device_t *dev)
{
	int retval = -1;
	u32 sevt = dwc_rd32(dev, &dev->ssic_regs->sevt[0]);
	u32 temp = sevt;
	dwc_debug(dev, "%s: SSIC Event 0x%08x\n", __func__, sevt);

	sevt &= (DWC_SEVT_ROM_INIT_CMPLT_BIT |
		DWC_SEVT_LACC_CMPLT_BIT |
		DWC_SEVT_RCMD_RES_RCVD_BIT |
		DWC_SEVT_RCMD_RES_SENT_BIT |
		DWC_SEVT_MPHY_ST_CHNG_BIT |
		DWC_SEVT_OK_STRT_RRAP_BIT |
		DWC_SEVT_RRAP_ERROR_BIT);

	dwc_wr32(dev, &dev->ssic_regs->sevt[0], sevt);

	sevt = temp;

	if (sevt & DWC_SEVT_MPHY_ST_CHNG_BIT) {
		u32 rmmictl = dwc_rd32(dev, &dev->core_global_regs->gusb3rmmictl[0]);
		if (((rmmictl & DWC_RMMICTL_MPHY_STATE_BITS) >> DWC_RMMICTL_MPHY_STATE_SHIFT) ==
		    DWC_MPHY_STATE_HIBERN8) {
			ssic_cfg_done(dev);
		}
	}

	if (sevt & DWC_SEVT_RCMD_RES_RCVD_BIT) {
		u8 rdata = (sevt & DWC_SEVT_RDATA_RCVD_BITS) >> DWC_SEVT_RDATA_RCVD_SHIFT;
		u8 rladdr = (sevt & DWC_SEVT_RLADDR_RCVD_BITS) >> DWC_SEVT_RLADDR_RCVD_SHIFT;
		u8 ruaddr = (sevt & DWC_SEVT_RUADDR_RCVD_BITS) >> DWC_SEVT_RUADDR_RCVD_SHIFT;
		int read = !!(sevt & DWC_SEVT_READ_RCVD_BIT);

		dwc_debug4(dev, "rdata=0x%02x, rladdr=0x%02x, ruaddr=0x%02x, read=%d\n",
			   rdata, rladdr, ruaddr, read);

		if (ruaddr <= 0x3) {
			if (read)
				dwc_error(dev, "Read not handled\n");
			else
				dwc_error(dev, "Write not handled\n");
		} else if (ruaddr == 0x4) {
			/* DSP_DISCONNECT */
			if (rladdr == 0x0) {
				dwc_error(dev, "DSP_DISCONNECT not handled\n");
			/* CONFIGURE_FOR_HS */
			} else if (rladdr == 0x1) {
				dwc_debug(dev, "CONFIGURE_FOR_HS\n");
				ssic_rrap_response(dev, !read, rladdr, ruaddr, 0x0);
				ssic_write_attr(dev, 0x21, 0x2);
				ssic_write_attr(dev, 0xa1, 0x2);
				retval = 0;
			/* BURST_CLOSURE */
			} else if (rladdr == 0x2) {
				dwc_debug(dev, "BURST_CLOSURE\n");
				ssic_rrap_response(dev, !read, rladdr, ruaddr, 0x0);
				ssic_cfg_done(dev);
			/* DISABLE_SCRAMBLING */
			} else if (rladdr == 0x3) {
				dwc_error(dev, "DISABLE_SCRAMBLING not handled\n");
			/* DISABLE_STALL_IN_U0 */
			} else if (rladdr == 0x4) {
				dwc_error(dev, "DISABLE_STALL_IN_U0 not handled\n");
			/* TEST_MODE */
			} else if (rladdr == 0xff) {
				dwc_error(dev, "DISABLE_TEST_MODE not handled\n");
			} else {
				dwc_error(dev, "UNKNOWN\n");
			}
		} else if (ruaddr == 0x5) {
			dwc_error(dev, "UNKNOWN\n");
		} else if (ruaddr == 0x6) {
			dwc_error(dev, "UNKNOWN\n");
		} else {
			dwc_error(dev, "UNKNOWN\n");
		}
	}

	return retval;
}

#endif


/**
 * This is the common interrupt handler routine.
 *
 * @param dev   Programming view of DWC_usb3 controller.
 * @param irq   IRQ number passed in by Linux kernel.
 * @return      1 if an interrupt event was seen, 0 if not.
 */
int dwc_usb3_irq(dwc_usb3_device_t *dev, int irq)
{
	int state, temp, ret;

#ifdef SSIC
	u32 gsts, sevt;

	gsts = dwc_rd32(dev, &dev->core_global_regs->gsts);
	sevt = dwc_rd32(dev, &dev->ssic_regs->sevt[0]);
	dwc_debug(dev, "%s: gsts=0x%08x, sevt=0x%08x\n", __func__, gsts, sevt);
#endif

	if (!dev->cmn_irq_installed)
		return 0;

#ifdef SSIC
	gsts = dwc_rd32(dev, &dev->core_global_regs->gsts);
	if (gsts & DWC_GSTS_SSIC_IP_BIT) {
		dwc_usb3_handle_ssic_event(dev);
		ret = 1;
	}
#endif

	state = dev->hibernate;

	if (state != DWC_HIBER_SLEEPING && dev->snpsid >= 0x5533230a &&
							dev->hiber_wait_u0) {
		/* Handle waiting for U0 after requesting link state RECOVERY,
		 * because we don't have the link state change event enabled.
		 * We also do this in dwc_wait_pme_thread() in case an event
		 * doesn't come.
		 */
		temp = dwc_usb3_pcd_get_link_state(&dev->pcd);
		dev->pcd.link_state = temp;
		dwc_debug1(dev, "intr WAIT_U0 state=%d\n", temp);

		if (temp == DWC_LINK_STATE_U0) {
			dwc_debug0(dev, "intr WAIT_U0 setting speed\n");
			dev->pcd.speed = dwc_usb3_get_device_speed(&dev->pcd);
			if (dev->pcd.remote_wakeup_enable)
				dwc_usb3_pcd_remote_wake(&dev->pcd, 0);
			dev->hiber_wait_u0 = 0;
		}
	}

	if (state >= DWC_HIBER_SLEEPING) {
		if (state == DWC_HIBER_WAIT_U0 ||
		    state == DWC_HIBER_SS_DIS_QUIRK) {
			dev->hibernate = DWC_HIBER_AWAKE;
		} else {
			if (dev->pme_ready) {
#ifndef SELA_PLATFORM
				ret = dwc_usb3_handle_pme_intr(dev);
				return ret;
#else
				return 1;
#endif
			} else {
				if (state != DWC_HIBER_WAIT_LINK_UP)
					dwc_info0(dev, "Intr in hibernate but"
						       " pme_ready not set\n");
				return 1;
			}
		}
	}

	ret = dwc_usb3_handle_event(dev);
	if (ret == 2)
		ret = 1;

#ifdef SSIC
	gsts = dwc_rd32(dev, &dev->core_global_regs->gsts);
	sevt = dwc_rd32(dev, &dev->ssic_regs->sevt[0]);
	dwc_debug(dev, "%s DONE: gsts=0x%08x, sevt=0x%08x\n", __func__, gsts, sevt);
#endif

	return ret;
}
