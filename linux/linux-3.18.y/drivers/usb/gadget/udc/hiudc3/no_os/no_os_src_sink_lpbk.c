/** @file
 *
 * Loopback/source-sink Function Driver which uses the application-specific
 * interface in no_os_gadget.c
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

#define DWC_BUF_SIZ	(1024 * 1024)

/** Driver context struct - defined in no_os_init.c */
extern dwc_usb3_device_t	g_usb3_dev;

#ifndef LINUXTEST
/** @{ */
/** Transfer buffers - 1MB each */
static char		g_in_buf[DWC_BUF_SIZ];
static char		g_out_buf[DWC_BUF_SIZ];
/** @} */
#endif

/**
 * The FPGA configuration is limited to a maximum transfer size of 128K by
 * default, so allocate 8 128K requests for each 1MB buffer.
 * NOTE: DWC_NUM_REQ * DWC_REQ_SIZ must be <= DWC_BUF_SIZ !
 */
/** @{ */
#define DWC_NUM_REQ	8
#define DWC_REQ_SIZ	(128 * 1024)
/** @} */

/**
 * Function Driver context struct
 */
typedef struct dwc_usb3_loopbk {
	usb_ep_t	*in_ep, *out_ep;
	char		*in_buf, *out_buf;
	dwc_dma_t	in_dma, out_dma;
	int		speed, maxp;
	u8		cfg, ifc, next0, src_sink;
} dwc_usb3_loopbk_t;

/**
 * The Function Driver context
 */
static dwc_usb3_loopbk_t	loopbk;

/**
 * Function Driver transfer complete callback routine
 */
static void loopbk_complete(usb_ep_t *ep, usb_request_t *req)
{
	int ret;

	dwc_debug1(&g_usb3_dev, "%s()\n", __func__);

	switch (req->status) {
	case 0:
		if (loopbk.src_sink) {
			if (ep == loopbk.out_ep) {
				dwc_debug0(&g_usb3_dev, "OUT req, requeuing\n");
				req->length = DWC_REQ_SIZ;
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.out_ep, req);
				if (ret == 0)
					return;
				dwc_error0(&g_usb3_dev, "Failed to requeue OUT req\n");
			} else {
				dwc_debug0(&g_usb3_dev, "IN req, requeuing\n");
				req->length = DWC_REQ_SIZ;
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.in_ep, req);
				if (ret == 0)
					return;
				dwc_error0(&g_usb3_dev, "Failed to requeue IN req\n");
			}
		} else {
			if (ep == loopbk.out_ep) {
				dwc_debug0(&g_usb3_dev, "OUT req, requeuing on IN\n");

				/* Handle 0-length marker packet */
				if (req->actual && (req->actual & (loopbk.maxp - 1)) == 0)
					loopbk.next0 = 1;

				/* Echo packet back to host on IN EP */
				req->length = req->actual;
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.in_ep, req);
				if (ret == 0)
					return;
				loopbk.next0 = 0;
				dwc_error0(&g_usb3_dev, "Failed to queue IN req, requeuing on OUT\n");
			} else {
				dwc_debug0(&g_usb3_dev, "IN req, requeuing on OUT\n");
			}

			/* Handle 0-length marker packet */
			if (loopbk.next0) {
				loopbk.next0 = 0;
				req->length = 0;
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.in_ep, req);
				if (ret == 0)
					return;
				dwc_error0(&g_usb3_dev, "Failed to queue IN 0-length req, requeuing on OUT\n");
			}

			/* Requeue for a future OUT EP transfer */
			req->length = DWC_REQ_SIZ;
			ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.out_ep, req);
			if (ret == 0)
				return;
			dwc_error0(&g_usb3_dev, "Failed to queue OUT req\n");
		}

		dwc_usb3_free_request(&g_usb3_dev, ep, req);
		break;

	default:
		dwc_error0(&g_usb3_dev, "Bad completion status\n");

		/* Requeue for a future OUT EP transfer */
		req->length = DWC_REQ_SIZ;
		ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.out_ep, req);
		if (ret == 0)
			return;
		dwc_error0(&g_usb3_dev, "Failed to queue OUT req\n");
		/* FALL-THRU */

	case -DWC_E_SHUTDOWN:
		dwc_debug0(&g_usb3_dev, "Shutdown status\n");
		dwc_usb3_free_request(&g_usb3_dev, ep, req);
		break;
	}
}

/**
 * Function Driver EP enable routine
 */
