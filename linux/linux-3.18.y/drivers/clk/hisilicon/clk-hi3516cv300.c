/*
 * Hi3516cv300 Clock Driver
 *
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

#include <dt-bindings/clock/hi3516cv300-clock.h>
#include "clk.h"
#include "reset.h"

static struct
hisi_fixed_rate_clock hi3516cv300_fixed_rate_clks_crg[] __initdata = {
	{ HI3516CV300_FIXED_3M, "3m", NULL, CLK_IS_ROOT, 3000000, },
	{ HI3516CV300_FIXED_6M, "6m", NULL, CLK_IS_ROOT, 6000000, },
	{ HI3516CV300_FIXED_12M, "12m", NULL, CLK_IS_ROOT, 12000000, },
	{ HI3516CV300_FIXED_15M, "15m", NULL, CLK_IS_ROOT, 15000000, },
	{ HI3516CV300_FIXED_24M, "24m", NULL, CLK_IS_ROOT, 24000000, },
	{ HI3516CV300_FIXED_25M, "25m", NULL, CLK_IS_ROOT, 25000000, },
	{ HI3516CV300_FIXED_27M, "27m", NULL, CLK_IS_ROOT, 27000000, },
	{ HI3516CV300_FIXED_37P125M, "37.125m", NULL, CLK_IS_ROOT, 37125000, },
	{ HI3516CV300_FIXED_44M, "44m", NULL, CLK_IS_ROOT, 44000000, },
	{ HI3516CV300_FIXED_49P5M, "49.5m", NULL, CLK_IS_ROOT, 49500000, },
	{ HI3516CV300_FIXED_50M, "50m", NULL, CLK_IS_ROOT, 50000000, },
	{ HI3516CV300_FIXED_54M, "54m", NULL, CLK_IS_ROOT, 54000000, },
	{ HI3516CV300_FIXED_74P25M, "74.25m", NULL, CLK_IS_ROOT, 74250000, },
	{ HI3516CV300_FIXED_75M, "75m", NULL, CLK_IS_ROOT, 75000000, },
	{ HI3516CV300_FIXED_83P3M, "83.3m", NULL, CLK_IS_ROOT, 83300000, },
	{ HI3516CV300_FIXED_99M, "99m", NULL, CLK_IS_ROOT, 99000000, },
	{ HI3516CV300_FIXED_100M, "100m", NULL, CLK_IS_ROOT, 100000000, },
	{ HI3516CV300_FIXED_125M, "125m", NULL, CLK_IS_ROOT, 125000000, },
	{ HI3516CV300_FIXED_148P5M, "148.5m", NULL, CLK_IS_ROOT, 148500000, },
	{ HI3516CV300_FIXED_150M, "150m", NULL, CLK_IS_ROOT, 150000000, },
	{ HI3516CV300_FIXED_166P6M, "166.6m", NULL, CLK_IS_ROOT, 166600000, },
	{ HI3516CV300_FIXED_198M, "198m", NULL, CLK_IS_ROOT, 198000000, },
	{ HI3516CV300_FIXED_200M, "200m", NULL, CLK_IS_ROOT, 200000000, },
	{ HI3516CV300_FIXED_250M, "250m", NULL, CLK_IS_ROOT, 250000000, },
	{ HI3516CV300_FIXED_297M, "297m", NULL, CLK_IS_ROOT, 297000000, },
	{ HI3516CV300_FIXED_300M, "300m", NULL, CLK_IS_ROOT, 300000000, },
	{ HI3516CV300_FIXED_396M, "396m", NULL, CLK_IS_ROOT, 396000000, },
	{ HI3516CV300_FIXED_400M, "400m", NULL, CLK_IS_ROOT, 400000000, },
};

static const char *apb_mux_p[] __initconst = {"24m", "50m"};
static const char *uart_mux_p[] __initconst = {"24m", "6m"};
static const char *fmc_mux_p[] __initconst = {
	"24m", "83.3m", "148.5m", "198m", "297m"
};
static const char *mmc_mux_p[] __initconst = {"49.5m"};
static const char *mmc2_mux_p[] __initconst = {"99m", "49.5m"};
static const char *sensor_mux_p[] = {"74.25m", "37.125m", "54m", "27m", "24m", "25m", "24m", "25m"};
static const char *viu_mux_p[] = {"83.3m", "125m", "148.5m", "198m", "250m"};
static const char *vedu_mux_p[] = {"166.6m", "198m"};
static const char *vpss_mux_p[] = {"148.5m", "198m", "250m"};
static const char *vgs_mux_p[] = {"198m", "250m", "297m"};
static const char *ive_mux_p[] = {"198m", "250m", "297m"};
static const char *pwm_mux_p[] = {"3m", "50m", "24m", "24m"};

static u32 apb_mux_table[] = {0, 1};
static u32 uart_mux_table[] = {0, 1};
static u32 fmc_mux_table[] = {0, 1, 2, 3, 4};
static u32 mmc_mux_table[] = {0};
static u32 mmc2_mux_table[] = {0, 2};
static u32 pwm_mux_table[] = {0, 1, 2, 3};

static u32 sensor_mux_p_table[] = {0, 1, 2, 3, 4, 5, 6, 7};
static u32 viu_mux_p_table[] = {0, 1, 2, 3, 4};
static u32 vedu_mux_p_table[] = {0, 1};
static u32 vpss_mux_p_table[] = {0, 1, 2};
static u32 vgs_mux_p_table[] = {0, 1, 2};
static u32 ive_mux_p_table[] = {0, 1, 2};
static struct hisi_mux_clock hi3516cv300_mux_clks_crg[] __initdata = {
	{ HI3516CV300_APB_CLK, "apb", apb_mux_p, ARRAY_SIZE(apb_mux_p),
		CLK_SET_RATE_PARENT, 0x30, 0, 1, 0, apb_mux_table, },
	{ HI3516CV300_UART_MUX, "uart_mux", uart_mux_p, ARRAY_SIZE(uart_mux_p),
		CLK_SET_RATE_PARENT, 0xe4, 19, 1, 0, uart_mux_table, },
	{ HI3516CV300_FMC_MUX, "fmc_mux", fmc_mux_p, ARRAY_SIZE(fmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc0, 2, 3, 0, fmc_mux_table, },
	{ HI3516CV300_MMC0_MUX, "mmc0_mux", mmc_mux_p, ARRAY_SIZE(mmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc4, 4, 2, 0, mmc_mux_table, },
	{ HI3516CV300_MMC1_MUX, "mmc1_mux", mmc_mux_p, ARRAY_SIZE(mmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc4, 12, 2, 0, mmc_mux_table, },
	{ HI3516CV300_MMC2_MUX, "mmc2_mux", mmc2_mux_p, ARRAY_SIZE(mmc2_mux_p),
		CLK_SET_RATE_PARENT, 0xc4, 20, 2, 0, mmc2_mux_table, },
	{ HI3516CV300_MMC3_MUX, "mmc3_mux", mmc_mux_p, ARRAY_SIZE(mmc_mux_p),
		CLK_SET_RATE_PARENT, 0xc8, 4, 2, 0, mmc_mux_table, },
	{ HI3516CV300_PWM_MUX, "pwm_mux", pwm_mux_p, ARRAY_SIZE(pwm_mux_p),
		CLK_SET_RATE_PARENT, 0x38, 2, 2, 0, pwm_mux_table, },

	/* media mux clock*/
	{ HI3516CV300_SENSOR_MUX, "sensor_mux", sensor_mux_p, ARRAY_SIZE(sensor_mux_p),
		CLK_SET_RATE_PARENT, 0x2c, 23, 3, 0, sensor_mux_p_table, },
	{ HI3516CV300_VIU_MUX, "viu_mux", viu_mux_p, ARRAY_SIZE(viu_mux_p),
		CLK_SET_RATE_PARENT, 0x2c, 2, 3, 0, viu_mux_p_table, },
	{ HI3516CV300_VEDU_MUX, "vedu_mux", vedu_mux_p, ARRAY_SIZE(vedu_mux_p),
		CLK_SET_RATE_PARENT, 0x40, 10, 2, 0, vedu_mux_p_table, },
	{ HI3516CV300_VPSS_MUX, "vpss_mux", vpss_mux_p, ARRAY_SIZE(vpss_mux_p),
		CLK_SET_RATE_PARENT, 0x48, 10, 2, 0, vpss_mux_p_table, },
	{ HI3516CV300_VGS_MUX, "vgs_mux", vgs_mux_p, ARRAY_SIZE(vgs_mux_p),
		CLK_SET_RATE_PARENT, 0x5c, 10, 2, 0, vgs_mux_p_table, },
	{ HI3516CV300_IVE_MUX, "ive_mux", ive_mux_p, ARRAY_SIZE(ive_mux_p),
		CLK_SET_RATE_PARENT, 0x6c, 2, 2, 0, ive_mux_p_table, },
};

