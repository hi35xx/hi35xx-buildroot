/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

int main(int argc, char *argv[])
{
	return 0;
}

/******************************************************************************
 * Function driver notification routines
 *
 * These routines receive notifications from the PCD when certain events
 * occur which the Function driver may need to be aware of.
 *
 * These routines *must* have the exact names and parameters shown here,
 * because they are part of the API between the PCD and the Function driver.
 ******************************************************************************/

/**
 * This routine receives Connect notifications from the PCD
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param speed Speed of the connection (as defined in usb.h)
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_gadget_connect(dwc_usb3_pcd_t *pcd, int speed)
{
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
	return 0;
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
	/* TODO */
}

/******************************************************************************
 * Function driver callback routines
 *
 * The PCD calls these routines when it needs something from the Function
 * driver.
 *
 * These routines *must* have the exact names and parameters shown here,
 * because they are part of the API between the PCD and the Function driver.
 ******************************************************************************/

/**
 * This routine allocates coherent DMA memory. It is used by the PCD to
 * allocate memory for TRBs. The block of memory returned must have a start
 * address aligned to a 16-byte boundary.
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
	return NULL;
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
	return NULL;
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
}

/**
 * This routine terminates all requests which are pending on an endpoint.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        EP to operate on.
 */
void dwc_usb3_gadget_request_nuke(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep)
{
}

/**
 * This routine marks all pending requests for an EP as not started.
 *
 * @param pcd           Programming view of DWC_usb3 peripheral controller.
 * @param pcd_ep        EP to operate on.
 */
void dwc_usb3_gadget_set_ep_not_started(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *pcd_ep)
{
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
}
