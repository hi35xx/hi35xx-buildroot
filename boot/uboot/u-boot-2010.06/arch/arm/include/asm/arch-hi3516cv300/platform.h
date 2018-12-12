/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __HI_CHIP_REGS_H__
#define __HI_CHIP_REGS_H__

#define BIT(nr)			(1 << (nr))

#define RAM_START_ADRS			0x04010500
#define FMC_REG_BASE			0x10000000
#define REG_BASE_SF			0x10050000

/*-----------------------------------------------------------------------
 * EMMC / SD
 *----------------------------------------------------------------------*/
#define SDIO0_BASE_REG			0x100c0000
#define SDIO1_BASE_REG			0x100d0000
#define EMMC_BASE_REG			0x100e0000
#define SDIO2_BASE_REG			0x100f0000
#define MMC_IOMUX_START_ADDR		0x68
#define MMC_IOMUX_END_ADDR		0x84
#define MMC_IOMUX_CTRL_MASK		BIT(0)
#define MMC_IOMUX_CTRL			1

#define HIUSB_OHCI_BASE			0x10110000
#define HIUSB_EHCI_BASE			0x10120000
#define REG_USB2_DEV_BASE		0x10130000

#define DDR_DDRT_REG_BASE		0x11200000
#define TIMER0_REG_BASE			0x12000000
#define TIMER1_REG_BASE			0x12000020
#define TIMER2_REG_BASE			0x12001000
#define TIMER3_REG_BASE			0x12001020
#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4
#define REG_TIMER_CONTROL		0x8

#define CRG_REG_BASE			0x12010000
#define REG_CRG12			0x30
#define REG_CRG46			0xb8
#define REG_CRG48			0xc0
#define REG_CRG49			0xc4
#define SDIO0_SRST			BIT(0)
#define SDIO0_CLK_EN			BIT(1)
#define SDIO0_CLK_SEL_MASK		(0x3 << 4)
#define SDIO0_CLK_SEL_SHIFT		4
#define SDIO0_CLK_SEL_49_5M		0x0
#define SDIO0_CLK_MODE			BIT(7)
#define SDIO1_SRST			BIT(8)
#define SDIO1_CLK_EN			BIT(9)
#define SDIO1_CLK_SEL_MASK		(0x3 << 12)
#define SDIO1_CLK_SEL_SHIFT		12
#define SDIO1_CLK_SEL_49_5M		0x0
#define SDIO1_CLK_MODE			BIT(15)
#define EMMC_SRST			BIT(16)
#define EMMC_CLK_EN			BIT(17)
#define EMMC_CLK_SEL_MASK		(0x3 << 20)
#define EMMC_CLK_SEL_SHIFT		20
#define EMMC_CLK_SEL_99M		0x0
#define EMMC_CLK_SEL_49_5M		0x2
#define EMMC_CLK_MODE			BIT(23)

#define REG_CRG50			0xc8
#define SDIO2_SRST			BIT(0)
#define SDIO2_CLK_EN			BIT(1)
#define SDIO2_CLK_SEL_MASK		(0x3 << 4)
#define SDIO2_CLK_SEL_SHIFT		4
#define SDIO2_CLK_SEL_49_5M		0x0
#define SDIO2_CLK_MODE			BIT(7)

#define PERI_CRG57			0xe4
#define UART0_SRST			BIT(7)
#define UART1_SRST			BIT(8)
#define UART2_SRST			BIT(9)
#define UART0_CLK_EN			BIT(15)
#define UART1_CLK_EN			BIT(16)
#define UART2_CLK_EN			BIT(17)
#define UART_CLK_SEL_MASK		BIT(19)
#define UART_CLK_SEL_SHIFT		19
#define UART_CLK_SEL_24M		0x0
#define UART_CLK_SEL_6M			0x1

#define PERI_CRG59			0xec
#define REG_ETH_CRG			PERI_CRG59

#define SYS_CTRL_REG_BASE		0x12020000
#define REG_BASE_SCTL			SYS_CTRL_REG_BASE
#define REG_SC_CTRL			0
#define REMAPCLEAR			BIT(8)
#define REMAPCLEAR_SHIFT		8
#define TIME0_CLK_SEL			BIT(16)
#define TIME0_CLK_SEL_SHIFT		16
#define TIME0_CLK_SEL_3M		0x0
#define TIME0_CLK_SEL_APB		0x1

#define REG_SC_SYSRES			0x4
#define REG_SYSSTAT			0x008C
#define REG_SC_GEN0			0x0138
#define REG_SC_GEN1			0x013c
#define REG_SC_GEN2			0x0140
#define REG_SC_GEN3			0x0144
#define REG_SC_GEN4			0x0148
#define REG_SC_GEN5			0x014c
#define REG_SC_GEN7			0x0154

