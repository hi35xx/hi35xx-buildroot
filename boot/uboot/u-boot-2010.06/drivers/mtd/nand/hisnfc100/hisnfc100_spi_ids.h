/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
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

#ifndef HISFC_SPI_IDSH
#define HISFC_SPI_IDSH

/*****************************************************************************/
#define INFINITE			(0xFFFFFFFF)

#define DEFAULT_ID_LEN			2
#define MAX_ID_LEN			3
#define MAX_SPI_NAND_OP			8

#define BBP_LAST_PAGE			0x01
#define BBP_FIRST_PAGE			0x02

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

#define SPI_IF_ERASE_SECTOR_128K	(0x08)  /* 128K */
#define SPI_IF_ERASE_SECTOR_256K	(0x10)  /* 256K */

/******************************************************************************/
#define HISNFC100_SUPPORT_READ		(SPI_IF_READ_STD \
					| SPI_IF_READ_FAST \
					| SPI_IF_READ_DUAL \
					| SPI_IF_READ_DUAL_ADDR \
					| SPI_IF_READ_QUAD \
					| SPI_IF_READ_QUAD_ADDR)

#define HISNFC100_SUPPORT_WRITE		(SPI_IF_WRITE_STD | SPI_IF_WRITE_QUAD)

#define HISNFC100_SUPPORT_MAX_DUMMY	8

#define SPI_NAND_READ			0
#define SPI_NAND_WRITE			1

#define HISNFC100_IFCYCLE_STD				0
#define HISNFC100_IFCYCLE_DUAL				1
#define HISNFC100_IFCYCLE_DUAL_ADDR			2
#define HISNFC100_IFCYCLE_QUAD				3
#define HISNFC100_IFCYCLE_QUAD_ADDR			4

/*****************************************************************************/
#define SPI_CMD_WREN			0x06   /* Write Enable */
#define SPI_CMD_WRDI			0x04   /* Write Disable */

#define SPI_CMD_GET_FEATURES		0x0F   /* Get Features */
#define SPI_CMD_SET_FEATURE		0x1F   /* Set Feature */

#define SPI_CMD_PAGE_READ		0x13   /* Page Read to Cache */
#define SPI_CMD_READ_STD	0x03   /* Read From Cache at Standard Speed */
#define SPI_CMD_READ_FAST	0x0B   /* Read From Cache at Higher Speed */
#define SPI_CMD_READ_DUAL	0x3B   /* Read From Cache at Dual Output */
#define SPI_CMD_READ_QUAD	0x6B   /* Read From Cache at Quad Output */
#define SPI_CMD_READ_DUAL_ADDR	0xBB   /* Read From Cache at Dual I/O */
#define SPI_CMD_READ_QUAD_ADDR	0xEB   /* Read From Cache at Quad I/O */

#define SPI_CMD_RDID			0x9F   /* Read Identification */

#define SPI_CMD_WRITE_STD	0x02   /* Page Load at Standard Input */
#define SPI_CMD_WRITE_QUAD	0x32   /* Page Load at Quad Input */

#define SPI_CMD_SE_128K			0xD8   /* 128KB sector Erase */
#define SPI_CMD_SE_256K			0xD8   /* 256KB sector Erase */

#define SPI_CMD_RESET			0xff   /* Reset the device */

/*****************************************************************************/
/* SPI operation information */
struct spi_op_info {
	unsigned char  iftype;
	unsigned char  cmd;
	unsigned char  dummy;
	unsigned int   size;
	unsigned int   clock;
};

struct spi_nand_driver;

struct hisnfc_op {
	void *host;
	struct spi_nand_driver *driver;
	struct spi_op_info  read[1];
	struct spi_op_info  write[1];
	struct spi_op_info  erase[MAX_SPI_NAND_OP];
};

struct spi_nand_driver {
	int (*wait_ready)(struct hisnfc_op *spi);
	int (*write_enable)(struct hisnfc_op *spi);
	int (*qe_enable)(struct hisnfc_op *spi);
};

struct hisnfc_chip_info {
	char *name;
	unsigned char id[MAX_ID_LEN];
	unsigned char id_len;
	unsigned long long chipsize;
	unsigned int erasesize;
	unsigned int pagesize;
	unsigned int oobsize;
	unsigned int badblock_pos;
	struct spi_op_info *read[MAX_SPI_NAND_OP];
	struct spi_op_info *write[MAX_SPI_NAND_OP];
	struct spi_op_info *erase[MAX_SPI_NAND_OP];
	struct spi_nand_driver *driver;
};

/*****************************************************************************/
void spi_nand_ids_register(void);

void hisnfc100_get_best_clock(unsigned int *clock);

struct hisnfc_host;

void spi_feature_op(struct hisnfc_host *host, int op, int addr, unsigned *val);

/******************************************************************************/

#endif /* HISFC_SPI_IDSH */

