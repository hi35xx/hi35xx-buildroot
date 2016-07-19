/******************************************************************************
 *    SPI NAND Flash Controller v100 Device Driver
 *    Copyright (c) 2014 Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By hisilicon
 *
******************************************************************************/

#include "hisnfc100_os.h"
#include "hisnfc100_spi_ids.h"
#include "hisnfc100.h"

#ifdef CONFIG_ARCH_HI3516A
	#include "hisnfc100_hi3516a.c"
#endif

#define DEBUG_ERASE	0
#define DEBUG_WRITE	0
#define DEBUG_READ	0

/*****************************************************************************/
static void hisnfc100_send_cmd_pageprog(struct hisnfc_host *host)
{
	unsigned char pages_per_block_shift;
	unsigned val, block_num, block_num_h, page_num;
	struct hisnfc_op *spi = host->spi;
	struct nand_chip *chip = host->chip;
#ifdef HISNFC100_SUPPORT_REG_WRITE
	const char *op_type = "reg";
#else
	const char *op_type = "dma";
#endif

	if (DEBUG_WRITE)
		pr_info("* Enter %s page program!\n", op_type);

	val = spi->driver->wait_ready(spi);
	if (val) {
		pr_info("%s: %s page program wait ready fail! status[%#x]\n",
			__func__, op_type, val);
		return;
	}

	if (spi->driver->write_enable(spi)) {
		pr_info("%s %s page program write enable failed!\n", __func__,
			op_type);
		return;
	}

	host->set_system_clock(spi->write, ENABLE);

	val = HISNFC100_INT_CLR_ALL;
	hisfc_write(host, HISNFC100_INT_CLR, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG INT_CLR[0x14]%#x\n", val);

	val = HISNFC100_OP_CFG_MEM_IF_TYPE(spi->write->iftype);
	hisfc_write(host, HISNFC100_OP_CFG, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG OP_CFG[0x28]%#x\n", val);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;
	val = HISNFC100_ADDRH_SET(block_num_h);
	hisfc_write(host, HISNFC100_ADDRH, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG ADDRH[0x2c]%#x\n", val);

	page_num = host->addr_value[1] - (block_num << pages_per_block_shift);
	val = ((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
	     | ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT);
	hisfc_write(host, HISNFC100_ADDRL, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG ADDRL[0x30]%#x\n", val);

#ifndef HISNFC100_SUPPORT_REG_WRITE
	val = HISNFC100_DMA_CTRL_ALL_ENABLE;
	hisfc_write(host, HISNFC100_DMA_CTRL, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG DMA_CTRL[0x3c]%#x\n", val);

	val = host->dma_buffer;
	hisfc_write(host, HISNFC100_DMA_SADDR_D, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG DMA_SADDR_D[0x40]%#x\n", val);

	val = host->dma_oob;
	hisfc_write(host, HISNFC100_DMA_SADDR_OOB, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG DMA_SADDR_OOB[%#x]%#x\n",
			HISNFC100_DMA_SADDR_OOB, val);
#endif

	val = HISNFC100_OP_CTRL_WR_OPCODE(spi->write->cmd)
		| HISNFC100_OP_CTRL_CS_OP(host->cmd_option.chipselect)
#ifdef HISNFC100_SUPPORT_REG_WRITE
		| HISNFC100_OP_CTRL_OP_TYPE(OP_TYPE_REG)
#else
		| HISNFC100_OP_CTRL_OP_TYPE(OP_TYPE_DMA)
#endif
		| HISNFC100_OP_CTRL_RW_OP(RW_OP_WRITE)
		| HISNFC100_OP_CTRL_OP_READY;
	hisfc_write(host, HISNFC100_OP_CTRL, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG OP_CTRL[0x34]%#x\n", val);

	HISNFC100_DMA_WAIT_INT_FINISH(host);

	if (DEBUG_WRITE) {
		val = spi->driver->wait_ready(spi);
		if (val & STATUS_P_FAIL_MASK)
			pr_info("hisnfc100: %s page program failed!" \
				" status[%#x]\n", op_type, val);
	}

	if (DEBUG_WRITE)
		pr_info("* End %s page program!\n", op_type);
}

/*****************************************************************************/
static void hisnfc100_send_cmd_readstart(struct hisnfc_host *host)
{
	unsigned char pages_per_block_shift, only_oob = 0;
	unsigned short wrap = 0;
	unsigned val, block_num, block_num_h, page_num, addr_of = 0;
	struct hisnfc_op *spi = host->spi;
	struct nand_chip *chip = host->chip;
#ifdef HISNFC100_SUPPORT_REG_READ
	char *op_type = "reg";
#else
	char *op_type = "dma";
#endif

	if (DEBUG_READ)
		pr_info("* Enter %s page read start!\n", op_type);

	if ((host->addr_value[0] == host->cache_addr_value[0])
		&& (host->addr_value[1] == host->cache_addr_value[1])) {
		if (DEBUG_READ)
			pr_info("* %s page read cache hit! addr1[%#x], " \
				"addr0[%#x]\n", op_type, host->addr_value[1],
				host->addr_value[0]);
		return;
	}

	val = spi->driver->wait_ready(spi);
	if (val) {
		pr_info("%s: %s read wait ready fail! status[%#x]\n", __func__,
			op_type, val);
		return;
	}

	host->set_system_clock(spi->read, ENABLE);

	val = HISNFC100_INT_CLR_ALL;
	hisfc_write(host, HISNFC100_INT_CLR, val);
	if (DEBUG_READ)
		pr_info("  Set REG INT_CLR[0x14]%#x\n", val);

	if (host->cmd_option.last_cmd == NAND_CMD_READOOB) {
		only_oob = 1;
		host->cmd_option.op_config =
				HISNFC100_OP_CFG_RD_OP_SEL(RD_OP_READ_OOB);
	} else
		host->cmd_option.op_config =
				HISNFC100_OP_CFG_RD_OP_SEL(RD_OP_READ_PAGE);

	val = host->cmd_option.op_config
		| HISNFC100_OP_CFG_MEM_IF_TYPE(spi->read->iftype)
		| HISNFC100_OP_CFG_DUMMY_ADDR_NUM(spi->read->dummy);
	hisfc_write(host, HISNFC100_OP_CFG, val);
	if (DEBUG_READ)
		pr_info("  Set REG OP_CFG[0x28]%#x\n", val);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;

	val = HISNFC100_ADDRH_SET(block_num_h);
	hisfc_write(host, HISNFC100_ADDRH, val);
	if (DEBUG_READ)
		pr_info("  Set REG ADDRH[0x2c]%#x\n", val);

	page_num = host->addr_value[1] - (block_num << pages_per_block_shift);
	if (only_oob)
		switch (host->ecctype) {
		case NAND_ECC_8BIT:
			addr_of = REG_CNT_ECC_8BIT_OFFSET;
			break;
		case NAND_ECC_16BIT:
			addr_of = REG_CNT_ECC_16BIT_OFFSET;
			break;
		case NAND_ECC_24BIT:
			addr_of = REG_CNT_ECC_24BIT_OFFSET;
			break;
		case NAND_ECC_0BIT:
		default:
			break;
		}

	val = (((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
		| ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT)
		| ((wrap & REG_CNT_WRAP_MASK) << REG_CNT_WRAP_SHIFT)
		| (addr_of & REG_CNT_ECC_OFFSET_MASK));
	hisfc_write(host, HISNFC100_ADDRL, val);
	if (DEBUG_READ)
		pr_info("  Set REG ADDRL[0x30]%#x\n", val);

#ifndef HISNFC100_SUPPORT_REG_READ
	val = HISNFC100_DMA_CTRL_ALL_ENABLE;
	hisfc_write(host, HISNFC100_DMA_CTRL, val);
	if (DEBUG_READ)
		pr_info("  Set REG DMA_CTRL[0x3c]%#x\n", val);

	val = host->dma_buffer;
	hisfc_write(host, HISNFC100_DMA_SADDR_D, val);
	if (DEBUG_READ)
		pr_info("  Set REG DMA_SADDR_D[0x40]%#x\n", val);

	val = host->dma_oob;
	hisfc_write(host, HISNFC100_DMA_SADDR_OOB, val);
	if (DEBUG_READ)
		pr_info("  Set REG DMA_SADDR_OOB[%#x]%#x\n",
			HISNFC100_DMA_SADDR_OOB, val);
#endif

	val = HISNFC100_OP_CTRL_RD_OPCODE(spi->read->cmd)
		| HISNFC100_OP_CTRL_CS_OP(host->cmd_option.chipselect)
#ifdef HISNFC100_SUPPORT_REG_READ
		| HISNFC100_OP_CTRL_OP_TYPE(OP_TYPE_REG)
#else
		| HISNFC100_OP_CTRL_OP_TYPE(OP_TYPE_DMA)
#endif
		| HISNFC100_OP_CTRL_RW_OP(RW_OP_READ)
		| HISNFC100_OP_CTRL_OP_READY;
	hisfc_write(host, HISNFC100_OP_CTRL, val);
	if (DEBUG_READ)
		pr_info("  Set REG OP_CTRL[0x34]%#x\n", val);

	HISNFC100_DMA_WAIT_INT_FINISH(host);

	host->cache_addr_value[0] = host->addr_value[0];
	host->cache_addr_value[1] = host->addr_value[1];

	if (DEBUG_READ)
		pr_info("* End %s page read start!\n", op_type);
}

/*****************************************************************************/
static void hisnfc100_send_cmd_erase(struct hisnfc_host *host)
{
	unsigned val;
	struct hisnfc_op *spi = host->spi;

	if (DEBUG_ERASE)
		pr_info("* Enter send cmd erase!\n");

	val = spi->driver->wait_ready(spi);
	if (val) {
		pr_info("hisnfc: erase wait ready fail! status[%#x]\n", val);
		return;
	}

	if (spi->driver->write_enable(spi)) {
		pr_info("%s erase write enable failed!\n", __func__);
		return;
	}

	if (DEBUG_ERASE) {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &val);
		pr_info("  Get feature addr[0xC0], val[%#x]\n", val);
	}

	host->set_system_clock(spi->erase, ENABLE);

	val = HISNFC100_INT_CLR_ALL;
	hisfc_write(host, HISNFC100_INT_CLR, val);
	if (DEBUG_ERASE)
		pr_info("  Set REG INT_CLR[0x14]%#x\n", val);

	val = spi->erase->cmd;
	hisfc_write(host, HISNFC100_OPCODE, val);
	if (DEBUG_ERASE)
		pr_info("  Set REG OPCODE[0x18]%#x\n", val);

	val = HISNFC100_OP_ADDRH_BLOCK_MASK(host->addr_value[1])
		| HISNFC100_OP_ADDRL_BLOCK_MASK(host->addr_value[0]);
	hisfc_write(host, HISNFC100_OP_ADDR, val);
	if (DEBUG_ERASE)
		pr_info("  Set REG OP_ADDR[0x18]%#x\n", val);

	val = HISNFC100_OP_CFG_DIR_TRANS_ENABLE;
	hisfc_write(host, HISNFC100_OP_CFG, val);
	if (DEBUG_ERASE)
		pr_info("  Set REG OP_CFG[0x28]%#x\n", val);

	val = HISNFC100_OP_SEL_CS(host->cmd_option.chipselect)
		| HISNFC100_OP_ADDR_NUM(STD_OP_ADDR_NUM)
		| HISNFC100_OP_OPCODE_EN(ENABLE)
		| HISNFC100_OP_ADDR_EN(ENABLE)
		| HISNFC100_OP_START;
	hisfc_write(host, HISNFC100_OP, val);
	if (DEBUG_ERASE)
		pr_info("  Set REG OP[0x20]%#x\n", val);

	HISNFC100_CMD_WAIT_CPU_FINISH(host);

	if (DEBUG_ERASE) {
		val = spi->driver->wait_ready(spi);
		if (val & STATUS_E_FAIL_MASK)
			pr_info("hisnfc100: erase failed! status[%#x]\n", val);
	}

	if (DEBUG_ERASE)
		pr_info("* End send cmd erase!\n");
}

/*****************************************************************************/
static void hisnfc100_send_cmd_status(struct hisnfc_host *host)
{
	unsigned regval, addr = 0;

	if ((host->cmd_option.last_cmd == NAND_CMD_ERASE1)
	    || (host->cmd_option.last_cmd == NAND_CMD_PAGEPROG))
		addr = PROTECTION_ADDR;
	else
		addr = STATUS_ADDR;

	spi_feature_op(host, GET_OP, addr, &regval);

	if (DEBUG_ERASE || DEBUG_WRITE)
		pr_info("hisnfc100: %s get %#x status[%#x]\n",
			((host->cmd_option.last_cmd == NAND_CMD_ERASE1)
			 ? "erase" : "write"), addr, regval);
}

/*****************************************************************************/
static void hisnfc100_send_cmd_readid(struct hisnfc_host *host)
{
	hisfc_write(host, HISNFC100_INT_CLR, HISNFC100_INT_CLR_OP_DONE);
	hisfc_write(host, HISNFC100_OPCODE, SPI_CMD_RDID);
	hisfc_write(host, HISNFC100_OP_ADDR, READ_ID_ADDR);
	hisfc_write(host, HISNFC100_DATA_NUM,
			HISNFC100_DATA_NUM_CNT(MAX_ID_LEN));
	hisfc_write(host, HISNFC100_OP_CFG, HISNFC100_OP_CFG_DIR_TRANS_ENABLE);

	hisfc_write(host, HISNFC100_OP,
			HISNFC100_OP_SEL_CS(host->cmd_option.chipselect)
			| HISNFC100_OP_ADDR_NUM(READ_ID_ADDR_NUM)
			| HISNFC100_OP_OPCODE_EN(ENABLE)
			| HISNFC100_OP_ADDR_EN(ENABLE)
			| HISNFC100_OP_DATE_READ_EN(ENABLE)
			| HISNFC100_OP_START);

	HISNFC100_CMD_WAIT_CPU_FINISH(host);
}

/*****************************************************************************/
static void hisnfc100_send_cmd_reset(struct hisnfc_host *host)
{
	hisfc_write(host, HISNFC100_INT_CLR, HISNFC100_INT_CLR_OP_DONE);
	hisfc_write(host, HISNFC100_OPCODE, SPI_CMD_RESET);
	hisfc_write(host, HISNFC100_OP_CFG, HISNFC100_OP_CFG_DIR_TRANS_ENABLE);
	hisfc_write(host, HISNFC100_OP,
			HISNFC100_OP_SEL_CS(host->cmd_option.chipselect)
			| HISNFC100_OP_OPCODE_EN(ENABLE)
			| HISNFC100_OP_START);

	HISNFC100_CMD_WAIT_CPU_FINISH(host);
}

/*****************************************************************************/
static uint8_t hisnfc100_read_byte(struct mtd_info *mtd)
{
	unsigned char value = 0;
	struct nand_chip *chip = mtd->priv;
	struct hisnfc_host *host = chip->priv;

	if (host->cmd_option.last_cmd == NAND_CMD_READID) {
		value = readb(chip->IO_ADDR_R + host->offset);
		host->offset++;
		if (host->cmd_option.date_num == host->offset)
			host->cmd_option.last_cmd = 0;
		return value;
	}

	if ((host->cmd_option.last_cmd == NAND_CMD_ERASE1)
	    || (host->cmd_option.last_cmd == NAND_CMD_PAGEPROG)) {
		value = hisfc_read(host, HISNFC100_STATUS);
		if (ANY_BP_ENABLE(value))
			value &= ~NAND_STATUS_WP;
		else
			value |= NAND_STATUS_WP;

		host->cmd_option.last_cmd = 0;

		return value;
	}

	if (host->cmd_option.last_cmd == NAND_CMD_ERASE2) {
		value = hisfc_read(host, HISNFC100_STATUS);
		if (value & STATUS_E_FAIL_MASK)
			value |= NAND_STATUS_FAIL;
		return value;
	}

	if (host->cmd_option.command == NAND_CMD_STATUS) {
		value = hisfc_read(host, HISNFC100_STATUS);
		return value;
	}

	if (host->cmd_option.last_cmd == NAND_CMD_READOOB) {
		value  = readb(host->buffer + host->pagesize + host->offset);
		host->offset++;
		return value;
	}

	host->offset++;

	return readb(host->buffer + host->column + host->offset - 1);
}

/*****************************************************************************/
static u16 hisnfc100_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hisnfc_host *host = chip->priv;

	host->offset += 2;
	return readw(host->buffer + host->column + host->offset - 2);
}

/*****************************************************************************/
static void hisnfc100_write_buf(struct mtd_info *mtd,
	const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hisnfc_host *host = chip->priv;

#ifdef HISNFC100_SUPPORT_REG_WRITE
	if (buf == chip->oob_poi)
		memcpy((char *)host->iobase + host->pagesize, buf, len);
	else
		memcpy((char *)host->iobase, buf, len);
#else
	if (buf == chip->oob_poi)
		memcpy((char *)(host->buffer + host->pagesize), buf, len);
	else
		memcpy((char *)host->buffer, buf, len);
#endif
	return;
}

/*****************************************************************************/
static void hisnfc100_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hisnfc_host *host = chip->priv;

#ifdef HISNFC100_SUPPORT_REG_READ
	if (buf == chip->oob_poi)
		memcpy(buf, (char *)host->iobase + host->pagesize, len);
	else
		memcpy(buf, (char *)host->iobase, len);
#else
	if (buf == chip->oob_poi)
		memcpy(buf, (char *)(host->buffer + host->pagesize), len);
	else
		memcpy(buf, (char *)host->buffer, len);
#endif

	if (buf != chip->oob_poi) {
		u_int reg, ecc_step = host->pagesize >> 10;

		reg = hisfc_read(host, HISNFC100_ECC_ERR_NUM);
		while (ecc_step) {
			u_char err_num;

			err_num = GET_ECC_ERR_NUM(--ecc_step, reg);
			if (err_num == 0xff)
				mtd->ecc_stats.failed++;
			else
				mtd->ecc_stats.corrected += err_num;
		}
	}

	return;
}

/*****************************************************************************/
static void hisnfc100_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hisnfc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_HISNFC100_MAX_CHIP)
		DBG_BUG("invalid chipselect: %d\n", chipselect);

	host->cmd_option.chipselect = chipselect + 1;

	switch (chip->state) {
	case FL_ERASING:
		host->cmd_option.last_cmd = NAND_CMD_ERASE1;
		break;

	case FL_WRITING:
		host->cmd_option.last_cmd = NAND_CMD_PAGEPROG;
		break;

	default:
		break;
	}
}

/*****************************************************************************/
static void hisnfc100_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned ctrl)
{
	unsigned char cmd;
	int is_cache_invalid = 1;
	struct nand_chip *chip = mtd->priv;
	struct hisnfc_host *host = chip->priv;

	if (ctrl & NAND_ALE) {
		unsigned int addr_value = 0;
		unsigned int addr_offset = 0;

		if (ctrl & NAND_CTRL_CHANGE) {
			host->addr_cycle = 0x0;
			host->addr_value[0] = 0x0;
			host->addr_value[1] = 0x0;
		}
		addr_offset = host->addr_cycle << 3;

		if (host->addr_cycle >= HISNFC100_ADDR_CYCLE_MASK) {
			addr_offset = (host->addr_cycle -
					HISNFC100_ADDR_CYCLE_MASK) << 3;
			addr_value = 1;
		}

		host->addr_value[addr_value] |=
			((dat & 0xff) << addr_offset);

		host->addr_cycle++;
	}

	if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE)) {
		cmd = dat & 0xff;
		host->cmd_option.command = cmd;
		switch (cmd) {
		case NAND_CMD_PAGEPROG:
			host->offset = 0;
			host->send_cmd_pageprog(host);
			break;

		case NAND_CMD_READSTART:
			is_cache_invalid = 0;
			if (host->addr_value[0] == host->pagesize)
				host->cmd_option.last_cmd = NAND_CMD_READOOB;
			host->send_cmd_readstart(host);
			break;

		case NAND_CMD_ERASE2:
			host->cmd_option.last_cmd = cmd;
			host->send_cmd_erase(host);
			break;

		case NAND_CMD_READID:
			memset((unsigned char *)(chip->IO_ADDR_R), 0,
				MAX_ID_LEN);
			host->cmd_option.last_cmd = cmd;
			host->cmd_option.date_num = MAX_ID_LEN;
			host->send_cmd_readid(host);
			break;

		case NAND_CMD_STATUS:
			host->send_cmd_status(host);
			break;

		case NAND_CMD_SEQIN:
			break;

		case NAND_CMD_ERASE1:
			break;

		case NAND_CMD_READ0:
			host->cmd_option.last_cmd = cmd;
			break;

		case NAND_CMD_RESET:
			host->send_cmd_reset(host);
			break;

		default:
			break;
		}
	}

	if ((dat == NAND_CMD_NONE) && host->addr_cycle) {
		if (host->cmd_option.command == NAND_CMD_SEQIN
		    || host->cmd_option.command == NAND_CMD_READ0
		    || host->cmd_option.command == NAND_CMD_READID) {
			host->offset = 0x0;
			host->column = (host->addr_value[0] & 0xffff);
		}
	}

	if (is_cache_invalid) {
		host->cache_addr_value[0] = ~0;
		host->cache_addr_value[1] = ~0;
	}
}

/*****************************************************************************/
static int hisnfc100_dev_ready(struct mtd_info *mtd)
{
	unsigned regval;
	unsigned deadline = 0;
	struct nand_chip *chip = mtd->priv;
	struct hisnfc_host *host = chip->priv;

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);
		if (!(regval & STATUS_OIP_MASK))
			return 1;
		udelay(1);
	} while (deadline++ < (40 << 20));

	pr_info("Wait spi nand flash ready timeout.\n");

	return 0;
}

