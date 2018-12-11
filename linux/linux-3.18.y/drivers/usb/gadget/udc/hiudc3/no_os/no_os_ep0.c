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

#ifdef DWC_MASS_STORAGE_GADGET
# define DWC_VENDOR_ID	0x16c3
# define DWC_PRODUCT_ID	0x1234
#else
/* We simulate the Linux gadget-zero device */
//# define DWC_VENDOR_ID	0x0525
//# define DWC_PRODUCT_ID	0xa4a0
# define DWC_VENDOR_ID	0x1e08
# define DWC_PRODUCT_ID	0x0001
#endif

/* Make the following structure type definitions "packed" if using a Microsoft
 * compiler. The UPACKED attribute (defined in no_os_defs.h) embedded in the
 * structure type definitions does the same thing for GCC. Other compilers may
 * need something different.
 */
#ifdef _MSC_VER
#include <pshpack1.h>
#endif

/* The language string always has string index 0 */
static struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		wString[1];
} UPACKED language_string = {
	4,				/* bLength (size of string array + 2) */
	UDESC_STRING,			/* bDescriptorType */
	{				/* wString[] */
		UCONSTW(0x0409),	/* US English */
		/* others can be added here */
	},
};

static struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		wString[8];
} UPACKED manuf_string = {
	18,				/* bLength (size of string array + 2) */
	UDESC_STRING,			/* bDescriptorType */
	{				/* wString[] */
		UCONSTW('S'), UCONSTW('y'), UCONSTW('n'), UCONSTW('o'),
		UCONSTW('p'), UCONSTW('s'), UCONSTW('y'), UCONSTW('s'),
	},
};
#define DWC_STRING_MANUFACTURER	1

static struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		wString[8];
} UPACKED product_string = {
	18,				/* bLength (size of string array + 2) */
	UDESC_STRING,			/* bDescriptorType */
	{				/* wString[] */
		UCONSTW('D'), UCONSTW('W'), UCONSTW('C'), UCONSTW(' '),
		UCONSTW('U'), UCONSTW('S'), UCONSTW('B'), UCONSTW('3'),
	},
};
#define DWC_STRING_PRODUCT	2

static struct {
	uByte		bLength;
	uByte		bDescriptorType;
	uWord		wString[10];
} UPACKED serial_string = {
	22,				/* bLength (size of string array + 2) */
	UDESC_STRING,			/* bDescriptorType */
	{				/* wString[] */
		UCONSTW('0'), UCONSTW('1'), UCONSTW('2'), UCONSTW('3'),
		UCONSTW('4'), UCONSTW('5'), UCONSTW('6'), UCONSTW('7'),
		UCONSTW('8'), UCONSTW('9'),
	},
};
#define DWC_STRING_SERIAL	3

/* Stop packing structure type definitions */
#ifdef _MSC_VER
#include <poppack.h>
#endif

/* These standard USB descriptor types are defined in usb.h */

static usb_device_descriptor_t device_desc = {
	USB_DEVICE_DESCRIPTOR_SIZE,		/* bLength */
	UDESC_DEVICE,				/* bDescriptorType */

	UCONSTW(0),				/* bcdUSB (filled in later) */

#ifdef DWC_MASS_STORAGE_GADGET
	UDCLASS_IN_INTERFACE,			/* bDeviceClass */
#else
	UDCLASS_VENDOR,				/* bDeviceClass */
#endif
	0,					/* bDeviceSubClass */
	0,					/* bDeviceProtocol */
	0,					/* bMaxPacketSize */

	UCONSTW(DWC_VENDOR_ID),			/* idVendor */
	UCONSTW(DWC_PRODUCT_ID),		/* idProduct */
	UCONSTW(0xffff),			/* bcdDevice */

	DWC_STRING_MANUFACTURER,		/* iManufacturer */
	DWC_STRING_PRODUCT,			/* iProduct */
	DWC_STRING_SERIAL,			/* iSerialNumber */

	1,					/* bNumConfigurations */
};

