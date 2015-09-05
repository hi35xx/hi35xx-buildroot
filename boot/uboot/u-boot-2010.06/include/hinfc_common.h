/******************************************************************************
 *    NAND Flash Controller V504 Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Czyong.
 *
******************************************************************************/

#ifndef HINFC_COMMONH
#define HINFC_COMMONH
/******************************************************************************/

#define HINFC_VER_300                   (0x300)
#define HINFC_VER_301                   (0x301)
#define HINFC_VER_310                   (0x310)
#define HINFC_VER_504                   (0x504)
#define HINFC_VER_505                   (0x505)
#define HINFC_VER_600                   (0x600)

/*
#define _512B                               (512)
#define _2K                                 (2048)
#define _4K                                 (4096)
#define _8K                                 (8192)
#define _16K                                (16384)
*/

#define ERSTR_HARDWARE  "Hardware configuration error. "
#define ERSTR_DRIVER    "Driver does not support. "

#define ENABLE                    1
#define DISABLE                   0
/*****************************************************************************/
/*
enum ecc_type {
	et_ecc_none    = 0x00,
	et_ecc_1bit    = 0x01,
	et_ecc_4bit    = 0x02,
	et_ecc_4bytes  = 0x02,
	et_ecc_8bytes  = 0x03,
	et_ecc_24bit1k = 0x04,
	et_ecc_40bit1k = 0x05,
};

enum page_type {
	pt_pagesize_512   = 0x00,
	pt_pagesize_2K    = 0x01,
	pt_pagesize_4K    = 0x02,
	pt_pagesize_8K    = 0x03,
	pt_pagesize_16K   = 0x04,
};

struct page_page_ecc_info {
	enum page_type pagetype;
	enum ecc_type  ecctype;
	unsigned int oobsize;
	struct nand_ecclayout *layout;
};

struct hinfc_host;

struct read_retry_t {
	int type;
	int count;
	int (*set_rr_param)(struct hinfc_host *host, int param);
	int (*get_rr_param)(struct hinfc_host *host);
	int (*enable_enhanced_slc)(struct hinfc_host *host, int enable);
};
*/
struct nand_flash_dev_ex {
	struct nand_flash_dev flash_dev;

	char *start_type;
	unsigned char ids[8];
	int oobsize;
	int ecctype;
	int is_randomizer;
#define NAND_RR_NONE                     0x00
#define NAND_RR_HYNIX_BG_BDIE            0x10
#define NAND_RR_HYNIX_BG_CDIE            0x11
#define NAND_RR_HYNIX_CG_ADIE            0x12
#define NAND_RR_MICRON                   0x20
#define NAND_RR_SAMSUNG                  0x30
#define NAND_RR_TOSHIBA_24nm             0x40
#define NAND_RR_MASK                     0xF0
	int read_retry_type;
};

/*char *get_pagesize_str(enum page_type pagetype);

unsigned int get_pagesize(enum page_type pagetype);
*/
extern struct nand_flash_dev *(*nand_get_spl_flash_type)(struct mtd_info *mtd,
	struct nand_chip *chip, struct nand_flash_dev_ex *flash_dev_ex);

extern int (*nand_oob_resize)(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex);

/******************************************************************************/
#endif /* HINFC_COMMONH */

