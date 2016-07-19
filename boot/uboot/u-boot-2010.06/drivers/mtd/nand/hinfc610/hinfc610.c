/******************************************************************************
 *    NAND Flash Controller V610 Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 *
 *    Create By Hisilicon.
 *
******************************************************************************/

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <malloc.h>
#include <linux/mtd/nand.h>
#include "hinfc610_gen.h"
#include "hinfc620_gen.h"

#ifdef CONFIG_ARCH_MMU
#include <asm/cache-cp15.h>
#endif

#include "hinfc610.h"

/*****************************************************************************/
extern void hinfc610_controller_enable(struct hinfc_host *host, int enable);
extern void nand_spl_ids_register(void);

/*****************************************************************************/
unsigned int nand_ip_version = HINFC_VER_610;
static struct hinfc_host host = {
	.chip = NULL,
};
/*****************************************************************************/

#ifdef CONFIG_HI3516A
	#include "hinfc610_hi3516a.c"
#endif

#ifdef CONFIG_HI3536
	#include "hinfc610_hi3536.c"
#endif

#ifdef CONFIG_HI3531A
	#include "hinfc610_hi3531a.c"
#endif

/*****************************************************************************/
extern struct read_retry_t hinfc610_hynix_bg_cdie_read_retry;
extern struct read_retry_t hinfc610_hynix_bg_bdie_read_retry;
extern struct read_retry_t hinfc610_hynix_cg_adie_read_retry;
extern struct read_retry_t hinfc610_micron_read_retry;
extern struct read_retry_t hinfc610_toshiba_24nm_read_retry;
extern struct read_retry_t hinfc610_samsung_read_retry;

static struct read_retry_t *read_retry_list[] = {
	&hinfc610_hynix_bg_bdie_read_retry,
	&hinfc610_hynix_bg_cdie_read_retry,
	&hinfc610_hynix_cg_adie_read_retry,
	&hinfc610_micron_read_retry,
	&hinfc610_toshiba_24nm_read_retry,
	&hinfc610_samsung_read_retry,
	NULL,
};
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

static void nand_register_dump(void)
{
	int ix;
	unsigned int regbase = CONFIG_HINFC610_REG_BASE_ADDRESS;

	printf("Register dump:");
	for (ix = 0; ix <= 0x98; ix += 0x04) {
		if (!(ix & 0x0F))
			printf("\n0x%08X: ", (regbase + ix));
		printf("%08X ", readl(regbase + ix));
	}
	printf("\n");
}
/*****************************************************************************/

static void hinfc610_dma_transfer(struct hinfc_host *host, int todev)
{
	unsigned long reg_val;
	unsigned int dma_addr = (unsigned int)host->dma_buffer;

#ifdef CONFIG_ARCH_MMU
	if (todev)
		dcache_clean_range(dma_addr, dma_addr + host->pagesize);
	else
		dcache_inv_range(dma_addr, dma_addr + host->pagesize);
#endif

	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA1);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA2);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA3);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA4);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA5);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA6);

	dma_addr += HINFC610_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC610_DMA_ADDR_DATA7);

	hinfc_write(host, host->dma_oob, HINFC610_DMA_ADDR_OOB);

	if (host->ecctype == NAND_ECC_0BIT) {
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

	{
		unsigned int timeout = 0xF0000000;
		while ((hinfc_read(host, HINFC610_DMA_CTRL))
			& HINFC610_DMA_CTRL_DMA_START && timeout)
			timeout--;
		if (!timeout)
			DBG_BUG("Wait DMA finish timeout.\n");
	}
}
/*****************************************************************************/

