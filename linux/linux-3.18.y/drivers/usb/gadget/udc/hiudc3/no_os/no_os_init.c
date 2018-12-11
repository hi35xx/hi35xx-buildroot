/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

#ifdef LINUXTEST
#define DWC_DRIVER_VERSION	"2.90b - November 2014"
#define DWC_DRIVER_DESC		"SS USB3 Controller driver"

static const char dwc_driver_name[] = "dwc_usb3";
#endif

/**
 * The simulation environment doesn't have a malloc() function, so all data
 * structures are statically allocated
 */

/** Driver context struct */
dwc_usb3_device_t			g_usb3_dev;

/** @{ */
/** Endpoint context structs */
static dwc_usb3_pcd_ep_t		g_ep0;
static dwc_usb3_pcd_ep_t		g_out_ep[DWC_MAX_EPS - 1];
static dwc_usb3_pcd_ep_t		g_in_ep[DWC_MAX_EPS - 1];
/** @} */

/** EP0 PCD request */
static dwc_usb3_pcd_req_t		g_ep0_req;

/** @{ */
/** PCD request pool */
dwc_usb3_pcd_req_t			g_pcd_req[32];
u32					g_pcd_req_bm;
/** @} */

/** Driver options struct, default values are defined here */
dwc_usb3_core_params_t usb3ss_module_params = {
	.burst = 1,
	.newcore = 0,
	.phy = 2,
	.wakeup = 0,
	.pwrctl = 3,
	.lpmctl = 1,
	.phyctl = 1,
	.usb2mode = 0,
	.hibernate = 0,
	.hiberdisc = 1,
	.clkgatingen = 0,
	.ssdisquirk = 0,
	.nobos = 0,
	.loop = 0,
	.nump = 16,
	.newcsr = 0,
	.rxfsz = 0,
	.txfsz = { 0, },
	.txfsz_cnt = 0,
};

#ifndef LINUXTEST
/*** The following data structures must be in coherent DMA capable memory.
 *** All memory in the simulation environment is such, and physical address
 *** == virtual address, so we can just allocate these statically.
 ***/

/** Event buffer */
static u32				g_event_buf[DWC_EVENT_BUF_SIZE];

/** EP0 SETUP packet buffer */
static char				g_ep0_setup_pkt[8];

/** Data packet buffer used to return data for GET_STATUS and
 *  GET_DESCRIPTOR requests, up to 512 bytes in length
 */
static u8				g_ep0_status_buf[DWC_STATUS_BUF_SIZE];

/** DMA descriptor (TRB) for EP0 SETUP packets */
static UALIGNED16 dwc_usb3_dma_desc_t	g_ep0_setup_desc;

/** DMA descriptor (TRB) for EP0 Data Out and Status Out phases */
static UALIGNED16 dwc_usb3_dma_desc_t	g_ep0_out_desc;

/** DMA descriptor (TRB) for EP0 Data In and Status In phases */
static UALIGNED16 dwc_usb3_dma_desc_t	g_ep0_in_desc;

/** @{ */
/** DMA descriptor (TRB) pools for non-EP0 EPs */
UALIGNED16 dwc_usb3_dma_desc_t	g_out_trb_pool[DWC_MAX_EPS - 1][DWC_NUM_ISOC_TRBS + 1];
UALIGNED16 dwc_usb3_dma_desc_t	g_in_trb_pool[DWC_MAX_EPS - 1][DWC_NUM_ISOC_TRBS + 1];
/** @} */

/** @{ */
/** Scratchpad buffers for hibernation support */
static char				g_hiber_scratchpad[15][64];
static struct dwc_hiber_scratchpad_array g_hiber_scratchpad_array;
/** @} */
#endif

