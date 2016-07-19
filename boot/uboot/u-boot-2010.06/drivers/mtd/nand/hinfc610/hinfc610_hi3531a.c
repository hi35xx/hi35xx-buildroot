/******************************************************************************
*    NAND Flash Controller V610 Device Driver
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <nand.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include "hinfc610.h"

/****************************************************************************/
void hinfc610_controller_enable(struct hinfc_host *host, int enable)
{
	unsigned int regval = readl(CRG_REG_BASE + REG_NFC_CRG);

	if (enable)
		regval |= (NFC_CLK_ENABLE | NFC_CLK_SEL(NFC_CLK_200M));
	else
		regval &= ~NFC_CLK_ENABLE;

	writel(regval, (CRG_REG_BASE + REG_NFC_CRG));
}

