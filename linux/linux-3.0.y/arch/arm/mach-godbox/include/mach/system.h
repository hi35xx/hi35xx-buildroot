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
	local_irq_disable();
	/* unclock wdg */
	writel(0x1ACCE551,  IO_ADDRESS(REG_BASE_WDG + 0xc00));
	/* wdg load value */
	writel(0x00000100,  IO_ADDRESS(REG_BASE_WDG + 0x0));
	/* bit0: int enable bit1: reboot enable */
	writel(0x00000003,  IO_ADDRESS(REG_BASE_WDG + 0x8));
	local_irq_enable();

	while (1)
		;
}
#endif
