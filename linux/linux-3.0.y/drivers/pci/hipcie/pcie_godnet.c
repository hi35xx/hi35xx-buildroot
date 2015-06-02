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

struct pcie_iatu pcie1_iatu_table[] = {
		{
			.viewport	= 0,
			.region_ctrl_1	= 0x00000004,
			.region_ctrl_2	= 0x90000000,
			.lbar		= 0x70000000,
			.ubar		= 0x0,
			.lar		= 0x700FFFFF,
			.ltar		= 0x01000000,
			.utar		= 0x00000000,
		},
		{
			.viewport	= 1,
			.region_ctrl_1	= 0x00000005,
			.region_ctrl_2	= 0x90000000,
			.lbar		= 0x70000000,
			.ubar		= 0x0,
			.lar		= 0x7FFFFFFF,
			.ltar		= 0x02000000,
			.utar		= 0x00000000,
		},
		{
			.viewport	= 2,
			.region_ctrl_1	= 0x00000002,
			.region_ctrl_2	= 0x80000000,
			.lbar		= 0x60000000 + 0x7800000,
			.ubar		= 0x0,
			.lar		= 0x68000000 - 1,
			.ltar		= 0x00000000,
			.utar		= 0x00000000,
		},
};


static int __init pcie_sys_init(unsigned int mode_sel)
{
	unsigned int val;

	void *crg_base = (void *)ioremap_nocache(PERI_CRG_BASE, 0x1000);
	if (!crg_base)
		return -EIO;

	if ((mode_sel & PCIE0_MODE_SEL) == PCIE0_MODE_SEL) {
		/* disable pcie controller 0 */
		val = readl(pcie_sys_base_virt + PERI_PCIE9);
		val &= (~(1 << pcie0_app_ltssm_enable));
		writel(val, pcie_sys_base_virt + PERI_PCIE9);

		/* enable pcie controller 0's clock
			added for asic only. */
		val = readl(crg_base + PCIE_RESET_REG_OFFSET);
		val |= 0x20;
		writel(val, crg_base + PCIE_RESET_REG_OFFSET);


		/* setup pcie controller 0's phy
			determine to use ref clock from inside(chip)
			or outside. */
		writel(PCIE_PHY_VALUE0, pcie_sys_base_virt + PERI_PCIE0);


		/* pcie controller 0: technical parameters
			further setup for phy, here we use its default value,
			so, comment it. */
		writel(PCIE_PHY_VALUE1, pcie_sys_base_virt + PERI_PCIE1);

		udelay(500);

		/* setup work mode for pcie controller0(RC). */
		val = readl(pcie_sys_base_virt + PERI_PCIE2);
		val &= (~(0xf << pcie0_slv_device_type));
		val |= (pcie0_work_mode << pcie0_slv_device_type);
		writel(val, pcie_sys_base_virt + PERI_PCIE2);

	}

	/* we need a little delay here, otherwise os will hang on. */
	udelay(1000);

	if ((mode_sel & PCIE1_MODE_SEL) == PCIE1_MODE_SEL) {
		/* disable pcie controller 1 */
		val = readl(pcie_sys_base_virt + PERI_PCIE77);
		val &= (~(1 << pcie1_app_ltssm_enable));
		writel(val, pcie_sys_base_virt + PERI_PCIE77);

		/* enable pcie controller 1's clock.
			for asic only. */
		val = readl(crg_base + PCIE_RESET_REG_OFFSET);
		val |= 0x02;
		writel(val, crg_base + PCIE_RESET_REG_OFFSET);


		/* setup pcie controller 1's phy
			determine to use ref clock from inside,
			or outside. */
		writel(PCIE_PHY_VALUE0, pcie_sys_base_virt + PERI_PCIE79);

		/* pcie controller 1: technical parameters
			further setup for phy, we use its default value.
			comment it if you wish. */
		writel(PCIE_PHY_VALUE1, pcie_sys_base_virt + PERI_PCIE80);

		udelay(500);

		/* setup work mode for pcie controller 1(RC). */
		val = readl(pcie_sys_base_virt + PERI_PCIE70);
		val &= (~(0xf << pcie1_slv_device_type));
		val |= (pcie1_work_mode << pcie1_slv_device_type);
		writel(val, pcie_sys_base_virt + PERI_PCIE70);

	}

	/* reset pcie controller 1 */
	writel(PCIE_RESET_VALUE, crg_base + PCIE_RESET_REG_OFFSET);
	/* unreset pcie controller 1,
		it's better to have a short delay in between */
	udelay(1000);

	writel(PCIE_RESET_CANCEL_VALUE, crg_base + PCIE_RESET_REG_OFFSET);

	udelay(1000);
	/* set pcie controller 0's class code. */
	if ((mode_sel & PCIE0_MODE_SEL) == PCIE0_MODE_SEL) {
		void *dbi_base = ioremap_nocache(DBI_BASE_ADDR_0, 0x1000);
		if (!dbi_base) {
			pcie_error("Cannot map dbi base for pcie0");
			return -EIO;
		}

		/* set pcie controller class code to pci-pci bridge device */
		val = readl(dbi_base + 0x8);
		val &= ~(0xffffff00);
		val |= (0x60400<<8);
		writel(val, dbi_base + 0x8);

		udelay(1000);
		iounmap(dbi_base);
		dbi_base = NULL;
	}

	/* set pcie controller 1's class code. */
	if ((mode_sel & PCIE1_MODE_SEL) == PCIE1_MODE_SEL) {
		void *dbi_base = ioremap_nocache(DBI_BASE_ADDR_1, 0x1000);
		if (!dbi_base) {
			pcie_error("Cannot map dbi base for pcie1");
			return -EIO;
		}

		val = readl(dbi_base + 0x8);
		val &= ~(0xffffff00);
		val |= (0x60400<<8);
		writel(val, dbi_base + 0x8);

		udelay(1000);
		iounmap(dbi_base);
		dbi_base = NULL;
	}

	/* enable controller 0. */
	if ((mode_sel & PCIE0_MODE_SEL) == PCIE0_MODE_SEL) {
		val = readl(pcie_sys_base_virt + PERI_PCIE9);
		/*val |= ((1 << pcie0_app_ltssm_enable) | 0x02200000);*/
		val |= (1 << pcie0_app_ltssm_enable);
		writel(val, pcie_sys_base_virt + PERI_PCIE9);
	}

	/* enable controller 1. */
	if ((mode_sel & PCIE1_MODE_SEL) == PCIE1_MODE_SEL) {
		val = readl(pcie_sys_base_virt + PERI_PCIE77);
		val |= (1 << pcie1_app_ltssm_enable);
		writel(val, pcie_sys_base_virt + PERI_PCIE77);
	}

	/** config the cmd register **/
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

		if (mode_sel & PCIE1_MODE_SEL) {
			dbi_base = ioremap_nocache(DBI_BASE_ADDR_1, 0x1000);
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
	/* disable controller first */
	val = readl(pcie_sys_base_virt + PERI_PCIE9);
	val &= (~(1 << pcie0_app_ltssm_enable));
	writel(val, pcie_sys_base_virt + PERI_PCIE9);

	/* soft reset */
	val = readl(crg_base + PERI_CRG37);
	val |= PERI_CRG37_PCIE0_SRST_REQ;
	val |= PERI_CRG37_PCIE0_RST_REQ;
	val |= PERI_CRG37_X2X_PCIE0_SRST_REQ;
	val |= PERI_CRG37_X2X_PCIES_SRST_REQ;
	val |= PERI_CRG37_PCIE_SLOT0_SRST_REQ;
	writel(val, crg_base + PERI_CRG37);
	udelay(1000);

	/* disable clock */
	val = readl(crg_base + PERI_CRG37);
	val &= (~PERI_CRG37_PCIE0_CKEN);
	val &= (~PERI_CRG37_PCIE_PHY_CKEN);
	val &= (~PERI_CRG37_X2X_PCIES_CKEN);
	val &= (~PERI_CRG37_PCIE_SLOT_CKEN);
	writel(val, crg_base + PERI_CRG37);

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


