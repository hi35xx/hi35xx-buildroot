/******************************************************************************
 *    NAND Flash Controller V610 Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Czyong.
 *
******************************************************************************/

#include "hinfc610_os.h"
#include "hinfc610.h"

/*****************************************************************************/

#define MICRON_RR_ADDR         0x89

static int hinfc610_micron_set_rr_reg(struct hinfc_host *host, int rr)
{
	int regval;

	host->enable_ecc_randomizer(host, DISABLE, DISABLE);

	hinfc_write(host, 1, HINFC610_DATA_NUM);

	writel(rr, host->chip->IO_ADDR_W);
	hinfc_write(host, MICRON_RR_ADDR, HINFC610_ADDRL);
	/* set read retry */
	hinfc_write(host, 0xEF, HINFC610_CMD);

	/* need to config WAIT_READY_EN, here config WAIT_READY_EN bit. */
	regval = (HINFC610_IS_SYNC(host) ?
		HINFC610_WRITE_1CMD_1ADD_DATA_SYNC_WAIT_READY :
		HINFC610_WRITE_1CMD_1ADD_DATA_WAIT_READY);

	hinfc_write(host, regval, HINFC610_OP);

	WAIT_CONTROLLER_FINISH();

	host->enable_ecc_randomizer(host, ENABLE, ENABLE);

	return 0;
}
/*****************************************************************************/
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
};
