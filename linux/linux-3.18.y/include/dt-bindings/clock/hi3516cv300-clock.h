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

#ifndef __DTS_HI3516CV300_CLOCK_H
#define __DTS_HI3516CV300_CLOCK_H

/* clk in hi3516cv300 CRG */
/* fixed rate clocks */
#define HI3516CV300_FIXED_3M		1
#define HI3516CV300_FIXED_6M		2
#define HI3516CV300_FIXED_12M		3
#define HI3516CV300_FIXED_15M		4
#define HI3516CV300_FIXED_24M		5
#define HI3516CV300_FIXED_25M		6
#define HI3516CV300_FIXED_27M		7
#define HI3516CV300_FIXED_37P125M	8
#define HI3516CV300_FIXED_44M		9
#define HI3516CV300_FIXED_49P5M		10
#define HI3516CV300_FIXED_50M		11
#define HI3516CV300_FIXED_54M		12
#define HI3516CV300_FIXED_74P25M	13
#define HI3516CV300_FIXED_75M		14
#define HI3516CV300_FIXED_83P3M		15
#define HI3516CV300_FIXED_99M		16
#define HI3516CV300_FIXED_100M		17
#define HI3516CV300_FIXED_125M		18
#define HI3516CV300_FIXED_148P5M	19
#define HI3516CV300_FIXED_150M		20
#define HI3516CV300_FIXED_166P6M	21
#define HI3516CV300_FIXED_198M		22
#define HI3516CV300_FIXED_200M		23
#define HI3516CV300_FIXED_250M		24
#define HI3516CV300_FIXED_297M		25
#define HI3516CV300_FIXED_300M		26
#define HI3516CV300_FIXED_396M		27
#define HI3516CV300_FIXED_400M		28

/* mux clocks */
#define HI3516CV300_APB_CLK		30
#define HI3516CV300_UART_MUX		31
#define HI3516CV300_FMC_MUX		32
#define HI3516CV300_MMC0_MUX		33
#define HI3516CV300_MMC1_MUX		34
#define HI3516CV300_MMC2_MUX		35
#define HI3516CV300_MMC3_MUX		36
#define HI3516CV300_SENSOR_MUX		37
#define HI3516CV300_VIU_MUX		    38
#define HI3516CV300_VEDU_MUX		39
#define HI3516CV300_VPSS_MUX		40
#define HI3516CV300_VGS_MUX		    41
#define HI3516CV300_IVE_MUX		    42
#define HI3516CV300_PWM_MUX		    43

/* div clocks */
#define HI3516CV300_ISP_DIV		    45

/* gate clocks */
#define HI3516CV300_UART0_CLK		50
#define HI3516CV300_UART1_CLK		51
#define HI3516CV300_UART2_CLK		52
#define HI3516CV300_SPI0_CLK		53
#define HI3516CV300_SPI1_CLK		54
#define HI3516CV300_FMC_CLK			55
#define HI3516CV300_MMC0_CLK		56
#define HI3516CV300_MMC1_CLK		57
#define HI3516CV300_MMC2_CLK		58
#define HI3516CV300_MMC3_CLK		59
#define HI3516CV300_ETH_CLK			60
#define HI3516CV300_ETH_MACIF_CLK	61
#define HI3516CV300_USB2_BUS_CLK	62
#define HI3516CV300_UTMI0_CLK		63
#define HI3516CV300_USB2_CLK		64
#define HI3516CV300_DMAC_CLK		65
#define HI3516CV300_SENSOR_CLK		66
#define HI3516CV300_MIPI_CLK		67
#define HI3516CV300_ISP_CLK			68
#define HI3516CV300_VIU_CLK			69
#define HI3516CV300_VEDU_CLK		70
#define HI3516CV300_VPSS_CLK		71
#define HI3516CV300_VGS_CLK			72
#define HI3516CV300_JPGE_CLK		73
#define HI3516CV300_IVE_CLK			74
#define HI3516CV300_AIAO_CLK		75
#define HI3516CV300_PWM_CLK			76

#define HI3516CV300_CRG_NR_CLKS		80
#define HI3516CV300_CRG_NR_RSTS		0x120

/* clk in hi3518ev200 CRG */
/* mux clocks */
#define HI3516CV300_TIME00_CLK		1
#define HI3516CV300_TIME01_CLK		2
#define HI3516CV300_TIME10_CLK		3
#define HI3516CV300_TIME11_CLK		4

#define HI3516CV300_SYS_NR_CLKS		10
#define HI3516CV300_SYS_NR_RSTS		0x10
#endif	/* __DTS_HI3516CV300_CLOCK_H */
