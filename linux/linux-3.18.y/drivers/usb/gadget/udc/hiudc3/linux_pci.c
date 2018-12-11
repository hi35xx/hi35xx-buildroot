/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/DWC_usb3/driver/linux/linux_pci.c $
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

#ifdef CONFIG_USB_OTG_DWC
extern int dwc_otg3_irq(struct usb_otg *otg);
extern void dwc_otg3_set_peripheral(struct usb_otg *otg, int yes);
#endif

#define PCI_VENDOR_ID_SYNOPSYS		0x16c3
#define PCI_DEVICE_ID_SYNOPSYS_SITKA	0xabcd
#define PCI_DEVICE_ID_SYNOPSYS_HAPS_AXI	0xabce

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
 * This function is called when a pci_dev is unregistered with the
 * dwc_usb3_driver. This happens, for example, when the rmmod command is
 * executed. The device may or may not be electrically present. If it is
 * present, the driver stops device processing. Any resources used on behalf
 * of this device are freed.
 *
 * @param dev pci_dev struct
 */
static void dwc_usb3_driver_remove(struct pci_dev *dev)
{
	dwc_usb3_device_t *usb3_dev = pci_get_drvdata(dev);
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
		free_irq(dev->irq, usb3_dev);
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
	pci_set_drvdata(dev, NULL);

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
	//pci_disable_device(dev);
	return;
}

/**
 * This function is called when a pci_dev is bound to a
 * dwc_usb3_driver. It creates the driver components required to
 * control the device (CIL and PCD) and it initializes the
 * device. The driver components are stored in a dwc_usb3_device
 * structure. A reference to the dwc_usb3_device is saved in the
 * pci_dev. This allows the driver to access the dwc_usb3_device
 * structure on subsequent calls to driver methods for this device.
 *
 * @param dev pci_dev struct
 * @param id  pci_dev_id struct
 */
static int dwc_usb3_driver_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	dwc_usb3_device_t *usb3_dev;
	u32 addr_ofs = 0xc000;
	int retval = 0;

#ifdef CONFIG_USB_OTG_DWC
	struct usb_phy *phy;
	struct usb_otg *otg;
#endif

	printk(KERN_DEBUG "%s: driver_probe()\n", dwc_driver_name);
	dev_dbg(&dev->dev, "dwc_usb3_driver_probe(%p)\n", dev);

	dev_dbg(&dev->dev, "start=0x%08x\n",
		(unsigned)pci_resource_start(dev, 0));
	dev_dbg(&dev->dev, "len=0x%08x\n", (unsigned)pci_resource_len(dev, 0));

#ifndef DWC_BAR2_GASKET_REG
	if (id->device == PCI_DEVICE_ID_SYNOPSYS_HAPS_AXI)
