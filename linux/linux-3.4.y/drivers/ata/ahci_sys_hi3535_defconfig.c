#ifdef CONFIG_ARCH_HI3535

enum {
	HI_SATA_PERI_CTRL		= IO_ADDRESS(0x20030000),
	HI_SATA_PERI_CRG43		= (HI_SATA_PERI_CTRL + 0xac),
	HI_SATA_CLK_VALUE		= 0x5,

	HI_SATA_PERI_CRG45		= (HI_SATA_PERI_CTRL + 0xb4),
	HI_SATA_PORT2_REFCLK_CKEN	= (1 << 19),
	HI_SATA_PORT2_MPLL_CKEN		= (1 << 18),
	HI_SATA_PORT1_REFCLK_CKEN	= (1 << 17),
	HI_SATA_PORT1_MPLL_CKEN		= (1 << 16),
	HI_SATA_CK0_RESET		= (1 << 13),
	HI_SATA_BUS_RESET		= (1 << 12),
	HI_SATA_PORT0_REFCLK_CKEN	= (1 << 10),
	HI_SATA_PORT0_MPLL_CKEN		= (1 << 9),
	HI_SATA_CKO_ALIVE_CKEN		= (1 << 8),
	HI_SATA_RX2_CKEN		= (1 << 7),
	HI_SATA_RX1_CKEN		= (1 << 6),
	HI_SATA_RX0_CKEN		= (1 << 5),
	HI_SATA_BUS_CKEN		= (1 << 4),
	HI_SATA_PORT01_CLK_EN		= HI_SATA_BUS_CKEN | HI_SATA_RX0_CKEN |
					HI_SATA_RX1_CKEN |
					HI_SATA_CKO_ALIVE_CKEN |
					HI_SATA_PORT0_MPLL_CKEN |
					HI_SATA_PORT0_REFCLK_CKEN |
					HI_SATA_PORT1_MPLL_CKEN |
					HI_SATA_PORT1_REFCLK_CKEN,
	HI_SATA_PORT2_CLK_EN		= HI_SATA_PORT2_MPLL_CKEN |
					HI_SATA_RX2_CKEN |
					HI_SATA_PORT2_REFCLK_CKEN,

	HI_SATA_MISC_CTRL		= IO_ADDRESS(0x20120000),
	HI_SATA_MISC_CTRL1		= (HI_SATA_MISC_CTRL + 0x4),
	HI_SATA_USE_ESATA		= (1 << 12),
};

static unsigned int hi_sata_use_esata(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_MISC_CTRL1);
	tmp_val &= HI_SATA_USE_ESATA;

	return tmp_val;
}

static void hi_sata_poweron(void)
{
}

static void hi_sata_poweroff(void)
{
}

static void hi_sata_reset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG45);
	tmp_val |= HI_SATA_BUS_RESET;
	tmp_val |= HI_SATA_CK0_RESET;
	writel(tmp_val, HI_SATA_PERI_CRG45);
}

static void hi_sata_unreset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG45);
	tmp_val &= ~(HI_SATA_BUS_RESET | HI_SATA_CK0_RESET);
	writel(tmp_val, HI_SATA_PERI_CRG45);

}

static void hi_sata_phy_reset(void)
{
}

static void hi_sata_phy_unreset(void)
{
}

static void hi_sata_clk_enable(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG45);
	tmp_val |= HI_SATA_PORT01_CLK_EN;
	if (!(hi_sata_use_esata()))
		tmp_val |= HI_SATA_PORT2_CLK_EN;
	writel(tmp_val, HI_SATA_PERI_CRG45);
}

static void hi_sata_clk_disable(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG45);
	tmp_val &= ~HI_SATA_PORT01_CLK_EN;
	if (!(hi_sata_use_esata()))
		tmp_val &= ~HI_SATA_PORT2_CLK_EN;
	writel(tmp_val, HI_SATA_PERI_CRG45);
}

static void hi_sata_clk_reset(void)
{
}

static void hi_sata_clk_unreset(void)
{
}

static void hi_sata_phy_clk_sel(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG43);
	tmp_val |= HI_SATA_CLK_VALUE;
	writel(tmp_val, HI_SATA_PERI_CRG43);
}

#endif
