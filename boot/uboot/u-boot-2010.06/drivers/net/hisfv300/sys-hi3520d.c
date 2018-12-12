#ifdef CONFIG_NET_HISFV300_HI3520D

#include "hieth.h"

#define HIETH_SYSCTL_REG	(SYS_CTRL_REG_BASE + PERIPHCTRL24)

#define HIETH_SYSREG_BASE	(CRG_REG_BASE)
#define HIETH_CRG_REG		(CRG_REG_BASE + REG_ETH_CRG)
#define ETH_SOFT_RESET		(1 << 0)
#define ETH_CLK_ENABLE		(1 << 1)
#define ETH_PHY_RESET		(1 << 2)

#define HIETH_EFUSE_REG		(CRG_REG_BASE + EFUSE_CTRL_CRG)
#define HIETH_EFUSE_DATA_REG	(EFUSE_REG_BASE + EFUSE_DATA_SEL)

#ifdef INNER_PHY
void set_efuse_unread(void)
{
	u32 reg_value;

	reg_value = readl(HIETH_EFUSE_REG);
	reg_value |= 0x1;
	writel(reg_value, HIETH_EFUSE_REG);

	reg_value = readl(HIETH_EFUSE_DATA_REG);
	reg_value |= 0x1;
	writel(reg_value, HIETH_EFUSE_DATA_REG);

	udelay(300000);

	reg_value = readl(HIETH_EFUSE_REG);
	reg_value &= ~0x1;
	writel(reg_value, HIETH_EFUSE_REG);
}
#endif

#ifdef INNER_PHY
void set_inner_phy_addr(u32 phy_addr)
{
	u32 reg_value;

	reg_value = readl(HIETH_SYSCTL_REG);
	reg_value &= ~0x1f;
	phy_addr &= 0x1f;
	reg_value |= phy_addr;
	writel(reg_value, HIETH_SYSCTL_REG);
}
#endif

#ifdef INNER_PHY
void revise_led_shine(void)
{
	#define MDIO_RWCTRL 0x1100
	#define tmp_mdio_ready() \
		(readl(REG_BASE_SF + MDIO_RWCTRL) & (0x1 << 15))

	unsigned int reg_value = 0;
	/* select page 7 */
	do {
		reg_value = 0x0007235f;
		writel(reg_value, REG_BASE_SF + MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* enable LED modify function */
	do {
		reg_value = 0xc03c2353;
		writel(reg_value, REG_BASE_SF + MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* set green LED shine all the while when link up,
	 * yellow LED blink when data coming
	 */
	do {
		reg_value = 0x00382351;
		writel(reg_value, REG_BASE_SF + MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	return;
}
#endif

#ifdef INNER_PHY
void set_phy_valtage(void)
{
	#define MDIO_RWCTRL 0x1100
	#define tmp_mdio_ready() \
		(readl(REG_BASE_SF + MDIO_RWCTRL) & (0x1 << 15))

	unsigned int reg_value = 0;
	/* select page 1 */
	do {
		reg_value = 0x0001235f;
		writel(reg_value, REG_BASE_SF + MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* set fe-phy in lower valtage */
	do {
		reg_value = 0x94482352;
		writel(reg_value, REG_BASE_SF + MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* select page 0 */
	do {
		reg_value = 0x0000235f;
		writel(reg_value, REG_BASE_SF + MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());
}
#endif

static void hieth_reset(int rst)
{
	u32 val;

	val = _readl(HIETH_CRG_REG);
	if (rst)
		val |= ETH_SOFT_RESET;
	else
		val &= ~ETH_SOFT_RESET;
	_writel(val, HIETH_CRG_REG);

	udelay(100);
#ifdef INNER_PHY
	val = _readl(HIETH_CRG_REG);
	if (rst)
		val |= ETH_PHY_RESET;
	else
		val &= ~ETH_PHY_RESET;
	_writel(val, HIETH_CRG_REG);

	udelay(100);
#endif
}

static inline void hieth_clk_ena(void)
{
	u32 val;

	val = _readl(HIETH_CRG_REG);
	val |= ETH_CLK_ENABLE;
	_writel(val, HIETH_CRG_REG);
}

static inline void hieth_clk_dis(void)
{
	u32 val;

	val = _readl(HIETH_CRG_REG);
	val &= ~ETH_CLK_ENABLE;
	_writel(val, HIETH_CRG_REG);
}

static void hieth_phy_reset(void)
{
#ifdef INNER_PHY
	u32 rst;

	rst = _readl(HIETH_CRG_REG);
	rst |= ETH_PHY_RESET;
	_writel(rst, HIETH_CRG_REG);
	/* udelay at least 10ms */
	udelay(15000);

	rst = _readl(HIETH_CRG_REG);
	rst &= ~ETH_PHY_RESET;
	_writel(rst, HIETH_CRG_REG);
#endif

#ifdef HISFV_RESET_GPIO_EN
	unsigned int val;
	/*gpiox[x] set to reset, then delay 200ms*/
	val = __raw_readw(HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	val |= (HISFV_RESET_GPIO_DIR_OUT << HISFV_RESET_GPIO_BIT);
	__raw_writew(val, HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	__raw_writew(HISFV_RESET_GPIO_DATA,
			HISFV_RESET_GPIO_BASE + (4 << HISFV_RESET_GPIO_BIT));

	udelay(200000);

	/*then,cancel reset,and should delay 200ms*/
	val = __raw_readw(HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	val |= (HISFV_RESET_GPIO_DIR_OUT << HISFV_RESET_GPIO_BIT);
	__raw_writew(val, HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	__raw_writew(((!HISFV_RESET_GPIO_DATA) << HISFV_RESET_GPIO_BIT),
			HISFV_RESET_GPIO_BASE + (4 << HISFV_RESET_GPIO_BIT));

	udelay(20000);
#endif
}

static void hieth_funsel_config(void)
{
}

static void hieth_funsel_restore(void)
{
}

#endif/*CONFIG_NET_HISFV300_HI3520D*/