#endif
	{
		dev_dbg(&dev->dev, "start2=0x%08x\n",
			(unsigned)pci_resource_start(dev, 2));
		dev_dbg(&dev->dev, "len2=0x%08x\n",
			(unsigned)pci_resource_len(dev, 2));
	}

	if (!id) {
		dev_err(&dev->dev, "id parameter NULL!\n");
		return -EINVAL;
	}

	if (pci_enable_device(dev) < 0) {
		dev_err(&dev->dev, "pci_enable_device() failed!\n");
		return -ENODEV;
	}

	dev->current_state = PCI_D0;
	dev->dev.power.power_state = PMSG_ON;

	if (!dev->irq) {
		dev_err(&dev->dev, "no IRQ for PCI device!\n");
		retval = -ENODEV;
		goto fail;
	}

	usb3_dev = kmalloc(sizeof(dwc_usb3_device_t), GFP_KERNEL);
	if (!usb3_dev) {
		dev_err(&dev->dev, "kmalloc of dwc_usb3_device failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	memset(usb3_dev, 0, sizeof(*usb3_dev));
	usb3_dev->dev = &dev->dev;
	usb3_dev->rsrc_start = pci_resource_start(dev, 0);
	usb3_dev->rsrc_len = pci_resource_len(dev, 0);

#ifndef DWC_BAR2_GASKET_REG
	if (id->device == PCI_DEVICE_ID_SYNOPSYS_HAPS_AXI)
#endif
	{
		usb3_dev->gasket_start = pci_resource_start(dev, 2);
		usb3_dev->gasket_len = pci_resource_len(dev, 2);
	}

	/*
	 * Initialize driver data to point to the global DWC_usb3
	 * Device structure.
	 */
	pci_set_drvdata(dev, usb3_dev);
	dev_dbg(&dev->dev, "dwc_usb3_device=0x%p\n", usb3_dev);

	if (!usb3_dev->rsrc_start || !usb3_dev->rsrc_len) {
		dev_err(&dev->dev, "bad PCI resource!\n");
		retval = -ENOMEM;
		goto fail;
	}

#ifndef DWC_BAR2_GASKET_REG
	if (id->device == PCI_DEVICE_ID_SYNOPSYS_HAPS_AXI)
#endif
	{
		if (!usb3_dev->gasket_start || !usb3_dev->gasket_len) {
			dev_err(&dev->dev, "bad PCI resource 2!\n");
			retval = -ENOMEM;
			goto fail;
		}
	}

	/*
	 * Map the DWC_usb3 Core memory into virtual address space.
	 */
	if (!request_mem_region(usb3_dev->rsrc_start, usb3_dev->rsrc_len,
				"usb3")) {
		dev_err(&dev->dev, "request_mem_region() failed!\n");

		/* Flag for dwc_usb3_driver_remove() that we failed */
		usb3_dev->rsrc_start = 0;

		retval = -EBUSY;
		goto fail;
	}

	usb3_dev->base = ioremap_nocache(usb3_dev->rsrc_start,
					 usb3_dev->rsrc_len);
	if (!usb3_dev->base) {
		dev_err(&dev->dev, "ioremap_nocache() failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	dev_dbg(&dev->dev, "base=%p\n", usb3_dev->base);

#ifndef DWC_BAR2_GASKET_REG
	if (id->device == PCI_DEVICE_ID_SYNOPSYS_HAPS_AXI)
#endif
	{
		if (!request_mem_region(usb3_dev->gasket_start,
					usb3_dev->gasket_len, "usb3")) {
			dev_err(&dev->dev, "request_mem_region() 2 failed!\n");

			/* Flag for dwc_usb3_driver_remove() that we failed */
			usb3_dev->gasket_start = 0;

			retval = -EBUSY;
			goto fail;
		}

		usb3_dev->gasket_base = ioremap_nocache(usb3_dev->gasket_start,
							usb3_dev->gasket_len);
		if (!usb3_dev->gasket_base) {
			dev_err(&dev->dev, "ioremap_nocache() 2 failed!\n");
			retval = -ENOMEM;
			goto fail;
		}

		dev_dbg(&dev->dev, "gasket_base=%p\n", usb3_dev->gasket_base);
	}

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
		dev->irq);
	retval = request_irq(dev->irq, dwc_usb3_common_irq,
			     IRQF_SHARED | IRQF_DISABLED,
			     dwc_driver_name, usb3_dev);
	if (retval) {
		dev_err(&dev->dev, "request of irq%d failed!\n", dev->irq);
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
	dwc_usb3_driver_remove(dev);
	return retval;
}

static const struct pci_device_id pci_ids[] = {
	{
		/* The Synopsys PCIe card */
		PCI_DEVICE(PCI_VENDOR_ID_SYNOPSYS,
			   PCI_DEVICE_ID_SYNOPSYS_SITKA),
		.driver_data = (unsigned long)0xdeadbeef,
	},
	{
		/* The Synopsys HAPS PCIe card with AXI, with gasket registers
		 * moved to 2nd BAR
		 */
		PCI_DEVICE(PCI_VENDOR_ID_SYNOPSYS,
			   PCI_DEVICE_ID_SYNOPSYS_HAPS_AXI),
		.driver_data = (unsigned long)0xdeadbeef,
	},
	{ 0, }	/* end: all zeroes */
};
MODULE_DEVICE_TABLE(pci, pci_ids);

/**
 * This structure defines the methods to be called by a bus driver
 * during the lifecycle of a device on that bus. Both drivers and
 * devices are registered with a bus driver. The bus driver matches
 * devices to drivers based on information in the device and driver
 * structures.
 *
 * The probe function is called when the bus driver matches a device
 * to this driver. The remove function is called when a device is
 * unregistered with the bus driver.
 */
static struct pci_driver dwc_usb3_driver = {
	.name		= (char *)dwc_driver_name,
	.id_table	= pci_ids,
	.probe		= dwc_usb3_driver_probe,
	.remove		= dwc_usb3_driver_remove,
	.driver		= {
		.name	= (char *)dwc_driver_name,
	},
};

/**
 * This function is called when the DWC_usb3 driver is loaded into the kernel
 * with the insmod command. It registers the dwc_usb3_driver structure with the
 * appropriate bus driver. This will cause the dwc_usb3_driver_probe function
 * to be called. In addition, the bus driver will automatically expose
 * attributes defined for the device and driver in the special sysfs file
 * system.
 */
static int __init dwc_usb3_driver_init(void)
{
	int retval;

	printk(KERN_INFO "%s: %s version %s\n", dwc_driver_name,
	       DWC_DRIVER_DESC, DWC_DRIVER_VERSION);

	retval = pci_register_driver(&dwc_usb3_driver);
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

	pci_unregister_driver(&dwc_usb3_driver);

	printk(KERN_INFO "%s: module removed\n", dwc_driver_name);
}
module_exit(dwc_usb3_driver_exit);
