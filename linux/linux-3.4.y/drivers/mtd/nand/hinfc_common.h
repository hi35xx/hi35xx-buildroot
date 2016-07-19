/******************************************************************************
 *    NAND Flash Controller Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Czyong.
 *
 *****************************************************************************/

#ifndef HINFC_COMMONH
#define HINFC_COMMONH

/*****************************************************************************/
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/string_helpers.h>
#include <asm/setup.h>
#include <linux/module.h>
#include "../hifmc_common.h"

/*****************************************************************************/
#define HINFC_VER_300                   (0x300)
#define HINFC_VER_301                   (0x301)
#define HINFC_VER_310                   (0x310)
#define HINFC_VER_504                   (0x504)
#define HINFC_VER_505                   (0x505)
#define HINFC_VER_600                   (0x600)
#define HINFC_VER_610                   (0x610)
#define HINFC_VER_620                   (0x620)

#define HISNFC_VER_100                  (0x400)

/*****************************************************************************/
#define NAND_PAGE_512B                   0
#define NAND_PAGE_1K                     1
#define NAND_PAGE_2K                     2
#define NAND_PAGE_4K                     3
#define NAND_PAGE_8K                     4
#define NAND_PAGE_16K                    5
#define NAND_PAGE_32K                    6

/*****************************************************************************/
#define NAND_ECC_NONE                    0
#define NAND_ECC_0BIT                    0
#define NAND_ECC_1BIT                    1
#define NAND_ECC_1BIT_512                1
#define NAND_ECC_4BIT                    2
#define NAND_ECC_4BIT_512                2
#define NAND_ECC_4BYTE                   2
#define NAND_ECC_8BIT                    2
#define NAND_ECC_8BIT_512                3
#define NAND_ECC_8BYTE                   3
#define NAND_ECC_13BIT                   4
#define NAND_ECC_16BIT                   5
#define NAND_ECC_18BIT                   6
#define NAND_ECC_24BIT                   7
#define NAND_ECC_27BIT                   8
#define NAND_ECC_28BIT                   9
#define NAND_ECC_32BIT                   10
#define NAND_ECC_40BIT                   11
#define NAND_ECC_41BIT                   12
#define NAND_ECC_42BIT                   13
#define NAND_ECC_48BIT                   14
#define NAND_ECC_60BIT                   15
#define NAND_ECC_64BIT                   16
#define NAND_ECC_72BIT                   17
#define NAND_ECC_80BIT                   18

enum ecc_type {
	et_ecc_none    = 0x00,
	et_ecc_1bit    = 0x01,
	et_ecc_4bit    = 0x02,
	et_ecc_8bit    = 0x03,
	et_ecc_8bytes  = 0x03,
	et_ecc_24bit1k = 0x04,
	et_ecc_40bit1k = 0x05,
	et_ecc_64bit1k = 0x06,
};

enum page_type {
	pt_pagesize_512   = 0x00,
	pt_pagesize_2K    = 0x01,
	pt_pagesize_4K    = 0x02,
	pt_pagesize_8K    = 0x03,
	pt_pagesize_16K   = 0x04,
};

/*****************************************************************************/
struct page_page_ecc_info {
	enum page_type pagetype;
	enum ecc_type  ecctype;
	unsigned int oobsize;
	struct nand_ecclayout *layout;
};

struct nand_config_info {
	unsigned int pagetype;
	unsigned int ecctype;
	unsigned int oobsize;
	struct nand_ecclayout *layout;
};

struct hinfc_host;

struct nand_sync {

#define SET_NAND_SYNC_TYPE(_mfr, _onfi, _version) \
	((((_mfr) & 0xFF) << 16) | (((_version) & 0xFF) << 8) \
	 | ((_onfi) & 0xFF))

#define GET_NAND_SYNC_TYPE_MFR(_type) (((_type) >> 16) & 0xFF)
#define GET_NAND_SYNC_TYPE_VER(_type) (((_type) >> 8) & 0xFF)
#define GET_NAND_SYNC_TYPE_INF(_type) ((_type) & 0xFF)

#define NAND_TYPE_ONFI_23_MICRON    \
	SET_NAND_SYNC_TYPE(NAND_MFR_MICRON, NAND_IS_ONFI, 0x23)
#define NAND_TYPE_ONFI_30_MICRON    \
	SET_NAND_SYNC_TYPE(NAND_MFR_MICRON, NAND_IS_ONFI, 0x30)
#define NAND_TYPE_TOGGLE_TOSHIBA    \
	SET_NAND_SYNC_TYPE(NAND_MFR_TOSHIBA, 0, 0)
#define NAND_TYPE_TOGGLE_SAMSUNG    \
	SET_NAND_SYNC_TYPE(NAND_MFR_SAMSUNG, 0, 0)

#define NAND_TYPE_TOGGLE_10         SET_NAND_SYNC_TYPE(0, 0, 0x10)
#define NAND_TYPE_ONFI_30           SET_NAND_SYNC_TYPE(0, NAND_IS_ONFI, 0x30)
#define NAND_TYPE_ONFI_23           SET_NAND_SYNC_TYPE(0, NAND_IS_ONFI, 0x23)

	int type;
	int (*enable)(struct nand_chip *chip);
	int (*disable)(struct nand_chip *chip);
};

