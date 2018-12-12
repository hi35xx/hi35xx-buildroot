/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
 *
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __HIFMC_SPI_IDS_H__
#define __HIFMC_SPI_IDS_H__

/*****************************************************************************/
#define INFINITE			(0xFFFFFFFF)

/*****************************************************************************/
#define SPI_IF_READ_STD			(0x01)
#define SPI_IF_READ_FAST		(0x02)
#define SPI_IF_READ_DUAL		(0x04)
#define SPI_IF_READ_DUAL_ADDR		(0x08)
#define SPI_IF_READ_QUAD		(0x10)
#define SPI_IF_READ_QUAD_ADDR		(0x20)
#define SPI_IF_READ_QUAD_DTR		(0x40)

#define SPI_IF_WRITE_STD		(0x01)
#define SPI_IF_WRITE_DUAL		(0x02)
#define SPI_IF_WRITE_DUAL_ADDR		(0x04)
#define SPI_IF_WRITE_QUAD		(0x08)
#define SPI_IF_WRITE_QUAD_ADDR		(0x10)

#define SPI_IF_ERASE_SECTOR_4K		(0x01)
#define SPI_IF_ERASE_SECTOR_32K		(0x02)
#define SPI_IF_ERASE_SECTOR_64K		(0x04)
#define SPI_IF_ERASE_SECTOR_128K	(0x08)
#define SPI_IF_ERASE_SECTOR_256K	(0x10)

/*****************************************************************************/
#define HIFMC_SPI_NOR_SUPPORT_READ	(SPI_IF_READ_STD \
					| SPI_IF_READ_FAST \
					| SPI_IF_READ_DUAL \
					| SPI_IF_READ_DUAL_ADDR \
					| SPI_IF_READ_QUAD \
					| SPI_IF_READ_QUAD_ADDR\
					| SPI_IF_READ_QUAD_DTR)

#define HIFMC_SPI_NOR_SUPPORT_WRITE	(SPI_IF_WRITE_STD \
					| SPI_IF_WRITE_DUAL \
					| SPI_IF_WRITE_DUAL_ADDR \
					| SPI_IF_WRITE_QUAD \
					| SPI_IF_WRITE_QUAD_ADDR)

#define HIFMC_SPI_NOR_STR_MAX_DUMMY		7
#define HIFMC_SPI_NOR_DTR_MAX_DUMMY		12

/******************************************************************************/
#define HIFMC_SPI_NAND_SUPPORT_READ	(SPI_IF_READ_STD \
					| SPI_IF_READ_FAST \
					| SPI_IF_READ_DUAL \
					| SPI_IF_READ_DUAL_ADDR \
					| SPI_IF_READ_QUAD \
					| SPI_IF_READ_QUAD_ADDR)

#define HIFMC_SPI_NAND_SUPPORT_WRITE	(SPI_IF_WRITE_STD | SPI_IF_WRITE_QUAD)

#define HIFMC_SPI_NAND_SUPPORT_MAX_DUMMY	8

/*****************************************************************************/
#define SPI_CMD_READ_STD		0x03	/* Standard read cache */
#define SPI_CMD_READ_FAST		0x0B	/* Higher speed read cache */
#define SPI_CMD_READ_DUAL		0x3B	/* 2 IO read cache only date */
#define SPI_CMD_READ_DUAL_ADDR		0xBB	/* 2 IO read cache date&addr */
#define SPI_CMD_READ_QUAD		0x6B	/* 4 IO read cache only date */
#define SPI_CMD_READ_QUAD_ADDR		0xEB	/* 4 IO read cache date&addr */
#define SPI_CMD_READ_QUAD_DTR		0xEE	/* 4DTR MODE */
#define SPI_CMD_READ_QUAD_DTR_WINBOND		0xED	/* 4DTR MODE */

#define SPI_CMD_WRITE_STD		0x02	/* Standard page program */
#define SPI_CMD_WRITE_DUAL		0xA2	/* 2 IO program only date */
#define SPI_CMD_WRITE_DUAL_ADDR		0xD2	/* 2 IO program date&addr */
#define SPI_CMD_WRITE_QUAD		0x32	/* 4 IO program only date */
#define SPI_CMD_WRITE_QUAD_ADDR		0x38	/* 4 IO program date&addr */
/* #define SPI_CMD_WRITE_QUAD_ADDR		0x12 */

