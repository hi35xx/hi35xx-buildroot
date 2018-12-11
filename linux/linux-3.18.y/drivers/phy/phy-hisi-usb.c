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
#include <linux/kthread.h>
#include <linux/export.h>

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
#endif /* defined(CONFIG_ARCH_HI3519) || defined(CONFIG_HI3519V101) || defined(CONFIG_ARCH_HI3516AV200)*/

#define USB2_SWITCH_BASE		ioremap_nocache(0x12020150, 0x4)
#define USB2_OTG_BASE		0x78
#define DWC_OTG_EN			(1 << 31)
#define USB2_PHY_DPPULL_DOWN		(0x3 << 26)

#define USB2_PHY0_RST		0xb4

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
#define HI3519_USB2_PHY_REQ		BIT(1)
#define HI3519_USB2_PHY_PORT0_TREQ	BIT(2)
#define HI3519_USB2_CTRL_HUB_REQ	BIT(4)
#define HI3519_USB_CKEN			BIT(7)
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
#define HI3559_USB2_PHY_REQ		BIT(1)
#define HI3559_USB2_PHY_PORT0_TREQ	BIT(2)
#define HI3559_USB2_CTRL_HUB_REQ	BIT(4)
#define HI3559_USB_CKEN			BIT(7)
#endif

#ifdef CONFIG_ARCH_HI3516AV200
#define HI3516AV200_USB2_PHY_REQ	BIT(1)
#define HI3516AV200_USB2_PHY_PORT0_TREQ	BIT(2)
#define HI3516AV200_USB2_CTRL_HUB_REQ 	BIT(4)
#define HI3516AV200_USB_CKEN		BIT(7)
#endif

#define USB2_PHY0_CTLL		0x80

/* HI3536C */
#define USB2_CTRL		0x130
#define USB2_BUS_CKEN		(1 << 0)
#define USB2_OHCI48M_CKEN	(1 << 1)
#define USB2_OHCI12M_CKEN	(1 << 2)
#define USB2_HST_PHY_CKEN	(1 << 4)
#define USB2_UTMI0_CKEN		(1 << 5)
#define USB2_UTMI1_CKEN		(1 << 6)
#define USB2_BUS_SRST_REQ	(1 << 12)
#define USB2_UTMI0_SRST_REQ	(1 << 13)
#define USB2_UTMI1_SRST_REQ	(1 << 14)
#define USB2_HST_PHY_SYST_REQ	(1 << 16)

#define REG_USB2_PHY0		0x134
#define USB_PHY0_REF_CKEN	(1 << 0)
#define USB_PHY0_SRST_REQ	(1 << 8)
#define USB_PHY0_SRST_TREQ	(1 << 9)
#define USB_PHY1_SRST_TREQ	(1 << 10)
#define USB_PHY0_TEST_SRST_REQ	(1 << 11)
#define USB_PHY0_REFCLK_SEL	(1 << 16)
#ifdef CONFIG_ARCH_HI3536C
#define USB1_CTRL0		0x50
#endif
#define WORDINTERFACE		(1 << 0)
#define SS_BURST4_EN		(1 << 7)
#define SS_BURST8_EN		(1 << 8)
#define SS_BURST16_EN		(1 << 9)

#define USB2_PHY_TEST_REG_ACCESS	(1 << 20)

#define USB2_CTRL1		0x94
/* write(0x1 << 5) 0x6 to addr 0x4 */
#define CONFIG_CLK	((0x1 << 21) | (0x6 << 8) | (0x4 << 0))
/* END HI3536C */

#ifndef CONFIG_ARCH_HI3536C
static struct task_struct *kusbotg_task;
static int *usb2_switch_base;

extern int otg_usbdev_stat;
int otg_usbhost_stat;
EXPORT_SYMBOL(otg_usbhost_stat);