static usb_device_qualifier_t dev_qualifier = {
	USB_DEVICE_QUALIFIER_SIZE,		/* bLength */
	UDESC_DEVICE_QUALIFIER,			/* bDescriptorType */

	UCONSTW(0),				/* bcdUSB (filled in later) */

#ifdef DWC_MASS_STORAGE_GADGET
	UDCLASS_IN_INTERFACE,			/* bDeviceClass */
#else
	UDCLASS_VENDOR,				/* bDeviceClass */
#endif
	0,					/* bDeviceSubClass */
	0,					/* bDeviceProtocol */
	0,					/* bMaxPacketSize0 */
	1,					/* bNumConfigurations */
	0,					/* bReserved */
};

/* These application-specific config descriptor types are defined in
 * no_os_defs.h
 */

fs_config_desc_t fs_config_desc = {
	/* config descriptor */
	{
		USB_CONFIG_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_CONFIG,			/* bDescriptorType */
		UCONSTW(0),			/* wTotalLength (filled in later) */
		1,				/* bNumInterface */
		1,				/* bConfigurationValue */
		0,				/* iConfiguration */
		UC_ATT_ONE | UC_ATT_SELFPOWER,	/* bmAttributes */
		100 / UC_POWER_FACTOR,		/* bMaxPower (100 ma) */
	},
	/* interface descriptor */
	{
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		0,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		2,				/* bNumEndpoints */

#ifdef DWC_MASS_STORAGE_GADGET
		UICLASS_MASS,			/* bInterfaceClass */
		UISUBCLASS_SCSI,		/* bInterfaceSubClass */
		UIPROTO_MASS_BBB,		/* bInterfaceProtocol */
#else
		UICLASS_VENDOR,			/* bInterfaceClass */
		0,				/* bInterfaceSubClass */
		0,				/* bInterfaceProtocol */
#endif
		0,				/* iInterface */
	},
	/* bulk IN endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_IN,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(64),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk OUT endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		2 | UE_DIR_OUT,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(64),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
};

hs_config_desc_t hs_config_desc = {
	/* config descriptor */
	{
		USB_CONFIG_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_CONFIG,			/* bDescriptorType */
		UCONSTW(0),			/* wTotalLength (filled in later) */
		1,				/* bNumInterface */
		1,				/* bConfigurationValue */
		0,				/* iConfiguration */
		UC_ATT_ONE | UC_ATT_SELFPOWER,	/* bmAttributes */
		100 / UC_POWER_FACTOR,		/* bMaxPower (100 ma) */
	},
	/* interface descriptor */
	{
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		0,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		2,				/* bNumEndpoints */

#ifdef DWC_MASS_STORAGE_GADGET
		UICLASS_MASS,			/* bInterfaceClass */
		UISUBCLASS_SCSI,		/* bInterfaceSubClass */
		UIPROTO_MASS_BBB,		/* bInterfaceProtocol */
#else
		UICLASS_VENDOR,			/* bInterfaceClass */
		0,				/* bInterfaceSubClass */
		0,				/* bInterfaceProtocol */
#endif
		0,				/* iInterface */
	},
	/* bulk IN endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_IN,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(512),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk OUT endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		2 | UE_DIR_OUT,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(512),			/* wMaxPacketSize */
		1,				/* bInterval (every uframe) */
	},
};

ss_config_desc_t ss_config_desc = {
	/* config descriptor */
	{
		USB_CONFIG_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_CONFIG,			/* bDescriptorType */
		UCONSTW(0),			/* wTotalLength (filled in later) */
		1,				/* bNumInterface */
		1,				/* bConfigurationValue */
		0,				/* iConfiguration */
		UC_ATT_ONE | UC_ATT_SELFPOWER,	/* bmAttributes */
		100 / UC_POWER_FACTOR,		/* bMaxPower (100 ma) */
	},
	/* interface descriptor */
	{
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		0,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		2,				/* bNumEndpoints */

#ifdef DWC_MASS_STORAGE_GADGET
		UICLASS_MASS,			/* bInterfaceClass */
		UISUBCLASS_SCSI,		/* bInterfaceSubClass */
		UIPROTO_MASS_BBB,		/* bInterfaceProtocol */
#else
		UICLASS_VENDOR,			/* bInterfaceClass */
		0,				/* bInterfaceSubClass */
		0,				/* bInterfaceProtocol */
#endif
		0,				/* iInterface */
	},
	/* bulk IN endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_IN,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(1024),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk IN companion descriptor */
	{
		USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE, /* bLength */
		UDESC_SS_USB_COMPANION,		/* bDescriptorType */
		15,				/* bMaxBurst */
		0,				/* bmAttributes */
		UCONSTW(0),			/* wBytesPerInterval */
	},
	/* bulk OUT endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		2 | UE_DIR_OUT,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(1024),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk OUT companion descriptor */
	{
		USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE, /* bLength */
		UDESC_SS_USB_COMPANION,		/* bDescriptorType */
		15,				/* bMaxBurst */
		0,				/* bmAttributes */
		UCONSTW(0),			/* wBytesPerInterval */
	},
};

