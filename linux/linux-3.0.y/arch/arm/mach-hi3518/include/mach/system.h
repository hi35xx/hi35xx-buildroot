#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <linux/io.h>
#include <mach/platform.h>

asmlinkage void tcm_slow_reset(void __iomem*, void __iomem *);

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
	void __iomem *tcm_virt_addr;
	void __iomem *sys_virt_addr;

	tcm_virt_addr = ioremap_nocache(0x0, 0x800);
	if (!tcm_virt_addr) {
		printk(KERN_INFO "ioremap tcm_virt_addr is error!!!\n");
		return;
	}

	tcm_virt_addr += 0x100;
	sys_virt_addr = (void *)IO_ADDRESS(SYS_CTRL_BASE);
	tcm_slow_reset(tcm_virt_addr, sys_virt_addr);
	__raw_writel(~0, IO_ADDRESS(SYS_CTRL_BASE) + REG_SC_SYSRES);
}
#endif
