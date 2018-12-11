#ifndef _DWC_WIN_DEFS_H_
#define _DWC_WIN_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 *
 * This file contains OS-specific includes and definitions.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

typedef unsigned long long	u64, u_int64_t;
typedef unsigned long		u_long;
typedef unsigned int		u32, u_int32_t, u_int;
typedef unsigned short		u16, u_int16_t, u_short;
typedef unsigned char		u8, u_int8_t, u_char;

typedef long long		s64;
typedef int			s32;
typedef short			s16;
typedef char			s8;

/** Type for DMA addresses */
typedef unsigned long		dwc_dma_t;
#define DWC_DMA_ADDR_INVALID	(~(dwc_dma_t)0)

/** Compiler 'packed' attribute for structs */
#define UPACKED			/* */

#define __iomem			/* */
#define __func__		__FUNCTION__

#define KERN_DEBUG	""	/* debug messages */
#define KERN_INFO	""	/* informational messages */
#define KERN_WARNING	""	/* warning messages */
#define KERN_ERR	""	/* error messages */

#define ESHUTDOWN	1001

#include "dwc_list.h"

/** @name Error Codes */
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

#define wmb()				do {} while (0)

#define interrupt_disable()		0
#define interrupt_enable()		do {} while (0)

#define spin_lock_init(p)		do {} while (0)
#define spin_lock(p)			do {} while (0)
#define spin_unlock(p)			do {} while (0)
#define spin_lock_irqsave(p, f)		do { (f) = interrupt_disable(); } while (0)
#define spin_unlock_irqrestore(p, f)	do { if (f) interrupt_enable(); } while (0)

struct task_struct {
	int dummy;
};

struct tasklet_struct {
	int dummy;
};

struct usb_ep {
	int dummy;
};

struct usb_request {
	int dummy;
};

#ifdef __cplusplus
}
#endif

#endif /* _DWC_WIN_DEFS_H_ */