struct read_retry_t {
	int type;
	int count;
	int (*set_rr_param)(struct hinfc_host *host, int param);
	int (*get_rr_param)(struct hinfc_host *host);
	int (*reset_rr_param)(struct hinfc_host *host);
};

struct nand_flash_dev_ex {
	struct nand_flash_dev flash_dev;

	char *start_type;
	unsigned char ids[8];
	int oobsize;
	int ecctype;

	/* (Controller) support ecc/page detect, driver don't need detect */
#define NANDC_HW_AUTO                         0x01
	/* (Controller) support ecc/page detect,
	 * and current ecc/page config finish */
#define NANDC_CONFIG_DONE                     0x02
	/* (Controller) is sync, default is async */
#define NANDC_IS_SYNC_BOOT                    0x04

/* (NAND) need randomizer */
#define NAND_RANDOMIZER                       0x10
/* (NAND) is ONFI interface, combine with sync/async symble */
#define NAND_IS_ONFI                          0x20
/* (NAND) support async and sync, such micron onfi, toshiba toggle 1.0 */
#define NAND_MODE_SYNC_ASYNC                  0x40
/* (NAND) support only sync, such samsung sync. */
#define NAND_MODE_ONLY_SYNC                   0x80

#define NAND_CHIP_MICRON   (NAND_MODE_SYNC_ASYNC | NAND_IS_ONFI)
/* This NAND is async, or sync/async, default is async mode,
 * toggle1.0 interface */
#define NAND_CHIP_TOSHIBA_TOGGLE_10  (NAND_MODE_SYNC_ASYNC)
/* This NAND is only sync mode, toggle2.0 interface */
#define NAND_CHIP_TOSHIBA_TOGGLE_20   (NAND_MODE_ONLY_SYNC)
/* This NAND is only sync mode */
#define NAND_CHIP_SAMSUNG  (NAND_MODE_ONLY_SYNC)

	int flags;
	int is_randomizer;

#define NAND_RR_NONE                   0x00
#define NAND_RR_HYNIX_BG_BDIE          0x10
#define NAND_RR_HYNIX_BG_CDIE          0x11
#define NAND_RR_HYNIX_CG_ADIE          0x12
#define NAND_RR_MICRON                 0x20
#define NAND_RR_SAMSUNG                0x30
#define NAND_RR_TOSHIBA_24nm           0x40
#define NAND_RR_TOSHIBA_19nm           0x41
	int read_retry_type;
};

/*****************************************************************************/

#define IS_NANDC_HW_AUTO(_host)         ((_host)->flags & NANDC_HW_AUTO)
#define IS_NANDC_CONFIG_DONE(_host)     ((_host)->flags & NANDC_CONFIG_DONE)
#define IS_NANDC_SYNC_BOOT(_host)       ((_host)->flags & NANDC_IS_SYNC_BOOT)

#define IS_NAND_RANDOM(_dev)         ((_dev)->flags & NAND_RANDOMIZER)
#define IS_NAND_ONLY_SYNC(_dev)      ((_dev)->flags & NAND_MODE_ONLY_SYNC)
#define IS_NAND_SYNC_ASYNC(_dev)     ((_dev)->flags & NAND_MODE_SYNC_ASYNC)
#define IS_NAND_ONFI(_dev)           ((_dev)->flags & NAND_IS_ONFI)

#define ERSTR_HARDWARE  "Hardware configuration error. "
#define ERSTR_DRIVER    "Driver does not support. "

#define DISABLE                          0
#define ENABLE                           1

/*****************************************************************************/

char *get_ecctype_str(enum ecc_type ecctype);

char *get_pagesize_str(enum page_type pagetype);

unsigned int get_pagesize(enum page_type pagetype);

const char *nand_ecc_name(int type);

const char *nand_page_name(int type);

int nandpage_size2type(int size);

int nandpage_type2size(int size);

extern struct nand_flash_dev *(*nand_get_flash_type_func)(struct mtd_info *mtd,
	struct nand_chip *chip, struct nand_flash_dev_ex *flash_dev_ex);

extern int (*nand_oob_resize)(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *nand_dev);

struct nand_flash_dev *nand_get_flash_type_ex(struct mtd_info *mtd,
	struct nand_chip *chip, struct nand_flash_dev_ex *nand_dev);

void nand_show_info(struct nand_flash_dev_ex *nand_dev,
	struct mtd_info *mtd, char *vendor, char *chipname);

void nand_show_chip(struct nand_chip *chip);

extern char *nand_dbgfs_options;
/*****************************************************************************/
extern unsigned char match_page_reg_to_type(unsigned char reg);

extern unsigned char match_page_type_to_reg(unsigned char type);

extern const char *match_page_type_to_str(unsigned char type);

/*****************************************************************************/
extern unsigned char match_ecc_reg_to_type(unsigned char reg);

extern unsigned char match_ecc_type_to_reg(unsigned char type);

extern const char *match_ecc_type_to_str(unsigned char type);

/*****************************************************************************/
extern unsigned char match_page_size_to_type(unsigned int size);

extern unsigned int match_page_type_to_size(unsigned char type);

/*****************************************************************************/

#endif /* HINFC_COMMONH */

