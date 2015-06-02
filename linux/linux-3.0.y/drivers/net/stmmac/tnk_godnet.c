#include <linux/io.h>
#include <linux/platform_device.h>
#include "tnk_godnet.h"

#undef TNK_PRINT

unsigned long tnk_clk_init(void)
{
	unsigned long busclk, tnkclk;
	unsigned long tnkctl;
	unsigned long toe_clk_srst;

	busclk = get_bus_clk();

	tnkclk = busclk >> 1;

	toe_clk_srst = readl(REG_CRG0_OFFSET + TOE_CLK_SRST);
#ifdef TNK_PRINT
	pr_info("%s: busclk: %.8x, tnkclk: %.8x, toe_clk_srst: %.8x\n",
	       __func__, (unsigned int)busclk,
		(unsigned int)tnkclk, (unsigned int)toe_clk_srst);

	pr_info("REG_CRG0_OFFSET: 0x%08X\n"
	       "REG_CRG1_OFFSET: 0x%08X\n"
	       "A9_AXI_SCALE_REG: 0x%08X\n",
	       (unsigned int)HW_REG(REG_CRG0_OFFSET),
	       (unsigned int)HW_REG(REG_CRG1_OFFSET),
	       (unsigned int)HW_REG(A9_AXI_SCALE_REG));
#endif

	if (toe_clk_srst & TOE_CLK_DEF_100M) {
		tnkctl = (TOE_DEFAULT_CLK << 12) | TOE_MAX_CON_NUM;
#ifdef TNK_PRINT
		pr_info
		("%s: Using TOE_DEFAULT_CLK %x, TOE_MAX_CON_NUM %x,tnkctl %x\n",
		__func__, TOE_DEFAULT_CLK, TOE_MAX_CON_NUM,
		(unsigned int)tnkctl);
#endif
		return tnkctl;
	} else {
		tnkctl = ((tnkclk / 1000) << 12) | TOE_MAX_CON_NUM;
#ifdef TNK_PRINT
		pr_info("%s: Using tnkclk %x, TOE_MAX_CON_NUM %x, tnkctl %x\n",
		__func__, (unsigned int)tnkclk,
		TOE_MAX_CON_NUM, (unsigned int)tnkctl);
#endif
		return tnkctl;
	}
}

unsigned long mdio_clk_init(void)
{
	unsigned long busclk, tnkclk, defclk;

	defclk = TOE_DEFAULT_CLK;

	busclk = get_bus_clk();

	tnkclk = busclk >> 1;

	if (readl(REG_CRG0_OFFSET + TOE_CLK_SRST) & TOE_CLK_DEF_100M)
		return defclk;
	else if (!(readl(REG_CRG0_OFFSET + TOE_CLK_SRST) & TOE_CLK_DEF_100M))
		return tnkclk;
	else
		return 0;
}

int get_clk_csr(unsigned long tnkclk)
{
	int clk_csr;

	if ((tnkclk >= 60000000) && (tnkclk <= 100000000))
		clk_csr = 0;
	else if ((tnkclk >= 100000000) && (tnkclk <= 150000000))
		clk_csr = 1;
	else if ((tnkclk >= 20000000) && (tnkclk <= 35000000))
		clk_csr = 2;
	else if ((tnkclk >= 35000000) && (tnkclk <= 60000000))
		clk_csr = 3;
	else if ((tnkclk >= 150000000) && (tnkclk <= 250000000))
		clk_csr = 4;
	else if ((tnkclk >= 250000000) && (tnkclk <= 300000000))
		clk_csr = 5;
	else
		clk_csr = -1;

	return clk_csr;
}

void reset_mac_interface_dual(int id, void *addr, u32 val)
{
	/*  Enter reset mode */
	writel(1 + id, addr + 0xc8);
	/*  Write the new configuration value */
	writel(val, addr + 0xec);
	/*  Make sure the write is fully flushed */
	readl(addr + 0xec);
	/*  Leave reset mode */
	writel(0, addr + 0xc8);
}
void reset_mac_interface_single(void *addr, u32 val)
{
	/*  Enter reset mode */
	writel(1, addr + 0xc8);
	/*  Write the new configuration value */
	writel(val, addr + 0xec);
	/*  Make sure the write is fully flushed */
	readl(addr + 0xec);
	/*  Leave reset mode */
	writel(0, addr + 0xc8);
}
