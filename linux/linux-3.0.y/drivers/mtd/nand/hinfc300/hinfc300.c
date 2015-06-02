/******************************************************************************
 * NAND Flash Controller V300 Device Driver
 * Copyright (c) 2009-2010 by Hisilicon.
 * All rights reserved.
 ******************************************************************************/

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <asm/setup.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/mtd/partitions.h>

/*#define CONFIG_HINFC300_DBG_NAND_PROC_FILE
 *#define CONFIG_HINFC300_DBG_NAND_EC_NOTICE
 *#define CONFIG_HINFC300_DBG_NAND_STAT_PROC_FILE
 */
#include "hinfc300.h"
#include "hinfc300dbg.c"
/*****************************************************************************/

static char *get_ecctype_str(enum ecc_type ecctype)
{
	static char *ecctype_str[] = {
		"None", "1bit", "4Bytes", "8Bytes",
		"24bits/1K", "unknown", "unknown", "unknown"
	};
	return ecctype_str[ecctype];
}
/*****************************************************************************/

static char *get_pagesize_str(enum page_type pagetype)
{
	static char *pagesize_str[] = {
		"512", "2K", "4K", "8K", "unknown"
	};
	return pagesize_str[pagetype];
}
/*****************************************************************************/

static unsigned int get_pagesize(enum page_type pagetype)
{
	unsigned int pagesize[] = {_512B, _2K, _4K, _8K, 0};
	return pagesize[pagetype];
}
/*****************************************************************************/

static void hinfc300_dma_transfer(struct hinfc_host *host, int todev)
{
	unsigned long reg_val;
	hinfc_write(host, host->dma_buffer, HINFC300_DMA_ADDR_DATA);
	hinfc_write(host, host->dma_oob, HINFC300_DMA_ADDR_OOB);

	if (host->ecctype == et_ecc_none) {
		hinfc_write(host,
			((host->oobsize & HINFC300_DMA_LEN_OOB_MASK)
				 << HINFC300_DMA_LEN_OOB_SHIFT),
			HINFC300_DMA_LEN);

		hinfc_write(host,
			HINFC300_DMA_PARA_DATA_RW_EN
			| HINFC300_DMA_PARA_OOB_RW_EN
			| ((host->n24bit_ext_len
				& HINFC300_DMA_PARA_EXT_LEN_MASK)
				<< HINFC300_DMA_PARA_EXT_LEN_SHIFT),
			HINFC300_DMA_PARA);
	} else {
		hinfc_write(host,
			HINFC300_DMA_PARA_DATA_RW_EN
			| HINFC300_DMA_PARA_OOB_RW_EN
			| HINFC300_DMA_PARA_DATA_EDC_EN
			| HINFC300_DMA_PARA_OOB_EDC_EN
			| HINFC300_DMA_PARA_DATA_ECC_EN
			| HINFC300_DMA_PARA_OOB_ECC_EN
			| ((host->n24bit_ext_len
				& HINFC300_DMA_PARA_EXT_LEN_MASK)
				<< HINFC300_DMA_PARA_EXT_LEN_SHIFT),
			HINFC300_DMA_PARA);
	}

	reg_val = (HINFC300_DMA_CTRL_DMA_START
		| HINFC300_DMA_CTRL_BURST4_EN
		| HINFC300_DMA_CTRL_BURST8_EN
		| HINFC300_DMA_CTRL_BURST16_EN
		| ((host->addr_cycle == 4 ? 1 : 0)
			<< HINFC300_DMA_CTRL_ADDR_NUM_SHIFT)
		| ((host->chipselect & HINFC300_DMA_CTRL_CS_MASK)
			<< HINFC300_DMA_CTRL_CS_SHIFT));

	if (todev)
		reg_val |= HINFC300_DMA_CTRL_WE;
	else
		reg_val |= (0x03 << 2); /* XXX fix mpw chip bug!*/

	hinfc_write(host, reg_val, HINFC300_DMA_CTRL);

	while ((hinfc_read(host, HINFC300_DMA_CTRL))
			& HINFC300_DMA_CTRL_DMA_START)
		_cond_resched();
}
/*****************************************************************************/

