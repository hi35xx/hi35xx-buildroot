/*
 * Copyright (c) 2009 hisilicon.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <asm/arch/l2_cache.h>

/****************************************************************************
*
* l2_cache_enable -  enable l2 cache
*
* This routine clean all l2 cache then enable l2 cache.
*
****************************************************************************/
void l2cache_enable(void)
{
	unsigned int wayNum;
	unsigned int reg;

	/*invalidate cache  all-way*/
	for (wayNum = 0; wayNum < L2_WAY_NUM; wayNum++)
		L2InvalidAuto(wayNum);

	reg = L2_REG_READ(REG_L2_CACHE_AUX_CTRL);

	reg |=  (0x1 << BIT_L2_AUCTRL_EVENT_MON_BUS_EN) |
		(0x1 << BIT_L2_AUCTRL_EVENT_CNT_EN);

	L2_REG_WRITE(reg, REG_L2_CACHE_AUX_CTRL);

	L2_REG_WRITE(0X1 << BIT_L2_CTRL_CACHE_ENABLE, REG_L2_CACHE_CTRL);
}

/***************************************************************************
*
* l2_cache_disable -  disable l2 cache
*
* This routine clean and invalid all l2 cache then disable l2 cache.
*
****************************************************************************/
void l2cache_disable(void)
{
	unsigned int wayNum;

	for (wayNum = 0; wayNum < L2_WAY_NUM; wayNum++) {
		L2CleanAuto(wayNum);
		L2InvalidAuto(wayNum);
	}

	L2_REG_WRITE(0, REG_L2_CACHE_CTRL);
}

/********************************************************************
*
* L2CleanAuto -  auto clean l2 cache
*
* This routine auto clean l2 cache
*
*********************************************************************/
void L2CleanAuto(unsigned int wayNum)
{
	unsigned int reg;

	reg = L2_REG_READ(REG_L2_CACHE_SYNC);

	reg = (wayNum << BIT_L2_MAINT_AUTO_WAYADDRESS) |
		(0x1 << BIT_L2_MAINT_AUTO_START) |
		(0x1 << BIT_L2_MAINT_AUTO_CLEAN);

	L2_REG_WRITE(reg, REG_L2_CACHE_MAINT_AUTO);

	while (!(L2_REG_READ(REG_L2_CACHE_RAW_INT) &
		(0X1 << BIT_L2_RINT_AUTO_END)))
		;

	reg = L2_REG_READ(REG_L2_CACHE_RAW_INT);

	L2_REG_WRITE(reg, REG_L2_CACHE_INTCLR);
}

/*****************************************************************************
*
* L2InvalidAuto -  auto invalid l2 cache
*
* This routine auto invalid l2 cache
*
******************************************************************************/
/* need invalid cache way num */
void L2InvalidAuto(unsigned int wayNum)
{
	unsigned int reg;

	reg = L2_REG_READ(REG_L2_CACHE_SYNC);

	reg = (wayNum << BIT_L2_MAINT_AUTO_WAYADDRESS) |
		(0x1 << BIT_L2_MAINT_AUTO_START);

	L2_REG_WRITE(reg, REG_L2_CACHE_MAINT_AUTO);

	while (!(L2_REG_READ(REG_L2_CACHE_RAW_INT) &
		(0X1 << BIT_L2_RINT_AUTO_END)))
		;

	reg = L2_REG_READ(REG_L2_CACHE_RAW_INT);

	L2_REG_WRITE(reg, REG_L2_CACHE_INTCLR);
}
