/******************************************************************************
 *	Flash Memory Controller Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <asm/setup.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/printk.h>
#include <linux/platform_device.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <mach/platform.h>

#include "../../hifmc_common.h"
#include "../hinfc_common.h"
#include "hifmc100.h"

#ifdef CONFIG_ARCH_HI3518EV200
	#include "hifmc100_hi3518ev200.c"
#endif

/*****************************************************************************/
SET_READ_STD(1, INFINITE, 24);

SET_READ_FAST(1, INFINITE, 60);
SET_READ_FAST(1, INFINITE, 75);
SET_READ_FAST(1, INFINITE, 80);
SET_READ_FAST(1, INFINITE, 104);
SET_READ_FAST(1, INFINITE, 108);
SET_READ_FAST(1, INFINITE, 120);

SET_READ_DUAL(1, INFINITE, 60);
SET_READ_DUAL(1, INFINITE, 75);
SET_READ_DUAL(1, INFINITE, 80);
SET_READ_DUAL(1, INFINITE, 104);
SET_READ_DUAL(1, INFINITE, 108);
SET_READ_DUAL(1, INFINITE, 120);

SET_READ_DUAL_ADDR(1, INFINITE, 60);
SET_READ_DUAL_ADDR(1, INFINITE, 75);
SET_READ_DUAL_ADDR(1, INFINITE, 80);
SET_READ_DUAL_ADDR(1, INFINITE, 104);
SET_READ_DUAL_ADDR(1, INFINITE, 108);
SET_READ_DUAL_ADDR(1, INFINITE, 120);

SET_READ_QUAD(1, INFINITE, 60);
SET_READ_QUAD(1, INFINITE, 75);
SET_READ_QUAD(1, INFINITE, 80);
SET_READ_QUAD(1, INFINITE, 104);
SET_READ_QUAD(1, INFINITE, 108);
SET_READ_QUAD(1, INFINITE, 120);

SET_READ_QUAD_ADDR(1, INFINITE, 60);
SET_READ_QUAD_ADDR(1, INFINITE, 75);
SET_READ_QUAD_ADDR(2, INFINITE, 75);
SET_READ_QUAD_ADDR(1, INFINITE, 80);
SET_READ_QUAD_ADDR(2, INFINITE, 80);
SET_READ_QUAD_ADDR(2, INFINITE, 104);
SET_READ_QUAD_ADDR(1, INFINITE, 108);
SET_READ_QUAD_ADDR(1, INFINITE, 120);

/*****************************************************************************/
SET_WRITE_STD(0, 256, 24);
SET_WRITE_STD(0, 256, 75);
SET_WRITE_STD(0, 256, 80);
SET_WRITE_STD(0, 256, 104);

SET_WRITE_QUAD(0, 256, 75);
SET_WRITE_QUAD(0, 256, 80);
SET_WRITE_QUAD(0, 256, 104);
SET_WRITE_QUAD(0, 256, 108);
SET_WRITE_QUAD(0, 256, 120);

/*****************************************************************************/
SET_ERASE_SECTOR_128K(0, _128K, 24);
SET_ERASE_SECTOR_128K(0, _128K, 75);
SET_ERASE_SECTOR_128K(0, _128K, 80);
SET_ERASE_SECTOR_128K(0, _128K, 104);

SET_ERASE_SECTOR_256K(0, _256K, 24);
SET_ERASE_SECTOR_256K(0, _256K, 75);
SET_ERASE_SECTOR_256K(0, _256K, 80);
SET_ERASE_SECTOR_256K(0, _256K, 104);

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
#define SPI_NAND_ID_TAB_VER		"2.2"

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
*		MXIC		MX35LF2GE4AB	256MB		(SOP-16Pin)
* 1.5		Paragon		PN26G01A	128MB		Add 1 chip
* 1.6		All-flash	AFS1GQ4UAC	128MB		Add 1 chip
* 1.7		TOSHIBA		TC58CVG0S3H	128MB		Add 2 chip
*		TOSHIBA		TC58CVG2S0H	512MB
* 1.8		ALL-flash	AFS2GQ4UAD	256MB		Add 2 chip
*		Paragon		PN26G02A	256MB
* 1.9		TOSHIBA		TC58CVG1S3H	256MB		Add 1 chip
* 2.0		HeYangTek	HYF1GQ4UAACAE	128MB		Add 3 chip
*		HeYangTek	HYF2GQ4UAACAE	256MB
*		HeYangTek	HYF4GQ4UAACBE	512MB
* 2.1		Micron		MT29F1G01ABA	128MB		Add 5 chip
		Paragon	1.8V	PN26Q01AWSIUG	128MB
		TOSHIBA 1.8V	TC58CYG0S3H	128MB
		TOSHIBA 1.8V	TC58CYG1S3H	256MB
		TOSHIBA 1.8V	TC58CYG2S0H	512MB
