/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
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
 
#include <linux/io.h>
#include <linux/compiler.h>
#include <linux/delay.h>
#include <linux/printk.h>
#include <linux/clk.h>
#include <linux/mfd/hisi_fmc.h>

#include "../hinfc_gen.h"
#include "hifmc100_nand_os.h"
#include "hifmc100_nand.h"

#include <mach/platform.h>
/*****************************************************************************/
static void hifmc100_dma_transfer(struct hifmc_host *host, int todev)
{
	unsigned int reg = (unsigned int)host->dma_buffer;
	char *op = todev ? "write" : "read";

	FMC_PR(DMA_DB, "\t\t *-Start %s page dma transfer\n", op);

	hifmc_writel(host, FMC_DMA_SADDR_D0, reg);
	FMC_PR(DMA_DB, "\t\t |-Set ADDR0[%#x]%#x\n", FMC_DMA_SADDR_D0, reg);

	reg += FMC_DMA_ADDR_OFFSET;
	hifmc_writel(host, FMC_DMA_SADDR_D1, reg);
	FMC_PR(DMA_DB, "\t\t |-Set ADDR1[%#x]%#x\n", FMC_DMA_SADDR_D1, reg);

	reg += FMC_DMA_ADDR_OFFSET;
	hifmc_writel(host, FMC_DMA_SADDR_D2, reg);
	FMC_PR(DMA_DB, "\t\t |-Set ADDR2[%#x]%#x\n", FMC_DMA_SADDR_D2, reg);

	reg += FMC_DMA_ADDR_OFFSET;
	hifmc_writel(host, FMC_DMA_SADDR_D3, reg);
	FMC_PR(DMA_DB, "\t\t |-Set ADDR3[%#x]%#x\n", FMC_DMA_SADDR_D3, reg);

	reg = host->dma_oob;
	hifmc_writel(host, FMC_DMA_SADDR_OOB, reg);
	FMC_PR(DMA_DB, "\t\t |-Set OOB[%#x]%#x\n", FMC_DMA_SADDR_OOB, reg);

	if (host->ecctype == NAND_ECC_0BIT) {
		hifmc_writel(host, FMC_DMA_LEN, FMC_DMA_LEN_SET(host->oobsize));
		FMC_PR(DMA_DB, "\t\t |-Set LEN[%#x]%#x\n", FMC_DMA_LEN, reg);
	}
	reg = FMC_OP_READ_DATA_EN | FMC_OP_WRITE_DATA_EN;
	hifmc_writel(host, FMC_OP, reg);
	FMC_PR(DMA_DB, "\t\t |-Set OP[%#x]%#x\n", FMC_OP, reg);

	reg = FMC_DMA_AHB_CTRL_DMA_PP_EN
		| FMC_DMA_AHB_CTRL_BURST16_EN
		| FMC_DMA_AHB_CTRL_BURST8_EN
		| FMC_DMA_AHB_CTRL_BURST4_EN;
	hifmc_writel(host, FMC_DMA_AHB_CTRL, reg);
	FMC_PR(DMA_DB, "\t\t |-Set AHBCTRL[%#x]%#x\n", FMC_DMA_AHB_CTRL, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs)
		| OP_CFG_ADDR_NUM(host->addr_cycle);
	hifmc_writel(host, FMC_OP_CFG, reg);
	FMC_PR(DMA_DB, "\t\t |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = OP_CTRL_DMA_OP_READY;
	if (todev)
		reg |= OP_CTRL_RW_OP(todev);
	hifmc_writel(host, FMC_OP_CTRL, reg);
	FMC_PR(DMA_DB, "\t\t |-Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, reg);

	FMC_DMA_WAIT_CPU_FINISH(host);

	FMC_PR(DMA_DB, "\t\t *-End %s page dma transfer\n", op);

	return;
}

/*****************************************************************************/
static void hifmc100_send_cmd_write(struct hifmc_host *host)
{
	unsigned int reg;

	FMC_PR(WR_DBG, "\t|*-Start send page programme cmd\n");

	if (*host->bbm != 0xFF && *host->bbm != 0x00)
		pr_info("WARNING: attempt to write an invalid bbm. " \
				"page: 0x%08x, mark: 0x%02x,\n",
				GET_PAGE_INDEX(host), *host->bbm);

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	reg = host->addr_value[1];
	hifmc_writel(host, FMC_ADDRH, reg);
	FMC_PR(WR_DBG, "\t||-Set ADDRH[%#x]%#x\n", FMC_ADDRH, reg);

	reg = host->addr_value[0] & 0xffff0000;
	hifmc_writel(host, FMC_ADDRL, reg);
	FMC_PR(WR_DBG, "\t||-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

	reg = FMC_CMD_CMD2(NAND_CMD_PAGEPROG) | FMC_CMD_CMD1(NAND_CMD_SEQIN);
	hifmc_writel(host, FMC_CMD, reg);
	FMC_PR(WR_DBG, "\t||-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	*host->epm = 0x0000;

	hifmc100_dma_transfer(host, RW_OP_WRITE);

	FMC_PR(WR_DBG, "\t|*-End send page read cmd\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_read(struct hifmc_host *host)
{
	unsigned int reg;

	FMC_PR(RD_DBG, "\t*-Start send page read cmd\n");

	if ((host->addr_value[0] == host->cache_addr_value[0])
		&& (host->addr_value[1] == host->cache_addr_value[1])) {
		FMC_PR(RD_DBG, "\t*-Cache hit! addr1[%#x], addr0[%#x]\n",
			host->addr_value[1], host->addr_value[0]);
		return;
	}

	host->page_status = 0;

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	reg = FMC_INT_CLR_ALL;
	hifmc_writel(host, FMC_INT_CLR, reg);
	FMC_PR(RD_DBG, "\t|-Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, reg);

	reg = host->nand_cfg;
	hifmc_writel(host, FMC_CFG, reg);
	FMC_PR(RD_DBG, "\t|-Set CFG[%#x]%#x\n", FMC_CFG, reg);

	reg = host->addr_value[1];
	hifmc_writel(host, FMC_ADDRH, reg);
	FMC_PR(RD_DBG, "\t|-Set ADDRH[%#x]%#x\n", FMC_ADDRH, reg);

	reg = host->addr_value[0] & 0xffff0000;
	hifmc_writel(host, FMC_ADDRL, reg);
	FMC_PR(RD_DBG, "\t|-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

	reg = FMC_CMD_CMD2(NAND_CMD_READSTART) | FMC_CMD_CMD1(NAND_CMD_READ0);
	hifmc_writel(host, FMC_CMD, reg);
	FMC_PR(RD_DBG, "\t|-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	hifmc100_dma_transfer(host, RW_OP_READ);

	if (hifmc_readl(host, FMC_INT) & FMC_INT_ERR_INVALID)
		host->page_status |= HIFMC100_PS_UC_ECC;

	host->cache_addr_value[0] = host->addr_value[0];
	host->cache_addr_value[1] = host->addr_value[1];

	FMC_PR(RD_DBG, "\t*-End send page read cmd\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_erase(struct hifmc_host *host)
{
	unsigned int reg;

	FMC_PR(ER_DBG, "\t *-Start send cmd erase\n");

	/* Don't case the read retry config */
	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	reg = host->addr_value[0];
	hifmc_writel(host, FMC_ADDRL, reg);
	FMC_PR(ER_DBG, "\t |-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

	reg = FMC_CMD_CMD2(NAND_CMD_ERASE2) | FMC_CMD_CMD1(NAND_CMD_ERASE1);
	hifmc_writel(host, FMC_CMD, reg);
	FMC_PR(ER_DBG, "\t |-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs)
		| OP_CFG_ADDR_NUM(host->addr_cycle);
	hifmc_writel(host, FMC_OP_CFG, reg);
	FMC_PR(ER_DBG, "\t |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	/* need to config WAIT_READY_EN */
	reg = FMC_OP_WAIT_READY_EN
		| FMC_OP_CMD1_EN
		| FMC_OP_CMD2_EN
		| FMC_OP_ADDR_EN
		| FMC_OP_REG_OP_START;
	hifmc_writel(host, FMC_OP, reg);
	FMC_PR(ER_DBG, "\t |-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	FMC_PR(ER_DBG, "\t |*-End send cmd erase\n");
}

/*****************************************************************************/
static void hifmc100_ecc_randomizer(struct hifmc_host *host, int ecc_en,
		int randomizer_en)
{
	unsigned int old_reg, reg, change = 0;
	char *ecc_op = ecc_en ? "Quit" : "Enter";
	char *rand_op = randomizer_en ? "Enable" : "Disable";

	if (IS_NAND_RANDOM(host)) {
		reg = old_reg = hifmc_readl(host, FMC_GLOBAL_CFG);
		if (randomizer_en)
			reg |= FMC_GLOBAL_CFG_RANDOMIZER_EN;
		else
			reg &= ~FMC_GLOBAL_CFG_RANDOMIZER_EN;

		if (old_reg != reg) {
			FMC_PR(EC_DBG, "\t |*-Start %s randomizer\n", rand_op);
			FMC_PR(EC_DBG, "\t ||-Get global CFG[%#x]%#x\n",
					FMC_GLOBAL_CFG, old_reg);
			hifmc_writel(host, FMC_GLOBAL_CFG, reg);
			FMC_PR(EC_DBG, "\t ||-Set global CFG[%#x]%#x\n",
					FMC_GLOBAL_CFG, reg);
			change++;
		}
	}

	old_reg = hifmc_readl(host, FMC_CFG);
	reg = (ecc_en ? host->nand_cfg : host->nand_cfg_ecc0);

	if (old_reg != reg) {
		FMC_PR(EC_DBG, "\t |%s-Start %s ECC0 mode\n", change ? "|":"*",
				ecc_op);
		FMC_PR(EC_DBG, "\t ||-Get CFG[%#x]%#x\n", FMC_CFG, old_reg);
		hifmc_writel(host, FMC_CFG, reg);
		FMC_PR(EC_DBG, "\t ||-Set CFG[%#x]%#x\n", FMC_CFG, reg);
		change++;
	}

	if (EC_DBG && change)
		FMC_PR(EC_DBG, "\t |*-End randomizer and ECC0 mode config\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_status(struct hifmc_host *host)
{
	unsigned int regval;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	regval = OP_CFG_FM_CS(host->cmd_op.cs);
	hifmc_writel(host, FMC_OP_CFG, regval);

	regval = FMC_OP_READ_STATUS_EN | FMC_OP_REG_OP_START;
	hifmc_writel(host, FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);
}

/*****************************************************************************/
static void hifmc100_send_cmd_readid(struct hifmc_host *host)
{
	unsigned int reg;

	FMC_PR(BT_DBG, "\t *-Start read nand flash ID\n");

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	reg = FMC_DATA_NUM_CNT(host->cmd_op.data_no);
	hifmc_writel(host, FMC_DATA_NUM, reg);
	FMC_PR(BT_DBG, "\t |-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_CMD_CMD1(NAND_CMD_READID);
	hifmc_writel(host, FMC_CMD, reg);
	FMC_PR(BT_DBG, "\t |-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = 0;
	hifmc_writel(host, FMC_ADDRL, reg);
	FMC_PR(BT_DBG, "\t |-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs)
		| OP_CFG_ADDR_NUM(READ_ID_ADDR_NUM);
	hifmc_writel(host, FMC_OP_CFG, reg);
	FMC_PR(BT_DBG, "\t |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN
		| FMC_OP_ADDR_EN
		| FMC_OP_READ_DATA_EN
		| FMC_OP_REG_OP_START;
	hifmc_writel(host, FMC_OP, reg);
	FMC_PR(BT_DBG, "\t |-Set OP[%#x]%#x\n", FMC_OP, reg);

	host->addr_cycle = 0x0;

	FMC_CMD_WAIT_CPU_FINISH(host);

	FMC_PR(BT_DBG, "\t *-End read nand flash ID\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_reset(struct hifmc_host *host)
{
	unsigned int reg;

	FMC_PR(BT_DBG, "\t *-Start reset nand flash\n");

	reg = FMC_CMD_CMD1(NAND_CMD_RESET);
	hifmc_writel(host, FMC_CMD, reg);
	FMC_PR(BT_DBG, "\t |-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(host->cmd_op.cs);
	hifmc_writel(host, FMC_OP_CFG, reg);
	FMC_PR(BT_DBG, "\t |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN
		| FMC_OP_WAIT_READY_EN
		| FMC_OP_REG_OP_START;
	hifmc_writel(host, FMC_OP, reg);
	FMC_PR(BT_DBG, "\t |-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	FMC_PR(BT_DBG, "\t *-End reset nand flash\n");
}

/*****************************************************************************/
static unsigned char hifmc100_read_byte(struct mtd_info *mtd)
{
	unsigned char value = 0;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	if (host->cmd_op.l_cmd == NAND_CMD_READID) {
		value = hifmc_readb((void __iomem *)(chip->IO_ADDR_R + host->offset));
		host->offset++;
		if (host->cmd_op.data_no == host->offset)
			host->cmd_op.l_cmd = 0;
		return value;
	}

	if (host->cmd_op.cmd == NAND_CMD_STATUS) {
		value = hifmc_readl(host, FMC_STATUS);
		if (host->cmd_op.l_cmd == NAND_CMD_ERASE1)
			FMC_PR(ER_DBG, "\t*-Erase WP status: %#x\n", value);
		if (host->cmd_op.l_cmd == NAND_CMD_PAGEPROG)
			FMC_PR(WR_DBG, "\t*-Write WP status: %#x\n", value);
		return value;
	}

	if (host->cmd_op.l_cmd == NAND_CMD_READOOB) {
		value = hifmc_readb((void __iomem *)(host->buffer + host->pagesize
					+ host->offset));
		host->offset++;
		return value;
	}

	host->offset++;

	return hifmc_readb((void __iomem *)(host->buffer + host->column \
				+ host->offset - 1));
}

/*****************************************************************************/
static unsigned short hifmc100_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	host->offset += 2;
	return hifmc_readw(host->buffer + host->column + host->offset - 2);
}

/*****************************************************************************/
static void hifmc100_write_buf(struct mtd_info *mtd,
	const u_char *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

#ifdef HIFMC100_NAND_SUPPORT_REG_WRITE
	if (buf == chip->oob_poi)
		memcpy((char *)host->iobase + host->pagesize, buf, len);
	else
		memcpy((char *)host->iobase, buf, len);
#else
	if (buf == chip->oob_poi)
		memcpy((char *)host->buffer + host->pagesize, buf, len);
	else
		memcpy((char *)host->buffer, buf, len);
#endif
	return;
}

#ifdef CONFIG_HISI_NAND_ECC_STATUS_REPORT
/*****************************************************************************/
static void hifmc100_ecc_err_num_count(struct mtd_info *mtd,
			u_int ecc_st, u_int reg)
{
	u_char err_num;

	if (ecc_st > 4)
		ecc_st = 4;

	while (ecc_st) {
		err_num = GET_ECC_ERR_NUM(--ecc_st, reg);
		if (err_num == 0xff)
			mtd->ecc_stats.failed++;
		else
			mtd->ecc_stats.corrected += err_num;
	}
}
#endif

/*****************************************************************************/
static void hifmc100_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

#ifdef HIFMC100_NAND_SUPPORT_REG_READ
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

#ifdef CONFIG_HISI_NAND_ECC_STATUS_REPORT
	if (buf != chip->oob_poi) {
		u_int reg, ecc_step = host->pagesize >> 10;

		/* 2K or 4K or 8K(1) or 16K(1-1) pagesize */
		reg = hifmc_readl(host, HIFMC100_ECC_ERR_NUM0_BUF0);
		hifmc100_ecc_err_num_count(mtd, ecc_step, reg);

		if (ecc_step > 4) {
			/* 8K(2) or 16K(1-2) pagesize */
			reg = hifmc_readl(host, HIFMC100_ECC_ERR_NUM1_BUF0);
			hifmc100_ecc_err_num_count(mtd, ecc_step, reg);
			if (ecc_step > 8) {
				/* 16K(2-1) pagesize */
				reg = hifmc_readl(host,
						HIFMC100_ECC_ERR_NUM0_BUF1);
				hifmc100_ecc_err_num_count(mtd, ecc_step, reg);
				/* 16K(2-2) pagesize */
				reg = hifmc_readl(host,
						HIFMC100_ECC_ERR_NUM1_BUF1);
				hifmc100_ecc_err_num_count(mtd, ecc_step, reg);
			}
		}
	}
#endif

	return;
}

/*****************************************************************************/
static void hifmc100_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_HIFMC100_MAX_NAND_CHIP)
		DB_BUG("Error: Invalid chip select: %d\n", chipselect);

	host->cmd_op.cs = chipselect;
	if (host->mtd != mtd)
		host->mtd = mtd;

	switch (chip->state) {
	case FL_ERASING:
		host->cmd_op.l_cmd = NAND_CMD_ERASE1;
		if (ER_DBG)
			pr_info("\n");
		FMC_PR(ER_DBG, "\t*-Erase chip: %d\n", chipselect);
		break;
	case FL_WRITING:
		host->cmd_op.l_cmd = NAND_CMD_PAGEPROG;
		if (WR_DBG)
			pr_info("\n");
		FMC_PR(WR_DBG, "\t*-Write chip: %d\n", chipselect);
		break;
	case FL_READING:
		host->cmd_op.l_cmd = NAND_CMD_READ0;
		if (RD_DBG)
			pr_info("\n");
		FMC_PR(RD_DBG, "\t*-Read chip: %d\n", chipselect);
		break;
	default:
		break;
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
			host->cmd_op.l_cmd = cmd;
			host->send_cmd_erase(host);
			break;

		case NAND_CMD_READID:
			memset((u_char *)(chip->IO_ADDR_R), 0, MAX_NAND_ID_LEN);
			host->cmd_op.l_cmd = cmd;
			host->cmd_op.data_no = MAX_NAND_ID_LEN;
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

	/* pass pagesize and ecctype to kernel when startup. */
	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

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
	return 0x1;
}

/*****************************************************************************/
/*
 * 'host->epm' only use the first oobfree[0] field, it looks very simple, But...
 */
static struct nand_ecclayout nand_ecc_default = {
	.oobfree = {{2, 30} }
};

/*****************************************************************************/
/* ecc/pagesize get from CPU PIN. */
static struct nand_config_info hifmc100_nand_hw_pin_config_table[] = {
	{NAND_PAGE_16K, NAND_ECC_64BIT, 1824/*1824*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_40BIT, 1152/*1152*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_0BIT,  32 ,          &nand_ecc_default},

	{NAND_PAGE_8K, NAND_ECC_64BIT, 928 /*928*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_40BIT, 592 /*592 */,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_24BIT, 368 /*368*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_0BIT,  32,           &nand_ecc_default},

	{NAND_PAGE_4K, NAND_ECC_24BIT, 200 /*200*/,  &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_16BIT, 128 /*128*/,  &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_8BIT,  88  /*88*/,   &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_0BIT,  32,           &nand_ecc_default},

	{NAND_PAGE_2K, NAND_ECC_24BIT, 116 /*116*/, &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_16BIT, 64  /*64*/,  &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_8BIT,  60  /*60*/,  &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_0BIT,  32,          &nand_ecc_default},

	{0, 0, 0, NULL},
};

/*****************************************************************************/
/* ecc/pagesize get from NAND controller */
static struct nand_config_info hifmc100_nand_hw_auto_config_table[] = {
	{NAND_PAGE_16K, NAND_ECC_64BIT, 1824/*1824*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_40BIT, 1200/*1152*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_0BIT,  32 ,          &nand_ecc_default},

	{NAND_PAGE_8K, NAND_ECC_64BIT, 928 /*928*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_40BIT, 600 /*592*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_24BIT, 368 /*368*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_0BIT,  32,           &nand_ecc_default},

	{NAND_PAGE_4K, NAND_ECC_24BIT, 200 /*200*/,  &nand_ecc_default},
#ifndef CONFIG_SUPPORT_YAFFS
	{NAND_PAGE_4K, NAND_ECC_16BIT,  128 /*128*/,  &nand_ecc_default},
#endif
	{NAND_PAGE_4K, NAND_ECC_8BIT,  128 /*88*/,   &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_0BIT,  32,           &nand_ecc_default},

	{NAND_PAGE_2K, NAND_ECC_24BIT, 128 /*116*/, &nand_ecc_default},
#ifndef CONFIG_SUPPORT_YAFFS
	{NAND_PAGE_2K, NAND_ECC_16BIT,  64 /*64*/, &nand_ecc_default},
#endif
	{NAND_PAGE_2K, NAND_ECC_8BIT,  64  /*60*/,  &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_0BIT,  32,          &nand_ecc_default},

	{0,		0,		0,	NULL},
};

/*****************************************************************************/
/*
 *  0 - This NAND NOT support randomizer
 *  1 - This NAND support randomizer.
 */
static int hifmc100_nand_support_randomizer(u_int pageisze, u_int ecctype)
{
	switch (pageisze) {
	case _8K:
		return (ecctype >= NAND_ECC_24BIT && ecctype <= NAND_ECC_80BIT);
	case _16K:
		return (ecctype >= NAND_ECC_40BIT && ecctype <= NAND_ECC_80BIT);
	case _32K:
		return (ecctype >= NAND_ECC_40BIT && ecctype <= NAND_ECC_80BIT);
	default:
		return 0;
	}
}

/*****************************************************************************/
/* used the best correct arithmetic. */
static struct nand_config_info *hifmc100_get_best_ecc(
		struct nand_config_info *config, struct mtd_info *mtd)
{
	struct nand_config_info *best = NULL;

	for (; config->layout; config++) {
		if (match_page_type_to_size(config->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < config->oobsize)
			continue;

		if (!best || (best->ecctype < config->ecctype))
			best = config;
	}

	if (!best)
		DB_BUG(ERSTR_DRIVER " the pagesize(%d) and oobsize(%d).\n",
				mtd->writesize, mtd->oobsize);
	return best;
}

/*****************************************************************************/
static void debug_register_dump(struct hifmc_host *host)
{
	int ix;

	pr_info("Register dump:");
	for (ix = 0; ix <= 0x98; ix += 0x04) {
		if (!(ix & 0x0F))
			pr_info("\n0x%08X: ",
					(unsigned int)(host->regbase + ix));
		pr_info("%08X ", hifmc_readl(host, ix));
	}
	pr_info("\n");
}

/*****************************************************************************/
static void hifmc100_check_config(struct hifmc_host *host,
				 struct nand_dev_t *flash_dev_ex,
				 struct nand_config_info *best, int is_hw_auto)
{
	unsigned char page_reg, pagetype, ecc_reg, ecctype;
	int isdump = 0;

	FMC_PR(BT_DBG, "\t  *-Start check best and controller auto config\n");

	page_reg = (host->nand_cfg & PAGE_SIZE_MASK) >> PAGE_SIZE_SHIFT;
	pagetype = match_page_reg_to_type(page_reg);

	ecc_reg = (host->nand_cfg & ECC_TYPE_MASK) >> ECC_TYPE_SHIFT;
	ecctype = match_ecc_reg_to_type(ecc_reg);

	if (pagetype != best->pagetype) {
		if (!isdump++)
			debug_register_dump(host);
		if (is_hw_auto) {
			DB_BUG("!!! warning: Hardware configure pagesize %s" \
				", but the Nand Flash pageszie is %s. Update" \
				" fastboot will resolve this problem.\n",
				match_page_type_to_str(pagetype),
				match_page_type_to_str(best->pagetype));
		} else {
			DB_BUG("!!! warning: Hardware configure pagesize %s" \
				", but the Nand Flash pageszie is %s. Modify" \
				"hardware pin will resolve this problem.\n",
				match_page_type_to_str(pagetype),
				match_page_type_to_str(best->pagetype));
		}
	}

	if (ecctype < best->ecctype) {
		if (!is_hw_auto)
			best->ecctype = ecctype;
	} else if (ecctype > best->ecctype) {
		DB_BUG("!!! warning: Hardware configure ecctype %s, " \
			"but %s is more suitable for this Nand Flash." \
			"Modify hardware pin will resolve this problem.\n",
			match_ecc_type_to_str(ecctype),
			match_ecc_type_to_str(best->ecctype));
	}

	if (IS_NAND_RANDOM(flash_dev_ex) && !IS_NAND_RANDOM(host)) {
		if (is_hw_auto) {
			DB_BUG("Hardware is't configure randomizer, but it "\
				"is more suitable for this Nand Flash. Update"\
				" fastboot will resolve this problem\n");
		} else {
			DB_BUG("Hardware is't configure randomizer, but it" \
				"is more suitable for this Nand Flash. Modify"\
				"hardware pin will resolve this problem.\n");
		}
	}

	FMC_PR(BT_DBG, "\t  *-End check, PageSize: %s, EccType: %s\n",
	match_page_type_to_str(pagetype), match_ecc_type_to_str(ecctype));
}

/*****************************************************************************/
static int hifmc100_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
		struct nand_dev_t *dev)
{
	int regval, buffer_len;
	char *start_type = "unknown";
	struct nand_config_info *best = NULL;
	struct nand_config_info *config = NULL;
	struct hifmc_host *host = chip->priv;
	struct nand_dev_t *nand_dev = &g_nand_dev;

	FMC_PR(BT_DBG, "\t *-Start match PageSize and EccType\n");

	if (IS_NANDC_HW_AUTO(host)) {
		FMC_PR(BT_DBG, "\t |-Get hardware auto config\n");
		config = hifmc100_nand_hw_auto_config_table;

		start_type = "HW-Auto";
		best = hifmc100_get_best_ecc(config, mtd);
	} else {
		FMC_PR(BT_DBG, "\t |-Get config from CPU PIN\n");
		config = hifmc100_nand_hw_pin_config_table;
		start_type = "HW-Pin";
		best = hifmc100_get_best_ecc(config, mtd);
		hifmc100_check_config(host, nand_dev, best, 0);
	}

	nand_dev->flags |= (IS_NANDC_HW_AUTO(host)
				| IS_NANDC_CONFIG_DONE(host));

	if (!best)
		DB_BUG(ERSTR_HARDWARE
			"Please configure Nand Flash pagesize and ecctype!\n");

	if (best->ecctype != NAND_ECC_0BIT)
		mtd->oobsize = best->oobsize;
	mtd->oobavail = HIFMC100_NAND_OOBSIZE_FOR_YAFFS;
	chip->ecc.layout = best->layout;

	host->ecctype = best->ecctype;
	FMC_PR(BT_DBG, "\t |-Save best EccType %d(%s)\n", host->ecctype,
			match_ecc_type_to_str(best->ecctype));
	host->pagesize = match_page_type_to_size(best->pagetype);
	FMC_PR(BT_DBG, "\t |-Save best PageSize %d(%s)\n", host->pagesize,
			match_page_type_to_str(best->pagetype));

	if (hifmc100_nand_support_randomizer(host->pagesize, host->ecctype))
		host->flags |= IS_NAND_RANDOM(nand_dev);

	host->oobsize = mtd->oobsize;
	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);

	buffer_len = host->pagesize + host->oobsize;
	host->buffer = dmam_alloc_coherent(host->dev, buffer_len,
			&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer) {
		DB_MSG("Error: Can't malloc memory for hifmc100 driver.\n");
		return -ENOMEM;
	}
	memset(host->buffer, 0xff, buffer_len);
	host->dma_oob = host->dma_buffer + host->pagesize;

	host->bbm = (unsigned char *)(host->buffer + host->pagesize
			+ HIFMC100_BAD_BLOCK_POS);

	host->epm = (unsigned short *)(host->buffer + host->pagesize
			+ chip->ecc.layout->oobfree[0].offset + 28);

	host->nand_cfg &= ~PAGE_SIZE_MASK;
	host->nand_cfg_ecc0 &= ~PAGE_SIZE_MASK;
	regval = match_page_type_to_reg(best->pagetype);
	host->nand_cfg |= FMC_CFG_PAGE_SIZE(regval);
	host->nand_cfg_ecc0 |= FMC_CFG_PAGE_SIZE(regval);

	host->nand_cfg &= ~ECC_TYPE_MASK;
	host->nand_cfg_ecc0 &= ~ECC_TYPE_MASK;
	regval = match_ecc_type_to_reg(best->ecctype);
	host->nand_cfg |= FMC_CFG_ECC_TYPE(regval);

	/* pass pagesize and ecctype to kernel when spiflash startup. */
	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	if (mtd->writesize > NAND_MAX_PAGESIZE
			|| mtd->oobsize > NAND_MAX_OOBSIZE) {
		DB_BUG(ERSTR_DRIVER
			"Driver does not support this Nand Flash. Please " \
			"increase NAND_MAX_PAGESIZE and NAND_MAX_OOBSIZE.\n");
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

	nand_dev->start_type = start_type;
	dev->start_type = nand_dev->start_type;
	nand_dev->ecctype = host->ecctype;
	dev->ecctype = nand_dev->ecctype;

	host->read_retry = NULL;

	/*
	 * If it want to support the 'read retry' feature, the 'randomizer'
	 * feature must be support first.
	 */
	if (host->read_retry && !IS_NAND_RANDOM(host)) {
		DB_BUG(ERSTR_HARDWARE
			"This Nand flash need to enable 'randomizer' feature. "
			"Please configure hardware randomizer PIN.");
	}

	/*
	 * Check if hardware enable randomizer PIN, But NAND does not need
	 * randomizer. We will notice user.
	 */
	if (IS_NAND_RANDOM(host) &&
	    !hifmc100_nand_support_randomizer(host->pagesize, host->ecctype))
		DB_BUG(ERSTR_HARDWARE
			"This NAND flash does not support `randomizer`, "
			"Please don't configure hardware randomizer PIN.");

	FMC_PR(BT_DBG, "\t *-End match ecctype\n");

	return 0;
}

/*****************************************************************************/
static void hifmc100_chip_init(struct nand_chip *chip)
{
	memset((char *)chip->IO_ADDR_R, 0xff, NAND_BUFFER_LEN);

	chip->read_byte = hifmc100_read_byte;
	chip->read_word = hifmc100_read_word;
	chip->write_buf = hifmc100_write_buf;
	chip->read_buf = hifmc100_read_buf;

	chip->select_chip = hifmc100_select_chip;

	chip->cmd_ctrl = hifmc100_cmd_ctrl;
	chip->dev_ready = hifmc100_dev_ready;

	chip->chip_delay = FMC_CHIP_DELAY;

	chip->options = NAND_NO_AUTOINCR | NAND_NEED_READRDY | NAND_BROKEN_XD
		| NAND_SKIP_BBTSCAN;

	chip->ecc.layout = NULL;
	chip->ecc.mode = NAND_ECC_NONE;
}

/*****************************************************************************/
static int hifmc100_host_init(struct hifmc_host *host)
{
	unsigned int addr, reg, flash_type;

	FMC_PR(BT_DBG, "\t *-Start nand host init\n");

	addr = (unsigned int)host->regbase + FMC_CFG;
	reg = hifmc_readl(host, FMC_CFG);
	FMC_PR(BT_DBG, "\t |-Read FMC CFG[%#x]%#x\n", addr, reg);
	flash_type = GET_SPI_FLASH_TYPE(reg);
	if (flash_type != FLASH_TYPE_NAND) {
		DB_MSG("Error: Flash type isn't Nand flash. reg[%#x]\n", reg);
		reg |= FMC_CFG_FLASH_SEL(FLASH_TYPE_NAND);
		FMC_PR(BT_DBG, "\t |-Change flash type to Nand flash\n");
	}

	if ((reg & FMC_CFG_OP_MODE_MASK) == FMC_CFG_OP_MODE_BOOT) {
		reg |= FMC_CFG_OP_MODE(FMC_CFG_OP_MODE_NORMAL);
		FMC_PR(BT_DBG, "\t |-Controller enter normal mode\n");
	}
	hifmc_writel(host, FMC_CFG, reg);
	FMC_PR(BT_DBG, "\t |-Set CFG[%#x]%#x\n", addr, reg);

	host->nand_cfg = reg;
	host->nand_cfg_ecc0 = (reg & ~ECC_TYPE_MASK) | ECC_TYPE_0BIT;

	addr = (unsigned int)host->regbase + FMC_GLOBAL_CFG;
	reg = hifmc_readl(host, FMC_GLOBAL_CFG);
	FMC_PR(BT_DBG, "\t |-Read global CFG[%#x]%#x\n", addr, reg);
	if (reg & FMC_GLOBAL_CFG_RANDOMIZER_EN) {
		host->flags &= ~NAND_RANDOMIZER;
		FMC_PR(BT_DBG, "\t |-Default disable randomizer\n");
		reg &= ~FMC_GLOBAL_CFG_RANDOMIZER_EN;
		hifmc_writel(host, FMC_GLOBAL_CFG, reg);
		FMC_PR(BT_DBG, "\t |-Set global CFG[%#x]%#x\n", addr, reg);
	}

#ifdef CONFIG_HIFMC100_NAND_EDO_MODE
	/* enable EDO node */
	reg = hifmc_readl(host, FMC_GLOBAL_CFG);
	hifmc_writel(host, FMC_GLOBAL_CFG, SET_NAND_EDO_MODE_EN(reg));
#endif

	host->addr_cycle = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->cache_addr_value[0] = ~0;
	host->cache_addr_value[1] = ~0;

	host->send_cmd_pageprog = hifmc100_send_cmd_write;
	host->send_cmd_status = hifmc100_send_cmd_status;
	host->send_cmd_readstart = hifmc100_send_cmd_read;
	host->send_cmd_erase = hifmc100_send_cmd_erase;
	host->send_cmd_readid = hifmc100_send_cmd_readid;
	host->send_cmd_reset = hifmc100_send_cmd_reset;

	/*
	 * check if start from nand.
	 * This register REG_SYSSTAT is set in start.S
	 * When start in NAND (Auto), the ECC/PAGESIZE driver don't detect.
	 */
	host->flags |= NANDC_HW_AUTO;

	if (GET_SYS_BOOT_MODE(reg) == BOOT_FROM_NAND) {
		host->flags |= NANDC_CONFIG_DONE;
		FMC_PR(BT_DBG, "\t |-Auto config pagesize and ecctype\n");
	}

	host->enable_ecc_randomizer = hifmc100_ecc_randomizer;

	FMC_PR(BT_DBG, "\t *-End nand host init\n");

	return 0;
}

/*****************************************************************************/
int hifmc100_nand_init(struct nand_chip *chip)
{
	struct hifmc_host *host = chip->priv;

	/* enable and set system clock */
	clk_prepare_enable(host->clk);

	/* fmc ip version check */
	host->version = hifmc_readl(host, FMC_VERSION);
	if (host->version != HIFMC_VER_100)
		return -EFAULT;
	pr_info("Found Flash Memory Controller v100 Nand Driver\n");

	/* hifmc host init */
	if (hifmc100_host_init(host)) {
		DB_MSG("Error: Nand host init failed!\n");
		return -EFAULT;
	}
	host->chip = chip;

	hifmc_writel(host, FMC_PND_PWIDTH_CFG, PWIDTH_CFG_RW_HCNT(CONFIG_RW_H_WIDTH)
			| PWIDTH_CFG_R_LCNT(CONFIG_R_L_WIDTH)
			| PWIDTH_CFG_W_LCNT(CONFIG_W_L_WIDTH));

	/* hifmc nand_chip struct init */
	hifmc100_chip_init(chip);

	hifmc_spl_ids_register();
	hinfc_param_adjust = hifmc100_ecc_probe;

	return 0;
}

#ifdef CONFIG_PM
/*****************************************************************************/
void hifmc100_nand_config(struct hifmc_host *host)
{
	/* enable system clock */
	clk_prepare_enable(host->clk);
	FMC_PR(PM_DBG, "\t |-enable system clock\n");
}
#endif  /* CONFIG_PM */
