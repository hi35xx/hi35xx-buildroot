/*
 * Copyright (c) 2013 Linaro Ltd.
 * Copyright (c) 2013 Hisilicon Limited.
 * Based on arch/arm/mach-vexpress/platsmp.c, Copyright (C) 2002 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include <asm/smp_scu.h>

#if defined(CONFIG_ARCH_HI3536C)
#include <mach/io.h>
#include <mach/platform.h>
#endif

#include "core.h"

#define HIX5HD2_BOOT_ADDRESS		0xffff0000
#define HI3519_BOOT_ADDRESS		0x00000000
#define HI3559_BOOT_ADDRESS		0x00000000
#define HI3516AV200_BOOT_ADDRESS	0x00000000
#define HI3536C_BOOT_ADDRESS	0x00000000

static void __iomem *hi3519_bootaddr;
static void __iomem *hi3559_bootaddr;
static void __iomem *hi3516av200_bootaddr;
static void __iomem *hi3536c_bootaddr;
static void __iomem *ctrl_base;
static DEFINE_SPINLOCK(boot_lock);

static void __cpuinit hi_write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
}

void hi3xxx_set_cpu_jump(int cpu, void *jump_addr)
{
	cpu = cpu_logical_map(cpu);
	if (!cpu || !ctrl_base)
		return;
	writel_relaxed(virt_to_phys(jump_addr), ctrl_base + ((cpu - 1) << 2));
}

int hi3xxx_get_cpu_jump(int cpu)
{
	cpu = cpu_logical_map(cpu);
	if (!cpu || !ctrl_base)
		return 0;
	return readl_relaxed(ctrl_base + ((cpu - 1) << 2));
}

static void __init hisi_enable_scu_a9(void)
{
	unsigned long base = 0;
	void __iomem *scu_base = NULL;

	if (scu_a9_has_base()) {
		base = scu_a9_get_base();
		scu_base = ioremap(base, SZ_4K);
		if (!scu_base) {
			pr_err("ioremap(scu_base) failed\n");
			return;
		}
		scu_enable(scu_base);
		iounmap(scu_base);
	}
}

static void __init hi3xxx_smp_prepare_cpus(unsigned int max_cpus)
{
	struct device_node *np = NULL;
	u32 offset = 0;

	hisi_enable_scu_a9();
	if (!ctrl_base) {
		np = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
		if (!np) {
			pr_err("failed to find hisilicon,sysctrl node\n");
			return;
		}
		ctrl_base = of_iomap(np, 0);
		if (!ctrl_base) {
			pr_err("failed to map address\n");
			return;
		}
		if (of_property_read_u32(np, "smp-offset", &offset) < 0) {
			pr_err("failed to find smp-offset property\n");
			return;
		}
		ctrl_base += offset;
	}
}

static int hi3xxx_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	hi3xxx_set_cpu(cpu, true);
	hi3xxx_set_cpu_jump(cpu, secondary_startup);
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	return 0;
}

struct smp_operations hi3xxx_smp_ops __initdata = {
	.smp_prepare_cpus	= hi3xxx_smp_prepare_cpus,
	.smp_boot_secondary	= hi3xxx_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hi3xxx_cpu_die,
	.cpu_kill		= hi3xxx_cpu_kill,
#endif
};

static void __init hix5hd2_smp_prepare_cpus(unsigned int max_cpus)
{
	hisi_enable_scu_a9();
}

void hix5hd2_set_scu_boot_addr(phys_addr_t start_addr, phys_addr_t jump_addr)
{
	void __iomem *virt;

	virt = ioremap(start_addr, PAGE_SIZE);

	writel_relaxed(0xe51ff004, virt);	/* ldr pc, [rc, #-4] */
	writel_relaxed(jump_addr, virt + 4);	/* pc jump phy address */
	iounmap(virt);
}

static int hix5hd2_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	phys_addr_t jumpaddr;

	jumpaddr = virt_to_phys(hix5hd2_secondary_startup);
	hix5hd2_set_scu_boot_addr(HIX5HD2_BOOT_ADDRESS, jumpaddr);
	hix5hd2_set_cpu(cpu, true);
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	return 0;
}


struct smp_operations hix5hd2_smp_ops __initdata = {
	.smp_prepare_cpus	= hix5hd2_smp_prepare_cpus,
	.smp_boot_secondary	= hix5hd2_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hix5hd2_cpu_die,
#endif
};

