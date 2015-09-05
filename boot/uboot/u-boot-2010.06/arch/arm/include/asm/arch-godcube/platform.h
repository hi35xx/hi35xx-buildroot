#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define DDR_MEM_BASE			0x80000000
#define SFC_MEM_BASE			0x58000000
#define ARM_REG_BASE			0x20300000
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
#define UART0_REG_BASE			0x20080000
#define RTC_REG_BASE			0x20060000
#define REG_BASE_SCTL			0x20050000
#define REG_SC_CTRL			0
#define REG_SC_SYSRES			0x4

#define WDG_REG_BASE			0x20040000
#define CRG_REG_BASE			0x20030000
#define REG_ETH_CRG			0xC8

#define TIMER1_REG_BASE			0x20010000
#define TIMER0_REG_BASE			0x20000000
#define REG_TIMER_CONTROL		0x8
#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4

#define REG_BASE_SF                     0x10090000
#define SFC_REG_BASE			0x10010000
/* for u-boot enable TCM */
#define MEM_BASE_ITCM			0x00000000
#define MEM_SIZE_ITCM			0x00000800
#define MEM_CONF_ITCM_SIZE		3

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define REG_SYSSTAT			0x8C
/* boot mode */
#define BOOT_FROM_SPI			0
#define BOOT_FROM_DDR			1

/******************PLL CONFIG START************************************/
/* APLL CONFIG */
#define REG_CRG0_OFFSET			(CRG_REG_BASE + 0)
#define REG_CRG0_VALUE			0x09000000 /* reset value */
#define REG_CRG1_OFFSET			(CRG_REG_BASE + 0x4)
#define REG_CRG1_VALUE			0x006c309B /* reset value */

/* EPLL CONFIG */
#define REG_CRG6_OFFSET			(CRG_REG_BASE + 0x18)
#define REG_CRG7_OFFSET			(CRG_REG_BASE + 0x1c)
#define REG_CRG8_OFFSET			(CRG_REG_BASE + 0x20)

/* ARM9 freq mode A9:AXI CONFIG */
#define REG_CRG10_OFFSET		(CRG_REG_BASE + 0x28)

/* PLL LOCK READ CONFIG */
#define REG_CRG58_OFFSET		(CRG_REG_BASE + 0xE8)

/* APLL WAIT TIME CONFIG */
#define REG_PLLCTL_OFFSET		(REG_BASE_SCTL + 0x14)

/*wait time: 0x2000000-0x1fff000*/
#define BITOFFSET_PLLCTL_PLLTIME	3
#define BITMASK_PLLCTL_PLLTIME		0xf0000007
#define BITVALUE_PLLCTL_PLLTIME		0x1fff000

/*0: apll controlled by mode switching*/
#define BITOFFSET_PLLCTL_PLLOVER	0
#define BITMASK_PLLCTL_PLLOVER		0xfffffffe
#define BITVALUE_PLLCTL_PLLOVER		0x0

#define SC_LOCKEN_OFFSET		(REG_BASE_SCTL + 0x44)
#define SC_LOCKEN_VALUE			0x1acce551

/******************PLL CONFIG END************************************/
#endif /* __HI_CHIP_REGS_H__ */
