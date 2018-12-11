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
#include "ute_if.h"
#endif

/*=======================================================================*/
/*
 * EP0 routines
 */

#if !defined(__linux__) || !defined(DWC_BOS_IN_GADGET)

/** The BOS Descriptor */

static usb_dev_cap_20_ext_desc_t cap1_desc = {
	sizeof(usb_dev_cap_20_ext_desc_t),	/* bLength */
	UDESC_DEVICE_CAPABILITY,		/* bDescriptorType */
	USB_DEVICE_CAPABILITY_20_EXTENSION,	/* bDevCapabilityType */
	UCONSTDW(USB_20_EXT_LPM),		/* bmAttributes */
};

static usb_dev_cap_ss_usb_t cap2_desc = {
	sizeof(usb_dev_cap_ss_usb_t),		/* bLength */
	UDESC_DEVICE_CAPABILITY,		/* bDescriptorType */
	USB_DEVICE_CAPABILITY_SS_USB,		/* bDevCapabilityType */
	0x0,					/* bmAttributes */
	UCONSTW(USB_DC_SS_USB_SPEED_SUPPORT_SS	/* wSpeedsSupported */
	    | USB_DC_SS_USB_SPEED_SUPPORT_HIGH
	    | USB_DC_SS_USB_SPEED_SUPPORT_FULL),
	USB_DC_SS_USB_SPEED_SUPPORT_FULL,	/* bFunctionalitySupport */
	/* @todo set these to correct values */
	10,					/* bU1DevExitLat (10 us) */
	UCONSTW(256),				/* wU2DevExitLat (256 us) */
};

#ifdef DWC_OPTIONAL_UUID
	/* The Denali host (and Win8) chokes on the optional container ID */

static usb_dev_cap_container_id_t cap3_desc = {
	sizeof(usb_dev_cap_container_id_t),	/* bLength */
	UDESC_DEVICE_CAPABILITY,		/* bDescriptorType */
	USB_DEVICE_CAPABILITY_CONTAINER_ID,	/* bDevCapabilityType */
	0,					/* bReserved */
	/* @todo Create UUID */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, /* containerID */
};
#endif

static wusb_bos_desc_t bos_desc = {
	sizeof(wusb_bos_desc_t),		/* bLength */
	WUDESC_BOS,				/* bDescriptorType */

#ifdef DWC_OPTIONAL_UUID
	UCONSTW(sizeof(wusb_bos_desc_t)		/* wTotalLength */
		+ sizeof(cap1_desc) + sizeof(cap2_desc) + sizeof(cap3_desc)),
	3,					/* bNumDeviceCaps */
#else
	UCONSTW(sizeof(wusb_bos_desc_t)		/* wTotalLength */
		+ sizeof(cap1_desc) + sizeof(cap2_desc)),
	2,					/* bNumDeviceCaps */
#endif
};

#endif /* !__linux__ || ... */

/**
 * This routine starts the data stage of a 3-stage control command.
 * pcd->ep0state must be set to EP0_OUT_DATA_PHASE or EP0_IN_DATA_PHASE, and
 * pcd->ep0->dwc_ep.is_in must be set to 0 or 1 before calling this routine.
 * For IN, the data to be sent must be placed in pcd->ep0_status_buf before
 * the call.
 */
void dwc_usb3_pcd_ep0_data_stage(dwc_usb3_pcd_t *pcd, int length)
{
	pcd->ep0_req->dwc_req.buf[0] = (char *)pcd->ep0_status_buf;
	pcd->ep0_req->dwc_req.bufdma[0] = pcd->ep0_status_buf_dma;
	pcd->ep0_req->dwc_req.length = length;
	pcd->ep0_req->dwc_req.actual = 0;
	pcd->ep0_status_pending = 1;
	pcd->ep0->dwc_ep.send_zlp = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);
}

/**
 * This routine processes the SET_ADDRESS Setup Commands.
 */
static void do_set_address(dwc_usb3_pcd_t *pcd)
{
	usb_device_request_t ctrl = pcd->ep0_setup_pkt->req;

	dwc_debug0(pcd->usb3_dev, "SET ADDRESS\n");

	if (ctrl.bmRequestType == UT_DEVICE) {
#ifdef DEBUG_EP0
		dwc_debug1(pcd->usb3_dev, "SET_ADDRESS %d\n",
			   UGETW(ctrl.wValue));
#endif
		dwc_usb3_set_address(pcd, UGETW(ctrl.wValue));
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		if (UGETW(ctrl.wValue))
			pcd->state = DWC_STATE_ADDRESSED;
		else
			pcd->state = DWC_STATE_DEFAULT;
	}
}

/**
 * This routine stalls EP0.
 */
static void ep0_do_stall(dwc_usb3_pcd_t *pcd, int err_val)
{
	usb_device_request_t ctrl = pcd->ep0_setup_pkt->req;
	dwc_usb3_pcd_ep_t *ep0 = pcd->ep0;

	dwc_print3(pcd->usb3_dev, "req %02x.%02x protocol STALL; err %d\n",
		   ctrl.bmRequestType, ctrl.bRequest, err_val);
	ep0->dwc_ep.is_in = 0;
	dwc_usb3_pcd_ep_set_stall(pcd, ep0);
	ep0->dwc_ep.stopped = 1;
	pcd->ep0state = EP0_IDLE;
	dwc_usb3_pcd_ep0_out_start(pcd);
}

/**
 * Clear the EP halt (STALL), and if there are pending requests start
 * the transfer.
 */
