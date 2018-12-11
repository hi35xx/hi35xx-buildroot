/*
 * Hisilicon Gigabit Ethernet MDIO Bus Driver
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/clk.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_mdio.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

#if defined(CONFIG_ARCH_HI3519) || defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3516AV200)
#ifdef readl
#undef readl
#undef writel
#define readl		hi_readl
#define writel		hi_writel
#endif
#endif

#define MDIO_SINGLE_CMD		0x00
#define MDIO_SINGLE_DATA	0x04
#define MDIO_RDATA_STATUS	0x10
#define BIT_PHY_ADDR_OFFSET	8
#define MDIO_WRITE		BIT(16)
#define MDIO_READ		BIT(17)
#define MDIO_START		BIT(20)
#define MDIO_START_READ		(MDIO_START | MDIO_READ)
#define MDIO_START_WRITE	(MDIO_START | MDIO_WRITE)

struct hisi_gemac_mdio_data {
	struct clk *clk;
	struct reset_control *phy_rst;
	void __iomem *membase;
};

static int hisi_gemac_mdio_wait_ready(struct hisi_gemac_mdio_data *data)
{
	u32 val;

	return readl_poll_timeout(data->membase + MDIO_SINGLE_CMD,
				  val, !(val & MDIO_START), 20, 10000);
}

static int hisi_gemac_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	struct hisi_gemac_mdio_data *data = bus->priv;
	int ret;

	ret = hisi_gemac_mdio_wait_ready(data);
	if (ret)
		return ret;

	writel(MDIO_START_READ | ((u32)mii_id << BIT_PHY_ADDR_OFFSET) |
		((u32)regnum),
	       data->membase + MDIO_SINGLE_CMD);

	ret = hisi_gemac_mdio_wait_ready(data);
	if (ret)
		return ret;

	/* if read data is invalid, we just return 0 instead of -EAGAIN.
	 * This can make MDIO more robust when reading PHY status.
	 */
	if (readl(data->membase + MDIO_RDATA_STATUS))
		return 0;

	return readl(data->membase + MDIO_SINGLE_DATA) >> 16;
}

static int hisi_gemac_mdio_write(struct mii_bus *bus, int mii_id, int regnum,
				 u16 value)
{
	struct hisi_gemac_mdio_data *data = bus->priv;
	int ret;

	ret = hisi_gemac_mdio_wait_ready(data);
	if (ret)
		return ret;

	writel(value, data->membase + MDIO_SINGLE_DATA);
	writel(MDIO_START_WRITE | ((u32)mii_id << BIT_PHY_ADDR_OFFSET) |
		((u32)regnum),
	       data->membase + MDIO_SINGLE_CMD);

	return hisi_gemac_mdio_wait_ready(data);
}

static void hisi_gemac_external_phy_reset(struct hisi_gemac_mdio_data *data)
{
	if (data->phy_rst) {
		/* write 0 to cancel reset */
		reset_control_deassert(data->phy_rst);
		msleep(50);

		/* HIFONE or 98cv200 use CRG register to reset phy */
		/* RST_BIT, write 0 to reset phy, write 1 to cancel reset */
		reset_control_assert(data->phy_rst);

		/* delay some time to ensure reset ok,
		 * this depends on PHY hardware feature
		 */
		msleep(50);

		/* write 0 to cancel reset */
		reset_control_deassert(data->phy_rst);
		/* delay some time to ensure later MDIO access */
		msleep(50);
	}
}

static int hisi_gemac_mdio_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct mii_bus *bus;
	struct hisi_gemac_mdio_data *data;
	struct resource *res;
	int ret;

	bus = mdiobus_alloc_size(sizeof(*data));
	if (!bus)
		return -ENOMEM;

	bus->name = "hisi_gemac_mii_bus";
	bus->read = &hisi_gemac_mdio_read;
	bus->write = &hisi_gemac_mdio_write;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s", pdev->name);
	bus->parent = &pdev->dev;

	data = bus->priv;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		ret = -ENXIO;
		goto err_out_free_mdiobus;
	}
	data->membase = devm_ioremap(&pdev->dev, res->start,
				     resource_size(res));
	if (!data->membase) {
		ret = -ENOMEM;
		goto err_out_free_mdiobus;
	}

	data->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(data->clk)) {
		ret = PTR_ERR(data->clk);
		goto err_out_free_mdiobus;
	}

	ret = clk_prepare_enable(data->clk);
	if (ret)
		goto err_out_free_mdiobus;

	data->phy_rst = devm_reset_control_get(&pdev->dev, "phy_reset");
	if (IS_ERR(data->phy_rst))
		data->phy_rst = NULL;
	hisi_gemac_external_phy_reset(data);

	ret = of_mdiobus_register(bus, np);
	if (ret)
		goto err_out_disable_clk;

	platform_set_drvdata(pdev, bus);

	return 0;

err_out_disable_clk:
	clk_disable_unprepare(data->clk);
err_out_free_mdiobus:
	mdiobus_free(bus);
	return ret;
}

static int hisi_gemac_mdio_remove(struct platform_device *pdev)
{
	struct mii_bus *bus = platform_get_drvdata(pdev);
	struct hisi_gemac_mdio_data *data = bus->priv;

	mdiobus_unregister(bus);
	clk_disable_unprepare(data->clk);
	mdiobus_free(bus);

	return 0;
}

static const struct of_device_id hisi_gemac_mdio_dt_ids[] = {
	{ .compatible = "hisilicon,hisi-gemac-mdio" },
	{ }
};
MODULE_DEVICE_TABLE(of, hisi_gemac_mdio_dt_ids);

static struct platform_driver hisi_gemac_mdio_driver = {
	.probe = hisi_gemac_mdio_probe,
	.remove = hisi_gemac_mdio_remove,
	.driver = {
		.name = "hisi-gemac-mdio",
		.of_match_table = hisi_gemac_mdio_dt_ids,
	},
};

module_platform_driver(hisi_gemac_mdio_driver);

MODULE_DESCRIPTION("Hisilicon Gigabit Ethernet MAC MDIO interface driver");
MODULE_AUTHOR("Dongpo Li <lidongpo@hisilicon.com>");
MODULE_LICENSE("GPL v2");
