/******************************************************************************
*    NAND Flash Controller V301 Device Driver
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
* ***
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
 * #define CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC
 */
#include "hinfc301.h"
/*****************************************************************************/

extern void hinfc301_controller_enable(struct hinfc_host *host, int enable);

extern void nand_spl_ids_register(void);

extern int (*nand_base_oob_resize)(struct mtd_info *mtd,
	struct nand_chip *chip, char *args);

unsigned int nand_ip_version = 0x0301;

static struct hinfc_host *host;

/*****************************************************************************/

static void nand_register_dump(void)
{
	int ix;
	unsigned int regbase = CONFIG_HINFC301_REG_BASE_ADDRESS;

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
	static char *pagesize_str[] = {"512", "2K", "4K", "8K", "unknown" };
	return pagesize_str[pagetype];
}
/*****************************************************************************/

static unsigned int get_pagesize(enum page_type pagetype)
{
	unsigned int pagesize[] = {_512B, _2K, _4K, _8K, 0};
	return pagesize[pagetype];
}
/*****************************************************************************/

static void hinfc301_dma_transfer(struct hinfc_host *host, int todev)
{
	unsigned long reg_val;
	hinfc_write(host, host->dma_buffer, HINFC301_DMA_ADDR_DATA);
	hinfc_write(host, host->dma_oob, HINFC301_DMA_ADDR_OOB);

	if (host->ecctype == et_ecc_none) {
		hinfc_write(host,
			((host->oobsize & HINFC301_DMA_LEN_OOB_MASK)
			<< HINFC301_DMA_LEN_OOB_SHIFT),
			HINFC301_DMA_LEN);

		hinfc_write(host,
			HINFC301_DMA_PARA_DATA_RW_EN
			| HINFC301_DMA_PARA_OOB_RW_EN
			| ((host->n24bit_ext_len
			& HINFC301_DMA_PARA_EXT_LEN_MASK)
			<< HINFC301_DMA_PARA_EXT_LEN_SHIFT),
			HINFC301_DMA_PARA);
	} else {
		hinfc_write(host,
			HINFC301_DMA_PARA_DATA_RW_EN
			| HINFC301_DMA_PARA_OOB_RW_EN
			| HINFC301_DMA_PARA_DATA_EDC_EN
			| HINFC301_DMA_PARA_OOB_EDC_EN
			| HINFC301_DMA_PARA_DATA_ECC_EN
			| HINFC301_DMA_PARA_OOB_ECC_EN
			| ((host->n24bit_ext_len
			& HINFC301_DMA_PARA_EXT_LEN_MASK)
			<< HINFC301_DMA_PARA_EXT_LEN_SHIFT),
			HINFC301_DMA_PARA);
	}

	reg_val = (HINFC301_DMA_CTRL_DMA_START
		| HINFC301_DMA_CTRL_BURST4_EN
		| HINFC301_DMA_CTRL_BURST8_EN
		| HINFC301_DMA_CTRL_BURST16_EN
		| HINFC301_DMA_CTRL_DATA_AREA_EN
		| HINFC301_DMA_CTRL_OOB_AREA_EN
		| ((host->addr_cycle == 4 ? 1 : 0)
		<< HINFC301_DMA_CTRL_ADDR_NUM_SHIFT)
		| ((host->chipselect
		& HINFC301_DMA_CTRL_CS_MASK)
		<< HINFC301_DMA_CTRL_CS_SHIFT));

	if (todev)
		reg_val |= HINFC301_DMA_CTRL_WE;

	hinfc_write(host, reg_val, HINFC301_DMA_CTRL);

	while ((hinfc_read(host, HINFC301_DMA_CTRL))
			& HINFC301_DMA_CTRL_DMA_START)
		;
}
/*****************************************************************************/

