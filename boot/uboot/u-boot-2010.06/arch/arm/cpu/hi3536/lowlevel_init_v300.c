/******************************************************************************
 *    Copyright (c) 2009-2012 by Hisi.
 *    All rights reserved.
 * ***
 *
 ******************************************************************************/

#include <asm/arch/platform.h>
#include <config.h>
#ifdef CONFIG_DDR_TRAINING_V2
#include <ddr_interface.h>
#endif

#define HI_SYS_CTL_REG	0x12050000
#define HI_PMC_CTL_REG	0x120e0000

#define DDR0_BASE_REG	0x12118000
#define DDR1_BASE_REG	0x12119000

#define DDR0_PLL_REG	0x1211c000
#define DDR1_PLL_REG	0x1211e000

#define HPM_REG_BASE		0x12300000
#define HPM_CPU_VOL_REG		0x120e0000
#define HPM_CORE_VOL_REG	0x120e0004

static inline void delay(unsigned int num)
{
	volatile unsigned int i;
	for (i = 0; i < (100 * num); i++)
		__asm__ __volatile__("nop");
}

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

void start_core_cpu_hpm(void)
{
	unsigned int core_aver_recorder = 0, cpu_aver_recorder = 0;
	unsigned int core_tmp0 = 0, core_tmp1 = 0, cpu_tmp0 = 0, cpu_tmp1 = 0;
	unsigned int u32Value0_CORE = 0, u32Value1_CORE = 0;
	unsigned int u32Value2_CORE = 0, u32Value3_CORE = 0;
	unsigned int u32Value0_CPU = 0, u32Value1_CPU = 0;
	unsigned int u32Value2_CPU = 0, u32Value3_CPU = 0;
	unsigned int aver_num, i;

	aver_num = 2;

	/* reset core hpm, cpu hpm, div frq */
	writel(0x0a000007, HI_PMC_CTL_REG + 0x28);
	writel(0x0a000017, HI_PMC_CTL_REG + 0x38);

	writel(0x05000007, HI_PMC_CTL_REG + 0x28);
	writel(0x05000017, HI_PMC_CTL_REG + 0x38);

	delay(500);

	for (i = 0; i < aver_num; i++) {
		delay(500);

		core_tmp0 = readl(HI_PMC_CTL_REG + 0x2C);
		u32Value0_CORE += core_tmp0 & 0x3ff;
		u32Value1_CORE += (core_tmp0 >> 12) & 0x3ff;
		core_tmp1 = readl(HI_PMC_CTL_REG + 0x30);
		u32Value2_CORE += core_tmp1 & 0x3ff;
		u32Value3_CORE += (core_tmp1 >> 12) & 0x3ff;
	}

	core_aver_recorder = (u32Value0_CORE + u32Value1_CORE +
			u32Value2_CORE + u32Value3_CORE) >> 3;

	writel(core_aver_recorder, HI_SYS_CTL_REG + 0xA8);

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

	cpu_aver_recorder = (u32Value0_CPU + u32Value1_CPU +
			u32Value2_CPU + u32Value3_CPU) >> 3;

	writel(cpu_aver_recorder, HI_SYS_CTL_REG + 0xAC);
}

#define PWM0_CFG0 0x200e0000
#define PWM0_CFG1 0x200e0004
#define PWM1_CFG0 0x200e0020
#define PWM1_CFG1 0x200e0024
void start_svb(void)
{
	int voltage_tmp;
	unsigned int core_aver_recorder = 0, cpu_aver_recorder = 0;

	/* auto svb operation */
	cpu_aver_recorder = readl(0x2005009c);
	core_aver_recorder = readl(0x20050098);
	/* the first uboot table type */
	if (0xa7 == readl(PWM0_CFG0)) {
		if (cpu_aver_recorder < 430)
			writel(0x0000001b, PWM0_CFG1); /* 1.25V */
		else if (cpu_aver_recorder > 460)
			writel(0x00000043, PWM0_CFG1); /* 1.15V */
		else
			writel(0x0000002f, PWM0_CFG1); /* 1.20V */

		if (core_aver_recorder < 480)
			writel(0x00000015, PWM1_CFG1); /* 1.15V */
		else
			writel(0x00000029, PWM1_CFG1); /* 1.10V */
	}
	/* the second uboot table type */
	if (0x7b == readl(PWM0_CFG0)) {
		if (cpu_aver_recorder < 430)
			writel(0x0000001b, PWM1_CFG1); /* 1.25V */
		else if (cpu_aver_recorder > 460)
			writel(0x00000043, PWM1_CFG1); /* 1.15V */
		else
			writel(0x0000002f, PWM1_CFG1); /* 1.20V */

		if (core_aver_recorder < 480)
			writel(0x00000015, PWM0_CFG1); /* 1.15V */
		else
			writel(0x00000029, PWM0_CFG1); /* 1.10V */
	}

	/* manual svb operation */
	voltage_tmp = readl(0x200500a0);
	if (voltage_tmp)
		writel(voltage_tmp, PWM0_CFG0);
	voltage_tmp = readl(0x200500a4);
	if (voltage_tmp)
		writel(voltage_tmp, PWM0_CFG1);
	voltage_tmp = readl(0x200500a8);
	if (voltage_tmp)
		writel(voltage_tmp, PWM1_CFG0);
	voltage_tmp = readl(0x200500ac);
	if (voltage_tmp)
		writel(voltage_tmp, PWM1_CFG1);

	writel(0x00000005, 0x200e000c);
	writel(0x00000005, 0x200e002c);
}

