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

#define HI_SYS_CTL_REG	0x12020000

#define DDR0_BASE_REG	0x12068000
#define DDR0_PLL_REG    0x1206c000

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

void hi3519_ddr_init(void)
{
	unsigned int temp1, temp2;

	temp2 = readl(HI_SYS_CTL_REG + 0xa0);
	if (temp2) {
		temp1 = readl(DDR0_BASE_REG + 0x108);
		writel((temp1 & 0xffff0000), DDR0_BASE_REG + 0x108);
		writel(temp2, DDR0_PLL_REG + 0x4);
		writel(0x2, DDR0_BASE_REG);

		while (1) {
			temp2 = readl(DDR0_PLL_REG + 0x4) & 0x1;
			if (!temp2)
				break;
		}

		writel(0x8000, DDR0_PLL_REG + 0x4);
		writel(0x0, DDR0_PLL_REG + 0x4);
		writel(temp1, DDR0_BASE_REG + 0x108);
	}

	/* for low cost*/
	/*writel(0x23201, DDR0_BASE_REG + 0x28);
	writel(0xaf501, DDR0_BASE_REG + 0x1c);
	writel(0xfe014830, DDR0_PLL_REG + 0x1e4);*/
	writel(0x2, DDR0_BASE_REG + 0x0);

	do {
		temp1 = readl(DDR0_BASE_REG + 0x294);
	} while (temp1 & 0x1);
}

#ifdef CONFIG_SVB_ENABLE

#define HI_PMC_CTL_REG		0x120a0000
#define HPM_CORE_VOL_REG	(HI_PMC_CTL_REG + 0x0)
#define HPM_CPU_VOL_REG		(HI_PMC_CTL_REG + 0x4)
#define HPM_DDR_VOL_REG		(HI_PMC_CTL_REG + 0x8)
#define HPM_MDA_VOL_REG		(HI_PMC_CTL_REG + 0xc)
#define CYCLE_NUM		4
#define HPM_CORE_REG0	0x2c
#define HPM_CORE_REG1	0x30
#define HPM_CPU_REG0	0x3c
#define HPM_CPU_REG1	0x40
#define HPM_DDR_REG0	0x4c
#define HPM_DDR_REG1	0x50
#define HPM_MDA_REG0	0x5c
#define HPM_MDA_REG1	0x60
#define PERI_CRG_PLL1	0x4
#define PERI_CRG13	0x34
#define CPU_A17_CKSEL_MASK	0x70

#define HPM_POWER_PROFILE_REG	0x1202015c
#define FOUR_WAY_POWER	0
#define THREE_WAY_POWER	1
#define TWO_WAY_POWER	2

#if 0
static unsigned int division(unsigned int dividend, unsigned int divisor)
{
	unsigned int major, quotient, i = 0;
	unsigned int tmp = dividend;

	if (dividend < divisor)
		return 0;

	do {
		tmp >>= 1;
		if (tmp < divisor)
			break;
		i++;
	} while (1);

	major = 1 << i;

	quotient = major + division((dividend - (divisor << i)), divisor);

	return quotient;
}

static unsigned int get_cpu_hpm_div(void)
{
	unsigned int val;
	unsigned int div, cpu_fmhz;
	unsigned int clk_sel, apll_fbdiv, apll_refdiv;
	unsigned int freq_mhz[] = {1000, 792, 594, 500, 400};

	val = readl(CRG_REG_BASE + PERI_CRG13);
	clk_sel = (val & CPU_A17_CKSEL_MASK) >> 4;
	if (clk_sel >= 3) {
		cpu_fmhz = freq_mhz[clk_sel - 3];
	} else if (clk_sel > 0) {
		val = readl(CRG_REG_BASE + PERI_CRG_PLL1);
		apll_fbdiv = val & 0xfff;
		apll_refdiv = (val >> 12) & 0x3f;
		cpu_fmhz = 24 * apll_fbdiv;
		cpu_fmhz = division(cpu_fmhz, apll_refdiv);
	} else {
		/* xtal mode can't test hpm! */
		cpu_fmhz = 0;
	}

	writel(cpu_fmhz, REG_BASE_SCTL + REG_SC_GEN4);
	div = division((cpu_fmhz + 24), 50);
	div = (div - 1) & 0x3f;

	return div;
}
#endif

static unsigned hpm_value_avg(unsigned int *val)
{
	unsigned int i;
	unsigned tmp = 0;

	for (i = 0; i < 4; i++)
		tmp += val[i] >> 2;

	return tmp >> 2;
}

