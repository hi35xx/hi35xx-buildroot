#ifndef _DWC_LINUX_DEV_H_
#define _DWC_LINUX_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 */

/** Wrapper function for _handshake(), shows the source line for any failure */
#define handshake(_dev_, _ptr_, _mask_, _done_) ({			\
	int _retval_ = _handshake(_dev_, _ptr_, _mask_, _done_);	\
	if (!_retval_)							\
		dwc_error2(_dev_, "handshake failed in %s():%d\n",	\
			   __func__, __LINE__);				\
	_retval_;							\
})

/** Takes a usb req pointer and returns the associated pcd req pointer */
#define dwc_usb3_get_pcd_req(usbreq) \
	container_of((usbreq), dwc_usb3_pcd_req_t, usb_req)

/** Takes a usb ep pointer and returns the associated pcd ep pointer */
#define dwc_usb3_get_pcd_ep(usbep) \
	container_of((usbep), dwc_usb3_pcd_ep_t, usb_ep)

/** @{ */
/**
 * Register read/write.
 */
static inline u32 dwc_rd32(struct dwc_usb3_device *dev,
			   volatile u32 __iomem *adr)
{
	return readl(adr);
}

static inline void dwc_wr32(struct dwc_usb3_device *dev,
			    volatile u32 __iomem *adr, u32 val)
{
	writel(val, adr);
}
/** @} */

/** @{ */
/**
 * Non-sleeping delays.
 */
#define dwc_udelay(dev, us)	udelay(us)
#define dwc_mdelay(dev, ms)	mdelay(ms)
/** @} */

/**
 * Sleeping delay.
 */
#define dwc_msleep(dev, ms)	msleep(ms)

/**
 * Debugging support - vanishes in non-debug builds.
 */

/** Prefix string for print macros. */
#define USB3_DWC	"dwc_usb3: "
#ifdef DEBUG
//# define dwc_debug(dev, x...)	printk(KERN_DEBUG USB3_DWC x )
# define dwc_debug(dev, x...)	printk(KERN_ERR USB3_DWC x )
#else
# define dwc_debug(dev, x...)	printk(KERN_ERR USB3_DWC x )
//# define dwc_debug(dev, x...)	do {} while (0)
#endif /* DEBUG */

# define dwc_debug0(dev, fmt)			dwc_debug(dev, fmt)
# define dwc_debug1(dev, fmt, a)		dwc_debug(dev, fmt, a)
# define dwc_debug2(dev, fmt, a, b)		dwc_debug(dev, fmt, a, b)
# define dwc_debug3(dev, fmt, a, b, c)		dwc_debug(dev, fmt, a, b, c)
# define dwc_debug4(dev, fmt, a, b, c, d)	dwc_debug(dev, fmt, a, b, c, d)
# define dwc_debug5(dev, fmt, a, b, c, d, e) \
			dwc_debug(dev, fmt, a, b, c, d, e)
# define dwc_debug6(dev, fmt, a, b, c, d, e, f) \
			dwc_debug(dev, fmt, a, b, c, d, e, f)
# define dwc_debug7(dev, fmt, a, b, c, d, e, f, g) \
			dwc_debug(dev, fmt, a, b, c, d, e, f, g)
# define dwc_debug8(dev, fmt, a, b, c, d, e, f, g, h) \
			dwc_debug(dev, fmt, a, b, c, d, e, f, g, h)
# define dwc_debug9(dev, fmt, a, b, c, d, e, f, g, h, i) \
			dwc_debug(dev, fmt, a, b, c, d, e, f, g, h, i)
# define dwc_debug10(dev, fmt, a, b, c, d, e, f, g, h, i, j) \
			dwc_debug(dev, fmt, a, b, c, d, e, f, g, h, i, j)

#if defined(DEBUG) || defined(ISOC_DEBUG)
# define dwc_isocdbg(dev, x...)	printk(USB3_DWC x )
#else
# define dwc_isocdbg(dev, x...)	do {} while (0)
#endif

