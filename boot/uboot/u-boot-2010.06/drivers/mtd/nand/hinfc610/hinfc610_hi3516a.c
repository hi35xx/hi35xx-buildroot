/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
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

#include <nand.h>
#include <asm/io.h>
#include "hinfc610.h"

/******************************************************************************/
#define HINFC610_PERIPHERY_REGBASE                  (0x20030000)

#define PERI_CRG52                                  (0x00D0)
#define PERI_CRG52_CLK_EN                           (1U << 1)
#define PERI_CRG52_CLK_SEL_198M                     (1U << 2)

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

