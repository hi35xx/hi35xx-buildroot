#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define DDR_MEM_BASE			0x80000000
#define FMC_MEM_BASE			0x58000000
#define DDR_DDRT_REG_BASE		0x206d0000

#define GPIO8_REG_BASE			0x201C0000
#define GPIO7_REG_BASE			0x201B0000
#define GPIO6_REG_BASE			0x201A0000
#define GPIO5_REG_BASE			0x20190000
#define GPIO4_REG_BASE			0x20180000
#define GPIO3_REG_BASE			0x20170000
#define GPIO2_REG_BASE			0x20160000
#define GPIO1_REG_BASE			0x20150000
#define GPIO0_REG_BASE			0x20140000
#define MISC_REG_BASE			0x20120000
#define DDRC0_REG_BASE			0x20110000
#define DDR_DMC0_REG_BASE	0x20111000
#define DDR_PHY0_REG_BASE	0x20118000

#define IO_CONFIG_REG_BASE		0x200F0000
#define UART2_REG_BASE			0x200A0000
#define UART1_REG_BASE			0x20090000
#define UART0_REG_BASE			0x20080000
#define IR_REG_BASE				0x20070000
#define RTC_REG_BASE			0x20060000
#define SYS_CTRL_REG_BASE		0x20050000
#define REG_BASE_SCTL			SYS_CTRL_REG_BASE
#define REG_SC_CTRL			0
#define TIME0_CLK_APB		(0x1 << 16)
#define REG_SC_SYSRES		0x4
#define REG_SYSSTAT			0x008C
#define REG_SC_GEN0			0x0138
#define REG_SC_GEN1			0x013c
#define REG_SC_GEN2			0x0140
#define REG_SC_GEN3			0x0144
#define REG_SC_GEN4			0x0148

#define WDG_REG_BASE			0x20040000
#define CRG_REG_BASE			0x20030000
#define REG_CRG12				0x30
#define REG_CRG46			0xb8
#define FMC_CRG48			0xc0
#define FMC_CRG65			0x104
/*-----------------------------------------------------------------------
 * PERI_CRG48 FMC REG
 * ----------------------------------------------------------------------*/
#define FMC_CLK_SEL_MASK		(0x3 << 2)
#define FMC_CRG48_CLK_SEL(_clk)		(((_clk) & 0x3) << 2)
#define GET_FMC_CLK_TYPE(_reg)          (((_reg) >> 2) & 0x3)
#define FMC_CLK_SEL_24M			FMC_CRG48_CLK_SEL(0x0)
#define FMC_CLK_SEL_148_5M		FMC_CRG48_CLK_SEL(0x1)
#define FMC_CLK_SEL_125M		FMC_CRG48_CLK_SEL(0x2)
#define FMC_CLK_SEL_198M		FMC_CRG48_CLK_SEL(0x3)
#define FMC_CRG48_CLK_EN		(1 << 1)
#define FMC_CRG48_SOFT_RST_REQ		(1 << 0)

#define GET_FMC_MUXIO_CLK(_clk)		(((_clk) >> 5) & 0x1)
#define FMC_MUXIO_CLK_SEL(_clk)		((_clk) & (~(0x1 << 5)))

#define REG_CRG49			0xc4
#define SDIO0_CLK_SEL_MASK		(0x3 << 10)
#define SDIO0_CLK_SEL_99M		(0x0 << 10)
#define SDIO0_CLK_SEL_74_25M	(0x1 << 10)
#define SDIO0_CLK_SEL_49_5M		(0x2 << 10)
#define SDIO0_CLK_SEL_148_5M	(0x3 << 10)
#define SDIO0_SRST_REQ			(0x1 << 8)
#define SDIO0_CKEN				(0x1 << 9)
#define SDIO1_CLK_SEL_MASK		(0x3 << 2)
#define SDIO1_CLK_SEL_49_5M		(0x0 << 2)
#define SDIO1_SRST_REQ			(0x1 << 0)
#define SDIO1_CKEN				(0x1 << 1)

#define PERI_CRG57			0xe4
#define UART_CKSEL_MASK		(0x3 << 19)
#define UART_CKSEL_APB		(0x0 << 19)
#define UART_CKSEL_24M		(0x1 << 19)
#define UART_CKSEL_6M		(0x2 << 19)
#define REG_ETH_CRG			0xEC

#define TIMER3_REG_BASE			0x20010020
#define TIMER2_REG_BASE			0x20010000
#define TIMER1_REG_BASE			0x20000020
#define TIMER0_REG_BASE			0x20000000
#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4
#define REG_TIMER_CONTROL		0x8

#define REG_BASE_SF				0x10090000
#define FMC_REG_BASE			0x10010000
#define RAM_START_ADRS			0x04010500

/*-----------------------------------------------------------------------
 * REG_SYSSTAT: boot mode
 * ----------------------------------------------------------------------*/
#define SYSSTAT_BOOTROM_SEL_IN		(1 << 31)
#define SYSSTAT_BOOT_MODE			(1 << 4)
#define	BOOT_MODE_SHIFT				4
#define SYSSTAT_FMC_DEVICE_TYPE		(1 << 3)
#define	FMC_DEVICE_MODE_SHIFT		3
#define SYSSTAT_MULTIMODE			(1 << 7)

#define GET_SPI_DEVICE_TYPE(_reg)	(((_reg) & SYSSTAT_FMC_DEVICE_TYPE) \
						>> FMC_DEVICE_MODE_SHIFT)

#define MULTIMODE_SHIFT				7
/* bit[3]=0; bit[7]:SPI nor address mode; bit[7]=(0:3-Byte | 1:4-Byte) */
#define SPI_NOR_ADDRESS_MODE_MASK		(0x1 << MULTIMODE_SHIFT)
/* bit[3]=1; bit[7]: SPI nand I/O widthe; bit[7]=(0: 1-I/O | 1: 4-I/O */
#define SPI_NAND_IO_WIDTHE_MASK			(0x1 << MULTIMODE_SHIFT)

#define GET_SPI_NOR_ADDR_MODE(_reg) (((_reg) & SPI_NOR_ADDRESS_MODE_MASK)\
		>> MULTIMODE_SHIFT)

#define BOOT_FROM_FMC	0
#define BOOT_FROM_EMMC	1

/********** Communication Register and flag used by bootrom *************/
#define REG_START_FLAG			(SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define CONFIG_START_MAGIC		(0x444f574e)

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/
#define STACK_TRAINING			0x04014000

#define _HI3518EV200			(0x003518e200LL)
#define _HI3518EV200_MASK		(0xFFFFFFFFFFLL)

/*-----------------------------------------------------------------------
 * EMMC / SD
 *----------------------------------------------------------------------*/
#define SDIO0_BASE_REG				0x10020000
#define	SDIO1_BASE_REG				0x10030000

#define REG_START_MODE                  0x0000
#define NF_BOOTBW_MASK                  (1<<7)

#define MMC_IOMUX_START_ADDR		0x98
#define MMC_IOMUX_END_ADDR		0xD0
#define MMC_IOMUX_CTRL_MASK		(1<<0)
#define MMC_IOMUX_CTRL			1

/* for fast boot*/
#define GZIP_REG_BASE			0x206B0000
#define REG_SNAPSHOT_IMAGE_MAGIC	0x200500a4
#define REG_SNAPSHOT_IMAGE_START	0x200500a8
#define REG_SNAPSHOT_IMAGE_SIZE		0x200500ac
#define HW_DEC_INTR		(56)
#endif /* End of __HI_CHIP_REGS_H__ */