static void hinfc301_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
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
		addr_offset =  host->addr_cycle << 3;

		if (host->addr_cycle >= HINFC301_ADDR_CYCLE_MASK) {
			addr_offset = (host->addr_cycle
					- HINFC301_ADDR_CYCLE_MASK) << 3;
			addr_value = 1;
		}

		host->addr_value[addr_value] |= ((dat & 0xff) << addr_offset);

		host->addr_cycle++;
	}

	if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE)) {
		host->command = dat & 0xff;
		switch (host->command) {
		case NAND_CMD_PAGEPROG:

			hinfc_write(host, host->NFC_CON, HINFC301_CON);
			hinfc_write(host, host->addr_value[0]
					& 0xffff0000, HINFC301_ADDRL);
			hinfc_write(host, host->addr_value[1], HINFC301_ADDRH);
			hinfc_write(host, NAND_CMD_PAGEPROG
					<< 8 | NAND_CMD_SEQIN, HINFC301_CMD);

			hinfc301_dma_transfer(host, 1);

			break;

		case NAND_CMD_READSTART:

			hinfc_write(host, HINFC301_INTCLR_UE, HINFC301_INTCLR);
			hinfc_write(host, host->NFC_CON, HINFC301_CON);
			hinfc_write(host, host->addr_value[0]
					& 0xffff0000, HINFC301_ADDRL);
			hinfc_write(host, host->addr_value[1], HINFC301_ADDRH);
			hinfc_write(host, NAND_CMD_READSTART
					<< 8 | NAND_CMD_READ0, HINFC301_CMD);

			hinfc_write(host, 0, HINFC301_LOG_READ_ADDR);
			hinfc_write(host, (host->pagesize + host->oobsize),
					HINFC301_LOG_READ_LEN);

			hinfc301_dma_transfer(host, 0);
			host->uc_er = ((hinfc_read(host, HINFC301_INTS)
						& HINFC301_INTS_UE) ? 1 : 0);
			host->uc_er = 0;

			break;

		case NAND_CMD_ERASE2:
			hinfc_write(host, host->NFC_CON_ECC_NONE, HINFC301_CON);
			hinfc_write(host, host->addr_value[0], HINFC301_ADDRL);
			hinfc_write(host, (NAND_CMD_ERASE2 << 8)
					| NAND_CMD_ERASE1, HINFC301_CMD);

			hinfc_write(host,
				HINFC301_OP_WAIT_READY_EN
				| HINFC301_OP_CMD2_EN
				| HINFC301_OP_CMD1_EN
				| HINFC301_OP_ADDR_EN
				| ((host->chipselect
				& HINFC301_OP_NF_CS_MASK)
				<< HINFC301_OP_NF_CS_SHIFT)
				| ((host->addr_cycle
				& HINFC301_OP_ADDR_CYCLE_MASK)
				<< HINFC301_OP_ADDR_CYCLE_SHIFT),
				HINFC301_OP);

			while ((hinfc_read(host, HINFC301_STATUS) & 0x1) == 0x0)
				;

			break;

		case NAND_CMD_READID:

			memset((unsigned char *)(chip->IO_ADDR_R), 0, 0x10);

			hinfc_write(host, host->NFC_CON_ECC_NONE, HINFC301_CON);
			hinfc_write(host, NAND_CMD_READID, HINFC301_CMD);
			hinfc_write(host, 0, HINFC301_ADDRL);

			hinfc_write(host,
				HINFC301_OP_CMD1_EN
				| HINFC301_OP_ADDR_EN
				| HINFC301_OP_READ_DATA_EN
				| HINFC301_OP_WAIT_READY_EN
				| ((host->chipselect & HINFC301_OP_NF_CS_MASK)
				<< HINFC301_OP_NF_CS_SHIFT)
				| (1 << HINFC301_OP_ADDR_CYCLE_SHIFT),
				HINFC301_OP);

			host->addr_cycle = 0x0;

			while ((hinfc_read(host, HINFC301_STATUS) & 0x1) == 0x0)
				;

			break;

		case NAND_CMD_STATUS:

			hinfc_write(host, host->NFC_CON_ECC_NONE, HINFC301_CON);
			hinfc_write(host, NAND_CMD_STATUS, HINFC301_CMD);
			hinfc_write(host,
				HINFC301_OP_CMD1_EN
				| HINFC301_OP_READ_DATA_EN
				| HINFC301_OP_WAIT_READY_EN
				| ((host->chipselect & HINFC301_OP_NF_CS_MASK)
				<< HINFC301_OP_NF_CS_SHIFT),
				HINFC301_OP);

			while ((hinfc_read(host, HINFC301_STATUS) & 0x1) == 0x0)
				;

			break;

		case NAND_CMD_SEQIN:
		case NAND_CMD_ERASE1:
		case NAND_CMD_READ0:
			break;
		case NAND_CMD_RESET:

			hinfc_write(host, NAND_CMD_RESET, HINFC301_CMD);

			hinfc_write(host,
				(HINFC301_OP_CMD1_EN
				| ((host->chipselect & HINFC301_OP_NF_CS_MASK)
				<< HINFC301_OP_NF_CS_SHIFT)
				| HINFC301_OP_WAIT_READY_EN),
				HINFC301_OP);

			while ((hinfc_read(host, HINFC301_STATUS) & 0x1) == 0x0)
				;

			break;

		default:
			break;
		}
	}

	/*
	 *  pass pagesize and ecctype to kernel when startup.
	 */
	hinfc_write(host, host->NFC_CON, HINFC301_CON);

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

