/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
******************************************************************************/

#include <linux/io.h>
#include <linux/mtd/mtd.h>
#include <linux/errno.h>

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
			printk(KERN_INFO "now w25q256fv is 4-byte address mode\n");

		hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);

		HISFC350_CMD_WAIT_CPU_FINISH(host);
	} else {
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
			printk(KERN_INFO "now W25Q256FV 6699 cmd\n");
	}
/*
	hisfc_write(host, HISFC350_CMD_CONFIG,
		HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_DATA_CNT(1)
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);
*/
	host->set_host_addr_mode(host, enable);

	return 0;
}

