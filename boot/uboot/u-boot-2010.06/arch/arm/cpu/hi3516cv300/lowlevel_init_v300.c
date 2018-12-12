/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <asm/arch/platform.h>
#include <config.h>
#ifdef CONFIG_DDR_TRAINING_V2
#include <ddr_interface.h>
#endif

#define HI_SYS_CTL_REG	SYS_CTRL_REG_BASE

#define SC_SYS_ID0_REG	(SYS_CTRL_REG_BASE + 0xEE0)
#define PERI_CRG_PLL0	(CRG_REG_BASE + 0x0)
#define DDR0_BASE_REG	(DDRC0_REG_BASE + 0x1000)
#define DDR0_PLL_REG    (DDRC0_REG_BASE + 0x8000)

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

static void ddr_hw_training(void)
{
	unsigned int temp1, temp2;

	temp2 = readl(HI_SYS_CTL_REG + 0xa0);
	if (temp2) {
		temp1 = readl(DDR0_BASE_REG + 0x88);
		writel((temp1 & 0xfffff000), DDR0_BASE_REG + 0x88);
		writel(temp2, DDR0_PLL_REG + 0x4);
		writel(0x2, DDR0_BASE_REG);

		while (1) {
			temp2 = readl(DDR0_PLL_REG + 0x4) & 0x1;
			if (!temp2)
				break;
		}

		writel(0x8000, DDR0_PLL_REG + 0x4);
		writel(0x0, DDR0_PLL_REG + 0x4);
		writel(temp1, DDR0_BASE_REG + 0x88);
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

#ifdef	CONFIG_SVB_ENABLE

#define HPM_REG_BASE		0x12078000
#define HPM_CORE_VOL_REG	0x12030078
unsigned int get_hpm_value(void)
{
	unsigned int reg_value;
	unsigned int cpl_flag;

	reg_value = 0;

	/*set read addr*/
	writel(14, HPM_REG_BASE + 0xc);

	/*enable read*/
	writel(0x1, HPM_REG_BASE + 0x8);

	/*wait read completion*/
	do {
		cpl_flag = readl(HPM_REG_BASE + 0x10);
	} while (!(cpl_flag & 0x2));

	reg_value = readl(HPM_REG_BASE + 0x14);

	return reg_value;
}

void set_hpm_core_volt(unsigned int hpm_core, unsigned int iddq_core)
{
	unsigned int val, high, mid, low_h, low_l;

	val = readl(SC_SYS_ID0_REG);
	if ((val >> 24) == 0x4) {
		high = 0x2d0c75;
		mid = 0x3e0c75;
		low_h = 0x5a0c75;
		low_l = 0x760c75;
	} else {
		val = readl(PERI_CRG_PLL0);
		if (val == 0x15000000) {
			high = 0x000c75;
			mid = 0x200c75;
			low_h = 0x400c75;
			low_l = 0x4e0c75;
		} else {
			high = 0x0c0c75;
			mid = 0x200c75;
			low_h = 0x4e0c75;
			low_l = 0x620c75;
		}
	}

	if (hpm_core <= 257) {
		val = high;
	} else if ((hpm_core > 257) && (hpm_core <= 287)) {
		val = mid;
	} else {
		if (iddq_core <= 25)
			val = low_h;
		else
			val = low_l;
	}

	writel(val, HPM_CORE_VOL_REG);
}

void start_svb(void)
{
	unsigned int val, hpm_core, iddq_core;

	val = get_hpm_value();
	hpm_core = val & 0x3ff;
	iddq_core = (val >> 10) & 0x3f;

	set_hpm_core_volt(hpm_core, iddq_core);
}
#endif

#if defined(CONFIG_DDR_WRITE_2T_PRE_ENABLE)
extern void uart_early_puts(const char *s);
static void ddr_write_2t_pre(void)
{
	unsigned int reg_val;

	reg_val = readl(DDR0_PLL_REG + 0x234);
	if ((reg_val & 0x3000) == 0x3000) {
		uart_early_puts("\n\rddr 2t pre err! 0x12068234[13:12]=0x3\n");
	} else {
		reg_val += 0x1000;
		writel(reg_val, DDR0_PLL_REG + 0x234);
	}

	reg_val = readl(DDR0_PLL_REG + 0x2b4);
	if ((reg_val & 0x3000) == 0x3000) {
		uart_early_puts("\n\rddr 2t pre err! 0x120682b4[13:12]=0x3\n");
	} else {
		reg_val += 0x1000;
		writel(reg_val, DDR0_PLL_REG + 0x2b4);
	}

	reg_val = readl(DDR0_PLL_REG + 0x70);
	reg_val |= 0x80000;
	writel(reg_val, DDR0_PLL_REG + 0x70);
	reg_val &= ~0x80000;
	writel(reg_val, DDR0_PLL_REG + 0x70);
}
#endif

void start_ddr_training(unsigned int base)
{
#ifdef	CONFIG_SVB_ENABLE
	start_svb();
#endif

	ddr_hw_training();

#if defined(CONFIG_DDR_WRITE_2T_PRE_ENABLE)
	ddr_write_2t_pre();
#endif

#ifdef CONFIG_DDR_TRAINING_V2
	int ret = 0;

	/* ddr training function */
	ret = ddr_sw_training_if(0);
	if (ret)
		reset_cpu(0);
#endif

	/* the value should config after trainning, or
	  it will cause chip compatibility problems */
	writel(0x401, DDR0_BASE_REG + 0x28);
}

