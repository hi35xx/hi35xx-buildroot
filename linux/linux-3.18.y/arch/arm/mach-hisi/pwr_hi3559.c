/*
 * The power mangager control for hisilicon soc
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
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
#include <mach/platform.h>

#define PWR_CTRL0	(0x34)

static void __iomem *pwr_base;

#define PWR_ADDRESS(reg)	(pwr_base + reg)
/* set bitfield of reg from start bit to end - 1 bit */
static void reg_bit_set(u32 reg, u32 start, u32 end, u32 val)
{
	u32 regval, mask;

	regval = readl((void __iomem *)PWR_ADDRESS(reg));
	mask = ((0xffffffff << (32 - start)) >> (32 - start))
	       | ((0xffffffff >> end) << end);

	regval &= mask;
	regval |= (val << start);

	writel(regval, (void __iomem *)PWR_ADDRESS(reg));
}

/* power off interface */
void hi_pwr_off(void)
{
	/* set pwr_off_ctrl to 0x1 not 0x2 */
	/* if set to 0x2, must wait about 2 seconds when power on */
	reg_bit_set(PWR_CTRL0, 0, 1, 0x1);
}

extern void (*hi_power_off)(void);
static int hi_pwr_init(void)
{
	struct device_node *np;
	int ret = 0;

	np = of_find_compatible_node(NULL, NULL, "hisilicon,hi_pwr");
	if (!np) {
	    ret = -ENODEV;
		goto err;
    }

	pwr_base = of_iomap(np, 0);
	if (!pwr_base) {
		pr_err("failed to map pwr base\n");
		ret = -ENOMEM;
		goto err;
	}

    hi_power_off = hi_pwr_off;
err:
	return ret;
}
early_initcall(hi_pwr_init);

