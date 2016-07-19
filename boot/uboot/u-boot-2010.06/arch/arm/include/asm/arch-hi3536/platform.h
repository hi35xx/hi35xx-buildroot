
#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define DDR_MEM_BASE			0x40000000
#define PCIE_MEM_BASE			0x30000000
#define PCIE_CFG_BASE			0x20000000
#define PCIE_REG_BASE			0x1F000000
#define NAND_MEM_BASE			0x16000000
#define SPI_NAND_MEM_BASE		0x14000000
#define SFC_MEM_BASE			0x15000000
#define RTC_REG_BASE			0x12310000
#define GPIO15_REG_BASE			0x12240000
#define GPIO14_REG_BASE			0x12230000
#define GPIO13_REG_BASE			0x12220000
#define GPIO12_REG_BASE			0x12210000
#define GPIO11_REG_BASE			0x12200000
#define GPIO10_REG_BASE			0x121F0000
#define GPIO9_REG_BASE			0x121E0000
#define GPIO8_REG_BASE			0x121D0000
#define GPIO7_REG_BASE			0x121C0000
#define GPIO6_REG_BASE			0x121B0000
#define GPIO5_REG_BASE			0x121A0000
#define GPIO4_REG_BASE			0x12190000
#define GPIO3_REG_BASE			0x12180000
#define GPIO2_REG_BASE			0x12170000
#define GPIO1_REG_BASE			0x12160000
#define GPIO0_REG_BASE			0x12150000
#define IR_REG_BASE			0x12140000
#define MIPS_REG_BASE			0x12120000
#define IO_CONFIG_REG_BASE		0x120F0000
#define UART3_REG_BASE			0x120B0000
#define UART2_REG_BASE			0x120A0000
#define UART1_REG_BASE			0x12090000
#define UART0_REG_BASE			0x12080000
#define WDG_REG_BASE			0x12070000
#define SYS_CTRL_REG_BASE		0x12050000
#define CRG_REG_BASE			0x12040000
#define TIMER1_REG_BASE			0x12010000
#define TIMER0_REG_BASE			0x12000000

#define REG_BASE_SCTL                   SYS_CTRL_REG_BASE

#define REG_SC_CTRL			0
#define REG_SC_SYSRES			0x4
#define PERIPHCTRL24			0x94
#define REG_SC_GEN0			0x0138
#define REG_SC_GEN1			0x013c
#define REG_SC_GEN2			0x0140
#define REG_SC_GEN3			0x0144
#define REG_SC_GEN4			0x0148

#define EFUSE_REG_BASE			0x12300000

#define EFUSE_DATA_SEL			0x28

#define REG_CRG12_OFFSET		(CRG_REG_BASE + 0x30)

#define PERI_CRG57			0xe4
#define UART_CKSEL_APB			(1 << 19)

#define REG_ETH_CRG			0xCC
#define EFUSE_CTRL_CRG			0xD8


#define REG_TIMER_CONTROL		0x8
#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4
#define USB20HOST1_EHCI_REG_BASE	0x100B0000
#define USB20HOST2_EHCI_REG_BASE	0x100A0000
#define SFC_REG_BASE			0x10010000
#define SFC_ADDR_MODE_REG		0x8c
#define NANDC_REG_BASE			0x10000000
#define SPI_NAND_REG_BASE		0x10020000
#define RAM_START_ADRS			0x04010c00

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define REG_SYSSTAT			0x008C
#define BOOT_FROM_NAND	2
#define BOOT_FROM_DDR	3
#define BOOT_FROM_SPI_NOR	0
#define BOOT_FROM_SPI_NAND	1
#define BOOT_FROM_EMMC		3

/* Communication Register and flag used by bootrom */
#define REG_START_FLAG         (SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define CONFIG_START_MAGIC     (0x444f574e)

/*-----------------------------------------------------------------------
 * EMMC / SD
 * ----------------------------------------------------------------------*/
#define SDIO0_BASE_REG			0x10080000
#define SDIO1_BASE_REG			0x10090000

#define REG_START_MODE                  0x0000
#define NF_BOOTBW_MASK                  (1<<7)

#define MMC_IOMUX_START_ADDR		0x9C
#define MMC_IOMUX_END_ADDR		0xD8
#define IOMUX_REG47			0xBC
#define MMC_IOMUX_CTRL_MASK		(1<<0 | 1<<1)
#define MMC_IOMUX_CTRL			(1<<1)
/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/

#define STACK_TRAINING			0x0401e000
#define _HI3536_V100                  (0x0035360100LL)
#define _HI3536_MASK                  (0xFFFFFFFFFFLL)
#define REG_SC_GEN20                0x0090
#define NORMAL_BOOTMODE_OFFSET      9
#define NORMAL_BOOTMODE_MASK        3
/******************************************************************************/

/*--------------------------------------------------------------------------
 *PCIE slave start up
 *--------------------------------------------------------------------------*/
#define PCIE_SLV_STACK          0xe000
#define PCIE_SLV_DDR_INIT_FLG   0x8080
#define HI3536_SYSBOOT9         0x154
#define HI3536_SYSBOOT10	0x158

#endif /* End of __HI_CHIP_REGS_H__ */
