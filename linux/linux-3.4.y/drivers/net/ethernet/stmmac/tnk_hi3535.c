#include <linux/io.h>
#include <linux/platform_device.h>
#include "tnk_hi3535.h"
#include "tnkhw.h"

#undef TNK_PRINT

unsigned long tnk_clk_init(void)
{
	unsigned long busclk, tnkclk;
	unsigned long tnkctl;
	unsigned long toe_clk_srst;

#ifdef TNK_HW_PLATFORM_FPGA
	busclk = 50000000; /* TODO: fpga use fixed clk */
#else
	busclk = get_bus_clk();
#endif
	tnkclk = mdio_clk_init();

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

		tnkctl = ((tnkclk / 1000) << 12) | TOE_MAX_CON_NUM;
#ifdef TNK_PRINT
		pr_info
		("%s: Using TOE_DEFAULT_CLK %x, TOE_MAX_CON_NUM %x,tnkctl %x\n",
		__func__, tnkclk, TOE_MAX_CON_NUM,
		(unsigned int)tnkctl);
#endif
		return tnkctl;
}

unsigned long mdio_clk_init(void)
{
	if (readl(REG_CRG0_OFFSET + TOE_CLK_SRST) & TOE_CLK_DEF_100M)
		return TOE_DEFAULT_CLK_100M;
	else if (!(readl(REG_CRG0_OFFSET + TOE_CLK_SRST) & TOE_CLK_DEF_100M))
		return TOE_DEFAULT_CLK_150M;
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
	unsigned int reg;

	/*  Enter reset mode */
	reg = readl(addr + TOE_CLK_SRST);
	reg |= (0x10 << (id << 1));
	writel(reg, addr + TOE_CLK_SRST);
	/*  Write the new configuration value */
	writel(val, addr + TOE_MAC_INTERFACE);
	/*  Make sure the write is fully flushed */
	readl(addr + TOE_MAC_INTERFACE);
	/*  Leave reset mode */
	reg &= ~(0x10 << (id << 1));
	writel(reg, addr + TOE_CLK_SRST);
}
