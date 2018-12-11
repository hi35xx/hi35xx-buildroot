/**
 * cpuidle-hi3519.c
 *
 * Copyright (c) 2009-2014, HiSilicon Technologies Co., Ltd.
 * All rights reserved.
 */

#include <linux/bitmap.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/clockchips.h>
#include <linux/debugfs.h>
#include <linux/hrtimer.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/tick.h>
#include <linux/vexpress.h>
#include <linux/cpuidle.h>
#include <asm/cputype.h>
#include <asm/idmap.h>
#include <asm/proc-fns.h>
#include <asm/suspend.h>
#include <linux/of.h>
#include <asm/smp_plat.h>

#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/cp15.h>
#include <asm/arch_timer.h>

#include <linux/irqchip/arm-gic.h>
#include <linux/arm-cci.h>
#include <linux/delay.h>


/* extern functions */
extern void hi3519_set_cpu_jump(int cpu, phys_addr_t jumpaddr);
extern void hi3519_cpu_resume(void);
extern void hi_pmc_set_ac_inactive(void);
extern void hi_pmc_automode_power_down(void);
extern void hi_pmc_power_up_done(void);

static int bl_cpuidle_simple_enter(struct cpuidle_device *dev,
		struct cpuidle_driver *drv, int index);

int bl_cpuidle_simple_enter(struct cpuidle_device *dev,
		struct cpuidle_driver *drv, int index)
{
#if defined(CPUIDLE_DEBUG)
	int cpuid = smp_processor_id();

	pr_debug("%s cpu:%d enter", __func__, cpuid);
#endif
	cpu_do_idle();
	return index;
}

static int bl_enter_cpu_powerdown(struct cpuidle_device *dev,
				struct cpuidle_driver *drv, int idx);


static struct cpuidle_state bl_cpuidle_set[] __initdata = {
	[0] = {
		.enter                  = bl_cpuidle_simple_enter,
		.exit_latency           = 1,
		.target_residency       = 1,
		.power_usage		= UINT_MAX,
		.flags                  = CPUIDLE_FLAG_TIME_VALID,
		.name                   = "WFI",
		.desc                   = "ARM WFI",
	},
	[1] = {
		.enter			= bl_enter_cpu_powerdown,
		.exit_latency		= 500,
		.target_residency	= 1000,
		.flags			= CPUIDLE_FLAG_TIME_VALID |
					  CPUIDLE_FLAG_TIMER_STOP,
		.name			= "C1",
		.desc			= "ARM cpu A17 Cluster power down",
	},
};

static struct cpuidle_driver bl_idle_driver = {
	.name = "bl_idle",
	.owner = THIS_MODULE,
	.safe_state_index = 0
};

static DEFINE_PER_CPU(struct cpuidle_device, bl_idle_dev);

static void bl_cpu_smp_disable(void)
{
	/* Set ACTLR.SMP to 0, AMP -> SMP */
	asm volatile (
	"	mrc	p15, 0, r0, c1, c0, 1\n"
	"	bic	r0,  #0x40\n"
	"	mcr	p15, 0, r0, c1, c0, 1\n"
	  :
	  :
	  : "r0", "cc");
}

/*
 * switch to init_mm, init_mm page table entries must in memory
 * not in the cpu cache
 */
static void setup_mm_for_idle(void)
{
	struct mm_struct *mm = &init_mm;

	cpu_switch_mm(mm->pgd, mm);
	local_flush_bp_all();
	local_flush_tlb_all();
}

