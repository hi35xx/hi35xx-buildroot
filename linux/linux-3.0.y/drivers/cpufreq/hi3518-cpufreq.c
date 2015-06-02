/*
 * drivers/cpufreq/hi3518-cpufreq.c
 *
 * Copyright (C) 2012 HiSilicon Technologies Co.,Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/suspend.h>

#include <asm/system.h>

#include <mach/hardware.h>

#define DFS_SKIP_MAX		(32)
#define REG_CRG1_OFFSET		IO_ADDRESS(CRG_REG_BASE + 0x0)
#define REG_CRG2_OFFSET		IO_ADDRESS(CRG_REG_BASE + 0x4)
#define REG_CRG10_OFFSET	IO_ADDRESS(CRG_REG_BASE + 0x28)

/*
 * Get CPU frequency
 * CPU clock comes from APLL.
 */
#define HW_REG(a) (*(unsigned long *)(a))
#define get_cpu_clk() ({\
	unsigned long fbdiv, refdiv, pstdiv1, pstdiv2;\
	unsigned long tmp_reg, foutvco, cpuclk;\
	tmp_reg = HW_REG(REG_CRG1_OFFSET);\
	pstdiv1 = (tmp_reg >> 24) & 0x7;\
	pstdiv2 = (tmp_reg >> 27) & 0x7;\
	tmp_reg = HW_REG(REG_CRG2_OFFSET);\
	refdiv = (tmp_reg >> 12) & 0x3f;\
	fbdiv = tmp_reg & 0xfff;\
	foutvco = 24000000 / refdiv;\
	foutvco *= fbdiv;\
	cpuclk = foutvco/(pstdiv1 * pstdiv2);\
	cpuclk;\
	})

/*static struct cpufreq_frequency_table freq_table[DFS_SKIP_MAX+1] = {0};*/
static struct cpufreq_frequency_table freq_table[DFS_SKIP_MAX+1];

static int hi3518_set_cpu_speed(unsigned int idx)
{
	unsigned int reg, val;

	val = DFS_SKIP_MAX-(idx+1);
	reg = readl(REG_CRG10_OFFSET);
	reg &= (~(0x3f<<5));
	reg |= (val<<5);
	/* step 1 */
	writel(reg, REG_CRG10_OFFSET);

	/* step 2 */
	/* reg &= (~(1<<10)); */
	writel(reg, REG_CRG10_OFFSET);

	/* step 3 */
	reg |= (1<<10);
	writel(reg, REG_CRG10_OFFSET);

	return 0;
}

int hi3518_verify_speed(struct cpufreq_policy *policy)
{
	return cpufreq_frequency_table_verify(policy, freq_table);
}

unsigned int hi3518_getspeed(unsigned int cpu)
{
	unsigned int rate;
	unsigned int index, val;

	if (cpu != 0)
		return 0;

	val = readl(REG_CRG10_OFFSET);
	index = (val >> 5) & 0x1f;
	index = DFS_SKIP_MAX - (index + 1);
	rate = freq_table[index].frequency;

	return rate;
}

static int hi3518_target(struct cpufreq_policy *policy,
		       unsigned int target_freq,
		       unsigned int relation)
{
	struct cpufreq_freqs freqs;
	unsigned int idx;

	/* scale the target frequency to one of the extremes supported */
	if (target_freq < policy->cpuinfo.min_freq)
		target_freq = policy->cpuinfo.min_freq;
	if (target_freq > policy->cpuinfo.max_freq)
		target_freq = policy->cpuinfo.max_freq;

	/* Lookup the next frequency */
	if (cpufreq_frequency_table_target
			(policy, freq_table, target_freq, relation, &idx)) {
		return -EINVAL;
	}

	freqs.old = policy->cur;
	freqs.new = freq_table[idx].frequency;
	freqs.cpu = 0;

	if (freqs.old == freqs.new)
		return 0;

	/* pre-change notification */
	cpufreq_notify_transition(&freqs, CPUFREQ_PRECHANGE);

	if (hi3518_set_cpu_speed(idx)) {
		pr_err("hi3518-cpufreq:  Failed to set freq\n");
		return -EINVAL;
	}

	/* post change notification */
	cpufreq_notify_transition(&freqs, CPUFREQ_POSTCHANGE);

	return 0;
}

static int hi3518_cpu_init(struct cpufreq_policy *policy)
{
	int res;

	if (policy->cpu != 0)
		return -EINVAL;

	/* get policy fields based on the table */
	res = cpufreq_frequency_table_cpuinfo(policy, freq_table);
	if (!res)
		cpufreq_frequency_table_get_attr(freq_table, policy->cpu);
	else {
		pr_err("hi3518-cpufreq : Failed to read policy table\n");
		return res;
	}

	policy->min = policy->cpuinfo.min_freq;
	policy->max = policy->cpuinfo.max_freq;
	policy->cur = hi3518_getspeed(0);

	/* ASIC TODO
	 * Setting the latency to 1000 us to switch between two frequencies
	 */
	policy->cpuinfo.transition_latency = 1000 * 1000;

	return 0;
}

static int hi3518_cpu_exit(struct cpufreq_policy *policy)
{
	cpufreq_frequency_table_put_attr(policy->cpu);
	return 0;
}

static struct freq_attr *hi3518_cpufreq_attr[] = {
	&cpufreq_freq_attr_scaling_available_freqs,
	NULL,
};

static struct cpufreq_driver hi3518_cpufreq_driver = {
	.name		= "hi3518",
	.owner          = THIS_MODULE,
	.verify		= hi3518_verify_speed,
	.target		= hi3518_target,
	.get		= hi3518_getspeed,
	.init		= hi3518_cpu_init,
	.exit		= hi3518_cpu_exit,
	.attr		= hi3518_cpufreq_attr,
};

static int __init hi3518_cpufreq_init(void)
{
	int i;
	unsigned long cpuclk;

	cpuclk = get_cpu_clk() / 1000;
	for (i = 0; i < DFS_SKIP_MAX; i++) {
		freq_table[i].index = i;
		freq_table[i].frequency = cpuclk * (i + 1) / DFS_SKIP_MAX;
	}
	freq_table[DFS_SKIP_MAX].index = DFS_SKIP_MAX;
	freq_table[DFS_SKIP_MAX].frequency = CPUFREQ_TABLE_END;

	return cpufreq_register_driver(&hi3518_cpufreq_driver);
}

static void __exit hi3518_cpufreq_exit(void)
{
	cpufreq_unregister_driver(&hi3518_cpufreq_driver);
}

MODULE_AUTHOR("Chuck Peng");
MODULE_DESCRIPTION("cpufreq driver for Hi3518");
MODULE_LICENSE("GPL");

module_init(hi3518_cpufreq_init);
module_exit(hi3518_cpufreq_exit);
