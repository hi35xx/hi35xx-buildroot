/******************************************************************************
*    Copyright (c) 2009-2011 by HiC.
*    All rights reserved.
******************************************************************************/

#include "../spi_ids.h"
#include "hisfc350.h"

#define SYSCTRL_SC_PERI_CLKSEL                    (0x0068)
#define SYSCTRL_SC_PERI_CLKSEL_SFC_CLK_PLLL   (0x01)

#define SYSCTRL_SC_PERI_CLKDIV1                   (0x0070)
#define SYSCTRL_SC_PERI_CLKDIV1_SFC_DIV_MASK  (0xF0000000)

#define SYSCTRL_SC_PERI_CLKEN                     (0x0020)
#define SYSCTRL_SC_PERI_CLKEN_SFC_CLK_EN      (0xC00000)

#define hisfc_sys_read(_host, _reg) \
	readl((unsigned)((char *)_host->sysreg + (_reg)))

#define hisfc_sys_write(_host, _reg, _value) \
	writel((unsigned)(_value), (unsigned)((char *)_host->sysreg + (_reg)))

/*****************************************************************************/
static const unsigned int hisfc350_sys_clk[] = {
	/* bus_clock  reg_value  */
	31, 0x70000000,  /* 216/7 */
#define HISFC350_MIN_CLOCK_REG      0x70000000
#define HISFC350_MIN_CLOCK_CLK      31
	36, 0x60000000,  /* 216/6 */
	44, 0x50000000,  /* 216/5 */
	54, 0x40000000,  /* 216/4 */
	72, 0x30000000,  /* 216/3 */
#define HISFC350_MAX_CLOCK_REG       0x30000000
	0, 0,
};

/*****************************************************************************/
void hisfc350_set_system_clock(struct hisfc_host *host,
	struct spi_operation *op, int clk_en)
{
#if 0 /* for fpga */
	unsigned int regval;
	unsigned int clk_reg;

	if (!clk_en) {
		local_irq_disable();
		regval = hisfc_sys_read(host, SYSCTRL_SC_PERI_CLKEN);
		if ((regval & SYSCTRL_SC_PERI_CLKEN_SFC_CLK_EN)) {
			regval &= ~SYSCTRL_SC_PERI_CLKEN_SFC_CLK_EN;
			hisfc_sys_write(host, SYSCTRL_SC_PERI_CLKEN, regval);
		}
		local_irq_enable();
		return;
	}
	local_irq_disable();

	regval = hisfc_sys_read(host, SYSCTRL_SC_PERI_CLKSEL);

	if ((regval & SYSCTRL_SC_PERI_CLKSEL_SFC_CLK_PLLL)
		!= SYSCTRL_SC_PERI_CLKSEL_SFC_CLK_PLLL) {
		regval |= SYSCTRL_SC_PERI_CLKSEL_SFC_CLK_PLLL;
		hisfc_sys_write(host, SYSCTRL_SC_PERI_CLKSEL, regval);
		printk(KERN_INFO "sfc not use PLL clock,"
			" select PLL clock now,"
			" reference PERI_CLKSEL[0]\n");
	}

	regval = hisfc_sys_read(host, SYSCTRL_SC_PERI_CLKEN);
	if ((regval & SYSCTRL_SC_PERI_CLKEN_SFC_CLK_EN)
		!= SYSCTRL_SC_PERI_CLKEN_SFC_CLK_EN) {

		regval |= SYSCTRL_SC_PERI_CLKEN_SFC_CLK_EN;
		hisfc_sys_write(host, SYSCTRL_SC_PERI_CLKEN, regval);
		printk("sfc clock not enable,"
			" enable sfc clock now,"
			" reference PERI_CLKEN[22:23]\n");
	}

	clk_reg = op ? op->clock : HISFC350_MIN_CLOCK_REG;
		regval = hisfc_sys_read(host, SYSCTRL_SC_PERI_CLKDIV1);
	if ((regval & SYSCTRL_SC_PERI_CLKDIV1_SFC_DIV_MASK) != clk_reg) {
		regval &= ~SYSCTRL_SC_PERI_CLKDIV1_SFC_DIV_MASK;
		regval |= clk_reg;
		hisfc_sys_write(host, SYSCTRL_SC_PERI_CLKDIV1, regval);
	}

	local_irq_enable();
#endif
}
/*****************************************************************************/
void hisfc350_get_best_clock(unsigned int *clock)
{
#if 0
	int ix;
	int clk_reg;

	if ((*clock) < HISFC350_MIN_CLOCK_CLK) {
		*clock = HISFC350_MIN_CLOCK_REG;
		return;
	}

	clk_reg = HISFC350_MAX_CLOCK_REG;
	for (ix = 0; hisfc350_sys_clk[ix]; ix += 2) {
		if (*clock < hisfc350_sys_clk[ix])
			break;
		clk_reg = hisfc350_sys_clk[ix+1];
	}
	*clock = clk_reg;
#endif
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
