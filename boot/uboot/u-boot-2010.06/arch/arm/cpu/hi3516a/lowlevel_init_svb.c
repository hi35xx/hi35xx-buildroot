/******************************************************************************
*    Copyright (c) 2009-2012 by Hisilicon.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <asm/arch/platform.h>
#include <config.h>

#define CHECK_TIME_OUT		100
#define MUX_CTRL_REG83		0x200F014C
#define MUX_CTRL_REG84		0x200F0150

#define REG_CRG_BASE		0x20030000
#define REG_SOC_CLK		(REG_CRG_BASE + 0x30)

#define  REG_BASE_PMC		0x20270000
#define  REG_CORE_PMC		(REG_BASE_PMC + 0x28)
#define  REG_CPU_PMC		(REG_BASE_PMC + 0x38)
#define  REG_DDR_PMC		(REG_BASE_PMC + 0x48)
#define  REG_MDA_PMC		(REG_BASE_PMC + 0x58)

#define  REG_CORE_STATUS	(REG_BASE_PMC + 0x2c)
#define  REG_CPU_STATUS		(REG_BASE_PMC + 0x3c)
#define  REG_DDR_STATUS		(REG_BASE_PMC + 0x4c)
#define  REG_MDA_STATUS		(REG_BASE_PMC + 0x5c)

#define  REG_PMC_I2C_CFG1	(REG_BASE_PMC + 0xE8)
#define  REG_PMC_I2C_CFG2	(REG_BASE_PMC + 0xEC)
#define  REG_PMC_I2C_CFG3	(REG_BASE_PMC + 0xF0)
#define  REG_PMC_I2C_CFG4	(REG_BASE_PMC + 0xF8)
#define  REG_PMC_I2C_CFG5	(REG_BASE_PMC + 0xFC)

#define  REG_PMC64		(REG_BASE_PMC + 0x100)
#define  REG_PMC65		(REG_BASE_PMC + 0x104)
#define  REG_PMC66		(REG_BASE_PMC + 0x108)
#define  REG_PMC67		(REG_BASE_PMC + 0x10C)

#ifdef	CONFIG_SVB_ENABLE

#define MISC_CTR_5	0x20120014
#define MISC_CTRL58	0x201200f0
#define REG_DVFS_FLAGS	0x2005015C

#define CORE	0
#define DDR	1
#define MDA	2
#define CPU	3

#define VOLTAGE_1_1V	0x0
#define VOLTAGE_1_2V	0x1
#define VOLTAGE_1_3V	0x2

#define REG_INNER_PWM0	(REG_BASE_PMC)
#define REG_INNER_PWM1	(REG_BASE_PMC + 0x8)
#define REG_INNER_PWM2	(REG_BASE_PMC + 0xc)
#define REG_INNER_PWM3	(REG_BASE_PMC + 0x4)

#define MDA_PMC_FREQ		24000000	/* 24MHz */
#define MDA_FREQ		120000		/* 120KHz */

/*************HPM recorder Boundory*********/
#define CORE_HPM_BOUNDARY_0	310
#define CORE_HPM_BOUNDARY_1	340

#define DDR_HPM_BOUNDARY_0	300

#define MDA_HPM_BOUNDARY_0	300
#define MDA_HPM_BOUNDARY_1	330

/* Zone CPU 1.1V */
#define CPU_HPM_BOUNDARY_1_1V_0	300
#define CPU_HPM_BOUNDARY_1_1V_1	330

/* Zone CPU 1.2V */
#define CPU_HPM_BOUNDARY_1_2V_0	360
#define CPU_HPM_BOUNDARY_1_2V_1	400

/* Zone CPU 1.3V */
#define CPU_HPM_BOUNDARY_1_3V_0	405
#define CPU_HPM_BOUNDARY_1_3V_1	445

#define EMA_HPM_BOUNDARY	300

/* compute PWM duty or i2c step based on PMC recorder and voltage */
#define INNER_PWM_PERIOD		(200 - 1)

/* CORE */
#define CORE_CONFIG_1_1V_0(attr)	(voltage_to_reg(attr, 1100))
#define CORE_CONFIG_1_1V_1(attr)	(voltage_to_reg(attr, 1050))
#define CORE_CONFIG_1_1V_2(attr)	(voltage_to_reg(attr, 1010))

/* DDR */
#define DDR_CONFIG_1_1V_0(attr)	(voltage_to_reg(attr, 1100))
#define DDR_CONFIG_1_1V_1(attr)	(voltage_to_reg(attr, 1010))

