/*
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

#ifndef __DTS_HI3536C_CLOCK_H
#define __DTS_HI3536C_CLOCK_H

/* clk in hi3536C CRG */
/* fixed rate clocks */
#define HI3536C_FIXED_3M		1
#define HI3536C_FIXED_6M		2
#define HI3536C_FIXED_12M		3
#define HI3536C_FIXED_24M		4
#define HI3536C_FIXED_83P3M		5
#define HI3536C_FIXED_100M		6
#define HI3536C_FIXED_125M		7
#define HI3536C_FIXED_148P5M	8
#define HI3536C_FIXED_150M		9
#define HI3536C_FIXED_200M		10
#define HI3536C_FIXED_250M		11
#define HI3536C_FIXED_300M		12
#define HI3536C_FIXED_324M		13
#define HI3536C_FIXED_342M		14
#define HI3536C_FIXED_375M		15
#define HI3536C_FIXED_400M		16
#define HI3536C_FIXED_448M		17
#define HI3536C_FIXED_500M		18
#define HI3536C_FIXED_540M		19
#define HI3536C_FIXED_600M		20
#define HI3536C_FIXED_750M		21
#define HI3536C_FIXED_1500M		22

/* mux clocks */
#define HI3536C_SYSAXI_CLK		23
#define HI3536C_FMC_MUX			24
#define HI3536C_UART_MUX		25

/* gate clocks */
#define HI3536C_UART0_CLK		26
#define HI3536C_UART1_CLK		27
#define HI3536C_UART2_CLK		28
#define HI3536C_SPI0_CLK		29
#define HI3536C_FMC_CLK			30
#define HI3536C_ETH_CLK			31
#define HI3536C_ETH_MACIF_CLK	32
#define HI3536C_USB2_BUS_CLK	33
#define HI3536C_USB2_CLK		34
#define HI3536C_SATA_CLK		35
#define HI3536C_ETH_PUB_CLK		36
#define HI3536C_ETH_PHY_CLK		37
#define HI3536C_ETH1_PHY_CLK		38
#define HI3536C_ETH1_CLK		39
#define HI3536C_ETH_MACIF1_CLK		40

#define HI3536C_CRG_NR_CLKS		50
#define HI3536C_CRG_NR_RSTS		0x200

/* clock in system control */
/* mux clocks */
#define HI3536C_TIME0_0_CLK		1
#define HI3536C_TIME0_1_CLK		2
#define HI3536C_TIME1_2_CLK		3
#define HI3536C_TIME1_3_CLK		4
#define HI3536C_TIME2_4_CLK		5
#define HI3536C_TIME2_5_CLK		6
#define HI3536C_TIME3_6_CLK		7
#define HI3536C_TIME3_7_CLK		8

#define HI3536C_SYS_NR_CLKS		10
#define HI3536C_SYS_NR_RSTS		0x10
#endif	/* __DTS_HI3536C_CLOCK_H */
