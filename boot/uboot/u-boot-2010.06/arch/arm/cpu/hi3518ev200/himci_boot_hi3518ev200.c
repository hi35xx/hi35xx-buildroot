static inline void emmc_sys_init(void)
{
	unsigned int tmp_reg;

	/* SDIO clock phase */
	tmp_reg = himci_readl(CRG_REG_BASE + REG_CRG49);
	tmp_reg &= ~SDIO0_CLK_SEL_MASK;
	tmp_reg |= SDIO0_CLK_SEL_49_5M; /* phasic move to xls table. */
	himci_writel(tmp_reg, CRG_REG_BASE + REG_CRG49);

	/* SDIO soft reset */
	tmp_reg |= SDIO0_SRST_REQ;
	himci_writel(tmp_reg, CRG_REG_BASE + REG_CRG49);
	delay(100 * DELAY_US);
	tmp_reg &= ~SDIO0_SRST_REQ;
	tmp_reg |= SDIO0_CKEN;
	himci_writel(tmp_reg, CRG_REG_BASE + REG_CRG49);
}

