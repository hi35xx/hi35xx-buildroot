/******************************************************************************
 *    COPYRIGHT (C) 2014 Hisilicon
 *    All rights reserved.
 * ***
 *    Create by hisilicon
 *
******************************************************************************/

#include "hisnfc100_os.h"
#include "hisnfc100_spi_ids.h"
#include "hisnfc100.h"

/*****************************************************************************/
#define MAX_MTD_PARTITIONS         (32)

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

static struct partition_info ptn_info;

/*****************************************************************************/
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
static int hinfc_os_add_paratitions(struct hisnfc_host *host)
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
		DBG_MSG("partitions[%d] = {.name = %s, .offset = 0x%.8x," \
			" .size = 0x%08x (%uKiB) }\n",
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
static int hisnfc100_os_probe(struct platform_device *pltdev)
{
	int size, result = 0;
	struct hisnfc_host *host;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct resource *res1, *res2 = NULL;

	size = sizeof(struct hisnfc_host) + sizeof(struct nand_chip)
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

	res1 = platform_get_resource_byname(pltdev, IORESOURCE_MEM, "base");
	if (!res1) {
		PR_BUG("Can't get resource for reg address.\n");
		result = -EIO;
		goto fail;
	}

	host->regbase = ioremap_nocache(res1->start,
					res1->end - res1->start + 1);
	if (!host->regbase) {
		PR_BUG("io remap reg address failed\n");
		result = -EFAULT;
		goto fail;
	}

	res2 = platform_get_resource_byname(pltdev, IORESOURCE_MEM, "buffer");
	if (!res2) {
		PR_BUG("Can't get resource for buffer address.\n");
		result = -EIO;
		goto fail;
	}

	host->iobase = ioremap_nocache(res2->start,
					res2->end - res2->start + 1);
	if (!host->iobase) {
		PR_BUG("io remap buffer address failed\n");
		result = -EFAULT;
		goto fail;
	}
	memset((char *)host->iobase, 0xff, HISNFC100_BUFFER_BASE_ADDRESS_LEN);
	chip->IO_ADDR_R = chip->IO_ADDR_W = host->iobase;

	host->buffer = dma_alloc_coherent(host->dev, HISNFC100_BUFFER_LEN,
						&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer) {
		PR_BUG("Can't malloc memory for SPI Nand driver.");
		result = -ENOMEM;
		goto fail;
	}
	memset(host->buffer, 0xff, HISNFC100_BUFFER_LEN);

	mtd->priv  = chip;
	mtd->owner = THIS_MODULE;
	mtd->name  = (char *)(pltdev->name);

	result = hisnfc100_host_init(host);
	if (result)
		return result;

	if (host->set_system_clock)
		host->set_system_clock(NULL, ENABLE);

	chip->priv = host;
	hisnfc100_nand_init(chip);

	spi_nand_ids_register();
	nand_oob_resize = hisnfc100_ecc_probe;

	if (nand_scan(mtd, CONFIG_HISNFC100_MAX_CHIP)) {
		result = -ENXIO;
		goto fail;
	}

	result = hinfc_os_add_paratitions(host);
	if (host->add_partition)
		return result;

	if (!add_mtd_device(host->mtd))
		return 0;

	result = -ENODEV;
	nand_release(mtd);

fail:
	if (host->buffer) {
		dma_free_coherent(host->dev, HISNFC100_BUFFER_LEN,
					host->buffer, host->dma_buffer);
		host->buffer = NULL;
	}

	if (host->iobase)
		iounmap(host->iobase);
	if (res2)
		release_resource(res2);
	if (host->regbase)
		iounmap(host->regbase);
	if (res1)
		release_resource(res1);
	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return result;
}

/*****************************************************************************/
static int hisnfc100_os_remove(struct platform_device *pltdev)
{
	struct hisnfc_host *host = platform_get_drvdata(pltdev);

	host->set_system_clock(NULL, DISABLE);

	nand_release(host->mtd);
	dma_free_coherent(host->dev,
				(SPI_NAND_MAX_PAGESIZE + SPI_NAND_MAX_OOBSIZE),
				host->buffer, host->dma_buffer);
	iounmap(host->regbase);
	iounmap(host->iobase);
	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return 0;
}

/*****************************************************************************/
static void hisnfc100_pltdev_release(struct device *dev)
{
}

#ifdef CONFIG_PM
/*****************************************************************************/
static int hisnfc100_os_suspend(struct platform_device *pltdev,
			       pm_message_t state)
{
	return 0;
}

/*****************************************************************************/
static int hisnfc100_os_resume(struct platform_device *pltdev)
{
	return 0;
}
#endif /* CONFIG_PM */

/*****************************************************************************/
static struct platform_driver hisnfc100_os_pltdrv = {
	.driver.name   = "hinand",
	.probe  = hisnfc100_os_probe,
	.remove = hisnfc100_os_remove,
#ifdef CONFIG_PM
	.suspend  = hisnfc100_os_suspend,
	.resume   = hisnfc100_os_resume,
#endif /* CONFIG_PM */
};

/*****************************************************************************/
static struct resource hisnfc100_resources[] = {
	{
		.start  = CONFIG_HISNFC100_REG_BASE_ADDRESS,
		.end    = CONFIG_HISNFC100_REG_BASE_ADDRESS
			+ HISNFC100_REG_BASE_ADDRESS_LEN,
		.flags  = IORESOURCE_MEM,
		.name	= "base"
	},

	{
		.start  = CONFIG_HISNFC100_BUFFER_BASE_ADDRESS,
		.end    = CONFIG_HISNFC100_BUFFER_BASE_ADDRESS
			+ HISNFC100_BUFFER_BASE_ADDRESS_LEN,
		.flags  = IORESOURCE_MEM,
		.name	= "buffer"
	},
};

static u64 hinand_dmamask = DMA_BIT_MASK(32);

static struct platform_device hisnfc100_os_pltdev = {
	.name           = "hinand",
	.id             = -1,

	.dev.platform_data     = NULL,
	.dev.dma_mask          = &hinand_dmamask,
	.dev.coherent_dma_mask = DMA_BIT_MASK(32),
	.dev.release           = hisnfc100_pltdev_release,

	.num_resources  = ARRAY_SIZE(hisnfc100_resources),
	.resource       = hisnfc100_resources,
};

/*****************************************************************************/
static int __init hisnfc100_os_module_init(void)
{
	int result = 0;

	pr_notice("Found SPI Nand Flash Controller v100.\n");

	result = platform_driver_register(&hisnfc100_os_pltdrv);
	if (result < 0)
		return result;

	result = platform_device_register(&hisnfc100_os_pltdev);
	if (result < 0) {
		platform_driver_unregister(&hisnfc100_os_pltdrv);
		return result;
	}

	return result;
}

/*****************************************************************************/
static void __exit hisnfc100_os_module_exit(void)
{
	platform_driver_unregister(&hisnfc100_os_pltdrv);
	platform_device_unregister(&hisnfc100_os_pltdev);
}

/*****************************************************************************/
module_init(hisnfc100_os_module_init);
module_exit(hisnfc100_os_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BVT_BSP");
MODULE_DESCRIPTION("Hisnfc Device Driver, Version 100");

