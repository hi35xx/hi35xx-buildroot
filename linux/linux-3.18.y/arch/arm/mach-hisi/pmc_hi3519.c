/*
 * power mangager control for hisilicon hi3519 soc
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

#include <linux/io.h>
#include <linux/linkage.h>
#include <linux/bug.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/resource.h>
#include <mach/io.h>
#include <mach/platform.h>

#include <linux/delay.h>

#define PERI_PMC77	(0x134)
#define PERI_PMC79	(0x13c)
#define PERI_PMC85	(0x154)

static void __iomem *pmc_base;
static u32  pmc_phys_addr;

#define PMC_ADDRESS(reg)	(pmc_base + reg)
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

void hi_pmc_power_up_done(void)
{
	writel(0, (void __iomem *)PMC_ADDRESS(PERI_PMC85));
	writel(1, (void __iomem *)PMC_ADDRESS(PERI_PMC85));

}

/* before power down set ac inactive */
void hi_pmc_set_ac_inactive(void)
{
	reg_bit_set(PERI_PMC79, 8, 9, 1);
}

/* after powerup clear ac inactive */
void hi_pmc_clear_ac_inactive(void)
{
	reg_bit_set(PERI_PMC79, 8, 9, 0);
}
EXPORT_SYMBOL(hi_pmc_clear_ac_inactive);

/*
 * call from assable context, same as the
 * hi_pmc_clear_ac_inactive
 */
asmlinkage void __naked hi_pmc_clear_a17_ac(void)
{
	asm volatile("\n"
		"adr     r2, 1f\n"
		"ldmia   r2, {r1, r3}\n"
		"sub     r0, r2, r1\n"
		"ldr     r2, [r0, r3]\n"
		"ldr r0, ="__stringify(PERI_PMC79)"\n"
		"add r0, r0, r2\n"
		"ldr r1, [r0]\n"
		"bic r1, #0x100\n"
		"str r1, [r0]\n"
		"mov r0, #0\n"
		"bx  lr\n"

		".align  2\n"
		"1:     .word   .\n"
		"       .word   pmc_phys_addr\n"
		);

	unreachable();
}

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

	reg_bit_set(PERI_PMC77, 1, 2, 0x0);

	mdelay(1);
	power_state = reg_bit_get(PERI_PMC77, 2, 3);
	while (power_state)
		power_state = reg_bit_get(PERI_PMC77, 2, 3);

	reg_bit_set(PERI_PMC77, 0, 1, 0x0);
	writel(readl((void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10))
			& 0xfff7ffff,
		 (void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10));

	reg_bit_set(PERI_PMC77, 5, 6, 0x1);
	mdelay(1);
	power_state = reg_bit_get(PERI_PMC77, 6, 7);
	while (power_state != 1)
		power_state = reg_bit_get(PERI_PMC77, 6, 7);

	writel(readl((void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10))
			& 0xfffffffd,
		(void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10));
}

void hi_pmc_kill_cpu(unsigned int cpu)
{
	unsigned int state;

	while (1) {
		state = readl((void *)IO_ADDRESS(0x12030004)) & 0x40000000;
		if (!state)
			break;
	}

	while (1) {
		state = reg_bit_get(PERI_PMC77, 7, 9);
		if (state == 3)
			break;
	}

	while (1) {
		state = reg_bit_get(PERI_PMC77, 3, 5);
		if (state == 0)
			break;
	}

	reg_bit_set(PERI_PMC77, 5, 6, 0x0);

	while (1) {
		state = reg_bit_get(PERI_PMC77, 6, 7);
		if (state == 0)
			break;
	}

	reg_bit_set(PERI_PMC77, 0, 1, 0x1);

	writel(readl((void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10))
			| (1<<19),
		 (void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10));
	writel(readl((void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10))
			& 0xffdfffff,
			(void *)IO_ADDRESS(CRG_REG_BASE + REG_PERI_CRG10));

	reg_bit_set(PERI_PMC77, 1, 2, 0x1);

	while (1) {
		state = reg_bit_get(PERI_PMC77, 2, 3);
		if (state == 1)
			break;
	}
}

/* cpu hotplug powerdown */
void hi_pmc_power_down(void)
{
#if 0
	u32 power_state;

	power_state = reg_bit_get(PERI_PMC79, 12, 16);
	BUG_ON(power_state != 6);

	/* disable interrupt wakeup */
	reg_bit_set(PERI_PMC79, 5, 6, 0);

	/* power off */
	reg_bit_set(PERI_PMC79, 4, 5, 0);
	reg_bit_set(PERI_PMC79, 4, 5, 1);
#endif
	reg_bit_set(PERI_PMC79, 0, 2, 2);
	writel(readl((void *)IO_ADDRESS(0x12030004)) & 0xbfffffff,
		(void *)IO_ADDRESS(0x12030004));

}

/* cpuidle powerdown */
void hi_pmc_automode_power_down(void)
{
	u32 power_state;

	power_state = reg_bit_get(PERI_PMC79, 12, 16);
	BUG_ON(power_state != 6);

	/* enable interrupt wakeup */
	reg_bit_set(PERI_PMC79, 5, 6, 1);

	/* power off */
	reg_bit_set(PERI_PMC79, 4, 5, 0);
	reg_bit_set(PERI_PMC79, 4, 5, 1);

}
EXPORT_SYMBOL(hi_pmc_automode_power_down);

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
static int hi_pmc_init(void)
{
	struct device_node *np;
	struct resource res;
	int ret = -ENODEV;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,pmc");
	if (!np)
		goto err;

	pmc_base = of_iomap(np, 0);
	if (!pmc_base) {
		pr_err("failed to map pmc base\n");
		ret = -ENOMEM;
		goto err;
	}

	ret = of_address_to_resource(np, 0, &res);
	if (ret) {
		pr_err("failed to get pmc base phys\n");
		ret = -ENOMEM;
		goto err;
	}
	pmc_phys_addr = res.start;

	hi_pmc_config();
err:
	return ret;
}

early_initcall(hi_pmc_init);
