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

#define SPI_CMD_FIRST_RESET_4ADDR  (0x66)
#define SPI_CMD_SECOND_RESET_4ADDR (0x99)

static int spi_w25q256fv_entry_4addr(struct hisfc_spi *spi, int enable)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN)
		return 0;
	/* This chip should not enable write here,
	 * we have confirmed with the WINBOND */
	/* spi->driver->write_enable(spi); */
	if (enable) {
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_EN4B);
		if (DEBUG_SPI)
			printf("now w25q256fv is 4-byte address mode\n");

		hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);

		HISFC350_CMD_WAIT_CPU_FINISH(host);
	} else {
/* reset cmd */
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_FIRST_RESET_4ADDR);
		hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);


		HISFC350_CMD_WAIT_CPU_FINISH(host);


		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_SECOND_RESET_4ADDR);
		hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);

		HISFC350_CMD_WAIT_CPU_FINISH(host);
		if (DEBUG_SPI)
			printf("now W25Q256FV 6699 cmd\n");
	}

	host->set_host_addr_mode(host, enable);

	return 0;
}

