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
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>

#include "../spi_ids.h"
#include "hisfc350.h"

/* MXIC QE(bit) include in Status Register */
#define MX_SPI_NOR_SR_QE_SHIFT	6
#define MX_SPI_NOR_SR_QE_MASK	(1 << MX_SPI_NOR_SR_QE_SHIFT)

/*****************************************************************************/
/*
   enable QE bit if QUAD read write is supported by SPI
*/
static int spi_mx25l25635e_qe_enable(struct hisfc_spi *spi)
{
	unsigned char status, op;
	unsigned int reg;
	const char *str[] = {"Disable", "Enable"};
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	op = hisfc350_is_quad(spi);

	if (DEBUG_SPI_QE)
		printf("* Start SPI Nor %s Quad.\n", str[op]);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	if (DEBUG_SPI_QE)
		printf("  Read status %#x, val[%#x]\n", SPI_CMD_RDSR, status);
	if (((status & MX_SPI_NOR_SR_QE_MASK) >> MX_SPI_NOR_SR_QE_SHIFT)
			== op) {
		if (DEBUG_SPI_QE)
			printf("* Quad was %sd!\n", str[op]);
		return op;
	}

	spi->driver->write_enable(spi);

	if (op)
		status |= MX_SPI_NOR_SR_QE_MASK;
	else
		status &= ~MX_SPI_NOR_SR_QE_MASK;
	hisfc_write(host, HISFC350_CMD_DATABUF0, status);
	if (DEBUG_SPI_QE)
		printf("  Set DATA[%#x]%#x\n", HISFC350_CMD_DATABUF0, status);

	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_WRSR);
	if (DEBUG_SPI_QE)
		printf("  Set INS[%#x]%#x\n", HISFC350_CMD_INS, SPI_CMD_WRSR);

	reg = HISFC350_CMD_CONFIG_DATA_CNT(SPI_NOR_SR_LEN)
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_START;
	hisfc_write(host, HISFC350_CMD_CONFIG, reg);
	if (DEBUG_SPI_QE)
		printf("  Set CONFIG[%#x]%#x\n", HISFC350_CMD_CONFIG, reg);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	if (((status & MX_SPI_NOR_SR_QE_MASK) >> MX_SPI_NOR_SR_QE_SHIFT)
			== op)
		printf("SPI Nor %s Quad succeed.\n", str[op]);
	else
		DBG_MSG("%s Quad failed! [%#x]\n", str[op], status);

	return op;
}

