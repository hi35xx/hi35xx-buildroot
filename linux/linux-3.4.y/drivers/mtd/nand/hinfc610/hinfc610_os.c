/******************************************************************************
 *    COPYRIGHT (C) 2013 Czyong. Hisilicon
 *    All rights reserved.
 * ***
 *    Create by Czyong 2013-02-07
 *
******************************************************************************/

#include "hinfc610_os.h"
#include "hinfc610.h"

/*****************************************************************************/
#define MAX_MTD_PARTITIONS         (32)

void hinfc610_controller_enable(struct hinfc_host *host, int enable);

struct partition_entry {
	char name[16];
	unsigned long long start;
	unsigned long long length;
	unsigned int flags;
};

struct partition_info {
	int parts_num;
	struct partition_entry entry[MAX_MTD_PARTITIONS];
	struct mtd_partition parts[MAX_MTD_PARTITIONS];
};

#ifdef CONFIG_MTD_PART_CHANGE
extern int register_mtd_partdev(struct mtd_info *mtd);
extern int unregister_mtd_partdev(struct mtd_info *mtd);
#else
int register_mtd_partdev(struct mtd_info *mtd)
{
	return 0;
};
int unregister_mtd_partdev(struct mtd_info *mtd)
{
	return 0;
};
#endif

static struct partition_info ptn_info = {0};

static int __init parse_nand_partitions(const struct tag *tag)
{
	int i;

	if (tag->hdr.size <= 2) {
		PR_BUG("tag->hdr.size <= 2\n");
		return 0;
	}
	ptn_info.parts_num = (tag->hdr.size - 2)
		/ (sizeof(struct partition_entry)/sizeof(int));
	memcpy(ptn_info.entry,
		&tag->u,
		ptn_info.parts_num * sizeof(struct partition_entry));

	for (i = 0; i < ptn_info.parts_num; i++) {
		ptn_info.parts[i].name   = ptn_info.entry[i].name;
		ptn_info.parts[i].size   = (ptn_info.entry[i].length);
		ptn_info.parts[i].offset = (ptn_info.entry[i].start);
		ptn_info.parts[i].mask_flags = 0;
		ptn_info.parts[i].ecclayout  = 0;
	}

	return 0;
}

/* turn to ascii is "HiNp" */
__tagtable(0x48694E70, parse_nand_partitions);
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
		PR_BUG("tag->hdr.size <= 2\n");
		return 0;
	}
	memcpy(nand_otp, &tag->u, nand_otp_len);
	return 0;
}
/* 0x48694E77 equal to fastoot ATAG_NAND_PARAM */
__tagtable(0x48694E77, parse_nand_param);
/*****************************************************************************/

static int hinfc_os_add_paratitions(struct hinfc_host *host)
{
	int ix;
	int nr_parts = 0;
	struct mtd_partition *parts = NULL;
	int ret;

#ifdef CONFIG_MTD_CMDLINE_PARTS
	static const char *part_probes[] = {"cmdlinepart", NULL, };
	nr_parts = parse_mtd_partitions(host->mtd, part_probes, &parts, 0);
#endif

	if (!nr_parts) {
		nr_parts = ptn_info.parts_num;
		parts    = ptn_info.parts;
	}

	if (nr_parts <= 0)
		return 0;

	for (ix = 0; ix < nr_parts; ix++) {
		DBG_MSG("partitions[%d] = {.name = %s, .offset = 0x%.8x, " \
			".size = 0x%08x (%uKiB) }\n",
			ix, parts[ix].name,
			(unsigned int)parts[ix].offset,
			(unsigned int)parts[ix].size,
			(unsigned int)parts[ix].size/1024);
	}

	host->add_partition = 1;

	ret = mtd_device_register(host->mtd, parts, nr_parts);

	kfree(parts);
	parts = NULL;

	return (1 == ret) ? -ENODEV : 0;
}
/*****************************************************************************/

static int hinfc610_nand_pre_probe(struct nand_chip *chip)
{
	uint8_t nand_maf_id;
	struct hinfc_host *host = chip->priv;

	/* Reset the chip first */
	host->send_cmd_reset(host, 0);

	/* Check the ID */
	host->offset = 0;
	memset((unsigned char *)(chip->IO_ADDR_R), 0, 0x10);
	host->send_cmd_readid(host);
	nand_maf_id = readb(chip->IO_ADDR_R);

	if (nand_maf_id == 0x00 || nand_maf_id == 0xff) {
		printk("\nCannot found a valid Nand Device\n");
		return 1;
	}

	return 0;
}
/*****************************************************************************/

