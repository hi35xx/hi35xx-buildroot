/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/cil.c $
 * $Revision: #110 $
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
 * The CIL manages the memory map for the core so that the PCD and OTG drivers
 * don't have to do this separately. The CIL also performs basic services
 * that are not specific to either the Device or OTG modes of operation.
 * These services include all functionality that requires specific
 * knowledge of the CSR layout or the DMA descriptor (TRB) layout. Also
 * included are services for invoking each of the commands provided by
 * the DGCMD and DEPCMD registers (see the "Control and Status Registers"
 * chapter of the USB3 controller databook for details).
 *
 * The Core Interface Layer has the following requirements:
 * - Provides basic controller operations.
 * - Minimal use of OS services.
 * - The OS services used will be abstracted by using inline functions
 *   or macros.
 *
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

/**
 * Fill in the four dwords of a DMA descriptor (aka a TRB).
 */
void dwc_usb3_fill_desc(dwc_usb3_dma_desc_t *desc, dwc_dma_t dma_addr,
			u32 dma_len, u32 stream, u32 type,
			u32 ctrlbits, int own)
{
	desc->bptl = (u32)(dma_addr & 0xffffffffU);
#ifdef DWC_64_BIT_ARCH
	desc->bpth = (u32)(dma_addr >> 32U & 0xffffffffU);
#else
	desc->bpth = 0;
#endif
	desc->status = dma_len << DWC_DSCSTS_XFRCNT_SHIFT;

	/* Note: If type is 0, leave original control bits intact (for isoc) */
	if (type)
		desc->control = type << DWC_DSCCTL_TRBCTL_SHIFT;

	desc->control |= stream << DWC_DSCCTL_STRMID_SOFN_SHIFT | ctrlbits;

	/* Must do this last! */
	if (own) {
		wmb();
		desc->control |= DWC_DSCCTL_HWO_BIT;
	}
}

/**
 * Make a DMA descriptor the start of a chain by setting its CHN bit and
 * clearing its IOC bit.
 *
 * @param desc  The DMA descriptor (TRB) to operate on.
 */
void dwc_usb3_start_desc_chain(dwc_usb3_dma_desc_t *desc)
{
	desc->control |= DWC_DSCCTL_CHN_BIT;
	desc->control &= ~DWC_DSCCTL_IOC_BIT;
}

/**
 * Make a DMA descriptor the end of a chain by clearing its CHN bit and
 * setting its IOC bit.
 *
 * @param desc  The DMA descriptor (TRB) to operate on.
 */
void dwc_usb3_end_desc_chain(dwc_usb3_dma_desc_t *desc)
{
	desc->control &= ~DWC_DSCCTL_CHN_BIT;
	desc->control |= DWC_DSCCTL_IOC_BIT;
}

/**
 * Enable a DMA descriptor by setting its HWO bit.
 *
 * @param desc  The DMA descriptor (TRB) to operate on.
 */
void dwc_usb3_enable_desc(dwc_usb3_dma_desc_t *desc)
{
	wmb();
	desc->control |= DWC_DSCCTL_HWO_BIT;
}

/**
 * Disable a DMA descriptor by clearing its HWO bit.
 *
 * NOTE: This must only be called if it is known that the hardware has finished
 * with the DMA descriptor, but for some reason the hardware has not cleared
 * the HWO bit.
 *
 * @param desc  The DMA descriptor (TRB) to operate on.
 */
void dwc_usb3_disable_desc(dwc_usb3_dma_desc_t *desc)
{
	desc->control &= ~DWC_DSCCTL_HWO_BIT;
	wmb();
}

/**
 * Spin on register bit until handshake completes or times out.
 *
 * @param dev   Programming view of DWC_usb3 controller.
 * @param ptr   Address of register to read.
 * @param mask  Bit to look at in result of read.
 * @param done  Value of the bit when handshake succeeds.
 * @return      1 when the mask bit has the specified value (handshake done),
 *              0 when timeout has passed (handshake failed).
 */
static int _handshake(dwc_usb3_device_t *dev, volatile u32 __iomem *ptr,
		      u32 mask, u32 done)
{
	u32 usec = 100000;
	u32 result;

	dwc_debug1(dev, "%s()\n", __func__);

	do {
		result = dwc_rd32(dev, ptr);
		if ((result & mask) == done) {
			dwc_debug1(dev, "after DEPCMD=%08x\n", result);
			return 1;
		}

		dwc_udelay(dev, 1);
		usec -= 1;
	} while (usec > 0);

	return 0;
}

/*
 * Routines for sending the various Device commands to the hardware.
 * See description of DGCMD register in "Control and Status Registers"
 * section of the USB3 controller databook.
 */

/**
 * Send TRANSMIT FUNCTION WAKE DEVICE NOTIFICATION command to Device
 */
int dwc_usb3_xmit_fn_remote_wake(dwc_usb3_pcd_t *pcd, u32 intf)
{
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);

	/* Use the generic Transmit Device Notification command if the core
	 * supports it
	 */
	if (pcd->usb3_dev->snpsid >= 0x5533210a) {
		/* Set param */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar,
			 intf << DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
			 DWC_DGCMD_FUNCTION_WAKE_DEV_NOTIF);
		dwc_debug1(pcd->usb3_dev, "DGCMDPAR=%08x\n",
			   intf << DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
			   DWC_DGCMD_FUNCTION_WAKE_DEV_NOTIF);

		/* Start the command */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
			 DWC_DGCMD_XMIT_DEV_NOTIF | DWC_DGCMD_ACT_BIT);
		dwc_debug1(pcd->usb3_dev, "DGCMD=%08x\n",
			   DWC_DGCMD_XMIT_DEV_NOTIF | DWC_DGCMD_ACT_BIT);
	} else {
		/* Set param */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar, intf);
		dwc_debug1(pcd->usb3_dev, "DGCMDPAR=%08x\n", intf);

		/* Start the command */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
			 DWC_DGCMD_XMIT_FUNC_WAKE_DEV_NOTIF | DWC_DGCMD_ACT_BIT);
		dwc_debug1(pcd->usb3_dev, "DGCMD=%08x\n",
			   DWC_DGCMD_XMIT_FUNC_WAKE_DEV_NOTIF | DWC_DGCMD_ACT_BIT);
	}

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		  DWC_DGCMD_ACT_BIT, 0);

	return 0;
}

/**
 * Send LATENCY TOLERANCE MESSAGE DEVICE NOTIFICATION command to Device
 */
int dwc_usb3_xmit_ltm(dwc_usb3_pcd_t *pcd, u32 value)
{
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);

	/* Use the generic Transmit Device Notification command if the core
	 * supports it. Otherwise this function is a no-op.
	 */
	if (pcd->usb3_dev->snpsid >= 0x5533210a) {
		/* Set param */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar,
			 value << DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
			 DWC_DGCMD_LATENCY_TOL_DEV_NOTIF);
		dwc_debug1(pcd->usb3_dev, "DGCMDPAR=%08x\n",
			   value << DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
			   DWC_DGCMD_LATENCY_TOL_DEV_NOTIF);

		/* Start the command */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
			 DWC_DGCMD_XMIT_DEV_NOTIF | DWC_DGCMD_ACT_BIT);
		dwc_debug1(pcd->usb3_dev, "DGCMD=%08x\n",
			   DWC_DGCMD_XMIT_DEV_NOTIF | DWC_DGCMD_ACT_BIT);

		/* Wait for command completion */
		handshake(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
			  DWC_DGCMD_ACT_BIT, 0);
	}

	return 0;
}

#ifdef CONFIG_USB_OTG_DWC
/**
 * Send ROLE REQUEST DEVICE NOTIFICATION command to Device
 */
int dwc_usb3_xmit_host_role_request(dwc_usb3_pcd_t *pcd, u32 param)
{
#ifdef DEBUG
	char *type = "UNKNOWN";

	if (param == DWC_DGCMDPAR_HOST_ROLE_REQ_INITIATE)
		type = "INITIATE";
	else if (param == DWC_DGCMDPAR_HOST_ROLE_REQ_CONFIRM)
		type = "CONFIRM";

	dwc_debug2(pcd->usb3_dev, "%s() - %s\n", __func__, type);
#endif

	/* Use the generic Transmit Device Notification command if the core
	 * supports it
	 */
	if (pcd->usb3_dev->snpsid >= 0x5533210a) {
		/* Set param */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar,
			 param << DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
			 DWC_DGCMD_HOST_ROLE_REQ_DEV_NOTIF);
		dwc_debug1(pcd->usb3_dev, "DGCMDPAR=%08x\n",
			   param << DWC_DGCMDPAR_DEV_NOTIF_PARAM_SHIFT |
			   DWC_DGCMD_HOST_ROLE_REQ_DEV_NOTIF);

		/* Start the command */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
			 DWC_DGCMD_XMIT_DEV_NOTIF | DWC_DGCMD_ACT_BIT);
		dwc_debug1(pcd->usb3_dev, "DGCMD=%08x\n",
			   DWC_DGCMD_XMIT_DEV_NOTIF | DWC_DGCMD_ACT_BIT);
	} else {
		/* Set param */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar, param);
		dwc_debug1(pcd->usb3_dev, "DGCMDPAR=%08x\n", param);

		/* Start the command */
		dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
			 DWC_DGCMD_XMIT_HOST_ROLE_REQUEST | DWC_DGCMD_ACT_BIT);

		dwc_debug1(pcd->usb3_dev, "DGCMD=%08x\n",
			   DWC_DGCMD_XMIT_HOST_ROLE_REQUEST | DWC_DGCMD_ACT_BIT);
	}

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		  DWC_DGCMD_ACT_BIT, 0);

	return 0;
}
#endif

