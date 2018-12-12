/*
 *
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
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

/* config DDRC, PHY, DDRT typte */
#define DDR_DDRC_V510_CONFIG
#define DDR_PHY_T28_CONFIG
#define DDR_DDRT_T28_CONFIG

#define DDR_TRAINING_UART_DISABLE
#define DDR_TRAINING_STAT_DISABLE
/*#define DDR_TRAINING_ADJUST_DISABLE
#define DDR_HW_READ_ADJ_CONFIG
#define DDR_WL_DATAEYE_ADJUST_CONFIG*/
/* #define DDR_TRAINING_CUT_CODE_CONFIG */
/* #define DDR_MPR_TRAINING_CONFIG */
/* Just for DDR4 */
#define DDR_VREF_TRAINING_CONFIG
#define DDR_VREF_WITHOUT_BDL_CONFIG

/* [CUSTOM] the number of PHY */
#define DDR_PHY_NUM 1

/* config DDRC, PHY, DDRT base address */
/* [CUSTOM] DDR DMC0 base register */
#define DDR_REG_BASE_DMC0		0x12118000
/* [CUSTOM] DDR DMC1 base register */
#define DDR_REG_BASE_DMC1		(DDR_REG_BASE_DMC0)
/* [CUSTOM] DDR PHY0 base register */
#define DDR_REG_BASE_PHY0		0x1211c000
/* [CUSTOM] DDR PHY1 base register */
#define DDR_REG_BASE_PHY1		(DDR_REG_BASE_PHY0)
/* [CUSTOM] DDR DDRT base register */
#define DDR_REG_BASE_DDRT		0x12100000
/* [CUSTOM] DDR DDRT1 base register */
#define DDR_REG_BASE_DDRT1		(DDR_REG_BASE_DDRT)
/* [CUSTOM] DDR training item system control */
#define DDR_REG_BASE_SYSCTRL		0x12050000
#define DDR_REG_BASE_AXI		0x12110000

/* config offset address */
/* Assume sysctrl offset address for DDR training as follows,
if not please define. */
/* [CUSTOM] ddrt reversed data */
#define SYSCTRL_DDRT_PATTERN		0x94
/* [CUSTOM] PHY2 ddrt reversed data */
#define SYSCTRL_DDRT_PATTERN_SEC    (SYSCTRL_DDRT_PATTERN)
/* [CUSTOM] ddr training item */
#define SYSCTRL_DDR_TRAINING_CFG	0x90
/* [CUSTOM] ddr training stat */
#define SYSCTRL_DDR_TRAINING_STAT	0x98

/* config other special */
/* [CUSTOM] DDR training start address. MEM_BASE_DDR*/
#define DDRT_CFG_BASE_ADDR		0x80000000
/* [CUSTOM] SRAM start address.
NOTE: Makefile will parse it, plase define it as Hex. eg: 0xFFFF0C00 */
#define DDR_TRAINING_RUN_STACK		0x04010500
#define DDR_RELATE_REG_DECLARE
#define DDR_TRAINING_SAVE_REG_FUNC(relate_reg, mask) \
	ddr_training_save_reg_custom(relate_reg, mask)

#define DDR_TRAINING_RESTORE_REG_FUNC(relate_reg) \
	ddr_training_restore_reg_custom(relate_reg)

struct tr_custom_reg {
	unsigned int scramb[2];
};
void ddr_training_save_reg_custom(void *relate_reg, unsigned int mask);
void ddr_training_restore_reg_custom(void *relate_reg);
