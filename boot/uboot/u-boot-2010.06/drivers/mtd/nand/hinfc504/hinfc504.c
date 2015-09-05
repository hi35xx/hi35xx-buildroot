/******************************************************************************
 *    NAND Flash Controller V504 Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Czyong.
 *
******************************************************************************/

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <malloc.h>
#include <linux/mtd/nand.h>

/*
 * used hardware pagesize ecctype. this should define in include/configs/xxx.h
 * #define CONFIG_HINFC504_HARDWARE_PAGESIZE_ECC
 */
#include "hinfc504.h"
/*****************************************************************************/

extern void hinfc504_controller_enable(struct hinfc_host *host, int enable);
extern void nand_spl_ids_register(void);

/*****************************************************************************/
unsigned int nand_ip_version = HINFC_VER_504;
static struct hinfc_host host = {
	.chip = NULL,
};
/*****************************************************************************/


/*****************************************************************************/
extern struct read_retry_t hinfc504_hynix_bg_cdie_read_retry;
extern struct read_retry_t hinfc504_hynix_bg_bdie_read_retry;
extern struct read_retry_t hinfc504_hynix_cg_adie_read_retry;
extern struct read_retry_t hinfc504_micron_read_retry;
extern struct read_retry_t hinfc504_toshiba_24nm_read_retry;
extern struct read_retry_t hinfc504_samsung_read_retry;

static struct read_retry_t *read_retry_list[] = {
	&hinfc504_hynix_bg_bdie_read_retry,
	&hinfc504_hynix_bg_cdie_read_retry,
	&hinfc504_hynix_cg_adie_read_retry,
	&hinfc504_micron_read_retry,
	&hinfc504_toshiba_24nm_read_retry,
	&hinfc504_samsung_read_retry,
	NULL,
};
/*****************************************************************************/


static unsigned int get_bits(unsigned char byte)
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

static void nand_register_dump(void)
{
	int ix;
	unsigned int regbase = CONFIG_HINFC504_REG_BASE_ADDRESS;

	printf("Register dump:");
	for (ix = 0; ix <= 0x98; ix += 0x04) {
		if (!(ix & 0x0F))
			printf("\n0x%08X: ", (regbase + ix));
		printf("%08X ", readl(regbase + ix));
	}
	printf("\n");
}
/*****************************************************************************/

static char *get_pagesize_str(enum page_type pagetype)
{
	static char *pagesize_str[] = {
		"512", "2K", "4K", "8K", "16K", "unknown",
		"unknown", "unknown"};
	return pagesize_str[(pagetype & 0x07)];
}

static unsigned int get_pagesize(enum page_type pagetype)
{
	unsigned int pagesize[] = {
		_512B, _2K, _4K, _8K, _16K, 0, 0, 0};
	return pagesize[(pagetype & 0x07)];
}

/*****************************************************************************/

static void hinfc504_dma_transfer(struct hinfc_host *host, int todev)
{
	unsigned long reg_val;
	unsigned int dma_addr = (unsigned int)host->dma_buffer;

	hinfc_write(host, dma_addr, HINFC504_DMA_ADDR_DATA);

	dma_addr += HINFC504_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC504_DMA_ADDR_DATA1);

	dma_addr += HINFC504_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC504_DMA_ADDR_DATA2);

	dma_addr += HINFC504_DMA_ADDR_OFFSET;
	hinfc_write(host, dma_addr, HINFC504_DMA_ADDR_DATA3);

	hinfc_write(host, host->dma_oob, HINFC504_DMA_ADDR_OOB);

	if (host->ecctype == et_ecc_none) {
		hinfc_write(host,
			((host->oobsize & HINFC504_DMA_LEN_OOB_MASK)
				<< HINFC504_DMA_LEN_OOB_SHIFT),
			HINFC504_DMA_LEN);

		hinfc_write(host,
			HINFC504_DMA_PARA_DATA_RW_EN
			| HINFC504_DMA_PARA_OOB_RW_EN
			| ((host->n24bit_ext_len
				& HINFC504_DMA_PARA_EXT_LEN_MASK)
				<< HINFC504_DMA_PARA_EXT_LEN_SHIFT),
			HINFC504_DMA_PARA);
	} else
		hinfc_write(host,
			HINFC504_DMA_PARA_DATA_RW_EN
			| HINFC504_DMA_PARA_OOB_RW_EN
			| HINFC504_DMA_PARA_DATA_EDC_EN
			| HINFC504_DMA_PARA_OOB_EDC_EN
			| HINFC504_DMA_PARA_DATA_ECC_EN
			| HINFC504_DMA_PARA_OOB_ECC_EN
			| ((host->n24bit_ext_len
				& HINFC504_DMA_PARA_EXT_LEN_MASK)
				<< HINFC504_DMA_PARA_EXT_LEN_SHIFT),
			HINFC504_DMA_PARA);

	reg_val = (HINFC504_DMA_CTRL_DMA_START
		| HINFC504_DMA_CTRL_BURST4_EN
		| HINFC504_DMA_CTRL_BURST8_EN
		| HINFC504_DMA_CTRL_BURST16_EN
		| HINFC504_DMA_CTRL_DATA_AREA_EN
		| HINFC504_DMA_CTRL_OOB_AREA_EN
		| ((host->addr_cycle == 4 ? 1 : 0)
			<< HINFC504_DMA_CTRL_ADDR_NUM_SHIFT)
		| ((host->chipselect & HINFC504_DMA_CTRL_CS_MASK)
			<< HINFC504_DMA_CTRL_CS_SHIFT));

	if (todev)
		reg_val |= HINFC504_DMA_CTRL_WE;

	hinfc_write(host, reg_val, HINFC504_DMA_CTRL);

	do {
		unsigned int timeout = 0xF0000000;
		while ((hinfc_read(host, HINFC504_DMA_CTRL))
			& HINFC504_DMA_CTRL_DMA_START && timeout)
				timeout--;
		if (!timeout)
			DBG_BUG("Wait DMA finish timeout.\n");
	} while (0);
}
/*****************************************************************************/

