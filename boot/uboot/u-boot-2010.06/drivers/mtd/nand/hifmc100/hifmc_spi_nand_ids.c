/******************************************************************************
 *	Flash Memory Controller Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <exports.h>
#include <asm/io.h>
#include <linux/mtd/nand.h>
#include <hifmc_common.h>
#include "../../hifmc_spi_ids.h"
#include "hinfc_common.h"
#include "hifmc100.h"

/*****************************************************************************/
SET_READ_STD(1, INFINITE, 24);

SET_READ_FAST(1, INFINITE, 104);
SET_READ_FAST(1, INFINITE, 120);

SET_READ_DUAL(1, INFINITE, 104);
SET_READ_DUAL(1, INFINITE, 120);

SET_READ_DUAL_ADDR(1, INFINITE, 104);
SET_READ_DUAL_ADDR(1, INFINITE, 120);

SET_READ_QUAD(1, INFINITE, 104);
SET_READ_QUAD(1, INFINITE, 120);

SET_READ_QUAD_ADDR(2, INFINITE, 104);
SET_READ_QUAD_ADDR(1, INFINITE, 120);

/*****************************************************************************/
SET_WRITE_STD(0, 256, 24);

SET_WRITE_QUAD(0, 256, 104);
SET_WRITE_QUAD(0, 256, 120);

/*****************************************************************************/
SET_ERASE_SECTOR_128K(0, _128K, 24);

SET_ERASE_SECTOR_256K(0, _256K, 24);

/*****************************************************************************/
#include "hifmc100_spi_general.c"
static struct spi_drv spi_driver_general = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_general_qe_enable,
};

static struct spi_drv spi_driver_no_qe = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_do_not_qe_enable,
};

/*****************************************************************************/
#define SPI_NAND_ID_TAB_VER		"1.4"

