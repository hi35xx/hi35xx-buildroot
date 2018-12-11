/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

#ifdef DWC_UTE
# include "ute_if.h"
#endif

#ifdef CONFIG_USB_OTG_DWC
extern void dwc_otg3_set_peripheral(struct usb_otg *otg, int yes);
extern int otg_host_release(struct usb_otg *otg);
extern int otg_start_rsp(struct usb_otg *otg);
#endif

#define dwc_wait_if_hibernating(_pcd_, _flags_)				\
{									\
	int _temp_ = (_pcd_)->usb3_dev->hibernate;			\
	while (_temp_ >= DWC_HIBER_SLEEPING &&				\
	       _temp_ != DWC_HIBER_WAIT_U0 &&				\
	       _temp_ != DWC_HIBER_SS_DIS_QUIRK) {			\
		spin_unlock_irqrestore(&(_pcd_)->lock, (_flags_));	\
		msleep(1);						\
		spin_lock_irqsave(&(_pcd_)->lock, (_flags_));		\
		_temp_ = (_pcd_)->usb3_dev->hibernate;			\
	}								\
}

/**
 * NOTE: These strings MUST reflect the number and type of endpoints that the
 * core was configured with in CoreConsultant, and their intended usage. In
 * particular, the type (bulk/int/iso) determines how many TRBs will be
 * allocated for each endpoint. See gadget_init_eps() and trb_alloc() below.
 *
 * Naming convention is described in drivers/usb/gadget/epautoconf.c
 */
#if 0
static const char *g_ep_names[] = { "ep0", "ep1out", "ep1in", "ep2out",
	"ep2in", "ep3out", "ep3in", "ep4out", "ep4in", "ep5out", "ep5in",
	"ep6out", "ep6in", "ep7out", "ep7in", "ep8out", "ep8in", "ep9out",
	"ep9in", "ep10out", "ep10in", "ep11out", "ep11in", "ep12out", "ep12in",
	"ep13out", "ep13in", "ep14out", "ep14in", "ep15out", "ep15in" };
#endif
#if 0
static const char *g_ep_names[] = { "ep0", "ep1out", "ep1in", "ep2out",
	"ep3in", "ep2in", "ep4in", "ep4out", "ep5out", "ep5in",
	"ep6out", "ep6in", "ep7out", "ep7in", "ep8out", "ep8in", "ep9out",
	"ep9in", "ep10out", "ep10in", "ep11out", "ep11in", "ep12out", "ep12in",
	"ep13out", "ep13in", "ep14out", "ep14in", "ep15out", "ep15in" };
#endif
static const char *g_ep_names[] = { "ep0", "ep1out", "ep1in", "ep2out",
	"ep2in", "ep3in", "ep4in", "ep4out", "ep4in", "ep5out", "ep5in",
	"ep6out", "ep6in", "ep7out", "ep7in", "ep8out", "ep8in", "ep9out",
	"ep9in", "ep10out", "ep10in", "ep11out", "ep11in", "ep12out", "ep12in",
	"ep13out", "ep13in", "ep14out", "ep14in", "ep15out", "ep15in" };
/*========================================================================*/
/*
 * Linux Gadget API related functions
 */

/* Gadget wrapper */

static struct gadget_wrapper {
	dwc_usb3_pcd_t *pcd;

	char ep_names[DWC_MAX_EPS * 2 - 1][16];

	struct usb_gadget gadget;
	struct usb_gadget_driver *driver;

	dwc_usb3_pcd_req_t ep0_req;

	dwc_usb3_pcd_ep_t ep0;
	dwc_usb3_pcd_ep_t out_ep[DWC_MAX_EPS - 1];
	dwc_usb3_pcd_ep_t in_ep[DWC_MAX_EPS - 1];
} *gadget_wrapper;


/**
 * Passes a Setup request to the Gadget driver
 */
static int gadget_setup(dwc_usb3_pcd_t *pcd, void *data)
{
	int retval = -DWC_E_NOT_SUPPORTED;

	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	if (!gadget_wrapper) {
		dwc_warn(pcd->usb3_dev, "%s null wrapper!\n", __func__);
		return -DWC_E_INVALID;
	}

	if (gadget_wrapper->driver && gadget_wrapper->driver->setup) {
		spin_unlock(&pcd->lock);
		retval = gadget_wrapper->driver->setup(&gadget_wrapper->gadget,
					(struct usb_ctrlrequest *)data);
		spin_lock(&pcd->lock);
		if (retval == -ENOTSUPP)
			retval = -DWC_E_NOT_SUPPORTED;
		else if (retval < 0)
			retval = -DWC_E_INVALID;
	}

	return retval;
}

/*****************************************************************************
 * Gadget notification functions
 *
 * These functions receive notifications from the PCD when certain events
 * occur which the Gadget must be aware of.
 *
 * These functions *must* have the exact names and parameters shown here,
 * since they are part of the API between the PCD and the Gadget.
 *
 *****************************************************************************/

/**
 * This function receives Connect notifications from the PCD
 */
int dwc_usb3_gadget_connect(dwc_usb3_pcd_t *pcd, int speed)
{
	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	if (!gadget_wrapper) {
		dwc_warn(pcd->usb3_dev, "%s null wrapper!\n", __func__);
		return -DWC_E_INVALID;
	}

	gadget_wrapper->gadget.speed = speed;
	pcd->ep0->usb_ep.maxpacket = pcd->ep0->dwc_ep.maxpacket;

	return 0;
}

/**
 * This function receives Disconnect notifications from the PCD
 */
int dwc_usb3_gadget_disconnect(dwc_usb3_pcd_t *pcd)
{
	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	if (!gadget_wrapper) {
		dwc_warn(pcd->usb3_dev, "%s null wrapper!\n", __func__);
		return -DWC_E_INVALID;
	}

	if (gadget_wrapper->driver && gadget_wrapper->driver->disconnect) {
		spin_unlock(&pcd->lock);
		gadget_wrapper->driver->disconnect(&gadget_wrapper->gadget);
		spin_lock(&pcd->lock);
	}

	return 0;
}

/**
 * This function receives Setup request notifications from the PCD
 */
int dwc_usb3_gadget_setup(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	int ret;

	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	ret = gadget_setup(pcd, ctrl);

	/* @todo This is a g_file_storage gadget driver specific
	 * workaround: a DELAYED_STATUS result from the fsg_setup
	 * routine will result in the gadget queueing a EP0 IN status
	 * phase for a two-stage control transfer. Exactly the same as
	 * a SET_CONFIGURATION/SET_INTERFACE except that this is a class
	 * specific request. Need a generic way to know when the gadget
	 * driver will queue the status phase. Can we assume when we
	 * call the gadget driver setup() function that it will always
	 * queue and require the following flag? Need to look into
	 * this.
	 */
	if (ret >= 256 + 999)
		pcd->request_config = 1;

	return ret;
}

/**
 * This function receives Suspend notifications from the PCD
 */
int dwc_usb3_gadget_suspend(dwc_usb3_pcd_t *pcd)
{
	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	if (!gadget_wrapper) {
		dwc_warn(pcd->usb3_dev, "%s null wrapper!\n", __func__);
		return -DWC_E_INVALID;
	}

	if (gadget_wrapper->driver && gadget_wrapper->driver->suspend) {
		spin_unlock(&pcd->lock);
		gadget_wrapper->driver->suspend(&gadget_wrapper->gadget);
		spin_lock(&pcd->lock);
	}

	return 0;
}

/**
 * This function receives Resume notifications from the PCD
 */
int dwc_usb3_gadget_resume(dwc_usb3_pcd_t *pcd)
{
	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	if (!gadget_wrapper) {
		dwc_warn(pcd->usb3_dev, "%s null wrapper!\n", __func__);
		return -DWC_E_INVALID;
	}

	if (gadget_wrapper->driver && gadget_wrapper->driver->resume) {
		spin_unlock(&pcd->lock);
		gadget_wrapper->driver->resume(&gadget_wrapper->gadget);
		spin_lock(&pcd->lock);
	}

	return 0;
}

static void map_buffers(struct device *dev, struct usb_request *usb_req,
			dwc_usb3_pcd_ep_t *pcd_ep, int *req_flags)
{
	if (dwc_usb3_pcd_ep_is_in(pcd_ep)) {
# ifdef DWC_TEST_ISOC_CHAIN
		if (dwc_usb3_pcd_ep_type(pcd_ep) == UE_ISOCHRONOUS) {
			if (usb_req->length > 58 && usb_req->buf2[0]) {
				usb_req->dma2[0] = dma_map_single(dev,
					usb_req->buf2[0], 29, DMA_TO_DEVICE);
			}
			if (usb_req->length > 58) {
				usb_req->dma = dma_map_single(dev, usb_req->buf,
					usb_req->length - 58, DMA_TO_DEVICE);
			} else {
				usb_req->dma = dma_map_single(dev, usb_req->buf,
					usb_req->length, DMA_TO_DEVICE);
			}
			if (usb_req->length > 58 && usb_req->buf2[1]) {
				usb_req->dma2[1] = dma_map_single(dev,
					usb_req->buf2[1], 29, DMA_TO_DEVICE);
			}
		} else
# endif
			usb_req->dma = dma_map_single(dev, usb_req->buf,
					usb_req->length, DMA_TO_DEVICE);
		*req_flags |= DWC_PCD_REQ_MAP_DMA | DWC_PCD_REQ_IN;
	} else {
# ifdef DWC_TEST_ISOC_CHAIN
		if (dwc_usb3_pcd_ep_type(pcd_ep) == UE_ISOCHRONOUS) {
			if (usb_req->length > 58 && usb_req->buf2[0]) {
				usb_req->dma2[0] = dma_map_single(dev,
				      usb_req->buf2[0], 29, DMA_FROM_DEVICE);
			}
			if (usb_req->length > 58) {
				usb_req->dma = dma_map_single(dev, usb_req->buf,
				      usb_req->length - 58, DMA_FROM_DEVICE);
			} else {
				usb_req->dma = dma_map_single(dev, usb_req->buf,
				      usb_req->length, DMA_FROM_DEVICE);
			}
			if (usb_req->length > 58 && usb_req->buf2[1]) {
				usb_req->dma2[1] = dma_map_single(dev,
				      usb_req->buf2[1], 29, DMA_FROM_DEVICE);
			}
		} else
# endif
			usb_req->dma = dma_map_single(dev, usb_req->buf,
					usb_req->length, DMA_FROM_DEVICE);
		*req_flags |= DWC_PCD_REQ_MAP_DMA;
	}
}

static void unmap_buffers(struct device *dev, struct usb_request *usb_req,
			  dwc_usb3_pcd_ep_t *pcd_ep, int *req_flags)
{
	if (*req_flags & DWC_PCD_REQ_IN) {
# ifdef DWC_TEST_ISOC_CHAIN
		if (dwc_usb3_pcd_ep_type(pcd_ep) == UE_ISOCHRONOUS) {
			if (usb_req->dma2[1] != DWC_DMA_ADDR_INVALID) {
				dma_unmap_single(dev, usb_req->dma2[1], 29,
						 DMA_TO_DEVICE);
			}
			if (usb_req->dma2[1] != DWC_DMA_ADDR_INVALID ||
			    usb_req->dma2[0] != DWC_DMA_ADDR_INVALID) {
				dma_unmap_single(dev, usb_req->dma,
					usb_req->length - 58, DMA_TO_DEVICE);
			} else {
				dma_unmap_single(dev, usb_req->dma,
					usb_req->length, DMA_TO_DEVICE);
			}
			if (usb_req->dma2[0] != DWC_DMA_ADDR_INVALID) {
				dma_unmap_single(dev, usb_req->dma2[0], 29,
						 DMA_TO_DEVICE);
			}
		} else
# endif
			dma_unmap_single(dev, usb_req->dma, usb_req->length,
					 DMA_TO_DEVICE);
	} else {
# ifdef DWC_TEST_ISOC_CHAIN
		if (dwc_usb3_pcd_ep_type(pcd_ep) == UE_ISOCHRONOUS) {
			if (usb_req->dma2[1] != DWC_DMA_ADDR_INVALID) {
				dma_unmap_single(dev, usb_req->dma2[1], 29,
						 DMA_FROM_DEVICE);
			}
			if (usb_req->dma2[1] != DWC_DMA_ADDR_INVALID ||
			    usb_req->dma2[0] != DWC_DMA_ADDR_INVALID) {
				dma_unmap_single(dev, usb_req->dma,
				      usb_req->length - 58, DMA_FROM_DEVICE);
			} else {
				dma_unmap_single(dev, usb_req->dma,
				      usb_req->length, DMA_FROM_DEVICE);
			}
			if (usb_req->dma2[0] != DWC_DMA_ADDR_INVALID) {
				dma_unmap_single(dev, usb_req->dma2[0], 29,
						 DMA_FROM_DEVICE);
			}
		} else
# endif
			dma_unmap_single(dev, usb_req->dma, usb_req->length,
					 DMA_FROM_DEVICE);
	}

	*req_flags &= ~(DWC_PCD_REQ_MAP_DMA | DWC_PCD_REQ_IN);

# ifdef DWC_TEST_ISOC_CHAIN
	usb_req->dma2[0] = DWC_DMA_ADDR_INVALID;
	usb_req->dma2[1] = DWC_DMA_ADDR_INVALID;
# endif
	usb_req->dma = DWC_DMA_ADDR_INVALID;
}

