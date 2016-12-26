#include <linux/init.h>
#include <linux/timer.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <asm/byteorder.h>
#include <linux/io.h>
#include <asm/system.h>
#include <asm/unaligned.h>
#include <linux/kthread.h>

static struct task_struct *kusbotg_task;

#define USB2_SWITCH_BASE                IO_ADDRESS(0x20050130)
#define PERI_USB0                       IO_ADDRESS(0x20120078)
#define DWC_OTG_EN                      (1 << 31)
#define USB2_PHY_DPPULL_DOWN		(0x3 << 26)

extern int otg_usbdev_stat;
int otg_usbhost_stat;
EXPORT_SYMBOL(otg_usbhost_stat);

void hisi_switch_func(int otg)
{
	int reg;

	reg = readl(USB2_SWITCH_BASE);
	if (otg) {
		reg |= 0x1;
		writel(reg, USB2_SWITCH_BASE);
	} else {
		reg &= ~(0x1);
		writel(reg, USB2_SWITCH_BASE);
	}
}
EXPORT_SYMBOL(hisi_switch_func);

static void device_to_host(void)
{
	int reg;
	int reg1;

	reg = readl(PERI_USB0);
	reg1 = readl(IO_ADDRESS(0x10080804));
	reg1 |= (0x1 << 1);
	writel(reg1, IO_ADDRESS(0x10080804));
	reg |= USB2_PHY_DPPULL_DOWN;
	reg &= ~DWC_OTG_EN;
	writel(reg, PERI_USB0);
}

static void host_to_device(void)
{
	int reg;
	int reg1;

	reg = readl(PERI_USB0);
	reg &= ~(USB2_PHY_DPPULL_DOWN);
	reg |= DWC_OTG_EN;
	writel(reg, PERI_USB0);
	reg1 = readl(IO_ADDRESS(0x10080804));
	reg1 &= ~(0x1 << 1);
	writel(reg1, IO_ADDRESS(0x10080804));
}

int  otg_run(void)
{
	int reg;

	reg = readl(PERI_USB0);

	/* device -->host */
	if ((reg & DWC_OTG_EN) == DWC_OTG_EN) {

		if (otg_usbhost_stat == 1)
			return 0;
		device_to_host();

/*		reg1 = readl(IO_ADDRESS(0x10080804));
		reg1 |= (0x1 << 1);
		writel(reg1, IO_ADDRESS(0x10080804));
		reg |= USB2_PHY_DPPULL_DOWN;
		reg &= ~DWC_OTG_EN;
		writel(reg, PERI_USB0);*/
	} else { /* host -->device */
		if (otg_usbdev_stat == 1)
			return 0;

		host_to_device();
/*		reg &= ~(USB2_PHY_DPPULL_DOWN);
		reg |= DWC_OTG_EN;
		writel(reg, PERI_USB0);
		reg1 = readl(IO_ADDRESS(0x10080804));
		reg1 &= ~(0x1 << 1);
		writel(reg1, IO_ADDRESS(0x10080804));*/
	}

/*	writel(reg, PERI_USB0); */

	return 0;

}


static int usbotg_thread(void *__unused)
{

#ifdef CONFIG_HIUSB_DEVICE2_0
#ifdef CONFIG_HIUSB_HOST
	do {
		otg_run();
		msleep(1000);

	} while (1);
#else
	host_to_device();
#endif
#endif

	return 0;
}

int usb_otg_init(void)
{

	kusbotg_task = kthread_run(usbotg_thread, NULL, "kusbotg");
	if (!IS_ERR(kusbotg_task))
		return 0;

	printk(KERN_ERR "can't start kusbotg\n");

	return -1;
}

/*
 * Cleanup
 */
static void __exit usb_otg_exit(void)
{
	/* TODO */
}

subsys_initcall(usb_otg_init);
module_exit(usb_otg_exit);
MODULE_LICENSE("GPL");