static void hinfc300_cmd_ctrl(struct mtd_info *mtd, int dat,
	unsigned int ctrl)
{
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

		if (host->addr_cycle >= HINFC300_ADDR_CYCLE_MASK) {
			addr_offset = (host->addr_cycle
				- HINFC300_ADDR_CYCLE_MASK)
				<< 3;
			addr_value = 1;
		}

		host->addr_value[addr_value] |= ((dat & 0xff) << addr_offset);

		host->addr_cycle++;
	}

	if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE)) {
		host->command = dat & 0xff;
		switch (host->command) {
		case NAND_CMD_PAGEPROG:

			hinfc_write(host, host->NFC_CON, HINFC300_CON);
			hinfc_write(host, host->addr_value[0] & 0xffff0000,
				HINFC300_ADDRL);
			hinfc_write(host, host->addr_value[1], HINFC300_ADDRH);
			hinfc_write(host,
				NAND_CMD_PAGEPROG << 8 | NAND_CMD_SEQIN,
				HINFC300_CMD);

			hinfc300_dma_transfer(host, 1);

			dbg_nand_proc_save_logs(host, "W");
			dbg_nand_stat_operation(host, "W");

			break;

		case NAND_CMD_READSTART:

			hinfc_write(host, HINFC300_INTCLR_UE, HINFC300_INTCLR);
			hinfc_write(host, host->NFC_CON, HINFC300_CON);
			hinfc_write(host,
				host->addr_value[0] & 0xffff0000,
				HINFC300_ADDRL);
			hinfc_write(host, host->addr_value[1], HINFC300_ADDRH);
			hinfc_write(host,
				NAND_CMD_READSTART << 8 | NAND_CMD_READ0,
				HINFC300_CMD);

			hinfc300_dma_transfer(host, 0);

			host->uc_er = (hinfc_read(host, HINFC300_INTS)
				& HINFC300_INTS_UE)
				? 1 : 0;

			dbg_nand_ec_notice(host);
			dbg_nand_proc_save_logs(host, "R");
			dbg_nand_stat_operation(host, "R");
			host->uc_er = 0;

			break;

		case NAND_CMD_ERASE2:
			hinfc_write(host, host->NFC_CON_ECC_NONE, HINFC300_CON);
			hinfc_write(host, host->addr_value[0], HINFC300_ADDRL);
			hinfc_write(host,
				(NAND_CMD_ERASE2 << 8) | NAND_CMD_ERASE1,
				HINFC300_CMD);

			hinfc_write(host,
				HINFC300_OP_WAIT_READY_EN
				| HINFC300_OP_CMD2_EN
				| HINFC300_OP_CMD1_EN
				| HINFC300_OP_ADDR_EN
				| ((host->chipselect & HINFC300_OP_NF_CS_MASK)
					<< HINFC300_OP_NF_CS_SHIFT)
				| ((host->addr_cycle
				& HINFC300_OP_ADDR_CYCLE_MASK)
					<< HINFC300_OP_ADDR_CYCLE_SHIFT),
				HINFC300_OP);

			while ((hinfc_read(host,
				HINFC300_STATUS) & 0x1) == 0x0)
				;
			dbg_nand_proc_save_logs(host, "E");
			dbg_nand_stat_operation(host, "E");

			break;

		case NAND_CMD_READID:

			memset((unsigned char *)(chip->IO_ADDR_R), 0, 0x10);

			hinfc_write(host, host->NFC_CON_ECC_NONE, HINFC300_CON);
			hinfc_write(host, NAND_CMD_READID, HINFC300_CMD);
			hinfc_write(host, 0, HINFC300_ADDRL);

			hinfc_write(host,
				HINFC300_OP_CMD1_EN
				| HINFC300_OP_ADDR_EN
				| HINFC300_OP_READ_DATA_EN
				| HINFC300_OP_WAIT_READY_EN
				| ((host->chipselect & HINFC300_OP_NF_CS_MASK)
					<< HINFC300_OP_NF_CS_SHIFT)
				| (1 << HINFC300_OP_ADDR_CYCLE_SHIFT),
				HINFC300_OP);

			host->addr_cycle = 0x0;

			while ((hinfc_read(host,
				HINFC300_STATUS) & 0x1) == 0x0)
				;

			break;

		case NAND_CMD_STATUS:

			hinfc_write(host,
				host->NFC_CON_ECC_NONE,
				HINFC300_CON);
			hinfc_write(host,
				NAND_CMD_STATUS,
				HINFC300_CMD);
			hinfc_write(host,
				HINFC300_OP_CMD1_EN
				| HINFC300_OP_READ_DATA_EN
				| HINFC300_OP_WAIT_READY_EN
				| ((host->chipselect & HINFC300_OP_NF_CS_MASK)
					<< HINFC300_OP_NF_CS_SHIFT),
				HINFC300_OP);

			while ((hinfc_read(host,
				HINFC300_STATUS) & 0x1) == 0x0)
				;

			break;

		case NAND_CMD_SEQIN:
		case NAND_CMD_ERASE1:
		case NAND_CMD_READ0:
			break;
		case NAND_CMD_RESET:

			hinfc_write(host, NAND_CMD_RESET, HINFC300_CMD);

			hinfc_write(host,
				(HINFC300_OP_CMD1_EN
				 | ((host->chipselect & HINFC300_OP_NF_CS_MASK)
				 << HINFC300_OP_NF_CS_SHIFT)
				 | HINFC300_OP_WAIT_READY_EN),
				HINFC300_OP);

			while ((hinfc_read(host,
				HINFC300_STATUS) & 0x1) == 0x0)
				;

			break;

		default:
			break;
	}
}

	if ((dat == NAND_CMD_NONE) && host->addr_cycle) {
		if (host->command == NAND_CMD_SEQIN
				|| host->command == NAND_CMD_READ0
				|| host->command == NAND_CMD_READID) {
			host->offset = 0x0;
			host->column = (host->addr_value[0] & 0xffff);
		}
	}
}
/*****************************************************************************/

