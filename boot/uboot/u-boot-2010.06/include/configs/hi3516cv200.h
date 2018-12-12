
/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the hi3518ev200 board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_HI3518EV200_H__
#define __CONFIG_HI3518EV200_H__

#include <asm/arch/platform.h>
/* Arm configuration */
#define CONFIG_L2_OFF
/*-----------------------------------------------------------------------
 * cpu_init configuration
 * if bit[0] = 0b, AXI = 24M
 * if bit[0] = 1b, AXI = 198M
 *----------------------------------------------------------------------*/
#define HW_REG(a) (*(unsigned long *)(a))
#define AXI_SCALE_REG   (CRG_REG_BASE + REG_CRG12)
#define get_bus_clk() ({\
	unsigned long tmp_reg, busclk = 0;\
	tmp_reg = HW_REG(AXI_SCALE_REG);\
	tmp_reg &= 0x1;\
	if (0x0 == tmp_reg) {\
		busclk = 24000000;\
	} else {\
		busclk = 198000000;\
	} \
	busclk;\
})

/* cpu_init configuration */
#define CFG_CLK_BUS  get_bus_clk()

#define CFG_TIMER_PER 4		/* AXI:APB is 4:1 */
#define CFG_TIMER_CLK (CFG_CLK_BUS / CFG_TIMER_PER)

#define CONFIG_HI3518EV200

/*-----------------------------------------------------------------------
 * Hisilicon Flash Memory Controller Configuration
 *----------------------------------------------------------------------*/
#define CONFIG_HIFMC

#ifdef CONFIG_HIFMC
	#define CONFIG_HIFMC_SPI_NOR
	#define CONFIG_HIFMC_SPI_NAND

	#define CONFIG_HIFMC_REG_BASE		FMC_REG_BASE
	#define CONFIG_HIFMC_BUFFER_BASE	FMC_MEM_BASE

	#define CONFIG_HIFMC_MAX_CS_NUM		2
#endif

#ifdef CONFIG_HIFMC_SPI_NOR
	#define CONFIG_CMD_SF
	#define CONFIG_ENV_IS_IN_SPI_FLASH
	#define CONFIG_SPI_NOR_MAX_CHIP_NUM	2
	#define CONFIG_SPI_NOR_QUIET_TEST
	#define CONFIG_SPI_BLOCK_PROTECT
#endif

#ifdef CONFIG_HIFMC_SPI_NAND
	#define CONFIG_CMD_NAND
	#define CONFIG_ENV_IS_IN_NAND
	#define CONFIG_SPI_NAND_MAX_CHIP_NUM	1
	#define CONFIG_SYS_MAX_NAND_DEVICE	CONFIG_SPI_NAND_MAX_CHIP_NUM
	#define CONFIG_SYS_NAND_MAX_CHIPS	CONFIG_SPI_NAND_MAX_CHIP_NUM
	#define CONFIG_SYS_NAND_BASE		FMC_MEM_BASE
	#define CONFIG_SYS_NAND_QUIET_TEST

	#undef CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC
	#define CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	#undef CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE
#endif

#define CONFIG_CLOSE_SPI_8PIN_4IO

/*-----------------------------------------------------------------------
 * for cpu/hi3518ev200/start.S
 *----------------------------------------------------------------------*/
#define FMC_TEXT_ADRS		(FMC_MEM_BASE)

#define MEM_BASE_DDR		(DDR_MEM_BASE)
#define CONFIG_SYS_MALLOC_LEN	(CONFIG_ENV_SIZE + 128*1024)
/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)
 * arch/arm/cpu/hi3518ev200/hi3518ev200/timer.c
 * enable timer				board/hi3518ev200/board.c
 *----------------------------------------------------------------------*/
#define CFG_TIMERBASE           TIMER0_REG_BASE
/* enable timer.32bit, periodic,mask irq,256 divider. */
#define CFG_TIMER_CTRL		0xCA
#define READ_TIMER		(*(volatile unsigned long *)\
					(CFG_TIMERBASE + REG_TIMER_VALUE))
/* how many ticks per second. show the precision of timer. */
#define CONFIG_SYS_HZ           (CFG_TIMER_CLK / 256)
#define CFG_HZ			CONFIG_SYS_HZ

/*allow change env*/
#define  CONFIG_ENV_OVERWRITE
/*-----------------------------------------------------------------------
 * environment && bd_info  && global_data  configure
 * used in file
 *----------------------------------------------------------------------*/
#include "../../product/env_setup.h"
/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_OFFSET	0x80000      /* environment starts here */
#define CONFIG_ENV_NAND_ADDR	(CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SPI_ADDR	(CONFIG_ENV_OFFSET)
#define CONFIG_CMD_SAVEENV

#define CONFIG_ENV_SIZE		0x40000    /*include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE	CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS	1          /* we have 1 bank of DRAM */
/* kernel parameter list phy addr */
#define CFG_BOOT_PARAMS		(MEM_BASE_DDR + 0x0100)

