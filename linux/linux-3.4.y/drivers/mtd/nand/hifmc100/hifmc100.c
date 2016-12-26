/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/mtd/nand.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <asm/setup.h>

#include "../../hifmc_common.h"
#include "../hinfc_common.h"
#include "hifmc100_os.h"
#include "hifmc100.h"

/*****************************************************************************/
void hifmc100_ecc0_switch(struct hifmc_host *host, unsigned char op)
{
	unsigned int config;
#if EC_DBG
	unsigned int cmp_cfg;

	config = hifmc_read(host, FMC_CFG);
	FMC_PR(EC_DBG, "\t *-Get CFG[%#x]%#x\n", FMC_CFG, config);

	if (op)
		cmp_cfg = host->fmc_cfg;
	else
		cmp_cfg = host->fmc_cfg_ecc0;

	if (cmp_cfg != config)
		DB_MSG("Warning: FMC config[%#x] is different.\n",
				cmp_cfg);
#endif

	if (op == ENABLE)
		config = host->fmc_cfg_ecc0;
	else if (op == DISABLE)
		config = host->fmc_cfg;
	else {
		DB_MSG("Error: Invalid opcode: %d\n", op);
		return;
	}

	hifmc_write(host, FMC_CFG, config);
	FMC_PR(EC_DBG, "\t *-Set CFG[%#x]%#x\n", FMC_CFG, config);
}

