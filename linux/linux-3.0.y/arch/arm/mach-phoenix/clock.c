#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include <linux/clkdev.h>
#include <linux/io.h>
#include <mach/io.h>
#include <mach/platform.h>
#include <mach/early-debug.h>
#include "clock.h"

static unsigned long hil_def_ahbclk = CONFIG_DEFAULT_BUSCLK;
static unsigned long set_busclk;

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

static int __init hil_setup_bus_clock(char *s)
{
	int ret;

	ret = strict_strtoul(s, 0, &hil_def_ahbclk);
	if (ret < 0)
		printk(KERN_WARNING "conver string err.%s->%d",
				__func__, __LINE__);

	set_busclk = 1;

	return 0;
}
__setup("busclk=", hil_setup_bus_clock);

unsigned long get_ahbclk_hw(void)
{
	if (set_busclk)
		return hil_def_ahbclk;

	/* read clk from PLL */
	return CONFIG_DEFAULT_BUSCLK;
}