/* MDA 5M */
#define MDA_CONFIG_1_1V_0(attr)	(voltage_to_reg(attr, 1100))
#define MDA_CONFIG_1_1V_1(attr)	(voltage_to_reg(attr, 1040))
#define MDA_CONFIG_1_1V_2(attr)	(voltage_to_reg(attr, 990))

/* CPU 1.1V */
#define CPU_CONFIG_1_1V_0(attr)	(voltage_to_reg(attr, 1100))
#define CPU_CONFIG_1_1V_1(attr)	(voltage_to_reg(attr, 1040))
#define CPU_CONFIG_1_1V_2(attr)	(voltage_to_reg(attr, 990))

/* CPU 1.2V */
#define CPU_CONFIG_1_2V_0(attr)	(voltage_to_reg(attr, 1200))
#define CPU_CONFIG_1_2V_1(attr)	(voltage_to_reg(attr, 1120))
#define CPU_CONFIG_1_2V_2(attr)	(voltage_to_reg(attr, 1050))

/* CPU 1.3V */
#define CPU_CONFIG_1_3V_0(attr)	(voltage_to_reg(attr, 1300))
#define CPU_CONFIG_1_3V_1(attr)	(voltage_to_reg(attr, 1200))
#define CPU_CONFIG_1_3V_2(attr)	(voltage_to_reg(attr, 1120))

struct hipmc_attr {
	unsigned int core_check;
	unsigned int cpu_check;
	unsigned int ddr_check;
	unsigned int mda_check;
	unsigned int volt_level;
	unsigned int adjust_mode;
	/* unsigned int profile_mda; */
};

static inline unsigned int readl(unsigned int addr)
{
	unsigned int val;
	val = (*(volatile unsigned int *)(addr));
	return val;
}

static inline void writel(unsigned int val, unsigned int addr)
{
	(*(volatile unsigned int *) (addr)) = (val);
}

static inline void delay(unsigned int num)
{
	volatile unsigned int i;
	for (i = 0; i < (100 * num); i++)
		__asm__ __volatile__("nop");
}

/* parse parameter to check if a zone need do SVB */
static void parse_parameter(struct hipmc_attr *pmc_attr)
{
	unsigned int profile;
	unsigned int tmp_flags;

	tmp_flags = readl(REG_DVFS_FLAGS);
	profile	= (tmp_flags >> 0) & 0xF;

	pmc_attr->adjust_mode	= (tmp_flags >> 8) & 0xF;
	pmc_attr->volt_level	= (tmp_flags >> 4) & 0xF;
	/* pmc_attr->profile_mda = (tmp_flags >> 12) & 0xF; */

	if (0x0 == profile) {
		if (VOLTAGE_1_1V != pmc_attr->volt_level) {
			pmc_attr->core_check = 0;
			pmc_attr->cpu_check = 1;
			pmc_attr->ddr_check = 0;
			pmc_attr->mda_check = 0;
		} else {
			pmc_attr->core_check = 1;
			pmc_attr->cpu_check = 0;
			pmc_attr->ddr_check = 0;
			pmc_attr->mda_check = 0;
		}
	} else if (0x1 == profile) {
		if (VOLTAGE_1_1V != pmc_attr->volt_level) {
			pmc_attr->core_check = 0;
			pmc_attr->cpu_check = 1;
			pmc_attr->ddr_check = 0;
			pmc_attr->mda_check = 1;
		} else {
			pmc_attr->core_check = 1;
			pmc_attr->cpu_check = 0;
			pmc_attr->ddr_check = 0;
			pmc_attr->mda_check = 1;
		}
	} else if (0x2 == profile) {
		pmc_attr->core_check = 1;
		pmc_attr->cpu_check = 1;
		pmc_attr->ddr_check = 0;
		pmc_attr->mda_check = 0;
	} else if (0x3 == profile) {
		pmc_attr->core_check = 1;
		pmc_attr->cpu_check = 1;
		pmc_attr->ddr_check = 0;
		pmc_attr->mda_check = 1;
	} else {
		pmc_attr->core_check = 1;
		pmc_attr->cpu_check = 1;
		pmc_attr->ddr_check = 1;
		pmc_attr->mda_check = 1;
	}
}

