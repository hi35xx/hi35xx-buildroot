#define SDIO_REG_BASE_CRG               IO_ADDRESS(CRG_REG_BASE)
#define SD_LDO_BASE_CRG                 IO_ADDRESS(0xF8A2011c)

/* SDIO0 REG */
#define PERI_CRG49				REG_CRG49

#define SDIO0_CLK_SEL_MASK		(0x3 << 10)
#define SDIO0_CLK_SEL_99M		(0x0 << 10)
#define SDIO0_CLK_SEL_74_25M		(0x1 << 10)
#define SDIO0_CLK_SEL_49_5M		(0x2 << 10)
#define SDIO0_CLK_SEL_148_5M		(0x3 << 10)
#define SDIO0_CKEN			(0x1 << 9)
#define SDIO0_SRST_REQ			(0x1 << 8)
#define SDIO0_RESET			(0x1 << 8)

#define SDIO0_CLK_99M			99000000
#define SDIO0_CLK_74_25M		74250000
#define SDIO0_CLK_49_5M			49500000
#define SDIO0_CLK_148_5M		148500000
/* SDIO1 REG */
#define SDIO1_CLK_SEL_MASK		(0x3 << 2)
#define SDIO1_CLK_SEL_49_5M		(0x0 << 2)
#define SDIO1_CKEN			(0x1 << 1)
#define SDIO1_RESET			(0x1 << 0)

#define SDIO1_CLK_49_5M			49500000
/* UHS REG EXT */
#define MCI_UHS_REG_EXT			0x108

#define MMC_DRV_PS_SEL_MASK		(0x7 << 23)
#define MMC_SAP_PS_SEL_MASK		(0x7 << 16)

#define MMC_DRV_PS_SEL_0		(0x0 << 23)
#define MMC_DRV_PS_SEL_45		(0x1 << 23)
#define MMC_DRV_PS_SEL_90		(0x2 << 23)
#define MMC_DRV_PS_SEL_135		(0x3 << 23)
#define MMC_DRV_PS_SEL_180		(0x4 << 23)
#define MMC_DRV_PS_SEL_225		(0x5 << 23)
#define MMC_DRV_PS_SEL_270		(0x6 << 23)
#define MMC_DRV_PS_SEL_315		(0x7 << 23)

#define MMC_SAP_PS_SEL_0		(0x0 << 16)
#define MMC_SAP_PS_SEL_45		(0x1 << 16)
#define MMC_SAP_PS_SEL_90		(0x2 << 16)
#define MMC_SAP_PS_SEL_135		(0x3 << 16)
#define MMC_SAP_PS_SEL_180		(0x4 << 16)
#define MMC_SAP_PS_SEL_225		(0x5 << 16)
#define MMC_SAP_PS_SEL_270		(0x6 << 16)
#define MMC_SAP_PS_SEL_315		(0x7 << 16)


#define REG_PAD_CTRL		0x200f0800

/* sdio0:sdxc pad ctrl reg */
#define SDIO0_0_CCLK	IO_ADDRESS(REG_PAD_CTRL + 0xb4)
#define SDIO0_0_CCMD	IO_ADDRESS(REG_PAD_CTRL + 0xb8)
#define SDIO0_0_CDATA0	IO_ADDRESS(REG_PAD_CTRL + 0xbc)
#define SDIO0_0_CDATA1	IO_ADDRESS(REG_PAD_CTRL + 0xc0)
#define SDIO0_0_CDATA2	IO_ADDRESS(REG_PAD_CTRL + 0xc4)
#define SDIO0_0_CDATA3	IO_ADDRESS(REG_PAD_CTRL + 0xc8)

