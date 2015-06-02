#ifdef CONFIG_ARCH_PHOENIX

#define SYS_CTRL_BASE		0x10021000
#define PERI_CRG14		0x38
#define SDIO_CLK_PCTRL		(0x01 << 3)
#define SDIO_CLK_50M		(0x01 << 2)
#define SDIO_CKEN		(0x01 << 1)
#define SDIO_SRST_REQ		(0x01 << 0)

static void hi_mci_sys_ctrl_init(struct himci_host *host)
{
	unsigned int tmp_reg;

	/* enable SDIO clock */
	tmp_reg = himci_readl(IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG14));
	tmp_reg &= ~(SDIO_SRST_REQ | SDIO_CKEN
	| SDIO_CLK_50M | SDIO_CLK_PCTRL);
	tmp_reg |= SDIO_CKEN;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG14));

	/* SDIO soft reset */
	tmp_reg |= SDIO_SRST_REQ;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG14));
	udelay(1000);
	tmp_reg &= ~SDIO_SRST_REQ;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG14));
}

#ifdef CONFIG_PM
static void hi_mci_sys_ctrl_suspend(struct himci_host *host)
{
	unsigned int tmp_reg;

	/* SDIO into soft reset */
	tmp_reg = himci_readl(IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG14));
	tmp_reg |= SDIO_SRST_REQ;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG14));

	/* disable SDIO clock */
	tmp_reg &= ~SDIO_CKEN;
	himci_writel(tmp_reg, IO_ADDRESS(SYS_CTRL_BASE + PERI_CRG14));
}
#endif

#endif /*CONFIG_ARCH_PHOENIX*/

