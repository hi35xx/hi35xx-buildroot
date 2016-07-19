/*
 * EHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 2010 Hisilicon
 *
 * This file is licenced under the GPL.
 */

#include <linux/platform_device.h>
#include "hiusbv100.h"

static int hiusb_ehci_setup(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int ret = ehci_init(hcd);

	/*
	 * usb2.0 host ip quirk(maybe lost IOC interrupt), so
	 * driver need_io_watchdog to avoid it.
	 */
	ehci->need_io_watchdog = 1;
	ehci->has_synopsys_hc_bug = 1;
	return ret;
}

static void hi_reset_ehci(struct usb_hcd *hcd)
{
#ifdef CONFIG_ARCH_HI3516A
	int reg1, reg2;
	reg1 = readl(IO_ADDRESS(0x100b0024));
	mdelay(1);
	reg2 = readl(IO_ADDRESS(0x100b0028));
	mdelay(1);
	writel(0x37, IO_ADDRESS(0x200300b8));
	mdelay(100);
	writel(0x80, IO_ADDRESS(0x200300b8));
	mdelay(1);
	writel(0xc06, IO_ADDRESS(0x20120080));
	mdelay(1);
	writel(0xc26, IO_ADDRESS(0x20120080));
	mdelay(1);
	writel(reg1, IO_ADDRESS(0x100b0024));
	mdelay(1);
	writel(reg2, IO_ADDRESS(0x100b0028));
	mdelay(1);
	writel(0x10005, IO_ADDRESS(0x100b0010));
	mdelay(1);
	writel(0x1, IO_ADDRESS(0x100b0050));
	mdelay(1);
	writel(0x37, IO_ADDRESS(0x100b0018));
	mdelay(1);
	writel(0x1000, IO_ADDRESS(0x100b0054));
	mdelay(1);
#endif
}
static const struct hc_driver hiusb_ehci_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "HIUSB EHCI",
	.hcd_priv_size		= sizeof(struct ehci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 *
	 * FIXME -- ehci_init() doesn't do enough here.
	 * See ehci-ppc-soc for a complete implementation.
	 */
	.reset			= hiusb_ehci_setup,
	.hi_ehci_reset		= hi_reset_ehci,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.endpoint_reset		= ehci_endpoint_reset,

	/*
	 * scheduling support
	 */
	.get_frame_number	= ehci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
	.bus_suspend		= ehci_bus_suspend,
	.bus_resume		= ehci_bus_resume,
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,

	.clear_tt_buffer_complete	= ehci_clear_tt_buffer_complete,
};

static int hiusb_ehci_hcd_drv_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	struct resource *res;
	int ret;

	if (usb_disabled())
		return -ENODEV;

	if (pdev->resource[1].flags != IORESOURCE_IRQ) {
		pr_debug("resource[1] is not IORESOURCE_IRQ");
		return -ENOMEM;
	}
	hcd = usb_create_hcd(&hiusb_ehci_hc_driver, &pdev->dev, "hiusb-ehci");
	if (!hcd)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		pr_debug("request_mem_region failed");
		ret = -EBUSY;
		goto err1;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed");
		ret = -ENOMEM;
		goto err2;
	}

	hiusb_start_hcd();

	ehci = hcd_to_ehci(hcd);
	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs
		+ HC_LENGTH(ehci, readl(&ehci->caps->hc_capbase));
	/* cache this readonly data; minimize chip reads */
	ehci->hcs_params = readl(&ehci->caps->hcs_params);

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

static int hiusb_ehci_hcd_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
	hiusb_stop_hcd();
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static void hiusb_ehci_shutdown(struct platform_device *pdev)
{

	hiusb_start_hcd();

	usb_hcd_platform_shutdown(pdev);

	hiusb_stop_hcd();
}

