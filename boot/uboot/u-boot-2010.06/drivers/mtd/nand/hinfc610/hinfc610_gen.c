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

#include "hinfc610_gen.h"

/*****************************************************************************/

static struct match_reg_type page_type2reg[] = {
	{
		hinfc610_pagesize_2K, NAND_PAGE_2K,
	}, {
		hinfc610_pagesize_4K, NAND_PAGE_4K,
	}, {
		hinfc610_pagesize_8K, NAND_PAGE_8K,
	}, {
		hinfc610_pagesize_16K, NAND_PAGE_16K,
	}, {
		hinfc610_pagesize_32K, NAND_PAGE_32K,
	}
};

enum hinfc610_page_reg hinfc610_page_type2reg(int type)
{
	return type2reg(page_type2reg, ARRAY_SIZE(page_type2reg), type, 0);
}

int hinfc610_page_reg2type(enum hinfc610_page_reg reg)
{
	return reg2type(page_type2reg, ARRAY_SIZE(page_type2reg), reg, 0);
}
/*****************************************************************************/

static struct match_reg_type ecc_type2reg[] = {
	{
		hinfc610_ecc_0bit, NAND_ECC_0BIT,
	}, {
		hinfc610_ecc_8bit, NAND_ECC_8BIT,
	}, {
		hinfc610_ecc_13bit, NAND_ECC_13BIT,
	}, {
		hinfc610_ecc_18bit, NAND_ECC_18BIT,
	}, {
		hinfc610_ecc_24bit, NAND_ECC_24BIT,
	}, {
		hinfc610_ecc_27bit, NAND_ECC_27BIT,
	}, {
		hinfc610_ecc_32bit, NAND_ECC_32BIT,
	}, {
		hinfc610_ecc_41bit, NAND_ECC_41BIT,
	}, {
		hinfc610_ecc_48bit, NAND_ECC_48BIT,
	}, {
		hinfc610_ecc_60bit, NAND_ECC_60BIT,
	}, {
		hinfc610_ecc_72bit, NAND_ECC_72BIT,
	}, {
		hinfc610_ecc_80bit, NAND_ECC_80BIT,
	}
};

enum hinfc610_ecc_reg hinfc610_ecc_type2reg(int type)
{
	return type2reg(ecc_type2reg, ARRAY_SIZE(ecc_type2reg), type, 0);
}

int hinfc610_ecc_reg2type(enum hinfc610_ecc_reg reg)
{
	return reg2type(ecc_type2reg, ARRAY_SIZE(ecc_type2reg), reg, 0);
}