#if defined(DEBUG) || defined(ISOC_DEBUG)
static void dbg_databuf(dwc_usb3_pcd_t *pcd, struct usb_request *usb_req,
			dwc_usb3_pcd_ep_t *pcd_ep, u32 actual,
			dma_addr_t dma
#ifdef DWC_TEST_ISOC_CHAIN
		       , dma_addr_t dma2[2]
#endif
		      )
{
	if (dwc_usb3_pcd_ep_type(pcd_ep) == UE_ISOCHRONOUS) {
		unsigned char *buf = (unsigned char *)usb_req->buf;
# ifdef DWC_TEST_ISOC_CHAIN
		unsigned char *buf0 = (unsigned char *)usb_req->buf2[0];
		unsigned char *buf1 = (unsigned char *)usb_req->buf2[1];
# endif
		if (buf) {
			if (actual >= 4) {
# ifdef DWC_TEST_ISOC_CHAIN
				dwc_isocdbg(pcd->usb3_dev,
					"%p %1lx %1lx %1lx bdata: %02x %02x %02x "
					"%02x .. %02x %02x %02x %02x .. %02x "
					"%02x %02x %02x\n",
					buf,
					(unsigned long)dma,
					(unsigned long)dma2[0],
					(unsigned long)dma2[1],
					buf[0], buf[1], buf[2], buf[3],
					buf0[0], buf0[1], buf0[2], buf0[3],
					buf1[0], buf1[1], buf1[2], buf1[3]);
# else
				dwc_isocdbg(pcd->usb3_dev,
					    "%p %1lx bdata: %02x %02x %02x %02x\n",
					    buf,
					    (unsigned long)dma,
					    buf[0], buf[1], buf[2], buf[3]);
# endif
			} else if (actual > 0) {
				dwc_isocdbg(pcd->usb3_dev,
					    "%p %1lx bdata: %02x ...\n",
					    buf,
					    (unsigned long)dma, buf[0]);
			} else {
				dwc_isocdbg(pcd->usb3_dev,
					    "%p %1lx bdata: 0-len!\n",
					    buf,
					    (unsigned long)dma);
			}
		} else {
			dwc_isocdbg(pcd->usb3_dev, "bdata: buf NULL!\n");
		}
	}
}
#endif

/**
 * This function receives Transfer Complete notifications from the PCD
 */
int dwc_usb3_gadget_complete(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep,
			     dwc_usb3_pcd_req_t *pcd_req, int status)
{
	struct usb_ep *usb_ep = &pcd_ep->usb_ep;
	struct usb_request *usb_req = &pcd_req->usb_req;
	u32 actual = pcd_req->dwc_req.actual;
	int *req_flags = &pcd_req->dwc_req.flags;
	dma_addr_t dma;
#ifdef DWC_TEST_ISOC_CHAIN
	dma_addr_t dma2[2];
#endif

	/* Remove the request from the queue */
	list_del_init(&pcd_req->entry);

	/* Save DMA address for debug */
	dma = usb_req->dma;
#ifdef DWC_TEST_ISOC_CHAIN
	dma2[0] = usb_req->dma2[0];
	dma2[1] = usb_req->dma2[1];
#endif

	/* Unmap DMA */
	if (*req_flags & DWC_PCD_REQ_MAP_DMA) {
		dwc_debug(pcd->usb3_dev, "DMA unmap req %p\n", usb_req);
		unmap_buffers(pcd->usb3_dev->dev, usb_req, pcd_ep, req_flags);
	}

#if defined(DEBUG) || defined(ISOC_DEBUG)
	dbg_databuf(pcd, usb_req, pcd_ep, actual, dma
# ifdef DWC_TEST_ISOC_CHAIN
		    , dma2
# endif
		   );
#endif

	if (usb_req->complete) {
		usb_req->status = status;
		usb_req->actual = actual;
		spin_unlock(&pcd->lock);
		usb_req->complete(usb_ep, usb_req);
		spin_lock(&pcd->lock);
	}

	if (pcd->request_pending > 0)
		--pcd->request_pending;

	return 0;
}

/**
 * This function allows overriding the standard Control transfer handling
 * (currently only done by the axs101 test gadget)
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_gadget_do_setup(dwc_usb3_pcd_t *pcd)
{
	/* Call the standard Control transfer handler */
	dwc_usb3_pcd_do_setup(pcd);
}

/*
 * Gadget EP ops
 */

/**
 * This function fills in an I/O Request for an EP.
 */
static void fill_req(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep,
		     dwc_usb3_pcd_req_t *req, int numbuf, char **buf,
		     dwc_dma_t *bufdma, u32 *buflen, u32 stream, int req_flags)
{
	int i;

	dwc_debug(pcd->usb3_dev, "req_flags=0x%1x\n", req_flags);

	req->dwc_req.length = 0;
	req->dwc_req.numbuf = numbuf;

	for (i = 0; i < numbuf; i++) {
		req->dwc_req.buf[i] = buf[i];
		req->dwc_req.bufdma[i] = bufdma[i];
		req->dwc_req.buflen[i] = buflen[i];
		req->dwc_req.length += buflen[i];
	}

	req->dwc_req.actual = 0;
	req->dwc_req.stream = stream;
	req->dwc_req.flags = req_flags;

	/* For EP0 IN without premature status, zlp is required? */
	if (ep == pcd->ep0 && ep->dwc_ep.is_in) {
		dwc_debug(pcd->usb3_dev, "%d-OUT ZLP\n", ep->dwc_ep.num);
		//req->dwc_req.flags |= DWC_PCD_REQ_ZERO;
	}
}

/**
 * This function enables an EP
 */
static int ep_enable(struct usb_ep *usb_ep,
		     const struct usb_endpoint_descriptor *ep_desc)
{
	dwc_usb3_pcd_ep_t *pcd_ep;
	dwc_usb3_pcd_t *pcd;
	dwc_usb3_dma_desc_t *desc;
	dma_addr_t desc_dma;
	int num, dir, type, num_trbs, maxpacket, link;
	int retval = 0;
	unsigned long flags;
	ss_endpoint_companion_descriptor_t ep_comp = { 0, };
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
	int maxstreams = 0, mult = 0, maxburst = 0;
#endif

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s(%p,%p)\n", __func__, usb_ep, ep_desc);
#endif
	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -EINVAL;
	}

	if (!usb_ep || !ep_desc ||
	    ep_desc->bDescriptorType != USB_DT_ENDPOINT) {
		printk(KERN_WARNING USB3_DWC
		       "%s, bad ep or descriptor %p %p %d!\n",
		       __func__, usb_ep, ep_desc,
		       ep_desc ? ep_desc->bDescriptorType : 0);
		return -EINVAL;
	}

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(USB3_DWC "usb_ep->name = %s\n", usb_ep->name);
#endif
	if (usb_ep == &gadget_wrapper->ep0.usb_ep) {
		printk(KERN_WARNING USB3_DWC "%s called for EP0!\n", __func__);
		return -EINVAL;
	}

	if (!ep_desc->wMaxPacketSize) {
		printk(KERN_WARNING USB3_DWC "%s, zero %s wMaxPacketSize!\n",
		       __func__, usb_ep->name);
		return -ERANGE;
	}

	if (!gadget_wrapper->driver ||
	    gadget_wrapper->gadget.speed == USB_SPEED_UNKNOWN) {
		printk(KERN_WARNING USB3_DWC "%s, bogus device state!\n",
		       __func__);
		return -ESHUTDOWN;
	}

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "usb_ep->maxpacket = %d\n",
	       le16_to_cpu(ep_desc->wMaxPacketSize));
#endif
	pcd = gadget_wrapper->pcd;
	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);

	if (pcd_ep->dwc_ep.phys >= DWC_MAX_PHYS_EP) {
		dwc_warn(pcd->usb3_dev, "%s, bad %s phys EP # %d!\n",
			 __func__, usb_ep->name, pcd_ep->dwc_ep.phys);
		return -ERANGE;
	}

	/* Free any existing TRB allocation for this EP */
	dwc_usb3_pcd_trb_free(pcd_ep);

	num = UE_GET_ADDR(ep_desc->bEndpointAddress);
	dir = UE_GET_DIR(ep_desc->bEndpointAddress);
	type = UE_GET_XFERTYPE(ep_desc->bmAttributes);
	link = 0;

	/* Allocate the number of TRBs based on EP type */
	switch (type) {
	case UE_INTERRUPT:
		num_trbs = DWC_NUM_INTR_TRBS;
		break;
	case UE_ISOCHRONOUS:
		num_trbs = DWC_NUM_ISOC_TRBS;
		link = 1;
		break;
	default:
		num_trbs = DWC_NUM_BULK_TRBS;
		break;
	}

	dwc_debug(pcd->usb3_dev, "ep%d-%s=%p phys=%d pcd_ep=%p\n",
		  num, dir == UE_DIR_IN ? "IN" : "OUT", usb_ep,
		  pcd_ep->dwc_ep.phys, pcd_ep);

	/* Set the TRB allocation for this EP */
	desc = dwc_usb3_pcd_trb_alloc(pcd_ep, num_trbs, type,
				      ep_desc->bInterval - 1, link,
				      &desc_dma);
	if (!desc)
		return -ENOMEM;

	maxpacket = le16_to_cpu(ep_desc->wMaxPacketSize);

	spin_lock_irqsave(&pcd->lock, flags);
	dwc_wait_if_hibernating(pcd, flags);
	pcd->usb3_dev->hiber_cnt++;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
	if (gadget_wrapper->gadget.speed == USB_SPEED_SUPER) {
		if (usb_ep->comp_desc) {
			if (type == UE_ISOCHRONOUS)
				mult = usb_ep->comp_desc->bmAttributes;
			maxburst = usb_ep->comp_desc->bMaxBurst;
			if (type == UE_BULK)
				maxstreams = usb_ep->comp_desc->bmAttributes;
		}
	} else {
		if (type == UE_ISOCHRONOUS)
			mult = maxpacket >> 11 & 3;
	}

	if (mult > 2 || maxburst > 15 || maxstreams > 16) {
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore(&pcd->lock, flags);
		dwc_usb3_pcd_trb_free(pcd_ep);
		return -EINVAL;
	}

	ep_comp.bmAttributes = USSE_SET_MAX_STREAMS(0, maxstreams);
	ep_comp.bmAttributes = USSE_SET_MAX_PACKET_NUM(ep_comp.bmAttributes,
						       mult);
	ep_comp.bMaxBurst = maxburst;
#else
	if (type == UE_BULK)
		ep_comp.bmAttributes = USSE_SET_MAX_STREAMS(0,
						usb_ep->numstreams);
	if (type == UE_ISOCHRONOUS)
		ep_comp.bmAttributes = USSE_SET_MAX_PACKET_NUM(0, usb_ep->mult);

	ep_comp.bMaxBurst = usb_ep->maxburst;
