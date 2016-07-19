/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/
#include <mach/io.h>

/*****************************************************************************/
void hifmc_set_nand_system_clock(struct spi_op *op, int clk_en)
{
	unsigned int old_val, regval;

	old_val = regval = readl(__io_address(CRG_REG_BASE + REG_FMC_CRG));

	regval &= ~FMC_CLK_SEL_MASK;

	if (op && op->clock)
		regval |= op->clock & FMC_CLK_SEL_MASK;
	else
		regval |= FMC_CLK_SEL(FMC_CLK_24M);	/* Default Clock */

	if (clk_en)
		regval |= FMC_CLK_ENABLE;
	else
		regval &= ~FMC_CLK_ENABLE;

	if (regval != old_val)
		writel(regval, __io_address(CRG_REG_BASE + REG_FMC_CRG));
}

/*****************************************************************************/
void hifmc_get_nand_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg, clk_type;
	static const char const *str[] = {"12", "74.25", "62.5", "99"};

#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),	FMC_CLK_SEL(FMC_CLK_24M),
		CLK_2X(125),	FMC_CLK_SEL(FMC_CLK_125M),
		CLK_2X(148),	FMC_CLK_SEL(FMC_CLK_148M),
		CLK_2X(198),	FMC_CLK_SEL(FMC_CLK_198M),
		0,		0,
	};
#undef CLK_2X

	clk_type = FMC_CLK_24M;
	clk_reg = FMC_CLK_SEL(clk_type);
	FMC_PR(QE_DBG, "\t|||*-matching flash clock %d\n", *clock);
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*clock < sysclk[ix])
			break;
		clk_reg = sysclk[ix + 1];
		clk_type = GET_FMC_CLK_TYPE(clk_reg);
		FMC_PR(QE_DBG, "\t||||-select system clock: %sMHz\n",
				str[clk_type]);
	}

	FMC_PR(QE_DBG, "\t|||*-matched best system clock: %sMHz\n",
			str[clk_type]);
	*clock = clk_reg;
}

