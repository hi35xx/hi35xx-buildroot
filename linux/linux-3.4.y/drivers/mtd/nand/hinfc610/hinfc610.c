/******************************************************************************
 *    COPYRIGHT (C) 2013 Czyong. Hisilicon
 *    All rights reserved.
 * ***
 *    Create by Czyong 2013-02-07
 *
******************************************************************************/

#define pr_fmt(fmt) "hinfc610: " fmt

#include "hinfc610_os.h"
#include "hinfc610.h"
#include "hinfc610_dbg_inf.h"
#include "hinfc610_gen.h"
#include "hinfc620_gen.h"
#include "hinfc610_sync.h"
#include "hinfc610_read_retry.h"

/*****************************************************************************/
#ifdef CONFIG_ARCH_HI3516A
#include "hinfc610_3516a.c"
#endif
/*****************************************************************************/

static unsigned int get_8bits(unsigned char byte)
{
	int ix = 0;
	int num = 0;

	if (byte == 0xFF)
		return 8;
	if (!byte)
		return 0;

	while (ix++ < 8) {
		if ((byte & 1))
			num++;
		byte = (byte >> 1);
	}
	return num;
}
/*****************************************************************************/

static unsigned int get_16bits(unsigned short byte)
{
	int ix = 0;
	int num = 0;

	if (byte == 0xFFFF)
		return 16;
	if (!byte)
		return 0;

	while (ix++ < 16) {
		if ((byte & 1))
			num++;
		byte = (byte >> 1);
	}
	return num;
}
/*****************************************************************************/

static void hinfc610_dma_transfer(struct hinfc_host *host, int todev)
{
	unsigned long reg_val;
	unsigned int dma_addr = (unsigned int)host->dma_buffer;

	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA1);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA2);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA3);

	/* 32K PAGESIZE need below. */
	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA4);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA5);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA6);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA7);

	hinfc_write(host, host->dma_oob, HINFC610_DMA_ADDR_OOB);

	if (host->ecctype == NAND_ECC_NONE) {
		hinfc_write(host,
			((host->oobsize & HINFC610_DMA_LEN_OOB_MASK)
			 << HINFC610_DMA_LEN_OOB_SHIFT),
			HINFC610_DMA_LEN);

		hinfc_write(host,
			HINFC610_DMA_PARA_DATA_RW_EN
			| HINFC610_DMA_PARA_OOB_RW_EN,
			HINFC610_DMA_PARA);
	} else
		hinfc_write(host,
			HINFC610_DMA_PARA_DATA_RW_EN
			| HINFC610_DMA_PARA_OOB_RW_EN
			| HINFC610_DMA_PARA_DATA_EDC_EN
			| HINFC610_DMA_PARA_OOB_EDC_EN,
			HINFC610_DMA_PARA);

	reg_val = (HINFC610_DMA_CTRL_DMA_START
		| HINFC610_DMA_CTRL_BURST4_EN
		| HINFC610_DMA_CTRL_BURST8_EN
		| HINFC610_DMA_CTRL_BURST16_EN
		| ((host->addr_cycle == 4 ? 1 : 0)
		   << HINFC610_DMA_CTRL_ADDR_NUM_SHIFT)
		| ((host->chipselect & HINFC610_DMA_CTRL_CS_MASK)
		   << HINFC610_DMA_CTRL_CS_SHIFT));

	if (todev)
		reg_val |= HINFC610_DMA_CTRL_WE;

	hinfc_write(host, reg_val, HINFC610_DMA_CTRL);

	do {
		unsigned int timeout = 0xF0000000;
		while ((hinfc_read(host, HINFC610_DMA_CTRL))
			& HINFC610_DMA_CTRL_DMA_START && timeout) {
			_cond_resched();
			timeout--;
		}
		if (!timeout)
			PR_BUG("Wait DMA finish timeout.\n");
	} while (0);
}
/*****************************************************************************/

