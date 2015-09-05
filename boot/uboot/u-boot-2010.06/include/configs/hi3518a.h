
/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the hi3518 board.
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

#ifndef __CONFIG_HI3518_H__
#define __CONFIG_HI3518_H__

#include <asm/arch/platform.h>

/* Arm configuration */
#define CONFIG_L2_OFF

/*
 * Get frequency of bus clock.
 * Clock for arm9 is supplied by APLL.
 * Clock for DDR/AXI/APB bus is supplied by BPLL.
 * BPLL : DDR : AXI : APB(AHB)
 *    x :  4  :	2   : 1
 */
#define HW_REG(a) (*(unsigned long *)(a))
#define get_bus_clk()({\
	unsigned long fbdiv, refdiv, pstdiv1, pstdiv2; \
	unsigned long tmp_reg, foutvco, busclk;\
	tmp_reg = HW_REG(REG_CRG4_OFFSET);\
	pstdiv1 = (tmp_reg >> 24) & 0x7;\
	pstdiv2 = (tmp_reg >> 27) & 0x7;\
	tmp_reg = HW_REG(REG_CRG5_OFFSET);\
	refdiv = (tmp_reg >> 12) & 0x3f;\
	fbdiv = tmp_reg & 0xfff;\
	foutvco = 24000000/refdiv;\
	foutvco *= fbdiv;\
	busclk = foutvco/(2 * pstdiv1 * pstdiv2);\
	busclk;\
})

#define CFG_CLK_BUS		get_bus_clk()
#define CFG_TIMER_PER		2	/* AXI:APB = 2:1 */
#define CFG_TIMER_CLK		(CFG_CLK_BUS/CFG_TIMER_PER)

#define CONFIG_HI3518		1

/*-----------------------------------------------------------------------
 * SPI Flash Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_CMD_SF				/* sf read\sf write\sf erase */
#define CONFIG_SPI_FLASH_HISFC350		1

#ifdef CONFIG_SPI_FLASH_HISFC350
	#define CONFIG_HISFC350_BUFFER_BASE_ADDRESS	SFC_MEM_BASE
	#define CONFIG_HISFC350_REG_BASE_ADDRESS	SFC_REG_BASE
	#define CONFIG_HISFC350_PERIPHERY_REGBASE	CRG_REG_BASE
	#define CONFIG_HISFC350_CHIP_NUM		2
	#define SFC_ADDR_MODE_REG			0x8c

#endif /* CONFIG_SPI_FLASH_HISFC350 */

/*-----------------------------------------------------------------------
 * NAND Flash Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_SYS_64BIT_VSPRINTF
#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BASE NAND_MEM_BASE
#define CONFIG_NAND_3518			1
#define CFG_MAX_NAND_DEVICE			CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define CONFIG_SYS_NAND_MAX_CHIPS		2
#define CONFIG_ENV_IS_IN_NAND			1

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
#define CONFIG_CMD_MEMORY	/*md mw cp etc.*/

/*-----------------------------------------------------------------------
 * for cpu/hi3518/start.S
 -----------------------------------------------------------------------*/
#define SF_TEXT_ADRS			(SFC_MEM_BASE)
#define NAND_TEXT_ADRS			(NAND_MEM_BASE)
#define MEM_BASE_DDR			(DDR_MEM_BASE)
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128*1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)
 * enable timer
 -----------------------------------------------------------------------*/
#define CFG_TIMERBASE		TIMER0_REG_BASE
/* enable timer.32bit, periodic,mask irq,256 divider. */
#define CFG_TIMER_CTRL		0xCA
#define READ_TIMER		(*(volatile unsigned long *) \
					(CFG_TIMERBASE + REG_TIMER_VALUE))
/* how many ticks per second. show the precision of timer. */
#define CONFIG_SYS_HZ		(CFG_TIMER_CLK/256)
#define CFG_HZ			CONFIG_SYS_HZ

/* allow change env */
#define CONFIG_ENV_OVERWRITE
/*-----------------------------------------------------------------------
 * environment && bd_info && global_data  configure
 * used in file
-----------------------------------------------------------------------*/
#include "../../product/env_setup.h"

#define CFG_ENV_OFFSET			CFG_ENV_ADDR

/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_IS_IN_SPI_FLASH	1

#define CONFIG_ENV_OFFSET		0x80000 /* environment starts here */
#define CONFIG_ENV_NAND_ADDR		(CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SPI_ADDR		(CONFIG_ENV_OFFSET)
#define CONFIG_CMD_SAVEENV

#define CONFIG_STACKSIZE		(128 * 1024)
#define CONFIG_ENV_SIZE			0x40000 /* include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE		CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS		1	/* we have 1 bank of DRAM */
#define CFG_BOOT_PARAMS			(MEM_BASE_DDR + 0x0100)

/*-----------------------------------------------------------------------
 *  Environment   Configuration
 ------------------------------------------------------------------------*/
#define CONFIG_BOOTCOMMAND "bootm 0x82000000"

#define CONFIG_BOOTDELAY 1
#define CONFIG_BOOTARGS	"mem=64M console=ttyAMA0,115200"
#define CONFIG_NETMASK	255.255.255.0		/* talk on MY local net */
#define CONFIG_IPADDR	192.168.1.10		/* default static IP */
#define CONFIG_SERVERIP	192.168.1.2		/* default tftp server ip */
#define CONFIG_ETHADDR	00:00:23:34:45:66
#define CONFIG_BOOTFILE	"uImage"		/* file to load */
#define CONFIG_BAUDRATE	115200

