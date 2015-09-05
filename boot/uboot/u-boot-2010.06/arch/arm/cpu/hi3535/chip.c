/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
* ***
******************************************************************************/

#include <config.h>

#define readl(_a)        (*(volatile unsigned int *)(_a))
#define writel(_v, _a)   (*(volatile unsigned int *)(_a) = (_v))

#define HI_CHIP_ID _HI3535_V100
/******************************************************************************/

static inline void delay(unsigned int cnt)
{
	while (cnt--)
		__asm__ __volatile__("nop");
}

/******************************************************************************/

long long get_chipid_reg(void)
{
	long long chipid = 0;

	chipid = HI_CHIP_ID;

	return chipid;
}

/******************************************************************************/

unsigned int get_ca_vendor_reg(void)
{
	unsigned long ca_vendor = 0;

	/* TODO: XXXX */

	return ca_vendor;
}

/******************************************************************************/

void reset_cpu(unsigned long addr)
{
	while (1)
		/* Any value to this reg will reset the cpu */
		writel(0x12345678, (REG_BASE_SCTL + REG_SC_SYSRES));
}
/******************************************************************************/