static void hinfc610_sync_entry(struct hinfc_host *host)
{
	struct nand_sync *sync = host->sync;
	struct nand_chip *chip = host->chip;

	if (!sync) {
		PR_BUG("this NAND not support sync feature.\n");
		return;
	}

	if (HINFC610_IS_SYNC(host)) {
		PR_BUG("this NAND not support sync feature.\n");
		return;
	}

	if (sync->enable)
		sync->enable(chip);

	hinfc610_controller_enable(host, ENABLE);

	switch (sync->type) {
	case NAND_TYPE_TOGGLE_10:
		host->NFC_CON |= HINFC610_CON_NF_MODE_TOGGLE;
		host->NFC_CON_ECC_NONE |= HINFC610_CON_NF_MODE_TOGGLE;
		break;

	case NAND_TYPE_ONFI_23:
		host->NFC_CON |= HINFC610_CON_NF_MODE_ONFI_23;
		host->NFC_CON_ECC_NONE |= HINFC610_CON_NF_MODE_ONFI_23;
		break;

	case NAND_TYPE_ONFI_30:
		host->NFC_CON |= HINFC610_CON_NF_MODE_ONFI_30;
		host->NFC_CON_ECC_NONE |= HINFC610_CON_NF_MODE_ONFI_30;
		break;

	default:
		PR_BUG("Unsupport sync type 0x%08X.\n", sync->type);
		break;
	}
}
/*****************************************************************************/

static void hinfc610_sync_exit(struct hinfc_host *host)
{
	struct nand_sync *sync = host->sync;
	struct nand_chip *chip = host->chip;

	if (!HINFC610_IS_SYNC(host)) {
		PR_BUG("Current already exit from sync feature.\n");
		return;
	}

	if (sync->disable)
		sync->disable(chip);

	host->NFC_CON &= ~HINFC610_CON_NF_MODE_MASK;
	host->NFC_CON_ECC_NONE &= ~HINFC610_CON_NF_MODE_MASK;

	hinfc610_controller_enable(host, DISABLE);
}
/*****************************************************************************/

