
#if defined(CONFIG_ARCH_HI3535)
#include "pcie_hi3535.h"
#endif

struct pcie_iatu pcie0_iatu_table[] = {
		{
			.viewport	= 0,
			.region_ctrl_1  = 0x00000004,
			.region_ctrl_2  = 0x90000000,
			.lbar           = 0x40100000,
			.ubar           = 0x0,
			.lar            = 0x401FFFFF,
			.ltar           = 0x01000000,
			.utar           = 0x00000000,
		},
		{
			.viewport       = 1,
			.region_ctrl_1  = 0x00000005,
			.region_ctrl_2  = 0x90000000,
			.lbar           = 0x40200000,
			.ubar           = 0x0,
			.lar            = 0x4FFFFFFF,
			.ltar           = 0x02000000,
			.utar           = 0x00000000,
		},
		{
			.viewport       = 2,
			.region_ctrl_1  = 0x00000002,
			.region_ctrl_2  = 0x80000000,
			.lbar           = 0x30000000 + 0x7800000,
			.ubar           = 0x0,
			.lar            = 0x38000000 - 1,
			.ltar           = 0x00000000,
			.utar           = 0x00000000,
		},
};

#if 1
void config_controller_iatu_table(int controller, struct pci_sys_data *sys)
{
	int i;
	void *dbi_base = NULL;
	struct pcie_iatu *p_table;

	int table_size = 0;

#ifdef CONFIG_PCIE0_SEL
	if (controller == pcie_controller_0) {
		dbi_base = ioremap_nocache(DBI_BASE_ADDR_0, 0x1000);
		if (!dbi_base) {
			pcie_error("Cannot map dbi base for pcie0");
			return;
		}

		p_table = pcie0_iatu_table;
		table_size = ARRAY_SIZE(pcie0_iatu_table);

	}
#endif

#ifdef CONFIG_PCIE1_SEL
	if (controller == pcie_controller_1) {
		int ctl1_lbar_offset = 0;
		int controller_bus = sys->busnr;

		dbi_base = ioremap_nocache(DBI_BASE_ADDR_1, 0x1000);
		if (!dbi_base) {
			pcie_error("Cannot map dbi base for pcie0");
			return;
		}

		p_table = pcie1_iatu_table;
		table_size = ARRAY_SIZE(pcie1_iatu_table);

		ctl1_lbar_offset = (controller_bus + 1) << 20;

		p_table->lbar |= ctl1_lbar_offset;
		p_table->lar |= ctl1_lbar_offset;

		ctl1_lbar_offset = (controller_bus + 2) << 20;
		(p_table + 1)->lbar |= ctl1_lbar_offset;

	}
#endif
	if ((controller != pcie_controller_0)
			&& (controller != pcie_controller_1)) {
		pcie_error("Invalid pcie controller number!");
		return;
	}

	if (NULL == dbi_base) {
		pcie_error("PCIe db_base is NULL!");
		return;
	}

	for (i = 0; i < table_size; i++) {
		writel((p_table + i)->viewport, dbi_base + 0x700 + 0x200);
		writel((p_table + i)->lbar, dbi_base + 0x700 + 0x20c);
		writel((p_table + i)->ubar, dbi_base + 0x700 + 0x210);
		writel((p_table + i)->lar, dbi_base + 0x700 + 0x214);
		writel((p_table + i)->ltar, dbi_base + 0x700 + 0x218);
		writel((p_table + i)->utar, dbi_base + 0x700 + 0x21c);
		writel((p_table + i)->region_ctrl_1,
				dbi_base + 0x700 + 0x204);
		writel((p_table + i)->region_ctrl_2,
				dbi_base + 0x700 + 0x208);
	}

	udelay(1000);
	iounmap(dbi_base);
	dbi_base = NULL;
}
#endif

static inline int pcie_check_link_status(struct pcie_info *info)
{
	int val;

	udelay(2000);
	val = readl(pcie_sys_base_virt + PCIE_SYS_CTRL15);
	return ((val & (1 << PCIE_XMLH_LINK_UP))
			&& (val & (1 << PCIE_RDLH_LINK_UP))) ? 1 : 0;
}

