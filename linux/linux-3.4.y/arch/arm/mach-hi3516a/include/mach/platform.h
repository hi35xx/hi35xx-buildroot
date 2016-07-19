#ifndef __HI_CHIP_REGS_H__
#define __HI_CHIP_REGS_H__

#include <mach/io.h>
#define CRG_REG_BASE		0x20030000
#define SYS_CTRL_BASE           0x20050000
#define REG_SC_SYSRES           0x4

#define REG_BASE_TIMER0		0x20000000
#define REG_BASE_TIMER1         0x20010000

#define REG_BASE_UART0                                  0x20080000
#define REG_BASE_UART1                                  0x20090000
#define REG_BASE_UART2                                  0x200a0000
#define REG_BASE_UART3                                  0x20230000

#define REG_BASE_CUR_UART                               REG_BASE_UART0

#define REG_BASE_A7_PERI                                0x20300000

/* Cortec-A7 */
#define REG_A7_PERI_GIC_CPU                             0x2000
/* #define REG_A7_PERI_GLOBAL_TIMER                        0x0200 */
/* #define REG_A7_PERI_PRI_TIMER_WDT                       0x0600 */
#define REG_A7_PERI_GIC_DIST                            0x1000

#define MISC_CTRL_BASE		0x20120000
#define DDRC_BASE		0x20110000
#define CFG_GIC_CPU_BASE    (IO_ADDRESS(REG_BASE_A7_PERI) + REG_A7_PERI_GIC_CPU)
#define CFG_GIC_DIST_BASE   (IO_ADDRESS(REG_BASE_A7_PERI) \
		+ REG_A7_PERI_GIC_DIST)

#define UART_CKSEL_APB          (1 << 19)


#endif /*End of __HI_CHIP_REGS_H__ */
