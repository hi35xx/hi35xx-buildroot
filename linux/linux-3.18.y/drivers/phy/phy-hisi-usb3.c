/*
 * Copyright (c) 2015 HiSilicon Technologies Co., Ltd.
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

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <mach/io.h>

#if defined(CONFIG_ARCH_HI3519) || defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3516AV200)
#ifdef readl
#undef readl
#undef readl_relaxed
#undef writel
#undef writel_relaxed
#define readl		hi_readl
#define readl_relaxed	hi_readl_relaxed
#define writel		hi_writel
#define writel_relaxed	hi_writel_relaxed
#endif /* readl */
#endif /* defined(CONFIG_ARCH_HI3519) || defined(CONFIG_ARCH_HI3519V101) || defined(CONFIG_ARCH_HI3516AV200)*/

#define USB3_CTRL		0xb8
#define USB3_UTMI_CLKSEL		BIT(13)
#define USB3_UTMI_CLKEN			BIT(12)
#define USB3_PIPE_CLKEN			BIT(11)
#define USB3_SUSPEND_CLKEN		BIT(10)
#define USB3_REF_CLKEN			BIT(9)
#define USB3_BUS_CLKEN			BIT(8)
#define USB3_VCC_SRST_REQ		BIT(0)

#define USB3_COMBPHY		0xac
#define HI3519_COMBPHY_SRST_REQ		BIT(0)

#define GTXTHRCFG		0xc108
#define GRXTHRCFG		0xc10c
#define REG_GCTL		0xc110
#define U2RSTECN		(0x1 << 16)

#define REG_GUSB2PHYCFG0	0xC200
#define BIT_UTMI_ULPI		(0x1 << 4)
#define BIT_UTMI_8_16		(0x1 << 3)

#define REG_GUSB3PIPECTL0	0xc2c0
#define PCS_SSP_SOFT_RESET	(0x1 << 31)
#define TX_MARGIN_MASK		(0x7 << 3)
#define TX_MARGIN_VAL		(0x2 << 3)

#define USB2_PHY0_CTLL		0x80

#define USB3_PHY		0x88
#define COMBO_PHY_TX_DEEMP_MASK		(0x7 << 12)
#define COMBO_PHY_TX_DEEMP_VAL		(0x1 << 12)
#ifdef CONFIG_USB3_DEVICE_GPIO_CTRL
#define GPIO_MODE		__io_address(0x12040000)
#define GPIO1_DIR		__io_address(0x12141400)
#define GPIO1_IBE		__io_address(0x12141408)
#define GPIO1_IC		__io_address(0x1214141c)
#define GPIO1_IE		__io_address(0x12141410)
#define GPIO1_0_DIR		(1<<0)
#define GPIO1_0_IBE		(1<<0)
#define GPIO1_0_IC		(1<<0)
#define GPIO1_0_IE		(1<<0)
#endif
struct hisi_priv {
	void __iomem	*base;
	void __iomem	*dwc3_ctrl; /* 0x10180000 */
	void __iomem	*peri_ctrl; /* 0x12010000 */
#if (defined CONFIG_ARCH_HI3519V101 || defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556 || defined CONFIG_ARCH_HI3516AV200)
	void __iomem	*misc_ctrl; /* 0x12030000 */
#endif
};

static void hisi_usb3_phy_eye(struct phy *phy)
{
	int reg;
	struct hisi_priv *priv = phy_get_drvdata(phy);

#if (defined CONFIG_ARCH_HI3519V101 || defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556 || defined CONFIG_ARCH_HI3516AV200)
	/* djustment LFPS AM */
	reg = readl(priv->dwc3_ctrl + REG_GUSB3PIPECTL0);
	reg &= ~TX_MARGIN_MASK;
	reg |= TX_MARGIN_VAL;
	writel(reg, priv->dwc3_ctrl + REG_GUSB3PIPECTL0);

	/* de-emphasis -3.5dB */
	reg = readl(priv->misc_ctrl + USB3_PHY);
	reg &= ~COMBO_PHY_TX_DEEMP_MASK;
	reg |= COMBO_PHY_TX_DEEMP_VAL;
	writel(reg, priv->misc_ctrl + USB3_PHY);
#endif
}

