#include <linux/mutex.h>
#include <linux/math64.h>
#include <linux/io.h>
#include <linux/mtd/mtd.h>
#include <linux/platform_device.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>

#include "../spi_ids.h"
#include "hisfc300.h"

/*periph hisfc CGR25 register*/
#define HISFC300_CRG25					(0x0064)
#define HISFC300_CRG25_RST				(1 << 0)
#define HISFC300_CRG25_CLKEN				(1 << 1)
#define HISFC300_CRG25_CLK_24M				((0x00)<<2) /* 24M */
#define HISFC300_CRG25_CLK_100M				((0x01)<<2) /* 100M */
#define HISFC300_CFG25_CLK_149M				((0x11)<<2) /* 148.5M */

/*****************************************************************************/
void hisfc200_set_system_clock(struct hisfc_host *host,
		struct spi_operation *op, int clk_en)
{
	unsigned int regval;

	if (clk_en) {
		/* enable clk, and set best clk */
		regval = readl(host->cfgreg + HISFC300_CRG25);
		if (op && op->clock)
			regval |= (op->clock & 0xc);
		else
			regval |= HISFC300_CRG25_CLK_24M;

		regval |= HISFC300_CRG25_CLKEN;
		writel(regval, (host->cfgreg + HISFC300_CRG25));
		/* undo reset */
		regval = readl(host->cfgreg + HISFC300_CRG25);
		regval &= ~HISFC300_CRG25_RST;
		writel(regval, (host->cfgreg + HISFC300_CRG25));
		udelay(50);
	} else {
		/* reset */
		regval = readl(host->cfgreg + HISFC300_CRG25);
		regval |= HISFC300_CRG25_RST;
		writel(regval, (host->cfgreg + HISFC300_CRG25));
		udelay(50);
		/* disable clk */
		regval = readl(host->cfgreg + HISFC300_CRG25);
		regval &= ~HISFC300_CRG25_CLKEN;
		writel(regval, (host->cfgreg + HISFC300_CRG25));
		udelay(50);
	}
}

/*****************************************************************************/
void hisfc200_get_best_clock(unsigned int *_clock)
{
	int ix, clk;
  #define CLK_2X(_clk)   (((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),  HISFC300_CRG25_CLK_24M,
		CLK_2X(100),  HISFC300_CRG25_CLK_100M,
		CLK_2X(149), HISFC300_CFG25_CLK_149M, /* 148.5M */
		0, 0,
	};
  #undef CLK_2X

	if (!_clock)
		return;

	clk = HISFC300_CRG25_CLK_24M;
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*_clock < sysclk[ix])
			break;
		clk = sysclk[ix+1];
	}

	*_clock = clk;
}
/*****************************************************************************/
