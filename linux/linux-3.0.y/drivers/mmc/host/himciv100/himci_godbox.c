#ifdef CONFIG_ARCH_GODBOX

#define SYS_CTRL_BASE		0x101f5000
#define PERI_CRG46		0xf8
#define SDIO_CLK_PCTRL		(0x01 << 17)
#define SDIO_CLK_50M		(0x01 << 16)
#define SDIO_HCLKEN		(0x01 << 9)
#define SDIO_CKEN		(0x01 << 8)
#define SDIO_SRST_REQ		(0x01 << 0)

static void hi_mci_sys_ctrl_init(struct himci_host *host)
{
	unsigned int tmp_reg;

	/* enable SDIO clock */
	tmp_reg = himci_readl(IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG46));
	tmp_reg &= ~(SDIO_SRST_REQ | SDIO_CKEN | SDIO_HCLKEN
		 | SDIO_CLK_50M | SDIO_CLK_PCTRL);
	tmp_reg |= SDIO_CKEN | SDIO_HCLKEN;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG46));

	/* SDIO soft reset */
	tmp_reg |= SDIO_SRST_REQ;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG46));
	udelay(1000);
	tmp_reg &= ~SDIO_SRST_REQ;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG46));
}

#ifdef CONFIG_PM
static void hi_mci_sys_ctrl_suspend(struct himci_host *host)
{
	unsigned int tmp_reg;

	/* SDIO into soft reset */
	tmp_reg = himci_readl(IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG46));
	tmp_reg |= SDIO_SRST_REQ;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG46));

	/* disable SDIO clock */
	tmp_reg &= ~(SDIO_CKEN | SDIO_HCLKEN);
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG46));
}
#endif

#endif /*CONFIG_ARCH_GODBOX*/

