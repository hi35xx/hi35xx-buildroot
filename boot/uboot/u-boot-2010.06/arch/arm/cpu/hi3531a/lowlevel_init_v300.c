/******************************************************************************
 *    Copyright (c) 2009-2012 by Hisi.
 *    All rights reserved.
 * ***
 *
 *****************************************************************************/

#include <asm/arch/platform.h>
#include <config.h>
#ifdef CONFIG_DDR_TRAINING_V2
#include <ddr_interface.h>
#endif


#define DDR_DMC_BASE_REG0	0x12118000
#define DDR_DMC_BASE_REG1	0x12119000

extern void reset_cpu(unsigned long addr);

static inline void DWB(void) /* drain write buffer */
{
}

static inline unsigned int readl(unsigned addr)
{
	unsigned int val;
	val = (*(volatile unsigned int *)(addr));
	return val;
}

static inline void writel(unsigned val, unsigned addr)
{
	DWB();
	(*(volatile unsigned *) (addr)) = (val);
	DWB();
}

static inline void delay(unsigned int num)
{
	unsigned int i;
	for (i = 0; i < (100 * num); i++)
		__asm__ __volatile__("nop");
}

#ifdef	CONFIG_SVB_ENABLE
#define HI_PMC_CTL_REG		0x120e0000
#define HPM_REG_BASE		0x12060000
#define HPM_CORE_VOL_REG	0x120e0000
#define HPM_CPU_VOL_REG		0x120e0004
void start_core_cpu_hpm(unsigned int *core_aver, unsigned int *cpu_aver)
{
	unsigned int core_tmp0 = 0, core_tmp1 = 0, cpu_tmp0 = 0, cpu_tmp1 = 0;
	unsigned int u32Value0_CORE = 0, u32Value1_CORE = 0;
	unsigned int u32Value2_CORE = 0, u32Value3_CORE = 0;
	unsigned int u32Value0_CPU = 0, u32Value1_CPU = 0;
	unsigned int u32Value2_CPU = 0, u32Value3_CPU = 0;
	unsigned int aver_num, i;

	aver_num = 2;

	/* reset core hpm, cpu hpm, div frq */
	writel(0x0a000007, HI_PMC_CTL_REG + 0x28);
	writel(0x0a000015, HI_PMC_CTL_REG + 0x38);

	writel(0x05000007, HI_PMC_CTL_REG + 0x28);
	writel(0x05000015, HI_PMC_CTL_REG + 0x38);

	delay(50000);

	/* Core Hpm*/
	for (i = 0; i < aver_num; i++) {
		delay(500);

		core_tmp0 = readl(HI_PMC_CTL_REG + 0x2C);
		u32Value0_CORE += core_tmp0 & 0x3ff;
		u32Value1_CORE += (core_tmp0 >> 12) & 0x3ff;
		core_tmp1 = readl(HI_PMC_CTL_REG + 0x30);
		u32Value2_CORE += core_tmp1 & 0x3ff;
		u32Value3_CORE += (core_tmp1 >> 12) & 0x3ff;
	}

	*core_aver = (u32Value0_CORE + u32Value1_CORE +
			u32Value2_CORE + u32Value3_CORE) >> 3;

	/* CPU Hpm */
	for (i = 0; i < aver_num; i++) {
		delay(500);

		cpu_tmp0 = readl(HI_PMC_CTL_REG + 0x3C);
		u32Value0_CPU += cpu_tmp0 & 0x3ff;
		u32Value1_CPU += (cpu_tmp0>>12) & 0x3ff;
		cpu_tmp1 = readl(HI_PMC_CTL_REG + 0x40);
		u32Value2_CPU += cpu_tmp1 & 0x3ff;
		u32Value3_CPU += (cpu_tmp1>>12) & 0x3ff;
	}

	*cpu_aver = (u32Value0_CPU + u32Value1_CPU +
			u32Value2_CPU + u32Value3_CPU) >> 3;
}

