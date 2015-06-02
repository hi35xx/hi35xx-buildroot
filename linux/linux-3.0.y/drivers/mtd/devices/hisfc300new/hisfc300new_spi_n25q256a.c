/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
******************************************************************************/

#include <linux/io.h>
#include <linux/mtd/mtd.h>
#include <linux/errno.h>
#include <linux/sched.h>
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
	struct hisfc300_host *host = (struct hisfc300_host *)spi->host;

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN)
		return 0;

	spi->driver->write_enable(spi);

	if (enable)
		hisfc300_write(host, HISFC300_CMD_INS, SPI_CMD_EN4B);
	else
		hisfc300_write(host, HISFC300_CMD_INS, SPI_CMD_EX4B);

	hisfc300_write(host, HISFC300_CMD_CONFIG,
			HISFC300_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	return 0;
}