static int hinfc610_os_probe(struct platform_device *pltdev)
{
	int size;
	int result = 0;
	struct hinfc_host *host;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct resource *rs_reg, *rs_io = NULL;

	size = sizeof(struct hinfc_host) + sizeof(struct nand_chip)
		+ sizeof(struct mtd_info);
	host = kmalloc(size, GFP_KERNEL);
	if (!host) {
		PR_BUG("failed to allocate device structure.\n");
		return -ENOMEM;
	}
	memset((char *)host, 0, size);
	platform_set_drvdata(pltdev, host);

	host->dev  = &pltdev->dev;
	host->chip = chip = (struct nand_chip *)&host[1];
	host->mtd  = mtd  = (struct mtd_info *)&chip[1];

	hinfc610_controller_enable(host, ENABLE);

	rs_reg = platform_get_resource_byname(pltdev, IORESOURCE_MEM, "base");
	if (!rs_reg) {
		PR_BUG("Error: Can't get resource for reg address.\n");
		result = -EIO;
		goto fail;
	}

	host->iobase = ioremap(rs_reg->start, rs_reg->end - rs_reg->start + 1);
	if (!host->iobase) {
		PR_BUG("ioremap failed\n");
		result = -EFAULT;
		goto fail;
	}

	mtd->priv  = chip;
	mtd->owner = THIS_MODULE;
	mtd->name  = (char *)(pltdev->name);

	rs_io = platform_get_resource_byname(pltdev, IORESOURCE_MEM, "buffer");
	if (!rs_io) {
		PR_BUG("Error: Can't get resource for buffer address.\n");
		result = -EIO;
		goto fail;
	}

	chip->IO_ADDR_R = chip->IO_ADDR_W = ioremap_nocache(rs_io->start,
		rs_io->end - rs_io->start + 1);
	if (!chip->IO_ADDR_R) {
		PR_BUG("Error: SPI nand buffer base-address ioremap failed.\n");
		result = -EFAULT;
		goto fail;
	}

	host->buffer = dma_alloc_coherent(host->dev,
		(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE),
		&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer) {
		PR_BUG("Can't malloc memory for NAND driver.");
		result = -EIO;
		goto fail;
	}

	chip->priv        = host;
	host->chip        = chip;
	chip->cmd_ctrl    = hinfc610_cmd_ctrl;
	chip->dev_ready   = hinfc610_dev_ready;
	chip->select_chip = hinfc610_select_chip;
	chip->read_byte   = hinfc610_read_byte;
	chip->read_word   = hinfc610_read_word;
	chip->write_buf   = hinfc610_write_buf;
	chip->read_buf    = hinfc610_read_buf;

	chip->chip_delay = HINFC610_CHIP_DELAY;
	chip->options = NAND_NO_AUTOINCR | NAND_SKIP_BBTSCAN | NAND_BROKEN_XD;
	chip->ecc.layout = NULL;
	chip->ecc.mode   = NAND_ECC_NONE;

	if (hinfc610_nand_init(host, chip)) {
		PR_BUG("failed to allocate device buffer.\n");
		result = -EIO;
		goto fail;
	}

	if (hinfc610_nand_pre_probe(chip)) {
		result = -EXDEV;
		goto fail;
	}

	if (nand_otp_len) {
		PR_MSG("Copy Nand read retry parameter from boot," \
		       " parameter length %d.\n", nand_otp_len);
		memcpy(host->rr_data, nand_otp, nand_otp_len);
	}

	if (nand_scan(mtd, CONFIG_HINFC610_MAX_CHIP)) {
		result = -ENXIO;
		goto fail;
	}

	result = hinfc_os_add_paratitions(host);
	if (host->add_partition)
		return result;

	if (!add_mtd_device(host->mtd))
		return 0;

	result = -ENODEV;
fail:
	if (host->buffer) {
		dma_free_coherent(host->dev,
			(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE),
			host->buffer,
			host->dma_buffer);
		host->buffer = NULL;
	}
	if (chip->IO_ADDR_W)
		iounmap(chip->IO_ADDR_W);
	if (rs_io)
		release_resource(rs_io);
	if (host->iobase)
		iounmap(host->iobase);
	if (rs_reg)
		release_resource(rs_reg);
	nand_release(host->mtd);
	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return result;
}
/*****************************************************************************/

