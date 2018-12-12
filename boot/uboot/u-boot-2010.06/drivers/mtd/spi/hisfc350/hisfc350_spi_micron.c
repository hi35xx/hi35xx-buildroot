/*
 * The SPI NOR Controller v350 Device Driver for hisilicon
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
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
static int spi_micron_entry_4addr(struct hisfc_spi *spi, int enable)
{
	unsigned char status;
	unsigned int reg;
	const char *str[] = {"Disable", "Enable"};
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN)
		return 0;

	status = spi_general_get_flash_register(spi, SPI_CMD_FLAG_SR_MICRON);
	if (DEBUG_SPI)
		printf("\t Read flag status register[%#x]:%#x\n",
				SPI_CMD_FLAG_SR_MICRON, status);

	if (SPI_NOR_GET_4BYTE_BY_FLAG_SR(status) == enable) {
		if (DEBUG_SPI)
			printf("\t* 4-byte was %sd, reg:%#x\n", str[enable],
					status);
		return 0;
	}

	spi->driver->write_enable(spi);

	if (enable)
		reg = SPI_CMD_EN4B;
	else
		reg = SPI_CMD_EX4B;

	hisfc_write(host, HISFC350_CMD_INS, reg);
	if (DEBUG_SPI)
		printf("\t  Set CMD[%#x]%#x\n", HISFC350_CMD_INS, reg);

	reg =  HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
	   	| HISFC350_CMD_CONFIG_START;
	hisfc_write(host, HISFC350_CMD_CONFIG, reg);

	if (DEBUG_SPI)
		printf("\t  Set OP_CFG[%#x]%#x\n", HISFC350_CMD_CONFIG, reg);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi,
			SPI_CMD_FLAG_SR_MICRON);
	if (DEBUG_SPI)
		printf("\t Read flag status register[%#x]:%#x\n",
				SPI_CMD_FLAG_SR_MICRON, status);
	if (SPI_NOR_GET_4BYTE_BY_FLAG_SR(status) != enable) {
		printf("Error: %s 4-byte failed! SR3:%#x\n",
				str[enable], status);
		return status;
	}

	if (DEBUG_SPI) {
		printf("\t  %s 4-byte success, SR3:%#x\n", str[enable], status);
		printf("\t* End SPI Nor flash %s 4-byte mode.\n", str[enable]);
	}
	return 0;
}