static void hinfc610_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
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
			addr_offset = (host->addr_cycle -
				HINFC610_ADDR_CYCLE_MASK) << 3;
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
			break;

		case NAND_CMD_READSTART:
			is_cache_invalid = 0;
			host->send_cmd_readstart(host);
			break;

		case NAND_CMD_ERASE2:
			host->send_cmd_erase(host);
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

	/* pass pagesize and ecctype to kernel when startup. */
	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

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
		printf("WARNING: attempt to write an invalid bbm. " \
		       "page: 0x%08x, mark: 0x%02x,\n",
		       GET_PAGE_INDEX(host), *host->bbm);

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC610_ADDRL);
	hinfc_write(host, host->addr_value[1], HINFC610_ADDRH);
	hinfc_write(host,
		((NAND_CMD_STATUS << 16) | (NAND_CMD_PAGEPROG << 8) |
		 NAND_CMD_SEQIN),
		HINFC610_CMD);

	*host->epm = 0x0000;

	hinfc610_dma_transfer(host, 1);

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
	unsigned int regval;

	for (ix = 1; IS_PS_UN_ECC(host) && ix < host->read_retry->count; ix++) {

		hinfc_write(host, HINFC610_INTCLR_UE | HINFC610_INTCLR_CE,
			HINFC610_INTCLR);

		host->enable_ecc_randomizer(host, DISABLE, DISABLE);

		host->read_retry->set_rr_param(host, ix);

		/* only enable randomize */
		if (IS_RANDOMIZER(host)) {
			regval = hinfc_read(host, HINFC610_CON);
			regval |= HINFC610_CON_RANDOMIZER_EN;
			hinfc_write(host, regval, HINFC610_CON);
		}

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

#ifdef CONFIG_NAND_NOTICE_RR
	printf("\nPage %d Do RR(%d/%d) %s.\n", GET_PAGE_INDEX(host),
	       ix-1, host->read_retry->count, (IS_PS_UN_ECC(host)
		       ? "Fail" : "Success"));
#endif

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);
	host->read_retry->reset_rr_param(host);

	return 0;
}
/*****************************************************************************/