/**
 * Send SET SCRATCHPAD BUFFER ARRAY command to Device
 */
int dwc_usb3_set_scratchpad_buf_array(dwc_usb3_pcd_t *pcd, dwc_dma_t dma_addr)
{
	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__,
		   (unsigned long)dma_addr);

	/* Set param */
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar,
		 dma_addr & 0xffffffffU);
	dwc_debug1(pcd->usb3_dev, "DGCMDPAR=%08lx\n",
		   (unsigned long)(dma_addr & 0xffffffffU));

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		 DWC_DGCMD_SET_SCRATCHPAD_ARRAY_ADR_LO | DWC_DGCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DGCMD=%08x\n",
		   DWC_DGCMD_SET_SCRATCHPAD_ARRAY_ADR_LO | DWC_DGCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		  DWC_DGCMD_ACT_BIT, 0);

	return 0;
}

/**
 * Send SELECTED FIFO FLUSH command to Device
 */
int dwc_usb3_flush_fifo(dwc_usb3_pcd_t *pcd, u32 fifo_sel)
{
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);

	/* Set param */
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmdpar, fifo_sel);
	dwc_debug1(pcd->usb3_dev, "DGCMDPAR=%08x\n", fifo_sel);

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		 DWC_DGCMD_SELECTED_FIFO_FLUSH | DWC_DGCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DGCMD=%08x\n",
		   DWC_DGCMD_SELECTED_FIFO_FLUSH | DWC_DGCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &pcd->dev_global_regs->dgcmd,
		  DWC_DGCMD_ACT_BIT, 0);

	return 0;
}

/*
 * Routines for sending the various Endpoint commands to the hardware.
 * See description of DEPCMDn register in "Control and Status Registers"
 * section of the USB3 controller databook.
 */

/**
 * Send DEPCFG command to EP
 */
int dwc_usb3_dep_cfg(dwc_usb3_pcd_t *pcd,
		     dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
		     u32 depcfg0, u32 depcfg1, u32 depcfg2)
{
	dwc_debug1(pcd->usb3_dev, "dep_cfg, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Set param 2 */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar2, depcfg2);
	dwc_debug1(pcd->usb3_dev, "DEPCFG2=%08x\n", depcfg2);

	/* Set param 1 */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar1, depcfg1);
	dwc_debug1(pcd->usb3_dev, "DEPCFG1=%08x\n", depcfg1);

	/* Set param 0 */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0, depcfg0);
	dwc_debug1(pcd->usb3_dev, "DEPCFG0=%08x\n", depcfg0);

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
		 DWC_EPCMD_SET_EP_CFG | DWC_EPCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n",
		   DWC_EPCMD_SET_EP_CFG | DWC_EPCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	return 0;
}

/**
 * Send DEPXFERCFG command to EP
 */
int dwc_usb3_dep_xfercfg(dwc_usb3_pcd_t *pcd,
			 dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			 u32 depstrmcfg)
{
	dwc_debug1(pcd->usb3_dev, "dep_xfercfg, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Set param 0 */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0, depstrmcfg);
	dwc_debug1(pcd->usb3_dev, "DEPSTRMCFG=%08x\n", depstrmcfg);

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
		 DWC_EPCMD_SET_XFER_CFG | DWC_EPCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n",
		   DWC_EPCMD_SET_XFER_CFG | DWC_EPCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	return 0;
}

/**
 * Send DEPGETEPSTATE command to EP
 */
u32 dwc_usb3_dep_getepstate(dwc_usb3_pcd_t *pcd,
				 dwc_usb3_dev_ep_regs_t __iomem *ep_reg)
{
	u32 retval;

	dwc_debug1(pcd->usb3_dev, "dep_getepstate, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
		 DWC_EPCMD_GET_EP_STATE | DWC_EPCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n",
		   DWC_EPCMD_GET_EP_STATE | DWC_EPCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	/* Get state returned in DEPCMDPAR2 */
	retval = dwc_rd32(pcd->usb3_dev, &ep_reg->depcmdpar2);

	return retval;
}

/**
 * Send DEPSSTALL command to EP
 */
int dwc_usb3_dep_sstall(dwc_usb3_pcd_t *pcd,
			dwc_usb3_dev_ep_regs_t __iomem *ep_reg)
{
	dwc_debug1(pcd->usb3_dev, "dep_sstall, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
		 DWC_EPCMD_SET_STALL | DWC_EPCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n",
		   DWC_EPCMD_SET_STALL | DWC_EPCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	return 0;
}

/**
 * Send DEPCSTALL command to EP
 */
int dwc_usb3_dep_cstall(dwc_usb3_pcd_t *pcd,
			dwc_usb3_dev_ep_regs_t __iomem *ep_reg, int clr_pend)
{
	u32 depcmd;

	dwc_debug1(pcd->usb3_dev, "dep_cstall, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Fill clear stall command */
	depcmd = DWC_EPCMD_CLR_STALL | DWC_EPCMD_ACT_BIT;
	if (clr_pend)
		depcmd |= DWC_EPCMD_HP_FRM_BIT;

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd, depcmd);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n", depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	return 0;
}

/**
 * Send DEPSTRTXFER command to EP
 */
int dwc_usb3_dep_startxfer(dwc_usb3_pcd_t *pcd,
			   dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			   dwc_dma_t dma_addr, u32 stream_or_uf)
{
	u32 retval;

	dwc_debug1(pcd->usb3_dev, "dep_startxfer, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Set param 1 */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar1, dma_addr & 0xffffffffU);
	dwc_debug1(pcd->usb3_dev, "TDADDRLO=%08lx\n",
		   (unsigned long)(dma_addr & 0xffffffffU));

	/* Set param 0 */
#ifdef DWC_64_BIT_ARCH
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0,
		 dma_addr >> 32U & 0xffffffffU);
	dwc_debug1(pcd->usb3_dev, "TDADDRHI=%08lx\n",
		   (unsigned long)(dma_addr >> 32U & 0xffffffffU));
#else
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmdpar0, 0);
	dwc_debug1(pcd->usb3_dev, "TDADDRHI=%08x\n", 0);
#endif
	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
		 stream_or_uf << DWC_EPCMD_STR_NUM_OR_UF_SHIFT |
		 DWC_EPCMD_START_XFER | DWC_EPCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n",
		   stream_or_uf << DWC_EPCMD_STR_NUM_OR_UF_SHIFT |
		   DWC_EPCMD_START_XFER | DWC_EPCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	retval = dwc_rd32(pcd->usb3_dev, &ep_reg->depcmd);

	return retval >> DWC_EPCMD_XFER_RSRC_IDX_SHIFT &
	       DWC_EPCMD_XFER_RSRC_IDX_BITS >> DWC_EPCMD_XFER_RSRC_IDX_SHIFT;
}

/**
 * Send DEPUPDTXFER command to EP
 */
int dwc_usb3_dep_updatexfer(dwc_usb3_pcd_t *pcd,
			    dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			    u32 tri)
{
	dwc_debug1(pcd->usb3_dev, "dep_updxfer, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
		 tri << DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
		 DWC_EPCMD_UPDATE_XFER | DWC_EPCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n",
		   tri << DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
		   DWC_EPCMD_UPDATE_XFER | DWC_EPCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	return 0;
}

/**
 * Send DEPENDXFER command to EP
 */
int dwc_usb3_dep_endxfer(dwc_usb3_pcd_t *pcd,
			 dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			 u32 tri, int flags, void *condition)
{
	u32 depcmd;

	dwc_debug1(pcd->usb3_dev, "dep_endxfer, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Fill end transfer command */
	depcmd = tri << DWC_EPCMD_XFER_RSRC_IDX_SHIFT | DWC_EPCMD_END_XFER;
	depcmd |= DWC_EPCMD_ACT_BIT;
	if (flags & DWC_ENDXFER_FORCE)
		depcmd |= DWC_EPCMD_HP_FRM_BIT;

	/* Start the command. */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd, depcmd);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n", depcmd);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);
	if (!(flags & DWC_ENDXFER_NODELAY))
		dwc_udelay(dev, 100);

	return 0;
}

#ifdef DWC_STAR_9000463548_WORKAROUND
int dwc_usb3_dep_endxfer_nowait(dwc_usb3_pcd_t *pcd,
				dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				u32 tri, int flags)
{
	u32 depcmd;

	dwc_debug1(pcd->usb3_dev, "dep_endxfer_nowait, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Fill end transfer command */
	depcmd = tri << DWC_EPCMD_XFER_RSRC_IDX_SHIFT | DWC_EPCMD_END_XFER;
	depcmd |= DWC_EPCMD_ACT_BIT;
	if (flags & DWC_ENDXFER_FORCE)
		depcmd |= DWC_EPCMD_HP_FRM_BIT;

	/* Start the command. */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd, depcmd);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n", depcmd);

	return 0;
}

int dwc_usb3_dep_wait_endxfer(dwc_usb3_pcd_t *pcd,
			      dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			      void *condition)
{
	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);
	dwc_udelay(dev, 100);

	return 0;
}
#endif

/**
 * Send DEPSTRTNEWCFG command to EP
 */
int dwc_usb3_dep_startnewcfg(dwc_usb3_pcd_t *pcd,
			     dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			     u32 rsrcidx)
{
	dwc_debug1(pcd->usb3_dev, "dep_startnewcfg, ep_reg=%lx\n",
		   (unsigned long)ep_reg);

	/* Start the command */
	dwc_wr32(pcd->usb3_dev, &ep_reg->depcmd,
		 rsrcidx << DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
		 DWC_EPCMD_START_NEW_CFG | DWC_EPCMD_ACT_BIT);
	dwc_debug1(pcd->usb3_dev, "DEPCMD=%08x\n",
		   rsrcidx << DWC_EPCMD_XFER_RSRC_IDX_SHIFT |
		   DWC_EPCMD_START_NEW_CFG | DWC_EPCMD_ACT_BIT);

	/* Wait for command completion */
	handshake(pcd->usb3_dev, &ep_reg->depcmd, DWC_EPCMD_ACT_BIT, 0);

	return 0;
}

/**********************/

/**
 * Enable an EP in the DALEPENA register.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param ep    The EP to enable.
 * @return      0 if succesful, -DWC_E_BUSY if already enabled.
 */
int dwc_usb3_enable_ep(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep)
{
	u32 ep_index_num, dalepena;

	ep_index_num = ep->dwc_ep.num * 2;

	if (ep->dwc_ep.is_in)
		ep_index_num += 1;

	dalepena = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dalepena);

	/* If we have already enabled this EP, leave it alone
	 * (shouldn't happen)
	 */
	if (dalepena & 1 << ep_index_num)
		return -DWC_E_BUSY;

	dalepena |= 1 << ep_index_num;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dalepena, dalepena);
	dwc_debug1(pcd->usb3_dev, "enable_ep: DALEPENA=%08x\n", dalepena);
	return 0;
}

