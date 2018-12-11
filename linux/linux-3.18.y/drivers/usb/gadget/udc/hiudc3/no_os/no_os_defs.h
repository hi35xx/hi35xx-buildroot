#ifndef _DWC_NO_OS_DEFS_H_
#define _DWC_NO_OS_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 *
 * This file contains OS-specific includes and definitions.
 *
 */

#undef linux
#undef __linux
#undef __linux__

#ifdef LINUXTEST

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
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

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
# include <asm/irq.h>
#endif

#include <asm/io.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
typedef int		gfp_t;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
# define IRQF_SHARED	SA_SHIRQ
#endif

/** @{ */
/** Data type for DMA addresses */
typedef dma_addr_t		dwc_dma_t;
#define DWC_DMA_ADDR_INVALID	(~(dwc_dma_t)0)
/** @} */

/** Compiler 'packed' attribute */
#define UPACKED		__attribute__ ((__packed__))

/** Compiler 'aligned(16)' attribute */
#define UALIGNED16	__attribute__ ((__aligned__(16)))

#define dwc_init_spinlock(d, p)			spin_lock_init(p)
#define dwc_acquire_spinlock(d, p)		spin_lock(p)
#define dwc_release_spinlock(d, p)		spin_unlock(p)
#define dwc_acquire_spinlock_irq(d, p, f)	spin_lock_irqsave(p, f)
#define dwc_release_spinlock_irq(d, p, f)	spin_unlock_irqrestore(p, f)

#else /* !LINUXTEST */

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>	// for printf
#include <string.h>	// for memcpy

/** @{ */
/** Data types needed by the PCD */
typedef uint64_t	u64, u_int64_t;
typedef uint32_t	u32, u_int32_t;
typedef uint16_t	u16, u_int16_t;
typedef uint8_t		u8, u_int8_t;

typedef int64_t		s64;
typedef int32_t		s32;
typedef int16_t		s16;
typedef int8_t		s8;

typedef unsigned long	u_long;
typedef unsigned int	u_int;
typedef unsigned short	u_short;
typedef unsigned char	u_char;
/** @} */

/** @{ */
/** Data type for DMA addresses */
typedef unsigned long		dwc_dma_t;
#define DWC_DMA_ADDR_INVALID	(~(dwc_dma_t)0)
/** @} */

/** Compiler 'packed' attribute */
#define UPACKED		__attribute__ ((__packed__))

/** Compiler 'aligned(16)' attribute */
#define UALIGNED16	__attribute__ ((__aligned__(16)))

/** I/O memory attribute for pointers. Needed for Linux "sparse" tool. */
#define __iomem		/* */

#define KERN_DEBUG	""	/* debug messages */
#define KERN_INFO	""	/* informational messages */
#define KERN_WARNING	""	/* warning messages */
#define KERN_ERR	""	/* error messages */

// from /usr/include/asm-x86_64/errno.h
#define EIO		5	/* I/O error */
#define EAGAIN		11	/* Try again */
#define ENOMEM		12	/* Out of memory */
#define EBUSY		16	/* Device or resource busy */
#define ENODEV		19	/* No such device */
#define EINVAL		22	/* Invalid argument */
#define ENOSPC		28	/* No space left on device */
#define EPIPE		32	/* Broken pipe */
#define EDOM		33	/* Math argument out of domain of func */
#define ENODATA		61	/* No data available */
#define ENOSR		63	/* Out of streams resources */
#define ECOMM		70	/* Communication error on send */
#define EPROTO		71	/* Protocol error */
#define EOVERFLOW	75	/* Value too large for defined data type */
#define ERESTART	85	/* Interrupted system call should be restarted */
#define EOPNOTSUPP	95	/* Operation not supported on transport endpoint */
#define ECONNABORTED	103	/* Software caused connection abort */
#define ECONNRESET	104	/* Connection reset by peer */
#define ESHUTDOWN	108	/* Cannot send after transport endpoint shutdown */
#define ETIMEDOUT	110	/* Connection timed out */
#define EINPROGRESS	115	/* Operation now in progress */

/** Write memory barrier macro */
#define wmb()		do {} while (0)

#define interrupt_disable()	0
#define interrupt_enable()	do {} while (0)

#define dwc_init_spinlock(d, p)			do {} while (0)
#define dwc_acquire_spinlock(d, p)		do {} while (0)
#define dwc_release_spinlock(d, p)		do {} while (0)
#define dwc_acquire_spinlock_irq(d, p, f)	do { (f) = interrupt_disable(); } while (0)
#define dwc_release_spinlock_irq(d, p, f)	do { if (f) interrupt_enable(); } while (0)

struct task_struct {
	int dummy;
};

struct tasklet_struct {
	int dummy;
};

#endif /* !LINUXTEST */

#include "usb.h"
#include "dwc_list.h"

