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

#define PCIE_DBG_REG		1
#define PCIE_DBG_FUNC		2
#define PCIE_DBG_MODULE		3

#define PCIE_DEBUG_LEVEL PCIE_DBG_MODULE

/*#define PCIE_DEBUG*/
#ifdef PCIE_DEBUG
#define pcie_debug(level, str, arg...)\
	do {\
		if ((level) <= PCIE_DEBUG_LEVEL) {\
			pr_debug("%s->%d," str "\n", \
				__func__, __LINE__, ##arg);\
		} \
	} while (0)
#else
#define pcie_debug(level, str, arg...)
#endif

#define pcie_assert(con)\
	do {\
		if (!(con)) {\
			pr_err("%s->%d,assert fail!\n", \
				__func__, __LINE__);\
		} \
	} while (0)

#define pcie_error(str, arg...)\
	pr_err("%s->%d" str "\n", __func__, __LINE__, ##arg)


#define __256MB__	0x10000000
#define __128MB__	0x8000000
#define __4KB__		0x1000
#define __8KB__		0x2000

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

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
#include "pcie_hi3519.c"
#elif defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
#include "pcie_hi3559.c"
#elif defined(CONFIG_ARCH_HI3516AV200)
#include "pcie_hi3516av200.c"
#else
#error You must have defined either CONFIG_ARCH_HI3519 or CONFIG_ARCH_HI3559 or CONFIG_ARCH_HI3556 or CONFIG_ARCH_HI3516AV200...
#endif

/*
 * PCIE memory size bootargs: pcie0_mem_size=0xa00000;pcie1_mem_size=0xa00000
 */
static int __init pcie0_mem_size_parser(char *str)
{
	unsigned int size;

	if (kstrtoul(str, 16, (long *)&size) < 0)
		return 0;

	/* if size >= 256MB, set default 256MB */
	if (size >= 0x10000000)
		size = 0x10000000;
	pcie0_mem_space_size = size;

	return 1;
}
__setup("pcie0_mem_size=", pcie0_mem_size_parser);

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

static void __init pcie_preinit(void)
{
	pcie_debug(PCIE_DBG_FUNC, "!");
	__arch_pcie_preinit();
}

static int request_pcie_res(int controller, struct pci_sys_data *sys)
{
	int ret;
	struct resource *mem = NULL;
	struct resource *io = NULL;

	__arch_get_pcie_res(controller, &mem, &io);

	ret = request_resource(&ioport_resource, io);
	if (ret) {
		pcie_error(
		"request io resource failed,io->start=0x%x,io->end=0x%x",
		io->start, io->end);
		return ret;
	}

	ret = request_resource(&iomem_resource, mem);
	if (ret) {
		pcie_error(
		"request mem resource failed,mem->start=0x%x,mem->end=0x%x",
		mem->start, mem->end);

		release_resource(io);
		return ret;
	}

	pci_add_resource_offset(&sys->resources, io, sys->io_offset);
	pci_add_resource_offset(&sys->resources, mem, sys->mem_offset);

	return 0;
}

static int __init pcie_setup(int nr, struct pci_sys_data *sys)
{
	struct pcie_info *info;
	int ret;

	pcie_debug(PCIE_DBG_FUNC, "nr %d, sys->busnr %d",
			nr, sys->busnr);
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

	__arch_config_iatu_tbl(info, sys);

	return 1;
}

static struct pcie_info *bus_to_info(int busnr)
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
	
	if (unlikely(!info)) {
		pcie_error(
		"%s:Cannot find corresponding controller for appointed device!", __func__);
		BUG();
	}

	pcie_debug(PCIE_DBG_FUNC,
		"dev->bus->number %d, slot %d, pin %d",
		dev->bus->number, slot, pin);
	return __arch_get_int_irq(info, pin);

}

#define PCIE_CFG_BUS(busnr)	((busnr & 0xff) << 20)
#define PCIE_CFG_DEV(devfn)	((devfn & 0xff) << 12)
#define PCIE_CFG_REG(reg)	(reg & 0xffc)	/*set dword align*/

static inline unsigned int to_pcie_address(struct pci_bus *bus,
		unsigned int devfn, int where)
{
	struct pcie_info *info = bus_to_info(bus->number);
	unsigned int address = 0;

	if (unlikely(!info)) {
		pcie_error(
		"%s:Cannot find corresponding controller for appointed device!", __func__);
		BUG();
	}

	address = info->base_addr + (PCIE_CFG_BUS(bus->number)
		| PCIE_CFG_DEV(devfn) | PCIE_CFG_REG(where));


	return address;
}

static inline int is_pcie_link_up(struct pcie_info *info)
{
	int i;

	 for (i = 0; i < 10000; i++) {
		if (__arch_check_pcie_link(info))
			break;
		udelay(100);
	}

	return (i < 10000);
}

static int pcie_read_from_device(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 *value)
{
	struct pcie_info *info = bus_to_info(bus->number);
	unsigned int val;
	void __iomem *addr;
	int i = 0;

	if (unlikely(!info)) {
		pcie_error(
		"%s:Cannot find corresponding controller for appointed device!", __func__);
		BUG();
	}
	
	if (!is_pcie_link_up(info)) {
		pcie_debug(PCIE_DBG_MODULE, "pcie %d not link up!",
				info->controller);
		return -1;
	}

	addr = (void __iomem *)to_pcie_address(bus, devfn, where);

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
		pcie_error("Unknown size(%d) for read ops", size);
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
		pcie_error("Unknown size for config read operation!");
		BUG();
	}

	return PCIBIOS_SUCCESSFUL;
}