#endif

	retval = dwc_usb3_pcd_ep_enable(pcd, pcd_ep,
		(usb_endpoint_descriptor_t *)ep_desc, &ep_comp);
	if (!retval)
		usb_ep->maxpacket = maxpacket;

	if (gadget_wrapper->gadget.speed == USB_SPEED_HIGH) {
		if (usb_ep->maxpacket == 5120)
			usb_ep->maxpacket = 3072;
		else if (usb_ep->maxpacket == 3072)
			usb_ep->maxpacket = 2048;
	}

	pcd->usb3_dev->hiber_cnt--;
	spin_unlock_irqrestore(&pcd->lock, flags);

	if (retval) {
		dwc_usb3_pcd_trb_free(pcd_ep);
		retval = -ENOMEM;
	}

	return retval;
}

/**
 * This function disables an EP
 */
static int ep_disable(struct usb_ep *usb_ep)
{
	int retval = 0;
	dwc_usb3_pcd_ep_t *pcd_ep;
	dwc_usb3_pcd_t *pcd;
	unsigned long flags;

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s()\n", __func__);
#endif
	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -EINVAL;
	}

	if (!usb_ep) {
		printk(KERN_WARNING USB3_DWC "EP not enabled!\n");
		return -EINVAL;
	}

	pcd = gadget_wrapper->pcd;

	spin_lock_irqsave(&pcd->lock, flags);
	dwc_wait_if_hibernating(pcd, flags);
	pcd->usb3_dev->hiber_cnt++;

	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	retval = dwc_usb3_pcd_ep_disable(pcd, pcd_ep);

	pcd->usb3_dev->hiber_cnt--;
	spin_unlock_irqrestore(&pcd->lock, flags);

	/* Free any existing TRB allocation for this EP */
	dwc_usb3_pcd_trb_free(pcd_ep);

	return retval ? -EINVAL : 0;
}

/**
 * This function allocates a request object to use with the specified USB EP.
 *
 * @param usb_ep    The USB EP to be used with with the request.
 * @param gfp_flags The GFP_* flags to use.
 */
static struct usb_request *alloc_request(struct usb_ep *usb_ep, gfp_t gfp_flags)
{
	dwc_usb3_pcd_req_t *pcd_req;

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s(%p,%d)\n", __func__, usb_ep, gfp_flags);
#endif
	if (!usb_ep) {
		printk(USB3_DWC "%s() %s\n", __func__, "Invalid EP!\n");
		return NULL;
	}

	pcd_req = kmalloc(sizeof(dwc_usb3_pcd_req_t), gfp_flags);
	if (!pcd_req) {
		printk(USB3_DWC "%s() pcd request allocation failed!\n",
		       __func__);
		return NULL;
	}

#if defined(DEBUG) || defined(ISOC_DEBUG)
	//printk(USB3_DWC "%s() allocated %p\n", __func__, pcd_req);
#endif
	memset(pcd_req, 0, sizeof(dwc_usb3_pcd_req_t));
	pcd_req->usb_req.dma = DWC_DMA_ADDR_INVALID;

#ifdef DWC_TEST_ISOC_CHAIN
	pcd_req->usb_req.dma2[0] = DWC_DMA_ADDR_INVALID;
	pcd_req->usb_req.dma2[1] = DWC_DMA_ADDR_INVALID;
#endif
	return &pcd_req->usb_req;
}

/**
 * This function frees a request object.
 *
 * @param usb_ep  The USB EP associated with the request.
 * @param usb_req The request being freed.
 */
static void free_request(struct usb_ep *usb_ep, struct usb_request *usb_req)
{
	dwc_usb3_pcd_req_t *pcd_req;

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s(%p,%p)\n", __func__, usb_ep, usb_req);
#endif
	if (!usb_ep || !usb_req) {
		printk(KERN_WARNING USB3_DWC "%s() %s\n", __func__,
		       "Invalid ep or req argument!");
		return;
	}

	pcd_req = dwc_usb3_get_pcd_req(usb_req);

#if defined(DEBUG) || defined(ISOC_DEBUG)
	//printk(USB3_DWC "%s() freed %p\n", __func__, pcd_req);
#endif
	kfree(pcd_req);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
/**
 * This function allocates an I/O buffer to be used for a transfer
 * to/from the specified USB EP.
 *
 * @param usb_ep    The USB EP to be used with with the request.
 * @param bytes     The desired number of bytes for the buffer.
 * @param dma       Pointer to the buffer's DMA address; must be valid.
 * @param gfp_flags The GFP_* flags to use.
 * @return          Address of a new buffer or null if buffer could not be
 *                  allocated.
 */
static void *alloc_buffer(struct usb_ep *usb_ep, unsigned bytes,
			  dma_addr_t *dma, gfp_t gfp_flags)
{
	gfp_t gfp_flags = GFP_KERNEL | GFP_DMA32;
	void *buf;

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s(%p,%d,%p,%0x)\n", __func__,
	       usb_ep, bytes, dma, gfp_flags);
#endif
	/* Check dword alignment */
	if ((bytes & 0x3) != 0) {
		printk(KERN_WARNING USB3_DWC
		      "%s() Buffer size is not a multiple of DWORD size (%d)\n",
		      __func__, bytes);
	}

	buf = dma_alloc_coherent(NULL, bytes, dma, gfp_flags);
	if (buf) {
		/* Check dword alignment */
		if (((unsigned long)buf & 0x3) != 0) {
			printk(KERN_WARNING USB3_DWC
			       "%s() Buffer is not DWORD aligned (%p)\n",
			       __func__, buf);
		}
	}

	return buf;
}

/**
 * This function frees an I/O buffer that was allocated by alloc_buffer.
 *
 * @param usb_ep The USB EP associated with the buffer.
 * @param buf    Address of the buffer.
 * @param dma    The buffer's DMA address.
 * @param bytes  The number of bytes of the buffer.
 */
static void free_buffer(struct usb_ep *usb_ep, void *buf,
			dma_addr_t dma, unsigned bytes)
{
#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s(%p,%p,%0x,%d)\n", __func__,
	       usb_ep, buf, dma, bytes);
#endif
	dma_free_coherent(NULL, bytes, buf, dma);
}
#endif

/**
 * This function queues a request to a USB EP
 */
static int ep_queue(struct usb_ep *usb_ep, struct usb_request *usb_req,
		    gfp_t gfp_flags)
{
	int is_atomic = 0;
	int retval = 0;
	int req_flags = 0;
	dwc_usb3_pcd_ep_t *pcd_ep;
	dwc_usb3_pcd_req_t *pcd_req;
	int numbufs;
	char *bufs[3];
	dma_addr_t bufdmas[3];
	u32 buflens[3];
	dwc_usb3_pcd_t *pcd;
	unsigned long flags;

#ifdef DEBUG
	printk(KERN_DEBUG USB3_DWC "%s(%p,%p,%d)\n", __func__,
	       usb_ep, usb_req, gfp_flags);
#endif
	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -EINVAL;
	}

#ifdef DEBUG
	printk(KERN_DEBUG USB3_DWC "pcd=%p\n", gadget_wrapper->pcd);
#endif
	if (!usb_req || !usb_req->complete || !usb_req->buf) {
		printk(KERN_WARNING USB3_DWC "%s, bad params\n", __func__);
		return -EINVAL;
	}

	if (!usb_ep) {
		printk(KERN_WARNING USB3_DWC "%s, bad ep\n", __func__);
		return -EINVAL;
	}

	if (!gadget_wrapper->driver ||
	    gadget_wrapper->gadget.speed == USB_SPEED_UNKNOWN) {
		printk(KERN_DEBUG USB3_DWC "gadget.speed=%d\n",
		       gadget_wrapper->gadget.speed);
		printk(KERN_WARNING USB3_DWC "%s, bogus device state\n",
		       __func__);
		return -ESHUTDOWN;
	}

	if (!gadget_wrapper->pcd) {
		printk(KERN_WARNING USB3_DWC
		       "%s, gadget_wrapper->pcd is NULL!\n", __func__);
		return -EINVAL;
	}

	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	pcd_req = dwc_usb3_get_pcd_req(usb_req);
	pcd = dwc_usb3_pcd_ep_to_pcd(pcd_ep);

#ifdef DEBUG
	printk(KERN_DEBUG USB3_DWC "%s 0x%p queue req 0x%p, len %d buf 0x%p\n",
	       usb_ep->name, pcd_ep, usb_req, usb_req->length, usb_req->buf);
#endif
	usb_req->status = -EINPROGRESS;
	usb_req->actual = 0;

	if (gfp_flags == GFP_ATOMIC)
		is_atomic = 1;

	if (usb_req->zero)
		req_flags |= DWC_PCD_REQ_ZERO;

	if (usb_req->length != 0 && usb_req->dma == DWC_DMA_ADDR_INVALID) {
		dwc_debug(pcd->usb3_dev, "DMA map req %p\n", usb_req);
		dwc_debug(pcd->usb3_dev, "dev=%p\n", pcd->usb3_dev->dev);
		map_buffers(pcd->usb3_dev->dev, usb_req, pcd_ep, &req_flags);
	}

	bufs[0] = usb_req->buf;
	bufdmas[0] = usb_req->dma;
	buflens[0] = usb_req->length;
	numbufs = 1;

#ifdef DWC_TEST_ISOC_CHAIN
	if (dwc_usb3_pcd_ep_type(pcd_ep) == UE_ISOCHRONOUS) {
		buflens[0] = usb_req->length - 58;

		bufs[1] = usb_req->buf2[0];
		bufdmas[1] = usb_req->dma2[0];
		buflens[1] = 29;

		bufs[2] = usb_req->buf2[1];
		bufdmas[2] = usb_req->dma2[1];
		buflens[2] = 29;

		numbufs = 3;
	}
#endif
	if (dwc_usb3_pcd_ep_num(pcd_ep) != 0 && !pcd_ep->dwc_ep.usb_ep_desc) {
		dwc_debug(pcd->usb3_dev, "%s, bad ep!\n", __func__);
		return -EINVAL;
	}

	spin_lock_irqsave(&pcd->lock, flags);
	dwc_wait_if_hibernating(pcd, flags);
	pcd->usb3_dev->hiber_cnt++;

	dwc_debug(pcd->usb3_dev, "%s: EP%d-%s %p stream %d req %p\n",
		  __func__, dwc_usb3_pcd_ep_num(pcd_ep),
		  dwc_usb3_pcd_ep_is_in(pcd_ep) ? "IN" : "OUT",
		  pcd_ep, usb_req->stream_id, pcd_req);

	dwc_debug(pcd->usb3_dev, "%s(%p,%p,%p,%p,%lx)\n", __func__, pcd,
		  usb_ep, usb_req, bufs[0], (unsigned long)bufdmas[0]);

	INIT_LIST_HEAD(&pcd_req->entry);

	fill_req(pcd, pcd_ep, pcd_req, numbufs, bufs, bufdmas, buflens,
		 usb_req->stream_id, req_flags);

	if (!list_empty(&pcd_ep->dwc_ep.queue)) {
		/* If queue is not empty, then don't start the transfer, unless
		 * this is an Isoc transfer. But we still queue the request.
		 */
		dwc_debug(pcd->usb3_dev,
			  " q not empty %d, stopped %d, avail %d, istart %d\n",
			  pcd->request_pending, pcd_ep->dwc_ep.stopped,
			  pcd_ep->dwc_ep.desc_avail,
			  pcd_ep->dwc_ep.xfer_started);

		if (!pcd_ep->dwc_ep.stopped &&
		    pcd_ep->dwc_ep.type == UE_ISOCHRONOUS &&
		    pcd_ep->dwc_ep.desc_avail > 0 &&
		    pcd_ep->dwc_ep.xfer_started)
			goto do_start;

	} else if (pcd_ep->dwc_ep.stopped ||
		   (pcd_ep != pcd->ep0 && pcd_ep->dwc_ep.desc_avail <= 0) ||
		   (pcd_ep->dwc_ep.type == UE_ISOCHRONOUS &&
						!pcd_ep->dwc_ep.xfer_started)) {
		/* If EP is stopped, or there is no TRB available, or this
		 * is an Isoc transfer and the EP is not started, then don't
		 * start the transfer. But we still queue the request.
		 */
		dwc_debug(pcd->usb3_dev,
			  " q empty %d, stopped %d, avail %d, istart %d\n",
			  pcd->request_pending, pcd_ep->dwc_ep.stopped,
			  pcd_ep->dwc_ep.desc_avail, pcd_ep->dwc_ep.xfer_started);
	} else {
do_start:
		/* Setup and start the transfer */
		dwc_usb3_pcd_fill_trbs(pcd, pcd_ep, pcd_req);
		retval = dwc_usb3_pcd_ep_submit_req(pcd, pcd_ep, pcd_req,
						    req_flags);
	}

	if (!retval) {
		list_add_tail(&pcd_req->entry, &pcd_ep->dwc_ep.queue);
		++pcd->request_pending;
	}

	pcd->usb3_dev->hiber_cnt--;
	spin_unlock_irqrestore(&pcd->lock, flags);

	/* dwc_usb3_pcd_ep_submit_req() can return positive values for
	 * Control transfers, don't propagate those from this function
	 */
	if (retval < 0)
		return -EINVAL;
	return 0;
}

