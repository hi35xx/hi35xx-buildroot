#ifndef __HI3536C_CHIP_REGS_H__
#define __HI3536C_CHIP_REGS_H__

/* -------------------------------------------------------------------- */
/* SRAM Base Address Register */
#define SRAM_BASE_ADDRESS		0x04010000

/* -------------------------------------------------------------------- */
/*CORTTX-A7 MPCORE MEMORY REGION*/
/* -------------------------------------------------------------------- */
#define REG_BASE_A7_PERI		0x10300000
#define A7_PERI_BASE			REG_BASE_A7_PERI

#define REG_A7_PERI_SCU			0x0000

/* -------------------------------------------------------------------- */
/* Clock and Reset Generator REG */
/* -------------------------------------------------------------------- */
#define REG_BASE_CRG			0x12040000
#define CRG_REG_BASE			REG_BASE_CRG

#define REG_CRG32				0x0080

/* A7 soft reset request register offset */
#define REG_A7_SRST_CRG			REG_CRG32
#define DBG1_SRST_REQ			BIT(4)
#define CORE1_SRST_REQ			BIT(2)

/* Ethernet CRG register offset */
#define REG_ETH_CRG				0x014C
#define REG_ETH_MAC_IF			0x008C

#define UART_CKSEL_MASK			(0x3 << 18)
#define UART_CKSEL_24M			(0x2 << 18)

#define REG_BASE_CUR_UART		REG_BASE_UART0

/*********************************************************************/
#define A7_AXI_SCALE_REG		IO_ADDRESS(REG_BASE_CRG + 0x50)

/* -------------------------------------------------------------------- */
#define DDR_MEM_BASE			0x80000000
#define CFG_TIMER_PER 			(4)	/* AXI:APB is 4:1 */
/* -------------------------------------------------------------------- */
#define get_bus_clk() ({ \
	unsigned long tmp_reg, busclk = 0;\
	tmp_reg = readl((void *)A7_AXI_SCALE_REG);\
	tmp_reg = (tmp_reg >> 2) & 0x3;\
	if (0x3 == tmp_reg) {\
		busclk = 300000000;\
	} else if (0x1 == tmp_reg) {\
		busclk = 200000000;\
	} \
	busclk;\
})

/* hisilicon satav200 register */
#define HI_SATA_PORT_FIFOTH		0x44
#define HI_SATA_PORT_PHYCTL1	0x48
#define HI_SATA_PORT_PHYCTL		0x74

#define HI_SATA_PHY_CTL0		0xA0
#define HI_SATA_PHY_CTL1		0xA4
#define HI_SATA_PHY_CTL2		0xB0
#define HI_SATA_PHY_RST_BACK_MASK    0xAC

#define HI_SATA_FIFOTH_VALUE	0xFEED9F24

#define HI_SATA_BIGENDINE       (1 << 3)

#define HI_SATA_PHY_MODE_1_5G	0
#define HI_SATA_PHY_MODE_3G		1
#define HI_SATA_PHY_MODE_6G		2

#define HI_SATA_PHY_1_5G		0x0e180000
#define HI_SATA_PHY_3G			0x0e390000
#define HI_SATA_PHY_6G			0x0e5a0000

#define HI_SATA_PHY_SG_1_5G		0x50438
#define HI_SATA_PHY_SG_3G		0x50438
#define HI_SATA_PHY_SG_6G		0x50438

#define	HI_SATA_MISC_CTRL		IO_ADDRESS(0x12120000)
#define HI_SATA_MISC_SATA_PHY0	(HI_SATA_MISC_CTRL + 0x58)
#define HI_SATA_MISC_SATA_PHY1	(HI_SATA_MISC_CTRL + 0x64)

#endif /* End of __HI_CHIP_REGS_H__ */

