/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#define W25Q256FV_CR_4BYTE_MASK		0x1

#define WB_SPI_NOR_SR_ADS_MASK	1
#define WB_SPI_NOR_GET_4BYTE_BY_SR(sr)	((sr) & WB_SPI_NOR_SR_ADS_MASK)

#define SPI_CMD_FIRST_RESET_4ADDR  (0x66)
#define SPI_CMD_SECOND_RESET_4ADDR (0x99)

/****************************************************************************/
static int spi_w25q256fv_entry_4addr(struct hifmc_spi *spi, int enable)
{
	unsigned char status;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	FMC_PR(AC_DBG, "\t* Start W25Q256FV SPI Nor %s 4-byte mode.\n",
			str[enable]);

	if (spi->addrcycle != SPI_NOR_4BYTE_ADDR_LEN) {
		FMC_PR(AC_DBG, "\t* W25Q(128/256)FV not support 4B mode.\n");
		return 0;
	}

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
	FMC_PR(AC_DBG, "\t  Read Status Register-3[%#x]:%#x\n", SPI_CMD_RDSR3,
			status);
	if (WB_SPI_NOR_GET_4BYTE_BY_SR(status) == enable) {
		FMC_PR(AC_DBG, "\t* 4-byte was %sd, reg:%#x\n", str[enable],
				status);
		return 0;
	}

	/* This chip should not write enable here,
	 * we have confirmed with the WINBOND */
	/* spi->driver->write_enable(spi); */

	if (enable) {
		regval = FMC_CMD_CMD1(SPI_CMD_EN4B);
		hifmc_write(host, FMC_CMD, regval);
		FMC_PR(AC_DBG, "\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

		regval = OP_CFG_FM_CS(spi->chipselect);
		hifmc_write(host, FMC_OP_CFG, regval);
		FMC_PR(AC_DBG, "\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

		regval = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
		hifmc_write(host, FMC_OP, regval);
		FMC_PR(AC_DBG, "\t  Set OP[%#x]%#x\n", FMC_OP, regval);

		FMC_CMD_WAIT_CPU_FINISH(host);

		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
		FMC_PR(AC_DBG, "\t  Get Status Register 3[%#x]:%#x\n",
				SPI_CMD_RDSR3, status);
		if (status & W25Q256FV_CR_4BYTE_MASK)
			FMC_PR(AC_DBG, "\t  Enter 4-byte success, reg[%#x]\n",
					status);
		else
			DB_MSG("Error: Enter 4-byte failed! [%#x]\n", status);

	} else {
/* reset cmd */
		regval = FMC_CMD_CMD1(SPI_CMD_FIRST_RESET_4ADDR);
		hifmc_write(host, FMC_CMD, regval);
		FMC_PR(AC_DBG, "\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

		regval = OP_CFG_FM_CS(spi->chipselect);
		hifmc_write(host, FMC_OP_CFG, regval);
		FMC_PR(AC_DBG, "\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

		regval = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
		hifmc_write(host, FMC_OP, regval);
		FMC_PR(AC_DBG, "\t  Set OP[%#x]%#x\n", FMC_OP, regval);

		FMC_CMD_WAIT_CPU_FINISH(host);

		regval = FMC_CMD_CMD1(SPI_CMD_SECOND_RESET_4ADDR);
		hifmc_write(host, FMC_CMD, regval);
		FMC_PR(AC_DBG, "\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

		regval = OP_CFG_FM_CS(spi->chipselect);
		hifmc_write(host, FMC_OP_CFG, regval);
		FMC_PR(AC_DBG, "\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

		regval = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
		hifmc_write(host, FMC_OP, regval);
		FMC_PR(AC_DBG, "\t  Set OP[%#x]%#x\n", FMC_OP, regval);

		FMC_CMD_WAIT_CPU_FINISH(host);

		FMC_PR(AC_DBG, "\tnow W25Q256FV start software reset\n");
	}

	host->set_host_addr_mode(host, enable);

	FMC_PR(AC_DBG, "\t* End W25Q256FV enter 4-byte mode.\n");

	return 0;
}

/****************************************************************************/
/*
   enable QE bit if QUAD read write is supported by W25Q(128/256)FV
*/
static int spi_w25q256fv_qe_enable(struct hifmc_spi *spi)
{
	unsigned char status, op;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	op = spi_is_quad(spi);

	FMC_PR(QE_DBG, "\t* Start SPI Nor W25Q(128/256)FV %s Quad.\n", str[op]);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR2);
	FMC_PR(QE_DBG, "\t  Read Status Register-2[%#x]%#x\n", SPI_CMD_RDSR2,
			status);
	if (SPI_NOR_GET_QE_BY_CR(status) == op) {
		FMC_PR(QE_DBG, "\t* Quad was %s status:%#x\n", str[op], status);
		return op;
	}

	spi->driver->write_enable(spi);

	if (op)
		status |= SPI_NOR_CR_QE_MASK;
	else
		status &= ~SPI_NOR_CR_QE_MASK;
	writeb(status, host->iobase);
	FMC_PR(QE_DBG, "\t  Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned char *)host->iobase);

	/* There is new cmd for Write Status Register 2 by W25Q(128/256)FV */
	regval = FMC_CMD_CMD1(SPI_CMD_WRSR2);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(QE_DBG, "\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(QE_DBG, "\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(QE_DBG, "\t  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(QE_DBG, "\t  Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	if (QE_DBG) {
		spi->driver->wait_ready(spi);

		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR2);
		FMC_PR(QE_DBG, "\t  Read Status Register-2[%#x]:%#x\n",
				SPI_CMD_RDSR2, status);
		if (SPI_NOR_GET_QE_BY_CR(status) == op)
			FMC_PR(QE_DBG, "\t  %s Quad success. status:%#x\n",
					str[op], status);
		else
			DB_MSG("Error: %s Quad failed! reg:%#x\n", str[op],
					status);
	}

	FMC_PR(QE_DBG, "\t* End SPI Nor W25Q(128/256)FV %s Quad.\n", str[op]);

	return op;
}