static void set_adjust_mode(struct hipmc_attr *pmc_attr)
{
	unsigned int tmp;

	if (0 == pmc_attr->adjust_mode) { /* pwm adjust voltage mode */

		/* set core zone voltage adjust mode */
		tmp = readl(REG_PMC64);
		tmp &= ~(0x1 << 19);
		writel(tmp, REG_PMC64);
		/* set cpu zone voltage adjust mode */
		tmp = readl(REG_PMC65);
		tmp &= ~(0x1 << 19);
		writel(tmp, REG_PMC65);
		/* set ddr zone voltage adjust mode */
		tmp = readl(REG_PMC66);
		tmp &= ~(0x1 << 19);
		writel(tmp, REG_PMC66);
		/* set media zone voltage adjust mode */
		tmp = readl(REG_PMC67);
		tmp &= ~(0x1 << 19);
		writel(tmp, REG_PMC67);
	} else if (1 == pmc_attr->adjust_mode) { /* i2c adjust voltage mode */

		writel(0x2, MUX_CTRL_REG83);
		writel(0x2, MUX_CTRL_REG84);

		/* set core zone voltage adjust mode */
		tmp = readl(REG_PMC64);
		tmp |= (0x1 << 19);
		writel(tmp, REG_PMC64);
		/* set cpu zone voltage adjust mode */
		tmp = readl(REG_PMC65);
		tmp |= (0x1 << 19);
		writel(tmp, REG_PMC65);
		/* set ddr zone voltage adjust mode */
		tmp = readl(REG_PMC66);
		tmp |= (0x1 << 19);
		writel(tmp, REG_PMC66);
		/* set media zone voltage adjust mode */
		tmp = readl(REG_PMC67);
		tmp |= (0x1 << 19);
		writel(tmp, REG_PMC67);
	}
}

static void hpm_init(struct hipmc_attr *pmc_attr)
{
	unsigned int pmc_pwm_sel;

	if (0 == pmc_attr->adjust_mode) { /* init pwm mode */

		/* PMC select inner PWM */
		pmc_pwm_sel = readl(MISC_CTR_5);
		writel(pmc_pwm_sel | 0xF0000, MISC_CTR_5);

		/* Config CORE--PWM0 DDRC--PWM1 MDA--PWM2 CPU--PWM3 */
		writel(0x7855, REG_BASE_PMC + 0x10);
	} else if (1 == pmc_attr->adjust_mode) { /* init i2c mode */

		/* Select PMU auto control reg */
		writel(0x7821c0, REG_PMC_I2C_CFG5);

		/* set i2c 400KHz */
		writel(0x0F9F0F9F, REG_PMC_I2C_CFG4);

		/* set i2c slave addr and control enable  */
		writel(0x8000002a, REG_PMC_I2C_CFG1);
	}
}