static void hinfc504_cmd_ctrl(struct mtd_info *mtd,
	int dat, unsigned int ctrl)
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
		addr_offset =  host->addr_cycle << 3;

		if (host->addr_cycle >= HINFC504_ADDR_CYCLE_MASK) {
			addr_offset =
				(host->addr_cycle - HINFC504_ADDR_CYCLE_MASK)
					<< 3;
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
		if (host->command == NAND_CMD_SEQIN
			|| host->command == NAND_CMD_READ0
			|| host->command == NAND_CMD_READID) {
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

static int hinfc504_send_cmd_pageprog(struct hinfc_host *host)
{

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	hinfc_write(host,
		host->addr_value[0] & 0xffff0000,
		HINFC504_ADDRL);
	hinfc_write(host,
		host->addr_value[1], HINFC504_ADDRH);
	hinfc_write(host,
		NAND_CMD_PAGEPROG << 8 | NAND_CMD_SEQIN,
		HINFC504_CMD);

	*host->epm = 0xFFFF;

	hinfc504_dma_transfer(host, 1);

	return 0;
}
/*****************************************************************************/
#define NAND_BAD_BLOCK              1
#define NAND_EMPTY_PAGE             2
#define NAND_VALID_DATA             3

static int hinfc504_get_data_status(struct hinfc_host *host)
{
	/* this is block start address */
	if (!((host->addr_value[0] >> 16) & host->block_page_mask)) {

		/* it is a bad block */
		if (*host->bbm == 0)
			return NAND_BAD_BLOCK;
		/*
		 * if there are more than 2 bits flipping, it is
		 * maybe a bad block
		 */
		if (host->uc_er
		     && *host->bbm != 0xFFFF
		     && get_bits(*host->bbm) <= 6)
			return NAND_BAD_BLOCK;
	}

	/* it is an empty page */
	if (*host->epm != 0xFFFF && host->is_randomizer)
		return NAND_EMPTY_PAGE;

	return NAND_VALID_DATA;
}
/*****************************************************************************/
/* This is a debug interface */
static void hinfc504_detect_ecc(struct hinfc_host *host)
{
	int i, x;
	int pagesize = host->pagesize;
	int ecctype  = host->ecctype;

#define val(x) hinfc_read(host, 0xA0+x)
#define detect_ecc(_start, _end, _val, _mask) do {\
	int _i, _ret, _step, _cr_er, _tmp = _val;\
	char *suffix[] = {"st", "nd", "rd", "th"};\
	if (0x3 == _mask) \
		_step = 2;\
	else if (0x1f == _mask) \
		_step = 5;\
	else if (0x3f == _mask) \
		_step = 6;\
	for (_i = _start; _i < _end; _i++) {\
		_ret = _tmp & _mask; _tmp >>= _step;\
		_cr_er = (hinfc_read(host, HINFC504_INTS) & (1<<5)) ? 1 : 0;\
		if (host->uc_er) \
			printf("invalid ecc error:\t");\
		else if (_cr_er) \
			printf("  valid ecc error:\t");\
		else if (!host->uc_er && !_cr_er) \
			printf("no ecc error:\t");\
		printf("\t%d%s unit: %d\n", _i+1, \
				suffix[_i < 3 ? _i : 3], _ret); } \
} while (0)

	switch (pagesize) {
	case _2K:
		switch (ecctype) {
		case 0x1:
			printf("device is 2K1B\n");
			detect_ecc(0, 8, val(0), 0x3);
			break;
		case 0x2:
			printf("device is 2K4B\n");
			detect_ecc(0, 2, val(0), 0x3f);
			break;
		case 0x4:
			printf("device is 2K24B\n");
			detect_ecc(0, 2, val(0), 0x1f);
			break;
		default:
			break;
		}
		break;
	case _4K:
		switch (ecctype) {
		case 0x1:
			printf("device is 4K1B\n");
			for (i = 0, x = 4; i < 16; i += 8, x -= 4)
				detect_ecc(i, i+8, val(x), 0x3);
			break;
		case 0x2:
			printf("device is 4K4B\n");
			for (i = 0, x = 4; i < 4; i += 2, x -= 4)
				detect_ecc(i, i+2, val(x), 0x3f);
			break;
		case 0x4:
			printf("device is 4K24B\n");
			for (i = 0, x = 4; i < 4; i += 2, x -= 4)
				detect_ecc(i, i+2, val(x), 0x1f);
			break;
		default:
			break;
		}
		break;
	case _8K:
		switch (ecctype) {
		case 0x4:
			printf("device is 8K24B\n");
			for (i = 0, x = 12; i < 8; i += 2, x -= 4)
				detect_ecc(i, i+2, val(x), 0x1f);
			break;
		case 0x5:
			printf("device is 8K40B\n");
			for (i = 0, x = 12; i < 8; i += 2, x -= 4)
				detect_ecc(i, i+2, val(x), 0x3f);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
#undef val
#undef detect_ecc
}
/*****************************************************************************/
/* This is a debug interface */
static void hinfc600_detect_ecc(struct hinfc_host *host)
{

}
/*****************************************************************************/

static int hinfc504_do_read_retry(struct hinfc_host *host)
{
	int ix;
	unsigned int regval;

	for (ix = 1; host->uc_er && ix < host->read_retry->count; ix++) {

		hinfc_write(host, HINFC504_INTCLR_UE | HINFC504_INTCLR_CE,
			HINFC504_INTCLR);

		host->enable_ecc_randomizer(host, DISABLE, DISABLE);

		/* check whether it is toshiba chip */
		if (host->read_retry->type == NAND_RR_TOSHIBA_24nm) {

			hinfc_write(host, HINFC_CMD_SEQ(0x5C, 0xC5),
				HINFC504_CMD);
			hinfc_write(host,
				HINFC504_WRITE_2CMD_0ADD_NODATA,
				HINFC504_OP);
			WAIT_CONTROLLER_FINISH();
		}

		host->read_retry->set_rr_param(host, ix);

		/* enable randomize */
		if (host->is_randomizer) {
			regval = hinfc_read(host, HINFC504_RANDOMIZER);
			regval |= HINFC504_RANDOMIZER_ENABLE;
			hinfc_write(host, regval, HINFC504_RANDOMIZER);
		}

		/* check whether it is toshiba chip */
		if (host->read_retry->type == NAND_RR_TOSHIBA_24nm) {
			hinfc_write(host, HINFC_CMD_SEQ(0x26, 0x5D),
				HINFC504_CMD);
			hinfc_write(host,
				HINFC504_WRITE_2CMD_0ADD_NODATA,
				HINFC504_OP);
			WAIT_CONTROLLER_FINISH();
		}

		hinfc_write(host, HINFC504_INTCLR_UE | HINFC504_INTCLR_CE,
			HINFC504_INTCLR);
		hinfc_write(host, host->NFC_CON, HINFC504_CON);
		hinfc_write(host, host->addr_value[0] & 0xffff0000,
				HINFC504_ADDRL);
		hinfc_write(host, host->addr_value[1], HINFC504_ADDRH);
		hinfc_write(host,
			HINFC_CMD_SEQ(NAND_CMD_READ0, NAND_CMD_READSTART),
			HINFC504_CMD);

		hinfc_write(host, 0, HINFC504_LOG_READ_ADDR);
		hinfc_write(host, (host->pagesize + host->oobsize),
			HINFC504_LOG_READ_LEN);

		hinfc504_dma_transfer(host, 0);
		host->uc_er = (hinfc_read(host, HINFC504_INTS)
				& HINFC504_INTS_UE) ? 1 : 0;

	}

#ifdef CONFIG_NAND_NOTICE_RR
	printf("\nPage %d Do RR(%d/%d) %s.\n", GET_PAGE_INDEX(host),
	       ix-1, host->read_retry->count,
	       (host->uc_er ? "Fail" : "Success"));
#endif

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	host->read_retry->set_rr_param(host, 0);

	return 0;
}
/*****************************************************************************/

static int hinfc504_send_cmd_readstart(struct hinfc_host *host)
{
	if ((host->addr_value[0] == host->cache_addr_value[0])
	    && (host->addr_value[1] == host->cache_addr_value[1]))
		return 0;

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	hinfc_write(host, HINFC504_INTCLR_UE | HINFC504_INTCLR_CE,
		HINFC504_INTCLR);
	hinfc_write(host, host->NFC_CON, HINFC504_CON);
	hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC504_ADDRL);
	hinfc_write(host, host->addr_value[1], HINFC504_ADDRH);
	hinfc_write(host, NAND_CMD_READSTART << 8 | NAND_CMD_READ0,
		HINFC504_CMD);

	hinfc_write(host, 0, HINFC504_LOG_READ_ADDR);
	hinfc_write(host, (host->pagesize + host->oobsize),
		HINFC504_LOG_READ_LEN);

	hinfc504_dma_transfer(host, 0);
	host->uc_er = (hinfc_read(host, HINFC504_INTS)
			& HINFC504_INTS_UE) ? 1 : 0;

	if (host->read_retry || host->is_randomizer) {

		int status = hinfc504_get_data_status(host);

		if (status == NAND_EMPTY_PAGE) {
			/*
			 * oob area used by yaffs2 only 32 bytes,
			 * so we only fill 32 bytes.
			 */
			if (host->is_randomizer)
				memset(host->buffer, 0xFF,
				       host->pagesize + host->oobsize);
			host->uc_er = 2;

		} else if (status == NAND_VALID_DATA) {

			/* if NAND chip support read retry */
			if (host->uc_er && host->read_retry)
				hinfc504_do_read_retry(host);

		} /* 'else' NAND have a bad block, do nothing. */
	}

	host->cache_addr_value[0] = host->addr_value[0];
	host->cache_addr_value[1] = host->addr_value[1];

	return 0;
}
/*****************************************************************************/

static int hinfc504_send_cmd_erase(struct hinfc_host *host)
{
	/* Don't case the read retry config */
	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host,
		host->addr_value[0],
		HINFC504_ADDRL);
	hinfc_write(host,
		(NAND_CMD_ERASE2 << 8) | NAND_CMD_ERASE1,
		HINFC504_CMD);

	hinfc_write(host,
		HINFC504_OP_WAIT_READY_EN
		| HINFC504_OP_CMD2_EN
		| HINFC504_OP_CMD1_EN
		| HINFC504_OP_ADDR_EN
		| ((host->chipselect
			& HINFC504_OP_NF_CS_MASK)
			<< HINFC504_OP_NF_CS_SHIFT)
		| ((host->addr_cycle
			& HINFC504_OP_ADDR_CYCLE_MASK)
			<< HINFC504_OP_ADDR_CYCLE_SHIFT),
		HINFC504_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc504_send_cmd_readid(struct hinfc_host *host)
{
	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, HINFC504_NANDINFO_LEN, HINFC504_DATA_NUM);
	hinfc_write(host, NAND_CMD_READID, HINFC504_CMD);
	hinfc_write(host, 0, HINFC504_ADDRL);

	hinfc_write(host,
		HINFC504_OP_CMD1_EN
		| HINFC504_OP_ADDR_EN
		| HINFC504_OP_READ_DATA_EN
		| HINFC504_OP_WAIT_READY_EN
		| ((host->chipselect & HINFC504_OP_NF_CS_MASK)
			<< HINFC504_OP_NF_CS_SHIFT)
		| (1 << HINFC504_OP_ADDR_CYCLE_SHIFT),
		HINFC504_OP);

	host->addr_cycle = 0x0;

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc504_enable_ecc_randomizer(struct hinfc_host *host,
					  int ecc_en, int randomizer_en)
{
	unsigned int regval;

	if (host->is_randomizer) {

		regval = hinfc_read(host, HINFC504_RANDOMIZER);
		if (randomizer_en)
			regval |= HINFC504_RANDOMIZER_ENABLE;
		else
			regval &= ~HINFC504_RANDOMIZER_ENABLE;
		hinfc_write(host, regval, HINFC504_RANDOMIZER);
	}

	if (ecc_en)
		hinfc_write(host, host->NFC_CON, HINFC504_CON);
	else
		hinfc_write(host, host->NFC_CON_ECC_NONE, HINFC504_CON);

	return 0;
}
/*****************************************************************************/

static int hinfc600_enable_ecc_randomizer(struct hinfc_host *host,
					  int ecc_en, int randomizer_en)
{
	unsigned int nfc_con
		= (ecc_en ? host->NFC_CON : host->NFC_CON_ECC_NONE);

	if (host->is_randomizer) {
		if (randomizer_en)
			nfc_con |= HINFC600_CON_RANDOMIZER_EN;
		else
			nfc_con &= ~HINFC600_CON_RANDOMIZER_EN;
	}

	hinfc_write(host, nfc_con, HINFC600_CON);

	return 0;
}
/*****************************************************************************/

static int hinfc504_send_cmd_status(struct hinfc_host *host)
{
	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, HINFC504_NANDINFO_LEN, HINFC504_DATA_NUM);
	hinfc_write(host, NAND_CMD_STATUS, HINFC504_CMD);
	hinfc_write(host,
		HINFC504_OP_CMD1_EN
		| HINFC504_OP_READ_DATA_EN
		| HINFC504_OP_WAIT_READY_EN
		| ((host->chipselect & HINFC504_OP_NF_CS_MASK)
			<< HINFC504_OP_NF_CS_SHIFT),
		HINFC504_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc504_send_cmd_reset(struct hinfc_host *host, int chipselect)
{
	hinfc_write(host,
		NAND_CMD_RESET, HINFC504_CMD);
	hinfc_write(host,
		(HINFC504_OP_CMD1_EN
		| ((chipselect & HINFC504_OP_NF_CS_MASK)
			<< HINFC504_OP_NF_CS_SHIFT)
		| HINFC504_OP_WAIT_READY_EN),
		HINFC504_OP);

	WAIT_CONTROLLER_FINISH();

	return 0;
}
/*****************************************************************************/

static int hinfc504_dev_ready(struct mtd_info *mtd)
{
	return 0x1;
}
/*****************************************************************************/

static void hinfc504_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_HINFC504_MAX_CHIP)
		DBG_BUG("invalid chipselect: %d\n", chipselect);

	host->chipselect = chipselect;
}
/*****************************************************************************/

static uint8_t hinfc504_read_byte(struct mtd_info *mtd)
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

static u16 hinfc504_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	host->offset += 2;
	return readw(host->buffer + host->column + host->offset - 2);
}
/*****************************************************************************/

static void hinfc504_write_buf(struct mtd_info *mtd,
	const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(host->buffer + host->column + host->offset, buf, len);
	host->offset += len;
}
/*****************************************************************************/

static void hinfc504_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
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

static struct page_page_ecc_info page_page_ecc_info[] = {
	{pt_pagesize_16K, et_ecc_40bit1k, 1200 /*1152*/, &nand_ecc_default},
	{pt_pagesize_8K, et_ecc_40bit1k, 600/*592*/, &nand_ecc_default},
	{pt_pagesize_8K, et_ecc_24bit1k, 368, &nand_ecc_default},
	{pt_pagesize_8K, et_ecc_none,    32,  &nand_ecc_default},

	{pt_pagesize_4K, et_ecc_24bit1k, 200, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_8bytes,  144/*104*/, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_4bit,    128/*104*/, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_1bit,    128, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_none,    32,  &nand_ecc_default},

	{pt_pagesize_2K, et_ecc_24bit1k, 128/*116*/, &nand_ecc_default},
	{pt_pagesize_2K, et_ecc_8bytes,  88,  &nand_ecc_default},
	{pt_pagesize_2K, et_ecc_4bit,    64,  &nand_ecc_default},
	{pt_pagesize_2K, et_ecc_none,    32,  &nand_ecc_default},

	{0, 0, 0, NULL},
};
/*****************************************************************************/
/* used the best correct arithmetic. */
struct page_page_ecc_info *hinfc504_get_best_ecc(struct mtd_info *mtd)
{
	struct page_page_ecc_info *best = NULL;
	struct page_page_ecc_info *info = page_page_ecc_info;

	for (; info->layout; info++) {
		if (get_pagesize(info->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < info->oobsize)
			continue;

		if (!best || (best->ecctype < info->ecctype))
			best = info;
	}

	if (!best)
		DBG_BUG(ERSTR_DRIVER
			"Driver does not support the pagesize(%d) and oobsize(%d).\n",
			mtd->writesize, mtd->oobsize);

	return best;
}
/*****************************************************************************/
/* force the pagesize and ecctype */
struct page_page_ecc_info *hinfc504_force_ecc
(
	struct mtd_info *mtd,
	enum page_type pagetype,
	enum ecc_type ecctype,
	char *cfgmsg,
	int allow_pagediv
)
{
	int pagesize;
	struct page_page_ecc_info *fit = NULL;
	struct page_page_ecc_info *info = page_page_ecc_info;

	/* * For logic use 2k_4bit instead of 2k_1bit in fact,
	 *   we keep the change.
	 */

	if ((pt_pagesize_2K == pagetype)
		&& (et_ecc_1bit == ecctype))
		ecctype = et_ecc_4bit;


	for (; info->layout; info++) {
		if (info->pagetype == pagetype
			&& info->ecctype == ecctype) {
			fit = info;
			break;
		}
	}

	if (!fit) {
		nand_register_dump();
		DBG_BUG(ERSTR_DRIVER
			"Driver(%s mode) does not support this Nand Flash "
			"pagesize:%s, ecctype:%s\n",
			cfgmsg,
			get_pagesize_str(pagetype),
			get_ecctype_str(ecctype));
		return NULL;
	}

	pagesize = get_pagesize(pagetype);
	if ((pagesize != mtd->writesize)
		&& (pagesize > mtd->writesize || !allow_pagediv)) {
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
			get_ecctype_str(ecctype));
		return NULL;
	}

	return fit;
}
/*****************************************************************************/

static int hinfc504_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex)
{
	char *start_type = "unknown";
	struct page_page_ecc_info *best = NULL;
	struct hinfc_host *host = chip->priv;

#ifdef CONFIG_HINFC504_AUTO_PAGESIZE_ECC
	best = hinfc504_get_best_ecc(mtd);
	start_type = "Auto";
#endif /* CONFIG_HINFC504_AUTO_PAGESIZE_ECC */

#ifdef CONFIG_HINFC504_HARDWARE_PAGESIZE_ECC
#  ifdef CONFIG_HINFC504_AUTO_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC504_AUTO_PAGESIZE_ECC \
	and CONFIG_HINFC504_HARDWARE_PAGESIZE_ECC at the same time
#  endif
	best = hinfc504_force_ecc(mtd,
		((host->NFC_CON >> HINFC504_CON_PAGEISZE_SHIFT)
			& HINFC504_CON_PAGESIZE_MASK),
		((host->NFC_CON >> HINFC504_CON_ECCTYPE_SHIFT)
			& HINFC504_CON_ECCTYPE_MASK),
		"hardware config", 0);

	start_type = "Hardware";
#endif /* CONFIG_HINFC504_HARDWARE_PAGESIZE_ECC */

#ifdef CONFIG_HINFC504_PAGESIZE_AUTO_ECC_NONE
#  ifdef CONFIG_HINFC504_AUTO_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC504_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HINFC504_AUTO_PAGESIZE_ECC at the same time
#  endif
#  ifdef CONFIG_HINFC504_HARDWARE_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC504_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HINFC504_HARDWARE_PAGESIZE_ECC at the same time
#  endif

	{
		int pagetype;

		switch (mtd->writesize) {
		case _512B:
			pagetype = pt_pagesize_512;
			break;
		case _2K:
			pagetype = pt_pagesize_2K;
			break;
		case _4K:
			pagetype = pt_pagesize_4K;
			break;
		case _8K:
			pagetype = pt_pagesize_8K;
			break;
		case _16K:
			pagetype = pt_pagesize_16K;
			break;
		default:
			pagetype = pt_pagesize_2K;
			break;
		}
		best = hinfc504_force_ecc(mtd, pagetype,
			et_ecc_none, "force config", 0);
		start_type = "AutoForce";
	}
#endif /* CONFIG_HINFC504_PAGESIZE_AUTO_ECC_NONE */

	if (!best)
		DBG_BUG(ERSTR_HARDWARE
			"Please configure Nand Flash pagesize and ecctype!\n");

	if (best->ecctype != et_ecc_none)
		mtd->oobsize = best->oobsize;
	chip->ecc.layout = best->layout;

	host->ecctype  = best->ecctype;
	host->pagesize = get_pagesize(best->pagetype);
	host->oobsize  = mtd->oobsize;
	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);

	if (host->ecctype == et_ecc_24bit1k) {
		if (host->pagesize == _4K)
			host->n24bit_ext_len = 0x03; /* 8bytes; */
		else if (host->pagesize == _8K)
			host->n24bit_ext_len = 0x01; /* 4bytes; */
	}
	host->dma_oob = host->dma_buffer + host->pagesize;
	host->bbm = (unsigned char *)(host->buffer
		+ host->pagesize + HINFC_BAD_BLOCK_POS);

	host->epm = (unsigned short *)(host->buffer
		+ host->pagesize + chip->ecc.layout->oobfree[0].offset + 28);

	host->NFC_CON  = (HINFC504_CON_OP_MODE_NORMAL
		| ((best->pagetype & HINFC504_CON_PAGESIZE_MASK)
			<< HINFC504_CON_PAGEISZE_SHIFT)
		| HINFC504_CON_READY_BUSY_SEL
		| ((best->ecctype & HINFC504_CON_ECCTYPE_MASK)
			<< HINFC504_CON_ECCTYPE_SHIFT));

	host->NFC_CON_ECC_NONE  = (HINFC504_CON_OP_MODE_NORMAL
		| ((best->pagetype & HINFC504_CON_PAGESIZE_MASK)
			<< HINFC504_CON_PAGEISZE_SHIFT)
		| HINFC504_CON_READY_BUSY_SEL);

	if (mtd->writesize > NAND_MAX_PAGESIZE
		|| mtd->oobsize > NAND_MAX_OOBSIZE) {
		DBG_BUG(ERSTR_DRIVER
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
		printf("Nand divide into 1/%u\n", (1 << shift));
	}

	flash_dev_ex->start_type = start_type;
	flash_dev_ex->ecctype = host->ecctype;
	flash_dev_ex->is_randomizer = host->is_randomizer;

	host->read_retry = NULL;
	if (flash_dev_ex->read_retry_type != NAND_RR_NONE) {
		struct read_retry_t **rr;
		for (rr = read_retry_list; rr; rr++) {
			if ((*rr)->type == flash_dev_ex->read_retry_type) {
				host->read_retry = *rr;
				break;
			}
		}

		if (!host->read_retry) {
			DBG_BUG(ERSTR_DRIVER
				"This Nand Flash need to enable the 'read retry' feature. "
				"but the driver dose not offer the feature");
		}

	}

	/*
	 * If it want to support the 'read retry' feature, the 'randomizer'
	 * feature must be support first.
	 */
	if (host->read_retry && !host->is_randomizer) {
		DBG_BUG(ERSTR_HARDWARE
			"This Nand flash need to enable 'randomizer' feature. "
			"Please configure hardware randomizer PIN.");
	}

	/*
	 * Check if hardware enable randomizer PIN, But NAND no need randomizer.
	 * We will notice user.
	 */
	if (host->is_randomizer) {
		while (HINFC_VER_505 == host->version) {

			if (host->pagesize == _4K
				&& host->ecctype == et_ecc_24bit1k)
				break;

			if (host->pagesize == _8K
				&& (host->ecctype == et_ecc_24bit1k
				|| host->ecctype == et_ecc_40bit1k))
				break;

			if (host->pagesize == _16K
				&& host->ecctype == et_ecc_40bit1k)
				break;

			DBG_BUG(ERSTR_HARDWARE
				"This NAND flash does not support `randomizer`, "
				"Please don't configure hardware randomizer PIN.");
		}
	}

	return 0;
}
/*****************************************************************************/

