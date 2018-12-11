/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/cil.h $
 * $Revision: #45 $
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

#ifndef _DWC_CIL_H_
#define _DWC_CIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * This file contains the interface to the Core Interface Layer.
 */

/** @addtogroup init_api_grp Initialization API Routines
 *
 * These routines handle initialization of the CIL and PCD driver components
 * and the DWC_usb3 controller.
 */
/** @{ */
extern int dwc_usb3_pcd_check_snpsid(dwc_usb3_device_t *dev, u32 addr_ofs);
extern int dwc_usb3_pcd_common_init(dwc_usb3_device_t *dev,
				    volatile u8 __iomem *base,
				    dwc_usb3_core_params_t *core_params);
extern void dwc_usb3_pcd_common_remove(dwc_usb3_device_t *dev);
extern void dwc_usb3_pcd_device_init(dwc_usb3_device_t *dev, int soft_reset,
				     int restore);
extern void dwc_usb3_pcd_device_remove(dwc_usb3_device_t *dev);
/** @} */

/** @addtogroup misc_api_grp */
/** @{ */
extern int dwc_usb3_pcd_get_link_state(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_pcd_set_link_state(dwc_usb3_pcd_t *pcd, int state);
extern void dwc_usb3_pcd_remote_wake(dwc_usb3_pcd_t *pcd, int function);
extern void dwc_usb3_pcd_do_test_mode(unsigned long data);
/** @} */

/* Peripheral CIL Routines
 *
 * The following routines support managing the DWC_usb3 controller in
 * peripheral mode.
 */
extern void dwc_usb3_fill_desc(dwc_usb3_dma_desc_t *desc, dwc_dma_t dma_addr,
			       u32 dma_len, u32 stream, u32 type,
			       u32 ctrlbits, int own);
extern void dwc_usb3_start_desc_chain(dwc_usb3_dma_desc_t *desc);
extern void dwc_usb3_end_desc_chain(dwc_usb3_dma_desc_t *desc);
extern void dwc_usb3_enable_desc(dwc_usb3_dma_desc_t *desc);
extern void dwc_usb3_disable_desc(dwc_usb3_dma_desc_t *desc);
extern int dwc_usb3_xmit_fn_remote_wake(dwc_usb3_pcd_t *pcd, u32 intf);
extern int dwc_usb3_xmit_ltm(dwc_usb3_pcd_t *pcd, u32 value);
extern int dwc_usb3_xmit_host_role_request(dwc_usb3_pcd_t *pcd, u32 param);
extern int dwc_usb3_set_scratchpad_buf_array(dwc_usb3_pcd_t *pcd,
					     dwc_dma_t dma_addr);
extern int dwc_usb3_flush_fifo(dwc_usb3_pcd_t *pcd, u32 fifo_sel);
extern int dwc_usb3_dep_cfg(dwc_usb3_pcd_t *pcd,
			    dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			    u32 depcfg0, u32 depcfg1,
			    u32 depcfg2);
extern int dwc_usb3_dep_xfercfg(dwc_usb3_pcd_t *pcd,
				dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				u32 depstrmcfg);
extern u32 dwc_usb3_dep_getepstate(dwc_usb3_pcd_t *pcd,
				   dwc_usb3_dev_ep_regs_t __iomem *ep_reg);
extern int dwc_usb3_dep_sstall(dwc_usb3_pcd_t *pcd,
			       dwc_usb3_dev_ep_regs_t __iomem *ep_reg);
extern int dwc_usb3_dep_cstall(dwc_usb3_pcd_t *pcd,
			       dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
			       int clr_pend);
extern int dwc_usb3_dep_startxfer(dwc_usb3_pcd_t *pcd,
				  dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				  dwc_dma_t dma_addr, u32 stream_or_uf);
extern int dwc_usb3_dep_updatexfer(dwc_usb3_pcd_t *pcd,
				   dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				   u32 tri);
extern int dwc_usb3_dep_endxfer(dwc_usb3_pcd_t *pcd,
				dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				u32 tri, int flags, void *condition);
#define DWC_ENDXFER_FORCE	1
#define DWC_ENDXFER_NODELAY	2

#ifdef DWC_STAR_9000463548_WORKAROUND
extern int dwc_usb3_dep_endxfer_nowait(dwc_usb3_pcd_t *pcd,
				       dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				       u32 tri, int flags);
extern int dwc_usb3_dep_wait_endxfer(dwc_usb3_pcd_t *pcd,
				     dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				     void *condition);
#endif
extern int dwc_usb3_dep_startnewcfg(dwc_usb3_pcd_t *pcd,
				    dwc_usb3_dev_ep_regs_t __iomem *ep_reg,
				    u32 rsrcidx);
extern int dwc_usb3_enable_ep(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep);
extern int dwc_usb3_disable_ep(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep);
extern int dwc_usb3_get_device_speed(dwc_usb3_pcd_t *pcd);
extern int dwc_usb3_get_frame(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_set_address(dwc_usb3_pcd_t *pcd, int addr);
extern void dwc_usb3_ena_usb2_suspend(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_dis_usb2_suspend(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_accept_u1(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_accept_u2(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_enable_u1(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_enable_u2(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_disable_u1(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_disable_u2(dwc_usb3_pcd_t *pcd);
extern int dwc_usb3_u1_enabled(dwc_usb3_pcd_t *pcd);
extern int dwc_usb3_u2_enabled(dwc_usb3_pcd_t *pcd);
extern void dwc_usb3_clr_eps_enabled(dwc_usb3_pcd_t *pcd);

extern void dwc_usb3_dump_dev_registers(dwc_usb3_pcd_t *pcd);

#define dwc_usb3_is_hwo(desc)	((desc)->control & DWC_DSCCTL_HWO_BIT)
#define dwc_usb3_is_ioc(desc)	((desc)->control & DWC_DSCCTL_IOC_BIT)

#define dwc_usb3_get_xfercnt(desc)				\
	((desc)->status >> DWC_DSCSTS_XFRCNT_SHIFT &		\
	 DWC_DSCSTS_XFRCNT_BITS >> DWC_DSCSTS_XFRCNT_SHIFT)

#define dwc_usb3_get_xfersts(desc)				\
	((desc)->status >> DWC_DSCSTS_TRBRSP_SHIFT &		\
	 DWC_DSCSTS_TRBRSP_BITS >> DWC_DSCSTS_TRBRSP_SHIFT)

#define dwc_usb3_get_xfersofn(desc)				\
	((desc)->control >> DWC_DSCCTL_STRMID_SOFN_SHIFT &	\
	 DWC_DSCCTL_STRMID_SOFN_BITS >> DWC_DSCCTL_STRMID_SOFN_SHIFT)

#define dwc_usb3_get_eventsofn(event)				\
	((event) >> DWC_DEPEVT_ISOC_UFRAME_NUM_SHIFT &	\
	 DWC_DEPEVT_ISOC_UFRAME_NUM_BITS >> DWC_DEPEVT_ISOC_UFRAME_NUM_SHIFT)

/* Common CIL Routines
 */
extern void dwc_usb3_dump_dbgregs(dwc_usb3_device_t *dev);
extern void dwc_usb3_dump_global_registers(dwc_usb3_device_t *dev);
extern void dwc_usb3_set_tx_fifo_size(dwc_usb3_device_t *dev, int *sz);
extern void dwc_usb3_set_rx_fifo_size(dwc_usb3_device_t *dev, int size);
extern void dwc_usb3_init_eventbuf(dwc_usb3_device_t *dev, int bufno,
			u32 *addr, unsigned int size, dwc_dma_t dma_addr);
extern void dwc_usb3_dis_flush_eventbuf_intr(dwc_usb3_device_t *dev, int bufno);
extern void dwc_usb3_enable_common_interrupts(dwc_usb3_device_t *dev);
extern void dwc_usb3_enable_device_interrupts(dwc_usb3_device_t *dev);
extern int dwc_usb3_handle_event(dwc_usb3_device_t *dev);
extern int dwc_usb3_irq(dwc_usb3_device_t *dev, int irq);

/**
 * This routine returns the current operating mode, host or device.
 *
 * @return 0 - Device Mode, 1 - Host Mode
 */
static __inline u32 dwc_usb3_mode(dwc_usb3_device_t *dev)
{
	return dwc_rd32(dev, &dev->core_global_regs->gsts) & 0x1;
}

/**
 * This routine returns true if the current operating mode is Device.
 *
 * @return 1 - Device mode, 0 - Not Device mode
 */
static __inline int dwc_usb3_is_device_mode(dwc_usb3_device_t *dev)
{
	return dwc_usb3_mode(dev) != DWC_GSTS_HOST_MODE;
}

/**
 * This routine returns true if the current operating mode is Host.
 *
 * @return 1 - Host mode, 0 - Not Host mode
 */
static __inline int dwc_usb3_is_host_mode(dwc_usb3_device_t *dev)
{
	return dwc_usb3_mode(dev) == DWC_GSTS_HOST_MODE;
}

#ifdef __cplusplus
}
#endif

#endif /* _DWC_CIL_H_ */
