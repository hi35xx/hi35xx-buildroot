#include "stmmac.h"
#include "sys_godarm.h"

unsigned long get_tnkclk(unsigned long busclk)
{
	unsigned long tnkclk;

	if (readl(SYS_CTRL_BASE + TOE_CLK_SRST) & TOE_CLK_DEF_100M)
		tnkclk = TOE_DEFAULT_CLK;
	else if (!(readl(SYS_CTRL_BASE + TOE_CLK_SRST) & TOE_CLK_DEF_100M))
		tnkclk = busclk >> 1;
	else
		return -1;

	return tnkclk;
}

void reset_mac_interface_single(int val)
{
	unsigned int reg;

	reg = readl(SYS_CTRL_BASE + TOE_CLK_SRST);
	reg |= 0x20;
	writel(reg, SYS_CTRL_BASE + TOE_CLK_SRST);

	writel(val, SYS_CTRL_BASE + TOE_MAC_INTERFACE);
	readl(SYS_CTRL_BASE + TOE_MAC_INTERFACE);

	reg &= ~0x20;
	writel(reg, SYS_CTRL_BASE + TOE_CLK_SRST);
}