/*-----------------------------------------------------------------------
 *  Environment   Configuration
 *-----------------------------------------------------------------------*/
#define CONFIG_BOOTCOMMAND "bootm 0x82000000"
#define CONFIG_BOOTDELAY	1
#define CONFIG_BOOTARGS "mem=96M console=ttyAMA0,115200"
#define CONFIG_NETMASK  255.255.255.0       /* talk on MY local net */
#define CONFIG_IPADDR   192.168.1.10        /* static IP I currently own */
#define CONFIG_SERVERIP 192.168.1.2     /* current IP of tftp server ip */
#define CONFIG_ETHADDR  00:00:23:34:45:66
#define CONFIG_BOOTFILE "uImage"        /* file to load */
#define CONFIG_BAUDRATE         115200
/*-----------------------------------------------------------------------
 * for bootm linux
 * used in file  board/hi3518ev200/board.c
 -----------------------------------------------------------------------*/

#define CONFIG_BOOTM_LINUX 1
/* default load address 0x80008000*/
#define CONFIG_SYS_LOAD_ADDR (CFG_DDR_PHYS_OFFSET + 0x08000)
#define CONFIG_ZERO_BOOTDELAY_CHECK 1   /*use ^c to interrupt*/
/*-----------------------------------------------------------------------
 * for  commond configure
 -----------------------------------------------------------------------*/
/* tftp comm */
#define CONFIG_TFTP_TSIZE

/* do_printenv  do_setenv common/cmd_nvedit.c */
#define CONFIG_SYS_BAUDRATE_TABLE { 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_MAXARGS 32          /* max number of command args   */


#define CONFIG_CMD_LOADB  /* loadb common/cmd_load.c*/

/*-----------------------------------------------------------------------
 * network config
 -----------------------------------------------------------------------*/
#define CONFIG_ARP_TIMEOUT		50000UL
#define CONFIG_NET_RETRY_COUNT		50
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_MII
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	1

/*-----------------------------------------------------------------------
 * HIETH driver
 -----------------------------------------------------------------------*/
/* default is hieth-switch-fabric */
#define CONFIG_NET_HISFV300
#ifdef CONFIG_NET_HISFV300
	#define CONFIG_NET_HISFV300_HI3518EV200		1
	#define HISFV_MII_MODE                  0
	#define HISFV_RMII_MODE                 1
	#define HIETH_MII_RMII_MODE_U           HISFV_RMII_MODE
	#define HIETH_MII_RMII_MODE_D           HISFV_RMII_MODE
	#define HISFV_PHY_U                     1
	#define HISFV_PHY_D                     2 /* fix me */
	#undef HISFV_RESET_GPIO_EN
	#ifdef HISFV_RESET_GPIO_EN
		/* use gpio5_0 to control sfv reset */
		#define HISFV_RESET_GPIO_BASE   GPIO0_REG_BASE
		#define HISFV_RESET_GPIO_DIR    0x400
		#define HISFV_RESET_GPIO_BIT    5
		#define HISFV_RESET_GPIO_DIR_OUT 1
		/* 0-reset; 1-undo reset */
		#define HISFV_RESET_GPIO_DATA   0
	#endif /* HISFV_RESET_GPIO_EN */
#endif /* CONFIG_NET_HISFV300 */

/* no nor flash */
#define CONFIG_SYS_NO_FLASH
/* cp.b */
#define CONFIG_CMD_MEMORY  /*md mw cp etc.*/

/*-----------------------------------------------------------------------
 * console display  Configuration
 ------------------------------------------------------------------------*/

#define CONFIG_VERSION_VARIABLE  1 /*used in common/main.c*/
#define CONFIG_SYS_PROMPT  "hisilicon # "	/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE  1024            /* Console I/O Buffer Size  */
#define CONFIG_SYS_PBSIZE  (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

#define CFG_LONGHELP
#define CFG_BARGSIZE    CFG_CBSIZE      /* Boot Argument Buffer Size */
#undef  CFG_CLKS_IN_HZ              /* everything, incl board info, in Hz */
/* default load address */
#define CFG_LOAD_ADDR   (CFG_DDR_PHYS_OFFSET + 0x08000)
#define CONFIG_AUTO_COMPLETE    1
#define CFG_CMDLINE_HISTORYS    8
#define CONFIG_CMDLINE_EDITING
#define CFG_DDR_PHYS_OFFSET MEM_BASE_DDR
#define CFG_DDR_SIZE		(512 * 1024 * 1024UL) /* 512MB */

#define CONFIG_SYS_MEMTEST_START       \
	(CFG_DDR_PHYS_OFFSET + sizeof(unsigned long))
#define CONFIG_SYS_MEMTEST_END         (CFG_DDR_PHYS_OFFSET + CFG_DDR_SIZE)
#define CONFIG_SYS_MEMTEST_SCRATCH     CFG_DDR_PHYS_OFFSET