/*****************************************************************************/
static void hifmc100_send_cmd_pageprog(struct hifmc_host *host)
{
	unsigned char pages_per_block_shift;
	unsigned int reg, block_num, block_num_h, page_num;
	struct hifmc_spi *spi = host->spi;
	struct nand_chip *chip = host->chip;
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
	const char *op = "Reg";
#else
	const char *op = "Dma";
#endif

	if (WR_DBG)
		pr_info("\n");
	FMC_PR(WR_DBG, "*-Enter %s page program!\n", op);

	reg = spi->driver->wait_ready(spi);
	if (reg) {
		DB_MSG("Error: %s program wait ready failed! status: %#x\n",
				op, reg);
		goto end;
	}

	reg = spi->driver->write_enable(spi);
	if (reg) {
		DB_MSG("Error: %s program write enable failed! reg: %#x\n",
				op, reg);
		goto end;
	}

	host->set_system_clock(spi->write, ENABLE);

	reg = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, reg);
	FMC_PR(WR_DBG, "|-Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs)
		| OP_CFG_MEM_IF_TYPE(spi->write->iftype);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(WR_DBG, "|-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;
	reg = FMC_ADDRH_SET(block_num_h);
	hifmc_write(host, FMC_ADDRH, reg);
	FMC_PR(WR_DBG, "|-Set ADDRH[%#x]%#x\n", FMC_ADDRH, reg);

	page_num = host->addr_value[1] - (block_num << pages_per_block_shift);
	reg = ((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
	     | ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT);
	hifmc_write(host, FMC_ADDRL, reg);
	FMC_PR(WR_DBG, "|-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

#ifndef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
	reg = host->dma_buffer;
	hifmc_write(host, FMC_DMA_SADDR_D0, reg);
	FMC_PR(WR_DBG, "|-Set DMA_SADDR_D[0x40]%#x\n", reg);

	reg = host->dma_oob;
	hifmc_write(host, FMC_DMA_SADDR_OOB, reg);
	FMC_PR(WR_DBG, "|-Set DMA_SADDR_OOB[%#x]%#x\n", FMC_DMA_SADDR_OOB, reg);
#endif

	reg = OP_CTRL_WR_OPCODE(spi->write->cmd)
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
		| OP_CTRL_DMA_OP(OP_TYPE_REG)
#else
		| OP_CTRL_DMA_OP(OP_TYPE_DMA)
#endif
		| OP_CTRL_RW_OP(RW_OP_WRITE)
		| OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, reg);
	FMC_PR(WR_DBG, "|-Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, reg);

	FMC_DMA_WAIT_INT_FINISH(host);

end:
	FMC_PR(WR_DBG, "*-End %s page program!\n", op);
}

/*****************************************************************************/
static void hifmc100_send_cmd_readstart(struct hifmc_host *host)
{
	unsigned char pages_per_block_shift, only_oob = 0;
	unsigned short wrap = 0;
	unsigned int reg, block_num, block_num_h, page_num, addr_of = 0;
	struct hifmc_spi *spi = host->spi;
	struct nand_chip *chip = host->chip;
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_READ
	char *op = "Reg";
#else
	char *op = "Dma";
#endif

	if (RD_DBG)
		pr_info("\n");
	FMC_PR(RD_DBG, "\t*-Start %s page read\n", op);

	if ((host->addr_value[0] == host->cache_addr_value[0])
		&& (host->addr_value[1] == host->cache_addr_value[1])) {
		FMC_PR(RD_DBG, "\t*-%s read cache hit, addr[%#x %#x]\n",
			op, host->addr_value[1], host->addr_value[0]);
		return;
	}

	FMC_PR(RD_DBG, "\t|-Wait ready before %s page read\n", op);
	reg = spi->driver->wait_ready(spi);
	if (reg) {
		DB_MSG("Error: %s read wait ready fail! reg: %#x\n", op, reg);
		goto end;
	}

	host->set_system_clock(spi->read, ENABLE);

	reg = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, reg);
	FMC_PR(RD_DBG, "\t|-Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, reg);

	if (host->cmd_op.l_cmd == NAND_CMD_READOOB) {
		only_oob = 1;
		host->cmd_op.op_cfg = OP_CTRL_RD_OP_SEL(RD_OP_READ_OOB);
	} else
		host->cmd_op.op_cfg = OP_CTRL_RD_OP_SEL(RD_OP_READ_ALL_PAGE);

	reg = OP_CFG_FM_CS(host->cmd_op.cs)
		| OP_CFG_MEM_IF_TYPE(spi->read->iftype)
		| OP_CFG_DUMMY_NUM(spi->read->dummy);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(RD_DBG, "\t|-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;

	reg = FMC_ADDRH_SET(block_num_h);
	hifmc_write(host, FMC_ADDRH, reg);
	FMC_PR(RD_DBG, "\t|-Set ADDRH[%#x]%#x\n", FMC_ADDRH, reg);

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

	reg = ((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
		| ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT)
		| ((wrap & REG_CNT_WRAP_MASK) << REG_CNT_WRAP_SHIFT)
		| (addr_of & REG_CNT_ECC_OFFSET_MASK);
	hifmc_write(host, FMC_ADDRL, reg);
	FMC_PR(RD_DBG, "\t|-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

#ifndef HIFMC100_SPI_NAND_SUPPORT_REG_READ
	reg = host->dma_buffer;
	hifmc_write(host, FMC_DMA_SADDR_D0, reg);
	FMC_PR(RD_DBG, "\t|-Set DMA_SADDR_D0[%#x]%#x\n", FMC_DMA_SADDR_D0, reg);

	reg = host->dma_oob;
	hifmc_write(host, FMC_DMA_SADDR_OOB, reg);
	FMC_PR(RD_DBG, "\t|-Set DMA_SADDR_OOB[%#x]%#x\n", FMC_DMA_SADDR_OOB,
			reg);
#endif

	reg = OP_CTRL_RD_OPCODE(spi->read->cmd) | host->cmd_op.op_cfg
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_READ
		| OP_CTRL_DMA_OP(OP_TYPE_REG)
#else
		| OP_CTRL_DMA_OP(OP_TYPE_DMA)
#endif
		| OP_CTRL_RW_OP(RW_OP_READ) | OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, reg);
	FMC_PR(RD_DBG, "\t|-Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, reg);

	FMC_DMA_WAIT_INT_FINISH(host);

	host->cache_addr_value[0] = host->addr_value[0];
	host->cache_addr_value[1] = host->addr_value[1];

end:
	FMC_PR(RD_DBG, "\t*-End %s page read\n", op);
}

/*****************************************************************************/
static void hifmc100_send_cmd_erase(struct hifmc_host *host)
{
	unsigned int reg;
	struct hifmc_spi *spi = host->spi;

	if (ER_DBG)
		pr_info("\n");
	FMC_PR(ER_DBG, "\t*-Start send cmd erase!\n");

	reg = spi->driver->wait_ready(spi);
	FMC_PR(ER_DBG, "\t|-Erase wait ready, reg: %#x\n", reg);
	if (reg) {
		DB_MSG("Error: Erase wait ready fail! status: %#x\n", reg);
		goto end;
	}

	reg = spi->driver->write_enable(spi);
	if (reg) {
		DB_MSG("Error: Erase write enable failed! reg: %#x\n", reg);
		goto end;
	}

	host->set_system_clock(spi->erase, ENABLE);

	reg = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, reg);
	FMC_PR(ER_DBG, "\t|-Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, reg);

	reg = spi->erase->cmd;
	hifmc_write(host, FMC_CMD, FMC_CMD_CMD1(reg));
	FMC_PR(ER_DBG, "\t|-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = FMC_ADDRL_BLOCK_H_MASK(host->addr_value[1])
		| FMC_ADDRL_BLOCK_L_MASK(host->addr_value[0]);
	hifmc_write(host, FMC_ADDRL, reg);
	FMC_PR(ER_DBG, "\t|-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs)
		| OP_CFG_MEM_IF_TYPE(spi->erase->iftype)
		| OP_CFG_ADDR_NUM(STD_OP_ADDR_NUM)
		| OP_CFG_DUMMY_NUM(spi->erase->dummy);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(ER_DBG, "\t|-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(ER_DBG, "\t|-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

end:
	FMC_PR(ER_DBG, "\t*-End send cmd erase!\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_status(struct hifmc_host *host)
{
	unsigned char status, addr = STATUS_ADDR;
	struct hifmc_spi *spi = host->spi;

	if (host->cmd_op.l_cmd == NAND_CMD_GET_FEATURES)
		addr = PROTECT_ADDR;

	status = spi_nand_feature_op(spi, GET_OP, addr, 0);
	FMC_PR((ER_DBG || WR_DBG), "\t*-Get status[%#x]: %#x\n", addr, status);
}

/*****************************************************************************/
static void hifmc100_send_cmd_readid(struct hifmc_host *host)
{
	unsigned int reg;

	FMC_PR(BT_DBG, "\t|*-Start send cmd read ID\n");

	hifmc100_ecc0_switch(host, ENABLE);

	reg = FMC_CMD_CMD1(SPI_CMD_RDID);
	hifmc_write(host, FMC_CMD, reg);
	FMC_PR(BT_DBG, "\t||-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = READ_ID_ADDR;
	hifmc_write(host, FMC_ADDRL, reg);
	FMC_PR(BT_DBG, "\t||-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs)
		| OP_CFG_ADDR_NUM(READ_ID_ADDR_NUM);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(BT_DBG, "\t||-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_DATA_NUM_CNT(MAX_SPI_NAND_ID_LEN);
	hifmc_write(host, FMC_DATA_NUM, reg);
	FMC_PR(BT_DBG, "\t||-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_READ_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(BT_DBG, "\t||-Set OP[%#x]%#x\n", FMC_OP, reg);

	host->addr_cycle = 0x0;

	FMC_CMD_WAIT_CPU_FINISH(host);

	hifmc100_ecc0_switch(host, DISABLE);

	FMC_PR(BT_DBG, "\t|*-End read flash ID\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_reset(struct hifmc_host *host)
{
	unsigned int reg;

	FMC_PR(BT_DBG, "\t|*-Start send cmd reset\n");

	reg = FMC_CMD_CMD1(SPI_CMD_RESET);
	hifmc_write(host, FMC_CMD, reg);
	FMC_PR(BT_DBG, "\t||-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(BT_DBG, "\t||-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(BT_DBG, "\t||-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	FMC_PR(BT_DBG, "\t|*-End send cmd reset\n");
}

/*****************************************************************************/
static unsigned char hifmc100_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;
	unsigned char value, ret_val = 0;

	if (host->cmd_op.l_cmd == NAND_CMD_READID) {
		value = readb(host->iobase + host->offset);
		host->offset++;
		if (host->cmd_op.data_no == host->offset)
			host->cmd_op.l_cmd = 0;
		return value;
	}

	if (host->cmd_op.cmd == NAND_CMD_STATUS) {
		value = hifmc_read(host, FMC_STATUS);
		if (host->cmd_op.l_cmd == NAND_CMD_GET_FEATURES) {
			FMC_PR((ER_DBG || WR_DBG), "\t\tRead BP status:%#x\n",
					value);
			if (ANY_BP_ENABLE(value))
				ret_val |= NAND_STATUS_WP;

			host->cmd_op.l_cmd = NAND_CMD_STATUS;
		}

		if (!(value & STATUS_OIP_MASK))
			ret_val |= NAND_STATUS_READY;

		if (value & STATUS_E_FAIL_MASK) {
			FMC_PR(ER_DBG, "\t\tGet erase status: %#x\n", value);
			ret_val |= NAND_STATUS_FAIL;
		}

		if (value & STATUS_P_FAIL_MASK) {
			FMC_PR(WR_DBG, "\t\tGet write status: %#x\n", value);
			ret_val |= NAND_STATUS_FAIL;
		}

		return ret_val;
	}

	if (host->cmd_op.l_cmd == NAND_CMD_READOOB) {
		value  = readb(host->buffer + host->pagesize + host->offset);
		host->offset++;
		return value;
	}

	host->offset++;

	return readb(host->buffer + host->column + host->offset - 1);
}

/*****************************************************************************/
static unsigned short hifmc100_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	host->offset += 2;
	return readw(host->buffer + host->column + host->offset - 2);
}

/*****************************************************************************/
static void hifmc100_write_buf(struct mtd_info *mtd,
	const u_char *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
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
static void hifmc100_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_READ
	if (buf == chip->oob_poi)
		memcpy(buf, (char *)host->iobase + host->pagesize, len);
	else
		memcpy(buf, (char *)host->iobase, len);
#else
	if (buf == chip->oob_poi)
		memcpy(buf, (char *)host->buffer + host->pagesize, len);
	else
		memcpy(buf, (char *)host->buffer, len);
#endif

	if (buf != chip->oob_poi) {
		u_int reg, ecc_step = host->pagesize >> 10;

		reg = hifmc_read(host, HIFMC100_ECC_ERR_NUM0_BUF0);
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
static void hifmc100_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_SPI_NAND_MAX_CHIP_NUM)
		DB_BUG("Error: Invalid chipselect: %d\n", chipselect);

	if (host->mtd != mtd) {
		host->mtd = mtd;
		host->cmd_op.cs = chipselect;
	}

	if (!(chip->options & NAND_BROKEN_XD)) {
		if ((chip->state == FL_ERASING) || (chip->state == FL_WRITING))
			host->cmd_op.l_cmd = NAND_CMD_GET_FEATURES;
	}
}

/*****************************************************************************/
static void hifmc100_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned ctrl)
{
	unsigned char cmd;
	int is_cache_invalid = 1;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	if (ctrl & NAND_ALE) {
		unsigned int addr_value = 0;
		unsigned int addr_offset = 0;

		if (ctrl & NAND_CTRL_CHANGE) {
			host->addr_cycle = 0x0;
			host->addr_value[0] = 0x0;
			host->addr_value[1] = 0x0;
		}
		addr_offset = host->addr_cycle << 3;

		if (host->addr_cycle >= HIFMC100_ADDR_CYCLE_MASK) {
			addr_offset = (host->addr_cycle -
					HIFMC100_ADDR_CYCLE_MASK) << 3;
			addr_value = 1;
		}

		host->addr_value[addr_value] |=
			((dat & 0xff) << addr_offset);

		host->addr_cycle++;
	}

	if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE)) {
		cmd = dat & 0xff;
		host->cmd_op.cmd = cmd;
		switch (cmd) {
		case NAND_CMD_PAGEPROG:
			host->offset = 0;
			host->send_cmd_pageprog(host);
			break;

		case NAND_CMD_READSTART:
			is_cache_invalid = 0;
			if (host->addr_value[0] == host->pagesize)
				host->cmd_op.l_cmd = NAND_CMD_READOOB;
			host->send_cmd_readstart(host);
			break;

		case NAND_CMD_ERASE2:
			host->send_cmd_erase(host);
			break;

		case NAND_CMD_READID:
			memset((u_char *)(host->iobase), 0,
					MAX_SPI_NAND_ID_LEN);
			host->cmd_op.l_cmd = cmd;
			host->cmd_op.data_no = MAX_SPI_NAND_ID_LEN;
			host->send_cmd_readid(host);
			break;

		case NAND_CMD_STATUS:
			host->send_cmd_status(host);
			break;

		case NAND_CMD_READ0:
			host->cmd_op.l_cmd = cmd;
			break;

		case NAND_CMD_RESET:
			host->send_cmd_reset(host);
			break;

		case NAND_CMD_SEQIN:
		case NAND_CMD_ERASE1:
		default:
			break;
		}
	}

	if ((dat == NAND_CMD_NONE) && host->addr_cycle) {
		if (host->cmd_op.cmd == NAND_CMD_SEQIN
			|| host->cmd_op.cmd == NAND_CMD_READ0
			|| host->cmd_op.cmd == NAND_CMD_READID) {
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
static int hifmc100_dev_ready(struct mtd_info *mtd)
{
	unsigned int reg;
	unsigned long deadline = jiffies + FMC_MAX_READY_WAIT_JIFFIES;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	do {
		reg = OP_CFG_FM_CS(host->cmd_op.cs);
		hifmc_write(host, FMC_OP_CFG, reg);

		reg = FMC_OP_READ_STATUS_EN(ENABLE) | FMC_OP_REG_OP_START;
		hifmc_write(host, FMC_OP, reg);

		FMC_CMD_WAIT_CPU_FINISH(host);

		reg = hifmc_read(host, FMC_STATUS);
		if (!(reg & STATUS_OIP_MASK))
			return NAND_STATUS_READY;

		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	DB_MSG("Error: SPI Nand wait ready timeout, status: %#x\n", reg);

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
static struct nand_config_info hifmc_spi_nand_config_table[] = {
	{NAND_PAGE_4K,	NAND_ECC_24BIT,	200,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_16BIT,	144,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_8BIT,	128,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_0BIT,	32,	&nand_ecc_default},

	{NAND_PAGE_2K,	NAND_ECC_24BIT,	128,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_16BIT,	88,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_8BIT,	64,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_0BIT,	32,	&nand_ecc_default},

	{0,		0,		0,	NULL},
};

/*****************************************************************************/
/* used the best correct arithmetic. */
static struct nand_config_info *hifmc100_get_best_ecc(struct mtd_info *mtd)
{
	struct nand_config_info *best = NULL;
	struct nand_config_info *info = hifmc_spi_nand_config_table;

	for (; info->layout; info++) {
		if (match_page_type_to_size(info->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < info->oobsize)
			continue;

		if (!best || (best->ecctype < info->ecctype))
			best = info;
	}

	if (!best)
		DB_BUG(ERR_STR_DRIVER "pagesize: %d and oobsize: %d.\n",
				mtd->writesize, mtd->oobsize);
	return best;
}

#if defined(CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE) \
	|| defined(CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC)
/*****************************************************************************/
/* force the pagesize and ecctype */
static struct nand_config_info *hifmc100_force_ecc(struct mtd_info *mtd,
		int pagetype, int ecctype, char *cfgmsg, int allow_pagediv)
{
	int pagesize;
	struct nand_config_info *fit = NULL;
	struct nand_config_info *info = hifmc_spi_nand_config_table;

	for (; info->layout; info++) {
		if (info->pagetype == pagetype && info->ecctype == ecctype) {
			fit = info;
			break;
		}
	}

	if (!fit) {
		DB_MSG("Driver(%s) can't find this configure\n", cfgmsg);
		DB_BUG(ERR_STR_DRIVER "pagesize: %s, ecctype: %s\n",
			nand_page_name(pagetype), nand_ecc_name(ecctype));
		return NULL;
	}

	pagesize = match_page_type_to_size(pagetype);
	if ((pagesize != mtd->writesize)
		&& (pagesize > mtd->writesize || !allow_pagediv)) {
		DB_MSG("This SPI Nand Flash pageszie: %d\n", mtd->writesize);
		DB_BUG("But (%s) configure pagesize: %d" ERR_STR_CHECK "\n",
				cfgmsg, pagesize);
		return NULL;
	}

	if (fit->oobsize > mtd->oobsize) {
		DB_MSG("This SPI Nand Flash offer space area is %dB\n",
				mtd->oobsize);
		DB_BUG("But (%s) the controller request %dB in ecc %s.",
				cfgmsg, fit->oobsize, nand_ecc_name(ecctype));
		return NULL;
	}

	return fit;
}
#endif

/*****************************************************************************/
static int hifmc100_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *nand_dev)
{
	unsigned char page_reg, pagetype, ecc_reg, ecctype, block_reg = 0;
	unsigned int reg, page_per_block;
	char *start_type = "unknown";
	struct nand_config_info *best = NULL;
	struct hifmc_host *host = chip->priv;

	FMC_PR(BT_DBG, "\t*-Start match PageSize and EccType\n");

	reg = hifmc_read(host, FMC_CFG);
	FMC_PR(BT_DBG, "\t|-Get FMC_CFG[%#x] config: %#x\n", FMC_CFG, reg);

#ifdef CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	best = hifmc100_get_best_ecc(mtd);
	start_type = "Auto";

	pagetype = best->pagetype;
	ecctype = best->ecctype;

	page_reg = match_page_type_to_reg(pagetype);
	reg &= ~PAGE_SIZE_MASK;
	reg |= FMC_CFG_PAGE_SIZE(page_reg);
	FMC_PR(BT_DBG, "\t|-%s Config, PageSize %s EccType %s OobSize %d\n",
			start_type, nand_page_name(pagetype),
			nand_ecc_name(ecctype), best->oobsize);

	ecc_reg = match_ecc_type_to_reg(ecctype);
	reg &= ~ECC_TYPE_MASK;
	reg |= FMC_CFG_ECC_TYPE(ecc_reg);
	FMC_PR(BT_DBG, "\t|-%s Config best EccType: %s\n", start_type,
			nand_ecc_name(best->ecctype));

	page_per_block = mtd->erasesize / match_page_type_to_size(pagetype);
	switch (page_per_block) {
	case 64:
		block_reg = BLOCK_SIZE_64_PAGE;
		break;
	case 128:
		block_reg = BLOCK_SIZE_128_PAGE;
		break;
	case 256:
		block_reg = BLOCK_SIZE_256_PAGE;
		break;
	case 512:
		block_reg = BLOCK_SIZE_512_PAGE;
		break;
	default:
		DB_MSG("Can't support block %#x and page %#x size\n",
				mtd->erasesize, mtd->writesize);
	}
	reg &= ~BLOCK_SIZE_MASK;
	reg |= FMC_CFG_BLOCK_SIZE(block_reg);

	hifmc_write(host, FMC_CFG, reg);
	FMC_PR(BT_DBG, "\t|-Set FMC_CFG[%#x] config: %#x\n", FMC_CFG, reg);
#endif

	host->fmc_cfg = reg;
	FMC_PR(BT_DBG, "\t|-Save FMC_CFG config: %#x\n", reg);

#ifdef CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC
	#ifdef CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	#error you SHOULD NOT define CONFIG_HIFMC100_AUTO_PAGESIZE_ECC \
		and CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC at the same time
	#endif

	page_reg = (host->fmc_cfg & PAGE_SIZE_MASK) >> PAGE_SIZE_SHIFT;
	pagetype = match_page_reg_to_type(page_reg);
	FMC_PR(BT_DBG, "\t|-Get Hardware Config PageSize: %s\n",
			nand_page_name(pagetype));

	ecc_reg = (host->fmc_cfg & ECC_TYPE_MASK) >> ECC_TYPE_SHIFT;
	ecctype = match_ecc_reg_to_type(ecc_reg);
	FMC_PR(BT_DBG, "\t|-Get Hardware Config EccType: %s\n",
			match_ecc_type_to_str(ecctype));

	FMC_PR(BT_DBG, "\t|-Check FMC_CFG Config with Hardware Config\n");
	best = hifmc100_force_ecc(mtd, pagetype, ecctype,
			"hardware config mode", 0);
	start_type = "Hardware";
#endif

#ifdef CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE

	#ifdef CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	#error you SHOULD NOT define CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE \
		and CONFIG_HIFMC100_AUTO_PAGESIZE_ECC at the same time
	#endif

	#ifdef CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC
	#error you SHOULD NOT define CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE \
		and CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC at the same time
	#endif

	pagetype = match_page_size_to_type(mtd->writesize);

	best = hifmc100_force_ecc(mtd, pagetype, NAND_ECC_0BIT,
			"force config mode", 0);
	start_type = "AutoForce";
	FMC_PR(BT_DBG, "\t|-Check PageSize %s Config\n", start_type);

	page_reg = match_page_type_to_reg(best->pagetype);

	reg &= ~(PAGE_SIZE_MASK | ECC_TYPE_MASK);
	reg |= FMC_CFG_PAGE_SIZE(page_reg);
	reg |= FMC_CFG_ECC_TYPE(ECC_TYPE_0BIT);
	host->fmc_cfg = reg;
	FMC_PR(BT_DBG, "\t|-Save FMC_CFG config: %#x\n", reg);
#endif /* End of CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE */

	if (!best)
		DB_BUG("Can't found any configure for SPI Nand Flash\n");

	if (best->ecctype != NAND_ECC_0BIT)
		mtd->oobsize = best->oobsize;

	chip->ecc.layout = best->layout;

	host->ecctype = best->ecctype;
	host->pagesize = match_page_type_to_size(best->pagetype);
	host->oobsize = mtd->oobsize;

	FMC_PR(BT_DBG, "\t|-%s Config end, best OOB Size: %d\n", start_type,
			best->oobsize);

	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);
	host->dma_oob = host->dma_buffer + host->pagesize;
	host->bbm = (u_char *)(host->buffer + host->pagesize
			+ HIFMC_BAD_BLOCK_POS);
	host->epm = (u_short *)(host->buffer + host->pagesize
			+ chip->ecc.layout->oobfree[0].offset + 28);
	host->fmc_cfg_ecc0 = (host->fmc_cfg & ~ECC_TYPE_MASK) | ECC_TYPE_0BIT;

	if (mtd->writesize > SPI_NAND_MAX_PAGESIZE
		|| mtd->oobsize > SPI_NAND_MAX_OOBSIZE) {
		DB_MSG(ERR_STR_DRIVER "pageszie: %d and oobsize: %d\n",
				mtd->writesize, mtd->oobsize);
		DB_BUG("Please increase MAX PAGESIZE and OOBSIZE.\n");
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
		DB_MSG("SPI Nand divide into 1/%u\n", (1 << shift));
	}

	nand_dev->start_type = start_type;
	nand_dev->oobsize = host->oobsize;
	nand_dev->ecctype = host->ecctype;

	FMC_PR(BT_DBG, "\t*-End match PageSize and EccType\n");

	return 0;
}

/*****************************************************************************/
static void hifmc100_chip_init(struct nand_chip *chip)
{
	chip->read_byte = hifmc100_read_byte;
	chip->read_word = hifmc100_read_word;
	chip->write_buf = hifmc100_write_buf;
	chip->read_buf = hifmc100_read_buf;

	chip->select_chip = hifmc100_select_chip;

	chip->cmd_ctrl = hifmc100_cmd_ctrl;
	chip->dev_ready = hifmc100_dev_ready;

	chip->chip_delay = FMC_CHIP_DELAY;

	chip->options = NAND_NO_AUTOINCR | NAND_SKIP_BBTSCAN | NAND_BROKEN_XD;

	chip->ecc.layout = NULL;
	chip->ecc.mode = NAND_ECC_NONE;
}

/*****************************************************************************/
static int hifmc100_host_init(struct hifmc_host *host)
{
	unsigned int reg, flash_type;

	FMC_PR(BT_DBG, "\t||*-Start SPI Nand host init\n");

	reg = hifmc_read(host, FMC_CFG);
	flash_type = (reg & FLASH_SEL_MASK) >> FLASH_SEL_SHIFT;
	if (flash_type != FLASH_TYPE_SPI_NAND) {
		DB_MSG("Error: Flash type isn't SPI Nand. reg: %#x\n", reg);
		return -ENODEV;
	}

	if ((reg & OP_MODE_MASK) == OP_MODE_BOOT) {
		reg |= FMC_CFG_OP_MODE(OP_MODE_NORMAL);
		hifmc_write(host, FMC_CFG, reg);
		FMC_PR(BT_DBG, "\t|||-Set CFG[%#x]%#x\n", FMC_CFG, reg);
	}

	host->fmc_cfg = reg;
	host->fmc_cfg_ecc0 = (reg & ~ECC_TYPE_MASK) | ECC_TYPE_0BIT;

	reg = hifmc_read(host, FMC_GLOBAL_CFG);
	if (reg & FMC_GLOBAL_CFG_WP_ENABLE) {
		reg &= ~FMC_GLOBAL_CFG_WP_ENABLE;
		hifmc_write(host, FMC_GLOBAL_CFG, reg);
	}

	host->addr_cycle = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->cache_addr_value[0] = ~0;
	host->cache_addr_value[1] = ~0;

	host->send_cmd_pageprog = hifmc100_send_cmd_pageprog;
	host->send_cmd_status = hifmc100_send_cmd_status;
	host->send_cmd_readstart = hifmc100_send_cmd_readstart;
	host->send_cmd_erase = hifmc100_send_cmd_erase;
	host->send_cmd_readid = hifmc100_send_cmd_readid;
	host->send_cmd_reset = hifmc100_send_cmd_reset;
	host->set_system_clock = hifmc_set_nand_system_clock;
#ifdef CONFIG_PM
	host->suspend = hifmc100_spi_nand_suspend;
	host->resume  = hifmc100_spi_nand_resume;
#endif

	reg = TIMING_CFG_TCSH(CS_HOLD_TIME)
		| TIMING_CFG_TCSS(CS_SETUP_TIME)
		| TIMING_CFG_TSHSL(CS_DESELECT_TIME);
	hifmc_write(host, FMC_SPI_TIMING_CFG, reg);
	FMC_PR(BT_DBG, "\t|||-Set TIMING[%#x]%#x\n", FMC_SPI_TIMING_CFG, reg);

	reg = ALL_BURST_ENABLE;
	hifmc_write(host, FMC_DMA_AHB_CTRL, reg);
	FMC_PR(BT_DBG, "\t|||-Set DMA_AHB[%#x]%#x\n", FMC_DMA_AHB_CTRL, reg);

	FMC_PR(BT_DBG, "\t||*-End SPI Nand host init\n");

	return 0;
}

/*****************************************************************************/
int hifmc100_spi_nand_init(struct nand_chip *chip)
{
	int ret = 0;
	struct hifmc_host *host = chip->priv;

	FMC_PR(BT_DBG, "\t|*-Start hifmc100 SPI Nand init\n");

	/* Hifmc host init */
	FMC_PR(BT_DBG, "\t||-Hifmc100 struct host init\n");
	ret = hifmc100_host_init(host);
	if (ret) {
		DB_MSG("Error: SPI Nand host init failed, result: %d\n", ret);
		return ret;
	}
	host->chip = chip;

	/* Set system clock and enable controller */
	FMC_PR(BT_DBG, "\t||-Set system clock and Enable Controller\n");
	if (host->set_system_clock)
		host->set_system_clock(NULL, ENABLE);

	/* Hifmc nand_chip struct init */
	FMC_PR(BT_DBG, "\t||-Hifmc100 struct nand_chip init\n");
	hifmc100_chip_init(chip);

	FMC_PR(BT_DBG, "\t||-Register SPI Nand ID table and ecc probe\n");
	hifmc_spi_nand_ids_register();
	nand_oob_resize = hifmc100_ecc_probe;

	FMC_PR(BT_DBG, "\t|*-End hifmc100 SPI Nand init\n");

	return 0;
}
#ifdef CONFIG_PM
/*****************************************************************************/
int hifmc100_spi_nand_suspend(struct platform_device *pltdev,
		pm_message_t state)
{
	unsigned int ret;
	struct hifmc_host *host = platform_get_drvdata(pltdev);
	struct hifmc_spi *spi = host->spi;

	ret = spi->driver->wait_ready(spi);
	if (ret) {
		DB_MSG("Error: wait ready failed!");
		return 0;
	}

	if (host->set_system_clock)
		host->set_system_clock(NULL, DISABLE);

	return 0;
}
/*****************************************************************************/
int hifmc100_spi_nand_resume(struct platform_device *pltdev)
{
	int cs;
	struct hifmc_host *host = platform_get_drvdata(pltdev);
	struct nand_chip *chip = host->chip;

	/* Set system clock and enable controller */
	FMC_PR(PM_DBG, "\t|-Set system clock and Enable Controller\n");
	if (host->set_system_clock)
		host->set_system_clock(NULL, ENABLE);

	for (cs = 0; cs < chip->numchips; cs++)
		host->send_cmd_reset(host);

	hifmc100_spi_nand_config(host);

	return 0;
}
#endif

