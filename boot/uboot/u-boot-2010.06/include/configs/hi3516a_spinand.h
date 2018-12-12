
/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the hi3516a board.
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

#ifndef __CONFIG_HI3516A_H__
#define __CONFIG_HI3516A_H__

#include <asm/arch/platform.h>

/*-----------------------------------------------------------------------
 * cpu_init configuration
 * if bit[3] = 0b, AXI = 198M
 * if bit[3] = 1b, AXI = 148.5M
 *----------------------------------------------------------------------*/
#define HW_REG(a) (*(unsigned long *)(a))
#define A7_AXI_SCALE_REG   REG_CRG12_OFFSET
#define get_bus_clk() ({\
	unsigned long tmp_reg, busclk;\
	tmp_reg = HW_REG(A7_AXI_SCALE_REG);\
	if (0x0 == (tmp_reg & 0x8)) {\
		busclk = 198000000;\
	} else {\
		busclk = 148500000;\
	} \
	busclk;\
})

/* cpu_init configuration */
#define CFG_CLK_BUS  get_bus_clk()

#define CFG_TIMER_PER 4		/* AXI:APB is 4:1 */
#define CFG_TIMER_CLK (CFG_CLK_BUS/CFG_TIMER_PER)

#define CONFIG_HI3516A		1

/*-----------------------------------------------------------------------
 * SPI Flash Configuration
 -----------------------------------------------------------------------*/
/* #define CONFIG_SPI_SUPPORT */

#ifdef CONFIG_SPI_SUPPORT
	#define CONFIG_ENV_IS_IN_SPI_FLASH /* env in spi flash */
	#define CONFIG_CMD_SF		/* sf read/sf write/sf erase */
	#define CONFIG_SPI_FLASH_HISFC350
#endif

#ifdef CONFIG_SPI_FLASH_HISFC350
	#define CONFIG_HISFC350_BUFFER_BASE_ADDRESS	SFC_MEM_BASE
	#define CONFIG_HISFC350_REG_BASE_ADDRESS	SFC_REG_BASE
	#define CONFIG_HISFC350_PERIPHERY_REGBASE	CRG_REG_BASE
	#define CONFIG_HISFC350_CHIP_NUM		2
#endif /* CONFIG_SPI_FLASH_HISFC350 */

/*-----------------------------------------------------------------------
 * NAND Flash Configuration
 *----------------------------------------------------------------------*/
#define CONFIG_NAND_SUPPORT

#ifdef CONFIG_NAND_SUPPORT
	#define CONFIG_ENV_IS_IN_NAND /* env in nand flash */
	#define CONFIG_CMD_NAND
	#define CONFIG_SYS_MAX_NAND_DEVICE		1
/*	#define CONFIG_NAND_FLASH_HINFC610	*/
	#define CONFIG_NAND_FLASH_HISNFC100
#endif /* CONFIG_NAND_SUPPORT */

#ifdef CONFIG_NAND_FLASH_HINFC610
	#define CONFIG_SYS_NAND_MAX_CHIPS		2
	#define CONFIG_SYS_NAND_BASE			NAND_MEM_BASE
	#define CONFIG_HINFC610_MAX_CHIP	CONFIG_SYS_MAX_NAND_DEVICE
	#define CONFIG_HINFC610_REG_BASE_ADDRESS	NANDC_REG_BASE
	#define CONFIG_HINFC610_BUFFER_BASE_ADDRESS	NAND_MEM_BASE
	#undef CONFIG_HINFC610_PAGESIZE_AUTO_ECC_NONE
#endif /* CONFIG_NAND_FLASH_HINFC610 */

#ifdef CONFIG_NAND_FLASH_HISNFC100
	#define CONFIG_SYS_NAND_MAX_CHIPS		1
	#define CONFIG_SYS_NAND_BASE			SPI_NAND_MEM_BASE
	#define CONFIG_HISNFC100_MAX_CHIP	CONFIG_SYS_MAX_NAND_DEVICE
	#define CONFIG_HISNFC100_REG_BASE_ADDRESS	SPI_NANDC_REG_BASE
	#define CONFIG_HISNFC100_BUFFER_BASE_ADDRESS	SPI_NAND_MEM_BASE
	#define CONFIG_HISNFC100_HARDWARE_PAGESIZE_ECC
#endif /* CONFIG_NAND_FLASH_HISNFC100 */

/*-----------------------------------------------------------------------
 * for cpu/hi3516a/start.S
 *----------------------------------------------------------------------*/
#define NAND_TEXT_ADRS			(NAND_MEM_BASE)
#define SF_TEXT_ADRS			(SFC_MEM_BASE)
#define SNAND_TEXT_ADRS			(SPI_NAND_MEM_BASE)

#define MEM_BASE_DDR			(DDR_MEM_BASE)
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128*1024)
/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)	cpu/hi3516a/hi3516a/timer.c
 * enable timer				board/hi3516a/board.c
 *----------------------------------------------------------------------*/
#define CFG_TIMERBASE           TIMER0_REG_BASE
/* enable timer.32bit, periodic,mask irq,256 divider. */
#define CFG_TIMER_CTRL		0xCA
#define READ_TIMER		(*(volatile unsigned long *)\
					(CFG_TIMERBASE + REG_TIMER_VALUE))
