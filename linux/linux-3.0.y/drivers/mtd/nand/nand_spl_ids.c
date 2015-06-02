/******************************************************************************
*    Copyright (c) 2009-2010 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
*
******************************************************************************/

#include <linux/mtd/nand.h>
#include <asm/setup.h>
#include "nand_spl_ids.h"

/*****************************************************************************
 * this is nand id probe function.
******************************************************************************/

#define DRV_VERSION       "1.35"
/*****************************************************************************/

static struct nand_flash_dev *hynix_probe_v02(unsigned char id[8])
{
	static struct nand_flash_dev type[2];

	int pagesizes[]   = {_2K, _4K, _8K, 0};
	int oobsizes[]    = {128, 224, 448, 0, 0, 0, 0, 0};
	int blocksizes[]  = {_128K, _256K, _512K, _768K, _1M, _2M, 0, 0};

	int blocktype = (((id[3] >> 5) & 0x04) | ((id[3] >> 4) & 0x03));
	int oobtype   = (((id[3] >> 2) & 0x03) | ((id[3] >> 4) & 0x04));

	type->options   = 0;
	type->pagesize  = pagesizes[(id[3] & 0x03)];
	type->erasesize = blocksizes[blocktype];
	*(unsigned long *)&type[1] = oobsizes[oobtype];

	return type;
}
/*****************************************************************************/

static struct nand_flash_dev *samsung_probe_v02(unsigned char id[8])
{
	static struct nand_flash_dev type[2];

	int pagesizes[]   = {_2K, _4K, _8K, 0};
	int oobsizes[]    = {0, 128, 218, 400, 436, 0, 0, 0};
	int blocksizes[]  = {_128K, _256K, _512K, _1M, 0, 0, 0, 0};

	int blocktype = (((id[3] >> 5) & 0x04) | ((id[3] >> 4) & 0x03));
	int oobtype   = (((id[3] >> 4) & 0x04) | ((id[3] >> 2) & 0x03));

	type->options   = 0;
	type->pagesize  = pagesizes[(id[3] & 0x03)];
	type->erasesize = blocksizes[blocktype];
	*(unsigned long *)&type[1] = oobsizes[oobtype];

	return type;
}
/*****************************************************************************/

static struct nand_flash_special_dev nand_flash_special_dev[] = {
	/*
	 * Micron
	 * 1st   2nd   3rd   4th   5th   6th   7th   8th   len  chipsize   device            pagesize erasesize oobsize options */
	{ {0x2C, 0x38, 0x00, 0x26, 0x85, 0x00, 0x00, 0x00}, 8,  _1G, NULL, "MT29F8G08ABxBAx",  _4K,  _512K,     224,    0 },
	{ {0x2C, 0x48, 0x04, 0x46, 0x85, 0x00, 0x00, 0x00}, 8,  _2G, NULL, "MT29F16G08CBABx",  _4K,  _1M,       224,    0 },
	{ {0x2C, 0x48, 0x04, 0x4A, 0xA5, 0x00, 0x00, 0x00}, 8,  _2G, NULL, "MT29F16G08CBACA",  _4K,  _1M,       224,    0 },
	{ {0x2C, 0x68, 0x04, 0x4A, 0xA9, 0x00, 0x00, 0x00}, 8,  _4G, NULL, "MT29F32G08CBACA",  _4K,  _1M,       224,    0 },
	{ {0x2C, 0x88, 0x04, 0x4B, 0xA9, 0x00, 0x00, 0x00}, 8,  _8G, NULL, "MT29F64G08CBAAA",  _8K,  _2M,       448,    0 },
	{ {0x2C, 0xA8, 0x05, 0xCB, 0xA9, 0x00, 0x00, 0x00}, 8, _16G, NULL, "MT29F128G08CXXXX", _8K,  _2M,       448,    0 },

	/*
	 * Toshiba
	 * 1st   2nd   3rd   4th   5th   6th                                 pagesize erasesize oobsize options */
	{ {0x98, 0xD5, 0x94, 0x32, 0x76, 0x55, }, 6, _4G, NULL, "TC58NVG4D2F", _8K,     _1M,      448,     0 },
	{ {0x98, 0xD7, 0x94, 0x32, 0x76, 0x55, }, 6, _8G, NULL, "TC58NVG6D2F", _8K,     _1M,      448,     0 },

	/*
	 * Samsung
	 *     K9xxG08UxD: K9LBG08U0D / K9HCG08U1D / K9XDG08U5D
	 * 1st   2nd   3rd   4th   5th   6th     len                                    */
	{ {0xEC, 0xD7, 0xD5, 0x29, 0x38, 0x41, }, 6, _4G, samsung_probe_v02, "K9xxG08UxD", 0, 0, 0, 0},
	{ {0xEC, 0xD5, 0x84, 0x72, 0x50, 0x42, }, 6, _2G, samsung_probe_v02, "K9GAG08U0E", 0, 0, 0, 0},
	{ {0xEC, 0xD7, 0xC5, 0x72, 0x54, 0x42, }, 6, _4G, samsung_probe_v02, "K9LBG08U0E", 0, 0, 0, 0},
	{ {0xEC, 0xD3, 0x84, 0x72, 0x50, 0x42, }, 6, _1G, samsung_probe_v02, "K9G8G08U0C", 0, 0, 0, 0},
	/* 1st   2nd   3rd   4th   5th     len  chipsize    device        pagesize erasesize oobsize options */
	{ {0xEC, 0xD7, 0x55, 0xB6, 0x78},  5,   _4G,  NULL, "K9xxG08UxM", _4K,     _512K,    128,    0},

