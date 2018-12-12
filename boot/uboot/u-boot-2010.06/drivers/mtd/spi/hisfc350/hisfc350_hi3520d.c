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

#define HISFC350_CRG_BASE			(CRG_REG_BASE)
#define HISFC350_CRG48				(0x00C0)
#define HISFC350_CRG48_RST			(1 << 0)
#define HISFC350_CRG48_CLKEN			(1 << 1)
#define HISFC350_CRG48_CLK_24M			(0 << 2) /* 24M */
#define HISFC350_CRG48_CLK_75M		((0 << 3) | (1 << 2)) /* 75M */
#define HISFC350_CRG48_CLK_112M		((1 << 3) | (1 << 2)) /* 112M */
#define SFC_ADDR_MODE_MASK (0x8000)

#define GET_SFC_ADDR_MODE ({\
	int start_up_mode = 0;\
	start_up_mode = readl(REG_BASE_SCTL + SFC_ADDR_MODE_REG);\
	start_up_mode &= SFC_ADDR_MODE_MASK;\
	start_up_mode; })

/*****************************************************************************/
static void hisfc350_set_system_clock(struct hisfc_host *host,
	struct spi_operation *op, int clk_en)
{
	unsigned int regval = HISFC350_CRG48_CLK_24M;

	if (op && op->clock)
		regval = (op->clock & 0xc);

	if (clk_en)
		regval |= HISFC350_CRG48_CLKEN;

	if (readl(host->sysreg + HISFC350_CRG48) != regval)
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
		CLK_2X(74),	HISFC350_CRG48_CLK_75M,
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
static char *hisfc350_get_clock_str(unsigned int clk_reg)
{
	static char buffer[40];

	/* calculate reference PERI_CLKDIV1[31:28] */
	clk_reg = 216 / ((clk_reg >> 28) & 0xF);
	sprintf(buffer, "%dM", clk_reg);

	return buffer;
}
#endif /* CONFIG_HISFC350_SHOW_CYCLE_TIMING */
