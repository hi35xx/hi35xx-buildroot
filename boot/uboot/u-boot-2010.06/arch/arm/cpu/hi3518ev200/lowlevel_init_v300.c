/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <asm/arch/platform.h>
#include <config.h>
#ifdef CONFIG_DDR_TRAINING_V2
#include <ddr_interface.h>
#endif


#define DDR_DMC_BASE_REG0	DDR_DMC0_REG_BASE
extern void reset_cpu(unsigned long addr);

static inline void DWB(void) /* drain write buffer */
{
}

static inline unsigned int readl(unsigned addr)
{
	unsigned int val;
	val = (*(volatile unsigned int *)(addr));
	return val;
}

static inline void writel(unsigned val, unsigned addr)
{
	DWB();
	(*(volatile unsigned *) (addr)) = (val);
	DWB();
}

static inline void delay(void)
{
	__asm__ __volatile__("nop");
}

void start_ddr_training(unsigned int base)
{
#ifdef CONFIG_DDR_TRAINING_V2
	int ret = 0;

	/* ddr training function */
	ret = ddr_sw_training_if(0);
	if (ret)
		reset_cpu(0);
#endif

	/*the value should config after trainning, or
	 it will cause chip compatibility problems*/
	writel(0x401, DDR_DMC_BASE_REG0 + 0x28);
}