#define CORE_MASK	0x8
#define CPU_MASK	0x300
#define DDR_MASK	0x30
#define MDA_MASK	0x3
static void config_zone_pmc(struct hipmc_attr *pmc_attr)
{
	/* Config value of Zone hpm, div frq */
	unsigned int core_pmc_config;
	unsigned int cpu_pmc_config;
	unsigned int ddr_pmc_config;
	unsigned int mda_pmc_config;

	unsigned int cpu_fhz;
	unsigned int cpu_fhz0;
	unsigned int cpu_fhz1;
	unsigned int cpu_fhz2;
	unsigned int cpu_fhz3;

	unsigned int clk_temp_bit;
	unsigned int clk_temp;

	clk_temp = readl(REG_SOC_CLK);

	if (pmc_attr->core_check) {
		clk_temp_bit = (clk_temp & CORE_MASK) >> 3;
		if (0 == clk_temp_bit)
			core_pmc_config = 0x05000003;   /* core=198 */
		else
			core_pmc_config = 0x05000002;   /* core=148.5 */

		writel(core_pmc_config, REG_CORE_PMC);
	}

	if (pmc_attr->cpu_check) {
		clk_temp_bit = (clk_temp & CPU_MASK) >> 8;
		if (0 == clk_temp_bit) {
			cpu_fhz0 = readl(REG_CRG_BASE + 0x0);
			cpu_fhz1 = (cpu_fhz0 >> 24) & 0x7;
			cpu_fhz0 = (cpu_fhz0 >> 28) & 0x7;

			cpu_fhz2 = readl(REG_CRG_BASE + 0x4);
			cpu_fhz3 = cpu_fhz2 & 0xfff;
			cpu_fhz2 = (cpu_fhz2 >> 12) & 0x3f;
			cpu_fhz = 24 * cpu_fhz3;
			cpu_fhz = cpu_fhz / (cpu_fhz0 * cpu_fhz1 * cpu_fhz2);
			cpu_fhz = (cpu_fhz + 24) / 50 - 1;

			cpu_fhz = 0x5000000 + (cpu_fhz & 0x3f);
			cpu_pmc_config = cpu_fhz;    /* cpu=apll */
		} else if (1 == clk_temp_bit)
			cpu_pmc_config = 0x05000009; /* cpu=500 */
		else if (2 == clk_temp_bit)
			cpu_pmc_config = 0x05000007; /* cpu=400 */
		else
			cpu_pmc_config = 0x05000007; /* cpu=bpll */

		writel(cpu_pmc_config, REG_CPU_PMC);
	}

	if (pmc_attr->ddr_check) {
		clk_temp_bit = (clk_temp & DDR_MASK) >> 4;
		if (0 == clk_temp_bit)
			ddr_pmc_config = 0x05000005;   /* ddr=297 */
		else if (1 == clk_temp_bit)
			ddr_pmc_config = 0x05000007;   /* ddr=400 */
		else if (2 == clk_temp_bit)
			ddr_pmc_config = 0x05000004;   /* ddr=250 */
		else
			ddr_pmc_config = 0x05000003;   /* ddr=198 */

		writel(ddr_pmc_config, REG_DDR_PMC);
	}

	{ /* media zone */
		clk_temp_bit = clk_temp & MDA_MASK;
		if (0 == clk_temp_bit)
			mda_pmc_config = 0x05000004;   /* media=237 */
		else if (1 == clk_temp_bit)
			mda_pmc_config = 0x05000005;   /* media=297 */
		else if (2 == clk_temp_bit)
			mda_pmc_config = 0x05000003;   /* media=200 */
		else
			mda_pmc_config = 0x05000004;   /* media=250 */

		writel(mda_pmc_config, REG_MDA_PMC);
	}
}

static unsigned int voltage_to_reg(struct hipmc_attr *pmc_attr,
		unsigned int voltage_out)
{
	unsigned int pwm_duty;
	unsigned int i2c_step;

	if (0 == pmc_attr->adjust_mode) {
		pwm_duty = ((1310 - voltage_out) * 100) / 255;
		return pwm_duty - 1;
	} else if (1 == pmc_attr->adjust_mode) {
		i2c_step = ((voltage_out - 800) * 2) / 10;
		return i2c_step;
	}

	return 0;
}

static void set_voltage(struct hipmc_attr *pmc_attr, unsigned int zone_index,
		unsigned int value)
{
	unsigned int i2c_step;
	unsigned int time_out;
	unsigned int reg_tmp;

	if (0 == pmc_attr->adjust_mode) {
		if (CORE == zone_index) {
			writel((value << 16) + INNER_PWM_PERIOD,
					REG_INNER_PWM0);
		} else if (CPU == zone_index) {
			writel((value << 16) + INNER_PWM_PERIOD,
					REG_INNER_PWM3);
		} else if (DDR == zone_index) {
			writel((value << 16) + INNER_PWM_PERIOD,
					REG_INNER_PWM1);
		} else {  /* MDA == zone_index */
			writel((value << 16) + INNER_PWM_PERIOD,
					REG_INNER_PWM2);
		}

	} else if (1 == pmc_attr->adjust_mode) {
		i2c_step = value;

		if (CORE == zone_index) {
			writel(i2c_step << 8, REG_PMC_I2C_CFG3);
			reg_tmp = readl(REG_PMC_I2C_CFG1);
			writel(reg_tmp | (0x1 << 17), REG_PMC_I2C_CFG1);
		} else if (CPU == zone_index) {
			writel(i2c_step << 0, REG_PMC_I2C_CFG3);
			reg_tmp = readl(REG_PMC_I2C_CFG1);
			writel(reg_tmp | (0x1 << 16), REG_PMC_I2C_CFG1);
		} else if (DDR == zone_index) {
			writel(i2c_step << 16, REG_PMC_I2C_CFG3);
			reg_tmp = readl(REG_PMC_I2C_CFG1);
			writel(reg_tmp | (0x1 << 18), REG_PMC_I2C_CFG1);
		} else {  /* MDA == zone_index */
			writel(i2c_step << 24, REG_PMC_I2C_CFG2);
			reg_tmp = readl(REG_PMC_I2C_CFG1);
			writel(reg_tmp | (0x1 << 15), REG_PMC_I2C_CFG1);
		}

		time_out = CHECK_TIME_OUT;

		while (time_out) {
			if ((readl(REG_PMC_I2C_CFG1) >> 15) && 0xF) {
				time_out--;
				delay(5);
			} else {
				break;
			}
		}
	}
}

