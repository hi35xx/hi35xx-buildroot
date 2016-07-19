/*****************************************************************************
 *    Copyright (c) 2014 by Hisilicon
 *    All rights reserved.
 *****************************************************************************/

#include "hisnfc100_os.h"
#include "hisnfc100.h"

#define DEBUG_SPI_NAND_DRV 0

/*****************************************************************************/
/*
  Send set/get features command to SPI Nand flash
*/
void spi_feature_op(struct hisnfc_host *host, int op, int addr, unsigned *val)
{
	unsigned regval = 0;
	struct nand_chip *chip = host->chip;

	hisfc_write(host, HISNFC100_INT_CLR, HISNFC100_INT_CLR_OP_DONE);

	hisfc_write(host, HISNFC100_OPCODE,
			(op ? SPI_CMD_SET_FEATURE : SPI_CMD_GET_FEATURES));
	hisfc_write(host, HISNFC100_OP_ADDR, addr);

	hisfc_write(host, HISNFC100_OP_CFG, HISNFC100_OP_CFG_DIR_TRANS_ENABLE);

	regval = (HISNFC100_OP_SEL_CS(host->cmd_option.chipselect)
		| HISNFC100_OP_ADDR_NUM(FEATURES_OP_ADDR_NUM)
		| HISNFC100_OP_OPCODE_EN(ENABLE)
		| HISNFC100_OP_ADDR_EN(ENABLE)
		| HISNFC100_OP_START);

	if (SET_OP == op) {
		hisfc_write(host, HISNFC100_DATA_NUM, FEATURES_DATE_NUM);
		regval |= HISNFC100_OP_DATE_WRITE_EN(ENABLE);
		writeb(*val, chip->IO_ADDR_W);
		if (DEBUG_SPI_NAND_DRV)
			pr_info("hisnfc: set feature [%#x]==>[%#x]\n", addr,
				*val);
	} else
		regval |=  HISNFC100_OP_STATUS_READ_EN(ENABLE);

	hisfc_write(host, HISNFC100_OP, regval);

	HISNFC100_CMD_WAIT_CPU_FINISH(host);

	if (GET_OP == op) {
		*val = hisfc_read(host, HISNFC100_STATUS);
		if (DEBUG_SPI_NAND_DRV && (addr != STATUS_ADDR))
			pr_info("hisnfc: get feature [%#x]<==[%#x]\n", addr,
				*val);
	}
}

/*****************************************************************************/
/*
  Read status[C0H]:[0]bit OIP, judge whether the device is busy or not
*/
static int spi_general_wait_ready(struct hisnfc_op *spi)
{
	unsigned regval = 0;
	unsigned deadline = 0;
	struct hisnfc_host *host = (struct hisnfc_host *)spi->host;

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);
		if (!(regval & STATUS_OIP_MASK)) {
			if ((host->cmd_option.last_cmd == NAND_CMD_ERASE2)
			    && (regval & STATUS_E_FAIL_MASK))
				return regval;
			if ((host->cmd_option.last_cmd == NAND_CMD_PAGEPROG)
			    && (regval & STATUS_P_FAIL_MASK))
				return regval;
			return 0;
		}
		udelay(1);
	} while (deadline++ < (40 << 20));

	pr_info("hisnfc: wait ready timeout.\n");

	return 1;
}

/*****************************************************************************/
/*
  Send write enable command to SPI Nand, status[C0H]:[2]bit WEL must be set 1
*/
static int spi_general_write_enable(struct hisnfc_op *spi)
{
	unsigned val;
	struct hisnfc_host *host = (struct hisnfc_host *)spi->host;

	hisfc_write(host, HISNFC100_INT_CLR, HISNFC100_INT_CLR_OP_DONE);

	val = hisfc_read(host, HISNFC100_GLOBAL_CFG);
	if (val & HISNFC100_GLOBAL_CFG_WP_ENABLE) {
		val &= ~HISNFC100_GLOBAL_CFG_WP_ENABLE;
		hisfc_write(host, HISNFC100_GLOBAL_CFG, val);
	}

	hisfc_write(host, HISNFC100_OPCODE, SPI_CMD_WREN);

	hisfc_write(host, HISNFC100_OP_CFG,
			HISNFC100_OP_CFG_DIR_TRANS_ENABLE);

	hisfc_write(host, HISNFC100_OP,
			HISNFC100_OP_SEL_CS(host->cmd_option.chipselect)
			| HISNFC100_OP_OPCODE_EN(ENABLE)
			| HISNFC100_OP_START);

	HISNFC100_CMD_WAIT_CPU_FINISH(host);

	spi_feature_op(host, GET_OP, STATUS_ADDR, &val);
	if (!(val & STATUS_WEL_MASK)) {
		pr_info("hisnfc: write enable failed! val[%#x]\n", val);
		return 1;
	}

	return 0;
}

/*****************************************************************************/
/*
  judge whether SPI Nand support QUAD read/write or not
*/
static int spi_is_quad(struct hisnfc_op *spi)
{
	if (DEBUG_SPI_NAND_DRV) {
		pr_info("hisnfc: SPI write iftype:%d\n", spi->write->iftype);
		pr_info("hisnfc: SPI read iftype:%d\n", spi->read->iftype);
	}

	if (HISNFC100_IFCYCLE_QUAD == spi->write->iftype
	    || HISNFC100_IFCYCLE_QUAD == spi->read->iftype
	    || HISNFC100_IFCYCLE_QUAD_ADDR == spi->read->iftype)
		return 1;

	return 0;
}

/*****************************************************************************/
/*
  Send set features command to SPI Nand, feature[B0H]:[0]bit QE would be set
*/
static int spi_general_qe_enable(struct hisnfc_op *spi)
{
	unsigned val, op;
	const char *str[] = {"Disable", "Enable"};
	struct hisnfc_host *host = (struct hisnfc_host *)spi->host;

	if (DEBUG_SPI_NAND_DRV)
		pr_info("* SPI Quad-Enable start.\n");

	if (spi_is_quad(spi))
		op = ENABLE;
	else
		op = DISABLE;

	if (DEBUG_SPI_NAND_DRV)
		pr_info("  Read Quad status.\n");
	spi_feature_op(host, GET_OP, FEATURE_ADDR, &val);
	if ((val & FEATURE_QE_ENABLE) == op) {
		if (DEBUG_SPI_NAND_DRV)
			pr_info("* Quad is %s!\n", str[op]);
		return 0;
	}

	if (op == ENABLE)
		val |= FEATURE_QE_ENABLE;
	else
		val &= ~FEATURE_QE_ENABLE;
	if (DEBUG_SPI_NAND_DRV)
		pr_info("  %s Quad\n", str[op]);
	spi_feature_op(host, SET_OP, FEATURE_ADDR, &val);

	spi->driver->wait_ready(spi);

	spi_feature_op(host, GET_OP, FEATURE_ADDR, &val);
	if ((val & FEATURE_QE_ENABLE) != op) {
		pr_info("hisnfc: %s Quad failed! val[%#x]\n", str[op], val);
		return 1;
	} else if (DEBUG_SPI_NAND_DRV)
		pr_info("*  %s Quad succeed!\n", str[op]);

	return 0;
}