unsigned int get_hpm_value(void)
{
	unsigned int reg_value;
	unsigned int cpl_flag;
	unsigned int i;
	unsigned int data[4];

	reg_value = 0;
	for (i = 0; i < 4; i++) {
		/*set read addr*/
		writel(4 + i, HPM_REG_BASE + 0xc);

		/*enable read*/
		writel(0x1, HPM_REG_BASE + 0x8);

		/*wait read completion*/
		do {
			cpl_flag = readl(HPM_REG_BASE + 0x10);
		} while (!(cpl_flag & 0x2));

		data[i] = readl(HPM_REG_BASE + 0x14);
		reg_value |= (data[i] & 0xff) << (i * 8);
	}

	return reg_value;
}

void set_hpm_cpu_volt_board(unsigned int hpm_cpu_value)
{
	if (hpm_cpu_value <= 310)
		writel(0x800c7, HPM_CPU_VOL_REG);
	else if (hpm_cpu_value <= 350)
		writel(0x2400c7, HPM_CPU_VOL_REG);
	else
		writel(0x5000c7, HPM_CPU_VOL_REG);
}

void set_hpm_core_volt_board(unsigned int hpm_core_value)
{
	if (hpm_core_value <= 210)
		writel(0x1000c7, HPM_CORE_VOL_REG);
	else if (hpm_core_value <= 245)
		writel(0x1600c7, HPM_CORE_VOL_REG);
	else
		writel(0x2d00c7, HPM_CORE_VOL_REG);
}

void set_hpm_core_volt(unsigned int hpm_core, unsigned int iddq_core)
{
	unsigned int val;

	if (hpm_core <= 246 || iddq_core <= 7)
		val = 0x1000c7;
	else if (hpm_core <= 281 || iddq_core <= 16)
		val = 0x2200c7;
	else if (hpm_core <= 310 || iddq_core <= 29)
		val = 0x3b00c7;
	else
		val = 0x4700c7;

	writel(val, HPM_CORE_VOL_REG);
}

void set_hpm_cpu_volt(unsigned int hpm_cpu, unsigned int iddq_cpu)
{
	unsigned int val;

	if (hpm_cpu <= 185 || iddq_cpu <= 1)
		val = 0x200c7;
	else if (hpm_cpu <= 206 || iddq_cpu <= 6)
		val = 0x2900c7;
	else if (hpm_cpu <= 228 || iddq_cpu <= 11)
		val = 0x5000c7;
	else
		val = 0x6b00c7;

	writel(val, HPM_CPU_VOL_REG);
}

void start_svb(void)
{
	unsigned int val, hpm_cpu, hpm_core, iddq_cpu, iddq_core;

	val = get_hpm_value();
	if (val) {
		hpm_core = val & 0x3ff;
		iddq_core = (val >> 10) & 0x3f;
		hpm_cpu = (val >> 16) & 0x3ff;
		iddq_cpu = (val >> 26) & 0x3f;

		set_hpm_core_volt(hpm_core, iddq_core);
		set_hpm_cpu_volt(hpm_cpu, iddq_cpu);
	} else {
		start_core_cpu_hpm(&hpm_core, &hpm_cpu);

		set_hpm_core_volt_board(hpm_core);
		set_hpm_cpu_volt_board(hpm_cpu);
	}
}
#endif

void start_ddr_training(unsigned int base)
{
#ifdef CONFIG_SVB_ENABLE
	/* add SVB function */
	start_svb();
#endif
#ifdef CONFIG_DDR_TRAINING_V2
	int ret = 0;

	/* ddr training function */
	ret = ddr_sw_training_if(0);
	if (ret)
		reset_cpu(0);
#endif

	/*the value should config after trainning, or
	  it will cause chip compatibility problems*/
	writel(0x401, DDR_DMC_BASE_REG0 + 0x28);
	writel(0x401, DDR_DMC_BASE_REG1 + 0x28);
}