static int hisi_usb3_ctrl_phy_config(struct phy *phy)
{
	int reg;
	struct hisi_priv *priv = phy_get_drvdata(phy);

#ifdef CONFIG_USB3_DEVICE_GPIO_CTRL
	/* GPIO mode */
	hi_writel(0x0, GPIO_MODE);

	reg = hi_readl(GPIO1_DIR);
	reg &= ~GPIO1_0_DIR;
	hi_writel(reg, GPIO1_DIR);
	mdelay(20);
	/* falling edge */
	reg = hi_readl(GPIO1_IBE);
	reg &= ~GPIO1_0_IBE;
	hi_writel(reg, GPIO1_IBE);
	mdelay(20);
	/* clear */
	reg = hi_readl(GPIO1_IC);
	reg |= GPIO1_0_IC;
	hi_writel(reg, GPIO1_IC);
	mdelay(20);
	/* unmask */
	reg = hi_readl(GPIO1_IE);
	reg |= GPIO1_0_IE;
	hi_writel(reg, GPIO1_IE);
	mdelay(20);
#endif
	reg = readl(priv->dwc3_ctrl + REG_GUSB3PIPECTL0);
	reg |= PCS_SSP_SOFT_RESET;
	writel(reg, priv->dwc3_ctrl + REG_GUSB3PIPECTL0);

	/*step 3: USB2 PHY chose ulpi 8bit interface */
	reg = readl(priv->dwc3_ctrl + REG_GUSB2PHYCFG0);
	reg &= ~BIT_UTMI_ULPI;
	reg &= ~(BIT_UTMI_8_16);
	writel(reg, priv->dwc3_ctrl + REG_GUSB2PHYCFG0);
	mdelay(20);

	reg = readl(priv->dwc3_ctrl + REG_GCTL);
	reg &= ~(0x3<<12);
	reg |= (0x1<<12); /*[13:12] 01: Host; 10: Device; 11: OTG*/
	reg &= ~U2RSTECN;
	writel(reg, priv->dwc3_ctrl + REG_GCTL);
	mdelay(20);

	reg = readl(priv->dwc3_ctrl + REG_GUSB3PIPECTL0);
	reg &= ~PCS_SSP_SOFT_RESET;
	reg &= ~(1<<17);       /* disable suspend */
	writel(reg, priv->dwc3_ctrl + REG_GUSB3PIPECTL0);
	mdelay(100);

	writel(0x0, __io_address(0x120401ac));
	mdelay(20);
	writel(0x8, __io_address(0x1214d400));
	mdelay(20);
	writel(0x8, __io_address(0x1214d020));
	mdelay(200);
	writel(0x23100000, priv->dwc3_ctrl + GTXTHRCFG);
	writel(0x23180000, priv->dwc3_ctrl + GRXTHRCFG);
	mdelay(20);

	hisi_usb3_phy_eye(phy);

	return 0;
}

static int hisi_usb3_phy_on(struct phy *phy)
{
	int reg;
	struct hisi_priv *priv = phy_get_drvdata(phy);

	reg = readl_relaxed(priv->peri_ctrl + USB3_CTRL);
	reg |= USB3_UTMI_CLKSEL;
	reg |= USB3_UTMI_CLKEN;
	reg |= USB3_PIPE_CLKEN;
	reg |= USB3_SUSPEND_CLKEN;
	reg |= USB3_REF_CLKEN;
	reg |= USB3_BUS_CLKEN;
	reg &= ~USB3_VCC_SRST_REQ;
	writel_relaxed(reg, priv->peri_ctrl + USB3_CTRL);
	mdelay(10);

	reg = readl_relaxed(priv->peri_ctrl + USB3_COMBPHY);
	reg &=  ~HI3519_COMBPHY_SRST_REQ;
	writel_relaxed(reg, priv->peri_ctrl + USB3_COMBPHY);
	mdelay(10);

	hisi_usb3_ctrl_phy_config(phy);

	return 0;
}

