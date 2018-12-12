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

#ifndef HINFC610_GENH
#define HINFC610_GENH

/******************************************************************************/
#include <linux/mtd/nand.h>
#include <hinfc_common.h>

enum hinfc610_ecc_reg {
	hinfc610_ecc_0bit   = 0x00,
	hinfc610_ecc_8bit   = 0x01,
	hinfc610_ecc_13bit  = 0x02,
	hinfc610_ecc_18bit  = 0x03,
	hinfc610_ecc_24bit  = 0x04,
	hinfc610_ecc_27bit  = 0x05,
	hinfc610_ecc_32bit  = 0x06,
	hinfc610_ecc_41bit  = 0x07,
	hinfc610_ecc_48bit  = 0x08,
	hinfc610_ecc_60bit  = 0x09,
	hinfc610_ecc_72bit  = 0x0a,
	hinfc610_ecc_80bit  = 0x0b,
};

enum hinfc610_page_reg {
	hinfc610_pagesize_2K    = 0x01,
	hinfc610_pagesize_4K    = 0x02,
	hinfc610_pagesize_8K    = 0x03,
	hinfc610_pagesize_16K   = 0x04,
	hinfc610_pagesize_32K   = 0x05,
};

enum hinfc610_page_reg hinfc610_page_type2reg(int type);

int hinfc610_page_reg2type(enum hinfc610_page_reg reg);

enum hinfc610_ecc_reg hinfc610_ecc_type2reg(int type);

int hinfc610_ecc_reg2type(enum hinfc610_ecc_reg reg);

/******************************************************************************/
#endif /* HINFC610_GENH */