	/*
	 * Hynix
	 * 1st   2nd   3rd   4th   5th   6th     len                                   */
	{ {0xAD, 0xD5, 0x94, 0x25, 0x44, 0x41, }, 6, _2G, hynix_probe_v02, "H27UAG8T2A", 0, 0, 0, 0},
	{ {0xAD, 0xD5, 0x94, 0x9A, 0x74, 0x42, }, 6, _2G, hynix_probe_v02, "H27UAG8T2B", 0, 0, 0, 0},
	{ {0xAD, 0xD7, 0x94, 0x9A, 0x74, 0x42, }, 6, _4G, hynix_probe_v02, "H27UBG8T2A", 0, 0, 0, 0},
	/* 1st   2nd   3rd   4th     len chipsize   device        pagesize erasesize oobsize options */

	/* boot tag */
	{{0}, 0, 0, 0, 0, 0, 0, 0, 0},

	{{0}, 0, 0, 0, 0, 0, 0, 0, 0},
};

#define NUM_OF_SPECIAL_DEVICE  \
	(sizeof(nand_flash_special_dev)/sizeof(struct nand_flash_special_dev))

/*****************************************************************************/

struct nand_flash_dev *nand_get_special_flash_type(unsigned char id[8])
{
	struct nand_flash_dev *flash_type;
	struct nand_flash_special_dev *spl_dev;

	printk(KERN_NOTICE "Nand ID: 0x%02X 0x%02X 0x%02X 0x%02X"
		" 0x%02X 0x%02X 0x%02X 0x%02X\n",
		id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);

	for (spl_dev = nand_flash_special_dev; spl_dev->length; spl_dev++) {
		if (memcmp(id, spl_dev->id, spl_dev->length))
			continue;

		if (spl_dev->probe)	{
			flash_type = spl_dev->probe(id);
		} else {
			static struct nand_flash_dev type[2];

			type->options   = spl_dev->options;
			type->pagesize  = spl_dev->pagesize;
			type->erasesize = spl_dev->erasesize;
			*(unsigned long *)&type[1] = spl_dev->oobsize;

			flash_type = type;
		}
		flash_type->id = id[1];
		flash_type->chipsize = (unsigned long)(spl_dev->chipsize >> 20);
		flash_type->name     = spl_dev->name;
		return flash_type;
	}

	return NULL;
}
/*****************************************************************************/

struct nand_tag {
	unsigned char id[8];
	int length;
	unsigned long long chipsize;
	unsigned long pagesize;
	unsigned long erasesize;
	unsigned long oobsize;
	unsigned long options;
	char name[16];
};
/*****************************************************************************/

static char *ToSizeString(unsigned long long size)
{
	int ix;
	static char buffer[20];
	char *fmt[] = {"%uByte", "%uK", "%uM", "%uG", "%uT", "%uT"};

	for (ix = 0; (ix < 5) && (size >= 1024); ix++)
		size = (size >> 10);

	sprintf(buffer, fmt[ix], size);
	return buffer;
}
/*****************************************************************************/

static int __init parse_nand_id(const struct tag *tag)
{
	static char chipname[16];
	struct nand_tag *nandtag;
	struct nand_flash_special_dev *spldev
		= nand_flash_special_dev + NUM_OF_SPECIAL_DEVICE - 2;

	if (tag->hdr.size !=
		((sizeof(struct tag_header) + sizeof(struct nand_tag)) >> 2)) {
		printk(KERN_NOTICE "%s(%d): tag->hdr.size(%d) too small.\n",
			__func__, __LINE__, tag->hdr.size);
		return 0;
	}

	nandtag = (struct nand_tag *)&tag->u;

	strncpy(chipname, nandtag->name, 16);
	chipname[sizeof(chipname)-1] = '\0';

	memcpy(spldev->id, nandtag->id, 8);
	spldev->length = nandtag->length;
	spldev->chipsize = nandtag->chipsize;
	spldev->probe = NULL;
	spldev->name  = chipname;

	spldev->pagesize = nandtag->pagesize;
	spldev->erasesize = nandtag->erasesize;
	spldev->oobsize = nandtag->oobsize;
	spldev->options = nandtag->options;

	printk(KERN_NOTICE "NAND TAG: hdr.tag: 0x%08X, hdr.size: %d\n",
		tag->hdr.tag, tag->hdr.size);
	printk(KERN_NOTICE "(%dByte): 0x%02X 0x%02X 0x%02X 0x%02X"
		" 0x%02X 0x%02X 0x%02X 0x%02X\n",
		nandtag->length,
		nandtag->id[0], nandtag->id[1], nandtag->id[2], nandtag->id[3],
		nandtag->id[4], nandtag->id[5], nandtag->id[6], nandtag->id[7]);
	printk(KERN_NOTICE "Block:%s ", ToSizeString(nandtag->erasesize));
	printk("Page:%s ", ToSizeString(nandtag->pagesize));
	printk("Chip:%s ", ToSizeString(nandtag->chipsize));
	printk("OOB:%s ", ToSizeString(nandtag->oobsize));
	printk("Opt:0x%08lX ", nandtag->options);
	printk("Name:(%s)", nandtag->name);
	printk("\n");

	return 0;
}

/* turn to ascii is "N_ID" */
__tagtable(0x4E5F4944, parse_nand_id);
/*****************************************************************************/

static int __init nand_spl_ids_init(void)
{
	printk(KERN_NOTICE "Special nand id table Version %s\n", DRV_VERSION);
	nand_base_get_special_flash_type = nand_get_special_flash_type;
	return 0;
}
/*****************************************************************************/

static void __exit nand_spl_ids_exit(void)
{
}
/*****************************************************************************/

module_init(nand_spl_ids_init);
module_exit(nand_spl_ids_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Special NAND id table");


