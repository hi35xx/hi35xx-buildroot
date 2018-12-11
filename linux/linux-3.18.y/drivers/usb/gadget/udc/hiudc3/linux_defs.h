#ifndef _DWC_LINUX_DEFS_H_
#define _DWC_LINUX_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 *
 * This file contains OS-specific includes and definitions.
 *
 */

#define DWC_DRIVER_VERSION	"2.90b - November 2014"
#define DWC_DRIVER_DESC		"SS USB3 Controller driver"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <linux/stat.h>
#include <linux/pci.h>

#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
# include <linux/irq.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
# include <linux/usb/ch9.h>
#else
# include <linux/usb_ch9.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
# include <linux/usb/gadget.h>
# include <linux/usb/otg.h>
#else
# include <linux/usb_gadget.h>
# include <linux/usb_otg.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
# include <asm/irq.h>
#endif

# include <asm/unaligned.h>
# include <asm/param.h>
# include <asm/io.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
typedef int		gfp_t;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
# define IRQF_SHARED	SA_SHIRQ
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
# define DWC_BOS_IN_GADGET
#endif

/** @name Error Codes */
/** @{ */
#define DWC_E_INVALID		EINVAL
#define DWC_E_NO_MEMORY		ENOMEM
#define DWC_E_NO_DEVICE		ENODEV
#define DWC_E_NOT_SUPPORTED	EOPNOTSUPP
#define DWC_E_TIMEOUT		ETIMEDOUT
#define DWC_E_BUSY		EBUSY
#define DWC_E_AGAIN		EAGAIN
#define DWC_E_ABORT		ECONNABORTED
#define DWC_E_SHUTDOWN		ESHUTDOWN
#define DWC_E_NO_DATA		ENODATA
#define DWC_E_DISCONNECT	ECONNRESET
#define DWC_E_UNKNOWN		EINVAL
#define DWC_E_NO_STREAM_RES	ENOSR
#define DWC_E_COMMUNICATION	ECOMM
#define DWC_E_OVERFLOW		EOVERFLOW
#define DWC_E_PROTOCOL		EPROTO
#define DWC_E_IN_PROGRESS	EINPROGRESS
#define DWC_E_PIPE		EPIPE
#define DWC_E_IO		EIO
#define DWC_E_NO_SPACE		ENOSPC
#define DWC_E_DOMAIN		EDOM
/** @} */

/** Compiler 'packed' attribute for structs */
#define UPACKED	__attribute__ ((__packed__))

/** @{ */
/** Type for DMA addresses */
typedef dma_addr_t		dwc_dma_t;
#define DWC_DMA_ADDR_INVALID	(~(dwc_dma_t)0)
/** @} */

/**
 * The number of DMA Descriptors (TRBs) to allocate for each endpoint type.
 * NOTE: The driver currently supports more than 1 TRB for Isoc EPs only.
 * So the values for Bulk and Intr must be 1.
 */
#define DWC_NUM_BULK_TRBS	1
#define DWC_NUM_INTR_TRBS	1
#define DWC_NUM_ISOC_TRBS	256

/**
 * These parameters may be specified when loading the module. They define how
 * the DWC_usb3 controller should be configured. The parameter values are passed
 * to the CIL initialization routine dwc_usb3_pcd_common_init().
 */
typedef struct dwc_usb3_core_params {
	int burst;
	int newcore;
	int phy;
	int wakeup;
	int pwrctl;
	int lpmctl;
	int phyctl;
	int usb2mode;
	int hibernate;
	int hiberdisc;
	int clkgatingen;
	int ssdisquirk;
	int nobos;
	int loop;
	int nump;
	int newcsr;
	int rxfsz;
	int txfsz[16];
	int txfsz_cnt;
	int baseline_besl;
	int deep_besl;
	int besl;
	int ebc;
} dwc_usb3_core_params_t;

extern dwc_usb3_core_params_t dwc_usb3_module_params;

#ifdef __cplusplus
}
#endif

#endif /* _DWC_LINUX_DEFS_H_ */
