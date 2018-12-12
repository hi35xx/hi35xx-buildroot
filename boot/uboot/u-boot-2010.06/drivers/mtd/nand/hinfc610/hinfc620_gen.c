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

#include <match_table.h>
#include <common.h>

#include "hinfc620_gen.h"

/*****************************************************************************/

static struct match_reg_type page_type2reg[] = {
	{
		hinfc620_pagesize_2K, NAND_PAGE_2K,
	}, {
		hinfc620_pagesize_4K, NAND_PAGE_4K,
	}, {
		hinfc620_pagesize_8K, NAND_PAGE_8K,
	}, {
		hinfc620_pagesize_16K, NAND_PAGE_16K,
	}, {
		hinfc620_pagesize_32K, NAND_PAGE_32K,
	}
};

enum hinfc620_page_reg hinfc620_page_type2reg(int type)
{
	return type2reg(page_type2reg, ARRAY_SIZE(page_type2reg), type, 0);
}

int hinfc620_page_reg2type(enum hinfc620_page_reg reg)
{
	return reg2type(page_type2reg, ARRAY_SIZE(page_type2reg), reg, 0);
}
/*****************************************************************************/

static struct match_reg_type ecc_type2reg[] = {
	{
		hinfc620_ecc_0bit, NAND_ECC_0BIT,
	}, {
		hinfc620_ecc_8bit, NAND_ECC_8BIT,
	}, {
		hinfc620_ecc_16bit, NAND_ECC_16BIT,
	}, {
		hinfc620_ecc_24bit, NAND_ECC_24BIT,
	}, {
		hinfc620_ecc_40bit, NAND_ECC_40BIT,
	}, {
		hinfc620_ecc_64bit, NAND_ECC_64BIT,
	}, {
		hinfc620_ecc_28bit, NAND_ECC_28BIT,
	}, {
		hinfc620_ecc_42bit, NAND_ECC_42BIT,
	}
};

enum hinfc620_ecc_reg hinfc620_ecc_type2reg(int type)
{
	return type2reg(ecc_type2reg, ARRAY_SIZE(ecc_type2reg), type, 0);
}

int hinfc620_ecc_reg2type(enum hinfc620_ecc_reg reg)
{
	return reg2type(ecc_type2reg, ARRAY_SIZE(ecc_type2reg), reg, 0);
}