void hi3519_set_cpu_jump(unsigned int cpu, phys_addr_t jumpaddr)
{
	/* only cortex-a17 boot from phys 0 address */
	if (cpu != 1)
		return;
	/* ldr pc, [rc, #-4] */
	writel_relaxed(0xe51ff004, hi3519_bootaddr);
	/* pc jump phy address */
	writel_relaxed(jumpaddr, hi3519_bootaddr + 4);

	dsb();
}

void hi3516av200_set_cpu_jump(unsigned int cpu, phys_addr_t jumpaddr)
{
	/* only cortex-a17 boot from phys 0 address */
	if (cpu != 1)
		return;
	/* ldr pc, [rc, #-4] */
	writel_relaxed(0xe51ff004, hi3516av200_bootaddr);
	/* pc jump phy address */
	writel_relaxed(jumpaddr, hi3516av200_bootaddr + 4);

	dsb();
}

void hi3559_set_cpu_jump(unsigned int cpu, phys_addr_t jumpaddr)
{
	/* only cortex-a17 boot from phys 0 address */
	if (cpu != 1)
		return;
	/* ldr pc, [rc, #-4] */
	writel_relaxed(0xe51ff004, hi3559_bootaddr);
	/* pc jump phy address */
	writel_relaxed(jumpaddr, hi3559_bootaddr + 4);

	dsb();
}

static void __init hi3519_smp_prepare_cpus(unsigned int max_cpus)
{
	if (!hi3519_bootaddr)
		hi3519_bootaddr = ioremap(HI3519_BOOT_ADDRESS, PAGE_SIZE);

	sync_cache_w(&hi3519_bootaddr);
}

static void __init hi3516av200_smp_prepare_cpus(unsigned int max_cpus)
{
	if (!hi3516av200_bootaddr)
		hi3516av200_bootaddr = ioremap(HI3516AV200_BOOT_ADDRESS, PAGE_SIZE);

	sync_cache_w(&hi3516av200_bootaddr);
}

static void __init hi3559_smp_prepare_cpus(unsigned int max_cpus)
{
	if (!hi3559_bootaddr)
		hi3559_bootaddr = ioremap(HI3559_BOOT_ADDRESS, PAGE_SIZE);

	sync_cache_w(&hi3559_bootaddr);
}

static int hi3519_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	flush_cache_all();

	hi3519_set_cpu_jump(cpu, virt_to_phys(hi3519_secondary_startup));

	hi_pmc_power_up();

	return 0;
}

static int hi3516av200_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	flush_cache_all();

	hi3516av200_set_cpu_jump(cpu, virt_to_phys(hi3516av200_secondary_startup));

	hi_pmc_power_up();

	return 0;
}

static int hi3559_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	flush_cache_all();

	hi3559_set_cpu_jump(cpu, virt_to_phys(hi3559_secondary_startup));

	hi_pmc_power_up();

	return 0;
}

static void HI3519_secondary_init(unsigned int cpu)
{
/*
	hi_pmc_power_up_done();
*/
}

static void HI3516av200_secondary_init(unsigned int cpu)
{
/*
	hi_pmc_power_up_done();
*/
}

static void HI3559_secondary_init(unsigned int cpu)
{
/*
	hi_pmc_power_up_done();
*/
}

void hi3536c_set_cpu_jump(unsigned int cpu, phys_addr_t jumpaddr)
{
	/* only cortex-a17 boot from phys 0 address */
	if (cpu != 1)
		return;
	/* ldr pc, [rc, #-4] */
	writel_relaxed(0xe51ff004, hi3536c_bootaddr);
	/* pc jump phy address */
	writel_relaxed(jumpaddr, hi3536c_bootaddr + 4);

	dsb();
}

static void __iomem *scu_base_addr(void)
{
#ifdef CONFIG_ARCH_HI3536C
	return __io_address(A7_PERI_BASE + REG_A7_PERI_SCU);
#endif
	return NULL;
}

static void __init hi3536c_smp_prepare_cpus(unsigned int max_cpus)
{
	void __iomem *base = NULL;

	base = scu_base_addr();
	if (!base)
		return;

	scu_enable(base);
}

/*****************************************************************************/
/*
 * copy startup code to sram, and flash cache.
 * @start_addr: slave start phy address
 * @jump_addr: slave jump phy address
 */
static void hi3536c_set_scu_boot_addr(unsigned int start_addr,
		unsigned int jump_addr)
{
	unsigned int *virtaddr;
	unsigned int *p_virtaddr;

	p_virtaddr = virtaddr = ioremap(start_addr, PAGE_SIZE);

	*p_virtaddr++ = 0xe51ff004; /* ldr  pc, [pc, #-4] */
	*p_virtaddr++ = jump_addr;  /* pc jump phy address */

	smp_wmb();

	__cpuc_flush_dcache_area((void *)virtaddr,
			(size_t)((char *)p_virtaddr - (char *)virtaddr));
	outer_clean_range(__pa(virtaddr), __pa(p_virtaddr));

	iounmap(virtaddr);
}

