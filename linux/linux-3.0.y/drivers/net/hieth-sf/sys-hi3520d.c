#ifdef CONFIG_ARCH_HI3520D

#define HIETH_SYSREG_BASE	IO_ADDRESS(CRG_REG_BASE)
#define HIETH_SYSREG_REG	(0xCC)
#define HIETH_SYSCTRL_BASE	IO_ADDRESS(SYS_CTRL_BASE)
#define HIETH_PER24		(0x0094)
#define HIETH_FEPHY_SEL_INNER	(1 << 8)
#define HIETH_PEPHY_ADDR_MASK	(0x1f)

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
#ifdef CONFIG_HIETH_INNER_PHY
	hieth_set_regbit(HIETH_SYSREG_BASE + HIETH_SYSREG_REG, rst, 2);
	msleep(20);
#endif
}

#ifdef CONFIG_HIETH_INNER_PHY
void revise_led_shine(void)
{
#define IO_MDIO_RWCTRL IO_ADDRESS(CONFIG_HIETH_IOBASE + MDIO_RWCTRL)
#define tmp_mdio_ready() \
	(readl(IO_MDIO_RWCTRL) & (0x1 << 15))

	unsigned int reg_value = 0;
	/* select page 7 */
	do {
		reg_value = 0x0007235f;
		writel(reg_value, IO_MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* enable LED modify function */
	do {
		reg_value = 0xc03c2353;
		writel(reg_value, IO_MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* set green LED shine all the while when link up,
	 * yellow LED blink when data coming
	 */
	do {
		reg_value = 0x00382351;
		writel(reg_value, IO_MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	return;
}
#endif

#ifdef CONFIG_HIETH_INNER_PHY
void set_phy_valtage(void)
{
	#define IO_MDIO_RWCTRL IO_ADDRESS(CONFIG_HIETH_IOBASE + MDIO_RWCTRL)
	#define tmp_mdio_ready() \
		(readl(IO_MDIO_RWCTRL) & (0x1 << 15))

	unsigned int reg_value = 0;
	/* select page 1 */
	do {
		reg_value = 0x0001235f;
		writel(reg_value, IO_MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* set fe-phy in lower valtage */
	do {
		reg_value = 0x94482352;
		writel(reg_value, IO_MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());

	/* select page 0 */
	do {
		reg_value = 0x0000235f;
		writel(reg_value, IO_MDIO_RWCTRL);
		udelay(10);
	} while (!tmp_mdio_ready());
}
#endif

static inline void hieth_clk_ena(void)
{
	unsigned long val;

#ifdef CONFIG_HIETH_INNER_PHY
	val = readl(HIETH_SYSCTRL_BASE + HIETH_PER24);
	val &= ~(HIETH_PEPHY_ADDR_MASK);
	val |= (HIETH_PEPHY_ADDR_MASK & CONFIG_HIETH_PHYID_U);
	writel(val, HIETH_SYSCTRL_BASE + HIETH_PER24);
#endif
	val = readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	val |= (1 << 1);
	writel(val, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
}

static inline void hieth_clk_dis(void)
{
}

static void hieth_phy_reset(void)
{
#ifdef CONFIG_HIETH_RESET_HELPER_EN
	int val;
	unsigned long flags;
#endif

#ifdef CONFIG_HIETH_INNER_PHY
	unsigned int tmp = 0;
	tmp = readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	tmp |= (1 << 2);
	writel(tmp, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	msleep(200);
	tmp = readl(HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	tmp &= ~(1 << 2);
	writel(tmp, HIETH_SYSREG_BASE + HIETH_SYSREG_REG);
	msleep(200);
#endif

#ifdef CONFIG_HIETH_RESET_HELPER_EN
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
#endif/*CONFIG_ARCH_HI3520D*/

/* vim: set ts=8 sw=8 tw=78: */
