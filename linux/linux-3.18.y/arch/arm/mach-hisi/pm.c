/*
 * power mangager control for hisilicon soc
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/suspend.h>
#include <asm/memory.h>
#include <mach/io.h>
#include <mach/platform.h>

#include <linux/delay.h>
#include <linux/suspend.h>
#include <linux/syscalls.h>
#include <asm/mach/time.h>
#include <linux/slab.h>

#include <asm/mach/time.h>
#include <asm/hardware/arm_timer.h>
#include <asm/mach/irq.h>

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/reboot.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kallsyms.h>
#include <asm/cacheflush.h>
#include <asm/processor.h>

#include <asm/mach/map.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>

struct hibvt_pm_dev {
	void __iomem		*sys_ctrl_base;
	void __iomem		*time0_base;
};

static struct hibvt_pm_dev g_hibvt_pm;

void (*hi_power_off)(void) = NULL;
EXPORT_SYMBOL(hi_power_off);

void hi_pm_power_off(void)
{
	/* Disable interrupts first */
	local_irq_disable();
	local_fiq_disable();

	/* Clean and invalidate caches */
	flush_cache_all();

	/* Turn off caching */
	cpu_proc_fin();

	/* Push out any further dirty data, and ensure cache is empty */
	flush_cache_all();

	/*
	 * Now call the architecture specific reboot code.
	 */
    if(hi_power_off)
        hi_power_off();
	/*
	 * Whoops - the architecture was unable to reboot.
	 * Tell the user!
	 */
	mdelay(1000);

	printk(KERN_EMERG "Poweroff failed -- System halted\n");

	while (1)
		;
}

/*hibernate methods*/
#ifdef CONFIG_HIBERNATION
unsigned long saved_interrupt_mask[128];
unsigned long saved_cpu_target_mask[128];

typedef struct __timer_register {
    unsigned long timer_load;
    unsigned long timer_value;
    unsigned long timer_ctrl;
    unsigned long timer_bgload;
} timer_register;

static timer_register timer0[2];
static int timer_init_value;
static int save_timer0(void)
{
	void __iomem *timer0_base_addr = g_hibvt_pm.time0_base;

	timer_init_value = readl(g_hibvt_pm.sys_ctrl_base);
	/*protect timer0_0*/
	timer0[0].timer_load   = readl(timer0_base_addr + TIMER_LOAD);
	timer0[0].timer_value  = readl(timer0_base_addr + TIMER_VALUE);
	timer0[0].timer_ctrl   = readl(timer0_base_addr + TIMER_CTRL);
	timer0[0].timer_bgload = readl(timer0_base_addr + TIMER_BGLOAD);
	/*disable timer0_0  */
	writel(0, timer0_base_addr + TIMER_CTRL);

	/*protect timer0_1*/
	timer0[1].timer_load   = readl(timer0_base_addr + 0x20 + TIMER_LOAD);
	timer0[1].timer_value  = readl(timer0_base_addr + 0x20 + TIMER_VALUE);
	timer0[1].timer_ctrl   = readl(timer0_base_addr + 0x20 + TIMER_CTRL);
	timer0[1].timer_bgload = readl(timer0_base_addr + 0x20 + TIMER_BGLOAD);
	/*disable timer0_1  */
	writel(0, timer0_base_addr + 0x20 + TIMER_CTRL);

	return 0;
}


