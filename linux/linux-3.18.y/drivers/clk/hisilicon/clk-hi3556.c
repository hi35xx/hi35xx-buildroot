/*
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

#include <linux/of_address.h>
#include <dt-bindings/clock/hi3556-clock.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <mach/io.h>

#include "clk.h"
#include "reset.h"

/******************************************************************************/
struct hi3556_pll_clock {
	u32		id;
	const char	*name;
	const char	*parent_name;
	u32		ctrl_reg1;
	u8		frac_shift;
	u8		frac_width;
	u8		postdiv1_shift;
	u8		postdiv1_width;
	u8		postdiv2_shift;
	u8		postdiv2_width;
	u32		ctrl_reg2;
	u8		fbdiv_shift;
	u8		fbdiv_width;
	u8		refdiv_shift;
	u8		refdiv_width;
};

struct hi3556_clk_pll {
	struct clk_hw	hw;
	u32		id;
	void __iomem	*ctrl_reg1;
	u8		frac_shift;
	u8		frac_width;
	u8		postdiv1_shift;
	u8		postdiv1_width;
	u8		postdiv2_shift;
	u8		postdiv2_width;
	void __iomem	*ctrl_reg2;
	u8		fbdiv_shift;
	u8		fbdiv_width;
	u8		refdiv_shift;
	u8		refdiv_width;
};

static struct hisi_fixed_rate_clock hi3556_fixed_rate_clks[] __initdata = {
	{ HI3556_FIXED_2376M, "2376m", NULL, CLK_IS_ROOT, 2376000000UL, },
	{ HI3556_FIXED_1188M, "1188m", NULL, CLK_IS_ROOT, 1188000000, },
	{ HI3556_FIXED_594M, "594m", NULL, CLK_IS_ROOT, 594000000, },
	{ HI3556_FIXED_297M, "297m", NULL, CLK_IS_ROOT, 297000000, },
	{ HI3556_FIXED_148P5M, "148p5m", NULL, CLK_IS_ROOT, 148500000, },
	{ HI3556_FIXED_74P25M, "74p25m", NULL, CLK_IS_ROOT, 74250000, },
	{ HI3556_FIXED_792M, "792m", NULL, CLK_IS_ROOT, 792000000, },
	{ HI3556_FIXED_475M, "475m", NULL, CLK_IS_ROOT, 475000000, },
	{ HI3556_FIXED_340M, "340m", NULL, CLK_IS_ROOT, 340000000, },
	{ HI3556_FIXED_72M, "72m", NULL, CLK_IS_ROOT, 72000000, },
	{ HI3556_FIXED_400M, "400m", NULL, CLK_IS_ROOT, 400000000, },
	{ HI3556_FIXED_200M, "200m", NULL, CLK_IS_ROOT, 200000000, },
	{ HI3556_FIXED_54M, "54m", NULL, CLK_IS_ROOT, 54000000, },
	{ HI3556_FIXED_27M, "27m", NULL, CLK_IS_ROOT, 1188000000, },
	{ HI3556_FIXED_37P125M, "37p125m", NULL, CLK_IS_ROOT, 37125000, },
	{ HI3556_FIXED_3000M, "3000m", NULL, CLK_IS_ROOT, 3000000000UL, },
	{ HI3556_FIXED_1500M, "1500m", NULL, CLK_IS_ROOT, 1500000000, },
	{ HI3556_FIXED_500M, "500m", NULL, CLK_IS_ROOT, 500000000, },
	{ HI3556_FIXED_250M, "250m", NULL, CLK_IS_ROOT, 250000000, },
	{ HI3556_FIXED_125M, "125m", NULL, CLK_IS_ROOT, 125000000, },
	{ HI3556_FIXED_1000M, "1000m", NULL, CLK_IS_ROOT, 1000000000, },
	{ HI3556_FIXED_600M, "600m", NULL, CLK_IS_ROOT, 600000000, },
	{ HI3556_FIXED_750M, "750m", NULL, CLK_IS_ROOT, 750000000, },
	{ HI3556_FIXED_150M, "150m", NULL, CLK_IS_ROOT, 150000000, },
	{ HI3556_FIXED_75M, "75m", NULL, CLK_IS_ROOT, 75000000, },
	{ HI3556_FIXED_300M, "300m", NULL, CLK_IS_ROOT, 300000000, },
	{ HI3556_FIXED_60M, "60m", NULL, CLK_IS_ROOT, 60000000, },
	{ HI3556_FIXED_214M, "214m", NULL, CLK_IS_ROOT, 214000000, },
	{ HI3556_FIXED_107M, "107m", NULL, CLK_IS_ROOT, 107000000, },
	{ HI3556_FIXED_100M, "100m", NULL, CLK_IS_ROOT, 100000000, },
	{ HI3556_FIXED_50M, "50m", NULL, CLK_IS_ROOT, 50000000, },
	{ HI3556_FIXED_25M, "25m", NULL, CLK_IS_ROOT, 25000000, },
	{ HI3556_FIXED_24M, "24m", NULL, CLK_IS_ROOT, 24000000, },
	{ HI3556_FIXED_3M, "3m", NULL, CLK_IS_ROOT, 3000000, },
	{ HI3556_FIXED_198M, "198m", NULL, CLK_IS_ROOT, 198000000, },
	{ HI3556_FIXED_396M, "396m", NULL, CLK_IS_ROOT, 396000000, },
};

