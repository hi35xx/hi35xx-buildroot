/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
/* SpanSion SPI Nor Flash "S25FL256S" Bank Address Register command */
#define SS_SPI_CMD_BRRD			0x16	/* Read Bank Register */
#define SS_SPI_CMD_BRWR			0x17	/* Write Bank Register */

/* Bank Address Register length(byte) */
#define SS_SPI_NOR_BR_LEN		1

/* Extended Address Enable bit[7] include in Bank Address Register */
#define SS_SPI_NOR_BR_EAE_SHIFT		7
#define SS_SPI_NOR_BR_EAE_MASK		(1 << SS_SPI_NOR_BR_EAE_SHIFT)
#define SS_SPI_NOR_GET_EAE_BY_BR(br)	(((br) & SS_SPI_NOR_BR_EAE_MASK) \
						>> SS_SPI_NOR_BR_EAE_SHIFT)

/*****************************************************************************/
/*
  enable 4byte address mode for SpanSion "s25fl256" SPI Nor
*/
static int spi_s25fl256s_entry_4addr(struct hifmc_spi *spi, int enable)
{
	unsigned char bank;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	FMC_PR(AC_DBG, "\t* Start SpanSion SPI Nor %s 4-byte mode.\n",
			str[enable]);

	if (spi->addrcycle != SPI_NOR_4BYTE_ADDR_LEN) {
		FMC_PR(AC_DBG, "\t* Flash isn't support 4-byte mode.\n");
		return 0;
	}

	/* Read old Bank Register value */
	bank = spi_general_get_flash_register(spi, SS_SPI_CMD_BRRD);
	FMC_PR(AC_DBG, "\t  Read Bank Register[%#x]%#x\n", SS_SPI_CMD_BRRD,
			bank);
	if (SS_SPI_NOR_GET_EAE_BY_BR(bank) == enable) {
		FMC_PR(AC_DBG, "\t* 4-byte was %sd, bank:%#x\n", str[enable],
				bank);
		return 0;
	}

	/* Write new Bank Register value */
	if (enable)
		bank |= SS_SPI_NOR_BR_EAE_MASK;
	else
		bank &= ~SS_SPI_NOR_BR_EAE_MASK;
	writeb(bank, host->iobase);
	FMC_PR(AC_DBG, "\t  Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned char *)host->iobase);

	regval = FMC_CMD_CMD1(SS_SPI_CMD_BRWR);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(AC_DBG, "\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(AC_DBG, "\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SS_SPI_NOR_BR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(AC_DBG, "\t  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(AC_DBG, "\t  Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	spi->driver->wait_ready(spi);

	/* Check out Bank Register value */
	bank = spi_general_get_flash_register(spi, SS_SPI_CMD_BRRD);
	FMC_PR(AC_DBG, "\t  Read Bank Register[%#x]%#x\n", SS_SPI_CMD_BRRD,
			bank);
	if (SS_SPI_NOR_GET_EAE_BY_BR(bank) == enable)
		FMC_PR(AC_DBG, "\t  %s 4byte success, bank:%#x.\n",
				str[enable], bank);
	else
		DB_MSG("Error: %s 4bytes failed! bank: %#x\n", str[enable],
				bank);

	FMC_PR(AC_DBG, "\t* End SpanSion SPI Nor %s 4-byte mode.\n",
			str[enable]);

	return 0;
}

