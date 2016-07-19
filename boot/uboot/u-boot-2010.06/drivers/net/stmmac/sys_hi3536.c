#include "stmmac.h"
#include "sys_hi3536.h"

#define mdelay(n)      udelay((n)*1000)

unsigned long get_tnkclk(unsigned long busclk)
{
	unsigned long tnkclk;

	if (readl(SYS_CTRL_BASE + TOE_RST) & TOE_CLK_DEF_250M)
		tnkclk = TOE_DEFAULT_CLK_250M;
	else if (!(readl(SYS_CTRL_BASE + TOE_RST) & TOE_CLK_DEF_250M))
		tnkclk = TOE_DEFAULT_CLK_150M;
	else
		return -1;

	return tnkclk;
}

void reset_mac_interface_single(int val)
{
	unsigned int reg;
	/* set bits gmac0_if_srst_req
	before config TOE_MAC_INTERFACE
	*/
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
	before config TOE_MAC_INTERFACE
	*/
	reg = readl(SYS_CTRL_BASE + TOE_RST);
	reg |= 0x50;
	writel(reg, SYS_CTRL_BASE + TOE_RST);

	writel(val, SYS_CTRL_BASE + TOE_MAC_INTERFACE);
	readl(SYS_CTRL_BASE + TOE_MAC_INTERFACE);

	reg &= ~0x50;
	writel(reg, SYS_CTRL_BASE + TOE_RST);

}

void stmmac_rst_phy_use_crg(int port_id)
{
	unsigned int reg;
	unsigned int phy_rst_bit;

	phy_rst_bit = TOE_CLK_EXT_PHY1_RST_BIT;

	/* Leave reset mode, make sure the RESET has a negative pulse */
	reg = readl(SYS_CTRL_BASE + TOE_RST);
	reg &= ~(1 << phy_rst_bit);
	writel(reg, SYS_CTRL_BASE + TOE_RST);

	mdelay(30);

	/*  Enter reset mode */
	reg = readl(SYS_CTRL_BASE + TOE_RST);
	reg |= (1 << phy_rst_bit);
	writel(reg, SYS_CTRL_BASE + TOE_RST);

	mdelay(30);

	/*  Leave reset mode */
	reg = readl(SYS_CTRL_BASE + TOE_RST);
	reg &= ~(1 << phy_rst_bit);
	writel(reg, SYS_CTRL_BASE + TOE_RST);

	/* wait at least 30ms for future MDIO operation */
	mdelay(30);
}

void stmmac_rst_phy_use_gpio(int port_id)
{
	unsigned int gpio_base = 0;
	unsigned int gpio_bit;
	unsigned int rst_data;
	unsigned int v;

	if (port_id == GMAC0_PORT) {
#ifdef CONFIG_MAC0_PHY_GPIO_RESET
		gpio_base = CONFIG_MAC0_PHY_RESET_GPIO_BASE;
		gpio_bit = CONFIG_MAC0_PHY_RESET_GPIO_BIT;
		rst_data = CONFIG_MAC0_PHY_RESET_GPIO_DATA;
#endif
	} else {
#ifdef CONFIG_MAC1_PHY_GPIO_RESET
		gpio_base = CONFIG_MAC1_PHY_RESET_GPIO_BASE;
		gpio_bit = CONFIG_MAC1_PHY_RESET_GPIO_BIT;
		rst_data = CONFIG_MAC1_PHY_RESET_GPIO_DATA;
#endif
	}

#if defined(CONFIG_MAC0_PHY_GPIO_RESET) || defined(CONFIG_MAC0_PHY_GPIO_RESET)
	if (!gpio_base)
		return;

	/* config gpip[x] dir to output */
	v = readb(gpio_base + REG_GPIO_DIR);
	v |= (1 << gpio_bit);
	writeb(v, gpio_base + REG_GPIO_DIR);

	/* gpiox[x] set to reset, then delay 30ms */
	writeb(rst_data << gpio_bit,
			gpio_base + (4 << gpio_bit));
	mdelay(30);
	/* then,cancel reset,and should delay 30ms */
	writeb((!rst_data) << gpio_bit,
			gpio_base + (4 << gpio_bit));
	mdelay(30);
	writeb(rst_data << gpio_bit,
			gpio_base + (4 << gpio_bit));

	/* wait at least 30ms for future MDIO operation */
	mdelay(30);
#endif
}

#define EXT_PHY_RST_USE_GPIO    (0)
#define EXT_PHY_RST_USE_CRG     (1)
void stmmac_external_phy_reset(int port_id)
{
	unsigned int how_to_rst;

	/* reset phy0 will reset phy1 at the same time,
	 * so we no need to reset phy1 any more.
	 */
	if (port_id != GMAC0_PORT)
		return;

	how_to_rst = readl(MUXCTL_BASE + MUXCTL_REG34_OFFSET);

	how_to_rst &= MUXCTL_PHY_MASK;

	if (how_to_rst == EXT_PHY_RST_USE_CRG)
		stmmac_rst_phy_use_crg(port_id);
	else
		stmmac_rst_phy_use_gpio(port_id);
}
