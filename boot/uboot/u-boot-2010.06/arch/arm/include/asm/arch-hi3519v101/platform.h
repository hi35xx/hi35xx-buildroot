#ifndef __HI_CHIP_REGS_H__
#define __HI_CHIP_REGS_H__

/* -------------------------------------------------------------------- */
#define RAM_START_ADRS			0x04010500

/* -------------------------------------------------------------------- */
#define FMC_REG_BASE			0x10000000

/* -------------------------------------------------------------------- */
#define GSF_REG_BASE			0x100a0000

/* -------------------------------------------------------------------- */
#define HIUSB_OHCI_BASE			0x10110000

/* -------------------------------------------------------------------- */
#define TIMER0_REG_BASE			0x12000000
#define TIMER1_REG_BASE			0x12000020
#define TIMER2_REG_BASE			0x12001000
#define TIMER3_REG_BASE			0x12001020
#define TIMER4_REG_BASE			0x12002000
#define TIMER5_REG_BASE			0x12002020

#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4
#define REG_TIMER_CONTROL		0x8

/* -------------------------------------------------------------------- */
/* Clock and Reset Generator REG */
/* -------------------------------------------------------------------- */
#define CRG_REG_BASE			0x12010000

#define PERI_CRG57			0xe4
#define UART_CKSEL_24M			(1 << 25)

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

/* Ethernet CRG register offset */
#define REG_ETH_CRG			0xcc
#define REG_ETH_MAC_IF			0xec

/* Cci register definations */
#define CCI_PORT_CTRL_0			0x1ff01000	/* cci for gsf/vdma */
#define CCI_PORT_CTRL_1			0x1ff02000	/* cci for A7 */
#define CCI_PORT_CTRL_2			0x1ff03000	/* cci for A17*/
#define CCI_CTRL_STATUS			0x1ff0000c
#define CCI_ENABLE_REQ			0x3


/* -------------------------------------------------------------------- */
/* System Control REG */
/* -------------------------------------------------------------------- */
#define SYS_CTRL_REG_BASE		0x12020000
#define REG_BASE_SCTL			SYS_CTRL_REG_BASE

/* System Control register offset */
#define REG_SC_CTRL			0x0000
#define SC_CTRL_TIMER0_CLK_SEL(_clk)	(((_clk) & 0x1) << 16)
#define TIMER0_CLK_SEL_MASK		(0x1 << 16)
#define TIMER_CLK_3M			0
#define TIMER_CLK_BUS			1
#define SC_CTRL_REMAP_CLEAR		(0x1 << 8)

/* System soft reset register offset */
#define REG_SC_SYSRES			0x0004

/* System Status register offset */
#define REG_SYSSTAT			0x008c
/* bit[9]=0: PCI-E Master mode; bit[9]=1: PCI-E Slave mode */
#define GET_PCIE_SLAVE_MODE(_reg)	(((_reg) >> 9) & 0x1)
/* if bit[3]=0 SPI nor flash
 * bit[7]=0: 3-Byte address mode; bit[7]=1: 4-Byte address mode */
#define GET_SPI_NOR_ADDR_MODE(_reg)	(((_reg) >> 7) & 0x1)
#define GET_SYS_BOOT_MODE(_reg)		(((_reg) >> 4) & 0x3)
#define BOOT_FROM_SPI			0
#define BOOT_FROM_NAND			1
#define BOOT_FROM_EMMC			2
#define BOOT_FROM_DDR			3
/* bit[3]=0; SPI nor flash; bit[3]=1: SPI nand flash */
#define GET_SPI_DEVICE_TYPE(_reg)	(((_reg) >> 3) & 0x1)

#define REG_SC_GEN20		0x0090
#define REG_SC_GEN0			0x0138
#define REG_SC_GEN1			0x013c
#define REG_SC_GEN2			0x0140
#define REG_SC_GEN3			0x0144
#define REG_SC_GEN4			0x0148
#define REG_SC_GEN5			0x014c
#define REG_SC_GEN_SD_ADDR		0x0130

/* -------------------------------------------------------------------- */
#define UART4_REG_BASE			0x12104000
#define UART3_REG_BASE			0x12103000
#define UART2_REG_BASE			0x12102000
#define UART1_REG_BASE			0x12101000
#define UART0_REG_BASE			0x12100000

#define UART0_RXD_PULL_MASK		(0x1 << 9)

