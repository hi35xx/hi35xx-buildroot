/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the hi3536 board.
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

#ifndef __CONFIG_HI3536_H__
#define __CONFIG_HI3536_H__

#include <asm/arch/platform.h>

/* cpu_init configuration */
#define HW_REG(a) (*(unsigned long *)(a))
#define A7_AXI_SCALE_REG   REG_CRG12_OFFSET
#define get_bus_clk()({\
		unsigned long tmp_reg, busclk = 0;\
		tmp_reg = HW_REG(A7_AXI_SCALE_REG);\
		tmp_reg = tmp_reg & 3;\
		if (tmp_reg  == 0x1)\
			busclk = 300000000;\
		else if (tmp_reg == 0x2)\
			busclk = 250000000;\
		else if (tmp_reg == 0x3)\
			busclk = 200000000;\
		busclk;\
		})

/* cpu_init configuration */
#define CFG_CLK_BUS  get_bus_clk()

#define CFG_TIMER_PER 2		/* PERI_AXI:APB is 2:1 */
#define CFG_TIMER_CLK (CFG_CLK_BUS / CFG_TIMER_PER)

#define CONFIG_HI3536		1

#define CONFIG_DDR_CHARGE_SHIFT		28
#define CONFIG_DDR_START_ADDR		0x8

/*-----------------------------------------------------------------------
 * for cpu/hi3536/start.S
 *----------------------------------------------------------------------*/
#define MEM_BASE_DDR		(0x80000000)
#define CONFIG_SYS_MALLOC_LEN      (CONFIG_ENV_SIZE + 128*1024)

/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)	cpu/hi3536/hi3536/timer.c
 *  enable timer			board/hi3536/board.c
 *----------------------------------------------------------------------*/
/* timer0/timer1 for A17, timer2/timer3 for A7 */
#define CFG_TIMERBASE           TIMER1_REG_BASE

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

#define CONFIG_ENV_IS_NOWHERE		1
/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_OFFSET          0x80000      /* environment starts here */
#define CONFIG_ENV_SPI_ADDR        (CONFIG_ENV_OFFSET)
#define CONFIG_ENV_NAND_ADDR       (CONFIG_ENV_OFFSET)
#define CONFIG_CMD_SAVEENV

#define CONFIG_ENV_SIZE            0x40000   /*include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS       1          /* we have 1 bank of DRAM */

/* kernel parameter list phy addr */
#define CFG_BOOT_PARAMS			(MEM_BASE_DDR+0x0100)

/*-----------------------------------------------------------------------
 *  Environment   Configuration
 ------------------------------------------------------------------------*/
/* only for pcie slave */
#define CONFIG_BOOTCOMMAND "bootm 0x82000000 0x83000000"

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
 *  used in file  board/hi3536/board.c
 -----------------------------------------------------------------------*/
#define CONFIG_BOOTM_LINUX		1

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
#define CONFIG_TNK

#ifdef CONFIG_NET_STMMAC
	#define STMMAC_GMACADDR		(0x11020000)
	#define STMMAC_DMAADDR		(0x11021000)
	#define STMMAC_IOSIZE		(0x10000)
	#define STMMAC_FRQDIV		(0)
	#define STMMAC_PHYADDR0		(1)
	#define STMMAC_PHYADDR1		(2)
	#define STMMAC_PHYNAME		"0:01"
	#define STMMAC_RGMII
	#define CONFIG_PHY_GIGE
	#define CONFIG_MAC0_PHY_GPIO_RESET
	#define CONFIG_MAC0_PHY_RESET_GPIO_BASE (0x121B0000)
	#define CONFIG_MAC0_PHY_RESET_GPIO_BIT  (5)
	#define CONFIG_MAC0_PHY_RESET_GPIO_DATA (1)
	#define CONFIG_MAC1_PHY_GPIO_RESET
	#define CONFIG_MAC1_PHY_RESET_GPIO_BASE (0x121B0000)
	#define CONFIG_MAC1_PHY_RESET_GPIO_BIT  (5)
	#define CONFIG_MAC1_PHY_RESET_GPIO_DATA (1)
#endif /* CONFIG_NET_STMMAC */

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
/* #define CFG_DDR_SIZE		(2*1024*1024*1024) */
/* #define CFG_DDR_SIZE		(3*1024*1024*1024) */  /* 3G mem  */
/* 1GBytes mem for A7 */
#define CFG_DDR_SIZE		(512*1024*1024UL)

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

/* due to bootrom, hi3536 use APB bus clock
 * UART is on APB, and PERI_AXI : APB is 2 : 1, so we set
 * UART clock 1/2 PERI_AXI bus_clk, but default is 2MHz.
 */
#define CONFIG_PL011_CLOCK	(CFG_CLK_BUS / 2)

#define CONFIG_PL01x_PORTS	\
{(void *)UART0_REG_BASE, (void *)UART1_REG_BASE, \
	(void *)UART2_REG_BASE, (void *)UART3_REG_BASE}

/* #define CONFIG_CONS_INDEX	0 */	/* uart0 for A17  */
#define CONFIG_CONS_INDEX	2	/* uart2 for A7 uart2 for A7  */

#define CFG_SERIAL0                   UART0_REG_BASE

/* accoding to CONFIG_CONS_INDEX */
#define CONFIG_CUR_UART_BASE          CFG_SERIAL0

#define CONFIG_PRODUCTNAME		"hi3536"

#define CONFIG_ARCH_HI3536
#define CONFIG_OSD_ENABLE
#define CONFIG_SVB_ENABLE

#define CONFIG_HI3536_A7
#endif	/* __CONFIG_H */
