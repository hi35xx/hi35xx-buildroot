/*
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <linux/delay.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <mach/io.h>
#include <mach/platform.h>

unsigned int sata_port_map;

enum {
	HI_SATA_PERI_CTRL		= IO_ADDRESS(0x12040000),
	HI_SATA_PERI_CRG72		= (HI_SATA_PERI_CTRL + 0x120),
	HI_SATA_PERI_CRG74		= (HI_SATA_PERI_CTRL + 0x128),

	HI_SATA_PHY0_REFCLK_SEL_MASK = (0x3 << 4),
	HI_SATA_PHY0_REFCLK_SEL = (0x2 << 4),
	HI_SATA_PHY1_REFCLK_SEL_MASK = (0x3 << 6),
	HI_SATA_PHY1_REFCLK_SEL = (0x2 << 6),

	HI_SATA_PHY0_CLK_EN		= (1 << 0),
	HI_SATA_PHY1_CLK_EN		= (1 << 1),

	HI_SATA_PHY0_RST		= (1 << 2),
	HI_SATA_PHY1_RST		= (1 << 3),

	HI_SATA_PHY_BACK_MASK_ALL	= 0xf0,
	HI_SATA_PHY1_RST_BACK_MASK	= (1 << 5),
	HI_SATA_PHY0_RST_BACK_MASK	= (1 << 4),

	HI_SATA_BUS_CKEN		= (1 << 0),
	HI_SATA_BUS_SRST_REQ	= (1 << 8),
	HI_SATA_CKO_ALIVE_CKEN	= (1 << 2),
	HI_SATA_CKO_ALIVE_SRST_REQ	= (1 << 9),
	HI_SATA_RX0_CKEN		= (1 << 1),
	HI_SATA_TX0_CKEN		= (1 << 3),
	HI_SATA_RX0_SRST_REQ	= (1 << 10),
	HI_SATA0_SRST_REQ		= (1 << 11),
	HI_SATA_RX1_CKEN		= (1 << 12),
	HI_SATA_TX1_CKEN		= (1 << 13),
	HI_SATA_RX1_SRST_REQ	= (1 << 14),
	HI_SATA1_SRST_REQ		= (1 << 15),

	HI_SATA_SYS_CTRL		= IO_ADDRESS(0x1205008C),
};

static void hi_sata_poweron(void)
{
	/* msleep(20); */
}

static void hi_sata_poweroff(void)
{
}

static void hi_sata_reset(void)
{
	unsigned int tmp_val;

	tmp_val = readl((void *)HI_SATA_PERI_CRG74);

	tmp_val |= HI_SATA_BUS_SRST_REQ | HI_SATA_CKO_ALIVE_SRST_REQ;
	if (n_ports == 2) {
			tmp_val |= HI_SATA_RX0_SRST_REQ
					| HI_SATA0_SRST_REQ
					| HI_SATA_RX1_SRST_REQ
					| HI_SATA1_SRST_REQ;
	} else if (n_ports == 1) {
			tmp_val |= HI_SATA_RX0_SRST_REQ
					| HI_SATA0_SRST_REQ;
	}

	writel(tmp_val, (void *)HI_SATA_PERI_CRG74);
}

static void hi_sata_unreset(void)
{
	unsigned int tmp_val;

	tmp_val = readl((void *)HI_SATA_PERI_CRG74);

	tmp_val &= ~(HI_SATA_BUS_SRST_REQ | HI_SATA_CKO_ALIVE_SRST_REQ);
	if (n_ports == 2) {
			tmp_val &= ~(HI_SATA_RX0_SRST_REQ
					| HI_SATA0_SRST_REQ
					| HI_SATA_RX1_SRST_REQ
					| HI_SATA1_SRST_REQ);

	} else if (n_ports == 1) {
		tmp_val &= ~(HI_SATA_RX0_SRST_REQ
					| HI_SATA0_SRST_REQ);
	}

	writel(tmp_val, (void *)HI_SATA_PERI_CRG74);
}

static void hi_sata_phy_reset(void)
{
	unsigned int tmp_val;

	tmp_val = readl((void *)HI_SATA_PERI_CRG72);

	if (n_ports == 2) {
			tmp_val |= HI_SATA_PHY0_RST
					| HI_SATA_PHY1_RST;

	} else if (n_ports == 1)
		tmp_val |= HI_SATA_PHY0_RST;

	writel(tmp_val, (void *)HI_SATA_PERI_CRG72);
}

static void hi_sata_phy_unreset(void)
{
	unsigned int tmp_val;

	tmp_val = readl((void *)HI_SATA_PERI_CRG72);

	if (n_ports == 2) {
			tmp_val &= ~(HI_SATA_PHY0_RST
					| HI_SATA_PHY1_RST);
	} else if (n_ports == 1)
			tmp_val &= ~HI_SATA_PHY0_RST;

	writel(tmp_val, (void *)HI_SATA_PERI_CRG72);
}

static void hi_sata_clk_enable(void)
{
	unsigned int tmp_val, tmp_reg;

	tmp_val = readl((void *)HI_SATA_PERI_CRG72);
	tmp_reg = readl((void *)HI_SATA_PERI_CRG74);

	if (n_ports == 2) {
			tmp_val |= HI_SATA_PHY0_CLK_EN;
			tmp_val |= HI_SATA_PHY1_CLK_EN;

			tmp_reg |= HI_SATA_RX0_CKEN
					| HI_SATA_TX0_CKEN
					| HI_SATA_RX1_CKEN
					| HI_SATA_TX1_CKEN;

	} else if (n_ports == 1) {
		tmp_val |= HI_SATA_PHY0_CLK_EN;

		tmp_reg |= HI_SATA_RX0_CKEN
				| HI_SATA_TX0_CKEN;

	}

	writel(tmp_val, (void *)HI_SATA_PERI_CRG72);
	writel(tmp_reg, (void *)HI_SATA_PERI_CRG74);

}