static int hinfc300_dev_ready(struct mtd_info *mtd)
{
	return 0x1;
}
/*****************************************************************************/

static void hinfc300_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > 2)
		DBG_BUG("invalid chipselect: %d\n", chipselect);


	host->chipselect = chipselect;
}
/*****************************************************************************/

static uint8_t hinfc300_read_byte(struct mtd_info *mtd)
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

static u16 hinfc300_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	host->offset += 2;

	return readw(host->buffer + host->column + host->offset - 2);
}
/*****************************************************************************/

static void hinfc300_write_buf(struct mtd_info *mtd,
	const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(host->buffer + host->column + host->offset, buf, len);
	host->offset += len;
}
/*****************************************************************************/

static void hinfc300_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(buf, host->buffer + host->column + host->offset, len);
	host->offset += len;
}
/*****************************************************************************/

static char *ToSizeString(unsigned long long size)
{
	int ix;
	static char buffer[20];
	char *fmt[] = {"%uByte", "%uK", "%uM", "%uG", "%uT", "%uT"};

	for (ix = 0; (ix < 5) && (size >= 1024); ix++)
		size = (size >> 10);

	sprintf(buffer, fmt[ix], size);
	return buffer;
}
/*****************************************************************************/

static struct nand_ecclayout nand_ecc_default = {
	.oobfree = { {2, 30} }
};

static struct nand_ecclayout nand_ecc_2K_1bit = {
	.oobfree = { {22, 30} }
};
/*****************************************************************************/

static struct page_page_ecc_info page_page_ecc_info[] = {
	{pt_pagesize_8K, et_ecc_24bit1k, 368, &nand_ecc_default},
	{pt_pagesize_8K, et_ecc_none,    32,  &nand_ecc_default},

	{pt_pagesize_4K, et_ecc_24bit1k, 200, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_4bytes,  128, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_1bit,    128, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_none,    32,  &nand_ecc_default},

	{pt_pagesize_2K, et_ecc_1bit,    64,  &nand_ecc_2K_1bit},
	{pt_pagesize_2K, et_ecc_none,    32,  &nand_ecc_default},

	{0},
};
/*****************************************************************************/
/*
 * used the best correct arithmetic.
 */