unsigned int get_hpm_value(void)
{
	unsigned int reg_value;
	unsigned int cpl_flag;

	/*set the read addr*/
	writel(0x1, HPM_REG_BASE + 0x8);
	/*read enable*/
	writel(0x10, HPM_REG_BASE + 0x4);
	delay(1);

	/*wait read completion*/
	do {
		cpl_flag = readl(HPM_REG_BASE + 0x10);
	} while (!(cpl_flag & 0x10));

	reg_value = readl(HPM_REG_BASE + 0xc);

	return reg_value;
}

void set_hpm_cpu_vol(unsigned int hpm_cpu_value)
{
	if (hpm_cpu_value >= 198)
		writel(0x290078, HPM_CPU_VOL_REG);
	else if (hpm_cpu_value >= 178)
		writel(0x170078, HPM_CPU_VOL_REG);
	else
		writel(0x60078, HPM_CPU_VOL_REG);
}

void set_hpm_core_vol(unsigned int hpm_core_value)
{
	if (hpm_core_value >= 475) {
		writel(0x570078, HPM_CORE_VOL_REG);
	} else if (hpm_core_value >= 420) {
		writel(0x4e0078, HPM_CORE_VOL_REG);
	} else {
		writel(0x320078, HPM_CORE_VOL_REG);
	}
}

void hi3536_svb(void)
{
	unsigned int temp, temp1, temp2, temp3;
	unsigned int hpm, hpm_cpu, hpm_core;

	start_core_cpu_hpm();

	temp = readl(HI_SYS_CTL_REG + 0x9c);
	if (temp) {
		temp1 = (temp & 0xffff) << 16;
		temp2 = temp & 0xffff0000;

		temp3 = readl(HI_PMC_CTL_REG) & 0xffff;
		writel((temp3 | temp1), HI_PMC_CTL_REG);

		temp3 = readl(HI_PMC_CTL_REG + 0x4) & 0xffff;
		writel((temp3 | temp2), HI_PMC_CTL_REG + 0x4);
	} else {
		hpm = get_hpm_value();
		hpm_cpu = (hpm >> 16) & 0x3ff;
		hpm_core = hpm & 0x3ff;

		set_hpm_cpu_vol(hpm_cpu);
		set_hpm_core_vol(hpm_core);
	}

}

void hi3536_ddr_init(void)
{
	unsigned int temp1, temp2, temp3, temp4;

	temp3 = readl(HI_SYS_CTL_REG + 0xa0);
	temp4 = readl(HI_SYS_CTL_REG + 0xa4);

	if (temp3) {
		temp1 = readl(DDR0_BASE_REG + 0x108);
		writel((temp1 & 0xffff0000), DDR0_BASE_REG + 0x108);

		writel(temp3, DDR0_PLL_REG + 0x4);
		writel(0x2, DDR0_BASE_REG);
		while (1) {
			temp3 = readl(DDR0_PLL_REG + 0x4) & 0x1;

			if (!temp3)
				break;
		}

		writel(0x8000, DDR0_PLL_REG + 0x4);
		writel(0x0, DDR0_PLL_REG + 0x4);

		writel(temp1, DDR0_BASE_REG + 0x108);
	}


	if (temp4) {
		temp2 = readl(DDR1_BASE_REG + 0x108);
		writel((temp2 & 0xffff0000), DDR1_BASE_REG + 0x108);

		writel(temp4, DDR1_PLL_REG + 0x4);
		writel(0x2, DDR1_BASE_REG);
		while (1) {
			temp4 = readl(DDR1_PLL_REG + 0x4) & 0x1;

			if (!temp4)
				break;
		}

		writel(0x8000, DDR1_PLL_REG + 0x4);
		writel(0x0, DDR1_PLL_REG + 0x4);

		writel(temp2, DDR1_BASE_REG + 0x108);
	}
}

void start_ddr_training(unsigned int base)
{
	hi3536_svb();

	delay(500);

	hi3536_ddr_init();

#ifdef CONFIG_DDR_TRAINING_V2
	int ret = 0;

	/* ddr training function */
	ret = ddr_sw_training_if(0);
	if (ret)
		reset_cpu(0);
#endif
	/*the value should config after trainning, or
	  it will cause chip compatibility problems*/
	writel(0x401, DDR0_BASE_REG + 0x28);
	writel(0x401, DDR1_BASE_REG + 0x28);
}
