/*
 * Hisilicon clock separated gate driver
 *
 * Copyright (c) 2012-2013 Hisilicon Limited.
 * Copyright (c) 2012-2013 Linaro Limited.
 *
 * Author: Haojian Zhuang <haojian.zhuang@linaro.org>
 *	   Xin Li <li.xin@linaro.org>
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
 */

#include <linux/kernel.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/clk.h>

#include "clk.h"

struct clk *clk_register_ops_table(struct device *dev,
					struct hiclk_hw *hihw,
					struct clk_ops *clkops)
{
	struct clk *clk;
	struct clk_init_data init;

	init.name = hihw->name;
	init.flags = CLK_IS_ROOT | CLK_IS_BASIC;
	init.parent_names = NULL;
	init.num_parents = 0;
	init.ops = hihw->ops;

	hihw->hw.init = &init;

	clk = clk_register(dev, &hihw->hw);
	if (IS_ERR(clk)) {
		pr_err("%s: register clock fail.\n", __func__);
		return NULL;
	}

	return clk;
}