struct page_page_ecc_info *hinfc300_get_best_ecc(struct mtd_info *mtd)
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

	if (!best) {
		DBG_BUG("not support this pagesize(%d) and oobsize(%d).\n",
				mtd->writesize,	mtd->oobsize);
	}

	return best;
}
/*****************************************************************************/
/* force the pagesize and ecctype */
struct page_page_ecc_info *hinfc300_force_ecc(struct mtd_info *mtd,
	enum page_type pagetype,
	enum ecc_type ecctype,
	char *cfgmsg,
	int allow_pagediv)
{
	int pagesize;
	struct page_page_ecc_info *fit = NULL;
	struct page_page_ecc_info *info = page_page_ecc_info;

	for (; info->layout; info++) {
		if (info->pagetype == pagetype
			&& info->ecctype == ecctype) {
			fit = info;
			break;
		}
	}

	if (!fit) {
		DBG_BUG("system not support this nand %s:"
			" pagesize:%s, ecctype:%s\n",
			cfgmsg,
			get_pagesize_str(pagetype),
			get_ecctype_str(ecctype));
		return NULL;
	}

	pagesize = get_pagesize(pagetype);

	if ((pagesize != mtd->writesize)
		&& (pagesize > mtd->writesize || !allow_pagediv)) {
		DBG_BUG("%s is inconsistent, config pagesize %d, "
			"but the nand chip pageszie is %d\n",
			cfgmsg,
			pagesize,
			mtd->writesize);
		return NULL;
	}

	if (fit->oobsize > mtd->oobsize) {
		DBG_BUG("%s is inconsistent,"
			" config %s require oobsize(%d), "
			"but the nand chip oobsize is %d,"
			" littler than require.\n",
			cfgmsg, get_ecctype_str(ecctype),
			fit->oobsize,
			mtd->oobsize);
		return NULL;
	}

	return fit;
}
/*****************************************************************************/

static void hinfc300_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip)
{
	char *start_type = "unknown";
	struct page_page_ecc_info *best = NULL;
	struct hinfc_host *host = chip->priv;

#ifdef CONFIG_HINFC300_AUTO_PAGESIZE_ECC
	best = hinfc300_get_best_ecc(mtd);
	start_type = "Auto";
#endif /* CONFIG_HINFC300_AUTO_PAGESIZE_ECC */

#ifdef CONFIG_HINFC300_HARDWARE_PAGESIZE_ECC
	best = hinfc300_force_ecc(mtd,
		((host->NFC_CON >> HINFC300_CON_PAGEISZE_SHIFT)
			 & HINFC300_CON_PAGESIZE_MASK),
		((host->NFC_CON >> HINFC300_CON_ECCTYPE_SHIFT)
			 & HINFC300_CON_ECCTYPE_MASK),
		"hardware config", 0);
	start_type = "Hardware";
#endif /* CONFIG_HINFC300_HARDWARE_PAGESIZE_ECC */

#ifdef CONFIG_HINFC300_PAGESIZE_AUTO_ECC_NONE
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
		default:
			pagetype = pt_pagesize_2K;
			break;
		}
		best = hinfc300_force_ecc(mtd, pagetype,
			et_ecc_none, "force config", 0);
		start_type = "AutoForce";
	}