static int hinfc301_dev_ready(struct mtd_info *mtd)
{
	mtd = mtd;
	return 0x1;
}
/*****************************************************************************/

static void hinfc301_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_HINFC301_MAX_CHIP)
		DBG_BUG("invalid chipselect: %d\n", chipselect);

	host->chipselect = chipselect;
}
/*****************************************************************************/

static uint8_t hinfc301_read_byte(struct mtd_info *mtd)
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

static u16 hinfc301_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	host->offset += 2;
	return readw(host->buffer + host->column + host->offset - 2);
}
/*****************************************************************************/

static void hinfc301_write_buf(struct mtd_info *mtd,
		const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(host->buffer + host->column + host->offset, buf, len);
	host->offset += len;
}
/*****************************************************************************/

static void hinfc301_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;

	memcpy(buf, host->buffer + host->column + host->offset, len);
	host->offset += len;
}
/*****************************************************************************/

static struct nand_ecclayout nand_ecc_default = {
	.oobfree = {{2, 30} }
};

static struct nand_ecclayout nand_ecc_2K_1bit = {
	.oobfree = {{22, 30} }
};
#ifdef CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC
static struct nand_ecclayout nand_ecc_2K_4bytes = {
	.oobfree = {{2, 22} }
};
#endif /* CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC */
/*****************************************************************************/

static struct page_page_ecc_info page_page_ecc_info[] = {
	{pt_pagesize_8K, et_ecc_24bit1k, 368, &nand_ecc_default},
	{pt_pagesize_8K, et_ecc_none,    32,  &nand_ecc_default},

	{pt_pagesize_4K, et_ecc_24bit1k, 200, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_4bytes,  128, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_1bit,    128, &nand_ecc_default},
	{pt_pagesize_4K, et_ecc_none,    32,  &nand_ecc_default},

	{pt_pagesize_2K, et_ecc_1bit,    64,  &nand_ecc_2K_1bit},
	/*
	 * 2k4bytes not support yaffs2 file system, it's not compatibility.
	 * only hardward config mode allow support 2k4Bytes,
	 */
#ifdef CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC
	{pt_pagesize_2K, et_ecc_4bytes,  64,  &nand_ecc_2K_4bytes},
#endif /* CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC */
	{pt_pagesize_2K, et_ecc_none,    32,  &nand_ecc_default},

