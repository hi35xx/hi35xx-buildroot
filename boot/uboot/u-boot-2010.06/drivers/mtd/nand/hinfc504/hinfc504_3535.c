
/******************************************************************************
*    NAND Flash Controller V600 Device Driver
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <nand.h>
#include <asm/io.h>
#include "hinfc504.h"

/******************************************************************************/

#define HINFC504_PERIPHERY_REGBASE                  (0x20030000)

#define PERI_CRG52                                  (0x00D0)
#define PERI_CRG52_CLK_EN	                    (1U << 1)
#define PERI_CRG52_CLK_SEL_200M                     (1U << 2)

/******************************************************************************/

void hinfc504_controller_enable(struct hinfc_host *host, int enable)
{
	unsigned int reg_val = readl(HINFC504_PERIPHERY_REGBASE + PERI_CRG52);
	if (enable)
		reg_val |= (PERI_CRG52_CLK_EN | PERI_CRG52_CLK_SEL_200M);
	else
		reg_val &= ~PERI_CRG52_CLK_EN;

	writel(reg_val, (HINFC504_PERIPHERY_REGBASE + PERI_CRG52));
}

