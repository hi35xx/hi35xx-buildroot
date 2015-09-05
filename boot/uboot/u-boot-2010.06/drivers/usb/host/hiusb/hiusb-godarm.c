#define HIUSB_OHCI_BASE		0x100a0000
#define HIUSB_OHCI_DEV_NAME	"hiusb-ohci"

#define PERI_CRG46		(CRG_REG_BASE + 0xb8)
#define USB_CKEN		(1 << 7)
#define USB_CTRL_UTMI1_REG	(1 << 6)
#define USB_CTRL_UTMI0_REG	(1 << 5)
#define USB_CTRL_HUB_REG	(1 << 4)
#define USBPHY_PORT1_TREQ	(1 << 3)
#define USBPHY_PORT0_TREQ	(1 << 2)
#define USBPHY_REQ		(1 << 1)
#define USB_AHB_SRST_REQ	(1 << 0)
#define PERI_USB		(SYS_CTRL_REG_BASE + 0x80)
#define WORDINTERFACE		(1 << 0)
#define ULPI_BYPASS_EN		(1 << 3)
#define SS_BURST16_EN		(1 << 9)
#define USBOVR_P_CTRL		(1 << 17)

static void hiusb_ohci_enable_clk(void)
{
	int reg;

	/* enable clock to EHCI block and HS PHY PLL */
	reg = readl(PERI_CRG46);
	reg |= USB_CKEN;
	reg &= ~(USB_CTRL_UTMI1_REG);
	reg &= ~(USB_CTRL_UTMI0_REG);
	reg &= ~(USB_CTRL_HUB_REG);
	reg &= ~(USBPHY_PORT1_TREQ);
	reg &= ~(USBPHY_PORT0_TREQ);
	reg &= ~(USBPHY_REQ);
	reg &= ~(USB_AHB_SRST_REQ);
	writel(reg, PERI_CRG46);
	udelay(10);
	/* enable phy */
	reg = readl(PERI_USB);
	reg |= ULPI_BYPASS_EN;
	reg &= ~(WORDINTERFACE);
	/* disable ehci burst16 mode */
	reg &= ~(SS_BURST16_EN);
	reg &= ~(USBOVR_P_CTRL);
	writel(reg, PERI_USB);
	udelay(10);
}

static void hiusb_ohci_disable_clk(void)
{
}

