#include <linux/platform_device.h>

#define CRG_REG_BASE	0x20030000
#define A7_AXI_SCALE_REG   IO_ADDRESS(CRG_REG_BASE + 0x30)

#define get_bus_clk() ({\
		unsigned long tmp_reg, busclk;\
		tmp_reg = readl(A7_AXI_SCALE_REG);\
		if (0x0 == (tmp_reg & 0x8))\
			busclk = 198000000;\
		else \
			busclk = 148500000;\
		busclk;\
		})
