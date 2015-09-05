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
  enable 4byte address mode for
  s25fl256
*/
static int spi_s25fl256s_entry_4addr(struct hisfc_spi *spi, int enable)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN)
		return 0;
	/*
	  when enable == 1 enable 4byte address
	  when enable == 0 change address mode from 4byte to 3byte,for reset
	*/
	if (enable) {
		hisfc_write(host, HISFC300_CMD_INS, SPI_CMD_BRWR);
		hisfc_write(host, HISFC300_CMD_DATABUF0, SPI_EN4B_VALUE);
	} else {
		hisfc_write(host, HISFC300_CMD_INS, SPI_CMD_BRWR);
		hisfc_write(host, HISFC300_CMD_DATABUF0, SPI_EX4B_VALUE);
	}

	hisfc_write(host, HISFC300_CMD_CONFIG,
		HISFC300_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC300_CMD_CONFIG_DATA_CNT(1)
		| HISFC300_CMD_CONFIG_DATA_EN
		| HISFC300_CMD_CONFIG_START);
	HISFC300_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	host->set_host_addr_mode(host, enable);

	return 0;
}