/**
 * This function dequeues a request from a USB EP
 */
static int ep_dequeue(struct usb_ep *usb_ep, struct usb_request *usb_req)
{
	dwc_usb3_pcd_req_t *pcd_req = NULL;
	dwc_usb3_pcd_ep_t *pcd_ep;
	dwc_usb3_pcd_t *pcd;
	struct list_head *list_item;
	unsigned long flags;

#ifdef DEBUG
	printk(KERN_DEBUG USB3_DWC "%s(%p,%p)\n", __func__, usb_ep, usb_req);
#endif
	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -EINVAL;
	}

	if (!usb_ep || !usb_req) {
		printk(KERN_WARNING USB3_DWC "%s, bad argument!\n", __func__);
		return -EINVAL;
	}

	if (!gadget_wrapper->driver ||
	    gadget_wrapper->gadget.speed == USB_SPEED_UNKNOWN) {
		printk(KERN_WARNING USB3_DWC "%s, bogus device state!\n",
		       __func__);
		return -ESHUTDOWN;
	}

	if (!gadget_wrapper->pcd) {
		printk(KERN_WARNING USB3_DWC
		       "%s, gadget_wrapper->pcd is NULL!\n", __func__);
		return -EINVAL;
	}

	pcd = gadget_wrapper->pcd;

	spin_lock_irqsave(&pcd->lock, flags);
	dwc_wait_if_hibernating(pcd, flags);
	pcd->usb3_dev->hiber_cnt++;

	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);

	if (dwc_usb3_pcd_ep_num(pcd_ep) != 0 && !pcd_ep->dwc_ep.usb_ep_desc) {
		//dwc_warn(pcd->usb3_dev, "%s, bad pcd_ep!\n", __func__);
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore(&pcd->lock, flags);
		return -EINVAL;
	}

	/* make sure it's actually queued on this EP */
	list_for_each (list_item, &pcd_ep->dwc_ep.queue) {
		pcd_req = list_entry(list_item, dwc_usb3_pcd_req_t, entry);
		if (&pcd_req->usb_req == usb_req)
			break;
	}

	if (!pcd_req) {
		dwc_warn(pcd->usb3_dev, "%s, no request in queue!\n", __func__);
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore(&pcd->lock, flags);
		return 0;
	}

	dwc_usb3_pcd_ep_cancel_req(pcd, pcd_ep, pcd_req, usb_req->stream_id);

	pcd->usb3_dev->hiber_cnt--;
	spin_unlock_irqrestore(&pcd->lock, flags);
	return 0;
}

/**
 * This routine allocates coherent DMA memory. It is used by the PCD to
 * allocate memory for TRBs. The block of memory returned must have a start
 * address aligned to a 16-byte boundary.
 *
 * @param ep            PCD EP that memory block will be associated with.
 * @param size          Size of memory block to allocate, in bytes.
 * @param mem_dma_ret   DMA address of allocated memory block is returned
 *                      through this pointer.
 * @return              Address of allocated memory block, or NULL if allocation
 *                      fails.
 */
void *dwc_usb3_gadget_alloc_dma(dwc_usb3_pcd_ep_t *ep, int size,
				dwc_dma_t *mem_dma_ret)
{
	void *mem;

	mem = dma_alloc_coherent(NULL, size, mem_dma_ret,
				 GFP_ATOMIC | GFP_DMA32);
	if (!mem)
		return NULL;
	memset(mem, 0, size);
	return mem;
}

/**
 * This routine frees DMA memory allocated by dwc_usb3_gadget_alloc_dma().
 *
 * @param ep            PCD EP that memory block is associated with.
 * @param size          Size of memory block to free, in bytes.
 * @param mem           Address of memory block.
 * @param mem_dma       DMA address of memory block.
 */
void dwc_usb3_gadget_free_dma(dwc_usb3_pcd_ep_t *ep, int size, void *mem,
			      dwc_dma_t mem_dma)
{
	dma_free_coherent(NULL, size, mem, mem_dma);
}

/**
 * This routine returns the PCD request corresponding to the current transfer
 * request for an endpoint. The current transfer request is the first request
 * submitted that has not been completed yet.
 */
dwc_usb3_pcd_req_t *dwc_usb3_gadget_get_request(dwc_usb3_pcd_t *pcd,
						dwc_usb3_pcd_ep_t *ep)
{
	dwc_usb3_pcd_req_t *req;

	/* Check for a pending request */
	if (list_empty(&ep->dwc_ep.queue)) {
		dwc_debug(pcd->usb3_dev, "%s(%p), ep->dwc_ep.queue empty!\n",
			  __func__, ep);
		return NULL;
	}

	req = list_first_entry(&ep->dwc_ep.queue, dwc_usb3_pcd_req_t, entry);
	return req;
}

/**
 * This function checks the EP request queue, if the queue is not empty then
 * the next transfer is started.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param ep    EP to operate on.
 */
void dwc_usb3_gadget_start_next_request(dwc_usb3_pcd_t *pcd,
					dwc_usb3_pcd_ep_t *ep)
{
	dwc_usb3_pcd_req_t *req = NULL;
	struct list_head *list_item;

	dwc_debug(pcd->usb3_dev, "%s(%p)\n", __func__, ep);

	if (list_empty(&ep->dwc_ep.queue)) {
		dwc_debug(pcd->usb3_dev, "start_next EP%d-%s: queue empty\n",
			  ep->dwc_ep.num, ep->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	list_for_each (list_item, &ep->dwc_ep.queue) {
		req = list_entry(list_item, dwc_usb3_pcd_req_t, entry);
		if (!(req->dwc_req.flags & DWC_PCD_REQ_STARTED)) {
			if (ep->dwc_ep.desc_avail <= 0) {
				dwc_debug(pcd->usb3_dev,
					  "start_next EP%d-%s: no TRB avail\n",
					  ep->dwc_ep.num, ep->dwc_ep.is_in ?
					  "IN" : "OUT");
				return;
			}

			dwc_debug(pcd->usb3_dev, "start_next EP%d-%s: OK\n",
				  ep->dwc_ep.num,
				  ep->dwc_ep.is_in ? "IN" : "OUT");

			/* Setup and start the transfer */
			//dwc_debug(pcd->usb3_dev,
			//	  " -> starting xfer (start_next_req) %s %s\n",
			//	  ep->dwc_ep.ep.name,
			//	  ep->dwc_ep.is_in ? "IN" : "OUT");
			dwc_usb3_pcd_fill_trbs(pcd, ep, req);
			dwc_usb3_pcd_ep_start_transfer(pcd, ep, req, 0);
			return;
		}
	}

	dwc_debug(pcd->usb3_dev, "start_next EP%d-%s: all req active\n",
		  ep->dwc_ep.num, ep->dwc_ep.is_in ? "IN" : "OUT");
}

/**
 * This function terminates all the requests in the EP request queue.
 */
void dwc_usb3_gadget_request_nuke(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep)
{
	dwc_usb3_pcd_req_t *req;

	dwc_debug(pcd->usb3_dev, "%s(%p)\n", __func__, ep);
	ep->dwc_ep.stopped = 1;

	/* called with irqs blocked?? */
	while (!list_empty(&ep->dwc_ep.queue)) {
		req = list_first_entry(&ep->dwc_ep.queue, dwc_usb3_pcd_req_t,
				       entry);
		dwc_usb3_pcd_request_done(pcd, ep, req, -DWC_E_SHUTDOWN);
	}

	if (ep != pcd->ep0) {
		ep->dwc_ep.desc_idx = 0;
		ep->dwc_ep.hiber_desc_idx = 0;
	}
}

/**
 * This function marks all requests in an EP queue as not started.
 */
void dwc_usb3_gadget_set_ep_not_started(dwc_usb3_pcd_t *pcd,
					dwc_usb3_pcd_ep_t *ep)
{
	dwc_usb3_pcd_req_t *req;
	struct list_head *list_item;

	list_for_each (list_item, &ep->dwc_ep.queue) {
		req = list_entry(list_item, dwc_usb3_pcd_req_t, entry);
		if (req->dwc_req.flags & DWC_PCD_REQ_STARTED)
			req->dwc_req.flags &= ~DWC_PCD_REQ_STARTED;
	}
}

/**
 * Start an Isoc EP running at the proper interval, after receiving the initial
 * XferNrdy event.
 */
void dwc_usb3_gadget_isoc_ep_start(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep,
				   u32 event)
{
	dwc_usb3_pcd_req_t *req = NULL;
	struct list_head *list_item;

	dwc_debug(pcd->usb3_dev, "%s(%p,%p,%x)\n", __func__, pcd, ep, event);
	dwc_isocdbg(pcd->usb3_dev, "%s(%08x)\n", __func__, event);

	if (list_empty(&ep->dwc_ep.queue)) {
		dwc_debug(pcd->usb3_dev, "%s(%p), ep->dwc_ep.queue empty!\n",
			  __func__, ep);
		return;
	}

	if (ep->dwc_ep.desc_avail <= 0) {
		dwc_print(pcd->usb3_dev, "EP%d-%s: no TRB avail!\n",
			  ep->dwc_ep.num, ep->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	/* If need to restart after hibernation, handle that and return */
	if (dwc_usb3_pcd_isoc_ep_hiber_restart(pcd, ep))
		return;

	/*
	 * Start the next queued transfer at the target uFrame
	 */

	list_for_each (list_item, &ep->dwc_ep.queue) {
		req = list_entry(list_item, dwc_usb3_pcd_req_t, entry);
		if (req->dwc_req.flags & DWC_PCD_REQ_STARTED)
			req = NULL;
		else
			break;
	}

	dwc_debug(pcd->usb3_dev, "req=%p\n", req);
	if (!req) {
		dwc_print(pcd->usb3_dev, "EP%d-%s: no requests to start!\n",
			  ep->dwc_ep.num, ep->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	dwc_usb3_pcd_fill_trbs(pcd, ep, req);
	dwc_usb3_pcd_ep_start_transfer(pcd, ep, req, event);

	/*
	 * Now start any remaining queued transfers
	 */

	while (!list_is_last(list_item, &ep->dwc_ep.queue)) {
		list_item = list_item->next;
		req = list_entry(list_item, dwc_usb3_pcd_req_t, entry);
		if (!(req->dwc_req.flags & DWC_PCD_REQ_STARTED)) {
			if (ep->dwc_ep.desc_avail <= 0) {
				dwc_debug(pcd->usb3_dev,
					  "start_next EP%d-%s: no TRB avail!\n",
					  ep->dwc_ep.num, ep->dwc_ep.is_in ?
					  "IN" : "OUT");
				return;
			}

			dwc_usb3_pcd_fill_trbs(pcd, ep, req);
			dwc_usb3_pcd_ep_start_transfer(pcd, ep, req, 0);
		}
	}
}

/**
 * This function sets/clears halt on a USB EP
 */
static int ep_set_halt(struct usb_ep *usb_ep, int value)
{
	dwc_usb3_pcd_ep_t *pcd_ep;
	dwc_usb3_pcd_t *pcd;
	unsigned long flags;

	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -EINVAL;
	}

	if (!usb_ep) {
		printk(KERN_WARNING USB3_DWC "%s, bad usb_ep!\n", __func__);
		return -EINVAL;
	}

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "HALT %s %d\n", usb_ep->name, value);
#endif

	pcd = gadget_wrapper->pcd;

	spin_lock_irqsave(&pcd->lock, flags);
	dwc_wait_if_hibernating(pcd, flags);
	pcd->usb3_dev->hiber_cnt++;

	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	dwc_debug(pcd->usb3_dev, "pcd_ep=%p\n", pcd_ep);
	dwc_debug(pcd->usb3_dev, "pcd->ep0=%p\n", pcd->ep0);
	dwc_debug(pcd->usb3_dev, "epnum=%d is_in=%d\n",
		  dwc_usb3_pcd_ep_num(pcd_ep), dwc_usb3_pcd_ep_is_in(pcd_ep));

	if ((!pcd_ep->dwc_ep.usb_ep_desc && dwc_usb3_pcd_ep_num(pcd_ep) != 0) ||
	    (pcd_ep->dwc_ep.usb_ep_desc && dwc_usb3_pcd_ep_type(pcd_ep)
							== UE_ISOCHRONOUS)) {
		dwc_warn(pcd->usb3_dev, "%s, bad pcd_ep!\n", __func__);
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore(&pcd->lock, flags);
		return -EINVAL;
	}

	if (!list_empty(&pcd_ep->dwc_ep.queue)) {
		dwc_warn(pcd->usb3_dev, "%s, Xfer in process!\n", __func__);
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore(&pcd->lock, flags);
		return -EAGAIN;
	}

	dwc_usb3_pcd_ep_set_halt(pcd, pcd_ep, value);

	pcd->usb3_dev->hiber_cnt--;
	spin_unlock_irqrestore(&pcd->lock, flags);
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
static int ep_set_wedge(struct usb_ep *usb_ep)
{
#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "WEDGE %s\n", usb_ep->name);
#endif
	return ep_set_halt(usb_ep, 3);
}
#endif

static struct usb_ep_ops pcd_ep_ops = {
	.enable		= ep_enable,
	.disable	= ep_disable,

	.alloc_request	= alloc_request,
	.free_request	= free_request,

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
	.alloc_buffer	= alloc_buffer,
	.free_buffer	= free_buffer,
#endif
	.queue		= ep_queue,
	.dequeue	= ep_dequeue,

	.set_halt	= ep_set_halt,

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	.set_wedge	= ep_set_wedge,
#endif
	.fifo_status	= NULL,
	.fifo_flush	= NULL,
};


/* Gadget ops */

#ifdef CONFIG_USB_OTG_DWC

void dwc_usb3_start_hnp(dwc_usb3_pcd_t *pcd)
{
	struct usb_phy *phy = usb_get_phy(USB_PHY_TYPE_USB3);
	struct usb_otg *otg;

	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);
	if (IS_ERR(phy) || !phy) {
		dwc_info(pcd->usb3_dev, "NO PHY!!\n");
		if (!IS_ERR(phy))
			usb_put_phy(phy);
		return;
	}

	otg = phy->otg;
	if (!otg) {
		dwc_info(pcd->usb3_dev, "NO OTG!!\n");
		usb_put_phy(phy);
		return;
	}

	otg_start_hnp(otg);
	usb_put_phy(phy);
}

void dwc_usb3_host_release(dwc_usb3_pcd_t *pcd)
{
	struct usb_phy *phy = usb_get_phy(USB_PHY_TYPE_USB3);
	struct usb_otg *otg;

	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);
	if (IS_ERR(phy) || !phy) {
		dwc_info(pcd->usb3_dev, "NO PHY!!\n");
		if (!IS_ERR(phy))
			usb_put_phy(phy);
		return;
	}

	otg = phy->otg;
	if (!otg) {
		dwc_info(pcd->usb3_dev, "NO OTG!!\n");
		usb_put_phy(phy);
		return;
	}

	otg_host_release(otg);
	usb_put_phy(phy);
}

static int pcd_stop_peripheral(struct usb_gadget *gadget,
			       struct usb_gadget_driver *driver)
{
	struct gadget_wrapper *d;
	dwc_usb3_pcd_t *pcd;
	unsigned long flags;
	u32 temp;
	int count = 0;

	d = container_of(gadget, struct gadget_wrapper, gadget);
	pcd = d->pcd;

	spin_lock_irqsave(&pcd->lock, flags);

	dwc_usb3_pcd_stop(pcd);

	/* Disable device interrupts */
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->devten, 0);

	/* Clear Run/Stop bit */
	temp = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	temp &= ~DWC_DCTL_RUN_STOP_BIT;
	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, temp);

	spin_unlock_irqrestore(&pcd->lock, flags);

	/* Wait for core stopped */
	do {
		msleep(1);
		temp = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dsts);
	} while (!(temp & DWC_DSTS_DEV_CTRL_HLT_BIT) && (++count < 30));

	return 0;
}

