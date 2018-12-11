/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
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
 
#ifndef __HIFMC100_H__
#define __HIFMC100_H__

/*****************************************************************************/
#include <linux/platform_device.h>
#include <linux/mfd/hisi_fmc.h>

/*****************************************************************************/
#define INFINITE			(0xFFFFFFFF)

/*****************************************************************************/
#define SPI_IF_READ_STD			(0x01)
#define SPI_IF_READ_FAST		(0x02)
#define SPI_IF_READ_DUAL		(0x04)
#define SPI_IF_READ_DUAL_ADDR		(0x08)
#define SPI_IF_READ_QUAD		(0x10)
#define SPI_IF_READ_QUAD_ADDR		(0x20)

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

#define SPI_CMD_WRITE_STD		0x02	/* Standard page program */
#define SPI_CMD_WRITE_DUAL		0xA2	/* 2 IO program only date */
#define SPI_CMD_WRITE_DUAL_ADDR		0xD2	/* 2 IO program date&addr */
#define SPI_CMD_WRITE_QUAD		0x32	/* 4 IO program only date */
#define SPI_CMD_WRITE_QUAD_ADDR		0x12	/* 4 IO program date&addr */

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

#define SPI_CMD_RDID			0x9F	/* Read Identification */

/*****************************************************************************/
#define SPI_CMD_GET_FEATURES		0x0F	/* Get Features */
#define SPI_CMD_SET_FEATURE		0x1F	/* Set Feature */

#define SPI_CMD_PAGE_READ		0x13	/* Page Read to Cache */

#define SPI_CMD_RESET			0xff	/* Reset the device */

/*****************************************************************************/
/* These macroes are for debug only, reg option is slower then dma option */
#undef HIFMC100_SPI_NAND_SUPPORT_REG_READ
/* #define HIFMC100_SPI_NAND_SUPPORT_REG_READ */

#undef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
/* #define HIFMC100_SPI_NAND_SUPPORT_REG_WRITE */

#ifdef CONFIG_HISI_NAND_ECC_STATUS_REPORT
/*****************************************************************************/
#define HIFMC100_ECC_ERR_NUM0_BUF0		0xc0

#define GET_ECC_ERR_NUM(_i, _reg)		(((_reg) >> ((_i) * 8)) & 0xff)
#endif
/*****************************************************************************/
#define REG_CNT_HIGH_BLOCK_NUM_SHIFT		10

#define REG_CNT_BLOCK_NUM_MASK			0x3ff
#define REG_CNT_BLOCK_NUM_SHIFT			22

#define REG_CNT_PAGE_NUM_MASK			0x3f
#define REG_CNT_PAGE_NUM_SHIFT			16

#define REG_CNT_WRAP_MASK			0xf
#define REG_CNT_WRAP_SHIFT			12

#define REG_CNT_ECC_OFFSET_MASK			0xfff
#define REG_CNT_ECC_8BIT_OFFSET			1054
#define REG_CNT_ECC_16BIT_OFFSET		1056
#define REG_CNT_ECC_24BIT_OFFSET		1082

#define ERR_STR_DRIVER "Driver does not support this configure "
#define ERR_STR_CHECK "Please make sure the hardware configuration is correct"

/*****************************************************************************/
#define HIFMC100_ADDR_CYCLE_MASK		0x2

/*****************************************************************************/
#define OP_STYPE_NONE			0x0
#define OP_STYPE_READ			0x01
#define OP_STYPE_WRITE			0x02
#define OP_STYPE_ERASE			0x04
#define CLK_FMC_TO_CRG_MHZ(_clk)	((_clk) * 2000000)

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
	int chipselect;
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
};

/* SPI interface special operation function hook */
struct spi_drv {
	int (*wait_ready)(struct hifmc_spi *spi);
	int (*write_enable)(struct hifmc_spi *spi);
	int (*qe_enable)(struct hifmc_spi *spi);
	int (*bus_prepare)(struct hifmc_spi *spi, int op);
	int (*entry_4addr)(struct hifmc_spi *spi, int en);
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

/*****************************************************************************/
extern u_char spi_nand_feature_op(struct hifmc_spi *spi, u_char op, u_char addr,
		u_char val);

/*****************************************************************************/
struct hifmc_host {
	struct mtd_info *mtd;
	struct nand_chip *chip;
	struct hifmc_spi spi[CONFIG_SPI_NAND_MAX_CHIP_NUM];
	struct hifmc_cmd_op cmd_op;

	void __iomem *iobase;
	void __iomem *regbase;
	struct clk *clk;
	u32 clkrate;

	unsigned int fmc_cfg;
	unsigned int fmc_cfg_ecc0;

	unsigned int offset;

	struct device *dev;
	struct mutex *lock;

	/* This is maybe an un-aligment address, only for malloc or free */
	char *buforg;
	char *buffer;

	unsigned int dma_buffer;
	unsigned int dma_oob;

	unsigned int addr_cycle;
	unsigned int addr_value[2];
	unsigned int cache_addr_value[2];

	unsigned int column;
	unsigned int block_page_mask;

	unsigned int ecctype;
	unsigned int pagesize;
	unsigned int oobsize;

	int add_partition;

	int  need_rr_data;
#define HIFMC100_READ_RETRY_DATA_LEN         128
	char rr_data[HIFMC100_READ_RETRY_DATA_LEN];
	struct read_retry_t *read_retry;

	int version;

	/* BOOTROM read two bytes to detect the bad block flag */
#define HIFMC_BAD_BLOCK_POS		0
	unsigned char *bbm;	/* nand bad block mark */
	unsigned short *epm;	/* nand empty page mark */

	unsigned int uc_er;

	void (*send_cmd_write)(struct hifmc_host *host);
	void (*send_cmd_status)(struct hifmc_host *host);
	void (*send_cmd_read)(struct hifmc_host *host);
	void (*send_cmd_erase)(struct hifmc_host *host);
	void (*send_cmd_readid)(struct hifmc_host *host);
	void (*send_cmd_reset)(struct hifmc_host *host);
#ifdef CONFIG_PM
	int (*suspend)(struct platform_device *pltdev, pm_message_t state);
	int (*resume)(struct platform_device *pltdev);
#endif
};

/*****************************************************************************/
void hifmc100_ecc0_switch(struct hifmc_host *host, unsigned char op);

int hifmc100_spi_nand_init(struct nand_chip *chip);

/*****************************************************************************/
extern void hifmc_spi_nand_ids_register(void);

extern void hifmc_set_nand_system_clock(struct spi_op *op, int clk_en);

/*****************************************************************************/
#ifdef CONFIG_PM
int hifmc100_suspend(struct platform_device *pltdev, pm_message_t state);
int hifmc100_resume(struct platform_device *pltdev);
void hifmc100_spi_nand_config(struct hifmc_host *host);
#endif

#endif /* End of __HIFMC100_H__ */
