/*
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
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

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <mach/io.h>
#include <mach/platform.h>

static int n_ports = CONFIG_HI_SATA_PORTS;
static int phy_mode = CONFIG_HI_SATA_MODE;
unsigned int sata_port_map;

#ifdef MODULE
module_param(phy_config, uint, 0600);
MODULE_PARM_DESC(phy_config, "sata phy config (default:0x0e180000)");

module_param(n_ports, uint, 0600);
MODULE_PARM_DESC(n_ports, "sata port number (default:2)");
module_param(mode_3g, uint, 0600);
MODULE_PARM_DESC(phy_mode, "sata phy mode (0:1.5G;1:3G(default);2:6G)");
#endif

#ifdef CONFIG_ARCH_HI3536C
#include "phy-hi3536c-sata.c"
#endif

static int hisi_sata_phy_init(struct phy *phy)
{
	void __iomem *mmio = phy_get_drvdata(phy);

	hi_sata_poweron();
	hi_sata_reset();
	hi_sata_phy_reset();
	hi_sata_phy_clk_sel();
	hi_sata_clk_enable();
	msleep(20);
	hi_sata_phy_unreset();
	msleep(20);
	hi_sata_set_phy_6G();
	msleep(20);
	hi_sata_unreset();
	msleep(20);
	hisata_phy_init(mmio, phy_mode, n_ports);

	return 0;
}

static int hisi_sata_phy_exit(struct phy *phy)
{
	hi_sata_phy_reset();
	msleep(20);
	hi_sata_reset();
	msleep(20);
	hi_sata_clk_reset();
	msleep(20);
	hi_sata_clk_disable();
	hi_sata_poweroff();
	msleep(20);

	return 0;
}

static struct phy_ops hisi_sata_phy_ops = {
	.init		= hisi_sata_phy_init,
	.exit		= hisi_sata_phy_exit,
	.owner		= THIS_MODULE,
};

static int hisi_sata_phy_probe(struct platform_device *pdev)
{
	struct phy_provider *phy_provider;
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct phy *phy;
	void __iomem *mmio;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "failed to get reg base\n");
		return -ENOENT;
	}

	mmio = devm_ioremap(dev, res->start, resource_size(res));
	if (!mmio)
		return -ENOMEM;

	phy = devm_phy_create(dev, NULL, &hisi_sata_phy_ops, NULL);
	if (IS_ERR(phy)) {
		dev_err(dev, "failed to create PHY\n");
		return PTR_ERR(phy);
	}

	phy_set_drvdata(phy, mmio);

	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(phy_provider))
		return PTR_ERR(phy_provider);

	return 0;
}

static int hisi_sata_phy_suspend(struct platform_device *pdev,
		pm_message_t state)
{
	struct device *dev = &pdev->dev;
	struct phy *phy = to_phy(dev);

	hisi_sata_phy_exit(phy);

	return 0;
}

static int hisi_sata_phy_resume(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct phy *phy = to_phy(dev);

	hisi_sata_phy_init(phy);

	return 0;
}

static const struct of_device_id hisi_sata_phy_of_match[] = {
	{.compatible = "hisilicon,hisi-sata-nano-phy",},
	{ },
};
MODULE_DEVICE_TABLE(of, hisi_sata_phy_of_match);

static struct platform_driver hisi_sata_phy_driver = {
	.probe	= hisi_sata_phy_probe,
	.suspend = hisi_sata_phy_suspend,
	.resume  = hisi_sata_phy_resume,
	.driver = {
		.name	= "hisi-sata-nano-phy",
		.of_match_table	= hisi_sata_phy_of_match,
	}
};
module_platform_driver(hisi_sata_phy_driver);

MODULE_AUTHOR("HiSilicon BVT");
MODULE_DESCRIPTION("HISILICON SATA NANO PHY driver");
MODULE_ALIAS("platform:hisi-sata-nano-phy");
MODULE_LICENSE("GPL v2");
