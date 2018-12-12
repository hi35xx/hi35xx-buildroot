/*
 * power mangager control for hisilicon hi3559 soc
 *
 * Copyright (c) 2015 HiSilicon Technologies Co., Ltd.
 * Authors: zengtao@hisilicon.com
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
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

#define __iomem

#define REG_PERI_CRG10 0x28
#define PERI_PMC77	(0x134)
#define PERI_PMC79	(0x13c)
#define PERI_PMC85	(0x154)
typedef unsigned int u32;

static void __iomem *pmc_base;
static void  *pmc_base;
static u32  pmc_phys_addr;

#define IO_ADDRESS(reg)		reg
#define CRG_REG_BASE		0x12010000
#define PMC_ADDRESS(reg)	(pmc_base + reg)

#define readl(_a)        (*(volatile unsigned int *)(_a))
#define writel(_v, _a)   (*(volatile unsigned int *)(_a) = (_v))

/* set bitfield of reg from start bit to end - 1 bit */
static void reg_bit_set(u32 reg, u32 start, u32 end, u32 val)
{
	u32 regval, mask;

	regval = readl((void __iomem *)PMC_ADDRESS(reg));
	mask = ((0xffffffff << (32 - start)) >> (32 - start))
	       | ((0xffffffff >> end) << end);

	regval &= mask;
	regval |= (val << start);

	writel(regval, (void __iomem *)PMC_ADDRESS(reg));
}

/* get bitfield of reg from start bit to end - 1 bit */
static u32 reg_bit_get(u32 reg, u32 start, u32 end)
{
	u32 regval;

	regval = readl((void __iomem *)PMC_ADDRESS(reg));
	regval = (regval << (32 - end)) >> (32 - end);
	regval = regval >> start;

	return regval;
}

extern void udelay(unsigned long usec);
/* cpu hotplug powerup */
void hi_pmc_power_up(void)
{
	u32 power_state;

	writel(readl((void *)IO_ADDRESS(0x12030004)) | 0x40000000,
		 (void *)IO_ADDRESS(0x12030004));

	reg_bit_set(PERI_PMC79, 0, 2, 0x2);

	writel(readl((void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10))
			| (1<<21),
			(void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10));

	reg_bit_set(PERI_PMC77, 1, 2, 0x1);

	udelay(1000);
	power_state = reg_bit_get(PERI_PMC77, 2, 3);
	while (!power_state)
		power_state = reg_bit_get(PERI_PMC77, 2, 3);

	reg_bit_set(PERI_PMC77, 0, 1, 0x0);
	writel(readl((void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10))
			& 0xfff7ffff,
		 (void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10));

	reg_bit_set(PERI_PMC77, 5, 6, 0x1);
	udelay(1000);
	power_state = reg_bit_get(PERI_PMC77, 6, 7);
	while (power_state != 1)
		power_state = reg_bit_get(PERI_PMC77, 6, 7);

	writel(readl((void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10))
			& 0xfffffffd,
		(void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10));
}

static void hi_pmc_config(void)
{
	/* enable pmc timeout */
	reg_bit_set(PERI_PMC77, 12, 13, 1);
	/* enable pmc auto mode */
	reg_bit_set(PERI_PMC79, 0, 2, 0);
	/* enable irq triger source power on */
	reg_bit_set(PERI_PMC79, 7, 8, 1);
}

/* enable timeout */
void hi_pmc_init(void)
{
	pmc_base = (void *)0x120a0000;
	pmc_phys_addr = (u32)0x120a0000;

	hi_pmc_config();
}

#include <common.h>
int start_other_cpus(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	/*
	 * prepare two commands for cortex-a17
	 */
	volatile int cmd_address = 0;
	volatile int cmd = 0xe51ff004;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	asm("str %0, [%1]"::"r" (cmd), "r" (cmd_address): "cc");
	cmd = simple_strtoul(argv[1], NULL, 16);
	printf("starting a17 liteos address 0x%x\n", cmd);
	asm("str %0, [%1, #4]"::"r" (cmd), "r" (cmd_address): "cc");
#if 1
	hi_pmc_init();
	hi_pmc_power_up();
#endif
	return 0;
}