void hisi_switch_func(int otg)
{
	int reg;

	reg = readl(usb2_switch_base);
	if (otg) {
		reg |= 0x1;
		writel(reg, usb2_switch_base);
	} else {
		reg &= ~(0x1);
		writel(reg, usb2_switch_base);
	}
}
EXPORT_SYMBOL(hisi_switch_func);
#endif
struct hisi_priv {
	void __iomem	*base;
	void __iomem	*misc_ctrl; /* 0x12030000 */
	void __iomem	*peri_ctrl; /* 0x12010000 */
};

#ifndef CONFIG_ARCH_HI3536C
static int hisi_usb_phy_config(struct phy *phy)
{
	struct hisi_priv *priv = phy_get_drvdata(phy);

	writel_relaxed(0x80001c, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa0001c, priv->misc_ctrl + USB2_PHY0_CTLL);
	usleep_range(200, 250);

#if defined(CONFIG_ARCH_HI3519V101) \
        || defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556) || defined(CONFIG_ARCH_HI3516AV200)
	writel_relaxed(0x800acb, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa00acb, priv->misc_ctrl + USB2_PHY0_CTLL);
	usleep_range(200, 250);

	writel_relaxed(0x801181, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa01181, priv->misc_ctrl + USB2_PHY0_CTLL);
	usleep_range(200, 250);

	writel_relaxed(0x800592, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa00592, priv->misc_ctrl + USB2_PHY0_CTLL);
	usleep_range(200, 250);
#endif

	writel_relaxed(0x800904, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa00904, priv->misc_ctrl + USB2_PHY0_CTLL);
	usleep_range(200, 250);

	writel_relaxed(0x80060f, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa0060f, priv->misc_ctrl + USB2_PHY0_CTLL);
	usleep_range(200, 250);

#ifdef CONFIG_ARCH_HI3519
	writel_relaxed(0x800a4b, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa00a4b, priv->misc_ctrl + USB2_PHY0_CTLL);
	usleep_range(200, 250);
#endif

	writel_relaxed(0x801141, priv->misc_ctrl + USB2_PHY0_CTLL);
	writel_relaxed(0xa01141, priv->misc_ctrl + USB2_PHY0_CTLL);
	msleep(20);

	return 0;
}
#endif