/**
 * Disable an EP in the DALEPENA register.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param ep    The EP to disable.
 * @return      0 if succesful, -DWC_E_INVALID if already disabled.
 */
int dwc_usb3_disable_ep(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep)
{
	u32 ep_index_num, dalepena;

	ep_index_num = ep->dwc_ep.num * 2;

	if (ep->dwc_ep.is_in)
		ep_index_num += 1;

	dalepena = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dalepena);

	/* If we have already disabled this EP, leave it alone
	 * (shouldn't happen)
	 */
	if (!(dalepena & 1 << ep_index_num))
		return -DWC_E_INVALID;

	dalepena &= ~(1 << ep_index_num);
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dalepena, dalepena);
	dwc_debug1(pcd->usb3_dev, "disable_ep: DALEPENA=%08x\n", dalepena);
	return 0;
}

/**
 * Get the device speed from the device status register and convert it
 * to USB speed constant.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      The device speed.
 */
int dwc_usb3_get_device_speed(dwc_usb3_pcd_t *pcd)
{
	u32 dsts;
	int speed = USB_SPEED_UNKNOWN;

	dsts = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dsts);

	switch (dsts >> DWC_DSTS_CONNSPD_SHIFT &
		DWC_DSTS_CONNSPD_BITS >> DWC_DSTS_CONNSPD_SHIFT) {
	case DWC_SPEED_HS_PHY_30MHZ_OR_60MHZ:
		dwc_debug0(pcd->usb3_dev, "HIGH SPEED\n");
		speed = USB_SPEED_HIGH;
		break;

	case DWC_SPEED_FS_PHY_30MHZ_OR_60MHZ:
	case DWC_SPEED_FS_PHY_48MHZ:
		dwc_debug0(pcd->usb3_dev, "FULL SPEED\n");
		speed = USB_SPEED_FULL;
		break;

	case DWC_SPEED_LS_PHY_6MHZ:
		dwc_debug0(pcd->usb3_dev, "LOW SPEED\n");
		speed = USB_SPEED_LOW;
		break;

	case DWC_SPEED_SS_PHY_125MHZ_OR_250MHZ:
		dwc_debug0(pcd->usb3_dev, "SUPER SPEED\n");
		speed = USB_SPEED_SUPER;
		break;
	}

	return speed;
}

/**
 * Get the current microframe number.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      The current microframe number.
 */
int dwc_usb3_get_frame(dwc_usb3_pcd_t *pcd)
{
	u32 dsts;

	/* read current frame/microframe number from DSTS register */
	dsts = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dsts);

	return dsts >> DWC_DSTS_SOF_FN_SHIFT &
	       DWC_DSTS_SOF_FN_BITS >> DWC_DSTS_SOF_FN_SHIFT;
}

/**
 * Get the current link state.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      The current link state.
 */
int dwc_usb3_pcd_get_link_state(dwc_usb3_pcd_t *pcd)
{
	u32 status, state;

	status = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dsts);
	state = status >> DWC_DSTS_USBLNK_STATE_SHIFT &
		DWC_DSTS_USBLNK_STATE_BITS >> DWC_DSTS_USBLNK_STATE_SHIFT;
	dwc_debug2(pcd->usb3_dev, "status=0x%08x state=%d\n", status, state);

	return state;
}

/**
 * Set state of USB link.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param state Link state to set.
 */
void dwc_usb3_pcd_set_link_state(dwc_usb3_pcd_t *pcd, int state)
{
	u32 dctl;

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl &= ~DWC_DCTL_ULST_CHNG_REQ_BITS;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl &= ~DWC_DCTL_ULST_CHNG_REQ_BITS;
	dctl |= state << DWC_DCTL_ULST_CHNG_REQ_SHIFT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
}

/**
 * Send a Remote Wakeup to the host.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param function      Function that caused the remote wakeup.
 */
void dwc_usb3_pcd_remote_wake(dwc_usb3_pcd_t *pcd, int function)
{
	/* For USB 3.0, send function remote wake notification */
	if (pcd->speed == USB_SPEED_SUPER)
		dwc_usb3_xmit_fn_remote_wake(pcd, function);
}

/**
 * Set the Device Address.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param addr  The address to set.
 */
void dwc_usb3_set_address(dwc_usb3_pcd_t *pcd, int addr)
{
	u32 dcfg;

	dcfg = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dcfg);
	dcfg &= ~DWC_DCFG_DEVADDR_BITS;
	dcfg |= addr << DWC_DCFG_DEVADDR_SHIFT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dcfg, dcfg);
}

/**
 * Enable USB2 Phy suspend.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
static void dwc_usb3_ena_usb2_phy_suspend(dwc_usb3_pcd_t *pcd)
{
	dwc_usb3_device_t *dev = pcd->usb3_dev;
	int hiber = dev->core_params->hibernate &&
		    (dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
		    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT;
	int writeit = 0;
	u32 usb2phycfg;

	if (dev->core_params->phyctl || hiber || dev->core_params->lpmctl) {
		usb2phycfg = dwc_rd32(dev,
				&dev->core_global_regs->gusb2phycfg[0]);

		if ((dev->core_params->phyctl || hiber) &&
		    !(usb2phycfg & DWC_USB2PHYCFG_SUS_PHY_BIT)) {
			usb2phycfg |= DWC_USB2PHYCFG_SUS_PHY_BIT;
			writeit = 1;
		}

		if (dev->core_params->lpmctl &&
		    !(usb2phycfg & DWC_USB2PHYCFG_ENBL_SLP_M_BIT)) {
			usb2phycfg |= DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
			writeit = 1;
		}

		if (writeit)
			dwc_wr32(dev, &dev->core_global_regs->gusb2phycfg[0],
				 usb2phycfg);
	}
}

/**
 * Disable USB2 Phy suspend.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
static void dwc_usb3_dis_usb2_phy_suspend(dwc_usb3_pcd_t *pcd)
{
	u32 usb2phycfg;

	usb2phycfg = dwc_rd32(pcd->usb3_dev,
			      &pcd->usb3_dev->core_global_regs->gusb2phycfg[0]);

	if (usb2phycfg & (DWC_USB2PHYCFG_SUS_PHY_BIT |
			  DWC_USB2PHYCFG_ENBL_SLP_M_BIT)) {
		usb2phycfg &= ~DWC_USB2PHYCFG_SUS_PHY_BIT;
		usb2phycfg &= ~DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
		dwc_wr32(pcd->usb3_dev,
			 &pcd->usb3_dev->core_global_regs->gusb2phycfg[0],
			 usb2phycfg);
	}
}

/**
 * Enable USB3 Phy suspend.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
static void dwc_usb3_ena_usb3_phy_suspend(dwc_usb3_pcd_t *pcd)
{
#ifndef SELA_PLATFORM
	u32 pipectl;

	pipectl = dwc_rd32(pcd->usb3_dev,
			   &pcd->usb3_dev->core_global_regs->gusb3pipectl[0]);
	if (!(pipectl & DWC_PIPECTL_SUS_PHY_BIT)) {
		pipectl |= DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(pcd->usb3_dev,
			 &pcd->usb3_dev->core_global_regs->gusb3pipectl[0],
			 pipectl);
	}
#endif
}

/**
 * Disable USB3 Phy suspend.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
static void dwc_usb3_dis_usb3_phy_suspend(dwc_usb3_pcd_t *pcd)
{
#ifndef SELA_PLATFORM
	u32 pipectl;

	pipectl = dwc_rd32(pcd->usb3_dev,
			   &pcd->usb3_dev->core_global_regs->gusb3pipectl[0]);
	if (pipectl & DWC_PIPECTL_SUS_PHY_BIT) {
		pipectl &= ~DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(pcd->usb3_dev,
			 &pcd->usb3_dev->core_global_regs->gusb3pipectl[0],
			 pipectl);
	}
#endif
}

/**
 * Enable USB2 Phy suspend if the device is connected at HS or FS.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_ena_usb2_suspend(dwc_usb3_pcd_t *pcd)
{
	if (pcd->speed == USB_SPEED_SUPER)
		return;

	dwc_usb3_ena_usb2_phy_suspend(pcd);
}

/**
 * Disable USB2 Phy suspend if the device is connected at HS or FS.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_dis_usb2_suspend(dwc_usb3_pcd_t *pcd)
{
	if (pcd->speed == USB_SPEED_SUPER)
		return;

	dwc_usb3_dis_usb2_phy_suspend(pcd);
}

/**
 * Enable the Device to accept U1 control commands from the Host.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_accept_u1(dwc_usb3_pcd_t *pcd)
{
	u32 dctl;

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl |= DWC_DCTL_ACCEPT_U1_EN_BIT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
}

/**
 * Enable the Device to accept U2 control commands from the Host.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_accept_u2(dwc_usb3_pcd_t *pcd)
{
#if 0
	u32 dctl;

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl |= DWC_DCTL_ACCEPT_U2_EN_BIT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
#endif
}

/**
 * Enable U1 sleep.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_enable_u1(dwc_usb3_pcd_t *pcd)
{
	u32 dctl;

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl |= DWC_DCTL_INIT_U1_EN_BIT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
}

/**
 * Enable U2 sleep.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_enable_u2(dwc_usb3_pcd_t *pcd)
{
	u32 dctl;
#if 0
	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl |= DWC_DCTL_INIT_U2_EN_BIT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
#endif
}

/**
 * Disable U1 sleep.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_disable_u1(dwc_usb3_pcd_t *pcd)
{
	u32 dctl;

#if 0
	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl &= ~DWC_DCTL_INIT_U1_EN_BIT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
#endif
}

/**
 * Disable U2 sleep.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_disable_u2(dwc_usb3_pcd_t *pcd)
{
	u32 dctl;

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl &= ~DWC_DCTL_INIT_U2_EN_BIT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
}

/**
 * Test whether U1 sleep is enabled.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      1 if enabled, 0 if not.
 */
