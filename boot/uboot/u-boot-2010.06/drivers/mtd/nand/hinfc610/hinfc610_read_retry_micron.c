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

#define MICRON_RR_ADDR         0x89

static int hinfc610_micron_set_rr_reg(struct hinfc_host *host, int rr)
{
#define MICRON_SET_RR          0xEF

	hinfc_write(host, 1, HINFC610_DATA_NUM);

	writel(rr, host->chip->IO_ADDR_W);
	hinfc_write(host, MICRON_RR_ADDR, HINFC610_ADDRL);
	hinfc_write(host, MICRON_SET_RR, HINFC610_CMD);
	hinfc_write(host, HINFC610_WRITE_1CMD_1ADD_DATA, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();
#undef MICRON_SET_RR
	return 0;
}
/*****************************************************************************/

#if 0
static int hinfc610_micron_get_rr_param(struct hinfc_host *host)
{
#define MICRON_GET_RR          0xEE
	hinfc_write(host, 1, HINFC610_DATA_NUM);

	hinfc_write(host, MICRON_RR_ADDR, HINFC610_ADDRL);
	hinfc_write(host, MICRON_GET_RR, HINFC610_CMD);
	hinfc_write(host, HINFC610_READ_1CMD_1ADD_DATA, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();
	hinfc_write(host, NAND_CMD_READ0, HINFC610_CMD);
	hinfc_write(host, HINFC610_READ_1CMD_0ADD_NODATA, HINFC610_OP);
#undef MICRON_GET_RR
	return readl(host->chip->IO_ADDR_R) & 0xff;
}
#endif
#undef MICRON_RR_ADDR
/*****************************************************************************/

static int hinfc610_micron_set_rr_param(struct hinfc_host *host, int rr_option)
{
	return hinfc610_micron_set_rr_reg(host, rr_option);
}
/*****************************************************************************/

static int hinfc610_micron_reset_rr_param(struct hinfc_host *host)
{
	return hinfc610_micron_set_rr_reg(host, 0);
}
/*****************************************************************************/

struct read_retry_t hinfc610_micron_read_retry = {
	.type = NAND_RR_MICRON,
	.count = 8,
	.set_rr_param = hinfc610_micron_set_rr_param,
	.get_rr_param = NULL,
	.reset_rr_param = hinfc610_micron_reset_rr_param,
	.enable_enhanced_slc = NULL,
};