static int hinfc610_os_remove(struct platform_device *pltdev)
{
	struct hinfc_host *host = platform_get_drvdata(pltdev);

	hinfc610_controller_enable(host, DISABLE);

	unregister_mtd_partdev(host->mtd);

	nand_release(host->mtd);

	dma_free_coherent(host->dev,
		(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE),
		host->buffer,
		host->dma_buffer);
	iounmap(host->chip->IO_ADDR_W);
	iounmap(host->iobase);
	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return 0;
}
/*****************************************************************************/

static void hinfc610_pltdev_release(struct device *dev)
{
}
/*****************************************************************************/
#ifdef CONFIG_PM
static int hinfc610_os_suspend(struct platform_device *pltdev,
			       pm_message_t state)
{
	struct hinfc_host *host = platform_get_drvdata(pltdev);

	while ((hinfc_read(host, HINFC610_STATUS) & 0x1) == 0x0)
		;

	while ((hinfc_read(host, HINFC610_DMA_CTRL))
		& HINFC610_DMA_CTRL_DMA_START)
		_cond_resched();

	hinfc610_controller_enable(host, DISABLE);

	return 0;
}
/*****************************************************************************/

static int hinfc610_os_resume(struct platform_device *pltdev)
{
	int cs;
	struct hinfc_host *host = platform_get_drvdata(pltdev);
	struct nand_chip *chip = host->chip;

	hinfc610_controller_enable(host, ENABLE);
	for (cs = 0; cs < chip->numchips; cs++)
		host->send_cmd_reset(host, cs);
	hinfc_write(host,
		SET_HINFC610_PWIDTH(CONFIG_HINFC610_W_LATCH,
			CONFIG_HINFC610_R_LATCH, CONFIG_HINFC610_RW_LATCH),
		HINFC610_PWIDTH);

	return 0;
}
#endif /* CONFIG_PM */
/*****************************************************************************/
static struct platform_driver hinfc610_os_pltdrv = {
	.driver.name   = "hinand",
	.probe  = hinfc610_os_probe,
	.remove = hinfc610_os_remove,
#ifdef CONFIG_PM
	.suspend  = hinfc610_os_suspend,
	.resume   = hinfc610_os_resume,
#endif /* CONFIG_PM */
};
/*****************************************************************************/
static struct resource hinfc610_resources[] = {
	{
		.start  = CONFIG_HINFC610_REG_BASE_ADDRESS,
		.end    = CONFIG_HINFC610_REG_BASE_ADDRESS
			+ HINFC610_REG_BASE_ADDRESS_LEN,
		.flags  = IORESOURCE_MEM,
		.name	= "base"
	},

	{
		.start  = CONFIG_HINFC610_BUFFER_BASE_ADDRESS,
		.end    = CONFIG_HINFC610_BUFFER_BASE_ADDRESS
			+ HINFC610_BUFFER_BASE_ADDRESS_LEN,
		.flags  = IORESOURCE_MEM,
		.name	= "buffer"
	},
	{
		.start = CONFIG_HINFC610_PERIPHERY_REGBASE,
		.end   = CONFIG_HINFC610_PERIPHERY_REGBASE + 100,
		.flags = IORESOURCE_MEM,
		.name	= "peri"
	},
};
static u64 hinand_dmamask = DMA_BIT_MASK(32);

static struct platform_device hinfc610_os_pltdev = {
	.name           = "hinand",
	.id             = -1,

	.dev.platform_data     = NULL,
	.dev.dma_mask          = &hinand_dmamask,
	.dev.coherent_dma_mask = DMA_BIT_MASK(32),
	.dev.release           = hinfc610_pltdev_release,
	.num_resources  = ARRAY_SIZE(hinfc610_resources),
	.resource       = hinfc610_resources,
};
/*****************************************************************************/

static int __init hinfc610_os_module_init(void)
{
	int result = 0;

	if (result)
		return result;

	pr_notice("Found Nand Flash Controller V610.\n");

	result = platform_driver_register(&hinfc610_os_pltdrv);
	if (result < 0)
		return result;

	result = platform_device_register(&hinfc610_os_pltdev);
	if (result < 0) {
		platform_driver_unregister(&hinfc610_os_pltdrv);
		return result;
	}

	return result;
}
/*****************************************************************************/

static void __exit hinfc610_os_module_exit(void)
{
	platform_driver_unregister(&hinfc610_os_pltdrv);
	platform_device_unregister(&hinfc610_os_pltdev);
}
/*****************************************************************************/

module_init(hinfc610_os_module_init);
module_exit(hinfc610_os_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Czyong");
MODULE_DESCRIPTION("Hisilicon Nand Flash Controller V610 Device Driver," \
	" Version 1.30");

/*****************************************************************************/
