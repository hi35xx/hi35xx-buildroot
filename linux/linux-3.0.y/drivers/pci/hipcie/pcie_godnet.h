#ifndef __HISI_PCIE_H__
#define __HISI_PCIE_H__

#define PCIE_SYS_BASE_PHYS	(0x20050000)
#define PCIE0_BASE_ADDR_PHYS	(0x30000000)
#define PCIE0_MEMIO_BASE	(0x40000000)
#define PCIE_BASE_ADDR_SIZE	(0x10000000)
#define DBI_BASE_ADDR_0                (0x20800000)

/* for PCIe controller 1 wucy */
#define PCIE1_BASE_ADDR_PHYS	(0x60000000)
#define PCIE1_MEMIO_BASE	(0x70000000)
#define	DBI_BASE_ADDR_1	               (0x20810000)
/* wucy added for 2controller */

#define PCIE_PHY_VALUE0		(0x05605001)
#define PCIE_PHY_VALUE1                (0x20050006)

#define PERI_CRG_BASE		(0x20030000)
#define PERI_CRG37		(0x7c)
#define pcie0_clk_slot_bit	(3)
#define pcie0_clk_ctl_bit	(1)
#define pcie0_rst_ctl_bit	(0)

#define PERI_PCIE0		(0xB4)   /* (0xB0) */
#define PERI_PCIE1		(0xB8)   /* (0xB4) */
#define PERI_PCIE2		(0x90)
#define pcie0_slv_device_type	(28)

#define PERI_PCIE79		(0x0170)
#define PERI_PCIE80		(0x0174)
#define PERI_PCIE70		(0x014c)
#define pcie1_slv_device_type	(28)

#define PERI_PCIE9		(0xac)
#define pcie0_app_ltssm_enable        (11)

#define PERI_PCIE77		(0x168)
#define pcie1_app_ltssm_enable	      (11)


#define PERI_PCIE_STAT0                (0xd0)
#define pciex_link_up		(10)
#define PERI_PCIE_STAT1                (0xd4)

#define IRQ_BASE		(32)


#define PCIE0_IRQ_INTA		(41 + IRQ_BASE)
#define PCIE0_IRQ_INTB		(42 + IRQ_BASE)
#define PCIE0_IRQ_INTC		(43 + IRQ_BASE)
#define PCIE0_IRQ_INTD		(44 + IRQ_BASE)

#define PCIE1_IRQ_INTA		(49 + IRQ_BASE)
#define PCIE1_IRQ_INTB		(50 + IRQ_BASE)
#define PCIE1_IRQ_INTC		(51 + IRQ_BASE)
#define PCIE1_IRQ_INTD		(52 + IRQ_BASE)

#define PCIE_INTA_PIN		(1)
#define PCIE_INTB_PIN		(2)
#define PCIE_INTC_PIN		(3)
#define PCIE_INTD_PIN		(4)

#define PCIE_RESET_REG_OFFSET    0x78
#define PCIE_RESET_VALUE         0x33
#define PCIE_RESET_CANCEL_VALUE  0x22

#define PCIE_DEBUG_LEVEL_REG	(1)
#define PCIE_DEBUG_LEVEL_FUNC	(2)
#define PCIE_DEBUG_LEVEL_MODULE        (3)

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

#endif