* 2.2		Micron		MT29F2G01ABA	256MB		Add 1 chip
******************************************************************************/
struct spi_nand_info hifmc_spi_nand_flash_table[] = {
	/* Micron MT29F1G01ABA 1GBit */
	{
		.name      = "MT29F1G01ABA",
		.id        = {0x2C, 0x14},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(1, INFINITE, 80),
			&READ_QUAD(1, INFINITE, 80),
			&READ_QUAD_ADDR(2, INFINITE, 80),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 80),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 80),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* Micron MT29F2G01ABA 2GBit */
	{
		.name      = "MT29F2G01ABA",
		.id        = {0x2C, 0x24},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 108),
			&READ_DUAL(1, INFINITE, 108),
			&READ_DUAL_ADDR(1, INFINITE, 108),
			&READ_QUAD(1, INFINITE, 108),
			&READ_QUAD_ADDR(2, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 80),
			&WRITE_QUAD(0, 256, 108),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 80),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

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

	/* Winbond W25N01GW 1Gbit 1.8V */
	{
		.name      = "W25N01GW",
		.id        = {0xef, 0xba, 0x21},
		.id_len    = 3,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 75),
			&READ_DUAL(1, INFINITE, 75),
			&READ_DUAL_ADDR(1, INFINITE, 75),
			&READ_QUAD(1, INFINITE, 75),
			&READ_QUAD_ADDR(2, INFINITE, 75),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 75),
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

	/* MXIC MX35LF2GE4AB 2Gbit SOP-16Pin */
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

	/* Paragon PN26Q01AWSIUG 1Gbit 1.8V */
	{
		.name      = "PN26G01AW",
		.id        = {0xa1, 0xc1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 75),
			&READ_DUAL(1, INFINITE, 75),
			&READ_DUAL_ADDR(1, INFINITE, 75),
			&READ_QUAD(1, INFINITE, 75),
			&READ_QUAD_ADDR(1, INFINITE, 75),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 75),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 75),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* Paragon PN26G01A 1Gbit */
	{
		.name      = "PN26G01A",
		.id        = {0xa1, 0xe1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 108),
			&READ_DUAL(1, INFINITE, 108),
			&READ_DUAL_ADDR(1, INFINITE, 108),
			&READ_QUAD(1, INFINITE, 108),
			&READ_QUAD_ADDR(1, INFINITE, 108),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 108),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* Paragon PN26G02A 2Gbit */
	{
		.name      = "PN26G02A",
		.id        = {0xa1, 0xe2},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 108),
			&READ_DUAL(1, INFINITE, 108),
			&READ_DUAL_ADDR(1, INFINITE, 108),
			&READ_QUAD(1, INFINITE, 108),
			&READ_QUAD_ADDR(1, INFINITE, 108),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 108),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* All-flash AFS1GQ4UAC 1Gbit */
	{
		.name      = "AFS1GQ4UAC",
		.id        = {0xc1, 0x51},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(1, INFINITE, 80),
			&READ_QUAD(1, INFINITE, 80),
			&READ_QUAD_ADDR(1, INFINITE, 80),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* All-flash AFS2GQ4UAD 2Gbit */
	{
		.name      = "AFS2GQ4UAD",
		.id        = {0xc1, 0x52},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(1, INFINITE, 80),
			&READ_QUAD(1, INFINITE, 80),
			&READ_QUAD_ADDR(1, INFINITE, 80),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* TOSHIBA TC58CVG0S3H 1Gbit */
	{
		.name      = "TC58CVG0S3H",
		.id        = {0x98, 0xc2},
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
			&WRITE_STD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 104),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* TOSHIBA TC58CYG0S3H 1.8V 1Gbit */
	{
		.name      = "TC58CYG0S3H",
		.id        = {0x98, 0xb2},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 75),
			&READ_DUAL(1, INFINITE, 75),
			&READ_QUAD(1, INFINITE, 75),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 75),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 75),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* TOSHIBA TC58CVG1S3H 2Gbit */
	{
		.name      = "TC58CVG1S3H",
		.id        = {0x98, 0xcb},
		.id_len    = 2,
		.chipsize  = _256M,
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
			&WRITE_STD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 104),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* TOSHIBA TC58CYG1S3H 1.8V 2Gbit */
	{
		.name      = "TC58CYG1S3H",
		.id        = {0x98, 0xbb},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 75),
			&READ_DUAL(1, INFINITE, 75),
			&READ_QUAD(1, INFINITE, 75),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 75),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 75),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* TOSHIBA TC58CVG2S0H 4Gbit */
	{
		.name      = "TC58CVG2S0H",
		.id        = {0x98, 0xcd},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_256K(0, _256K, 104),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* TOSHIBA TC58CYG2S0H 1.8V 4Gbit */
	{
		.name      = "TC58CYG2S0H",
		.id        = {0x98, 0xbd},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 256,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 75),
			&READ_DUAL(1, INFINITE, 75),
			&READ_QUAD(1, INFINITE, 75),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 75),
			0
		},
		.erase     = {
			&ERASE_SECTOR_256K(0, _256K, 75),
			0
		},
		.driver    = &spi_driver_no_qe,
	},

	/* HeYangTek HYF1GQ4UAACAE 1Gbit */
	{
		.name      = "HYF1GQ4UAACAE",
		.id        = {0xc9, 0x51},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 60),
			&READ_DUAL(1, INFINITE, 60),
			&READ_DUAL_ADDR(1, INFINITE, 60),
			&READ_QUAD(1, INFINITE, 60),
			&READ_QUAD_ADDR(1, INFINITE, 60),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 80),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 80),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* HeYangTek HYF2GQ4UAACAE 2Gbit */
	{
		.name      = "HYF2GQ4UAACAE",
		.id        = {0xc9, 0x52},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 60),
			&READ_DUAL(1, INFINITE, 60),
			&READ_DUAL_ADDR(1, INFINITE, 60),
			&READ_QUAD(1, INFINITE, 60),
			&READ_QUAD_ADDR(1, INFINITE, 60),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 80),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 80),
			0
		},
		.driver    = &spi_driver_general,
	},

	/* HeYangTek HYF4GQ4UAACBE 4Gbit */
	{
		.name      = "HYF4GQ4UAACBE",
		.id        = {0xc9, 0xd4},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 256,
		.badblock_pos = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 60),
			&READ_DUAL(1, INFINITE, 60),
			&READ_DUAL_ADDR(1, INFINITE, 60),
			&READ_QUAD(1, INFINITE, 60),
			&READ_QUAD_ADDR(1, INFINITE, 60),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 80),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		.erase     = {
			&ERASE_SECTOR_256K(0, _256K, 80),
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

	FMC_PR(BT_DBG, "\t||*-Start Get SPI operation iftype & clock\n");

	for (ix = 0; iftype_write[ix]; ix += 2) {
		if (spi->write->iftype == iftype_write[ix]) {
			spi->write->iftype = iftype_write[ix + 1];
			break;
		}
	}
	hifmc_get_nand_best_clock(&spi->write->clock);
	FMC_PR(BT_DBG, "\t|||-Get best write iftype: %s clock type: %d\n",
		if_str[spi->write->iftype],
		GET_FMC_CLK_TYPE(spi->write->clock));

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}
	hifmc_get_nand_best_clock(&spi->read->clock);
	FMC_PR(BT_DBG, "\t|||-Get best read iftype: %s clock type: %d\n",
		if_str[spi->read->iftype],
		GET_FMC_CLK_TYPE(spi->read->clock));

	hifmc_get_nand_best_clock(&spi->erase->clock);
	spi->erase->iftype = IF_TYPE_STD;
	FMC_PR(BT_DBG, "\t|||-Get best erase iftype: %s clock type: %d\n",
		if_str[spi->erase->iftype],
		GET_FMC_CLK_TYPE(spi->erase->clock));

	FMC_PR(BT_DBG, "\t||*-End Get SPI operation iftype & clock\n");
}

