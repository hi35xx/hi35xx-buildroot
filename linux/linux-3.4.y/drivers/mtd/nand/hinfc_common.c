/******************************************************************************
*    Copyright (c) 2009-2014 by Hisilicon.
*    All rights reserved.
* ***
*    Create By Hisi.
*
******************************************************************************/

#include "hinfc_common_os.h"
#include "match_table.h"
#include "../hifmc_common.h"
#include "hinfc_common.h"

/*****************************************************************************/
struct nand_flash_dev *(*nand_get_flash_type_func)(struct mtd_info *mtd,
	struct nand_chip *chip, struct nand_flash_dev_ex *flash_dev_ex) = NULL;

int (*nand_oob_resize)(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *nand_dev) = NULL;

/*****************************************************************************/
static struct match_type_str ecc2name[] = {
	{NAND_ECC_0BIT, "none" },
	{NAND_ECC_1BIT_512, "1bit/512" },
	{NAND_ECC_4BIT,     "4bit/512" },
	{NAND_ECC_4BIT_512, "4bit/512" },
	{NAND_ECC_4BYTE,    "4byte/1k" },
	{NAND_ECC_8BIT,     "4bit/512" },
	{NAND_ECC_8BIT_512, "8bit/512" },
	{NAND_ECC_8BYTE, "8byte/1k" },
	{NAND_ECC_13BIT, "13bit/1k" },
	{NAND_ECC_16BIT, "16bit/1k" },
	{NAND_ECC_18BIT, "18bit/1k" },
	{NAND_ECC_24BIT, "24bit/1k" },
	{NAND_ECC_27BIT, "27bit/1k" },
	{NAND_ECC_28BIT, "28bit/1k" },
	{NAND_ECC_32BIT, "32bit/1k" },
	{NAND_ECC_40BIT, "40bit/1k" },
	{NAND_ECC_41BIT, "41bit/1k" },
	{NAND_ECC_48BIT, "48bit/1k" },
	{NAND_ECC_60BIT, "60bit/1k" },
	{NAND_ECC_64BIT, "64bit/1k" },
	{NAND_ECC_72BIT, "72bit/1k" },
	{NAND_ECC_80BIT, "80bit/1k" },
};

const char *nand_ecc_name(int type)
{
	return type2str(ecc2name, ARRAY_SIZE(ecc2name), type, "unknown");
}

char *get_ecctype_str(enum ecc_type ecctype)
{
	static char *ecctype_string[] = {
		"None", "1bit/512Byte", "4bits/512Byte", "8bits/512Byte",
		"24bits/1K", "40bits/1K", "unknown", "unknown"};
	return ecctype_string[(ecctype & 0x07)];
}

/*****************************************************************************/
static struct match_type_str page2name[] = {
	{ NAND_PAGE_512B, "512" },
	{ NAND_PAGE_2K,   "2K" },
	{ NAND_PAGE_4K,   "4K" },
	{ NAND_PAGE_8K,   "8K" },
	{ NAND_PAGE_16K,  "16K" },
	{ NAND_PAGE_32K,  "32K" },
};

const char *nand_page_name(int type)
{
	return type2str(page2name, ARRAY_SIZE(page2name), type, "unknown");
}

char *get_pagesize_str(enum page_type pagetype)
{
	static char *pagesize_str[] = {
		"512", "2K", "4K", "8K", "16K", "unknown",
		"unknown", "unknown"};
	return pagesize_str[(pagetype & 0x07)];
}

/*****************************************************************************/
static struct match_reg_type page2size[] = {
	{ _512B, NAND_PAGE_512B },
	{ _2K, NAND_PAGE_2K },
	{ _4K, NAND_PAGE_4K },
	{ _8K, NAND_PAGE_8K },
	{ _16K, NAND_PAGE_16K },
	{ _32K, NAND_PAGE_32K },
};

unsigned int get_pagesize(enum page_type pagetype)
{
	unsigned int pagesize[] = {
		_512B, _2K, _4K, _8K, _16K, 0, 0, 0};
	return pagesize[(pagetype & 0x07)];
}

int nandpage_size2type(int size)
{
	return reg2type(page2size, ARRAY_SIZE(page2size), size, NAND_PAGE_2K);
}

int nandpage_type2size(int size)
{
	return type2reg(page2size, ARRAY_SIZE(page2size), size, NAND_PAGE_2K);
}

char *nand_dbgfs_options;

static int __init dbgfs_options_setup(char *s)
{
	nand_dbgfs_options = s;
	return 1;
}
__setup("nanddbgfs=", dbgfs_options_setup);

static struct match_t page_table[] = {
	{NAND_PAGE_2K,	PAGE_SIZE_2KB,	"2K"},
	{NAND_PAGE_4K,	PAGE_SIZE_4KB,	"4K"},
	{NAND_PAGE_8K,	PAGE_SIZE_8KB,	"8K"},
	{NAND_PAGE_16K,	PAGE_SIZE_16KB,	"16K"},
};

unsigned char match_page_reg_to_type(unsigned char reg)
{
	return match_reg_to_type(page_table, ARRAY_SIZE(page_table), reg,
			NAND_PAGE_2K);
}

unsigned char match_page_type_to_reg(unsigned char type)
{
	return match_type_to_reg(page_table, ARRAY_SIZE(page_table), type,
			PAGE_SIZE_2KB);
}

const char *match_page_type_to_str(unsigned char type)
{
	return match_type_to_data(page_table, ARRAY_SIZE(page_table), type,
			"unknown");
}

/*****************************************************************************/
static struct match_t ecc_table[] = {
	{NAND_ECC_0BIT,		ECC_TYPE_0BIT,	"none"},
	{NAND_ECC_8BIT,		ECC_TYPE_8BIT,	"4bit/512"},
	{NAND_ECC_16BIT,	ECC_TYPE_16BIT,	"8bit/512"},
	{NAND_ECC_24BIT,	ECC_TYPE_24BIT,	"24bit/1K"},
	{NAND_ECC_28BIT,	ECC_TYPE_28BIT,	"28bit/1K"},
	{NAND_ECC_40BIT,	ECC_TYPE_40BIT,	"40bit/1K"},
	{NAND_ECC_64BIT,	ECC_TYPE_64BIT,	"64bit/1K"},
};

unsigned char match_ecc_reg_to_type(unsigned char reg)
{
	return match_reg_to_type(ecc_table, ARRAY_SIZE(ecc_table), reg,
			NAND_ECC_8BIT);
}

unsigned char match_ecc_type_to_reg(unsigned char type)
{
	return match_type_to_reg(ecc_table, ARRAY_SIZE(ecc_table), type,
			ECC_TYPE_8BIT);
}

const char *match_ecc_type_to_str(unsigned char type)
{
	return match_type_to_data(ecc_table, ARRAY_SIZE(ecc_table), type,
			"unknown");
}

/*****************************************************************************/
static struct match_t page_type_size_table[] = {
	{NAND_PAGE_2K,	_2K,	NULL},
	{NAND_PAGE_4K,	_4K,	NULL},
	{NAND_PAGE_8K,	_8K,	NULL},
	{NAND_PAGE_16K,	_16K,	NULL},
};

unsigned char match_page_size_to_type(unsigned int size)
{
	return match_reg_to_type(page_type_size_table,
			ARRAY_SIZE(page_type_size_table), size, NAND_PAGE_2K);
}

unsigned int match_page_type_to_size(unsigned char type)
{
	return match_type_to_reg(page_type_size_table,
			ARRAY_SIZE(page_type_size_table), type, _2K);
}

