/******************************************************************************
 *	Flash Memory Controller Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <common.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <hifmc_common.h>
#include <spi_flash.h>
#include "../../hifmc_spi_ids.h"
#include "hifmc100.h"

#ifdef CONFIG_HI3521A
	#include "hifmc100_hi3521a.c"
#endif

#ifdef CONFIG_HI3518EV200
	#include "hifmc100_hi3518ev200.c"
#endif

#ifdef CONFIG_HI3531A
	#include "hifmc100_hi3531a.c"
#endif

/*****************************************************************************/
int start_up_addr_mode;

/*****************************************************************************/
SET_READ_STD(0, INFINITE, 0);
SET_READ_STD(0, INFINITE, 20);
SET_READ_STD(0, INFINITE, 32);
SET_READ_STD(0, INFINITE, 33);
SET_READ_STD(0, INFINITE, 40);
SET_READ_STD(0, INFINITE, 50);
SET_READ_STD(0, INFINITE, 54);
SET_READ_STD(0, INFINITE, 66);

SET_READ_FAST(1, INFINITE, 50);
SET_READ_FAST(1, INFINITE, 64);
SET_READ_FAST(1, INFINITE, 66);
SET_READ_FAST(1, INFINITE, 75);
SET_READ_FAST(1, INFINITE, 80);
SET_READ_FAST(1, INFINITE, 86);
SET_READ_FAST(1, INFINITE, 100);
SET_READ_FAST(1, INFINITE, 104);
SET_READ_FAST(1, INFINITE, 108);

SET_READ_DUAL(1, INFINITE, 64);
SET_READ_DUAL(1, INFINITE, 75);
SET_READ_DUAL(1, INFINITE, 80);
SET_READ_DUAL(1, INFINITE, 104);
SET_READ_DUAL(2, INFINITE, 104);
SET_READ_DUAL(1, INFINITE, 108);

SET_READ_DUAL_ADDR(2, INFINITE, 64);
SET_READ_DUAL_ADDR(0, INFINITE, 80);
SET_READ_DUAL_ADDR(1, INFINITE, 80);
SET_READ_DUAL_ADDR(1, INFINITE, 84);
SET_READ_DUAL_ADDR(2, INFINITE, 108);

#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
SET_READ_QUAD(1, INFINITE, 64);
SET_READ_QUAD(1, INFINITE, 80);
SET_READ_QUAD(1, INFINITE, 108);

SET_READ_QUAD_ADDR(2, INFINITE, 80);
SET_READ_QUAD_ADDR(3, INFINITE, 50);
SET_READ_QUAD_ADDR(3, INFINITE, 75);
SET_READ_QUAD_ADDR(3, INFINITE, 80);
SET_READ_QUAD_ADDR(5, INFINITE, 64);
SET_READ_QUAD_ADDR(5, INFINITE, 108);
#endif
/*****************************************************************************/
SET_WRITE_STD(0, 256, 0);
SET_WRITE_STD(0, 256, 50);
SET_WRITE_STD(0, 256, 64);
SET_WRITE_STD(0, 256, 66);
SET_WRITE_STD(0, 256, 75);
SET_WRITE_STD(0, 256, 80);
SET_WRITE_STD(0, 256, 86);
SET_WRITE_STD(0, 256, 100);
SET_WRITE_STD(0, 256, 104);
SET_WRITE_STD(0, 256, 108);

SET_WRITE_DUAL(0, 256, 64);
SET_WRITE_DUAL(0, 256, 75);
SET_WRITE_DUAL(0, 256, 108);

SET_WRITE_DUAL_ADDR(0, 256, 64);
SET_WRITE_DUAL_ADDR(0, 256, 108);

#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
SET_WRITE_QUAD(0, 256, 64);
SET_WRITE_QUAD(0, 256, 80);
SET_WRITE_QUAD(0, 256, 108);

SET_WRITE_QUAD_ADDR(0, 256, 64);
SET_WRITE_QUAD_ADDR(0, 256, 108);
#endif
/*****************************************************************************/
SET_ERASE_SECTOR_32K(0, _32K, 0);