#define SPI_CMD_SE_4K			0x20	/* 4KB sector Erase */
#define SPI_CMD_SE_32K			0x52	/* 32KB sector Erase */
#define SPI_CMD_SE_64K			0xD8	/* 64KB sector Erase */
#define SPI_CMD_SE_128K			0xD8	/* 128KB sector Erase */
#define SPI_CMD_SE_256K			0xD8	/* 256KB sector Erase */

/*****************************************************************************/
#define SET_READ_STD(_dummy_, _size_, _clk_) \
	static struct spi_op read_std_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_STD, SPI_CMD_READ_STD, _dummy_, _size_, _clk_ }

#define SET_READ_FAST(_dummy_, _size_, _clk_) \
	static struct spi_op read_fast_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_FAST, SPI_CMD_READ_FAST, _dummy_, _size_, _clk_ }

#define SET_READ_DUAL(_dummy_, _size_, _clk_) \
	static struct spi_op read_dual_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_DUAL, SPI_CMD_READ_DUAL, _dummy_, _size_, _clk_ }

#define SET_READ_DUAL_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_op read_dual_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_DUAL_ADDR, SPI_CMD_READ_DUAL_ADDR, _dummy_, _size_, _clk_ }

#define SET_READ_QUAD(_dummy_, _size_, _clk_) \
	static struct spi_op read_quad_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_QUAD, SPI_CMD_READ_QUAD, _dummy_, _size_, _clk_ }

#define SET_READ_QUAD_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_op read_quad_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_QUAD_ADDR, SPI_CMD_READ_QUAD_ADDR, _dummy_, _size_, _clk_ }
#ifdef CONFIG_DTR_MODE_SUPPORT
#define SET_READ_QUAD_DTR(_dummy_, _size_, _clk_) \
	static struct spi_op read_quad_dtr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_QUAD_DTR, SPI_CMD_READ_QUAD_DTR, _dummy_, _size_, _clk_ }
#define SET_READ_QUAD_DTR_WINBOND(_dummy_, _size_, _clk_) \
	static struct spi_op read_quad_dtr_winbond_##_dummy_##_size_##_clk_ = \
	{SPI_IF_READ_QUAD_DTR, SPI_CMD_READ_QUAD_DTR_WINBOND, \
		_dummy_, _size_, _clk_ }
#endif

/*****************************************************************************/
#define SET_WRITE_STD(_dummy_, _size_, _clk_) \
	static struct spi_op write_std_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_STD, SPI_CMD_WRITE_STD, _dummy_, _size_, _clk_ }

#define SET_WRITE_DUAL(_dummy_, _size_, _clk_) \
	static struct spi_op write_dual_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_DUAL, SPI_CMD_WRITE_DUAL, _dummy_, _size_, _clk_ }

#define SET_WRITE_DUAL_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_op write_dual_addr_##_dummy_##_size_##_clk_ = { \
SPI_IF_WRITE_DUAL_ADDR, SPI_CMD_WRITE_DUAL_ADDR, _dummy_, _size_, _clk_ }

#define SET_WRITE_QUAD(_dummy_, _size_, _clk_) \
	static struct spi_op write_quad_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_QUAD, SPI_CMD_WRITE_QUAD, _dummy_, _size_, _clk_ }

#define SET_WRITE_QUAD_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_op write_quad_addr_##_dummy_##_size_##_clk_ = { \
SPI_IF_WRITE_QUAD_ADDR, SPI_CMD_WRITE_QUAD_ADDR, _dummy_, _size_, _clk_ }

/*****************************************************************************/
#define SET_ERASE_SECTOR_4K(_dummy_, _size_, _clk_) \
	static struct spi_op erase_sector_4k_##_dummy_##_size_##_clk_ = { \
	SPI_IF_ERASE_SECTOR_4K, SPI_CMD_SE_4K, _dummy_, _size_, _clk_ }

#define SET_ERASE_SECTOR_32K(_dummy_, _size_, _clk_) \
	static struct spi_op erase_sector_32k_##_dummy_##_size_##_clk_ = { \
	SPI_IF_ERASE_SECTOR_32K, SPI_CMD_SE_32K, _dummy_, _size_, _clk_ }

#define SET_ERASE_SECTOR_64K(_dummy_, _size_, _clk_) \
	static struct spi_op erase_sector_64k_##_dummy_##_size_##_clk_ = { \
	SPI_IF_ERASE_SECTOR_64K, SPI_CMD_SE_64K, _dummy_, _size_, _clk_ }