static struct hisi_divider_clock hi3516cv300_div_clks[] = {
	{ HI3516CV300_ISP_DIV, "isp_div", "viu_mux", 0, 0x2c, 17, 1, 0, NULL, },
};

static struct hisi_gate_clock hi3516cv300_gate_clks_crg[] __initdata = {
	/* uart */
	{ HI3516CV300_UART0_CLK, "clk_uart0", "uart_mux",
		CLK_SET_RATE_PARENT, 0xe4, 15, 0, },
	{ HI3516CV300_UART1_CLK, "clk_uart1", "uart_mux",
		CLK_SET_RATE_PARENT, 0xe4, 16, 0, },
	{ HI3516CV300_UART2_CLK, "clk_uart2", "uart_mux",
		CLK_SET_RATE_PARENT, 0xe4, 17, 0, },
	/* spi */
	{ HI3516CV300_SPI0_CLK, "clk_spi0", "100m",
		CLK_SET_RATE_PARENT, 0xe4, 13, 0, },
	{ HI3516CV300_SPI1_CLK, "clk_spi1", "100m",
		CLK_SET_RATE_PARENT, 0xe4, 14, 0, },
	/* fmc */
	{ HI3516CV300_FMC_CLK, "clk_fmc", "fmc_mux",
		CLK_SET_RATE_PARENT, 0xc0, 1, 0, },
	{ HI3516CV300_MMC0_CLK, "clk_mmc0", "mmc0_mux",
		CLK_SET_RATE_PARENT, 0xc4, 1, 0, },
	{ HI3516CV300_MMC1_CLK, "clk_mmc1", "mmc1_mux",
		CLK_SET_RATE_PARENT, 0xc4, 9, 0, },
	{ HI3516CV300_MMC2_CLK, "clk_mmc2", "mmc2_mux",
		CLK_SET_RATE_PARENT, 0xc4, 17, 0, },
	{ HI3516CV300_MMC3_CLK, "clk_mmc3", "mmc3_mux",
		CLK_SET_RATE_PARENT, 0xc8, 1, 0, },
	/* ethernet mac */
	{ HI3516CV300_ETH_CLK, "clk_eth", NULL, CLK_IS_ROOT, 0xec, 1, 0, },
	{ HI3516CV300_USB2_BUS_CLK, "clk_usb2_bus", NULL,
		CLK_SET_RATE_PARENT, 0xb8, 8, 1, },
	{ HI3516CV300_UTMI0_CLK, "clk_utmi0", NULL,
		CLK_SET_RATE_PARENT, 0xb8, 11, 1, },
	{ HI3516CV300_USB2_CLK, "clk_usb2", NULL,
		CLK_SET_RATE_PARENT, 0xb8, 12, 1, },
	{ HI3516CV300_DMAC_CLK, "clk_dmac", NULL, CLK_IS_ROOT, 0xd8, 5, 0, },

