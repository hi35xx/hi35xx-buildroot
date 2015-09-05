#ifndef __FASTBOOT_PLATFORM_H
#define __FASTBOOT_PLATFORM_H

/*-----------------------------------------------------------------------
 * Serial Configuration
 */
#define REG_BASE_UART0			0x101FB000
#define REG_BASE_UART1			0x101FC000

#define CONFIG_BAUDRATE			115200
#define CONFIG_PL011_CLOCK		54000000
#define CONFIG_SERIAL0			REG_BASE_UART0
#define CONFIG_SERIAL1			REG_BASE_UART1
#define CONFIG_CONS_INDEX		0

/*sys register define*/
#define REG_BASE_CRG			0x101F5000
#define REG_BASE_PERI_CTRL		REG_BASE_CRG
#define REG_START_MODE			0
#define REG_PERI_CRG26			0x00A8
#define RAM_START_ADRS			0x10c00

#define REG_BASE_SCTL			0x101e0000
#define REG_SC_APLLFREQCTRL0	0x0018
#define REG_SC_APLLFREQCTRL1	0x001C
#define REG_SC_CTRL				0
#define REG_TIMEREN0OV			(16)
#define REG_TIMEREN1OV			(18)

#define REG_TIMER2_BASE			0x10202008
#define REG_TIMER2_VALUE		0x10202004

#define REG_MINISYS_TIMER_VALUE REG_TIMER2_VALUE

#define REG_SC_SYSRES			0x0004

#define BOOT_FROM_DDR			2
#define BOOT_FROM_NAND			1
#define BOOT_FROM_SPI			0

#define MEM_BASE_DDR			0x80000000

#define REG_SC_GEN0             0x0080
#define REG_SC_GEN1             0x0084
#define REG_SC_GEN2             0x0088
#define REG_SC_GEN3             0x008C
#define REG_SC_GEN4             0x0090
#define REG_SC_GEN5             0x0094
#define REG_SC_GEN6             0x0098
#define REG_SC_GEN7             0x009C
#define REG_SC_GEN8             0x00A0
#define REG_SC_GEN9             0x00A4
#define REG_SC_GEN10            0x00A8
#define REG_SC_GEN11            0x00AC
#define REG_SC_GEN12            0x00B0
#define REG_SC_GEN13            0x00B4
#define REG_SC_GEN14            0x00B8
#define REG_SC_GEN15            0x00BC

#define REG_BASE_TIMER01		0x101E2000
#define REG_TIMER_RELOAD		0x000
#define REG_TIMER_VALUE			0x004
#define REG_TIMER_CONTROL		0x008
#define REG_SC_WDG_RST_CTRL     0x0050

/*****************************************************************************/

#define CONFIG_PRODUCTNAME      "godbox-v1"

#define CONFIG_ARCH_TYPE        8000

#define CONFIG_DDR_PHYS         0x80000000

#define CONFIG_MAX_DOWNLOAD_BUF_LEN (10*1024*1024)/* FIXME: 10MB */

#define CONFIG_MAX_NAND_DEVICE  4 
#define MEM_BASE_SPI			0x26000000
#define CONFIG_SFC_MEM_BASE		MEM_BASE_SPI
#define REG_BASE_SFC			0x60020000
#define CONFIG_SFC_REG_BASE		REG_BASE_SFC
#define REG_BASE_SF				0x600D0000
/* set user input wait timeout when fastboot startup, it's value from 0 to 9 */
#define CONFIG_FASTBOOT_DELAY   (3)
/*****************************************************************************/
/* Nand flash config */
#define REG_BASE_NANDC			(0x60010000)
#define MEM_BASE_NAND           (0x24000000)
#define CONFIG_HINFC300_REG_BASE_ADDRESS               REG_BASE_NANDC
#define CONFIG_HINFC300_PERIPHERY_REGBASE              REG_BASE_CRG
#define CONFIG_HINFC300_BUFFER_BASE_ADDRESS            MEM_BASE_NAND


//#define CONFIG_HINFC300_SHOW_NAND_INFO

#define REG_BASE_A9_PERI        0x10220000
#define REG_A9_PERI_GIC_CPU		0x0100
#define REG_A9_PERI_GIC_DIST	0x1000

#define CFG_GIC_CPU_BASE		(REG_BASE_A9_PERI +REG_A9_PERI_GIC_CPU)
#define CFG_GIC_DIST_BASE		(REG_BASE_A9_PERI +REG_A9_PERI_GIC_DIST)

#define REG_GIC_ICCICR			(0)
#define REG_GIC_ICDSGIR			(0xF00)

#endif /* __ASM_ARCH_PLATFORM_H */

