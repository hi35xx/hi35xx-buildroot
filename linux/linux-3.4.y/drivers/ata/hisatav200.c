#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/io.h>

#include "hisatav200.h"

void hisata_v200_set_fifo(void *mmio, int n_ports)
{
	int i;

	for (i = 0; i < n_ports; i++)
		writel(HI_SATA_FIFOTH_VALUE, (mmio + 0x100 + i*0x80
					+ HI_SATA_PORT_FIFOTH));
}

void hisata_v200_init(void *mmio, int phy_mode, int n_ports)
{
	unsigned int tmp, phy_config;
	int i;

	hisata_v200_set_fifo(mmio, n_ports);

	if ((n_ports < 1) || (n_ports > 3))
		printk(KERN_ERR "ERROR: PORT num you set is WRONG!!!\n");

	writel(HI_SATA_PHY_VALUE, (mmio + HI_SATA_PHY0_CTLL));
	writel(HI_SATA_PHY_VALUE, (mmio + HI_SATA_PHY1_CTLL));
	if (n_ports == 3)
		writel(HI_SATA_PHY_VALUE, (mmio + HI_SATA_PHY2_CTLL));

	for (i = 0; i < n_ports; i++)
		writel(HI_SATA_PHYCTL2_VALUE, (mmio + 0x100 + i*0x80
						+ HI_SATA_PORT_PHYCTL2));

	tmp = readl(mmio + HI_SATA_PHY0_CTLL);
	tmp |= HI_SATA_PHY_REV_CLK;
	writel(tmp, (mmio + HI_SATA_PHY0_CTLL));
	tmp = readl(mmio + HI_SATA_PHY1_CTLL);
	tmp |= HI_SATA_PHY_REV_CLK;
	writel(tmp, (mmio + HI_SATA_PHY1_CTLL));
	if (n_ports == 3) {
		tmp = readl(mmio + HI_SATA_PHY2_CTLL);
		tmp |= HI_SATA_PHY_REV_CLK;
		writel(tmp, (mmio + HI_SATA_PHY2_CTLL));
	}

	for (i = 0; i < n_ports; i++) {
		tmp = readl(mmio + 0x100 + i*0x80
				+ HI_SATA_PORT_PHYCTL2);
		tmp &= ~HI_SATA_LANE0_RESET;
		writel(tmp, (mmio + 0x100 + i*0x80
				+ HI_SATA_PORT_PHYCTL2));
	}

	tmp = readl(mmio + HI_SATA_PHY0_CTLL);
	tmp &= ~HI_SATA_PHY_RESET;
	writel(tmp, (mmio + HI_SATA_PHY0_CTLL));
	tmp = readl(mmio + HI_SATA_PHY1_CTLL);
	tmp &= ~HI_SATA_PHY_RESET;
	writel(tmp, (mmio + HI_SATA_PHY1_CTLL));
	if (n_ports == 3) {
		tmp = readl(mmio + HI_SATA_PHY2_CTLL);
		tmp &= ~HI_SATA_PHY_RESET;
		writel(tmp, (mmio + HI_SATA_PHY2_CTLL));
	}

	tmp = readl(mmio + HI_SATA_PHY0_CTLH);
	tmp |= HI_SATA_BIGENDINE;
	writel(tmp, (mmio + HI_SATA_PHY0_CTLH));
	tmp = readl(mmio + HI_SATA_PHY1_CTLH);
	tmp |= HI_SATA_BIGENDINE;
	writel(tmp, (mmio + HI_SATA_PHY1_CTLH));
	if (n_ports == 3) {
		tmp = readl(mmio + HI_SATA_PHY2_CTLH);
		tmp |= HI_SATA_BIGENDINE;
		writel(tmp, (mmio + HI_SATA_PHY2_CTLH));
	}

	for (i = 0; i < n_ports; i++) {
		tmp = HI_SATA_PX_TX_AMPLITUDE;
		writel(tmp, (mmio + 0x100 + i*0x80
				+ HI_SATA_PORT_PHYCTL1));
	}

	for (i = 0; i < n_ports; i++) {
		tmp = HI_SATA_PX_TX_PREEMPH;
		writel(tmp, (mmio + 0x100 + i*0x80
				+ HI_SATA_PORT_PHYCTL2));
	}

	for (i = 0; i < n_ports; i++)
		writel(HI_SATA_FORCE_1_5G, (mmio + 0x100 + i*0x80
					+ HI_SATA_PORT_PHYCTL));
	for (i = 0; i < n_ports; i++)
		writel(HI_SATA_FORCE_3G, (mmio + 0x100 + i*0x80
					+ HI_SATA_PORT_PHYCTL));
	for (i = 0; i < n_ports; i++)
		writel(HI_SATA_FORCE_6G, (mmio + 0x100 + i*0x80
					+ HI_SATA_PORT_PHYCTL));

	if (phy_mode == HI_SATA_PHY_MODE_1_5G)
		phy_config = HI_SATA_PHY_1_5G;
	if (phy_mode == HI_SATA_PHY_MODE_3G)
		phy_config = HI_SATA_PHY_3G;
	if (phy_mode == HI_SATA_PHY_MODE_6G)
		phy_config = HI_SATA_PHY_6G;

	for (i = 0; i < n_ports; i++)
		writel(phy_config, (mmio + 0x100 + i*0x80
					+ HI_SATA_PORT_PHYCTL));
}
