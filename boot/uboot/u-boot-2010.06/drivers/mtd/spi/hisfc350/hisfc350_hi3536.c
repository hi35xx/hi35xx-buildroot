/******************************************************************************
*    Copyright (c) 2009-2011 by Hisilicon.
*    All rights reserved.
******************************************************************************/

#define HISFC350_CRG48		(0x00C0)
#define HISFC350_CRG48_RST	(1 << 0)
#define HISFC350_CRG48_CLKEN	(1 << 1)
#define HISFC350_CRG48_CLK_24M	(0 << 2) /* bit[3:2]=00b 24M */
#define HISFC350_CRG48_CLK_83M	((0 << 3) | (1 << 2)) /* bit[3:2]=01b 83M */
#define HISFC350_CRG48_CLK_125M	((1 << 3) | (1 << 2)) /* bit[3:2]=1xb 125M */
#define SFC_ADDR_MODE_MASK	(0x80)
#define SFC_CLSEL_MASK		(0xC) /* bit[3:2] clk Mask */
#define SFC_PERI_CLKDIV1_SHIFT	(28)
#define SFC_PERI_CLKDIV1_MASK	(0xF)
#define GET_SFC_ADDR_MODE ({\
	int start_up_mode = 0;\
	start_up_mode = readl(REG_BASE_SCTL + SFC_ADDR_MODE_REG);\
	start_up_mode &= SFC_ADDR_MODE_MASK;\
	start_up_mode; })

/*****************************************************************************/
static void hisfc350_set_system_clock(struct hisfc_host *host,
	struct spi_operation *op, int clk_en)
{
	unsigned int regval = readl(host->sysreg + HISFC350_CRG48);

	regval = regval & (~SFC_CLSEL_MASK);
	if (op && op->clock)
		regval = regval | (op->clock & SFC_CLSEL_MASK);
	else
		regval = regval | HISFC350_CRG48_CLK_24M; /* Default Clock */

	if (clk_en)
		regval |= HISFC350_CRG48_CLKEN;

	if (regval != readl(host->sysreg + HISFC350_CRG48))
		writel(regval, (host->sysreg + HISFC350_CRG48));

}

/*****************************************************************************/
static void hisfc350_get_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg;
#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),	HISFC350_CRG48_CLK_24M,
		CLK_2X(83),	HISFC350_CRG48_CLK_83M,
		CLK_2X(125),	HISFC350_CRG48_CLK_125M,
		0, 0,
	};
#undef CLK_2X

	clk_reg = HISFC350_CRG48_CLK_24M;
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*clock < sysclk[ix])
			break;
		clk_reg = sysclk[ix + 1];
	}

	*clock = clk_reg;
}


/*****************************************************************************/
#ifdef CONFIG_HISFC350_SHOW_CYCLE_TIMING
static char *hisfc350_get_clock_str(unsigned int clk_reg)
{
	static char buffer[40];

	/* calculate reference PERI_CLKDIV1[31:28] */
	clk_reg = 216 / ((clk_reg >> SFC_PERI_CLKDIV1_SHIFT)
					& SFC_PERI_CLKDIV1_MASK);
	sprintf(buffer, "%dM", clk_reg);

	return buffer;
}
#endif /* CONFIG_HISFC350_SHOW_CYCLE_TIMING */