/*****************************************************************************/
static void hifmc100_spi_ids_probe(struct mtd_info *mtd,
				struct spi_nand_info *spi_dev)
{
	unsigned int reg;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;
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
	reg = spi_nand_feature_op(spi, GET_OP, PROTECT_ADDR, 0);
	FMC_PR(BT_DBG, "\t||-Get protect status[%#x]: %#x\n", PROTECT_ADDR,
			reg);
	if (ANY_BP_ENABLE(reg)) {
		reg &= ~ALL_BP_MASK;
		spi_nand_feature_op(spi, SET_OP, PROTECT_ADDR, reg);
		FMC_PR(BT_DBG, "\t||-Set [%#x]FT %#x\n", PROTECT_ADDR, reg);

		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, PROTECT_ADDR, 0);
		FMC_PR(BT_DBG, "\t||-Check BP disable result: %#x\n", reg);
		if (ANY_BP_ENABLE(reg))
			DB_MSG("Error: Write protection disable failed!\n");
	}

	/* Disable chip internal ECC */
	reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
	FMC_PR(BT_DBG, "\t||-Get feature status[%#x]: %#x\n", FEATURE_ADDR,
			reg);
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
	unsigned char ix, len = 0, *id = flash_dev_ex->ids;
	char buffer[100];
	struct hifmc_host *host = chip->priv;
	struct spi_nand_info *spi_dev = hifmc_spi_nand_flash_table;
	struct nand_flash_dev *type = &flash_dev_ex->flash_dev;

	FMC_PR(BT_DBG, "\t *-Start find SPI Nand flash\n");

	len = sprintf(buffer, "SPI Nand(cs %d) ID: %#x %#x",
			host->cmd_op.cs, id[0], id[1]);

	for (; spi_dev->id_len; spi_dev++) {
		if (memcmp(id, spi_dev->id, spi_dev->id_len))
			continue;

		for (ix = 2; ix < spi_dev->id_len; ix++)
			len += sprintf(buffer + len, " %#x", id[ix]);
		pr_info("%s\n", buffer);

		FMC_PR(BT_DBG, "\t||-CS(%d) found SPI Nand: %s\n",
				host->cmd_op.cs, spi_dev->name);

		type->name = spi_dev->name;
		type->id = id[1];
		type->pagesize = spi_dev->pagesize;
		type->chipsize = spi_dev->chipsize >> 20;
		type->erasesize = spi_dev->erasesize;

		flash_dev_ex->oobsize = spi_dev->oobsize;
		FMC_PR(BT_DBG, "\t|-Save struct nand_flash_dev info\n");

		chip->chipsize = (uint64_t)spi_dev->chipsize;
		mtd->size = type->chipsize;
		mtd->erasesize = type->erasesize;
		mtd->writesize = type->pagesize;
		mtd->oobsize = spi_dev->oobsize;

		hifmc100_spi_ids_probe(mtd, spi_dev);

		FMC_PR(BT_DBG, "\t*-Found SPI nand: %s\n", spi_dev->name);

		return type;
	}

	FMC_PR(BT_DBG, "\t*-Not found SPI nand flash, ID: %s\n", buffer);

	return NULL;
}

