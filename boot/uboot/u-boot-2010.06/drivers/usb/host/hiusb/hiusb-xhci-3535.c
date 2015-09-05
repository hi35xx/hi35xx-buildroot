/*XHCI for Hi3535*/
#define HIUSB_XHCI_BASE        0x12000000
#define PERI_CRG46            (0x20030000 + 0xb8)
#define MISC_CTRL_33          (0x20120000 + 0x84)
#define MISC_CTRL_37          (0x20120000 + 0x94)
#define MISC_CTRL_38          (0x20120000 + 0x98)
#define USB3_VCC_SRST_REQ     (1<<12)
#define USB3_VAUX_SRST_REQ    (1<<27)
#define USB3_PHY_SRST_REQ       (1 << 13)
#define USB3_PHY_SRST_TREQ      (1 << 14)

#define GTXTHRCFG             0xc108
#define GRXTHRCFG             0xc10c
#define REG_GCTL              0xc110
#define BIT_SOFT_RESET        (0x1 << 11)
#define BIT_PORT_DIR_HOST     ((~(0x1 << 13))|(0x1 << 12))

#define REG_GUSB3_PIPECTL     0xC2C0
#define BIT_PHY_SOFT_RESET    (0x1 << 31)
#define BIT_TX_MARGIN0        (0x1 << 3)

#define REG_GUSB2PHYCFG0         0xC200
#define BIT_UTMI_ULPI         (0x1 << 4)
#define BIT_UTMI_8_16         (0x1 << 3)

#define REG_GUSB3PIPECTL0       0xc2c0
#define PCS_SSP_SOFT_RESET      (0x1 << 31)

#define MUXCTRL_REG59           (0x200f00ec)
#define MUXCTRL_REG60           (0x200f00f0)

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

	/* step 1: USB3.0/2.0 CRG */
	reg = readl(PERI_CRG46);
	reg &= 0xff;
	reg |= (0x0ff48000);
	writel(reg, PERI_CRG46);
	wait_ms(200);

	/* de-assert usb3_vcc_srst_req */
	reg = readl(PERI_CRG46);
	reg &= ~(USB3_VCC_SRST_REQ);
	writel(reg, PERI_CRG46);
	wait_ms(100);

	reg = readl(HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);
	reg |= PCS_SSP_SOFT_RESET;
	writel(reg, HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);

	/*step 3: USB2 PHY chose ulpi 8bit interface */
	reg = readl(HIUSB_XHCI_BASE + REG_GUSB2PHYCFG0);
	reg &= ~BIT_UTMI_ULPI;
	reg &= ~(BIT_UTMI_8_16);
	/* disable phy suspend */
	reg &= ~(1<<6);
	writel(reg, HIUSB_XHCI_BASE + REG_GUSB2PHYCFG0);
	wait_ms(20);

	reg = readl(HIUSB_XHCI_BASE + REG_GCTL);
	reg &= ~(0x3<<12);
	reg |= (0x1<<12); /*[13:12] 01: Host; 10: Device; 11: OTG*/
	writel(reg, HIUSB_XHCI_BASE + REG_GCTL);

	/* de-assert usb3phy hard-macro por */
	reg = readl(PERI_CRG46);
	reg &= ~USB3_PHY_SRST_REQ;
	writel(reg, PERI_CRG46);
	wait_ms(100);

	reg = readl(PERI_CRG46);
	/* de-assert hsp hard-macro port reset */
	reg &= ~USB3_PHY_SRST_TREQ;
	writel(reg, PERI_CRG46);

	reg = readl(HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);
	reg &= ~PCS_SSP_SOFT_RESET;
	writel(reg, HIUSB_XHCI_BASE + REG_GUSB3PIPECTL0);
	wait_ms(100);

	wait_ms(20);

	reg = readl(MISC_CTRL_33);
	/* set the irq signal level type */
	reg &= ~(1 << 12);
	writel(reg, MISC_CTRL_33);
	wait_ms(100);

	/* config eye chart */
	writel(0x000040f0, MISC_CTRL_37);
	writel(0x00001002, MISC_CTRL_38);
	writel(0x100040f0, MISC_CTRL_37);
	writel(0x000040f0, MISC_CTRL_37);
	writel(0x00004aff, MISC_CTRL_38);
	writel(0x200040f0, MISC_CTRL_37);
	writel(0x000040f0, MISC_CTRL_37);
	writel(0x800040f0, MISC_CTRL_37);
	writel(0x000040f0, MISC_CTRL_37);

	/* config pin multiplexing */
	writel(0x1, MUXCTRL_REG59);
	writel(0x1, MUXCTRL_REG60);

}
static void hiusb_stop_hcd(void)
{
	unsigned int reg;

	reg = readl(PERI_CRG46);
	writel(reg | (USB3_VCC_SRST_REQ), PERI_CRG46);
	wait_ms(500);
}

