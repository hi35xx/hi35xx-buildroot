#include <linux/init.h>
#include <linux/timer.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <asm/byteorder.h>
#include <linux/io.h>
#include <asm/system.h>
#include <asm/unaligned.h>

#define PERI_CRG46		(IO_ADDRESS(0x20030000 + 0xb8))
#define USB_CKEN		(1 << 7)
#define USB_CTRL_UTMI1_REG	(1 << 6)
#define USB_CTRL_UTMI0_REG	(1 << 5)
#define USB_CTRL_HUB_REG	(1 << 4)
#define USBPHY_PORT1_TREQ	(1 << 3)
#define USBPHY_PORT0_TREQ	(1 << 2)
#define USBPHY_REQ		(1 << 1)
#define USB_AHB_SRST_REQ	(1 << 0)
#define PERI_USB		(IO_ADDRESS(0x20050000 + 0x80))
#define WORDINTERFACE		(1 << 0)
#define	ULPI_BYPASS_EN		(1 << 3)
#define	SS_BURST4_EN		(1 << 7)
#define	SS_BURST8_EN		(1 << 8)
#define	SS_BURST16_EN		(1 << 9)
#define	USBOVR_P_CTRL		(1 << 17)
#define PERI1_USB		(IO_ADDRESS(0x20050000 + 0x84))
#define USBPHY_SHUTDOWN		(1 << 22)

static atomic_t dev_open_cnt = {
	.counter = 0,
};

void hiusb_start_hcd(void)
{
	unsigned long flags;

	int reg;

	local_irq_save(flags);

	/* USB PHY analog power on */
	reg = readl(PERI1_USB);
	reg &= ~(USBPHY_SHUTDOWN);
	writel(reg, PERI1_USB);

	if (atomic_add_return(1, &dev_open_cnt) == 1) {


		/* enable clock to EHCI block and HS PHY PLL*/
		reg = readl(PERI_CRG46);
		reg |= USB_CKEN;
		reg &= ~(USB_CTRL_UTMI0_REG);
		reg &= ~(USB_CTRL_HUB_REG);
		reg &= ~(USBPHY_PORT0_TREQ);
		reg &= ~(USBPHY_REQ);
		reg &= ~(USB_AHB_SRST_REQ);
		writel(reg, PERI_CRG46);
		udelay(100);

		/* enable phy */
		reg = readl(PERI_USB);
		reg |= ULPI_BYPASS_EN;
		reg &= ~(WORDINTERFACE);
		/* disable ehci burst16 mode*/
		reg &= ~(SS_BURST16_EN);
		reg &= ~(USBOVR_P_CTRL);
		writel(reg, PERI_USB);
		udelay(100);
	}
	local_irq_restore(flags);
}
EXPORT_SYMBOL(hiusb_start_hcd);

void hiusb_stop_hcd(void)
{
	unsigned long flags;
	int reg;

	local_irq_save(flags);
	if (atomic_sub_return(1, &dev_open_cnt) == 0) {


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

		/* disable phy */
		reg = readl(PERI_USB);
		reg &= ~ULPI_BYPASS_EN;
		reg |= (WORDINTERFACE);
		reg |= (SS_BURST16_EN);
		reg |= (USBOVR_P_CTRL);
		writel(reg, PERI_USB);
		udelay(100);

	}
	/* USB PHY analog power off */
	reg = readl(PERI1_USB);
	reg |= (USBPHY_SHUTDOWN);
	writel(reg, PERI1_USB);
	local_irq_restore(flags);
}
EXPORT_SYMBOL(hiusb_stop_hcd);
