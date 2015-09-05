#include "stmmac.h"
#include "sys_godnet.h"

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

void reset_mac_interface_dual(int val)
{
	writel(1, SYS_CTRL_BASE + TOE_RST);
	writel(val, SYS_CTRL_BASE + TOE_MAC_INTERFACE);
	readl(SYS_CTRL_BASE + TOE_MAC_INTERFACE);
	writel(0, SYS_CTRL_BASE + TOE_RST);
}

