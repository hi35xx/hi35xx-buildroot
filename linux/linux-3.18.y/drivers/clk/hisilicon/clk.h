/*
 * Hisilicon Hi3620 clock gate driver
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef	__HISI_CLK_H
#define	__HISI_CLK_H

#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/spinlock.h>

#define CLK(_id, _mask, _value, _rstbit, _rate, _ops) \
{.id = _id,         \
	.name = #_id,      \
	.offset = _id,     \
	.mask = _mask,     \
	.value = _value,   \
	.rstbit = _rstbit, \
	.rate = _rate,     \
	.ops = _ops,}

#define CLK_SHARED(_id, _off, _mask, _value, _rstbit, _rate, _ops) \
{.id = _id,         \
	.name = #_id,      \
	.offset = _off,     \
	.mask = _mask,     \
	.value = _value,   \
	.rstbit = _rstbit, \
	.rate = _rate,     \
	.ops = _ops,}

struct hiclk_hw {
	int id;
	const char *name;
	u32 offset;
	u32 mask;
	u32 value;
	u32 rstbit;

	unsigned long rate;
	struct clk_ops *ops;
	struct clk_hw hw;

#define CLKHW_RESET         (0x01)
#define CLKHW_ENABLE        (0x02)

	u32 flags;
};

extern struct clk_ops clk_ops_hiusb2_host;
extern struct clk_ops clk_ops_hiusb3;
extern struct clk_ops clk_ops_himci;

#define to_hiclk_hw(_hw) container_of(_hw, struct hiclk_hw, hw)

struct hisi_clock_data {
	struct clk_onecell_data	clk_data;
	void __iomem		*base;
};

struct hisi_fixed_rate_clock {
	unsigned int		id;
	char			*name;
	const char		*parent_name;
	unsigned long		flags;
	unsigned long		fixed_rate;
};

struct hisi_fixed_factor_clock {
	unsigned int		id;
	char			*name;
	const char		*parent_name;
	unsigned long		mult;
	unsigned long		div;
	unsigned long		flags;
};

struct hisi_mux_clock {
	unsigned int		id;
	const char		*name;
	const char		**parent_names;
	u8			num_parents;
	unsigned long		flags;
	unsigned long		offset;
	u8			shift;
	u8			width;
	u8			mux_flags;
	u32			*table;
	const char		*alias;
};

struct hisi_divider_clock {
	unsigned int		id;
	const char		*name;
	const char		*parent_name;
	unsigned long		flags;
	unsigned long		offset;
	u8			shift;
	u8			width;
	u8			div_flags;
	struct clk_div_table	*table;
	const char		*alias;
};

struct hisi_gate_clock {
	unsigned int		id;
	const char		*name;
	const char		*parent_name;
	unsigned long		flags;
	unsigned long		offset;
	u8			bit_idx;
	u8			gate_flags;
	const char		*alias;
};

struct clk *hisi_register_clkgate_sep(struct device *, const char *,
				const char *, unsigned long,
				void __iomem *, u8,
				u8, spinlock_t *);

struct hisi_clock_data __init *hisi_clk_init(struct device_node *, int);
struct clk *clk_register_ops_table(struct device *,
		struct hiclk_hw *,
		struct clk_ops *);
void __init hisi_clk_register_fixed_rate(struct hisi_fixed_rate_clock *,
					int, struct hisi_clock_data *);
void __init hisi_clk_register_fixed_factor(struct hisi_fixed_factor_clock *,
					int, struct hisi_clock_data *);
void __init hisi_clk_register_mux(struct hisi_mux_clock *, int,
				struct hisi_clock_data *);
void __init hisi_clk_register_divider(struct hisi_divider_clock *,
				int, struct hisi_clock_data *);
void __init hisi_clk_register_gate(struct hisi_gate_clock *,
					int, struct hisi_clock_data *);
void __init hisi_clk_register_gate_sep(struct hisi_gate_clock *,
					int, struct hisi_clock_data *);
void __init hisi_clk_register_ops(struct hiclk_hw *,
					int, struct hisi_clock_data *);
#endif	/* __HISI_CLK_H */
