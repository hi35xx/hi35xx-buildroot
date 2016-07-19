#ifndef __HI_CHIP_REGS_H__
#define __HI_CHIP_REGS_H__

#include <mach/io.h>
#define MISC_REG_BASE			0x20120000
#define REG_MISC_CTRL1		0x04
#define REG_MISC_CTRL30		0x78
#define REG_MISC_CTRL32		0x80

#define REG_BASE_UART2			0x200a0000
#define REG_BASE_UART1			0x20090000
#define REG_BASE_UART0			0x20080000
#define REG_BASE_CUR_UART		REG_BASE_UART0

/* -------------------------------------------------------------------- */
/* System Control REG */
/* -------------------------------------------------------------------- */
#define SYS_CTRL_BASE			0x20050000
#define	TIME0_CLK_APB		(0x1 << 16)
#define	TIME1_CLK_APB		(0x1 << 18)
#define	TIME2_CLK_APB		(0x1 << 20)
#define	TIME3_CLK_APB		(0x1 << 22)
#define REG_SC_SYSRES		0x4

/* System Status register offset */
#define REG_SC_STAT			0x008c
/* if bit[4]=0b: boot form SPI && bit[3]=0: SPI nor flash
 * bit[7]=0: 3-Byte address mode; bit[7]=1: 4-Byte address mode */
#define GET_SPI_NOR_ADDR_MODE(_reg)	(((_reg) >> 7) & 0x1)
/* bit[3]=0; SPI nor flash; bit[3]=1: SPI nand flash */
#define GET_SPI_DEVICE_TYPE(_reg)	(((_reg) >> 3) & 0x1)

/* -------------------------------------------------------------------- */
/* Clock and Reset Generator REG */
/* -------------------------------------------------------------------- */
#define CRG_REG_BASE			0x20030000
#define REG_CRG12			0x30
#define REG_CRG46			0xb8

#define REG_CRG48			0x00c0
/* FMC CRG register offset */
#define REG_FMC_CRG			REG_CRG48
#define FMC_CLK_SEL(_clk)		(((_clk) & 0x3) << 2)
#define FMC_CLK_SEL_MASK		(0x3 << 2)
#define GET_FMC_CLK_TYPE(_reg)		(((_reg) >> 2) & 0x3)
#define FMC_CLK_24M			0
#define FMC_CLK_148M			1
#define FMC_CLK_125M			2
#define FMC_CLK_198M			3
#define FMC_CLK_ENABLE			(0x1 << 1)
#define FMC_SOFT_RST_REQ		(0x1 << 0)

#define REG_CRG49			0xc4
#define REG_CRG57			0xe4
#define UART_CLK_MASK       (0x3 << 19)
#define UART_CLK_APB        (0x0 << 19)
#define UART_CLK_24M        (0x1 << 19)
#define UART_CLK_6M			(0x2 << 19)
#define REG_ETH_CRG			0xec

#define REG_BASE_TIMER23			0x20010000
#define REG_BASE_TIMER01			0x20000000

/* ARM9 */
#define REG_BASE_INTC			0x100d0000
#define INTNR_IRQ_START		0

/*********************************************************************/

/*-----------------------------------------------------------------------
 * cpu_init configuration
 * if bit[0] = 0b, AXI = 24M
 * if bit[0] = 1b, AXI = 198M
 *----------------------------------------------------------------------*/
#define AXI_SCALE_REG	IO_ADDRESS(CRG_REG_BASE + REG_CRG12)
#define get_bus_clk() ({\
		unsigned long tmp_reg, busclk = 0;\
		tmp_reg = readl(AXI_SCALE_REG);\
		tmp_reg &= 0x1;\
		if (0x0 == tmp_reg) {\
			busclk = 24000000;\
		} else {\
			busclk = 198000000;\
		} \
		busclk;\
		})
#endif /* End of __HI_CHIP_REGS_H__ */

