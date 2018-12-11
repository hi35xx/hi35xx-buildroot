#ifndef __HI3516AV200_CHIP_REGS_H__
#define __HI3516AV200_CHIP_REGS_H__

/* SRAM Base Address Register */
#define SRAM_BASE_ADDRESS                               0x4010000

#define REG_BASE_TIMER01                                0x12000000
#define REG_BASE_TIMER23                                0x12001000

/* -------------------------------------------------------------------- */
/* Clock and Reset Generator REG */
/* -------------------------------------------------------------------- */
#define CRG_REG_BASE			0x12010000
#define REG_BASE_CRG			CRG_REG_BASE

#define REG_CRG48			0x00c0

/* FMC CRG register offset */
#define REG_FMC_CRG			REG_CRG48
#define FMC_CLK_SEL(_clk)		(((_clk) & 0x7) << 2)
#define FMC_CLK_SEL_MASK		(0x7 << 2)
#define GET_FMC_CLK_TYPE(_reg)		(((_reg) >> 2) & 0x7)
/* SDR/DDR clock */
#define FMC_CLK_24M			0
#define FMC_CLK_75M			1
#define FMC_CLK_125M			2
#define FMC_CLK_150M			3
#define FMC_CLK_200M			4
#define FMC_CLK_250M			5
/* Only DDR clock */
#define FMC_CLK_300M			6
#define FMC_CLK_400M			7
#define FMC_CLK_ENABLE			(0x1 << 1)
#define FMC_SOFT_RST_REQ		(0x1 << 0)

#define REG_BASE_UART0                                  0x12100000
#define REG_BASE_UART1                                  0x12101000
#define REG_BASE_UART2                                  0x12102000
#define REG_BASE_UART3                                  0x12103000
#define REG_BASE_UART4                                  0x12104000
#define REG_BASE_CUR_UART                               REG_BASE_UART0

#define PMC_BASE					0x120a0000

#define CCI_BASE					0x1ff00000
#define CCI_PORT0_BASE				        0x1ff01000
#define CCI_PORT1_BASE				        0x1ff02000


#define REG_BASE_A7_PERI                               0x10300000

/*CORTTX-A7 MPCORE MEMORY REGION*/
#define REG_A7_PERI_SCU                                 0x0000
#define REG_A7_PERI_GIC_CPU                             0x2000
#define REG_A7_PERI_GLOBAL_TIMER                        0x0200
#define REG_A7_PERI_PRI_TIMER_WDT                       0x0600
#define REG_A7_PERI_GIC_DIST                            0x1000
/* -------------------------------------------------------------------- */
/* System Control REG */
/* -------------------------------------------------------------------- */
#define SYS_CTRL_BASE                                    0x12020000

/* System Control register offset */
#define REG_SC_CTRL			0x0000

/* System soft reset register offset */
#define REG_SC_SYSRES			0x0004

/* System Status register offset */
#define REG_SC_STAT			0x008c

/* if bit[5:4]=00b: boot form SPI && bit[3]=0: SPI nor flash
 * bit[7]=0: 3-Byte address mode; bit[7]=1: 4-Byte address mode */
#define GET_SPI_NOR_ADDR_MODE(_reg)	(((_reg) >> 7) & 0x1)
#define GET_SYS_BOOT_MODE(_reg)		(((_reg) >> 4) & 0x3)
#define BOOT_MODE_MASK				((0x3) << 4)
#define BOOT_FROM_SPI			0
#define BOOT_FROM_NAND			1
#define BOOT_FROM_EMMC			2
#define BOOT_FROM_DDR			3
/* bit[3]=0; SPI nor flash; bit[3]=1: SPI nand flash */
#define GET_SPI_DEVICE_TYPE(_reg)	(((_reg) >> 3) & 0x1)