#define SET_ERASE_SECTOR_128K(_dummy_, _size_, _clk_) \
	static struct spi_op erase_sector_128k_##_dummy_##_size_##_clk_ = { \
	SPI_IF_ERASE_SECTOR_128K, SPI_CMD_SE_128K, _dummy_, _size_, _clk_ }

#define SET_ERASE_SECTOR_256K(_dummy_, _size_, _clk_) \
	static struct spi_op erase_sector_256k_##_dummy_##_size_##_clk_ = { \
	SPI_IF_ERASE_SECTOR_256K, SPI_CMD_SE_256K, _dummy_, _size_, _clk_ }

/*****************************************************************************/
#define READ_STD(_dummy_, _size_, _clk_) read_std_##_dummy_##_size_##_clk_
#define READ_FAST(_dummy_, _size_, _clk_) read_fast_##_dummy_##_size_##_clk_
#define READ_DUAL(_dummy_, _size_, _clk_) read_dual_##_dummy_##_size_##_clk_
#define READ_DUAL_ADDR(_dummy_, _size_, _clk_) \
		read_dual_addr_##_dummy_##_size_##_clk_
#define READ_QUAD(_dummy_, _size_, _clk_) read_quad_##_dummy_##_size_##_clk_
#define READ_QUAD_ADDR(_dummy_, _size_, _clk_) \
		read_quad_addr_##_dummy_##_size_##_clk_
#ifdef CONFIG_DTR_MODE_SUPPORT
#define READ_QUAD_DTR(_dummy_, _size_, _clk_) \
		read_quad_dtr_##_dummy_##_size_##_clk_
#define READ_QUAD_DTR_WINBOND(_dummy_, _size_, _clk_) \
		read_quad_dtr_winbond_##_dummy_##_size_##_clk_
#endif

/*****************************************************************************/
#define WRITE_STD(_dummy_, _size_, _clk_) write_std_##_dummy_##_size_##_clk_
#define WRITE_DUAL(_dummy_, _size_, _clk_) write_dual_##_dummy_##_size_##_clk_
#define WRITE_DUAL_ADDR(_dummy_, _size_, _clk_) \
		write_dual_addr_##_dummy_##_size_##_clk_
#define WRITE_QUAD(_dummy_, _size_, _clk_) write_quad_##_dummy_##_size_##_clk_
#define WRITE_QUAD_ADDR(_dummy_, _size_, _clk_) \
		write_quad_addr_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define ERASE_SECTOR_4K(_dummy_, _size_, _clk_) \
		erase_sector_4k_##_dummy_##_size_##_clk_
#define ERASE_SECTOR_32K(_dummy_, _size_, _clk_) \
		erase_sector_32k_##_dummy_##_size_##_clk_
#define ERASE_SECTOR_64K(_dummy_, _size_, _clk_) \
		erase_sector_64k_##_dummy_##_size_##_clk_
#define ERASE_SECTOR_128K(_dummy_, _size_, _clk_) \
		erase_sector_128k_##_dummy_##_size_##_clk_
#define ERASE_SECTOR_256K(_dummy_, _size_, _clk_) \
		erase_sector_256k_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SPI_CMD_WREN			0x06	/* Write Enable */
#define SPI_CMD_WRDI			0x04	/* Write Disable */

/*****************************************************************************/
#define SPI_CMD_WRSR			0x01	/* Write Status Register */
#define SPI_CMD_WRSR2			0x31	/* Write Status Register-2 */
#define SPI_CMD_WRSR3			0x11	/* Write Status Register-3 */

#define SPI_CMD_RDSR			0x05	/* Read Status Register */
#define SPI_CMD_RDSR2			0x35	/* Read Status Register-2 */
#define SPI_CMD_RDSR3			0x15	/* Read Status Register-3 */

#define SPI_CMD_RDCR			0x35	/* Read Config Register */

#define SPI_CMD_RDID			0x9F	/* Read Identification */

#define SPI_CMD_RD_SFDP			0x5A	/* Read SFDP */
/*****************************************************************************/
#define SPI_CMD_GET_FEATURES		0x0F	/* Get Features */
#define SPI_CMD_SET_FEATURE		0x1F	/* Set Feature */

