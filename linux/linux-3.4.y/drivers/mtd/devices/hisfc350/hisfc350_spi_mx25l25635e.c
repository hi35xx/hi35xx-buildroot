/*****************************************************************************
*    Copyright (c) 2009-2014 by Hisilicon.
*    All rights reserved.
 *****************************************************************************/

#include <linux/io.h>
#include <linux/mtd/mtd.h>
#include <linux/errno.h>

#include "../spi_ids.h"
#include "hisfc350.h"

/* MXIC QE(bit) include in Status Register */
#define MX_SPI_NOR_SR_QE_SHIFT	6
#define MX_SPI_NOR_SR_QE_MASK	(1 << MX_SPI_NOR_SR_QE_SHIFT)

/*****************************************************************************/
/*
   enable QE bit if QUAD read write is supported by SPI
*/
static int spi_mx25l25635e_qe_enable(struct hisfc_spi *spi)
{
	unsigned char status, op;
	unsigned int reg;
	const char *str[] = {"Disable", "Enable"};
	struct hisfc_host *host = (struct hisfc_host *)spi->host;

	op = hisfc350_is_quad(spi);

	if (DEBUG_SPI_QE)
		printk(KERN_INFO "* Start SPI Nor %s Quad.\n", str[op]);

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	if (DEBUG_SPI_QE)
		printk(KERN_INFO "  Read status %#x, val[%#x]\n", SPI_CMD_RDSR,
				status);
	if (((status & MX_SPI_NOR_SR_QE_MASK) >> MX_SPI_NOR_SR_QE_SHIFT)
			== op) {
		if (DEBUG_SPI_QE)
			printk(KERN_INFO "* Quad was %sd!\n", str[op]);
		return op;
	}

	spi->driver->write_enable(spi);

	if (op)
		status |= MX_SPI_NOR_SR_QE_MASK;
	else
		status &= ~MX_SPI_NOR_SR_QE_MASK;
	hisfc_write(host, HISFC350_CMD_DATABUF0, status);
	if (DEBUG_SPI_QE)
		printk(KERN_INFO "  Set DATA[%#x]%#x\n", HISFC350_CMD_DATABUF0,
				status);

	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_WRSR);
	if (DEBUG_SPI_QE)
		printk(KERN_INFO "  Set INS[%#x]%#x\n", HISFC350_CMD_INS,
				SPI_CMD_WRSR);

	reg = HISFC350_CMD_CONFIG_DATA_CNT(SPI_NOR_SR_LEN)
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_START;
	hisfc_write(host, HISFC350_CMD_CONFIG, reg);
	if (DEBUG_SPI_QE)
		printk(KERN_INFO "  Set CONFIG[%#x]%#x\n", HISFC350_CMD_CONFIG,
				reg);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	if (DEBUG_SPI_QE) {
		spi->driver->wait_ready(spi);

		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
		if (((status & MX_SPI_NOR_SR_QE_MASK) >> MX_SPI_NOR_SR_QE_SHIFT)
				== op)
			printk(KERN_INFO "* SPI %s Quad succeed.\n", str[op]);
		else
			DBG_MSG("%s Quad failed! [%#x]\n", str[op], status);
	}

	return op;
}

