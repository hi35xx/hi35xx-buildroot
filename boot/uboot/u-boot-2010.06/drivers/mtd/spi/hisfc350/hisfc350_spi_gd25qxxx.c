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

#define GD_SPI_CMD_SR_QE   (0x2)
#define GD_SPI_CMD_SR_DISQE   (0x0)
#define SPI_CMD_WRSR2      (0x31)
#define SPI_CMD_WRSR1      (0x1)

/*
   enable QE bit if QUAD read write is supported by SPI
 */
static int spi_gd25qxxx_qe_enable(struct hisfc_spi *spi)
{
	struct hisfc_host *host = (struct hisfc_host *)spi->host;
	unsigned int regval = 0;
	unsigned int qe_op1 = 0;
	unsigned int qe_op2 = 0;
	if (hisfc350_is_quad(spi)) {
		qe_op1 = SPI_CMD_SR_QE;
		qe_op2 = GD_SPI_CMD_SR_QE;
	} else {
		qe_op1 = SPI_CMD_SR_XQE;
		qe_op2 = GD_SPI_CMD_SR_DISQE;
	}

	spi->driver->write_enable(spi);

	/* First, we enable QE(4bit r&w) for 16pin gd flash */
	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_WRSR1);
	hisfc_write(host, HISFC350_CMD_DATABUF0, qe_op1);

	hisfc_write(host, HISFC350_CMD_CONFIG,
			HISFC350_CMD_CONFIG_MEM_IF_TYPE(spi->
				write->iftype)
			| HISFC350_CMD_CONFIG_DATA_CNT(2)
			| HISFC350_CMD_CONFIG_DATA_EN
			| HISFC350_CMD_CONFIG_DUMMY_CNT(spi->
				write->dummy)
			| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	/* Second, we enable QE for 8 pin gd flash. This will not affect
	   16 gd spi, if the QE bit has been set 1.
	 */
	spi->driver->write_enable(spi);

	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_WRSR2);
	hisfc_write(host, HISFC350_CMD_DATABUF0, qe_op2);

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
		hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_RDSR2);

		hisfc_write(host, HISFC350_CMD_CONFIG,
				HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
				| HISFC350_CMD_CONFIG_DATA_CNT(1)
				| HISFC350_CMD_CONFIG_DATA_EN
				| HISFC350_CMD_CONFIG_RW_READ
				| HISFC350_CMD_CONFIG_START);
		HISFC350_CMD_WAIT_CPU_FINISH(host);
		regval = hisfc_read(host, HISFC350_CMD_DATABUF0);
		printf("QEbit = 0x2? : 0x%x\n", regval);
		if ((regval & GD_SPI_CMD_SR_QE))
			printf("QE bit enable success\n");
		else
			printf("QE bit enable failed\n");
	}
	return 0;
}
