#ifdef CONFIG_ARCH_GODBOX_V1

#define HI_SATA_PERI_CTRL    IO_ADDRESS(0x101f5000)
#define HI_SATA_PERI_CRG     (HI_SATA_PERI_CTRL + 0xcc)
#define HI_SATA_HRST            (1 << 0)
#define HI_SATA_PHYRST          (1 << 1)
#define HI_SATA_RST             (1 << 2)
#define HI_SATA_TX0_RST         (1 << 3)
#define HI_SATA_TX1_RST         (1 << 4)
#define HI_SATA_RX0_RST         (1 << 5)
#define HI_SATA_RX1_RST         (1 << 6)
#define HI_SATA_ALIVE_RST       (1 << 7)
#define HI_SATA_CKEN            (1 << 8)
#define HI_SATA_PORT0_CKEN      (1 << 9)
#define HI_SATA_PORT1_CKEN      (1 << 10)
#define HI_SATA_CLK_SEL         (1 << 16)

#define HI_GPIO_BASE         IO_ADDRESS(0x101f1000)
#define HI_GPIO_DIR          (HI_GPIO_BASE + 0x400)
#define HI_GPIO_BIT          4
#define HI_GPIO_BIT_ADDR     (HI_GPIO_BASE + (4 << HI_GPIO_BIT))

static void hi_sata_poweron(void)
{
	unsigned int tmp_val;
	unsigned long flags;

	local_irq_save(flags);
	tmp_val = readl(HI_GPIO_DIR);
	tmp_val |= (1 << HI_GPIO_BIT);
	writel(tmp_val, HI_GPIO_DIR);
	writel((1 << HI_GPIO_BIT), HI_GPIO_BIT_ADDR);
	local_irq_restore(flags);
}

static void hi_sata_poweroff(void)
{
	unsigned int tmp_val;
	unsigned long flags;

	local_irq_save(flags);
	tmp_val = readl(HI_GPIO_DIR);
	tmp_val |= (1 << HI_GPIO_BIT);
	writel(tmp_val, HI_GPIO_DIR);
	writel(0, HI_GPIO_BIT_ADDR);
	local_irq_restore(flags);
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

	tmp_val = readl(HI_SATA_PERI_CRG);
	tmp_val |= HI_SATA_CLK_SEL;
	writel(tmp_val, HI_SATA_PERI_CRG);
}

#endif