#endif /* CONFIG_HINFC300_PAGESIZE_AUTO_ECC_NONE */

	if (!best) {
		DBG_BUG("Please select nand flash pagesize"
			" and ecc type!\n");
	}

	if (best->ecctype != et_ecc_none)
		mtd->oobsize = best->oobsize;
	chip->ecc.layout = best->layout;

	host->ecctype  = best->ecctype;
	host->pagesize = get_pagesize(best->pagetype);
	host->oobsize  = mtd->oobsize;

	if (host->ecctype == et_ecc_24bit1k) {
		if (host->pagesize == _4K)
			host->n24bit_ext_len = 0x03; /* 8bytes */
		else if (host->pagesize == _8K)
			host->n24bit_ext_len = 0x01; /* 4bytes */
	}
	host->dma_oob = host->dma_buffer + host->pagesize;

	host->NFC_CON  = (HINFC300_CON_OP_MODE_NORMAL
			| ((best->pagetype & HINFC300_CON_PAGESIZE_MASK)
				<< HINFC300_CON_PAGEISZE_SHIFT)
			| HINFC300_CON_READY_BUSY_SEL
			| ((best->ecctype & HINFC300_CON_ECCTYPE_MASK)
				<< HINFC300_CON_ECCTYPE_SHIFT));

	host->NFC_CON_ECC_NONE = (HINFC300_CON_OP_MODE_NORMAL
			| ((best->pagetype & HINFC300_CON_PAGESIZE_MASK)
				<< HINFC300_CON_PAGEISZE_SHIFT)
			| HINFC300_CON_READY_BUSY_SEL);

	if (mtd->writesize > NAND_MAX_PAGESIZE
		|| mtd->oobsize > NAND_MAX_OOBSIZE) {
		DBG_BUG("kernel not support this NAND. "
			"please increase NAND_MAX_PAGESIZE and NAND_MAX_OOBSIZE.\n");
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
		printk(KERN_INFO "Nand divide into 1/%u\n", (1 << shift));
	}

	dbg_nand_stat_proc_init(mtd->writesize, mtd->erasesize);

	printk(KERN_INFO "Nand(%s): ", start_type);
	printk("Block:%s ", ToSizeString(mtd->erasesize));
	printk("Page:%s ", ToSizeString(mtd->writesize));
	printk("Ecc:%s ", get_ecctype_str(host->ecctype));
	printk("Chip:%s*%d ", ToSizeString(chip->chipsize),
		CONFIG_HINFC300_MAX_CHIP);
	printk("OOB:%s", ToSizeString(mtd->oobsize));
	printk("\n");
}
/*****************************************************************************/

static int hinfc300_nand_init(struct hinfc_host *host, struct nand_chip *chip)
{
	chip->priv        = host;
	chip->cmd_ctrl    = hinfc300_cmd_ctrl;
	chip->dev_ready   = hinfc300_dev_ready;
	chip->select_chip = hinfc300_select_chip;
	chip->read_byte   = hinfc300_read_byte;
	chip->read_word   = hinfc300_read_word;
	chip->write_buf   = hinfc300_write_buf;
	chip->read_buf    = hinfc300_read_buf;

	chip->chip_delay = HINFC300_CHIP_DELAY;
	chip->options    = NAND_NO_AUTOINCR |  NAND_SKIP_BBTSCAN;

	chip->ecc.layout = NULL;
	chip->ecc.mode   = NAND_ECC_NONE;

	host->chip          = chip;
	host->addr_cycle    = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->chipselect    = 0;

	host->NFC_CON = (hinfc_read(host, HINFC300_CON)
		| HINFC300_CON_OP_MODE_NORMAL
		| HINFC300_CON_READY_BUSY_SEL);
	host->NFC_CON_ECC_NONE = (host->NFC_CON
		& (~(HINFC300_CON_ECCTYPE_MASK
			<< HINFC300_CON_ECCTYPE_SHIFT)));

	host->buffer = dma_alloc_coherent(host->dev,
		(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE),
		&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer)
		return 1;
	memset(host->buffer, 0xff, (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE));

	hinfc_write(host, SET_HINFC300_PWIDTH(5, 5, 3),
		HINFC300_PWIDTH);

	nand_base_oob_resize = hinfc300_ecc_probe;

	dbg_nand_ec_init();
	dbg_nand_proc_init();

	return 0;
}
/*****************************************************************************/

static void hinfc300_controller_enable(struct hinfc_host *host, int enable)
{
	unsigned int reg_val = readl(host->sysreg + PERI_CRG30);
	if (enable)
		reg_val |= (PERI_CRG30_CLK_EN | PERI_CRG30_CLK_SEL_99M);
	else
		reg_val &= ~PERI_CRG30_CLK_EN;

	writel(reg_val, (host->sysreg + PERI_CRG30));
}
/*****************************************************************************/
struct ptn_entry {
	char name[16];
	unsigned long long start;
	unsigned long long length;
	unsigned int flags;
};

struct ptn_info {
	int nParts;

#define MAX_PARTITIONS  (16)
	struct ptn_entry entry[MAX_PARTITIONS];