SET_ERASE_SECTOR_64K(0, _64K, 0);
SET_ERASE_SECTOR_64K(0, _64K, 50);
SET_ERASE_SECTOR_64K(0, _64K, 64);
SET_ERASE_SECTOR_64K(0, _64K, 66);
SET_ERASE_SECTOR_64K(0, _64K, 75);
SET_ERASE_SECTOR_64K(0, _64K, 80);
SET_ERASE_SECTOR_64K(0, _64K, 86);
SET_ERASE_SECTOR_64K(0, _64K, 100);
SET_ERASE_SECTOR_64K(0, _64K, 104);
SET_ERASE_SECTOR_64K(0, _64K, 108);

SET_ERASE_SECTOR_256K(0, _256K, 50);
SET_ERASE_SECTOR_256K(0, _256K, 104);

/*****************************************************************************/
#include "hifmc100_spi_general.c"
static struct spi_drv spi_driver_general = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.entry_4addr = spi_general_entry_4addr,
	.qe_enable = spi_general_qe_enable,
};

static struct spi_drv spi_driver_no_qe = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.entry_4addr = spi_general_entry_4addr,
	.qe_enable = spi_do_not_qe_enable,
};

#include "hifmc100_spi_s25fl256s.c"
static struct spi_drv spi_driver_s25fl256s = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.entry_4addr = spi_s25fl256s_entry_4addr,
	.qe_enable = spi_general_qe_enable,
};

#include "hifmc100_spi_w25q256fv.c"
static struct spi_drv spi_driver_w25q256fv = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.entry_4addr = spi_w25q256fv_entry_4addr,
	.qe_enable = spi_w25q256fv_qe_enable,
};

#include "hifmc100_spi_mx25l25635e.c"
static struct spi_drv spi_driver_mx25l25635e = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.entry_4addr = spi_general_entry_4addr,
	.qe_enable = spi_mx25l25635e_qe_enable,
};

static struct spi_drv spi_driver_f25l64q = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.entry_4addr = spi_general_entry_4addr,
	.qe_enable = spi_mx25l25635e_qe_enable,
};

#include "hifmc100_spi_gd25qxxx.c"
static struct spi_drv spi_driver_gd25qxxx = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.entry_4addr = spi_general_entry_4addr,
	.qe_enable = spi_gd25qxxx_qe_enable,
};