int dwc_usb3_u1_enabled(dwc_usb3_pcd_t *pcd)
{
	u32 dctl;

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	return !!(dctl & DWC_DCTL_INIT_U1_EN_BIT);
}

/**
 * Test whether U2 sleep is enabled.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      1 if enabled, 0 if not.
 */
int dwc_usb3_u2_enabled(dwc_usb3_pcd_t *pcd)
{
	u32 dctl;

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	return !!(dctl & DWC_DCTL_INIT_U2_EN_BIT);
}

/**
 * Clear 'eps_enabled' flag and 'ena_once' flags for all EPs, so EPs will get
 * completely reconfigured by SetConfig and SetInterface.
 */
void dwc_usb3_clr_eps_enabled(dwc_usb3_pcd_t *pcd)
{
	dwc_usb3_pcd_ep_t *ep;
	int i;

	pcd->eps_enabled = 0;

	for (i = 0; i < pcd->num_in_eps; i++) {
		ep = pcd->in_ep[i];
		if (ep)
			ep->dwc_ep.ena_once = 0;
	}

	for (i = 0; i < pcd->num_out_eps; i++) {
		ep = pcd->out_ep[i];
		if (ep)
			ep->dwc_ep.ena_once = 0;
	}
}

/**
 * This routine is called when the SET_FEATURE TEST_MODE Setup packet
 * is sent from the host. The Device Control register is written with
 * the Test Mode bits set to the specified Test Mode. This is done as
 * a tasklet so that the "Status" phase of the control transfer
 * completes before transmitting the TEST packets.
 */
void dwc_usb3_pcd_do_test_mode(unsigned long data)
{
	dwc_usb3_pcd_t *pcd = (dwc_usb3_pcd_t *)data;
	int test_mode = pcd->test_mode;
	u32 dctl;

#ifdef CONFIG_USB_OTG_DWC
	struct usb_phy *phy;
	struct usb_otg *otg;
#endif

	dctl = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	dctl &= ~DWC_DCTL_TSTCTL_BITS;

	switch (test_mode) {
	case 1: // TEST_J
		dctl |= 1 << DWC_DCTL_TSTCTL_SHIFT;
		break;

	case 2: // TEST_K
		dctl |= 2 << DWC_DCTL_TSTCTL_SHIFT;
		break;

	case 3: // TEST_SE0_NAK
		dctl |= 3 << DWC_DCTL_TSTCTL_SHIFT;
		break;

	case 4: // TEST_PACKET
		dctl |= 4 << DWC_DCTL_TSTCTL_SHIFT;
		break;

	case 5: // TEST_FORCE_ENABLE
		dctl |= 5 << DWC_DCTL_TSTCTL_SHIFT;
		break;

#ifdef CONFIG_USB_OTG_DWC
	case 6: // otg_srp_reqd
		dwc_error0(pcd->usb3_dev, "otg_srp_reqd\n");
		phy = usb_get_phy(USB_PHY_TYPE_USB3);
		if (IS_ERR(phy))
			break;
		if (!phy) {
			usb_put_phy(phy);
			break;
		}
		otg = phy->otg;
		if (!otg) {
			usb_put_phy(phy);
			break;
		}
		otg_start_srp(otg);
		usb_put_phy(phy);
		return;

	case 7: // otg_hnp_reqd
		dwc_error0(pcd->usb3_dev, "otg_hnp_reqd\n");
		phy = usb_get_phy(USB_PHY_TYPE_USB3);
		if (IS_ERR(phy))
			break;
		if (!phy) {
			usb_put_phy(phy);
			break;
		}
		otg = phy->otg;
		if (!otg) {
			usb_put_phy(phy);
			break;
		}
		otg_start_hnp(otg);
		usb_put_phy(phy);
		return;
#endif
	}

	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, dctl);
}

/**
 * This routine calculates the number of IN EPs (excluding EP0)
 * using GHWPARAMS3 register values
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
static int calc_num_in_eps(dwc_usb3_device_t *dev)
{
	u32 num_in_eps = dev->hwparams3 >> DWC_HWP3_NUM_IN_EPS_SHIFT &
		DWC_HWP3_NUM_IN_EPS_BITS >> DWC_HWP3_NUM_IN_EPS_SHIFT;

	return num_in_eps - 1;
}

/**
 * This routine calculates the number of OUT EPs (excluding EP0)
 * using GHWPARAMS3 register values
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
static int calc_num_out_eps(dwc_usb3_device_t *dev)
{
	u32 num_eps = dev->hwparams3 >> DWC_HWP3_NUM_EPS_SHIFT &
		DWC_HWP3_NUM_EPS_BITS >> DWC_HWP3_NUM_EPS_SHIFT;
	u32 num_in_eps = dev->hwparams3 >> DWC_HWP3_NUM_IN_EPS_SHIFT &
		DWC_HWP3_NUM_IN_EPS_BITS >> DWC_HWP3_NUM_IN_EPS_SHIFT;

	return num_eps - num_in_eps - 1;
}

/**
 * This routine is called to initialize the DWC_usb3 CSR data structures. The
 * register addresses in the device structures are initialized from the
 * <strong><em>base</em></strong> address supplied by the caller. The calling
 * routine must make the OS calls to get the base address of the DWC_usb3
 * controller registers. The <strong><em>core_params</em></strong> argument
 * holds the parameters that specify how the core should be configured.
 *
 * @param dev           Programming view of DWC_usb3 controller.
 * @param base          Base address of DWC_usb3 core registers.
 * @param core_params   Pointer to the core configuration parameters.
 */