static int hisi_usb_phy_on(struct phy *phy)
{
	int reg;
	struct hisi_priv *priv = phy_get_drvdata(phy);

#ifdef CONFIG_ARCH_HI3536C
	reg = readl(priv->peri_ctrl + USB2_CTRL);
	reg |= (USB2_BUS_SRST_REQ
		| USB2_UTMI0_SRST_REQ
		| USB2_HST_PHY_SYST_REQ);
	reg |= USB2_UTMI1_SRST_REQ;
	writel(reg, priv->peri_ctrl + USB2_CTRL);
	udelay(200);

	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg |= (USB_PHY0_SRST_REQ
		| USB_PHY0_SRST_TREQ);
	reg |= USB_PHY1_SRST_TREQ;
	writel(reg, priv->peri_ctrl + REG_USB2_PHY0);
	udelay(200);
	reg = readl(priv->misc_ctrl + USB1_CTRL0);
	reg &= ~(WORDINTERFACE); /* 8bit */
	reg &= ~(SS_BURST16_EN); /* 16 bit burst disable */
	writel(reg, priv->misc_ctrl + USB1_CTRL0);
	udelay(100);
	/* for ssk usb storage ok */
	msleep(20);

	/* open ref clock */
	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg |= (USB_PHY0_REF_CKEN);
	writel(reg, priv->peri_ctrl + REG_USB2_PHY0);
	udelay(100);

	/* cancel power on reset */
	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg &= ~(USB_PHY0_SRST_REQ);
	reg &= ~(USB_PHY0_TEST_SRST_REQ);
	writel(reg , priv->peri_ctrl + REG_USB2_PHY0);
	udelay(300);

	/* config type */
	reg = readl(priv->misc_ctrl + 0x10);
	reg |= USB2_PHY_TEST_REG_ACCESS;
	writel(reg, priv->misc_ctrl + 0x10);
	udelay(2);
	/* config clock */
	writel(0xc, priv->misc_ctrl + 0x8018);
	mdelay(2);
	/* cancel port reset */
	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg &= ~(USB_PHY0_SRST_TREQ);
	reg &= ~(USB_PHY1_SRST_TREQ);
	writel(reg, priv->peri_ctrl + REG_USB2_PHY0);
	udelay(300);

	/* cancel control reset */
	reg = readl(priv->peri_ctrl + USB2_CTRL);
	reg &= ~(USB2_BUS_SRST_REQ
		| USB2_UTMI0_SRST_REQ
		| USB2_HST_PHY_SYST_REQ);
	reg &= ~USB2_UTMI1_SRST_REQ;

	reg |= (USB2_BUS_CKEN
			| USB2_OHCI48M_CKEN
			| USB2_OHCI12M_CKEN
			| USB2_HST_PHY_CKEN
			| USB2_UTMI0_CKEN);
	reg |= USB2_UTMI1_CKEN;
	writel(reg, priv->peri_ctrl + USB2_CTRL);
	udelay(200);

#else
	reg = readl_relaxed(priv->peri_ctrl + USB2_PHY0_RST);
#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
	reg &= ~HI3519_USB2_PHY_REQ;
	reg &= ~HI3519_USB2_PHY_PORT0_TREQ;
	reg &= ~HI3519_USB2_CTRL_HUB_REQ;
	reg |= HI3519_USB_CKEN;
#endif

#if defined(CONFIG_ARCH_HI3516AV200)
	reg &= ~HI3516AV200_USB2_PHY_REQ;
	reg &= ~HI3516AV200_USB2_PHY_PORT0_TREQ;
	reg &= ~HI3516AV200_USB2_CTRL_HUB_REQ;
	reg |= HI3516AV200_USB_CKEN;
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
	reg &=  ~HI3559_USB2_PHY_REQ;
	reg &=  ~HI3559_USB2_PHY_PORT0_TREQ;
	reg &=  ~HI3559_USB2_CTRL_HUB_REQ;
	reg |= HI3559_USB_CKEN;
#endif
	writel_relaxed(reg, priv->peri_ctrl + USB2_PHY0_RST);
	mdelay(100);

	writel_relaxed(0x800000, priv->misc_ctrl + USB2_PHY0_CTLL);
	mdelay(100);
	writel_relaxed(0xa0060c, priv->misc_ctrl + USB2_PHY0_CTLL);
	mdelay(100);

	hisi_usb_phy_config(phy);
#endif
	return 0;
}

#ifndef CONFIG_ARCH_HI3536C
/* hiotg run */
static void device_to_host(struct hisi_priv *priv)
{
	int reg;

	reg = readl_relaxed(priv->misc_ctrl + USB2_OTG_BASE);
	reg |= USB2_PHY_DPPULL_DOWN;
	reg &= ~DWC_OTG_EN;
	writel_relaxed(reg, priv->misc_ctrl + USB2_OTG_BASE);
}

static void host_to_device(struct hisi_priv *priv)
{
	int reg;

	reg = readl_relaxed(priv->misc_ctrl + USB2_OTG_BASE);
	reg &= ~(USB2_PHY_DPPULL_DOWN);
	reg |= DWC_OTG_EN;
	writel_relaxed(reg, priv->misc_ctrl + USB2_OTG_BASE);
}

int  otg_run(struct hisi_priv *priv)
{
	int reg;

	reg = readl(priv->misc_ctrl + USB2_OTG_BASE);

	/* device -->host */
	if ((reg & DWC_OTG_EN) == DWC_OTG_EN) {

		if (otg_usbhost_stat == 1)
			return 0;
		device_to_host(priv);

	} else { /* host -->device */
		if (otg_usbdev_stat == 1)
			return 0;

		host_to_device(priv);
	}


	return 0;

}

