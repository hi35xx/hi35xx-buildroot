#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include <asm/clkdev.h>
#include <linux/io.h>
#include <mach/io.h>
#include <mach/platform.h>
#include <mach/early-debug.h>
#include "clock.h"

int clk_enable(struct clk *clk)
{
	return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
	return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);

unsigned long get_ahbclk_hw(void)
{
	unsigned long fbdiv, pstdiv1, pstdiv2, refdiv;
	unsigned long reg_base, tmp_reg, foutvco, busclk;

	reg_base = IO_ADDRESS(REG_BASE_SCTL);

	tmp_reg = readl(reg_base + REG_SC_APLLFREQCTRL0);
	pstdiv1 = (tmp_reg >> 24) & 0x7;
	pstdiv2 = (tmp_reg >> 27) & 0x7;

	tmp_reg = readl(reg_base + REG_SC_APLLFREQCTRL1);
	refdiv = (tmp_reg >> 12) & 0x3f;
	fbdiv = tmp_reg & 0xfff;

	foutvco = 24000000/refdiv;
	foutvco *= fbdiv;

	busclk = foutvco/(pstdiv1*pstdiv2);

	/* cpu:busclk = 2:1 (3:2) */
	tmp_reg = readl(reg_base);
	tmp_reg = (tmp_reg >> 12) & 0x3;
	if (tmp_reg == 0x10 || tmp_reg == 0x01)	{
		printk(KERN_ERR "bus clk set is error!\n");
		return CONFIG_DEFAULT_BUSCLK;
	}
	busclk >>= 1;

	return busclk;
}
