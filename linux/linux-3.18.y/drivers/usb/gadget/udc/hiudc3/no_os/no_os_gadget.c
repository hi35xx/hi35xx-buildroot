/** @file
 *
 * Application-specific Function Driver interface
 *
 * Simple applications may wish to implement their entire Function Driver here.
 *
 * More complex applications, or environments with an existing Function Driver
 * framework (such as the Linux Gadget framework) will implement the interface
 * to their Function Driver here.
 *
 * See the linux/ subdirectory for an example implementation of a Linux Gadget
 * framework interface.
 *
 * For historical reasons, the PCD API uses the Linux term "gadget" to refer to
 * a Function Driver.
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

/** Driver context struct - defined in no_os_init.c */
extern dwc_usb3_device_t	g_usb3_dev;

/** @{ */
/** PCD request pool - defined in no_os_init.c */
extern dwc_usb3_pcd_req_t	g_pcd_req[];
extern u32			g_pcd_req_bm;
/** @} */

#ifndef LINUXTEST
/** @{ */
/** DMA descriptor (TRB) pools for non-EP0 EPs - defined in no_os_init.c */
extern dwc_usb3_dma_desc_t	g_out_trb_pool[][DWC_NUM_ISOC_TRBS + 1];
extern dwc_usb3_dma_desc_t	g_in_trb_pool[][DWC_NUM_ISOC_TRBS + 1];
/** @} */
#endif

/******************************************************************************
 * Function Driver notification routines
 *
 * These routines receive notifications from the PCD when certain events occur
 * which the Function Driver may need to be aware of.
 *
 * These routines *must* have the exact names and parameters shown here,
 * because they are part of the API between the PCD and the Function Driver.
 ******************************************************************************/

/**
 * This routine receives Connect notifications from the PCD
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param speed Speed of the connection (as defined in usb.h).
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_gadget_connect(dwc_usb3_pcd_t *pcd, int speed)
{
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);
	dwc_usb3_function_connect(pcd->usb3_dev, speed);
	return 0;
}

/**
 * This routine receives Disconnect notifications from the PCD
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_gadget_disconnect(dwc_usb3_pcd_t *pcd)
{
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);
	dwc_usb3_function_disconnect(pcd->usb3_dev);
	return 0;
}

/**
 * This routine receives Suspend notifications from the PCD
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_gadget_suspend(dwc_usb3_pcd_t *pcd)
{
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);
	return 0;
}

/**
 * This routine receives Resume notifications from the PCD
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_gadget_resume(dwc_usb3_pcd_t *pcd)
{
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);
	return 0;
}

/**
 * This routine receives Setup request notifications from the PCD
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param ctrl  Pointer to the Setup packet for the request.
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_gadget_setup(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	int ret;

	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);

	ret = dwc_usb3_no_os_setup(pcd, ctrl);

	return ret;
}

/**
 * This routine receives Transfer Complete notifications from the PCD
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        PCD EP for the transfer.
 * @param pcd_req       PCD request for the transfer.
 * @param status        Transfer status, 0 for success else negative error code.
 * @return              0 for success, else negative error code.
 */
int dwc_usb3_gadget_complete(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep,
			     dwc_usb3_pcd_req_t *pcd_req, int status)
{
	usb_ep_t	*usb_ep = &pcd_ep->usb_ep;
	usb_request_t	*usb_req = &pcd_req->usb_req;
	u32		actual = pcd_req->dwc_req.actual;

	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd_ep);

	/* Remove the request from the queue */
	DWC_CIRCLEQ_REMOVE_INIT(&pcd_ep->dwc_ep.queue, pcd_req, entry);

	usb_req->status = status;
	usb_req->actual = actual;

	if (usb_req->complete) {
		dwc_release_spinlock(pcd->usb3_dev, &pcd->lock);
		usb_req->complete(usb_ep, usb_req);
		dwc_acquire_spinlock(pcd->usb3_dev, &pcd->lock);
	}

	if (pcd->request_pending > 0)
		--pcd->request_pending;

	return 0;
}

/**
 * This routine allows overriding the standard Control transfer handling
 * (currently only done by the axs101 test gadget)
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 */
void dwc_usb3_gadget_do_setup(dwc_usb3_pcd_t *pcd)
{
	/* Call the standard Control transfer handler */
	dwc_usb3_pcd_do_setup(pcd);
}