/* sdio0:eMMC pad ctrl reg */
#define SDIO0_1_CCLK	IO_ADDRESS(REG_PAD_CTRL + 0xcc)
#define SDIO0_1_CCMD	IO_ADDRESS(REG_PAD_CTRL + 0xd8)
#define SDIO0_1_CDATA0	IO_ADDRESS(REG_PAD_CTRL + 0xdc)
#define SDIO0_1_CDATA1	IO_ADDRESS(REG_PAD_CTRL + 0xf8)
#define SDIO0_1_CDATA2	IO_ADDRESS(REG_PAD_CTRL + 0xfc)
#define SDIO0_1_CDATA3	IO_ADDRESS(REG_PAD_CTRL + 0xd4)
#define SDIO0_1_CDATA4	IO_ADDRESS(REG_PAD_CTRL + 0xd0)
#define SDIO0_1_CDATA5	IO_ADDRESS(REG_PAD_CTRL + 0xec)
#define SDIO0_1_CDATA6	IO_ADDRESS(REG_PAD_CTRL + 0xe8)
#define SDIO0_1_CDATA7	IO_ADDRESS(REG_PAD_CTRL + 0xf0)

/* sdio1 pad ctrl reg */
#define SDIO1_CCLK	IO_ADDRESS(REG_PAD_CTRL + 0x10)
#define SDIO1_CCMD	IO_ADDRESS(REG_PAD_CTRL + 0x28)
#define SDIO1_CDATA0	IO_ADDRESS(REG_PAD_CTRL + 0x20)
#define SDIO1_CDATA1	IO_ADDRESS(REG_PAD_CTRL + 0x1c)
#define SDIO1_CDATA2	IO_ADDRESS(REG_PAD_CTRL + 0x34)
#define SDIO1_CDATA3	IO_ADDRESS(REG_PAD_CTRL + 0x24)

/* sdio0:sdxc pad ctrl config */
#define SDIO0_0_CCLK_DS_3V3		0xe0
#define SDIO0_0_CCMD_DS_3V3		0xe0
#define SDIO0_0_CDATA0_DS_3V3		0xe0
#define SDIO0_0_CDATA1_DS_3V3		0xe0
#define SDIO0_0_CDATA2_DS_3V3		0xe0
#define SDIO0_0_CDATA3_DS_3V3		0xe0

#define SDIO0_0_CCLK_DS_1V8		0xb0
#define SDIO0_0_CCMD_DS_1V8		0xd0
#define SDIO0_0_CDATA0_DS_1V8		0xd0
#define SDIO0_0_CDATA1_DS_1V8		0xd0
#define SDIO0_0_CDATA2_DS_1V8		0xd0
#define SDIO0_0_CDATA3_DS_1V8		0xd0
/* sdio0:emmc pad ctrl config */
#define SDIO0_1_CCLK_DS			0x40
#define SDIO0_1_CCMD_DS			0x20
#define SDIO0_1_CDATA0_DS		0x20
#define SDIO0_1_CDATA1_DS		0x20
#define SDIO0_1_CDATA2_DS		0x20
#define SDIO0_1_CDATA3_DS		0x20
#define SDIO0_1_CDATA4_DS		0x20
#define SDIO0_1_CDATA5_DS		0x20
#define SDIO0_1_CDATA6_DS		0x20
#define SDIO0_1_CDATA7_DS		0x20

/* sdio1 pad ctrl config */
#define SDIO1_CCLK_DS		0xd0
#define SDIO1_CCMD_DS		0xa0
#define SDIO1_CDATA0_DS		0xa0
#define SDIO1_CDATA1_DS		0xa0
#define SDIO1_CDATA2_DS		0xd0
#define SDIO1_CDATA3_DS		0xa0

#define HIMCI_SUPPORT_CARD_NUM		2

