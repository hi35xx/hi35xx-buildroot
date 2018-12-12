/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
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

#ifdef CONFIG_NET_HISFV300_HI3516CV300

#define HIETH_CRG_REG		(CRG_REG_BASE + REG_ETH_CRG)
#define ETH_SOFT_RESET		(1 << 0)
#define ETH_CLK_ENABLE		(1 << 1)
#define ETH_PHY_RESET		(1 << 3)
#define ETH_CORE_CLK_SELECT_54M	(1 << 7)

static void hieth_reset(int rst)
{
	u32 val;

	val = _readl(HIETH_CRG_REG);
	if (rst)
		val |= ETH_SOFT_RESET;
	else
		val &= ~ETH_SOFT_RESET;
	_writel(val, HIETH_CRG_REG);

	udelay(100);
}

static inline void hieth_clk_ena(void)
{
	u32 val;

	val = _readl(HIETH_CRG_REG);
	val |= ETH_CORE_CLK_SELECT_54M;
	val |= ETH_CLK_ENABLE;
	_writel(val, HIETH_CRG_REG);
}

static inline void hieth_clk_dis(void)
{
	u32 val;

	val = _readl(HIETH_CRG_REG);
	val &= ~ETH_CLK_ENABLE;
	_writel(val, HIETH_CRG_REG);
}

static void hieth_phy_reset(void)
{
	u32 v = 0;

	/************************************************/
	/* reset external phy with default reset pin */
	v = readl(HIETH_CRG_REG);
	v |= ETH_PHY_RESET;
	writel(v, HIETH_CRG_REG);

	udelay(50000);

	/* then, cancel reset, and should delay some time */
	v = readl(HIETH_CRG_REG);
	v &= ~ETH_PHY_RESET;
	writel(v, HIETH_CRG_REG);

	udelay(50000);

#ifdef HISFV_RESET_GPIO_EN
	unsigned int val;
	/*gpiox[x] set to reset, then delay 200ms*/
	val = __raw_readw(HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	val |= (HISFV_RESET_GPIO_DIR_OUT << HISFV_RESET_GPIO_BIT);
	__raw_writew(val, HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	__raw_writew(HISFV_RESET_GPIO_DATA,
			HISFV_RESET_GPIO_BASE + (4 << HISFV_RESET_GPIO_BIT));

	udelay(200000);

	/*then,cancel reset,and should delay 200ms*/
	val = __raw_readw(HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	val |= (HISFV_RESET_GPIO_DIR_OUT << HISFV_RESET_GPIO_BIT);
	__raw_writew(val, HISFV_RESET_GPIO_BASE + HISFV_RESET_GPIO_DIR);
	__raw_writew(((!HISFV_RESET_GPIO_DATA) << HISFV_RESET_GPIO_BIT),
			HISFV_RESET_GPIO_BASE + (4 << HISFV_RESET_GPIO_BIT));

	udelay(20000);
#endif
}

static void hieth_funsel_config(void)
{
}

static void hieth_funsel_restore(void)
{
}

#endif/*CONFIG_NET_HISFV300_3516CV300*/
