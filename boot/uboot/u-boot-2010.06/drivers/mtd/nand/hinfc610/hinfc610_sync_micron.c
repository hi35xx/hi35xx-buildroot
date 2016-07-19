/******************************************************************************
 *    NAND Flash Controller V610 Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Hisi.
 *
******************************************************************************/

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <malloc.h>
#include <linux/mtd/nand.h>
#include "hinfc610.h"

/*****************************************************************************/
#define MICRON_GET_SYNC_ADDR           0x00
#define MICRON_GET_SYNC_DATA_NUM       0x8

static int micron_parse_sync_data(unsigned char
		sync_data[MICRON_GET_SYNC_DATA_NUM],
				  struct hinfc_sync *sync)
{
	unsigned int ix;

	for (ix = 0; ix < 4; ix++)
		sync->sync_type[ix] = sync_data[ix];
	if (strncmp(sync->sync_type, "ONFI", 4)) {
		printf("UNKNOWN sync type.\n");
		return -1;
	}

#define MICRON_SYNC_REVISION_ONFI_1_0     (1 << 1)
#define MICRON_SYNC_REVISION_ONFI_2_0     (1 << 2)
#define MICRON_SYNC_REVISION_ONFI_2_1     (1 << 3)
#define MICRON_SYNC_REVISION_ONFI_2_2     (1 << 4)
#define MICRON_SYNC_REVISION_ONFI_2_3     (1 << 5)
#define MICRON_SYNC_REVISION_ONFI_3_0     (1 << 6)

	if (sync_data[4] & MICRON_SYNC_REVISION_ONFI_3_0)
		sync->sync_revision = HINFC610_SYNC_TYPE_ONFI3_0;
	else if (sync_data[4] & MICRON_SYNC_REVISION_ONFI_2_3)
		sync->sync_revision = HINFC610_SYNC_TYPE_ONFI2_3;
	else if (sync_data[4] & MICRON_SYNC_REVISION_ONFI_2_2)
		sync->sync_revision = HINFC610_SYNC_TYPE_ONFI2_2;
	else if (sync_data[4] & MICRON_SYNC_REVISION_ONFI_2_1)
		sync->sync_revision = MICRON_SYNC_REVISION_ONFI_2_1;
	else if (sync_data[4] & MICRON_SYNC_REVISION_ONFI_2_0)
		sync->sync_revision = HINFC610_SYNC_TYPE_ONFI2_0;
	else if (sync_data[4] & MICRON_SYNC_REVISION_ONFI_1_0)
		sync->sync_revision = MICRON_SYNC_REVISION_ONFI_1_0;
	else {
		printf("unknown sync revision.\n");
		return -1;
	}

#define MICRON_SYNC_ONFI_INTERFACE_SUPPORT (1 << 5)
	sync->support = (sync_data[6] & MICRON_SYNC_ONFI_INTERFACE_SUPPORT)
			? 1 : 0;
	return 0;
}

static int hinfc610_micron_read_page_param(struct hinfc_host *host,
					  struct hinfc_sync *sync,
					  int current_sync)
{
	unsigned char ix;
	unsigned int regval;
	unsigned char micron_sync_data[MICRON_GET_SYNC_DATA_NUM];

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	/* onfi2.3 sync read page need. */
	if (current_sync) {
		regval = hinfc_read(host, HINFC610_CON);
		regval |= 2 << HINFC610_CON_SYNC_TYPE_SHIFT;
		hinfc_write(host, regval, HINFC610_CON);
	}

	hinfc_write(host, MICRON_GET_SYNC_DATA_NUM, HINFC610_DATA_NUM);
	hinfc_write(host, 0xec, HINFC610_CMD);
	hinfc_write(host, MICRON_GET_SYNC_ADDR, HINFC610_ADDRL);
	hinfc_write(host, HINFC610_READ_1CMD_4ADD_DATA, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	for (ix = 0; ix < MICRON_GET_SYNC_DATA_NUM; ix++)
		micron_sync_data[ix] = readb(host->chip->IO_ADDR_R + ix);
	if (micron_parse_sync_data(micron_sync_data, sync) == 0
	    && (sync->sync_revision == HINFC610_SYNC_TYPE_ONFI2_3)) {
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);
		return 0;
	}

	/* onfi3.0 sync read page need----FIX-ME. */
	if (current_sync) {
		regval = hinfc_read(host, HINFC610_CON);
		regval |= 3 << HINFC610_CON_SYNC_TYPE_SHIFT;
		hinfc_write(host, regval, HINFC610_CON);
	}

	hinfc_write(host, HINFC610_READ_1CMD_4ADD_DATA, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	for (ix = 0; ix < MICRON_GET_SYNC_DATA_NUM; ix++)
		micron_sync_data[ix] = readb(host->chip->IO_ADDR_R + ix);
	if (micron_parse_sync_data(micron_sync_data, sync) == 0
	    && (sync->sync_revision == HINFC610_SYNC_TYPE_ONFI3_0)) {
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);
		return 0;
	}

	printf("parse micron sync data . sync data is:\n"
		"0x%02x, 0x%02x, 0x%02x, 0x%02x, "
		"0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
		micron_sync_data[0],
		micron_sync_data[1],
		micron_sync_data[2],
		micron_sync_data[3],
		micron_sync_data[4],
		micron_sync_data[5],
		micron_sync_data[6],
		micron_sync_data[7]);

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);
	return -1;
}