/******* SPI Nand ID Table ***************************************************
* Version	Manufacturer	Chip Name	Size		Operation
* 1.0		ESMT		F50L512M41A	64MB		Add 5 chip
*		GD		5F1GQ4UAYIG	128MB
*		GD		5F2GQ4UAYIG	256MB
*		GD		5F4GQ4UAYIG	512MB
*		GD		5F4GQ4UBYIG	512MB
* 1.1		ESMT		F50L1G41A	128MB		Add 2 chip
*		Winbond		W25N01GV	128MB
* 1.2		GD		5F1GQ4UBYIG	128MB		Add 2 chip
*		GD		5F2GQ4UBYIG	256MB
* 1.3		ATO		ATO25D1GA	128MB		Add 1 chip
* 1.4		MXIC		MX35LF1GE4AB	128MB		Add 2 chip
*		MXIC		MX35LF2GE4AB	256MB
******************************************************************************/
struct spi_nand_info hifmc_spi_nand_flash_table[] = {
	/* ESMT F50L512M41A 512Mbit */
	{
		.name      = "F50L512M41A",
		.id        = {0xC8, 0x20},
		.id_len    = 2,
		.chipsize  = _64M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* ESMT F50L1G41A 1Gbit */
	{
		.name      = "F50L1G41A",
		.id        = {0xC8, 0x21},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* GD 5F1GQ4UAYIG 1Gbit */
	{
		.name      = "5F1GQ4UAYIG",
		.id        = {0xc8, 0xf1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* GD 5F1GQ4UBYIG 1Gbit */
	{
		.name      = "5F1GQ4UBYIG",
		.id        = {0xc8, 0xd1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* GD 5F2GQ4UAYIG 2Gbit */
	{
		.name      = "5F2GQ4UAYIG",
		.id        = {0xc8, 0xf2},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* GD 5F2GQ4UBYIG 2Gbit */
	{
		.name      = "5F2GQ4UBYIG",
		.id        = {0xc8, 0xd2},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* GD 5F4GQ4UAYIG 4Gbit */
	{
		.name      = "5F4GQ4UAYIG",
		.id        = {0xc8, 0xf4},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* GD 5F4GQ4UBYIG 4Gbit */
	{
		.name      = "5F4GQ4UBYIG",
		.id        = {0xc8, 0xd4},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 256,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_256K(0, _256K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* Winbond W25N01GV 1Gbit */
	{
		.name      = "W25N01GV",
		.id        = {0xef, 0xaa, 0x21},
		.id_len    = 3,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_DUAL_ADDR(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			&READ_QUAD_ADDR(2, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* ATO ATO25D1GA 1Gbit */
	{
		.name      = "ATO25D1GA",
		.id        = {0x9b, 0x12},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* MXIC MX35LF1GE4AB 1Gbit */
	{
		.name      = "MX35LF1GE4AB",
		.id        = {0xc2, 0x12},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* MXIC MX35LF2GE4AB 2Gbit */
	{
		.name      = "MX35LF2GE4AB",
		.id        = {0xc2, 0x22},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	{	.id_len    = 0,	},
};

/*****************************************************************************/
static void hifmc100_spi_nand_search_rw(struct spi_nand_info *spiinfo,
	struct spi_op *spiop_rw, u_int iftype, u_int max_dummy, int rw_type)
{
	int ix = 0;
	struct spi_op **spiop, **fitspiop;

	for (fitspiop = spiop = (rw_type ? spiinfo->write : spiinfo->read);
		(*spiop) && ix < MAX_SPI_OP; spiop++, ix++) {
		if (((*spiop)->iftype & iftype)
			&& ((*spiop)->dummy <= max_dummy)
			&& (*fitspiop)->iftype < (*spiop)->iftype)
			fitspiop = spiop;
	}
	memcpy(spiop_rw, (*fitspiop), sizeof(struct spi_op));
}

/*****************************************************************************/
static void hifmc100_spi_nand_get_erase(struct spi_nand_info *spiinfo,
		struct spi_op *spiop_erase)
{
	int ix;

	spiop_erase->size = 0;
	for (ix = 0; ix < MAX_SPI_OP; ix++) {
		if (spiinfo->erase[ix] == NULL)
			break;
		if (spiinfo->erasesize == spiinfo->erase[ix]->size) {
			memcpy(&spiop_erase[ix], spiinfo->erase[ix],
					sizeof(struct spi_op));
			break;
		}
	}
}

/*****************************************************************************/
static void hifmc100_map_spi_op(struct hifmc_spi *spi)
{
	unsigned char ix;
	const int iftype_read[] = {
		SPI_IF_READ_STD,	IF_TYPE_STD,
		SPI_IF_READ_FAST,	IF_TYPE_STD,
		SPI_IF_READ_DUAL,	IF_TYPE_DUAL,
		SPI_IF_READ_DUAL_ADDR,	IF_TYPE_DIO,
		SPI_IF_READ_QUAD,	IF_TYPE_QUAD,
		SPI_IF_READ_QUAD_ADDR,	IF_TYPE_QIO,
		0,			0,
	};
	const int iftype_write[] = {
		SPI_IF_WRITE_STD,	IF_TYPE_STD,
		SPI_IF_WRITE_QUAD,	IF_TYPE_QUAD,
		0,			0,
	};
	const char *if_str[] = {"STD", "DUAL", "DIO", "QUAD", "QIO"};
	const char *clk_num[] = {"24", "75", "125", "150", "200"};

	FMC_PR(BT_DBG, "\t||*-Start Get SPI operation iftype & clock\n");

	for (ix = 0; iftype_write[ix]; ix += 2) {
		if (spi->write->iftype == iftype_write[ix]) {
			spi->write->iftype = iftype_write[ix + 1];
			break;
		}
	}
	hifmc100_get_fmc_best_clock(&spi->write->clock);
	FMC_PR(BT_DBG, "\t|||-Get best write iftype: %s clock: %sMHz\n",
		if_str[spi->write->iftype], clk_num[spi->write->clock >> 2]);

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}
	hifmc100_get_fmc_best_clock(&spi->read->clock);
	FMC_PR(BT_DBG, "\t|||-Get best read iftype: %s clock: %sMHz\n",
		if_str[spi->read->iftype], clk_num[spi->read->clock >> 2]);

	hifmc100_get_fmc_best_clock(&spi->erase->clock);
	spi->erase->iftype = IF_TYPE_STD;
	FMC_PR(BT_DBG, "\t|||-Get best erase iftype: %s clock: %sMHz\n",
		if_str[spi->erase->iftype], clk_num[spi->erase->clock >> 2]);

	FMC_PR(BT_DBG, "\t||*-End Get SPI operation iftype & clock\n");
}

/*****************************************************************************/
static void hifmc100_spi_ids_probe(struct hifmc_host *host,
				struct spi_nand_info *spi_dev)
{
	unsigned int reg;
	struct hifmc_spi *spi = host->spi;

	FMC_PR(BT_DBG, "\t|*-Start match SPI operation & chip init\n");

	spi->host = host;
	spi->name = spi_dev->name;
	spi->driver = spi_dev->driver;

	hifmc100_spi_nand_search_rw(spi_dev, spi->read,
			HIFMC_SPI_NAND_SUPPORT_READ,
			HIFMC_SPI_NAND_SUPPORT_MAX_DUMMY, RW_OP_READ);
	FMC_PR(BT_DBG, "\t||-Save spi->read op cmd:%#x\n", spi->read->cmd);

	hifmc100_spi_nand_search_rw(spi_dev, spi->write,
			HIFMC_SPI_NAND_SUPPORT_WRITE,
			HIFMC_SPI_NAND_SUPPORT_MAX_DUMMY, RW_OP_WRITE);
	FMC_PR(BT_DBG, "\t||-Save spi->write op cmd:%#x\n", spi->write->cmd);

	hifmc100_spi_nand_get_erase(spi_dev, spi->erase);
	FMC_PR(BT_DBG, "\t||-Save spi->erase op cmd:%#x\n", spi->erase->cmd);

	hifmc100_map_spi_op(spi);

	spi->driver->qe_enable(spi);

	/* Disable write protection */
	reg = spi_nand_feature_op(spi, GET_OP, PROTECTION_ADDR, 0);
	FMC_PR(BT_DBG, "\t||-Get protection status: %#x\n", reg);
	if (ANY_BP_ENABLE(reg)) {
		reg &= ~ALL_BP_MASK;
		spi_nand_feature_op(spi, SET_OP, PROTECTION_ADDR, reg);
		FMC_PR(BT_DBG, "\t||-Set [%#x]FT %#x\n", PROTECTION_ADDR, reg);

		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, PROTECTION_ADDR, 0);
		FMC_PR(BT_DBG, "\t||-Check BP disable result: %#x\n", reg);
		if (ANY_BP_ENABLE(reg))
			DB_MSG("Error: Write protection disable failed!\n");
	}

	/* Disable chip internal ECC */
	reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
	FMC_PR(BT_DBG, "\t||-Get feature status: %#x\n", reg);
	if (reg & FEATURE_ECC_ENABLE) {
		reg &= ~FEATURE_ECC_ENABLE;
		spi_nand_feature_op(spi, SET_OP, FEATURE_ADDR, reg);
		FMC_PR(BT_DBG, "\t||-Set [%#x]FT: %#x\n", FEATURE_ADDR, reg);

		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
		FMC_PR(BT_DBG, "\t||-Check internal ECC disable result: %#x\n",
				reg);
		if (reg & FEATURE_ECC_ENABLE)
			DB_MSG("Error: Chip internal ECC disable failed!\n");
	}

	hifmc_cs_user[host->cmd_op.cs]++;

	FMC_PR(BT_DBG, "\t|*-End match SPI operation & chip init\n");
}

/*****************************************************************************/
static struct nand_flash_dev *spi_nand_get_flash_info(struct mtd_info *mtd,
		struct nand_chip *chip, struct nand_flash_dev_ex *flash_dev_ex)
{
	unsigned char ix, len = 0;
	char buffer[100];
	unsigned char *byte = flash_dev_ex->ids;
	struct hifmc_host *host = chip->priv;
	struct spi_nand_info *spi_dev = hifmc_spi_nand_flash_table;
	struct nand_flash_dev *flash_type = &flash_dev_ex->flash_dev;

	FMC_PR(BT_DBG, "\t*-Start find SPI Nand flash\n");

	for (ix = 2; ix < MAX_SPI_NAND_ID_LEN; ix++)
		byte[ix] = chip->read_byte(mtd);

	len = sprintf(buffer, "SPI Nand(cs %d) ID: %#x %#x", host->cmd_op.cs,
			byte[0], byte[1]);

	for (; spi_dev->id_len; spi_dev++) {
		if (memcmp(byte, spi_dev->id, spi_dev->id_len))
			continue;

		for (ix = 2; ix < spi_dev->id_len; ix++)
			len += sprintf(buffer + len, " %#x", byte[ix]);
		printf("%s Name:\"%s\"\n", buffer, spi_dev->name);

		FMC_PR(BT_DBG, "\t*-Found SPI Nand: %s\n", spi_dev->name);

		flash_type->name = spi_dev->name;
		flash_type->id = byte[1];
		flash_type->pagesize = spi_dev->pagesize;
		flash_type->chipsize = spi_dev->chipsize >> 20;
		flash_type->erasesize = spi_dev->erasesize;
		flash_type->options = chip->options;
		FMC_PR(BT_DBG, "\t|-Save struct spi_nand_info info\n");

		flash_dev_ex->oobsize = spi_dev->oobsize;

		hifmc100_spi_ids_probe(host, spi_dev);

		FMC_PR(BT_DBG, "\t*-Found SPI nand: %s\n", spi_dev->name);

		return flash_type;
	}

	FMC_PR(BT_DBG, "\t*-Not found SPI nand flash, ID:%s\n", buffer);

	return NULL;
}

/*****************************************************************************/
void hifmc_spi_nand_ids_register(void)
{
#ifndef CONFIG_SYS_NAND_QUIET_TEST
	printf("SPI Nand ID Table Version %s\n", SPI_NAND_ID_TAB_VER);
#endif
	nand_get_spl_flash_type = spi_nand_get_flash_info;
}

