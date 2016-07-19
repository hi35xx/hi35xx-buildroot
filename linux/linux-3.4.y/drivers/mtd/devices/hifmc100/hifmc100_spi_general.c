/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
/*
  Get status/config register value from SPI Nor flash
*/
u_char spi_general_get_flash_register(struct hifmc_spi *spi, u_char cmd)
{
	unsigned char status;
	unsigned int reg;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	FMC_PR(SR_DBG, "\t * Start get flash Register[%#x]\n", cmd);

	reg = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(SR_DBG, "\t   Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	if (cmd == SPI_CMD_RDSR) {
		reg = FMC_OP_READ_STATUS_EN(ENABLE) | FMC_OP_REG_OP_START;
		goto cmd_config_done;
	}

	hifmc_write(host, FMC_CMD, cmd);
	FMC_PR(SR_DBG, "\t   Set CMD[%#x]%#x\n", FMC_CMD, cmd);

	reg = FMC_DATA_NUM_CNT(SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, reg);
	FMC_PR(SR_DBG, "\t   Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_READ_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
cmd_config_done:
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(SR_DBG, "\t   Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	if (cmd == SPI_CMD_RDSR)
		status = hifmc_read(host, FMC_STATUS);
	else
		status = readb(host->iobase);
	FMC_PR(SR_DBG, "\t * End get flash Register[%#x], val: %#x\n", cmd,
			status);

	return status;
}

/*****************************************************************************/
/*
	Read status[C0H]:[0]bit OIP, judge whether the device is busy or not
*/
static int spi_general_wait_ready(struct hifmc_spi *spi)
{
	unsigned char status;
	unsigned long deadline = jiffies + FMC_MAX_READY_WAIT_JIFFIES;

	do {
		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
		if (!(status & SPI_NOR_SR_WIP_MASK))
			return 0;

		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	DB_MSG("Error: SPI nor wait ready timeout, status[%#x]\n", status);

	return 1;
}

/*****************************************************************************/
/*
	Send write enable cmd to SPI Nor, status[C0H]:[2]bit WEL must be set 1
*/
static int spi_general_write_enable(struct hifmc_spi *spi)
{
	unsigned char status;
	unsigned int reg;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	if (WE_DBG)
		pr_info("\n");
	FMC_PR(WE_DBG, "\t  * Start Write Enable\n");

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(WE_DBG, "\t    Read Status Register[%#x]:%#x\n", SPI_CMD_RDSR,
			status);
	if (status & STATUS_WEL_MASK) {
		FMC_PR(WE_DBG, "\t    Write Enable was opened! reg: %#x\n",
				status);
		return 0;
	}

	reg = hifmc_read(host, FMC_GLOBAL_CFG);
	if (reg & FMC_GLOBAL_CFG_WP_ENABLE) {
		reg &= ~FMC_GLOBAL_CFG_WP_ENABLE;
		hifmc_write(host, FMC_GLOBAL_CFG, reg);
		FMC_PR(WE_DBG, "\t    Set GLOBAL_CFG[%#x]%#x\n",
				FMC_GLOBAL_CFG, reg);
	}

	reg = FMC_CMD_CMD1(SPI_CMD_WREN);
	hifmc_write(host, FMC_CMD, reg);
	FMC_PR(WE_DBG, "\t    Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(WE_DBG, "\t    Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(WE_DBG, "\t    Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

#if WE_DBG
	spi->driver->wait_ready(spi);

	reg = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	if (reg & STATUS_WEL_MASK)
		FMC_PR(WE_DBG, "\t    Write Enable success.reg: %#x\n", reg);
	else {
		DB_MSG("Error: Write Enable failed! status: %#x\n", reg);
		return status;
	}
#endif

	FMC_PR(WE_DBG, "\t  * End Write Enable\n");

	return 0;
}

/*****************************************************************************/
/*
  enable 4byte address for SPI which memory more than 16M
*/
static int spi_general_entry_4addr(struct hifmc_spi *spi, int enable)
{
	unsigned char status;
	unsigned int reg;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	FMC_PR(AC_DBG, "\t* Start SPI Nor flash %s 4-byte mode.\n",
			str[enable]);

	if (spi->addrcycle != SPI_NOR_4BYTE_ADDR_LEN) {
		FMC_PR(AC_DBG, "\t* Flash isn't support entry 4-byte mode.\n");
		return 0;
	}

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
	FMC_PR(AC_DBG, "\t  Read Status Register-3[%#x]:%#x\n", SPI_CMD_RDSR3,
			status);
	if (SPI_NOR_GET_4BYTE_BY_CR(status) == enable) {
		FMC_PR(AC_DBG, "\t* 4-byte was %sd, reg:%#x\n", str[enable],
				status);
		return 0;
	}

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

	if (AC_DBG) {
		spi->driver->wait_ready(spi);

		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
		FMC_PR(AC_DBG, "\t  Read SR-3[%#x]:%#x\n", SPI_CMD_RDSR3,
				status);
		if (SPI_NOR_GET_4BYTE_BY_CR(status) == enable)
			FMC_PR(AC_DBG, "\t  %s 4-byte success, SR3:%#x\n",
					str[enable], status);
		else
			DB_MSG("Error: %s 4-byte failed! SR3:%#x\n",
					str[enable], status);
	}

	FMC_PR(AC_DBG, "\t* End SPI Nor flash %s 4-byte mode.\n", str[enable]);

	return 0;
}

/*****************************************************************************/
/*
	judge whether SPI Nor support QUAD read write or not
*/
int spi_is_quad(struct hifmc_spi *spi)
{
	const char *if_str[] = {"STD", "DUAL", "DIO", "QUAD", "QIO"};

	FMC_PR(QE_DBG, "\t\t|*-SPI read iftype: %s write iftype: %s\n",
		if_str[spi->read->iftype], if_str[spi->write->iftype]);

	if ((IF_TYPE_QUAD == spi->read->iftype)
	    || (IF_TYPE_QIO == spi->read->iftype)
	    || (IF_TYPE_QUAD == spi->write->iftype)
	    || (IF_TYPE_QIO == spi->write->iftype))
		return 1;

	return 0;
}

/*****************************************************************************/
/*
	enable QE bit if QUAD read write is supported by SPI
*/
static int spi_general_qe_enable(struct hifmc_spi *spi)
{
	unsigned char status, config, op;
	unsigned int reg;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	op = spi_is_quad(spi);

	FMC_PR(QE_DBG, "\t*-Start SPI Nor %s Quad.\n", str[op]);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	FMC_PR(QE_DBG, "\t|-Read Config Register[%#x]%#x\n", SPI_CMD_RDCR,
			config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op) {
		FMC_PR(QE_DBG, "\t* Quad was %sd, config:%#x\n", str[op],
				config);
		return op;
	}

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

	reg = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, reg);
	FMC_PR(QE_DBG, "\t|-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(QE_DBG, "\t|-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN + SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, reg);
	FMC_PR(QE_DBG, "\t|-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(QE_DBG, "\t|-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	if (SPI_NOR_GET_QE_BY_CR(config) == op)
		FMC_PR(QE_DBG, "\t|-%s Quad success, config: %#x\n", str[op],
				config);
	else
		DB_MSG("Error: %s Quad failed! reg: %#x\n", str[op], config);

	FMC_PR(QE_DBG, "\t* End SPI Nor %s Quad.\n", str[op]);

	return op;
}

/*****************************************************************************/
/*
  some chip don't QUAD enable
*/
static int spi_do_not_qe_enable(struct hifmc_spi *spi)
{
	return 0;
}

