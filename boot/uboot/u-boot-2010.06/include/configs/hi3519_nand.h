
/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the hi3519 board.
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

#ifndef __CONFIG_HI3519_H__
#define __CONFIG_HI3519_H__

#include <asm/arch/platform.h>

/*-----------------------------------------------------------------------
 * cpu_init configuration
 * if bit[13:12] = 1b, AXI = 200M
 *----------------------------------------------------------------------*/
#define HW_REG(a) (*(unsigned long *)(a))
#define A7_AXI_SCALE_REG   (CRG_REG_BASE + 0x34)
#define get_bus_clk() ({\
		unsigned long tmp_reg, busclk = 0;\
		tmp_reg = HW_REG(A7_AXI_SCALE_REG);\
		tmp_reg = (tmp_reg >> 12) & 0x3;\
		if (0x1 == tmp_reg) {\
			busclk = 200000000;\
		} \
		busclk;\
		})

/* cpu_init configuration */
#define CFG_CLK_BUS  get_bus_clk() /* For ASIC */

#define CFG_TIMER_PER 4		/* AXI:APB is 4:1 */

#define CFG_TIMER_CLK (3000000)

#define CONFIG_HI3519		1

#define CONFIG_SVB_ENABLE

/*-----------------------------------------------------------------------
 * Flash Memory Configuration v100
 *----------------------------------------------------------------------*/
#define CONFIG_HIFMC

#ifdef CONFIG_HIFMC
	#define CONFIG_HIFMC_NAND

	#define CONFIG_HIFMC_REG_BASE		FMC_REG_BASE
	#define CONFIG_HIFMC_BUFFER_BASE	FMC_MEM_BASE
	#define CONFIG_HIFMC_MAX_CS_NUM		1
#endif

#ifdef CONFIG_HIFMC_NAND
	#define CONFIG_CMD_NAND
	#define CONFIG_ENV_IS_IN_NAND
	#define CONFIG_NAND_MAX_CHIP_NUM	1
	#define CONFIG_SYS_MAX_NAND_DEVICE	CONFIG_NAND_MAX_CHIP_NUM
	#define CONFIG_SYS_NAND_MAX_CHIPS	CONFIG_NAND_MAX_CHIP_NUM
	#define CONFIG_SYS_NAND_BASE		FMC_MEM_BASE
/* #define CONFIG_SYS_NAND_QUIET_TEST */

	#undef CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC
	#define CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	#undef CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE
#endif

/* #define CONFIG_CLOSE_SPI_8PIN_4IO */

/*-----------------------------------------------------------------------
 * for cpu/hi3519/start.S
 *----------------------------------------------------------------------*/
#define FMC_TEXT_ADRS			(FMC_MEM_BASE)

#define MEM_BASE_DDR			(DDR_MEM_BASE)
#define CONFIG_SYS_MALLOC_LEN      (CONFIG_ENV_SIZE + 128*1024)
/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE   128

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)	cpu/hi3519/hi3519/timer.c
 * enable timer				board/hi3519/board.c
 *----------------------------------------------------------------------*/
#define CFG_TIMERBASE           TIMER0_REG_BASE
/* enable timer.32bit, periodic,mask irq, 1 divider.*/
#define CFG_TIMER_CTRL		0xC2
#define READ_TIMER		(*(volatile unsigned long *)\
					(CFG_TIMERBASE + REG_TIMER_VALUE))
/* how many ticks per second. show the precision of timer. */
#define CONFIG_SYS_HZ           (CFG_TIMER_CLK/1)
#define CFG_HZ			CONFIG_SYS_HZ

/*allow change env*/
#define  CONFIG_ENV_OVERWRITE
/*-----------------------------------------------------------------------
 * environment && bd_info  && global_data  configure
 * used in file
 *----------------------------------------------------------------------*/

/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_OFFSET          0x80000      /* environment starts here */
#define CONFIG_ENV_NAND_ADDR		(CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SPI_ADDR        (CONFIG_ENV_OFFSET)
#define CONFIG_CMD_SAVEENV

#define CONFIG_ENV_SIZE            0x40000    /*include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS       1          /* we have 1 bank of DRAM */
/* kernel parameter list phy addr */
#define CFG_BOOT_PARAMS			(MEM_BASE_DDR+0x0100)

/*-----------------------------------------------------------------------
 * eMMC Flash Configuration
 ************************************************************************/
#define CONFIG_EMMC_SUPPORT
#ifdef CONFIG_EMMC_SUPPORT
#  define CONFIG_HIMCI_V200                   1
#endif

