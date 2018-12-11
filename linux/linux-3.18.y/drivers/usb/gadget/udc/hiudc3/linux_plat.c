/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/linux/linux_plat.c $
 * $Revision: #16 $
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

/** @file
 * The driver module provides the initialization and cleanup entry
 * points for the DWC_usb3 driver. This module will be dynamically installed
 * after Linux is booted using the insmod command. When the module is
 * installed, the driver_init function is called. When the module is
 * removed (using rmmod), the driver_cleanup function is called.
 *
 * This module also defines a data structure for the driver, which is
 * used in conjunction with the standard pci_dev structure. These
 * structures allow the USB3 driver to comply with the standard Linux driver
 * model in which devices and drivers are registered with a bus driver. This
 * has the benefit that Linux can expose attributes of the driver and device
 * in its special sysfs file system. Users can then read or write files in
 * this file system to perform diagnostics on the driver components or the
 * device.
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"
#include <linux/of.h>
#include <mach/io.h>
//#include "../../host/hiusb.h"

#ifndef __devinit
# define __devinit
# define __devexit
# define __devexit_p(x)	x
#endif

#ifdef CONFIG_USB_OTG_DWC
extern int dwc_otg3_irq(struct usb_otg *otg);
extern void dwc_otg3_set_peripheral(struct usb_otg *otg, int yes);
#endif

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
//#if LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0)
/* Enable this to replace the DWC3 driver in recent Linux kernels */
# define DWC_REPLACE_DWC3
//#endif

/* Enable this to support the Synopsys Xplorer platform */
//#define DWC_XPLORER

static const char dwc_driver_name[] = "dwc_usb3";

/*
 * Hook to override the default Phy configuration in dwc_usb3_pcd_device_init()
 * with a HAPS-specific one
 */
static void haps_phy_config_hook(struct dwc_usb3_device *dev, int soft_reset,
				 int restore)
{
	dwc_usb3_core_global_regs_t __iomem *global_regs =
						dev->core_global_regs;
	u32 temp;

	switch (dev->core_params->phy) {
	case 3:		// 16-bit UTMI+ SNPS Phy
		temp = dwc_rd32(dev, &global_regs->gusb2phycfg[0]);
		temp &= ~DWC_USB2PHYCFG_USB_TRD_TIM_BITS;
		temp |= DWC_USB2PHYCFG_16B_PHY_IF_BIT;
		temp |= 5 << DWC_USB2PHYCFG_USB_TRD_TIM_SHIFT;
		dwc_wr32(dev, &global_regs->gusb2phycfg[0], temp);
		break;
	case 2:		// 8-bit UTMI+ / ULPI TI or SNPS Phy
	case 1:		// old 8-bit UTMI+ SNPS Phy
		temp = dwc_rd32(dev, &global_regs->gusb2phycfg[0]);
		temp &= ~DWC_USB2PHYCFG_USB_TRD_TIM_BITS;
		temp &= ~DWC_USB2PHYCFG_16B_PHY_IF_BIT;
		temp |= 9 << DWC_USB2PHYCFG_USB_TRD_TIM_SHIFT;
		dwc_wr32(dev, &global_regs->gusb2phycfg[0], temp);
		break;
	default:	// RocketIO Phy
		if (dev->core_params->usb2mode == 0) {
			/* Set rx-eq, differential swing */
			dwc_wr32(dev, (volatile u32 __iomem *)
				 (dev->base + dev->gasket_ofs + 8), 0x41);
#ifdef LECROY
			/* Rx-detect for LeCroy */
			dwc_wr32(dev, (volatile u32 __iomem *)
				 (dev->base + dev->gasket_ofs + 4), 0x200);
#else
			dwc_wr32(dev, (volatile u32 __iomem *)
				 (dev->base + dev->gasket_ofs + 4), 0);
#endif
		}
	}
}

/**
 * This function is the top level interrupt handler for the Common
 * (Core and Device) interrupts.
 */
