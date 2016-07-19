#ifdef CONFIG_NET_HISFV300_HI3518EV200

#define HIETH_SYSREG_BASE	(CRG_REG_BASE)
#define HIETH_SYSREG_REG	(REG_ETH_CRG)
#define ETH_SOFT_RESET		(1 << 0)
#define ETH_CLK_ENABLE		(1 << 1)
#define ETH_PHY_RESET		(1 << 3)
#define ETH_CORE_CLK_SELECT_54M	(1 << 7)

static void hieth_set_regbit(unsigned long addr, int bit, int shift)
{
	unsigned long reg;
	reg = _readl(addr);
	bit = bit ? 1 : 0;
	reg &= ~(1<<shift);
	reg |= bit<<shift;
	_writel(reg, addr);
}

static void hieth_reset(int rst)
{
	hieth_set_regbit(HIETH_SYSREG_BASE + HIETH_SYSREG_REG, rst, 0);

	udelay(100);
}

static inline void hieth_clk_ena(void)
{
	ulong val;

	val = _readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	val |= ETH_CORE_CLK_SELECT_54M;
	val |= ETH_CLK_ENABLE;
	_writel(val, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
}

static inline void hieth_clk_dis(void)
{
	ulong val;

	val = _readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	val &= ~ETH_CLK_ENABLE;
	_writel(val, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
}

static void hieth_phy_reset(void)
{
	u32 v = 0;

	/************************************************/
	/* reset external phy with default reset pin */
	v = readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	v |= ETH_PHY_RESET;
	writel(v, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);

	udelay(50000);

	/* then, cancel reset, and should delay some time */
	v = readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	v &= ~ETH_PHY_RESET;
	writel(v, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);

	udelay(50000);

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

#endif/*CONFIG_NET_HISFV300_3518*/
