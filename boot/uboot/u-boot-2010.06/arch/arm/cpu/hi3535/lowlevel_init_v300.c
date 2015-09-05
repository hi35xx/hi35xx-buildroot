/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <asm/arch/platform.h>
#include <config.h>

#ifdef CONFIG_DDR_DATAEYE_TRAINING_STARTUP
extern int ddr_dataeye_training(void *param);
#endif /* CONFIG_DDR_DATAEYE_TRAINING_STARTUP */

extern void ddr_training_info(int value);
extern int ddrphy_train_route(void);

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

static inline void delay(void)
{
	__asm__ __volatile__("nop");
}

#ifdef	CONFIG_SVB_ENABLE
void start_core_cpu_hpm(void)
{
	unsigned int core_aver_recorder = 0, cpu_aver_recorder = 0;
	unsigned int core_tmp0 = 0, core_tmp1 = 0, cpu_tmp0 = 0, cpu_tmp1 = 0;
	unsigned int u32Value0_CORE = 0, u32Value1_CORE = 0;
	unsigned int u32Value2_CORE = 0, u32Value3_CORE = 0;
	unsigned int u32Value0_CPU = 0, u32Value1_CPU = 0;
	unsigned int u32Value2_CPU = 0, u32Value3_CPU = 0;
	unsigned int aver_num, i;

	aver_num = 5;

	/* reset core hpm, cpu hpm, div frq */
	writel(0x0a00000a, 0x2003012c);
	writel(0x02000013, 0x2003013c);

	/* Core Hpm */
	for (i = 0; i < aver_num; i++) {
		writel(0x0200000a, 0x2003012c);
		writel(0x0300000a, 0x2003012c);
		while ((readl(0x20030130) & 0x400) == 0)
			;
	}

	for (i = 0; i < aver_num; i++) {
		writel(0x0200000a, 0x2003012c);
		writel(0x0300000a, 0x2003012c);
		while ((readl(0x20030130) & 0x400) == 0)
			;
		core_tmp0 = readl(0x20030130);
		u32Value0_CORE += core_tmp0 & 0x3ff;
		u32Value1_CORE += (core_tmp0 >> 12) & 0x3ff;
		core_tmp1 = readl(0x20030134);
		u32Value2_CORE += core_tmp1 & 0x3ff;
		u32Value3_CORE += (core_tmp1 >> 12) & 0x3ff;
	}

	core_aver_recorder = (u32Value0_CORE + u32Value1_CORE +
			u32Value2_CORE + u32Value3_CORE) / aver_num / 4;

	writel(core_aver_recorder, 0x20050098);

	/* CPU Hpm */
	for (i = 0; i < aver_num; i++) {
		writel(0x0a000013, 0x2003013c);
		writel(0x0b000013, 0x2003013c);
		while ((readl(0x20030140) & 0x400) == 0)
			;
	}

	for (i = 0; i < aver_num; i++) {
		writel(0x0a000013, 0x2003013c);
		writel(0x0b000013, 0x2003013c);
		while ((readl(0x20030140) & 0x400) == 0)
			;

		cpu_tmp0 = readl(0x20030140);
		u32Value0_CPU += cpu_tmp0 & 0x3ff;
		u32Value1_CPU += (cpu_tmp0>>12) & 0x3ff;
		cpu_tmp1 = readl(0x20030144);
		u32Value2_CPU += cpu_tmp1 & 0x3ff;
		u32Value3_CPU += (cpu_tmp1>>12) & 0x3ff;
	}

	cpu_aver_recorder = (u32Value0_CPU + u32Value1_CPU +
			u32Value2_CPU + u32Value3_CPU) / aver_num / 4;

	writel(cpu_aver_recorder, 0x2005009c);
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
#endif

void start_ddr_training(unsigned int base)
{
	int ret = 0;

#ifdef	CONFIG_SVB_ENABLE
	/* add SVB function */
	start_core_cpu_hpm();
	start_svb();
#endif

	/* ddr training function */
	if (!(readl(base + REG_SC_GEN20) & 0x1)) {
			ret = ddrphy_train_route();
			if (ret) {
				ddr_training_info(ret);
				reset_cpu(0);
			}
		}

#ifdef CONFIG_DDR_DATAEYE_TRAINING_STARTUP
	if (!(readl(base + REG_SC_GEN20) & (0x1 << 16))) {
			ret = ddr_dataeye_training(0);
			if (ret)
				reset_cpu(0);
		}
#endif /* CONFIG_DDR_DATAEYE_TRAINING_STARTUP */

	/*the value should config after trainning, or
	 it will cause chip compatibility problems*/
	writel(0x401, 0x20111028);
}