static int pcd_start_peripheral(struct usb_gadget *gadget,
				struct usb_gadget_driver *driver)
{
	struct gadget_wrapper *d;
	dwc_usb3_pcd_t *pcd;
	unsigned long flags;
	u32 temp;

	d = container_of(gadget, struct gadget_wrapper, gadget);
	pcd = d->pcd;

	spin_lock_irqsave(&pcd->lock, flags);

	dwc_usb3_pcd_device_init(pcd->usb3_dev, 0, 0);

	/* Enable Device mode interrupts */
	dwc_usb3_enable_device_interrupts(pcd->usb3_dev);

	/* Set Run/Stop bit, and Keep-Connect bit if hibernation enabled */
	temp = dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dctl);
	temp |= DWC_DCTL_RUN_STOP_BIT;

	if (pcd->usb3_dev->core_params->hibernate &&
	    (pcd->usb3_dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT)
		temp |= DWC_DCTL_KEEP_CONNECT_BIT;

	dwc_wr32(pcd->usb3_dev, &pcd->dev_global_regs->dctl, temp);
	pcd->wants_host = 0;

	spin_unlock_irqrestore(&pcd->lock, flags);

	return 0;
}

static int pcd_send_hrr(struct usb_gadget *gadget, int is_init)
{
	struct gadget_wrapper *d;
	dwc_usb3_pcd_t *pcd;
	unsigned long flags;
	u32 param;

	d = container_of(gadget, struct gadget_wrapper, gadget);
	pcd = d->pcd;

	spin_lock_irqsave(&pcd->lock, flags);

	param = is_init ? DWC_DGCMDPAR_HOST_ROLE_REQ_INITIATE :
		DWC_DGCMDPAR_HOST_ROLE_REQ_CONFIRM;
	dwc_usb3_xmit_host_role_request(pcd, param);

	spin_unlock_irqrestore(&pcd->lock, flags);

	return 0;
}

#endif /* CONFIG_USB_OTG_DWC */

/**
 * This function returns the current USB frame number.
 *
 * @param gadget The gadget context.
 */
static int dwc_get_frame(struct usb_gadget *gadget)
{
	struct gadget_wrapper	*d;
	dwc_usb3_pcd_t		*pcd;
	dwc_usb3_device_t	*dev;
	unsigned long		flags;
	int			ret;

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s()\n", __func__);
#endif
	if (!gadget)
		return -ENODEV;

	d = container_of(gadget, struct gadget_wrapper, gadget);
	pcd = d->pcd;
	dev = pcd->usb3_dev;

	spin_lock_irqsave(&pcd->lock, flags);
	dwc_wait_if_hibernating(pcd, flags);
	pcd->usb3_dev->hiber_cnt++;

	ret = dwc_usb3_pcd_get_frame_number(pcd);

	pcd->usb3_dev->hiber_cnt--;
	spin_unlock_irqrestore(&pcd->lock, flags);
	return ret;
}

/**
 * This function sends a remote wakeup to the host.
 *
 * @param gadget The gadget context.
 */
int dwc_usb3_wakeup(struct usb_gadget *gadget)
{
	struct gadget_wrapper	*d;
	dwc_usb3_pcd_t		*pcd;
	dwc_usb3_device_t	*dev;
	unsigned long		timeout, flags;
	int			state;

#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s()\n", __func__);
#endif
	if (!gadget)
		return -ENODEV;

	d = container_of(gadget, struct gadget_wrapper, gadget);
	pcd = d->pcd;
	dev = pcd->usb3_dev;

	if (!pcd->remote_wakeup_enable) {
		dwc_info(dev, "hardware not enabled for wakeup\n");
		return -ENOTSUPP;
	}

	spin_lock_irqsave(&pcd->lock, flags);

	if (dev->hibernate != DWC_HIBER_AWAKE) {
		state = dwc_exit_hibernation(pcd, 1);
		spin_unlock_irqrestore(&pcd->lock, flags);
		printk(USB3_DWC "dwc_exit_hibernation() returned %d\n", state);
		return 0;
	}

	dwc_wait_if_hibernating(pcd, flags);
	pcd->usb3_dev->hiber_cnt++;

	state = dwc_usb3_pcd_get_link_state(pcd);
	pcd->link_state = state;
	dwc_usb3_pcd_set_link_state(pcd, DWC_LINK_STATE_REQ_REMOTE_WAKEUP);
	spin_unlock_irqrestore(&pcd->lock, flags);

	dwc_info(dev, "link state before: %d\n", state);
#if 0
	/* Clear DCTL bits after 2 ms */
	msleep(2);

	spin_lock_irqsave(&pcd->lock, flags);
	dwc_usb3_pcd_set_link_state(pcd, 0);
	spin_unlock_irqrestore(&pcd->lock, flags);
#endif
	/* Wait 500 ms for link state to go to U0 */
	timeout = jiffies + msecs_to_jiffies(500);

	while (!time_after(jiffies, timeout)) {
		spin_lock_irqsave(&pcd->lock, flags);
		state = dwc_usb3_pcd_get_link_state(pcd);
		pcd->link_state = state;
		spin_unlock_irqrestore(&pcd->lock, flags);
		if (state == DWC_LINK_STATE_U0)
			break;
		else
			msleep(1);
	}

	dwc_info(dev, "link state after: %d\n", state);

	if (state != DWC_LINK_STATE_U0) {
		spin_lock_irqsave(&pcd->lock, flags);
		pcd->usb3_dev->hiber_cnt--;
		spin_unlock_irqrestore(&pcd->lock, flags);
		return -ETIMEDOUT;
	}

	/* Send function remote wake notification */
	spin_lock_irqsave(&pcd->lock, flags);
	dwc_usb3_pcd_remote_wake(pcd, 0);
	pcd->wkup_rdy = 0;
	pcd->usb3_dev->hiber_cnt--;
	spin_unlock_irqrestore(&pcd->lock, flags);
	dwc_info(dev, "remote wake sent\n");

	return 0;
}

static int dwc_usb3_pullup(struct usb_gadget *gadget, int is_on)
{
	return 0;
}

static const struct usb_gadget_ops pcd_ops = {
	.get_frame	= dwc_get_frame,
	.wakeup		= dwc_usb3_wakeup,
	.pullup		= dwc_usb3_pullup,
	// current versions must always be self-powered

#ifdef CONFIG_USB_OTG_DWC
	.udc_start	= pcd_start_peripheral,
	.udc_stop	= pcd_stop_peripheral,

	.send_hrr	= pcd_send_hrr,
#endif
};


/*=======================================================================*/

/**
 * Initialize the Linux gadget specific parts of the default Control EP (EP0).
 */
