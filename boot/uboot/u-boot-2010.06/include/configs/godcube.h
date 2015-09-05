
/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the godcube board.
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

#ifndef __CONFIG_GODCUBE_H__
#define __CONFIG_GODCUBE_H__

#include <asm/arch/platform.h>

/* Arm configuration*/
#define CONFIG_L2_OFF

/* bus clk configuration */
/*
 * APLL : ARM9 : DDR : AXI(AHB) : APB
 *  8   :  4   :  4  :	2       : 1
 */
#define HW_REG(a) (*(unsigned long *)(a))
#define get_bus_clk()({\
	unsigned long fbdiv, refdiv; \
	unsigned long tmp_reg, foutvco, busclk;\
	tmp_reg = HW_REG(REG_CRG1_OFFSET);\
	refdiv = (tmp_reg >> 12) & 0x3f;\
	fbdiv = tmp_reg & 0xfff;\
	foutvco = 24000000/refdiv;\
	foutvco *= fbdiv;\
	busclk = foutvco/4;\
	busclk;\
})

/*
 * default APPL	      = 24M * 0x9B/ 0x3 = 1240M
 *	   AXI busclk = APPL / 4 = 310M
 */

#define CFG_CLK_BUS				get_bus_clk()
#define CFG_TIMER_PER				2 /* AXI:APB = 2:1 */
#define CFG_TIMER_CLK				(CFG_CLK_BUS/CFG_TIMER_PER)

/*-----------------------------------------------------------------------
 * SPI Flash Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_CMD_SF		/*sf read \ sf write \ sf erase */
#define CONFIG_HISFC_FLASH_GODCUBE		1

#define CONFIG_HISFC300_BUFFER_BASE_ADDRESS	SFC_MEM_BASE
#define CONFIG_HISFC300_REG_BASE_ADDRESS	SFC_REG_BASE
#define CONFIG_HISFC300_PERIPHERY_REGBASE	CRG_REG_BASE
#define CONFIG_HISFC300_CHIP_NUM		2

/*-----------------------------------------------------------------------
 * NAND Flash Configuration
 -----------------------------------------------------------------------*/
/* no nand flash in this board */

/* no nor flash */
#define CONFIG_SYS_NO_FLASH
/* cp.b */
#define CONFIG_CMD_MEMORY  /*md mw cp etc.*/

/*-----------------------------------------------------------------------
 * for cpu/godcube/start.S
 -----------------------------------------------------------------------*/
#define SF_TEXT_ADRS			(SFC_MEM_BASE)
#define MEM_BASE_DDR			(DDR_MEM_BASE)
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128*1024)
/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)
 * enable timer
 -----------------------------------------------------------------------*/
#define CFG_TIMERBASE           TIMER0_REG_BASE
/* enable timer.32bit, periodic,mask irq,256 divider. */
#define CFG_TIMER_CTRL          0xCA
#define READ_TIMER		(*(volatile unsigned long *)\
					(CFG_TIMERBASE + REG_TIMER_VALUE))
/* how many ticks per second. show the precision of timer. */
#define CONFIG_SYS_HZ           (CFG_TIMER_CLK/256)
#define CFG_HZ			CONFIG_SYS_HZ

/*allow change env*/
#define  CONFIG_ENV_OVERWRITE
/*-----------------------------------------------------------------------
 * environment && bd_info && global_data  configure
 * used in file
-----------------------------------------------------------------------*/
#include "../../product/env_setup.h"

#define CFG_ENV_OFFSET			CFG_ENV_ADDR

/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_IS_IN_SPI_FLASH	1

#define CONFIG_ENV_OFFSET		0x80000 /* environment starts here  */
#define CONFIG_ENV_NAND_ADDR		(CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SPI_ADDR		(CONFIG_ENV_OFFSET)
#define CONFIG_CMD_SAVEENV

#define CONFIG_STACKSIZE		(128 * 1024)
#define CONFIG_ENV_SIZE			0x40000    /*include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE		CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS		1          /* we have 1 bank of DRAM */
#define CFG_BOOT_PARAMS			(MEM_BASE_DDR + 0x0100)

/*-----------------------------------------------------------------------
 *  Environment   Configuration
 ------------------------------------------------------------------------*/
/* for pcie slave mode only */
#define CONFIG_BOOTCOMMAND "bootm 0x81000000 0x82000000"
#define CONFIG_BOOTDELAY    1
#define CONFIG_BOOTARGS "mem=64M console=ttyAMA0,115200"
#define CONFIG_NETMASK  255.255.255.0       /* talk on MY local net */
#define CONFIG_IPADDR   192.168.1.10        /* static IP I currently own */
#define CONFIG_SERVERIP 192.168.1.2	    /* current IP of tftp server ip */
#define CONFIG_ETHADDR  00:00:23:34:45:66
#define CONFIG_BOOTFILE "uImage"            /* file to load */
#define CONFIG_BAUDRATE 115200