static void bl_cpu_powerdown(u64 expected_residency)
{
	int cpu = smp_processor_id();
	/* disable irq */
	if (WARN(!irqs_disabled(), "Interrupts should be disabled\n"))
		local_irq_disable();

	/*
	 * Once the Dcache is disabled and the tlb is empty, the ptw will
	 * fetch page table entries for the code after Dcache disable from
	 * memory, but the mm is active_mm from last user process's mm, and
	 * the mm->pgd may located in cpu cache(see dcache_clean_area), so
	 * switch to init_mm to avoid access fault.
	 */
	setup_mm_for_idle();
	/* move the power code here to measure the idle enter time */
	hi_pmc_automode_power_down();

	gic_cpu_if_down();

	/* close Dcache */
	set_cr(get_cr() & ~CR_C);
	/* CLREX */
	asm volatile ("clrex");

	/* Clean & Invalidata L1 Data Cache, L2 Cache */
/*	for cortex a17 just one single instruction is enough
	flush_cache_all();
*/
	/* clean&invalidate l1 cache */
	asm volatile("mov r0, #0");
	asm volatile("mcr p15, 1, r0, c15, c14, 0");
	asm volatile("dsb");

	/* clean&invalidate l2 cache */
	asm volatile("mov r0, #2");
	asm volatile("mcr p15, 1, r0, c15, c14, 0");
	asm volatile("dsb");

	/* switch SMP to AMP(ACTLR.SMP->1'b0) */
	bl_cpu_smp_disable();

	/* disable cci snoop */
	cci_disable_port_by_cpu(cpu_logical_map(cpu));

	/* asm volatile("mcr p14, 0, %0, c1, c3, 0" : : "r" (1)); */

	/* ISB & DSB */
	isb();
	dsb();

	hi_pmc_set_ac_inactive();
/*
	hi_pmc_automode_power_down();
*/
	dsb();
	/* WFI */
	while (1)
		wfi();

	BUG();
}

static int bl_cpu_powered_up(void)
{
	/*dcache enble*/
	set_cr(get_cr() | CR_C);

	hi_pmc_power_up_done();

	return 0;
}


static int notrace bl_cpu_powerdown_finisher(unsigned long arg)
{
	hi3519_set_cpu_jump(smp_processor_id(),
				(phys_addr_t)virt_to_phys(hi3519_cpu_resume));

	bl_cpu_powerdown(0);

	return 1;
}

/*
 * bl_enter_cpu_powerdown - Programs CPU to enter the specified state
 * @dev: cpuidle device
 * @drv: The target state to be programmed
 * @idx: state index
 *
 * Called from the CPUidle framework to program the device to the
 * specified target state selected by the governor.
 */
int bl_enter_cpu_powerdown(struct cpuidle_device *dev,
				struct cpuidle_driver *drv, int idx)
{
	int cpuid = smp_processor_id();

	/* A7 can not power down */
	if (cpuid == 0) {
		cpu_do_idle();
		return idx;
	}

	BUG_ON(!irqs_disabled());

	cpu_pm_enter();

	cpu_suspend((unsigned long) dev, bl_cpu_powerdown_finisher);

	bl_cpu_powered_up();

	cpu_pm_exit();

	return idx;
}
/*
 * bl_idle_init
 *
 * Registers the bl specific cpuidle driver with the cpuidle
 * framework with the valid set of states.
 */

static int __init bl_idle_init(void)
{
	struct cpuidle_device *dev;
	int i, cpu_id;
	struct cpuidle_driver *drv = &bl_idle_driver;

	drv->state_count = (sizeof(bl_cpuidle_set) /
			sizeof(struct cpuidle_state));


	for (i = 0; i < drv->state_count; i++) {
		memcpy(&drv->states[i], &bl_cpuidle_set[i],
				sizeof(struct cpuidle_state));
	}
	cpuidle_register_driver(drv);

	for_each_cpu(cpu_id, cpu_online_mask) {
		/* cpu 0 use default idle */
		if (cpu_id == 0)
			continue;
		pr_err("CPUidle for CPU%d registered\n", cpu_id);
		dev = &per_cpu(bl_idle_dev, cpu_id);
		dev->cpu = cpu_id;

		if (cpuidle_register_device(dev)) {
			pr_err("%s: Cpuidle register device failed\n",
					__func__);
			return -EIO;
		}
	}


	return 0;
}

device_initcall(bl_idle_init);