static int enable_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk)
{
	usb_ep_t *in_ep, *out_ep;

	switch (loopbk->speed) {
	case USB_SPEED_SUPER:
		in_ep = dwc_usb3_ep_enable(usb3_dev, &ss_config_desc.bulk_in_ep_desc,
					   &ss_config_desc.bulk_in_ss_ep_comp_desc);
		if (!in_ep)
			return -DWC_E_INVALID;

		out_ep = dwc_usb3_ep_enable(usb3_dev, &ss_config_desc.bulk_out_ep_desc,
					    &ss_config_desc.bulk_out_ss_ep_comp_desc);
		if (!out_ep) {
			dwc_usb3_ep_disable(usb3_dev, in_ep);
			return -DWC_E_INVALID;
		}

		break;

	case USB_SPEED_HIGH:
		in_ep = dwc_usb3_ep_enable(usb3_dev, &hs_config_desc.bulk_in_ep_desc, NULL);
		if (!in_ep)
			return -DWC_E_INVALID;

		out_ep = dwc_usb3_ep_enable(usb3_dev, &hs_config_desc.bulk_out_ep_desc, NULL);
		if (!out_ep) {
			dwc_usb3_ep_disable(usb3_dev, in_ep);
			return -DWC_E_INVALID;
		}

		break;

	case USB_SPEED_FULL:
		in_ep = dwc_usb3_ep_enable(usb3_dev, &fs_config_desc.bulk_in_ep_desc, NULL);
		if (!in_ep)
			return -DWC_E_INVALID;

		out_ep = dwc_usb3_ep_enable(usb3_dev, &fs_config_desc.bulk_out_ep_desc, NULL);
		if (!out_ep) {
			dwc_usb3_ep_disable(usb3_dev, in_ep);
			return -DWC_E_INVALID;
		}

		break;

	default:
		return -DWC_E_INVALID;
	}

	loopbk->in_ep = in_ep;
	loopbk->out_ep = out_ep;

	return 0;
}

/**
 * Function Driver EP disable routine
 */
static void disable_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk)
{
	if (loopbk->out_ep) {
		dwc_usb3_ep_disable(usb3_dev, loopbk->out_ep);
		loopbk->out_ep = NULL;
	}

	if (loopbk->in_ep) {
		dwc_usb3_ep_disable(usb3_dev, loopbk->in_ep);
		loopbk->in_ep = NULL;
	}
}

/**
 * Function Driver SET_INTERFACE routine
 */
static int set_interface(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk, int alt)
{
	usb_request_t	*req;
	int		i, ret = 0;

	if (alt == -1)
		goto cleanup;

	/* Already set? */
	if (loopbk->out_ep)
		return 0;

	ret = enable_eps(usb3_dev, loopbk);
	if (ret)
		return ret;

	ret = -DWC_E_NO_MEMORY;

	if (loopbk->src_sink) {
		for (i = 0; i < DWC_NUM_REQ; i++) {
			req = dwc_usb3_alloc_request(usb3_dev, loopbk->in_ep);
			if (!req)
				goto cleanup;
			req->buf = loopbk->in_buf + DWC_REQ_SIZ * i;
			req->dma = loopbk->in_dma + DWC_REQ_SIZ * i;
			req->length = DWC_REQ_SIZ;
			req->complete = loopbk_complete;
			ret = dwc_usb3_ep_queue(usb3_dev, loopbk->in_ep, req);
			if (ret)
				goto cleanup;
		}
	}

	for (i = 0; i < DWC_NUM_REQ; i++) {
		req = dwc_usb3_alloc_request(usb3_dev, loopbk->out_ep);
		if (!req)
			goto cleanup;
		req->buf = loopbk->out_buf + DWC_REQ_SIZ * i;
		req->dma = loopbk->out_dma + DWC_REQ_SIZ * i;
		req->length = DWC_REQ_SIZ;
		req->complete = loopbk_complete;
		ret = dwc_usb3_ep_queue(usb3_dev, loopbk->out_ep, req);
		if (ret)
			goto cleanup;
	}

	return 0;

cleanup:
	/* disable_eps() will eventually dequeue all requests queued on each EP,
	 * and call the ->complete routine with -DWC_E_SHUTDOWN status for each
	 * one. That in turn will free the request. So all cleanup is done for
	 * us by this one call.
	 */
	disable_eps(usb3_dev, loopbk);
	loopbk->next0 = 0;
	return ret;
}