/*****************************************************************************/
void hifmc_spi_nand_ids_register(void)
{
	pr_info("SPI Nand ID Table Version %s\n", SPI_NAND_ID_TAB_VER);
	nand_get_flash_type_func = spi_nand_get_flash_info;
}
#ifdef CONFIG_PM
/*****************************************************************************/
void hifmc100_spi_nand_config(struct hifmc_host *host)
{
	unsigned int reg;
	struct hifmc_spi *spi = host->spi;
	static const char const *str[] = {"STD", "DUAL", "DIO", "QUAD", "QIO"};

	/* judge whether support QUAD read/write or not, set it if yes */
	FMC_PR(PM_DBG, "\t|-SPI read iftype: %s write iftype: %s\n",
		str[spi->read->iftype], str[spi->write->iftype]);
	spi->driver->qe_enable(spi);

	/* Disable write protection */
	reg = spi_nand_feature_op(spi, GET_OP, PROTECT_ADDR, 0);
	FMC_PR(PM_DBG, "\t|-Get protect status[%#x]: %#x\n", PROTECT_ADDR,
			reg);
	if (ANY_BP_ENABLE(reg)) {
		reg &= ~ALL_BP_MASK;
		spi_nand_feature_op(spi, SET_OP, PROTECT_ADDR, reg);
		FMC_PR(PM_DBG, "\t|-Set [%#x]FT %#x\n", PROTECT_ADDR, reg);

		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, PROTECT_ADDR, 0);
		FMC_PR(PM_DBG, "\t|-Check BP disable result: %#x\n", reg);
		if (ANY_BP_ENABLE(reg))
			DB_MSG("Error: Write protection disable failed!\n");
	}

	/* Disable chip internal ECC */
	reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
	FMC_PR(PM_DBG, "\t|-Get feature status[%#x]: %#x\n", FEATURE_ADDR,
			reg);
	if (reg & FEATURE_ECC_ENABLE) {
		reg &= ~FEATURE_ECC_ENABLE;
		spi_nand_feature_op(spi, SET_OP, FEATURE_ADDR, reg);
		FMC_PR(PM_DBG, "\t|-Set [%#x]FT: %#x\n", FEATURE_ADDR, reg);

		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
		FMC_PR(PM_DBG, "\t|-Check internal ECC disable result: %#x\n",
				reg);
		if (reg & FEATURE_ECC_ENABLE)
			DB_MSG("Error: Chip internal ECC disable failed!\n");
	}
}
/*****************************************************************************/
#endif /* CONFIG_PM */