	{ HI3516CV300_PWM_CLK, "clk_pwm", "pwm_mux", CLK_SET_RATE_PARENT,
		0x38, 1, 0, },

	/* media gate clock*/
	{ HI3516CV300_SENSOR_CLK, "clk_sensor", "sensor_mux", CLK_SET_RATE_PARENT,
		0x2c, 26, 0, },
	{ HI3516CV300_MIPI_CLK, "clk_mipi", NULL, CLK_SET_RATE_PARENT,
		0x2c, 15, 0, },
	{ HI3516CV300_ISP_CLK, "clk_isp", "isp_div", CLK_SET_RATE_PARENT, 0x2c, 18, 0, },
	{ HI3516CV300_VIU_CLK, "clk_viu", "viu_mux", CLK_SET_RATE_PARENT,
		0x2c, 0, 0, },
	{ HI3516CV300_VEDU_CLK, "clk_vedu", "vedu_mux", CLK_SET_RATE_PARENT,
		0x40, 1, 0, },
	{ HI3516CV300_VPSS_CLK, "clk_vpss", "vpss_mux", CLK_SET_RATE_PARENT,
		0x48, 1, 0, },
	{ HI3516CV300_VGS_CLK, "clk_vgs", "vgs_mux", CLK_SET_RATE_PARENT,
		0x5c, 1, 0, },
	{ HI3516CV300_JPGE_CLK, "clk_jpge", NULL, CLK_SET_RATE_PARENT,
		0x60, 1, 0, },
	{ HI3516CV300_IVE_CLK, "clk_ive", "ive_mux", CLK_SET_RATE_PARENT,
		0x6c, 1, 0, },
	{ HI3516CV300_AIAO_CLK, "clk_aiao", NULL, CLK_SET_RATE_PARENT,
		0x8c, 1, 0, },
};

