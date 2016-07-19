/******************************************************************************
 *    SPI Nand Flash Controller V100 Device Driver
 *    Copyright (c) 2014 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Hisilicon
 *
******************************************************************************/

#ifndef HISNFC100_OSH
#define HISNFC100_OSH

/*****************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <asm/errno.h>
#include <asm/setup.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mtd/nand.h>

#include <mach/clkdev.h>
#include <mach/platform.h>

#include <linux/clk.h>
#include <linux/clkdev.h>

#include "../hinfc_common.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 5))
	#include "../../mtdcore.h"
#endif

/*****************************************************************************/
#ifndef CONFIG_HISNFC100_REG_BASE_ADDRESS
	#define CONFIG_HISNFC100_REG_BASE_ADDRESS            (0x10040000)
	#warning NOT config CONFIG_HISNFC100_REG_BASE_ADDRESS,\
	used default value, maybe invalid.
#endif /* CONFIG_HISNFC100_REG_BASE_ADDRESS */

#define HISNFC100_REG_BASE_ADDRESS_LEN	0x100

#ifndef CONFIG_HISNFC100_BUFFER_BASE_ADDRESS
#  define CONFIG_HISNFC100_BUFFER_BASE_ADDRESS         (0x54000000)
#  warning NOT config CONFIG_HISNFC100_BUFFER_BASE_ADDRESS,\
	used default value, maybe invalid.
#endif /* CONFIG_HISNFC100_BUFFER_BASE_ADDRESS */

#define HISNFC100_BUFFER_BASE_ADDRESS_LEN		(2048 + 256)

#ifndef CONFIG_HISNFC100_MAX_CHIP
#  define CONFIG_HISNFC100_MAX_CHIP                    (1)
#  warning NOT config CONFIG_HISNFC100_MAX_CHIP, \
	used default value, maybe invalid.
#endif /* CONFIG_HISNFC100_MAX_CHIP */

/*****************************************************************************/
#define PR_BUG(fmt, args...) do { \
	pr_info("%s(%d): bug " fmt, __FILE__, __LINE__, ##args); \
	asm("b ."); \
} while (0)

#if 1
#  define DBG_MSG(_fmt, arg...)
#else
#  define DBG_MSG(_fmt, arg...) \
	printk(KERN_INFO "%s(%d): " _fmt, __FILE__, __LINE__, ##arg);
#endif

#endif /* HISNFC100_OSH */

