/******************************************************************************
*    NAND Flash Controller V301 Device Driver
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
*
******************************************************************************/
#ifndef NAND_SPL_IDS_H
#define NAND_SPL_IDS_H

/*****************************************************************************/
#define _2K             (2048)
#define _4K             (4096)
#define _8K             (8192)

#define _128K           (0x20000)
#define _256K           (0x40000)
#define _512K           (0x80000)
#define _768K           (_256K + _512K)
#define _1M             (0x100000)
#define _2M             (_1M << 1)

#define _512M           (0x20000000UL)
#define _1G             (0x40000000ULL)
#define _2G             (0x80000000ULL)
#define _4G            (0x100000000ULL)
#define _8G            (0x200000000ULL)
#define _16G           (0x400000000ULL)

/*****************************************************************************/
struct nand_flash_special_dev {
	unsigned char id[8];
	int length;             /* length of id. */
	unsigned long long chipsize;
	struct nand_flash_dev *(*probe)(unsigned char id[8]);
	char *name;

	unsigned long pagesize;
	unsigned long erasesize;
	unsigned long oobsize;
	unsigned long options;
};

/*****************************************************************************/
extern struct nand_flash_dev *(*nand_base_get_special_flash_type)
	(unsigned char id[8]);


/*****************************************************************************/
#endif /* NAND_SPL_IDS_H */