static void __init hi3516cv300_clk_crg_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data;
	unsigned int count = 0;

	clk_data = hisi_clk_init(np, HI3516CV300_CRG_NR_CLKS);
	if (!clk_data)
		return;

	hisi_clk_register_fixed_rate(hi3516cv300_fixed_rate_clks_crg,
			ARRAY_SIZE(hi3516cv300_fixed_rate_clks_crg),
			clk_data);
	hisi_clk_register_mux(hi3516cv300_mux_clks_crg,
			ARRAY_SIZE(hi3516cv300_mux_clks_crg), clk_data);
	hisi_clk_register_divider(hi3516cv300_div_clks,
			ARRAY_SIZE(hi3516cv300_div_clks), clk_data);
	hisi_clk_register_gate(hi3516cv300_gate_clks_crg,
			ARRAY_SIZE(hi3516cv300_gate_clks_crg), clk_data);

	if (!of_property_read_u32(np, "#reset-cells", &count) && (count == 2))
		hisi_reset_init(np, HI3516CV300_CRG_NR_RSTS);
}

static const char *timer_mux_p[] __initconst = { "3m", "apb" };
static u32 timer_mux_table[] = {0, 1};

static struct hisi_mux_clock hi3516cv300_mux_clks_sys[] __initdata = {
	{ HI3516CV300_TIME00_CLK, "timer00", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 16, 1, 0, timer_mux_table, },

	{ HI3516CV300_TIME01_CLK, "timer01", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 18, 1, 0, timer_mux_table, },

	{ HI3516CV300_TIME10_CLK, "timer10", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 20, 1, 0, timer_mux_table, },

	{ HI3516CV300_TIME11_CLK, "timer11", timer_mux_p,
		ARRAY_SIZE(timer_mux_p), CLK_SET_RATE_PARENT,
		0x0, 22, 1, 0, timer_mux_table, },
};

static void __init hi3516cv300_clk_sys_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data;
	unsigned int count = 0;

	clk_data = hisi_clk_init(np, HI3516CV300_SYS_NR_CLKS);
	if (!clk_data)
		return;

	hisi_clk_register_mux(hi3516cv300_mux_clks_sys,
			ARRAY_SIZE(hi3516cv300_mux_clks_sys), clk_data);

	if (!of_property_read_u32(np, "#reset-cells", &count) && (count == 2))
		hisi_reset_init(np, HI3516CV300_SYS_NR_RSTS);
}

CLK_OF_DECLARE(hi3516cv300_clk, "hisilicon,hi3516cv300-crg",
		hi3516cv300_clk_crg_init);
CLK_OF_DECLARE(hi3516cv300_clk_sys, "hisilicon,hi3516cv300-sys",
		hi3516cv300_clk_sys_init);
