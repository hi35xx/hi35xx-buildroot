#ifndef __HI_CHIP_REGS_H__
#define __HI_CHIP_REGS_H__

#include <mach/io.h>

#define DDR_BASE		0x80000000
#define SPI_MEM_BASE		0x58000000
#define NAND_MEM_BASE		0x50000000

#define UART1_BASE		0x10609000
#define TIMER1_BASE		0x10600000

#define UART0_BASE		0x200d6000
#define SYS_CTRL_BASE		0x200d0000
#define TIMER0_BASE		0x200d2000
#define NANDC_REG_BASE		0x20010000
#define ARM_INTNL_BASE		0x20300000
#define REG_BASE_L2CACHE	0x20700000

#define REG_SC_CTRL             0x0000
#define REG_SC_SYSRES           0x0004
#define REG_SC_WDG_RST_CTRL     0x0050
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
#define REG_SLAVE_RST           0x0ef0

#define REG_A9_PERI_SCU                 0x0000
#define REG_A9_PERI_GLOBAL_TIMER        0x0200
#define REG_A9_PERI_PRI_TIMER_WDT       0x0600
#define REG_SC_LOCAL_TIMER              0x600
#define REG_BASE_A9_PERI        ARM_INTNL_BASE

#define A9_GIC_OFFSET		0x100
#define A9_GIC_DIST		0x1000
#define UART0_IOSIZE            0x1000
#define CFG_GIC_CPU_BASE	(IO_ADDRESS(ARM_INTNL_BASE) + A9_GIC_OFFSET)
#define CFG_GIC_DIST_BASE	(IO_ADDRESS(ARM_INTNL_BASE) + A9_GIC_DIST)

#define REG_TIMER_RELOAD        0x000
#define REG_TIMER_VALUE         0x004
#define REG_TIMER_CONTROL       0x008
#define REG_TIMER_INTCLR        0x00C
#define REG_TIMER_RIS           0x010
#define REG_TIMER_MIS           0x014
#define REG_TIMER_BGLOAD        0x018

#define REG_TIMER1_RELOAD	0x020
#define REG_TIMER1_VALUE	0x024
#define REG_TIMER1_CONTROL	0x028
#define REG_TIMER1_INTCLR	0x02C
#define REG_TIMER1_RIS		0x030
#define REG_TIMER1_MIS		0x034
#define REG_TIMER1_BGLOAD	0x038

#define SMP_COREX_START_ADDR_REG (SYS_CTRL_BASE + REG_SC_GEN13)

#endif
