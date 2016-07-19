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
	unsigned int old_val, regval;

	old_val = regval = readl(CRG_REG_BASE + REG_FMC_CRG);

	if (!(regval & FMC_CLK_SEL_MASK)) {
		regval &= ~FMC_CLK_SEL_MASK;
		regval |= FMC_CLK_SEL(FMC_CLK_83M);
	}

	if (clk_en)
		regval |= FMC_CLK_ENABLE;
	else
		regval &= ~FMC_CLK_ENABLE;

	if (regval != old_val)
		writel(regval, (CRG_REG_BASE + REG_FMC_CRG));
}

/*****************************************************************************/
void hifmc100_get_fmc_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg;

#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),	FMC_CLK_SEL(FMC_CLK_24M),
		CLK_2X(83),	FMC_CLK_SEL(FMC_CLK_83M),
		CLK_2X(125),	FMC_CLK_SEL(FMC_CLK_125M),
		CLK_2X(150),	FMC_CLK_SEL(FMC_CLK_150M),
		0,		0,
	};
#undef CLK_2X

	clk_reg = FMC_CLK_SEL(FMC_CLK_24M);
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*clock < sysclk[ix])
			break;
		clk_reg = sysclk[ix + 1];
	}

	*clock = clk_reg;
}