/* config Pin drive capability */
static void himci_set_padctrl(struct himci_slot *slot, unsigned int voltage)
{
	struct himci_host *host = slot->host;
	if (0 == host->devid) {
		/* sdxc 3.3V */
		if ((0 == slot->port) && (MMC_SIGNAL_VOLTAGE_330 == voltage)) {
			himci_writel(SDIO0_0_CCLK_DS_3V3, SDIO0_0_CCLK);
			himci_writel(SDIO0_0_CCMD_DS_3V3, SDIO0_0_CCMD);
			himci_writel(SDIO0_0_CDATA0_DS_3V3, SDIO0_0_CDATA0);
			himci_writel(SDIO0_0_CDATA1_DS_3V3, SDIO0_0_CDATA1);
			himci_writel(SDIO0_0_CDATA2_DS_3V3, SDIO0_0_CDATA2);
			himci_writel(SDIO0_0_CDATA3_DS_3V3, SDIO0_0_CDATA3);
		}
		/* sdxc 1.8V */
		if ((0 == slot->port) && (MMC_SIGNAL_VOLTAGE_180 == voltage)) {
			himci_writel(SDIO0_0_CCLK_DS_1V8, SDIO0_0_CCLK);
			himci_writel(SDIO0_0_CCMD_DS_1V8, SDIO0_0_CCMD);
			himci_writel(SDIO0_0_CDATA0_DS_1V8, SDIO0_0_CDATA0);
			himci_writel(SDIO0_0_CDATA1_DS_1V8, SDIO0_0_CDATA1);
			himci_writel(SDIO0_0_CDATA2_DS_1V8, SDIO0_0_CDATA2);
			himci_writel(SDIO0_0_CDATA3_DS_1V8, SDIO0_0_CDATA3);
		}
		/* emmc 3.3V */
		if (1 == slot->port) { /* emmc */
			himci_writel(SDIO0_1_CCLK_DS, SDIO0_1_CCLK);
			himci_writel(SDIO0_1_CCMD_DS, SDIO0_1_CCMD);
			himci_writel(SDIO0_1_CDATA0_DS, SDIO0_1_CDATA0);
			himci_writel(SDIO0_1_CDATA1_DS, SDIO0_1_CDATA1);
			himci_writel(SDIO0_1_CDATA2_DS, SDIO0_1_CDATA2);
			himci_writel(SDIO0_1_CDATA3_DS, SDIO0_1_CDATA3);
			himci_writel(SDIO0_1_CDATA4_DS, SDIO0_1_CDATA4);
			himci_writel(SDIO0_1_CDATA5_DS, SDIO0_1_CDATA5);
			himci_writel(SDIO0_1_CDATA6_DS, SDIO0_1_CDATA6);
			himci_writel(SDIO0_1_CDATA7_DS, SDIO0_1_CDATA7);
		}
	} else if (1 == host->devid) {
		himci_writel(SDIO1_CCLK_DS, SDIO1_CCLK);
		himci_writel(SDIO1_CCMD_DS, SDIO1_CCMD);
		himci_writel(SDIO1_CDATA0_DS, SDIO1_CDATA0);
		himci_writel(SDIO1_CDATA1_DS, SDIO1_CDATA1);
		himci_writel(SDIO1_CDATA2_DS, SDIO1_CDATA2);
		himci_writel(SDIO1_CDATA3_DS, SDIO1_CDATA3);
	}
}

static void himci_ctr_reset(struct himci_host *host)
{
	unsigned int reg_value;

	reg_value = himci_readl(SDIO_REG_BASE_CRG + PERI_CRG49);

	if (0 == host->devid) {
		reg_value |= SDIO0_RESET;
	} else if (1 == host->devid) {
		reg_value |= SDIO1_RESET;
	} else {
		himci_error("himci host id error!");
		return;
	}

	himci_writel(reg_value, SDIO_REG_BASE_CRG + PERI_CRG49);
}

static void himci_ctr_undo_reset(struct himci_host *host)
{
	unsigned int reg_value;

	reg_value = himci_readl(SDIO_REG_BASE_CRG + PERI_CRG49);

	if (0 == host->devid) {
		reg_value &= ~(SDIO0_RESET);
	} else if (1 == host->devid) {
		reg_value &= ~(SDIO1_RESET);
	} else {
		himci_error("himci host id error!");
		return;
	}

	himci_writel(reg_value, SDIO_REG_BASE_CRG + PERI_CRG49);
}