	{0, 0, 0, NULL},
};
/*****************************************************************************/
/* used the best correct arithmetic. */
struct page_page_ecc_info *hinfc301_get_best_ecc(struct mtd_info *mtd)
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
			mtd->writesize, mtd->oobsize);
	}

	return best;
}
/*****************************************************************************/
/* force the pagesize and ecctype */
struct page_page_ecc_info *hinfc301_force_ecc
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

	for (; info->layout; info++) {
		if (info->pagetype == pagetype
			&& info->ecctype == ecctype) {
			fit = info;
			break;
		}
	}

	if (!fit) {
		nand_register_dump();
		DBG_BUG("system not support this nand %s:"
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
		DBG_BUG("%s is inconsistent, config pagesize %d, "
			"but the nand chip pageszie is %d\n",
			cfgmsg,
			pagesize,
			mtd->writesize);
		return NULL;
	}

	if (fit->oobsize > mtd->oobsize) {
		DBG_BUG("%s is inconsistent, config %s require oobsize(%d), "
			"but the nand chip oobsize is %d, littler than config require.\n",
			cfgmsg, get_ecctype_str(ecctype),
			fit->oobsize,
			mtd->oobsize);
		return NULL;
	}

	return fit;
}
/*****************************************************************************/

static int hinfc301_ecc_probe(struct mtd_info *mtd,
		struct nand_chip *chip, char *args) {
	char *start_type = "unknown";
	struct page_page_ecc_info *best = NULL;
	struct hinfc_host *host = chip->priv;

#ifdef CONFIG_HINFC301_AUTO_PAGESIZE_ECC
	best = hinfc301_get_best_ecc(mtd);
	start_type = "Auto";
#endif /* CONFIG_HINFC301_AUTO_PAGESIZE_ECC */

#ifdef CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC
#  ifdef CONFIG_HINFC301_AUTO_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC301_AUTO_PAGESIZE_ECC \
	and CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC at the same time
#  endif
	best = hinfc301_force_ecc(mtd,
		((host->NFC_CON >> HINFC301_CON_PAGEISZE_SHIFT)
		& HINFC301_CON_PAGESIZE_MASK),
		((host->NFC_CON >> HINFC301_CON_ECCTYPE_SHIFT)
		& HINFC301_CON_ECCTYPE_MASK),
		"hardware config", 0);
	start_type = "Hardware";
#endif /* CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC */

#ifdef CONFIG_HINFC301_PAGESIZE_AUTO_ECC_NONE
#  ifdef CONFIG_HINFC301_AUTO_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC301_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HINFC301_AUTO_PAGESIZE_ECC at the same time
#  endif
#  ifdef CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC
#  error you SHOULD NOT define CONFIG_HINFC301_PAGESIZE_AUTO_ECC_NONE \
	and CONFIG_HINFC301_HARDWARE_PAGESIZE_ECC at the same time
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
		default:
			pagetype = pt_pagesize_2K;
			break;
		}
		best = hinfc301_force_ecc(mtd, pagetype,
				et_ecc_none, "force config", 0);
		start_type = "AutoForce";
	}
#endif /* CONFIG_HINFC301_PAGESIZE_AUTO_ECC_NONE */

	if (!best)
		DBG_BUG("Please select nand flash pagesize and ecctype!\n");

	if (best->ecctype != et_ecc_none)
		mtd->oobsize = best->oobsize;
	chip->ecc.layout = best->layout;

	host->ecctype  = best->ecctype;
	host->pagesize = get_pagesize(best->pagetype);
	host->oobsize  = mtd->oobsize;

	*(int *)args = host->ecctype;

	if (host->ecctype == et_ecc_24bit1k) {
		if (host->pagesize == _4K)
			host->n24bit_ext_len = 0x03; /* 8bytes; */
		else if (host->pagesize == _8K)
			host->n24bit_ext_len = 0x01; /* 4bytes; */
	}
	host->dma_oob = host->dma_buffer + host->pagesize;

	host->NFC_CON  = (HINFC301_CON_OP_MODE_NORMAL
		| ((best->pagetype & HINFC301_CON_PAGESIZE_MASK)
		<< HINFC301_CON_PAGEISZE_SHIFT)
		| HINFC301_CON_READY_BUSY_SEL
		| ((best->ecctype & HINFC301_CON_ECCTYPE_MASK)
		<< HINFC301_CON_ECCTYPE_SHIFT));

	host->NFC_CON_ECC_NONE  = (HINFC301_CON_OP_MODE_NORMAL
		| ((best->pagetype & HINFC301_CON_PAGESIZE_MASK)
		<< HINFC301_CON_PAGEISZE_SHIFT)
		| HINFC301_CON_READY_BUSY_SEL);

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

	printk(KERN_INFO "Nand(%s): ", start_type);
	return 0;
}
/*****************************************************************************/

