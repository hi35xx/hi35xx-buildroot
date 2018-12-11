#include <linux/phy.h>
#include <linux/micrel_phy.h>
#include "../higmac.h"
#include "autoeee.h"

void init_autoeee(struct higmac_netdev_local *ld)
{
	int phy_id = ld->phy->phy_id;
	struct phy_info *phy_info;

	if (ld->eee_init)
		goto eee_init;

	phy_info = phy_search_ids(phy_id);
	if (phy_info) {
		int eee_available, lp_eee_capable, v;
		u32 link_stat = 0;

		eee_available = phy_info->eee_available;
		if (netif_msg_wol(ld))
			pr_info("fit phy_id:0x%x, phy_name:%s, eee:%d\n",
				phy_info->phy_id, phy_info->name,
				eee_available);

		if (!eee_available)
			goto not_support;

		if (eee_available == PHY_EEE) {
			if (netif_msg_wol(ld))
				pr_info("enter phy-EEE mode\n");

			v = readl(ld->gmac_iobase + EEE_ENABLE);
			v &= ~BIT_EEE_ENABLE;	/* disable auto-EEE */
			writel(v, ld->gmac_iobase + EEE_ENABLE);
			return;
		}

		ld->eee_init = phy_info->eee_init;
eee_init:
		switch (ld->phy->speed) {
		case SPEED_10:
			link_stat |= HIGMAC_SPD_10M;
			break;
		case SPEED_100:
			link_stat |= HIGMAC_SPD_100M;
			break;
		case SPEED_1000:
			link_stat |= HIGMAC_SPD_1000M;
			break;
		default:
			break;
		}

		lp_eee_capable = ld->eee_init(ld->phy);
		if (lp_eee_capable < 0)
			return;

		if (ld->phy->link) {
			if (((u32)lp_eee_capable) & link_stat) {
				if ((phy_id & REALTEK_PHY_MASK) ==
				    REALTEK_PHY_ID_8211E) {
					v = readl(ld->gmac_iobase + EEE_CLK);
					v &= ~MASK_EEE_CLK;
					v |= BIT_DISABLE_TX_CLK;
					writel(v, ld->gmac_iobase + EEE_CLK);
				} else if ((phy_id & MICREL_PHY_ID_MASK) ==
					   PHY_ID_KSZ9031) {
					v = readl(ld->gmac_iobase + EEE_CLK);
					v &= ~MASK_EEE_CLK;
					v |= (BIT_DISABLE_TX_CLK |
						BIT_PHY_KSZ9031);
					writel(v, ld->gmac_iobase + EEE_CLK);
				}

				/* EEE_1us: 0x7c for 125M */
				writel(0x7c, ld->gmac_iobase +
				       EEE_TIME_CLK_CNT);
				writel(0x1e0400, ld->gmac_iobase +
				       EEE_TIMER);/* FIXME */

				v = readl(ld->gmac_iobase + EEE_LINK_STATUS);
				v |= 0x3 << 1;	/* auto EEE and ... */
				v |= BIT_PHY_LINK_STATUS;	/* phy linkup */
				writel(v, ld->gmac_iobase + EEE_LINK_STATUS);

				v = readl(ld->gmac_iobase + EEE_ENABLE);
				v |= BIT_EEE_ENABLE;	/* enable EEE */
				writel(v, ld->gmac_iobase + EEE_ENABLE);

				if (netif_msg_wol(ld))
					pr_info("enter auto-EEE mode\n");
			} else {
				if (netif_msg_wol(ld))
					pr_info("link partner not support EEE\n");
			}
		} else {
			v = readl(ld->gmac_iobase + EEE_LINK_STATUS);
			v &= ~(BIT_PHY_LINK_STATUS);	/* phy linkdown */
			writel(v, ld->gmac_iobase + EEE_LINK_STATUS);
		}

		return;
	}

not_support:
	ld->eee_init = NULL;
	if (netif_msg_wol(ld))
		pr_info("non-EEE mode\n");
}

void eee_phy_linkdown(struct higmac_netdev_local *ld)
{
	int v = readl(ld->gmac_iobase + EEE_LINK_STATUS);
	/* update phy link state */
	v &= ~BIT_PHY_LINK_STATUS;
	writel(v, ld->gmac_iobase + EEE_LINK_STATUS);
}

void eee_phy_linkup(struct higmac_netdev_local *ld)
{
	int v = readl(ld->gmac_iobase + EEE_LINK_STATUS);
	/* update phy link state */
	v |= BIT_PHY_LINK_STATUS;
	writel(v, ld->gmac_iobase + EEE_LINK_STATUS);
}