int hinfc504_nand_get_rr_param(char *param)
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

static int hinfc504_get_randomizer(struct hinfc_host *host)
{
	unsigned int regval;

	regval = hinfc_read(host, HINFC504_RANDOMIZER);
	host->is_randomizer
		= ((regval & HINFC504_RANDOMIZER_PAD) ? 1 : 0);
	regval |= (host->is_randomizer ? HINFC504_RANDOMIZER_ENABLE : 0);
	hinfc_write(host, regval, HINFC504_RANDOMIZER);

	return host->is_randomizer;
}
/*****************************************************************************/

static int hinfc600_get_randomizer(struct hinfc_host *host)
{
	unsigned int regval;

	regval = hinfc_read(host, HINFC600_BOOT_CFG);
	host->is_randomizer
		= ((regval & HINFC600_BOOT_CFG_RANDOMIZER_PAD) ? 1 : 0);

	return host->is_randomizer;
}
/*****************************************************************************/

static int hinfc504_nand_init(struct hinfc_host *host, struct nand_chip *chip)
{
	chip->priv        = host;
	chip->cmd_ctrl    = hinfc504_cmd_ctrl;
	chip->dev_ready   = hinfc504_dev_ready;
	chip->select_chip = hinfc504_select_chip;
	chip->read_byte   = hinfc504_read_byte;
	chip->read_word   = hinfc504_read_word;
	chip->write_buf   = hinfc504_write_buf;
	chip->read_buf    = hinfc504_read_buf;

	chip->chip_delay = HINFC504_CHIP_DELAY;
	chip->options    = NAND_NO_AUTOINCR | NAND_BBT_SCANNED;

	chip->ecc.layout = NULL;
	chip->ecc.mode   = NAND_ECC_NONE;

	host->chip          = chip;
	host->addr_cycle    = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->cache_addr_value[0] = ~0;
	host->cache_addr_value[1] = ~0;
	host->chipselect    = 0;

	host->send_cmd_pageprog  = hinfc504_send_cmd_pageprog;
	host->send_cmd_readstart = hinfc504_send_cmd_readstart;
	host->send_cmd_erase     = hinfc504_send_cmd_erase;
	host->send_cmd_readid    = hinfc504_send_cmd_readid;
	host->send_cmd_status    = hinfc504_send_cmd_status;
	host->send_cmd_reset     = hinfc504_send_cmd_reset;

	host->NFC_CON = (hinfc_read(host, HINFC504_CON)
		| HINFC504_CON_OP_MODE_NORMAL
		| HINFC504_CON_READY_BUSY_SEL);
	host->NFC_CON_ECC_NONE = (host->NFC_CON
		& (~(HINFC504_CON_ECCTYPE_MASK
			<< HINFC504_CON_ECCTYPE_SHIFT))
		& (~HINFC600_CON_RANDOMIZER_EN));

	host->buforg = kmalloc((NAND_MAX_PAGESIZE
		+ NAND_MAX_OOBSIZE + HINFC504_DMA_ALIGN), GFP_KERNEL);
	if (!host->buforg) {
		DBG_BUG(ERSTR_DRIVER
			"Can't malloc memory for NAND driver.");
		return 1;
	}
	/* 32 bytes alignment */
	host->buffer = (char *)((unsigned int)(host->buforg
		+ HINFC504_DMA_ALIGN - 1) & ~(HINFC504_DMA_ALIGN - 1));
	host->dma_buffer = (unsigned int)host->buffer;

	memset((char *)chip->IO_ADDR_R,
		0xff, HINFC504_BUFFER_BASE_ADDRESS_LEN);
	memset(host->buffer,
		0xff, (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE));

	hinfc_write(host,
		SET_HINFC504_PWIDTH(CONFIG_HINFC504_W_LATCH,
			CONFIG_HINFC504_R_LATCH,
			CONFIG_HINFC504_RW_LATCH),
		HINFC504_PWIDTH);

	host->version = hinfc_read(host, HINFC504_VERSION);
	if (HINFC_VER_600 == host->version) {
		hinfc600_get_randomizer(host);
		host->enable_ecc_randomizer = hinfc600_enable_ecc_randomizer;
		host->detect_ecc = hinfc600_detect_ecc;
	} else {
		hinfc504_get_randomizer(host);
		host->enable_ecc_randomizer = hinfc504_enable_ecc_randomizer;
		host->detect_ecc = hinfc504_detect_ecc;
	}

	nand_oob_resize = hinfc504_ecc_probe;

	return 0;
}
/*****************************************************************************/