static int hinfc301_nand_init(struct hinfc_host *host, struct nand_chip *chip)
{
	chip->priv        = host;
	chip->cmd_ctrl    = hinfc301_cmd_ctrl;
	chip->dev_ready   = hinfc301_dev_ready;
	chip->select_chip = hinfc301_select_chip;
	chip->read_byte   = hinfc301_read_byte;
	chip->read_word   = hinfc301_read_word;
	chip->write_buf   = hinfc301_write_buf;
	chip->read_buf    = hinfc301_read_buf;

	chip->chip_delay = HINFC301_CHIP_DELAY;
	chip->options    = NAND_NO_AUTOINCR |  NAND_BBT_SCANNED;

	chip->ecc.layout = NULL;
	chip->ecc.mode   = NAND_ECC_NONE;

	host->chip          = chip;
	host->addr_cycle    = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->chipselect    = 0;

	host->NFC_CON = (hinfc_read(host, HINFC301_CON)
		| HINFC301_CON_OP_MODE_NORMAL | HINFC301_CON_READY_BUSY_SEL);
	host->NFC_CON_ECC_NONE = (host->NFC_CON
		& (~(HINFC301_CON_ECCTYPE_MASK << HINFC301_CON_ECCTYPE_SHIFT)));

	host->buffer = kmalloc((NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE),
		GFP_KERNEL);
	host->dma_buffer = (unsigned int)host->buffer;
	if (!host->buffer)
		return 1;

	memset((char *)chip->IO_ADDR_R, 0xff, HINFC301_BUFFER_BASE_ADDRESS_LEN);
	memset(host->buffer, 0xff, (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE));

	hinfc_write(host,
		SET_HINFC301_PWIDTH(CONFIG_HINFC301_W_LATCH,
		CONFIG_HINFC301_R_LATCH,
		CONFIG_HINFC301_RW_LATCH),
		HINFC301_PWIDTH);

	nand_base_oob_resize = hinfc301_ecc_probe;

	return 0;
}
/*****************************************************************************/

int board_nand_init(struct nand_chip *chip)
{
	if (host)
		return -1;

	host = kmalloc(sizeof(struct hinfc_host), GFP_KERNEL);
	if (!host) {
		printk(KERN_INFO "failed to allocate device structure.\n");
		return -ENOMEM;
	}
	memset((char *)host, 0, sizeof(struct hinfc_host));

	host->iobase = (void __iomem *)CONFIG_HINFC301_REG_BASE_ADDRESS;
	chip->IO_ADDR_R = (void *)CONFIG_HINFC301_BUFFER_BASE_ADDRESS;
	chip->IO_ADDR_W = (void *)CONFIG_HINFC301_BUFFER_BASE_ADDRESS;
	hinfc301_controller_enable(host, 1);

	if (hinfc301_nand_init(host, chip)) {
		printk(KERN_INFO "failed to allocate device buffer.\n");
		return -ENOMEM;
	}

	nand_spl_ids_register();

	return 0;
}
/*****************************************************************************/

int nand_get_ecctype(void)
{
	if (!host) {
		printf("nand flash uninitialized.\n");
		return -1;
	}
	return host->ecctype;
}