static const char *sysaxi_mux_p[] __initconst = {"24m", "198m", };
static u32 sysaxi_mux_table[] = {0, 1};

static const char *fmc_mux_p[] __initconst = {
	"24m", "75m", "125m", "150m", "198m", "250m", "300m", "396m", };
static u32 fmc_mux_table[] = {0, 1, 2, 3, 4, 5, 6, 7};

static const char *mmc_mux_p[] __initconst = {
	"100m", "198m", "396m", "594m", "792m", "1188m", };
static u32 mmc_mux_table[] = {0, 1, 3, 4, 5, 7};

static const char *a17_mux_p[] __initconst = {
	"24m", "apll", "594m", "792m", };
static u32 a17_mux_table[] = {0, 1, 3, 4};

static const char *i2c_mux_p[] __initconst = {"clk_sysapb", "50m"};
static u32 i2c_mux_table[] = {0, 1};

static struct hisi_mux_clock hi3556_mux_clks[] __initdata = {
	{ HI3556_SYSAXI_MUX, "sysaxi_mux", sysaxi_mux_p,
		ARRAY_SIZE(sysaxi_mux_p),
		CLK_SET_RATE_PARENT, 0x34, 12, 2, 0, sysaxi_mux_table, },
	{ HI3556_FMC_MUX, "fmc_mux", fmc_mux_p, ARRAY_SIZE(fmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc0, 2, 3, 0, fmc_mux_table, },
	{ HI3556_MMC0_MUX, "mmc0_mux", mmc_mux_p, ARRAY_SIZE(mmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc4, 10, 3, 0, mmc_mux_table, },
	{ HI3556_MMC1_MUX, "mmc1_mux", mmc_mux_p, ARRAY_SIZE(mmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc4, 2, 3, 0, mmc_mux_table, },
	{ HI3556_MMC2_MUX, "mmc2_mux", mmc_mux_p, ARRAY_SIZE(mmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc4, 18, 3, 0, mmc_mux_table, },
	{ HI3556_A17_MUX, "a17_mux", a17_mux_p, ARRAY_SIZE(a17_mux_p),
		CLK_SET_RATE_PARENT, 0x34, 4, 3, 0, a17_mux_table, },
	{ HI3556_I2C_MUX, "i2c_mux", i2c_mux_p, ARRAY_SIZE(i2c_mux_p),
		CLK_SET_RATE_PARENT, 0xe4, 26, 1, 0, i2c_mux_table, },

};

static struct hisi_fixed_factor_clock
	hi3556_fixed_factor_clks[] __initdata = {
	{ HI3556_SYSAPB_CLK, "clk_sysapb", "sysaxi_mux", 1, 4,
		CLK_SET_RATE_PARENT},
	{ HI3556_MMC0_FAC_CLK, "mmc0_fac", "mmc0_mux", 1, 8,
		CLK_SET_RATE_PARENT},
	{ HI3556_MMC1_FAC_CLK, "mmc1_fac", "mmc1_mux", 1, 8,
		CLK_SET_RATE_PARENT},
	{ HI3556_MMC2_FAC_CLK, "mmc2_fac", "mmc2_mux", 1, 8,
		CLK_SET_RATE_PARENT},
};

static struct hisi_gate_clock hi3556_gate_clks[] __initdata = {
#ifdef CONFIG_HIFMC
	/* fmc */
	{ HI3556_FMC_CLK, "clk_fmc", "fmc_mux",
		CLK_SET_RATE_PARENT, 0xc0, 1, 0, },
#endif
	/* mmc */
	{ HI3556_MMC0_CLK, "clk_mmc0", "mmc0_fac",
		CLK_SET_RATE_PARENT, 0xc4, 9, 0, },
	{ HI3556_MMC1_CLK, "clk_mmc1", "mmc1_fac",
		CLK_SET_RATE_PARENT, 0xc4, 1, 0, },
	{ HI3556_MMC2_CLK, "clk_mmc2", "mmc2_fac",
		CLK_SET_RATE_PARENT, 0xc4, 17, 0, },

	/* usb ctrl */
	{ HI3556_USB2_CTRL_UTMI0_REQ, "usb2_cttl_utmi0_req", NULL,
		CLK_SET_RATE_PARENT, 0xb4, 5, 1, },
	{ HI3556_USB2_HRST_REQ, "usb2_hrst_req", NULL,
		CLK_SET_RATE_PARENT, 0xb4, 0, 1, },
	{ HI3556_USB3_CLK, "usb3_vcc_srst_req2", NULL,
		CLK_SET_RATE_PARENT, 0xb8, 0, 1, },
	
	/* uart */
	{ HI3556_UART0_CLK, "clk_uart0", "24m",
		CLK_SET_RATE_PARENT, 0xe4, 20, 0, },
	{ HI3556_UART1_CLK, "clk_uart1", "24m",
		CLK_SET_RATE_PARENT, 0xe4, 21, 0, },
	{ HI3556_UART2_CLK, "clk_uart2", "24m",
		CLK_SET_RATE_PARENT, 0xe4, 22, 0, },
	{ HI3556_UART3_CLK, "clk_uart3", "24m",
		CLK_SET_RATE_PARENT, 0xe4, 23, 0, },
	{ HI3556_UART4_CLK, "clk_uart4", "24m",
		CLK_SET_RATE_PARENT, 0xe4, 24, 0, },

	/* ethernet mac */
	{ HI3556_ETH_CLK, "clk_eth", NULL,
		CLK_IS_ROOT, 0xcc, 1, 0, },
	{ HI3556_ETH_MACIF_CLK, "clk_eth_macif", NULL,
		CLK_IS_ROOT, 0xcc, 3, 0, },

	/* spi */
	{ HI3556_SPI0_CLK, "clk_spi0", "clk_sysapb",
		CLK_SET_RATE_PARENT, 0xe4, 16, 0, },
	{ HI3556_SPI1_CLK, "clk_spi1", "clk_sysapb",
		CLK_SET_RATE_PARENT, 0xe4, 17, 0, },
	{ HI3556_SPI2_CLK, "clk_spi2", "clk_sysapb",
		CLK_SET_RATE_PARENT, 0xe4, 18, 0, },
	{ HI3556_SPI3_CLK, "clk_spi3", "clk_sysapb",
		CLK_SET_RATE_PARENT, 0xe4, 28, 0, },
};

static struct hi3556_pll_clock hi3556_pll_clks[] __initdata = {
	{ HI3556_APLL_CLK, "apll", NULL, 0x0, 0, 24, 24, 3, 28, 3,
		0x4, 0, 12, 12, 6},
};


#define to_pll_clk(_hw) container_of(_hw, struct hi3556_clk_pll, hw)

static void hi3556_calc_pll(u32 *frac_val,
		u32 *postdiv1_val,
		u32 *postdiv2_val,
		u32 *fbdiv_val,
		u32 *refdiv_val,
		unsigned long rate)
{
	u64 rem;
	*frac_val = 0;
	rem = do_div(rate, 1000000);
	*fbdiv_val = rate;
	*refdiv_val = 24;
	rem = rem * (1 << 24);
	do_div(rem, 1000000);
	*frac_val = rem;
}

static int clk_pll_set_rate(struct clk_hw *hw,
			unsigned long rate,
			unsigned long parent_rate)
{
	struct hi3556_clk_pll *clk = to_pll_clk(hw);
	u32 frac_val, postdiv1_val, postdiv2_val, fbdiv_val, refdiv_val;
	u32 val;

	/*Fixme  ignore postdives now because apll don't use them*/
	postdiv1_val = postdiv2_val = 0;

	hi3556_calc_pll(&frac_val, &postdiv1_val, &postdiv2_val,
			&fbdiv_val, &refdiv_val, rate);

	val = readl_relaxed(clk->ctrl_reg1);
	val &= ~(((1 << clk->frac_width) - 1) << clk->frac_shift);
	val &= ~(((1 << clk->postdiv1_width) - 1) << clk->postdiv1_shift);
	val &= ~(((1 << clk->postdiv2_width) - 1) << clk->postdiv2_shift);

	val |= frac_val << clk->frac_shift;
	val |= postdiv1_val << clk->postdiv1_shift;
	val |= postdiv2_val << clk->postdiv2_shift;
	writel_relaxed(val, clk->ctrl_reg1);

	val = readl_relaxed(clk->ctrl_reg2);
	val &= ~(((1 << clk->fbdiv_width) - 1) << clk->fbdiv_shift);
	val &= ~(((1 << clk->refdiv_width) - 1) << clk->refdiv_shift);

	val |= fbdiv_val << clk->fbdiv_shift;
	val |= refdiv_val << clk->refdiv_shift;
	writel_relaxed(val, clk->ctrl_reg2);

	return 0;
}

static unsigned long clk_pll_recalc_rate(struct clk_hw *hw,
		unsigned long parent_rate)
{
	struct hi3556_clk_pll *clk = to_pll_clk(hw);
	u64 frac_val, fbdiv_val, refdiv_val;
	u32 val;
	u64 tmp, rate;

	val = readl_relaxed(clk->ctrl_reg1);
	val = val >> clk->frac_shift;
	val &= ((1 << clk->frac_width) - 1);
	frac_val = val;

	val = readl_relaxed(clk->ctrl_reg2);
	val = val >> clk->fbdiv_shift;
	val &= ((1 << clk->fbdiv_width) - 1);
	fbdiv_val = val;

	val = readl_relaxed(clk->ctrl_reg2);
	val = val >> clk->refdiv_shift;
	val &= ((1 << clk->refdiv_width) - 1);
	refdiv_val = val;

	/* rate = 24000000 * (fbdiv + frac / (1<<24) ) / refdiv  */
	rate = 0;
	tmp = 24000000 * fbdiv_val;
	rate += tmp;
	do_div(rate, refdiv_val);

	return rate;
}


static long clk_pll_determine_rate(struct clk_hw *hw, unsigned long rate,
		unsigned long *best_parent_rate,
		struct clk **best_parent_p)
{
	return rate;
}

static struct clk_ops clk_pll_ops = {
	.set_rate = clk_pll_set_rate,
	.determine_rate = clk_pll_determine_rate,
	.recalc_rate = clk_pll_recalc_rate,
};

void __init hi3556_clk_register_pll(struct hi3556_pll_clock *clks,
		int nums, struct hisi_clock_data *data)
{
	void __iomem *base = data->base;
	int i;

	for (i = 0; i < nums; i++) {
		struct hi3556_clk_pll *p_clk;
		struct clk *clk;
		struct clk_init_data init;

		p_clk = kzalloc(sizeof(*p_clk), GFP_KERNEL);
		if (!p_clk)
			return;

		init.name = clks[i].name;
		init.flags = CLK_IS_BASIC;
		init.parent_names =
			(clks[i].parent_name ? &clks[i].parent_name : NULL);
		init.num_parents = (clks[i].parent_name ? 1 : 0);
		init.ops = &clk_pll_ops;

		p_clk->ctrl_reg1 = base + clks[i].ctrl_reg1;
		p_clk->frac_shift = clks[i].frac_shift;
		p_clk->frac_width = clks[i].frac_width;
		p_clk->postdiv1_shift = clks[i].postdiv1_shift;
		p_clk->postdiv1_width = clks[i].postdiv1_width;
		p_clk->postdiv2_shift = clks[i].postdiv2_shift;
		p_clk->postdiv2_width = clks[i].postdiv2_width;

		p_clk->ctrl_reg2 = base + clks[i].ctrl_reg2;
		p_clk->fbdiv_shift = clks[i].fbdiv_shift;
		p_clk->fbdiv_width = clks[i].fbdiv_width;
		p_clk->refdiv_shift = clks[i].refdiv_shift;
		p_clk->refdiv_width = clks[i].refdiv_width;
		p_clk->hw.init = &init;

		clk = clk_register(NULL, &p_clk->hw);
		if (IS_ERR(clk)) {
			kfree(p_clk);
			pr_err("%s: failed to register clock %s\n",
					__func__, clks[i].name);
			continue;
		}

		data->clk_data.clks[clks[i].id] = clk;
	}


}

static void __init hi3556_clk_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data;

	clk_data = hisi_clk_init(np, HI3556_NR_CLKS);
	if (!clk_data)
		return;
	if (IS_ENABLED(CONFIG_RESET_CONTROLLER))
		hisi_reset_init(np, HI3556_NR_RSTS);

	hisi_clk_register_fixed_rate(hi3556_fixed_rate_clks,
			ARRAY_SIZE(hi3556_fixed_rate_clks),
			clk_data);
	hi3556_clk_register_pll(hi3556_pll_clks,
			ARRAY_SIZE(hi3556_pll_clks), clk_data);

	hisi_clk_register_mux(hi3556_mux_clks,
			ARRAY_SIZE(hi3556_mux_clks),
			clk_data);
	hisi_clk_register_fixed_factor(hi3556_fixed_factor_clks,
			ARRAY_SIZE(hi3556_fixed_factor_clks), clk_data);
	hisi_clk_register_gate(hi3556_gate_clks,
			ARRAY_SIZE(hi3556_gate_clks), clk_data);
}

CLK_OF_DECLARE(hi3556_clk, "hisilicon,hi3556-clock", hi3556_clk_init);
