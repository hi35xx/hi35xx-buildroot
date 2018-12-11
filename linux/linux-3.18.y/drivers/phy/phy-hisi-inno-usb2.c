 /*
 * HiSilicon INNO USB2 PHY Driver.
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
#include <linux/delay.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h>
#include <linux/io.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/kthread.h>
#include <linux/export.h>
#include <linux/pm.h>
#include <linux/of_address.h>

#define USB2_SWITCH_BASE		ioremap_nocache(0x12020150, 0x4)
#define USB2_OTG_BASE		0x5c
#define USB2_PHY_SET		0x60
#define USB_PHY_REG_ACCESS	(0x1 << 16)
#define DWC_OTG_EN                      (1 << 31)
#define USB2_PHY_DPPULL_DOWN            (0x3 << 26)
#define	MAX_PORTS	4
#define	PHY_CLK		0x18

#define US2_CRG_BASE		0xb8
#define	USB2_OHCI48M_CKEN	(0x1 << 1)
#define	USB2_OHCI12M_CKEN	(0x1 << 2)
#define	USB2_HST_PHY_CKEN	(0x1 << 4)
#define	USB2_UTMI0_CKEN		(0x1 << 5)
#define	USB2_PHY_CKEN		(0x1 << 7)
#define	USB2_BUS_SRST_REQ	(0x1 << 8)
#define	USB2_UTMI0_SRST_REQ	(0x1 << 9)
#define	USB2_HST_PHY_SRST_REQ	(0x1 << 11)
#define	USB2_PHY_REQ		(0x1 << 13)
#define	USB2_PHY_PORT0_TREQ	(0x1 << 14)
#define	USB2_PHY_CLKSEL		(0x1 << 15)

struct  hisi_inno_phy_port {
	struct clk *utmi_clk;
	struct reset_control *port_rst;
	struct reset_control *utmi_rst;
};

struct hisi_inno_phy_priv {
	void __iomem	*peri_ctrl; /* 0x12030000 */
	void __iomem	*phy_reg; /* 0x120d0000 */
	struct clk *ref_clk;
	struct reset_control *test_rst;
	struct reset_control *por_rst;
	struct reg_sequence *reg_seq;
	u32	reg_num;
	struct  hisi_inno_phy_port *ports;
	u8	port_num;
};
static struct task_struct *kusbotg_task;
static int *usb2_switch_base;

extern void usb2_low_power(int);
extern int otg_usbdev_stat;
int otg_usbhost_stat;
EXPORT_SYMBOL(otg_usbhost_stat);
static void __iomem	*peri_crg; /* 0x12010000 */

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

void usb2_low_power(int portsc)
{
#ifdef CONFIG_HIUSB_DEVICE2_0
	return;
#else
	int reg;

	if (portsc & 0x1) {
		reg = readl(peri_crg + US2_CRG_BASE);
		reg |= USB2_PHY_CKEN;
		writel(reg, peri_crg + US2_CRG_BASE);
		udelay(100);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg &= ~USB2_PHY_REQ;
		writel(reg, peri_crg + US2_CRG_BASE);
		mdelay(1);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg &= ~USB2_PHY_PORT0_TREQ;
		writel(reg, peri_crg + US2_CRG_BASE);
		udelay(10);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg &= ~USB2_OHCI48M_CKEN;
		reg &= ~USB2_OHCI12M_CKEN;
		reg &= ~USB2_UTMI0_CKEN;
		writel(reg, peri_crg + US2_CRG_BASE);
		udelay(10);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg &= ~USB2_PHY_CLKSEL;
		writel(reg, peri_crg + US2_CRG_BASE);
		udelay(10);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg |= USB2_OHCI48M_CKEN;
		reg |= USB2_OHCI12M_CKEN;
		reg |= USB2_HST_PHY_CKEN;
		reg |= USB2_UTMI0_CKEN;
		writel(reg, peri_crg + US2_CRG_BASE);
	} else {
		reg = readl(peri_crg + US2_CRG_BASE);
		reg &= ~USB2_OHCI48M_CKEN;
		reg &= ~USB2_OHCI12M_CKEN;
		reg &= ~USB2_HST_PHY_CKEN;
		reg &= ~USB2_UTMI0_CKEN;
		writel(reg, peri_crg + US2_CRG_BASE);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg |= USB2_PHY_CLKSEL;
		writel(reg, peri_crg + US2_CRG_BASE);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg |= USB2_OHCI48M_CKEN;
		reg |= USB2_OHCI12M_CKEN;
		reg |= USB2_UTMI0_CKEN;
		writel(reg, peri_crg + US2_CRG_BASE);
		reg = readl(peri_crg + US2_CRG_BASE);
		reg |= USB2_PHY_PORT0_TREQ;
		reg |= USB2_PHY_REQ;
		reg &= ~USB2_PHY_CKEN;
		writel(reg, peri_crg + US2_CRG_BASE);
	}
#endif
}