static irqreturn_t dwc_usb3_common_irq(int irq, void *dev
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
				       , struct pt_regs *regs
#endif
				      )
{
	dwc_usb3_device_t *usb3_dev = dev;
	int retval = 0;

#ifdef CONFIG_USB_OTG_DWC
	u32 gsts = 0;
	int temp = usb3_dev->hibernate;

	/* Skip OTG IRQ handler if in hibernation */
	if (temp < DWC_HIBER_SLEEPING) {
		gsts = dwc_rd32(usb3_dev, &usb3_dev->core_global_regs->gsts);
	} else {
		dwc_info(usb3_dev, "%s() possible OTG IRQ in hibernation\n",
			 __func__);
		retval = 1;
	}

	if (gsts & DWC_GSTS_OTG_EVT_PENDING_BIT ||
	    gsts & DWC_GSTS_ADP_EVT_PENDING_BIT ||
	    gsts & DWC_GSTS_BC_EVT_PENDING_BIT) {
		struct usb_phy *phy = usb_get_phy(USB_PHY_TYPE_USB3);

		if (!IS_ERR(phy)) {
			dwc_info(usb3_dev, "%s() OTG IRQ\n", __func__);
			dwc_info(usb3_dev, "gsts = %08x\n", gsts);
			if (phy && phy->otg)
				dwc_otg3_irq(phy->otg);
			else
				dwc_info(usb3_dev, "OTG IRQ but no OTG\n");
			usb_put_phy(phy);
		} else {
			dwc_info(usb3_dev, "%s() OTG IRQ but no PHY\n",
				 __func__);
		}

		retval = 1;
	}

	if (temp >= DWC_HIBER_SLEEPING || (gsts & DWC_GSTS_DEV_EVT_PENDING_BIT))
#endif
	{
		spin_lock(&usb3_dev->pcd.lock);
		retval = dwc_usb3_irq(usb3_dev, irq);
		spin_unlock(&usb3_dev->pcd.lock);
	}

	return IRQ_RETVAL(retval);
}

#ifdef DWC_UTE
static void dwc_usb3_save_fifosiz_def_vals(dwc_usb3_device_t *dev)
{
	unsigned i, size;
	dwc_usb3_pcd_t *pcd;

	if (!dev)
		return;

	pcd = &dev->pcd;

	for (i = 0; i < pcd->num_in_eps + 1; i++) {
		size = dwc_rd32(dev, &dev->core_global_regs->gtxfifosiz[i]) &
							DWC_FIFOSZ_DEPTH_BITS;
		dwc_print(dev, "Saving %d TxFIFO default size: %d\n", i, size);
		pcd->def_txf_size[i] = size;
	}

	size = dwc_rd32(dev, &dev->core_global_regs->grxfifosiz[0]) &
							DWC_FIFOSZ_DEPTH_BITS;
	pcd->def_rxf_size = size;

	dwc_print(dev, "Saving RxFIFO default size: %d\n", size);
}
#endif

/**
 * This function is called when a platform_device is unregistered with the
 * dwc_usb3_driver. This happens, for example, when the rmmod command is
 * executed. The device may or may not be electrically present. If it is
 * present, the driver stops device processing. Any resources used on behalf
 * of this device are freed.
 *
 * @param dev platform_device struct
 */
