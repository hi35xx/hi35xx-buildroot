/******************************************************************************
*    Copyright (c) 2009-2011 by HiC.
*    All rights reserved.
******************************************************************************/

#include "../spi_ids.h"
#include "hisfc350.h"
#include <linux/io.h>
#define HISFC350_CRG48				(0x00C0)
#define HISFC350_CRG48_RST			(1 << 0)
#define HISFC350_CRG48_CLKEN			(1 << 1)
#define HISFC350_CRG48_CLK_24M			(0 << 2) /* 24M */
#define HISFC350_CRG48_CLK_112M			(1 << 2) /* 112.5M */
#define SYS_CTRL_BASE				(0x20050000)
#define SFC_ADDR_MODE_REG			(0x8c)
#define SFC_ADDR_MODE_MASK			(0x10000)
/*****************************************************************************/
#define GET_SFC_ADDR_MODE ({\
	int start_up_mode = 0;\
	start_up_mode = readl(IO_ADDRESS(SYS_CTRL_BASE + SFC_ADDR_MODE_REG));\
	start_up_mode &= SFC_ADDR_MODE_MASK;\
	start_up_mode; })
/*****************************************************************************/
void hisfc350_set_system_clock(struct hisfc_host *host,
	struct spi_operation *op, int clk_en)
{
	unsigned int regval = HISFC350_CRG48_CLK_24M;

	if (op && op->clock)
		regval = (op->clock & 0x4);

	if (clk_en)
		regval |= HISFC350_CRG48_CLKEN;

	if (readl(host->sysreg + HISFC350_CRG48) != regval)
		writel(regval, (host->sysreg + HISFC350_CRG48));
}
/*****************************************************************************/
void hisfc350_get_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg;

#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),	HISFC350_CRG48_CLK_24M,
		CLK_2X(112),	HISFC350_CRG48_CLK_112M,
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
char *hisfc350_get_clock_str(unsigned int clk_reg)
{
	static char buffer[40];

	/* calculate reference PERI_CLKDIV1[31:28] */
	clk_reg = 216 / ((clk_reg >> 28) & 0xF);
	sprintf(buffer, "%dM", clk_reg);

	return buffer;
}
#endif /* CONFIG_HISFC350_SHOW_CYCLE_TIMING */