static void get_hpm_value(unsigned int *hpm_core, unsigned int *hpm_cpu,
		unsigned int *hpm_ddr, unsigned int *hpm_mda)
{
	int i;
	unsigned int temp;
	unsigned int core_value[4] = {0, 0, 0, 0};
	unsigned int cpu_value[4] = {0, 0, 0, 0};
	unsigned int ddr_value[4] = {0, 0, 0, 0};
	unsigned int mda_value[4] = {0, 0, 0, 0};

	for (i = 0; i < CYCLE_NUM; i++) {
		delay(100);

		/* core */
		temp = readl(HI_PMC_CTL_REG + HPM_CORE_REG0);
		core_value[1] += (temp >> 12) & 0x3ff;
		core_value[0] += temp & 0x3ff;
		temp = readl(HI_PMC_CTL_REG + HPM_CORE_REG1);
		core_value[3] += (temp >> 12) & 0x3ff;
		core_value[2] += temp & 0x3ff;

		/* cpu */
		temp = readl(HI_PMC_CTL_REG + HPM_CPU_REG0);
		cpu_value[1] += (temp >> 12) & 0x3ff;
		cpu_value[0] += temp & 0x3ff;
		temp = readl(HI_PMC_CTL_REG + HPM_CPU_REG1);
		cpu_value[3] += (temp >> 12) & 0x3ff;
		cpu_value[2] += temp & 0x3ff;

		/* ddr */
		temp = readl(HI_PMC_CTL_REG + HPM_DDR_REG0);
		ddr_value[1] += (temp >> 12) & 0x3ff;
		ddr_value[0] += temp & 0x3ff;
		temp = readl(HI_PMC_CTL_REG + HPM_DDR_REG1);
		ddr_value[3] += (temp >> 12) & 0x3ff;
		ddr_value[2] += temp & 0x3ff;

		/* mda */
		temp = readl(HI_PMC_CTL_REG + HPM_MDA_REG0);
		mda_value[1] += (temp >> 12) & 0x3ff;
		mda_value[0] += temp & 0x3ff;
		temp = readl(HI_PMC_CTL_REG + HPM_MDA_REG1);
		mda_value[3] += (temp >> 12) & 0x3ff;
		mda_value[2] += temp & 0x3ff;
	}

	*hpm_core = hpm_value_avg(core_value);
	*hpm_cpu = hpm_value_avg(cpu_value);
	*hpm_ddr = hpm_value_avg(ddr_value);
	*hpm_mda = hpm_value_avg(mda_value);
}

static void start_hpm(unsigned int *hpm_core, unsigned int *hpm_cpu,
		unsigned int *hpm_ddr, unsigned int *hpm_mda)
{
	/* unsigned int cpu_hpm_div;

	cpu_hpm_div = get_cpu_hpm_div(); */

	/* reset core hpm, cpu hpm, ddr hpm, mda hpm, div frq */
	writel(0x0a000003, HI_PMC_CTL_REG + 0x28);
	/* writel(0x0a000000 + cpu_hpm_div, HI_PMC_CTL_REG + 0x38); */
	writel(0x0a000003, HI_PMC_CTL_REG + 0x48);
	writel(0x0a000003, HI_PMC_CTL_REG + 0x58);

	writel(0x05000003, HI_PMC_CTL_REG + 0x28);
	/* writel(0x05000000 + cpu_hpm_div, HI_PMC_CTL_REG + 0x38); */
	writel(0x05000003, HI_PMC_CTL_REG + 0x48);
	writel(0x05000003, HI_PMC_CTL_REG + 0x58);

	delay(500);

	get_hpm_value(hpm_core, hpm_cpu, hpm_ddr, hpm_mda);
}

static void set_hpm_core_volt(unsigned int hpm_core_value)
{
	if (hpm_core_value <= 310)
		writel(0x6800c7, HPM_CORE_VOL_REG);
	else
		writel(0x6c00c7, HPM_CORE_VOL_REG);
}

#if 0
static void set_hpm_cpu_volt(unsigned int hpm_cpu_value)
{
	unsigned int val, cpu_fmhz;

	cpu_fmhz = readl(REG_BASE_SCTL + REG_SC_GEN4);
	if (hpm_cpu_value <= 370)
		val = cpu_fmhz > 880 ? 0x4800c7 : 0x6800c7;
	else if (hpm_cpu_value <= 410)
		val = cpu_fmhz > 880 ? 0x5600c7 : 0x7e00c7;
	else
		val = cpu_fmhz > 880 ? 0x6500c7 : 0x8d00c7;

	writel(val, HPM_CPU_VOL_REG);
}
#endif

static void set_hpm_ddr_volt(unsigned int hpm_ddr_value)
{
	if (hpm_ddr_value <= 310)
		writel(0x6800c7, HPM_DDR_VOL_REG);
	else if (hpm_ddr_value <= 350)
		writel(0x6c00c7, HPM_DDR_VOL_REG);
	else if (hpm_ddr_value <= 380)
		writel(0x7300c7, HPM_DDR_VOL_REG);
	else
		writel(0x7a00c7, HPM_DDR_VOL_REG);
}

static void set_hpm_mda_volt(unsigned int hpm_mda_value)
{
	if (hpm_mda_value <= 310)
		writel(0x5a00c7, HPM_MDA_VOL_REG);
	else if (hpm_mda_value <= 335)
		writel(0x6800c7, HPM_MDA_VOL_REG);
	else if (hpm_mda_value <= 350)
		writel(0x7000c7, HPM_MDA_VOL_REG);
	else if (hpm_mda_value <= 380)
		writel(0x7a00c7, HPM_MDA_VOL_REG);
	else
		writel(0x8500c7, HPM_MDA_VOL_REG);
}

static unsigned int get_power_profile(void)
{
	unsigned int reg_val;

	reg_val = readl(HPM_POWER_PROFILE_REG);

	return reg_val & 0x3;
}

static void start_svb(void)
{
	unsigned int hpm_core, hpm_cpu, hpm_ddr, hpm_mda;
	unsigned int profile;

	start_hpm(&hpm_core, &hpm_cpu, &hpm_ddr, &hpm_mda);

	profile = get_power_profile();
	switch (profile) {
	case FOUR_WAY_POWER:
		set_hpm_ddr_volt(hpm_ddr);
		/* fall through*/
	case THREE_WAY_POWER:
		set_hpm_core_volt(hpm_core);
		/* fall through*/
	case TWO_WAY_POWER:
		/* set_hpm_cpu_volt(hpm_cpu); */
		set_hpm_mda_volt(hpm_mda);
		break;
	default:
		break;
	}
}
#endif

void start_ddr_training(unsigned int base)
{
#ifdef CONFIG_SVB_ENABLE
	start_svb();
#endif

	hi3519_ddr_init();

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
}
