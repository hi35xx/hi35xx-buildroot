#define PERI_CRG49              (CRG_REG_BASE + 0xC4)
#define EMMC_DLL_CTRL		(MISC_REG_BASE + 0x140)
/* SDIO0 REG */
#define SDIO0_CLK_SEL_MASK	(0x7 << 10)
#define SDIO0_CLK_SEL_100M	(0x0 << 10)
#define SDIO0_CLK_SEL_200M	(0x1 << 10)
#define SDIO0_CLK_SEL_300M	(0x2 << 10)
#define SDIO0_CLK_SEL_400M	(0x3 << 10)
#define SDIO0_CLK_SEL_594M	(0x4 << 10)
#define SDIO0_CLK_SEL_792M	(0x5 << 10)
#define SDIO0_CLK_SEL_1500M	(0x6 << 10)
#define SDIO0_SRST_REQ		(0x1 << 8)
#define SDIO0_CKEN		(0x1 << 9)
#define SDIO0_CLK_MODE		(0x1 << 13)

/* SDIO1 REG */
#define SDIO1_CLK_SEL_MASK	(0x7 << 2)
#define SDIO1_CLK_SEL_100M	(0x0 << 2)
#define SDIO1_CLK_SEL_200M	(0x1 << 2)
#define SDIO1_CLK_SEL_300M	(0x2 << 2)
#define SDIO1_CLK_SEL_400M	(0x3 << 2)
#define SDIO1_CLK_SEL_594M	(0x4 << 2)
#define SDIO1_CLK_SEL_792M	(0x5 << 2)
#define SDIO1_CLK_SEL_1500M	(0x6 << 2)
#define SDIO1_SRST_REQ		(0x1 << 0)
#define SDIO1_CKEN		(0x1 << 1)
#define SDIO1_CLK_MODE		(0x1 << 5)

/* EMMC REG*/
#define EMMC_CLK_SEL_MASK	(0x7 << 18)
#define EMMC_CLK_SEL_100M	(0x0 << 18)
#define EMMC_CLK_SEL_200M	(0x1 << 18)
#define EMMC_CLK_SEL_300M	(0x2 << 18)
#define EMMC_CLK_SEL_400M	(0x3 << 18)
#define EMMC_CLK_SEL_594M	(0x4 << 18)
#define EMMC_CLK_SEL_792M	(0x5 << 18)
#define EMMC_CLK_SEL_1500M	(0x6 << 18)
#define EMMC_SRST_REQ		(0x1 << 16)
#define EMMC_CKEN		(0x1 << 17)
#define EMMC_CLK_MODE		(0x1 << 21)
#define EMMC_DLL_CKEN		(0x1 << 22)
#define EMMC_DLL_SRST_REQ	(0x1 << 23)

static void hi_mci_sys_init(unsigned int dev_num)
{
	unsigned int tmp_reg;

	HIMCI_DEBUG_FUN("Function Call");

	if (dev_num == 2) {
		/* SDIO0 clock phase */
		tmp_reg = himci_readl(PERI_CRG49);
		tmp_reg &= ~SDIO0_CLK_SEL_MASK;
		tmp_reg |= SDIO0_CLK_SEL_200M;
		himci_writel(tmp_reg, PERI_CRG49);

		/* SDIO0 soft reset */
		tmp_reg |= SDIO0_SRST_REQ;
		himci_writel(tmp_reg, PERI_CRG49);
		udelay(1000);
		tmp_reg &= ~SDIO0_SRST_REQ;
		tmp_reg |= SDIO0_CKEN;
		himci_writel(tmp_reg, PERI_CRG49);
		udelay(1000);
	} else if (dev_num == 1) {
		/* SDIO1 clock phase */
		tmp_reg = himci_readl(PERI_CRG49);
		tmp_reg &= ~SDIO1_CLK_SEL_MASK;
		tmp_reg |= SDIO1_CLK_SEL_200M;
		himci_writel(tmp_reg, PERI_CRG49);

		/* SDIO1 soft reset */
		tmp_reg |= SDIO1_SRST_REQ;
		himci_writel(tmp_reg, PERI_CRG49);
		udelay(1000);
		tmp_reg &= ~SDIO1_SRST_REQ;
		tmp_reg |= SDIO1_CKEN;
		himci_writel(tmp_reg, PERI_CRG49);
		udelay(1000);
	} else {
		/* EMMC clock phase */
		tmp_reg = himci_readl(PERI_CRG49);
		tmp_reg &= ~EMMC_CLK_SEL_MASK;
		tmp_reg |= EMMC_CLK_SEL_200M;
		himci_writel(tmp_reg, PERI_CRG49);

		/* EMMC soft reset */
		tmp_reg |= EMMC_SRST_REQ;
		himci_writel(tmp_reg, PERI_CRG49);
		udelay(1000);
		tmp_reg &= ~EMMC_SRST_REQ;
		tmp_reg |= EMMC_CKEN;
		himci_writel(tmp_reg, PERI_CRG49);
		udelay(1000);
	}
}
