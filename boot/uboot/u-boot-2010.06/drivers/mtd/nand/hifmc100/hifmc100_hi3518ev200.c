/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
void hifmc100_set_fmc_system_clock(struct hifmc_host *host, int clk_en)
{
	unsigned int old_val, regval = readl(CRG_REG_BASE + FMC_CRG48);

	old_val = regval;

	if (!(regval & FMC_CLK_SEL_MASK)) {
		regval &= ~FMC_CLK_SEL_MASK;
		regval |= FMC_CLK_SEL_125M;
	}

	if (clk_en)
		regval |= FMC_CRG48_CLK_EN;
	else
		regval &= ~FMC_CRG48_CLK_EN;

	if (regval != old_val)
		writel(regval, (CRG_REG_BASE + FMC_CRG48));
}

/*****************************************************************************/
void hifmc100_get_fmc_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg;

#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24), FMC_CLK_SEL_24M,
		CLK_2X(125), FMC_CLK_SEL_125M,
		CLK_2X(148), FMC_CLK_SEL_148_5M,
		CLK_2X(198), FMC_CLK_SEL_198M,
		0,		0,
	};
#undef CLK_2X

	clk_reg = FMC_CLK_SEL_24M;
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*clock < sysclk[ix])
			break;
		clk_reg = sysclk[ix + 1];
	}

	*clock = clk_reg;
}