static void do_clear_halt(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_ep_t *ep)
{
	dwc_usb3_dev_ep_regs_t __iomem *ep_reg;

	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);

	if (ep->dwc_ep.stall_clear_flag == 0) {
		if (ep != pcd->ep0)
			dwc_usb3_pcd_ep_clear_stall(pcd, ep);

		if (ep->dwc_ep.stopped) {
			ep->dwc_ep.stopped = 0;

			/* If there is a request in the EP queue start it */
			if (ep != pcd->ep0 && ep->dwc_ep.is_in)
				dwc_usb3_gadget_start_next_request(pcd, ep);
		}
	} else {
		dwc_usb3_dis_usb2_suspend(pcd);

		/* Clear sequence number using DEPCFG */
		if (ep->dwc_ep.is_in) {
			ep_reg = ep->dwc_ep.in_ep_reg;
			dwc_usb3_dep_cfg(pcd, ep_reg, ep->dwc_ep.param0in,
					 ep->dwc_ep.param1in, 0);
		} else {
			ep_reg = ep->dwc_ep.out_ep_reg;
			dwc_usb3_dep_cfg(pcd, ep_reg, ep->dwc_ep.param0out,
					 ep->dwc_ep.param1out, 0);
		}

		dwc_usb3_ena_usb2_suspend(pcd);
	}

	/* Start Control Status Phase */
	pcd->ep0->dwc_ep.is_in = 1;
	pcd->ep0state = EP0_IN_WAIT_NRDY;
}

/**
 * This routine handles the Get Descriptor request for the BOS descriptor
 * and the OTG descriptor, and passes all other requests to the Gadget driver.
 */
static void do_get_descriptor(dwc_usb3_pcd_t *pcd)
{
	usb_device_request_t ctrl = pcd->ep0_setup_pkt->req;
	u8 desc_type = UGETW(ctrl.wValue) >> 8;
	int ret;

#if defined(CONFIG_USB_OTG_DWC) || !defined(__linux__) || !defined(DWC_BOS_IN_GADGET)
	u16 max = UGETW(ctrl.wLength);
	u8 *buf = pcd->ep0_status_buf;
	int length;
#endif

#ifdef DEBUG_EP0
	dwc_debug5(pcd->usb3_dev, "GET_DESCRIPTOR %02x.%02x v%04x i%04x l%04x\n",
		   ctrl.bmRequestType, ctrl.bRequest, UGETW(ctrl.wValue),
		   UGETW(ctrl.wIndex), UGETW(ctrl.wLength));
#endif

	switch (desc_type) {

#ifdef CONFIG_USB_OTG_DWC
	case UDESC_OTG:
		dwc_debug0(pcd->usb3_dev, "\nGET_DESCRIPTOR(OTG)\n\n");
		buf[0] = 5;
		buf[1] = UDESC_OTG;
		if (pcd->speed == USB_SPEED_SUPER)
			buf[2] = 0xd;
		else
			buf[2] = 0x7;

		buf[3] = 0;
		if (pcd->speed == USB_SPEED_SUPER)
			buf[4] = 0x3;
		else
			buf[4] = 0x2;

		length = 5;
		dwc_usb3_pcd_ep0_data_stage(pcd, length < max ? length : max);
		break;
#endif

	case UDESC_BOS:
		dwc_debug0(pcd->usb3_dev, "\nGET_DESCRIPTOR(BOS)\n\n");
		if (pcd->speed != USB_SPEED_SUPER &&
		    pcd->usb3_dev->core_params->nobos) {
			ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
			return;
		}

#if !defined(__linux__) || !defined(DWC_BOS_IN_GADGET)
		memcpy(buf, &bos_desc, sizeof(bos_desc));
		buf += sizeof(bos_desc);

		if (pcd->usb3_dev->core_params->besl) {
			u32 d = UGETDW(cap1_desc.bmAttributes);

			d |= USB_20_EXT_BESL | USB_20_EXT_BASELINE_BESL_VALID |
				USB_20_EXT_DEEP_BESL_VALID;
			d &= ~(USB_20_EXT_BASELINE_BESL_BITS |
			       USB_20_EXT_DEEP_BESL_BITS);
			d |= pcd->usb3_dev->core_params->baseline_besl <<
				USB_20_EXT_BASELINE_BESL_SHIFT;
			d |= pcd->usb3_dev->core_params->deep_besl <<
				USB_20_EXT_DEEP_BESL_SHIFT;
			USETDW(cap1_desc.bmAttributes, d);
		}

		if (pcd->usb3_dev->core_params->lpmctl == 0)
			USETDW(cap1_desc.bmAttributes, 0);

		memcpy(buf, &cap1_desc, sizeof(cap1_desc));
		buf += sizeof(cap1_desc);
		memcpy(buf, &cap2_desc, sizeof(cap2_desc));

#ifdef DWC_OPTIONAL_UUID
		buf += sizeof(cap2_desc);
		memcpy(buf, &cap3_desc, sizeof(cap3_desc));
#endif
		length = UGETW(bos_desc.wTotalLength);
		dwc_usb3_pcd_ep0_data_stage(pcd, length < max ? length : max);
		break;
#else
		/* FALL THROUGH */
#endif

	default:
		/* Call the Gadget driver's setup routine */
		ret = dwc_usb3_gadget_setup(pcd, &ctrl);
		if (ret < 0)
			ep0_do_stall(pcd, ret);
		break;
	}
}

/**
 * This routine processes the GET_STATUS Setup Commands.
 */
