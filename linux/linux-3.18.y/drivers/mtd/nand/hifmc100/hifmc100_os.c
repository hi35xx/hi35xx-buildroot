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

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/of_platform.h>
#include <linux/mfd/hisi_fmc.h>

#include <asm/setup.h>

#include "../../mtdcore.h"
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
	nand_maf_id = hifmc_readb(chip->IO_ADDR_R);

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

	for (cs = 0; chip_num && (cs < HIFMC_MAX_CHIP_NUM); cs++) {
		if (hifmc_cs_user[cs]) {
			FMC_PR(BT_DBG, "\t\t*-Current CS(%d) is occupied.\n",
					cs);
			continue;
		}

		host->cmd_op.cs = cs;

		if (hifmc100_spi_nand_pre_probe(chip))
			return -ENODEV;

		FMC_PR(BT_DBG, "\t\t*-Scan SPI nand flash on CS: %d\n", cs);
		if (nand_scan(mtd, chip_num))
			continue;
		chip_num--;
	}

	if (chip_num == CONFIG_SPI_NAND_MAX_CHIP_NUM)
		result = -ENXIO;
	else
		result = 0;

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

	/* (1) if nr_parts > 0, Using commandline partition definition
	 * (2) if nr_parts = 0, Using board partition definition, or parse
	 * the parameter from __tag.
	 */
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

	return (1 == ret) ? -ENODEV : 0;
}

/*****************************************************************************/
static int hisi_spi_nand_probe(struct platform_device *pltdev)
{
	int len, result = 0;
	struct hifmc_host *host;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct device *dev = &pltdev->dev;
	struct device_node *np = NULL;
	struct hisi_fmc *fmc = dev_get_drvdata(dev->parent);

	FMC_PR(BT_DBG, "\t*-Start SPI Nand flash driver probe\n");

	len = sizeof(struct hifmc_host) + sizeof(struct nand_chip)
		+ sizeof(struct mtd_info);
	host = devm_kzalloc(dev, len, GFP_KERNEL);
	if (!host)
		return -ENOMEM;
	memset((char *)host, 0, len);

	platform_set_drvdata(pltdev, host);
	host->dev = &pltdev->dev;

	host->chip = chip = (struct nand_chip *)&host[1];
	host->mtd = mtd = (struct mtd_info *)&chip[1];

	host->regbase = fmc->regbase;
	host->iobase = fmc->iobase;
	host->clk = fmc->clk;
	host->lock = &fmc->lock;

	memset((char *)host->iobase, 0xff, SPI_NAND_BUFFER_LEN);
	chip->IO_ADDR_R = chip->IO_ADDR_W = host->iobase;

	host->buffer = dmam_alloc_coherent(host->dev, SPI_NAND_BUFFER_LEN,
			&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer) {
		DB_MSG("Error: Can't allocate memory for dma buffer.");
		result = -EIO;
		goto fail;
	}
	memset(host->buffer, 0xff, SPI_NAND_BUFFER_LEN);

	chip->priv = host;
	result = hifmc100_spi_nand_init(chip);
	if (result) {
		FMC_PR(BT_DBG, "\t|-SPI Nand init failed, ret: %d\n", result);
		result = -ENODEV;
		goto fail;
	}

	np = of_get_next_available_child(dev->of_node, NULL);
	mtd->name = np->name;
	mtd->type = MTD_NANDFLASH;
	mtd->priv = chip;
	mtd->owner = THIS_MODULE;

	result = of_property_read_u32(np, "spi-max-frequency", &host->clkrate);
	if (result)
		goto fail;

	result = hifmc_nand_scan(mtd);
	if (result) {
		FMC_PR(BT_DBG, "\t|-Scan SPI Nand failed.\n");
		goto fail;
	}

	result = hifmc_os_add_paratitions(host);
	if (host->add_partition)
		goto end;

	if (!add_mtd_device(host->mtd)) {
		result = 0;
		goto end;
	}

	result = -ENODEV;
	DB_MSG("Error: MTD partition register failed! result: %d\n",
			result);
fail:
	nand_release(mtd);
	clk_disable_unprepare(host->clk);
end:
	FMC_PR(BT_DBG, "\t*-End driver probe, result: %d\n", result);
	return result;
}

/*****************************************************************************/
static int hisi_spi_nand_remove(struct platform_device *pltdev)
{
	struct hifmc_host *host = platform_get_drvdata(pltdev);

	clk_disable_unprepare(host->clk);
	nand_release(host->mtd);

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
static const struct of_device_id hisi_spi_nand_dt_ids[] = {
	{ .compatible = "hisilicon,hisi-spi-nand"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hisi_spi_nand_dt_ids);

static struct platform_driver hisi_spi_nand_driver = {
	.driver = {
		.name	= "hisi_spi_nand",
		.of_match_table = hisi_spi_nand_dt_ids,
	},
	.probe	= hisi_spi_nand_probe,
	.remove = hisi_spi_nand_remove,
#ifdef CONFIG_PM
	.suspend	= hifmc100_os_suspend,
	.resume		= hifmc100_os_resume,
#endif
};
module_platform_driver(hisi_spi_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BVT_BSP");
MODULE_DESCRIPTION("Hisilicon Flash Memory Controller V100 SPI Nand Driver");
