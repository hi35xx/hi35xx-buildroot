#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/suspend.h>
#include <asm/memory.h>
#include <mach/system.h>
#include <mach/early-debug.h>
#include <mach/platform.h>
#include <linux/delay.h>
#include <linux/suspend.h>
#include <linux/syscalls.h>
#include <asm/mach/time.h>
#include <linux/slab.h>

void __iomem *hi_sc_virtbase;
void __iomem *hi_scu_virtbase;

asmlinkage void hi_pm_sleep(void);

static int hi_pm_enter(suspend_state_t state)
{
	hi_sc_virtbase = (void *)IO_ADDRESS(REG_BASE_SCTL);
	hi_scu_virtbase = (void *)IO_ADDRESS(REG_BASE_A9_PERI);
	hi_pm_sleep();
	return 0;
}

int hipm_valid(suspend_state_t state)
{
	return 1;
}

static const struct platform_suspend_ops hi_pm_ops = {
	.enter		= hi_pm_enter,
	.valid		= hipm_valid,
};

static int __init hi_pm_init(void)
{
	suspend_set_ops(&hi_pm_ops);
	return 0;
}

module_init(hi_pm_init);