static void init_zone_voltage(struct hipmc_attr *pmc_attr)
{
	unsigned int reg_value;

	if (pmc_attr->cpu_check) {
		if (VOLTAGE_1_2V == pmc_attr->volt_level)
			reg_value = voltage_to_reg(pmc_attr, 1200);
		else if (VOLTAGE_1_3V == pmc_attr->volt_level)
			reg_value = voltage_to_reg(pmc_attr, 1300);
		else
			reg_value = voltage_to_reg(pmc_attr, 1100);

		set_voltage(pmc_attr, CPU, reg_value);
		/* delay(100); */
	}

	reg_value = voltage_to_reg(pmc_attr, 1100);
	if (pmc_attr->core_check) {
		set_voltage(pmc_attr, CORE, reg_value);
		/* delay(100); */
	}
	if (pmc_attr->ddr_check) {
		set_voltage(pmc_attr, DDR, reg_value);
		/* delay(100); */
	}
	if (pmc_attr->mda_check) {
		set_voltage(pmc_attr, MDA, reg_value);
		/* delay(100); */
	}
}

static unsigned int get_hpm_recorder(unsigned int zone_index)
{
	unsigned int aver_num = 5;
	volatile unsigned int i;

	unsigned int reg_zone_status;
	unsigned int recorder0 = 0;
	unsigned int recorder1 = 0;
	unsigned int hpm_value0 = 0;
	unsigned int hpm_value1 = 0;

	unsigned int zone_recorder_aver = 0;

	if (CORE == zone_index)
		reg_zone_status = REG_CORE_STATUS;
	else if (CPU == zone_index)
		reg_zone_status = REG_CPU_STATUS;
	else if (DDR == zone_index)
		reg_zone_status = REG_DDR_STATUS;
	else  /* MDA == zone_index */
		reg_zone_status = REG_MDA_STATUS;

	for (i = 0; i < aver_num; i++) {
		recorder0 = readl(reg_zone_status);
		hpm_value0 += (recorder0 + (recorder0 >> 12)) & 0x3ff;

		recorder1 = readl(reg_zone_status + 0x4);
		hpm_value1 += (recorder1 + (recorder1 >> 12)) & 0x3ff;
		delay(1);
	}

	zone_recorder_aver = (hpm_value0 + hpm_value1) / (aver_num * 4);

	return zone_recorder_aver;
}

void config_ema(void)
{
	unsigned int mda_hpm_record = get_hpm_recorder(MDA);

	if (EMA_HPM_BOUNDARY < mda_hpm_record)
		writel(0x02020302, MISC_CTRL58);
}