static void do_get_status(dwc_usb3_pcd_t *pcd)
{
	usb_device_request_t ctrl = pcd->ep0_setup_pkt->req;
	u8 *status = pcd->ep0_status_buf;
	dwc_usb3_pcd_ep_t *ep;
	int length;

#ifdef DEBUG_EP0
	dwc_debug5(pcd->usb3_dev, "GET_STATUS %02x.%02x v%04x i%04x l%04x\n",
		   ctrl.bmRequestType, ctrl.bRequest, UGETW(ctrl.wValue),
		   UGETW(ctrl.wIndex), UGETW(ctrl.wLength));
#endif

	if (UGETW(ctrl.wLength) != 2
#ifdef CONFIG_USB_OTG_DWC
	    && UGETW(ctrl.wIndex) != 0xf000
#endif
	) {
		ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
		return;
	}

	switch (UT_GET_RECIPIENT(ctrl.bmRequestType)) {
	case UT_DEVICE:
#ifdef CONFIG_USB_OTG_DWC
		/* HNP Polling */
		if (UGETW(ctrl.wIndex) == 0xf000)
			*status = pcd->wants_host ? 1 : 0;
		else
#endif
		{
			*status = 1; /* Self powered */

			if (pcd->speed == USB_SPEED_SUPER) {
				if (pcd->state == DWC_STATE_CONFIGURED) {
					if (dwc_usb3_u1_enabled(pcd))
						*status |= 1 << 2;

					if (dwc_usb3_u2_enabled(pcd))
						*status |= 1 << 3;

					*status |= pcd->ltm_enable << 4;
				}
			} else {
				*status |= pcd->remote_wakeup_enable << 1;
			}
		}

		dwc_debug1(pcd->usb3_dev, "GET_STATUS(Device)=%02x\n", *status);
		*(status + 1) = 0;
		break;

	case UT_INTERFACE:
		*status = 0;
		if (pcd->usb3_dev->core_params->wakeup)
			*status |= 1;
		*status |= pcd->remote_wakeup_enable << 1;
		dwc_debug2(pcd->usb3_dev, "GET_STATUS(Interface %d)=%02x\n",
			   UGETW(ctrl.wIndex), *status);
		*(status + 1) = 0;
		break;

	case UT_ENDPOINT:
		ep = dwc_usb3_pcd_get_ep_by_addr(pcd, UGETW(ctrl.wIndex));

		/* @todo check for EP stall */
		*status = ep->dwc_ep.stopped;
		dwc_debug2(pcd->usb3_dev, "GET_STATUS(Endpoint %d)=%02x\n",
			   UGETW(ctrl.wIndex), *status);
		*(status + 1) = 0;
		break;

	default:
		ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
		return;
	}

#ifdef CONFIG_USB_OTG_DWC
	if (UGETW(ctrl.wIndex) == 0xf000)
		length = 1;
	else
#endif
		length = 2;
	dwc_usb3_pcd_ep0_data_stage(pcd, length);
}

/**
 * This routine processes the SET_FEATURE Setup Commands.
 */
static void do_set_feature(dwc_usb3_pcd_t *pcd)
{
	usb_device_request_t ctrl = pcd->ep0_setup_pkt->req;
	dwc_usb3_pcd_ep_t *ep;
	int ret;

#ifdef DEBUG_EP0
	dwc_debug5(pcd->usb3_dev, "SET_FEATURE %02x.%02x v%04x i%04x l%04x\n",
		   ctrl.bmRequestType, ctrl.bRequest, UGETW(ctrl.wValue),
		   UGETW(ctrl.wIndex), UGETW(ctrl.wLength));
#endif

	switch (UT_GET_RECIPIENT(ctrl.bmRequestType)) {
	case UT_DEVICE:
		switch (UGETW(ctrl.wValue)) {
		case UF_DEVICE_REMOTE_WAKEUP:
			pcd->remote_wakeup_enable = 1;
			break;

		case UF_TEST_MODE:
			/* Setup the Test Mode tasklet to do the Test
			 * Packet generation after the SETUP Status
			 * phase has completed. */
			pcd->test_mode = UGETW(ctrl.wIndex) >> 8;
			dwc_usb3_task_schedule(&pcd->test_mode_tasklet);
			break;

		case UF_DEVICE_B_HNP_ENABLE:
			dwc_debug0(pcd->usb3_dev,
				   "SET_FEATURE: USB_DEVICE_B_HNP_ENABLE\n");
#ifdef CONFIG_USB_OTG_DWC
			if (pcd->wants_host) {
				pcd->b_hnp_enable = 0;
				pcd->wants_host = 0;
				dwc_usb3_start_hnp(pcd);
			} else {
				pcd->b_hnp_enable = 1;
			}
#endif
			break;

		case UOTG_NTF_HOST_REL:
			dwc_debug0(pcd->usb3_dev,
				   "SET_FEATURE: USB_NTF_HOST_REL\n");
#ifdef CONFIG_USB_OTG_DWC
			dwc_usb3_host_release(pcd);
#endif
			break;

		case UOTG_B3_RSP_ENABLE:
			dwc_debug0(pcd->usb3_dev,
				   "SET_FEATURE: USB_B3_RSP_ENABLE\n");
			break;

		case UF_DEVICE_A_HNP_SUPPORT:
			/* RH port supports HNP */
			dwc_debug0(pcd->usb3_dev,
				   "SET_FEATURE: USB_DEVICE_A_HNP_SUPPORT\n");
			break;

		case UF_DEVICE_A_ALT_HNP_SUPPORT:
			/* other RH port does */
			dwc_debug0(pcd->usb3_dev,
				   "SET_FEATURE: USB_DEVICE_A_ALT_HNP_SUPPORT\n");
			break;

		case UF_U1_ENABLE:
			dwc_debug0(pcd->usb3_dev, "SET_FEATURE: UF_U1_ENABLE\n");
			if (pcd->speed != USB_SPEED_SUPER ||
			    pcd->state != DWC_STATE_CONFIGURED) {
				ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
				return;
			}

			if (pcd->usb3_dev->core_params->pwrctl & 1)
				dwc_usb3_enable_u1(pcd);
			break;

		case UF_U2_ENABLE:
			dwc_debug0(pcd->usb3_dev, "SET_FEATURE: UF_U2_ENABLE\n");
			if (pcd->speed != USB_SPEED_SUPER ||
			    pcd->state != DWC_STATE_CONFIGURED) {
				ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
				return;
			}

			if (pcd->usb3_dev->core_params->pwrctl & 2)
				dwc_usb3_enable_u2(pcd);
			break;

		case UF_LTM_ENABLE:
			dwc_debug0(pcd->usb3_dev,
				   "SET_FEATURE: UF_LTM_ENABLE\n");
			if (pcd->speed != USB_SPEED_SUPER ||
			    pcd->state != DWC_STATE_CONFIGURED ||
			    UGETW(ctrl.wIndex) != 0) {
				ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
				return;
			}

			pcd->ltm_enable = 1;
			pcd->send_lpm = 1;
			break;

		default:
			ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
			return;
		}

		break;

	case UT_INTERFACE:
		/* if FUNCTION_SUSPEND ... */
		if (UGETW(ctrl.wValue) == 0) {
			/* if Function Remote Wake Enabled ... */
			if (UGETW(ctrl.wIndex) >> 8 & 2)
				pcd->remote_wakeup_enable = 1;
			else
				pcd->remote_wakeup_enable = 0;

			/* if Function Low Power Suspend ... */
			// TODO

			break;
		}

		ret = dwc_usb3_gadget_setup(pcd, &ctrl);
		if (ret < 0)
			ep0_do_stall(pcd, ret);
		return;

	case UT_ENDPOINT:
		ep = dwc_usb3_pcd_get_ep_by_addr(pcd, UGETW(ctrl.wIndex));
		if (UGETW(ctrl.wValue) != UF_ENDPOINT_HALT) {
			ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
			return;
		}

		ep->dwc_ep.stopped = 1;
		dwc_usb3_pcd_ep_set_stall(pcd, ep);
		break;
	}

	pcd->ep0->dwc_ep.is_in = 1;
	pcd->ep0state = EP0_IN_WAIT_NRDY;
}