static int usbotg_thread(void *arg)
{
#ifdef CONFIG_USB_AUTO_SWITCH
	struct hisi_priv *priv = arg;
	int reg;
#endif


	writel_relaxed(0x0, usb2_switch_base);
#ifdef CONFIG_USB_AUTO_SWITCH
	do {
		reg = readl_relaxed(usb2_switch_base);
		if (!(reg & (0x1 << 1)))
			otg_run(priv);

		msleep(1000);

	} while (1);
#endif

	return 0;
}
#endif

static int hisi_usb_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct phy *phy;
	struct hisi_priv *priv;
	struct device_node *np = pdev->dev.of_node;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->misc_ctrl = of_iomap(np, 0); /* 0x12030000 */
	if (IS_ERR(priv->misc_ctrl))
		priv->misc_ctrl = NULL;

	priv->peri_ctrl = of_iomap(np, 1); /* 0x12010000 */
	if (IS_ERR(priv->peri_ctrl))
		priv->peri_ctrl = NULL;

	phy = devm_kzalloc(dev, sizeof(*phy), GFP_KERNEL);
	if (!phy)
		return -ENOMEM;
	platform_set_drvdata(pdev, phy);
	phy_set_drvdata(phy, priv);
	hisi_usb_phy_on(phy);

#ifndef CONFIG_ARCH_HI3536C
	usb2_switch_base = (int *)USB2_SWITCH_BASE;
	/* run hiotg */
	kusbotg_task = kthread_run(usbotg_thread, priv, "kusbotg");
	if (IS_ERR(kusbotg_task)) {
		dev_err(dev, "can't start kusbotg\n");
		return -1;
	}
#endif
	return 0;
}

static int hisi_usb_phy_remove(struct platform_device *pdev)
{
	int reg;
	struct phy *phy = platform_get_drvdata(pdev);
	struct hisi_priv *priv = phy_get_drvdata(phy);

#ifdef CONFIG_ARCH_HI3536C
	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg |= (USB_PHY0_SRST_REQ
		| USB_PHY0_SRST_TREQ);
	reg |= USB_PHY1_SRST_TREQ;
	writel(reg, priv->peri_ctrl + REG_USB2_PHY0);
	udelay(100);

	/* close clock */
	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg &= ~(USB_PHY0_REFCLK_SEL
		| USB_PHY0_REF_CKEN);
	writel(reg, priv->peri_ctrl + REG_USB2_PHY0);
	udelay(300);

	/* close clock */
	reg = readl(priv->peri_ctrl + USB2_CTRL);
	reg &= ~(USB2_BUS_CKEN
		| USB2_OHCI48M_CKEN
		| USB2_OHCI12M_CKEN
		| USB2_HST_PHY_CKEN
		| USB2_UTMI0_CKEN);
	reg &= ~USB2_UTMI1_CKEN;
	writel(reg, priv->peri_ctrl + USB2_CTRL);
	udelay(200);
#else
	reg = readl_relaxed(priv->peri_ctrl + USB2_PHY0_RST);
#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
	reg |= HI3519_USB2_PHY_REQ;
	reg |= HI3519_USB2_PHY_PORT0_TREQ;
	reg |= HI3519_USB2_CTRL_HUB_REQ;
	reg &= ~HI3519_USB_CKEN;
#endif

#if defined(CONFIG_ARCH_HI3516AV200)
	reg |= HI3516AV200_USB2_PHY_REQ;
	reg |= HI3516AV200_USB2_PHY_PORT0_TREQ;
	reg |= HI3516AV200_USB2_CTRL_HUB_REQ;
	reg &= ~HI3516AV200_USB_CKEN;
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
	reg |= HI3559_USB2_PHY_REQ;
	reg |= HI3559_USB2_PHY_PORT0_TREQ;
	reg |= HI3559_USB2_CTRL_HUB_REQ;
	reg &= ~HI3559_USB_CKEN;
#endif
	writel_relaxed(reg, priv->peri_ctrl + USB2_PHY0_RST);
	mdelay(100);
#endif
	return 0;
}