/*****************************************************************************/
static struct spi_nor_info hifmc_spi_nor_info_table[] = {
	/* name		id	id_len	chipsize(Bytes)	erasesize */
	{
		"at25fs010", {0x1f, 0x66, 0x01}, 3, _128K, _32K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_32K(0, _32K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"at25fs040",  {0x1f, 0x66, 0x04}, 3,  _512K,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"at25df041a", {0x1f, 0x44, 0x01}, 3,  _512K,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"at25df641",  {0x1f, 0x48, 0x00}, 3,  _8M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"at26f004",   {0x1f, 0x04, 0x00}, 3,  _512K,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"at26df081a", {0x1f, 0x45, 0x01}, 3,  _1M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"at26df161a", {0x1f, 0x46, 0x01}, 3,  _2M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"at26df321",  {0x1f, 0x47, 0x01}, 3,  _4M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	/* Macronix */
	{
		"mx25l4005a",  {0xc2, 0x20, 0x13}, 3, _512K,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"MX25L8006E",  {0xc2, 0x20, 0x14}, 3, _1M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 86),
			&READ_DUAL(1, INFINITE, 80),
			0
		},
		{
			&WRITE_STD(0, 256, 86),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 86),
			0
		},
		&spi_driver_general,
	},

	{
		"MX25L1606E",  {0xc2, 0x20, 0x15}, 3, _2M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 86),
			&READ_DUAL(1, INFINITE, 80),
			0
		},
		{
			&WRITE_STD(0, 256, 86),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 86),
			0
		},
		&spi_driver_no_qe,
	},

	{
		"mx25l3205d",  {0xc2, 0x20, 0x16}, 3, _4M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"MX25L6406E",  {0xc2, 0x20, 0x17}, 3, _8M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 86),
			&READ_DUAL(1, INFINITE, 80),
			0
		},

		{
			&WRITE_STD(0, 256, 86),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 86),
			0
		},
		&spi_driver_no_qe,
	},

	{
		"MX25L128XX", {0xc2, 0x20, 0x18}, 3, _16M, _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 108),
			0
		},

		{
			&WRITE_STD(0, 256, 108),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 108),
			0
		},
		&spi_driver_no_qe,
	},
	/*
	 The follow chips have the same chipid, but command have some difference

	{"MX25L12836E", {0xc2, 0x20, 0x18}, 3, _16M,   _64K, 3,
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 108), 0},
		{&WRITE_STD(0, 256, 108), 0},
		{&ERASE_SECTOR_64K(0, _64K, 108), 0}},

	{"MX25L12845E", {0xc2, 0x20, 0x18}, 3, _16M,   _64K, 3,
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 108), 0},
		{&WRITE_STD(0, 256, 108), 0},
		{&ERASE_SECTOR_64K(0, _64K, 108), 0}},

	{"MX25L12835F", {0xc2, 0x20, 0x18}, 3, _16M,   _64K, 3,
		{&READ_STD(0, INFINITE, 50), &READ_FAST(1, INFINITE, 108), 0},
		{&WRITE_STD(0, 256, 108), 0},
		{&ERASE_SECTOR_64K(0, _64K, 108), 0}},
	*/

	{
		"MX25L(256/257)35(E/F)",
		{0xc2, 0x20, 0x19}, 3, _32M, _64K, 4,
		{
			&READ_STD(0, INFINITE, 40/*50*/),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(2, INFINITE, 104),
			&READ_DUAL_ADDR(1, INFINITE, 84),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD_ADDR(3, INFINITE, 75),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 75),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 80),
			0
		},
		&spi_driver_mx25l25635e,
	},

	{
		"mx25l1655d",  {0xc2, 0x26, 0x15}, 3, _2M,    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"mx25l12855e", {0xc2, 0x26, 0x18}, 3, _16M,   _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"s25sl004a", {0x01, 0x02, 0x12}, 3, (_64K * 8),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"s25sl008a", {0x01, 0x02, 0x13}, 3, (_64K * 16),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"s25sl016a", {0x01, 0x02, 0x14}, 3, (_64K * 32),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"S25FL064P", {0x01, 0x02, 0x16, 0x4d}, 4, (_64K * 128), _64K, 3,
		{
			&READ_STD(0, INFINITE, 40),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_general,
	},

	{
		"s25sl064a", {0x01, 0x02, 0x16}, 3, (_64K * 128), _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},
	/* Spansion */

	{
		"S25FL032P", {0x01, 0x02, 0x15, 0x4d}, 4, (_64K * 64),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 40),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(0, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
			&READ_QUAD_ADDR(2, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_general,
	},

	{
		"S25FL032A", {0x01, 0x02, 0x15}, 3, (_64K * 64),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 50),
			0
		},

		{
			&WRITE_STD(0, 256, 50),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 50),
			0
		},
		&spi_driver_general,
	},

	{
		"S25FL128P-0",
		{0x01, 0x20, 0x18, 0x03, 0x00}, 5, (_256K * 64),  _256K, 3,
		{
			&READ_STD(0, INFINITE, 40),
			&READ_FAST(1, INFINITE, 104),
			0
		},

		{
			&WRITE_STD(0, 256, 104),
			0
		},

		{
			&ERASE_SECTOR_256K(0, _256K, 104),
			0
		},
		&spi_driver_no_qe,
	},

	{
		"S25FL128P-1",
		{0x01, 0x20, 0x18, 0x03, 0x01}, 5, (_64K * 256),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 40),
			&READ_FAST(1, INFINITE, 104),
			0
		},

		{
			&WRITE_STD(0, 256, 104), 0},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_no_qe,
	},

	{
		"S25FL129P0",
		{0x01, 0x20, 0x18, 0x4d, 0x00}, 5, (_256K * 64),  _256K, 3,
		{
			&READ_STD(0, INFINITE, 40),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(0, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
			&READ_QUAD_ADDR(2, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_256K(0, _256K, 104),
			0
		},
		&spi_driver_general,
	},

	{
		"S25FL129P1",
		{0x01, 0x20, 0x18, 0x4d, 0x01}, 5, (_64K * 256),  _64K,  3,
		{
			&READ_STD(0, INFINITE, 40),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 64),
			&READ_DUAL_ADDR(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
			&READ_QUAD_ADDR(3, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_general,
	},

	{
		"S25FL256S", {0x01, 0x02, 0x19, 0x4d, 0x01}, 5, _32M,  _64K,  4,
		{
			&READ_STD(0, INFINITE, 40),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 64),
			&READ_DUAL_ADDR(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
			&READ_QUAD_ADDR(3, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_s25fl256s,
	},

	/*
	The chip and chip W25Q16B have the same chipid,
	but clock frequency have some difference

	{"S25FL016K", {0xef, 0x40, 0x15}, 3, (_64K * 32),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		{
			&WRITE_STD(0, 256, 104),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		}
	},
	*/

	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{
		"sst25vf040b", {0xbf, 0x25, 0x8d}, 3, (_64K * 8),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"sst25vf080b", {0xbf, 0x25, 0x8e}, 3, (_64K * 16), _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"sst25vf016b", {0xbf, 0x25, 0x41}, 3, (_64K * 32), _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"sst25vf032b", {0xbf, 0x25, 0x4a}, 3, (_64K * 64), _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"sst25wf512",  {0xbf, 0x25, 0x01}, 3, (_64K * 1),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"sst25wf010",  {0xbf, 0x25, 0x02}, 3, (_64K * 2),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"sst25wf020",  {0xbf, 0x25, 0x03}, 3, (_64K * 4),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"sst25wf040",  {0xbf, 0x25, 0x04}, 3, (_64K * 8),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	/* ST Microelectronics -- newer production may have feature updates */
	{
		"m25p05",  {0x20, 0x20, 0x10}, 3, (_32K * 2), _32K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_32K(0, _32K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m25p10",  {0x20, 0x20, 0x11}, 3, (_32K * 4), _32K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_32K(0, _32K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m25p20",  {0x20, 0x20, 0x12}, 3, (_64K * 4),   _64K,  3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m25p40",  {0x20, 0x20, 0x13}, 3, (_64K * 8),   _64K,  3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m25p80",  {0x20, 0x20, 0x14}, 3, (_64K * 16),  _64K,  3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m25p16",  {0x20, 0x20, 0x15}, 3, (_64K * 32),  _64K,  3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"M25P32",  {0x20, 0x20, 0x16, 0x10}, 4, _4M, _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 75),
			0
		},

		{
			&WRITE_STD(0, 256, 75),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 75),
			0
		},
		&spi_driver_general,
	},

	{
		"m25p64",  {0x20, 0x20, 0x17}, 3, (_64K * 128), _64K,  3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"M25P128", {0x20, 0x20, 0x18}, 3, _16M, _256K, 3,
		{
			&READ_STD(0, INFINITE, 20),
			&READ_FAST(1, INFINITE, 50),
			0
		},

		{
			&WRITE_STD(0, 256, 50),
			0
		},

		{
			&ERASE_SECTOR_256K(0, _256K, 50),
			0
		},
		&spi_driver_general,
	},

	{
		"m45pe10", {0x20, 0x40, 0x11}, 3, (_64K * 2),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m45pe80", {0x20, 0x40, 0x14}, 3, (_64K * 16),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m45pe16", {0x20, 0x40, 0x15}, 3, (_64K * 32),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m25pe80", {0x20, 0x80, 0x14}, 3, (_64K * 16), _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"m25pe16", {0x20, 0x80, 0x15}, 3, (_64K * 32), _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"N25Q032", {0x20, 0xba, 0x16}, 3, (_64K * 64), _64K, 3,
		{
			&READ_STD(0, INFINITE, 32/*54*/),
			&READ_FAST(1, INFINITE, 64/*108*/),
			&READ_DUAL(1, INFINITE, 64/*108*/),
			&READ_DUAL_ADDR(2, INFINITE, 64/*108*/),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 64/*108*/),
			&READ_QUAD_ADDR(5, INFINITE, 64/*108*/),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 64/*108*/),
			&WRITE_DUAL(0, 256, 64/*108*/),
			&WRITE_DUAL_ADDR(0, 256, 64/*108*/),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 64/*108*/),
			&WRITE_QUAD_ADDR(0, 256, 64/*108*/),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 64/*108*/),
			0
		},
		&spi_driver_general,
	},

	{
		"N25Q128",   {0x20, 0xba, 0x18}, 3, (_64K * 256), _64K, 3,
		{
			&READ_STD(0, INFINITE, 54),
			&READ_FAST(1, INFINITE, 108),
			&READ_DUAL(1, INFINITE, 108),
			&READ_DUAL_ADDR(2, INFINITE, 108),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 108),
			&READ_QUAD_ADDR(5, INFINITE, 108),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 108),
			&WRITE_DUAL(0, 256, 108),
			&WRITE_DUAL_ADDR(0, 256, 108),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 108),
			&WRITE_QUAD_ADDR(0, 256, 108),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 108),
			0
		},
		&spi_driver_general,
	},

	{
		"M25PX16",  {0x20, 0x71, 0x15}, 3, (_64K * 32),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 75),
			&READ_DUAL(1, INFINITE, 75),
			0
		},

		{
			&WRITE_STD(0, 256, 75),
			&WRITE_DUAL(0, 256, 75),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 75),
			0
		},
		&spi_driver_general,
	},

	{
		"M25PX32", {0x20, 0x71, 0x16}, 3, (_64K * 64),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 75),
			&READ_DUAL(1, INFINITE, 75),
			0
		},

		{
			&WRITE_STD(0, 256, 75),
			&WRITE_DUAL(0, 256, 75),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 75),
			0
		},
		&spi_driver_general,
	},

	{
		"m25px64",  {0x20, 0x71, 0x17}, 3, (_64K * 128), _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{
		"w25x10",  {0xef, 0x30, 0x11}, 3, (_64K * 2),    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"w25x20",  {0xef, 0x30, 0x12}, 3, (_64K * 4),    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

			{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"w25x40",  {0xef, 0x30, 0x13}, 3, (_64K * 8),    _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"w25x80",  {0xef, 0x30, 0x14}, 3, (_64K * 16),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

			{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"w25x16",  {0xef, 0x30, 0x15}, 3, (_64K * 32),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},

		{
			&WRITE_STD(0, 256, 0),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"w25x32",  {0xef, 0x30, 0x16}, 3, (_64K * 64),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 0), 0},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"w25x64",  {0xef, 0x30, 0x17}, 3, (_64K * 128),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 0),
			0
		},
		{
			&WRITE_STD(0, 256, 0),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 0),
			0
		},
		&spi_driver_general,
	},

	{
		"W25Q80BV",  {0xef, 0x40, 0x14}, 3, (_64K * 16),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 80),
			0
		},
		&spi_driver_general,
	},

	{
		"W25Q16(B/C)V/S25FL016K",
		{0xef, 0x40, 0x15}, 3, (_64K * 32), _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 80),
			0
		},
		&spi_driver_general,
	},
	/*
	 The follow chips have the same chipid, but command have some difference
	{
		"W25Q16BV",  {0xef, 0x40, 0x15}, 3, (_64K * 32),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
			&READ_QUAD(1, INFINITE, 80),
			0
		},
		{
			&WRITE_STD(0, 256, 80),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 80),
			0
		}
	},

	{
		"W25Q16CV",  {0xef, 0x40, 0x15}, 3, (_64K * 32),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
			&READ_QUAD(1, INFINITE, 80),
			0
		},
		{
			&WRITE_STD(0, 256, 80),
			&WRITE_QUAD(0, 256, 80),
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 80),
			0
		}
	},

	*/
	{
		"W25Q32BV",  {0xef, 0x40, 0x16}, 3, (_64K * 64),   _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 80),
			0
		},
		&spi_driver_general,
	},

		{
		"W25Q64FV",  {0xef, 0x40, 0x17}, 3, _8M,   _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 80),
			&READ_DUAL(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 80),
			0
		},
		&spi_driver_general,
	},

	{
		"W25Q128(B/F)V", {0xEF, 0x40, 0x18}, 3, _16M, _64K, 3,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, /*70*/80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, /*70*/80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_general,
	},

	{
		"W25Q256FV", {0xEF, 0x40, 0x19}, 3, _32M, _64K, 4,
		{
			&READ_STD(0, INFINITE, 33),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 104),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_w25q256fv,
	},

	/* Eon -- fit clock frequency of RDSR instruction*/
	{
		"EN25F80", {0x1c, 0x31, 0x14}, 3, (_64K * 16),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 66),
			&READ_FAST(1, INFINITE, 66/*100*/),
			0
		},

		{
			&WRITE_STD(0, 256, 66/*100*/),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 66/*100*/),
			0
		},
		&spi_driver_general,
	},

	{
		"EN25F16", {0x1c, 0x31, 0x15}, 3, _2M,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 66),
			&READ_FAST(1, INFINITE, 66/*100*/),
			0
		},

		{
			&WRITE_STD(0, 256, 66/*100*/),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 66/*100*/),
			0
		},
		&spi_driver_general,
	},

	{
		"EN25Q32B", {0x1c, 0x30, 0x16}, 3, (_64K * 64),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 80/*104*/),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(1, INFINITE, 80),
			/*&READ_QUAD(3, INFINITE, 80), */
			0
		},

		{
			&WRITE_STD(0, 256, 80/*104*/),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 80/*104*/),
			0
		},
		&spi_driver_general,
	},

	{
		"EN25Q64", {0x1c, 0x30, 0x17}, 3, (_64K * 128),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 100),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(1, INFINITE, 80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD_ADDR(3, INFINITE, 50),
#endif
			0
		},

		{
			&WRITE_STD(0, 256, 80),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_no_qe,
	},

	{
		"EN25Q128", {0x1c, 0x30, 0x18}, 3, (_64K * 256),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 50),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 80),
			&READ_DUAL_ADDR(1, INFINITE, 80),
			/* &READ_QUAD_ADDR(3, INFINITE, 50), */
			0
		},

		{
			&WRITE_STD(0, 256, 104),
			0
		},

		{
			&ERASE_SECTOR_64K(0, _64K, 104),
			0
		},
		&spi_driver_no_qe,
	},

	/* ESMT */
	{
		"F25L64QA", {0x8C, 0x41, 0x17}, 3, (_64K * 128),  _64K, 3,
		{
			&READ_STD(0, INFINITE, 66),
			&READ_FAST(1, INFINITE, /*66*/100),
			&READ_DUAL(1, INFINITE, /*66*/80),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},
		{
			&WRITE_STD(0, 256, /*66*/100),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, /*66*/100),
			0
		},
		&spi_driver_f25l64q,
	},

	{
		"GD25Q128", {0xC8, 0x40, 0x18}, 3, _16M,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 66),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},
		{
			&WRITE_STD(0, 256, 100),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 100),
			0
		},
		&spi_driver_gd25qxxx,
	},

	{
		"GD25Q64", {0xC8, 0x40, 0x17}, 3, _8M,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 66),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},
		{
			&WRITE_STD(0, 256, 100),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 100),
			0
		},
		&spi_driver_gd25qxxx,
	},
	{
		"GD25Q32", {0xC8, 0x40, 0x16}, 3, _4M,  _64K, 3,
		{
			&READ_STD(0, INFINITE, 66),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&READ_QUAD(1, INFINITE, 80),
#endif
			0
		},
		{
			&WRITE_STD(0, 256, 100),
#ifndef CONFIG_CLOSE_SPI_8PIN_4IO
			&WRITE_QUAD(0, 256, 80),
#endif
			0
		},
		{
			&ERASE_SECTOR_64K(0, _64K, 100),
			0
		},
		&spi_driver_general,
	},

	{0, {0}, 0, 0, 0, 0, {0}, {0}, {0}, NULL},
};