static int hisi_usb3_phy_power_off(struct device *dev)
{
	int reg;
	struct phy *phy = dev_get_drvdata(dev);
	struct hisi_priv *priv = phy_get_drvdata(phy);

	reg = readl_relaxed(priv->peri_ctrl + USB3_COMBPHY);
	reg |=  HI3519_COMBPHY_SRST_REQ;
	writel_relaxed(reg, priv->peri_ctrl + USB3_COMBPHY);
	mdelay(100);

	reg = readl_relaxed(priv->peri_ctrl + USB3_CTRL);
	reg |= USB3_VCC_SRST_REQ;
	writel_relaxed(reg, priv->peri_ctrl + USB3_CTRL);
	mdelay(10);

	return 0;
}

static int hisi_usb3_phy_power_on(struct device *dev)
{
	struct phy *phy = dev_get_drvdata(dev);

	hisi_usb3_phy_on(phy);

	return 0;
}

static int hisi_usb3_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct phy *phy;
	struct hisi_priv *priv;
	struct device_node *np = pdev->dev.of_node;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dwc3_ctrl = of_iomap(np, 0);
	if (IS_ERR(priv->dwc3_ctrl))
		priv->peri_ctrl = NULL;

	priv->peri_ctrl = of_iomap(np, 1);
	if (IS_ERR(priv->peri_ctrl))
		priv->peri_ctrl = NULL;

#if (defined CONFIG_ARCH_HI3519V101 || defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556 || defined CONFIG_ARCH_HI3516AV200)
	priv->misc_ctrl = of_iomap(np, 2);
	if (IS_ERR(priv->misc_ctrl))
		priv->misc_ctrl = NULL;
#endif

	phy = devm_kzalloc(dev, sizeof(*phy), GFP_KERNEL);
	if (!phy)
		return -ENOMEM;

	platform_set_drvdata(pdev, phy);
	phy_set_drvdata(phy, priv);
	hisi_usb3_phy_on(phy);

	return 0;
}

static int hisi_usb3_phy_remove(struct platform_device *pdev)
{
	int reg;
	struct phy *phy = platform_get_drvdata(pdev);
	struct hisi_priv *priv = phy_get_drvdata(phy);

	reg = readl_relaxed(priv->peri_ctrl + USB3_CTRL);
	reg |= USB3_UTMI_CLKSEL;
	reg &= ~USB3_UTMI_CLKEN;
	reg &= ~USB3_PIPE_CLKEN;
	reg &= ~USB3_SUSPEND_CLKEN;
	reg &= ~USB3_REF_CLKEN;
	reg &= ~USB3_BUS_CLKEN;
	reg |= USB3_VCC_SRST_REQ;
	writel_relaxed(reg, priv->peri_ctrl + USB3_CTRL);
	mdelay(10);

	return 0;
}

static const struct dev_pm_ops hisi_usb3_pmops = {
	.suspend = hisi_usb3_phy_power_off,
	.resume  = hisi_usb3_phy_power_on,
#if defined(CONFIG_PM_HIBERNATE) || defined(CONFIG_HISI_SNAPSHOT_BOOT)
	.freeze = hisi_usb3_phy_power_off,
	.thaw = hisi_usb3_phy_power_on,
	.poweroff = hisi_usb3_phy_power_off,
	.restore = hisi_usb3_phy_power_on,
#endif
};

static const struct of_device_id hisi_usb3_phy_of_match[] = {
	{.compatible = "hisilicon,hisi-usb3-phy",},
	{ },
};
MODULE_DEVICE_TABLE(of, hisi_usb3_phy_of_match);

static struct platform_driver hisi_usb3_phy_driver = {
	.probe	= hisi_usb3_phy_probe,
	.remove = hisi_usb3_phy_remove,
	.driver = {
		.name	= "hisi-usb3-phy",
		.of_match_table	= hisi_usb3_phy_of_match,
		.pm    = &hisi_usb3_pmops,
	}
};
module_platform_driver(hisi_usb3_phy_driver);

MODULE_AUTHOR("Pengcheng Li <lpc.li@hisilicon.com>");
MODULE_DESCRIPTION("HISILICON USB PHY driver");
MODULE_ALIAS("platform:hisi-usb3-phy");
MODULE_LICENSE("GPL v2");
