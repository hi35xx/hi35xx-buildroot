/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/dev.h $
 * $Revision: #12 $
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

#ifndef _DWC_DEV_H_
#define _DWC_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * This file contains the interface to the Linux driver.
 */

/**
 * This structure is a wrapper that encapsulates the driver components used to
 * manage a single DWC_usb3 controller.
 */
typedef struct dwc_usb3_device {
	/**
	 * OS-specific stuff. KEEP THIS AT THE VERY BEGINNING OF THE DEVICE
	 * STRUCT. OSes such as FreeBSD and NetBSD require this.
	 */

	/** Base address returned from ioremap() */
	volatile u8 __iomem *base;
	volatile u8 __iomem *gasket_base;

	/** Offset to 'gasket' registers (Synopsys FPGA only) */
	int gasket_ofs;

#if defined(__linux__) || defined(LINUXTEST)
	/** Device context */
	struct device *dev;

	/** Start address of IOMEM region */
	resource_size_t rsrc_start;
	resource_size_t gasket_start;

	/** Length of IOMEM region */
	resource_size_t rsrc_len;
	resource_size_t gasket_len;

	/** IRQ resource */
	int irq;

	struct task_struct *pme_thread;
#endif
	/* ==== End of OS-specific stuff ==== */

	/** Count of threads inside Gadget API */
	int hiber_cnt;

	/** Hibernation state */
	int hibernate;

#define DWC_HIBER_AWAKE		0
#define DWC_HIBER_ENTER_NOSAVE	1
#define DWC_HIBER_ENTER_SAVE	2
#define DWC_HIBER_SLEEPING	3
#define DWC_HIBER_WAIT_LINK_UP	4
#define DWC_HIBER_WAIT_U0	5
#define DWC_HIBER_SS_DIS_QUIRK	6

	int pme_ready;

	/** PCD structure */
	struct dwc_usb3_pcd pcd;

	/** Value from SNPSID register */
	u32 snpsid;

	/** Parameters that define how the core should be configured */
	dwc_usb3_core_params_t *core_params;

	/** Core Global registers starting at offset 100h */
	dwc_usb3_core_global_regs_t __iomem *core_global_regs;

#ifdef SSIC
	dwc_usb3_ssic_regs_t __iomem *ssic_regs;
#endif

	/** @{ */
#define DWC_EVENT_BUF_SIZE	256	// size in dwords
#define DWC_NUM_EVENT_BUFS	1
	/** Event Buffers for receiving interrupts. Up to 32 buffers are
	 * supported by the hardware, but we only use 1.
	 */
	u32 *event_ptr[DWC_NUM_EVENT_BUFS];
	u32 *event_buf[DWC_NUM_EVENT_BUFS];
	dwc_dma_t event_buf_dma[DWC_NUM_EVENT_BUFS];
	/** @} */

	/** Total RAM for FIFOs (Bytes) */
	u16 total_fifo_size;

	/** Size of Rx FIFO (Bytes) */
	u16 rx_fifo_size;

	/** @{ */
	/** Hardware Configuration - stored here for convenience */
	u32 hwparams0;
	u32 hwparams1;
	u32 hwparams2;
	u32 hwparams3;
	u32 hwparams4;
	u32 hwparams5;
	u32 hwparams6;
	u32 hwparams7;
	u32 hwparams8;
	/** @} */

	/** @{ */
	/** Register state, saved across core hibernation */
	u32 dcfg_save;
	u32 dctl_save;
	u32 gtxfifosiz0_save;
	u32 gtxfifosiz1_save;
	u32 gtxfifosiz2_save;
	u32 gtxfifosiz3_save;
	u32 grxfifosiz0_save;
	u32 guctl_save;
	u32 guctl1_save;
	/** @} */

	/** @{ */
	/** Hooks for customizing device initialization. See
	 *  dwc_usb3_pcd_device_init() in cil.c to see how these work.
	 */
	void (*soft_reset_hook)(struct dwc_usb3_device *dev, int softrst, int rstor);
	void (*phy_config_hook)(struct dwc_usb3_device *dev, int softrst, int rstor);
	void (*fifo_sizing_hook)(struct dwc_usb3_device *dev, int softrst, int rstor);
	void (*gctl_init_hook)(struct dwc_usb3_device *dev, int softrst, int rstor);
	void (*set_address_hook)(struct dwc_usb3_device *dev, int softrst, int rstor);
	void (*ep0_start_hook)(struct dwc_usb3_device *dev, int softrst, int rstor);
	/** @} */

	/** Value to write into the DCTL HIRD_Thresh field on register
	 * initialization. If 0 then a default value of 0x1c will be used.
	 */
	u32 hird_thresh;

	/** Values to write into GSBUSCFG0 and GSBUSCFG1 on initialization or
	 * when exiting from hibernation. 'program_gsbuscfg' below must also be
	 * set to 1 to enable the writing of these values.
	 */
	u32 gsbuscfg0;
	u32 gsbuscfg1;

	/** True if common functionality has been initialized */
	unsigned int cmn_initialized		: 1;

	/** True if Gadget has been initialized */
	unsigned int gadget_initialized		: 1;

	/** True if PCD has been initialized */
	unsigned int pcd_initialized		: 1;

	/** True if common IRQ handler has been installed */
	unsigned int cmn_irq_installed		: 1;

	/** True if sysfs functions have been installed */
	unsigned int sysfs_initialized		: 1;

	/** True if waiting for connect before resuming from hibernation */
	unsigned int hiber_wait_connect		: 1;

	/** True if waiting for U0 state before sending remote wake */
	unsigned int hiber_wait_u0		: 1;

	/** True if GBUSCFG0/GBUSCFG1 should be written with the above
	 * values when exiting hibernation */
	unsigned int program_gsbuscfg		: 1;
} dwc_usb3_device_t;

#ifdef __cplusplus
}
#endif

#endif /* _DWC_DEV_H_ */