static int dwc_usb3_platform_remove(struct platform_device *dev)
{
	dwc_usb3_device_t *usb3_dev = platform_get_drvdata(dev);
	u32 *event_buf;
	dwc_dma_t event_buf_dma;

#ifdef CONFIG_USB_OTG_DWC
	struct usb_phy *phy;
#endif

	dev_dbg(&dev->dev, "%s(%p)\n", __func__, dev);

	if (!usb3_dev) {
		/* Memory allocation for the dwc_usb3_device failed */
		dev_dbg(&dev->dev, "%s: usb3_dev NULL\n", __func__);
		goto disable;
	}

#ifdef CONFIG_USB_OTG_DWC
	/* Switch otg to peripheral mode */
	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (!IS_ERR(phy)) {
		if (phy && phy->otg)
			dwc_otg3_set_peripheral(phy->otg, 1);
		usb_put_phy(phy);
	}
#endif

	/*
	 * Free the IRQ
	 */
	if (usb3_dev->cmn_irq_installed) {
		usb3_dev->cmn_irq_installed = 0;
		free_irq(usb3_dev->irq, usb3_dev);
	}

	if (usb3_dev->pme_thread) {
		kthread_stop(usb3_dev->pme_thread);
		usb3_dev->pme_thread = NULL;
	}

	if (usb3_dev->pcd_initialized) {
		usb3_dev->pcd_initialized = 0;
		dwc_usb3_pcd_remove(usb3_dev);
	}

	if (usb3_dev->gadget_initialized) {
		usb3_dev->gadget_initialized = 0;
		dwc_usb3_gadget_remove(usb3_dev, &dev->dev);
	}

	if (usb3_dev->cmn_initialized) {
		usb3_dev->cmn_initialized = 0;
		dwc_usb3_pcd_common_remove(usb3_dev);
	}

	event_buf = usb3_dev->event_buf[0];
	event_buf_dma = usb3_dev->event_buf_dma[0];
	if (event_buf) {
		dwc_usb3_dis_flush_eventbuf_intr(usb3_dev, 0);
		usb3_dev->event_buf[0] = NULL;
		dma_free_coherent(NULL, DWC_EVENT_BUF_SIZE * sizeof(u32),
				  event_buf, event_buf_dma);
	}

	if (usb3_dev->sysfs_initialized) {
		dwc_usb3_remove_dev_files(&dev->dev);
		usb3_dev->sysfs_initialized = 0;
	}

	/*
	 * Clear the drvdata pointer.
	 */
	platform_set_drvdata(dev, NULL);

	/*
	 * Return the memory.
	 */
	if (usb3_dev->base)
		iounmap((void __iomem *)usb3_dev->base);
	if (usb3_dev->rsrc_start)
		release_mem_region(usb3_dev->rsrc_start, usb3_dev->rsrc_len);
	if (usb3_dev->gasket_base)
		iounmap((void __iomem *)usb3_dev->gasket_base);
	if (usb3_dev->gasket_start)
		release_mem_region(usb3_dev->gasket_start, usb3_dev->gasket_len);

	kfree(usb3_dev);

disable:
	return 0;
}

static u64 dwc3_vexpress_dma_mask = DMA_BIT_MASK(32);

/**
 * This function is called when a platform_device is bound to a
 * dwc_usb3_driver. It creates the driver components required to
 * control the device (CIL and PCD) and it initializes the
 * device. The driver components are stored in a dwc_usb3_device
 * structure. A reference to the dwc_usb3_device is saved in the
 * platform_device. This allows the driver to access the dwc_usb3_device
 * structure on subsequent calls to driver methods for this device.
 *
 * @param dev platform_device struct
 */
