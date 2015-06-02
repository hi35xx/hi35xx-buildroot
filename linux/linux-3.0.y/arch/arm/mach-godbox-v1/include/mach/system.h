#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <linux/io.h>
#include <mach/platform.h>

static inline void arch_idle(void)
{
	/*
	 * This should do all the clock switching
	 * and wait for interrupt tricks
	 */
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
	unsigned int reg = __raw_readl(IO_ADDRESS(REG_BASE_SCTL));
	do {
		/* Cpu from normal to slow, should be 2:1  */
		reg &= ~0x7000;
		__raw_writel(reg, IO_ADDRESS(REG_BASE_SCTL));
		reg = __raw_readl(IO_ADDRESS(REG_BASE_SCTL));
	} while (reg & 0x7000);

	__raw_writel(~0, IO_ADDRESS(REG_BASE_SCTL) + REG_SC_SYSRES);
}
#endif
