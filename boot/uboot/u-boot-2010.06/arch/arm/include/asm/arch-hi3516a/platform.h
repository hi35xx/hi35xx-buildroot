
#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define DDR_MEM_BASE			0x80000000
#define SFC_MEM_BASE			0x58000000
#define SPI_NAND_MEM_BASE		0x54000000
#define NAND_MEM_BASE			0x50000000
#define ARM_DEBUG_BASE			0x20400000
#define ARM_REG_BASE			0x20300000

#define GPIO15_REG_BASE			0x20260000
#define GPIO14_REG_BASE			0x20220000
#define GPIO13_REG_BASE			0x20210000
#define GPIO12_REG_BASE			0x20200000
#define GPIO11_REG_BASE			0x201F0000
#define GPIO10_REG_BASE			0x201E0000
#define GPIO9_REG_BASE			0x201D0000
#define GPIO8_REG_BASE			0x201C0000
#define GPIO7_REG_BASE			0x201B0000
#define GPIO6_REG_BASE			0x201A0000
#define GPIO5_REG_BASE			0x20190000
#define GPIO4_REG_BASE			0x20180000
#define GPIO3_REG_BASE			0x20170000
#define GPIO2_REG_BASE			0x20160000
#define GPIO1_REG_BASE			0x20150000
#define GPIO0_REG_BASE			0x20140000

#define DDRC0_REG_BASE			0x20110000

#define IO_CONFIG_REG_BASE		0x200F0000

#define UART3_REG_BASE			0x20230000
#define UART2_REG_BASE			0x200A0000
#define UART1_REG_BASE			0x20090000
#define UART0_REG_BASE			0x20080000
#define IR_REG_BASE			0x20070000
#define RTC_REG_BASE			0x20060000
#define SYS_CTRL_REG_BASE		0x20050000
#define REG_BASE_SCTL                   SYS_CTRL_REG_BASE

#define REG_SC_CTRL			0
#define REG_SC_SYSRES			0x4
#define PERIPHCTRL24			0x94
#define REG_SC_GEN0			0x0138
#define REG_SC_GEN1			0x013c
#define REG_SC_GEN2			0x0140
#define REG_SC_GEN3			0x0144
#define REG_SC_GEN4			0x0148

#define WDG_REG_BASE			0x20040000

#define CRG_REG_BASE			0x20030000
#define PERI_CRG57			0xe4
#define UART_CKSEL_APB			(1 << 19)
#define REG_ETH_CRG			0xCC
#define REG_ETH_MAC_IF			0xEC
#define EFUSE_CTRL_CRG			0xD8

#define TIMER3_REG_BASE			0x20010020
#define TIMER2_REG_BASE			0x20010000
#define TIMER1_REG_BASE			0x20000020
#define TIMER0_REG_BASE			0x20000000
#define REG_TIMER_CONTROL		0x8
#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4

#define REG_BASE_SF			0x10090000
#define USB20HOST1_EHCI_REG_BASE	0x100B0000
#define USB20HOST2_EHCI_REG_BASE	0x100A0000

#define SFC_REG_BASE			0x10010000
#define SFC_ADDR_MODE_REG		0x8c
#define NANDC_REG_BASE			0x10000000
#define SPI_NANDC_REG_BASE		0x10040000

#define RAM_START_ADRS			0x04010500

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define REG_SYSSTAT			0x008C
#define BOOT_FROM_NAND	2
#define BOOT_FROM_DDR	1
#define BOOT_FROM_SPI	0

/* DDR FREQ_SCALE CONFIG */
#define REG_CRG12_OFFSET (CRG_REG_BASE + 0x30)
#define REG_DDR_FRE_SCALE_VALUE  0 /*0-(2:1); 1-(1:1)*/
/********** Communication Register and flag used by bootrom *************/
#define REG_START_FLAG         (SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define CONFIG_START_MAGIC     (0x444f574e)

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/

#define STACK_TRAINING			0x04010c00
#define _HI3516A_V100			(0x003516a100LL)
#define _HI3516A_MASK			(0xFFFFFFFFFFLL)
#define REG_SC_DDRT0			0x0090
#define NORMAL_BOOTMODE_OFFSET      9
#define NORMAL_BOOTMODE_MASK        3

/* Cortec-A7 */
#define CONFIG_MAX_IRQ			96

#define REG_A7_PERI_GIC_CPU     0x2000
#define REG_A7_PERI_GIC_DIST    0x1000

#define CFG_GIC_CPU_BASE		(ARM_REG_BASE + REG_A7_PERI_GIC_CPU)
#define CFG_GIC_DIST_BASE		(ARM_REG_BASE + REG_A7_PERI_GIC_DIST)
/******************************************************************************/
#endif /* End of __HI_CHIP_REGS_H__ */