/*****************************************************************************/
/*
 * 'host->epm' only use the first oobfree[0] field, it looks very simple, But...
 */
static struct nand_ecclayout nand_ecc_default = {
	.oobfree = {{2, 30} }
};

/*****************************************************************************/
static struct nand_config_info hisnfc_spi_nand_config_table[] = {
	{NAND_PAGE_4K,	NAND_ECC_24BIT,	200,		&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_16BIT,	144,		&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_8BIT,	128/*88*/,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_0BIT,	32,		&nand_ecc_default},

	{NAND_PAGE_2K,	NAND_ECC_24BIT,	128/*116*/,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_16BIT,	88,		&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_8BIT,	64/*60*/,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_0BIT,	32,		&nand_ecc_default},

	{0, 0, 0, NULL},
};

/*****************************************************************************/
/* used the best correct arithmetic. */
struct nand_config_info *hisnfc100_get_best_ecc(struct mtd_info *mtd)
{
	struct nand_config_info *best = NULL;
	struct nand_config_info *info = hisnfc_spi_nand_config_table;

	for (; info->layout; info++) {
		if (nandpage_type2size(info->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < info->oobsize)
			continue;

		if (!best || (best->ecctype < info->ecctype))
			best = info;
	}

	if (!best)
		DBG_BUG(ERSTR_DRIVER "Driver does not support the pagesize" \
		"(%d) and oobsize(%d).\n", mtd->writesize, mtd->oobsize);

	return best;
}

/*****************************************************************************/
/* force the pagesize and ecctype */
struct nand_config_info *hisnfc100_force_ecc(struct mtd_info *mtd, int pagetype,
		int ecctype, char *cfgmsg, int allow_pagediv)
{
	int pagesize;
	struct nand_config_info *fit = NULL;
	struct nand_config_info *info = hisnfc_spi_nand_config_table;

	for (; info->layout; info++) {
		if (info->pagetype == pagetype && info->ecctype == ecctype) {
			fit = info;
			break;
		}
	}

	if (!fit) {
		DBG_BUG(ERSTR_DRIVER "Driver(%s mode) does not support this" \
			" Nand Flash pagesize:%s, ecctype:%s\n", cfgmsg,
			nand_page_name(pagetype), nand_ecc_name(ecctype));
		return NULL;
	}

	pagesize = nandpage_type2size(pagetype);
	if ((pagesize != mtd->writesize)
		&& (pagesize > mtd->writesize || !allow_pagediv)) {
		DBG_BUG(ERSTR_HARDWARE "Hardware (%s mode) configure pagesize" \
			" %d, but the Nand Flash pageszie is %d\n", cfgmsg,
			pagesize, mtd->writesize);
		return NULL;
	}

	if (fit->oobsize > mtd->oobsize) {
		DBG_BUG(ERSTR_HARDWARE "(%s mode) The Nand Flash offer space" \
			" area is %d bytes, but the controller request %d" \
			"bytes in ecc %s. Please make sure the hardware ECC " \
			"configuration is correct.", cfgmsg, mtd->oobsize,
			fit->oobsize, nand_ecc_name(ecctype));
		return NULL;
	}

	return fit;
}

/*****************************************************************************/
int hisnfc100_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex)
{
	char *start_type = "unknown";
	struct nand_config_info *best = NULL;
	struct hisnfc_host *host = chip->priv;
	unsigned reg_pagetype, reg_ecctype, pagetype, ecctype;

#ifdef CONFIG_HISNFC100_AUTO_PAGESIZE_ECC
	best = hisnfc100_get_best_ecc(mtd);
	start_type = "Auto";
#endif /* CONFIG_HISNFC100_AUTO_PAGESIZE_ECC */

#ifdef CONFIG_HISNFC100_HARDWARE_PAGESIZE_ECC
#  ifdef CONFIG_HISNFC100_AUTO_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HISNFC100_AUTO_PAGESIZE_ECC \
	and CONFIG_HISNFC100_HARDWARE_PAGESIZE_ECC at the same time
#  endif

	reg_pagetype = (host->NFC_CFG & PAGE_SIZE_MASK) >> PAGE_SIZE_SHIFT;
	switch (reg_pagetype) {
	case 0:
		pagetype = NAND_PAGE_2K;
		break;
	case 1:
		pagetype = NAND_PAGE_4K;
		break;
	default:
		pagetype = NAND_PAGE_2K;
	}

	reg_ecctype = (host->NFC_CFG & ECC_TYPE_MASK) >> ECC_TYPE_SHIFT;
	switch (reg_ecctype) {
	case 0x01:
		ecctype = NAND_ECC_8BIT;
		break;
	case 0x02:
		ecctype = NAND_ECC_16BIT;
		break;
	case 0x03:
		ecctype = NAND_ECC_24BIT;
		break;
	case 0:
	default:
		ecctype = NAND_ECC_8BIT;
	}
	best = hisnfc100_force_ecc(mtd, pagetype, ecctype,
			"hardware config", 0);
	start_type = "Hardware";

#endif /* CONFIG_HISNFC100_HARDWARE_PAGESIZE_ECC */

#ifdef CONFIG_HISNFC100_PAGESIZE_AUTO_ECC_NONE
#  ifdef CONFIG_HISNFC100_AUTO_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HISNFC100_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HISNFC100_AUTO_PAGESIZE_ECC at the same time
#  endif
#  ifdef CONFIG_HISNFC100_HARDWARE_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HISNFC100_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HISNFC100_HARDWARE_PAGESIZE_ECC at the same time
#  endif

	{
		int pagetype;

		switch (mtd->writesize) {
		case _2K:
			pagetype = NAND_PAGE_2K;
			break;
		case _4K:
			pagetype = NAND_PAGE_4K;
			break;
		default:
			pagetype = NAND_PAGE_2K;
			break;
		}
		best = hisnfc100_force_ecc(mtd, pagetype, NAND_ECC_0BIT,
					  "force config", 0);
		start_type = "AutoForce";
	}
#endif /* CONFIG_HISNFC100_PAGESIZE_AUTO_ECC_NONE */

	if (!best)
		DBG_BUG(ERSTR_HARDWARE
			"Please configure SPI Nand Flash pagesize and ecctype!\n");

	if (best->ecctype != NAND_ECC_0BIT)
		mtd->oobsize = best->oobsize;

	chip->ecc.layout = best->layout;

	host->ecctype  = best->ecctype;
	host->pagesize = nandpage_type2size(best->pagetype);
	host->oobsize  = mtd->oobsize;
	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);

	host->dma_oob = host->dma_buffer + host->pagesize;

	host->bbm = (unsigned char *)(host->buffer + host->pagesize
			+ HINFC_BAD_BLOCK_POS);

	host->epm = (unsigned short *)(host->buffer + host->pagesize
			+ chip->ecc.layout->oobfree[0].offset + 28);

	host->NFC_CFG |= (HISNFC100_CFG_ECC_TYPE(best->ecctype)
		| HISNFC100_CFG_PAGE_SIZE(best->pagetype)
		| HISNFC100_CFG_OP_MODE(OP_MODE_NORMAL));

	if (mtd->writesize > SPI_NAND_MAX_PAGESIZE
		|| mtd->oobsize > SPI_NAND_MAX_OOBSIZE) {
		DBG_BUG(ERSTR_DRIVER "Driver does not support this Nand " \
			"Flash. Please increase SPI_NAND_MAX_PAGESIZE and " \
			"SPI_NAND_MAX_OOBSIZE.\n");
	}

	if (mtd->writesize != host->pagesize) {
		unsigned int shift = 0;
		unsigned int writesize = mtd->writesize;
		while (writesize > host->pagesize) {
			writesize >>= 1;
			shift++;
		}
		chip->chipsize = chip->chipsize >> shift;
		mtd->erasesize = mtd->erasesize >> shift;
		mtd->writesize = host->pagesize;
		pr_info("Nand divide into 1/%u\n", (1 << shift));
	}

	flash_dev_ex->start_type = start_type;
	flash_dev_ex->ecctype = host->ecctype;

	return 0;
}