static int restore_timer0(void)
{
	void __iomem *timer0_base_addr = g_hibvt_pm.time0_base;

	writel(timer_init_value, g_hibvt_pm.sys_ctrl_base);
	/* restore timer0_0 */
	writel(0, timer0_base_addr + TIMER_CTRL);
	writel(1, timer0_base_addr + TIMER_INTCLR);

	writel(timer0[0].timer_value, timer0_base_addr + TIMER_LOAD);
	writel(timer0[0].timer_ctrl, timer0_base_addr + TIMER_CTRL);
	writel(timer0[0].timer_bgload, timer0_base_addr + TIMER_BGLOAD);

	/* restore timer0_1  */
	writel(0, timer0_base_addr + 0x20 + TIMER_CTRL);
	writel(1, timer0_base_addr + 0x20 + TIMER_INTCLR);

	writel(timer0[1].timer_value, timer0_base_addr + 0x20 + TIMER_LOAD);
	writel(timer0[1].timer_ctrl, timer0_base_addr + 0x20 + TIMER_CTRL);
	writel(timer0[1].timer_bgload, timer0_base_addr + 0x20 + TIMER_BGLOAD);

	return 0;
}
static int hi_hiber_begin(void)
{
	return 0;
}

static void hi_hiber_end(void)
{
}

static int hi_hiber_pre_snapshot(void)
{
	save_timer0();
	return 0;
}

static void hi_hiber_finish(void)
{
	restore_timer0();
}

static int hi_hiber_prepare(void)
{
	return 0;
}

static void hi_hiber_leave(void)
{
}

static int hi_hiber_enter(void)
{
#ifdef CONFIG_DEFAULT_MTD
	hi_pm_power_off();
#else
	kernel_restart(NULL);
#endif
	return 0;
}

static int hi_hiber_restore(void)
{
	return 0;
}

static void hi_hiber_restore_cleanup(void)
{
}

static void hi_hiber_recover(void)
{
}

struct platform_hibernation_ops hi_hibernation_ops = {
    .begin           = hi_hiber_begin,
    .end             = hi_hiber_end,
    .pre_snapshot    = hi_hiber_pre_snapshot,
    .finish          = hi_hiber_finish,
    .prepare         = hi_hiber_prepare,
    .enter           = hi_hiber_enter,
    .leave           = hi_hiber_leave,
    .pre_restore     = hi_hiber_restore,
    .restore_cleanup = hi_hiber_restore_cleanup,
    .recover         = hi_hiber_recover,
};
#endif /* CONFIG_HIBERNATION */


/*pm methods*/
static int hi_pm_enter(suspend_state_t state)
{
	return 0;
}

int hi_pm_valid(suspend_state_t state)
{
	return 1;
}

static const struct platform_suspend_ops hi_pm_ops = {
	.enter		= hi_pm_enter,
	.valid		= hi_pm_valid,
};

static int hi_pm_probe(struct platform_device *pdev)
{
	g_hibvt_pm.sys_ctrl_base = of_iomap(pdev->dev.of_node, 0);
	if (!g_hibvt_pm.sys_ctrl_base) {
		pr_err("%s: failed to map system controller registers\n",
				__func__);
		return -ENOMEM;
	}

	g_hibvt_pm.time0_base = of_iomap(pdev->dev.of_node, 1);
	if (!g_hibvt_pm.time0_base) {
		pr_err("%s: failed to map timer registers\n", __func__);
		iounmap(g_hibvt_pm.sys_ctrl_base);
		return -ENOMEM;
	}

	pm_power_off = hi_pm_power_off;

	suspend_set_ops(&hi_pm_ops);

#ifdef CONFIG_HIBERNATION
	/* registering hibernation call backs */
	hibernation_set_ops(&hi_hibernation_ops);
#endif
	return 0;
}

static const struct of_device_id hi_pm_match_table[] = {
	{ .compatible = "hisilicon,hibvt-pm" },
	{ }
};
MODULE_DEVICE_TABLE(of, hi_pm_match_table);

static struct platform_driver hi_pm_driver = {
	.probe          = hi_pm_probe,
	.driver         = {
		.name   = "hibvt-pm",
		.of_match_table = hi_pm_match_table,
	},
};

static int __init hi_pm_init(void)
{
	return platform_driver_register(&hi_pm_driver);
}
module_init(hi_pm_init);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("HiSilicon BVT PM Driver");