int dwc_usb3_pcd_common_init(dwc_usb3_device_t *dev, volatile u8 __iomem *base,
			     dwc_usb3_core_params_t *core_params)
{
	dwc_usb3_pcd_t *pcd;
	u32 temp;

	dwc_debug3(dev, "%s(%lx,%lx)\n", __func__, (unsigned long)base,
		   (unsigned long)core_params);

	dev->core_params = core_params;
	dev->core_global_regs = (dwc_usb3_core_global_regs_t __iomem *)
					(base + DWC_CORE_GLOBAL_REG_OFFSET);

#ifdef COSIM
	/* scramble-off, scaledown */
	dwc_wr32(dev, &dev->core_global_regs->gctl, 0x38);
#endif

	pcd = &dev->pcd;

	pcd->dev_global_regs = (dwc_usb3_dev_global_regs_t __iomem *)
					(base + DWC_DEV_GLOBAL_REG_OFFSET);
	pcd->in_ep_regs = (dwc_usb3_dev_ep_regs_t __iomem *)
					(base + DWC_DEV_IN_EP_REG_OFFSET);
	pcd->out_ep_regs = (dwc_usb3_dev_ep_regs_t __iomem *)
					(base + DWC_DEV_OUT_EP_REG_OFFSET);

#ifdef SSIC
	dev->ssic_regs = (dwc_usb3_ssic_regs_t __iomem *)
					(base + DWC_SSIC_REG_OFFSET);
#endif

	/*
	 * Store the contents of the hardware configuration registers here for
	 * easy access later.
	 */
	dev->hwparams0 = dwc_rd32(dev, &dev->core_global_regs->ghwparams0);
	dev->hwparams1 = dwc_rd32(dev, &dev->core_global_regs->ghwparams1);
	dev->hwparams2 = dwc_rd32(dev, &dev->core_global_regs->ghwparams2);
	dev->hwparams3 = dwc_rd32(dev, &dev->core_global_regs->ghwparams3);
	dev->hwparams4 = dwc_rd32(dev, &dev->core_global_regs->ghwparams4);
	dev->hwparams5 = dwc_rd32(dev, &dev->core_global_regs->ghwparams5);
	dev->hwparams6 = dwc_rd32(dev, &dev->core_global_regs->ghwparams6);
	dev->hwparams7 = dwc_rd32(dev, &dev->core_global_regs->ghwparams7);
	dev->hwparams8 = dwc_rd32(dev, &dev->core_global_regs->ghwparams8);

	temp = dwc_rd32(dev, &pcd->dev_global_regs->dcfg);
	dwc_debug1(dev, "dcfg=%08x\n", temp);

#ifndef SELA_PLATFORM
	dwc_debug1(dev, "mode=%0x\n",
		   dev->hwparams0 >> DWC_HWP0_MODE_SHIFT &
		   DWC_HWP0_MODE_BITS >> DWC_HWP0_MODE_SHIFT);
	dwc_debug1(dev, "num_ep=%d\n",
		   dev->hwparams3 >> DWC_HWP3_NUM_EPS_SHIFT &
		   DWC_HWP3_NUM_EPS_BITS >> DWC_HWP3_NUM_EPS_SHIFT);
	dwc_debug1(dev, "num_in_ep=%d\n",
		   dev->hwparams3 >> DWC_HWP3_NUM_IN_EPS_SHIFT &
		   DWC_HWP3_NUM_IN_EPS_BITS >> DWC_HWP3_NUM_IN_EPS_SHIFT);
	dwc_debug1(dev, "dfq_fifo_depth=%d\n",
	       dev->hwparams5 >> DWC_HWP5_DFQ_FIFO_DEPTH_SHIFT &
	       DWC_HWP5_DFQ_FIFO_DEPTH_BITS >> DWC_HWP5_DFQ_FIFO_DEPTH_SHIFT);
	dwc_debug1(dev, "dwq_fifo_depth=%d\n",
	       dev->hwparams5 >> DWC_HWP5_DWQ_FIFO_DEPTH_SHIFT &
	       DWC_HWP5_DWQ_FIFO_DEPTH_BITS >> DWC_HWP5_DWQ_FIFO_DEPTH_SHIFT);
	dwc_debug1(dev, "txq_fifo_depth=%d\n",
	       dev->hwparams5 >> DWC_HWP5_TXQ_FIFO_DEPTH_SHIFT &
	       DWC_HWP5_TXQ_FIFO_DEPTH_BITS >> DWC_HWP5_TXQ_FIFO_DEPTH_SHIFT);
	dwc_debug1(dev, "rxq_fifo_depth=%d\n",
	       dev->hwparams5 >> DWC_HWP5_RXQ_FIFO_DEPTH_SHIFT &
	       DWC_HWP5_RXQ_FIFO_DEPTH_BITS >> DWC_HWP5_RXQ_FIFO_DEPTH_SHIFT);
#endif

	/* Initialize parameters from Hardware configuration registers. */
	dev->pcd.num_in_eps = calc_num_in_eps(dev);
	dev->pcd.num_in_eps = 4;
	printk("\n###%s,%d,dev->pcd.num_in_eps=0x%x\n",__func__,__LINE__,dev->pcd.num_in_eps);
	if (dev->pcd.num_in_eps > 15) {
		dwc_debug1(dev, "Number of IN endpoints (%d) too large\n",
			   dev->pcd.num_in_eps);
		return -DWC_E_INVALID;
	}

	dev->pcd.num_out_eps = calc_num_out_eps(dev);
	dev->pcd.num_out_eps = 2;
	printk("\n###%s,%d,dev->pcd.num_out_eps=0x%x\n",__func__,__LINE__,dev->pcd.num_out_eps);
	if (dev->pcd.num_out_eps > 15) {
		dwc_debug1(dev, "Number of OUT endpoints (%d) too large\n",
			   dev->pcd.num_out_eps);
		return -DWC_E_INVALID;
	}

#ifdef SELA_PLATFORM
	/* Limit the number of EPs to speed up simulation */
	if (dev->pcd.num_in_eps > dev->pcd.max_in_eps)
		dev->pcd.num_in_eps = dev->pcd.max_in_eps;
	if (dev->pcd.num_out_eps > dev->pcd.max_out_eps)
		dev->pcd.num_out_eps = dev->pcd.max_out_eps;
#endif

	return 0;
}

/**
 * This routine frees any allocations made by dwc_usb3_pcd_common_init().
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
void dwc_usb3_pcd_common_remove(dwc_usb3_device_t *dev)
{
}

/**
 * This routine ensures the device is really a DWC_usb3 controller, by reading
 * and verifying the SNPSID register contents. The value should be 0x5533XXXX,
 * which corresponds to "U3", as in "USB3 version X.XXX".
 *
 * The SNPSID value is also saved in <em>dev->snpsid</em> for later use in
 * determining if any version-specific operations need to be performed.
 *
 * This routine should be called before any other initialization routines, to
 * ensure that the <em>dev->snpsid</em> value is set in case any of the other
 * routines need it.
 *
 * @param dev           Programming view of DWC_usb3 controller.
 * @param addr_ofs      Offset to the Device registers in the CSR space. It is
 *                      needed because this routine is called early, before the
 *                      normal register access routines are functional.
 * @return              0 if the SNPSID value is valid, -DWC_E_INVALID if not.
 */
int dwc_usb3_pcd_check_snpsid(dwc_usb3_device_t *dev, u32 addr_ofs)
{
	dev->snpsid = dwc_rd32(dev, (volatile u32 __iomem *)
					(dev->base + addr_ofs + 0x120));
	if ((dev->snpsid & 0xffff0000) != 0x55330000) {
		dwc_error1(dev, "bad value for SNPSID: 0x%08x!\n", dev->snpsid);
		return -DWC_E_INVALID;
	}

	return 0;
}

/**
 * This routine dumps the core's internal debug registers
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
void dwc_usb3_dump_dbgregs(dwc_usb3_device_t *dev)
{
	u32 rd_data, rd_data2;
	unsigned int i, num_eps;

	/* Dump the 11 LSP debug registers */
	for (i = 0; i < 11; i++) {
		dwc_wr32(dev, &dev->core_global_regs->gdbglspmux, i << 4);
		rd_data = dwc_rd32(dev, &dev->core_global_regs->gdbglsp);
		dwc_info2(dev, "lsp%d: %08x\n", i, rd_data);
	}

	/* Dump the EP debug registers */
	num_eps = dev->hwparams3 >> DWC_HWP3_NUM_EPS_SHIFT &
		  DWC_HWP3_NUM_EPS_BITS >> DWC_HWP3_NUM_EPS_SHIFT;
	for (i = 0; i < num_eps; i++) {
		dwc_wr32(dev, &dev->core_global_regs->gdbglspmux, i);
		rd_data = dwc_rd32(dev, &dev->core_global_regs->gdbgepinfo0);
		rd_data2 = dwc_rd32(dev, &dev->core_global_regs->gdbgepinfo1);
		dwc_info3(dev, " ep%d: %08x %08x\n", i, rd_data, rd_data2);
	}

	/* Dump the BMU debug register */
	rd_data = dwc_rd32(dev, &dev->core_global_regs->gdbgbmu);
	dwc_info1(dev, "bmu: %08x\n", rd_data);
}

/**
 * This routine reads the core global registers and prints them
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
void dwc_usb3_dump_global_registers(dwc_usb3_device_t *dev)
{
	volatile u32 __iomem *addr;

	dwc_print0(dev, "Core Global Registers\n");
	addr = &dev->core_global_regs->gsbuscfg0;
	dwc_print2(dev, "GSBUSCFG0	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gsbuscfg1;
	dwc_print2(dev, "GSBUSCFG1	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gusb2phycfg[0];
	dwc_print2(dev, "USB2PHYCFG0	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gevten;
	dwc_print2(dev, "GEVTEN		@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->grxfifosiz[0];
	dwc_print2(dev, "GRXFIFOSIZ0	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gtxfifosiz[0];
	dwc_print2(dev, "GTXFIFOSIZ0	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gtxfifosiz[1];
	dwc_print2(dev, "GTXFIFOSIZ1	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gtxfifosiz[2];
	dwc_print2(dev, "GTXFIFOSIZ2	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gtxfifosiz[3];
	dwc_print2(dev, "GTXFIFOSIZ3	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gusb2i2cctl[0];
	dwc_print2(dev, "GUSB2I2CCTL0	@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->ggpio;
	dwc_print2(dev, "GGPIO		@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->guid;
	dwc_print2(dev, "GUID		@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
	addr = &dev->core_global_regs->gsnpsid;
	dwc_print2(dev, "GSNPSID		@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(dev, addr));
}

/**
 * This routine reads the device registers and prints them
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_dump_dev_registers(dwc_usb3_pcd_t *pcd)
{
	volatile u32 __iomem *addr;

	dwc_print0(pcd->usb3_dev, "Device Global Registers\n");
	addr = &pcd->dev_global_regs->dcfg;
	dwc_print2(pcd->usb3_dev, "DCFG		@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(pcd->usb3_dev, addr));
	addr = &pcd->dev_global_regs->dctl;
	dwc_print2(pcd->usb3_dev, "DCTL		@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(pcd->usb3_dev, addr));
	addr = &pcd->dev_global_regs->dsts;
	dwc_print2(pcd->usb3_dev, "DSTS		@0x%08lx : 0x%08x\n",
		   (unsigned long)addr, dwc_rd32(pcd->usb3_dev, addr));
}

/**
 * Set the size of the Tx FIFOs
 *
 * NOTE: The following code for setting the FIFO sizes only
 * works for cores configured with the 3 RAM option. Setting
 * FIFO sizes for the 2 RAM option is not implemented.
 *
 * @param dev   Programming view of DWC_usb3 controller.
 * @param sz    New sizes for the FIFOs.
 */
