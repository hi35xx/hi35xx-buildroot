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


#define DDR_DMC_BASE_REG0	0x12118000
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
unsigned int g_core_aver_recorder;
unsigned int g_cpu_aver_recorder;
#define HI_PMC_CTL_REG    0x120e0000
#define HI_SYS_CTL_REG    0x12050000
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

	writel(0x5000c7, HI_PMC_CTL_REG);
	writel(0x2800c7, HI_PMC_CTL_REG + 0x4);

	writel(0x5000004, HI_PMC_CTL_REG + 0x28);
	writel(0x5000015, HI_PMC_CTL_REG + 0x58);

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

	core_aver_recorder = (u32Value0_CORE + u32Value1_CORE +
			u32Value2_CORE + u32Value3_CORE) >> 3;

	writel(core_aver_recorder, HI_SYS_CTL_REG + 0xA8);

	/* CPU Hpm */
	for (i = 0; i < aver_num; i++) {
		delay(500);

		cpu_tmp0 = readl(HI_PMC_CTL_REG + 0x5C);
		u32Value0_CPU += cpu_tmp0 & 0x3ff;
		u32Value1_CPU += (cpu_tmp0>>12) & 0x3ff;
		cpu_tmp1 = readl(HI_PMC_CTL_REG + 0x60);
		u32Value2_CPU += cpu_tmp1 & 0x3ff;
		u32Value3_CPU += (cpu_tmp1>>12) & 0x3ff;
	}

	cpu_aver_recorder = (u32Value0_CPU + u32Value1_CPU +
			u32Value2_CPU + u32Value3_CPU) >> 3;

	writel(cpu_aver_recorder, HI_SYS_CTL_REG + 0xAC);

	g_core_aver_recorder = core_aver_recorder;
	g_cpu_aver_recorder = cpu_aver_recorder;
}

void start_svb(void)
{
	unsigned int core_aver_recorder = 0, cpu_aver_recorder = 0;

	/* auto svb operation */
	cpu_aver_recorder = g_cpu_aver_recorder;
	core_aver_recorder = g_core_aver_recorder;

	if (core_aver_recorder >=  215)
		writel(0x7400c7, HI_PMC_CTL_REG); /* 1.05V */
	else if (core_aver_recorder >= 185)
		writel(0x5800c7, HI_PMC_CTL_REG); /* 1.12V */
	else
		writel(0x4c00c7, HI_PMC_CTL_REG); /* 1.15V */

	if (cpu_aver_recorder >= 330)
		writel(0x6000c7, HI_PMC_CTL_REG + 0x4); /* 1.15V */
	else if (cpu_aver_recorder >= 285)
		writel(0x3800c7, HI_PMC_CTL_REG + 0x4); /* 1.25V */
	else
		writel(0x2400c7, HI_PMC_CTL_REG + 0x4); /* 1.30V */

	delay(50000);

}


void start_20dv300_core_hpm(void)
{
	unsigned int core_aver_recorder = 0;
	unsigned int core_tmp0 = 0, core_tmp1 = 0;
	unsigned int u32Value0_CORE = 0, u32Value1_CORE = 0;
	unsigned int u32Value2_CORE = 0, u32Value3_CORE = 0;
	unsigned int aver_num, i;

	aver_num = 2;

	writel(0x1200c7, HI_PMC_CTL_REG);
	writel(0x5000004, HI_PMC_CTL_REG + 0x28);

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

	core_aver_recorder = (u32Value0_CORE + u32Value1_CORE +
			u32Value2_CORE + u32Value3_CORE) >> 3;

	writel(core_aver_recorder, HI_SYS_CTL_REG + 0xA8);

	g_core_aver_recorder = core_aver_recorder;
}

void start_20dv300_svb(void)
{
	unsigned int core_aver_recorder = 0;

	/* auto svb operation */
	core_aver_recorder = g_core_aver_recorder;

	if (core_aver_recorder > 260)
		writel(0x4b00c7, HI_PMC_CTL_REG); /* 1.15V */
	else if (core_aver_recorder >= 230)
		writel(0x3700c7, HI_PMC_CTL_REG); /* 1.20V */
	else
		writel(0x2300c7, HI_PMC_CTL_REG); /* 1.25V */

	delay(50000);

}

#endif

void start_ddr_training(unsigned int base)
{
#ifdef	CONFIG_SVB_ENABLE
	/* add SVB function */
#ifdef CONFIG_HI3520DV300_SVB_ENABLE
	start_20dv300_core_hpm();
	start_20dv300_svb();
#else
	start_core_cpu_hpm();
	start_svb();
#endif
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
}

