
#define PERI_CRG49		(CRG_REG_BASE + 0xC4)
#define SDIO0CLK_PCTRL		(0x1 << 4)
#define SDIO0_CLK_BIT_HIGH	(1U << 3)
#define SDIO0_CLK_BIT_LOW	(1U << 2)
#define SDIO0_CLK_25M		(1 << 3)
#define SDIO0_CKEN		(0x1 << 1)
#define SDIO_RESET		(0x1 << 0)
#define SYS_PERIPHCTRL4		(0x20120004)
#define SDIO0_DET_MODE		(0x1 << 2)

#define REG_UPDATE_MCI_BASE	REG_BASE_MCI

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

static void hi_mci_sys_init(void)
{
	unsigned int reg_value;
	unsigned int value;

	/* set detect polarity */
	reg_value = himci_readl(SYS_PERIPHCTRL4);
	reg_value &= ~SDIO0_DET_MODE;
	himci_writel(reg_value, SYS_PERIPHCTRL4);
	/* set clk polarity, mmc clk */
	reg_value = 0;
	reg_value = himci_readl(PERI_CRG49);
	reg_value &= ~(SDIO0CLK_PCTRL);
	reg_value &= ~(SDIO0_CLK_BIT_HIGH);
	reg_value &= ~(SDIO0_CLK_BIT_LOW);
	reg_value |= SDIO0_CLK_25M;
	reg_value |= SDIO0_CKEN;
	himci_writel(reg_value, PERI_CRG49);

}