static int __cpuinit hi3536c_boot_secondary(unsigned int cpu,
		struct task_struct *idle)
{
	unsigned long timeout;

	hi3536c_set_scu_boot_addr(0x00000000,
			(unsigned int)virt_to_phys(hi3536c_secondary_startup));

	/*
	 * set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	hi3536c_scu_power_up(cpu);
	/*
	 * The secondary processor is waiting to be released from
	 * the holding pen - release it, then wait for it to flag
	 * that it has been released by resetting pen_release.
	 *
	 * Note that "pen_release" is the hardware CPU ID, whereas
	 * "cpu" is Linux's internal ID.
	 */
	hi_write_pen_release(cpu);

	/*
	 * Send the secondary CPU a soft interrupt, thereby causing
	 * the boot monitor to read the system wide flags register,
	 * and branch to the address found there.
	 */
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));

	/*
	 * Send the secondary CPU a soft interrupt, thereby causing
	 * the boot monitor to read the system wide flags register,
	 * and branch to the address found there.
	 */
	timeout = jiffies + (5 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

static void __cpuinit hi3536c_secondary_init(unsigned int cpu)
{
	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	hi_write_pen_release(-1);

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

static void __init hi3536c_smp_init_cpus(void)
{
	unsigned int i, ncores, l2ctlr;

	asm volatile("mrc p15, 1, %0, c9, c0, 2\n" : "=r" (l2ctlr));
	ncores = ((l2ctlr >> 24) & 0x3) + 1;

	/* sanity check */
	if (ncores > NR_CPUS) {
		printk(KERN_WARNING
				"Realview: no. of cores (%d) greater than configured "
				"maximum of %d - clipping\n",
				ncores, NR_CPUS);
		ncores = NR_CPUS;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
}

struct smp_operations hi3536c_smp_ops __initdata = {
	.smp_init_cpus = hi3536c_smp_init_cpus,
	.smp_prepare_cpus = hi3536c_smp_prepare_cpus,
	.smp_secondary_init = hi3536c_secondary_init,
	.smp_boot_secondary = hi3536c_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die			= hi3536c_cpu_die,
	.cpu_kill			= hi3536c_cpu_kill,
#endif
};

struct smp_operations hi3519_smp_ops __initdata = {
	.smp_prepare_cpus	= hi3519_smp_prepare_cpus,
	.smp_boot_secondary	= hi3519_boot_secondary,
	.smp_secondary_init	= HI3519_secondary_init,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hi3519_cpu_die,
	.cpu_kill		= hi3519_cpu_kill,
#endif
};

struct smp_operations hi3516av200_smp_ops __initdata = {
	.smp_prepare_cpus	= hi3516av200_smp_prepare_cpus,
	.smp_boot_secondary	= hi3516av200_boot_secondary,
	.smp_secondary_init	= HI3516av200_secondary_init,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hi3516av200_cpu_die,
	.cpu_kill		= hi3516av200_cpu_kill,
#endif
};

struct smp_operations hi3559_smp_ops __initdata = {
	.smp_prepare_cpus	= hi3559_smp_prepare_cpus,
	.smp_boot_secondary	= hi3559_boot_secondary,
	.smp_secondary_init	= HI3559_secondary_init,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= hi3559_cpu_die,
	.cpu_kill		= hi3559_cpu_kill,
#endif
};


CPU_METHOD_OF_DECLARE(hi3xxx_smp, "hisilicon,hi3620-smp", &hi3xxx_smp_ops);
CPU_METHOD_OF_DECLARE(hix5hd2_smp, "hisilicon,hix5hd2-smp", &hix5hd2_smp_ops);
CPU_METHOD_OF_DECLARE(hi3519_smp, "hisilicon,hi3519-smp", &hi3519_smp_ops);
CPU_METHOD_OF_DECLARE(hi3559_smp, "hisilicon,hi3559-smp", &hi3559_smp_ops);
CPU_METHOD_OF_DECLARE(hi3516av200_smp, "hisilicon,hi3516av200-smp", &hi3516av200_smp_ops);
CPU_METHOD_OF_DECLARE(hi3536c_smp, "hisilicon,hi3536c-smp", &hi3536c_smp_ops);
