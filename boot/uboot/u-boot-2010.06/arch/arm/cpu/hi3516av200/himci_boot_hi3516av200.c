#define REG_PERI_CRG49		0xC4
#define EMMC_SRST_REQ           (0x1 << 16)
#define EMMC_CKEN               (0x1 << 17)
static inline void emmc_sys_init(void)
{

	unsigned int tmp_reg;
	/* emmc clock phase */
	tmp_reg = himci_readl(CRG_REG_BASE + REG_PERI_CRG49);

	/* emmc soft reset */
	tmp_reg |= EMMC_SRST_REQ;
	himci_writel(tmp_reg, CRG_REG_BASE + REG_PERI_CRG49);
	delay(1000 * DELAY_US);
	tmp_reg &= ~EMMC_SRST_REQ;
	tmp_reg |= EMMC_CKEN;
	himci_writel(tmp_reg, CRG_REG_BASE + REG_PERI_CRG49);
}