static int hinfc610_send_cmd_readstart(struct hinfc_host *host)
{
	if ((host->addr_value[0] == host->cache_addr_value[0])
	    && (host->addr_value[1] == host->cache_addr_value[1]))
		return 0;

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

	if (host->read_retry || IS_RANDOMIZER(host)) {
		host->page_status |= hinfc610_get_data_status(host);

		if (IS_PS_EMPTY_PAGE(host)) {
			/*
			 * oob area used by yaffs2 only 32 bytes,
			 * so we only fill 32 bytes.
			 */
			if (IS_RANDOMIZER(host))
				memset(host->buffer, 0xFF,
				       host->pagesize + host->oobsize);

		} else if (!IS_PS_BAD_BLOCK(host)) {
			/* if NAND chip support read retry */
			if (IS_PS_UN_ECC(host) && host->read_retry)
				hinfc610_do_read_retry(host);

		} /* 'else' NAND have a bad block, do nothing. */
	}

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

static int hinfc610_send_cmd_readid(struct hinfc_host *host)
{
	unsigned int regval;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);
	hinfc_write(host, 0xfffffff, HINFC610_SYNC_TIMING);

	hinfc_write(host, HINFC610_NANDINFO_LEN, HINFC610_DATA_NUM);
	hinfc_write(host, NAND_CMD_READID, HINFC610_CMD);
	hinfc_write(host, 0, HINFC610_ADDRL);

	regval = HINFC610_OP_CMD1_EN
		 | HINFC610_OP_ADDR_EN
		 | HINFC610_OP_READ_DATA_EN
		 | HINFC610_OP_WAIT_READY_EN
		 | ((host->chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT)
		 | (1 << HINFC610_OP_ADDR_CYCLE_SHIFT);

	hinfc_write(host, regval, HINFC610_OP);

	host->addr_cycle = 0x0;

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_enable_ecc_randomizer(struct hinfc_host *host, int ecc_en,
					  int randomizer_en)
{
	unsigned int nfc_con;

	if (IS_RANDOMIZER(host)) {
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

	regval = HINFC610_OP_CMD1_EN
		 | HINFC610_OP_READ_DATA_EN
		 | HINFC610_OP_WAIT_READY_EN
		 | ((host->chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT);

	hinfc_write(host, regval, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}

/*****************************************************************************/
static int hinfc610_send_cmd_reset(struct hinfc_host *host, int chipselect)
{
	unsigned int regval;

	hinfc_write(host, NAND_CMD_RESET, HINFC610_CMD);

	regval = HINFC610_OP_CMD1_EN
		 | (((chipselect & HINFC610_OP_NF_CS_MASK)
		    << HINFC610_OP_NF_CS_SHIFT)
		 | HINFC610_OP_WAIT_READY_EN);

	hinfc_write(host, regval, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc610_dev_ready(struct mtd_info *mtd)
{
	return 0x1;
}
/*****************************************************************************/

static void hinfc610_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_HINFC610_MAX_CHIP)
		DBG_BUG("invalid chipselect: %d\n", chipselect);

	host->chipselect = chipselect;
	host->mtd = mtd;
}
/*****************************************************************************/

static uint8_t hinfc610_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (host->command == NAND_CMD_STATUS)
		return readb(chip->IO_ADDR_R);

	host->offset++;

	if (host->command == NAND_CMD_READID)
		return readb(chip->IO_ADDR_R + host->offset - 1);

#ifdef CONFIG_ARCH_MMU
	dcache_inv_range((unsigned)(host->buffer + host->column
				+ host->offset - 1),
			(unsigned)(host->buffer + host->column
				+ host->offset));
#endif
	return readb(host->buffer + host->column + host->offset - 1);
}
/*****************************************************************************/

static u16 hinfc610_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	host->offset += 2;
#ifdef CONFIG_ARCH_MMU
	dcache_inv_range((unsigned)(host->buffer + host->column
				+ host->offset - 2),
			(unsigned)(host->buffer + host->column
				+ host->offset));
#endif
	return readw(host->buffer + host->column + host->offset - 2);
}
/*****************************************************************************/

static void hinfc610_write_buf(struct mtd_info *mtd,
	const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(host->buffer + host->column + host->offset, buf, len);
	host->offset += len;
}
/*****************************************************************************/

static void hinfc610_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;
#ifdef CONFIG_ARCH_MMU
	dcache_inv_range((unsigned)(host->buffer + host->column
				+ host->offset),
			(unsigned)(host->buffer + host->column
				+ host->offset + len));
#endif
	memcpy(buf, host->buffer + host->column + host->offset, len);
	host->offset += len;
}
/*****************************************************************************/

static struct nand_ecclayout nand_ecc_default = {
	.oobfree = {{2, 30} }
};
/*****************************************************************************/
/* ecc/pagesize get from NAND controller */
static struct nand_config_info hinfc610_hw_auto_config_table[] = {
	{NAND_PAGE_16K, NAND_ECC_64BIT, 1824/*1824*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_40BIT, 1200/*1152*/, &nand_ecc_default},
	{NAND_PAGE_16K, NAND_ECC_0BIT,  32 ,          &nand_ecc_default},

	{NAND_PAGE_8K, NAND_ECC_64BIT, 928 /*928*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_40BIT, 600 /*592*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_24BIT, 368 /*368*/,  &nand_ecc_default},
	{NAND_PAGE_8K, NAND_ECC_0BIT,  32,           &nand_ecc_default},

	{NAND_PAGE_4K, NAND_ECC_24BIT, 200 /*200*/,  &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_8BIT,  128  /*88*/,   &nand_ecc_default},
	{NAND_PAGE_4K, NAND_ECC_0BIT,  32,           &nand_ecc_default},

	{NAND_PAGE_2K, NAND_ECC_24BIT, 128 /*116*/, &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_8BIT,  64  /*60*/,  &nand_ecc_default},
	{NAND_PAGE_2K, NAND_ECC_0BIT,  32,          &nand_ecc_default},

	{0, 0, 0, NULL},
};
/*****************************************************************************/

static struct nand_config_info *
hinfc610_get_best_ecc(struct nand_config_info *config, struct mtd_info *mtd)
{
	struct nand_config_info *best = NULL;

	for (; config->layout; config++) {
		if (nandpage_type2size(config->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < config->oobsize)
			continue;

		if (!best || (best->ecctype < config->ecctype))
			best = config;
	}

	if (!best)
		DBG_BUG(ERSTR_DRIVER
			"Driver does not support the pagesize(%d) "
			"and oobsize(%d).\n",
			mtd->writesize, mtd->oobsize);

	return best;
}
/*****************************************************************************/
/* force the pagesize and ecctype */
struct nand_config_info *
hinfc610_force_ecc(struct nand_config_info *config, struct mtd_info *mtd,
		   int pagetype, int ecctype, char *cfgmsg, int allow_pagediv)
{
	int pagesize;
	struct nand_config_info *fit = NULL;

	for (; config->layout; config++) {
		if (config->pagetype == pagetype &&
		    config->ecctype == ecctype) {
			fit = config;
			break;
		}
	}

	if (!fit) {
		nand_register_dump();
		DBG_BUG(ERSTR_DRIVER
			"Driver(%s mode) does not support this Nand Flash "
			"pagesize:%s, ecctype:%s\n",
			cfgmsg,
			nand_page_name(pagetype),
			nand_ecc_name(ecctype));
		return NULL;
	}

	pagesize = nandpage_type2size(pagetype);
	if ((pagesize != mtd->writesize) &&
	    (pagesize > mtd->writesize || !allow_pagediv)) {
		nand_register_dump();
		DBG_BUG(ERSTR_HARDWARE
			"Hardware (%s mode) configure pagesize %d, "
			"but the Nand Flash pageszie is %d\n",
			cfgmsg,
			pagesize,
			mtd->writesize);
		return NULL;
	}

	if (fit->oobsize > mtd->oobsize) {
		DBG_BUG(ERSTR_HARDWARE
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

int hinfc610_ecc_type2reg_intf(int type)
{
	if (HINFC_VER_620 == host.version)
		return hinfc620_ecc_type2reg(type);
	else
		return hinfc610_ecc_type2reg(type);
}
/*****************************************************************************/

int hinfc610_ecc_reg2type_intf(int reg)
{
	if (HINFC_VER_620 == host.version)
		return hinfc620_ecc_reg2type(reg);
	else
		return hinfc610_ecc_reg2type(reg);
}
/*****************************************************************************/

static int hinfc610_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
			      struct nand_flash_dev_ex *nand_dev)
{
	char *start_type = "unknown";
	struct nand_config_info *best = NULL;
	struct nand_config_info *config = NULL;
	struct hinfc_host *host = chip->priv;

	config = hinfc610_hw_auto_config_table;
	start_type = "HW-Auto";
	best = hinfc610_get_best_ecc(config, mtd);
	if ((mtd->writesize == _8K)
	|| (mtd->writesize == _16K)
	|| (mtd->writesize == _32K))
		host->flags |= NAND_RANDOMIZER;
	nand_dev->flags |= host->flags;

#ifdef CONFIG_HINFC610_PAGESIZE_AUTO_ECC_NONE
	{
		int pagetype = nandpage_size2type(mtd->writesize);
		int ecctype = NAND_ECC_0BIT;
		best = hinfc610_force_ecc(config, mtd, pagetype, ecctype,
			"force config", 0);
		start_type = "NoECC";
	}
#endif /* CONFIG_HINFC610_PAGESIZE_AUTO_ECC_NONE */

	if (!best)
		DBG_BUG(ERSTR_HARDWARE
			"Please configure Nand Flash pagesize and ecctype!\n");

	if (best->ecctype != NAND_ECC_0BIT)
		mtd->oobsize = best->oobsize;
	mtd->oobused = HINFC610_OOBSIZE_FOR_YAFFS;
	chip->ecc.layout = best->layout;

	host->ecctype  = best->ecctype;
	host->pagesize = nandpage_type2size(best->pagetype);
	host->oobsize  = mtd->oobsize;
	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);

	host->buforg = kmalloc((host->pagesize
		+ host->oobsize + HINFC610_DMA_ALIGN), GFP_KERNEL);
	if (!host->buforg) {
		DBG_BUG(ERSTR_DRIVER
			"Can't malloc memory for NAND driver.");
		return 1;
	}
	/* 32 bytes alignment */
	host->buffer = (char *)((unsigned int)(host->buforg
		+ HINFC610_DMA_ALIGN - 1) & ~(HINFC610_DMA_ALIGN - 1));
	host->dma_buffer = (unsigned int)host->buffer;

	memset((char *)chip->IO_ADDR_R, 0xff,
	       HINFC610_BUFFER_BASE_ADDRESS_LEN);
	memset(host->buffer, 0xff,
		(host->pagesize + host->oobsize));

	host->dma_oob = host->dma_buffer + host->pagesize;
	host->bbm = (unsigned char *)(host->buffer
		+ host->pagesize + HINFC_BAD_BLOCK_POS);

	host->epm = (unsigned short *)(host->buffer
		+ host->pagesize + chip->ecc.layout->oobfree[0].offset + 28);

	host->NFC_CON  = (HINFC610_CON_OP_MODE_NORMAL
		| ((hinfc610_page_type2reg(best->pagetype) &
		    HINFC610_CON_PAGESIZE_MASK)
		   << HINFC610_CON_PAGEISZE_SHIFT)
		| HINFC610_CON_READY_BUSY_SEL
		| ((hinfc610_ecc_type2reg_intf(best->ecctype) &
		    HINFC610_CON_ECCTYPE_MASK)
		   << HINFC610_CON_ECCTYPE_SHIFT));

	host->NFC_CON_ECC_NONE = (HINFC610_CON_OP_MODE_NORMAL
		| ((hinfc610_page_type2reg(best->pagetype) &
		    HINFC610_CON_PAGESIZE_MASK)
		   << HINFC610_CON_PAGEISZE_SHIFT)
		| HINFC610_CON_READY_BUSY_SEL);

	if (mtd->writesize > NAND_MAX_PAGESIZE ||
	    mtd->oobsize > NAND_MAX_OOBSIZE) {
		DBG_BUG(ERSTR_DRIVER
			"Driver does not support this Nand Flash. "
			"Please increase NAND_MAX_PAGESIZE and "
			"NAND_MAX_OOBSIZE.\n");
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
		printf("Nand divide into 1/%u\n", (1 << shift));
	}

	nand_dev->start_type = start_type;
	nand_dev->ecctype = host->ecctype;

	host->read_retry = NULL;
	if (nand_dev->read_retry_type != NAND_RR_NONE) {
		struct read_retry_t **rr;
		for (rr = read_retry_list; rr; rr++) {
			if ((*rr)->type == nand_dev->read_retry_type) {
				host->read_retry = *rr;
				break;
			}
		}

		if (!host->read_retry) {
			DBG_BUG(ERSTR_DRIVER
				"This Nand Flash need to enable the "
				"'read retry' feature. "
				"but the driver dose not offer the feature");
		}
	}

	/*
	 * If it want to support the 'read retry' feature, the 'randomizer'
	 * feature must be support first.
	 */
	if (host->read_retry && !IS_RANDOMIZER(host)) {
		DBG_BUG(ERSTR_HARDWARE
			"This Nand flash need to enable 'randomizer' "
			"feature. Please configure hardware randomizer PIN.");
	}

	/*
	 * Check if hardware enable randomizer PIN, But NAND does not need
	 * randomizer. We will notice user.
	 */
	while (IS_RANDOMIZER(host)) {
		if (host->pagesize == _8K
		    && (host->ecctype >= NAND_ECC_24BIT
		    && host->ecctype <= NAND_ECC_80BIT))
			break;

		if (host->pagesize == _16K
		    && (host->ecctype >= NAND_ECC_41BIT
		    && host->ecctype <= NAND_ECC_80BIT))
			break;

		if (host->pagesize == _32K
		    && (host->ecctype >= NAND_ECC_41BIT
		    && host->ecctype <= NAND_ECC_80BIT))
			break;

		if (host->ecctype == NAND_ECC_0BIT)
			break;

		DBG_BUG(ERSTR_HARDWARE
			"This NAND flash does not support `randomizer`, "
			"Please don't configure hardware randomizer PIN.");
	}

	nand_dev->hostver = HINFC_VER_610;

	return 0;
}
/*****************************************************************************/

int hinfc610_nand_get_rr_param(char *param)
{
	int ret = 0;

	if ((host.read_retry->type == NAND_RR_HYNIX_BG_CDIE) ||
	    (host.read_retry->type == NAND_RR_HYNIX_CG_ADIE)) {
		ret = 64;
		memcpy(param, host.rr_data, ret);
	}
	return ret;
}
/*****************************************************************************/

static int hinfc610_nand_init(struct hinfc_host *host, struct nand_chip *chip)
{
	unsigned int regval;

	chip->priv        = host;
	chip->cmd_ctrl    = hinfc610_cmd_ctrl;
	chip->dev_ready   = hinfc610_dev_ready;
	chip->select_chip = hinfc610_select_chip;
	chip->read_byte   = hinfc610_read_byte;
	chip->read_word   = hinfc610_read_word;
	chip->write_buf   = hinfc610_write_buf;
	chip->read_buf    = hinfc610_read_buf;

	chip->chip_delay = HINFC610_CHIP_DELAY;
	chip->options = NAND_NO_AUTOINCR | NAND_BBT_SCANNED | NAND_BROKEN_XD;

	chip->ecc.layout = NULL;
	chip->ecc.mode   = NAND_ECC_NONE;

	host->chip          = chip;
	host->addr_cycle    = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->cache_addr_value[0] = ~0;
	host->cache_addr_value[1] = ~0;
	host->chipselect    = 0;

	host->send_cmd_pageprog  = hinfc610_send_cmd_pageprog;
	host->send_cmd_readstart = hinfc610_send_cmd_readstart;
	host->send_cmd_erase     = hinfc610_send_cmd_erase;
	host->send_cmd_readid    = hinfc610_send_cmd_readid;
	host->send_cmd_status    = hinfc610_send_cmd_status;
	host->send_cmd_reset     = hinfc610_send_cmd_reset;

	regval = hinfc_read(host, HINFC610_CON);

	host->NFC_CON = (regval
		| HINFC610_CON_OP_MODE_NORMAL
		| HINFC610_CON_READY_BUSY_SEL);

	host->NFC_CON_ECC_NONE = (host->NFC_CON
		& (~(HINFC610_CON_ECCTYPE_MASK
		     << HINFC610_CON_ECCTYPE_SHIFT))
		& (~HINFC610_CON_RANDOMIZER_EN));

	hinfc_write(host,
		(SET_HINFC610_PWIDTH(CONFIG_HINFC610_W_LATCH,
		 CONFIG_HINFC610_R_LATCH,
		 CONFIG_HINFC610_RW_LATCH)),
		HINFC610_PWIDTH);

	host->version = hinfc_read(host, HINFC610_VERSION);

	host->flags |= NAND_HW_AUTO;

	host->enable_ecc_randomizer = hinfc610_enable_ecc_randomizer;

	nand_oob_resize = hinfc610_ecc_probe;

	return 0;
}
/*****************************************************************************/

static int hinfc610_version_check(void)
{
	unsigned long regval;
	regval = readl(CONFIG_HINFC610_REG_BASE_ADDRESS + HINFC610_VERSION);
	return ((regval == HINFC_VER_610) || (regval == HINFC_VER_620));
}
/*****************************************************************************/

int board_nand_init(struct nand_chip *chip)
{
	printf("Check Nand Flash Controller v610 ... ");
	if (!hinfc610_version_check()) {
		printf("\n");
		return -ENODEV;
	}
	printf("found\n");

	if (host.chip)
		return 0;

#ifdef CONFIG_NAND_NOTICE_RR
	printf("Nand CONFIG_NAND_NOTICE_RR is defined\n");
#endif /* CONFIG_NAND_NOTICE_RR */

	memset((char *)&host, 0, sizeof(struct hinfc_host));

	host.iobase = (void __iomem *)CONFIG_HINFC610_REG_BASE_ADDRESS;
	chip->IO_ADDR_R = chip->IO_ADDR_W =
		(void *)CONFIG_HINFC610_BUFFER_BASE_ADDRESS;

	hinfc610_controller_enable(&host, 1);

	if (hinfc610_nand_init(&host, chip)) {
		printf("failed to allocate device buffer.\n");
		return -ENOMEM;
	}

	nand_spl_ids_register();

	return 0;
}
/*****************************************************************************/
int nand_get_ecctype(void)
{
	int ecctype;

	if (!host.chip) {
		printf("nand flash uninitialized.\n");
		return -1;
	}
	ecctype = hinfc610_ecc_type2reg_intf(host.ecctype);

	return ecctype;
}
/*****************************************************************************/

