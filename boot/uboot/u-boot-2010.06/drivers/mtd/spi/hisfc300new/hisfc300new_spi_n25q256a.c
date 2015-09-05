/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
******************************************************************************/

#include <common.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>

#include "../spi_ids.h"
#include "hisfc300new.h"

/*
   enable QE bit if QUAD read write is supported by SPI
*/
static int spi_n25q256a_qe_enable(struct hisfc_spi *spi)
{

	return 0;
}


static int spi_n25q256a_entry_4addr(struct hisfc_spi *spi, int enable)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN) {
		if (DEBUG_SPI)
			printf("SPI is only support 3-byte address mode\n");
		return 0;
	}

	spi->driver->write_enable(spi);

	if (enable) {
		hisfc_write(host, HISFC300_CMD_INS, SPI_CMD_EN4B);
		if (DEBUG_SPI)
			printf("now is 4-byte address mode\n");
	} else {
		hisfc_write(host, HISFC300_CMD_INS, SPI_CMD_EX4B);
		if (DEBUG_SPI)
			printf("now is 3-byte address mode\n");
	}

	hisfc_write(host, HISFC300_CMD_CONFIG,
		HISFC300_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	return 0;
}