/**
 * This routine handles Function Driver specific Setup requests. Generic
 * requests are handled in ep0.c and no_os_ep0.c.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param ctrl  Pointer to the Setup packet for the request.
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_function_setup(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	u16	windex = UGETW(ctrl->wIndex);
	u16	wvalue = UGETW(ctrl->wValue);
	u16	wlength = UGETW(ctrl->wLength);
	int	len = 0, ret = 0;

	switch (ctrl->bRequest) {
	case UR_SET_FEATURE:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_FEATURE\n");

		/* We don't need to do anything for this */
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	case UR_SET_INTERFACE:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_INTERFACE\n");
		if (!loopbk.cfg || windex) {
			dwc_error2(pcd->usb3_dev, "cfg=%x wIndex=%x\n", loopbk.cfg, windex);
			return -DWC_E_NOT_SUPPORTED;
		}

		dwc_debug1(pcd->usb3_dev, "ifc=%x\n", wvalue);

		/* If interface has changed, disable the old EPs and enable the new ones */
		if (loopbk.ifc != wvalue) {
			set_interface(pcd->usb3_dev, &loopbk, -1);
			ret = set_interface(pcd->usb3_dev, &loopbk, wvalue);
			if (ret)
				return ret;
		}

		loopbk.ifc = wvalue;
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	case UR_GET_INTERFACE:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_GET_INTERFACE\n");
		if (!loopbk.cfg) {
			dwc_error1(pcd->usb3_dev, "cfg=%x\n", loopbk.cfg);
			return -DWC_E_NOT_SUPPORTED;
		}

		if (windex) {
			dwc_error1(pcd->usb3_dev, "wIndex=%x\n", windex);
			return -DWC_E_DOMAIN;
		}

		pcd->ep0_status_buf[0] = loopbk.ifc;
		len = 1;
		break;

	case UR_SET_CONFIG:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_CONFIG\n");
		if (wvalue != 0 && wvalue != 1) { // we only have one configuration
			dwc_error1(pcd->usb3_dev, "wValue=%x\n", wvalue);
			return -DWC_E_NOT_SUPPORTED;
		}

		/* If config already set, clear it and disable the EPs */
		if (loopbk.cfg) {
			loopbk.cfg = 0;
			loopbk.ifc = 0;
			set_interface(pcd->usb3_dev, &loopbk, -1);
		}

		/* If new config is 1, enable the EPs for interface 0 */
		if (wvalue) {
			loopbk.cfg = wvalue;
			loopbk.ifc = 0;
			ret = set_interface(pcd->usb3_dev, &loopbk, 0);
			if (ret)
				loopbk.cfg = 0;
		}

		dwc_debug1(pcd->usb3_dev, "cfg=%x\n", loopbk.cfg);
		if (ret)
			return ret;
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	case UR_GET_CONFIG:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_GET_CONFIG\n");
		pcd->ep0_status_buf[0] = loopbk.cfg;
		len = 1;
		break;

	case UR_SYNCH_FRAME:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SYNCH_FRAME\n");

		/* We don't need to do anything for this */
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	default:
		dwc_debug0(pcd->usb3_dev, "Unknown request!\n");
		return -DWC_E_NOT_SUPPORTED;
	}

	/* Start the data phase for 3-stage transfers */
	pcd->ep0state = EP0_IN_DATA_PHASE;
	dwc_usb3_pcd_ep0_data_stage(pcd, len < wlength ? len : wlength);

	return 0;
}

/**
 * Function Driver CONNECT routine
 */
int dwc_usb3_function_connect(struct dwc_usb3_device *usb3_dev, int speed)
{
	loopbk.speed = speed;

	switch (speed) {
	case USB_SPEED_SUPER:
		loopbk.maxp = 1024;
		break;

	case USB_SPEED_HIGH:
		loopbk.maxp = 512;
		break;

	case USB_SPEED_FULL:
		loopbk.maxp = 64;
		break;
	}

	return 0;
}

/**
 * Function Driver DISCONNECT routine
 */
int dwc_usb3_function_disconnect(struct dwc_usb3_device *usb3_dev)
{
	return 0;
}

/**
 * Function Driver initialization routine
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @return              0 for success, else negative error code.
 */
int dwc_usb3_function_init(dwc_usb3_device_t *usb3_dev)
{
	dwc_debug1(usb3_dev, "%s()\n", __func__);

	loopbk.src_sink = 0;	/* for now */

#ifndef LINUXTEST
	if (loopbk.src_sink) {
		loopbk.in_buf = g_in_buf;
		loopbk.in_dma = (dwc_dma_t)g_in_buf;
	}

	loopbk.out_buf = g_out_buf;
	loopbk.out_dma = (dwc_dma_t)g_out_buf;

	return 0;
#else
	if (loopbk.src_sink) {
		loopbk.in_buf = dma_alloc_coherent(usb3_dev->dev, DWC_BUF_SIZ,
						   &loopbk.in_dma, GFP_KERNEL | GFP_DMA32);
		if (!loopbk.in_buf)
			goto out0;
	}

	loopbk.out_buf = dma_alloc_coherent(usb3_dev->dev, DWC_BUF_SIZ,
					    &loopbk.out_dma, GFP_KERNEL | GFP_DMA32);
	if (!loopbk.out_buf)
		goto out1;

	return 0;

out1:
	if (loopbk.src_sink)
		dma_free_coherent(usb3_dev->dev, DWC_BUF_SIZ, loopbk.in_buf, loopbk.in_dma);
out0:
	return -DWC_E_NO_MEMORY;
#endif
}

/**
 * Function Driver removal routine
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 */
void dwc_usb3_function_remove(dwc_usb3_device_t *usb3_dev)
{
	dwc_debug1(usb3_dev, "%s()\n", __func__);

#ifdef LINUXTEST
	dma_free_coherent(usb3_dev->dev, DWC_BUF_SIZ, loopbk.out_buf, loopbk.out_dma);
	if (loopbk.src_sink)
		dma_free_coherent(usb3_dev->dev, DWC_BUF_SIZ, loopbk.in_buf, loopbk.in_dma);
#endif
}