/*-----------------------------------------------------------------------
 * for bootm linux
 -----------------------------------------------------------------------*/
#define CONFIG_BOOTM_LINUX		1 /* for bootm */
/* default load address 0x80008000*/
#define CONFIG_SYS_LOAD_ADDR		(CFG_DDR_PHYS_OFFSET + 0x08000)
#define CONFIG_ZERO_BOOTDELAY_CHECK	1 /*use ^c to interrupt*/

/*-----------------------------------------------------------------------
 * for  commond configure
 -----------------------------------------------------------------------*/
/* tftp comm */
#define CONFIG_TFTP_TSIZE

/* do_printenv  do_setenv common/cmd_nvedit.c */
#define CONFIG_SYS_BAUDRATE_TABLE { 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_MAXARGS	  32 /* max number of command args */

#define CONFIG_CMD_LOADB	  /* loadb common/cmd_load.c */

/*-----------------------------------------------------------------------
 * network config
 -----------------------------------------------------------------------*/
#define CONFIG_ARP_TIMEOUT		50000UL/* FIXME */
#define CONFIG_NET_RETRY_COUNT		50/* FIXME */
#define CONFIG_CMD_NET			/* do_ping common/cmd_net.c */

#define CONFIG_CMD_PING
#define CONFIG_CMD_MII
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	1

/*-----------------------------------------------------------------------
 * HIETH driver
 -----------------------------------------------------------------------*/
/* default is hieth-switch-fabric */
#define CONFIG_NET_HISFV300
#ifdef CONFIG_NET_HISFV300
	#define CONFIG_NET_HISFV300_GODCUBE 1
	#define HISFV_MII_MODE		0
	#define HISFV_RMII_MODE		1
	#define HIETH_MII_RMII_MODE_U   HISFV_MII_MODE
	#define HIETH_MII_RMII_MODE_D   HISFV_MII_MODE
	#define HISFV_PHY_U		1
	#define HISFV_PHY_D		0
	#undef HISFV_RESET_GPIO_EN /* godcube don't use it */
	#ifdef HISFV_RESET_GPIO_EN
		/* use gpio9_6 to control sfv reset */
		#define HISFV_RESET_GPIO_BASE REG_BASE_GPIO9
		#define HISFV_RESET_GPIO_DIR 0x400
		#define HISFV_RESET_GPIO_BIT 6
		#define HISFV_RESET_GPIO_DIR_OUT 1
		/* 0-reset; 1-undo reset */
		#define HISFV_RESET_GPIO_DATA 0
	#endif /* HISFV_RESET_GPIO_EN */
#endif /* CONFIG_NET_HISFV300 */

/*-----------------------------------------------------------------------
 * console display  Configuration
 ------------------------------------------------------------------------*/
#define CONFIG_VERSION_VARIABLE	1 /*used in common/main.c*/
#define CONFIG_SYS_PROMPT	"hisilicon # " /* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	1024           /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)

#define CFG_BARGSIZE		CFG_CBSIZE  /* Boot Argument Buffer Size */
#undef  CFG_CLKS_IN_HZ          /* everything, incl board info, in Hz */
#define CFG_LOAD_ADDR		(CFG_DDR_PHYS_OFFSET + 0x08000)
				/* default load address */
#define CONFIG_AUTO_COMPLETE		1
#define CFG_CMDLINE_HISTORYS		8
#define CONFIG_CMDLINE_EDITING
#define CFG_DDR_PHYS_OFFSET		MEM_BASE_DDR
#define CFG_DDR_SIZE			(1024 * 1024 * 1024)/* 1G bit */

#define CONFIG_SYS_MEMTEST_START       (CFG_DDR_PHYS_OFFSET +\
					sizeof(unsigned long))
#define CONFIG_SYS_MEMTEST_END         (CFG_DDR_PHYS_OFFSET + CFG_DDR_SIZE)
#define CONFIG_SYS_MEMTEST_SCRATCH     CFG_DDR_PHYS_OFFSET

#define CONFIG_CMDLINE_TAG		1 /* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG		1 /* support initrd */
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_MISC_INIT_R		1 /* call misc_init_r during start up */

#define CONFIG_ETHADDR_TAG		1
#define CONFIG_ETHADDR_TAG_VAL		0x726d6d73

#undef CONFIG_NANDID_TAG
#undef CONFIG_SPIID_TAG

/* serial driver */
#define CONFIG_PL011_SERIAL		1
#define UART_CKSEL_APB (1 << 13)
#define CONFIG_PL011_CLOCK  (CFG_CLK_BUS/2)
#define CONFIG_PL01x_PORTS		{(void *)UART0_REG_BASE}
#define CONFIG_CONS_INDEX		0

#define CONFIG_PRODUCTNAME              "godcube"

#endif	/* __CONFIG_H */