/*****************************************************************************/
void hisnfc100_nand_init(struct nand_chip *chip)
{
	chip->read_byte   = hisnfc100_read_byte;
	chip->read_word   = hisnfc100_read_word;
	chip->write_buf   = hisnfc100_write_buf;
	chip->read_buf    = hisnfc100_read_buf;

	chip->select_chip = hisnfc100_select_chip;

	chip->cmd_ctrl    = hisnfc100_cmd_ctrl;
	chip->dev_ready   = hisnfc100_dev_ready;

	chip->chip_delay  = HISNFC100_CHIP_DELAY;

	chip->options	  = NAND_NO_AUTOINCR | NAND_SKIP_BBTSCAN;

	chip->ecc.layout  = NULL;
	chip->ecc.mode    = NAND_ECC_NONE;
}

/*****************************************************************************/
int hisnfc100_host_init(struct hisnfc_host *host)
{
	unsigned regval;

	regval = hisfc_read(host, HISNFC100_CFG);
	if (((regval & DEVICE_TYPE_MASK) >> DEVICE_TYPE_SHIFT)
		!= DEVICE_TYPE_NAND_FLASH) {
		pr_info("%s: Device type(SPI nor flash) error.\n", __func__);
		return -ENXIO;
	}

	if ((regval & OP_MODE_MASK) == OP_MODE_BOOT)
		regval |= HISNFC100_CFG_OP_MODE(OP_MODE_NORMAL);

	if (!(regval & HISNFC100_CFG_DEVICE_INTERNAL_ECC_ENABLE))
		regval &= ~HISNFC100_CFG_DEVICE_INTERNAL_ECC_ENABLE;

	hisfc_write(host, HISNFC100_CFG, regval);

	host->NFC_CFG = regval;

	host->addr_cycle    = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->cache_addr_value[0] = ~0;
	host->cache_addr_value[1] = ~0;

	host->send_cmd_pageprog  = hisnfc100_send_cmd_pageprog;
	host->send_cmd_status    = hisnfc100_send_cmd_status;
	host->send_cmd_readstart = hisnfc100_send_cmd_readstart;
	host->send_cmd_erase     = hisnfc100_send_cmd_erase;
	host->send_cmd_readid    = hisnfc100_send_cmd_readid;
	host->send_cmd_reset     = hisnfc100_send_cmd_reset;
	host->set_system_clock = hisnfc100_set_system_clock;

	hisfc_write(host, HISNFC100_TIMING_CFG,
			HISNFC100_TIMING_CFG_TCSH(CS_HOLD_TIME)
			| HISNFC100_TIMING_CFG_TCSS(CS_SETUP_TIME)
			| HISNFC100_TIMING_CFG_TSHSL(CS_DESELECT_TIME));
	return 0;
}

