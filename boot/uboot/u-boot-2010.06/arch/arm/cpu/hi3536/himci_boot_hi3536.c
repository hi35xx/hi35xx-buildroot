#define PERI_CRG132			(0x12040000 + 0x210)

#define SDIO1_CLK_SEL_MASK              (0x7 << 4)
#define SDIO1_CLK_SEL_50M               (0x0 << 4)
#define SDIO1_CLK_SEL_25M               (0x2 << 4)
#define SDIO1_SRST_REQ                  (0x1 << 0)
#define SDIO1_CKEN                      (0x1 << 1)

static inline void emmc_sys_init(void)
{
	unsigned int tmp_reg;

	/* SDIO1 clock phase */
	tmp_reg = himci_readl(PERI_CRG132);
	tmp_reg &= ~SDIO1_CLK_SEL_MASK;
	tmp_reg |= SDIO1_CLK_SEL_50M;		/* phasic move to xls table */
	himci_writel(tmp_reg, PERI_CRG132);

	/* SDIO1 soft reset */
	tmp_reg |= SDIO1_SRST_REQ;
	himci_writel(tmp_reg, PERI_CRG132);
	delay(1000 * DELAY_US);
	tmp_reg &= ~SDIO1_SRST_REQ;
	tmp_reg |= SDIO1_CKEN;
	himci_writel(tmp_reg, PERI_CRG132);
}

