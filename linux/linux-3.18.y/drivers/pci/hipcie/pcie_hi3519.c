#include <mach/io.h>
#include "pcie_hi3519.h"

static void *dbi_base;
static int __arch_pcie_info_setup(struct pcie_info *info, int *controllers_nr);
static int __arch_pcie_sys_init(struct pcie_info *info);
static void __arch_pcie_info_release(struct pcie_info *info);

struct pcie_iatu iatu_table[] = {
		{
			.viewport	= 0,
			.region_ctrl_1  = 0x00000004,
			.region_ctrl_2  = 0x90000000,
			.lbar           = PCIE_EP_CONF_BASE + (1<<20),
			.ubar           = 0x0,
			.lar            = PCIE_EP_CONF_BASE + (2<<20) - 1,
			.ltar           = 0x01000000,
			.utar           = 0x00000000,
		},
		{
			.viewport       = 1,
			.region_ctrl_1  = 0x00000005,
			.region_ctrl_2  = 0x90000000,
			.lbar           = PCIE_EP_CONF_BASE + (2<<20),
			.ubar           = 0x0,
			.lar            = PCIE_EP_CONF_BASE + (__128MB__ - 1),
			.ltar           = 0x02000000,
			.utar           = 0x00000000,
		},
};

static void __arch_pcie_preinit(void)
{

}

static struct resource pcie_mem;
static struct resource pcie_io;
static void __arch_get_pcie_res(int controller,
		struct resource **pmem,
		struct resource **pio)
{
	*pmem = &pcie_mem;
	(*pmem)->start = PCIE_MEM_BASE;
	(*pmem)->end = PCIE_MEM_BASE + __128MB__ - 0x100000 - 1;
	(*pmem)->flags = IORESOURCE_MEM;
	(*pmem)->name = "memory";

	*pio = &pcie_io;
	(*pio)->start = PCIE_MEM_BASE + __128MB__ - 0x100000;
	(*pio)->end = PCIE_MEM_BASE + __128MB__ - 1;
	(*pio)->flags = IORESOURCE_IO;
	(*pio)->name = "io";
}

static int __arch_get_int_irq(struct pcie_info *info, u8 pin)
{
	switch (pin) {
	case PCIE_INTA_PIN:
		return PCIE_IRQ_INTA;
	case PCIE_INTB_PIN:
		return PCIE_IRQ_INTB;
	case PCIE_INTC_PIN:
		return PCIE_IRQ_INTC;
	case PCIE_INTD_PIN:
		return PCIE_IRQ_INTD;
	default:
		pcie_error("Unknown pin for mapping irq!");
		return -1;
	}
}

static void __arch_config_iatu_tbl(struct pcie_info *info,
		struct pci_sys_data *sys)
{
	int i;
	void __iomem *config_base = (void __iomem *)info->conf_base_addr;
	struct pcie_iatu *ptable = iatu_table;
	int table_size = ARRAY_SIZE(iatu_table);

	for (i = 0; i < table_size; i++) {
		writel((ptable + i)->viewport, config_base + 0x900);
		writel((ptable + i)->lbar, config_base + 0x90c);
		writel((ptable + i)->ubar, config_base + 0x910);
		writel((ptable + i)->lar,  config_base + 0x914);
		writel((ptable + i)->ltar, config_base + 0x918);
		writel((ptable + i)->utar, config_base + 0x91c);
		writel((ptable + i)->region_ctrl_1, config_base + 0x904);
		writel((ptable + i)->region_ctrl_2, config_base + 0x908);
	}

}

static inline int __arch_check_pcie_link(struct pcie_info *info)
{
	int val;

	val = readl(dbi_base + PCIE_SYS_STATE0);
	return ((val & (1 << PCIE_XMLH_LINK_UP))
			&& (val & (1 << PCIE_RDLH_LINK_UP))) ? 1 : 0;
}

/*
 * ret:
 */
static int __arch_pcie_info_setup(struct pcie_info *info, int *controllers_nr)
{
	unsigned int mem_size = CONFIG_PCIE0_DEVICES_MEM_SIZE;
	unsigned int cfg_size = CONFIG_PCIE0_DEVICES_CONFIG_SIZE;

	if ((mem_size > __128MB__) || (cfg_size > __128MB__)) {
		pcie_error(
		"Invalid parameter: pcie mem size[0x%x], pcie cfg size[0x%x]!",
		mem_size, cfg_size);
		return -EINVAL;
	}

	info->controller = 0;

	/* RC configuration space */
	info->conf_base_addr = (unsigned int)ioremap_nocache(PCIE_DBI_BASE,
							 __8KB__);
	if (!info->conf_base_addr) {
		pcie_error("Address mapping for RC dbi failed!");
		return -EIO;
	}

	/* Configuration space for all EPs */
	info->base_addr = (unsigned int)ioremap_nocache(PCIE_EP_CONF_BASE,
							 cfg_size);
	if (!info->base_addr) {
		iounmap((void *)info->conf_base_addr);
		pcie_error("Address mapping for EPs cfg failed!");
		return -EIO;
	}

	*controllers_nr = 1;

	return 0;

}

static void __arch_pcie_info_release(struct pcie_info *info)
{
	if (info->base_addr)
		iounmap((void *)info->base_addr);

	if (info->conf_base_addr)
		iounmap((void *)info->conf_base_addr);
}

