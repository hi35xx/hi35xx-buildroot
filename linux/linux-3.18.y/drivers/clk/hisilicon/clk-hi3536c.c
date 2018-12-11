/*
 * Hi3536C Clock Driver
 *
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
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

#include <dt-bindings/clock/hi3536c-clock.h>
#include "clk.h"
#include "reset.h"

static struct
hisi_fixed_rate_clock hi3536c_fixed_rate_clks_crg[] __initdata = {
	{ HI3536C_FIXED_3M,		"3m",	NULL, CLK_IS_ROOT, 3000000, },
	{ HI3536C_FIXED_6M,		"6m",	NULL, CLK_IS_ROOT, 6000000, },
	{ HI3536C_FIXED_12M,	"12m",	NULL, CLK_IS_ROOT, 12000000, },
	{ HI3536C_FIXED_24M,	"24m",	NULL, CLK_IS_ROOT, 24000000, },
	{ HI3536C_FIXED_83P3M,	"83.3m", NULL, CLK_IS_ROOT, 83300000, },
	{ HI3536C_FIXED_100M,	"100m", NULL, CLK_IS_ROOT, 100000000, },
	{ HI3536C_FIXED_125M,	"125m", NULL, CLK_IS_ROOT, 125000000, },
	{ HI3536C_FIXED_150M,	"150m", NULL, CLK_IS_ROOT, 150000000, },
	{ HI3536C_FIXED_200M,	"200m", NULL, CLK_IS_ROOT, 200000000, },
	{ HI3536C_FIXED_250M,	"250m", NULL, CLK_IS_ROOT, 250000000, },
	{ HI3536C_FIXED_300M,	"300m", NULL, CLK_IS_ROOT, 300000000, },
	{ HI3536C_FIXED_324M,	"324m", NULL, CLK_IS_ROOT, 324000000, },
	{ HI3536C_FIXED_342M,	"342m", NULL, CLK_IS_ROOT, 342000000, },
	{ HI3536C_FIXED_342M,	"375m", NULL, CLK_IS_ROOT, 375000000, },
	{ HI3536C_FIXED_400M,	"400m", NULL, CLK_IS_ROOT, 400000000, },
	{ HI3536C_FIXED_448M,	"448m", NULL, CLK_IS_ROOT, 448000000, },
	{ HI3536C_FIXED_500M,	"500m", NULL, CLK_IS_ROOT, 500000000, },
	{ HI3536C_FIXED_540M,	"540m", NULL, CLK_IS_ROOT, 540000000, },
	{ HI3536C_FIXED_600M,	"600m", NULL, CLK_IS_ROOT, 600000000, },
	{ HI3536C_FIXED_750M,	"750m",	NULL, CLK_IS_ROOT, 750000000, },
	{ HI3536C_FIXED_1500M,	"1500m", NULL, CLK_IS_ROOT, 1500000000UL, },
};

static const char *sysaxi_mux_p[] __initconst = {
	"24m", "200m", "250m", "300m"};
/* If syaaxi bus clock is 200MHz, so the APB clock is 50MHz, factor is 4 */
static const char *uart_mux_p[] __initconst = {"sysaxi_mux", "24m", "2m"};
static const char *fmc_mux_p[] __initconst = {
	"24m", "83.3m", "150m"};

static u32 sysaxi_mux_table[] = {0, 1, 2, 3};
static u32 uart_mux_table[] = {0, 1, 2};
static u32 fmc_mux_table[] = {0, 1, 2};

static struct hisi_mux_clock hi3536c_mux_clks_crg[] __initdata = {
	{ HI3536C_SYSAXI_CLK, "sysaxi_mux", sysaxi_mux_p,
		ARRAY_SIZE(sysaxi_mux_p),
		CLK_SET_RATE_PARENT, 0x50, 2, 2, 0, sysaxi_mux_table, },
	{ HI3536C_FMC_MUX, "fmc_mux", fmc_mux_p, ARRAY_SIZE(fmc_mux_p),
		CLK_SET_RATE_PARENT, 0x148, 2, 2, 0, fmc_mux_table, },
	{ HI3536C_UART_MUX, "uart_mux", uart_mux_p,
		ARRAY_SIZE(uart_mux_p),
		CLK_SET_RATE_PARENT, 0x154, 19, 2, 0, uart_mux_table, },
};

static struct hisi_fixed_factor_clock
				hi3536c_fixed_factor_clks[] __initdata = {
	{ HI3536C_SYSAXI_CLK, "clk_sysaxi", "sysaxi_mux", 1, 4,
		CLK_SET_RATE_PARENT},
};

