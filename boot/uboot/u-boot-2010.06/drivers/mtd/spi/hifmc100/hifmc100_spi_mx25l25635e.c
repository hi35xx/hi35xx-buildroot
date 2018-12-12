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

#ifdef CONFIG_DTR_MODE_SUPPORT
/*****************************************************************************/
int spi_mxic_output_driver_strength_set(struct hifmc_spi *spi, int dtr_en)
{
	unsigned char status, config;
	unsigned short reg;
	unsigned short val = 0;
	unsigned int ix, regval;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;
	struct spi_op *read = spi->read;
/************************************************************************/
/* DC[1:0]	|  Numbers of Dummy clock cycles|  Quad IO DTR Read     */
/*   00(default)|		6		|	54		*/
/*   01		|		6		|	54		*/
/*   10		|		8		|	70/80R		*/
/*   11		|		10		|	84/100R		*/
/************************************************************************/
	unsigned int str_dummy[] = {
		DTR_DUMMY_CYCLES_6,      DTR_RDCR_DC_MASK(0),
		DTR_DUMMY_CYCLES_6,      DTR_RDCR_DC_MASK(1),
		DTR_DUMMY_CYCLES_8,      DTR_RDCR_DC_MASK(2),
		DTR_DUMMY_CYCLES_10,     DTR_RDCR_DC_MASK(3),
		0,      0,
	};

	/* get the RDCR and RDSR */
	spi->driver->wait_ready(spi);

	/* setting the DC value to match high system clock */
	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR_MX);
	FMC_PR(DTR_DB, "Get Config Register[%#x]\n", config);

	/* check the QE value */
	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(DTR_DB, "Get Status Register[%#x]\n", status);

	reg = ((unsigned short)config << SPI_NOR_CR_SHIFT) | status;

	if (dtr_en == ENABLE) {
		/* setting DC value */
		FMC_PR(DTR_DB, "Get the dummy value[%#x]\n", read->dummy);
		for (ix = 0; str_dummy[ix]; ix += 2) {
			if (read->dummy < str_dummy[ix])
				break;
			val = (unsigned short)str_dummy[ix + 1];
		}
	} else
		val = DTR_RDCR_DC_MASK(0);

	reg = DTR_RDCR_DC_BIT_CLR(reg) | (val << DTR_RDSR_DC_SHIFT);

	spi->driver->write_enable(spi);
	writew(reg, host->iobase);
	FMC_PR(DTR_DB, "Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned short *)host->iobase);
	regval = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(DTR_DB, " Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(DTR_DB, " Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN + SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(DTR_DB, " Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(DTR_DB, " Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR_MX);
	if ((config >> DTR_RDCR_DC_SHIFT) != (unsigned char)val) {
		printf("* Set DC dummy fail.\n");
		return -1;
	}
	return 0;
}

unsigned int spi_mxic_check_spi_dtr_support(struct hifmc_spi *spi)
{
	unsigned int regval, rd_sfdp_dummy = 1, sfdp_addrcycle = 3;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	/* get the RDCR and RDSR */
	spi->driver->wait_ready(spi);

	/* Read the Serial Flash Discoverable Parameter (SFDP) */
	hifmc_write(host, FMC_CMD, SPI_CMD_RD_SFDP);
	FMC_PR(DTR_DB, "\t   Set CMD[%#x]%#x\n", FMC_CMD, SPI_CMD_RD_SFDP);

	regval = OP_CFG_FM_CS(spi->chipselect)
		| OP_CFG_ADDR_NUM(sfdp_addrcycle)
		| OP_CFG_DUMMY_NUM(rd_sfdp_dummy);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(DTR_DB, "\t\t   Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SFDP_BUF_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(DTR_DB, "\t   Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_DUMMY_EN(ENABLE)
		| FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_READ_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(DTR_DB, "\t   Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	regval = readb((char *)host->iobase + SFDP_DTR_BYTE_SHIFT);
	FMC_PR(DTR_DB, "\t the dtr_mode_support is: %#x\n", regval);

	/* get the DTR mode support bit */
	spi->dtr_mode_support = (regval >> SFDP_DTR_BIT_SHIFT)
				& SFDP_DTR_BIT_MASK;

	return spi->dtr_mode_support;
}
#endif /* CONFIG_DTR_MODE_SUPPORT */
