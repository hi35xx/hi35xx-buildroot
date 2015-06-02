/******************************************************************************
 *   NAND Flash Controller V301 Device Driver
 *   Copyright (c) 2009-2010 by Hisilicon.
 *   All rights reserved.
 * ***
 *
 ******************************************************************************/
#include <linux/io.h>
#include <linux/delay.h>
#include "hinfc301.h"

/******************************************************************************/

#define PERI_CRG                                (0x0060)
#define PERI_CRG_RST                            (1U << 0)
#define PERI_CRG_CLK_EN                         (1U << 1)
#define PERI_CRG_CLK_SEL_100M                   (1U << 2)

/******************************************************************************/

void hinfc301_controller_enable(struct hinfc_host *host, int enable)
{
	unsigned int reg_val = 0;
	if (enable) {
		/* enable clk, and choose 100MHz */
		reg_val = readl(host->sysreg + PERI_CRG);
		reg_val |= (PERI_CRG_CLK_EN | PERI_CRG_CLK_SEL_100M);
		writel(reg_val, (host->sysreg + PERI_CRG));
		udelay(50);

		/* undo reset */
		reg_val = readl(host->sysreg + PERI_CRG);
		reg_val &= ~PERI_CRG_RST;
		writel(reg_val, (host->sysreg + PERI_CRG));
		udelay(50);
	} else {
		/* reset */
		reg_val = readl(host->sysreg + PERI_CRG);
		reg_val |= PERI_CRG_RST;
		writel(PERI_CRG_RST, (host->sysreg + PERI_CRG));
		udelay(50);
		/* disable clk */
		reg_val = readl(host->sysreg + PERI_CRG);
		reg_val &= ~PERI_CRG_CLK_EN;
		writel(reg_val, (host->sysreg + PERI_CRG));
		udelay(50);
	}
}