#define REG_SC_XTALCTRL                                 0x0010
#define REG_SC_APLLCTRL                                 0x0014
#define REG_SC_APLLFREQCTRL0                            0x0018
#define REG_SC_APLLFREQCTRL1                            0x001C
#define REG_SC_VPLL0FREQCTRL0                           0x0020
#define REG_SC_VPLL0FREQCTRL1                           0x0024
#define REG_SC_VPLL1FREQCTRL0                           0x0028
#define REG_SC_VPLL1FREQCTRL1                           0x002C
#define REG_SC_EPLLFREQCTRL0                            0x0030
#define REG_SC_EPLLFREQCTRL1                            0x0034
#define REG_SC_QPLLFREQCTRL0                            0x0038
#define REG_SC_QPLLFREQCTRL1                            0x003C
#define REG_SC_LOW_POWER_CTRL                           0x0040
#define REG_SC_IO_REUSE_SEL                             0x0044
#define REG_SC_SRST_REQ_CTRL                            0x0048
#define REG_SC_CA_RST_CTRL                              0x004C
#define REG_SC_WDG_RST_CTRL                             0x0050
#define REG_SC_DDRC_DFI_RST_CTRL                        0x0054
#define REG_SC_PLLLOCK_STAT                             0x0070
#define REG_SC_GEN0                                     0x0080
#define REG_SC_GEN1                                     0x0084
#define REG_SC_GEN2                                     0x0088
#define REG_SC_GEN3                                     0x008C
#define REG_SC_GEN4                                     0x0090
#define REG_SC_GEN5                                     0x0094
#define REG_SC_GEN6                                     0x0098
#define REG_SC_GEN7                                     0x009C
#define REG_SC_GEN8                                     0x00A0
#define REG_SC_GEN9                                     0x00A4
#define REG_SC_GEN10                                    0x00A8
#define REG_SC_GEN11                                    0x00AC
#define REG_SC_GEN12                                    0x00B0
#define REG_SC_GEN13                                    0x00B4
#define REG_SC_GEN14                                    0x00B8
#define REG_SC_GEN15                                    0x00BC
#define REG_SC_GEN16                                    0x00C0
#define REG_SC_GEN17                                    0x00C4
#define REG_SC_GEN18                                    0x00C8
#define REG_SC_GEN19                                    0x00CC
#define REG_SC_GEN20                                    0x00D0
#define REG_SC_GEN21                                    0x00D4
#define REG_SC_GEN22                                    0x00D8
#define REG_SC_GEN23                                    0x00DC
#define REG_SC_GEN24                                    0x00E0
#define REG_SC_GEN25                                    0x00E4
#define REG_SC_GEN26                                    0x00E8
#define REG_SC_GEN27                                    0x00EC
#define REG_SC_GEN28                                    0x00F0
#define REG_SC_GEN29                                    0x00F4
#define REG_SC_GEN30                                    0x00F8
#define REG_SC_GEN31                                    0x00FC
#define REG_SC_LOCKEN                                   0x020C
#define REG_SC_SYSID0                                   0x0EE0
#define REG_SC_SYSID1                                   0x0EE4
#define REG_SC_SYSID2                                   0x0EE8
#define REG_SC_SYSID3                                   0x0EEC

/*#define REG_BASE_WDG0                                   0xF8A2C000  */

#define REG_PERI_CRG94                                  0x178
#define REG_PERI_CRG10                                  0x0028
#define REG_PERI_CRG98					0x0188

#define CFG_GIC_CPU_BASE    (IO_ADDRESS(REG_BASE_A7_PERI)\
				+ REG_A7_PERI_GIC_CPU)
#define CFG_GIC_DIST_BASE   (IO_ADDRESS(REG_BASE_A7_PERI)\
				+ REG_A7_PERI_GIC_DIST)


/*********************************************************************/
/*
 * 0x1-> init item1
 * 0x2-> init item2
 * 0x3->init item1 & item2
 */
#define INIT_REG_ITEM1               1
#define INIT_REG_ITEM2               2
#define INIT_REG_ITEM1_ITEM2         (INIT_REG_ITEM1 | INIT_REG_ITEM2)


#endif /* End of __HI_CHIP_REGS_H__ */
