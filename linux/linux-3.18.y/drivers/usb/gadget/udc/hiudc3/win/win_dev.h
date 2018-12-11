/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/win/win_dev.h $
 * $Revision: #9 $
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

#ifndef _DWC_WIN_DEV_H_
#define _DWC_WIN_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 */

/** Wrapper function for _handshake() */
#define handshake(_dev_, _ptr_, _mask_, _done_)	\
	_handshake(_dev_, _ptr_, _mask_, _done_)

/** Takes a usb req pointer and returns the associated pcd req pointer */
#define dwc_usb3_get_pcd_req(usbreq)			\
	((dwc_usb3_pcd_req_t *)((char *)(usbreq) -	\
		offsetof(struct dwc_usb3_pcd_req, usb_req)))

/** Takes a usb ep pointer and returns the associated pcd ep pointer */
#define dwc_usb3_get_pcd_ep(usbep)			\
	((dwc_usb3_pcd_ep_t *)((char *)(usbep) -	\
		offsetof(struct dwc_usb3_pcd_ep, usb_ep)))

/**
 * Register read/write.
 */
static __inline u32 dwc_rd32(struct dwc_usb3_device *dev, volatile u32 *adr)
{
	return *adr;
}

static __inline void dwc_wr32(struct dwc_usb3_device *dev, volatile u32 *adr, u32 val)
{
	*adr = val;
}

/**
 * Non-sleeping delays.
 */
#define dwc_udelay(dev, us)	do {} while (0)
#define dwc_mdelay(dev, ms)	do {} while (0)

/**
 * Sleeping delay.
 */
#define dwc_msleep(dev, ms)	do {} while (0)

/**
 * Debugging support - vanishes in non-debug builds.
 */

/** Prefix string for print macros. */
#define USB3_DWC	"dwc_usb3: "

//#ifdef DEBUG
# define dwc_debug	printf
//#else
//# define dwc_debug(dev, x)	do {} while (0)
//#endif /* DEBUG */

# define dwc_debug0(dev, fmt)			dwc_debug(fmt)
# define dwc_debug1(dev, fmt, a)		dwc_debug(fmt, a)
# define dwc_debug2(dev, fmt, a, b)		dwc_debug(fmt, a, b)
# define dwc_debug3(dev, fmt, a, b, c)		dwc_debug(fmt, a, b, c)
# define dwc_debug4(dev, fmt, a, b, c, d)	dwc_debug(fmt, a, b, c, d)
# define dwc_debug5(dev, fmt, a, b, c, d, e) \
			dwc_debug(fmt, a, b, c, d, e)
# define dwc_debug6(dev, fmt, a, b, c, d, e, f) \
			dwc_debug(fmt, a, b, c, d, e, f)
# define dwc_debug7(dev, fmt, a, b, c, d, e, f, g) \
			dwc_debug(fmt, a, b, c, d, e, f, g)
# define dwc_debug8(dev, fmt, a, b, c, d, e, f, g, h) \
			dwc_debug(fmt, a, b, c, d, e, f, g, h)
# define dwc_debug9(dev, fmt, a, b, c, d, e, f, g, h, i) \
			dwc_debug(fmt, a, b, c, d, e, f, g, h, i)
# define dwc_debug10(dev, fmt, a, b, c, d, e, f, g, h, i, j) \
			dwc_debug(fmt, a, b, c, d, e, f, g, h, i, j)

//#if defined(DEBUG) || defined(ISOC_DEBUG)
# define dwc_isocdbg	printf
//#else
//# define dwc_isocdbg(dev, x)	do {} while (0)
//#endif

# define dwc_isocdbg0(dev, fmt)			dwc_isocdbg(fmt)
# define dwc_isocdbg1(dev, fmt, a)		dwc_isocdbg(fmt, a)
# define dwc_isocdbg2(dev, fmt, a, b)		dwc_isocdbg(fmt, a, b)
# define dwc_isocdbg3(dev, fmt, a, b, c)	dwc_isocdbg(fmt, a, b, c)
# define dwc_isocdbg4(dev, fmt, a, b, c, d) \
			dwc_isocdbg(fmt, a, b, c, d)
# define dwc_isocdbg5(dev, fmt, a, b, c, d, e) \
			dwc_isocdbg(fmt, a, b, c, d, e)
# define dwc_isocdbg6(dev, fmt, a, b, c, d, e, f) \
			dwc_isocdbg(fmt, a, b, c, d, e, f)

/**
 * Print an Error message.
 */
#define dwc_error	printf

#define dwc_error0(dev, fmt)			dwc_error(fmt)
#define dwc_error1(dev, fmt, a)			dwc_error(fmt, a)
#define dwc_error2(dev, fmt, a, b)		dwc_error(fmt, a, b)
#define dwc_error3(dev, fmt, a, b, c)		dwc_error(fmt, a, b, c)
#define dwc_error4(dev, fmt, a, b, c, d)	dwc_error(fmt, a, b, c, d)

/**
 * Print a Warning message.
 */
#define dwc_warn	printf

#define dwc_warn0(dev, fmt)			dwc_warn(fmt)
#define dwc_warn1(dev, fmt, a)			dwc_warn(fmt, a)
#define dwc_warn2(dev, fmt, a, b)		dwc_warn(fmt, a, b)
#define dwc_warn3(dev, fmt, a, b, c)		dwc_warn(fmt, a, b, c)
#define dwc_warn4(dev, fmt, a, b, c, d)		dwc_warn(fmt, a, b, c, d)

/**
 * Print an Informational message (normal but significant).
 */
#define dwc_info	printf

#define dwc_info0(dev, fmt)			dwc_info(fmt)
#define dwc_info1(dev, fmt, a)			dwc_info(fmt, a)
#define dwc_info2(dev, fmt, a, b)		dwc_info(fmt, a, b)
#define dwc_info3(dev, fmt, a, b, c)		dwc_info(fmt, a, b, c)
#define dwc_info4(dev, fmt, a, b, c, d)		dwc_info(fmt, a, b, c, d)

/**
 * Basic message printing.
 */
#define dwc_print	printf

#define dwc_print0(dev, fmt)			dwc_print(fmt)
#define dwc_print1(dev, fmt, a)			dwc_print(fmt, a)
#define dwc_print2(dev, fmt, a, b)		dwc_print(fmt, a, b)
#define dwc_print3(dev, fmt, a, b, c)		dwc_print(fmt, a, b, c)
#define dwc_print4(dev, fmt, a, b, c, d)	dwc_print(fmt, a, b, c, d)
#define dwc_print5(dev, fmt, a, b, c, d, e) \
			dwc_print(fmt, a, b, c, d, e)

extern int dwc_usb3_gadget_init(struct dwc_usb3_device *usb3_dev, struct device *dev);
extern void dwc_usb3_gadget_remove(struct dwc_usb3_device *usb3_dev, struct device *dev);

#ifdef __cplusplus
}
#endif

#endif /* _DWC_WIN_DEV_H_ */
