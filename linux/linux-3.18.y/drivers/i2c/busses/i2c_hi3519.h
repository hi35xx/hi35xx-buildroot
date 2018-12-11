#include <linux/platform_device.h>

#define CRG_REG_BASE	0x12010000
#define A7_AXI_SCALE_REG   IO_ADDRESS(CRG_REG_BASE + 0x34)

#define get_bus_clk() ({\
		unsigned long tmp_reg, busclk = 0;\
		tmp_reg = readl((void *)A7_AXI_SCALE_REG);\
		if (0x1 == ((tmp_reg >> 12) & 0x3))\
			busclk = 200000000;\
		busclk;\
	})