static int __devinit dwc_usb3_platform_probe(struct platform_device *dev)
{
#if 0
	int reg;
	reg = dwc_rd32(dev, IO_ADDRESS(0x10180430));
	reg = dwc_rd32(dev, IO_ADDRESS(0x1018c110));
	reg &= ~(3 << 12);
	reg |= (2 << 12);
	dwc_wr32(dev, IO_ADDRESS(0x1018c110), reg);
#endif
	dwc_usb3_device_t *usb3_dev;
	struct resource *res_mem;
	u32 addr_ofs = 0xc000;
	int irq, retval = 0;

#ifdef DWC_BAR2_GASKET_REG
	struct resource *res_mem2;
#endif

#ifdef CONFIG_USB_OTG_DWC
	struct usb_phy *phy;
	struct usb_otg *otg;
#endif

	printk(KERN_DEBUG "%s: platform_probe()\n", dwc_driver_name);
	dev_dbg(&dev->dev, "dwc_usb3_platform_probe(%p)\n", dev);

	if (!dev->dev.dma_mask)
		dev->dev.dma_mask = &dwc3_vexpress_dma_mask;

	irq = platform_get_irq(dev, 0);
	if (irq < 0) {
		dev_err(&dev->dev, "no irq provided");
		return irq;
	}

	res_mem = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (!res_mem) {
		dev_err(&dev->dev, "no memory resource provided");
		return -ENXIO;
	}

#ifdef DWC_BAR2_GASKET_REG
	res_mem2 = platform_get_resource(dev, IORESOURCE_MEM, 2);
	if (!res_mem2) {
		dev_err(&dev->dev, "no memory resource 2 provided");
		return -ENXIO;
	}
#endif

	usb3_dev = kmalloc(sizeof(dwc_usb3_device_t), GFP_KERNEL);
	if (!usb3_dev) {
		dev_err(&dev->dev, "kmalloc of dwc_usb3_device failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	memset(usb3_dev, 0, sizeof(*usb3_dev));
	usb3_dev->dev = &dev->dev;
	usb3_dev->irq = irq;

	/*
	 * Initialize driver data to point to the global DWC_usb3
	 * Device structure.
	 */
	platform_set_drvdata(dev, usb3_dev);
	dev_dbg(&dev->dev, "dwc_usb3_device=0x%p\n", usb3_dev);

	/*
	 * Map the DWC_usb3 Core memory into virtual address space.
	 */
#if 0
	if (!request_mem_region(res_mem->start, resource_size(res_mem),
				"usb3")) {
		dev_err(&dev->dev, "request_mem_region() failed!\n");
		retval = -EBUSY;
		goto fail;
	}
#endif
	usb3_dev->rsrc_start = res_mem->start;
	usb3_dev->rsrc_len = resource_size(res_mem);

	usb3_dev->base = ioremap_nocache(usb3_dev->rsrc_start,
					 usb3_dev->rsrc_len);
	if (!usb3_dev->base) {
		dev_err(&dev->dev, "ioremap_nocache() failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	dev_dbg(&dev->dev, "base=%p\n", usb3_dev->base);

#ifdef DWC_BAR2_GASKET_REG
	if (!request_mem_region(res_mem2->start, resource_size(res_mem2),
				"usb3")) {
		dev_err(&dev->dev, "request_mem_region() 2 failed!\n");
		retval = -EBUSY;
		goto fail;
	}

	usb3_dev->gasket_start = res_mem2->start;
	usb3_dev->gasket_len = resource_size(res_mem2);

	usb3_dev->gasket_base = ioremap_nocache(usb3_dev->gasket_start,
						usb3_dev->gasket_len);
	if (!usb3_dev->gasket_base) {
		dev_err(&dev->dev, "ioremap_nocache() 2 failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	dev_dbg(&dev->dev, "gasket_base=%p\n", usb3_dev->gasket_base);
#endif

	if (dwc_usb3_module_params.newcsr)
		usb3_dev->gasket_ofs = 0xf000;
	else
		usb3_dev->gasket_ofs = 0x80000;

#ifdef CONFIG_USB_OTG_DWC
	/* Switch otg to peripheral mode */
	phy = usb_get_phy(USB_PHY_TYPE_USB3);
	if (IS_ERR(phy) || !phy) {
		dev_err(&dev->dev, "OTG PHY not available!\n");
		if (!IS_ERR(phy))
			usb_put_phy(phy);
		retval = -ENODEV;
		goto fail;
	}

	otg = phy->otg;
	if (!otg) {
		dev_err(&dev->dev, "OTG not available!\n");
		usb_put_phy(phy);
		retval = -ENODEV;
		goto fail;
	}

	phy->io_priv = (void __iomem *)usb3_dev->base;
	dwc_otg3_set_peripheral(otg, 1);
	usb_put_phy(phy);
#endif

	retval = dwc_usb3_create_dev_files(&dev->dev);
	if (retval) {
		dev_err(&dev->dev, "sysfs initialization failed!\n");
		goto fail;
	}

	usb3_dev->sysfs_initialized = 1;

	/*
	 * Checks that this device is really a DWC_usb3 controller. Also saves
	 * the SNPSID register value in usb3_dev->snpsid for later use by the
	 * PCD.
	 */
	retval = dwc_usb3_pcd_check_snpsid(usb3_dev, addr_ofs);
	if (retval) {
		dev_err(&dev->dev, "bad value for SNPSID!\n");
		goto fail;
	}

	if (dwc_usb3_module_params.newcore && usb3_dev->snpsid < 0x5533109a)
		usb3_dev->snpsid = 0x5533109a;

	/*
	 * Up to 32 Event Buffers are supported by the hardware,
	 * but we only use 1
	 */
	usb3_dev->event_buf[0] = dma_alloc_coherent(NULL,
					DWC_EVENT_BUF_SIZE * sizeof(u32),
					&usb3_dev->event_buf_dma[0],
					GFP_KERNEL | GFP_DMA32);
	if (!usb3_dev->event_buf[0]) {
		dev_err(&dev->dev, "allocation of event_buf failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	/*
	 * Add our hook to override the default Phy register setup
	 */
	usb3_dev->phy_config_hook = haps_phy_config_hook;

	/*
	 * Initialize the DWC_usb3 Core.
	 */
	retval = dwc_usb3_pcd_common_init(usb3_dev, usb3_dev->base + addr_ofs,
					  &dwc_usb3_module_params);
	if (retval) {
		dev_err(&dev->dev, "CIL initialization failed!\n");
		goto fail;
	}

	usb3_dev->cmn_initialized = 1;

#ifdef DWC_UTE
	dwc_usb3_save_fifosiz_def_vals(usb3_dev);
#endif

	spin_lock_init(&usb3_dev->pcd.lock);

	/*
	 * Initialize the Gadget
	 */
	retval = dwc_usb3_gadget_init(usb3_dev, &dev->dev);
	if (retval) {
		dev_err(&dev->dev, "gadget initialization failed!\n");
		goto fail;
	}

	usb3_dev->gadget_initialized = 1;

	/*
	 * Initialize the PCD
	 */
	retval = dwc_usb3_pcd_init(usb3_dev);
	if (retval) {
		dev_err(&dev->dev, "PCD initialization failed!\n");
		goto fail;
	}

	usb3_dev->pcd_initialized = 1;

	/* Allocate the test mode tasklet */
	tasklet_init(&usb3_dev->pcd.test_mode_tasklet,
		     dwc_usb3_pcd_do_test_mode,
		     (unsigned long)&usb3_dev->pcd);

	/* Start the hibernation thread */
	usb3_dev->pme_thread = kthread_run(dwc_wait_pme_thread, &usb3_dev->pcd,
					   "pmethr");
	if (IS_ERR(usb3_dev->pme_thread)) {
		retval = PTR_ERR(usb3_dev->pme_thread);
		usb3_dev->pme_thread = NULL;
		goto fail;
	}

	/*
	 * Install the interrupt handler for the common interrupts.
	 */
	dev_dbg(&dev->dev, "registering (common) handler for irq%d\n",
		usb3_dev->irq);
	retval = request_irq(usb3_dev->irq, dwc_usb3_common_irq,
			     IRQF_SHARED | IRQF_DISABLED,
			     dwc_driver_name, usb3_dev);
	if (retval) {
		dev_err(&dev->dev, "request of irq%d failed!\n", usb3_dev->irq);
		goto fail;
	}

	usb3_dev->cmn_irq_installed = 1;
#if 0
	if (dwc_usb3_module_params.hibernate &&
	    (usb3_dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT) {
		unsigned long flags;

		spin_lock_irqsave(&usb3_dev->pcd.lock, flags);
		dwc_enter_hibernation(&usb3_dev->pcd, 0);
		spin_unlock_irqrestore(&usb3_dev->pcd.lock, flags);
	}
#endif
	return 0;

fail:
	dwc_usb3_platform_remove(dev);
	return retval;
}

#ifdef DWC_REPLACE_DWC3

#ifdef CONFIG_OF
static const struct of_device_id of_dwc_usb3_match[] = {
#if 0
#ifdef DWC_XPLORER
	{
		.compatible = "snps,xplorer-dwusb3"
	},
#else
	{
		.compatible = "synopsys,dwc3"
	},
#endif
#endif
	{ .compatible = "dwc_usb3", },
	{ },
};
MODULE_DEVICE_TABLE(of, of_dwc_usb3_match);
#endif

static struct platform_driver dwc_usb3_platform_driver = {
	.probe		= dwc_usb3_platform_probe,
	.remove		= dwc_usb3_platform_remove,
	.driver		= {
#ifdef DWC_XPLORER
		.name	= "xplorer-dwc3",
#else
		.name  = (char *)dwc_driver_name,
//		.name	= "dwc3",
#endif
		.of_match_table	= of_dwc_usb3_match,
	},
};

module_platform_driver(dwc_usb3_platform_driver);

#ifdef DWC_XPLORER
MODULE_ALIAS("platform:xplorer-dwc3");
#else
MODULE_ALIAS("platform:dwc3");
#endif

#else /* DWC_REPLACE_DWC3 */

static const struct platform_device_id dwc_usb3_platform_ids[] = {
	{ "dwc-usb3-platform", 0 },
	{ "dwc_usb3", 0 },
	{ }
};
MODULE_DEVICE_TABLE(platform, dwc_usb3_platform_ids);

static struct platform_driver dwc_usb3_platform_driver = {
	.id_table	= dwc_usb3_platform_ids,
	.probe		= dwc_usb3_platform_probe,
	.remove		= __devexit_p(dwc_usb3_platform_remove),
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "dwc_usb3",
	}
};

/**
 * This function is called when the DWC_usb3 driver is loaded into the kernel
 * with the insmod command. It registers the dwc_usb3_driver structure with the
 * appropriate bus driver. This will cause the dwc_usb3_platform_probe function
 * to be called. In addition, the bus driver will automatically expose
 * attributes defined for the device and driver in the special sysfs file
 * system.
 */
static int __init dwc_usb3_driver_init(void)
{
	int retval;

	printk("\n###%s,%d,reg(c110)=0x%x\n",__func__,__LINE__,readl((IO_ADDRESS(0x1018c110))));
	printk("\n###%s,%d\n",__func__,__LINE__);
	printk(KERN_INFO "%s: %s version %s\n", dwc_driver_name,
	       DWC_DRIVER_DESC, DWC_DRIVER_VERSION);

	retval = platform_driver_register(&dwc_usb3_platform_driver);
	if (retval < 0) {
		printk(KERN_ERR "%s retval=%d\n", __func__, retval);
		return retval;
	}

	printk(KERN_INFO "%s: module installed\n", dwc_driver_name);
	return retval;
}
module_init(dwc_usb3_driver_init);

/**
 * This function is called when the DWC_usb3 driver is removed from the kernel
 * with the rmmod command. The driver unregisters itself with its bus driver.
 *
 */
static void __exit dwc_usb3_driver_exit(void)
{
	printk(KERN_DEBUG "%s: driver_exit()\n", dwc_driver_name);

	platform_driver_unregister(&dwc_usb3_platform_driver);

	printk(KERN_INFO "%s: module removed\n", dwc_driver_name);
}
module_exit(dwc_usb3_driver_exit);
#endif
