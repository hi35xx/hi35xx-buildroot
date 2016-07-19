#define HIGMAC_SYSCTL_IOBASE		(IO_ADDRESS(0x200300EC)) /* macif */

#define HIGMAC_CRG_IOBASE		(IO_ADDRESS(0x200300CC))/* CRG */

void __iomem *soc_fwdctl_iobase(void)
{
	/* there is no fwd module in 3516a */
	return NULL;
}

void higmac_hw_mac_core_reset(struct higmac_netdev_local *ld)
{
	struct higmac_adapter *adapter = ld->adapter;
	unsigned int v = 0;
	unsigned long flags;

	spin_lock_irqsave(&adapter->lock, flags);
	if (CONFIG_HIGMAC_PHY0_INTERFACE_MODE == 1) { /* mii selecd pad clk */
		v = readl(HIGMAC_CRG_IOBASE);
		v |= (0x1 << 4); /* select pad clk */
		writel(v, HIGMAC_CRG_IOBASE);
	}
	v = readl(HIGMAC_CRG_IOBASE);
	v |= (0x1 << 1); /* enable clk */
	writel(v, HIGMAC_CRG_IOBASE);

	/* set reset bit */
	v = readl(HIGMAC_CRG_IOBASE);
	v |= 0x1;
	writel(v, HIGMAC_CRG_IOBASE);
	spin_unlock_irqrestore(&adapter->lock, flags);

	udelay(50);

	spin_lock_irqsave(&adapter->lock, flags);
	/* clear reset bit */
	v = readl(HIGMAC_CRG_IOBASE);
	v &= ~(0x1);
	writel(v, HIGMAC_CRG_IOBASE);

	spin_unlock_irqrestore(&adapter->lock, flags);
}

void higmac_set_macif(struct higmac_netdev_local *ld, int mode, int speed)
{
	struct higmac_adapter *adapter = ld->adapter;
	unsigned long flags;
	unsigned long v;

	/* enable change: port_mode */
	higmac_writel_bits(ld, 1, MODE_CHANGE_EN, BIT_MODE_CHANGE_EN);
	if (speed == 2)		/* FIXME */
		speed = 5;	/* 1000M */
	higmac_writel_bits(ld, speed, PORT_MODE, BITS_PORT_MODE);
	/* disable change: port_mode */
	higmac_writel_bits(ld, 0, MODE_CHANGE_EN, BIT_MODE_CHANGE_EN);

	spin_lock_irqsave(&adapter->lock, flags);
	/* soft reset mac_if */
	v = readl(HIGMAC_CRG_IOBASE);
	v |= (1 << 2);
	writel(v, HIGMAC_CRG_IOBASE);

	/* config mac_if */
	writel(mode, HIGMAC_SYSCTL_IOBASE);

	/* undo reset */
	v = readl(HIGMAC_CRG_IOBASE);
	v &= ~(1 << 2);
	writel(v, HIGMAC_CRG_IOBASE);
	spin_unlock_irqrestore(&adapter->lock, flags);
}

void higmac_hw_internal_fephy_reset(struct higmac_adapter *adapter)
{

}

void higmac_hw_external_phy_reset(void)
{
	int val;

	/* phy clk select */
	val = readl(HIGMAC_CRG_IOBASE);
	val |= (0x1 << 6);
	writel(val, HIGMAC_CRG_IOBASE);

#ifdef CONFIG_HIGMAC_RESET_HELPER_EN
	/* set direction */
	val = readl(IO_ADDRESS(CONFIG_HIGMAC_RESET_HELPER_GPIO_BASE) + 0x400);
	val |= (1 << CONFIG_HIGMAC_RESET_HELPER_GPIO_BIT);
	writel(val, IO_ADDRESS(CONFIG_HIGMAC_RESET_HELPER_GPIO_BASE) + 0x400);

	/* Firstly, set to 1 regardless of the value of this pin */
	writel(0xFF, IO_ADDRESS(CONFIG_HIGMAC_RESET_HELPER_GPIO_BASE)
				+ (4 << CONFIG_HIGMAC_RESET_HELPER_GPIO_BIT));
	mdelay(20);

	/* Set to 0 to reset, then sleep 200ms */
	writel(0x0, IO_ADDRESS(CONFIG_HIGMAC_RESET_HELPER_GPIO_BASE)
				+ (4 << CONFIG_HIGMAC_RESET_HELPER_GPIO_BIT));
	mdelay(20);

	/* then, cancel reset, and should sleep 50ms */
	writel(0xFF, IO_ADDRESS(CONFIG_HIGMAC_RESET_HELPER_GPIO_BASE)
				+ (4 << CONFIG_HIGMAC_RESET_HELPER_GPIO_BIT));
	mdelay(200);
#endif
}

void higmac_hw_all_clk_disable(void)
{
}