/**
 * This routine handles Get Descriptor requests.
 */
static int no_os_get_descriptor(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	u8 desc_type = UGETW(ctrl->wValue) >> 8;
	u8 desc_idx = UGETW(ctrl->wValue);
	u16 max = UGETW(ctrl->wLength);
	u8 *buf = pcd->ep0_status_buf;
	u16 len;

#ifdef DEBUG_EP0
	dwc_debug5(pcd->usb3_dev, "GET_DESCRIPTOR %02x.%02x v%04x i%04x l%04x\n",
		   ctrl->bmRequestType, ctrl->bRequest, UGETW(ctrl->wValue),
		   UGETW(ctrl->wIndex), UGETW(ctrl->wLength));
#endif
	switch (desc_type) {
	case UDESC_DEVICE:
		if (desc_idx)
			return -DWC_E_NOT_SUPPORTED;

		switch (pcd->speed) {
		case USB_SPEED_SUPER:
			USETW(device_desc.bcdUSB, 0x300);
			device_desc.bMaxPacketSize = 9;
			break;

		case USB_SPEED_HIGH:
			USETW(device_desc.bcdUSB, 0x200);
			device_desc.bMaxPacketSize = 64;
			break;

		default:
			USETW(device_desc.bcdUSB, 0x200);
			device_desc.bMaxPacketSize = 64;
			break;
		}

		memcpy(buf, &device_desc, sizeof(device_desc));
		len = sizeof(device_desc);
		break;

	case UDESC_CONFIG:
		if (desc_idx)
			return -DWC_E_NOT_SUPPORTED;

		switch (pcd->speed) {
		case USB_SPEED_SUPER:
			USETW(ss_config_desc.config_desc.wTotalLength,
			      sizeof(ss_config_desc));
			memcpy(buf, &ss_config_desc, sizeof(ss_config_desc));
			len = sizeof(ss_config_desc);
			break;

		case USB_SPEED_HIGH:
			USETW(hs_config_desc.config_desc.wTotalLength,
			      sizeof(hs_config_desc));
			memcpy(buf, &hs_config_desc, sizeof(hs_config_desc));
			len = sizeof(hs_config_desc);
			break;

		default:
			USETW(fs_config_desc.config_desc.wTotalLength,
			      sizeof(fs_config_desc));
			memcpy(buf, &fs_config_desc, sizeof(fs_config_desc));
			len = sizeof(fs_config_desc);
			break;
		}

		break;

	case UDESC_STRING:
		switch (desc_idx) {
		case 0:
			memcpy(buf, &language_string, sizeof(language_string));
			len = sizeof(language_string);
			break;

		case DWC_STRING_MANUFACTURER:
			memcpy(buf, &manuf_string, sizeof(manuf_string));
			len = sizeof(manuf_string);
			break;

		case DWC_STRING_PRODUCT:
			memcpy(buf, &product_string, sizeof(product_string));
			len = sizeof(product_string);
			break;

		case DWC_STRING_SERIAL:
			memcpy(buf, &serial_string, sizeof(serial_string));
			len = sizeof(serial_string);
			break;

		default:
			return -DWC_E_NOT_SUPPORTED;
		}

		break;

	case UDESC_DEVICE_QUALIFIER:
		if (desc_idx || pcd->speed == USB_SPEED_SUPER)
			return -DWC_E_NOT_SUPPORTED;

		USETW(dev_qualifier.bcdUSB, 0x200);
		dev_qualifier.bMaxPacketSize0 = 9;
		memcpy(buf, &dev_qualifier, sizeof(dev_qualifier));
		len = sizeof(dev_qualifier);
		break;

	case UDESC_OTHER_SPEED_CONFIGURATION:
		if (desc_idx || pcd->speed == USB_SPEED_SUPER)
			return -DWC_E_NOT_SUPPORTED;

		switch (pcd->speed) {
		case USB_SPEED_HIGH:
			ss_config_desc.config_desc.bDescriptorType =
					UDESC_OTHER_SPEED_CONFIGURATION;
			USETW(ss_config_desc.config_desc.wTotalLength,
			      sizeof(ss_config_desc));
			memcpy(buf, &ss_config_desc, sizeof(ss_config_desc));
			ss_config_desc.config_desc.bDescriptorType = UDESC_CONFIG;
			len = sizeof(ss_config_desc);
			break;

		default:
			hs_config_desc.config_desc.bDescriptorType =
					UDESC_OTHER_SPEED_CONFIGURATION;
			USETW(hs_config_desc.config_desc.wTotalLength,
			      sizeof(hs_config_desc));
			memcpy(buf, &hs_config_desc, sizeof(hs_config_desc));
			hs_config_desc.config_desc.bDescriptorType = UDESC_CONFIG;
			len = sizeof(hs_config_desc);
			break;
		}

		break;

	default:
		return -DWC_E_NOT_SUPPORTED;
	}

	pcd->ep0state = EP0_IN_DATA_PHASE;
	pcd->ep0_req->dwc_req.length = len < max ? len : max;
	pcd->ep0_status_pending = 1;
	pcd->ep0_req->dwc_req.buf[0] = pcd->ep0_status_buf;
	pcd->ep0_req->dwc_req.bufdma[0] = pcd->ep0_status_buf_dma;
	pcd->ep0_req->dwc_req.actual = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);

	return 0;
}

