#ifdef CONFIG_ARCH_HI3516A

#define PERI_CRG49		IO_ADDRESS(0x200300C4)

#define SDIO1_CLK_BIT_HIGH	(1U << 11)
#define SDIO1_CLK_BIT_LOW	(1U << 10)
#define SDIO1_CLK_SEL_100M	(1U << 10) /* 100MHz */
#define SDIO1_CLK_SEL_75M	(3U << 10) /* 75MHz */
#define SDIO1_CLK_SEL_50M	(0U << 10) /* 50MHz */
#define SDIO1_CLK_SEL_25M	(2U << 10) /* 25MHz */
#define SDIO1_CKEN		(1U << 9)
#define SDIO1_RESET		(1U << 8)

#define SDIO0_CLK_BIT_HIGH	(1U << 3)
#define SDIO0_CLK_BIT_LOW	(1U << 2)
#define SDIO0_CLK_SEL_100M	(1U << 2) /* 100MHz */
#define SDIO0_CLK_SEL_75M	(3U << 2) /* 75MHz */
#define SDIO0_CLK_SEL_50M	(0U << 2) /* 50MHz */
#define SDIO0_CLK_SEL_25M	(2U << 2) /* 25MHz */
#define SDIO0_CKEN		(1U << 1)
#define SDIO0_RESET		(1U << 0)

#define MISC_REG_BASE		IO_ADDRESS(0x20120004)
#define SDIO1_DET_MODE		(1U << 0)
#define SDIO0_DET_MODE		(1U << 1)

#define REG_PAD_CTRL		0x200f0800

#define REG_CTRL_SDIO0_CCLK	IO_ADDRESS(REG_PAD_CTRL + 0xcc)
#define REG_CTRL_SDIO0_CCMD	IO_ADDRESS(REG_PAD_CTRL + 0xdc)
#define REG_CTRL_SDIO0_CDATA0	IO_ADDRESS(REG_PAD_CTRL + 0xe0)
#define REG_CTRL_SDIO0_CDATA1	IO_ADDRESS(REG_PAD_CTRL + 0xe4)
#define REG_CTRL_SDIO0_CDATA2	IO_ADDRESS(REG_PAD_CTRL + 0xe8)
#define REG_CTRL_SDIO0_CDATA3	IO_ADDRESS(REG_PAD_CTRL + 0xec)

#define REG_CTRL_SDIO1_CCLK	IO_ADDRESS(REG_PAD_CTRL + 0x104)
#define REG_CTRL_SDIO1_CCMD	IO_ADDRESS(REG_PAD_CTRL + 0x114)
#define REG_CTRL_SDIO1_CDATA0	IO_ADDRESS(REG_PAD_CTRL + 0x118)
#define REG_CTRL_SDIO1_CDATA1	IO_ADDRESS(REG_PAD_CTRL + 0x11c)
#define REG_CTRL_SDIO1_CDATA2	IO_ADDRESS(REG_PAD_CTRL + 0x120)
#define REG_CTRL_SDIO1_CDATA3	IO_ADDRESS(REG_PAD_CTRL + 0x124)

#define SDIO_CCLK_DS_3V3		0x60
#define SDIO_CCMD_DS_3V3		0xe0
#define SDIO_CDATA0_DS_3V3		0xe0
#define SDIO_CDATA1_DS_3V3		0xe0
#define SDIO_CDATA2_DS_3V3		0xe0
#define SDIO_CDATA3_DS_3V3		0xe0

#define SDIO_CCLK_DS_1V8		0x40
#define SDIO_CCMD_DS_1V8		0xd0
#define SDIO_CDATA0_DS_1V8		0xd0
#define SDIO_CDATA1_DS_1V8		0xd0
#define SDIO_CDATA2_DS_1V8		0xd0
#define SDIO_CDATA3_DS_1V8		0xd0

