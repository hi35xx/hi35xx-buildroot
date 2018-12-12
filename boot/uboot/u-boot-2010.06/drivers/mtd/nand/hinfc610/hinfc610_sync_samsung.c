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

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <malloc.h>
#include <linux/mtd/nand.h>
#include "hinfc610.h"

/*****************************************************************************/

static int hinfc610_samsung_get_sync_param(struct hinfc_host *host,
					   int *current_sync)
{
	return 0;
}

/*****************************************************************************/

static int hinfc610_samsung_set_sync_param(struct hinfc_host *host,
		int enable)
{
	return 0;
}
/*****************************************************************************/

struct synchronous_t hinfc610_samsung_sync = {
	.type = HINFC610_SYNC_SAMSUNG,
	.set_sync_param = hinfc610_samsung_set_sync_param,
	.get_sync_param = hinfc610_samsung_get_sync_param,
};

