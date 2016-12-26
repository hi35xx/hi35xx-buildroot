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
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <asm/setup.h>

#include "../../mtdcore.h"
#include "../../hifmc_common.h"
#include "hifmc100.h"
#include "hifmc100_os.h"

/*****************************************************************************/
#define MAX_MTD_PARTITIONS	(32)

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

static struct partition_info ptn_info = {0};

/*****************************************************************************/
static int __init parse_nand_partitions(const struct tag *tag)
{
	int i, len;

	if (tag->hdr.size <= 2) {
		pr_info("tag->hdr.size <= 2\n");
		return 0;
	}

	len = sizeof(struct partition_entry) / sizeof(int);
	ptn_info.parts_num = (tag->hdr.size - 2) / len;

	len = ptn_info.parts_num * sizeof(struct partition_entry);
	memcpy(ptn_info.entry, &tag->u, len);

	for (i = 0; i < ptn_info.parts_num; i++) {
		ptn_info.parts[i].name = ptn_info.entry[i].name;
		ptn_info.parts[i].size = (ptn_info.entry[i].length);
		ptn_info.parts[i].offset = (ptn_info.entry[i].start);
		ptn_info.parts[i].mask_flags = 0;
		ptn_info.parts[i].ecclayout = 0;
	}

	return 0;
}

/* turn to ascii is "HiNp" */
__tagtable(0x48694E70, parse_nand_partitions);

/*****************************************************************************/
static int hifmc100_spi_nand_pre_probe(struct nand_chip *chip)
{
	uint8_t nand_maf_id;
	struct hifmc_host *host = chip->priv;

	/* Reset the chip first */
	host->send_cmd_reset(host);
	udelay(1000);

	/* Check the ID */
	host->offset = 0;
	memset((unsigned char *)(chip->IO_ADDR_R), 0, 0x10);
	host->send_cmd_readid(host);
	nand_maf_id = readb(chip->IO_ADDR_R);

	if (nand_maf_id == 0x00 || nand_maf_id == 0xff) {
		printk("Cannot found a valid SPI Nand Device\n");
		return 1;
	}

	return 0;
}

/*****************************************************************************/
static int hifmc_nand_scan(struct mtd_info *mtd)
{
	int result = 0;
	unsigned char cs, chip_num = CONFIG_SPI_NAND_MAX_CHIP_NUM;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	for (cs = 0; chip_num && (cs < CONFIG_HIFMC_MAX_CS_NUM); cs++) {
		if (hifmc_cs_user[cs]) {
			result = -ENODEV;
			FMC_PR(BT_DBG, "\t\t*-Current CS(%d) is occupied.\n",
					cs);
			continue;
		}

		host->cmd_op.cs = cs;

		if (hifmc100_spi_nand_pre_probe(chip)) {
			result = -ENODEV;
			goto fail;
		}

		result = nand_scan(mtd, chip_num);
		if (result) {
			result = -ENXIO;
			goto fail;
		}
		chip_num--;
	}

fail:
	return result;
}

/*****************************************************************************/
static int hifmc_os_add_paratitions(struct hifmc_host *host)
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

	if (BT_DBG)
		for (ix = 0; ix < nr_parts; ix++) {
			DB_MSG("partitions[%d] = {.name = %s, ",
					ix, parts[ix].name);
			DB_MSG(".offset = 0x%.8x, .size = 0x%08x (%uMB) }\n",
				(unsigned int)parts[ix].offset,
				(unsigned int)parts[ix].size,
				(unsigned int)parts[ix].size / (1024 * 1024));
		}

	host->add_partition = 1;

	ret = mtd_device_register(host->mtd, parts, nr_parts);

	kfree(parts);
	parts = NULL;

	return (1 == ret) ? -ENODEV : 0;
}

