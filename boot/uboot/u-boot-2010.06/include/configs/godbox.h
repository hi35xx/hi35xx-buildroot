/*
 * (C) Copyright 2009, HISILICON
 * Configuation settings for the godbox board.
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

#include <asm/arch/platform.h>

#ifndef __CONFIG_GODBOX_H__
#define __CONFIG_GODBOX_H__

/* Arm configuration*/
#define CONFIG_L2_OFF  
/* cpu_init configuration */

#define HW_REG(a) *(volatile unsigned long *)(a)

#define get_bus_clk() ({\
        unsigned long fbdiv,pstdiv1,pstdiv2,refdiv; \
        unsigned long reg_base,tmp_reg,foutvco,busclk;\
        reg_base = REG_BASE_SCTL;\
        tmp_reg = HW_REG(reg_base + REG_SC_APLLFREQCTRL0);\
        pstdiv1 = (tmp_reg >> 24) & 0x7;\
        pstdiv2 = (tmp_reg >> 27) & 0x7;\
        tmp_reg = HW_REG(reg_base + REG_SC_APLLFREQCTRL1);\
        refdiv = (tmp_reg >> 12) & 0x3f;\
        fbdiv = tmp_reg & 0xfff;\
        foutvco = 24000000/refdiv;\
        foutvco *= fbdiv;\
        busclk = foutvco/(pstdiv1*pstdiv2);\
        /*cpu:busclk = 2:1 (3:2) */ \
        tmp_reg = HW_REG(reg_base);\
        tmp_reg = (tmp_reg >> 12) & 0x3;\
        busclk >>= 1;\
        if (tmp_reg == 0x10 || tmp_reg == 0x01)\
        {\
                busclk = 0;\
        }\
        busclk;\
})

#define CFG_CLK_BUS get_bus_clk()
#define CFG_TIMER_PER 4
#define CFG_TIMER_CLK ((get_bus_clk())/CFG_TIMER_PER)

/*-----------------------------------------------------------------------
 * for cpu/arm_cortexa9/start.S 
 -----------------------------------------------------------------------*/
#define ROM_TEXT_ADRS       (MEM_BASE_FLASH)
#define NAND_TEXT_ADRS      (MEM_BASE_NAND)
#define SF_TEXT_ADRS        (MEM_BASE_SPI)

#define CONFIG_SYS_MALLOC_LEN      (CONFIG_ENV_SIZE + 128*1024)
#define CONFIG_SYS_GBL_DATA_SIZE   128     /* size in bytes reserved for initial data */

/*-----------------------------------------------------------------------
 * for timer configuration (udelay)
*  enable timer
 -----------------------------------------------------------------------*/
#define CFG_TIMERBASE           REG_BASE_TIMER01
#define CFG_TIMER_CTRL          0xCA 
#define READ_TIMER      	(*(volatile unsigned long *)(CFG_TIMERBASE + REG_TIMER_VALUE))
#define CONFIG_SYS_HZ           (CFG_TIMER_CLK/256)
#define CFG_HZ                  CONFIG_SYS_HZ
/*-----------------------------------------------------------------------
 * environment && bd_info  && global_data  configure
*     used in file
-----------------------------------------------------------------------*/
#include "../../product/env_setup.h"
#ifndef CFG_ENV_ADDR
#  error CFG_ENV_ADDR NOT defined, Please define CFG_ENV_ADDR value in "product/env_setup.h"
#endif
#ifndef CFG_ENV_SIZE
#  error CFG_ENV_SIZE NOT defined, Please define CFG_ENV_SIZE value in "product/env_setup.h"
#endif
#define CFG_ENV_OFFSET			CFG_ENV_ADDR
#define CONFIG_ENV_IS_IN_NAND		1/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_IS_IN_SPI_FLASH	1/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_IS_IN_EMMC		1/* env in flash instead of CFG_ENV_IS_NOWHERE */
#define CONFIG_ENV_OFFSET		CFG_ENV_OFFSET/* environment starts here  */
#define CONFIG_ENV_NAND_ADDR		CFG_ENV_ADDR
#define CONFIG_ENV_SPI_ADDR		CFG_ENV_ADDR
#define CONFIG_ENV_ADDR		CFG_ENV_ADDR
#define CONFIG_CMD_SAVEENV
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_SIZE		CFG_ENV_SIZE/*include ENV_HEADER_SIZE */
#define CONFIG_ENV_SECT_SIZE		CONFIG_ENV_SIZE
#define CONFIG_NR_DRAM_BANKS		1/* we have 1 bank of DRAM */
#define CFG_BOOT_PARAMS		(MEM_BASE_DDR+0x0100)

/*-----------------------------------------------------------------------
 *  Environment   Configuration
 ------------------------------------------------------------------------*/
