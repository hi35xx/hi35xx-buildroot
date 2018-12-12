
/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the hi3520a board.
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

#ifndef __CONFIG_GODARM_H__
#define __CONFIG_GODARM_H__

#include <asm/arch/platform.h>

/* Arm configuration*/
#define CONFIG_L2_OFF


/* cpu_init configuration */
#define HW_REG(a) (*(unsigned long *)(a))
#define get_bus_clk()({\
	unsigned long fbdiv, pstdiv1, pstdiv2, refdiv; \
	unsigned long tmp_reg, foutvco, foutpostdiv, busclk;\
	tmp_reg = HW_REG(REG_CRG0_OFFSET);\
	pstdiv1 = (tmp_reg >> 24) & 0x7;\
	pstdiv2 = (tmp_reg >> 27) & 0x7;\
	tmp_reg = HW_REG(REG_CRG1_OFFSET);\
	refdiv = (tmp_reg >> 12) & 0x3f;\
	fbdiv = tmp_reg & 0xfff;\
	foutvco = 24000000/refdiv;\
	foutvco *= fbdiv;\
	foutpostdiv = foutvco/(pstdiv1 * pstdiv2);\
	busclk = foutpostdiv/6;\
	busclk;\
})

/* cpu_init configuration */
#define CFG_CLK_BUS  get_bus_clk()
#define CFG_TIMER_PER 2
#define CFG_TIMER_CLK (CFG_CLK_BUS/CFG_TIMER_PER)

/*-----------------------------------------------------------------------
 * SPI Flash Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_HISFC_FLASH_GODARM 1
#define CONFIG_CMD_SF	  /*sf read \ sf write \ sf erase */
#define CONFIG_HISFC300_BUFFER_BASE_ADDRESS SFC_MEM_BASE
#define CONFIG_HISFC300_REG_BASE_ADDRESS SFC_REG_BASE
#define CONFIG_HISFC300_PERIPHERY_REGBASE CRG_REG_BASE
#define CONFIG_HISFC300_CHIP_NUM 2
/*-----------------------------------------------------------------------
 * for cpu/arm_cortexa9/start.S
 -----------------------------------------------------------------------*/
#define NAND_TEXT_ADRS      (CONFIG_SYS_NAND_BASE)
#define SF_TEXT_ADRS        (SFC_MEM_BASE)
#define MEM_BASE_DDR	    (DDR_MEM_BASE)
#define CONFIG_SYS_MALLOC_LEN      (CONFIG_ENV_SIZE + 128*1024)
/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE   128

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)	cpu/godarm/godarm/timer.c
*  enable timer				board/godarm/board.c
 -----------------------------------------------------------------------*/
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
*     used in file
-----------------------------------------------------------------------*/
#include "../../product/env_setup.h"
/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_IS_IN_NAND     1
/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_IS_IN_SPI_FLASH     1
#define CONFIG_ENV_OFFSET          0x80000      /* environment starts here */
#define CONFIG_ENV_NAND_ADDR       (CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SPI_ADDR        (CONFIG_ENV_OFFSET)
#define CONFIG_CMD_SAVEENV

#define CONFIG_ENV_SIZE            0x40000    /*include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS       1          /* we have 1 bank of DRAM */
/* kernel parameter list phy addr */
#define CFG_BOOT_PARAMS			(MEM_BASE_DDR+0x0100)

/*-----------------------------------------------------------------------
 *  Environment   Configuration
 ------------------------------------------------------------------------*/
#define CONFIG_BOOTDELAY	1
#define CONFIG_BOOTARGS "mem=96M console=ttyAMA0,115200"
#define CONFIG_NETMASK  255.255.254.0       /* talk on MY local net */
#define CONFIG_IPADDR   192.168.1.10        /* static IP I currently own */
#define CONFIG_SERVERIP 192.168.1.2     /* current IP of tftp server ip */
#define CONFIG_ETHADDR  00:00:23:34:45:66
#define CONFIG_BOOTFILE "uImage"        /* file to load */
#define CONFIG_BAUDRATE         115200
/*-----------------------------------------------------------------------
 * for bootm linux
*  used in file  board/godarm/board.c
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
 * STMMAC driver
 -----------------------------------------------------------------------*/
