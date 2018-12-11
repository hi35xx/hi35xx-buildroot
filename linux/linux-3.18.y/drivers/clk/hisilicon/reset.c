/*
 * Hisilicon Reset Controller driver
 *
 * Copyright (c) 2015 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset-controller.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>

#define	HISI_RESET_BIT_SHIFT	0
#define	HISI_RESET_BIT_WIDTH	16
#define	HISI_RESET_OFFSET_SHIFT	16
#define	HISI_RESET_OFFSET_WIDTH	16

struct hisi_reset_controller {
	spinlock_t					lock;
	void __iomem				*membase;
	struct reset_controller_dev	rcdev;
};


#define to_hisi_reset_controller(rcdev)  \
	container_of(rcdev, struct hisi_reset_controller, rcdev)

/*31                        16                         0
   |---reset_spec->args[0]---|---reset_spec->args[1]---|
   |-------reg_offset--------|--------reg_bit----------|*/
static int hisi_reset_of_xlate(struct reset_controller_dev *rcdev,
			const struct of_phandle_args *reset_spec)
{
	unsigned int offset, bit, id;
	__be32 *addr;
	u64 size;

	if (WARN_ON(reset_spec->args_count != rcdev->of_reset_n_cells))
		return -EINVAL;

	addr = (__be32 *)of_get_address(rcdev->of_node, 0, &size, NULL);
	if (!addr)
		return -EINVAL;

	if (reset_spec->args[1] >= 32
		|| reset_spec->args[0] + reset_spec->args[1] / 8 > size)
		return -EINVAL;

	offset = reset_spec->args[0] & (BIT(HISI_RESET_OFFSET_WIDTH) - 1);
	bit = (reset_spec->args[1] & (BIT(HISI_RESET_BIT_WIDTH) - 1));
	id = offset << HISI_RESET_OFFSET_SHIFT | bit;

	return id;
}

static int hisi_reset_assert(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	struct hisi_reset_controller *rstc = to_hisi_reset_controller(rcdev);
	unsigned int offset, bit;
	unsigned long flags;
	u32 reg;

	offset = id >> HISI_RESET_OFFSET_SHIFT;
	offset &= (BIT(HISI_RESET_OFFSET_WIDTH) - 1);
	bit = id & (BIT(HISI_RESET_BIT_WIDTH) - 1);

	spin_lock_irqsave(&rstc->lock, flags);

	reg = readl(rstc->membase + offset);
	writel(reg | BIT(bit), rstc->membase + offset);

	spin_unlock_irqrestore(&rstc->lock, flags);

	return 0;
}

static int hisi_reset_deassert(struct reset_controller_dev *rcdev,
				unsigned long id)
{
	struct hisi_reset_controller *rstc = to_hisi_reset_controller(rcdev);
	unsigned int offset, bit;
	unsigned long flags;
	u32 reg;

	offset = id >> HISI_RESET_OFFSET_SHIFT;
	offset &= (BIT(HISI_RESET_OFFSET_WIDTH) - 1);
	bit = id & (BIT(HISI_RESET_BIT_WIDTH) - 1);

	spin_lock_irqsave(&rstc->lock, flags);

	reg = readl(rstc->membase + offset);
	writel(reg & ~BIT(bit), rstc->membase + offset);

	spin_unlock_irqrestore(&rstc->lock, flags);

	return 0;
}

static struct reset_control_ops hisi_reset_ops = {
	.assert		= hisi_reset_assert,
	.deassert	= hisi_reset_deassert,
};

int __init hisi_reset_init(struct device_node *np,
					     int nr_rsts)
{
	struct hisi_reset_controller *rstc;

	rstc = kzalloc(sizeof(*rstc), GFP_KERNEL);
	if (!rstc)
		return -ENOMEM;

	rstc->membase = of_iomap(np, 0);
	if (!rstc->membase)
		return -EINVAL;

	spin_lock_init(&rstc->lock);

	rstc->rcdev.owner = THIS_MODULE;
	rstc->rcdev.nr_resets = nr_rsts;
	rstc->rcdev.ops = &hisi_reset_ops;
	rstc->rcdev.of_node = np;
	rstc->rcdev.of_reset_n_cells = 2;
	rstc->rcdev.of_xlate = hisi_reset_of_xlate;

	return reset_controller_register(&rstc->rcdev);
}
