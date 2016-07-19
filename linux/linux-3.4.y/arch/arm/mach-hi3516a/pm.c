/*copyright:hisilicon,bvt,liucan,2014-7-14*/

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

#include <mach/platform.h>
#include <mach/time.h>
#include <asm/mach/time.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
#include <asm/mach/irq.h>

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/reboot.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kallsyms.h>
#include <asm/cacheflush.h>
#include <asm/processor.h>
#include <asm/system.h>

#include <mach/hardware.h>
#include <asm/mach/map.h>

static void hi3516a_dv_power_off(void)
{
	unsigned int gpio_group, gpio_offset;

	gpio_group = 13;
	gpio_offset = 2;

#define GPIO_BASE 0x20140000
	/*config gpio is output*/
	writel((0x01 << gpio_offset),
			IO_ADDRESS(GPIO_BASE + (gpio_group << 16) + 0x400));

	/*output high*/
	writel((0x01 << gpio_offset),
			IO_ADDRESS(GPIO_BASE + (gpio_group << 16) + 0x00
				+ (1 << (gpio_offset +  2))));
}

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
	hi3516a_dv_power_off();

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

static int save_gic(void)
{
	unsigned int max_irq, i;
	unsigned int intack;

	void __iomem *gic_dist_base_addr;
	void __iomem *gic_cpu_base_addr;

	gic_dist_base_addr = (void __iomem *)CFG_GIC_DIST_BASE;
	gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;

	/* disable gic dist */
	writel(0, gic_dist_base_addr + GIC_DIST_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	max_irq = readl(gic_dist_base_addr + GIC_DIST_CTR) & 0x1f;
	max_irq = (max_irq + 1) * 32;

	/*
	 * The GIC only supports up to 1020 interrupt sources.
	 * Limit this to either the architected maximum, or the
	 * platform maximum.
	 */
	if (max_irq > max(1020, NR_IRQS)) {
		max_irq = max(1020, NR_IRQS);
	}

	/* save Dist target */
	for (i = 32; i < max_irq; i += 4) {
		saved_cpu_target_mask[i / 4] =
		    readl(gic_dist_base_addr + GIC_DIST_TARGET + i * 4 / 4);
	}

	/* save mask irq */
	for (i = 0; i < max_irq; i += 32) {
		saved_interrupt_mask[i / 32] =
		    readl(gic_dist_base_addr + GIC_DIST_ENABLE_SET +
			  i * 4 / 32);
	}

	/* clear all interrupt */
	for (i = 0; i < max_irq; i += 32) {
		writel(0xffffffff,
		       gic_dist_base_addr + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);
	}

	/* read INT_ACK in CPU interface, until result is 1023 */
	for (i = 0; i < max_irq; i++) {
		intack = readl(gic_cpu_base_addr + 0x0c);
		if (1023 == intack) {
			break;
		}
		writel(intack, gic_cpu_base_addr + 0x10);
	}

#if 0   /* comment off wakeup intr, cause we will go directly to deepsleep */
	/* enable softinterrupt mask */
	writel(0xffff, gic_dist_base_addr + GIC_DIST_ENABLE_SET);

	/* enable KPC/TBC/RTC interrupt */
	writel(GET_IRQ_BIT(IRQ_KPC) | GET_IRQ_BIT(IRQ_TBC) |
	       GET_IRQ_BIT(IRQ_RTC),
	       gic_dist_base_addr + GIC_DIST_ENABLE_SET + 4);

	writel(0, gic_dist_base_addr + GIC_DIST_ENABLE_SET + 8);

	/* enable all gpio interrupt */
	writel(0x3fffff, gic_dist_base_addr + GIC_DIST_ENABLE_SET + 0xc);
#endif
	/* Enable GIC Dist Ctrl, SR needed */
	/* no intr response mode, need not enable dist ctrl */
	//writel(1, gic_dist_base_addr + GIC_DIST_CTRL);

	return 0;
}


static int restore_gic(void)
{
	unsigned int max_irq, i;

	/* PRINT OUT the GIC Status */
	unsigned int irq_status[5];

	void __iomem *gic_dist_base_addr;
	void __iomem *gic_cpu_base_addr;

	gic_dist_base_addr = (void __iomem *)CFG_GIC_DIST_BASE;
	gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;

	for (i = 0; i < 5; i++) {
		irq_status[i] = readl(gic_dist_base_addr + 0xd00 + i * 4);
	}

	writel(0, gic_dist_base_addr + GIC_DIST_CTRL);
	writel(0, gic_cpu_base_addr + GIC_CPU_CTRL);

	/*
	 * Find out how many interrupts are supported.
	 */
	max_irq = readl(gic_dist_base_addr + GIC_DIST_CTR) & 0x1f;
	max_irq = (max_irq + 1) * 32;

	/*
	 * The GIC only supports up to 1020 interrupt sources.
	 * Limit this to either the architected maximum, or the
	 * platform maximum.
	 */
	if (max_irq > max(1020, NR_IRQS)) {
		max_irq = max(1020, NR_IRQS);
	}

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for (i = 32; i < max_irq; i += 16) {
		writel(0, gic_dist_base_addr + GIC_DIST_CONFIG + i * 4 / 16);
	}

	/*
	 * Set all global interrupts to this CPU only.
	 */
	for (i = 32; i < max_irq; i += 4) {
		writel(saved_cpu_target_mask[i / 4],
		       gic_dist_base_addr + GIC_DIST_TARGET + i * 4 / 4);
	}

	/*
	 * Set priority on all interrupts.
	 */
	for (i = 0; i < max_irq; i += 4) {
		writel(0xa0a0a0a0,
		       gic_dist_base_addr + GIC_DIST_PRI + i * 4 / 4);
	}

	/*
	 * Disable all interrupts.
	 */
	for (i = 0; i < max_irq; i += 32) {
		writel(0xffffffff,
		       gic_dist_base_addr + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);
	}

	for (i = 0; i < max_irq; i += 32) {
		writel(saved_interrupt_mask[i / 32], gic_dist_base_addr +
		       GIC_DIST_ENABLE_SET + i * 4 / 32);
	}

	writel(1, gic_dist_base_addr + GIC_DIST_CTRL);

	/* set the BASE priority 0xf0 */
	writel(0xf0, gic_cpu_base_addr + GIC_CPU_PRIMASK);

	writel(1, gic_cpu_base_addr + GIC_CPU_CTRL);

	return 0;
}

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
	void __iomem *timer0_base_addr = (void __iomem *)CFG_TIMER_VABASE;

	timer_init_value = readl(IO_ADDRESS(SYS_CTRL_BASE));
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
	void __iomem *timer0_base_addr = (void __iomem *)CFG_TIMER_VABASE;

	writel(timer_init_value, IO_ADDRESS(SYS_CTRL_BASE));
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
   save_gic();
   return 0;
}

static void hi_hiber_finish(void)
{
	restore_gic();
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

static int __init hi_pm_init(void)
{
	pm_power_off = hi_pm_power_off;

	suspend_set_ops(&hi_pm_ops);

#ifdef CONFIG_HIBERNATION
	/* registering hibernation call backs */
	hibernation_set_ops(&hi_hibernation_ops);
#endif
	return 0;
}

module_init(hi_pm_init);