static int pcie_sys_init(unsigned int mode_sel)
{
	unsigned int val;

	void *crg_base = (void *)ioremap_nocache(PERI_CRG_BASE, 0x1000);
	if (!crg_base)
		return -EIO;

	/*
	 * Disable PCIE
	 */
	val = readl(pcie_sys_base_virt + PCIE_SYS_CTRL7);
	val &= (~(1 << PCIE0_APP_LTSSM_ENBALE));
	writel(val, pcie_sys_base_virt + PCIE_SYS_CTRL7);

	/*
	 * Setup PHY (clock from PLL)
	 */
	val = readl(pcie_sys_base_virt + PCIE_SYS_CTRL13);
	val &= (~(1 << PCIE_CFG_REF_USE_PAD));
	writel(val, pcie_sys_base_virt + PCIE_SYS_CTRL13);

	/*
	 * Reset
	 */
	val = readl(crg_base + PERI_CRG44);
	val |= (1 << PCIE_BUS_SRST_REQ);
	writel(val, crg_base + PERI_CRG44);

	/*
	 * PCIE RC work mode
	 */
	val = readl(pcie_sys_base_virt + PCIE_SYS_CTRL0);
	val &= (~(0xf << PCIE_DEVICE_TYPE));
	val |= (PCIE_WM_RC << PCIE_DEVICE_TYPE);
	writel(val, pcie_sys_base_virt + PCIE_SYS_CTRL0);

	/*
	 * Enable clk
	 */
	val = readl(crg_base + PERI_CRG44);
	val |= ((1 << PCIE_BUS_CKEN) | (1 << PCIE_RX0_CKEN)
			| (1 << PCIE_AUX_CKEN)
			| (1 << PCIE_CKO_ALIVE_CKEN)
			| (1 << PCIE_MPLL_DWORD_CKEN)
			| (1 << PCIE_REFCLK_CKEN));
	writel(val, crg_base + PERI_CRG44);

	/*
	 * Retreat from the reset state
	 */
	udelay(500);
	val = readl(crg_base + PERI_CRG44);
	val &= ~(1 << PCIE_BUS_SRST_REQ);
	writel(val, crg_base + PERI_CRG44);

	if ((mode_sel & PCIE0_MODE_SEL) == PCIE0_MODE_SEL) {
		void *dbi_base = ioremap_nocache(DBI_BASE_ADDR_0, 0x1000);
		if (!dbi_base) {
			pcie_error("Cannot map dbi base for pcie0");
			return -EIO;
		}

		/*
		 * Set PCIE controller class code to be PCI-PCI bridge device
		 */
		val = readl(dbi_base + 0x8);
		val &= ~(0xffffff00);
		val |= (0x60400<<8);
		writel(val, dbi_base + 0x8);

		udelay(1000);
		iounmap(dbi_base);
		dbi_base = NULL;
	}

	/*
	 * Enable controller
	 */
	if (mode_sel & PCIE0_MODE_SEL) {
		val = readl(pcie_sys_base_virt + PCIE_SYS_CTRL7);
		val |= (1 << PCIE0_APP_LTSSM_ENBALE);
		writel(val, pcie_sys_base_virt + PCIE_SYS_CTRL7);
	}

	udelay(1000);

	{
		void *dbi_base  = 0;
		unsigned int val;
		if (mode_sel & PCIE0_MODE_SEL) {
			dbi_base = ioremap_nocache(DBI_BASE_ADDR_0, 0x1000);
			if (!dbi_base)
				return -EIO;

			val = readl(dbi_base + 0x4);
			val |= 7;
			writel(val, dbi_base + 0x4);

			iounmap(dbi_base);
		}
	}

	return 0;
}


#ifdef CONFIG_PM
int hisi_pcie_plat_driver_suspend(struct device *dev)
{
	void *crg_base = (void *)IO_ADDRESS(REG_BASE_CRG);
	unsigned int val;

	/*
	 * Disable PCIE
	 */
	val = readl(pcie_sys_base_virt + PCIE_SYS_CTRL7);
	val &= (~(1 << PCIE0_APP_LTSSM_ENBALE));
	writel(val, pcie_sys_base_virt + PCIE_SYS_CTRL7);

	/*
	 * Reset
	 */
	val = readl(crg_base + PERI_CRG44);
	val |= (1 << PCIE_BUS_SRST_REQ);
	writel(val, crg_base + PERI_CRG44);

	udelay(1000);

	/*
	 * Disable clk
	 */
	val = readl(crg_base + PERI_CRG44);
	val &= (~(1 << PCIE_BUS_CKEN));
	val &= (~(1 << PCIE_RX0_CKEN));
	val &= (~(1 << PCIE_AUX_CKEN));
	val &= (~(1 << PCIE_CKO_ALIVE_CKEN));
	val &= (~(1 << PCIE_MPLL_DWORD_CKEN));
	val &= (~(1 << PCIE_REFCLK_CKEN));
	writel(val, crg_base + PERI_CRG44);

	udelay(1000);
	return 0;
}

int hisi_pcie_plat_driver_resume(struct device *pdev)
{
	return pcie_sys_init(hisi_pcie_mode_sel);
}

const struct dev_pm_ops hisi_pcie_pm_ops = {
	.suspend = NULL,
	.suspend_noirq = hisi_pcie_plat_driver_suspend,
	.resume = NULL,
	.resume_noirq = hisi_pcie_plat_driver_resume
};

#define HISI_PCIE_PM_OPS (&hisi_pcie_pm_ops)
#else
#define HISI_PCIE_PM_OPS NULL
#endif


