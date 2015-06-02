/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 2010 Hisilicon
 *
 * This file is licenced under the GPL.
 */

#include <linux/platform_device.h>
#include <linux/signal.h>
#include "hiusb.h"

static int __devinit hiusb_ohci_start(struct usb_hcd *hcd)
{
	struct ohci_hcd	*ohci = hcd_to_ohci(hcd);
	int ret;

	ohci_dbg(ohci, "hiusb_ohci_start, ohci:%p", ohci);
	ret = ohci_init(ohci);
	if (ret < 0)
		return ret;
	ret = ohci_run(ohci);
	if (ret < 0) {
		err("can't start %s", hcd->self.bus_name);
		ohci_stop(hcd);
		return ret;
	}

	return 0;
}

static const struct hc_driver hiusb_ohci_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"HIUSB OHCI",
	.hcd_priv_size =	sizeof(struct ohci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq =			ohci_irq,
	.flags =		HCD_USB11 | HCD_MEMORY,

	/*
	 * basic lifecycle operations
	 */
	.start =		hiusb_ohci_start,
	.stop =			ohci_stop,
	.shutdown =		ohci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		ohci_urb_enqueue,
	.urb_dequeue =		ohci_urb_dequeue,
	.endpoint_disable =	ohci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number =	ohci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data =	ohci_hub_status_data,
	.hub_control =		ohci_hub_control,
#ifdef	CONFIG_PM
	.bus_suspend =		ohci_bus_suspend,
	.bus_resume =		ohci_bus_resume,
#endif
	.start_port_reset =	ohci_start_port_reset,
};

static int hiusb_ohci_hcd_drv_probe(struct platform_device *pdev)
{
	int ret;
	struct usb_hcd *hcd;

	if (usb_disabled())
		return -ENODEV;

	if (pdev->resource[1].flags != IORESOURCE_IRQ) {
		pr_debug("resource[1] is not IORESOURCE_IRQ\n");
		return -ENOMEM;
	}

	hcd = usb_create_hcd(&hiusb_ohci_hc_driver, &pdev->dev, "hiusb-ohci");
	if (!hcd)
		return -ENOMEM;

	hcd->rsrc_start = pdev->resource[0].start;
	hcd->rsrc_len = pdev->resource[0].end - pdev->resource[0].start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		pr_debug("request_mem_region failed\n");
		ret = -EBUSY;
		goto err1;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed\n");
		ret = -ENOMEM;
		goto err2;
	}

	hiusb_start_hcd();
	ohci_hcd_init(hcd_to_ohci(hcd));

	ret = usb_add_hcd(hcd, pdev->resource[1].start,
			  IRQF_DISABLED | IRQF_SHARED);
	if (ret == 0) {
		platform_set_drvdata(pdev, hcd);
		return ret;
	}

	hiusb_stop_hcd();
	iounmap(hcd->regs);
err2:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err1:
	usb_put_hcd(hcd);
	return ret;
}

static int hiusb_ohci_hcd_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_remove_hcd(hcd);
	hiusb_stop_hcd();
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static void hiusb_ohci_shutdown(struct platform_device *pdev)
{

	hiusb_start_hcd();

	usb_hcd_platform_shutdown(pdev);

	hiusb_stop_hcd();
}

#ifdef CONFIG_PM
static int hiusb_ohci_hcd_drv_suspend(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ohci_hcd	*ohci = hcd_to_ohci(hcd);
	unsigned long flags;
	int rc;

	rc = 0;

	/* Root hub was already suspended. Disable irq emission and
	 * mark HW unaccessible, bail out if RH has been resumed. Use
	 * the spinlock to properly synchronize with possible pending
	 * RH suspend or resume activity.
	 *
	 * This is still racy as hcd->state is manipulated outside of
	 * any locks =P But that will be a different fix.
	 */
	spin_lock_irqsave(&ohci->lock, flags);
	if (hcd->state != HC_STATE_SUSPENDED) {
		rc = -EINVAL;
		goto bail;
	}
	ohci_writel(ohci, OHCI_INTR_MIE, &ohci->regs->intrdisable);
	(void)ohci_readl(ohci, &ohci->regs->intrdisable);

	clear_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	hiusb_stop_hcd();
bail:
	spin_unlock_irqrestore(&ohci->lock, flags);

	return rc;
}

static int hiusb_ohci_hcd_drv_resume(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);

	hiusb_start_hcd();

	set_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);
	ohci_finish_controller_resume(hcd);

	return 0;
}

static const struct dev_pm_ops hiusb_ohci_pmops = {
	.suspend	= hiusb_ohci_hcd_drv_suspend,
	.resume		= hiusb_ohci_hcd_drv_resume,
};

#define HIUSB_OHCI_PMOPS (&hiusb_ohci_pmops)

#else
#define HIUSB_OHCI_PMOPS NULL
#endif

static struct platform_driver hiusb_ohci_hcd_driver = {
	.probe		= hiusb_ohci_hcd_drv_probe,
	.remove		= hiusb_ohci_hcd_drv_remove,
	.shutdown	= hiusb_ohci_shutdown,
	.driver		= {
		.name	= "hiusb-ohci",
		.owner	= THIS_MODULE,
		.pm	= HIUSB_OHCI_PMOPS,
	},
};

MODULE_ALIAS("platform:hiusb-ohci");

/*****************************************************************************/

static struct resource hiusb_ohci_res[] = {
	[0] = {
		.start = CONFIG_HIUSB_OHCI_IOBASE,
		.end   = CONFIG_HIUSB_OHCI_IOBASE +
				 CONFIG_HIUSB_OHCI_IOSIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = CONFIG_HIUSB_OHCI_IRQNUM,
		.end   = CONFIG_HIUSB_OHCI_IRQNUM,
		.flags = IORESOURCE_IRQ,
	},
};

static void usb_ohci_platdev_release(struct device *dev)
{
		/* These don't need to do anything because the pdev
		 structures are statically allocated. */
}

static u64 usb_dmamask = DMA_BIT_MASK(32);

static struct platform_device hiusb_ohci_platdev = {
	.name   = "hiusb-ohci",
	.id     = 0,
	.dev    = {
		.platform_data  = NULL,
		.dma_mask = &usb_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.release = usb_ohci_platdev_release,
	},
	.num_resources = ARRAY_SIZE(hiusb_ohci_res),
	.resource       = hiusb_ohci_res,
};