#define CONFIG_BOOTDELAY    1
#define CONFIG_BOOTARGS "mem=256M console=ttyAMA0,115200 console=ttyMTD,blackbox"
#define CONFIG_NETMASK  255.255.255.0       /* talk on MY local net */
#define CONFIG_IPADDR   192.168.1.10        /* static IP I currently own */
#define CONFIG_SERVERIP 192.168.1.1     /* current IP of tftp server ip */
#define CONFIG_ETHADDR  00:00:00:00:00:00
#define CONFIG_BOOTFILE "uImage"        /* file to load */
#define CONFIG_BAUDRATE         115200
/*-----------------------------------------------------------------------
 * for bootm linux 
 *
 -----------------------------------------------------------------------*/

#define CONFIG_BOOTM_LINUX 1    //for bootm
#define CONFIG_SYS_LOAD_ADDR (CFG_DDR_PHYS_OFFSET + 0x08000)   /* default load address 0x80008000*/
#define CONFIG_ZERO_BOOTDELAY_CHECK 1   /*use ^c to interrupt*/
 
/*-----------------------------------------------------------------------
 * for  commond configure 
 -----------------------------------------------------------------------*/
//tftp comm
#define CONFIG_SYS_MAX_FLASH_BANKS 1    /* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT 256

//do_printenv  do_setenv common/cmd_nvedit.c
#define CONFIG_SYS_BAUDRATE_TABLE { 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_MAXARGS 16          /* max number of command args   */

#define CONFIG_CMD_LOADB  /* loadb   common/cmd_load.c*/ 

/*-----------------------------------------------------------------------
 * network config
 -----------------------------------------------------------------------*/
#define CONFIG_ARP_TIMEOUT		50000UL//FIXME
#define CONFIG_NET_RETRY_COUNT		50//FIXME
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
	#define CONFIG_NET_HISFV300_GODBOX	1
	#define HISFV_MII_MODE		0
	#define HISFV_RMII_MODE		1
	#define HIETH_MII_RMII_MODE_U   HISFV_RMII_MODE
	#define HIETH_MII_RMII_MODE_D   HISFV_RMII_MODE
	#define HISFV_PHY_U		1
	#define HISFV_PHY_D		3
	#undef HISFV_RESET_GPIO_EN
	#ifdef HISFV_RESET_GPIO_EN
		/* godbox use gpio9_6 to control sfv reset */
		#define HISFV_RESET_GPIO_BASE REG_BASE_GPIO9
		#define HISFV_RESET_GPIO_DIR 0x400
		#define HISFV_RESET_GPIO_BIT 6
		#define HISFV_RESET_GPIO_DIR_OUT 1
		/* gpio9_6: 0-reset; 1-undo reset */
		#define HISFV_RESET_GPIO_DATA 0
	#endif /* HISFV_RESET_GPIO_EN */
#endif /* CONFIG_NET_HISFV300 */

/*-----------------------------------------------------------------------
 * STMMAC driver
 -----------------------------------------------------------------------*/
//#define CONFIG_NET_STMMAC
#ifdef CONFIG_NET_STMMAC
	#define STMMAC_GMACADDR		(0x60140000)
	#define STMMAC_DMAADDR		(0x60141000)
	#define STMMAC_IOSIZE		(0x10000)
	#define STMMAC_FRQDIV		(0)
	#define STMMAC_PHYADDR0		(3)
	#define STMMAC_PHYADDR1		(3)
	#define STMMAC_PHYNAME		"0:03"
	#define CONFIG_PHY_GIGE
#endif /* CONFIG_NET_STMMAC */

/*-----------------------------------------------------------------------
 * NAND Flash Configuration
 -----------------------------------------------------------------------*/

#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BASE REG_BASE_NANDC 
#define CONFIG_NAND_GODBOX	1
#define CFG_MAX_NAND_DEVICE CONFIG_SYS_MAX_NAND_DEVICE
#define CFG_NAND_U_BOOT_OFFS 0x20000
#define CFG_NAND_U_BOOT_SUFF_SZ	0xA0000	//uboot left size 256k 
#define CFG_NAND_U_BOOT_DST  (TEXT_BASE + CFG_NAND_U_BOOT_OFFS)
#define CONFIG_SYS_MAX_NAND_DEVICE    1
#define CONFIG_SYS_NAND_MAX_CHIPS            2

#define CONFIG_HINFC301_MAX_CHIP CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_HINFC301_REG_BASE_ADDRESS REG_BASE_NANDC  
#define CONFIG_HINFC301_BUFFER_BASE_ADDRESS MEM_BASE_NAND 
#define CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC 

/*-----------------------------------------------------------------------
 * SPI Flash Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_CMD_SF	  /*sf read \ sf write \ sf erase */
#define SFC_MEM_BASE MEM_BASE_SPI
#define SFC_REG_BASE REG_BASE_SFC 
#define CONFIG_HISFC_FLASH_GODBOX 1

#define CONFIG_HISFC300_BUFFER_BASE_ADDRESS SFC_MEM_BASE
#define CONFIG_HISFC300_REG_BASE_ADDRESS SFC_REG_BASE
#define CONFIG_HISFC300_PERIPHERY_REGBASE REG_BASE_CRG
#define CONFIG_HISFC300_CHIP_NUM 2
// cp.b 
#define CONFIG_CMD_MEMORY  /*md mw cp etc.*/