static int __arch_pcie_sys_init(struct pcie_info *info)
{
	unsigned int val;
	void *crg_base = (void *)IO_ADDRESS(PERI_CRG_BASE);

	dbi_base = (void *)info->conf_base_addr;

#ifdef CONFIG_ARCH_HI3519V101
	/*open pcie pad oe*/
	val = readl(crg_base + PERI_CRG44);
	val &= ~(PCIE_PAD_OE_MASK);
	writel(val, crg_base + PERI_CRG44);
	
	/* refclk output from phy */
	writel(PCIE_CLKREQ_FILTER_BYPASS,
		(void *)IO_ADDRESS(MISC_CTRL_BASE + MISC_CTRL34));
	mdelay(1);
#endif

	/*
	 * Disable PCIE
	 */
	val = readl(dbi_base + PCIE_SYS_CTRL7);
	val &= (~(1 << PCIE_APP_LTSSM_ENBALE));
	val |= (1 << PCIE_ACCESS_ENABLE);
	writel(val, dbi_base + PCIE_SYS_CTRL7);

	/*
	 * Reset
	 */
	val = readl(crg_base + PERI_CRG44);
	val |= (1 << PCIE_X2_SRST_REQ);
	writel(val, crg_base + PERI_CRG44);

	/*
	 * Retreat from the reset state
	 */
	udelay(500);
	val = readl(crg_base + PERI_CRG44);
	val &= ~(1 << PCIE_X2_SRST_REQ);
	writel(val, crg_base + PERI_CRG44);
	mdelay(10);


	/*
	 * PCIE RC work mode
	 */
	val = readl(dbi_base + PCIE_SYS_CTRL0);
	val &= (~(0xf << PCIE_DEVICE_TYPE));
	val |= (PCIE_WM_RC << PCIE_DEVICE_TYPE);
	writel(val, dbi_base + PCIE_SYS_CTRL0);

	/*
	 * Enable clk
	 */
	val = readl(crg_base + PERI_CRG44);
	val |= ((1 << PCIE_X2_BUS_CKEN)
			| (1 << PCIE_X2_SYS_CKEN)
			| (1 << PCIE_X2_PIPE_CKEN)
			| (1 << PCIE_X2_AUX_CKEN));
	writel(val, crg_base + PERI_CRG44);

	mdelay(10);

	/*
	 * Set PCIE controller class code to be PCI-PCI bridge device
	 */
	val = readl(dbi_base + PCI_CLASS_REVISION);
	val &= ~(0xffffff00);
	val |= (0x60400 << 8);
	writel(val, dbi_base + PCI_CLASS_REVISION);
	udelay(1000);

#ifdef CONFIG_ARCH_HI3519V101
	/* phy always work at 5Gbps */
	writel(COM_PHY_TEST_VAL1,
		(void *)IO_ADDRESS(MISC_CTRL_BASE + MISC_CTRL33));
	writel(COM_PHY_TEST_VAL2,
		(void *)IO_ADDRESS(MISC_CTRL_BASE + MISC_CTRL33));
	writel(COM_PHY_TEST_VAL1,
		(void *)IO_ADDRESS(MISC_CTRL_BASE + MISC_CTRL33));
	writel(0x0, (void *)IO_ADDRESS(MISC_CTRL_BASE + MISC_CTRL33));
	
	/* default deemphasis to -3.5 dB */
	writel(DEEMPHASIS_VAL, (void *)IO_ADDRESS(PCIE_DBI_BASE + DEEMPHASIS_REG));
#endif

	/*
	 * Enable controller
	 */
	val = readl(dbi_base + PCIE_SYS_CTRL7);
	val |= (1 << PCIE_APP_LTSSM_ENBALE);
	writel(val, dbi_base + PCIE_SYS_CTRL7);
	udelay(1000);

	val = readl(dbi_base + PCI_COMMAND);
	val |= 7;
	writel(val, dbi_base + PCI_COMMAND);

	/* set pcie to gen 1*/
#if 0
	writel(0x1, dbi_base + 0x8BC);
	val = readl(dbi_base + 0x7C);
	val = ((val >> 4) << 4) | 0x1;
	writel(val, dbi_base + 0x7C);
#endif
	return 0;
}

static void __arch_pcie_sys_exit(void)
{
	unsigned int val;
	void *crg_base = (void *)IO_ADDRESS(PERI_CRG_BASE);

	/*
	 * Disable PCIE
	 */
	val = readl(dbi_base + PCIE_SYS_CTRL7);
	val &= (~(1 << PCIE_APP_LTSSM_ENBALE));
	writel(val, dbi_base + PCIE_SYS_CTRL7);

	/*
	 * Reset
	 */
	val = readl(crg_base + PERI_CRG44);
	val |= (1 << PCIE_X2_SRST_REQ);
	writel(val, crg_base + PERI_CRG44);

	udelay(1000);

	/*
	 * Disable clk
	 */
	val = readl(crg_base + PERI_CRG44);
	val &= (~(1 << PCIE_X2_AUX_CKEN));
	val &= (~(1 << PCIE_X2_PIPE_CKEN));
	val &= (~(1 << PCIE_X2_SYS_CKEN));
	val &= (~(1 << PCIE_X2_BUS_CKEN));
	writel(val, crg_base + PERI_CRG44);

	udelay(1000);
}
