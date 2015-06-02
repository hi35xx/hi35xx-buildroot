#include <linux/io.h>
#include <linux/platform_device.h>
#include "higmacv300_godarm.h"

#undef GMAC_PRINT

unsigned long mdio_clk_init(void)
{
	unsigned long busclk, gmac_clk, defclk;

	defclk = GMAC_DEFAULT_CLK;

	busclk = get_bus_clk();

	gmac_clk = busclk >> 1;

	if (readl(REG_CRG0_OFFSET + GMAC_CLK_SRST) & GMAC_CLK_DEF_100M)
		return defclk;
	else if (!(readl(REG_CRG0_OFFSET + GMAC_CLK_SRST) & GMAC_CLK_DEF_100M))
		return gmac_clk;
	else
		return 0;
}

int get_clk_csr(unsigned long gmac_clk)
{
	int clk_csr;

	if ((gmac_clk >= 60000000) && (gmac_clk <= 100000000))
		clk_csr = 0;
	else if ((gmac_clk >= 100000000) && (gmac_clk <= 150000000))
		clk_csr = 1;
	else if ((gmac_clk >= 20000000) && (gmac_clk <= 35000000))
		clk_csr = 2;
	else if ((gmac_clk >= 35000000) && (gmac_clk <= 60000000))
		clk_csr = 3;
	else if ((gmac_clk >= 150000000) && (gmac_clk <= 250000000))
		clk_csr = 4;
	else if ((gmac_clk >= 250000000) && (gmac_clk <= 300000000))
		clk_csr = 5;
	else
		clk_csr = -1;

	clk_csr = 5;

	return clk_csr;
}

void reset_mac_interface_single(void *addr, u32 val)
{
	unsigned int reg;

	reg = readl(addr + 0xcc);
	reg |= 0x20;
	writel(reg, addr + 0xcc);

	writel(val, addr + 0xec);
	readl(addr + 0xec);

	reg &= ~0x20;
	writel(reg, addr + 0xcc);
}
