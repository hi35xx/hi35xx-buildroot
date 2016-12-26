/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#define SPI_CMD_FIRST_RESET_4ADDR  (0x66)
#define SPI_CMD_SECOND_RESET_4ADDR (0x99)

#define SPI_CMD_FLAG_SR_MICRON  0x70  /* READ FLAG STATUS REGISTER */
#define SPI_CMD_RD_RDCR_MICRON	0xB5  /* READ NONVOLATILE CONFIGURATION
					 REGISTER*/
#define SPI_CMD_WR_RDCR_MICRON	0xB1 /* WRITE NONVOLATILE CONFIGURATION
					 REGISTER*/
#define SPI_NOR_ADS_MASK	0x1
#define SPI_NOR_GET_4BYTE_BY_FLAG_SR(sr)     ((sr) & SPI_NOR_ADS_MASK)

#define SPI_NOR_ADS_SET_4BYTE(cr)	     ((cr) & (~SPI_NOR_ADS_MASK))
#define SPI_NOR_ADS_GET_4BYTE(cr)	     ((cr) & SPI_NOR_ADS_MASK)
/****************************************************************************/
static int spi_micron_entry_4addr(struct hifmc_spi *spi, int enable)
{
	unsigned char status;
	unsigned int reg;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	FMC_PR(AC_DBG, "\t* Start SPI Nor %s 4-byte mode.\n",
			str[enable]);

	if (spi->addrcycle != SPI_NOR_4BYTE_ADDR_LEN) {
		FMC_PR(AC_DBG, "\t* Not support 4B mode.\n");
		return 0;
	}

	status = spi_general_get_flash_register(spi, SPI_CMD_FLAG_SR_MICRON);
	FMC_PR(AC_DBG, "\t Read flag status register[%#x]:%#x\n",
			SPI_CMD_FLAG_SR_MICRON, status);
	if (SPI_NOR_GET_4BYTE_BY_FLAG_SR(status) == enable) {
		FMC_PR(AC_DBG, "\t* 4-byte was %sd, reg:%#x\n", str[enable],
				status);
		return 0;
	}

	spi->driver->write_enable(spi);

	if (enable)
		reg = SPI_CMD_EN4B;
	else
		reg = SPI_CMD_EX4B;
	hifmc_write(host, FMC_CMD, FMC_CMD_CMD1(reg));
	FMC_PR(AC_DBG, "\t  Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(AC_DBG, "\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(AC_DBG, "\t  Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi,
			SPI_CMD_FLAG_SR_MICRON);
	FMC_PR(AC_DBG, "\t Read flag status register[%#x]:%#x\n",
			SPI_CMD_FLAG_SR_MICRON, status);
	if (SPI_NOR_GET_4BYTE_BY_FLAG_SR(status) == enable)
		FMC_PR(AC_DBG, "\t  %s 4-byte success, SR3:%#x\n",
				str[enable], status);
	else
		DB_MSG("Error: %s 4-byte failed! SR3:%#x\n",
				str[enable], status);

	FMC_PR(AC_DBG, "\t* End SPI Nor flash %s 4-byte mode.\n", str[enable]);

	return 0;
}
