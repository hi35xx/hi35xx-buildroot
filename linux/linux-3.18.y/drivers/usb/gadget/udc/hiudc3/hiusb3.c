#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/types.h>
#include "os_dev.h"
//#include "xhci.h"
//#include "hiusb3.h"


MODULE_LICENSE("Dual MIT/GPL");

static struct resource hiusb_xhci_res[] = {
    [0] = {
        .start  = CONFIG_HIUSB_XHCI_REG_BASE_ADDRESS,
        .end    = CONFIG_HIUSB_XHCI_REG_BASE_ADDRESS
            + CONFIG_HIUSB_XHCI_REG_BASE_ADDRESS_LEN - 1,
        .flags  = IORESOURCE_MEM,
    },
    [1] = {
        .start  = CONFIG_HIUSB_XHCI_IRQ_NUMBER,
        .end    = CONFIG_HIUSB_XHCI_IRQ_NUMBER,
        .flags  = IORESOURCE_IRQ,
    },
};
static void usb_xhci_platdev_release(struct device *dev)
{
                /* These don't need to do anything because the
                 pdev structures are statically allocated. */
}

//u64 usb_dmamask = DMA_BIT_MASK(32);

static struct platform_device hiusb_xhci_platdev = {
    .name = "dwc_usb3",
    .id = -1,
    .dev = {
        .init_name = "dwc_usb3",
        .platform_data = NULL,
        .dma_mask = DMA_BIT_MASK(32),
        .coherent_dma_mask = DMA_BIT_MASK(32),
	.release = usb_xhci_platdev_release,
    },
    .num_resources = ARRAY_SIZE(hiusb_xhci_res),
    .resource = hiusb_xhci_res,
};

static int __init xhci_device_init(void)
{
//    if (usb_disabled())
  //      return -ENODEV;
        int reg;
//	reg = readl(IO_ADDRESS(0x11080430));
	printk("\n########################################################%s,%d\n",__func__,__LINE__);
	return 0;
	return platform_device_register(&hiusb_xhci_platdev);
}

static void __exit xhci_device_exit(void)
{
    platform_device_unregister(&hiusb_xhci_platdev);
}

module_init(xhci_device_init);
module_exit(xhci_device_exit);