/* how many ticks per second. show the precision of timer. */
#define CONFIG_SYS_HZ           (CFG_TIMER_CLK/256)
#define CFG_HZ			CONFIG_SYS_HZ

/*allow change env*/
#define  CONFIG_ENV_OVERWRITE
/*-----------------------------------------------------------------------
 * environment && bd_info  && global_data  configure
 * used in file
 *----------------------------------------------------------------------*/
#include "../../product/env_setup.h"
#define CONFIG_ENV_OFFSET		0x80000 /* environment starts here */
#define CONFIG_ENV_NAND_ADDR		(CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SPI_ADDR		(CONFIG_ENV_OFFSET)
#define CONFIG_CMD_SAVEENV

#define CONFIG_ENV_SIZE			0x40000 /*include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE		CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS		1	/* we have 1 bank of DRAM */
/* kernel parameter list phy addr */
#define CFG_BOOT_PARAMS			(MEM_BASE_DDR+0x0100)

/*-----------------------------------------------------------------------
 *  Environment   Configuration
 *-----------------------------------------------------------------------*/
#define CONFIG_BOOTCOMMAND	"bootm 0x82000000"
#define CONFIG_BOOTDELAY	1
#define CONFIG_BOOTARGS		"mem=96M console=ttyAMA0,115200"
#define CONFIG_NETMASK		255.255.255.0	/* talk on MY local net */
#define CONFIG_IPADDR		192.168.1.10	/* static IP I currently own */
#define CONFIG_SERVERIP		192.168.1.2  /* current IP of tftp server ip */
#define CONFIG_ETHADDR		00:00:23:34:45:66
#define CONFIG_BOOTFILE		"uImage"	/* file to load */
#define CONFIG_BAUDRATE		115200
/*-----------------------------------------------------------------------
 * for bootm linux
 * used in file  board/hi3516a/board.c
 -----------------------------------------------------------------------*/

#define CONFIG_BOOTM_LINUX		1
/* default load address 0x80008000*/
#define CONFIG_SYS_LOAD_ADDR		(CFG_DDR_PHYS_OFFSET + 0x08000)
#define CONFIG_ZERO_BOOTDELAY_CHECK	1	/*use ^c to interrupt*/
/*-----------------------------------------------------------------------
 * for  commond configure
 -----------------------------------------------------------------------*/
/* tftp comm */
#define CONFIG_TFTP_TSIZE

/* do_printenv  do_setenv common/cmd_nvedit.c */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_MAXARGS		32 /* max number of command args   */


#define CONFIG_CMD_LOADB		/* loadb common/cmd_load.c */

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
 * HIETH-GMAC driver
 -----------------------------------------------------------------------*/
#define CONFIG_NET_HIGMACV300
#ifdef CONFIG_NET_HIGMACV300
	#define CONFIG_GMAC_NUMS		1
	#define HIGMAC0_IOBASE          0x10090000
/*	#define HIGMAC1_IOBASE          0x10091000	  */
	#define CONFIG_HIGMAC_PHY1_ADDR		1
/*	#define CONFIG_HIGMAC_PHY2_ADDR		2	  */
	#define CONFIG_HIGMAC_PHY1_INTERFACE_MODE	2
/*	#define CONFIG_HIGMAC_PHY2_INTERFACE_MODE	2 */

	/* need GPIO to reset external phy */
	#define HIGAMC_USE_GPIO_RESET_PHY
	#define HIGMAC_RESET_GPIO_BASE		0x20140000	/* GPIO0 */
	#define HIGMAC_RESET_GPIO_DIR_OFS	0x400
	#define HIGMAC_RESET_GPIO_DIR_OUT	(0x1 << 1)	/* Output */
	#define HIGMAC_RESET_GPIO_DATA_OFS	(0x4 << 1)	/* bit1 */
	#define HIGMAC_RESET_GPIO_VALUE		(0x1 << 1)
#endif

/* no nor flash */
#define CONFIG_SYS_NO_FLASH
/* cp.b */
#define CONFIG_CMD_MEMORY  /*md mw cp etc.*/

/*-----------------------------------------------------------------------
 * console display  Configuration
 ------------------------------------------------------------------------*/

#define CONFIG_VERSION_VARIABLE	1	/* used in common/main.c */
#define CONFIG_SYS_PROMPT	"hisilicon # " /* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	1024           /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE  (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)

#define CFG_LONGHELP
#define CFG_BARGSIZE		CFG_CBSIZE     /* Boot Argument Buffer Size */
#undef  CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */
/* default load address */
#define CFG_LOAD_ADDR		(CFG_DDR_PHYS_OFFSET + 0x08000)
#define CONFIG_AUTO_COMPLETE    1
#define CFG_CMDLINE_HISTORYS    8
#define CONFIG_CMDLINE_EDITING
#define CFG_DDR_PHYS_OFFSET	MEM_BASE_DDR
#define CFG_DDR_SIZE		(1024*1024*1024UL) /* 1GB */

