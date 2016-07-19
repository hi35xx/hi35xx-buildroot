#define HIUSB_OHCI_BASE		0x100a0000
#define HIUSB_OHCI_DEV_NAME	"hiusb-ohci"

#define PERI_CRG46		(CRG_REG_BASE + 0xb8)
#define USB_CKEN		(1 << 7)
#define USB_CTRL_UTMI0_REG	(1 << 5)
#define USB_CTRL_HUB_REG	(1 << 4)
#define USBPHY_PORT0_TREQ	(1 << 2)
#define USBPHY_REQ		(1 << 1)
#define USB_AHB_SRST_REQ	(1 << 0)

#define MISC_CTRL		0x20120000
#define PERI_USB		(MISC_CTRL + 0x78)
#define WORDINTERFACE		(1 << 0)
#define SS_BURST4_EN		(1 << 7)
#define SS_BURST8_EN		(1 << 8)
#define SS_BURST16_EN		(1 << 9)
#define USBOVR_P_CTRL		(1 << 17)
#define MISC_USB                (MISC_CTRL + 0x80)
static void hiusb_ohci_enable_clk(void)
{
	int reg;

	/* enable phy ref clk to enable phy */
	reg = readl(PERI_CRG46);
	reg |= USB_CKEN;
	writel(reg, PERI_CRG46);
	udelay(100);

	/* config controller */
	reg = readl(PERI_USB);
	reg &= ~(WORDINTERFACE); /* 8bit */
	/* disable ehci burst16 mode */
	reg &= ~(SS_BURST16_EN);
	reg |= USBOVR_P_CTRL;
	writel(reg, PERI_USB);
	udelay(100);

	/* de-assert phy port */
	reg = readl(PERI_CRG46);
	reg &= ~(USBPHY_REQ);
	writel(reg, PERI_CRG46);
	udelay(100);

	/* open phy clk */
	writel(0xc06, MISC_USB);
	udelay(10);
	writel(0xc26, MISC_USB);
	mdelay(5);

	/* usb2.0 phy eye pattern */
	writel(0x1c00, MISC_USB);
	udelay(10);
	writel(0x1c20, MISC_USB);
	mdelay(5);

	writel(0x0c09, MISC_USB);
	udelay(10);
	writel(0x0c29, MISC_USB);
	mdelay(5);

	writel(0x1a0a, MISC_USB);
	udelay(10);
	writel(0x1a2a, MISC_USB);
	mdelay(5);

	/* cancel phy utmi reset */
	reg = readl(PERI_CRG46);
	reg &= ~(USBPHY_PORT0_TREQ);
	writel(reg, PERI_CRG46);
	udelay(300);

	/* de-assert all the rsts of ctrl */
	reg = readl(PERI_CRG46);
	reg &= ~(USB_CTRL_UTMI0_REG);
	reg &= ~(USB_CTRL_HUB_REG);
	reg &= ~(USB_AHB_SRST_REQ);
	writel(reg, PERI_CRG46);
	udelay(200);

	/* decrease the threshold value from 650 to 550*/
	writel(0xa, MISC_USB);
	udelay(10);
	writel(0x092a, MISC_USB);
	mdelay(5);

}

static void hiusb_ohci_disable_clk(void)
{
	int reg;

	/* Disable EHCI clock.
	   If the HS PHY is unused disable it too. */

	reg = readl(PERI_CRG46);
	reg &= ~(USB_CKEN);
	reg |= (USB_CTRL_UTMI0_REG);
	reg |= (USB_CTRL_HUB_REG);
	reg |= (USBPHY_PORT0_TREQ);
	reg |= (USBPHY_REQ);
	reg |= (USB_AHB_SRST_REQ);
	writel(reg, PERI_CRG46);
	udelay(100);

	/* enable phy */
	reg = readl(PERI_USB);
	reg |= (WORDINTERFACE);
	reg |= (SS_BURST16_EN);
	reg |= (USBOVR_P_CTRL);
	writel(reg, PERI_USB);
	udelay(100);
}