#ifdef CONFIG_PM
static int hiusb_ehci_hcd_drv_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	struct device *dev = &pdev->dev;
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	unsigned long flags;
	int rc;

	rc = 0;

	if (time_before(jiffies, ehci->next_statechange))
		msleep(20);

	/* Root hub was already suspended. Disable irq emission and
	 * mark HW unaccessible.  The PM and USB cores make sure that
	 * the root hub is either suspended or stopped.
	 */
	spin_lock_irqsave(&ehci->lock, flags);
	ehci_prepare_ports_for_controller_suspend(ehci,
			device_may_wakeup(dev));

	ehci_writel(ehci, 0, &ehci->regs->intr_enable);
	(void)ehci_readl(ehci, &ehci->regs->intr_enable);

	clear_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	hiusb_stop_hcd();
	spin_unlock_irqrestore(&ehci->lock, flags);

	/* could save FLADJ in case of Vaux power loss
		... we'd only use it to handle clock skew*/

	return rc;
}

static int hiusb_ehci_hcd_drv_resume(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);

	hiusb_start_hcd();

	/* maybe restore FLADJ*/

	if (time_before(jiffies, ehci->next_statechange))
		msleep(100);

	/* Mark hardware accessible again as we are out of D3 state by now */
	set_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	/* If CF is still set, we maintained PCI Vaux power.
	 * Just undo the effect of ehci_pci_suspend().
	 */
	if (ehci_readl(ehci, &ehci->regs->configured_flag) == FLAG_CF) {
		int	mask = INTR_MASK;

		ehci_prepare_ports_for_controller_resume(ehci);
		if (!hcd->self.root_hub->do_remote_wakeup)
			mask &= ~STS_PCD;
		ehci_writel(ehci, mask, &ehci->regs->intr_enable);
		ehci_readl(ehci, &ehci->regs->intr_enable);
		return 0;
	}

	ehci_dbg(ehci, "lost power, restarting\n");
	usb_root_hub_lost_power(hcd->self.root_hub);

	/* Else reset, to cope with power loss or flush-to-storage
	 * style "resume" having let BIOS kick in during reboot.
	 */
	(void) ehci_halt(ehci);
	(void) ehci_reset(ehci);

	/* emptying the schedule aborts any urbs */
	spin_lock_irq(&ehci->lock);
	if (ehci->reclaim)
		end_unlink_async(ehci);
	ehci_work(ehci);
	spin_unlock_irq(&ehci->lock);

	ehci_writel(ehci, ehci->command, &ehci->regs->command);
	ehci_writel(ehci, FLAG_CF, &ehci->regs->configured_flag);
	ehci_readl(ehci, &ehci->regs->command);	/* unblock posted writes */

	/* here we "know" root ports should always stay powered */
	ehci_port_power(ehci, 1);

	hcd->state = HC_STATE_SUSPENDED;

	return 0;
}
#endif

static struct platform_driver hiusb_ehci_hcd_driver = {
	.probe		= hiusb_ehci_hcd_drv_probe,
	.remove		= hiusb_ehci_hcd_drv_remove,
#ifdef CONFIG_PM
	.suspend	= hiusb_ehci_hcd_drv_suspend,
	.resume		= hiusb_ehci_hcd_drv_resume,
#endif
	.shutdown	= hiusb_ehci_shutdown,
	.driver = {
		.name	= "hiusb-ehci",
		.owner	= THIS_MODULE,
	}
};

MODULE_ALIAS("platform:hiusb-ehci");

/*****************************************************************************/

static struct resource hiusb_ehci_res[] = {
	[0] = {
		.start = CONFIG_HIUSB_EHCI_IOBASE,
		.end = CONFIG_HIUSB_EHCI_IOBASE
				 + CONFIG_HIUSB_EHCI_IOSIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = CONFIG_HIUSB_EHCI_IRQNUM,
		.end   = CONFIG_HIUSB_EHCI_IRQNUM,
		.flags = IORESOURCE_IRQ,
	},
};

static void usb_ehci_platdev_release(struct device *dev)
{
		/* These don't need to do anything because the
		 pdev structures are statically allocated. */
}

static u64 usb_dmamask = DMA_BIT_MASK(32);

static struct platform_device hiusb_ehci_platdev = {
	.name = "hiusb-ehci",
	.id = 0,
	.dev = {
		.platform_data = NULL,
		.dma_mask = &usb_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.release = usb_ehci_platdev_release,
	},
	.num_resources = ARRAY_SIZE(hiusb_ehci_res),
	.resource = hiusb_ehci_res,
};
