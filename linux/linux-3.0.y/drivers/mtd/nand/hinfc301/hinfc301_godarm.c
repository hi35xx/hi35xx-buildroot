/******************************************************************************
 *   NAND Flash Controller V301 Device Driver
 *   Copyright (c) 2009-2010 by Hisilicon.
 *   All rights reserved.
 * ***
 *
 ******************************************************************************/
#include <linux/io.h>
#include "hinfc301.h"

/******************************************************************************/

#define PERI_CRG                                       (0x00D0)
#define PERI_CRG_CLK_EN                                (1U << 1)
#define PERI_CRG_CLK_SEL_100M                          (1U << 2)

/******************************************************************************/

void hinfc301_controller_enable(struct hinfc_host *host, int enable)
{
	unsigned int reg_val = readl(host->sysreg + PERI_CRG);
	if (enable)
		reg_val |= (PERI_CRG_CLK_EN | PERI_CRG_CLK_SEL_100M);
	else
		reg_val &= ~PERI_CRG_CLK_EN;

	writel(reg_val, (host->sysreg + PERI_CRG));
}