/**
 * This routine processes the CLEAR_FEATURE Setup Commands.
 */
static void do_clear_feature(dwc_usb3_pcd_t *pcd)
{
	usb_device_request_t ctrl = pcd->ep0_setup_pkt->req;
	dwc_usb3_pcd_ep_t *ep;

#ifdef DEBUG_EP0
	dwc_debug5(pcd->usb3_dev, "CLEAR_FEATURE %02x.%02x v%04x i%04x l%04x\n",
		   ctrl.bmRequestType, ctrl.bRequest, UGETW(ctrl.wValue),
		   UGETW(ctrl.wIndex), UGETW(ctrl.wLength));
#endif

	switch (UT_GET_RECIPIENT(ctrl.bmRequestType)) {
	case UT_DEVICE:
		switch (UGETW(ctrl.wValue)) {
		case UF_DEVICE_REMOTE_WAKEUP:
			pcd->remote_wakeup_enable = 0;
			break;

		case UF_TEST_MODE:
			/* @todo Add CLEAR_FEATURE for TEST modes. */
			break;

		case UF_U1_ENABLE:
			dwc_debug0(pcd->usb3_dev,
				   "CLEAR_FEATURE: UF_U1_ENABLE\n");
			if (pcd->speed != USB_SPEED_SUPER ||
			    pcd->state != DWC_STATE_CONFIGURED) {
				ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
				return;
			}

			dwc_usb3_disable_u1(pcd);
			break;

		case UF_U2_ENABLE:
			dwc_debug0(pcd->usb3_dev,
				   "CLEAR_FEATURE: UF_U2_ENABLE\n");
			if (pcd->speed != USB_SPEED_SUPER ||
			    pcd->state != DWC_STATE_CONFIGURED) {
				ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
				return;
			}

			dwc_usb3_disable_u2(pcd);
			break;

		case UF_LTM_ENABLE:
			dwc_debug0(pcd->usb3_dev,
				   "CLEAR_FEATURE: UF_LTM_ENABLE\n");
			if (pcd->speed != USB_SPEED_SUPER ||
			    pcd->state != DWC_STATE_CONFIGURED ||
			    UGETW(ctrl.wIndex) != 0) {
				ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
				return;
			}

			pcd->ltm_enable = 0;
			pcd->send_lpm = 1;
			break;

		default:
			ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
			return;
		}

		break;

	case UT_INTERFACE:
		/* if FUNCTION_SUSPEND ... */
		if (UGETW(ctrl.wValue) == 0) {
			/* if Function Remote Wake Enabled ... */
			if (UGETW(ctrl.wIndex) >> 8 & 2) {
				pcd->remote_wakeup_enable = 0;
			}

			/* if Function Low Power Suspend ... */
			// TODO

			break;
		}

		ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
		return;

	case UT_ENDPOINT:
		ep = dwc_usb3_pcd_get_ep_by_addr(pcd, UGETW(ctrl.wIndex));
		if (UGETW(ctrl.wValue) != UF_ENDPOINT_HALT) {
			ep0_do_stall(pcd, -DWC_E_NOT_SUPPORTED);
			return;
		}

		do_clear_halt(pcd, ep);
		return;
	}

	pcd->ep0->dwc_ep.is_in = 1;
	pcd->ep0state = EP0_IN_WAIT_NRDY;
}

/**
 * This routine processes SETUP commands. The USB Command processing is
 * done in two places - the first being the PCD and the second being the
 * Gadget driver (for example, the File-Backed Storage Gadget driver).
 *
 * <table>
 * <tr><td> Command </td><td> Driver </td><td> Description </td></tr>
 *
 * <tr><td> GET_STATUS </td><td> PCD </td><td> Command is processed
 * as defined in chapter 9 of the USB 2.0 Specification. </td></tr>
 *
 * <tr><td> SET_FEATURE </td><td> PCD / Gadget driver </td><td> Device
 * and Endpoint requests are processed by the PCD. Interface requests
 * are passed to the Gadget driver. </td></tr>
 *
 * <tr><td> CLEAR_FEATURE </td><td> PCD </td><td> Device and Endpoint
 * requests are processed by the PCD. Interface requests are ignored.
 * The only Endpoint feature handled is ENDPOINT_HALT. </td></tr>
 *
 * <tr><td> SET_ADDRESS </td><td> PCD </td><td> Program the DCFG register
 * with device address received. </td></tr>
 *
 * <tr><td> GET_DESCRIPTOR </td><td> Gadget driver </td><td> Return the
 * requested descriptor. </td></tr>
 *
 * <tr><td> SET_DESCRIPTOR </td><td> Gadget driver </td><td> Optional -
 * not implemented by any of the existing Gadget drivers. </td></tr>
 *
 * <tr><td> GET_CONFIGURATION </td><td> Gadget driver </td><td> Return
 * the current configuration. </td></tr>
 *
 * <tr><td> SET_CONFIGURATION </td><td> Gadget driver </td><td> Disable
 * all EPs and enable EPs for new configuration. </td></tr>
 *
 * <tr><td> GET_INTERFACE </td><td> Gadget driver </td><td> Return the
 * current interface. </td></tr>
 *
 * <tr><td> SET_INTERFACE </td><td> Gadget driver </td><td> Disable all
 * EPs and enable EPs for new interface. </td></tr>
 * </table>
 *
 * When the SETUP Phase Done interrupt occurs, the generic SETUP commands
 * are processed by this routine. Calling the Gadget driver's
 * dwc_usb3_gadget_setup() routine from here processes the gadget-specific
 * SETUP commands.
 */