static void himci_clk_ctrl(struct himci_host *host, bool enable)
{
	unsigned int tmp_reg = 0;
	if (enable) {
		/* enable CRG clk */
		tmp_reg = himci_readl(SDIO_REG_BASE_CRG + PERI_CRG49);
		if (host->devid == 0)
			tmp_reg |= SDIO0_CKEN;
		else
			tmp_reg |= SDIO1_CKEN;

		himci_writel(tmp_reg, SDIO_REG_BASE_CRG + PERI_CRG49);
	} else {
		/* disable CRG clk */
		tmp_reg = himci_readl(SDIO_REG_BASE_CRG + PERI_CRG49);
		if (host->devid == 0)
			tmp_reg &= ~SDIO0_CKEN;
		else
			tmp_reg &= ~SDIO1_CKEN;

		himci_writel(tmp_reg, SDIO_REG_BASE_CRG + PERI_CRG49);
	}
}

static void himci_clk_sel(struct himci_host *host, unsigned int clk)
{
	unsigned int tmp_reg = 0;
	if (host->devid == 0) {
		/* self-adaptive card clock */
		tmp_reg = himci_readl(SDIO_REG_BASE_CRG + PERI_CRG49);
		tmp_reg &= ~SDIO0_CLK_SEL_MASK;
		if (clk >= SDIO0_CLK_148_5M) {
			host->hclk = SDIO0_CLK_148_5M;
			tmp_reg |= SDIO0_CLK_SEL_148_5M;
		} else if (clk >= SDIO0_CLK_99M) {
			host->hclk = SDIO0_CLK_99M;
			tmp_reg |= SDIO0_CLK_SEL_99M;
		} else if (clk >= SDIO0_CLK_74_25M) {
			host->hclk = SDIO0_CLK_74_25M;
			tmp_reg |= SDIO0_CLK_SEL_74_25M;
		} else if (clk >= SDIO0_CLK_49_5M) {
			host->hclk = SDIO0_CLK_49_5M;
			tmp_reg |= SDIO0_CLK_SEL_49_5M;
		} else {
			host->hclk = SDIO0_CLK_49_5M;
			tmp_reg |= SDIO0_CLK_SEL_49_5M;
		}
		himci_writel(tmp_reg, SDIO_REG_BASE_CRG + PERI_CRG49);
	} else if (host->devid == 1) {
		/* self-adaptive card clock */
		tmp_reg = himci_readl(SDIO_REG_BASE_CRG + PERI_CRG49);
		tmp_reg &= ~SDIO1_CLK_SEL_MASK;
		if (clk >= SDIO1_CLK_49_5M) {
			host->hclk = SDIO1_CLK_49_5M;
			tmp_reg |= SDIO1_CLK_SEL_49_5M;
		}
		himci_writel(tmp_reg, SDIO_REG_BASE_CRG + PERI_CRG49);
	}
}

static void hi_mci_sys_ctrl_init(struct himci_host *host,
				 resource_size_t host_crg_addr,
				 unsigned int id)
{
	if ((SDIO_REG_BASE_CRG + PERI_CRG49) == (unsigned int)host_crg_addr) {
		/* disable CRG clk */
		himci_clk_ctrl(host, false);

		if (host->devid == 0) {
#ifdef CONFIG_HIMCIV200_SDIO0
			/* self-adaptive card clock */
			himci_clk_sel(host, CONFIG_HIMCIV200_SDIO0_CLK);
#endif
		} else if (host->devid == 1) {
#ifdef CONFIG_HIMCIV200_SDIO1
			/* self-adaptive card clock */
			himci_clk_sel(host, CONFIG_HIMCIV200_SDIO1_CLK);
#endif
		}

		/* enable CRG clk */
		himci_clk_ctrl(host, true);

		himci_ctr_reset(host);
		udelay(100);
		himci_ctr_undo_reset(host);
	} else {
		himci_trace(5, "CRG addr is not correct!!!");
	}

	return;
}

#ifdef CONFIG_PM
static void hi_mci_sys_ctrl_suspend(struct himci_host *host,
				    resource_size_t host_crg_addr)
{
	if ((SDIO_REG_BASE_CRG + PERI_CRG49) == (unsigned int)host_crg_addr) {
		himci_ctr_reset(host);
		udelay(1000);
		himci_clk_ctrl(host, false);
	}

	return;
}
#endif
