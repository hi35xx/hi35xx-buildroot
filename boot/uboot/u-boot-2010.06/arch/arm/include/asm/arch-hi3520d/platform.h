
#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define DDR_MEM_BASE			0x80000000
#define PCIE1_CFG_BASE			0x70000000
#define PCIE1_MEM_BASE			0x60000000
#define SFC_MEM_BASE			0x58000000
#define NAND_MEM_BASE			0x50000000
#define PCIE0_CFG_BASE			0x40000000
#define PCIE0_MEM_BASE			0x30000000
#define PCIE1_REG_BASE			0x20810000
#define PCIE0_REG_BASE			0x20800000
#define L2_CACHE_BASE			0x20700000
#define ARM_DEBUG_BASE			0x20400000
#define ARM_REG_BASE			0x20300000
#define GPIO18_REG_BASE			0x20270000
#define GPIO17_REG_BASE			0x20260000
#define GPIO16_REG_BASE			0x20250000
#define GPIO15_REG_BASE			0x20240000
#define GPIO14_REG_BASE			0x20230000
#define GPIO13_REG_BASE			0x20220000
#define GPIO12_REG_BASE			0x20210000
#define GPIO11_REG_BASE			0x20200000
#define GPIO10_REG_BASE			0x201F0000
#define GPIO9_REG_BASE			0x201E0000
#define GPIO8_REG_BASE			0x201D0000
#define GPIO7_REG_BASE			0x201C0000
#define GPIO6_REG_BASE			0x201B0000
#define GPIO5_REG_BASE			0x201A0000
#define GPIO4_REG_BASE			0x20190000
#define GPIO3_REG_BASE			0x20180000
#define GPIO2_REG_BASE			0x20170000
#define GPIO1_REG_BASE			0x20160000
#define GPIO0_REG_BASE			0x20150000
#define DDRC1_REG_BASE			0x20120000
#define DDRC0_REG_BASE			0x20110000
#define IO_CONFIG_REG_BASE		0x200F0000
#define UART3_REG_BASE			0x200B0000
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

#define EFUSE_REG_BASE			0x20100000

#define EFUSE_DATA_SEL			0x28

#define WDG_REG_BASE			0x20040000
#define CRG_REG_BASE			0x20030000
#define PERI_CRG57			0xe4
#define UART_CKSEL_APB			(1 << 14)

#define REG_ETH_CRG			0xCC
#define EFUSE_CTRL_CRG			0xD8

#define TIMER1_REG_BASE			0x20010000
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
#define RAM_START_ADRS			0x04010500

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define REG_SYSSTAT			0x008C
#define BOOT_FROM_NAND	2
#define BOOT_FROM_DDR	1
#define BOOT_FROM_SPI	0
/******************PLL DDR CONFIG START************************************/

/* APLL CONFIG */
#define REG_CRG0_OFFSET (CRG_REG_BASE + 0)
#define REG_CRG0_VALUE  0x89000000
#define REG_CRG1_OFFSET (CRG_REG_BASE + 0x4)
#define REG_CRG1_VALUE  0x006c3064

/* A9 MAX FRE && A9-AXI FRE-SCALE CONFIG */
#define REG_CRG8_OFFSET (CRG_REG_BASE + 0x20)

#define BITOFFSET_A9_MAX_FREQUENCY    0
#define BITMASK_A9_MAX_FREQUENCY      0xfffffffe
#define BITVALUE_A9_MAX_FREQUENCY     0 /* 0:800MHZ, 1:750MHZ*/

#define BITOFFSET_A9_AXI_FRE_SCALE    1
#define BITMASK_A9_AXI_FRE_SCALE      0xfffffff9
#define BITVALUE_A9_AXI_FRE_SCALE 0 /* 0-(4:1);1-(3:1);2-(2:1);3(1:1) */

/* DDR FREQ_SCALE CONFIG */
#define REG_CRG10_OFFSET (CRG_REG_BASE + 0x28)
#define REG_DDR_FRE_SCALE_VALUE  0 /*0-(2:1); 1-(1:1)*/

/* EPLL CONFIG */
#define REG_CRG6_OFFSET (CRG_REG_BASE + 0x18)
#define REG_CRG6_VALUE  0x99000000
#define REG_CRG7_OFFSET (CRG_REG_BASE + 0x1c)
#define REG_CRG7_VALUE  0x007c207d

/* APLL WAIT TIME CONFIG */
#define REG_PLLCTL_OFFSET (SYS_CTRL_REG_BASE + 0x14)

#define BITOFFSET_PLLCTL_PLLTIME              3
#define BITMASK_PLLCTL_PLLTIME      0xf0000007
#define BITVALUE_PLLCTL_PLLTIME     0x0fff8000/*wait time: 0x2000000-0x1fff000*/

#define BITOFFSET_PLLCTL_PLLOVER             0
#define BITMASK_PLLCTL_PLLOVER     0xfffffffe
#define BITVALUE_PLLCTL_PLLOVER     0x0 /*0: apll controlled by mode switching*/

/* PLL LOCK READ CONFIG */
#define REG_CRG43_OFFSET (CRG_REG_BASE + 0xac)
#define REG_CRG43_MASK  (0xf)

#define SC_LOCKEN_OFFSET (SYS_CTRL_REG_BASE + 0x44)
#define SC_LOCKEN_VALUE	0x1acce551

/* A9 WORKMODE SWITCH */
#define SC_SYSCTL_OFFSET (SYS_CTRL_REG_BASE + 0x0)
#define BITOFFSET_A9_WORKMODE 0
#define BITMASK_A9_WORKMODE   0xfffffff8
/*0:reserve; 0x1:doze; 0x2: slow; 0x4: normal*/
#define BITVALUE_A9_WORKMODE  0x4
/******************PLL DDR CONFIG END************************************/
/* Slave bootloader running address after  master clear remap */
#define COREX_BOOT_ADDR                 0x83c00000

/* Communication Register and flag used by bootrom */
#define REG_START_FLAG         (SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define CONFIG_START_MAGIC     (0x444f574e)

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/

#define DDRT_BASE_ADDR 0x206d0000
#define DDRT_SYS_BASE_ADDR 0x2005003c
#define DDRT_ENABLE_SYS (1 << 15)

#define DDRT_CLK_BASE_ADDR 0x200300d8
#define DDRT_ENABLE_DDRTEST 0x8
#define DDRT_RESET (1 << 2)
#define DDRT_CLK (1 << 3)

#define DDRT_CHECK_VALUE 0x64647274
#define DDRT_SP 0x2450

#define DDRT_OPTION_REG1 0x2005009c
#define DDRT_OPTION_REG2 0x200500a0
#define DDRC_PHY_REG1 0x20110800

#define DDRT_STRUCT_ADDR 0x2460

/* ddrt window level number */
#define DDRT_WR_WL_NR 15
#define DDRT_RD_WL_NR 15
#define DDRT_RESULT_NR 8

#define REG_CHIP_ID_BASE 0x2005006c

#endif /* End of __HI_CHIP_REGS_H__ */
