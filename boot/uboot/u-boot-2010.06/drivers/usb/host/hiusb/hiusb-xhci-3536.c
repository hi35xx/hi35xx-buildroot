/*XHCI for Hi3536*/
#define HIUSB_XHCI_BASE		0x11040000
#define PERI_CRG46		(0x12040000 + 0xb8)
#define USB3_VCC_SRST_REQ2	(1 << 13)
#define USB3_PHY_SRST_REQ2	(1 << 3)
#define USB3_PHY_REF_CKEN	(1 << 0)

#define GTXTHRCFG		0xC108
#define USB_TXPKTCNT_SEL	(1 << 29)
#define USB_TXPKTCNT		(3 << 24)
#define USB_MAX_TXBUTST_SIZE	(0x10 << 16)
#define GRXTHRCFG		0xC10C
#define USB_RXPKTCNT_SEL	(1 << 29)
#define USB_RXPKTCNT		(3 << 24)
#define USB_MAX_RXBUTST_SIZE	(0x10 << 16)

#define REG_GCTL		0xC110
#define PRTCAPDIR_HOST		(1 << 12)
#define PRTCAPDIR_MASK		((1 << 12) | (1 << 13))

#define USB3_UTMI_CKEN		(1 << 12)
#define USB3_PIPE_CKEN		(1 << 11)
#define USB3_SUSPEND_CKEN	(1 << 10)
#define USB3_REF_CKEN		(1 << 9)
#define USB3_BUS_CKEN		(1 << 8)

#define REG_GUSB2PHYCFG0	0xC200
#define BIT_UTMI_ULPI		(1 << 4)
#define BIT_UTMI_8_16		(1 << 3)

#define REG_GUSB3PIPECTL0	0xC2C0
#define PCS_SSP_SOFT_RESET	(1 << 31)
#define USB3_PHY_SUSPEND_EN	(1 << 17)

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	*hccr = (struct xhci_hccr *)(HIUSB_XHCI_BASE);
	*hcor = (struct xhci_hcor *)((uint32_t) *hccr
			+ HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

static void hiusb_start_hcd(void)
{
	unsigned int reg;

	/* enable u2 1p phy reset  */
	writel(0x301, 0x12040194);
	wait_ms(1);
	/* cancel u2 1p phy POR */
	writel(0x201, (0x12040194));
	wait_ms(1);
	/* config u2 1p phy clock */
	writel(0x0c26, (0x12120088));
	wait_ms(1);
	writel(0x0, (0x12120088));
	wait_ms(5);
	/* cancel u2 1p phy utmi reset */
	writel(0x1, (0x12040194));
	wait_ms(1);

	/* open COMBPHY ref clk */
	reg = readl(PERI_CRG46);
	reg |= USB3_PHY_REF_CKEN;
	writel(reg, PERI_CRG46);
	wait_ms(100);
	/* cancel u3 combphy POR */
	reg = readl(PERI_CRG46);
	reg |= USB3_PHY_SRST_REQ2;
	writel(reg, PERI_CRG46);
	wait_ms(100);

	/* de-assert usb3_vcc_srst_req , default (0x3f09) */
	reg = readl(PERI_CRG46);
	reg |= USB3_VCC_SRST_REQ2;
	reg |= USB3_UTMI_CKEN;
	reg |= USB3_PIPE_CKEN;
	reg |= USB3_SUSPEND_CKEN;
	reg |= USB3_REF_CKEN;
	reg |= USB3_BUS_CKEN;
	reg |= USB3_PHY_SRST_REQ2;
	reg |= USB3_PHY_REF_CKEN;
	writel(reg, PERI_CRG46);
	wait_ms(100);

	reg = readl(PERI_CRG46);
	reg &= ~(USB3_VCC_SRST_REQ2);
	writel(reg, PERI_CRG46);
	wait_ms(100);

	/* USB3 soft reset */
	reg = readl(HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);
	reg |= PCS_SSP_SOFT_RESET;
	writel(reg, HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);

	/*step 3: USB2 PHY chose utmi 8bit interface */
	reg = readl(HIUSB_XHCI_BASE + REG_GUSB2PHYCFG0);
	reg &= ~BIT_UTMI_ULPI;
	reg &= ~(BIT_UTMI_8_16);
	writel(reg, HIUSB_XHCI_BASE + REG_GUSB2PHYCFG0);
	wait_ms(20);

	/* set host mode. [13:12] 01: Host; 10: Device; 11: OTG */
	reg = readl(HIUSB_XHCI_BASE + REG_GCTL);
	reg &= ~PRTCAPDIR_MASK;
	reg |= PRTCAPDIR_HOST;
	writel(reg, HIUSB_XHCI_BASE + REG_GCTL);

	/* de-assert usb3phy hard-macro por */
	reg = readl(PERI_CRG46);
	reg &= ~USB3_PHY_SRST_REQ2;
	writel(reg, PERI_CRG46);
	wait_ms(100);

	reg = readl(HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);
	reg &= ~PCS_SSP_SOFT_RESET;
	reg &= ~USB3_PHY_SUSPEND_EN;	/* disable suspend */
	writel(reg, HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);
	wait_ms(100);

	reg = USB_TXPKTCNT_SEL | USB_TXPKTCNT | USB_MAX_TXBUTST_SIZE;
	writel(reg, HIUSB_XHCI_BASE + GTXTHRCFG);

	reg = USB_RXPKTCNT_SEL | USB_RXPKTCNT | USB_MAX_RXBUTST_SIZE;
	writel(reg, HIUSB_XHCI_BASE + GRXTHRCFG);
	wait_ms(20);

}
static void hiusb_stop_hcd(void)
{
	unsigned int reg;

	reg = readl(PERI_CRG46);
	writel(reg | (USB3_VCC_SRST_REQ2), PERI_CRG46);
	wait_ms(500);
}

