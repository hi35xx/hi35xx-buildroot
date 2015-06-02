#ifdef CONFIG_ARCH_PHOENIX

static void hieth_reset(int rst)
{
}

static inline void hieth_clk_ena(void)
{
}

static inline void hieth_clk_dis(void)
{
}

static void hieth_phy_reset(void)
{
}

static void hieth_phy_suspend(void)
{
	/* FIXME: phy power down */
}

static void hieth_phy_resume(void)
{
	/* FIXME: phy power up */

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
				BITS_ETH_SOFT_RESET_UP);
		msleep(20);
		hieth_writel_bits(ld, 0, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_UP);
		msleep(20);
		hieth_writel_bits(ld, 1, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_UP);
		msleep(20);
		hieth_writel_bits(ld, 0, GLB_SOFT_RESET,
				BITS_ETH_SOFT_RESET_UP);
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
#endif/*CONFIG_ARCH_PHOENIX*/