void dwc_usb3_pcd_do_setup(dwc_usb3_pcd_t *pcd)
{
	usb_device_request_t ctrl = pcd->ep0_setup_pkt->req;
	dwc_usb3_pcd_ep_t *ep0 = pcd->ep0;
	u16 wvalue, wlength;
	int ret;

	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd);
	wvalue = UGETW(ctrl.wValue);
	wlength = UGETW(ctrl.wLength);
#if 0
#ifdef DEBUG_EP0
	dwc_debug0(pcd->usb3_dev, "\n");
	dwc_print1(pcd->usb3_dev, "setup_pkt[0]=0x%08x\n",
		   pcd->ep0_setup_pkt->d32[0]);
	dwc_print1(pcd->usb3_dev, "setup_pkt[1]=0x%08x\n",
		   pcd->ep0_setup_pkt->d32[1]);
	dwc_print5(pcd->usb3_dev, "SETUP %02x.%02x v%04x i%04x l%04x\n",
		   ctrl.bmRequestType, ctrl.bRequest, wvalue,
		   UGETW(ctrl.wIndex), wlength);
	dwc_debug0(pcd->usb3_dev, "\n");
#endif
#endif
	/* Clean up the request queue */
	dwc_usb3_gadget_request_nuke(pcd, ep0);
	ep0->dwc_ep.stopped = 0;
	ep0->dwc_ep.three_stage = 1;

	if (ctrl.bmRequestType & UE_DIR_IN) {
		ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_DATA_PHASE;
	} else {
		ep0->dwc_ep.is_in = 0;
		pcd->ep0state = EP0_OUT_DATA_PHASE;
	}

	if (wlength == 0) {
		ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_GADGET;
		ep0->dwc_ep.three_stage = 0;
	}

	if ((UT_GET_TYPE(ctrl.bmRequestType)) != UT_STANDARD) {
		/* handle non-standard (class/vendor) requests
		 * in the gadget driver
		 */
		ret = dwc_usb3_gadget_setup(pcd, &ctrl);
		if (ret < 0)
			ep0_do_stall(pcd, ret);
		return;
	}

	/* @todo NGS: Handle bad setup packet? */

///////////////////////////////////////////
//// --- Standard Request handling --- ////

	switch (ctrl.bRequest) {
	case UR_GET_STATUS:
		do_get_status(pcd);
		break;

	case UR_CLEAR_FEATURE:
		do_clear_feature(pcd);
		break;

	case UR_SET_FEATURE:
		do_set_feature(pcd);
		break;

	case UR_SET_ADDRESS:
		do_set_address(pcd);
		break;

	case UR_SET_INTERFACE:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_INTERFACE\n");
#ifndef DWC_UTE
		dwc_usb3_clr_eps_enabled(pcd);

# ifdef DWC_STAR_9000463548_WORKAROUND
		pcd->configuring = 1;
# endif
#endif
		ret = dwc_usb3_gadget_setup(pcd, &ctrl);
		if (ret < 0) {
#ifndef DWC_UTE
# ifdef DWC_STAR_9000463548_WORKAROUND
			pcd->configuring = 0;
# endif
#endif
			ep0_do_stall(pcd, ret);
			return;
		}

		break;

	case UR_SET_CONFIG:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_CONFIGURATION\n");
#ifdef DWC_UTE
		if (wvalue != 0)
			dwc_usb3_ute_config(pcd->usb3_dev);
#endif
		dwc_usb3_clr_eps_enabled(pcd);

#ifdef DWC_STAR_9000463548_WORKAROUND
		pcd->configuring = 1;
#endif
		if (pcd->ltm_enable)
			pcd->send_lpm = 1;

		ret = dwc_usb3_gadget_setup(pcd, &ctrl);
		if (ret >= 0) {
			if (wvalue != 0)
				pcd->state = DWC_STATE_CONFIGURED;
			else
				pcd->state = DWC_STATE_ADDRESSED;
		} else {
			ep0_do_stall(pcd, ret);
#ifdef DWC_STAR_9000463548_WORKAROUND
			pcd->configuring = 0;
#endif
			return;
		}

		/* Must wait until SetConfig before accepting U1/U2 link
		 * control, otherwise we have problems with VIA hubs
		 */
		if (pcd->usb3_dev->core_params->pwrctl & 1)
			dwc_usb3_accept_u1(pcd);
		if (pcd->usb3_dev->core_params->pwrctl & 2)
			dwc_usb3_accept_u2(pcd);

		pcd->ltm_enable = 0;
		break;

	case UR_GET_DESCRIPTOR:
		do_get_descriptor(pcd);
		break;

	case UR_SET_SEL:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_SEL\n");

		/* For now this is a no-op */
		dwc_usb3_pcd_ep0_data_stage(pcd, DWC_STATUS_BUF_SIZE < wlength ?
					    DWC_STATUS_BUF_SIZE : wlength);
		break;

	case UR_SET_ISOC_DELAY:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_ISOC_DELAY\n");

		/* For now this is a no-op */
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		break;

	default:
		/* Call the Gadget driver's setup routine */
		ret = dwc_usb3_gadget_setup(pcd, &ctrl);
		if (ret < 0)
			ep0_do_stall(pcd, ret);
		break;
	}
}

