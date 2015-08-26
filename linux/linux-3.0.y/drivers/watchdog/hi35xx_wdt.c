/*
 * drivers/char/watchdog/hi35xx_wdt.c
 *
 * Watchdog driver for Hisilicon HI35xx SoC
 *
 * Author: Watson Xu <xuhuashan@gmail.com>
 *
 * Copyright 2004 (c) MontaVista, Software, Inc.
 * Based on sa1100 driver, Copyright (C) 2000 Oleg Drokin <green@crimea.edu>
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/bitops.h>
#include <linux/uaccess.h>
#include <mach/hardware.h>

#define DRV_NAME		"hi35xx_wdt"

#define WDT_IO_BASE		WDG_BASE
#define WDT_IO_SIZE		0x1000

#define WDG_LOAD		0x000
#define WDG_VALUE		0x004
#define WDG_CONTROL		0x008
#define WDG_INTCLR		0x00C
#define WDG_RIS			0x010
#define WDG_MIS			0x014
#define WDG_LOCK		0xC00

#define LOCK			0x00000000
#define UNLOCK			0x1ACCE551

#define INTEN			(1 << 0)
#define RESEN			(1 << 1)

static int nowayout = WATCHDOG_NOWAYOUT;
static int heartbeat = 60;	/* (secs) Default is 1 minute */
static unsigned long wdt_status;
static void __iomem *wdt_base;
static DEFINE_SPINLOCK(wdt_lock);

#define WDT_TICK_RATE		(3000000UL)

#define	WDT_IN_USE		0
#define	WDT_OK_TO_CLOSE		1

static void wdt_enable(void)
{
	__u32 load_val = heartbeat * WDT_TICK_RATE / 2;

	spin_lock(&wdt_lock);

	writel(UNLOCK, wdt_base + WDG_LOCK);
	writel(load_val, wdt_base + WDG_LOAD);
	writel(INTEN | RESEN, wdt_base + WDG_CONTROL);
	writel(LOCK, wdt_base + WDG_LOCK);

	spin_unlock(&wdt_lock);
}

static void wdt_disable(void)
{
	spin_lock(&wdt_lock);

	writel(UNLOCK, wdt_base + WDG_LOCK);
	writel(0, wdt_base + WDG_INTCLR);
	writel(0, wdt_base + WDG_CONTROL);
	writel(LOCK, wdt_base + WDG_LOCK);

	spin_unlock(&wdt_lock);
}

static void wdt_keepalive(void)
{
	spin_lock(&wdt_lock);

	writel(UNLOCK, wdt_base + WDG_LOCK);
	writel(0, wdt_base + WDG_INTCLR);
	writel(LOCK, wdt_base + WDG_LOCK);

	spin_unlock(&wdt_lock);
}

static int hi35xx_wdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(WDT_IN_USE, &wdt_status))
		return -EBUSY;

	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);
	wdt_enable();
	return nonseekable_open(inode, file);
}

static ssize_t
hi35xx_wdt_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
	if (len) {
		if (!nowayout) {
			size_t i;

			clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

			for (i = 0; i != len; i++) {
				char c;

				if (get_user(c, data + i))
					return -EFAULT;
				if (c == 'V')
					set_bit(WDT_OK_TO_CLOSE, &wdt_status);
			}
		}
		wdt_keepalive();
	}
	return len;
}

static const struct watchdog_info ident = {
	.options =		WDIOF_MAGICCLOSE |
				WDIOF_SETTIMEOUT |
				WDIOF_KEEPALIVEPING,
	.firmware_version =	1,
	.identity		= "HI35xx Watchdog",
};


static long hi35xx_wdt_ioctl(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	int ret = -ENOTTY;
	int time;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		ret = copy_to_user((struct watchdog_info *)arg, &ident,
				   sizeof(ident)) ? -EFAULT : 0;
		break;

	case WDIOC_GETSTATUS:
		ret = put_user(0, (int *)arg);
		break;

	case WDIOC_KEEPALIVE:
		wdt_keepalive();
		ret = 0;
		break;

	case WDIOC_SETTIMEOUT:
		ret = get_user(time, (int *)arg);
		if (ret)
			break;

		if (time <= 0 || time > 1431) {
			ret = -EINVAL;
			break;
		}

		heartbeat = time;
		wdt_enable();
		/* Fall through */

	case WDIOC_GETTIMEOUT:
		ret = put_user(heartbeat, (int *)arg);
		break;
	}
	return ret;
}

static int hi35xx_wdt_release(struct inode *inode, struct file *file)
{
	if (test_bit(WDT_OK_TO_CLOSE, &wdt_status))
		wdt_disable();
	else
		printk(KERN_CRIT "WATCHDOG: Device closed unexpectedly - "
					"timer will not stop\n");
	clear_bit(WDT_IN_USE, &wdt_status);
	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

	return 0;
}


static const struct file_operations hi35xx_wdt_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= hi35xx_wdt_write,
	.unlocked_ioctl	= hi35xx_wdt_ioctl,
	.open		= hi35xx_wdt_open,
	.release	= hi35xx_wdt_release,
};

static struct miscdevice hi35xx_wdt_miscdev = {
	.minor		= WATCHDOG_MINOR,
	.name		= "watchdog",
	.fops		= &hi35xx_wdt_fops,
};

static int __init hi35xx_wdt_init(void)
{
	int ret;

	spin_lock_init(&wdt_lock);

	if (!request_mem_region(WDT_IO_BASE, WDT_IO_SIZE, DRV_NAME)) {
		printk(KERN_ERR "cannot request memory region\n");
		ret = -ENOENT;
		goto err;
	}

	wdt_base = ioremap(WDT_IO_BASE, WDT_IO_SIZE);
	if (!wdt_base) {
		printk(KERN_ERR "ioremap fail\n");
		ret = -ENOMEM;
		goto err_ioremap;
	}

	ret = misc_register(&hi35xx_wdt_miscdev);
	if (ret < 0) {
		printk(KERN_ERR "cannot register misc device\n");
		goto err_misc_register;
	}

	printk(KERN_INFO "HI35xx Watchdog Timer: heartbeat %d sec\n",
			heartbeat);

	return 0;

err_misc_register:
	iounmap(wdt_base);
err_ioremap:
	release_mem_region(WDT_IO_BASE, WDT_IO_SIZE);
err:
	return ret;
}

static void __exit hi35xx_wdt_exit(void)
{
	misc_deregister(&hi35xx_wdt_miscdev);
	iounmap(wdt_base);
	release_mem_region(WDT_IO_BASE, WDT_IO_SIZE);
}


module_init(hi35xx_wdt_init);
module_exit(hi35xx_wdt_exit);

MODULE_AUTHOR("Watson Xu <xuhuashan@gmail.com>");
MODULE_DESCRIPTION("Hisilicon HI35xx SoC Watchdog");

module_param(heartbeat, int, 0);
MODULE_PARM_DESC(heartbeat, "Watchdog heartbeat in seconds (default 60s)");

module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started");

MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);