#define MISC_REG_BASE			0x12030000
#define MISC_CTRL0			0x0
#define GZIP_DDRT_SEL_MASK		BIT(19)
#define GZIP_DDRT_SEL_SHIFT		19
#define GZIP_DDRT_SEL_DDRT		0
#define GZIP_DDRT_SEL_GZIP		1
#define MISC_CTRL47			0xbc
#define IO_CONFIG_REG_BASE		0x12040000
#define DDRC0_REG_BASE			0x12060000
#define WDG_REG_BASE			0x12080000
#define RTC_REG_BASE			0x12090000
#define UART0_REG_BASE			0x12100000
#define UART1_REG_BASE			0x12101000
#define UART2_REG_BASE			0x12102000
#define I2C0_REG_BASE			0x12110000
#define I2C1_REG_BASE			0x12112000
#define SPI0_REG_BASE			0x12120000
#define SPI1_REG_BASE			0x12121000
#define PWM_REG_BASE			0x12130000
#define GPIO0_REG_BASE			0x12140000
#define GPIO1_REG_BASE			0x12141000
#define GPIO2_REG_BASE			0x12142000
#define GPIO3_REG_BASE			0x12143000
#define GPIO4_REG_BASE			0x12144000
#define GPIO5_REG_BASE			0x12145000
#define GPIO6_REG_BASE			0x12146000
#define GPIO7_REG_BASE			0x12147000
#define GPIO8_REG_BASE			0x12148000
#define FMC_MEM_BASE			0x14000000
#define DDR_MEM_BASE			0x80000000

/*-----------------------------------------------------------------------
 * PERI_CRG48 FMC REG
 * ----------------------------------------------------------------------*/
#define REG_FMC_CRG			REG_CRG48
#define FMC_SRST			BIT(0)
#define FMC_CLK_EN			BIT(1)
#define FMC_CLK_SEL_MASK		(0x7 << 2)
#define FMC_CLK_SEL_SHIFT		2
/* SDR/DDR clock */
#define FMC_CLK_24M			0x0
#define FMC_CLK_83P3M			0x1
#define FMC_CLK_148P5M			0x2
/* Only DDR clock */
#define FMC_CLK_198M			0x3
#define FMC_CLK_297M			0x4

#define FMC_CLK_SEL(_clk) \
	(((_clk) << FMC_CLK_SEL_SHIFT) & FMC_CLK_SEL_MASK)
#define GET_FMC_CLK_TYPE(_reg) \
	(((_reg) & FMC_CLK_SEL_MASK) >> FMC_CLK_SEL_SHIFT)

/*-----------------------------------------------------------------------
 * REG_SYSSTAT: boot mode
 * ----------------------------------------------------------------------*/
#define BOOT_MODE_MASK			BIT(4)
#define BOOT_MODE_SHIFT			4
#define BOOT_FROM_FMC			0x0
#define BOOT_FROM_EMMC			0x1
#define FMC_BOOT_DEV_TYPE		BIT(6)
#define FMC_BOOT_DEV_TYPE_SHIFT		6
#define FMC_BOOT_MULTI_MODE		BIT(7)
#define FMC_BOOT_MULTI_MODE__SHIFT	7

/* FMC device type; 0:spi nor, 1:spi nand */
#define GET_SPI_DEVICE_TYPE(_reg) \
	(((_reg) & FMC_BOOT_DEV_TYPE) >> FMC_BOOT_DEV_TYPE_SHIFT)

/* SPI nor address mode; 0:3-Byte | 1:4-Byte */
#define GET_SPI_NOR_ADDR_MODE(_reg) \
	(((_reg) & (FMC_BOOT_MULTI_MODE)) >> FMC_BOOT_MULTI_MODE__SHIFT)

/* SPI nand I/O widthe; 0: 1-I/O | 1: 4-I/O */
#define GET_SPI_NAND_IO_WIDTHE_MODE(_reg) \
	(((_reg) & (FMC_BOOT_MULTI_MODE)) >> FMC_BOOT_MULTI_MODE__SHIFT)

/********** Communication Register and flag used by bootrom *************/
#define REG_START_FLAG			(SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define REG_USB_UART_FLAG		(SYS_CTRL_REG_BASE + REG_SC_GEN7)
#define CONFIG_START_MAGIC		0x444f574e
#define SELF_BOOT_TYPE_UART		1  /* boot type: UART */
#define SELF_BOOT_TYPE_USBDEV		2  /* boot type: USBDEV */

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/
#define STACK_TRAINING			0x04018000

/*-----------------------------------------------------------------------
 * for fast boot
 *-----------------------------------------------------------------------*/
#define GZIP_REG_BASE			0x11210000
#define REG_SNAPSHOT_IMAGE_MAGIC	0x120200a4
#define REG_SNAPSHOT_IMAGE_START	0x120200a8
#define REG_SNAPSHOT_IMAGE_SIZE		0x120200ac
#define HW_DEC_INTR			8

#define _HI3516CV300			(0x003516c300LL)
#define _HI3516CV300_MASK		(0xFFFFFFFFFFLL)
#endif /* End of __HI_CHIP_REGS_H__ */