/******************************************************************************
 * Function Driver callback routines
 *
 * The PCD calls these routines when it needs something from the Function
 * Driver.
 *
 * These routines *must* have the exact names and parameters shown here,
 * because they are part of the API between the PCD and the Function Driver.
 ******************************************************************************/

/**
 * This routine allocates coherent DMA memory. It is used by the PCD to
 * allocate memory for TRBs. The block of memory returned must have a start
 * address aligned to a 16-byte boundary.
 *
 * This routine may be called from interrupt context, so it must be able to
 * allocate coherent DMA memory in that context. One strategy would be to
 * preallocate a block of memory at initialization time, and hand out chunks
 * from that block in this routine.
 *
 * @param pcd_ep        PCD EP that memory block will be associated with.
 * @param size          Size of memory block to allocate, in bytes.
 * @param mem_dma_ret   Physical address of allocated memory block is returned
 *                      through this pointer.
 * @return              Address of allocated memory block, or NULL if allocation
 *                      fails.
 */
void *dwc_usb3_gadget_alloc_dma(dwc_usb3_pcd_ep_t *pcd_ep, int size, dwc_dma_t *mem_dma_ret)
{
	void		*mem;
	dwc_dma_t	mem_dma;

	dwc_debug2(pcd_ep->dwc_ep.pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd_ep);

	if (!pcd_ep->dwc_ep.num) {
		dwc_error0(pcd_ep->dwc_ep.pcd->usb3_dev, "USB epnum is 0 for pcd_ep\n");
		return NULL;
	}

#ifdef LINUXTEST
	mem = dma_alloc_coherent(g_usb3_dev.dev, size, &mem_dma, GFP_ATOMIC | GFP_DMA32);
	if (!mem)
		return NULL;
#else
	if (pcd_ep->dwc_ep.is_in)
		mem = (void *)&g_in_trb_pool[pcd_ep->dwc_ep.num - 1];
	else
		mem = (void *)&g_out_trb_pool[pcd_ep->dwc_ep.num - 1];

	mem_dma = (dwc_dma_t)mem;
#endif
	*mem_dma_ret = mem_dma;

	return mem;
}

/**
 * This routine frees DMA memory allocated by dwc_usb3_gadget_alloc_dma().
 *
 * @param pcd_ep        PCD EP that memory block is associated with.
 * @param size          Size of memory block to free, in bytes.
 * @param mem           Address of memory block.
 * @param mem_dma       Physical address of memory block.
 */
void dwc_usb3_gadget_free_dma(dwc_usb3_pcd_ep_t *pcd_ep, int size, void *mem, dwc_dma_t mem_dma)
{
	dwc_debug2(pcd_ep->dwc_ep.pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd_ep);

#ifdef LINUXTEST
	dma_free_coherent(g_usb3_dev.dev, size, mem, mem_dma);
#endif
}

/**
 * This routine returns the PCD request corresponding to the current transfer
 * request for an endpoint. The current transfer request is the first request
 * submitted that has not been completed yet.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        PCD EP to operate on.
 * @return              Pointer to PCD request, or NULL if no request available.
 */
dwc_usb3_pcd_req_t *dwc_usb3_gadget_get_request(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep)
{
	dwc_usb3_pcd_req_t *pcd_req;

	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd_ep);

	/* Check for a pending request */
	if (DWC_CIRCLEQ_EMPTY(&pcd_ep->dwc_ep.queue)) {
		dwc_debug0(pcd->usb3_dev, "ep->dwc_ep.queue empty\n");
		return NULL;
	}

	pcd_req = DWC_CIRCLEQ_FIRST(&pcd_ep->dwc_ep.queue);
	return pcd_req;
}

/**
 * This routine checks to see if there is another transfer request waiting
 * on an endpoint that has not been started yet. If so then that transfer is
 * started.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        PCD EP to operate on.
 */