static void gadget_init_ep0(dwc_usb3_pcd_t *pcd, struct gadget_wrapper *d)
{
	dwc_usb3_pcd_ep_t *pcd_ep;
	struct usb_ep *ep0;

	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	d->gadget.ep0 = &d->ep0.usb_ep;
	INIT_LIST_HEAD(&d->gadget.ep0->ep_list);

	pcd_ep = &d->ep0;
	ep0 = &pcd_ep->usb_ep;
	dwc_debug(pcd->usb3_dev, "ep0=%p\n", ep0);

	INIT_LIST_HEAD(&pcd_ep->dwc_ep.queue);

	/* Init the usb_ep structure */
	ep0->name = d->ep_names[0];
	ep0->ops = (struct usb_ep_ops *)&pcd_ep_ops;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0)
	ep0->maxpacket = DWC_MAX_EP0_SIZE;
#else
	usb_ep_set_maxpacket_limit(ep0, DWC_MAX_EP0_SIZE);
#endif

	dwc_debug(pcd->usb3_dev, "EP0 name=%s\n", ep0->name);
	dwc_debug(pcd->usb3_dev, "ep0 eplist: %p(%p,%p) = %p(%p,%p)\n",
		  &d->gadget.ep0->ep_list, d->gadget.ep0->ep_list.prev,
		  d->gadget.ep0->ep_list.next, &ep0->ep_list,
		  ep0->ep_list.prev, ep0->ep_list.next);
}

/**
 * Initialize the Linux gadget specific parts of the non-EP0 EPs.
 */
static int gadget_init_eps(dwc_usb3_pcd_t *pcd, struct gadget_wrapper *d)
{
	dwc_usb3_pcd_ep_t *pcd_ep;
	struct usb_ep *ep;
	const char *name, *tmp, *ttmp;
	int i, num, ep_in, ep_out;

	dwc_debug(pcd->usb3_dev, "%s()\n", __func__);

	INIT_LIST_HEAD(&d->gadget.ep_list);

	for (i = 1, ep_in = 0, ep_out = 0; i < ARRAY_SIZE(d->ep_names); i++) {
		name = d->ep_names[i];
		if (!name || !name[0])
			break;

		/* Find '-' in e.g. "ep1in-bulk" */
		tmp = strrchr(name, '-');
		if (!tmp)
			/* If no '-' then find end of string */
			tmp = name + strlen(name);

		/* If not 0-len string then back up 1 char */
		if (tmp != name)
			tmp--;

		/* Get the EP number */
		num = 0;
		ttmp = tmp;

		if (*tmp == 't') {
			/* If "out", back up to the number before the 'o' */
			if (tmp >= name + 3)
				tmp -= 3;
		} else if (*tmp == 'n') {
			/* If "in", back up to the number before the 'i' */
			if (tmp >= name + 2)
				tmp -= 2;
		}

		if (isdigit(*tmp)) {
			/* If numeric, handle 1 or 2 digits */
			if (tmp > name && isdigit(*(tmp - 1)))
				num = simple_strtol(tmp - 1, NULL, 10);
			else
				num = simple_strtol(tmp, NULL, 10);
		}

		dwc_debug(pcd->usb3_dev, "num=%d\n", num);
		tmp = ttmp;
		if (num < 1 || num >= DWC_MAX_EPS)
			goto bad;

		/* If e.g. "ep1" or "ep1out" then OUT ep */
		if ((isdigit(*tmp) || *tmp == 't') && (ep_out < 2)) {
			ep_out++;
			pcd_ep = &d->out_ep[ep_out - 1];
			ep = &pcd_ep->usb_ep;
			dwc_debug(pcd->usb3_dev,
				  "ep%d-OUT=%p name=%s phys=%d pcd_ep=%p\n",
				  num, ep, name, ep_out, pcd_ep);
			dwc_error(pcd->usb3_dev,
				  "ep%d-OUT=%p name=%s phys=%d pcd_ep=%p\n",
				  num, ep, name, ep_out, pcd_ep);

			pcd_ep->dwc_ep.num = num;
			INIT_LIST_HEAD(&pcd_ep->dwc_ep.queue);

			/* Init the usb_ep structure */
			ep->name = name;
			ep->ops = (struct usb_ep_ops *)&pcd_ep_ops;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0)
			ep->maxpacket = DWC_MAX_PACKET_SIZE;
#else
			usb_ep_set_maxpacket_limit(ep, DWC_MAX_PACKET_SIZE);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
			ep->max_streams = 15;
#else
			ep->numstreams = 0;
#endif
			ep->mult = 0;
			ep->maxburst = 0;

			dwc_debug(pcd->usb3_dev,
				"ep%d-OUT eplist pre: %p(%p,%p) = %p(%p,%p)\n",
				num, &d->gadget.ep_list, d->gadget.ep_list.prev,
				d->gadget.ep_list.next, &ep->ep_list,
				ep->ep_list.prev, ep->ep_list.next);
			list_add_tail(&ep->ep_list, &d->gadget.ep_list);
			dwc_debug(pcd->usb3_dev,
				"ep%d-OUT eplist post: %p(%p,%p) = %p(%p,%p)\n",
				num, &d->gadget.ep_list, d->gadget.ep_list.prev,
				d->gadget.ep_list.next, &ep->ep_list,
				ep->ep_list.prev, ep->ep_list.next);
		}

		/* If e.g. "ep1" or "ep1in" then IN ep */
		if ((isdigit(*tmp) || *tmp == 'n')&&(ep_in < 4)) {
			ep_in++;
			pcd_ep = &d->in_ep[ep_in - 1];
			ep = &pcd_ep->usb_ep;
			dwc_debug(pcd->usb3_dev,
				"ep%d-IN=%p name=%s phys=%d pcd_ep=%p\n",
				num, ep, name, ep_in, pcd_ep);
			dwc_error(pcd->usb3_dev,
				"ep%d-IN=%p name=%s phys=%d pcd_ep=%p\n",
				num, ep, name, ep_in, pcd_ep);

			pcd_ep->dwc_ep.num = num;
			INIT_LIST_HEAD(&pcd_ep->dwc_ep.queue);
			/* Init the usb_ep structure */
			ep->name = name;
			ep->ops = (struct usb_ep_ops *)&pcd_ep_ops;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0)
			ep->maxpacket = DWC_MAX_PACKET_SIZE;
#else
			usb_ep_set_maxpacket_limit(ep, DWC_MAX_PACKET_SIZE);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
			ep->max_streams = 15;
#else
			ep->numstreams = 0;
#endif
			ep->mult = 0;
			ep->maxburst = 0;

			dwc_debug(pcd->usb3_dev,
				"ep%d-IN eplist pre: %p(%p,%p) = %p(%p,%p)\n",
				num, &d->gadget.ep_list, d->gadget.ep_list.prev,
				d->gadget.ep_list.next, &ep->ep_list,
				ep->ep_list.prev, ep->ep_list.next);
			list_add_tail(&ep->ep_list, &d->gadget.ep_list);
			dwc_debug(pcd->usb3_dev,
				"ep%d-IN eplist post: %p(%p,%p) = %p(%p,%p)\n",
				num, &d->gadget.ep_list, d->gadget.ep_list.prev,
				d->gadget.ep_list.next, &ep->ep_list,
				ep->ep_list.prev, ep->ep_list.next);
		}

		if (!isdigit(*tmp) && *tmp != 't' && *tmp != 'n') {
bad:
			dwc_debug(pcd->usb3_dev, "ep%d ????\n", num);
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * Free any descriptor allocations made for each non-EP0 EP.
 */
static void gadget_free_ep_allocations(dwc_usb3_pcd_t *pcd,
				       struct gadget_wrapper *d)
{
	dwc_usb3_pcd_ep_t *pcd_ep;
	int i;

	for (i = DWC_MAX_EPS - 1; i > 0; i--) {
		pcd_ep = &d->in_ep[i - 1];
		dwc_debug(pcd->usb3_dev, "physpair%d-IN=%p\n", i, pcd_ep);
		dwc_usb3_pcd_trb_free(pcd_ep);

		pcd_ep = &d->out_ep[i - 1];
		dwc_debug(pcd->usb3_dev, "physpair%d-OUT=%p\n", i, pcd_ep);
		dwc_usb3_pcd_trb_free(pcd_ep);
	}
}

/**
 * This function releases the Gadget device.
 * Required by device_unregister().
 *
 * @param dev The device context.
 */
static void gadget_release(struct device *dev)
{
	/* @todo Should this do something? Should it free the PCD?
	 */
#if defined(DEBUG) || defined(ISOC_DEBUG)
	printk(KERN_DEBUG USB3_DWC "%s()\n", __func__);
#endif
}


#ifdef DWC_UTE
/*=======================================================================*/
/*
 * UTE support functions
 */

/**
 * Return the PCD pointer for a given gadget instance.
 */
dwc_usb3_pcd_t *dwc_usb3_get_pcd_instance(unsigned devnum)
{
	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return NULL;
	}

	return gadget_wrapper->pcd;
}

/**
 * Set mapping of a USB EP to a physical EP, by changing the contents of the
 * gadget instance's ep_names[] array.
 */
int dwc_usb3_set_usb_ep_map(unsigned devnum, unsigned phys_ep_num,
			    unsigned usb_ep_num)
{
	unsigned usb_ep, usb_dir, usb_type;
	char *dirstr, *typestr, *buf;
	static char *ep_type_str[] = { "ctrl", "iso", "bulk", "int" };

	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -ENODEV;
	}

	/* Phys EP 0 & 1 cannot be remapped */
	if (phys_ep_num < 2 || phys_ep_num >= DWC_MAX_PHYS_EP)
		return -EINVAL;

	/* ep_names[] has a single entry for both EP0 IN & OUT */
	phys_ep_num--;

	/* These fields are defined by UTE */
	usb_ep = usb_ep_num & 0xf;
	usb_dir = usb_ep_num >> 4 & 0x3;
	usb_type = usb_ep_num >> 6 & 0x7;

	/* Cannot remap the default Control EP */
	if (usb_ep == 0)
		return -EINVAL;

	switch (usb_dir) {
	case 0:
		dirstr = "out";
		break;
	case 1:
		dirstr = "in";
		break;
	default:
		/* USB3 does not have bidirectional physical EPs */
		return -EINVAL;
	}

	buf = gadget_wrapper->ep_names[phys_ep_num];

	if (usb_type >= 4) {
		snprintf(buf, sizeof(gadget_wrapper->ep_names[0]),
			 "ep%u%s", usb_ep, dirstr);
	} else {
		typestr = ep_type_str[usb_type];
		snprintf(buf, sizeof(gadget_wrapper->ep_names[0]),
			 "ep%u%s-%s", usb_ep, dirstr, typestr);
	}

	return 0;
}

/**
 * Get mapping of a USB EP to a physical EP, by reading the contents of the
 * gadget instance's ep_names[] array.
 */
int dwc_usb3_get_usb_ep_map(unsigned devnum, unsigned phys_ep_num,
			    unsigned *usb_ep_num_ret)
{
	unsigned usb_ep, usb_dir, usb_type;
	char dirstr[16], typestr[16], *dashp, *buf;
	int ret;

	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -ENODEV;
	}

	/* Phys EP 0 & 1 cannot be remapped */
	if (phys_ep_num < 2 || phys_ep_num >= DWC_MAX_PHYS_EP)
		return -EINVAL;

	/* ep_names[] has a single entry for both EP0 IN & OUT */
	phys_ep_num--;

	buf = gadget_wrapper->ep_names[phys_ep_num];
	printk("phys EP %d, config=\"%s\"\n", phys_ep_num, buf);
	usb_ep=16;
	dirstr[0] = 0;
	typestr[0] = 0;
	dashp = strchr(buf, '-');
	if (dashp) {
		*dashp = ' ';
		ret = sscanf(buf, "ep%u%15s %15s", &usb_ep, dirstr, typestr);
		*dashp = '-';
		printk("sscanf() ret=%d\n", ret);
		if (ret != 3)
			return -EINVAL;
	} else {
		ret = sscanf(buf, "ep%u%15s", &usb_ep, dirstr);
		printk("sscanf() ret=%d\n", ret);
		if (ret != 2)
			return -EINVAL;
	}

	if (usb_ep > 15)
		return -ERANGE;

	if (strcmp(dirstr, "out") == 0)
		usb_dir = 0;
	else if (strcmp(dirstr, "in") == 0)
		usb_dir = 1;
	else
		return -ERANGE;

	if (strcmp(typestr, "ctrl") == 0)
		usb_type = 0;
	else if (strcmp(typestr, "iso") == 0)
		usb_type = 1;
	else if (strcmp(typestr, "bulk") == 0)
		usb_type = 2;
	else if (strcmp(typestr, "int") == 0)
		usb_type = 3;
	else
		usb_type = 4;

	*usb_ep_num_ret = usb_ep | usb_dir << 4 | usb_type << 6;
	return 0;
}

