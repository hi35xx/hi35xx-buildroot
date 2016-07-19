/******************************************************************************
*    NAND Flash Controller V504 Device Driver
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
* ***
*    written by Czy 2010-11-04
*
******************************************************************************/

#include "hinfc504_os.h"
#include "hinfc504.h"
#include <mach/platform.h>

/*****************************************************************************/
/* For Hi3535 */
#define PERI_CRG				(0x00D0)
#define PERI_CRG_CLK_EN				(1U << 1)
#define PERI_CRG_CLK_SEL_200M			(1U << 2)

/******************************************************************************/

void hinfc504_controller_enable(struct hinfc_host *host, int enable)
{
	unsigned int base = IO_ADDRESS(CRG_REG_BASE);

	unsigned int reg_val = readl(base + PERI_CRG);

	if (enable)
		reg_val |= (PERI_CRG_CLK_EN | PERI_CRG_CLK_SEL_200M);
	else
		reg_val &= ~PERI_CRG_CLK_EN;

	writel(reg_val, (base + PERI_CRG));
}