/**
 * This routine starts the Zero-Length Packet for the IN status phase of a
 * control write transfer.
 */
static void setup_in_status_phase(dwc_usb3_pcd_t *pcd, void *buf,
				  dwc_dma_t dma)
{
	dwc_usb3_pcd_ep_t *ep0 = pcd->ep0;

	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);

	if (pcd->ep0state == EP0_STALL) {
		dwc_debug0(pcd->usb3_dev, "EP0 STALLED\n");
		return;
	}

	ep0->dwc_ep.is_in = 1;
	pcd->ep0state = EP0_IN_STATUS_PHASE;

	dwc_debug0(pcd->usb3_dev, "EP0 IN ZLP\n");

	pcd->ep0_req->dwc_req.buf[0] = buf;
	pcd->ep0_req->dwc_req.bufdma[0] = dma;
	pcd->ep0_req->dwc_req.length = 0;
	pcd->ep0_req->dwc_req.actual = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);
}

/**
 * This routine starts the Zero-Length Packet for the OUT status phase of a
 * control read transfer.
 */
static void setup_out_status_phase(dwc_usb3_pcd_t *pcd, void *buf,
				   dwc_dma_t dma)
{
	dwc_usb3_pcd_ep_t *ep0 = pcd->ep0;

	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);

	if (pcd->ep0state == EP0_STALL) {
		dwc_debug0(pcd->usb3_dev, "EP0 STALLED\n");
		return;
	}

	ep0->dwc_ep.is_in = 0;
	pcd->ep0state = EP0_OUT_STATUS_PHASE;

	dwc_debug0(pcd->usb3_dev, "EP0 OUT ZLP\n");

	pcd->ep0_req->dwc_req.buf[0] = buf;
	pcd->ep0_req->dwc_req.bufdma[0] = dma;
	pcd->ep0_req->dwc_req.length = 0;
	pcd->ep0_req->dwc_req.actual = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);
}

#ifdef DEBUG_EP0
/**
 * This routine prints the ep0 state for debug purposes.
 */
void dwc_usb3_print_ep0_state(dwc_usb3_pcd_t *pcd)
{
#ifdef DEBUG
	char *str;

	switch (pcd->ep0state) {
	case EP0_IDLE:
		str = "EP0_IDLE";
		break;
	case EP0_IN_DATA_PHASE:
		str = "EP0_IN_DATA_PHASE";
		break;
	case EP0_OUT_DATA_PHASE:
		str = "EP0_OUT_DATA_PHASE";
		break;
	case EP0_IN_WAIT_GADGET:
		str = "EP0_IN_WAIT_GADGET";
		break;
	case EP0_OUT_WAIT_GADGET:
		str = "EP0_OUT_WAIT_GADGET";
		break;
	case EP0_IN_WAIT_NRDY:
		str = "EP0_IN_WAIT_NRDY";
		break;
	case EP0_OUT_WAIT_NRDY:
		str = "EP0_OUT_WAIT_NRDY";
		break;
	case EP0_IN_STATUS_PHASE:
		str = "EP0_IN_STATUS_PHASE";
		break;
	case EP0_OUT_STATUS_PHASE:
		str = "EP0_OUT_STATUS_PHASE";
		break;
	case EP0_STALL:
		str = "EP0_STALL";
		break;
	default:
		str = "EP0_INVALID";
	}

	dwc_debug2(pcd->usb3_dev, "%s(%d)\n", str, pcd->ep0state);
#endif
}
#endif

/**
 * This routine completes the ep0 control transfer.
 */
static int ep0_complete_request(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_req_t *req,
				dwc_usb3_dma_desc_t *desc, int status)
{
	dwc_usb3_pcd_ep_t *ep = pcd->ep0;
	int is_last = 0;

	dwc_debug4(pcd->usb3_dev, "%s(%lx,%lx,%d)\n", __func__,
		   (unsigned long)req, (unsigned long)desc, status);

	if (pcd->ep0_status_pending && !req) {
		if (ep->dwc_ep.is_in) {
#ifdef DEBUG_EP0
			dwc_debug0(pcd->usb3_dev,
				   "Do setup OUT status phase\n");
#endif
			pcd->ep0->dwc_ep.is_in = 0;
			pcd->ep0state = EP0_OUT_WAIT_NRDY;
		} else {
#ifdef DEBUG_EP0
			dwc_debug0(pcd->usb3_dev, "Do setup IN status phase\n");
#endif
			pcd->ep0->dwc_ep.is_in = 1;
			pcd->ep0state = EP0_IN_WAIT_NRDY;
		}

		pcd->ep0_status_pending = 0;
		return 1;
	}

	if (!req)
		return 0;

	dwc_debug1(pcd->usb3_dev, "req=%lx\n", (unsigned long)req);

	if (pcd->ep0state == EP0_OUT_STATUS_PHASE ||
	    pcd->ep0state == EP0_IN_STATUS_PHASE) {
		is_last = 1;

	} else if (ep->dwc_ep.is_in) {
#ifdef DEBUG_EP0
		dwc_debug4(pcd->usb3_dev,
			   "IN len=%d actual=%d xfrcnt=%d trbrsp=0x%02x\n",
			   req->dwc_req.length, req->dwc_req.actual,
			   dwc_usb3_get_xfercnt(desc),
			   dwc_usb3_get_xfersts(desc));
#endif
		if (dwc_usb3_get_xfercnt(desc) == 0) {
			/* Is a Zero Len Packet needed? */
			if (req->dwc_req.flags & DWC_PCD_REQ_ZERO) {
#ifdef DEBUG_EP0
				dwc_debug0(pcd->usb3_dev, "Setup Rx ZLP\n");
#endif
				req->dwc_req.flags &= ~DWC_PCD_REQ_ZERO;
			}

			pcd->ep0->dwc_ep.is_in = 0;
			pcd->ep0state = EP0_OUT_WAIT_NRDY;
		}
	} else {
#ifdef DEBUG_EP0
		dwc_debug4(pcd->usb3_dev,
			   "OUT len=%d actual=%d xfrcnt=%d trbrsp=0x%02x\n",
			   req->dwc_req.length, req->dwc_req.actual,
			   dwc_usb3_get_xfercnt(desc),
			   dwc_usb3_get_xfersts(desc));
#endif
		/* Is a Zero Len Packet needed? */
		if (req->dwc_req.flags & DWC_PCD_REQ_ZERO) {
#ifdef DEBUG_EP0
			dwc_debug0(pcd->usb3_dev, "Setup Tx ZLP\n");
#endif
			req->dwc_req.flags &= ~DWC_PCD_REQ_ZERO;
		}

		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
	}

	/* Complete the request */
	if (is_last) {
		dwc_debug2(pcd->usb3_dev, "is_last len=%d actual=%d\n",
			   req->dwc_req.length, req->dwc_req.actual);
		dwc_usb3_pcd_request_done(pcd, ep, req, status);
		return 1;
	}

	return 0;
}