static void hi_sata_clk_disable(void)
{
}

static void hi_sata_clk_reset(void)
{
}

static void hi_sata_phy_clk_sel(void)
{
	unsigned int tmp_val;

	tmp_val = readl((void *)HI_SATA_PERI_CRG72);

	if (n_ports == 2) {
			tmp_val &= ~HI_SATA_PHY0_REFCLK_SEL_MASK;
			tmp_val &= ~HI_SATA_PHY1_REFCLK_SEL_MASK;

			tmp_val |= HI_SATA_PHY0_REFCLK_SEL;
			tmp_val |= HI_SATA_PHY1_REFCLK_SEL;

	} else if (n_ports == 1) {
		tmp_val &= ~HI_SATA_PHY1_REFCLK_SEL_MASK;
		tmp_val |= HI_SATA_PHY1_REFCLK_SEL;
	}
	writel(tmp_val, (void *)HI_SATA_PERI_CRG72);
}

void hisata_v200_set_fifo(void *mmio, int n_ports)
{
	int i;

	for (i = 0; i < n_ports; i++)
		writel(HI_SATA_FIFOTH_VALUE, (mmio + 0x100 + i*0x80
					+ HI_SATA_PORT_FIFOTH));
}

void hisata_phy_init(void *mmio, int phy_mode, int n_ports)
{
	unsigned int tmp, phy_config = HI_SATA_PHY_3G;
	unsigned int phy_sg = HI_SATA_PHY_SG_3G;
	int i, port_no;

	if ((n_ports < 1) || (n_ports > 2))
		pr_err("ERROR: PORT num you set is WRONG!!!\n");

	sata_port_map = (1 << n_ports) - 1;
	hisata_v200_set_fifo(mmio, n_ports);

	tmp = readl(mmio + HI_SATA_PHY_CTL1);
	tmp |= HI_SATA_BIGENDINE;
	writel(tmp, (mmio + HI_SATA_PHY_CTL1));
	tmp = readl(mmio + HI_SATA_PHY_CTL2);
	tmp |= HI_SATA_BIGENDINE;
	writel(tmp, (mmio + HI_SATA_PHY_CTL2));

	tmp = readl(mmio + HI_SATA_PHY_RST_BACK_MASK);
	tmp |= HI_SATA_PHY_BACK_MASK_ALL;
	if (n_ports == 1)
		tmp &= ~HI_SATA_PHY1_RST_BACK_MASK;
	else if (n_ports == 2)
		/* Not need mask any port */

	writel(tmp, (mmio + HI_SATA_PHY_RST_BACK_MASK));

	if (phy_mode == HI_SATA_PHY_MODE_1_5G) {
		phy_config = HI_SATA_PHY_1_5G;
		phy_sg = HI_SATA_PHY_SG_1_5G;
	}

	if (phy_mode == HI_SATA_PHY_MODE_3G) {
		phy_config = HI_SATA_PHY_3G;
		phy_sg = HI_SATA_PHY_SG_3G;
	}

	if (phy_mode == HI_SATA_PHY_MODE_6G) {
		phy_config = HI_SATA_PHY_6G;
		phy_sg = HI_SATA_PHY_SG_6G;
	}

	for (i = 0; i < n_ports; i++) {
		port_no = i;
		writel(phy_config, (mmio + 0x100 + port_no*0x80
					+ HI_SATA_PORT_PHYCTL));

		writel(phy_sg, (mmio + 0x100 + port_no*0x80
					+ HI_SATA_PORT_PHYCTL1));
	}
}

static void hi_sata_set_phy_6G(void)
{
	unsigned int i, port_no;

	for (i = 0; i < n_ports; i++) {
		port_no = i;

		if (port_no == 0) {
			/* Phy(txpll and cdr) always work at 6Gbps */
			writel(0x501, (void *)HI_SATA_MISC_SATA_PHY0);
			writel(0x541, (void *)HI_SATA_MISC_SATA_PHY0);
			writel(0x501, (void *)HI_SATA_MISC_SATA_PHY0);
			writel(0x0, (void *)HI_SATA_MISC_SATA_PHY0);

			/* disable SSC */
			writel(0x803, (void *)HI_SATA_MISC_SATA_PHY0);
			writel(0x843, (void *)HI_SATA_MISC_SATA_PHY0);
			writel(0x808, (void *)HI_SATA_MISC_SATA_PHY0);
			writel(0x0, (void *)HI_SATA_MISC_SATA_PHY0);
		} else if (port_no == 1) {
			/* Phy(txpll and cdr) always work at 6Gbps */
			writel(0x501, (void *)HI_SATA_MISC_SATA_PHY1);
			writel(0x541, (void *)HI_SATA_MISC_SATA_PHY1);
			writel(0x501, (void *)HI_SATA_MISC_SATA_PHY1);
			writel(0x0, (void *)HI_SATA_MISC_SATA_PHY1);

			/* disable SSC */
			writel(0x803, (void *)HI_SATA_MISC_SATA_PHY1);
			writel(0x843, (void *)HI_SATA_MISC_SATA_PHY1);
			writel(0x808, (void *)HI_SATA_MISC_SATA_PHY1);
			writel(0x0, (void *)HI_SATA_MISC_SATA_PHY1);
		}
	}
}

