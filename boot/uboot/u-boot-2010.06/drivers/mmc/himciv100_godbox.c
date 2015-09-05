
#include <asm/arch/platform.h>

static void hi_mci_sys_init(void)
{
	unsigned int tmp_reg;

	HIMCI_DEBUG_FUN("Function Call");

	/* enable SDIO clock and clock 50MHz */
	tmp_reg = himci_readl(REG_BASE_CRG + REG_PERI_CRG46);
	tmp_reg &= ~(SDIO_SRST_REQ | SDIO_CKEN | SDIO_HCLKEN
			 | SDIO_CLK_50M | SDIO_CLK_PCTRL);
	tmp_reg |= SDIO_CKEN | SDIO_HCLKEN | SDIO_CLK_50M;
	himci_writel(tmp_reg, REG_BASE_CRG + REG_PERI_CRG46);

	/* SDIO soft reset */
	tmp_reg |= SDIO_SRST_REQ;
	himci_writel(tmp_reg, REG_BASE_CRG + REG_PERI_CRG46);
	udelay(1000);
	tmp_reg &= ~SDIO_SRST_REQ;
	himci_writel(tmp_reg, REG_BASE_CRG + REG_PERI_CRG46);
}

