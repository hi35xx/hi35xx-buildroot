/******************************************************************************
*    NAND Flash Controller V610 Device Driver
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
* ***
*
******************************************************************************/
#include <mach/platform.h>

#include "hinfc610.h"
#include "hinfc610_os.h"

/******************************************************************************/
#define HINFC610_PERIPHERY_REGBASE		IO_ADDRESS(0x20030000)

#define PERI_CRG52                              (0x00D0)
#define PERI_CRG52_CLK_EN			(1U << 1)
#define PERI_CRG52_CLK_SEL_198M			(1U << 2)

/******************************************************************************/

void hinfc610_controller_enable(struct hinfc_host *host, int enable)
{
	unsigned int reg_val = readl(HINFC610_PERIPHERY_REGBASE + PERI_CRG52);

	if (enable)
		reg_val |= (PERI_CRG52_CLK_EN | PERI_CRG52_CLK_SEL_198M);
	else
		reg_val &= ~PERI_CRG52_CLK_EN;

	writel(reg_val, (HINFC610_PERIPHERY_REGBASE + PERI_CRG52));
}

