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

#include <common.h>
#include <asm/io.h>
#include <linux/mtd/nand.h>
#include <hinfc_common.h>
#include "hisnfc100_spi_ids.h"
#include "hisnfc100.h"

/*****************************************************************************/
#define HISNFC100_CRG48					0xc0
#define HISNFC100_CRG48_SPI_NAND_CLK_SEL(_clk)		(((_clk) & 0x3) << 6)
#define HISNFC100_CRG48_SPI_NAND_CLK_EN			(1 << 5)
#define HISNFC100_CRG48_SPI_NAND_SOFT_RST_REQ		(1 << 4)

#define SPI_NAND_CLK_SEL_MASK				(0x3 << 6)

#define CLK_24M						0x0
#define CLK_83M						0x1
#define CLK_125M					0x2

#define SPI_NAND_CLK_SEL_24M	HISNFC100_CRG48_SPI_NAND_CLK_SEL(CLK_24M)
#define SPI_NAND_CLK_SEL_83M	HISNFC100_CRG48_SPI_NAND_CLK_SEL(CLK_83M)
#define SPI_NAND_CLK_SEL_125M	HISNFC100_CRG48_SPI_NAND_CLK_SEL(CLK_125M)

/*****************************************************************************/
void hisnfc100_set_system_clock(struct hisnfc_host *host, int clk_en)
{
	unsigned regval = readl(CRG_REG_BASE + HISNFC100_CRG48);

	if (!(regval & SPI_NAND_CLK_SEL_MASK))
			regval |= SPI_NAND_CLK_SEL_83M;

	if (clk_en)
		regval |= HISNFC100_CRG48_SPI_NAND_CLK_EN;
	else
		regval &= ~HISNFC100_CRG48_SPI_NAND_CLK_EN;

	if (readl(CRG_REG_BASE + HISNFC100_CRG48) != regval)
		writel(regval, (CRG_REG_BASE + HISNFC100_CRG48));
}

/*****************************************************************************/
void hisnfc100_get_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg;
#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),		SPI_NAND_CLK_SEL_24M,
		CLK_2X(83),		SPI_NAND_CLK_SEL_83M,
		CLK_2X(125),	SPI_NAND_CLK_SEL_125M,
		0, 0,
	};
#undef CLK_2X

	clk_reg = SPI_NAND_CLK_SEL_24M;
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*clock < sysclk[ix])
			break;
		clk_reg = sysclk[ix + 1];
	}

	*clock = clk_reg;
}

