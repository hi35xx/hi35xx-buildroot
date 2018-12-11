/* HiSilicon Flash Memory Controller Driver
 *
 * Copyright (c) 2016 Hisilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/mfd/core.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/mfd/hisi_fmc.h>

unsigned char hifmc_cs_user[HIFMC_MAX_CHIP_NUM];

/* ------------------------------------------------------------------------ */
static const struct mfd_cell hisi_fmc_devs[] = {
	{
		.name = "hisi_spi_nor",
		.of_compatible = "hisilicon,hisi-sfc",
	},
	{
		.name = "hisi_spi_nand",
		.of_compatible = "hisilicon,hisi-spi-nand",
	},
	{
		.name = "hisi_nand",
		.of_compatible = "hisilicon,hisi-nand",
	},
};

static int hisi_fmc_probe(struct platform_device *pdev)
{
	int ret;
	struct hisi_fmc *fmc;
	struct resource *res;

	fmc = devm_kzalloc(&pdev->dev, sizeof(*fmc), GFP_KERNEL);
	if (!fmc)
		return -ENOMEM;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "control");
	fmc->regbase = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(fmc->regbase))
		return PTR_ERR(fmc->regbase);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "memory");
	fmc->iobase = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(fmc->iobase))
		return PTR_ERR(fmc->iobase);

	fmc->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(fmc->clk))
		return PTR_ERR(fmc->clk);

	mutex_init(&fmc->lock);

	platform_set_drvdata(pdev, fmc);

	ret = mfd_add_devices(&pdev->dev, 0, hisi_fmc_devs,
			ARRAY_SIZE(hisi_fmc_devs), NULL, 0, NULL);
	if (ret) {
		dev_err(&pdev->dev, "add mfd devices failed: %d\n", ret);
		return ret;
	}

	return 0;
}

static int hisi_fmc_remove(struct platform_device *pdev)
{
	struct hisi_fmc *fmc = platform_get_drvdata(pdev);

	mfd_remove_devices(&pdev->dev);
	mutex_destroy(&fmc->lock);

	return 0;
}

static const struct of_device_id hisi_fmc_of_match[] = {
	{ .compatible = "hisilicon,hisi-fmc"},
	{ }
};

static struct platform_driver hisi_fmc_driver = {
	.driver = {
		.name = "hifmc",
		.of_match_table = hisi_fmc_of_match,
	},
	.probe = hisi_fmc_probe,
	.remove = hisi_fmc_remove,
};
module_platform_driver(hisi_fmc_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HiSilicon Flash Memory Controller Driver");
