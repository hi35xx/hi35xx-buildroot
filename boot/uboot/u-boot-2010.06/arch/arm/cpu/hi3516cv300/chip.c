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

#include <config.h>

#define readl(_a)        (*(volatile unsigned int *)(_a))
#define writel(_v, _a)   (*(volatile unsigned int *)(_a) = (_v))

#define HI_CHIP_ID _HI3516CV300
extern void udelay(unsigned long usec);
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
	while (1) {
		/* Any value to this reg will reset the cpu */
		writel(0x12345678, (REG_BASE_SCTL + REG_SC_SYSRES));
		udelay(500);
	}
}
/******************************************************************************/
