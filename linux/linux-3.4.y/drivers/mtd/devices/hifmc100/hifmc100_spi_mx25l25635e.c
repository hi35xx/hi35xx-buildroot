/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
/* MXIC QE(bit) include in Status Register */
#define MX_SPI_NOR_SR_QE_SHIFT	6
#define MX_SPI_NOR_SR_QE_MASK	(1 << MX_SPI_NOR_SR_QE_SHIFT)
#define MX_SPI_NOR_GET_QE_BY_SR(sr)	(((sr) & MX_SPI_NOR_SR_QE_MASK) \
						>> MX_SPI_NOR_SR_QE_SHIFT)

/*****************************************************************************/
/*
   enable QE bit if 4X R/W is supported by MXIC "25L(256/257)35(E/F)" SPI
*/
static int spi_mx25l25635e_qe_enable(struct hifmc_spi *spi)
{
	unsigned char status, op;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	op = spi_is_quad(spi);

	FMC_PR(QE_DBG, "\t|*-Start MXIC SPI Nor %s Quad.\n", str[op]);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(QE_DBG, "\t||-Read Status Register[%#x]%#x\n", SPI_CMD_RDSR,
			status);
	if (MX_SPI_NOR_GET_QE_BY_SR(status) == op) {
		FMC_PR(QE_DBG, "\t|*-Quad was %sd, status:%#x\n", str[op],
				status);
		return op;
	}

	spi->driver->write_enable(spi);

	if (op)
		status |= MX_SPI_NOR_SR_QE_MASK;
	else
		status &= ~MX_SPI_NOR_SR_QE_MASK;
	writeb(status, host->iobase);
	FMC_PR(QE_DBG, "\t||-Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned char *)host->iobase);

	regval = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(QE_DBG, "\t||-Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(QE_DBG, "\t||-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(QE_DBG, "\t||-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(QE_DBG, "\t||-Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	if (MX_SPI_NOR_GET_QE_BY_SR(status) == op)
		FMC_PR(QE_DBG, "\t||-%s Quad success, status:%#x.\n", str[op],
				status);
	else
		DB_MSG("Error: %s Quad failed! reg: %#x\n", str[op], status);

	FMC_PR(QE_DBG, "\t|*-End MXIC SPI Nor %s Quad.\n", str[op]);

	return op;
}

