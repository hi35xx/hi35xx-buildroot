#include "i2c_hi3516a.h"

unsigned int get_apb_clk(void)
{
	unsigned int apb_clk;

	apb_clk = get_bus_clk() / 4;

	return apb_clk;
}