/**
 * Activate the changes made by the UTE to the core's features.
 */
void dwc_usb3_ute_config(dwc_usb3_device_t *usb3_dev)
{
	dwc_usb3_pcd_t *pcd = &usb3_dev->pcd;
	int i, cnt, txsz[DWC_MAX_TX_FIFOS];

	dwc_debug(usb3_dev, "%s()\n", __func__);

	if (!gadget_wrapper) {
		dwc_warn(usb3_dev, "%s null wrapper!\n", __func__);
		return;
	}

	if (pcd->ute_change) {
		/* Free any remaining descriptor allocations made for
		 * non-EP0 EPs
		 */
		gadget_free_ep_allocations(pcd, gadget_wrapper);

		/* Set the Tx FIFO sizes */
		for (i = 0, cnt = 0; i < pcd->num_in_eps + 1; i++) {
			txsz[i] = pcd->txf_size[i];
			if (txsz[i])
				cnt++;
		}
		if (cnt)
			dwc_usb3_set_tx_fifo_size(usb3_dev, txsz);

		/* Set the Rx FIFO size */
		if (pcd->rxf_size)
			dwc_usb3_set_rx_fifo_size(usb3_dev, pcd->rxf_size);

		/* Re-initialize non-EP0 EPs to pick up any mapping changes */
		if (gadget_init_eps(pcd, gadget_wrapper)) {
			dwc_error(usb3_dev, "%s, gadget_init_eps error!\n",
				  __func__);
		}

		pcd->ute_change = 0;
	}
}

/**
 * Reset usb endpoint mapping to it's default state.
 */
int dwc_usb3_reset_usb_ep_map(unsigned devnum)
{
	struct gadget_wrapper *d;
	dwc_usb3_pcd_t *pcd;
	int i;

	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -ENODEV;
	}

	d = gadget_wrapper;
	pcd = d->pcd;

	for (i = 0; i < ARRAY_SIZE(g_ep_names) &&
	     i < ARRAY_SIZE(d->ep_names) &&
	     i < pcd->num_in_eps + pcd->num_out_eps + 1; i++) {
		strncpy(d->ep_names[i], g_ep_names[i],
			sizeof(d->ep_names[0]) - 1);
		d->ep_names[i][sizeof(d->ep_names[0]) - 1] = 0;
		dwc_debug(pcd->usb3_dev, "~phys EP%d name=%s\n", i, d->ep_names[i]);
	}

	return gadget_init_eps(pcd, d);
}

int dwc_usb3_switch_speed(unsigned devnum, int speed)
{
	int ret = 0;
	struct gadget_wrapper *d;
	dwc_usb3_pcd_t *pcd;
	dwc_usb3_device_t *usb3_dev;
	//u32 temp;

	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -ENODEV;
	}

	d = gadget_wrapper;
	pcd = d->pcd;
	usb3_dev = pcd->usb3_dev;
	/*
	temp = dwc_rd32(usb3_dev, &pcd->dev_global_regs->dctl);
	temp &= ~DWC_DCTL_RUN_STOP_BIT;
	dwc_wr32(usb3_dev, &pcd->dev_global_regs->dctl, temp);

	dwc_udelay(usb3_dev, 1500); */
	/* Soft-reset the core */
	/* temp = dwc_rd32(usb3_dev, &pcd->dev_global_regs->dctl);
	temp &= ~DWC_DCTL_RUN_STOP_BIT;
	temp |= DWC_DCTL_CSFT_RST_BIT;
	dwc_wr32(usb3_dev, &pcd->dev_global_regs->dctl, temp); */

	/* Wait for core to come out of reset */
	/* do {
		dwc_udelay(usb3_dev, 1);
		temp = dwc_rd32(usb3_dev, &pcd->dev_global_regs->dctl);
	} while (temp & DWC_DCTL_CSFT_RST_BIT);

	dwc_mdelay(usb3_dev, 2); */

	if (speed == 3) {
		/* Set device speed feature to Super Speed */
		speed = 0;
	}
	usb3_dev->core_params->usb2mode = speed;

	dwc_usb3_pcd_device_init(usb3_dev, 1, 0);

	return ret;
}

int dwc_usb3_get_dev_speed(unsigned devnum)
{
	struct gadget_wrapper *d;
	dwc_usb3_pcd_t *pcd;
	int ret;

	if (!gadget_wrapper) {
		printk(KERN_WARNING USB3_DWC "%s null wrapper!\n", __func__);
		return -ENODEV;
	}

	d = gadget_wrapper;
	pcd = d->pcd;

	ret = pcd->usb3_dev->core_params->usb2mode;
	if (ret == 0) {
		/* If speed feature is set to Super Speed */
		ret = 3;
	}
	return ret;
}

#endif /* DWC_UTE */

/**
 * This function initializes the Gadget portion of the driver.
 *
 * @param usb3_dev Programming view of DWC_usb3 controller.
 * @param dev      The device context.
 */
int dwc_usb3_gadget_init(dwc_usb3_device_t *usb3_dev, struct device *dev)
{
	dwc_usb3_pcd_t *pcd = &usb3_dev->pcd;
	int retval = -ENOMEM;
	int hiberbufs = 0;
	dma_addr_t dma_addr = 0;
	int i;

	dwc_debug(usb3_dev, "%s()\n", __func__);
	dwc_debug(usb3_dev, "pcd=%p\n", pcd);

	gadget_wrapper = kmalloc(sizeof(struct gadget_wrapper), GFP_KERNEL);
	if (!gadget_wrapper)
		goto out1;

	memset(gadget_wrapper, 0, sizeof(*gadget_wrapper));
	gadget_wrapper->pcd = pcd;

	for (i = 0; i < ARRAY_SIZE(g_ep_names) &&
	     i < ARRAY_SIZE(gadget_wrapper->ep_names) &&
	     i < pcd->num_in_eps + pcd->num_out_eps + 1; i++) {
		strncpy(gadget_wrapper->ep_names[i], g_ep_names[i],
			sizeof(gadget_wrapper->ep_names[0]) - 1);
		gadget_wrapper->ep_names[i][sizeof(gadget_wrapper->ep_names[0]) - 1] = 0;
		dwc_debug(usb3_dev, "~phys EP%d name=%s\n", i,
			  gadget_wrapper->ep_names[i]);
	}

	gadget_wrapper->gadget.name = "dwc_usb3_pcd";
	dwc_debug(usb3_dev, "gadget.name=%s\n", gadget_wrapper->gadget.name);

#ifdef CONFIG_USB_OTG_DWC
	gadget_wrapper->gadget.is_otg = 1;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
	strcpy(gadget_wrapper->gadget.dev.bus_id, "gadget");
#else
	dev_set_name(&gadget_wrapper->gadget.dev, "%s", "gadget");
#endif
	gadget_wrapper->gadget.dev.parent = dev;
	gadget_wrapper->gadget.dev.release = gadget_release;
	gadget_wrapper->gadget.speed = USB_SPEED_UNKNOWN;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0)
	gadget_wrapper->gadget.is_dualspeed = 1;
#else
	gadget_wrapper->gadget.max_speed = USB_SPEED_SUPER;
#endif
	gadget_wrapper->gadget.ops = &pcd_ops;
	gadget_wrapper->driver = NULL;
	dwc_debug(usb3_dev, "gadget=%p ops=%p\n", &gadget_wrapper->gadget,
		  gadget_wrapper->gadget.ops);

	/* Set the PCD's EP0 request pointer to the wrapper's request */
	pcd->ep0_req = &gadget_wrapper->ep0_req;

	/* Set the PCD's EP array pointers to the wrapper's EPs */
	pcd->ep0 = &gadget_wrapper->ep0;
	for (i = 0; i < DWC_MAX_EPS - 1; i++) {
		pcd->out_ep[i] = &gadget_wrapper->out_ep[i];
		pcd->in_ep[i] = &gadget_wrapper->in_ep[i];
	}

	/* Allocate the EP0 packet buffers */
	pcd->ep0_setup_pkt = dma_alloc_coherent(NULL,
			sizeof(*pcd->ep0_setup_pkt) * 5,
			&pcd->ep0_setup_pkt_dma, GFP_KERNEL | GFP_DMA32);
	if (!pcd->ep0_setup_pkt)
		goto out2;

	pcd->ep0_status_buf = dma_alloc_coherent(NULL, DWC_STATUS_BUF_SIZE,
			 &pcd->ep0_status_buf_dma, GFP_KERNEL | GFP_DMA32);
	if (!pcd->ep0_status_buf)
		goto out3;

	/* Allocate the EP0 DMA descriptors */
	pcd->ep0_setup_desc = dma_alloc_coherent(NULL,
			 sizeof(dwc_usb3_dma_desc_t), &pcd->ep0_setup_desc_dma,
			 GFP_KERNEL | GFP_DMA32);
	if (!pcd->ep0_setup_desc)
		goto out4;

	pcd->ep0_in_desc = dma_alloc_coherent(NULL, sizeof(dwc_usb3_dma_desc_t),
			&pcd->ep0_in_desc_dma, GFP_KERNEL | GFP_DMA32);
	if (!pcd->ep0_in_desc)
		goto out5;

	pcd->ep0_out_desc = dma_alloc_coherent(NULL,
			sizeof(dwc_usb3_dma_desc_t), &pcd->ep0_out_desc_dma,
			GFP_KERNEL | GFP_DMA32);
	if (!pcd->ep0_out_desc)
		goto out6;

	/* If hibernation is supported */
	if (usb3_dev->core_params->hibernate &&
	    (usb3_dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT) {
		hiberbufs = usb3_dev->hwparams4 >> DWC_HWP4_HIBER_SPAD_SHIFT &
			DWC_HWP4_HIBER_SPAD_BITS >> DWC_HWP4_HIBER_SPAD_SHIFT;
		if (hiberbufs) {
			/* Allocate scratch buffer pointer array */
			pcd->hiber_scratchpad_array =
				dma_alloc_coherent(NULL,
					sizeof(*pcd->hiber_scratchpad_array),
					&pcd->hiber_scratchpad_array_dma,
					GFP_KERNEL | GFP_DMA32);
			if (!pcd->hiber_scratchpad_array) {
				dwc_debug(usb3_dev,
				      "%s hibernation array allocation error\n",
				      __func__);
				goto out7;
			}
		}

		/* Allocate scratch buffers */
		for (i = 0; i < hiberbufs; i++) {
			pcd->hiber_scratchpad[i] =
				dma_alloc_coherent(NULL, 4096, &dma_addr,
						   GFP_KERNEL | GFP_DMA32);
			if (!pcd->hiber_scratchpad[i]) {
				dwc_debug(usb3_dev,
					"%s hibernation buf allocation error\n",
					__func__);
				while (i-- > 0) {
					dma_addr = (dma_addr_t)pcd->
						hiber_scratchpad_array->dma_addr[i];
					dma_free_coherent(NULL, 4096,
						pcd->hiber_scratchpad[i],
						dma_addr);
					pcd->hiber_scratchpad[i] = NULL;
				}

				goto out8;
			}

			pcd->hiber_scratchpad_array->dma_addr[i] = (u64)dma_addr;
		}
	}

	/* Initialize all the EP structures */
	gadget_init_ep0(pcd, gadget_wrapper);
	retval = gadget_init_eps(pcd, gadget_wrapper);
	if (retval) {
		dwc_debug(usb3_dev, "%s gadget_init_eps error\n", __func__);
		goto out9;
	}

	/* Register the gadget device */
	retval = device_register(&gadget_wrapper->gadget.dev);
	if (retval) {
		dwc_debug(usb3_dev, "%s cannot register gadget device\n",
			  __func__);
		goto out10;
	}

	return 0;

out10:
	gadget_free_ep_allocations(pcd, gadget_wrapper);
out9:
	for (i = hiberbufs - 1; i >= 0; i--) {
		if (pcd->hiber_scratchpad[i]) {
			dma_addr = (dma_addr_t)
				pcd->hiber_scratchpad_array->dma_addr[i];
			dma_free_coherent(NULL, 4096, pcd->hiber_scratchpad[i],
					  dma_addr);
			pcd->hiber_scratchpad[i] = NULL;
		}
	}
out8:
	if (hiberbufs)
		dma_free_coherent(NULL, sizeof(*pcd->hiber_scratchpad_array),
				  pcd->hiber_scratchpad_array,
				  pcd->hiber_scratchpad_array_dma);
out7:
	dma_free_coherent(NULL, sizeof(dwc_usb3_dma_desc_t), pcd->ep0_out_desc,
			  pcd->ep0_out_desc_dma);
out6:
	dma_free_coherent(NULL, sizeof(dwc_usb3_dma_desc_t), pcd->ep0_in_desc,
			  pcd->ep0_in_desc_dma);
out5:
	dma_free_coherent(NULL, sizeof(dwc_usb3_dma_desc_t),
			  pcd->ep0_setup_desc, pcd->ep0_setup_desc_dma);
out4:
	dma_free_coherent(NULL, DWC_STATUS_BUF_SIZE, pcd->ep0_status_buf,
			  pcd->ep0_status_buf_dma);
out3:
	dma_free_coherent(NULL, sizeof(*pcd->ep0_setup_pkt) * 5,
			  pcd->ep0_setup_pkt, pcd->ep0_setup_pkt_dma);
out2:
	kfree(gadget_wrapper);
	gadget_wrapper = NULL;
out1:
	return retval;
}

