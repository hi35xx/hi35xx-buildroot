#ifndef __HISI_CLOCK_H__
#define __HISI_CLOCK_H__

#define OSC_FREQ 24000000
struct clk {
	unsigned long rate;
};

unsigned long get_ahbclk_hw(void);

#endif

