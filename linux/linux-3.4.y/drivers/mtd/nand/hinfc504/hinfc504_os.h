/******************************************************************************
 *    COPYRIGHT (C) 2013 Czyong. Hisilicon
 *    All rights reserved.
 * ***
 *    Create by Czyong 2013-02-07
 *
******************************************************************************/
#ifndef HINFC504_OSH
#define HINFC504_OSH
/******************************************************************************/

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <asm/setup.h>
#include <asm/errno.h>
#include <linux/platform_device.h>
#include <linux/mtd/partitions.h>
#include <mach/clkdev.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <mach/clkdev.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 5))
#  include "../../mtdcore.h"
#endif

/*****************************************************************************/

#define DUMP_DATA(_p, _n) do { \
	int ix; \
	unsigned char *rr = (unsigned char *)(_p); \
	for (ix = 0; ix < _n; ix++) { \
		printk(KERN_INFO "%02X ", rr[ix]); \
		if (!((ix + 1) % 16)) \
			printk("\n"); \
	} \
} while (0)

#define DBG_OUT(fmt, args...) do {\
	printk(KERN_INFO "%s(%d): " fmt, __FILE__, __LINE__, ##args); \
} while (0)

#if 1
#  define DBG_MSG(_fmt, arg...)
#else
#  define DBG_MSG(_fmt, arg...) \
	printk(KERN_INFO "%s(%d): " _fmt, __FILE__, __LINE__, ##arg);
#endif

#define PR_BUG(fmt, args...) do {\
	printk("%s(%d): bug " fmt, __FILE__, __LINE__, ##args); \
	asm("b ."); \
} while (0)

#define PR_MSG(_fmt, arg...) \
	printk(_fmt, ##arg)

/******************************************************************************/
#endif /* HINFC504_OSH */
