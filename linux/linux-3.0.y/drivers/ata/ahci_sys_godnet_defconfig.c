#ifdef CONFIG_ARCH_GODNET

#define HI_SATA_PERI_CTRL    IO_ADDRESS(0x20030000)
#define HI_SATA_PERI_CRG     (HI_SATA_PERI_CTRL + 0xb4)
#define HI_SATA_HRST            (1 << 1)
#define HI_SATA_PHYRST          (1 << 0)
#define HI_SATA_RST             (1 << 2)
#define HI_SATA_TX0_RST         (1 << 3)
#define HI_SATA_TX1_RST         (1 << 4)
#define HI_SATA_RX0_RST         (1 << 5)
#define HI_SATA_RX1_RST         (1 << 6)
#define HI_SATA_ALIVE_RST       (1 << 7)
#define HI_SATA_CKEN            (1 << 10)
#define HI_SATA_PORT0_CKEN      (1 << 9)
#define HI_SATA_PORT1_CKEN      (1 << 8)
#define HI_SATA_CLK_SEL         (1 << 11)

static void hi_sata_poweron(void)
{

}

static void hi_sata_poweroff(void)
{

}

static void hi_sata_reset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val |= HI_SATA_HRST;
	writel(tmp_val, HI_SATA_PERI_CRG);
}

static void hi_sata_unreset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val &= ~HI_SATA_HRST;
	writel(tmp_val, HI_SATA_PERI_CRG);
}

static void hi_sata_phy_reset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val |= HI_SATA_PHYRST;
	writel(tmp_val, HI_SATA_PERI_CRG);
}

static void hi_sata_phy_unreset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val &= ~HI_SATA_PHYRST;
	writel(tmp_val, HI_SATA_PERI_CRG);
}

static void hi_sata_clk_open(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val |= (HI_SATA_CKEN | HI_SATA_PORT0_CKEN | HI_SATA_PORT1_CKEN);
	writel(tmp_val, HI_SATA_PERI_CRG);
}

static void hi_sata_clk_close(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val &= ~(HI_SATA_CKEN | HI_SATA_PORT0_CKEN | HI_SATA_PORT1_CKEN);
	writel(tmp_val, HI_SATA_PERI_CRG);
}
static void hi_sata_clk_reset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val |= (HI_SATA_RST | HI_SATA_TX0_RST | HI_SATA_TX1_RST
	       | HI_SATA_RX0_RST | HI_SATA_RX1_RST | HI_SATA_ALIVE_RST);
	writel(tmp_val, HI_SATA_PERI_CRG);
}

static void hi_sata_clk_unreset(void)
{
	unsigned int tmp_val;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val &= ~(HI_SATA_RST | HI_SATA_TX0_RST | HI_SATA_TX1_RST
	       | HI_SATA_RX0_RST | HI_SATA_RX1_RST | HI_SATA_ALIVE_RST);
	writel(tmp_val, HI_SATA_PERI_CRG);
}

static void hi_sata_phy_clk_sel(void)
{
	unsigned int tmp_val;
	unsigned int i;

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val &= ~HI_SATA_CLK_SEL;
	writel(tmp_val, HI_SATA_PERI_CRG);

	/* due to a bug of sata controller in fetch, here
	 * we set specific value to both port to bypass it.
	 * 0x100 + i * 0x80 + 0x44:
	 * [10]:fetch bypass
	 * [9] :avoid seagate rerror bug
	 */
	for (i = 0; i < CONFIG_HI_SATA_PORTS; i++)
		writel(0x724,
			(IO_ADDRESS(0x10080000) + 0x100 + i * 0x80 + 0x44));
}

#endif



