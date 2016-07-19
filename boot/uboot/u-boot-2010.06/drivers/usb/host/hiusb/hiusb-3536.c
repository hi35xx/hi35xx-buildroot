#define HIUSB_OHCI_BASE			0x10030000
#define HIUSB_OHCI_DEV_NAME		"hiusb-ohci"

#define PERI_CRG103			(0x12040000 + 0x19c)
#define USB2_BUS_CKEN			(1 << 0)
#define USB2_OHCI48M_CKEN		(1 << 1)
#define USB2_OHCI12M_CKEN		(1 << 2)
#define USB2_HST_PHY_CKEN		(1 << 4)
#define USB2_UTMI0_CKEN			(1 << 5)
#define USB2_UTMI1_CKEN			(1 << 6)
#define USB2_BUS_SRST_REQ		(1 << 12)
#define USB2_UTMI0_SRST_REQ		(1 << 13)
#define USB2_UTMI1_SRST_REQ		(1 << 14)
#define USB2_HST_PHY_SYST_REQ		(1 << 16)
#define USB2_OTG_PHY_SRST_REQ		(1 << 17)

#define PERI_CRG104			(0x12040000 + 0x1a0)
#define USB_PHY0_REF_CKEN		(1 << 0)
#define USB_PHY0_SRST_REQ		(1 << 8)
#define USB_PHY0_SRST_TREQ		(1 << 9)
#define USB_PHY1_SRST_TREQ		(1 << 11)
#define USB_PHY0_REFCLK_SEL		(1 << 16)

#define PERI_USB0			(0x12120000 + 0x7c)
#define WORDINTERFACE			(1 << 0)
#define ULPI_BYPASS_EN_PORT0		(1 << 3)
#define SS_BURST4_EN			(1 << 7)
#define SS_BURST8_EN			(1 << 8)
#define SS_BURST16_EN			(1 << 9)

#define PERI_USB1			(0x12120000 + 0x80)
/* write(0x1 << 5) 0xc to addr 0x3 */
#define CONFIG_CLK			((0x1 << 5) | (0x3 << 1) | (0xc << 8))

static void hiusb_ohci_enable_clk(void) {
	int reg;

	/* reset enable */
	reg = readl(PERI_CRG103);
	reg |= (USB2_BUS_SRST_REQ
			| USB2_UTMI0_SRST_REQ
			| USB2_UTMI1_SRST_REQ
			| USB2_HST_PHY_SYST_REQ
			| USB2_OTG_PHY_SRST_REQ);

	writel(reg, PERI_CRG103);
	udelay(200);

	reg = readl(PERI_CRG104);
	reg |= (USB_PHY0_SRST_REQ
			| USB_PHY0_SRST_TREQ
			| USB_PHY1_SRST_TREQ);
	writel(reg, PERI_CRG104);
	udelay(200);

	reg = readl(PERI_USB0);
	reg |= ULPI_BYPASS_EN_PORT0;	/* 3 ports utmi */
	reg &= ~(WORDINTERFACE);	/* 8bit */
	reg &= ~(SS_BURST16_EN);	/* 16 bit burst disable */
	writel(reg, PERI_USB0);
	udelay(100);

	/* for ssk usb storage ok */
	mdelay(10);

	/* open ref clock */
	reg = readl(PERI_CRG104);
	reg |= (USB_PHY0_REF_CKEN);
	writel(reg, PERI_CRG104);
	udelay(100);

	/* cancel power on reset */
	reg = readl(PERI_CRG104);
	reg &= ~(USB_PHY0_SRST_REQ);
	writel(reg , PERI_CRG104);
	udelay(300);

	/* config clock */
	reg = readl(PERI_USB1);
	reg |= CONFIG_CLK;
	writel(reg, PERI_USB1);
	udelay(100);

	writel(0x0, PERI_USB1);
	mdelay(2);

	/* cancel port reset */
	reg = readl(PERI_CRG104);
	reg &= ~(USB_PHY0_SRST_TREQ
		| USB_PHY1_SRST_TREQ);
	writel(reg, PERI_CRG104);
	udelay(300);

	/* cancel control reset */
	reg = readl(PERI_CRG103);
	reg &= ~(USB2_BUS_SRST_REQ
		| USB2_UTMI0_SRST_REQ
		| USB2_UTMI1_SRST_REQ
		| USB2_HST_PHY_SYST_REQ
		| USB2_OTG_PHY_SRST_REQ);

	reg |= (USB2_BUS_CKEN
		| USB2_OHCI48M_CKEN
		| USB2_OHCI12M_CKEN
		| USB2_HST_PHY_CKEN
		| USB2_UTMI0_CKEN
		| USB2_UTMI1_CKEN);
	writel(reg, PERI_CRG103);
	udelay(200);
}

static void hiusb_ohci_disable_clk(void) {
	int reg;

	reg = readl(PERI_CRG104);
	reg |= (USB_PHY0_SRST_REQ
		| USB_PHY0_SRST_TREQ
		| USB_PHY1_SRST_TREQ);
	writel(reg, PERI_CRG104);
	udelay(100);

	/* close clock */
	reg = readl(PERI_CRG104);
	reg &= ~(USB_PHY0_REFCLK_SEL
		| USB_PHY0_REF_CKEN);
	writel(reg, PERI_CRG104);
	udelay(300);

	/* close clock  */
	reg = readl(PERI_CRG103);
	reg &= ~(USB2_BUS_CKEN
		| USB2_OHCI48M_CKEN
		| USB2_OHCI12M_CKEN
		| USB2_HST_PHY_CKEN
		| USB2_UTMI0_CKEN
		| USB2_UTMI1_CKEN);
	writel(reg, PERI_CRG103);
	udelay(200);
}
