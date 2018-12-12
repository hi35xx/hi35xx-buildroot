/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
 *
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
	unsigned int deadline = 1 << 20;

	do {
		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
		if (!(status & SPI_NOR_SR_WIP_MASK))
			return 0;

		udelay(1);

	} while (deadline--);

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
		printf("\n");
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

	spi->driver->wait_ready(spi);

	reg = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	if (reg & STATUS_WEL_MASK)
		FMC_PR(WE_DBG, "\t    Write Enable success.reg: %#x\n", reg);
	else {
		DB_MSG("Error: Write Enable failed! status: %#x\n", reg);
		return status;
	}

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

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
	FMC_PR(AC_DBG, "\t  Read SR-3[%#x]:%#x\n", SPI_CMD_RDSR3,
			status);
	if (SPI_NOR_GET_4BYTE_BY_CR(status) != enable) {
		DB_MSG("Error: %s 4-byte failed! SR3:%#x\n",
				str[enable], status);
		return status;
	}

	FMC_PR(AC_DBG, "\t  %s 4-byte success, SR3:%#x\n", str[enable], status);
	FMC_PR(AC_DBG, "\t* End SPI Nor flash %s 4-byte mode.\n", str[enable]);

	return 0;
}

/****************************************************************************/
/*
	judge whether SPI Nor support QUAD read write or not
*/
int spi_is_quad(struct hifmc_spi *spi)
{
	char *const if_str[] = {"STD", "DUAL", "DIO", "QUAD", "QIO", "DTR"};

	FMC_PR(QE_DBG, "\t\t|*-SPI read iftype: %s write iftype: %s\n",
		if_str[spi->read->iftype], if_str[spi->write->iftype]);

	if ((IF_TYPE_QUAD == spi->read->iftype)
	    || (IF_TYPE_QIO == spi->read->iftype)
	    || (IF_TYPE_QUAD == spi->write->iftype)
	    || (IF_TYPE_QIO == spi->write->iftype)
#ifdef CONFIG_DTR_MODE_SUPPORT
	    || (IF_TYPE_DTR == spi->read->iftype)
#endif
	    )
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

/*****************************************************************************/
/*
 * some chip set the mux HOLD#/RESET#/IO3 pin to RESET#, as it is HOLD# default.
 */
static void spi_nor_reset_pin_enable(struct hifmc_spi *spi, int enable)
{
	unsigned char config;
	unsigned int regval;
	const char *str[] = {"HOLD#", "RESET#"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	config = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
	FMC_PR(RST_DB, "\t|-Read SR-3[%#x], val: %#x\n",
			SPI_CMD_RDSR3, config);

	if (SPI_NOR_GET_RST_HOLD_BY_CR(config) == enable) {
		FMC_PR(RST_DB, " Device has worked on %s.\n", str[enable]);
		return;
	}

	FMC_PR(RST_DB, " Start to enable %s function.\n", str[enable]);
	spi->driver->write_enable(spi);

	if (enable)
		config = SPI_NOR_SET_RST_BY_CR(config);
	else
		config = SPI_NOR_SET_HOLD_BY_CR(config);

	writeb(config, host->iobase);
	FMC_PR(RST_DB, "\t|-Write IO[%#x]%#x\n", (uint)host->iobase,
			*(unsigned char *)host->iobase);

	regval = FMC_CMD_CMD1(SPI_CMD_WRSR3);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(RST_DB, "\t|-Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(RST_DB, "\t|-Set OP_CFG[%#x]%#x\n",
			FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(RST_DB, "\t|-Set DATA_NUM[%#x]%#x\n",
			FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(RST_DB, "\t|-Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
	FMC_PR(RST_DB, "\t|-Read SR-3[%#x], val: %#x\n",
			SPI_CMD_RDSR3, config);
	if (SPI_NOR_GET_RST_HOLD_BY_CR(config) == enable)
		FMC_PR(RST_DB, "\t|- Set the MUX pin to RESET# success!\n");
	else
		FMC_PR(RST_DB, "\t|- The MUX pin works on HOLD# or DNU!\n");
}