static int hisi_inno_phy_setup(struct hisi_inno_phy_priv *priv)
{
	int reg;
	/* config eye */
	reg = readl(priv->peri_ctrl + USB2_PHY_SET);
	reg |= USB_PHY_REG_ACCESS;
	writel(reg, priv->peri_ctrl + USB2_PHY_SET);
	writel(0x4, priv->phy_reg + PHY_CLK);
	mdelay(2);
	writel(0x18, priv->phy_reg);
	udelay(20);
	writel(0xc4, priv->phy_reg + 0x20);
	udelay(20);
	writel(0xc1, priv->phy_reg + 0x44);
	udelay(20);
	writel(0x1b, priv->phy_reg + 0x28);
	udelay(20);

	return 0;
}

static int hisi_inno_port_init(struct hisi_inno_phy_port *port)
{
	int ret = 0;

	reset_control_deassert(port->port_rst);
	mdelay(2);

	ret = clk_prepare_enable(port->utmi_clk);
	if (ret)
		return ret;
	udelay(200);

	reset_control_deassert(port->utmi_rst);
	udelay(200);

	return 0;
}

static int hisi_inno_phy_init(struct phy *phy)
{
	struct hisi_inno_phy_priv *priv = phy_get_drvdata(phy);
	int ret, port;

	ret = clk_prepare_enable(priv->ref_clk);
	if (ret)
		return ret;
	udelay(100);

	if (priv->test_rst) {
		reset_control_deassert(priv->test_rst);
		udelay(100);
	}

	reset_control_deassert(priv->por_rst);
	udelay(300);

	/* config phy clk and phy eye diagram */
	ret = hisi_inno_phy_setup(priv);
	if (ret)
		goto err_disable_ref_clk;

	for (port = 0; port < priv->port_num; port++) {
		ret = hisi_inno_port_init(&priv->ports[port]);
		if (ret)
			goto err_disable_clks;
	}

	return 0;

err_disable_clks:
	while (--port >= 0)
		clk_disable_unprepare(priv->ports[port].utmi_clk);
err_disable_ref_clk:
	clk_disable_unprepare(priv->ref_clk);

	return ret;
}

static void hisi_inno_phy_disable(struct phy *phy)
{
	struct hisi_inno_phy_priv *priv = phy_get_drvdata(phy);
	int i;

	for (i = 0; i < priv->port_num; i++)
		clk_disable_unprepare(priv->ports[i].utmi_clk);

	clk_disable_unprepare(priv->ref_clk);
}

static int hisi_inno_phy_of_get_ports(struct device *dev,
					struct  hisi_inno_phy_priv *priv)
{
	struct device_node *node = dev->of_node, *child;
	int port = 0, ret = 0;

	priv->port_num = of_get_child_count(node);
	if (priv->port_num > MAX_PORTS) {
		dev_err(dev, "too many ports : %d (max = %d)\n",
				priv->port_num, MAX_PORTS);
		return -EINVAL;
	}

	priv->ports = devm_kcalloc(dev, priv->port_num,
				sizeof(struct hisi_inno_phy_port), GFP_KERNEL);
	if (!priv->ports)
		return -ENOMEM;

	for_each_child_of_node(node, child) {
		struct hisi_inno_phy_port *phy_port = &priv->ports[port];

		phy_port->utmi_clk = of_clk_get(child, 0);
		if (IS_ERR(phy_port->utmi_clk)) {
			ret = PTR_ERR(phy_port->utmi_clk);
			goto fail;
		}

		phy_port->port_rst = of_reset_control_get(child, "port_rst");
		if (IS_ERR(phy_port->port_rst)) {
			ret = PTR_ERR(phy_port->port_rst);
			clk_put(phy_port->utmi_clk);
			goto fail;
		}

		phy_port->utmi_rst = of_reset_control_get(child, "utmi_rst");
		if (IS_ERR(phy_port->utmi_rst)) {
			ret = PTR_ERR(phy_port->utmi_rst);
			reset_control_put(phy_port->port_rst);
			clk_put(phy_port->utmi_clk);
			goto fail;
		}
		port++;
	}

	return ret;

fail:
	while (--port >= 0) {
		struct hisi_inno_phy_port *phy_port = &priv->ports[port];

		reset_control_put(phy_port->utmi_rst);
		reset_control_put(phy_port->port_rst);
		clk_put(phy_port->utmi_clk);
	}
	of_node_put(child);