/** @name Error Codes */
/** @{ */
#define DWC_E_INVALID		EINVAL
#define DWC_E_NO_MEMORY		ENOMEM
#define DWC_E_NO_DEVICE		ENODEV
#define DWC_E_NOT_SUPPORTED	EOPNOTSUPP
#define DWC_E_TIMEOUT		ETIMEDOUT
#define DWC_E_BUSY		EBUSY
#define DWC_E_AGAIN		EAGAIN
#define DWC_E_RESTART		ERESTART
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

/**
 * The number of DMA Descriptors (TRBs) to allocate for each endpoint type.
 * NOTE: The driver currently supports more than 1 TRB for Isoc EPs only.
 * So the values for Bulk and Intr must be 1.
 */
#define DWC_NUM_BULK_TRBS	1
#define DWC_NUM_INTR_TRBS	1
#define DWC_NUM_ISOC_TRBS	32

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

// linux/usb/gadget.h

/**
 * Platform-specific USB endpoint
 */
typedef struct usb_ep {
	const void	*desc;
	const void	*comp_desc;
	unsigned	maxpacket:16;
	u8		address;
} usb_ep_t;

/**
 * Platform-specific USB request
 */
typedef struct usb_request {
	void		*buf;
	unsigned	length;
	dwc_dma_t	dma;

	unsigned	stream_id:16;
	unsigned	zero:1;

	void		(*complete)(usb_ep_t *ep, struct usb_request *req);

	int		status;
	unsigned	actual;
} usb_request_t;

/**
 */
static inline void dwc_usb3_task_schedule(struct tasklet_struct *tasklet)
{
#ifdef LINUXTEST
	tasklet_schedule(tasklet);
#endif
}

/* Make the following structure type definitions "packed" if using a Microsoft
 * compiler. The UPACKED attribute (defined above) embedded in the structure
 * type definitions does the same thing for GCC. Other compilers may need
 * something different.
 */
#ifdef _MSC_VER
#include <pshpack1.h>
#endif

/**
 */
typedef struct fs_config_desc_st {
	usb_config_descriptor_t			config_desc;
	usb_interface_descriptor_t		intf_desc;
	usb_endpoint_descriptor_t		bulk_in_ep_desc;
	usb_endpoint_descriptor_t		bulk_out_ep_desc;
} UPACKED fs_config_desc_t;

/**
 */
typedef struct hs_config_desc_st {
	usb_config_descriptor_t			config_desc;
	usb_interface_descriptor_t		intf_desc;
	usb_endpoint_descriptor_t		bulk_in_ep_desc;
	usb_endpoint_descriptor_t		bulk_out_ep_desc;
} UPACKED hs_config_desc_t;

/**
 */
typedef struct ss_config_desc_st {
	usb_config_descriptor_t			config_desc;
	usb_interface_descriptor_t		intf_desc;
	usb_endpoint_descriptor_t		bulk_in_ep_desc;
	ss_endpoint_companion_descriptor_t	bulk_in_ss_ep_comp_desc;
	usb_endpoint_descriptor_t		bulk_out_ep_desc;
	ss_endpoint_companion_descriptor_t	bulk_out_ss_ep_comp_desc;
} UPACKED ss_config_desc_t;

/* Stop packing structure type definitions */
#ifdef _MSC_VER
#include <poppack.h>
#endif

/* These structures are defined in no_os_ep0.c */
extern fs_config_desc_t fs_config_desc;
extern hs_config_desc_t hs_config_desc;
extern ss_config_desc_t ss_config_desc;

/**
 * Function driver API routines
 */

struct dwc_usb3_device;	/* so the simulation code can include just this file */

extern dwc_usb3_core_params_t usb3ss_module_params;

#ifndef LINUXTEST
extern struct dwc_usb3_device *dwc_usb3_driver_init(u32 base_addr_dwc);
extern void dwc_usb3_driver_remove(void);
extern void dwc_usb3_common_irq(int irq, void *dev);
#endif

extern usb_ep_t *dwc_usb3_ep_enable(struct dwc_usb3_device *usb3_dev, const void *epdesc,
				    const void *epcomp);
extern int dwc_usb3_ep_disable(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep);
extern usb_request_t *dwc_usb3_alloc_request(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep);
extern void dwc_usb3_free_request(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep,
				  usb_request_t *usb_req);
extern int dwc_usb3_ep_queue(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep,
			     usb_request_t *usb_req);
extern int dwc_usb3_ep_dequeue(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep,
			       usb_request_t *usb_req);
extern int dwc_usb3_wait_pme(struct dwc_usb3_device *usb3_dev);
extern int dwc_usb3_handle_pme_intr(struct dwc_usb3_device *usb3_dev);

extern int dwc_usb3_function_init(struct dwc_usb3_device *usb3_dev);
extern void dwc_usb3_function_remove(struct dwc_usb3_device *usb3_dev);
extern int dwc_usb3_function_connect(struct dwc_usb3_device *usb3_dev, int speed);
extern int dwc_usb3_function_disconnect(struct dwc_usb3_device *usb3_dev);

#ifdef __cplusplus
}
#endif

#endif /* _DWC_NO_OS_DEFS_H_ */
