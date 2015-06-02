/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
******************************************************************************/

#include <linux/io.h>
#include <linux/mtd/mtd.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include "../spi_ids.h"
#include "hisfc350.h"

static int spi_n25q256a_entry_4addr(struct hisfc_spi *spi, int enable)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	if (spi->addrcycle != 4)
		return 0;

	if (enable) {
		spi->driver->write_enable(spi);
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_EN4B);
	} else {
		spi->driver->write_enable(spi);
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_EX4B);
	}

	hisfc_write(host, HISFC350_CMD_CONFIG,
		HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	return 0;
}