/**
 * This routine handles EP0 Control transfers.
 *
 * The state of the control tranfers are tracked in <code>ep0state</code>.
 */
static void dwc_usb3_handle_ep0(dwc_usb3_pcd_t *pcd, dwc_usb3_pcd_req_t *req,
				u32 event)
{
	dwc_usb3_pcd_ep_t *ep0 = pcd->ep0;
	dwc_usb3_dma_desc_t *desc;
	u32 byte_count, len;
	int status;

#ifdef DEBUG_EP0
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);
	dwc_usb3_print_ep0_state(pcd);
#endif
	dwc_debug0(pcd->usb3_dev, "HANDLE EP0\n");

	switch (pcd->ep0state) {
	case EP0_IN_DATA_PHASE:
		if (!req)
			req = pcd->ep0_req;
		desc = dwc_usb3_ep0_in_desc(pcd);
		dwc_debug1(pcd->usb3_dev, "req=%lx\n", (unsigned long)req);
#ifdef DEBUG_EP0
		dwc_debug5(pcd->usb3_dev,
			"DATA_IN EP%d-%s: type=%d mps=%d trb.status=0x%08x\n",
			ep0->dwc_ep.num, (ep0->dwc_ep.is_in ? "IN" : "OUT"),
			ep0->dwc_ep.type, ep0->dwc_ep.maxpacket, desc->status);
#endif
		if (dwc_usb3_is_hwo(desc)) {
			dwc_print3(pcd->usb3_dev,
				"### %s, EP%d-%s HWO bit set 1! ###\n",
				__func__, ep0->dwc_ep.num, ep0->dwc_ep.is_in ?
				"IN" : "OUT");
			goto out;
		}

		status = dwc_usb3_get_xfersts(desc);
		if (status & DWC_TRBRSP_SETUP_PEND) {
			/* Start of a new Control transfer */
			dwc_debug0(pcd->usb3_dev, "IN SETUP PENDING\n");
			desc->status = 0;
		}

		byte_count = req->dwc_req.length - dwc_usb3_get_xfercnt(desc);
		req->dwc_req.actual += byte_count;
		req->dwc_req.buf[0] += byte_count;
		req->dwc_req.bufdma[0] += byte_count;
		dwc_debug3(pcd->usb3_dev, "length=%d byte_count=%d actual=%d\n",
			req->dwc_req.length, byte_count, req->dwc_req.actual);

		if (req->dwc_req.actual < req->dwc_req.length) {
#ifdef DEBUG_EP0
			dwc_usb3_dump_dbgregs(pcd->usb3_dev);
#endif
			dwc_debug0(pcd->usb3_dev, "IN CONTINUE\n");
			//dwc_usb3_pcd_ep0_continue_transfer(pcd, req);
			//dwc_debug0(pcd->usb3_dev, "CONTINUE TRANSFER\n");
			dwc_debug0(pcd->usb3_dev, "Stall EP0\n");
			ep0->dwc_ep.is_in = 0;
			dwc_usb3_pcd_ep_set_stall(pcd, ep0);
			ep0->dwc_ep.stopped = 1;
			pcd->ep0state = EP0_IDLE;
			dwc_usb3_pcd_ep0_out_start(pcd);

		} else if (ep0->dwc_ep.send_zlp) {
			dwc_debug0(pcd->usb3_dev, "IN ZLP\n");
			dwc_usb3_pcd_ep0_continue_transfer(pcd, req);
			ep0->dwc_ep.send_zlp = 0;
			dwc_debug0(pcd->usb3_dev, "CONTINUE TRANSFER\n");

		} else {
			dwc_debug0(pcd->usb3_dev, "IN COMPLETE\n");
			/* This sets ep0state = EP0_IN/OUT_WAIT_NRDY */
			ep0_complete_request(pcd, req, desc, 0);
			dwc_debug0(pcd->usb3_dev, "COMPLETE TRANSFER\n");
		}

		break;

	case EP0_OUT_DATA_PHASE:
		if (!req)
			req = pcd->ep0_req;
		desc = dwc_usb3_ep0_out_desc(pcd);
		dwc_debug1(pcd->usb3_dev, "req=%lx\n", (unsigned long)req);
#ifdef DEBUG_EP0
		dwc_debug5(pcd->usb3_dev,
			"DATA_OUT EP%d-%s: type=%d mps=%d trb.status=0x%08x\n",
			ep0->dwc_ep.num, (ep0->dwc_ep.is_in ? "IN" : "OUT"),
			ep0->dwc_ep.type, ep0->dwc_ep.maxpacket, desc->status);
#endif
		if (dwc_usb3_is_hwo(desc)) {
			dwc_print3(pcd->usb3_dev,
				"### %s, EP%d-%s HWO bit set 2! ###\n",
				__func__, ep0->dwc_ep.num, ep0->dwc_ep.is_in ?
				"IN" : "OUT");
			goto out;
		}

		status = dwc_usb3_get_xfersts(desc);
		if (status & DWC_TRBRSP_SETUP_PEND) {
			/* Start of a new Control transfer */
			dwc_debug0(pcd->usb3_dev, "OUT SETUP PENDING\n");
		}

		len = (req->dwc_req.length + ep0->dwc_ep.maxpacket - 1) &
			~(ep0->dwc_ep.maxpacket - 1);
		byte_count = len - dwc_usb3_get_xfercnt(desc);
		req->dwc_req.actual += byte_count;
		req->dwc_req.buf[0] += byte_count;
		req->dwc_req.bufdma[0] += byte_count;
		dwc_debug3(pcd->usb3_dev, "length=%d byte_count=%d actual=%d\n",
			req->dwc_req.length, byte_count, req->dwc_req.actual);

		/*if (req->dwc_req.actual < req->dwc_req.length) {
			dwc_debug0(pcd->usb3_dev, "OUT CONTINUE\n");
			dwc_usb3_pcd_ep0_continue_transfer(pcd, req);
			dwc_debug0(pcd->usb3_dev, "CONTINUE TRANSFER\n");

		} else*/ if (ep0->dwc_ep.send_zlp) {
			dwc_debug0(pcd->usb3_dev, "OUT ZLP\n");
			dwc_usb3_pcd_ep0_continue_transfer(pcd, req);
			ep0->dwc_ep.send_zlp = 0;
			dwc_debug0(pcd->usb3_dev, "CONTINUE TRANSFER\n");

		} else {
			dwc_debug0(pcd->usb3_dev, "OUT COMPLETE\n");
			/* This sets ep0state = EP0_IN/OUT_WAIT_NRDY */
			ep0_complete_request(pcd, req, desc, 0);
			dwc_debug0(pcd->usb3_dev, "COMPLETE TRANSFER\n");
		}

		break;

	case EP0_IN_WAIT_GADGET:
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		break;

	case EP0_OUT_WAIT_GADGET:
		pcd->ep0state = EP0_OUT_WAIT_NRDY;
		break;

	case EP0_IN_WAIT_NRDY:
	case EP0_OUT_WAIT_NRDY:
		if (ep0->dwc_ep.is_in)
			setup_in_status_phase(pcd, dwc_usb3_ep0_setup_pkt(pcd),
					      dwc_usb3_ep0_setup_pkt_dma(pcd));
		else
			setup_out_status_phase(pcd, dwc_usb3_ep0_setup_pkt(pcd),
					       dwc_usb3_ep0_setup_pkt_dma(pcd));
		break;

	case EP0_IN_STATUS_PHASE:
	case EP0_OUT_STATUS_PHASE:
		if (ep0->dwc_ep.is_in)
			desc = dwc_usb3_ep0_in_desc(pcd);
		else
			desc = dwc_usb3_ep0_out_desc(pcd);
#ifdef DEBUG_EP0
		dwc_debug2(pcd->usb3_dev, "STATUS EP%d-%s\n", ep0->dwc_ep.num,
			   (ep0->dwc_ep.is_in ? "IN" : "OUT"));
#endif
		ep0_complete_request(pcd, req, desc, 0);
		pcd->ep0state = EP0_IDLE;
		ep0->dwc_ep.stopped = 1;
		ep0->dwc_ep.is_in = 0;	/* OUT for next SETUP */

		if (pcd->send_lpm) {
			pcd->send_lpm = 0;

#if 0	// This is only for testing
			dwc_usb3_xmit_ltm(pcd,
					32 << DWC_DGCMDPAR_BELT_VALUE_SHIFT |
					DWC_LATENCY_VALUE_MULT_32768 <<
					DWC_DGCMDPAR_BELT_SCALE_SHIFT);
#endif
		}

		/* Prepare for more SETUP Packets */
		dwc_usb3_pcd_ep0_out_start(pcd);
		break;

	case EP0_STALL:
		dwc_error0(pcd->usb3_dev, "EP0 STALLed, should not get here\n");
		break;

	case EP0_IDLE:
		dwc_error0(pcd->usb3_dev, "EP0 IDLE, should not get here\n");
		break;
	}