void start_svb(void)
{
	unsigned int cpu_aver_recorder = 0;
	unsigned int ddr_aver_recorder = 0;
	unsigned int core_aver_recorder = 0;
	unsigned int mda_aver_recorder = 0;

	unsigned int cpu_boundary_0;
	unsigned int cpu_boundary_1;

	unsigned int cpu_pwm_duty_0;
	unsigned int cpu_pwm_duty_1;
	unsigned int cpu_pwm_duty_2;

	unsigned int flag_voltage;

	struct hipmc_attr pmc_attr;

	parse_parameter(&pmc_attr);

	/* delay(100); */
	config_zone_pmc(&pmc_attr);
	/* delay(100); */
	set_adjust_mode(&pmc_attr);

	hpm_init(&pmc_attr);

	init_zone_voltage(&pmc_attr);
	delay(200);

	config_ema();

	/* CORE Hpm */
	if (pmc_attr.core_check) {
		core_aver_recorder = get_hpm_recorder(CORE);
		writel(core_aver_recorder, 0x2005014c); /* save recorder*/

		/* adjusting voltage based on aver record */
		if (core_aver_recorder <= CORE_HPM_BOUNDARY_0) {
			/* 0~310: 1.1V */
			set_voltage(&pmc_attr, CORE,
					CORE_CONFIG_1_1V_0(&pmc_attr));
		} else if (core_aver_recorder < CORE_HPM_BOUNDARY_1) {
			/* 340~max: 1.01V */
			set_voltage(&pmc_attr, CORE,
					CORE_CONFIG_1_1V_2(&pmc_attr));
		} else {
			/* 310~340: 1.05V */
			set_voltage(&pmc_attr, CORE,
					CORE_CONFIG_1_1V_1(&pmc_attr));
		}
		delay(100);
	}

	/* CPU Hpm */
	if (pmc_attr.cpu_check) {
		cpu_aver_recorder = get_hpm_recorder(CPU);
		writel(cpu_aver_recorder, 0x20050158); /* save recorder */
		flag_voltage = readl(REG_DVFS_FLAGS);
		flag_voltage &= 0xF0;
		flag_voltage = flag_voltage >> 4;

		if (VOLTAGE_1_2V == flag_voltage) { /* CPU 1.2V */
			cpu_boundary_0 = CPU_HPM_BOUNDARY_1_2V_0;
			cpu_boundary_1 = CPU_HPM_BOUNDARY_1_2V_1;

			cpu_pwm_duty_0 = CPU_CONFIG_1_2V_0(&pmc_attr);
			cpu_pwm_duty_1 = CPU_CONFIG_1_2V_1(&pmc_attr);
			cpu_pwm_duty_2 = CPU_CONFIG_1_2V_2(&pmc_attr);
		} else if (VOLTAGE_1_3V == flag_voltage) { /* CPU 1.3V */
			cpu_boundary_0 = CPU_HPM_BOUNDARY_1_3V_0;
			cpu_boundary_1 = CPU_HPM_BOUNDARY_1_3V_1;

			cpu_pwm_duty_0 = CPU_CONFIG_1_3V_0(&pmc_attr);
			cpu_pwm_duty_1 = CPU_CONFIG_1_3V_1(&pmc_attr);
			cpu_pwm_duty_2 = CPU_CONFIG_1_3V_2(&pmc_attr);
		} else { /* CPU 1.1V */
			cpu_boundary_0 = CPU_HPM_BOUNDARY_1_1V_0;
			cpu_boundary_1 = CPU_HPM_BOUNDARY_1_1V_1;

			cpu_pwm_duty_0 = CPU_CONFIG_1_1V_0(&pmc_attr);
			cpu_pwm_duty_1 = CPU_CONFIG_1_1V_1(&pmc_attr);
			cpu_pwm_duty_2 = CPU_CONFIG_1_1V_2(&pmc_attr);
		}

		/* adjusting voltage based on aver record */
		if (cpu_aver_recorder <= cpu_boundary_0)
			set_voltage(&pmc_attr, CPU, cpu_pwm_duty_0);
		else if (cpu_aver_recorder > cpu_boundary_1)
			set_voltage(&pmc_attr, CPU, cpu_pwm_duty_2);
		else
			set_voltage(&pmc_attr, CPU, cpu_pwm_duty_1);

		delay(100);
	}

	/* DDR Hpm */
	if (pmc_attr.ddr_check) {
		ddr_aver_recorder = get_hpm_recorder(DDR);
		writel(ddr_aver_recorder, 0x20050150); /* save recorder */

		/* adjusting voltage based on aver record */
		if (ddr_aver_recorder <= DDR_HPM_BOUNDARY_0) {
			/* 0~290: 1.1V */
			set_voltage(&pmc_attr, DDR,
					DDR_CONFIG_1_1V_0(&pmc_attr));
		} else {
			/* 290~max: 1.01V */
			set_voltage(&pmc_attr, DDR,
					DDR_CONFIG_1_1V_1(&pmc_attr));
		}
		delay(100);
	}

	/* MEDIA Hpm */
	if (pmc_attr.mda_check) {
		mda_aver_recorder = get_hpm_recorder(MDA);
		writel(mda_aver_recorder, 0x20050154); /* save recorder */

		/* adjusting voltage based on aver record */
		if (mda_aver_recorder <= MDA_HPM_BOUNDARY_0) {
			/* 0~270: 1.1V */
			set_voltage(&pmc_attr, MDA,
					MDA_CONFIG_1_1V_0(&pmc_attr));
		} else if (mda_aver_recorder > MDA_HPM_BOUNDARY_1) {
			/* 0x320~max: 1.01V */
			set_voltage(&pmc_attr, MDA,
					MDA_CONFIG_1_1V_2(&pmc_attr));
		} else {
			/* 270~320: 1.03V */
			set_voltage(&pmc_attr, MDA,
					MDA_CONFIG_1_1V_1(&pmc_attr));
		}
		delay(100);
	}
}
#endif
