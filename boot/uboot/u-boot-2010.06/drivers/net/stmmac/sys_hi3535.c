#include "stmmac.h"
#include "sys_hi3535.h"

unsigned long get_tnkclk(unsigned long busclk)
{
	unsigned long tnkclk;

	if (readl(SYS_CTRL_BASE + TOE_RST) & TOE_CLK_DEF_100M)
		tnkclk = TOE_DEFAULT_CLK_100M;
	else if (!(readl(SYS_CTRL_BASE + TOE_RST) & TOE_CLK_DEF_100M))
		tnkclk = TOE_DEFAULT_CLK_150M;
	else
		return -1;

	return tnkclk;
}

void reset_mac_interface_single(int val)
{
	unsigned int reg;
	/* set bits gmac0_if_srst_req
	   before config TOE_MAC_INTERFACE */
	reg = readl(SYS_CTRL_BASE + TOE_RST);
	reg |= 0x10;
	writel(reg, SYS_CTRL_BASE + TOE_RST);

	writel(val, SYS_CTRL_BASE + TOE_MAC_INTERFACE);
	readl(SYS_CTRL_BASE + TOE_MAC_INTERFACE);

	reg &= ~0x10;
	writel(reg, SYS_CTRL_BASE + TOE_RST);

}

void reset_mac_interface_dual(int val)
{
	unsigned int reg;
	/* set bits gmac0_if_srst_req and
	   gmac1_if_srst_req
	   before config TOE_MAC_INTERFACE */
	reg = readl(SYS_CTRL_BASE + TOE_RST);
	reg |= 0x50;
	writel(reg, SYS_CTRL_BASE + TOE_RST);

	writel(val, SYS_CTRL_BASE + TOE_MAC_INTERFACE);
	readl(SYS_CTRL_BASE + TOE_MAC_INTERFACE);

	reg &= ~0x50;
	writel(reg, SYS_CTRL_BASE + TOE_RST);

}
