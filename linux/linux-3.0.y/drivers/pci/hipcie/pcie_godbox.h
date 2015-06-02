#ifndef __HISI_PCIE_H__
#define __HISI_PCIE_H__

#define PCIE_SYS_BASE_PHYS	(0x10200000)

#define PCIE0_BASE_ADDR_PHYS	(0x61000000)
#define PCIE1_BASE_ADDR_PHYS	(0x62000000)
#define PCIE_BASE_ADDR_SIZE	(0x10000000)

#define DBI_BASE_ADDR_0		(0x60050000)
#define DBI_BASE_ADDR_1		(0x60060000)

#define PCIE0_MEMIO_BASE	(0x70000000)
#define PCIE1_MEMIO_BASE	(0x0)

#define PCIE_PHY_VALUE0         (0x05605001)
#define PCIE_PHY_VALUE1         (0x20050006)

#define PERI_CRG37			(0x00D4)
#define PERI_CRG37_PCIE0_SRST_REQ	(1U << 0)
#define PERI_CRG37_PCIE0_RST_REQ	(1U << 1)
#define PERI_CRG37_X2X_PCIE0_SRST_REQ	(1U << 2)
#define PERI_CRG37_X2X_PCIES_SRST_REQ	(1U << 6)
#define PERI_CRG37_PCIE0_CKEN		(1U << 8)
#define PERI_CRG37_PCIE_PHY_CKEN	(1U << 10)
#define PERI_CRG37_X2X_PCIES_CKEN	(1U << 11)
#define PERI_CRG37_PCIE_CLK_SEL		(1U << 16)
#define PERI_CRG37_PCIE_SLOT0_SRST_REQ	(1U << 24)
#define PERI_CRG37_PCIE_SLOT_CKEN	(1U << 26)

#define PERI_PCIE0		(0x140)
#define PERI_PCIE1		(0x144)
#define PERI_PCIE2		(0x148)
#define pcie0_slv_device_type	(28)

#define PERI_PCIE9		(0x164)
#define pcie0_app_ltssm_enable	(11)

#define PERI_PCIE_STAT0		(0x1a0)
#define pciex_link_up		(10)
#define PERI_PCIE_STAT1		(0x1a4)
#define IRQ_BASE		(32)
#define PCIE0_IRQ_INTA		(80 + IRQ_BASE)
#define PCIE0_IRQ_INTB		(81 + IRQ_BASE)
#define PCIE0_IRQ_INTC		(82 + IRQ_BASE)
#define PCIE0_IRQ_INTD		(83 + IRQ_BASE)

#define PCIE1_IRQ_INTA		(-1)
#define PCIE1_IRQ_INTB		(-1)
#define PCIE1_IRQ_INTC		(-1)
#define PCIE1_IRQ_INTD		(-1)

#define PCIE_INTA_PIN		(1)
#define PCIE_INTB_PIN		(2)
#define PCIE_INTC_PIN		(3)
#define PCIE_INTD_PIN		(4)

#define PCIE_DEBUG_LEVEL_REG	(1)
#define PCIE_DEBUG_LEVEL_FUNC	(2)
#define PCIE_DEBUG_LEVEL_MODULE	(3)

#define PCIE_DEBUG_LEVEL PCIE_DEBUG_LEVEL_MODULE

#define PCIE_DEBUG
#ifdef PCIE_DEBUG
#define pcie_debug(level, str, arg...)\
	do {\
		if ((level) <= PCIE_DEBUG_LEVEL) {\
			printk(KERN_DEBUG "%s->%d," str "\n",\
					__func__, __LINE__, ##arg);\
		} \
	} while (0)
#else
#define pcie_debug(level, str, arg...)
#endif

#define pcie_assert(con)\
	do {\
		if (!(con)) {\
			printk(KERN_ERR "%s->%d,assert fail!\n",\
					__func__, __LINE__);\
		} \
	} while (0)

#define pcie_error(str, arg...)\
	do {\
		printk(KERN_ERR "%s->%d" str "\n", __func__, __LINE__, ##arg);\
	} while (0)

#endif
