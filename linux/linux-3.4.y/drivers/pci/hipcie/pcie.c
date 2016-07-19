#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/mbus.h>
#include <asm/irq.h>
#include <asm/mach/pci.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/version.h>
#include <asm/siginfo.h>

#define PCIE_DEBUG_LEVEL_REG    1
#define PCIE_DEBUG_LEVEL_FUNC   2
#define PCIE_DEBUG_LEVEL_MODULE 3

#define PCIE_DEBUG_LEVEL PCIE_DEBUG_LEVEL_MODULE

#define PCIE_DEBUG
#ifdef PCIE_DEBUG
#define pcie_debug(level, str, arg...)\
	do {\
		if ((level) <= PCIE_DEBUG_LEVEL) {\
			printk(KERN_DEBUG "%s->%d," str "\n", \
					__func__, __LINE__, ##arg);\
		} \
	} while (0)
#else
#define pcie_debug(level, str, arg...)
#endif

#define pcie_assert(con)\
	do {\
		if (!(con)) {\
			printk(KERN_ERR "%s->%d,assert fail!\n", \
					__func__, __LINE__);\
		} \
	} while (0)

#define pcie_error(str, arg...)\
	do {\
		printk(KERN_ERR "%s->%d" str "\n", __func__, __LINE__, ##arg);\
	} while (0)

enum pcie_sel {
	/*
	 * No controller selected.
	 */
	pcie_sel_none,
	/*
	 * PCIE0 selected.
	 */
	pcie0_x1_sel,
	/*
	 * PCIE1 selected.
	 */
	pcie1_x1_sel
};

enum pcie_rc_sel {
	pcie_controller_unselected,
	pcie_controller_selected
};

enum pcie_controller {
	pcie_controller_none = -1,
	pcie_controller_0 = 0,
	pcie_controller_1 = 1
};

struct pcie_iatu {
	unsigned int viewport;          /* iATU Viewport Register        */
	unsigned int region_ctrl_1;     /* Region Control 1 Register     */
	unsigned int region_ctrl_2;     /* Region Control 2 Register     */
	unsigned int lbar;              /* Lower Base Address Register   */
	unsigned int ubar;              /* Upper Base Address Register   */
	unsigned int lar;               /* Limit Address Register        */
	unsigned int ltar;		/* Lower Target Address Register */
	unsigned int utar;              /* Upper Target Address Register */
};

#define MAX_IATU_PER_CTRLLER	(6)

struct pcie_info {
	/*
	 * Root bus number
	 */
	u8		root_bus_nr;
	enum		pcie_controller controller;

	/*
	 * Devices configuration space base
	 */
	unsigned int	base_addr;

	/*
	 * RC configuration space base
	 */
	unsigned int	conf_base_addr;
};

static struct pcie_info pcie_info[2];
static int pcie_controllers_nr;

static unsigned int pcie0_sel = pcie_controller_unselected;
static unsigned int pcie0_mem_space_size = 0x0;
static unsigned int pcie0_cfg_space_size = 0x0;

static unsigned int pcie1_sel = pcie_controller_unselected;
static unsigned int pcie1_mem_space_size = 0x0;
static unsigned int pcie1_cfg_space_size = 0x0;

static void * __iomem pcie_sys_base_virt;
static unsigned int pcie_errorvalue;

/*
 * For number 22 bus err.
 */
static int pcie_fault(unsigned long addr, unsigned int fsr,
		struct pt_regs *regs)
{
	pcie_errorvalue = 1;
	return 0;
}

static DEFINE_SPINLOCK(cw_lock);

#define PCIE0_MODE_SEL  (1 << 0)
#define PCIE1_MODE_SEL  (1 << 1)
static unsigned int hisi_pcie_mode_sel;

#if defined(CONFIG_ARCH_HI3535)
#include "pcie_hi3535.c"
#else
#error You must have defined either CONFIG_ARCH_HI3535 or ...
#endif

/*
 * PCIE memory size bootargs: pcie0_mem=0xa00000;pcie1_mem=0xa00000
 */
static int __init pcie0_mem_parser(char *str)
{
	unsigned int size;
	if (strict_strtoul(str, 16, (long *)&size) < 0)
		return 0;
	/*if size >= 128M, set default 120M*/
	if (size >= 0x10000000)
		size = 0x7800000;
	pcie0_mem_space_size = size;

	return 1;
}
__setup("pcie0_mem=", pcie0_mem_parser);

/*
 * PCIE sel bootargs: pcie0_sel=x1 pcie1=x1 or pcie1=x2
 * Any other value after "pcieX_sel=" prefix
 * will be treated as none controller selected.
 * e.g. "pcie0_sel=none" will be treated as no PCIE0 selected.
 */
static int __init pcie0_sel_parser(char *str)
{
	if (strncasecmp(str, "x1", 2) == 0)
		pcie0_sel = pcie0_x1_sel;
	else
		pcie0_sel = pcie_sel_none;

	return 1;
}
__setup("pcie0_sel=", pcie0_sel_parser);

#ifdef CONFIG_PCIE0_SEL
static struct resource pcie0_mem_space = {
	.name   = "PCIE0 memory space",
	.start  = PCIE0_BASE_ADDR_PHYS,
	.end    = 0,
	.flags  = IORESOURCE_MEM,
};

static struct resource pcie0_io_space = {
	.name   = "PCIE0 io space",
	.start  = 0,
	.end    = 0,
	.flags  = IORESOURCE_IO,
};
#endif

#ifdef CONFIG_PCIE1_SEL
static struct resource pcie1_mem_space = {
	.name   = "PCIE1 memory space",
	.start  = PCIE1_BASE_ADDR_PHYS,
	.end    = 0,
	.flags  = IORESOURCE_MEM,
};

static struct resource pcie1_io_space = {
	.name   = "PCIE1 io space",
	.start  = 0,
	.end    = 0,
	.flags  = IORESOURCE_IO,
};
#endif

static int init_pcie_controller_info(int controller)
{
	unsigned int dbi_base_addr = 0x0;
	unsigned int config_base_addr = 0x0;
	unsigned int dbi_base_size = 0x0;

#ifdef CONFIG_PCIE0_SEL
	if (controller == pcie_controller_0) {
		config_base_addr = DBI_BASE_ADDR_0;
		dbi_base_addr = PCIE0_MEMIO_BASE;
		dbi_base_size = pcie0_cfg_space_size;
	}
#endif

#ifdef CONFIG_PCIE1_SEL
	if (controller == pcie_controller_1) {
		config_base_addr = DBI_BASE_ADDR_1;
		dbi_base_addr = PCIE1_MEMIO_BASE;
		dbi_base_size = pcie1_cfg_space_size;
	}
#endif
	if ((controller != pcie_controller_0)
		       && (controller != pcie_controller_1)) {
		pcie_error("Invalid pcie controller number!");
		return -1;
	}

	pcie_info[pcie_controllers_nr].controller = controller;

	printk(KERN_INFO "controller%u, config base:0x%x, mem size:0x%x\n",
			controller, config_base_addr, dbi_base_size);

	pcie_info[pcie_controllers_nr].conf_base_addr =
		(unsigned int)ioremap_nocache(config_base_addr, 0x1000);
	if (!pcie_info[pcie_controllers_nr].conf_base_addr) {
		pcie_error("Cannot map host's dbi space"\
				" for controller%d!", controller);
		return -1;
	}

	pcie_info[pcie_controllers_nr].base_addr =
		(unsigned int)ioremap_nocache(dbi_base_addr,
				dbi_base_size);
	if (!pcie_info[pcie_controllers_nr].base_addr) {
		pcie_error("Cannot map config space base"\
				" for controller%d!", controller);
		return -1;
	}

	return 1;
}

static void __init pcie_preinit(void)
{
	/*
	 * Setup resource.
	 */
#ifdef CONFIG_PCIE0_SEL
	pcie0_mem_space.end = PCIE0_BASE_ADDR_PHYS + pcie0_mem_space_size - 1;
	pcie0_io_space.start = pcie0_mem_space.end + 1;
	pcie0_io_space.end = PCIE0_BASE_ADDR_PHYS + PCIE_BASE_ADDR_SIZE - 1;
#endif

#ifdef CONFIG_PCIE1_SEL
	pcie1_mem_space.end = PCIE1_BASE_ADDR_PHYS + pcie1_mem_space_size - 1;
	pcie1_io_space.start = pcie1_mem_space.end + 1;
	pcie1_io_space.end = PCIE1_BASE_ADDR_PHYS + PCIE_BASE_ADDR_SIZE - 1;
#endif
}

static int request_pcie_res(int controller, struct pci_sys_data *sys)
{
	int ret;
	struct resource *controller_mem_space = NULL;
	struct resource *controller_io_space = NULL;

#ifdef CONFIG_PCIE0_SEL
	if (controller == pcie_controller_0) {
		controller_mem_space = &pcie0_mem_space;
		controller_io_space = &pcie0_io_space;
	}
#endif

#ifdef CONFIG_PCIE1_SEL
	if (controller == pcie_controller_1) {
		controller_mem_space = &pcie1_mem_space;
		controller_io_space = &pcie1_io_space;
	}
#endif

	if ((controller != pcie_controller_0)
			&& (controller != pcie_controller_1)) {
		pcie_error("Invalid pcie controller number!");
		return -1;
	}

	ret = request_resource(&ioport_resource, controller_io_space);
	if (ret) {
		pcie_error("Cannot request io resource for pcie%d,"\
				"pcie0_io_space.start=0x%x,end:0x%x",
					controller - 1,
					controller_mem_space->start,
					controller_io_space->end);
		return ret;
	}

	ret = request_resource(&iomem_resource, controller_mem_space);
	if (ret) {
		pcie_error("Cannot request mem resource for pcie0");
				release_resource(controller_io_space);
		return ret;
	}

	pci_add_resource_offset(&sys->resources,
			controller_io_space, sys->io_offset);
	pci_add_resource_offset(&sys->resources,
			controller_mem_space, sys->mem_offset);


	return 0;
}

static int __init pcie_setup(int nr, struct pci_sys_data *sys)
{
	struct pcie_info *info;
	int ret;

	if (nr >= pcie_controllers_nr)
		return 0;

	info = &pcie_info[nr];
	info->root_bus_nr = sys->busnr;
	sys->mem_offset = 0;

	/*
	 * Requeset resources for the right controller.
	 */
	ret = request_pcie_res(info->controller, sys);
	if (ret)
		return ret;

	config_controller_iatu_table(info->controller, sys);

	return 1;
}

static struct pcie_info * __init bus_to_info(int busnr)
{
	int i = pcie_controllers_nr - 1;

	for (; i >= 0; i--) {
		if (pcie_info[i].controller != pcie_controller_none
				&& pcie_info[i].root_bus_nr <= busnr
				&& pcie_info[i].root_bus_nr != -1)
			return &pcie_info[i];
	}

	return NULL;
}


static int __init pcie_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	struct pcie_info *info = bus_to_info(dev->bus->number);


	if (!info) {
		pcie_error("Cannot find corresponding controller "\
				"for appointed device!");
		BUG();
		return -1;
	}

#ifdef CONFIG_PCIE0_SEL
	if (info->controller == pcie_controller_0) {
		switch (pin) {
		case PCIE_INTA_PIN:
			return PCIE0_IRQ_INTA;
		case PCIE_INTB_PIN:
			return PCIE0_IRQ_INTB;
		case PCIE_INTC_PIN:
			return PCIE0_IRQ_INTC;
		case PCIE_INTD_PIN:
			return PCIE0_IRQ_INTD;
		default:
			pcie_error("Unkown pin for mapping irq!");
			return -1;
		}
	}
#endif

#ifdef CONFIG_PCIE1_SEL
	if (info->controller == pcie_controller_1) {
		switch (pin) {
		case PCIE_INTA_PIN:
			return PCIE1_IRQ_INTA;
		case PCIE_INTB_PIN:
			return PCIE1_IRQ_INTB;
		case PCIE_INTC_PIN:
			return PCIE1_IRQ_INTC;
		case PCIE_INTD_PIN:
			return PCIE1_IRQ_INTD;
		default:
			pcie_error("Unkown pin for mapping irq!");
			return -1;
		}
	}
#endif

	pcie_error("Invalide PCIE controller[%d]!", info->controller);
	BUG();
	return -1;
}

#define PCIE_CFG_BUS(busnr)	((busnr & 0xff) << 20)
#define PCIE_CFG_DEV(devfn)	((devfn & 0xff) << 12)
#define PCIE_CFG_REG(reg)	(reg & 0xffc)	/*set dword align*/

static inline unsigned int to_pcie_address(struct pci_bus *bus,
		unsigned int devfn, int where)
{
	struct pcie_info *info = bus_to_info(bus->number);
	unsigned int address = 0;
	if (!info) {
		pcie_error("Cannot find corresponding controller "\
				"for appointed device!");
		BUG();
	}

	address = info->base_addr | PCIE_CFG_BUS(bus->number)
		| PCIE_CFG_DEV(devfn) | PCIE_CFG_REG(where);


	return address;
}

static int pcie_read_from_device(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 *value)
{
	struct pcie_info *info = bus_to_info(bus->number);
	unsigned int val;
	unsigned int addr;
	int i = 0;

	for (i = 0; i < 1000; i++) {
		if (pcie_check_link_status(info))
			break;
		udelay(1000);
	}
	if (i >= 1000) {
		pcie_debug(PCIE_DEBUG_LEVEL_MODULE, "pcie%d not link up!",
				info->controller == pcie_controller_0 ? 0 : 1);
		printk(KERN_ERR "pcie%d not link up!\n",
				info->controller == pcie_controller_0 ? 0 : 1);
		return -1;
	}

	addr = to_pcie_address(bus, devfn, where);

	val = readl(addr);

	i = 0;
	while (i < 2000) {
		__asm__ __volatile__("nop\n");
		i++;
	}

	if (pcie_errorvalue == 1) {
		pcie_errorvalue = 0;
		val = 0xffffffff;
	}

	if (size == 1)
		*value = ((val >> ((where & 0x3) << 3)) & 0xff);
	else if (size == 2)
		*value = ((val >> ((where & 0x3) << 3)) & 0xffff);
	else if (size == 4)
		*value = val;
	else{
		pcie_error("Unkown size(%d) for read ops", size);
		BUG();
	}

	return PCIBIOS_SUCCESSFUL;
}

static int pcie_read_from_dbi(struct pcie_info *info, unsigned int devfn,
		int where, int size, u32 *value)
{
	unsigned int val;

	/*
	 * For host-side config space read, ignore device func nr.
	 */
	if (devfn > 0)
		return -EIO;

	val = (u32)readl((void *)(info->conf_base_addr + (where & (~0x3))));

	if (1 == size)
		*value = (val >> ((where & 0x3) << 3)) & 0xff;
	else if (2 == size)
		*value = (val >> ((where & 0x3) << 3)) & 0xffff;
	else if (4 == size)
		*value = val;
	else {
		pcie_error("Unkown size for config read operation!");
		BUG();
	}

	return PCIBIOS_SUCCESSFUL;
}

static int pcie_read_conf(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 *value)
{
	struct pcie_info *info = bus_to_info(bus->number);


	if (unlikely(!info)) {
		pcie_error("Cannot find corresponding controller "\
				"for appointed device!");
		BUG();
	}

	if (bus->number == info->root_bus_nr)
		return  pcie_read_from_dbi(info, devfn, where, size, value);
	else
		return  pcie_read_from_device(bus, devfn, where, size, value);
}

static int pcie_write_to_device(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 value)
{
	struct pcie_info *info = bus_to_info(bus->number);
	unsigned int addr;
	unsigned int org;
	unsigned long flag;

	if (!pcie_check_link_status(info)) {
		pcie_debug(PCIE_DEBUG_LEVEL_MODULE, "PCIE%d not link up!",
				info->controller == pcie_controller_0 ? 0 : 1);
		printk(KERN_ERR "PCIE%d not link up!\n",
				info->controller == pcie_controller_0 ? 0 : 1);
		return -1;
	}

	spin_lock_irqsave(&cw_lock, flag);

	pcie_read_from_device(bus, devfn, where, 4, &org);

	addr = to_pcie_address(bus, devfn, where);

	if (size == 1) {
		org &= (~(0xff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	} else if (size == 2) {
		org &= (~(0xffff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	} else if (size == 4) {
		org = value;
	} else {
		pcie_error("Unkown size(%d) for read ops", size);
		BUG();
	}
	writel(org, addr);

	spin_unlock_irqrestore(&cw_lock, flag);

	return PCIBIOS_SUCCESSFUL;

}

static int pcie_write_to_dbi(struct pcie_info *info, unsigned int devfn,
		int where, int size, u32 value)
{
	unsigned long flag;
	unsigned int org;

	spin_lock_irqsave(&cw_lock, flag);

	if (pcie_read_from_dbi(info, devfn, where, 4, &org)) {
		pcie_error("Cannot read from dbi! 0x%x:0x%x:0x%x!",
				0, devfn, where);
		spin_unlock_irqrestore(&cw_lock, flag);
		return -EIO;
	}
	if (size == 1) {
		org &= (~(0xff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	} else if (size == 2) {
		org &= (~(0xffff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	} else if (size == 4) {
		org = value;
	} else {
		pcie_error("Unkown size(%d) for read ops", size);
		BUG();
	}
	writel(org, info->conf_base_addr + (where & (~0x3)));

	spin_unlock_irqrestore(&cw_lock, flag);

	return PCIBIOS_SUCCESSFUL;
}

static int pcie_write_conf(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 value)
{
	struct pcie_info *info = bus_to_info(bus->number);

	if (unlikely(!info)) {
		pcie_error("Cannot find corresponding controller "\
				"for appointed device!");
		BUG();
	}

	if (bus->number == info->root_bus_nr)
		return pcie_write_to_dbi(info, devfn, where, size, value);
	else
		return pcie_write_to_device(bus, devfn, where, size, value);
}

static struct pci_ops pcie_ops = {
	.read = pcie_read_conf,
	.write = pcie_write_conf,
};

static struct pci_bus *__init pcie_scan_bus(int nr, struct pci_sys_data *sys)
{
	struct pci_bus *bus;

	if (nr < pcie_controllers_nr) {
		bus = pci_scan_root_bus(NULL, sys->busnr, &pcie_ops, sys,
				&sys->resources);
	} else {
		bus = NULL;
		pcie_error("Unkown controller nr :0x%x!", nr);
		BUG();
	}

	return bus;
}


static struct hw_pci hipcie __initdata = {
	.nr_controllers = 1,
	.preinit    = pcie_preinit,
	.swizzle    = pci_std_swizzle,
	.setup      = pcie_setup,
	.scan       = pcie_scan_bus,
	.map_irq    = pcie_map_irq,
};


static int __init pcie_init(void)
{
#ifdef CONFIG_PCIE0_SEL
	pcie0_sel = CONFIG_PCIE0_SEL;
	pcie0_mem_space_size = CONFIG_PCIE0_DEVICES_MEM_SIZE;
	pcie0_cfg_space_size = CONFIG_PCIE0_DEVICES_CONFIG_SIZE;
#endif

#ifdef CONFIG_PCIE1_SEL
	pcie1_sel = CONFIG_PCIE1_SEL;
	pcie1_mem_space_size = CONFIG_PCIE1_DEVICES_MEM_SIZE;
	pcie1_cfg_space_size = CONFIG_PCIE1_DEVICES_CONFIG_SIZE;
#endif

	if ((pcie0_sel != pcie_controller_selected
			&& pcie0_sel != pcie_controller_unselected)
			|| pcie0_mem_space_size >= 0x10000000
			|| pcie0_cfg_space_size > 0x10000000
			|| pcie0_cfg_space_size < 0) {
		pcie_error("Invalid parameter pcie0_sel(%d), "\
				"pcie0_mem_space_size(0x%x), "\
				"CONFIG_PCIE0_DEVICES_CONFIG_SIZE(0x%x)",
				pcie0_sel, pcie0_mem_space_size,
				pcie0_cfg_space_size);
		return -EINVAL;
	}

	if ((pcie1_sel != pcie_controller_selected
			&& pcie1_sel != pcie_controller_unselected)
			|| pcie1_mem_space_size >= 0x10000000
			|| pcie1_cfg_space_size > 0x10000000
			|| pcie1_cfg_space_size < 0) {
		pcie_error("Invalid parameter pcie1_sel(%d), "\
				"pcie1_mem_space_size(0x%x), "\
				"CONFIG_PCIE1_DEVICES_CONFIG_SIZE(0x%x)",
				pcie1_sel, pcie1_mem_space_size,
				pcie1_cfg_space_size);
		return -EINVAL;
	}

	if ((pcie0_sel == pcie_controller_unselected)
			&& (pcie1_sel == pcie_controller_unselected)) {
		pcie_error("No PCIe Controller enabled!\n");
		return -EINVAL;
	} else if (pcie0_sel == pcie_controller_unselected) {
		printk(KERN_INFO "No PCIe controller 0!\n");
	} else if (pcie1_sel == pcie_controller_unselected) {
		printk(KERN_INFO "No PCIe controller 1!\n");
	}

	pcie_sys_base_virt = ioremap_nocache(PCIE_SYS_BASE_PHYS, 0x1000);
	if (!pcie_sys_base_virt) {
		pcie_error("Cannot map system controller register base!");
		return -EIO;
	}

	pcie_info[0].root_bus_nr = -1;
	pcie_info[1].root_bus_nr = -1;

	/*
	* scene: PCIe host(RC)<--->SWITCH<--->PCIe device(*)
	*                            |
	*                            |------->NULL SLOT
	* PCIe generate a "DataAbort" to ARM, when scaning NULL SLOT.
	* Register "ARM VECTOR HOOK" capture and fix it.
	*/
	hook_fault_code(22, pcie_fault, 7, BUS_OBJERR,
		"external abort on non-linefetch");

	if (pcie0_sel == pcie_controller_selected) {
		if (init_pcie_controller_info(pcie_controller_0) == -1) {
			pcie_error("Init PCIE controller0 failed!");
			goto Err_controller;
		}

		pcie_controllers_nr++;
		hisi_pcie_mode_sel |= PCIE0_MODE_SEL;
	}

	if (pcie1_sel == pcie_controller_selected) {
		if (init_pcie_controller_info(pcie_controller_1) == -1) {
			pcie_error("Init PCIE controller1 failed!");
			goto Err_controller;
		}
		pcie_controllers_nr++;
		hisi_pcie_mode_sel |= PCIE1_MODE_SEL;
	}

	/* none of the two controllers works at rc mode or is enabled,
	 * we do nothing
	 */

	if (!pcie_controllers_nr) {
		pcie_error("None of the two pcie controllers is enabled!");
		goto Err_controller;
	}

	if (pcie_sys_init(hisi_pcie_mode_sel))
		goto Err_controller;

	/* reset how many controllers to enable. */
	hipcie.nr_controllers = pcie_controllers_nr;
	pci_common_init(&hipcie);
	return 0;

Err_controller:
	if (pcie_info[1].conf_base_addr)
		iounmap((void *)pcie_info[1].conf_base_addr);

	if (pcie_info[1].base_addr)
		iounmap((void *)pcie_info[1].base_addr);

	if (pcie_info[0].conf_base_addr)
		iounmap((void *)pcie_info[0].conf_base_addr);

	if (pcie_info[0].base_addr)
		iounmap((void *)pcie_info[0].base_addr);

	if (pcie_sys_base_virt)
		iounmap(pcie_sys_base_virt);

	return -EIO;

}

#include <linux/platform_device.h>
#include <linux/pm.h>

int __devinit hisi_pcie_plat_driver_probe(struct platform_device *pdev)
{
	return 0;
}
int __devexit hisi_pcie_plat_driver_remove(struct platform_device *pdev)
{
	return 0;
}

#define PCIE_RC_DRV_NAME "hisi pcie root complex"
static struct platform_driver hisi_pcie_platform_driver = {
	.probe          = hisi_pcie_plat_driver_probe,
	.remove         = hisi_pcie_plat_driver_remove,
#ifdef CONFIG_PM
	.suspend	= hisi_pcie_plat_driver_suspend,
	.resume		= hisi_pcie_plat_driver_resume,
#endif
	.driver         = {
		.owner  = THIS_MODULE,
		.name   = PCIE_RC_DRV_NAME,
		.bus    = &platform_bus_type,
		.pm     = HISI_PCIE_PM_OPS
	},
};

static void hisi_pcie_platform_device_release(struct device *dev)
{
}

static struct resource hisi_pcie_resources[] = {
	[0] = {
		.start  = DBI_BASE_ADDR_0,
		.end    = DBI_BASE_ADDR_0 + 0x1000 - 1,
		.flags  = IORESOURCE_MEM,
	}
};

static u64 hipcie_dmamask = DMA_BIT_MASK(32);

static struct platform_device hisi_pcie_platform_device = {
	.name = PCIE_RC_DRV_NAME,
	.id   = 0,
	.dev = {
		.platform_data  = NULL,
		.dma_mask = &hipcie_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.release = hisi_pcie_platform_device_release,
	},
	.num_resources = ARRAY_SIZE(hisi_pcie_resources),
	.resource = hisi_pcie_resources,
};

static int __init hisi_pcie_init(void)
{
	int ret;
	ret = platform_device_register(&hisi_pcie_platform_device);
	if (ret)
		goto err_device;

	ret = platform_driver_register(&hisi_pcie_platform_driver);
	if (ret)
		goto err_driver;

	if (pcie_init()) {
		pcie_error("pcie sys init failed!");
		goto err_init;
	}

	return 0;
err_init:
	platform_driver_unregister(&hisi_pcie_platform_driver);
err_driver:
	platform_device_unregister(&hisi_pcie_platform_device);
err_device:
	return -1;
}

static void __exit hisi_pcie_exit(void)
{
	platform_device_unregister(&hisi_pcie_platform_device);
	platform_driver_unregister(&hisi_pcie_platform_driver);
}

subsys_initcall(hisi_pcie_init);
module_exit(hisi_pcie_exit);

MODULE_DESCRIPTION("Hisilicon PCI-Express Root Complex driver");
MODULE_LICENSE("GPL");