static const struct of_device_id hisi_usb_phy_of_match[] = {
	{.compatible = "hisilicon,hisi-usb-phy",},
	{ },
};
MODULE_DEVICE_TABLE(of, hisi_usb_phy_of_match);

#ifdef CONFIG_PM_SLEEP

static int hisi_usb_phy_suspend(struct device *dev)
{
	int reg;
	struct phy *phy = dev_get_drvdata(dev);
	struct hisi_priv *priv = phy_get_drvdata(phy);

#ifdef CONFIG_ARCH_HI3536C
	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg |= (USB_PHY0_SRST_REQ
		| USB_PHY0_SRST_TREQ);
	reg |= USB_PHY1_SRST_TREQ;
	writel(reg, priv->peri_ctrl + REG_USB2_PHY0);
	udelay(100);

	/* close clock */
	reg = readl(priv->peri_ctrl + REG_USB2_PHY0);
	reg &= ~(USB_PHY0_REFCLK_SEL
		| USB_PHY0_REF_CKEN);
	writel(reg, priv->peri_ctrl + REG_USB2_PHY0);
	udelay(300);

	/* close clock */
	reg = readl(priv->peri_ctrl + USB2_CTRL);
	reg &= ~(USB2_BUS_CKEN
		| USB2_OHCI48M_CKEN
		| USB2_OHCI12M_CKEN
		| USB2_HST_PHY_CKEN
		| USB2_UTMI0_CKEN);
	reg &= ~USB2_UTMI1_CKEN;
	writel(reg, priv->peri_ctrl + USB2_CTRL);
	udelay(200);
#else
	reg = readl_relaxed(priv->peri_ctrl + USB2_PHY0_RST);
#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
	reg |= HI3519_USB2_PHY_REQ;
	reg |= HI3519_USB2_PHY_PORT0_TREQ;
	reg |= HI3519_USB2_CTRL_HUB_REQ;
	reg &= ~HI3519_USB_CKEN;
#endif

#if defined(CONFIG_ARCH_HI3516AV200)
	reg |= HI3516AV200_USB2_PHY_REQ;
	reg |= HI3516AV200_USB2_PHY_PORT0_TREQ;
	reg |= HI3516AV200_USB2_CTRL_HUB_REQ;
	reg &= ~HI3516AV200_USB_CKEN;
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
	reg |= HI3559_USB2_PHY_REQ;
	reg |= HI3559_USB2_PHY_PORT0_TREQ;
	reg |= HI3559_USB2_CTRL_HUB_REQ;
	reg &= ~HI3559_USB_CKEN;
#endif
	writel_relaxed(reg, priv->peri_ctrl + USB2_PHY0_RST);
	mdelay(100);
#endif
	return 0;
}

static int hisi_usb_phy_resume(struct device *dev)
{
	struct phy *phy = dev_get_drvdata(dev);

	hisi_usb_phy_on(phy);
	return 0;
}

#endif /* CONFIG_PM_SLEEP */

static SIMPLE_DEV_PM_OPS(hisi_usb2_pm_ops, hisi_usb_phy_suspend,
		       hisi_usb_phy_resume);

static struct platform_driver hisi_usb_phy_driver = {
	.probe	= hisi_usb_phy_probe,
	.remove = hisi_usb_phy_remove,
	.driver = {
		.name	= "hisi-usb-phy",
		.pm	= &hisi_usb2_pm_ops,
		.of_match_table	= hisi_usb_phy_of_match,
	}
};
module_platform_driver(hisi_usb_phy_driver);

MODULE_AUTHOR("Pengcheng Li <lpc.li@hisilicon.com>");
MODULE_DESCRIPTION("HISILICON USB PHY driver");
MODULE_ALIAS("platform:hisi-usb-phy");
MODULE_LICENSE("GPL v2");
