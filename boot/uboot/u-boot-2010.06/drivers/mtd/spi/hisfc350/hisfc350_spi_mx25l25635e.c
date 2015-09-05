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

#define MX_SPI_CMD_SR_QE   (0x40)

/*
   enable QE bit if QUAD read write is supported by SPI
 */
static int spi_mx25l25635e_qe_enable(struct hisfc_spi *spi)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;
	unsigned int regval = 0;
	unsigned int qe_op = 0;
	if (hisfc350_is_quad(spi))
		qe_op = MX_SPI_CMD_SR_QE;
	else
		qe_op = SPI_CMD_SR_XQE;

	spi->driver->write_enable(spi);

	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_WRSR);
	hisfc_write(host, HISFC350_CMD_DATABUF0, qe_op);

	hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_MEM_IF_TYPE(spi->
				write->iftype)
			| HISFC350_CMD_CONFIG_DATA_CNT(1)
			| HISFC350_CMD_CONFIG_DATA_EN
			| HISFC350_CMD_CONFIG_DUMMY_CNT(spi->
				write->dummy)
			| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	if (DEBUG_SPI) {
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_RDSR);

		hisfc_write(host, HISFC350_CMD_CONFIG,
				HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
				| HISFC350_CMD_CONFIG_DATA_CNT(1)
				| HISFC350_CMD_CONFIG_DATA_EN
				| HISFC350_CMD_CONFIG_RW_READ
				| HISFC350_CMD_CONFIG_START);
		HISFC350_CMD_WAIT_CPU_FINISH(host);
		regval = hisfc_read(host, HISFC350_CMD_DATABUF0);
		printf("QEbit = 0x40? : 0x%x\n", regval);
		if ((regval & MX_SPI_CMD_SR_QE))
			printf("QE bit enable success\n");
		else
			printf("QE bit enable failed\n");
	}
	return 0;
}