out:
#ifdef DEBUG_EP0
	dwc_usb3_print_ep0_state(pcd);
#endif
	return;
}

/**
 * This routine handles EP0 transfers.
 *
 * This routine gets the request corresponding to the current EP0 transfer. If
 * EP0 is in IDLE state, it calls dwc_usb3_do_setup() to begin processing
 * the next Setup request, otherwise it calls dwc_usb3_handle_ep0() to handle
 * the next stage of the current transfer.
 */
void dwc_usb3_handle_ep0_xfer(dwc_usb3_pcd_t *pcd, u32 event)
{
	dwc_usb3_pcd_ep_t *ep0 = pcd->ep0;
	dwc_usb3_pcd_req_t *req = NULL;

#ifdef DEBUG_EP0
	dwc_debug1(pcd->usb3_dev, "%s()\n", __func__);
#endif
	req = dwc_usb3_gadget_get_request(pcd, ep0);

	if (pcd->ep0state == EP0_IDLE) {
#ifdef DEBUG_EP0
		dwc_usb3_print_ep0_state(pcd);
		dwc_debug2(pcd->usb3_dev, "IDLE EP%d-%s\n", ep0->dwc_ep.num,
			   (ep0->dwc_ep.is_in ? "IN" : "OUT"));
#endif
		pcd->request_config = 0;
		dwc_usb3_gadget_do_setup(pcd);
	} else {
		dwc_usb3_handle_ep0(pcd, req, event);
	}
}
