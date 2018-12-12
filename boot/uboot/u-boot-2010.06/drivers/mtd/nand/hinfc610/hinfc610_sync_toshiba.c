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


#define TOSHIBA_GET_PAGE_PARA_ADDR      0x40
#define TOSHIBA_GET_SYNC_DATA_NUM       0x8

/*****************************************************************************/

static int hinfc610_toshiba_set_sync_param(struct hinfc_host *host, int enable)
{
#define TOSHIBA_SET_SYNC_ADDR         0x80

	unsigned long regval;
	unsigned char para0 = enable ? 0x00 : 0x01;

	static unsigned char toshiba_sync_param[4];
	toshiba_sync_param[0] = para0;
	toshiba_sync_param[1] = 0x00;
	toshiba_sync_param[2] = 0x00;
	toshiba_sync_param[3] = 0x00;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, 1, HINFC610_DATA_NUM);
	hinfc_write(host, 0xef, HINFC610_CMD);
	hinfc_write(host, TOSHIBA_SET_SYNC_ADDR, HINFC610_ADDRL);
	writel(toshiba_sync_param[0], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_1CMD_1ADD_DATA
			: HINFC610_WRITE_1CMD_1ADD_DATA_SYNC, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	writel(toshiba_sync_param[1], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_0CMD_1ADD_DATA
			: HINFC610_WRITE_0CMD_1ADD_DATA_SYNC, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	writel(toshiba_sync_param[2], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_0CMD_1ADD_DATA
			: HINFC610_WRITE_0CMD_1ADD_DATA_SYNC, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	writel(toshiba_sync_param[3], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_0CMD_1ADD_DATA
			: HINFC610_WRITE_0CMD_1ADD_DATA_SYNC, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	/* toggle need reset after set sync mode. */
	if (enable) {
		hinfc_write(host, NAND_CMD_RESET, HINFC610_CMD);
		hinfc_write(host,
			    (host->NFC_CON
			     | (1 << HINFC610_CON_SYNC_TYPE_SHIFT)),
			    HINFC610_CON);

		regval = HINFC610_OP_CMD1_EN
			 | (((host->chipselect & HINFC610_OP_NF_CS_MASK)
				<< HINFC610_OP_NF_CS_SHIFT)
			 | HINFC610_OP_WAIT_READY_EN);

		hinfc_write(host, regval, HINFC610_OP);

		WAIT_CONTROLLER_FINISH();
	}

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

#undef TOSHIBA_SET_SYNC_ADDR

	return 0;

}
/*****************************************************************************/

static int toshiba_parse_sync_data(unsigned char
		sync_data[TOSHIBA_GET_SYNC_DATA_NUM],
				  struct hinfc_sync *sync)
{
	unsigned int ix;

	for (ix = 0; ix < 4; ix++)
		sync->sync_type[ix] = sync_data[ix];
	if (strncmp(sync->sync_type, "JESD", 4)) {
		printf("UNKNOWN sync type.\n");
		return -1;
	}

#define TOSHIBA_SYNC_REVISION_TOGGLE_1_0     (1 << 1)

	if (sync_data[4] & TOSHIBA_SYNC_REVISION_TOGGLE_1_0)
		sync->sync_revision = HINFC610_SYNC_TYPE_TOGGLE1_0;

	sync->support = 1;

#undef TOSHIBA_SYNC_REVISION_TOGGLE_1_0
	return 0;
}
/*****************************************************************************/

static int hinfc610_toshiba_read_page_param(struct hinfc_host *host,
						struct hinfc_sync *sync,
						int current_sync)
{
	unsigned char ix;
	unsigned int regval;
	unsigned char toshiba_sync_data[TOSHIBA_GET_SYNC_DATA_NUM];

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	if (current_sync) {
		regval = hinfc_read(host, HINFC610_CON);
		regval |= 1 << HINFC610_CON_SYNC_TYPE_SHIFT;
		hinfc_write(host, regval, HINFC610_CON);
	}

	hinfc_write(host, TOSHIBA_GET_SYNC_DATA_NUM, HINFC610_DATA_NUM);
	hinfc_write(host, 0xec, HINFC610_CMD);
	hinfc_write(host, TOSHIBA_GET_PAGE_PARA_ADDR, HINFC610_ADDRL);
	/* toggle sync config HINFC610_OP[13:12]:
	 * 1.get/set feature :          10
	 * 2.read id :                  01(FIX ME---yubingxu: must 10/11?)
	 * 3.read/write page, read status, other operations: 00
	 */
	hinfc_write(host, HINFC610_READ_1CMD_4ADD_DATA, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	for (ix = 0; ix < TOSHIBA_GET_SYNC_DATA_NUM; ix++)
		toshiba_sync_data[ix] = readb(host->chip->IO_ADDR_R + ix);
	if (toshiba_parse_sync_data(toshiba_sync_data, sync) < 0) {
		printf("parse toshiba sync data failed. sync data is:\n"
			"0x%02x, 0x%02x, 0x%02x, 0x%02x, "
			"0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
			toshiba_sync_data[0],
			toshiba_sync_data[1],
			toshiba_sync_data[2],
			toshiba_sync_data[3],
			toshiba_sync_data[4],
			toshiba_sync_data[5],
			toshiba_sync_data[6],
			toshiba_sync_data[7]);
		return -1;
	}
	printf("parse micron sync data . sync data is:\n"
		"0x%02x, 0x%02x, 0x%02x, 0x%02x, "
		"0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
		toshiba_sync_data[0],
		toshiba_sync_data[1],
		toshiba_sync_data[2],
		toshiba_sync_data[3],
		toshiba_sync_data[4],
		toshiba_sync_data[5],
		toshiba_sync_data[6],
		toshiba_sync_data[7]);

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	return 0;
}
/*****************************************************************************/
/*
 * current_sync: 1-current is sync; 0-current is async.
 */

static int hinfc610_toshiba_get_sync_param(struct hinfc_host *host,
		int *current_sync)
{

#define TOSHIBA_GET_SYNC_ADDR              0x10
#define TOSHIBA_DEFAULT_DRIVER_STRENGTH    0x0004

	unsigned long regval;

	/* async mode get feature of driver strength */
	host->enable_ecc_randomizer(host, DISABLE, DISABLE);
	regval = hinfc_read(host, HINFC610_CON);
	regval &= ~(HINFC610_CON_SYNC_TYPE_MASK);
	hinfc_write(host, regval, HINFC610_CON);
	memset(host->chip->IO_ADDR_R, 0xff, 4);
	hinfc_write(host, 0xfffffff, HINFC610_SYNC_TIMING);
	hinfc_write(host, 4, HINFC610_DATA_NUM);
	hinfc_write(host, 0xee, HINFC610_CMD);
	hinfc_write(host, TOSHIBA_GET_SYNC_ADDR, HINFC610_ADDRL);
	hinfc_write(host, HINFC610_READ_1CMD_1ADD_DATA, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	regval = readl(host->chip->IO_ADDR_R);
	printf("get current sync data . sync data is:0x%lx\n", regval);

	if ((regval & 0xffff) == TOSHIBA_DEFAULT_DRIVER_STRENGTH) {
		printf("NAND Flash is in async mode.\n");
		*current_sync = HINFC610_SYNC_TYPE_NONE;
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);
		return 0;
	}
	/* sync mode get geature of driver strength */
	regval = hinfc_read(host, HINFC610_CON);
	regval |= 1 << HINFC610_CON_SYNC_TYPE_SHIFT;
	hinfc_write(host, regval, HINFC610_CON);

	hinfc_write(host, 4, HINFC610_DATA_NUM);
	hinfc_write(host, 0xee, HINFC610_CMD);
	hinfc_write(host, TOSHIBA_GET_SYNC_ADDR, HINFC610_ADDRL);
	hinfc_write(host, HINFC610_READ_1CMD_1ADD_DATA_SYNC, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	regval = readl(host->chip->IO_ADDR_R);
	printf("get current sync data . sync data is:0x%lx\n", regval);
	if ((regval & 0xffff) == TOSHIBA_DEFAULT_DRIVER_STRENGTH) {
		printf("NAND Flash is in sync mode.\n");
		*current_sync = HINFC610_SYNC_TYPE_TOGGLE1_0;
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);
		return 0;
	}

	printf("get current async/sync mode failed.\n");
	return -1;

#undef TOSHIBA_GET_SYNC_ADDR
}

/*****************************************************************************/

struct synchronous_t hinfc610_toshiba_sync = {
	.type = HINFC610_SYNC_TOSHIBA,
	.set_sync_param  = hinfc610_toshiba_set_sync_param,
	.get_sync_param  = hinfc610_toshiba_get_sync_param,
	.read_page_param = hinfc610_toshiba_read_page_param
};
