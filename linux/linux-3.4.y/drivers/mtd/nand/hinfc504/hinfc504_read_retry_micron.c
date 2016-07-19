/******************************************************************************
 *    NAND Flash Controller V504 Device Driver
 *    Copyright (c) 2009-2010 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Czyong.
 *
******************************************************************************/

#include "hinfc504_os.h"
#include "hinfc504.h"

/*****************************************************************************/

#define MICRON_RR_ADDR         0x89

static int hinfc504_micron_set_rr_reg(struct hinfc_host *host, int rr)
{
#define MICRON_SET_RR          0xEF
	hinfc_write(host, 1, HINFC504_DATA_NUM);

	writel(rr, host->chip->IO_ADDR_W);
	hinfc_write(host, MICRON_RR_ADDR, HINFC504_ADDRL);
	hinfc_write(host, MICRON_SET_RR, HINFC504_CMD);
	hinfc_write(host, HINFC504_WRITE_1CMD_1ADD_DATA, HINFC504_OP);
	WAIT_CONTROLLER_FINISH();
#undef MICRON_SET_RR
	return 0;
}
#undef MICRON_RR_ADDR
/*****************************************************************************/

static int hinfc504_micron_set_rr_param(struct hinfc_host *host, int rr_option)
{
	hinfc504_micron_set_rr_reg(host, rr_option);
	return 0;
}
/*****************************************************************************/

struct read_retry_t hinfc504_micron_read_retry = {
	.type = NAND_RR_MICRON,
	.count = 8,
	.set_rr_param = hinfc504_micron_set_rr_param,
	.get_rr_param = NULL,
};