void hinfc610_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	int is_cache_invalid = 1;
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (ctrl & NAND_ALE) {
		unsigned int addr_value = 0;
		unsigned int addr_offset = 0;

		if (ctrl & NAND_CTRL_CHANGE) {
			host->addr_cycle = 0x0;
			host->addr_value[0] = 0x0;
			host->addr_value[1] = 0x0;
		}
		addr_offset = host->addr_cycle << 3;

		if (host->addr_cycle >= HINFC610_ADDR_CYCLE_MASK) {
			addr_offset =
			    (host->addr_cycle - HINFC610_ADDR_CYCLE_MASK) << 3;
			addr_value = 1;
		}

		host->addr_value[addr_value] |=
			((dat & 0xff) << addr_offset);

		host->addr_cycle++;
	}

	if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE)) {
		host->command = dat & 0xff;
		switch (host->command) {
		case NAND_CMD_PAGEPROG:
			host->send_cmd_pageprog(host);
			hinfc610_dbg_write(host);
			break;

		case NAND_CMD_READSTART:
			is_cache_invalid = 0;
			host->send_cmd_readstart(host);
			hinfc610_dbg_read(host);

			break;

		case NAND_CMD_ERASE2:
			host->send_cmd_erase(host);
			hinfc610_dbg_erase(host);

			break;

		case NAND_CMD_READID:
			memset((unsigned char *)(chip->IO_ADDR_R), 0, 0x10);
			host->send_cmd_readid(host);
			break;

		case NAND_CMD_STATUS:
			host->send_cmd_status(host);
			break;

		case NAND_CMD_SEQIN:
		case NAND_CMD_ERASE1:
		case NAND_CMD_READ0:
			break;
		case NAND_CMD_RESET:
			host->send_cmd_reset(host, host->chipselect);
			break;

		default:
			break;
		}
	}

	if ((dat == NAND_CMD_NONE) && host->addr_cycle) {
		if (host->command == NAND_CMD_SEQIN ||
		    host->command == NAND_CMD_READ0 ||
		    host->command == NAND_CMD_READID) {
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

static int hinfc610_send_cmd_pageprog(struct hinfc_host *host)
{
	if (*host->bbm != 0xFF && *host->bbm != 0x00)
		pr_warn("Attempt to write an invalid bbm. page: 0x%08x, mark: 0x%02x, current process(pid): %s(%d).\n",
			GET_PAGE_INDEX(host), *host->bbm,
			current->comm, current->pid);

	if (IS_NAND_SYNC_ASYNC(host))
		hinfc610_sync_entry(host);

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC610_ADDRL);
	hinfc_write(host, host->addr_value[1], HINFC610_ADDRH);
	hinfc_write(host,
		((NAND_CMD_STATUS << 16) | (NAND_CMD_PAGEPROG << 8) |
		 NAND_CMD_SEQIN),
		HINFC610_CMD);

	*host->epm = 0x0000;

	hinfc610_dma_transfer(host, 1);

	if (IS_NAND_SYNC_ASYNC(host))
		hinfc610_sync_exit(host);

	return 0;
}
/*****************************************************************************/

static int hinfc610_get_data_status(struct hinfc_host *host)
{
	unsigned int page_status = 0;

	if (IS_PS_UN_ECC(host))
		page_status = HINFC610_PS_UC_ECC;

	/* this is block start address */
	if (!((host->addr_value[0] >> 16) & host->block_page_mask)) {

		/* it is a bad block */
		if (*host->bbm == 0x00) {
			page_status |= HINFC610_PS_BAD_BLOCK;
			goto out;
		}

		if (*host->bbm != 0xFF) {
			page_status |= HINFC610_PS_BBM_ERROR;

			/*
			 * if there are more than 2 bits flipping, it is
			 * maybe a bad block
			 */
			if (!IS_PS_UN_ECC(host) || get_8bits(*host->bbm) < 6) {
				page_status |= HINFC610_PS_BAD_BLOCK;
				goto out;
			}
		}
	}

	if (*host->epm == 0x0000)
		goto out;

	if (*host->epm == 0xFFFF) {
		page_status |= HINFC610_PS_EMPTY_PAGE;
		goto out;
	}

	page_status |= HINFC610_PS_EPM_ERROR;

	if (IS_PS_UN_ECC(host) && get_16bits(*host->epm) > 12) {
		page_status |= HINFC610_PS_EMPTY_PAGE;
		goto out;
	}

out:
	return page_status;
}
/*****************************************************************************/

static int hinfc610_do_read_retry(struct hinfc_host *host)
{
	int ix;

	for (ix = 1; IS_PS_UN_ECC(host) && ix < host->read_retry->count; ix++) {

		hinfc_write(host, HINFC610_INTCLR_UE | HINFC610_INTCLR_CE,
			    HINFC610_INTCLR);

		host->enable_ecc_randomizer(host, DISABLE, DISABLE);
		host->read_retry->set_rr_param(host, ix);

		/* enable ecc and randomizer */
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);

		hinfc_write(host, HINFC610_INTCLR_UE | HINFC610_INTCLR_CE,
			    HINFC610_INTCLR);
		hinfc_write(host, host->NFC_CON, HINFC610_CON);
		hinfc_write(host, host->addr_value[0] & 0xffff0000,
			    HINFC610_ADDRL);
		hinfc_write(host, host->addr_value[1], HINFC610_ADDRH);
		hinfc_write(host,
			    HINFC_CMD_SEQ(NAND_CMD_READ0, NAND_CMD_READSTART),
			    HINFC610_CMD);

		hinfc610_dma_transfer(host, 0);

		if (hinfc_read(host, HINFC610_INTS) & HINFC610_INTS_UE)
			host->page_status |= HINFC610_PS_UC_ECC;
		else
			host->page_status &= ~HINFC610_PS_UC_ECC;
	}

	host->page_status = hinfc610_get_data_status(host);

	hinfc610_dbg_read_retry(host, ix);

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);
	host->read_retry->reset_rr_param(host);

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_readstart(struct hinfc_host *host)
{
	if ((host->addr_value[0] == host->cache_addr_value[0]) &&
	    (host->addr_value[1] == host->cache_addr_value[1]))
		return 0;

	if (IS_NAND_SYNC_ASYNC(host))
		hinfc610_sync_entry(host);

	host->page_status = 0;

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	hinfc_write(host, HINFC610_INTCLR_UE | HINFC610_INTCLR_CE,
		    HINFC610_INTCLR);
	hinfc_write(host, host->NFC_CON, HINFC610_CON);
	hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC610_ADDRL);
	hinfc_write(host, host->addr_value[1], HINFC610_ADDRH);
	hinfc_write(host, NAND_CMD_READSTART << 8 | NAND_CMD_READ0,
		    HINFC610_CMD);

	hinfc610_dma_transfer(host, 0);

	if (hinfc_read(host, HINFC610_INTS) & HINFC610_INTS_UE)
		host->page_status |= HINFC610_PS_UC_ECC;

	if (host->read_retry || IS_NAND_RANDOM(host)) {
		host->page_status |= hinfc610_get_data_status(host);

		if (IS_PS_EMPTY_PAGE(host)) {
			/*
			 * oob area used by yaffs2 only 32 bytes,
			 * so we only fill 32 bytes.
			 */
			if (IS_NAND_RANDOM(host))
				memset(host->buffer, 0xFF,
				       host->pagesize + host->oobsize);

		} else if (!IS_PS_BAD_BLOCK(host)) {
			/* if NAND chip support read retry */
			if (IS_PS_UN_ECC(host) && host->read_retry)
				hinfc610_do_read_retry(host);

		} /* 'else' NAND have a bad block, do nothing. */
	}

	if (IS_NAND_SYNC_ASYNC(host))
		hinfc610_sync_exit(host);

	host->cache_addr_value[0] = host->addr_value[0];
	host->cache_addr_value[1] = host->addr_value[1];

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_erase(struct hinfc_host *host)
{
	unsigned int regval;

	/* Don't case the read retry config */
	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, host->addr_value[0], HINFC610_ADDRL);
	hinfc_write(host, (NAND_CMD_ERASE2 << 8) | NAND_CMD_ERASE1,
		HINFC610_CMD);

	regval = HINFC610_OP_WAIT_READY_EN
		 | HINFC610_OP_CMD2_EN
		 | HINFC610_OP_CMD1_EN
		 | HINFC610_OP_ADDR_EN
		 | ((host->chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT)
		 | ((host->addr_cycle & HINFC610_OP_ADDR_CYCLE_MASK)
		    << HINFC610_OP_ADDR_CYCLE_SHIFT);

	hinfc_write(host, regval, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_sync_readid(struct hinfc_host *host)
{
	unsigned int regval;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, HINFC610_NANDINFO_LEN, HINFC610_DATA_NUM);
	hinfc_write(host, NAND_CMD_READID, HINFC610_CMD);
	hinfc_write(host, 0, HINFC610_ADDRL);

	/* no need to config HINFC610_OP_WAIT_READY_EN, here not config. */
	regval = HINFC610_OP_CMD1_EN
		 | HINFC610_OP_ADDR_EN
		 | HINFC610_OP_READ_DATA_EN
		 | ((host->chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT)
		 | (1 << HINFC610_OP_ADDR_CYCLE_SHIFT)
		 | HINFC610_OP_READID_EN
		 | HINFC610_OP_RW_REG_EN;

	hinfc_write(host, regval, HINFC610_OP);

	host->addr_cycle = 0x0;

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_async_readid(struct hinfc_host *host)
{
	unsigned int regval;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, HINFC610_NANDINFO_LEN, HINFC610_DATA_NUM);
	hinfc_write(host, NAND_CMD_READID, HINFC610_CMD);
	hinfc_write(host, 0, HINFC610_ADDRL);

	/* no need to config HINFC610_OP_WAIT_READY_EN, here not config. */
	regval = HINFC610_OP_CMD1_EN
		 | HINFC610_OP_ADDR_EN
		 | HINFC610_OP_READ_DATA_EN
		 | ((host->chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT)
		 | (1 << HINFC610_OP_ADDR_CYCLE_SHIFT);

	hinfc_write(host, regval, HINFC610_OP);

	host->addr_cycle = 0x0;

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_readid(struct hinfc_host *host)
{
	if (HINFC610_IS_SYNC(host))
		return hinfc610_send_cmd_sync_readid(host);
	else
		return hinfc610_send_cmd_async_readid(host);
}
/*****************************************************************************/

static int hinfc610_enable_ecc_randomizer(struct hinfc_host *host, int ecc_en,
					  int randomizer_en)
{
	unsigned int nfc_con;

	if (IS_NAND_RANDOM(host)) {
		if (randomizer_en) {
			host->NFC_CON |= HINFC610_CON_RANDOMIZER_EN;
			host->NFC_CON_ECC_NONE |= HINFC610_CON_RANDOMIZER_EN;
		} else {
			host->NFC_CON &= ~HINFC610_CON_RANDOMIZER_EN;
			host->NFC_CON_ECC_NONE &= ~HINFC610_CON_RANDOMIZER_EN;
		}
	}

	nfc_con = (ecc_en ? host->NFC_CON : host->NFC_CON_ECC_NONE);

	hinfc_write(host, nfc_con, HINFC610_CON);

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_status(struct hinfc_host *host)
{
	unsigned int regval;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, HINFC610_NANDINFO_LEN, HINFC610_DATA_NUM);
	hinfc_write(host, NAND_CMD_STATUS, HINFC610_CMD);

	/* no need config HINFC610_OP_WAIT_READY_EN, here not config */
	regval = HINFC610_OP_CMD1_EN
		 | HINFC610_OP_READ_DATA_EN
		 | ((host->chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT);

	hinfc_write(host, regval, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_async_reset(struct hinfc_host *host,
					 int chipselect)
{
	unsigned int regval;

	hinfc_write(host, NAND_CMD_RESET, HINFC610_CMD);

	/* need to config HINFC610_OP_WAIT_READY_EN */
	regval = HINFC610_OP_CMD1_EN
		 | (((chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT)
		 | HINFC610_OP_WAIT_READY_EN);

	hinfc_write(host, regval, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_sync_reset(struct hinfc_host *host,
					int chipselect)
{
	unsigned int regval;

	/*
	 * Regarding the ONFI chip sync mode,
	 * NAND_CMD_SYNC_RESET make chip remain sync mode.
	 * But NAND_CMD_RESET will change chip mode to async mode.
	 */
	hinfc_write(host, NAND_CMD_SYNC_RESET, HINFC610_CMD);

	/* need to config HINFC610_OP_WAIT_READY_EN */
	regval = HINFC610_OP_CMD1_EN
		 | ((chipselect & HINFC610_OP_NF_CS_MASK)
		     << HINFC610_OP_NF_CS_SHIFT)
		 | HINFC610_OP_WAIT_READY_EN;

	hinfc_write(host, regval, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_reset(struct hinfc_host *host, int chipselect)
{
	if (HINFC610_IS_SYNC(host))
		return hinfc610_send_cmd_sync_reset(host, chipselect);
	else
		return hinfc610_send_cmd_async_reset(host, chipselect);
}
/*****************************************************************************/

int hinfc610_dev_ready(struct mtd_info *mtd)
{
	return 0x1;
}
/*****************************************************************************/

void hinfc610_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_HINFC610_MAX_CHIP)
		PR_BUG("invalid chipselect: %d\n", chipselect);

	host->chipselect = chipselect;
}
/*****************************************************************************/

uint8_t hinfc610_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (host->command == NAND_CMD_STATUS)
		return readb(chip->IO_ADDR_R);

	host->offset++;

	if (host->command == NAND_CMD_READID)
		return readb(chip->IO_ADDR_R + host->offset - 1);

	return readb(host->buffer + host->column + host->offset - 1);
}
/*****************************************************************************/

u16 hinfc610_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	host->offset += 2;
	return readw(host->buffer + host->column + host->offset - 2);
}
/*****************************************************************************/

void hinfc610_write_buf(struct mtd_info *mtd, const uint8_t *buf,
			       int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(host->buffer + host->column + host->offset, buf, len);
	host->offset += len;
}
/*****************************************************************************/

void hinfc610_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(buf, host->buffer + host->column + host->offset, len);
	host->offset += len;
}
/*****************************************************************************/
/*
 * 'host->epm' only use the first oobfree[0] field, it looks very simple, But...
 */
static struct nand_ecclayout nand_ecc_default = {
	.oobfree = {{2, 30} }
};

/*****************************************************************************/

static struct nand_config_info hinfc610_soft_auto_config_table[] = {
	{NAND_PAGE_16K, NAND_ECC_64BIT, 1824/*1824*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_40BIT, 1200/*1152*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_NONE,  32 ,          &nand_ecc_default},

	{NAND_PAGE_8K, NAND_ECC_64BIT, 928 /*928*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_40BIT, 600 /*592*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_24BIT, 368 /*368*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_NONE,  32,           &nand_ecc_default},

	{NAND_PAGE_4K, NAND_ECC_24BIT, 200 /*200*/,  &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_4BIT_512,  128  /*88*/,   &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_NONE,  32,           &nand_ecc_default},

	{NAND_PAGE_2K, NAND_ECC_24BIT, 128 /*116*/, &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_4BIT_512,  64  /*60*/,  &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_NONE,  32,          &nand_ecc_default},

	{0, 0, 0, NULL},

};
/*****************************************************************************/
/* used the best correct arithmetic. */
struct nand_config_info *hinfc610_get_best_ecc(struct mtd_info *mtd)
{
	struct nand_config_info *best = NULL;
	struct nand_config_info *config = hinfc610_soft_auto_config_table;

	for (; config->layout; config++) {
		if (nandpage_type2size(config->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < config->oobsize)
			continue;

		if (!best || (best->ecctype < config->ecctype))
			best = config;
	}

	if (!best)
		PR_BUG(ERSTR_DRIVER
		       "Driver does not support the pagesize(%d) "
		       "and oobsize(%d).\n",
		       mtd->writesize, mtd->oobsize);

	return best;
}
/*****************************************************************************/
/* force the pagesize and ecctype */
struct nand_config_info *hinfc610_force_ecc(struct mtd_info *mtd, int pagetype,
					     int ecctype, char *cfgmsg,
					     int allow_pagediv)
{
	int pagesize;

	struct nand_config_info *fit = NULL;
	struct nand_config_info *config = hinfc610_soft_auto_config_table;

	for (; config->layout; config++) {
		if (config->pagetype == pagetype
			&& config->ecctype == ecctype) {
			fit = config;
			break;
		}
	}

	if (!fit) {
		PR_BUG(ERSTR_DRIVER
		       "Driver(%s mode) does not support this Nand Flash "
		       "pagesize:%s, ecctype:%s\n",
		       cfgmsg,
		       nand_page_name(pagetype),
		       nand_ecc_name(ecctype));
		return NULL;
	}

	pagesize = nandpage_type2size(pagetype);
	if ((pagesize != mtd->writesize)
		&& (pagesize > mtd->writesize || !allow_pagediv)) {
		PR_BUG(ERSTR_HARDWARE
		       "Hardware (%s mode) configure pagesize %d, "
		       "but the Nand Flash pageszie is %d\n",
		       cfgmsg,
		       pagesize,
		       mtd->writesize);
		return NULL;
	}

	if (fit->oobsize > mtd->oobsize) {
		PR_BUG(ERSTR_HARDWARE
		       "(%s mode) The Nand Flash offer space area is %d bytes, "
		       "but the controller request %d bytes in ecc %s. "
		       "Please make sure the hardware ECC configuration is correct.",
		       cfgmsg, mtd->oobsize, fit->oobsize,
		       nand_ecc_name(ecctype));
		return NULL;
	}

	return fit;
}
/*****************************************************************************/
static unsigned int  nand_otp_len;
static unsigned char nand_otp[128] = {0};

/* Get NAND parameter table. */
static int __init parse_nand_param(const struct tag *tag)
{
	if (tag->hdr.size <= 2)
		return 0;

	nand_otp_len = ((tag->hdr.size << 2) - sizeof(struct tag_header));

	if (nand_otp_len > sizeof(nand_otp)) {
		printk(KERN_WARNING "%s(%d): Get Nand OTP from tag fail.\n",
			__func__, __LINE__);
		return 0;
	}
	memcpy(nand_otp, &tag->u, nand_otp_len);
	return 0;
}
/* 0x48694E77 equal to fastoot ATAG_NAND_PARAM */
__tagtable(0x48694E77, parse_nand_param);

/*****************************************************************************/
int hinfc610_ecc_type2reg_intf(int type, struct hinfc_host *host)
{
	if (HINFC_VER_620 == host->version)
		return hinfc620_ecc_type2reg(type);
	else
		return hinfc610_ecc_type2reg(type);
}
/*****************************************************************************/
int hinfc610_ecc_reg2type_intf(int reg, struct hinfc_host *host)
{
	if (HINFC_VER_620 == host->version)
		return hinfc620_ecc_reg2type(reg);
	else
		return hinfc610_ecc_reg2type(reg);
}

/*****************************************************************************/

static int hinfc610_param_adjust(struct mtd_info *mtd, struct nand_chip *chip,
				 struct nand_flash_dev_ex *nand_dev)
{
	int pagetype;
	int ecctype;
	int regval;
	char *start_type = "unknown";
	struct nand_config_info *best = NULL;
	struct hinfc_host *host = chip->priv;

	if (IS_NANDC_HW_AUTO(host))
		start_type = "HW-Auto";
	else
		start_type = "HW-Reg";

	pagetype = hinfc610_page_reg2type((host->NFC_CON
				>> HINFC610_CON_PAGEISZE_SHIFT)
		& HINFC610_CON_PAGESIZE_MASK);

	ecctype = hinfc610_ecc_reg2type_intf((host->NFC_CON
				>> HINFC610_CON_ECCTYPE_SHIFT)
		& HINFC610_CON_ECCTYPE_MASK, host);

	best = hinfc610_force_ecc(mtd, pagetype, ecctype,
		start_type, 0);

#ifdef CONFIG_HINFC610_PAGESIZE_AUTO_ECC_NONE
#  ifdef CONFIG_HINFC610_AUTO_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC610_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HINFC610_AUTO_PAGESIZE_ECC at the same time
#  endif
#  ifdef CONFIG_HINFC610_HARDWARE_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC610_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HINFC610_HARDWARE_PAGESIZE_ECC at the same time
#  endif

	pagetype = nandpage_size2type(mtd->writesize);
	ecctype = NAND_ECC_NONE;
	best = hinfc610_force_ecc(mtd, pagetype, ecctype,
				  "force config", 0);
	start_type = "AutoForce";

#endif /* CONFIG_HINFC610_PAGESIZE_AUTO_ECC_NONE */

	if (!best)
		PR_BUG(ERSTR_HARDWARE
		       "Please configure Nand Flash pagesize and ecctype!\n");

	/* only in case fastboot check randomizer failed.
	 * Update fastboot or configure hardware randomizer pin
	 * fix this problem.
	 */
	if (IS_NAND_RANDOM(nand_dev) && !(IS_NAND_RANDOM(host)))
		PR_BUG(ERSTR_HARDWARE
		       "Hardware is not configure randomizer, "
		       "but it is more suitable for this Nand Flash. "
		       "1. Please configure hardware randomizer PIN."
		       "2. Please updata fastboot.\n");

	host->flags |= (IS_NAND_RANDOM(nand_dev) |
		IS_NAND_SYNC_ASYNC(nand_dev) |
		IS_NAND_ONLY_SYNC(nand_dev) |
		IS_NAND_ONFI(nand_dev));

	/* only for print nand info. */
	nand_dev->flags |= (IS_NANDC_HW_AUTO(host) |
		IS_NANDC_SYNC_BOOT(host));

	/* only in case fastboot check sync boot pin failed.
	 * Update fastboot or configure hardware sync boot pin fix this problem.
	 */
	if (IS_NANDC_SYNC_BOOT(host)) {
		/* But NAND do not support sync mode, warning ! */
		if (!IS_NAND_ONLY_SYNC(nand_dev))
			PR_BUG(ERSTR_HARDWARE
				"Hardware SYNC BOOT PIN has configured sync mode, "
				"but the Nand Flash is async mode.\n"
				"1. DO NOT configure SYNC BOOT PIN. "
				"2. Update fastboot.\n");
	} else {
		if (IS_NAND_ONLY_SYNC(nand_dev))
			PR_BUG(ERSTR_HARDWARE
				"Hardware SYNC BOOT PIN has configured async mode, "
				"but the Nand Flash only support sync mode.\n"
				"1. Please configure SYNC BOOT PIN."
				"2. Update fastboot.\n");
	}

	if (IS_NAND_SYNC_ASYNC(nand_dev))
		hinfc610_get_sync_info(host);

	if (best->ecctype != NAND_ECC_NONE)
		mtd->oobsize = best->oobsize;
	chip->ecc.layout = best->layout;

	host->ecctype  = best->ecctype;
	host->pagesize = nandpage_type2size(best->pagetype);
	host->oobsize  = mtd->oobsize;
	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);

	host->buffer = dma_alloc_coherent(host->dev,
		(host->pagesize + host->oobsize),
		&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer) {
		PR_BUG("Can't malloc memory for NAND driver.");
		return -EIO;
	}
	memset(host->buffer, 0xff, (host->pagesize + host->oobsize));

	host->dma_oob = host->dma_buffer + host->pagesize;
	host->bbm = (unsigned char *)(host->buffer
		+ host->pagesize + HINFC_BAD_BLOCK_POS);

	host->epm = (unsigned short *)(host->buffer
		+ host->pagesize + chip->ecc.layout->oobfree[0].offset + 28);

	regval = ~(HINFC610_CON_PAGESIZE_MASK << HINFC610_CON_PAGEISZE_SHIFT);
	host->NFC_CON &= regval;
	host->NFC_CON_ECC_NONE &= regval;
	regval = (hinfc610_page_type2reg(best->pagetype)
		& HINFC610_CON_PAGESIZE_MASK) << HINFC610_CON_PAGEISZE_SHIFT;
	host->NFC_CON |= regval;
	host->NFC_CON_ECC_NONE |= regval;

	regval = ~(HINFC610_CON_ECCTYPE_MASK << HINFC610_CON_ECCTYPE_SHIFT);
	host->NFC_CON &= regval;
	host->NFC_CON_ECC_NONE &= regval;
	regval = (hinfc610_ecc_type2reg_intf(best->ecctype, host)
		& HINFC610_CON_ECCTYPE_MASK) << HINFC610_CON_ECCTYPE_SHIFT;
	host->NFC_CON |= regval;

	if (mtd->writesize > NAND_MAX_PAGESIZE ||
	    mtd->oobsize > NAND_MAX_OOBSIZE) {
		PR_BUG(ERSTR_DRIVER
		       "Driver does not support this Nand Flash. "
		       "Please increase NAND_MAX_PAGESIZE and NAND_MAX_OOBSIZE.\n");
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
		PR_MSG("Nand divide into 1/%u\n", (1 << shift));
	}

	nand_dev->start_type = start_type;
	nand_dev->ecctype = host->ecctype;

	host->read_retry = NULL;
	if (nand_dev->read_retry_type != NAND_RR_NONE) {
		host->read_retry
		    = hinfc610_find_read_retry(nand_dev->read_retry_type);
		if (!host->read_retry) {
			PR_BUG(ERSTR_DRIVER
			       "This Nand Flash need to enable the "
			       "'read retry' feature. "
			       "but the driver dose not offer the feature");
		}

		if (nand_otp_len)
			memcpy(host->rr_data, nand_otp, nand_otp_len);
	}

	/*
	 * If it want to support the 'read retry' feature, the 'randomizer'
	 * feature must be support first.
	 */
	if (host->read_retry && !IS_NAND_RANDOM(host)) {
		PR_BUG(ERSTR_HARDWARE
		       "This Nand flash need to enable 'randomizer' feature. "
		       "Please configure hardware randomizer PIN.");
	}

	hinfc610_dbg_init(host);

	return 0;
}
/*****************************************************************************/

int hinfc610_nand_init(struct hinfc_host *host, struct nand_chip *chip)
{
	unsigned int regval;
	host->version = hinfc_read(host, HINFC610_VERSION);

	host->addr_cycle    = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->cache_addr_value[0] = ~0;
	host->cache_addr_value[1] = ~0;
	host->chipselect    = 0;

	host->send_cmd_pageprog = hinfc610_send_cmd_pageprog;
	host->send_cmd_readstart = hinfc610_send_cmd_readstart;
	host->send_cmd_erase = hinfc610_send_cmd_erase;
	host->send_cmd_readid = hinfc610_send_cmd_readid;
	host->send_cmd_status = hinfc610_send_cmd_status;
	host->send_cmd_reset = hinfc610_send_cmd_reset;

	host->flags = 0;

	regval = hinfc_read(host, HINFC610_CON);
	if (regval & HINFC610_CON_RANDOMIZER_EN)
		host->flags |= NAND_RANDOMIZER;

	host->NFC_CON = (regval
		| HINFC610_CON_OP_MODE_NORMAL
		| HINFC610_CON_READY_BUSY_SEL);

	host->NFC_CON_ECC_NONE = host->NFC_CON
		& (~(HINFC610_CON_ECCTYPE_MASK
		     << HINFC610_CON_ECCTYPE_SHIFT))
		& (~HINFC610_CON_RANDOMIZER_EN);

	hinfc_write(host,
		(SET_HINFC610_PWIDTH(CONFIG_HINFC610_W_LATCH,
				     CONFIG_HINFC610_R_LATCH,
				     CONFIG_HINFC610_RW_LATCH)),
		HINFC610_PWIDTH);
	regval = hinfc_read(host, HINFC610_BOOT_CFG);
	if (regval & HINFC610_BOOT_CFG_SAVE_PIN_MODE)
		host->flags |= NANDC_HW_AUTO;

	/* check if chip is sync mode. */
	if (regval & HINFC610_BOOT_CFG_SYC_NAND_PAD) {
		host->flags |= NANDC_IS_SYNC_BOOT;

		/*
		 * NAND default is sync mode, and read id, reset in sync mode.
		 */
		host->NFC_CON |= HINFC610_CON_NF_MODE_TOGGLE;
		host->NFC_CON_ECC_NONE |= HINFC610_CON_NF_MODE_TOGGLE;

		/* set synchronous clock and timing. */
		hinfc610_controller_enable(host, ENABLE);
	}

	memset((char *)chip->IO_ADDR_R,
		0xff, HINFC610_BUFFER_BASE_ADDRESS_LEN);



	host->enable_ecc_randomizer = hinfc610_enable_ecc_randomizer;
	nand_oob_resize = hinfc610_param_adjust;

	return 0;
}