/*-----------------------------------------------------------------------
 * for bootm linux
 -----------------------------------------------------------------------*/
#define CONFIG_BOOTM_LINUX		1	/* for bootm */
/* default load address 0x80008000 */
#define CONFIG_SYS_LOAD_ADDR		(CFG_DDR_PHYS_OFFSET + 0x08000)
#define CONFIG_ZERO_BOOTDELAY_CHECK	1	/*use ^c to interrupt*/

/*-----------------------------------------------------------------------
 * for common configuration
 -----------------------------------------------------------------------*/
/* tftp comm */
#define CONFIG_TFTP_TSIZE

/* do_printenv do_setenv common/cmd_nvedit.c */
#define CONFIG_SYS_BAUDRATE_TABLE	{9600, 19200, 38400, 57600, 115200}
#define CONFIG_SYS_MAXARGS		16 /* max number of command args */

#define CONFIG_CMD_LOADB		/* loadb common/cmd_load.c */

/*-----------------------------------------------------------------------
 * network config
 -----------------------------------------------------------------------*/
#define CONFIG_ARP_TIMEOUT		50000UL	/* FIXME */
#define CONFIG_NET_RETRY_COUNT		50	/* FIXME */
#define CONFIG_CMD_NET				/* do_ping common/cmd_net.c */

#define CONFIG_CMD_PING
#define CONFIG_CMD_MII
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	1

/*-----------------------------------------------------------------------
 * HIETH driver
 -----------------------------------------------------------------------*/
/* default is hieth-switch-fabric */
#define CONFIG_NET_HISFV300
#ifdef CONFIG_NET_HISFV300
	#define CONFIG_NET_HISFV300_3518	1
	#define HISFV_MII_MODE			0
	#define HISFV_RMII_MODE			1
	#define HIETH_MII_RMII_MODE_U		HISFV_MII_MODE
	#define HIETH_MII_RMII_MODE_D		HISFV_MII_MODE
	#define HISFV_PHY_U			1
	#define HISFV_PHY_D			2 /* fix me */
	#undef HISFV_RESET_GPIO_EN
	#ifdef HISFV_RESET_GPIO_EN
		/* use gpio5_0 to control sfv reset */
		#define HISFV_RESET_GPIO_BASE	GPIO0_REG_BASE
		#define HISFV_RESET_GPIO_DIR	0x400
		#define HISFV_RESET_GPIO_BIT	5
		#define HISFV_RESET_GPIO_DIR_OUT 1
		/* 0-reset; 1-undo reset */
		#define HISFV_RESET_GPIO_DATA	0
	#endif /* HISFV_RESET_GPIO_EN */
#endif /* CONFIG_NET_HISFV300 */

/*-----------------------------------------------------------------------
 * console display  Configuration
 ------------------------------------------------------------------------*/
#define CONFIG_VERSION_VARIABLE	1		/* used in common/main.c */
#define CONFIG_SYS_PROMPT	"hisilicon # "	/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)

#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#undef  CFG_CLKS_IN_HZ          /* everything, incl board info, in Hz */
#define CFG_LOAD_ADDR		(CFG_DDR_PHYS_OFFSET + 0x08000)
				/* default load address */
#define CONFIG_AUTO_COMPLETE		1
#define CFG_CMDLINE_HISTORYS		8
#define CONFIG_CMDLINE_EDITING
#define CFG_DDR_PHYS_OFFSET		MEM_BASE_DDR
#define CFG_DDR_SIZE			(256 * 1024 * 1024)	/* 256M Bytes */

#define CONFIG_SYS_MEMTEST_START	(CFG_DDR_PHYS_OFFSET +\
						sizeof(unsigned long))
#define CONFIG_SYS_MEMTEST_END		(CFG_DDR_PHYS_OFFSET + CFG_DDR_SIZE)
#define CONFIG_SYS_MEMTEST_SCRATCH	CFG_DDR_PHYS_OFFSET

#define CONFIG_CMDLINE_TAG		1 /* enable passing of ATAGs */
#define CONFIG_INITRD_TAG		1 /* support initrd */
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_MISC_INIT_R		1 /* call misc_init_r during start up */

#define CONFIG_ETHADDR_TAG		1
#define CONFIG_ETHADDR_TAG_VAL		0x726d6d73

#undef CONFIG_NANDID_TAG
#undef CONFIG_SPIID_TAG

/* serial driver */
#define CONFIG_PL011_SERIAL		1
#define CONFIG_PL011_CLOCK		3000000 /* hi3518 use fixed clk 3MHZ */

#define CONFIG_PL01x_PORTS		{(void *)UART0_REG_BASE}
#define CONFIG_CONS_INDEX		0

#define CONFIG_PRODUCTNAME              "hi3518a"

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
	#define CONFIG_HIMCI_HI3518
	#define REG_BASE_MCI			0x10020000
	#define CONFIG_HIMCI_V100
	#define CONFIG_GENERIC_MMC
	#define CONFIG_MMC			1
	#define CONFIG_CMD_MMC
#endif


/*-----------------------------------------------------------------------
 * usb
 * ----------------------------------------------------------------------*/
#define CONFIG_USB_OHCI				1
#define CONFIG_CMD_USB				1
#define CONFIG_USB_STORAGE			1
#define CONFIG_LEGACY_USB_INIT_SEQ

/*-----------------------------------------------------------------------
 * SVB
 * ----------------------------------------------------------------------*/
/* #define CONFIG_SVB_ENABLE */

#endif	/* __CONFIG_H */
