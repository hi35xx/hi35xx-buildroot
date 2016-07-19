/* linux/drivers/spi/spi-hi3518ev200.c
 *
 * HI3518EV200 SPI Controller driver
 *
 * Copyright (c) 2014 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * History:
 *      3-February-2015 create this file
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi-hisilicon.h>

#include <mach/platform.h>

/* ********** sys reg define *************** */
#define REG_CRG57_MAP		IO_ADDRESS(CRG_REG_BASE + REG_CRG57)
#define CRG57_SSP1_CKEN_SHIFT	(14)
#define CRG57_SSP0_CKEN_SHIFT	(13)
#define CRG57_SSP1_RST_SHIFT	(6)
#define CRG57_SSP0_RST_SHIFT	(5)
#define CRG57_SSP1_CKEN		(0x1 << 14) /* 0:close clk, 1:open clk */
#define CRG57_SSP0_CKEN		(0x1 << 13) /* 0:close clk, 1:open clk */
#define CRG57_SSP1_RST		(0x1 << 6) /* 0:cancel reset, 1:reset */
#define CRG57_SSP0_RST		(0x1 << 5) /* 0:cancel reset, 1:reset */


#define REG_MISC_CTRL1_MAP	IO_ADDRESS(MISC_REG_BASE + REG_MISC_CTRL1)
#define MISC_CTRL1_SSP1_CS_SHIFT	(26)
#define MISC_CTRL1_SSP1_CS		(0x3 << 26) /* 00:cs0, 01:cs1:*/

/* ********************** spi cs ******************************* */
#define HI3518EV200_SPI0_NUM_CS	1
#define HI3518EV200_SPI1_NUM_CS	2

static int hi3518ev200_spi_cfg_cs(s16 bus_num, u8 csn)
{
	unsigned int value;
	hi_msg("\n");
	switch (bus_num) {
	case 0:
		if ((csn + 1) > HI3518EV200_SPI0_NUM_CS) {
			dev_err(NULL, "%s, %s, %d line: error\n",
					__FILE__, __func__, __LINE__);
			return -1;
		}
		break;

	case 1:
		if (csn < HI3518EV200_SPI1_NUM_CS) {
			value = readl(REG_MISC_CTRL1_MAP);
			value &= ~MISC_CTRL1_SSP1_CS;
			value |= (csn << MISC_CTRL1_SSP1_CS_SHIFT);
			writel(value, REG_MISC_CTRL1_MAP); /* set spi1 csn */
		} else {
			dev_err(NULL, "%s, %s, %d line: error\n",
					__FILE__, __func__, __LINE__);
			return -1;
		}
		break;

	default:
		dev_err(NULL, "%s, %s, %d line: error\n",
				__FILE__, __func__, __LINE__);
		return -1;

	}
	return 0;
}

static int hi3518ev200_spi_hw_init_cfg(s16 bus_num)
{
	unsigned int value;

	hi_msg("\n");
	switch (bus_num) {
	case 0:
		value = readl(REG_CRG57_MAP);
		value &= ~CRG57_SSP0_RST;
		value |= CRG57_SSP0_CKEN;
		writel(value, REG_CRG57_MAP); /* open spi0 clk */
		break;

	case 1:
		value = readl(REG_CRG57_MAP);
		value &= ~CRG57_SSP1_RST;
		value |= CRG57_SSP1_CKEN;
		writel(value, REG_CRG57_MAP); /* open spi1 clk */
		break;

	default:
		dev_err(NULL, "%s, %s, %d line: error\n",
				__FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}

static int hi3518ev200_spi_hw_exit_cfg(s16 bus_num)
{
	unsigned int value;

	hi_msg("\n");
	switch (bus_num) {
	case 0:
		value = readl(REG_CRG57_MAP);
		value |= CRG57_SSP0_RST;
		value &= ~CRG57_SSP0_CKEN;
		writel(value, REG_CRG57_MAP); /* close spi0 clk */
		break;

	case 1:
		value = readl(REG_CRG57_MAP);
		value |= CRG57_SSP1_RST;
		value &= ~CRG57_SSP1_CKEN;
		writel(value, REG_CRG57_MAP);/* close spi1 clk */
		break;

	default:
		dev_err(NULL, "%s, %s, %d line: error\n",
				__FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}

int hi_spi_set_platdata(struct hi_spi_platform_data *spd,
		struct platform_device *pdev)
{
	hi_msg("\n");

	if ((spd == NULL) || (pdev == NULL)) {
		dev_err(NULL, "%s (spd || pdev) == NULL\n", __func__);
		return -1;
	}

	switch (pdev->id) {
	case 0:
		spd->num_cs = HI3518EV200_SPI0_NUM_CS;
		break;

	case 1:
		spd->num_cs = HI3518EV200_SPI1_NUM_CS;
		break;

	default:
		dev_err(NULL, "%s bus num error\n", __func__);
		return -1;
	}

	spd->clk_rate = get_bus_clk() / 4;
	spd->cfg_cs = hi3518ev200_spi_cfg_cs;
	spd->hw_init_cfg = hi3518ev200_spi_hw_init_cfg;
	spd->hw_exit_cfg = hi3518ev200_spi_hw_exit_cfg;

	pdev->dev.platform_data = spd;

	return 0;
}
EXPORT_SYMBOL(hi_spi_set_platdata);