/*****************************************************************************/
static struct spi_nor_info *hifmc_spi_nor_serach_ids(u_char ids[8])
{
	struct spi_nor_info *info = hifmc_spi_nor_info_table;
	struct spi_nor_info *fit_info = NULL;

	for (; info->name; info++) {
		if (memcmp(info->id, ids, info->id_len))
			continue;

		if ((fit_info == NULL) || (fit_info->id_len < info->id_len))
			fit_info = info;
	}
	return fit_info;
}

/*****************************************************************************/
static void hifmc_spi_nor_search_rw(struct spi_nor_info *info,
	struct spi_op *spiop_rw, u_int iftype, u_int max_dummy, int rw_type)
{
	int ix = 0;
	struct spi_op **spiop, **fitspiop;

	for (fitspiop = spiop = (rw_type ? info->write : info->read);
		(*spiop) && ix < MAX_SPI_OP; spiop++, ix++) {
		if (((*spiop)->iftype & iftype)
			&& ((*spiop)->dummy <= max_dummy)
			&& (*fitspiop)->iftype < (*spiop)->iftype)
			fitspiop = spiop;
	}
	memcpy(spiop_rw, (*fitspiop), sizeof(struct spi_op));
}

/*****************************************************************************/
static void hifmc_map_iftype_and_clock(struct hifmc_spi *spi)
{
	int ix;
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
		SPI_IF_WRITE_DUAL,	IF_TYPE_DUAL,
		SPI_IF_WRITE_DUAL_ADDR,	IF_TYPE_DIO,
		SPI_IF_WRITE_QUAD,	IF_TYPE_QUAD,
		SPI_IF_WRITE_QUAD_ADDR,	IF_TYPE_QIO,
		0,			0,
	};

	for (ix = 0; iftype_write[ix]; ix += 2) {
		if (spi->write->iftype == iftype_write[ix]) {
			spi->write->iftype = iftype_write[ix + 1];
			break;
		}
	}
	hifmc_get_fmc_best_clock(&spi->write->clock);

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}
	hifmc_get_fmc_best_clock(&spi->read->clock);

	hifmc_get_fmc_best_clock(&spi->erase->clock);
	spi->erase->iftype = IF_TYPE_STD;
}

