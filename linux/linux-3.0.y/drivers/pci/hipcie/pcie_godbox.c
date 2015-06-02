/* pcie0 iatu table */
struct pcie_iatu pcie0_iatu_table[] = {
	{
		.viewport	= 0,
		.region_ctrl_1	= 0x00000004,
		.region_ctrl_2	= 0x90000000,
		.lbar		= 0x70100000,
		.ubar		= 0x0,
		.lar		= 0x701FFFFF,
		.ltar		= 0x01000000,
		.utar		= 0x00000000,
	},
	{
		.viewport	= 1,
		.region_ctrl_1	= 0x00000005,
		.region_ctrl_2	= 0x90000000,
		.lbar		= 0x70200000,
		.ubar		= 0x0,
		.lar		= 0x7FFFFFFF,
		.ltar		= 0x02000000,
		.utar		= 0x00000000,
	},
	{
		.viewport	= 2,
		.region_ctrl_1	= 0x00000002,
		.region_ctrl_2	= 0x80000000,
		.lbar		= 0x61000000 + 0x7800000,
		.ubar		= 0x0,
		.lar		= 0x69000000 - 1,
		.ltar		= 0x00000000,
		.utar		= 0x00000000,
	},
};

struct pcie_iatu pcie1_iatu_table[] = {
	{0,},
	{0,},
	{0,}
};


int pcie_sys_init(unsigned int mode_sel)
{
	unsigned int val;
	void *crg_base = (void *)IO_ADDRESS(REG_BASE_CRG);

	/* soft reset */
	val = readl(crg_base + PERI_CRG37);
	val |= PERI_CRG37_PCIE0_SRST_REQ;
	val |= PERI_CRG37_PCIE0_RST_REQ;
	val |= PERI_CRG37_X2X_PCIE0_SRST_REQ;
	val |= PERI_CRG37_X2X_PCIES_SRST_REQ;
	val |= PERI_CRG37_PCIE_SLOT0_SRST_REQ;
	writel(val, crg_base + PERI_CRG37);
	udelay(1000);

	/* cancle soft reset and enable clock */
	val &= (~PERI_CRG37_PCIE0_SRST_REQ);
	val &= (~PERI_CRG37_PCIE0_RST_REQ);
	val &= (~PERI_CRG37_X2X_PCIE0_SRST_REQ);
	val &= (~PERI_CRG37_X2X_PCIES_SRST_REQ);
	val &= (~PERI_CRG37_PCIE_SLOT0_SRST_REQ);
	val |= PERI_CRG37_PCIE0_CKEN;
	val |= PERI_CRG37_PCIE_PHY_CKEN;
	val |= PERI_CRG37_X2X_PCIES_CKEN;
	val |= PERI_CRG37_PCIE_SLOT_CKEN;
	writel(val, crg_base + PERI_CRG37);
	udelay(1000);

	/* disable controller first */
	val = readl(pcie_sys_base_virt + PERI_PCIE9);
	val &= (~(1 << pcie0_app_ltssm_enable));
	writel(val, pcie_sys_base_virt + PERI_PCIE9);

	/* setup phy */
	writel(PCIE_PHY_VALUE0, pcie_sys_base_virt + PERI_PCIE0);
	writel(PCIE_PHY_VALUE1, pcie_sys_base_virt + PERI_PCIE1);

	if ((mode_sel & PCIE0_MODE_SEL) == PCIE0_MODE_SEL) {

		void *dbi_base = (void *)pcie_info[0].conf_base_addr;

		/* set work mode */
		val = readl(pcie_sys_base_virt + PERI_PCIE2);
		val &= (~(0xf << pcie0_slv_device_type));
		val |= (pcie0_work_mode << pcie0_slv_device_type);
		writel(val, pcie_sys_base_virt + PERI_PCIE2);

		/* setup correct classe code */
		val = readl(dbi_base + 0x8);
		val &= ~(0xffffff00);
		val |= (0x60400<<8);
		writel(val, dbi_base + 0x8);
		/* set hotplug capability */
		val = readl(dbi_base + 0x70);
		val |= (1<<24);
		writel(val, dbi_base + 0x70);
		/* for hotplug support */
		writel(0xff | (1 << 18), dbi_base+0x84);

		val = readl(dbi_base + 0x88);
		val |= 0x2f;
		writel(val, dbi_base + 0x88);

	}

	/* enable controller */
	if (mode_sel & PCIE0_MODE_SEL) {
		val = readl(pcie_sys_base_virt + PERI_PCIE9);
		val |= ((1 << pcie0_app_ltssm_enable) | 0x02200000);
		writel(val, pcie_sys_base_virt + PERI_PCIE9);
	}
	udelay(1000);

	{
		if (mode_sel & PCIE0_MODE_SEL) {

			void *dbi_base;
			unsigned int val;
			dbi_base = (void *)pcie_info[0].conf_base_addr;

			val = readl(dbi_base + 0x4);
			val &= (~7);
			val |= 7;
			writel(val, dbi_base + 0x4);
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