#undef MICRON_GET_SYNC_ADDR
#undef MICRON_GET_SYNC_DATA_NUM

/*****************************************************************************/
static int hinfc610_micron_get_sync_param(struct hinfc_host *host,
					  int *current_sync)
{
#define MICRON_GET_SYNC_ADDR                0x10
#define MICRON_DEFAULT_DRIVER_STRENGTH      0X02


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
	hinfc_write(host, MICRON_GET_SYNC_ADDR, HINFC610_ADDRL);
	hinfc_write(host, HINFC610_READ_1CMD_1ADD_DATA, HINFC610_OP);
	printf("host->hinfc_con=%x, op=%x", hinfc_read(host, HINFC610_CON),
			hinfc_read(host, HINFC610_OP));
	WAIT_CONTROLLER_FINISH();

	regval = readl(host->chip->IO_ADDR_R);
	printf("get current sync data . sync data is:0x%lx\n", regval);

	if ((regval & 0xffff) == MICRON_DEFAULT_DRIVER_STRENGTH) {
		printf("NAND Flash is in async mode.\n");
		*current_sync = HINFC610_SYNC_TYPE_NONE;
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);
		return 0;
	}
	/* sync mode(ONFI2.3) get geature of driver strength */
	regval = hinfc_read(host, HINFC610_CON);
	regval |= 2 << HINFC610_CON_SYNC_TYPE_SHIFT;
	hinfc_write(host, regval, HINFC610_CON);

	hinfc_write(host, 4, HINFC610_DATA_NUM);
	hinfc_write(host, 0xee, HINFC610_CMD);
	hinfc_write(host, MICRON_GET_SYNC_ADDR, HINFC610_ADDRL);
	hinfc_write(host, HINFC610_READ_1CMD_1ADD_DATA_SYNC, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	regval = readl(host->chip->IO_ADDR_R);
	printf("get current sync data . sync data is:0x%lx\n", regval);
	if ((regval & 0xffff) == MICRON_DEFAULT_DRIVER_STRENGTH) {
		printf("NAND Flash is in sync mode.\n");
		*current_sync = HINFC610_SYNC_TYPE_ONFI2_3;
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);
		return 0;
	}

	/* sync mode(ONFI3.0) get geature of driver strength */
	regval = hinfc_read(host, HINFC610_CON);
	regval |= 3 << HINFC610_CON_SYNC_TYPE_SHIFT;
	hinfc_write(host, regval, HINFC610_CON);
	hinfc_write(host, HINFC610_READ_1CMD_1ADD_DATA_SYNC, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	regval = readl(host->chip->IO_ADDR_R);
	printf("get current sync data . sync data is:0x%lx\n", regval);
	if ((regval & 0xffff) == MICRON_DEFAULT_DRIVER_STRENGTH) {
		printf("NAND Flash is in sync mode.\n");
		*current_sync = HINFC610_SYNC_TYPE_ONFI3_0;
		host->enable_ecc_randomizer(host, ENABLE, ENABLE);
		return 0;
	}

	printf("get current async/sync mode failed.\n");
	return -1;

#undef MICRON_GET_SYNC_ADDR
	return 0;
}

/*****************************************************************************/

static int hinfc610_micron_set_sync_param(struct hinfc_host *host,
					  int enable)
{
#define MICRON_SET_SYNC_ADDR         0x01


	unsigned char para0 = enable ? 0x10 : 0x00;

	static unsigned char micron_sync_param[4];
	micron_sync_param[0] = para0;
	micron_sync_param[1] = 0x00;
	micron_sync_param[2] = 0x00;
	micron_sync_param[3] = 0x00;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, 1, HINFC610_DATA_NUM);
	hinfc_write(host, 0xef, HINFC610_CMD);
	hinfc_write(host, MICRON_SET_SYNC_ADDR, HINFC610_ADDRL);
	writel(micron_sync_param[0], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_1CMD_1ADD_DATA
			: HINFC610_WRITE_1CMD_1ADD_DATA_SYNC, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	writel(micron_sync_param[1], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_0CMD_1ADD_DATA
			: HINFC610_WRITE_0CMD_1ADD_DATA_SYNC, HINFC610_OP);
	WAIT_CONTROLLER_FINISH();

	writel(micron_sync_param[2], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_0CMD_1ADD_DATA
			: HINFC610_WRITE_0CMD_1ADD_DATA_SYNC, HINFC610_OP);

	writel(micron_sync_param[3], host->chip->IO_ADDR_R);
	hinfc_write(host, enable ? HINFC610_WRITE_0CMD_1ADD_DATA
			: HINFC610_WRITE_0CMD_1ADD_DATA_SYNC, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();
#undef MICRON_SET_SYNC_ADDR
	return 0;
}
/*****************************************************************************/

struct synchronous_t hinfc610_micron_sync = {
	.type = HINFC610_SYNC_MICRON,
	.set_sync_param = hinfc610_micron_set_sync_param,
	.get_sync_param = hinfc610_micron_get_sync_param,
	.read_page_param = hinfc610_micron_read_page_param
};
