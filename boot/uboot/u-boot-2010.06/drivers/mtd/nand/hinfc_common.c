/******************************************************************************
 *    NAND Flash Controller V504 Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Czyong.
 *
******************************************************************************/

#include <common.h>
#include <nand.h>
#include <linux/mtd/nand.h>
#include <hinfc_common.h>

struct nand_flash_dev *(*nand_get_spl_flash_type)(
	struct mtd_info *mtd,
	struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex) = NULL;

int (*nand_oob_resize)(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex) = NULL;
/*
char *get_ecctype_str(enum ecc_type ecctype)
{
	static char *ecctype_string[] = {
		"None", "1bit", "4bits", "8bits", "24bits/1K",
		"40bits/1K", "unknown", "unknown"};
	return ecctype_string[(ecctype & 0x07)];
}

char *get_pagesize_str(enum page_type pagetype)
{
	static char *pagesize_str[] = {
		"512", "2K", "4K", "8K", "16K", "unknown",
		"unknown", "unknown"};
	return pagesize_str[(pagetype & 0x07)];
}

unsigned int get_pagesize(enum page_type pagetype)
{
	unsigned int pagesize[] = {
		_512B, _2K, _4K, _8K, _16K, 0, 0, 0};
	return pagesize[(pagetype & 0x07)];
}
*/
