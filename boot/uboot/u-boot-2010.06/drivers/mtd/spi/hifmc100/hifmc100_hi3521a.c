/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#define GET_FMC_BOOT_MODE ({ \
	unsigned int boot_mode = 0; \
	boot_mode = readl(SYS_CTRL_REG_BASE + REG_SYSSTAT); \
	boot_mode &= SPI_NOR_ADDRESS_MODE_MASK; \
	boot_mode; })

/*****************************************************************************/
void hifmc_set_fmc_system_clock(struct spi_op *op, int clk_en)
{
	unsigned int old_val, regval = readl(CRG_REG_BASE + FMC_CRG29);

	old_val = regval;

	regval &= ~FMC_CLK_SEL_MASK;
	regval |= FMC_CLK_SEL_24M;		/* Default Clock */

	if (op && op->clock) {
		regval &= ~FMC_CLK_SEL_MASK;
		regval |= op->clock & FMC_CLK_SEL_MASK;
	}

	if (clk_en)
		regval |= FMC_CRG29_CLK_EN;
	else
		regval &= ~FMC_CRG29_CLK_EN;

	if (regval != old_val)
		writel(regval, (CRG_REG_BASE + FMC_CRG29));
}

/*****************************************************************************/
static void hifmc_get_fmc_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg;

#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),	FMC_CLK_SEL_24M,
		CLK_2X(83),	FMC_CLK_SEL_83M,
		CLK_2X(150),	FMC_CLK_SEL_150M,
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