void dwc_usb3_set_tx_fifo_size(dwc_usb3_device_t *dev, int *sz)
{
	dwc_usb3_core_global_regs_t __iomem *global_regs =
						dev->core_global_regs;
	int i, ram_width, ram_depth, size, prev_start, txsz[DWC_MAX_TX_FIFOS];

	ram_width = (dev->hwparams0 >> DWC_HWP0_MDWIDTH_SHIFT &
		     DWC_HWP0_MDWIDTH_BITS >> DWC_HWP0_MDWIDTH_SHIFT)
		    / 8;
	ram_depth = (dev->hwparams7 >> DWC_HWP7_RAM1_DEPTH_SHIFT &
		     DWC_HWP7_RAM1_DEPTH_BITS >> DWC_HWP7_RAM1_DEPTH_SHIFT)
		    * ram_width;
	size = dwc_rd32(dev, &global_regs->gtxfifosiz[0]);
	prev_start = size >> DWC_FIFOSZ_STARTADDR_SHIFT &
		DWC_FIFOSZ_STARTADDR_BITS >> DWC_FIFOSZ_STARTADDR_SHIFT;

	for (i = 0; i < dev->pcd.num_in_eps + 1; i++) {
		size = sz[i];

		if (i == 0 && size && size < 512 + 2 * ram_width) {
			dwc_print1(dev, "Requested Tx FIFO %d size too small\n",
				   i);
			dwc_print0(dev, "Not setting Tx FIFO sizes\n");
			goto txerr;
		}

		if (!size) {
			/* Default to 512 for EP0, 1K for others */
			size = i ? 1024 : 512;
			size += 2 * ram_width;
		}

		size = (size + ram_width - 1) & ~(ram_width - 1);
		dwc_debug3(dev,
			   "Tx FIFO %d size = %d bytes out of %d available\n",
			   i, size, ram_depth);
		if (size > ram_depth) {
			dwc_print1(dev, "Requested Tx FIFO %d size too large\n",
				   i);
			dwc_print0(dev, "Not setting Tx FIFO sizes\n");
			goto txerr;
		}

		txsz[i] = size;
		ram_depth -= size;
	}

	for (i = 0; i < dev->pcd.num_in_eps + 1; i++) {
		size = txsz[i];
		dwc_debug2(dev, "Setting GTXFIFOSIZ%d = 0x%08x\n", i,
			   (size / ram_width) << DWC_FIFOSZ_DEPTH_SHIFT |
			   prev_start << DWC_FIFOSZ_STARTADDR_SHIFT);
		dwc_wr32(dev, &global_regs->gtxfifosiz[i],
			 (size / ram_width) << DWC_FIFOSZ_DEPTH_SHIFT |
			 prev_start << DWC_FIFOSZ_STARTADDR_SHIFT);
		dwc_debug2(dev, "GTXFIFOSIZ%d = 0x%08x\n", i,
			   dwc_rd32(dev, &global_regs->gtxfifosiz[i]));
		prev_start += size / ram_width;
	}
txerr:
	return;
}

/**
 * Set the size of the Rx FIFO
 *
 * NOTE: The following code for setting the FIFO sizes only
 * works for cores configured with the 3 RAM option. Setting
 * FIFO sizes for the 2 RAM option is not implemented.
 *
 * @param dev   Programming view of DWC_usb3 controller.
 * @param size  New size for the FIFO.
 */
void dwc_usb3_set_rx_fifo_size(dwc_usb3_device_t *dev, int size)
{
	dwc_usb3_core_global_regs_t __iomem *global_regs =
						dev->core_global_regs;
	u32 sz, rxthrcfg;
	int ram_width, ram_depth, prev_start, cnt, bst;

	ram_width = (dev->hwparams0 >> DWC_HWP0_MDWIDTH_SHIFT &
		     DWC_HWP0_MDWIDTH_BITS >> DWC_HWP0_MDWIDTH_SHIFT)
		    / 8;
	ram_depth = (dev->hwparams7 >> DWC_HWP7_RAM2_DEPTH_SHIFT &
		     DWC_HWP7_RAM2_DEPTH_BITS >> DWC_HWP7_RAM2_DEPTH_SHIFT)
		    * ram_width;
	sz = dwc_rd32(dev, &global_regs->grxfifosiz[0]);
	prev_start = sz >> DWC_FIFOSZ_STARTADDR_SHIFT &
		     DWC_FIFOSZ_STARTADDR_BITS >> DWC_FIFOSZ_STARTADDR_SHIFT;

	if (size < 512 + 24 + 16 + (ram_width == 16 ? 24 : 0)) {
		dwc_print0(dev, "Requested Rx FIFO size too small\n");
		dwc_print0(dev, "Not setting Rx FIFO size\n");
		goto rxerr;
	}

	size = (size + ram_width - 1) & ~(ram_width - 1);
	if (size > ram_depth) {
		dwc_print0(dev, "Requested Rx FIFO size too large\n");
		dwc_print0(dev, "Not setting Rx FIFO size\n");
		goto rxerr;
	}

	dwc_debug1(dev, "Setting GRXFIFOSIZ0 = 0x%08x\n",
		   (size / ram_width) << DWC_FIFOSZ_DEPTH_SHIFT |
		   prev_start << DWC_FIFOSZ_STARTADDR_SHIFT);
	dwc_wr32(dev, &global_regs->grxfifosiz[0],
		 (size / ram_width) << DWC_FIFOSZ_DEPTH_SHIFT |
		 prev_start << DWC_FIFOSZ_STARTADDR_SHIFT);
	dwc_debug1(dev, "GRXFIFOSIZ0 = 0x%08x\n",
		   dwc_rd32(dev, &global_regs->grxfifosiz[0]));
	dwc_debug2(dev, "Rx FIFO size = %d bytes out of %d available\n",
		   size, ram_depth);

	/*
	 * If thresholding is enabled in GRXTHRCFG, update USBRxPktCnt according
	 * to the new FIFO size
	 */
	rxthrcfg = dwc_rd32(dev, &global_regs->grxthrcfg);
	dwc_debug1(dev, "GRXTHRCFG = 0x%08x\n", rxthrcfg);
	if (rxthrcfg & DWC_RXTHRCTL_USB_RX_PKT_CNT_EN_BIT) {
		cnt = (size - ram_width * 4) / 1024;
		if (cnt > 0) {
			if (cnt > DWC_RXTHRCTL_USB_RX_PKT_CNT_BITS >>
					DWC_RXTHRCTL_USB_RX_PKT_CNT_SHIFT)
				cnt = DWC_RXTHRCTL_USB_RX_PKT_CNT_BITS >>
					DWC_RXTHRCTL_USB_RX_PKT_CNT_SHIFT;
			bst = rxthrcfg >>
				DWC_RXTHRCTL_USB_MAX_RX_BURST_SIZE_SHIFT &
			     DWC_RXTHRCTL_USB_MAX_RX_BURST_SIZE_BITS >>
				DWC_RXTHRCTL_USB_MAX_RX_BURST_SIZE_SHIFT;
			if (cnt > bst)
				cnt = bst;
			if (cnt < 1)
				goto disable;
			rxthrcfg &= ~DWC_RXTHRCTL_USB_RX_PKT_CNT_BITS;
			rxthrcfg |= cnt << DWC_RXTHRCTL_USB_RX_PKT_CNT_SHIFT;
		} else {
disable:
			rxthrcfg &= ~DWC_RXTHRCTL_USB_RX_PKT_CNT_EN_BIT;
		}
		dwc_debug1(dev, "Setting GRXTHRCFG = 0x%08x\n", rxthrcfg);
		dwc_wr32(dev, &global_regs->grxthrcfg, rxthrcfg);
	}
rxerr:
	return;
}

/**
 * This routine initializes the DWC_usb3 controller registers.
 *
 * If the <strong><em>soft_reset</em></strong> parameter is
 * <strong>true</strong>, then this routine must be called in a context that
 * allows <em>dwc_msleep()</em> to be used, because that function is called
 * while waiting for the core to come out of reset.
 *
 * This routine is called by dwc_usb3_pcd_init() when the driver is loaded,
 * so it normally does not need to be called separately, except in special
 * circumstances, such as when exiting from hibernation.
 *
 * @param dev           Programming view of DWC_usb3 controller.
 * @param soft_reset    True if doing a soft reset of the core.
 * @param restore       True if restoring register state after hibernation.
 */
