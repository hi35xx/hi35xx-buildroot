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

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>

#include "../spi_ids.h"
#include "hisfc350.h"

#define DEBUG_SPI 0
#define DEBUG_GET_SR 0
#define DEBUG_SPI_QE 0

/*****************************************************************************/
u_char spi_general_get_flash_register(struct hisfc_spi *spi, u_char cmd)
{
	unsigned char status;
	unsigned int regval;
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	if (DEBUG_GET_SR)
		printf("* Start get flash Register %#x.\n", cmd);

	hisfc_write(host, HISFC350_CMD_INS, cmd);
	if (DEBUG_GET_SR)
		printf("  Set INS[%#x]%#x\n", HISFC350_CMD_INS, cmd);

	regval = HISFC350_CMD_CONFIG_DATA_CNT(SPI_NOR_SR_LEN)
		| HISFC350_CMD_CONFIG_RW_READ
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_START;

	hisfc_write(host, HISFC350_CMD_CONFIG, regval);
	if (DEBUG_GET_SR)
		printf("  Set CONFIG[%#x]%#x\n", HISFC350_CMD_CONFIG, regval);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	status = hisfc_read(host, HISFC350_CMD_DATABUF0);
	if (DEBUG_GET_SR)
		printf("* Get flash Register %#x, val[%#x]\n", cmd, status);

	return status;
}

/*****************************************************************************/
static int spi_general_wait_ready(struct hisfc_spi *spi)
{
	unsigned long status;
	unsigned long deadline = 0;

	do {
		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
		if (!(status & SPI_CMD_SR_WIP))
			return 0;

		udelay(1);

	} while (deadline++ < (40 << 20));

	printf("Wait spi flash ready timeout.\n");

	return 1;
}

/*****************************************************************************/
static int spi_general_write_enable(struct hisfc_spi *spi)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_WREN);

	hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	return 0;
}

/*****************************************************************************/
/*
  enable 4byte adress for SPI which memory more than 16M
*/
static int spi_general_entry_4addr(struct hisfc_spi *spi, int enable)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN)
		return 0;

	if (enable)
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_EN4B);
	else
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_EX4B);

	hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	return 0;
}

/*****************************************************************************/
/*
  configure prepared for dma or bus read or write mode
*/
static int spi_general_bus_prepare(struct hisfc_spi *spi, int op)
{
	unsigned int regval = 0;
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

#ifdef HISFCV350_SUPPORT_BUS_WRITE
	regval |= HISFC350_BUS_CONFIG1_WRITE_EN;
#endif
	regval |= HISFC350_BUS_CONFIG1_WRITE_INS(spi->write->cmd);
	regval |= HISFC350_BUS_CONFIG1_WRITE_DUMMY_CNT(spi->write->dummy);
	regval |= HISFC350_BUS_CONFIG1_WRITE_IF_TYPE(spi->write->iftype);

#ifdef HISFCV350_SUPPORT_BUS_READ
	regval |= HISFC350_BUS_CONFIG1_READ_EN;
#endif
	regval |= HISFC350_BUS_CONFIG1_READ_PREF_CNT(0);
	regval |= HISFC350_BUS_CONFIG1_READ_INS(spi->read->cmd);
	regval |= HISFC350_BUS_CONFIG1_READ_DUMMY_CNT(spi->read->dummy);
	regval |= HISFC350_BUS_CONFIG1_READ_IF_TYPE(spi->read->iftype);

	hisfc_write(host, HISFC350_BUS_CONFIG1, regval);
	hisfc_write(host, HISFC350_BUS_CONFIG2,
		HISFC350_BUS_CONFIG2_WIP_LOCATE(0));
	if (op == READ)
		host->set_system_clock(host, spi->read, TRUE);
	else if (op == WRITE)
		host->set_system_clock(host, spi->write, TRUE);

	return 0;
}

/*****************************************************************************/
/*
  judge whether SPI support QUAD read write or not
*/
static int hisfc350_is_quad(struct hisfc_spi *spi)
{
	if (DEBUG_SPI_QE)
		printf("SPI read if[%d] write if[%d]\n",
			spi->read->iftype, spi->write->iftype);

	if (spi->write->iftype == 5 || spi->write->iftype == 6
		|| spi->write->iftype == 7 || spi->read->iftype == 5
		|| spi->read->iftype == 6 || spi->read->iftype == 7)
		return 1;

	return 0;
}

/*****************************************************************************/
/*
   enable QE bit if QUAD read write is supported by SPI
*/
static int spi_general_qe_enable(struct hisfc_spi *spi)
{
	unsigned char status, config, op;
	unsigned int reg;
	const char *str[] = {"Disable", "Enable"};
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	op = hisfc350_is_quad(spi);

	if (DEBUG_SPI_QE)
		printf("* Start SPI Nor %s Quad.\n", str[op]);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	if (DEBUG_SPI_QE)
		printf("  Read config %#x, val[%#x]\n", SPI_CMD_RDCR, config);
	if (((config & SPI_NOR_CR_QE_MASK) >> SPI_NOR_CR_QE_SHIFT) == op) {
		if (DEBUG_SPI_QE)
			printf("* Quad was %sd!\n", str[op]);
		return op;
	}

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	reg = (config << SPI_NOR_CR_SHIFT) | status;
	if (DEBUG_SPI_QE)
		printf("  Read CR/SR[%#x]\n", reg);

	spi->driver->write_enable(spi);

	if (op)
		reg |= (SPI_NOR_CR_QE_MASK << SPI_NOR_CR_SHIFT);
	else
		reg &= ~(SPI_NOR_CR_QE_MASK << SPI_NOR_CR_SHIFT);
	hisfc_write(host, HISFC350_CMD_DATABUF0, reg);
	if (DEBUG_SPI_QE)
		printf("  Set DATABUF0[%#x]%#x\n", HISFC350_CMD_DATABUF0, reg);

	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_WRSR);
	if (DEBUG_SPI_QE)
		printf("  Set INS[%#x]%#x\n", HISFC350_CMD_INS, SPI_CMD_WRSR);

	reg = HISFC350_CMD_CONFIG_DATA_CNT(SPI_NOR_SR_LEN + SPI_NOR_CR_LEN)
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_START;
	hisfc_write(host, HISFC350_CMD_CONFIG, reg);
	if (DEBUG_SPI_QE)
		printf("  Set CONFIG[%#x]%#x\n", HISFC350_CMD_CONFIG, reg);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	if (((config & SPI_NOR_CR_QE_MASK) >> SPI_NOR_CR_QE_SHIFT)
			== op)
		printf("SPI Quad %s succeed. [%#x]\n", str[op],
				config);
	else
		DBG_MSG("%s Quad failed! [%#x]\n", str[op], config);

	return op;
}

/*****************************************************************************/
/*
  some chip don't QUAD enable
*/
static int spi_do_not_qe_enable(struct hisfc_spi *spi)
{
	return 0;
}
