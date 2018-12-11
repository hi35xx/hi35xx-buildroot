/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __HIFMC100_NAND_OS_H__
#define __HIFMC100_NAND_OS_H__

/*****************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>
#include <asm/errno.h>
#include <asm/setup.h>
#include <linux/io.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

#include <asm/clkdev.h>
#include <linux/resource.h>
#include <linux/clk.h>
#include <linux/clkdev.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 5))
	#include "../../mtdcore.h"
#endif

/*****************************************************************************/
#define DEFAULT_NAND_PAGESIZE	2048
#define DEFAULT_NAND_OOBSIZE	64

#define NAND_BUFFER_LEN		(DEFAULT_NAND_PAGESIZE + DEFAULT_NAND_OOBSIZE)

/*****************************************************************************/
#ifndef CONFIG_HIFMC100_MAX_NAND_CHIP
	#define CONFIG_HIFMC100_MAX_NAND_CHIP	(1)
	#warning NOT config CONFIG_HIFMC100_MAX_NAND_CHIP, \
		used default value, maybe invalid.
#endif

#ifndef CONFIG_RW_H_WIDTH
	#define CONFIG_RW_H_WIDTH	(10)
	#warning NOT config CONFIG_RW_H_WIDTH, used default value, maybe invalid.
#endif

#ifndef CONFIG_R_L_WIDTH
	#define CONFIG_R_L_WIDTH	(10)
	#warning NOT config CONFIG_R_L_WIDTH, used default value, maybe invalid.
#endif

#ifndef CONFIG_W_L_WIDTH
	#define CONFIG_W_L_WIDTH	(10)
	#warning NOT config CONFIG_W_L_WIDTH, used default value, maybe invalid.
#endif

extern void hifmc100_nand_controller_enable(int enable);

#endif /* End of __HIFMC100_NAND_OS_H__ */