#define SPI_CMD_PAGE_READ		0x13	/* Page Read to Cache */

#define SPI_CMD_RESET			0xff	/* Reset the device */

/*****************************************************************************/
#define SPI_CMD_EN4B	0xB7 /* enter 4 bytes mode and set 4 byte bit as '1' */
#define SPI_CMD_EX4B	0xE9	/* exit 4 bytes mode and clear 4 byte bit */

/*****************************************************************************/
#define MAX_SPI_OP			8

/*****************************************************************************/
/* SPI general operation parameter */
struct spi_op {
	unsigned char iftype;
	unsigned char cmd;
	unsigned char dummy;
	unsigned int size;
	unsigned int clock;
};

struct spi_drv;

/* SPI interface all operation */
struct hifmc_spi {
	char *name;
	unsigned int chipselect;
	unsigned long long chipsize;
	unsigned int erasesize;
#define SPI_NOR_3BYTE_ADDR_LEN	3	/* address len 3Bytes */
#define SPI_NOR_4BYTE_ADDR_LEN	4	/* address len 4Bytes for 32MB */
	unsigned int addrcycle;

	struct spi_op read[1];
	struct spi_op write[1];
	struct spi_op erase[MAX_SPI_OP];

	void *host;

	struct spi_drv *driver;
#ifdef CONFIG_DTR_MODE_SUPPORT
	unsigned int dtr_mode_support;
	/* @dtr_cookie: Some device must set some registers when wants to
	 * work on DTR mode, so this cookie tells us to set s.th */
	unsigned int dtr_cookie;
	#define DTR_MODE_SET_NONE 0x0	/* Need not set anything */
	#define DTR_MODE_SET_ODS  0x1	/* Need to set output driver strength */
#endif
};

/* SPI interface special operation function hook */
struct spi_drv {
	int (*wait_ready)(struct hifmc_spi *spi);
	int (*write_enable)(struct hifmc_spi *spi);
	int (*qe_enable)(struct hifmc_spi *spi);
	int (*bus_prepare)(struct hifmc_spi *spi, int op);
	int (*entry_4addr)(struct hifmc_spi *spi, int en);
#ifdef CONFIG_DTR_MODE_SUPPORT
	int (*dtr_set_device)(struct hifmc_spi *spi, int dtr_en);
#endif
};

struct spi_nand_info {
	char *name;
	unsigned char id[MAX_SPI_NAND_ID_LEN];
	unsigned char id_len;
	unsigned long long chipsize;
	unsigned int erasesize;
	unsigned int pagesize;
	unsigned int oobsize;
#define BBP_LAST_PAGE		0x01
#define BBP_FIRST_PAGE		0x02
	unsigned int badblock_pos;
	struct spi_op *read[MAX_SPI_OP];
	struct spi_op *write[MAX_SPI_OP];
	struct spi_op *erase[MAX_SPI_OP];
	struct spi_drv *driver;
};

struct spi_nor_info {
	char *name;
	unsigned char id[MAX_SPI_NOR_ID_LEN];
	unsigned int id_len;
	unsigned long chipsize;
	unsigned int erasesize;
	unsigned int addrcycle;
	struct spi_op *read[MAX_SPI_OP];
	struct spi_op *write[MAX_SPI_OP];
	struct spi_op *erase[MAX_SPI_OP];
	struct spi_drv *driver;
};

/****************************************************************************/
/* REG_SYSSTAT 0: 3 Bytes address boot mode; 1: 4Bytes address boot mode */
#define GET_FMC_BOOT_MODE ({ \
	unsigned int regval, boot_mode = 0; \
	regval = readl(SYS_CTRL_REG_BASE + REG_SYSSTAT); \
	boot_mode = GET_SPI_NOR_ADDR_MODE(regval); \
	boot_mode; })

/*****************************************************************************/
extern u_char spi_nand_feature_op(struct hifmc_spi *spi, u_char op, u_char addr,
		u_char val);

extern void hifmc_set_fmc_system_clock(struct spi_op *op, int clk_en);

extern void hifmc_get_fmc_best_2x_clock(unsigned int *clock);
#ifdef CONFIG_DTR_MODE_SUPPORT
extern void hifmc_get_fmc_best_4x_clock(unsigned int *clock);
#endif
/*****************************************************************************/

#endif /* End of __HIFMC_SPI_IDS_H__ */