void dwc_usb3_gadget_start_next_request(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep)
{
	dwc_usb3_pcd_req_t *pcd_req = NULL;

	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd_ep);

	if (DWC_CIRCLEQ_EMPTY(&pcd_ep->dwc_ep.queue)) {
		dwc_debug2(pcd->usb3_dev, "start_next EP%d-%s: queue empty\n",
			   pcd_ep->dwc_ep.num, pcd_ep->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	DWC_CIRCLEQ_FOREACH(pcd_req, &pcd_ep->dwc_ep.queue, entry) {
		if (!(pcd_req->dwc_req.flags & DWC_PCD_REQ_STARTED)) {
			dwc_debug2(pcd->usb3_dev, "start_next EP%d-%s: OK\n",
				   pcd_ep->dwc_ep.num, pcd_ep->dwc_ep.is_in ? "IN" : "OUT");

			/* Setup and start the transfer */
			dwc_usb3_pcd_fill_trbs(pcd, pcd_ep, pcd_req);
			dwc_usb3_pcd_ep_start_transfer(pcd, pcd_ep, pcd_req, 0);
			return;
		}
	}

	dwc_debug2(pcd->usb3_dev, "start_next EP%d-%s: all req active\n",
		   pcd_ep->dwc_ep.num, pcd_ep->dwc_ep.is_in ? "IN" : "OUT");
}

/**
 * Start an Isoc EP running at the proper interval, after receiving the initial
 * XferNrdy event.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        EP to operate on.
 * @param event         Event data containing the XferNrdy microframe.
 */
void dwc_usb3_gadget_isoc_ep_start(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep, u32 event)
{
	dwc_usb3_pcd_req_t *pcd_req = NULL;

	dwc_debug3(pcd->usb3_dev, "%s(%lx,%x)\n", __func__, (unsigned long)pcd_ep, event);
	dwc_isocdbg2(pcd->usb3_dev, "%s(%08x)\n", __func__, event);

	if (DWC_CIRCLEQ_EMPTY(&pcd_ep->dwc_ep.queue)) {
		dwc_debug2(pcd->usb3_dev, "%s(%lx), ep->dwc_ep.queue empty\n",
			   __func__, (unsigned long)pcd_ep);
		return;
	}

	if (pcd_ep->dwc_ep.desc_avail <= 0) {
		dwc_debug2(pcd->usb3_dev, "EP%d-%s: no TRB avail\n",
			   pcd_ep->dwc_ep.num, pcd_ep->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	/* If need to restart after hibernation, handle that and return */
	if (dwc_usb3_pcd_isoc_ep_hiber_restart(pcd, pcd_ep))
		return;

	/*
	 * Start the next queued transfer at the target uFrame
	 */

	DWC_CIRCLEQ_FOREACH(pcd_req, &pcd_ep->dwc_ep.queue, entry) {
		if (pcd_req->dwc_req.flags & DWC_PCD_REQ_STARTED)
			pcd_req = NULL;
		else
			break;
	}

	dwc_debug1(pcd->usb3_dev, "req=%lx\n", (unsigned long)pcd_req);
	if (!pcd_req) {
		dwc_debug2(pcd->usb3_dev, "EP%d-%s: no requests to start\n",
			   pcd_ep->dwc_ep.num, pcd_ep->dwc_ep.is_in ? "IN" : "OUT");
		return;
	}

	dwc_usb3_pcd_fill_trbs(pcd, pcd_ep, pcd_req);
	dwc_usb3_pcd_ep_start_transfer(pcd, pcd_ep, pcd_req, event);

	/*
	 * Now start any remaining queued transfers
	 */

	while (pcd_req != DWC_CIRCLEQ_LAST(&pcd_ep->dwc_ep.queue)) {
		pcd_req = DWC_CIRCLEQ_NEXT(pcd_req, entry);
		if (!(pcd_req->dwc_req.flags & DWC_PCD_REQ_STARTED)) {
			dwc_usb3_pcd_fill_trbs(pcd, pcd_ep, pcd_req);
			dwc_usb3_pcd_ep_start_transfer(pcd, pcd_ep, pcd_req, 0);
		}
	}
}

/**
 * This routine terminates all requests which are pending on an endpoint.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        EP to operate on.
 */
void dwc_usb3_gadget_request_nuke(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep)
{
	dwc_usb3_pcd_req_t *pcd_req;

	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd_ep);
	pcd_ep->dwc_ep.stopped = 1;

	while (!DWC_CIRCLEQ_EMPTY(&pcd_ep->dwc_ep.queue)) {
		pcd_req = DWC_CIRCLEQ_FIRST(&pcd_ep->dwc_ep.queue);
		dwc_usb3_pcd_request_done(pcd, pcd_ep, pcd_req, -DWC_E_SHUTDOWN);
	}

	if (pcd_ep != pcd->ep0)
		pcd_ep->dwc_ep.hiber_desc_idx = 0;
}

/**
 * This routine marks all pending requests for an EP as not started.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        EP to operate on.
 */
void dwc_usb3_gadget_set_ep_not_started(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep)
{
	dwc_usb3_pcd_req_t *pcd_req;

	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd_ep);

	DWC_CIRCLEQ_FOREACH(pcd_req, &pcd_ep->dwc_ep.queue, entry) {
		if (pcd_req->dwc_req.flags & DWC_PCD_REQ_STARTED)
			pcd_req->dwc_req.flags &= ~DWC_PCD_REQ_STARTED;
	}
}

/******************************************************************************
 * Function Driver user-defined routines
 *
 * These routines are completely user-defined, and can implement whatever is
 * required for the application's Function Driver.
 *
 * The example code here implements a simple interface which allows external
 * code to implement the application's Function Driver by calling these
 * routines.
 ******************************************************************************/

/**
 * This routine enables a USB endpoint
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @param epdesc        USB endpoint descriptor for the EP.
 * @param epcomp        USB SS endpoint companion descriptor for the EP.
 * @return              Pointer to USB EP context, or NULL on failure.
 */
usb_ep_t *dwc_usb3_ep_enable(struct dwc_usb3_device *usb3_dev, const void *epdesc, const void *epcomp)
{
	const usb_endpoint_descriptor_t			*ep_desc = epdesc;
	const ss_endpoint_companion_descriptor_t	*ep_comp = epcomp;
	dwc_usb3_pcd_t					*pcd = &usb3_dev->pcd;
	dwc_usb3_pcd_ep_t				*pcd_ep;
	dwc_usb3_dma_desc_t				*desc;
	dwc_dma_t					desc_dma;
	usb_ep_t					*usb_ep;
	int						ep_num, ep_dir, ep_type;
	int						link, num_trbs, retval;

	dwc_debug1(usb3_dev, "%s()\n", __func__);

	ep_num = UE_GET_ADDR(ep_desc->bEndpointAddress);
	ep_dir = UE_GET_DIR(ep_desc->bEndpointAddress);
	ep_type = UE_GET_XFERTYPE(ep_desc->bmAttributes);
	link = 0;

	if (ep_num < 1 || ep_num >= DWC_MAX_EPS)
		return NULL;

	if (ep_dir == UE_DIR_IN)
		pcd_ep = pcd->in_ep[ep_num - 1];
	else
		pcd_ep = pcd->out_ep[ep_num - 1];
	usb_ep = &pcd_ep->usb_ep;

	/* Allocate the number of TRBs based on EP type */
	switch (ep_type) {
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

	dwc_debug5(usb3_dev, "ep%d-%s=%lx phys=%d pcd_ep=%lx\n", ep_num,
		   ep_dir == UE_DIR_IN ? "IN" : "OUT", (unsigned long)usb_ep,
		   pcd_ep->dwc_ep.phys, (unsigned long)pcd_ep);

	/* Set the TRB allocation for this EP */
	desc = dwc_usb3_pcd_trb_alloc(pcd_ep, num_trbs, ep_type, ep_desc->bInterval - 1,
				      link, &desc_dma);
	if (!desc)
		return NULL;

	/* Call the PCD API routine to enable the endpoint */
	retval = dwc_usb3_pcd_ep_enable(pcd, pcd_ep, ep_desc, ep_comp);
	if (retval) {
		dwc_usb3_pcd_trb_free(pcd_ep);
		return NULL;
	}

	usb_ep->maxpacket = UGETW(ep_desc->wMaxPacketSize);
	usb_ep->desc = epdesc;

	return usb_ep;
}

/**
 * This routine disables a USB endpoint
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @param usb_ep        USB EP to disable.
 * @return              0 for success, else negative error code.
 */
int dwc_usb3_ep_disable(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep)
{
	dwc_usb3_pcd_t		*pcd = &usb3_dev->pcd;
	dwc_usb3_pcd_ep_t	*pcd_ep;
	int			retval;

	dwc_debug2(usb3_dev, "%s(%lx)\n", __func__, (unsigned long)usb_ep);
	pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);

	/* Call the PCD API routine to disable the endpoint */
	retval = dwc_usb3_pcd_ep_disable(pcd, pcd_ep);

	dwc_usb3_pcd_trb_free(pcd_ep);

	return retval;
}

/**
 * This routine allocates a USB request object to use with the specified USB
 * endpoint. The contents of a USB request are defined by the Function Driver,
 * and are opaque to the PCD. The USB request is embedded inside of an enclosing
 * PCD request object (see the definition of struct dwc_usb3_pcd_req in pcd.h).
 * One request object is needed for each transfer that is submitted to the PCD.
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @param usb_ep        USB EP for the request.
 */
usb_request_t *dwc_usb3_alloc_request(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep)
{
	dwc_usb3_pcd_req_t	*pcd_req;
	unsigned int		req_idx;

	dwc_debug2(usb3_dev, "%s(%lx)\n", __func__, (unsigned long)usb_ep);

	for (req_idx = 0; req_idx < 32; req_idx++) {
		if (g_pcd_req_bm & (1 << req_idx)) {
			g_pcd_req_bm &= ~(1 << req_idx);
			pcd_req = &g_pcd_req[req_idx];
			memset(pcd_req, 0, sizeof(*pcd_req));
			pcd_req->usb_req.dma = DWC_DMA_ADDR_INVALID;
			return &pcd_req->usb_req;
		}
	}

	return NULL;
}

/**
 * This routine frees a USB request object
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @param usb_ep        USB EP for the request.
 * @param usb_req       USB request to be freed.
 */
void dwc_usb3_free_request(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep, usb_request_t *usb_req)
{
	dwc_usb3_pcd_req_t	*pcd_req = dwc_usb3_get_pcd_req(usb_req);
	unsigned int		req_idx = pcd_req - &g_pcd_req[0];

	dwc_debug2(usb3_dev, "%s(%lx)\n", __func__, (unsigned long)usb_ep);

	if (req_idx >= 32) {
		dwc_error1(usb3_dev, "Bad request index %u\n", req_idx);
		return;
	}

	g_pcd_req_bm |= 1 << req_idx;
}

/**
 * This routine submits a transfer request for a USB endpoint. The example code
 * maintains a queue of requests for each endpoint, so that the application can
 * start another transfer on the same endpoint without having to wait for the
 * first transfer to complete.
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @param usb_ep        USB EP for the transfer.
 * @param usb_req       USB request for the transfer.
 * @return              0 for success, else negative error code.
 */
int dwc_usb3_ep_queue(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep, usb_request_t *usb_req)
{
	dwc_usb3_pcd_t		*pcd = &usb3_dev->pcd;
	dwc_usb3_pcd_ep_t	*pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	dwc_usb3_pcd_req_t	*pcd_req = dwc_usb3_get_pcd_req(usb_req);
	int			retval = 0, req_flags = 0;

	dwc_debug2(usb3_dev, "%s(%lx)\n", __func__, (unsigned long)usb_ep);

	usb_req->status = -DWC_E_IN_PROGRESS;
	usb_req->actual = 0;

	if (usb_req->zero)
		req_flags |= DWC_PCD_REQ_ZERO;

	pcd_req->dwc_req.buf[0] = usb_req->buf;
	pcd_req->dwc_req.bufdma[0] = usb_req->dma;
	pcd_req->dwc_req.buflen[0] = usb_req->length;
	pcd_req->dwc_req.numbuf = 1;
	pcd_req->dwc_req.length = usb_req->length;
	pcd_req->dwc_req.actual = 0;
	pcd_req->dwc_req.stream = usb_req->stream_id;
	pcd_req->dwc_req.flags = req_flags;

	DWC_CIRCLEQ_INIT_ENTRY(pcd_req, entry);
#if 1
	if (!DWC_CIRCLEQ_EMPTY(&pcd_ep->dwc_ep.queue)) {
		/* If queue is not empty, and this is not an Isoc transfer, then
		 * don't submit the transfer. But we still queue the request so
		 * it can be submitted later.
		 */
		dwc_debug4(usb3_dev, "q !empty, pend %d, stop %d, avail %d, start %d\n",
			   pcd->request_pending, pcd_ep->dwc_ep.stopped,
			   pcd_ep->dwc_ep.desc_avail, pcd_ep->dwc_ep.xfer_started);

		if (!pcd_ep->dwc_ep.stopped && pcd_ep->dwc_ep.type == UE_ISOCHRONOUS &&
		    pcd_ep->dwc_ep.desc_avail > 0 && pcd_ep->dwc_ep.xfer_started)
			goto do_start;
	} else
#endif
	if (pcd_ep->dwc_ep.stopped || (pcd_ep != pcd->ep0 && pcd_ep->dwc_ep.desc_avail <= 0) ||
	    (pcd_ep->dwc_ep.type == UE_ISOCHRONOUS && !pcd_ep->dwc_ep.xfer_started)) {
		/* If EP is stopped, or there is no TRB available, or this
		 * is an Isoc transfer and the EP is not started, then don't
		 * submit the transfer. But we still queue the request so it
		 * can be submitted later.
		 */
		dwc_debug4(usb3_dev,
			   " q empty, pend %d, stop %d, avail %d, start %d\n",
//			   " pend %d, stop %d, avail %d, start %d\n",
			   pcd->request_pending, pcd_ep->dwc_ep.stopped,
			   pcd_ep->dwc_ep.desc_avail, pcd_ep->dwc_ep.xfer_started);
	} else {
do_start:
		/* Call the PCD API routine to set up the request TRBs */
		dwc_usb3_pcd_fill_trbs(pcd, pcd_ep, pcd_req);

		/* Call the PCD API routine to submit the transfer request */
		retval = dwc_usb3_pcd_ep_submit_req(pcd, pcd_ep, pcd_req, req_flags);
	}

	if (!retval) {
		DWC_CIRCLEQ_INSERT_TAIL(&pcd_ep->dwc_ep.queue, pcd_req, entry);
		++pcd->request_pending;
	}

	/* dwc_usb3_pcd_ep_submit_req() can return positive values for
	 * Control transfers, don't propagate those from this function
	 */
	return retval < 0 ? retval : 0;
}

/**
 * This routine cancels a transfer request for a USB endpoint. This is only
 * needed in exceptional cases, a normal transfer completion does not require
 * this.
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @param usb_ep        USB EP for the transfer.
 * @param usb_req       USB request for the transfer.
 * @return              0 for success, else negative error code.
 */
int dwc_usb3_ep_dequeue(struct dwc_usb3_device *usb3_dev, usb_ep_t *usb_ep, usb_request_t *usb_req)
{
	dwc_usb3_pcd_t		*pcd = &usb3_dev->pcd;
	dwc_usb3_pcd_ep_t	*pcd_ep = dwc_usb3_get_pcd_ep(usb_ep);
	dwc_usb3_pcd_req_t	*pcd_req;

	dwc_debug2(usb3_dev, "%s(%lx)\n", __func__, (unsigned long)usb_ep);

	if (dwc_usb3_pcd_ep_num(pcd_ep) != 0 && !pcd_ep->dwc_ep.usb_ep_desc) {
		dwc_warn1(usb3_dev, "%s, bad pcd_ep\n", __func__);
		return -DWC_E_INVALID;
	}

	/* Make sure it's actually queued on this EP */
	DWC_CIRCLEQ_FOREACH(pcd_req, &pcd_ep->dwc_ep.queue, entry) {
		if (&pcd_req->usb_req == usb_req)
			break;
	}

	if (!pcd_req) {
		dwc_warn1(usb3_dev, "%s, no request in queue\n", __func__);
		return 0;
	}

	/* Call the PCD API routine to cancel the transfer request */
	dwc_usb3_pcd_ep_cancel_req(pcd, pcd_ep, pcd_req, usb_req->stream_id);

	return 0;
}

/**
 * Function Driver initialization routine
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @return              0 for success, else negative error code.
 */
int dwc_usb3_gadget_init(dwc_usb3_device_t *usb3_dev)
{
	dwc_usb3_pcd_t		*pcd = &usb3_dev->pcd;
	dwc_usb3_pcd_ep_t	*ep;
	int			i;

	dwc_debug1(usb3_dev, "%s()\n", __func__);

	ep = pcd->ep0;
	ep->dwc_ep.num = 0;
	DWC_CIRCLEQ_INIT(&ep->dwc_ep.queue);

	for (i = 0; i < pcd->num_out_eps; i++) {
		ep = pcd->out_ep[i];
		ep->dwc_ep.num = i + 1;
		DWC_CIRCLEQ_INIT(&ep->dwc_ep.queue);
	}

	for (i = 0; i < pcd->num_in_eps; i++) {
		ep = pcd->in_ep[i];
		ep->dwc_ep.num = i + 1;
		DWC_CIRCLEQ_INIT(&ep->dwc_ep.queue);
	}

	return 0;
}

/**
 * Function Driver removal routine
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 */
void dwc_usb3_gadget_remove(dwc_usb3_device_t *usb3_dev)
{
	dwc_debug1(usb3_dev, "%s()\n", __func__);
}