/**
 * Cleanup the Gadget.
 *
 * @param usb3_dev Programming view of DWC_usb3 controller.
 * @param dev      The device context.
 */
void dwc_usb3_gadget_remove(dwc_usb3_device_t *usb3_dev, struct device *dev)
{
	dwc_usb3_pcd_t *pcd = &usb3_dev->pcd;
	int hiberbufs, i;
	void *addr;
	dma_addr_t dma_addr;

	dwc_debug(usb3_dev, "%s()\n", __func__);
	dwc_debug(usb3_dev, "pcd=%p\n", pcd);

	if (!gadget_wrapper) {
		dwc_warn(usb3_dev, "%s null wrapper!\n", __func__);
		return;
	}

	/* start with the driver above us */
	if (gadget_wrapper->driver) {
		/* should have been done already by driver model core */
		dwc_warn(usb3_dev, "driver '%s' is still registered!\n",
			 gadget_wrapper->driver->driver.name);
		usb_gadget_unregister_driver(gadget_wrapper->driver);
	}

	device_unregister(&gadget_wrapper->gadget.dev);
	gadget_free_ep_allocations(pcd, gadget_wrapper);

	/* If hibernation is supported */
	if (usb3_dev->core_params->hibernate &&
	    (usb3_dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT) {
		hiberbufs = usb3_dev->hwparams4 >> DWC_HWP4_HIBER_SPAD_SHIFT &
			DWC_HWP4_HIBER_SPAD_BITS >> DWC_HWP4_HIBER_SPAD_SHIFT;

		/* Free hibernation scratch buffers */
		for (i = hiberbufs - 1; i >= 0; i--) {
			addr = pcd->hiber_scratchpad[i];
			dma_addr = (dma_addr_t)pcd->hiber_scratchpad_array->dma_addr[i];
			pcd->hiber_scratchpad[i] = NULL;
			if (addr)
				dma_free_coherent(NULL, 4096, addr, dma_addr);
		}

		if (hiberbufs)
			dma_free_coherent(NULL,
					  sizeof(*pcd->hiber_scratchpad_array),
					  pcd->hiber_scratchpad_array,
					  pcd->hiber_scratchpad_array_dma);
	}

	dma_free_coherent(NULL, sizeof(dwc_usb3_dma_desc_t), pcd->ep0_out_desc,
			  pcd->ep0_out_desc_dma);
	dma_free_coherent(NULL, sizeof(dwc_usb3_dma_desc_t), pcd->ep0_in_desc,
			  pcd->ep0_in_desc_dma);
	dma_free_coherent(NULL, sizeof(dwc_usb3_dma_desc_t), pcd->ep0_setup_desc,
			  pcd->ep0_setup_desc_dma);
	dma_free_coherent(NULL, DWC_STATUS_BUF_SIZE, pcd->ep0_status_buf,
			  pcd->ep0_status_buf_dma);
	dma_free_coherent(NULL, sizeof(*pcd->ep0_setup_pkt) * 5,
			  pcd->ep0_setup_pkt, pcd->ep0_setup_pkt_dma);

	kfree(gadget_wrapper);
	gadget_wrapper = NULL;
}

/**
 * This function registers a gadget driver with the PCD.
 *
 * When a driver is successfully registered, it will receive control
 * requests including set_configuration(), which enables non-control
 * requests. Then usb traffic follows until a disconnect is reported.
 * Then a host may connect again, or the driver might get unbound.
 *
 * @param driver The driver being registered.
 * @param bind   The gadget driver's bind function.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
int usb_gadget_register_driver(struct usb_gadget_driver *driver)
#else
int usb_gadget_probe_driver(struct usb_gadget_driver *driver
# if LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)
			    , int (*bind)(struct usb_gadget *)
# endif
			   )
#endif
{
	dwc_usb3_pcd_t *pcd;
	int retval = -ENODEV;
#ifdef CONFIG_USB_OTG_DWC
	struct usb_phy *phy;
	struct usb_otg *otg;
#endif

	printk(KERN_DEBUG USB3_DWC "%s(%p)\n", __func__, driver);
	printk(KERN_DEBUG USB3_DWC "gdt_wrp=%p\n", gadget_wrapper);

	if (!gadget_wrapper)
		goto out;

	pcd = gadget_wrapper->pcd;
	printk(KERN_DEBUG USB3_DWC "pcd=%p\n", pcd);

	if (!pcd) {
		printk(KERN_DEBUG USB3_DWC "ENODEV\n");
		goto out;
	}

	if (!driver ||
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0)
	    driver->speed == USB_SPEED_UNKNOWN ||
#else
	    driver->max_speed == USB_SPEED_UNKNOWN ||
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37) || \
    LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
	    !driver->bind ||
#endif
	    !driver->unbind ||
	    !driver->disconnect || !driver->setup) {
		printk(KERN_DEBUG USB3_DWC "EINVAL\n");
		retval = -EINVAL;
		goto out;
	}

	printk(KERN_DEBUG USB3_DWC "registering gadget driver '%s'\n",
	       driver->driver.name);

	if (gadget_wrapper->driver) {
		printk(KERN_DEBUG USB3_DWC "EBUSY (%p)\n",
		       gadget_wrapper->driver);
		retval = -EBUSY;
		goto out;
	}

#ifdef CONFIG_USB_OTG_DWC
	/* Check that the otg transceiver driver is loaded */
	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (IS_ERR(phy) || !phy) {
		printk(KERN_DEBUG USB3_DWC "OTG PHY not available!\n");
		if (!IS_ERR(phy))
			usb_put_phy(phy);
		goto out;
	}

	otg = phy->otg;
	usb_put_phy(phy);
	if (!otg) {
		printk(KERN_DEBUG USB3_DWC "OTG not available!\n");
		goto out;
	}
#endif

	/* hook up the driver */
	gadget_wrapper->driver = driver;
	gadget_wrapper->gadget.dev.driver = &driver->driver;
	pcd->gadget = &gadget_wrapper->gadget;

	dwc_debug(pcd->usb3_dev, "bind to driver %s\n", driver->driver.name);
	dwc_debug(pcd->usb3_dev, "&gadget_gadget_wrapper->gadget = %p\n",
		  &gadget_wrapper->gadget);
	dwc_debug(pcd->usb3_dev, "gadget_gadget_wrapper->driver = %p\n",
		  gadget_wrapper->driver);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
	retval = driver->bind(&gadget_wrapper->gadget);
#else
# if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
	retval = driver->bind(&gadget_wrapper->gadget,
			      gadget_wrapper->driver);
# else
	retval = bind(&gadget_wrapper->gadget);
# endif
#endif
	if (retval) {
		dwc_error(pcd->usb3_dev, "bind to driver %s --> error %d\n",
			  driver->driver.name, retval);
		gadget_wrapper->driver = NULL;
		gadget_wrapper->gadget.dev.driver = NULL;
		pcd->gadget = NULL;
		goto out;
	}

#ifdef CONFIG_USB_OTG_DWC
	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (!IS_ERR(phy)) {
		if (phy && phy->otg)
			otg_set_peripheral(phy->otg, &gadget_wrapper->gadget);
		usb_put_phy(phy);
	}
#endif

	printk(KERN_DEBUG USB3_DWC "registered gadget driver '%s'\n",
	       driver->driver.name);
out:
#ifdef CONFIG_USB_OTG_DWC
	/* Switch otg to host mode now that gadget is bound */
	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (!IS_ERR(phy)) {
		if (phy && phy->otg)
			dwc_otg3_set_peripheral(phy->otg, 0);
		usb_put_phy(phy);
	}
#endif
	return retval;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
EXPORT_SYMBOL_GPL(usb_gadget_register_driver);
#else
EXPORT_SYMBOL_GPL(usb_gadget_probe_driver);
#endif

/**
 * This function unregisters a gadget driver
 *
 * @param driver The driver being unregistered.
 */
int usb_gadget_unregister_driver(struct usb_gadget_driver *driver)
{
#ifdef CONFIG_USB_OTG_DWC
	struct usb_phy *phy;
	struct usb_otg *otg;
#endif

	printk(KERN_DEBUG USB3_DWC "%s(%p)\n", __func__, driver);
	printk(KERN_DEBUG USB3_DWC "unregistering gadget driver '%s'\n",
	       driver->driver.name);

	if (!gadget_wrapper)
		return -ENODEV;

	if (!gadget_wrapper->pcd) {
		printk(KERN_DEBUG USB3_DWC "%s Return(%d): pcd==NULL\n",
		       __func__, -ENODEV);
		return -ENODEV;
	}

	if (!gadget_wrapper->driver || driver != gadget_wrapper->driver) {
		printk(KERN_DEBUG USB3_DWC "%s Return(%d): driver?\n",
		       __func__, -EINVAL);
		return -EINVAL;
	}

#ifdef CONFIG_USB_OTG_DWC
	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	otg = phy->otg;
	otg_set_peripheral(otg, NULL);
	usb_put_phy(phy);
#endif

	driver->disconnect(&gadget_wrapper->gadget);
	driver->unbind(&gadget_wrapper->gadget);
	gadget_wrapper->driver = NULL;
	gadget_wrapper->gadget.dev.driver = NULL;
	gadget_wrapper->pcd->gadget = NULL;

	printk(KERN_DEBUG USB3_DWC "unregistered gadget driver '%s'\n",
	       driver->driver.name);
	return 0;
}
EXPORT_SYMBOL_GPL(usb_gadget_unregister_driver);

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
int udc_attach_driver(const char *name, struct usb_gadget_driver *driver)
{
	return 0;
}
EXPORT_SYMBOL_GPL(udc_attach_driver);
//#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
void usb_gadget_set_state(struct usb_gadget *gadget,
		enum usb_device_state state)
{
	gadget->state = state;
}
EXPORT_SYMBOL_GPL(usb_gadget_set_state);
#endif

MODULE_DESCRIPTION(DWC_DRIVER_DESC);
MODULE_AUTHOR("Synopsys Inc.");
MODULE_LICENSE("GPL");