/**
 * This routine processes SETUP commands. The USB Command processing is
 * done in two places - the first being the PCD and the second being the
 * Gadget driver (for example, the File-Backed Storage Gadget driver).
 *
 * <table>
 * <tr><td> Command </td><td> Driver </td><td> Description </td></tr>
 *
 * <tr><td> SET_FEATURE </td><td> PCD / Gadget driver </td><td> Device
 * and Endpoint requests are processed by the PCD. Interface requests
 * are passed to the Gadget driver. </td></tr>
 *
 * <tr><td> CLEAR_FEATURE </td><td> PCD </td><td> Device and Endpoint
 * requests are processed by the PCD. Interface requests are ignored.
 * The only Endpoint feature handled is ENDPOINT_HALT. </td></tr>
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
 * are processed by dwc_usb3_do_setup(). Calling the gadget driver's
 * dwc_usb3_gadget_setup() routine from dwc_usb3_do_setup() in turn calls
 * this routine to process the gadget-specific SETUP commands. Any requests
 * not handled here are passed to the Function Driver's
 * dwc_usb3_function_setup() routine.
 */
int dwc_usb3_no_os_setup(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd);

	if ((UT_GET_TYPE(ctrl->bmRequestType)) != UT_STANDARD) {
		/* handle non-standard (class/vendor) requests
		 * in the Function Driver
		 */
		return dwc_usb3_function_setup(pcd, ctrl);
	}

	/* @todo NGS: Handle bad setup packet? */

///////////////////////////////////////////
//// --- Standard Request handling --- ////

	switch (ctrl->bRequest) {
	case UR_SET_FEATURE:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_SET_INTERFACE:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_GET_INTERFACE:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_SET_CONFIG:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_GET_CONFIG:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_SYNCH_FRAME:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_GET_DESCRIPTOR:
		return no_os_get_descriptor(pcd, ctrl);

	default:
		/* Call the Function Driver setup routines */
		return dwc_usb3_function_setup(pcd, ctrl);
	}

	return 0;
}
