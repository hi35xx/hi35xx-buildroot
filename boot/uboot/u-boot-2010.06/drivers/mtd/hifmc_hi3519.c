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
void hifmc100_nand_controller_enable(int enable)
{
	unsigned int old_val, regval;

	old_val = regval = readl(CRG_REG_BASE + REG_FMC_CRG);

	regval &= ~FMC_CLK_SEL_MASK;
	regval |= FMC_CLK_SEL(FMC_CLK_200M);

	if (enable)
		regval |= FMC_CLK_ENABLE;
	else
		regval &= ~FMC_CLK_ENABLE;

	regval &= ~FMC_SOFT_RST_REQ;
	if (regval != old_val)
		writel(regval, (CRG_REG_BASE + REG_FMC_CRG));
}

/*****************************************************************************/
void hifmc_set_fmc_system_clock(struct spi_op *op, int clk_en)
{
	unsigned int old_val, regval;

	old_val = regval = readl(CRG_REG_BASE + REG_FMC_CRG);

	regval &= ~FMC_CLK_SEL_MASK;

	if (op && op->clock) {
		regval |= op->clock & FMC_CLK_SEL_MASK;
		FMC_PR(DTR_DB, "\t|||*-get the setting clock value: %#x\n",
				op->clock);
	} else
		regval |= FMC_CLK_SEL(FMC_CLK_24M);	/* Default Clock */

	if (clk_en)
		regval |= FMC_CLK_ENABLE;
	else
		regval &= ~FMC_CLK_ENABLE;

	if (regval != old_val) {
		FMC_PR(DTR_DB, "\t|||*-setting system clock [%#x]%#x\n",
				REG_FMC_CRG, regval);
		writel(regval, (CRG_REG_BASE + REG_FMC_CRG));
	}
}

/*****************************************************************************/
void hifmc_get_fmc_best_2x_clock(unsigned int *clock)
{
	int ix;
	int clk_reg, clk_type;
	const char *str[] = {"12", "37.5", "62.5", "75", "100"};

#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sys_2X_clk[] = {
		CLK_2X(24),	FMC_CLK_SEL(FMC_CLK_24M),
		CLK_2X(75),	FMC_CLK_SEL(FMC_CLK_75M),
		CLK_2X(125),	FMC_CLK_SEL(FMC_CLK_125M),
		CLK_2X(150),	FMC_CLK_SEL(FMC_CLK_150M),
		CLK_2X(200),	FMC_CLK_SEL(FMC_CLK_200M),
		0,		0,
	};
#undef CLK_2X

	clk_type = FMC_CLK_24M;
	clk_reg = FMC_CLK_SEL(clk_type);
	FMC_PR(QE_DBG, "\t|||*-matching flash clock %d\n", *clock);
	for (ix = 0; sys_2X_clk[ix]; ix += 2) {
		if (*clock < sys_2X_clk[ix])
			break;
		clk_reg = sys_2X_clk[ix + 1];
		clk_type = GET_FMC_CLK_TYPE(clk_reg);
		FMC_PR(QE_DBG, "\t||||-select system clock: %sMHz\n",
				str[clk_type]);
	}
#ifdef CONFIG_DTR_MODE_SUPPORT
	FMC_PR(DTR_DB, "best system clock for SDR.\n");
#endif
	FMC_PR(QE_DBG, "\t|||*-matched best system clock: %sMHz\n",
			str[clk_type]);
	*clock = clk_reg;
}

#ifdef CONFIG_DTR_MODE_SUPPORT
/*****************************************************************************/
void hifmc_get_fmc_best_4x_clock(unsigned int *clock)
{
	int ix;
	int clk_reg, clk_type;
	char *const str[] = {"6", "9.25", "31.25", "37.5", "50",
		"62.5", "75", "100"};

#define CLK_4X(_clk)	(((_clk) + 1) >> 2)
	unsigned int sys_4X_clk[] = {
		CLK_4X(24),	FMC_CLK_SEL(FMC_CLK_24M),
		CLK_4X(75),	FMC_CLK_SEL(FMC_CLK_75M),
		CLK_4X(125),	FMC_CLK_SEL(FMC_CLK_125M),
		CLK_4X(150),	FMC_CLK_SEL(FMC_CLK_150M),
		CLK_4X(200),	FMC_CLK_SEL(FMC_CLK_200M),
		CLK_4X(250),	FMC_CLK_SEL(FMC_CLK_250M),
		CLK_4X(300),	FMC_CLK_SEL(FMC_CLK_300M),
		CLK_4X(400),	FMC_CLK_SEL(FMC_CLK_400M),
		0,		0,
	};
#undef CLK_4X
	clk_type = FMC_CLK_24M;
	clk_reg = FMC_CLK_SEL(clk_type);
	FMC_PR(DTR_DB, "\t|||*-matching flash clock %d\n", *clock);
	for (ix = 0; sys_4X_clk[ix]; ix += 2) {
		if (*clock < sys_4X_clk[ix])
			break;
		clk_reg = sys_4X_clk[ix + 1];
		clk_type = GET_FMC_CLK_TYPE(clk_reg);
		FMC_PR(DTR_DB, "\t||||-select system clock: %sMHz\n",
				str[clk_type]);
	}
	FMC_PR(DTR_DB, "best system clock for DTR.\n");
	FMC_PR(DTR_DB, "\t|||*-matched best system clock: %sMHz\n",
			str[clk_type]);
	*clock = clk_reg;
}
/*****************************************************************************/
#endif/* CONFIG_DTR_MODE_SUPPORT */
