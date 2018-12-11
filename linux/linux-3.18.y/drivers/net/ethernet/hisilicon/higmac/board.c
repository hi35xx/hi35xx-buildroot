#include <linux/kernel.h>
#include <linux/reset.h>
#include <linux/clk-provider.h>
#include "higmac.h"

void higmac_mac_core_reset(struct higmac_netdev_local *priv)
{
	/* undo reset */
	reset_control_deassert(priv->port_rst);
	usleep_range(50, 60);

	/* soft reset mac port */
	reset_control_assert(priv->port_rst);
	usleep_range(50, 60);
	/* undo reset */
	reset_control_deassert(priv->port_rst);
}

void higmac_hw_internal_phy_reset(struct higmac_netdev_local *priv)
{
}

void higmac_hw_phy_reset(struct higmac_netdev_local *priv)
{
	if (priv->internal_phy)
		higmac_hw_internal_phy_reset(priv);
	else
		higmac_hw_external_phy_reset(priv);
}

void higmac_hw_external_phy_reset(struct higmac_netdev_local *priv)
{
	if (priv->phy_rst) {
		/* write 0 to cancel reset */
		reset_control_deassert(priv->phy_rst);
		msleep(50);

		/* HIFONE or 98cv200 use CRG register to reset phy */
		/* RST_BIT, write 0 to reset phy, write 1 to cancel reset */
		reset_control_assert(priv->phy_rst);

		/* delay some time to ensure reset ok,
		 * this depends on PHY hardware feature
		 */
		msleep(50);

		/* write 0 to cancel reset */
		reset_control_deassert(priv->phy_rst);
		/* delay some time to ensure later MDIO access */
		msleep(50);
	}
}

void higmac_internal_phy_clk_disable(struct higmac_netdev_local *priv)
{
}

void higmac_internal_phy_clk_enable(struct higmac_netdev_local *priv)
{
}

void higmac_hw_all_clk_disable(struct higmac_netdev_local *priv)
{
	/* If macif clock is enabled when suspend, we should
	 * disable it here.
	 * Because when resume, PHY will link up again and
	 * macif clock will be enabled too. If we don't disable
	 * macif clock in suspend, macif clock will be enabled twice.
	 */
	if (priv->netdev->flags & IFF_UP)
		clk_disable_unprepare(priv->macif_clk);

	/* This is called in suspend, when net device is down,
	 * MAC clk is disabled.
	 * So we need to judge whether MAC clk is enabled,
	 * otherwise kernel will WARNING if clk disable twice.
	 */
	if (priv->netdev->flags & IFF_UP)
		clk_disable_unprepare(priv->clk);

	if (priv->internal_phy)
		higmac_internal_phy_clk_disable(priv);
}

void higmac_hw_all_clk_enable(struct higmac_netdev_local *priv)
{
	if (priv->internal_phy)
		higmac_internal_phy_clk_enable(priv);

	if (priv->netdev->flags & IFF_UP)
		clk_prepare_enable(priv->macif_clk);

	/* If net device is down when suspend, we should not enable MAC clk. */
	if (priv->netdev->flags & IFF_UP)
		clk_prepare_enable(priv->clk);
}