#define CONFIG_SYS_MEMTEST_START       \
	(CFG_DDR_PHYS_OFFSET + sizeof(unsigned long))
#define CONFIG_SYS_MEMTEST_END         (CFG_DDR_PHYS_OFFSET + CFG_DDR_SIZE)
#define CONFIG_SYS_MEMTEST_SCRATCH     CFG_DDR_PHYS_OFFSET

#define CONFIG_CMDLINE_TAG		1 /* enable passing of ATAGs */
#define CONFIG_INITRD_TAG		1 /* support initrd */
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_MISC_INIT_R		1 /* call misc_init_r during start up */

#undef CONFIG_SPIID_TAG
/* serial driver */
#define CONFIG_PL011_SERIAL 1
/* Except bootrom, hi3516a use APB bus clock
 * UART is on APB, and AXI : APB is 4 : 1, so
 * we set UART clock 1/4 AXI bus_clk, but default
 * is 6MHz. */
#define CONFIG_PL011_CLOCK	(CFG_CLK_BUS/4)

#define CONFIG_PL01x_PORTS	\
	{(void *)UART0_REG_BASE, (void *)UART1_REG_BASE, \
	(void *)UART2_REG_BASE, (void *)UART3_REG_BASE}
#define CONFIG_CONS_INDEX		0

#define CFG_SERIAL0			UART0_REG_BASE

/* accoding to CONFIG_CONS_INDEX */
#define CONFIG_CUR_UART_BASE		CFG_SERIAL0
#define CONFIG_PRODUCTNAME		"hi3516a"

/*-----------------------------------------------------------------------
 * bootrom Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_BOOTROM_SUPPORT
#if defined(CONFIG_BOOTROM_SUPPORT) \
	&& (!defined(REG_START_FLAG) || !defined(CONFIG_START_MAGIC))
#  error Please define CONFIG_START_MAGIC and CONFIG_START_MAGIC first
#endif

/*-----------------------------------------------------------------------
 * usb storage system update
 * ----------------------------------------------------------------------*/
/* #define CONFIG_AUTO_UPDATE			1 */
#ifdef CONFIG_AUTO_UPDATE
	#define CONFIG_AUTO_SD_UPDATE		1
	#define CONFIG_AUTO_USB_UPDATE		1
#endif

#define __LITTLE_ENDIAN				1
#define CONFIG_DOS_PARTITION			1

#define CONFIG_CMD_FAT				1

/*-----------------------------------------------------------------------
 * sdcard
 * ----------------------------------------------------------------------*/
#ifdef CONFIG_AUTO_SD_UPDATE
	#define CONFIG_HIMCI_HI3516a
	#define REG_BASE_MCI			0x206e0000 /* MCI0 Only */
	#define CONFIG_HIMCI_V100
	#define CONFIG_GENERIC_MMC
	#define CONFIG_MMC			1
	#define CONFIG_CMD_MMC
#endif


/*-----------------------------------------------------------------------
 * usb
 * ----------------------------------------------------------------------*/
#define CONFIG_CMD_USB			1
#define CONFIG_USB_OHCI			1
#define CONFIG_USB_STORAGE		1
#define CONFIG_LEGACY_USB_INIT_SEQ

/*-----------------------------------------------------------------------
 * SVB Configure
 * ----------------------------------------------------------------------*/
#define CONFIG_SVB_ENABLE

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/
#define CONFIG_DDR_TRAINING_V300

#ifdef CONFIG_DDR_TRAINING_V300
	#undef  CONFIG_DDR_TRAINING_STARTUP
	#define CONFIG_DDR_DATAEYE_TRAINING_STARTUP
	#define CONFIG_DDR_TRAINING_HI3516A
#endif /* CONFIG_DDR_TRAINING_V300 */

/*-----------------------------------------------------------------------
 * Snapshot boot support
 * ----------------------------------------------------------------------*/
/* #define CONFIG_SNAPSHOT_BOOT 1 */

#ifdef CONFIG_SNAPSHOT_BOOT

/* enable MMU for fast decompress */
#define CONFIG_ARCH_MMU

/* reserved 4M size for uboot*/
/* 1G vitual size, use 1024 entries, 4K per table, total 4M size */
#define CONFIG_STB_ADDR	         (TEXT_BASE + (4<<20))

/* ttb table, 4096 entries, 4 bytes per entry, total 4*4096=16k */
#define CONFIG_TTB_ADDR	         (TEXT_BASE + (8<<20))

#define CONFIG_SNAPSHOT_LOADADDR (TEXT_BASE + (9<<20))

#define CONFIG_LZO
#define CONFIG_SHA1

#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET

/* #define CONFIG_USE_IRQ */

#endif

/*-----------------------------------------------------------------------
 * for interrupt configuration		arch/arm/cpu/hi3516a/interrupts.c
 * use interrupt for snapshot fast read
 *----------------------------------------------------------------------*/
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ    (4*1024)        /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ    (4*1024)        /* FIQ stack */
#endif

/* #define CONFIG_AUDIO_ENABLE */
/* #define CONFIG_OSD_ENABLE */

#endif	/* __CONFIG_H */

