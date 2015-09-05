/******************************************************************************
*    Copyright (c) 2009-2010 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/


#include <linux/mtd/nand.h>

#include <exports.h>

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

extern struct nand_flash_dev *(*nand_base_get_special_flash_type)
	(struct mtd_info *mtd, struct nand_chip *chip, unsigned char byte[8]);

/*****************************************************************************/
/*                    this is nand probe function.                           */
/*****************************************************************************/

#define DRV_VERSION     "1.35"


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
	 * 1st   2nd   3rd   4th   5th   6th   7th   8th   len
		chipsize   device      pagesize erasesize oobsize options */
	{ {0x2C, 0x38, 0x00, 0x26, 0x85, 0x00, 0x00, 0x00}, 8,
		_1G, NULL, "MT29F8G08ABxBAx",  _4K,  _512K,     224,    0 },
	{ {0x2C, 0x48, 0x04, 0x46, 0x85, 0x00, 0x00, 0x00}, 8,
		_2G, NULL, "MT29F16G08CBABx",  _4K,  _1M,       224,    0 },
	{ {0x2C, 0x48, 0x04, 0x4A, 0xA5, 0x00, 0x00, 0x00}, 8,
		_2G, NULL, "MT29F16G08CBACA",  _4K,  _1M,       224,    0 },
	{ {0x2C, 0x68, 0x04, 0x4A, 0xA9, 0x00, 0x00, 0x00}, 8,
		_4G, NULL, "MT29F32G08CBACA",  _4K,  _1M,       224,    0 },
	{ {0x2C, 0x88, 0x04, 0x4B, 0xA9, 0x00, 0x00, 0x00}, 8,
		_8G, NULL, "MT29F64G08CBAAA",  _8K,  _2M,       448,    0 },
	{ {0x2C, 0xA8, 0x05, 0xCB, 0xA9, 0x00, 0x00, 0x00}, 8,
		_16G, NULL, "MT29F128G08CXXXX", _8K,  _2M,       448,    0 },

	/*
	 * Toshiba
	 * 1st   2nd   3rd   4th   5th   6th
					pagesize erasesize oobsize options */
	{ {0x98, 0xD5, 0x94, 0x32, 0x76, 0x55, }, 6,
		_4G, NULL, "TC58NVG4D2F", _8K,     _1M,      448,     0 },
	{ {0x98, 0xD7, 0x94, 0x32, 0x76, 0x55, }, 6,
		_8G, NULL, "TC58NVG6D2F", _8K,     _1M,      448,     0 },

	/*
	 * Samsung
	 * K9xxG08UxD: K9LBG08U0D / K9HCG08U1D / K9XDG08U5D
		* 1st   2nd   3rd   4th   5th   6th     len */
	{ {0xEC, 0xD7, 0xD5, 0x29, 0x38, 0x41, }, 6,
		_4G, samsung_probe_v02, "K9xxG08UxD", 0, 0, 0, 0},
	{ {0xEC, 0xD5, 0x84, 0x72, 0x50, 0x42, }, 6,
		_2G, samsung_probe_v02, "K9GAG08U0E", 0, 0, 0, 0},
	{ {0xEC, 0xD7, 0xC5, 0x72, 0x54, 0x42, }, 6,
		_4G, samsung_probe_v02, "K9LBG08U0E", 0, 0, 0, 0},
	{ {0xEC, 0xD3, 0x84, 0x72, 0x50, 0x42, }, 6,
		_1G, samsung_probe_v02, "K9G8G08U0C", 0, 0, 0, 0},
	/* 1st   2nd   3rd   4th   5th     len
		chipsize    device        pagesize erasesize oobsize options */
	{ {0xEC, 0xD7, 0x55, 0xB6, 0x78},  5,
		_4G,  NULL, "K9xxG08UxM", _4K,     _512K,    128,    0},

	/*
	 * Hynix
	 * 1st   2nd   3rd   4th   5th   6th     len */
	{ {0xAD, 0xD5, 0x94, 0x25, 0x44, 0x41, }, 6,
		_2G, hynix_probe_v02, "H27UAG8T2A", 0, 0, 0, 0},
	{ {0xAD, 0xD5, 0x94, 0x9A, 0x74, 0x42, }, 6,
		_2G, hynix_probe_v02, "H27UAG8T2B", 0, 0, 0, 0},
	{ {0xAD, 0xD7, 0x94, 0x9A, 0x74, 0x42, }, 6,
		_4G, hynix_probe_v02, "H27UBG8T2A", 0, 0, 0, 0},
	/* 1st   2nd   3rd   4th  len chipsize
	device  pagesize erasesize oobsize options */

	{{0}, 0, 0, 0, 0, 0, 0, 0, 0},
};

/*****************************************************************************/
struct nand_tag nandtag[1] = {{{0}, 0, 0, 0, 0, 0, 0, {0} } };

struct nand_flash_dev *nand_get_special_flash_type(struct mtd_info *mtd,
	struct nand_chip *chip, unsigned char byte[8])
{
	int ix;
	struct nand_flash_dev *flash_type;
	struct nand_flash_special_dev *spl_dev;

	for (ix = 2; ix < 8; ix++)
		byte[ix] = chip->read_byte(mtd);


	printf("Nand ID: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
		byte[0], byte[1], byte[2], byte[3],
		byte[4], byte[5], byte[6], byte[7]);

	for (spl_dev = nand_flash_special_dev; spl_dev->length; spl_dev++) {
		if (memcmp(byte, spl_dev->id, spl_dev->length))
			continue;

		if (spl_dev->probe) {
			flash_type = spl_dev->probe(byte);
		} else {
			static struct nand_flash_dev type[2];

			type->options   = spl_dev->options;
			type->pagesize  = spl_dev->pagesize;
			type->erasesize = spl_dev->erasesize;
			*(unsigned long *)&type[1] = spl_dev->oobsize;

			flash_type = type;
		}

		flash_type->id = byte[1];
		flash_type->chipsize = (unsigned long)(spl_dev->chipsize >> 20);
		flash_type->name     = spl_dev->name;

		memcpy(nandtag->id, byte, 8);
		nandtag->length    = spl_dev->length;
		nandtag->chipsize  = spl_dev->chipsize;
		nandtag->pagesize  = spl_dev->pagesize;
		nandtag->erasesize = spl_dev->erasesize;
		nandtag->oobsize   = spl_dev->oobsize;
		nandtag->options   = flash_type->options;
		strncpy(nandtag->name, flash_type->name, 16);
		nandtag->name[sizeof(nandtag->name)-1] = '\0';

		return flash_type;
	}

	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);
	chip->read_byte(mtd);
	chip->read_byte(mtd);

	return NULL;
}
/*****************************************************************************/

void nand_spl_ids_register(void)
{
	printf("Special Nand id table Version %s\n", DRV_VERSION);
	nand_base_get_special_flash_type = nand_get_special_flash_type;
}
/*****************************************************************************/