/*****************************************************************************/
static int hifmc100_os_probe(struct platform_device *pltdev)
{
	int len, result = 0;
	struct hifmc_host *host;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct resource *rs_reg, *rs_io = NULL;

	FMC_PR(BT_DBG, "\t*-Start SPI Nand flash driver probe\n");

	/* Hifmc ip version check */
	result = hifmc_ip_ver_check();
	if (result) {
		FMC_PR(BT_DBG, "\t*-IP version unknown, result: %d\n", result);
		return result;
	}

	FMC_PR(BT_DBG, "\t|-Allocate SPI nand driver host space\n");
	len = sizeof(struct hifmc_host) + sizeof(struct nand_chip)
		+ sizeof(struct mtd_info);
	host = kmalloc(len, GFP_KERNEL);
	if (!host) {
		DB_MSG("Error: Failed to allocate device structure.\n");
		return -ENOMEM;
	}
	memset((char *)host, 0, len);

	FMC_PR(BT_DBG, "\t|-Save SPI nand driver private data: host\n");
	platform_set_drvdata(pltdev, host);

	host->dev = &pltdev->dev;
	host->chip = chip = (struct nand_chip *)&host[1];
	host->mtd = mtd = (struct mtd_info *)&chip[1];

	FMC_PR(BT_DBG, "\t|-Get SPI nand driver resource for regbase\n");
	rs_reg = platform_get_resource_byname(pltdev, IORESOURCE_MEM, "base");
	if (!rs_reg) {
		DB_MSG("Error: Can't get resource for reg address.\n");
		result = -EIO;
		goto fail;
	}

	len = rs_reg->end - rs_reg->start + 1;
	host->regbase = ioremap_nocache(rs_reg->start, len);
	if (!host->regbase) {
		DB_MSG("Error: SPI nand reg base-address ioremap failed.\n");
		result = -EFAULT;
		goto fail;
	}

	FMC_PR(BT_DBG, "\t|-Get SPI nand driver resource for iobase\n");
	rs_io = platform_get_resource_byname(pltdev, IORESOURCE_MEM, "buffer");
	if (!rs_io) {
		DB_MSG("Error: Can't get resource for buffer address.\n");
		result = -EIO;
		goto fail;
	}

	len = rs_io->end - rs_io->start + 1;
	host->iobase = ioremap_nocache(rs_io->start, len);
	if (!host->iobase) {
		DB_MSG("Error: SPI nand buffer base-address ioremap failed.\n");
		result = -EFAULT;
		goto fail;
	}
	memset((char *)host->iobase, 0xff, SPI_NAND_BUFFER_LEN);
	chip->IO_ADDR_R = chip->IO_ADDR_W = host->iobase;

	FMC_PR(BT_DBG, "\t|-Allocate SPI nand DMA buffer space\n");
	host->buffer = dma_alloc_coherent(host->dev, SPI_NAND_BUFFER_LEN,
			&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer) {
		DB_MSG("Error: Can't allocate memory for dma buffer.");
		result = -EIO;
		goto fail;
	}
	memset(host->buffer, 0xff, SPI_NAND_BUFFER_LEN);

	FMC_PR(BT_DBG, "\t|-Initialize MTD and SPI nand driver\n");
	mtd->type = MTD_NANDFLASH;
	mtd->name = (char *)(pltdev->name);
	mtd->priv = chip;
	mtd->owner = THIS_MODULE;

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	FMC_PR(BT_DBG, "\t|-Switch device type to SPI nand\n");
	mtd_switch_spi_type(mtd);
#endif

	chip->priv = host;
	result = hifmc100_spi_nand_init(chip);
	if (result) {
		FMC_PR(BT_DBG, "\t|-SPI Nand init failed, ret: %d\n", result);
		result = -ENODEV;
		goto fail;
	}

	FMC_PR(BT_DBG, "\t|-Scan SPI Nand chip and ECC probe\n");
	result = hifmc_nand_scan(mtd);
	if (result) {
		FMC_PR(BT_DBG, "\t|-Scan SPI Nand failed.\n");
		goto fail;
	}

	FMC_PR(BT_DBG, "\t|-Register MTD partdev\n");
	result = hifmc_os_add_paratitions(host);
	if (host->add_partition)
		goto end;

	FMC_PR(BT_DBG, "\t|-Add mtd device\n");
	if (!add_mtd_device(host->mtd)) {
		result = 0;
		goto end;
	}

	DB_MSG("Error: MTD partition register failed! result: %d\n", result);
	result = -ENODEV;
fail:
	if (host->buffer) {
		dma_free_coherent(host->dev, SPI_NAND_BUFFER_LEN, host->buffer,
				host->dma_buffer);
		host->buffer = NULL;
	}

	if (host->iobase)
		iounmap(host->iobase);
	if (rs_io)
		release_resource(rs_io);
	if (host->regbase)
		iounmap(host->regbase);
	if (rs_reg)
		release_resource(rs_reg);

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	FMC_PR(BT_DBG, "\t|-Switch device type to default.\n");
	mtd->type = MTD_ABSENT;
	mtd_switch_spi_type(mtd);
	mtd->type = MTD_NANDFLASH;
#endif
	nand_release(host->mtd);
	kfree(host);
	platform_set_drvdata(pltdev, NULL);
	return result;

end:
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	FMC_PR(BT_DBG, "\t|-Switch device type to default.\n");
	mtd->type = MTD_ABSENT;
	mtd_switch_spi_type(mtd);
	mtd->type = MTD_NANDFLASH;
#endif
	FMC_PR(BT_DBG, "\t*-End driver probe, result: %d\n", result);

	return result;
}

