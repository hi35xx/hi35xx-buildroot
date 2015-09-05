
#define PERI_CRG49		(CRG_REG_BASE + 0xC4)
#define SDIO0CLK_PCTRL		(0x1 << 3)
#define SDIO0CLK_SEL_50M	(0x1 << 2) /* 50MHz */
#define SDIO0_CKEN		(0x1 << 1)
#define SDIO_RESET		(0x1 << 0)
#define SYS_PERIPHCTRL4		(SYS_CTRL_REG_BASE + 0x3C)
#define SDIO0_DET_MODE		(0x1 << 2)

static void hi_mci_sys_init(void)
{
	unsigned int reg_value;
	/* set detect polarity */
	reg_value = himci_readl(SYS_PERIPHCTRL4);
	reg_value &= ~SDIO0_DET_MODE;
	himci_writel(reg_value, SYS_PERIPHCTRL4);
	/* set clk polarity, mmc clk */
	reg_value = 0;
	reg_value = himci_readl(PERI_CRG49);
	reg_value &= ~(SDIO0CLK_PCTRL);
	reg_value |= SDIO0CLK_SEL_50M;
	reg_value |= SDIO0_CKEN;
	himci_writel(reg_value, PERI_CRG49);
}

static void hi_mci_ctr_reset(void)
{
	unsigned int reg_value;

	reg_value = himci_readl(PERI_CRG49);
	reg_value |= SDIO_RESET;
	himci_writel(reg_value, PERI_CRG49);
}

static void hi_mci_ctr_undo_reset(void)
{
	unsigned int reg_value;

	reg_value = himci_readl(PERI_CRG49);
	reg_value &= ~(SDIO_RESET);
	himci_writel(reg_value, PERI_CRG49);
}
