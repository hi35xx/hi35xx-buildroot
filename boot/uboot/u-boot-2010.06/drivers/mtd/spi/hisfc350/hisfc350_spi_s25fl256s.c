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
#include "hisfc350.h"

#define SPI_BRWR	0x17
#define SPI_EN4B	0x80
#define SPI_EX4B	0x00
/*
  enable 4byte address mode for
  s25fl256
*/
static int spi_s25fl256s_entry_4addr(struct hisfc_spi *spi, int enable)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;
	unsigned int regval = 0;

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN)
		return 0;
/*
  when enable == 1 enable 4byte address
  when enable == 0 change address mode from 4byte to 3byte,for reset
*/
	if (enable) {
		hisfc_write(host, HISFC350_CMD_INS, SPI_BRWR);
		hisfc_write(host, HISFC350_CMD_DATABUF0, SPI_EN4B);
	} else {
		hisfc_write(host, HISFC350_CMD_INS, SPI_BRWR);
		hisfc_write(host, HISFC350_CMD_DATABUF0, SPI_EX4B);
	}

	hisfc_write(host, HISFC350_CMD_CONFIG,
		HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_DATA_CNT(1)
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);
	if (DEBUG_SPI) {
		regval = hisfc_read(host, HISFC350_CMD_DATABUF0);
		if (!(regval & SPI_EN4B)) {
			printf("now is 3-byte address mode\n");
			printf("regval_read_SPI : 0x%x\n", regval);
		} else {
			printf("now is 4-byte address mode\n");
		}
	}
	host->set_host_addr_mode(host, enable);

	return 0;
}