# define dwc_isocdbg0(dev, fmt)			dwc_isocdbg(dev, fmt)
# define dwc_isocdbg1(dev, fmt, a)		dwc_isocdbg(dev, fmt, a)
# define dwc_isocdbg2(dev, fmt, a, b)		dwc_isocdbg(dev, fmt, a, b)
# define dwc_isocdbg3(dev, fmt, a, b, c)	dwc_isocdbg(dev, fmt, a, b, c)
# define dwc_isocdbg4(dev, fmt, a, b, c, d) \
			dwc_isocdbg(dev, fmt, a, b, c, d)
# define dwc_isocdbg5(dev, fmt, a, b, c, d, e) \
			dwc_isocdbg(dev, fmt, a, b, c, d, e)
# define dwc_isocdbg6(dev, fmt, a, b, c, d, e, f) \
			dwc_isocdbg(dev, fmt, a, b, c, d, e, f)

/**
 * Print an Error message.
 */
#define dwc_error(dev, x...)	printk(KERN_ERR USB3_DWC x )

#define dwc_error0(dev, fmt)			dwc_error(dev, fmt)
#define dwc_error1(dev, fmt, a)			dwc_error(dev, fmt, a)
#define dwc_error2(dev, fmt, a, b)		dwc_error(dev, fmt, a, b)
#define dwc_error3(dev, fmt, a, b, c)		dwc_error(dev, fmt, a, b, c)
#define dwc_error4(dev, fmt, a, b, c, d)	dwc_error(dev, fmt, a, b, c, d)

/**
 * Print a Warning message.
 */
#define dwc_warn(dev, x...)	printk(KERN_WARNING USB3_DWC x )

#define dwc_warn0(dev, fmt)			dwc_warn(dev, fmt)
#define dwc_warn1(dev, fmt, a)			dwc_warn(dev, fmt, a)
#define dwc_warn2(dev, fmt, a, b)		dwc_warn(dev, fmt, a, b)
#define dwc_warn3(dev, fmt, a, b, c)		dwc_warn(dev, fmt, a, b, c)
#define dwc_warn4(dev, fmt, a, b, c, d)		dwc_warn(dev, fmt, a, b, c, d)

/**
 * Print an Informational message (normal but significant).
 */
#define dwc_info(dev, x...)	printk(KERN_INFO USB3_DWC x )

#define dwc_info0(dev, fmt)			dwc_info(dev, fmt)
#define dwc_info1(dev, fmt, a)			dwc_info(dev, fmt, a)
#define dwc_info2(dev, fmt, a, b)		dwc_info(dev, fmt, a, b)
#define dwc_info3(dev, fmt, a, b, c)		dwc_info(dev, fmt, a, b, c)
#define dwc_info4(dev, fmt, a, b, c, d)		dwc_info(dev, fmt, a, b, c, d)

/**
 * Basic message printing.
 */
#define dwc_print(dev, x...)	0
//#define dwc_print(dev, x...)	printk(USB3_DWC x )

#define dwc_print0(dev, fmt)			dwc_print(dev, fmt)
#define dwc_print1(dev, fmt, a)			dwc_print(dev, fmt, a)
#define dwc_print2(dev, fmt, a, b)		dwc_print(dev, fmt, a, b)
#define dwc_print3(dev, fmt, a, b, c)		dwc_print(dev, fmt, a, b, c)
#define dwc_print4(dev, fmt, a, b, c, d)	dwc_print(dev, fmt, a, b, c, d)
#define dwc_print5(dev, fmt, a, b, c, d, e) \
			dwc_print(dev, fmt, a, b, c, d, e)

extern int dwc_usb3_gadget_init(struct dwc_usb3_device *usb3_dev, struct device *dev);
extern void dwc_usb3_gadget_remove(struct dwc_usb3_device *usb3_dev, struct device *dev);
extern int dwc_usb3_create_dev_files(struct device *dev);
extern void dwc_usb3_remove_dev_files(struct device *dev);
extern int dwc_usb3_wakeup(struct usb_gadget *gadget);
extern int dwc_wait_pme_thread(void *data);

#ifdef __cplusplus
}
#endif

#endif /* _DWC_LINUX_DEV_H_ */
