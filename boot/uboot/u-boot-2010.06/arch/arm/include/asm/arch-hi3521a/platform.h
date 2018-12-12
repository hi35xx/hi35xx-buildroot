#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define RAM_START_ADRS			0x04010500
#define FMC_REG_BASE			0x10000000

#define TIMER0_REG_BASE			0x12000000
#define TIMER1_REG_BASE			0x12000020
#define TIMER2_REG_BASE			0x12010000
#define TIMER3_REG_BASE			0x12010020
#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4
#define REG_TIMER_CONTROL		0x8

#define CRG_REG_BASE			0x12040000
#define REG_CRG13			0x34
#define FMC_CRG29			0x74
#define PERI_CRG33			0x84
#define UART_CKSEL_MASK			(0x3 << 18)
#define UART_CKSEL_24M			(0x2 << 18)
#define REG_ETH_CRG			0x78
#define REG_ETH_MAC_IF			0x8C

#define SYS_CTRL_REG_BASE		0x12050000
#define REG_BASE_SCTL			SYS_CTRL_REG_BASE
#define REG_SC_CTRL			0
#define REG_SC_SYSRES			0x4
#define REG_SC_GEN0			0x0138
#define REG_SC_GEN1			0x013c
#define REG_SC_GEN2			0x0140
#define REG_SC_GEN3			0x0144
#define REG_SC_GEN4			0x0148

#define WDG_REG_BASE			0x12070000
#define UART0_REG_BASE			0x12080000
#define UART1_REG_BASE			0x12090000
#define UART2_REG_BASE			0x120A0000
#define RTC_REG_BASE			0x120B0000
#define IO_CONFIG_REG_BASE		0x120F0000
#define DDRC0_REG_BASE			0x12110000
#define MISC_REG_BASE			0x12120000
#define IR_REG_BASE			0x12140000
#define GPIO0_REG_BASE			0x12150000
#define GPIO1_REG_BASE			0x12160000
#define GPIO2_REG_BASE			0x12170000
#define GPIO3_REG_BASE			0x12180000
#define GPIO4_REG_BASE			0x12190000
#define GPIO5_REG_BASE			0x121A0000
#define GPIO6_REG_BASE			0x121B0000
#define GPIO7_REG_BASE			0x121C0000
#define GPIO8_REG_BASE			0x121D0000
#define GPIO9_REG_BASE			0x121E0000
#define GPIO10_REG_BASE			0x121F0000
#define GPIO11_REG_BASE			0x12200000
#define GPIO12_REG_BASE			0x12210000
#define GPIO13_REG_BASE			0x12220000
#define FMC_MEM_BASE			0x14000000
#define DDR_MEM_BASE			0x80000000

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define REG_SYSSTAT			0x008C
#define BOOT_FROM_NAND	2
#define BOOT_FROM_DDR	1
#define BOOT_FROM_SPI	0

#define SYSSTAT_BOOTROM_SEL_IN			(1 << 31)
#define SYSSTAT_SPI_DEVICE_TYPE			(1 << 8)
#define SYSSTAT_MULTIMODE			(1 << 7)

#define SPI_DEVICE_TYPE_SHIFT			8
#define GET_SPI_DEVICE_TYPE(_reg)	(((_reg) & SYSSTAT_SPI_DEVICE_TYPE) \
						>> SPI_DEVICE_TYPE_SHIFT)

#define MULTIMODE_SHIFT				7
/* bit[8]=0; bit[7]:SPI nor address mode; bit[7]=(0:3-Byte | 1:4-Byte) */
#define SPI_NOR_ADDRESS_MODE_MASK		(0x1 << MULTIMODE_SHIFT)
/* bit[8]=1; bit[7]: SPI nand I/O widthe; bit[7]=(0: 1-I/O | 1: 4-I/O */
#define SPI_NAND_IO_WIDTHE_MASK			(0x1 << MULTIMODE_SHIFT)

#define GET_SPI_NOR_ADDR_MODE(_reg) (((_reg) >> MULTIMODE_SHIFT) \
				& SPI_NOR_ADDRESS_MODE_MASK)

/********** Communication Register and flag used by bootrom *************/
#define REG_START_FLAG         (SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define CONFIG_START_MAGIC     (0x444f574e)

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/

#define STACK_TRAINING			0x04014000
#define _HI3521A_V100                  (0x003521a100LL)
#define _HI3521A_MASK                  (0xFFFFFFFFFFLL)
/******************************************************************************/

/*-----------------------------------------------------------------------
 * PERI_CRG29 FMC REG
 * ----------------------------------------------------------------------*/
#define FMC_CRG29			0x74
#define FMC_CRG29_CLK_SEL(_clk)		(((_clk) & 0x3) << 2)
#define FMC_CRG29_CLK_EN		(1 << 1)
#define FMC_CRG29_SOFT_RST_REQ		(1 << 0)

#define FMC_CLK_SEL(_clk)		(((_clk) & 0x3) << 2)
#define FMC_CLK_SEL_MASK		(0x3 << 2)
#define GET_FMC_CLK_TYPE(_reg)		(((_reg) >> 2) & 0x3)

#define CLK_24M				0x0
#define CLK_83M				0x1
#define CLK_150M			0x2

#define FMC_CLK_SEL_24M		FMC_CRG29_CLK_SEL(CLK_24M)
#define FMC_CLK_SEL_83M		FMC_CRG29_CLK_SEL(CLK_83M)
#define FMC_CLK_SEL_150M	FMC_CRG29_CLK_SEL(CLK_150M)

#endif /* End of __HI_CHIP_REGS_H__ */