#ifdef LINUXTEST
module_param_named(newcsr, usb3ss_module_params.newcsr, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(newcsr, "'gasket' has new, smaller CSR space (0=no, 1=yes)");
#endif

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

#ifdef LINUXTEST
static irqreturn_t dwc_usb3_common_irq(int irq, void *dev
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
				       , struct pt_regs *regs
#endif
				      )
{
	dwc_usb3_device_t *usb3_dev = dev;
	int retval;

	dwc_acquire_spinlock(usb3_dev, &usb3_dev->pcd.lock);
	retval = dwc_usb3_irq(usb3_dev, irq);
	dwc_release_spinlock(usb3_dev, &usb3_dev->pcd.lock);

	return IRQ_RETVAL(retval);
}
#else
/**
 * This routine is the top level interrupt handler for the Common
 * (Core and Device) interrupts
 */
void dwc_usb3_common_irq(int irq, void *dev)
{
	dwc_usb3_device_t *usb3_dev = dev;

	dwc_acquire_spinlock(usb3_dev, &usb3_dev->pcd.lock);
	dwc_usb3_irq(usb3_dev, irq);
	dwc_release_spinlock(usb3_dev, &usb3_dev->pcd.lock);
}
#endif

/**
 * This routine de-initializes the driver
 */
#ifdef LINUXTEST
static void dwc_usb3_driver_remove(struct pci_dev *dev)
#else
void dwc_usb3_driver_remove(void)
#endif
{
	dwc_usb3_device_t *usb3_dev = &g_usb3_dev;
	u32 *event_buf;
	dwc_dma_t event_buf_dma;

	dwc_debug0(usb3_dev, "usb3ss_driver_remove()\n");

	if (usb3_dev->cmn_irq_installed) {
		usb3_dev->cmn_irq_installed = 0;
#ifdef LINUXTEST
		free_irq(dev->irq, usb3_dev);
#endif
	}

	if (usb3_dev->pcd_initialized) {
		usb3_dev->pcd_initialized = 0;
		dwc_usb3_pcd_remove(usb3_dev);
	}

	if (usb3_dev->gadget_initialized) {
		usb3_dev->gadget_initialized = 0;
		dwc_usb3_function_remove(usb3_dev);
		dwc_usb3_gadget_remove(usb3_dev);
	}

	if (usb3_dev->cmn_initialized) {
		usb3_dev->cmn_initialized = 0;
		dwc_usb3_pcd_common_remove(usb3_dev);
	}

#ifdef LINUXTEST
	if (usb3_dev->pcd.ep0_setup_pkt)
		dma_free_coherent(&dev->dev,
				  sizeof(*usb3_dev->pcd.ep0_setup_pkt) * 5,
				  usb3_dev->pcd.ep0_setup_pkt,
				  usb3_dev->pcd.ep0_setup_pkt_dma);

	if (usb3_dev->pcd.ep0_status_buf)
		dma_free_coherent(&dev->dev,
				  DWC_STATUS_BUF_SIZE,
				  usb3_dev->pcd.ep0_status_buf,
				  usb3_dev->pcd.ep0_status_buf_dma);

	if (usb3_dev->pcd.ep0_in_desc)
		dma_free_coherent(&dev->dev,
				  sizeof(dwc_usb3_dma_desc_t),
				  usb3_dev->pcd.ep0_in_desc,
				  usb3_dev->pcd.ep0_in_desc_dma);

	if (usb3_dev->pcd.ep0_out_desc)
		dma_free_coherent(&dev->dev,
				  sizeof(dwc_usb3_dma_desc_t),
				  usb3_dev->pcd.ep0_out_desc,
				  usb3_dev->pcd.ep0_out_desc_dma);

	if (usb3_dev->pcd.ep0_setup_desc)
		dma_free_coherent(&dev->dev,
				  sizeof(dwc_usb3_dma_desc_t),
				  usb3_dev->pcd.ep0_setup_desc,
				  usb3_dev->pcd.ep0_setup_desc_dma);
#endif

	event_buf = usb3_dev->event_buf[0];
	event_buf_dma = usb3_dev->event_buf_dma[0];
	if (event_buf) {
		dwc_usb3_dis_flush_eventbuf_intr(usb3_dev, 0);
		usb3_dev->event_buf[0] = NULL;
#ifdef LINUXTEST
		dma_free_coherent(&dev->dev, DWC_EVENT_BUF_SIZE * sizeof(u32),
				  event_buf, event_buf_dma);
#endif
	}

	if (usb3_dev->sysfs_initialized)
		usb3_dev->sysfs_initialized = 0;

#ifdef LINUXTEST
	/*
	 * Clear the drvdata pointer.
	 */
	pci_set_drvdata(dev, NULL);

	/*
	 * Return the memory.
	 */
	if (usb3_dev->base)
		iounmap(usb3_dev->base);
	if (usb3_dev->rsrc_start)
		release_mem_region(usb3_dev->rsrc_start, usb3_dev->rsrc_len);

	//pci_disable_device(dev);
#endif

	return;
}

/**
 * This routine initializes the driver
 *
 * This routine must be called in a context that allows <strong>dwc_msleep()
 * </strong> to be used, because that function is called while waiting for the
 * core to come out of reset.
 */
#ifdef LINUXTEST
static int dwc_usb3_driver_probe(struct pci_dev *dev,
				 const struct pci_device_id *id)
#else
dwc_usb3_device_t *dwc_usb3_driver_init(u32 base_addr_dwc)
#endif
{
	dwc_usb3_device_t *usb3_dev = &g_usb3_dev;
	u32 addr_ofs = 0xc000;
	int retval, i;

	dwc_debug0(usb3_dev, "usb3ss_driver_init()\n");

	memset(usb3_dev, 0, sizeof(*usb3_dev));
	dwc_init_spinlock(usb3_dev, &usb3_dev->pcd.lock);

#ifdef LINUXTEST
	dev_dbg(&dev->dev, "start=0x%08x\n", (unsigned)pci_resource_start(dev, 0));
	dev_dbg(&dev->dev, "len=0x%08x\n", (unsigned)pci_resource_len(dev, 0));

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

	usb3_dev->dev = &dev->dev;
	pci_set_drvdata(dev, usb3_dev);

	dev_dbg(&dev->dev, "dwc_usb3_device=%lx\n", (unsigned long)usb3_dev);

	usb3_dev->rsrc_start = pci_resource_start(dev, 0);
	usb3_dev->rsrc_len = pci_resource_len(dev, 0);

	if (!usb3_dev->rsrc_start || !usb3_dev->rsrc_len) {
		dev_err(&dev->dev, "bad PCI resource!\n");
		retval = -ENOMEM;
		goto fail;
	}

	/*
	 * Map the DWC_usb3 Core memory into virtual address space.
	 */
	if (!request_mem_region(usb3_dev->rsrc_start,
				usb3_dev->rsrc_len, "usb3")) {
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

	if (usb3ss_module_params.newcsr)
		usb3_dev->gasket_ofs = 0xf000;
	else
		usb3_dev->gasket_ofs = 0x80000;

	dev_dbg(&dev->dev, "base=%lx\n", (unsigned long)usb3_dev->base);

	usb3_dev->sysfs_initialized = 1;
#else
	usb3_dev->base = (volatile u8 __iomem *)(long)base_addr_dwc;
#endif

	/*
	 * Checks that this device is really a DWC_usb3 controller. Also saves
	 * the SNPSID register value in usb3_dev->snpsid for later use by the
	 * PCD.
	 */
	retval = dwc_usb3_pcd_check_snpsid(usb3_dev, addr_ofs);
	if (retval) {
		dwc_error0(&dev->dev, "bad value for SNPSID!\n");
		goto fail;
	}

	if (usb3ss_module_params.newcore && usb3_dev->snpsid < 0x5533109a)
		usb3_dev->snpsid = 0x5533109a;

#ifdef LINUXTEST
	retval = -ENOMEM;

	/*
	 * Up to 32 Event Buffers are supported by the hardware, but we only
	 * use 1
	 */
	usb3_dev->event_buf[0] = dma_alloc_coherent(&dev->dev,
					DWC_EVENT_BUF_SIZE * sizeof(u32),
					&usb3_dev->event_buf_dma[0],
					GFP_KERNEL | GFP_DMA32);
	if (!usb3_dev->event_buf[0]) {
		dev_err(&dev->dev, "allocation of event_buf failed!\n");
		goto fail;
	}

	/*
	 * Allocate all the data structures that must be in DMA memory
	 */
	usb3_dev->pcd.ep0_setup_desc = dma_alloc_coherent(
			&dev->dev, sizeof(dwc_usb3_dma_desc_t),
			&usb3_dev->pcd.ep0_setup_desc_dma,
			GFP_KERNEL | GFP_DMA32);
	if (!usb3_dev->pcd.ep0_setup_desc)
		goto fail;

	usb3_dev->pcd.ep0_out_desc = dma_alloc_coherent(
			&dev->dev, sizeof(dwc_usb3_dma_desc_t),
			&usb3_dev->pcd.ep0_out_desc_dma,
			GFP_KERNEL | GFP_DMA32);
	if (!usb3_dev->pcd.ep0_out_desc)
		goto fail;

	usb3_dev->pcd.ep0_in_desc = dma_alloc_coherent(
			&dev->dev, sizeof(dwc_usb3_dma_desc_t),
			&usb3_dev->pcd.ep0_in_desc_dma,
			GFP_KERNEL | GFP_DMA32);
	if (!usb3_dev->pcd.ep0_in_desc)
		goto fail;

	usb3_dev->pcd.ep0_status_buf = dma_alloc_coherent(
			&dev->dev, DWC_STATUS_BUF_SIZE,
			&usb3_dev->pcd.ep0_status_buf_dma,
			GFP_KERNEL | GFP_DMA32);
	if (!usb3_dev->pcd.ep0_status_buf)
		goto fail;

	usb3_dev->pcd.ep0_setup_pkt = dma_alloc_coherent(
			&dev->dev, sizeof(*usb3_dev->pcd.ep0_setup_pkt) * 5,
			&usb3_dev->pcd.ep0_setup_pkt_dma,
			GFP_KERNEL | GFP_DMA32);
	if (!usb3_dev->pcd.ep0_setup_pkt)
		goto fail;
#else
	/*
	 * Up to 32 Event Buffers are supported by the hardware, but we only
	 * use 1
	 */
	usb3_dev->event_buf[0] = g_event_buf;
	usb3_dev->event_buf_dma[0] = (dwc_dma_t)g_event_buf;

	/*
	 * "allocate" all the data structures that must be in DMA memory
	 */
	usb3_dev->pcd.ep0_setup_desc = &g_ep0_setup_desc;
	usb3_dev->pcd.ep0_setup_desc_dma = (dwc_dma_t)&g_ep0_setup_desc;
	usb3_dev->pcd.ep0_out_desc = &g_ep0_out_desc;
	usb3_dev->pcd.ep0_out_desc_dma = (dwc_dma_t)&g_ep0_out_desc;
	usb3_dev->pcd.ep0_in_desc = &g_ep0_in_desc;
	usb3_dev->pcd.ep0_in_desc_dma = (dwc_dma_t)&g_ep0_in_desc;
	usb3_dev->pcd.ep0_status_buf = g_ep0_status_buf;
	usb3_dev->pcd.ep0_status_buf_dma = (dwc_dma_t)g_ep0_status_buf;
	usb3_dev->pcd.ep0_setup_pkt = (union dwc_setup_pkt *)g_ep0_setup_pkt;
	usb3_dev->pcd.ep0_setup_pkt_dma = (dwc_dma_t)g_ep0_setup_pkt;

	for (i = 0; i < 15; i++) {
		g_hiber_scratchpad_array.dma_addr[i] = (u64)(long)&g_hiber_scratchpad[i];
		usb3_dev->pcd.hiber_scratchpad[i] = &g_hiber_scratchpad[i];
	}

	usb3_dev->pcd.hiber_scratchpad_array = &g_hiber_scratchpad_array;
	usb3_dev->pcd.hiber_scratchpad_array_dma = (dwc_dma_t)&g_hiber_scratchpad_array;
#endif

	/*
	 * Now "allocate" the remaining data structures
	 */
	usb3_dev->pcd.ep0_req = &g_ep0_req;
	usb3_dev->pcd.ep0 = &g_ep0;
	for (i = 0; i < DWC_MAX_EPS - 1; i++) {
		usb3_dev->pcd.out_ep[i] = &g_out_ep[i];
		usb3_dev->pcd.in_ep[i] = &g_in_ep[i];
	}

	g_pcd_req_bm = 0xffffffff;

	/*
	 * Add our hook to override the default Phy register setup
	 */
	usb3_dev->phy_config_hook = haps_phy_config_hook;

	/*
	 * Initialize the DWC_usb3 core
	 */
	retval = dwc_usb3_pcd_common_init(usb3_dev, usb3_dev->base + addr_ofs,
					  &usb3ss_module_params);
	if (retval) {
		dwc_error0(usb3_dev, "CIL initialization failed!\n");
		goto fail;
	}

	usb3_dev->cmn_initialized = 1;

	/*
	 * Initialize the Function Driver interface
	 */
	retval = dwc_usb3_gadget_init(usb3_dev);
	if (retval) {
		dwc_error0(usb3_dev, "gadget initialization failed!\n");
		goto fail;
	}

	/*
	 * Initialize the Function Driver
	 */
	retval = dwc_usb3_function_init(usb3_dev);
	if (retval) {
		dwc_error0(usb3_dev, "loopback gadget initialization failed!\n");
		dwc_usb3_gadget_remove(usb3_dev);
		goto fail;
	}

	usb3_dev->gadget_initialized = 1;

	/*
	 * Initialize the PCD
	 */
	retval = dwc_usb3_pcd_init(usb3_dev);
	if (retval) {
		dwc_error0(usb3_dev, "PCD initialization failed!\n");
		goto fail;
	}

	usb3_dev->pcd_initialized = 1;

#ifdef LINUXTEST
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
		retval = -EBUSY;
		goto fail;
	}
#else
# if 0 // Interrupt handler is hooked up before this routine is called
	// Register interrupt service routine for DWC interrupt
	interrupt_priority_set(USB3SS_DWC_INT, 5);
	interrupt_target_set(USB3SS_DWC_INT, CPU0, 1);
	intRegister(USB3SS_DWC_INT, usb3ss_common_irq, usb3_dev);
	interrupt_enable(USB3SS_DWC_INT, 1);
# endif
#endif
	usb3_dev->cmn_irq_installed = 1;

#ifdef LINUXTEST
	return 0;
#else
	return usb3_dev;
#endif

fail:
#ifdef LINUXTEST
	dwc_usb3_driver_remove(dev);

	return retval;
#else
	dwc_usb3_driver_remove();

	return NULL;
#endif
}

#ifdef LINUXTEST

#define PCI_VENDOR_ID_SYNOPSYS		0x16c3
#define PCI_DEVICE_ID_SYNOPSYS_SITKA	0xabcd

static const struct pci_device_id pci_ids[] = {
	{
		/* The Synopsys PCIe card */
		PCI_DEVICE(PCI_VENDOR_ID_SYNOPSYS,
			   PCI_DEVICE_ID_SYNOPSYS_SITKA),
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
 * The probe routine is called when the bus driver matches a device
 * to this driver. The remove routine is called when a device is
 * unregistered with the bus driver.
 */
static struct pci_driver dwc_usb3_driver = {
	.name		= (char *)dwc_driver_name,
	.id_table	= pci_ids,
	.probe		= dwc_usb3_driver_probe,
	.remove		= dwc_usb3_driver_remove,
	.driver = {
		.name		= (char *)dwc_driver_name,
	},
};

/**
 * This routine is called when the DWC_usb3 driver is loaded into the kernel
 * with the insmod command. It registers the dwc_usb3_driver structure with the
 * appropriate bus driver. This will cause the dwc_usb3_driver_probe routine
 * to be called. In addition, the bus driver will automatically expose
 * attributes defined for the device and driver in the special sysfs file
 * system.
 */
static int __init dwc_usb3_driver_init(void)
{
	int retval = 0;

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
 * This routine is called when the DWC_usb3 driver is removed from the kernel
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

MODULE_DESCRIPTION(DWC_DRIVER_DESC);
MODULE_AUTHOR("Synopsys Inc.");
MODULE_LICENSE("GPL");

#else /* !LINUXTEST */

int main(int argc, char *argv[])
{
	return 0;
}

#endif
