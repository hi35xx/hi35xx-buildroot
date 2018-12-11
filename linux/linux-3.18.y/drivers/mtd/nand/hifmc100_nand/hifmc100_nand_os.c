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

#include <linux/of_platform.h>

#include "hifmc100_nand_os.h"
#include "hifmc100_nand.h"
#include <linux/mfd/hisi_fmc.h>

/*****************************************************************************/
static inline int mtd_has_partitions(void) { return 1; }

/*****************************************************************************/
static int hisi_nand_os_probe(struct platform_device *pltdev)
{
	int len, result = 0;
	struct hifmc_host *host;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	int nr_parts = 0;
	struct mtd_partition *parts = NULL;
	struct device *dev = &pltdev->dev;
	struct device_node *np = NULL;
	struct hisi_fmc *fmc = dev_get_drvdata(dev->parent);

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
	chip->IO_ADDR_R = chip->IO_ADDR_W = host->iobase;

	/* hifmc Nand host init */
	chip->priv = host;
	result = hifmc100_nand_init(chip);
	if (result) {
		DB_MSG("Error: host init failed! result: %d\n", result);
		goto fail;
	}

	np = of_get_next_available_child(dev->of_node, NULL);
	mtd->name = np->name;
	mtd->type = MTD_NANDFLASH;
	mtd->priv = chip;
	mtd->flags = MTD_CAP_NANDFLASH;
	mtd->owner = THIS_MODULE;

	if (nand_scan(mtd, CONFIG_HIFMC100_MAX_NAND_CHIP)) {
		result = -ENXIO;
		goto fail;
	}

	if (mtd_has_partitions()) {
		static char const *part_probes[] = {
			"cmdlinepart",
			NULL,
		};

		nr_parts = parse_mtd_partitions(host->mtd,
				part_probes, &parts, 0);
		FMC_PR(BT_DBG, "parse mtd partitions: %d\n", nr_parts);
		if (nr_parts > 0)
			host->add_partition = 1;
	}

	result = mtd_device_register(host->mtd, parts, nr_parts);
	if (result) {
		kfree(parts);
		parts = NULL;
	}

	return (1 == result) ? -ENODEV : 0;

fail:
	nand_release(mtd);
	clk_disable_unprepare(host->clk);
	return result;
}

/*****************************************************************************/
static int hisi_nand_os_remove(struct platform_device *pltdev)
{
	struct hifmc_host *host = platform_get_drvdata(pltdev);

	clk_disable_unprepare(host->clk);
	nand_release(host->mtd);

	return 0;
}

#ifdef CONFIG_PM
/*****************************************************************************/
static int hifmc100_nand_os_suspend(struct platform_device *pltdev,
		pm_message_t state)
{
	struct hifmc_host *host = platform_get_drvdata(pltdev);
	if (!host)
		return 0;

	while ((hifmc_readl(host, FMC_OP) & FMC_OP_REG_OP_START))
		_cond_resched();

	while ((hifmc_readl(host, FMC_OP_CTRL) & OP_CTRL_DMA_OP_READY))
		_cond_resched();

	clk_disable_unprepare(host->clk);
	FMC_PR(PM_DBG, "\t|-disable system clock\n");
	return 0;
}

/*****************************************************************************/
static int hifmc100_nand_os_resume(struct platform_device *pltdev)
{
	int cs;
	struct hifmc_host *host = platform_get_drvdata(pltdev);
	struct nand_chip *chip;

	if (!host)
		return 0;

	chip = host->chip;

	for (cs = 0; cs < chip->numchips; cs++)
		host->send_cmd_reset(host);

	hifmc100_nand_config(host);
	return 0;
}
#endif /* CONFIG_PM */

/*****************************************************************************/
static const struct of_device_id hisi_nand_dt_ids[] = {
	{ .compatible = "hisilicon,hisi-nand" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hisi_nand_dt_ids);

static struct platform_driver hisi_nand_driver = {
	.driver = {
		.name	= "hisi-nand",
		.of_match_table = hisi_nand_dt_ids,
	},
	.probe	= hisi_nand_os_probe,
	.remove = hisi_nand_os_remove,
#ifdef CONFIG_PM
	.suspend	= hifmc100_nand_os_suspend,
	.resume		= hifmc100_nand_os_resume,
#endif
};
module_platform_driver(hisi_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BVT_BSP");
MODULE_DESCRIPTION("Hisilicon Flash Memory Controller V100 Nand Driver");
