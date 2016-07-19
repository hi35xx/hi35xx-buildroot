#define HIUSB_OHCI_BASE		0x10030000
#define HIUSB_OHCI_DEV_NAME	"hiusb-ohci"

#define PERI_CRG28		(CRG_REG_BASE + 0x70)
#define USB_CKEN		(1 << 7)
#define USB_CTRL_UTMI1_REQ	(1 << 6)
#define USB_CTRL_UTMI0_REQ	(1 << 5)
#define USB_CTRL_HUB_REQ	(1 << 4)
#define USBPHY_PORT1_TREQ	(1 << 3)
#define USBPHY_PORT0_TREQ	(1 << 2)
#define USBPHY_REQ		(1 << 1)
#define USB_AHB_SRST_REQ	(1 << 0)
#define PERI_USB		(0x12120050)
#define WORDINTERFACE		(1 << 0)
#define ULPI_BYPASS_EN		(1 << 3)
#define SS_BURST16_EN		(1 << 9)
#define USBOVR_P_CTRL		(1 << 17)
#define MISC_USB		(0x12120054)

static void hiusb_ohci_enable_clk(void)
{
	int reg;

	/* enable phy ref clk to enable phy */
	reg = readl(PERI_CRG28);
	reg |= USB_CKEN;
	writel(reg, PERI_CRG28);
	udelay(100);

	/* config controller */
	reg = readl(PERI_USB);
	reg &= ~(WORDINTERFACE); /* 8bit */
	/* disable ehci burst16 mode*/
	reg &= ~(SS_BURST16_EN);
	reg |= USBOVR_P_CTRL;
	writel(reg, PERI_USB);
	udelay(100);

	/* de-assert phy port */
	reg = readl(PERI_CRG28);
	reg &= ~(USBPHY_REQ);
	writel(reg, PERI_CRG28);
	udelay(300);

	writel(0x406, MISC_USB);
	udelay(10);
	writel(0x426, MISC_USB);
	mdelay(8);

	/* cancel phy utmi reset */
	reg = readl(PERI_CRG28);
	reg &= ~(USBPHY_PORT0_TREQ);
	reg &= ~(USBPHY_PORT1_TREQ);
	writel(reg, PERI_CRG28);
	udelay(300);

	/* de-assert all the rsts of ctrl */
	reg = readl(PERI_CRG28);
	reg &= ~(USB_CTRL_UTMI0_REQ);
	reg &= ~(USB_CTRL_UTMI1_REQ);
	reg &= ~(USB_CTRL_HUB_REQ);
	reg &= ~(USB_AHB_SRST_REQ);
	writel(reg, PERI_CRG28);
	udelay(200);
}

static void hiusb_ohci_disable_clk(void)
{
	int reg;

	/* Disable EHCI clock.
	If the HS PHY is unused disable it too. */

	reg = readl(PERI_CRG28);
	reg &= ~(USB_CKEN);
	reg |= (USB_CTRL_UTMI1_REQ);
	reg |= (USB_CTRL_UTMI0_REQ);
	reg |= (USB_CTRL_HUB_REQ);
	reg |= (USBPHY_PORT1_TREQ);
	reg |= (USBPHY_PORT0_TREQ);
	reg |= (USBPHY_REQ);
	reg |= (USB_AHB_SRST_REQ);
	writel(reg, PERI_CRG28);
	udelay(100);

	/* enable phy */
	reg = readl(PERI_USB);
	reg |= (WORDINTERFACE);
	reg |= (SS_BURST16_EN);
	reg |= (USBOVR_P_CTRL);
	writel(reg, PERI_USB);
	udelay(100);
}