/*****************************************************************************/
static int hifmc100_os_remove(struct platform_device *pltdev)
{
	struct hifmc_host *host = platform_get_drvdata(pltdev);

	host->set_system_clock(NULL, DISABLE);

	nand_release(host->mtd);

	if (host->buffer)
		dma_free_coherent(host->dev, SPI_NAND_BUFFER_LEN,
			host->buffer, host->dma_buffer);

	if (host->iobase)
		iounmap(host->iobase);
	if (host->regbase)
		iounmap(host->regbase);

	platform_set_drvdata(pltdev, NULL);
	kfree(host);

	return 0;
}

#ifdef CONFIG_PM
/*****************************************************************************/
static int hifmc100_os_suspend(struct platform_device *pltdev,
		pm_message_t state)
{
	struct hifmc_host *host = platform_get_drvdata(pltdev);

	if (host && host->suspend)
		return (host->suspend)(pltdev, state);

	return 0;
}

/*****************************************************************************/
static int hifmc100_os_resume(struct platform_device *pltdev)
{
	struct hifmc_host *host = platform_get_drvdata(pltdev);

	if (host && host->resume)
		return (host->resume)(pltdev);

	return 0;
}
#endif /* End of CONFIG_PM */

/*****************************************************************************/
static void hifmc100_pltdev_release(struct device *dev)
{
}

/*****************************************************************************/
static struct resource hifmc100_resources[] = {
	[0] = {
		.start	= CONFIG_HIFMC_REG_BASE,
		.end	= CONFIG_HIFMC_REG_BASE + HIFMC_REG_ADDRESS_LEN - 1,
		.flags	= IORESOURCE_MEM,
		.name	= "base"
	},
	[1] = {
		.start	= CONFIG_HIFMC_BUFFER_BASE,
		.end	= CONFIG_HIFMC_BUFFER_BASE + SPI_NAND_BUFFER_LEN - 1,
		.flags	= IORESOURCE_MEM,
		.name	= "buffer"
	},
};

/*****************************************************************************/
static struct platform_driver hifmc100_os_pltdrv = {
	.driver.name	= "hinand",
	.probe		= hifmc100_os_probe,
	.remove		= hifmc100_os_remove,
#ifdef CONFIG_PM
	.suspend	= hifmc100_os_suspend,
	.resume		= hifmc100_os_resume,
#endif
};

/*****************************************************************************/
static u64 hinand_dmamask = DMA_BIT_MASK(32);

/*****************************************************************************/
static struct platform_device hifmc100_os_pltdev = {
	.name			= "hinand",
	.id			= -1,

	.dev.platform_data	= NULL,
	.dev.dma_mask		= &hinand_dmamask,
	.dev.coherent_dma_mask	= DMA_BIT_MASK(32),
	.dev.release		= hifmc100_pltdev_release,

	.num_resources  = ARRAY_SIZE(hifmc100_resources),
	.resource       = hifmc100_resources,
};

/*****************************************************************************/
static int __init hifmc100_os_module_init(void)
{
	int result = 0;

	result = platform_driver_register(&hifmc100_os_pltdrv);
	if (result < 0)
		return result;

	result = platform_device_register(&hifmc100_os_pltdev);
	if (result < 0) {
		platform_driver_unregister(&hifmc100_os_pltdrv);
		return result;
	}

	return result;
}

/*****************************************************************************/
static void __exit hifmc100_os_module_exit(void)
{
	platform_device_unregister(&hifmc100_os_pltdev);
	platform_driver_unregister(&hifmc100_os_pltdrv);
}

/*****************************************************************************/
module_init(hifmc100_os_module_init);
module_exit(hifmc100_os_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BVT_BSP");
MODULE_DESCRIPTION("Hisilicon Flash Memory Controller V100 SPI Nand Driver");

