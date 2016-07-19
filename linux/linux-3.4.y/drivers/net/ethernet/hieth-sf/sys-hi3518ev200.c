#define HIETH_SYSREG_BASE	IO_ADDRESS(CRG_REG_BASE)
#define HIETH_SYSREG_REG	REG_ETH_CRG

#define ETH_PHY_RESET		BIT(3)
#define ETH_CORE_CLK_SELECT_54M	BIT(7)

static void hieth_set_regbit(unsigned long addr, int bit, int shift)
{
	unsigned long reg;
	reg = readl(addr);
	bit = bit ? 1 : 0;
	reg &= ~(1<<shift);
	reg |= bit<<shift;
	writel(reg, addr);
}

static void hieth_reset(int rst)
{
	hieth_set_regbit(HIETH_SYSREG_BASE + HIETH_SYSREG_REG, rst, 0);
	msleep(20);
}

static inline void hieth_clk_ena(void)
{
	unsigned long val;
	val = readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	val |= ETH_CORE_CLK_SELECT_54M;
	/* eth enable */
	val |= (1 << 1);
	writel(val, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
}

static inline void hieth_clk_dis(void)
{
}

static void hieth_phy_reset(void)
{
	u32 v = 0;

	/************************************************/
	/* reset external phy with default reset pin */
	v = readl((void __iomem *)(HIETH_SYSREG_BASE + HIETH_SYSREG_REG));
	v |= ETH_PHY_RESET;
	writel(v, (void __iomem *)(HIETH_SYSREG_BASE + HIETH_SYSREG_REG));

	msleep(50);

	/* then, cancel reset, and should delay 50ms */
	v = readl((void __iomem *)(HIETH_SYSREG_BASE + HIETH_SYSREG_REG));
	v &= ~ETH_PHY_RESET;
	writel(v, (void __iomem *)(HIETH_SYSREG_BASE + HIETH_SYSREG_REG));

	msleep(50);

#ifdef CONFIG_HIETH_RESET_HELPER_EN
	int val;
	unsigned long flags;
	local_irq_save(flags);
	/*gpiox[x] set to reset, then sleep 200ms*/
	val = readw(IO_ADDRESS(CONFIG_HIETH_RESET_HELPER_GPIO_BASE) + 0x400);
	val |= (1 << CONFIG_HIETH_RESET_HELPER_GPIO_BIT);
	writew(val, IO_ADDRESS(CONFIG_HIETH_RESET_HELPER_GPIO_BASE) + 0x400);
	writew(0, (IO_ADDRESS(CONFIG_HIETH_RESET_HELPER_GPIO_BASE)
				+ (4 << CONFIG_HIETH_RESET_HELPER_GPIO_BIT)));
	local_irq_restore(flags);
	msleep(200);
	local_irq_save(flags);
	/*then,cancel reset,and should sleep 50ms*/
	val = readw(IO_ADDRESS(CONFIG_HIETH_RESET_HELPER_GPIO_BASE) + 0x400);
	val |= (1 << CONFIG_HIETH_RESET_HELPER_GPIO_BIT);
	writew(val, IO_ADDRESS(CONFIG_HIETH_RESET_HELPER_GPIO_BASE) + 0x400);
	writew((1 << (CONFIG_HIETH_RESET_HELPER_GPIO_BIT)),
			(IO_ADDRESS(CONFIG_HIETH_RESET_HELPER_GPIO_BASE)
			 + (4 << CONFIG_HIETH_RESET_HELPER_GPIO_BIT)));

	local_irq_restore(flags);
	msleep(50);
#endif
}

static void hieth_phy_suspend(void)
{
	/* FIXME: phy power down */
}

static void hieth_phy_resume(void)
{
	/* FIXME: phy power up */

	hieth_phy_reset();
}

static void hieth_funsel_config(void)
{
}

static void hieth_funsel_restore(void)
{
}

int hieth_port_reset(struct hieth_netdev_local *ld, int port)
{
	hieth_assert(port == ld->port);

	/*soft reset*/
	if (ld->port == UP_PORT) {
		/* Note: sf ip need reset twice */
		hieth_writel_bits(ld, 1, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_ALL);
		msleep(20);
		hieth_writel_bits(ld, 0, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_ALL);
		msleep(20);
		hieth_writel_bits(ld, 1, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_ALL);
		msleep(20);
		hieth_writel_bits(ld, 0, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_ALL);
		msleep(20);
	} else if (ld->port == DOWN_PORT) {
		/* Note: sf ip need reset twice */
		hieth_writel_bits(ld, 1, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_DOWN);
		msleep(20);
		hieth_writel_bits(ld, 0, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_DOWN);
		msleep(20);
		hieth_writel_bits(ld, 1, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_DOWN);
		msleep(20);
		hieth_writel_bits(ld, 0, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_DOWN);
	} else {
		BUG();
	}

	return 0;
}

/* vim: set ts=8 sw=8 tw=78: */
