/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
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

#define HIUSB_OHCI_DEV_NAME	"hiusb-ohci"

#define USB_CKEN		BIT(0)
#define USB_OHCI48M_CKEN	BIT(1)
#define USB_OHCI12M_CKEN	BIT(2)
#define USB_OTG_UTMI_CKEN	BIT(3)
#define USB_HST_PHY_CKEN	BIT(4)
#define USB_UTMI0_CKEN		BIT(5)
#define USB_PHY_CKEN		BIT(7)
#define USB_BUS_SRST_REQ	BIT(8)
#define USB_UTMI0_SRST_REQ	BIT(9)
#define USB_HST_PHY_SRST_REQ	BIT(11)
#define USB_OTG_PHY_SRST_REQ	BIT(12)
#define USB_PHY_REQ		BIT(13)
#define USB_PHY_PORT0_TREQ	BIT(14)
#define USB_PHY_CLKSEL		BIT(15)
#define USB_PHY_TEST_SRST_REQ	BIT(17)

#define PERI_USB		(MISC_REG_BASE + 0x5c)
#define WORDINTERFACE		BIT(0)
#define SS_BURST4_EN		BIT(7)
#define SS_BURST8_EN		BIT(8)
#define SS_BURST16_EN		BIT(9)
#define USBOVR_P_CTRL		BIT(17)
#define MISC_USB		(MISC_REG_BASE + 0x64)

#define USB2_PHY_SET		0x60
#define USB_PHY_REG_ACCESS	(0x1 << 16)
static void hiusb_ohci_enable_clk(void)
{
	int reg;

	/* enable phy ref clk to enable phy */
	reg = readl(CRG_REG_BASE + REG_CRG46);
	reg |= USB_BUS_SRST_REQ;
	reg |= USB_UTMI0_SRST_REQ;
	reg |= USB_HST_PHY_SRST_REQ;
	reg |= USB_OTG_PHY_SRST_REQ;
	reg |= USB_PHY_REQ;
	reg |= USB_PHY_PORT0_TREQ;
	reg |= USB_PHY_TEST_SRST_REQ;
	writel(reg, CRG_REG_BASE + REG_CRG46);
	udelay(100);

	/* enable phy ref clk to enable phy */
	reg = readl(CRG_REG_BASE + REG_CRG46);
	reg |= USB_CKEN;
	reg |= USB_OHCI48M_CKEN;
	reg |= USB_OHCI12M_CKEN;
	reg |= USB_OTG_UTMI_CKEN;
	reg |= USB_HST_PHY_CKEN;
	reg |= USB_UTMI0_CKEN;
	reg |= USB_PHY_CKEN;
	writel(reg, CRG_REG_BASE + REG_CRG46);
	udelay(100);

	/* step1：release phy test reset */
	reg = readl(CRG_REG_BASE + REG_CRG46);
	reg &= ~USB_PHY_TEST_SRST_REQ;
	writel(reg, CRG_REG_BASE + REG_CRG46);
	udelay(100);

	/* step2：release phy por reset */
	reg = readl(CRG_REG_BASE + REG_CRG46);
	reg &= ~USB_PHY_REQ;
	writel(reg, CRG_REG_BASE + REG_CRG46);
	udelay(100);

	/* step3：config phy clk60m */
	reg = readl(MISC_REG_BASE + USB2_PHY_SET);
	reg |= USB_PHY_REG_ACCESS;
	writel(reg, MISC_REG_BASE + USB2_PHY_SET);
	writel(0x4, 0x120d0018);
	mdelay(2);
	writel(0x18, 0x120d0000);
	udelay(20);
	writel(0xc4, 0x120d0000 + 0x20);
	udelay(20);
	writel(0xc1, 0x120d0000 + 0x44);
	udelay(20);
	writel(0x1b, 0x120d0000 + 0x28);
	udelay(20);

	/* step4：release phy utmi reseti */
	reg = readl(CRG_REG_BASE + REG_CRG46);
	reg &= ~USB_PHY_PORT0_TREQ;
	writel(reg, CRG_REG_BASE + REG_CRG46);
	udelay(100);

	/* step5：release controller reset */
	reg = readl(CRG_REG_BASE + REG_CRG46);
	reg &= ~USB_BUS_SRST_REQ;
	reg &= ~USB_UTMI0_SRST_REQ;
	reg &= ~USB_HST_PHY_SRST_REQ;
	reg &= ~USB_OTG_PHY_SRST_REQ;
	writel(reg, CRG_REG_BASE + REG_CRG46);
	udelay(100);
	/* the end */
}

static void hiusb_ohci_disable_clk(void)
{
	int reg;

	/* If the HS PHY is unused disable it too. */
	reg = readl(CRG_REG_BASE + REG_CRG46);
	reg |= USB_BUS_SRST_REQ;
	reg |= USB_UTMI0_SRST_REQ;
	reg |= USB_HST_PHY_SRST_REQ;
	reg |= USB_OTG_PHY_SRST_REQ;
	reg |= USB_PHY_REQ;
	reg |= USB_PHY_PORT0_TREQ;
	reg |= USB_PHY_TEST_SRST_REQ;
	writel(reg, CRG_REG_BASE + REG_CRG46);
	udelay(100);
}

