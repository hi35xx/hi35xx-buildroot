/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
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
#include <asm/arch/platform.h>
#include <hifmc_common.h>

#include "hifmc_spi_ids.h"

/*****************************************************************************/
void hifmc_set_fmc_system_clock(struct spi_op *op, int clk_en)
{
	unsigned int old_val, regval;
	unsigned int eco_reserv;
	unsigned int mux_io_reg;
	unsigned int mux_io_tmp;
	const char *str[] = {"12", "99"};

	old_val = regval = readl(CRG_REG_BASE + FMC_CRG48);

	regval &= ~FMC_CLK_SEL_MASK;

	if (op && op->clock)
		regval |= op->clock & FMC_CLK_SEL_MASK;
	else {
		eco_reserv = readl(CRG_REG_BASE + FMC_CRG65);
		mux_io_reg = GET_FMC_MUXIO_CLK(eco_reserv);

		/*reset the mux_IO to 24MHz if mux_IO was setted to 99MHz*/
		if (mux_io_reg) {
			FMC_PR(BT_DBG, "\t||-get mux_IO clock: %sMHz\n",
					str[mux_io_reg]);
			mux_io_tmp = FMC_MUXIO_CLK_SEL(mux_io_reg);
			mux_io_reg = GET_FMC_MUXIO_CLK(mux_io_tmp);
			writel(mux_io_reg, (CRG_REG_BASE + FMC_CRG65));
			FMC_PR(BT_DBG, "\t||-set mux_IO clock: %sMHz\n",
					str[mux_io_reg]);
		}
		regval |= FMC_CLK_SEL_24M;	/* Default Clock */
	}
	if (clk_en)
		regval |= FMC_CRG48_CLK_EN;
	else
		regval &= ~FMC_CRG48_CLK_EN;

	if (regval != old_val)
		writel(regval, (CRG_REG_BASE + FMC_CRG48));
}

/*****************************************************************************/
void hifmc_get_fmc_best_2x_clock(unsigned int *clock)
{
	int ix;
	int clk_reg, clk_type;
	const char *str[] = {"12", "74.25", "62.5", "99"};

#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),	FMC_CLK_SEL_24M,
		CLK_2X(125),	FMC_CLK_SEL_125M,
		CLK_2X(148),	FMC_CLK_SEL_148_5M,
		CLK_2X(198),	FMC_CLK_SEL_198M,
		0,		0,
	};
#undef CLK_2X

	clk_reg = FMC_CLK_SEL_24M;
	clk_type = GET_FMC_CLK_TYPE(clk_reg);
	FMC_PR(QE_DBG, "\t|*-matching flash clock %d\n", *clock);
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*clock < sysclk[ix])
			break;
		clk_reg = sysclk[ix + 1];
		clk_type = GET_FMC_CLK_TYPE(clk_reg);
		FMC_PR(QE_DBG, "\t||-select system clock: %sMHz\n",
				str[clk_type]);
	}

	FMC_PR(QE_DBG, "\t|*-matched best system clock: %sMHz\n",
			str[clk_type]);
	*clock = clk_reg;
}