static struct hisi_gate_clock hi3536c_gate_clks[] __initdata = {
#ifdef CONFIG_HIFMC
	/* fmc */
	{ HI3536C_FMC_CLK, "clk_fmc", "fmc_mux",
		CLK_SET_RATE_PARENT, 0x148, 1, 0, },
#endif
	/* uart */
	{ HI3536C_UART0_CLK, "clk_uart0", "24m",
		CLK_SET_RATE_PARENT, 0x154, 15, 0, },
	{ HI3536C_UART1_CLK, "clk_uart1", "24m",
		CLK_SET_RATE_PARENT, 0x154, 16, 0, },
	{ HI3536C_UART2_CLK, "clk_uart2", "24m",
		CLK_SET_RATE_PARENT, 0x154, 17, 0, },
	/* ethernet mac */
	{ HI3536C_ETH_PHY_CLK, "clk_eth_phy", NULL,
		CLK_SET_RATE_PARENT, 0x14c, 6, 0, },
	{ HI3536C_ETH_PUB_CLK, "clk_eth_pub", "clk_eth_phy",
		CLK_SET_RATE_PARENT, 0x14c, 7, 0, },
	{ HI3536C_ETH_CLK, "clk_eth", "clk_eth_pub",
		CLK_SET_RATE_PARENT, 0x14c, 1, 0, },
	{ HI3536C_ETH_MACIF_CLK, "clk_eth_macif", "clk_eth_pub",
		CLK_SET_RATE_PARENT, 0x14c, 3, 0, },
	/* ethernet mac1 */
	{ HI3536C_ETH1_PHY_CLK, "clk_eth1_phy", NULL,
		CLK_SET_RATE_PARENT, 0x14c, 14, 0, },
	{ HI3536C_ETH1_CLK, "clk_eth1", "clk_eth1_phy",
		CLK_SET_RATE_PARENT, 0x14c, 9, 0, },
	{ HI3536C_ETH_MACIF1_CLK, "clk_eth_macif1", "clk_eth1_phy",
		CLK_SET_RATE_PARENT, 0x14c, 11, 0, },
	/* spi */
	{ HI3536C_SPI0_CLK, "clk_spi0", "clk_sysaxi",
		CLK_SET_RATE_PARENT, 0x154, 13, 0, },
};

static void __init hi3536c_clk_crg_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data;
	unsigned int count = 0;

	clk_data = hisi_clk_init(np, HI3536C_CRG_NR_CLKS);
	if (!clk_data)
		return;

	hisi_clk_register_fixed_rate(hi3536c_fixed_rate_clks_crg,
			ARRAY_SIZE(hi3536c_fixed_rate_clks_crg),
			clk_data);
	hisi_clk_register_mux(hi3536c_mux_clks_crg,
			ARRAY_SIZE(hi3536c_mux_clks_crg), clk_data);
	hisi_clk_register_fixed_factor(hi3536c_fixed_factor_clks,
			ARRAY_SIZE(hi3536c_fixed_factor_clks), clk_data);
	hisi_clk_register_gate(hi3536c_gate_clks,
			ARRAY_SIZE(hi3536c_gate_clks), clk_data);

	if (!of_property_read_u32(np, "#reset-cells", &count) && (count == 2))
		hisi_reset_init(np, HI3536C_CRG_NR_RSTS);
}

static const char *timer_mux_p[] __initconst = { "3m", "clk_sysapb" };
static u32 timer_mux_table[] = {0, 1};

static struct hisi_mux_clock hi3536c_mux_clks_sys[] __initdata = {
	{ HI3536C_TIME0_0_CLK, "timer00", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 16, 1, 0, timer_mux_table, },

	{ HI3536C_TIME0_1_CLK, "timer01", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 18, 1, 0, timer_mux_table, },

	{ HI3536C_TIME1_2_CLK, "timer12", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 20, 1, 0, timer_mux_table, },

	{ HI3536C_TIME1_3_CLK, "timer13", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 22, 1, 0, timer_mux_table, },
};

static void __init hi3536c_clk_sys_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data;
	unsigned int count = 0;

	clk_data = hisi_clk_init(np, HI3536C_SYS_NR_CLKS);
	if (!clk_data)
		return;

	hisi_clk_register_mux(hi3536c_mux_clks_sys,
			ARRAY_SIZE(hi3536c_mux_clks_sys), clk_data);

	if (!of_property_read_u32(np, "#reset-cells", &count) && (count == 2))
		hisi_reset_init(np, HI3536C_SYS_NR_RSTS);
}

CLK_OF_DECLARE(hi3536c_clk_crg, "hisilicon,hi3536c-clock",
							hi3536c_clk_crg_init);
CLK_OF_DECLARE(hi3536c_clk_sys, "hisilicon,sysctrl", hi3536c_clk_sys_init);