/* -------------------------------------------------------------------- */
#define WDG_REG_BASE			0x12080000
#define RTC_REG_BASE			0x12090000
#define IO_CONFIG_REG_BASE		0x12040000
#define DDRC0_REG_BASE			0x12060000

#define PAD_CTRL_REG35			0x088c

#define IR_REG_BASE			0x120f0000

/*------------------------------------------------
 *GPIO reg base address
 *------------------------------------------------*/
#define GPIO14_REG_BASE			0x1214e000
#define GPIO13_REG_BASE			0x1214d000
#define GPIO12_REG_BASE			0x1214c000
#define GPIO11_REG_BASE			0x1214b000
#define GPIO10_REG_BASE			0x1214a000
#define GPIO9_REG_BASE			0x12149000
#define GPIO8_REG_BASE			0x12148000
#define GPIO7_REG_BASE			0x12147000
#define GPIO6_REG_BASE			0x12146000
#define GPIO5_REG_BASE			0x12145000
#define GPIO4_REG_BASE			0x12144000
#define GPIO3_REG_BASE			0x12143000
#define GPIO2_REG_BASE			0x12142000
#define GPIO1_REG_BASE			0x12141000
#define GPIO0_REG_BASE			0x12140000

/* -------------------------------------------------------------------- */
#define FMC_MEM_BASE			0x14000000
#define DDR_MEM_BASE			0x80000000

/********** Communication Register and flag used by bootrom *************/
#define REG_START_FLAG			(SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define CONFIG_START_MAGIC		(0x444f574e)

/* --------------------------------------------------------------------
 * DDR Training
 * -------------------------------------------------------------------- */
#define STACK_TRAINING			0x04017ff0
#define _HI3519_V101			(0x003519101LL)
#define _HI3519V101_MASK		(0xFFFFFFFFFFLL)
#define REG_SC_DDRT0			0x0090
#define NORMAL_BOOTMODE_OFFSET      9
#define NORMAL_BOOTMODE_MASK        3
/* --------------------------------------------------------------------
 * PCIE base address
 * -------------------------------------------------------------------- */
#define PCIE_CFG_BASE			0x20000000
#define PCIE_REG_BASE			0x12160000

/*--------------------------------------------------------------------------
 *PCIE slave start up
 *--------------------------------------------------------------------------*/
#define PCIE_SLV_STACK			0x7000
#define PCIE_SLV_DDR_INIT_FLG		0x8080
#define HI3519V101_SYSBOOT9			0x154
#define HI3519V101_SYSBOOT10		0x158

/*--------------------------------------------------------------------------
* Cci register definations
 *--------------------------------------------------------------------------*/
#define CCI_PORT_CTRL_0			0x1ff01000
#define CCI_PORT_CTRL_1			0x1ff02000
#define CCI_CTRL_STATUS			0x1ff0000c
#define CCI_ENABLE_REQ			0x3

/*--------------------------------------------------------------------------
 * for fast boot
 *-------------------------------------------------------------------------*/
#define GZIP_REG_BASE			0x110c0000
#define REG_SNAPSHOT_IMAGE_MAGIC	0x120200a4
#define REG_SNAPSHOT_IMAGE_START	0x120200a8
#define REG_SNAPSHOT_IMAGE_SIZE		0x120200ac

/*-----------------------------------------------------------------------
 * EMMC / SD
 * ----------------------------------------------------------------------*/
/* SDIO0 REG */
#define SDIO0_BASE_REG			0x100C0000

/* SDIO1 REG */
#define SDIO1_BASE_REG			0x100D0000

/* EMMC REG*/
#define EMMC_BASE_REG			0x100E0000

#define NF_BOOTBW_MASK                  (1 << 7)
#define REG_BASE_PERI_CTRL              REG_BASE_SCTL
#define REG_BASE_IO_CONFIG              IO_CONFIG_REG_BASE

#define MMC_IOMUX_START_ADDR            0xF8
#define MMC_IOMUX_END_ADDR              0x13C
#define MMC_IOMUX_CTRL_MASK             (1<<0 | 1<<1)
#define MMC_IOMUX_CTRL                  (1<<1)

/* for fast boot */
#define GZIP_REG_BASE			0x110c0000
#define REG_SNAPSHOT_IMAGE_MAGIC	0x120200a4
#define REG_SNAPSHOT_IMAGE_START	0x120200a8
#define REG_SNAPSHOT_IMAGE_SIZE		0x120200ac
#define HW_DEC_INTR		(73)
#endif /* End of __HI_CHIP_REGS_H__ */
