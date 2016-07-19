/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#define GD_SPI_CMD_RDSR1		0x35	/* Read Status Register-1 */

/*****************************************************************************/
/*
	enable QE bit if QUAD read write is supported by GD "25qxxx" SPI
*/
static int spi_gd25qxxx_qe_enable(struct hifmc_spi *spi)
{
	unsigned char config, status, op;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	op = spi_is_quad(spi);

	FMC_PR(QE_DBG, "\t*-Start GD SPI nor %s Quad.\n", str[op]);

	config = spi_general_get_flash_register(spi, GD_SPI_CMD_RDSR1);
	FMC_PR(QE_DBG, "\t|-Read GD SR-1[%#x], val: %#x\n", GD_SPI_CMD_RDSR1,
			config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op) {
		FMC_PR(QE_DBG, "\t* Quad was %sd, status:%#x\n", str[op],
				config);
		return op;
	}

	/* First, we enable/disable QE for 16Pin GD flash, use WRSR[01h] cmd */
	FMC_PR(QE_DBG, "\t|-First, 16Pin GD flash %s Quad.\n", str[op]);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(QE_DBG, "\t|-Read Status Register[%#x]%#x\n", SPI_CMD_RDSR,
			status);

	spi->driver->write_enable(spi);

	if (op)
		config |= SPI_NOR_CR_QE_MASK;
	else
		config &= ~SPI_NOR_CR_QE_MASK;
	writeb(status, host->iobase);
	writeb(config, host->iobase + SPI_NOR_SR_LEN);
	FMC_PR(QE_DBG, "\t|-Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned short *)host->iobase);

	regval = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(QE_DBG, "\t|-Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(QE_DBG, "\t|-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN + SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(QE_DBG, "\t|-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(QE_DBG, "\t|-Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	config = spi_general_get_flash_register(spi, GD_SPI_CMD_RDSR1);
	FMC_PR(QE_DBG, "\t|-Read GD SR-1[%#x], val: %#x\n", GD_SPI_CMD_RDSR1,
			config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op) {
		FMC_PR(QE_DBG, "\t|-16P %s Quad success reg: %#x\n", str[op],
				config);
		goto QE_END;
	} else
		FMC_PR(QE_DBG, "\t|-16P %s Quad failed, reg: %#x\n", str[op],
				config);

	/* Second, we enable/disable QE for 8Pin GD flash, use WRSR2[31h] cmd */
	FMC_PR(QE_DBG, "\t|-Second, 8Pin GD flash %s Quad.\n", str[op]);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(QE_DBG, "\t|-Read Status Register[%#x]:%#x\n", SPI_CMD_RDSR,
			status);
	if (!(status & STATUS_WEL_MASK))
		spi->driver->write_enable(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDSR2);
	FMC_PR(QE_DBG, "\t|-Read SR-2[%#x], val: %#x\n", SPI_CMD_RDSR2,
			config);
	if (op)
		config |= SPI_NOR_CR_QE_MASK;
	else
		config &= ~SPI_NOR_CR_QE_MASK;
	writeb(config, host->iobase);
	FMC_PR(QE_DBG, "\t|-Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned char *)host->iobase);

	regval = FMC_CMD_CMD1(SPI_CMD_WRSR2);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(QE_DBG, "\t|-Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(QE_DBG, "\t|-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(QE_DBG, "\t|-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(QE_DBG, "\t|-Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDSR2);
	FMC_PR(QE_DBG, "\t|-Read GD SR-2[%#x], val: %#x\n", SPI_CMD_RDSR2,
			config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op)
		FMC_PR(QE_DBG, "\t|-8P %s Quad success, reg: %#x.\n", str[op],
				config);
	else
		DB_MSG("Error: %s Quad failed, reg: %#x\n", str[op], config);

QE_END:

	FMC_PR(QE_DBG, "\t*-End GD SPI nor %s Quad end.\n", str[op]);

	return op;
}

