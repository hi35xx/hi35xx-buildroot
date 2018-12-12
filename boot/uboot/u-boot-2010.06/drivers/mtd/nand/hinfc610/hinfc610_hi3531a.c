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