	struct mtd_partition parts[MAX_PARTITIONS];
};

static struct ptn_info ptn_info;
/*****************************************************************************/
static int __init parse_nand_partitions(const struct tag *tag)
{
	int i;

	if (tag->hdr.size <= 2) {
		printk(KERN_INFO "%s(%d): tag->hdr.size <= 2\n",
			__func__, __LINE__);
		return 0;
	}
	ptn_info.nParts = (tag->hdr.size - 2)
		/ (sizeof(struct ptn_entry)/sizeof(int));
	memcpy(ptn_info.entry, &tag->u,
		ptn_info.nParts * sizeof(struct ptn_entry));

	for (i = 0; i < ptn_info.nParts; i++) {
		ptn_info.parts[i].name   = ptn_info.entry[i].name;
		ptn_info.parts[i].size   = (ptn_info.entry[i].length);
		ptn_info.parts[i].offset = (ptn_info.entry[i].start);
		ptn_info.parts[i].mask_flags = 0;
		ptn_info.parts[i].ecclayout  = 0;
	}

	return 0;
}

/* turn to asci is "HiNp" */
__tagtable(0x48694E70, parse_nand_partitions);

/*****************************************************************************/
static int hinfc300_probe(struct platform_device *pltdev)
{
	int result = 0;

	struct hinfc_host *host;
	struct nand_chip  *chip;
	struct mtd_info   *mtd;

	int size = sizeof(struct hinfc_host) + sizeof(struct nand_chip)
		+ sizeof(struct mtd_info);

	host = kmalloc(size, GFP_KERNEL);
	if (!host) {
		dev_err(&pltdev->dev, "failed to allocate device structure.\n");
		return -ENOMEM;
	}

	memset((char *)host, 0, size);
	platform_set_drvdata(pltdev, host);

	host->dev  = &pltdev->dev;
	host->chip = chip = (struct nand_chip *)&host[1];
	host->mtd  = mtd  = (struct mtd_info *)&chip[1];

	host->iobase = ioremap(pltdev->resource[0].start,
		pltdev->resource[0].end
		- pltdev->resource[0].start + 1);
	if (!host->iobase) {
		dev_err(&pltdev->dev, "ioremap failed\n");
		kfree(host);
		return -EIO;
	}

	mtd->priv  = chip;
	mtd->owner = THIS_MODULE;
	mtd->name  = (char *)(pltdev->name);

	chip->IO_ADDR_R = chip->IO_ADDR_W = ioremap_nocache(
		pltdev->resource[1].start,
		pltdev->resource[1].end
		- pltdev->resource[1].start + 1);
	if (!chip->IO_ADDR_R) {
		dev_err(&pltdev->dev, "ioremap failed\n");
		iounmap(host->iobase);
		kfree(host);
		return -EIO;
	}
	memset((char *)chip->IO_ADDR_R, 0xFF, HINFC300_BUFFER_BASE_ADDRESS_LEN);

	host->sysreg = ioremap_nocache(HINFC300_PERIPHERY_REGBASE, 0x100);
	if (!host->sysreg) {
		dev_err(&pltdev->dev, "ioremap failed\n");
		iounmap(chip->IO_ADDR_W);
		iounmap(host->iobase);
		kfree(host);
		return -EIO;
	}
	hinfc300_controller_enable(host, 1);

	if (hinfc300_nand_init(host, chip)) {
		dev_err(&pltdev->dev, "failed to allocate device buffer.\n");
		result = -ENOMEM;
		goto err;
	}

	if (nand_scan(mtd, CONFIG_HINFC300_MAX_CHIP)) {
		result = -ENXIO;
		goto err;
	}

	int i;
	int nr_parts = 0;
	struct mtd_partition *parts = NULL;

#ifdef CONFIG_MTD_CMDLINE_PARTS
	static const char * const part_probes[] = {"cmdlinepart", NULL,};
	nr_parts = parse_mtd_partitions(mtd,
			part_probes, &parts, 0);
#endif
	if (!nr_parts) {
		nr_parts = ptn_info.nParts;
		parts    = ptn_info.parts;
	}

	result = mtd_device_register(mtd, parts, nr_parts);
	if (!result)
		return result;



	result = -ENODEV;
	nand_release(mtd);

err:
	if (host->buffer) {
		dma_free_coherent(host->dev,
			(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE),
			host->buffer, host->dma_buffer);
		host->buffer = NULL;
	}
	iounmap(chip->IO_ADDR_W);
	iounmap(host->iobase);
	iounmap(host->sysreg);
	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return result;
}
/*****************************************************************************/

