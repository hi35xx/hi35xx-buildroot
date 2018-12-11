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

#ifndef __DTS_HI3556_CLOCK_H
#define __DTS_HI3556_CLOCK_H

/* fixed rate */
#define HI3556_FIXED_2376M		1
#define HI3556_FIXED_1188M		2
#define HI3556_FIXED_594M		3
#define HI3556_FIXED_297M		4
#define HI3556_FIXED_148P5M		5
#define HI3556_FIXED_74P25M		6
#define HI3556_FIXED_792M		7
#define HI3556_FIXED_475M		8
#define HI3556_FIXED_340M		9
#define HI3556_FIXED_72M		10
#define HI3556_FIXED_400M		11
#define HI3556_FIXED_200M		12
#define HI3556_FIXED_54M		13
#define HI3556_FIXED_27M		14
#define HI3556_FIXED_37P125M	15
#define HI3556_FIXED_3000M		16
#define HI3556_FIXED_1500M		17
#define HI3556_FIXED_500M		18
#define HI3556_FIXED_250M		19
#define HI3556_FIXED_125M		20
#define HI3556_FIXED_1000M		21
#define HI3556_FIXED_600M		22
#define HI3556_FIXED_750M		23
#define HI3556_FIXED_150M		24
#define HI3556_FIXED_75M		25
#define HI3556_FIXED_300M		26
#define HI3556_FIXED_60M		27
#define HI3556_FIXED_214M		28
#define HI3556_FIXED_107M		29
#define HI3556_FIXED_100M		30
#define HI3556_FIXED_50M		31
#define HI3556_FIXED_25M		32
#define HI3556_FIXED_24M		33
#define HI3556_FIXED_3M                34
#define HI3556_FIXED_198M               35
#define HI3556_FIXED_396M               36

/* mux clocks */
#define HI3556_FMC_MUX			64
#define HI3556_I2C_MUX			65
#define HI3556_UART_MUX			66
#define HI3556_SYSAXI_MUX		67
#define HI3556_A17_MUX			68
#define HI3556_MMC0_MUX			69
#define HI3556_MMC1_MUX			70
#define HI3556_MMC2_MUX			71

/*fixed factor clocks*/
#define HI3556_SYSAPB_CLK		97
#define HI3556_MMC0_FAC_CLK		98
#define HI3556_MMC1_FAC_CLK		99
#define HI3556_MMC2_FAC_CLK		100

/* gate clocks */
#define HI3556_FMC_CLK			129
#define HI3556_MMC0_CLK			130
#define HI3556_MMC1_CLK			131
#define HI3556_MMC2_CLK			132
#define HI3556_UART0_CLK		153
#define HI3556_UART1_CLK		154
#define HI3556_UART2_CLK		155
#define HI3556_UART3_CLK		156
#define HI3556_UART4_CLK		157
#define HI3556_SPI0_CLK			160
#define HI3556_SPI1_CLK			161
#define HI3556_SPI2_CLK			162
#define HI3556_SPI3_CLK			163

#define HI3556_USB2_CTRL_UTMI0_REQ      183
#define HI3556_USB2_PHY_PORT0_TREQ      184
#define HI3556_USB2_PHY_REQ             185
#define HI3556_USB2_HRST_REQ            186
#define HI3556_USB2_CLK                 187
#define HI3556_USB3_CLK                 188

#define HI3556_ETH_CLK			192
#define HI3556_ETH_MACIF_CLK		193

/* complex */
#define HI3556_USB_CLK		195
#define HI3556_APLL_CLK		196

#define HI3556_NR_CLKS		256
#define HI3556_NR_RSTS		256
#endif	/* __DTS_HI3556_CLOCK_H */