static int pcie_read_conf(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 *value)
{
	struct pcie_info *info = bus_to_info(bus->number);
	int ret;

	if (unlikely(!info)) {
		pcie_error(
		"%s:Cannot find corresponding controller for appointed device!", __func__);
		BUG();
	}

	if (bus->number == info->root_bus_nr)
		ret =  pcie_read_from_dbi(info, devfn, where, size, value);
	else
		ret =  pcie_read_from_device(bus, devfn, where, size, value);

	pcie_debug(PCIE_DBG_REG,
		"bus %d, devfn %d, where 0x%x, size 0x%x, value 0x%x",
		bus->number & 0xff, devfn, where, size, *value);

	return ret;
}

static int pcie_write_to_device(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 value)
{
	struct pcie_info *info = bus_to_info(bus->number);
	void __iomem *addr;
	unsigned int org;
	unsigned long flag;

	if (unlikely(!info)) {
		pcie_error(
		"%s:Cannot find corresponding controller for appointed device!", __func__);
		BUG();
	}

	if (!is_pcie_link_up(info)) {
		pcie_debug(PCIE_DBG_MODULE, "pcie %d not link up!",
				info->controller);
		return -1;
	}

	spin_lock_irqsave(&cw_lock, flag);

	pcie_read_from_device(bus, devfn, where, 4, &org);

	addr = (void __iomem *)to_pcie_address(bus, devfn, where);

	if (size == 1) {
		org &= (~(0xff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	} else if (size == 2) {
		org &= (~(0xffff << ((where & 0x3) << 3)));
		org |= (value << ((where & 0x3) << 3));
	} else if (size == 4) {
		org = value;
	} else {
		pcie_error("Unknown size(%d) for read ops", size);
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
		pcie_error("Unknown size(%d) for read ops", size);
		BUG();
	}
	writel(org, ((void __iomem *)info->conf_base_addr + (where & (~0x3))));

	spin_unlock_irqrestore(&cw_lock, flag);

	return PCIBIOS_SUCCESSFUL;
}

static int pcie_write_conf(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 value)
{
	struct pcie_info *info = bus_to_info(bus->number);

	pcie_debug(PCIE_DBG_REG,
		"bus %d, devfn %d, where 0x%x, size 0x%x, value 0x%x",
		bus->number & 0xff, devfn, where, size, value);

	if (unlikely(!info)) {
		pcie_error(
		"%s:Cannot find corresponding controller for appointed device!", __func__);
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

void pci_set_max_rd_req_size(const struct pci_bus *bus)
{
	struct pci_dev *dev;
	struct pci_bus *child;
	int pos;
	unsigned short dev_contrl_reg_val = 0;
	unsigned int max_rd_req_size = 0;

	list_for_each_entry(dev, &bus->devices, bus_list) {

		/* set device max read requset size*/
		pos = pci_find_capability(dev, PCI_CAP_ID_EXP);
		if (pos) {
			pci_read_config_word(dev, pos + PCI_EXP_DEVCTL,
					&dev_contrl_reg_val);
			max_rd_req_size = (dev_contrl_reg_val >> 12) & 0x7;
			if (max_rd_req_size > 0x0) {
				dev_contrl_reg_val &= ~(max_rd_req_size << 12);
				pci_write_config_word(dev, pos + PCI_EXP_DEVCTL,
						dev_contrl_reg_val);
			}

		}
	}

	list_for_each_entry(dev, &bus->devices, bus_list) {
		BUG_ON(!dev->is_added);
		child = dev->subordinate;
		if (child)
			pci_set_max_rd_req_size(child);
	}
}

static struct pci_bus *__init pcie_scan_bus(int nr, struct pci_sys_data *sys)
{
	struct pci_bus *bus;

	pcie_debug(PCIE_DBG_FUNC,
		"nr %d, sys->busnr %d, sys->mem_offset 0x%llx, sys->io_offset 0x%lx",
		nr, sys->busnr, sys->mem_offset, sys->io_offset);
	if (nr < pcie_controllers_nr) {
		bus = pci_scan_root_bus(NULL, sys->busnr, &pcie_ops, sys,
				&sys->resources);
	} else {
		bus = NULL;
		pcie_error("Unknown controller nr :0x%x!", nr);
		BUG();
	}

#ifdef CONFIG_LIMIT_MAX_RD_REQ_SIZE
	if (bus)
		pci_set_max_rd_req_size(bus);
#endif

	return bus;
}


static struct hw_pci hipcie __initdata = {
	.nr_controllers = 1,
	.preinit    = pcie_preinit,
	.swizzle    = pci_common_swizzle,
	.setup      = pcie_setup,
	.scan       = pcie_scan_bus,
	.map_irq    = pcie_map_irq,
};


static int __init pcie_init(void)
{
	pcie_info[0].root_bus_nr = -1;
	pcie_info[1].root_bus_nr = -1;

	/*
	 * Scene: PCIe host(RC)<--->SWITCH<--->PCIe device(*)
	 *                            |
	 *                            |------->NULL SLOT
	 * PCIe will generate a DataAbort to ARM, when scanning NULL SLOT.
	 * Register hook to capture this exception and handle it.
	 */
	hook_fault_code(22, pcie_fault, 7, BUS_OBJERR,
		"external abort on non-linefetch");

	if (__arch_pcie_info_setup(pcie_info, &pcie_controllers_nr))
		return -EIO;

	if (__arch_pcie_sys_init(pcie_info))
		goto pcie_init_err;
	hipcie.nr_controllers = pcie_controllers_nr;
	pr_err("Number of PCIe controllers: %d\n",
		hipcie.nr_controllers);

	pci_common_init(&hipcie);

	return 0;
pcie_init_err:
	__arch_pcie_info_release(pcie_info);

	return -EIO;
}

static void __exit pcie_uinit(void)
{
	__arch_pcie_info_release(pcie_info);
}

#include <linux/platform_device.h>
#include <linux/pm.h>

int  hisi_pcie_plat_driver_probe(struct platform_device *pdev)
{
	return 0;
}
int  hisi_pcie_plat_driver_remove(struct platform_device *pdev)
{
	return 0;
}

#ifdef CONFIG_PM
int hisi_pcie_plat_driver_suspend(struct device *dev)
{
	__arch_pcie_sys_exit();
	return 0;
}

int hisi_pcie_plat_driver_resume(struct device *dev)
{
	return __arch_pcie_sys_init(pcie_info);
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

#define PCIE_RC_DRV_NAME "hisi pcie root complex"

static struct platform_driver hisi_pcie_platform_driver = {
	.probe          = hisi_pcie_plat_driver_probe,
	.remove         = hisi_pcie_plat_driver_remove,
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
		.start  = PCIE_DBI_BASE,
		.end    = PCIE_DBI_BASE + __8KB__ - 1,
		.flags  = IORESOURCE_REG,
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
	pcie_uinit();
	platform_device_unregister(&hisi_pcie_platform_device);
	platform_driver_unregister(&hisi_pcie_platform_driver);
}

subsys_initcall(hisi_pcie_init);
module_exit(hisi_pcie_exit);

MODULE_DESCRIPTION("Hisilicon PCI-Express Root Complex driver");
MODULE_LICENSE("GPL");