int hinfc300_remove(struct platform_device *pltdev)
{
	struct hinfc_host *host = platform_get_drvdata(pltdev);

	hinfc300_controller_enable(host, 0);

	nand_release(host->mtd);
	dma_free_coherent(host->dev, (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE),
		host->buffer, host->dma_buffer);
	iounmap(host->sysreg);
	iounmap(host->chip->IO_ADDR_W);
	iounmap(host->iobase);
	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return 0;
}
/*****************************************************************************/

static void hinfc300_pltdev_release(struct device *dev)
{
}
/*****************************************************************************/
#ifdef CONFIG_PM
static int hinfc300_suspend(struct platform_device *pltdev, pm_message_t state)
{
	struct hinfc_host *host = platform_get_drvdata(pltdev);

	while ((hinfc_read(host, HINFC300_STATUS) & 0x1) == 0x0)
		;

	while ((hinfc_read(host, HINFC300_DMA_CTRL))
		& HINFC300_DMA_CTRL_DMA_START)
		_cond_resched();

	hinfc300_controller_enable(host, 0);

	return 0;
}
/*****************************************************************************/

static int hinfc300_resume(struct platform_device *pltdev)
{
	struct hinfc_host *host = platform_get_drvdata(pltdev);

	hinfc300_controller_enable(host, 1);

	return 0;
}
#endif /* CONFIG_PM */
/*****************************************************************************/
static struct platform_driver hinfc300_pltdrv = {
	.driver.name   = "hinand",
	.probe = hinfc300_probe,
	.remove = hinfc300_remove,
#ifdef CONFIG_PM
	.suspend  = hinfc300_suspend,
	.resume   = hinfc300_resume,
#endif /* CONFIG_PM */
};
/*****************************************************************************/

static struct resource hinfc300_resources[] = {
	{
		.start = HINFC300_REG_BASE_ADDRESS,
		.end   = HINFC300_REG_BASE_ADDRESS
			+ HINFC300_REG_BASE_ADDRESS_LEN,
		.flags = IORESOURCE_MEM,
	},

	{
		.start = HINFC300_BUFFER_BASE_ADDRESS,
		.end   = HINFC300_BUFFER_BASE_ADDRESS
			+ HINFC300_BUFFER_BASE_ADDRESS_LEN,
		.flags = IORESOURCE_MEM,
	},
};
/*****************************************************************************/

static struct platform_device hinfc300_pltdev = {
	.name = "hinand",
	.id   = -1,

	.dev.platform_data     = NULL,
	.dev.dma_mask          = (u64 *)~0,
	.dev.coherent_dma_mask = (u64) ~0,
	.dev.release           = hinfc300_pltdev_release,

	.num_resources = ARRAY_SIZE(hinfc300_resources),
	.resource      = hinfc300_resources,
};
/*****************************************************************************/

static int __init hinfc300_module_init(void)
{
	int result = 0;

	printk(KERN_INFO "Hisilicon Nand Flash Controller V300 Device Driver,"
		"Version 1.10\n");

	result = platform_driver_register(&hinfc300_pltdrv);
	if (result < 0)
		return result;

	result = platform_device_register(&hinfc300_pltdev);
	if (result < 0) {
		platform_driver_unregister(&hinfc300_pltdrv);
		return result;
	}

	return result;
}
/*****************************************************************************/

static void __exit hinfc300_module_exit(void)
{
	platform_driver_unregister(&hinfc300_pltdrv);
	platform_device_unregister(&hinfc300_pltdev);
}
/*****************************************************************************/

module_init(hinfc300_module_init);
module_exit(hinfc300_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HIC");
MODULE_DESCRIPTION("Hisilicon Nand Flash Controller V300 Device Driver, Version 1.10");

/*****************************************************************************/