#define CONFIG_NET_STMMAC
#define CONFIG_GODARM
#define CONFIG_TNK
#ifdef CONFIG_NET_STMMAC
	#define STMMAC_SINGLE_MAC
	#define STMMAC_GMACADDR		(0x101c0000)
	#define STMMAC_DMAADDR		(0x101c1000)
	#define STMMAC_IOSIZE		(0x10000)
	#define STMMAC_FRQDIV		(0)
	#define STMMAC_PHYADDR0		(1)
	#define STMMAC_PHYADDR1		(2)
	#define STMMAC_PHYNAME		"0:01"
	#define STMMAC_RGMII
	#define CONFIG_PHY_GIGE
#endif /* CONFIG_NET_STMMAC */

/* nand flash */
#define CONFIG_SYS_64BIT_VSPRINTF
#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BASE NAND_MEM_BASE
#define CONFIG_NAND_GODARM 1
#define CFG_MAX_NAND_DEVICE CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_SYS_MAX_NAND_DEVICE 1
#define CONFIG_SYS_NAND_MAX_CHIPS 2

#define CONFIG_HINFC301_MAX_CHIP		CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_HINFC301_REG_BASE_ADDRESS	NANDC_REG_BASE
#define CONFIG_HINFC301_BUFFER_BASE_ADDRESS	NAND_MEM_BASE
#define CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC
#define CONFIG_HINFC301_W_LATCH			(0xa)
#define CONFIG_HINFC301_R_LATCH			(0xa)
#define CONFIG_HINFC301_RW_LATCH		(0xa)

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
#define CFG_DDR_SIZE		(256*1024*1024)

#define CONFIG_SYS_MEMTEST_START       \
	(CFG_DDR_PHYS_OFFSET + \
	sizeof(unsigned long))
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
#define UART_CKSEL_APB (1 << 13)
#define CONFIG_PL011_CLOCK  (CFG_CLK_BUS/2)
#define CONFIG_PL01x_PORTS	\
	{(void *)UART0_REG_BASE, (void *)UART1_REG_BASE, \
	(void *)UART2_REG_BASE, (void *)UART3_REG_BASE}
#define CONFIG_CONS_INDEX	0

#define CONFIG_PRODUCTNAME		"godarm"

/*-----------------------------------------------------------------------
 * bootrom Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_BOOTROM_SUPPORT
#if defined(CONFIG_BOOTROM_SUPPORT) \
	&& (!defined(REG_START_FLAG) || !defined(CONFIG_START_MAGIC))
#  error Please define CONFIG_START_MAGIC and CONFIG_START_MAGIC first
#endif

/*-----------------------------------------------------------------------
 * sdcard/usb storage system update
 * ----------------------------------------------------------------------*/
/* #define CONFIG_AUTO_UPDATE			1 */
#ifdef CONFIG_AUTO_UPDATE
	#define CONFIG_AUTO_SD_UPDATE		1
	#define CONFIG_AUTO_USB_UPDATE		1
#endif

#define __LITTLE_ENDIAN				1
#define CONFIG_DOS_PARTITION			1

#define CONFIG_CMD_FAT				1
#define CONFIG_CMD_EXT2				1

/*-----------------------------------------------------------------------
 * sdcard
 * ----------------------------------------------------------------------*/
#ifdef CONFIG_AUTO_SD_UPDATE
	#define CONFIG_MMC			1
	#define CONFIG_CMD_MMC
	#define CONFIG_GENERIC_MMC
	#define CONFIG_HIMCI_V100

	#define REG_BASE_MCI			0x10020000

	#define CONFIG_HIMCI_GODARM
#endif

/*-----------------------------------------------------------------------
 * usb
 * ----------------------------------------------------------------------*/
#define CONFIG_CMD_USB				1
#define CONFIG_USB_OHCI				1/* FIXME:CONFIG_USB_OHCI_NEW */

#define CONFIG_USB_STORAGE			1
#define CONFIG_LEGACY_USB_INIT_SEQ

/*-----------------------------------------------------------------------
  * ddrt Configuration
  -----------------------------------------------------------------------*/
/*#define CONFIG_DDR_TRAINING*/

#ifdef CONFIG_DDR_TRAINING
#define REG_DDR0_BASE 0x20110000
#define SSN_DDR0_BASE 0x83000000
#endif

#define CONFIG_OSD_ENABLE

#endif	/* __CONFIG_H */