/*****************************************************************************/
void hifmc_spi_nor_get_erase(struct spi_nor_info *info,
		struct spi_op *spiop_erase)
{
	int ix;

	spiop_erase->size = 0;
	for (ix = 0; ix < MAX_SPI_OP; ix++) {
		if (info->erase[ix] == NULL)
			break;
		if (info->erasesize == info->erase[ix]->size) {
			memcpy(&spiop_erase[ix], info->erase[ix],
					sizeof(struct spi_op));
			break;
		}
	}
}

/*****************************************************************************/
int hifmc_spi_nor_probe(struct mtd_info_ex *mtd, struct hifmc_spi *spi)
{
	unsigned char cs, ids[MAX_SPI_NOR_ID_LEN], ix, len = 0;
	unsigned int total = 0;
	char buffer[100];
	struct spi_nor_info *spiinfo = NULL;
#if BT_DBG
	const char *str[] = {"STD", "DUAL", "DIO", "QUAD", "QIO"};
#endif

	FMC_PR(BT_DBG, "\t||*-Start probe SPI nor flash\n");

	mtd->numchips = 0;

	for (cs = 0; cs < CONFIG_SPI_NOR_MAX_CHIP_NUM; cs++) {
		if (hifmc_cs_user[cs]) {
			FMC_PR(BT_DBG, "\t|||-CS(%d) is occupied\n", cs);
			continue;
		}

		hifmc100_read_ids(spi, cs, ids);

		/* can't find spi flash device. */
		if (!(ids[0] | ids[1] | ids[2])
			|| ((ids[0] & ids[1] & ids[2]) == 0xFF))
			continue;

		len = sprintf(buffer, "SPI Nor(cs %d) ID: %#x %#x %#x", cs,
				ids[0], ids[1], ids[2]);

		spiinfo = hifmc_spi_nor_serach_ids(ids);
		for (ix = 3; (spiinfo) && (ix < spiinfo->id_len); ix++)
			len += sprintf(buffer + len, " %#x", ids[ix]);

		if (spiinfo) {
			printf("%s\n", buffer);

			FMC_PR(BT_DBG, "\t|||-CS-%d found SPI nor flash: %s\n",
					cs, spiinfo->name);

			spi->name = spiinfo->name;
			spi->chipselect = cs;
			spi->chipsize = spiinfo->chipsize;
			spi->erasesize = spiinfo->erasesize;
			spi->addrcycle = spiinfo->addrcycle;
			spi->driver = spiinfo->driver;

			hifmc_spi_nor_search_rw(spiinfo, spi->read,
				HIFMC_SPI_NOR_SUPPORT_READ,
				HIFMC_SPI_NOR_SUPPORT_MAX_DUMMY, RW_OP_READ);
			hifmc_map_iftype_and_clock(spi);

			spi->driver->qe_enable(spi);

			hifmc_spi_nor_search_rw(spiinfo, spi->read,
				HIFMC_SPI_NOR_SUPPORT_READ,
				HIFMC_SPI_NOR_SUPPORT_MAX_DUMMY, RW_OP_READ);

			hifmc_spi_nor_search_rw(spiinfo, spi->write,
				HIFMC_SPI_NOR_SUPPORT_WRITE,
				HIFMC_SPI_NOR_SUPPORT_MAX_DUMMY, RW_OP_WRITE);

			hifmc_spi_nor_get_erase(spiinfo, spi->erase);
			hifmc_map_iftype_and_clock(spi);

			/* auto check fmc_addr_mode 3 bytes or 4 bytes */
			start_up_addr_mode = GET_FMC_BOOT_MODE;

			if (start_up_addr_mode == SPI_NOR_ADDR_MODE_3_BYTES) {
				FMC_PR(BT_DBG, "\t|||-start up: 3-Byte mode\n");
				spi->driver->entry_4addr(spi, ENABLE);
			} else
				FMC_PR(BT_DBG, "\t|||-start up: 4-Byte mode\n");

			printf("Block:%sB ", ulltostr(spi->erasesize));
			printf("Chip:%sB ", ulltostr(spi->chipsize));
			printf("Name:\"%s\"\n", spi->name);
#if BT_DBG
			DB_MSG("\t|||-Read if: %s, cmd: %#X, clock: %dMHz\n",
					str[spi->read->iftype],
					spi->read->cmd, spi->read->clock);
			DB_MSG("\t|||-Write if: %s, cmd: %#X, clock: %dMHz\n",
					str[spi->write->iftype],
					spi->write->cmd, spi->write->clock);
			DB_MSG("\t|||-Erase if: %s, cmd: %#X, clock: %dMHz\n",
					str[spi->erase[0].iftype],
					spi->erase[0].cmd, spi->erase[0].clock);
#endif

			if (mtd->type == 0) {
				mtd->type = MTD_NORFLASH;
				mtd->chipsize = spi->chipsize;
				mtd->erasesize = spi->erasesize;
				mtd->pagesize = 1;
				mtd->addrcycle = spi->addrcycle;

				if (spiinfo->id_len > sizeof(mtd->ids)) {
					printf("BUG! ID len out of range.\n");
					BUG();
				}

				mtd->id_length = spiinfo->id_len;
				memcpy(mtd->ids, spiinfo->id, spiinfo->id_len);
				strncpy(mtd->name, spi->name,
						sizeof(mtd->name));
				mtd->name[sizeof(mtd->name) - 1] = '\0';
			}

			mtd->numchips++;
			total += spi->chipsize;
			spi++;
			hifmc_cs_user[cs]++;
		}
	}

	FMC_PR(BT_DBG, "\t||*-End probe SPI nor flash, num: %d\n",
			mtd->numchips);

	return mtd->numchips;
}