static int hinfc504_version_check(void)
{
	unsigned long regval;
	regval = readl(CONFIG_HINFC504_REG_BASE_ADDRESS
		+ HINFC504_VERSION);

	return (regval == HINFC_VER_600);
}
/*****************************************************************************/
int board_nand_init(struct nand_chip *chip)/*, int **ecctype,
	unsigned int *version)*/
{
	printf("Check nand flash controller v504. ");
if (!hinfc504_version_check()) {
		printf("\n");
		return -ENODEV;
	}

	printf("found\n");

	if (host.chip)
		goto out;

#ifdef CONFIG_NAND_NOTICE_RR
	printf("Nand CONFIG_NAND_NOTICE_RR is defined\n");
#endif /* CONFIG_NAND_NOTICE_RR */


	memset((char *)&host, 0, sizeof(struct hinfc_host));

	host.iobase = (void __iomem *)CONFIG_HINFC504_REG_BASE_ADDRESS;

	chip->IO_ADDR_R = chip->IO_ADDR_W =
		(void *)CONFIG_HINFC504_BUFFER_BASE_ADDRESS;
	hinfc504_controller_enable(&host, 1);


	if (hinfc504_nand_init(&host, chip)) {
		printf("failed to allocate device buffer.\n");
		return -ENOMEM;
	}

	nand_spl_ids_register();

out:
/*
	(*ecctype) = &host.ecctype;
	(*version) = HINFC_VER_504;
*/
	return 0;
}
/*****************************************************************************/

int nand_get_ecctype(void)
{
	if (!host.chip) {
		printf("nand flash uninitialized.\n");
		return -1;
	}
	return host.ecctype;
}