	return ret;
}

/* hiotg run */
static void device_to_host(struct hisi_inno_phy_priv *priv)
{
	int reg;

	reg = readl_relaxed(priv->peri_ctrl + USB2_OTG_BASE);
	reg |= USB2_PHY_DPPULL_DOWN;
	reg &= ~DWC_OTG_EN;
	writel_relaxed(reg, priv->peri_ctrl + USB2_OTG_BASE);

}

static void host_to_device(struct hisi_inno_phy_priv *priv)
{

	int reg;

	reg = readl_relaxed(priv->peri_ctrl + USB2_OTG_BASE);
	reg &= ~(USB2_PHY_DPPULL_DOWN);
	reg |= DWC_OTG_EN;
	writel_relaxed(reg, priv->peri_ctrl + USB2_OTG_BASE);

}

int  otg_run(struct hisi_inno_phy_priv *priv)
{
	int reg;

	reg = readl(priv->peri_ctrl + USB2_OTG_BASE);

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
	struct hisi_inno_phy_priv *priv = arg;
	int reg;
#endif

	writel(0x0, usb2_switch_base);
#ifdef CONFIG_USB_AUTO_SWITCH
	do {
		reg = readl(usb2_switch_base);
		if (!(reg & (0x1 << 1)))
			otg_run(priv);

		msleep(1000);

	} while (1);
#endif

	return 0;
}


static int hisi_inno_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct phy *phy;
	struct hisi_inno_phy_priv *priv;
	struct device_node *node = dev->of_node;
	int ret = 0;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->peri_ctrl = of_iomap(node, 0); /* 0x12030000 */
	priv->phy_reg = of_iomap(node, 1); /* 0x120d0000 */

#ifdef CONFIG_HIUSB_DEVICE2_0
	peri_crg = NULL;
#else
	peri_crg = of_iomap(node, 2); /* 0x12010000 */
#endif

	priv->ref_clk = devm_clk_get(dev, NULL);
	if (IS_ERR(priv->ref_clk))
		return PTR_ERR(priv->ref_clk);

	priv->por_rst = devm_reset_control_get(dev, "por_rst");
	if (IS_ERR(priv->por_rst))
		return PTR_ERR(priv->por_rst);

	priv->test_rst = devm_reset_control_get(dev, "test_rst");
	if (IS_ERR(priv->test_rst))
		priv->test_rst = NULL;

	ret = hisi_inno_phy_of_get_ports(dev, priv);
	if (ret)
		return ret;

	phy = devm_kzalloc(dev, sizeof(*phy), GFP_KERNEL);
	if (!phy)
		return -ENOMEM;

	platform_set_drvdata(pdev, phy);
	phy_set_drvdata(phy, priv);
	ret = hisi_inno_phy_init(phy);

	usb2_switch_base = (int *)USB2_SWITCH_BASE;
	/* run hiotg */
	kusbotg_task = kthread_run(usbotg_thread, priv, "kusbotg");
	if (IS_ERR(kusbotg_task)) {
		dev_err(dev, "can't start kusbotg\n");

		return ret;
	}

	return ret;
}

#ifdef CONFIG_PM_SLEEP
static int hisi_inno_phy_suspend(struct device *dev)
{
	struct phy *phy = dev_get_drvdata(dev);

	hisi_inno_phy_disable(phy);

	return 0;
}

static int hisi_inno_phy_resume(struct device *dev)
{
	struct phy *phy = dev_get_drvdata(dev);
	int ret = 0;

	ret = hisi_inno_phy_init(phy);
	if (ret)
		return ret;

	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static const struct dev_pm_ops hisi_inno_phy_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(hisi_inno_phy_suspend, hisi_inno_phy_resume)
};

static const struct of_device_id hisi_inno_phy_of_match[] = {
	{.compatible = "hisilicon,inno_usb2_phy",},
	{ },
};
MODULE_DEVICE_TABLE(of, hisi_inno_phy_of_match);

static struct platform_driver hisi_inno_phy_driver = {
	.probe	= hisi_inno_phy_probe,
	.driver = {
		.name	= "hisi-inno-phy",
		.of_match_table	= hisi_inno_phy_of_match,
		.pm    = &hisi_inno_phy_pm_ops,
	}
};
module_platform_driver(hisi_inno_phy_driver);

MODULE_AUTHOR("Pengcheng Li <lpc.li@hisilicon.com>");
MODULE_DESCRIPTION("HISILICON USB PHY driver");
MODULE_ALIAS("platform:hisi-usb-phy");
MODULE_LICENSE("GPL v2");