/*****************************************************************************/
#if 0 /* Define ubi/ubifs supports */
/* UBI/UBIFS need much more malloc memory. */
#  warning adjust malloc macro CONFIG_SYS_MALLOC_LEN for ubi/ubifs.
#  undef CONFIG_SYS_MALLOC_LEN
#  define CONFIG_SYS_MALLOC_LEN      (CONFIG_ENV_SIZE + 128*1024 + (512 << 11))
#  define CONFIG_CMD_UBI              /* UBI Support      */
#  define CONFIG_CMD_UBIFS            /* UBIFS Support    */
#  define CONFIG_MTD_PARTITIONS
#  define CONFIG_MTD_DEVICE
#  define CONFIG_RBTREE
#  define CONFIG_LZO
#  define CONFIG_CMD_MTDPARTS
#endif
/*****************************************************************************/
#if 0 /* Define net-console supports */
#  define CONFIG_NETCONSOLE              1
#endif
/*****************************************************************************/
#if 1 /* Enable Ctrl+C to stop autoboot.*/
#  define CONFIG_AUTOBOOT_KEYED      1
#  define CONFIG_AUTOBOOT_PROMPT     "Press Ctrl+C to stop autoboot\n"
#  define CONFIG_AUTOBOOT_DELAY_STR  "hisilicon #"
#  define CONFIG_AUTOBOOT_STOP_STR   "\x03" 
#endif
/*****************************************************************************/
/*-----------------------------------------------------------------------
 * console display  Configuration
 ------------------------------------------------------------------------*/

#define CONFIG_VERSION_VARIABLE  1 /*used in common/main.c*/
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_PROMPT  "fastboot# "        /* Monitor Command Prompt   */
#define CONFIG_SYS_CBSIZE  1024            /* Console I/O Buffer Size  */
#define CONFIG_SYS_PBSIZE  (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)

#define CFG_LONGHELP
#define CFG_BARGSIZE    CFG_CBSIZE      /* Boot Argument Buffer Size    */
#undef  CFG_CLKS_IN_HZ              /* everything, incl board info, in Hz */
#define CFG_LOAD_ADDR   (CFG_DDR_PHYS_OFFSET + 0x08000)     /* default load address */
#define CONFIG_AUTO_COMPLETE    1
#define CFG_CMDLINE_HISTORYS    8
#define CONFIG_CMDLINE_EDITING

#define CFG_DDR_PHYS_OFFSET MEM_BASE_DDR
#define CFG_DDR_SIZE     0x10000000//   ((0x800000<<CFG_DDRC_CHIPCAP) * ((16<<CFG_DDRC_BUSWITH)/(8<<CFG_DDRC_CHIPBITS)))

#define CONFIG_SYS_MEMTEST_START       (CFG_DDR_PHYS_OFFSET + sizeof(unsigned long))
#define CONFIG_SYS_MEMTEST_END         (CFG_DDR_PHYS_OFFSET + CFG_DDR_SIZE)
#define CONFIG_SYS_MEMTEST_SCRATCH     CFG_DDR_PHYS_OFFSET

#define CONFIG_CMDLINE_TAG      1   /* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG       1   /* support initrd */
#define CONFIG_SETUP_MEMORY_TAGS    1
#define CONFIG_MISC_INIT_R      1   /* call misc_init_r during start up */

#define CONFIG_ETHADDR_TAG	1
#ifdef  CONFIG_ETHADDR_TAG
#define CONFIG_ETHADDR_TAG_VAL	0x726d6d73
#endif

#undef CONFIG_NANDID_TAG
#undef CONFIG_SPIID_TAG

#if 0
#define CONFIG_NANDPTN_TAG	1
#define CONFIG_NANDPTN_TAG_VAL	0x48694E70

#define CONFIG_SPIPTN_TAG	1
#define CONFIG_SPIPTN_TAG_VAL	0x48695370
#endif

#define CONFIG_SYS_NO_FLASH
#define CONFIG_PL011_SERIAL 1
#define CONFIG_PL011_CLOCK  54000000
#define CFG_SERIAL0     REG_BASE_UART0
#define CFG_SERIAL1     REG_BASE_UART1
#define CONFIG_PL01x_PORTS  { (void *)CFG_SERIAL0, (void *)CFG_SERIAL1 }
#define CONFIG_CONS_INDEX   0

#define CONFIG_PRODUCTNAME                      "godbox"

#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_HIMCI_V100
#define CONFIG_HIMCI_GODBOX

#define CONFIG_MMC_BOOT_ADDR	0x200

/*-----------------------------------------------------------------------
 * bootrom Configuration
 -----------------------------------------------------------------------*/
#define CONFIG_BOOTROM_SUPPORT
#if defined(CONFIG_BOOTROM_SUPPORT) \
	&& (!defined(REG_START_FLAG) || !defined(CONFIG_START_MAGIC))
#  error Please define CONFIG_START_MAGIC and CONFIG_START_MAGIC first
#endif

#endif	/* __CONFIG_H */