static void hi_mci_sys_ctrl_init(struct himci_host *host)
{
	unsigned int reg_value;

	if (0 == host->id) {
#ifdef CONFIG_HIMCI0
		/* set detect polarity */
		reg_value = himci_readl(MISC_REG_BASE);
		reg_value &= ~SDIO0_DET_MODE;
		himci_writel(reg_value, MISC_REG_BASE);
		/* set clk polarity, mmc clk */
		reg_value = 0;
		reg_value = himci_readl(PERI_CRG49);
		reg_value &= ~(SDIO0_CLK_BIT_HIGH);
		reg_value &= ~(SDIO0_CLK_BIT_LOW);
		if (CONFIG_MMC0_CLK == 100000000)
			reg_value |= SDIO0_CLK_SEL_100M;
		else if (CONFIG_MMC0_CLK == 75000000)
			reg_value |= SDIO0_CLK_SEL_75M;
		else if (CONFIG_MMC0_CLK == 25000000)
			reg_value |= SDIO0_CLK_SEL_25M;
		else
			reg_value |= SDIO0_CLK_SEL_50M; /* SDIO0 default 50Hz */
		reg_value |= SDIO0_CKEN;
		himci_writel(reg_value, PERI_CRG49);

		/* config Pin drive capability */
		himci_writel(SDIO_CCLK_DS_3V3, REG_CTRL_SDIO0_CCLK);
		himci_writel(SDIO_CCMD_DS_3V3, REG_CTRL_SDIO0_CCMD);
		himci_writel(SDIO_CDATA0_DS_3V3, REG_CTRL_SDIO0_CDATA0);
		himci_writel(SDIO_CDATA1_DS_3V3, REG_CTRL_SDIO0_CDATA1);
		himci_writel(SDIO_CDATA2_DS_3V3, REG_CTRL_SDIO0_CDATA2);
		himci_writel(SDIO_CDATA3_DS_3V3, REG_CTRL_SDIO0_CDATA3);
#endif
	} else if (1 == host->id) {
#ifdef CONFIG_HIMCI1
		/* set detect polarity */
		reg_value = himci_readl(MISC_REG_BASE);
		reg_value &= ~SDIO1_DET_MODE;
		himci_writel(reg_value, MISC_REG_BASE);
		/* set clk polarity, mmc clk */
		reg_value = 0;
		reg_value = himci_readl(PERI_CRG49);
		reg_value &= ~(SDIO1_CLK_BIT_HIGH);
		reg_value &= ~(SDIO1_CLK_BIT_LOW);
		if (CONFIG_MMC1_CLK == 100000000)
			reg_value |= SDIO1_CLK_SEL_100M;
		else if (CONFIG_MMC1_CLK == 75000000)
			reg_value |= SDIO1_CLK_SEL_75M;
		else if (CONFIG_MMC1_CLK == 25000000)
			reg_value |= SDIO1_CLK_SEL_25M;
		else
			reg_value |= SDIO1_CLK_SEL_50M; /* SDIO1 default 50Hz */
		reg_value |= SDIO1_CKEN;
		himci_writel(reg_value, PERI_CRG49);

		/* config Pin drive capability */
		himci_writel(SDIO_CCLK_DS_3V3, REG_CTRL_SDIO1_CCLK);
		himci_writel(SDIO_CCMD_DS_3V3, REG_CTRL_SDIO1_CCMD);
		himci_writel(SDIO_CDATA0_DS_3V3, REG_CTRL_SDIO1_CDATA0);
		himci_writel(SDIO_CDATA1_DS_3V3, REG_CTRL_SDIO1_CDATA1);
		himci_writel(SDIO_CDATA2_DS_3V3, REG_CTRL_SDIO1_CDATA2);
		himci_writel(SDIO_CDATA3_DS_3V3, REG_CTRL_SDIO1_CDATA3);
#endif
	} else {
		himci_error("himci host id error!");
	}
}

static void hi_mci_ctr_reset(struct himci_host *host)
{
	unsigned int reg_value;

	reg_value = himci_readl(PERI_CRG49);

	if (0 == host->id) {
#ifdef CONFIG_HIMCI0
		reg_value |= SDIO0_RESET;
#endif
	} else if (1 == host->id) {
#ifdef CONFIG_HIMCI1
		reg_value |= SDIO1_RESET;
#endif
	} else {
		himci_error("himci host id error!");
		return;
	}

	himci_writel(reg_value, PERI_CRG49);
}

static void hi_mci_ctr_undo_reset(struct himci_host *host)
{
	unsigned int reg_value;

	reg_value = himci_readl(PERI_CRG49);

	if (0 == host->id) {
#ifdef CONFIG_HIMCI0
		reg_value &= ~(SDIO0_RESET);
#endif
	} else if (1 == host->id) {
#ifdef CONFIG_HIMCI1
		reg_value &= ~(SDIO1_RESET);
#endif
	} else {
		himci_error("himci host id error!");
		return;
	}

	himci_writel(reg_value, PERI_CRG49);
}

#ifdef CONFIG_PM
static void hi_mci_sys_ctrl_suspend(struct himci_host *host)
{
}
#endif

#endif /*CONFIG_ARCH_HI3516A*/