#ifdef CONFIG_HIMCI_V200
#  define CONFIG_MMC_DEVID	              1 /* emmc = 1; SD = 0 */
#  define CONFIG_MMC_BOOT_ADDR                0
#  define CONFIG_MMC_POWER_OFF_TIMEOUT        50
#  define CONFIG_MMC_POWER_ON_TIMEROUT        50
#  define CONFIG_MMC_RESET_LOW_TIMEOUT        10
#  define CONFIG_MMC_RESET_HIGH_TIMEROUT      300
#  define CONFIG_GENERIC_MMC
#endif /* CONFIG_HIMCI_V200 */

#ifdef CONFIG_GENERIC_MMC
/* env in flash instead of CFG_ENV_IS_NOWHERE */
#  define CONFIG_ENV_IS_IN_EMMC               1
#  define CONFIG_CMD_MMC
#  define CONFIG_EXT4
#  define CONFIG_EXT4_SPARSE
#endif /* CONFIG_GENERIC_MMC */
/*-----------------------------------------------------------------------
 *  Environment   Configuration
 *-----------------------------------------------------------------------*/
#define CONFIG_BOOTCOMMAND "bootm 0x81000000 0x82000000"
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
 * used in file  board/hi3519/board.c
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
 * HIETH-GMAC driver
 -----------------------------------------------------------------------*/
#define CONFIG_NET_HIGMACV300
#ifdef CONFIG_NET_HIGMACV300
	#define CONFIG_GMAC_NUMS		1
	#define HIGMAC0_IOBASE          0x10050000
/*	#define HIGMAC1_IOBASE          0x10091000	  */
	#define CONFIG_HIGMAC_PHY1_ADDR		1
/*	#define CONFIG_HIGMAC_PHY2_ADDR		2	  */
	#define CONFIG_HIGMAC_PHY1_INTERFACE_MODE	2
/*	#define CONFIG_HIGMAC_PHY2_INTERFACE_MODE	2 */
	#define CONFIG_HIGMAC_DESC_4_WORD
#endif

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
#define CONFIG_SYS_PBSIZE  (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)

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

#undef CONFIG_NANDID_TAG
#undef CONFIG_SPIID_TAG
/* serial driver */
#define CONFIG_PL011_SERIAL 1
#define CONFIG_PL011_CLOCK	(24000000)

#define CONFIG_PL01x_PORTS	\
	{(void *)UART0_REG_BASE, (void *)UART1_REG_BASE, \
	(void *)UART2_REG_BASE, (void *)UART3_REG_BASE}
#define CONFIG_CONS_INDEX	0

#define CFG_SERIAL0                   UART0_REG_BASE

/* according to CONFIG_CONS_INDEX */
#define CONFIG_CUR_UART_BASE          CFG_SERIAL0
#define CONFIG_PRODUCTNAME		"hi3519"

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
 #define CONFIG_AUTO_UPDATE			1
#ifdef CONFIG_AUTO_UPDATE
	/*#define CONFIG_AUTO_SD_UPDATE		1*/
	/*#define CONFIG_AUTO_USB_UPDATE		1*/
#endif

#define CONFIG_DOS_PARTITION			1

#define CONFIG_CMD_FAT				1
/*#define CONFIG_CMD_EXT2				1*/

/*-----------------------------------------------------------------------
 * sdcard
 * ----------------------------------------------------------------------*/
#ifdef CONFIG_AUTO_SD_UPDATE

#ifndef CONFIG_HIMCI_V200
	#define CONFIG_HIMCI_V200
#endif

#ifndef CONFIG_GENERIC_MMC
	#define CONFIG_GENERIC_MMC
	#define CONFIG_CMD_MMC
#endif

	#define CONFIG_MMC			1
#endif


/*-----------------------------------------------------------------------
 * usb
 * ----------------------------------------------------------------------*/
//#define CONFIG_USB_XHCI		1
#define CONFIG_CMD_USB			1
#define CONFIG_USB_OHCI			1 /* FIXME: CONFIG_USB_OHCI_NEW */
#define CONFIG_USB_STORAGE		1
#define CONFIG_LEGACY_USB_INIT_SEQ	1
//#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS 2

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/
#define CONFIG_DDR_TRAINING_V2	1
#define CONFIG_ARCH_HI3519	1

/*#define CONFIG_OSD_ENABLE*/

/*-----------------------------------------------------------------------
 * Snapshot boot support
 * ----------------------------------------------------------------------*/
#define CONFIG_SNAPSHOT_BOOT 1

#ifdef CONFIG_SNAPSHOT_BOOT

/* enable MMU for fast decompress */
//#define CONFIG_ARCH_MMU

#define CONFIG_LZO
#define CONFIG_SHA1
#define CONFIG_HW_DEC

#endif

#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET

/* #define CONFIG_USE_IRQ */

/*#define CONFIG_AUDIO_ENABLE*/

#endif	/* __CONFIG_H */