#define CONFIG_CMDLINE_TAG      1   /* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG       1   /* support initrd */
#define CONFIG_SETUP_MEMORY_TAGS    1
#define CONFIG_MISC_INIT_R      1   /* call misc_init_r during start up */

#undef CONFIG_SPIID_TAG
/* serial driver */
#define CONFIG_PL011_SERIAL 1
/* Except bootrom, hi3518ev200 UART use XTAL OSC clk 24M */
#define CONFIG_PL011_CLOCK	(24000000)

#define CONFIG_PL01x_PORTS	\
	{(void *)UART0_REG_BASE, (void *)UART1_REG_BASE, \
	(void *)UART2_REG_BASE}
#define CONFIG_CONS_INDEX	0

#define CFG_SERIAL0                   UART0_REG_BASE

/* accoding to CONFIG_CONS_INDEX */
#define CONFIG_CUR_UART_BASE          CFG_SERIAL0
#define CONFIG_PRODUCTNAME		"hi3518ev200"

/*-----------------------------------------------------------------------
 * bootrom Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_BOOTROM_SUPPORT
#if defined(CONFIG_BOOTROM_SUPPORT) \
	&& (!defined(REG_START_FLAG) || !defined(CONFIG_START_MAGIC))
#  error Please define CONFIG_START_MAGIC and CONFIG_START_MAGIC first
#endif

/*-----------------------------------------------------------------------
 * sdcard or usb storage system update
 * ----------------------------------------------------------------------*/
/* #define CONFIG_AUTO_UPDATE			1 */
#ifdef CONFIG_AUTO_UPDATE
	#define CONFIG_AUTO_SD_UPDATE		1
	#define CONFIG_AUTO_USB_UPDATE		1
#endif

/* #define __LITTLE_ENDIAN				1 */
#define CONFIG_DOS_PARTITION			1

#define CONFIG_CMD_FAT				1
#define CONFIG_CMD_EXT2				1

/*-----------------------------------------------------------------------
 * sdcard
 * ----------------------------------------------------------------------*/
#ifdef CONFIG_AUTO_SD_UPDATE
	#ifndef CONFIG_HIMCI_V200
		#define CONFIG_HIMCI_V200
	#endif
#endif

/*-----------------------------------------------------------------------
 * eMMC Flash Configuration
 * ----------------------------------------------------------------------*/
#ifndef CONFIG_AUTO_SD_UPDATE /* Host0 is multiplex for sd/eMMC */
#define CONFIG_EMMC_SUPPORT
#ifdef CONFIG_EMMC_SUPPORT
	#define CONFIG_HIMCI_V200
	#define CONFIG_EMMC_PORT	1
#endif
#endif

#ifdef CONFIG_HIMCI_V200
	#define CONFIG_MMC_DEVID	            1 /* emmc = 1; SD = 0 */
	#define CONFIG_MMC_BOOT_ADDR                0
	#define CONFIG_MMC_POWER_OFF_TIMEOUT        5
	#define CONFIG_MMC_POWER_ON_TIMEROUT        40
	#define CONFIG_MMC_RESET_LOW_TIMEOUT        10
	#define CONFIG_MMC_RESET_HIGH_TIMEROUT      300
	#define CONFIG_GENERIC_MMC
#endif /* CONFIG_HIMCI_V200 */
#ifdef CONFIG_GENERIC_MMC
	/* env in flash instead of CFG_ENV_IS_NOWHERE */
	#define CONFIG_ENV_IS_IN_EMMC               1
	#define CONFIG_CMD_MMC
	#define CONFIG_MMC			1
	#define CONFIG_EXT4
	#define CONFIG_EXT4_SPARSE
#endif /* CONFIG_GENERIC_MMC */

/*-----------------------------------------------------------------------
 * usb
 * ----------------------------------------------------------------------*/
#define CONFIG_CMD_USB			1
#define CONFIG_USB_OHCI			1	/* FIXME: CONFIG_USB_OHCI_NEW */
#define CONFIG_USB_STORAGE		1
#define CONFIG_LEGACY_USB_INIT_SEQ

/*-----------------------------------------------------------------------
 * SVB Configure
 * ----------------------------------------------------------------------*/
#define CONFIG_SVB_ENABLE

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/
#define CONFIG_DDR_TRAINING_V2

/* #define CONFIG_OSD_ENABLE */

/* #define CONFIG_PRODUCT_HDMI_PHY */

/*-----------------------------------------------------------------------
 * Snapshot boot support
 * ----------------------------------------------------------------------*/
#define CONFIG_SNAPSHOT_BOOT 1
#ifdef CONFIG_SNAPSHOT_BOOT
	/* enable MMU for fast decompress */
	/* #define CONFIG_ARCH_MMU */

	#define CONFIG_LZMA
	#define CONFIG_SHA1
	#define CONFIG_HW_DEC
	#define CONFIG_USE_ARCH_MEMCPY
	#define CONFIG_USE_ARCH_MEMSET
	/* #define CONFIG_USE_IRQ */
#endif

#endif	/* __CONFIG_H */