void dwc_usb3_pcd_device_init(dwc_usb3_device_t *dev, int soft_reset,
			      int restore)
{
	dwc_usb3_core_global_regs_t __iomem *global_regs =
						dev->core_global_regs;
	dwc_usb3_pcd_t *pcd = &dev->pcd;
	u32 temp;
	int i, ram_width, ram_depth, size;

	dwc_debug1(dev, "%s()\n", __func__);

	if (dev->program_gsbuscfg) {
		dwc_debug2(dev, "Programming SBUSCFG0,1 to %08x %08x\n",
			   dev->gsbuscfg0, dev->gsbuscfg1);
		dwc_wr32(dev, &global_regs->gsbuscfg0, dev->gsbuscfg0);
		dwc_wr32(dev, &global_regs->gsbuscfg1, dev->gsbuscfg1);
	}

	if (dev->soft_reset_hook) {
		dev->soft_reset_hook(dev, soft_reset, restore);
	} else {
		/*
		 * TODO Workaround: PCD can't handle soft reset during HNP.
		 * RTL issue will be fixed. Skip the reset when called with
		 * soft_reset=0. When not configured for OTG do the reset
		 * unconditionally.
		 */
		if (soft_reset) {
			/* Soft-reset the core */
			temp = dwc_rd32(dev, &pcd->dev_global_regs->dctl);
			temp &= ~DWC_DCTL_RUN_STOP_BIT;
			temp |= DWC_DCTL_CSFT_RST_BIT;
			dwc_wr32(dev, &pcd->dev_global_regs->dctl, temp);

			/* Wait for core to come out of reset */
			do {
				dwc_msleep(dev, 1);
				temp = dwc_rd32(dev, &pcd->dev_global_regs->dctl);
			} while (temp & DWC_DCTL_CSFT_RST_BIT);

			/* Wait for at least 3 PHY clocks */
			dwc_msleep(dev, 1);
		}
	}

	/* Soft reset clears the GSBUSCFG registers, so write them again */
	if (dev->program_gsbuscfg) {
		dwc_debug2(dev, "Programming SBUSCFG0,1 to %08x %08x\n",
			   dev->gsbuscfg0, dev->gsbuscfg1);
		dwc_wr32(dev, &global_regs->gsbuscfg0, dev->gsbuscfg0);
		dwc_wr32(dev, &global_regs->gsbuscfg1, dev->gsbuscfg1);
	}

	pcd->link_state = 0;
	pcd->wkup_rdy = 0;

	if (dev->phy_config_hook) {
		dev->phy_config_hook(dev, soft_reset, restore);
	}

	if (dev->fifo_sizing_hook) {
		dev->fifo_sizing_hook(dev, soft_reset, restore);
	} else {
		/*
		 * Set Tx FIFO sizes
		 */
		ram_width = (dev->hwparams0 >> DWC_HWP0_MDWIDTH_SHIFT &
			     DWC_HWP0_MDWIDTH_BITS >> DWC_HWP0_MDWIDTH_SHIFT)
			/ 8;
		ram_depth = (dev->hwparams7 >> DWC_HWP7_RAM1_DEPTH_SHIFT &
			     DWC_HWP7_RAM1_DEPTH_BITS >> DWC_HWP7_RAM1_DEPTH_SHIFT)
			* ram_width;
		dwc_debug2(dev, "RAM width:%d RAM1 depth:%d\n", ram_width, ram_depth);

		for (i = 0; i < pcd->num_in_eps + 1; i++) {
			size = dwc_rd32(dev, &global_regs->gtxfifosiz[i]);
			dwc_debug2(dev, "Initial GTXFIFOSIZ%d = 0x%08x\n", i, size);
		}

		/* Only set if non-default Tx FIFO sizes were requested */
		if (dev->core_params->txfsz_cnt)
			dwc_usb3_set_tx_fifo_size(dev, dev->core_params->txfsz);

		/*
		 * Set Rx FIFO size
		 */
		ram_depth = (dev->hwparams7 >> DWC_HWP7_RAM2_DEPTH_SHIFT &
			     DWC_HWP7_RAM2_DEPTH_BITS >> DWC_HWP7_RAM2_DEPTH_SHIFT)
			* ram_width;
		dwc_debug1(dev, "RAM2 depth:%d\n", ram_depth);
		size = dwc_rd32(dev, &global_regs->grxfifosiz[0]);
		dwc_debug1(dev, "Initial GRXFIFOSIZ0 = 0x%08x\n", size);
		size = dev->core_params->rxfsz;

		/* Only set if non-default Rx FIFO size was requested */
		if (size)
			dwc_usb3_set_rx_fifo_size(dev, size);
	}

#if 1
	if (dev->gctl_init_hook) {
		dev->gctl_init_hook(dev, soft_reset, restore);
	} else {
		temp = dwc_rd32(dev, &global_regs->gctl);
		temp &= ~(DWC_GCTL_PRT_CAP_DIR_BITS | DWC_GCTL_SCALE_DOWN_BITS);

#ifdef DWC_STAR_9000468158_WORKAROUND
		temp |= DWC_GCTL_U2RSTECN_BIT;
#endif
#ifdef CONFIG_USB_OTG_DWC
		temp |= DWC_GCTL_PRT_CAP_OTG << DWC_GCTL_PRT_CAP_DIR_SHIFT;
#else
		temp |= DWC_GCTL_PRT_CAP_DEVICE << DWC_GCTL_PRT_CAP_DIR_SHIFT;
#endif
#ifdef COSIM
		/* Scale down, disable scrambling */
		temp |= 3 << DWC_GCTL_SCALE_DOWN_SHIFT | DWC_GCTL_DIS_SCRAMBLE_BIT;
#else
# if 1
		temp &= ~DWC_GCTL_PWR_DN_SCALE_BITS;

		switch (dev->core_params->phy) {
		case 3:		// 16-bit UTMI+ SNPS Phy
		case 2:		// 8-bit UTMI+ / ULPI TI or SNPS Phy
			/* Set power down scale */
			temp |= 0x270 << DWC_GCTL_PWR_DN_SCALE_SHIFT;
			break;
		case 1:		// old 8-bit UTMI+ SNPS Phy
			/* Set LFPS filter */
			dwc_wr32(dev, &global_regs->gusb3pipectl[0],
				 DWC_PIPECTL_LFPS_FILTER_BIT |
				 1 << DWC_PIPECTL_TX_DEMPH_SHIFT);

			/* Set power down scale */
			temp |= 0x270 << DWC_GCTL_PWR_DN_SCALE_SHIFT;
			break;
		default:	// RocketIO Phy
			/* Set power down scale, disable scrambling */
			temp |= 0x1e84 << DWC_GCTL_PWR_DN_SCALE_SHIFT |
				DWC_GCTL_DEBUG_ATTACH_BIT |
				DWC_GCTL_DIS_SCRAMBLE_BIT;
		}
# endif
#endif
		dwc_wr32(dev, &global_regs->gctl, temp);
	}
#endif
	/* Initialize the Event Buffer registers */
	dwc_usb3_init_eventbuf(dev, 0, dev->event_buf[0], DWC_EVENT_BUF_SIZE,
			       dev->event_buf_dma[0]);
	dev->event_ptr[0] = dev->event_buf[0];

#ifdef CONFIG_USB_OTG_DWC
	/* TODO Workaround: this is a workaround for OTG 3.0 where the utmi
	 * clock doesn't come unless the device speed is momentarily programmed
	 * to HS. This causes the peripheral state on OSTS to not get set
	 * properly.
	 */
	temp = dwc_rd32(dev, &pcd->dev_global_regs->dcfg);
	temp &= ~(DWC_DCFG_DEVSPD_BITS << DWC_DCFG_DEVSPD_SHIFT);
	temp |= DWC_SPEED_HS_PHY_30MHZ_OR_60MHZ	<< DWC_DCFG_DEVSPD_SHIFT;
	dwc_wr32(dev, &pcd->dev_global_regs->dcfg, temp);
	dwc_udelay(dev, 200);
#endif

	/* If forcing to a USB2 mode was requested */
	if (dev->core_params->usb2mode == 1) {
		/* Set speed to Full */
		temp = dwc_rd32(dev, &pcd->dev_global_regs->dcfg);
		temp &= ~(DWC_DCFG_DEVSPD_BITS << DWC_DCFG_DEVSPD_SHIFT);
		temp |= DWC_SPEED_FS_PHY_30MHZ_OR_60MHZ
						<< DWC_DCFG_DEVSPD_SHIFT;
		dwc_wr32(dev, &pcd->dev_global_regs->dcfg, temp);

	} else if ((dev->core_params->usb2mode == 2)
#ifdef CONFIG_USB_OTG_DWC
		 /* Check the OTG_SS bit when in OTG mode to see if superspeed
		  * is supported
		  */
		|| !(dev->hwparams6 & DWC_HWP6_EN_OTG_BIT)
#endif
	) {
		/* Set speed to High */
		temp = dwc_rd32(dev, &pcd->dev_global_regs->dcfg);
		temp &= ~(DWC_DCFG_DEVSPD_BITS << DWC_DCFG_DEVSPD_SHIFT);
		temp |= DWC_SPEED_HS_PHY_30MHZ_OR_60MHZ
						<< DWC_DCFG_DEVSPD_SHIFT;
		dwc_wr32(dev, &pcd->dev_global_regs->dcfg, temp);

	} else {
		/* Set speed to Super */
		temp = dwc_rd32(dev, &pcd->dev_global_regs->dcfg);
		temp &= ~(DWC_DCFG_DEVSPD_BITS << DWC_DCFG_DEVSPD_SHIFT);

		if ((dev->hwparams3 & DWC_HWP3_SSPHY_IFC_BITS) == 0)
			temp |= DWC_SPEED_HS_PHY_30MHZ_OR_60MHZ
						<< DWC_DCFG_DEVSPD_SHIFT;
		else
			temp |= DWC_SPEED_SS_PHY_125MHZ_OR_250MHZ
						<< DWC_DCFG_DEVSPD_SHIFT;
		dwc_wr32(dev, &pcd->dev_global_regs->dcfg, temp);
	}

	/* If LPM enable was requested */
	if (dev->core_params->lpmctl) {
		/* Set LPMCap bit */
		temp = dwc_rd32(dev, &pcd->dev_global_regs->dcfg);
		temp |= DWC_DCFG_LPM_CAP_BIT;
		dwc_wr32(dev, &pcd->dev_global_regs->dcfg, temp);

		if (dev->core_params->lpmctl > 1) {
			/* Set AppL1Res bit */
			temp = dwc_rd32(dev, &pcd->dev_global_regs->dctl);
			temp |= DWC_DCTL_APP_L1_RES_BIT;
			dwc_wr32(dev, &pcd->dev_global_regs->dctl, temp);
		}
	}

	/* If non-default NUMP was requested */
	if (dev->core_params->nump > 0 && dev->core_params->nump <= 16) {
		/* Set NUMP */
		temp = dwc_rd32(dev, &pcd->dev_global_regs->dcfg);
		temp &= ~DWC_DCFG_NUM_RCV_BUF_BITS;
		temp |= dev->core_params->nump << DWC_DCFG_NUM_RCV_BUF_SHIFT;
		dwc_wr32(dev, &pcd->dev_global_regs->dcfg, temp);
	}

	if (dev->set_address_hook) {
		dev->set_address_hook(dev, soft_reset, restore);
	} else {
		if (!restore)
			/* Set device address to 0 */
			dwc_usb3_set_address(pcd, 0);
	}

	/* Enable hibernation if supported */
	if (dev->core_params->hibernate &&
	    (dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT) {
		/* Enable global hibernation bit */
		temp = dwc_rd32(dev, &global_regs->gctl);
		temp |= DWC_GCTL_GBL_HIBER_EN_BIT;
		if (dev->core_params->clkgatingen)
			temp &= ~DWC_GCTL_DSBL_CLCK_GTNG_BIT;
		else
			temp |= DWC_GCTL_DSBL_CLCK_GTNG_BIT;
		dwc_wr32(dev, &global_regs->gctl, temp);

		if (dev->core_params->lpmctl) {
			/* Set L1 hibernation values */
			temp = dwc_rd32(dev, &pcd->dev_global_regs->dctl);
			temp &= ~DWC_DCTL_HIRD_THR_BITS;
			if (dev->hird_thresh)
				temp |= dev->hird_thresh << DWC_DCTL_HIRD_THR_SHIFT &
					DWC_DCTL_HIRD_THR_BITS;
			else
				temp |= 0x1c << DWC_DCTL_HIRD_THR_SHIFT;

			/* Enable L1 hibernation */
			temp |= DWC_DCTL_L1_HIBER_EN_BIT;
			dwc_wr32(dev, &pcd->dev_global_regs->dctl, temp);
		}

		if (!restore) {
			/* Issue Set Scratchpad Buffer Array command */
			dwc_usb3_set_scratchpad_buf_array(pcd,
					pcd->hiber_scratchpad_array_dma);
		}

		/* Set GUSB2PHYCFG[6] (suspend 2.0 phy) */
		temp = dwc_rd32(dev, &global_regs->gusb2phycfg[0]);
		temp |= DWC_USB2PHYCFG_SUS_PHY_BIT;
		if (dev->core_params->lpmctl)
			temp |= DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
		dwc_wr32(dev, &global_regs->gusb2phycfg[0], temp);

#ifndef SELA_PLATFORM
		/* Set GUSB3PIPECTL[17] (suspend SS phy) */
		temp = dwc_rd32(dev, &global_regs->gusb3pipectl[0]);
		temp |= DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(dev, &global_regs->gusb3pipectl[0], temp);
#endif
	} else {
		if (dev->core_params->phyctl) {
			/* Enable Phy suspend */
			dwc_usb3_ena_usb3_phy_suspend(pcd);
			dwc_usb3_ena_usb2_phy_suspend(pcd);
		} else {
			/* Disable Phy suspend */
			dwc_usb3_dis_usb3_phy_suspend(pcd);
			dwc_usb3_dis_usb2_phy_suspend(pcd);
		}
	}

#ifndef CONFIG_USB_OTG_DWC
	/* Enable Global and Device interrupts */
	dwc_usb3_enable_device_interrupts(dev);
#endif
	/* Activate EP0 */
	dwc_usb3_ep0_activate(pcd, restore);

	if (dev->ep0_start_hook) {
		dev->ep0_start_hook(dev, soft_reset, restore);
	} else {
		/* Start EP0 to receive SETUP packets */
		dwc_usb3_pcd_ep0_out_start(pcd);
	}

	/* Enable EP0-OUT/IN in DALEPENA register */
	dwc_wr32(dev, &pcd->dev_global_regs->dalepena, 3);

	pcd->eps_enabled = 0;

#ifndef CONFIG_USB_OTG_DWC
	/* Set Run/Stop bit, and Keep-Connect bit if hibernation enabled */
	temp = dwc_rd32(dev, &pcd->dev_global_regs->dctl);
	temp |= DWC_DCTL_RUN_STOP_BIT;
	if (dev->core_params->hibernate &&
	    (dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT)
		temp |= DWC_DCTL_KEEP_CONNECT_BIT;
	dwc_wr32(dev, &pcd->dev_global_regs->dctl, temp);
#endif

#ifdef SSIC
	dwc_debug(dev, "SSIC Initialization\n");
	temp = dwc_rd32(dev, &dev->core_global_regs->gusb3pipectl[0]);
	temp |= DWC_PIPECTL_SSIC_EN_BIT;
	temp |= DWC_PIPECTL_PHY_SOFT_RST_BIT;
	dwc_wr32(dev, &dev->core_global_regs->gusb3pipectl[0], temp);
	dwc_wr32(dev, &dev->ssic_regs->sctl[0], 0);
	dwc_wr32(dev, &dev->ssic_regs->sevt[0], 0xffffffff);
	dwc_wr32(dev, &dev->ssic_regs->sevten[0], 0);
	dwc_wr32(dev, &dev->ssic_regs->sevten[0],
		 DWC_SEVTEN_ROM_INIT_CMPLT_EN_BIT | DWC_SEVTEN_MPHY_ST_CHNGD_EN_BIT);

	temp = dwc_rd32(dev, &dev->core_global_regs->gusb3rmmictl[0]);
	temp &= ~(DWC_RMMICTL_AUTO_EXIT_RRAP_BIT | DWC_RMMICTL_AUTO_ROM_RRAP_BIT |
		DWC_RMMICTL_AUTO_EXIT_H8_BIT | DWC_RMMICTL_AUTO_ROM_H8_BIT);
	dwc_wr32(dev, &dev->core_global_regs->gusb3rmmictl[0], temp);

	temp = dwc_rd32(dev, &dev->core_global_regs->gusb3pipectl[0]);
	temp &= ~DWC_PIPECTL_PHY_SOFT_RST_BIT;
	dwc_wr32(dev, &dev->core_global_regs->gusb3pipectl[0], temp);

	if (!handshake(dev, &dev->core_global_regs->gusb3rmmictl[0],
			DWC_RMMICTL_MPHY_STATE_BITS,
			(DWC_MPHY_STATE_HIBERN8 << DWC_RMMICTL_MPHY_STATE_SHIFT))) {
		dwc_error(dev, "%s: wait for mphy state hibern8 timed out\n", __func__);
	}

	temp = dwc_rd32(dev, &dev->ssic_regs->sevten[0]);
	temp |= (DWC_SEVTEN_RCMD_RES_SENT_EN_BIT |
		DWC_SEVTEN_RCMD_RES_RCVD_EN_BIT |
		DWC_SEVTEN_LACC_CMPLT_EN_BIT);
	dwc_wr32(dev, &dev->ssic_regs->sevten[0], temp);

	temp = dwc_rd32(dev, &dev->ssic_regs->sctl[0]);
	temp |= DWC_SCTL_CFG_DONE_BIT;
	dwc_wr32(dev, &dev->ssic_regs->sctl[0], temp);

	if (!handshake(dev, &dev->ssic_regs->sctl[0],
			DWC_SCTL_CFG_DONE_BIT, 0)) {
		dwc_error(dev, "%s: cfg done timed out\n", __func__);
	}
#endif
}

/**
 * This routine deinitializes the DWC_usb3 controller registers.
 *
 * This routine is called by dwc_usb3_pcd_remove() when the driver is unloaded,
 * so it normally does not need to be called separately,
 *
 * @param dev   Programming view of DWC_usb3 controller.
 */
void dwc_usb3_pcd_device_remove(dwc_usb3_device_t *dev)
{
	dwc_usb3_core_global_regs_t __iomem *global_regs =
						dev->core_global_regs;
	dwc_usb3_pcd_t *pcd = &dev->pcd;
	u32 temp;

	if (dev->hibernate >= DWC_HIBER_SLEEPING)
		return;

	/* Clear the Run/Stop and Keep-Connect bits */
	temp = dwc_rd32(dev, &pcd->dev_global_regs->dctl);
	temp &= ~(DWC_DCTL_RUN_STOP_BIT | DWC_DCTL_KEEP_CONNECT_BIT);
	dwc_wr32(dev, &pcd->dev_global_regs->dctl, temp);

	/* Disable device interrupts */
	dwc_wr32(dev, &pcd->dev_global_regs->devten, 0);

	/* Disable hibernation if supported */
	if (dev->core_params->hibernate &&
	    (dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT) {
		/* Clear GUSB3PIPECTL[17] (suspend SS phy) */
		temp = dwc_rd32(dev, &global_regs->gusb3pipectl[0]);
		temp &= ~DWC_PIPECTL_SUS_PHY_BIT;
		dwc_wr32(dev, &global_regs->gusb3pipectl[0], temp);

		/* Clear GUSB2PHYCFG[6] (suspend 2.0 phy) */
		temp = dwc_rd32(dev, &global_regs->gusb2phycfg[0]);
		temp &= ~DWC_USB2PHYCFG_SUS_PHY_BIT;
		temp &= ~DWC_USB2PHYCFG_ENBL_SLP_M_BIT;
		dwc_wr32(dev, &global_regs->gusb2phycfg[0], temp);

		/* Disable L1 hibernation */
		temp = dwc_rd32(dev, &pcd->dev_global_regs->dctl);
		temp &= ~DWC_DCTL_L1_HIBER_EN_BIT;
		dwc_wr32(dev, &pcd->dev_global_regs->dctl, temp);

		/* Disable global hibernation bit */
		temp = dwc_rd32(dev, &global_regs->gctl);
		temp &= ~DWC_GCTL_GBL_HIBER_EN_BIT;
		temp |= DWC_GCTL_DSBL_CLCK_GTNG_BIT;
		dwc_wr32(dev, &global_regs->gctl, temp);
	}
}
