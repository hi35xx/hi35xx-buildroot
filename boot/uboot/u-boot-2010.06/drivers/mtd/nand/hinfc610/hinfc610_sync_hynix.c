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

static int hinfc610_hynix_get_sync_param(struct hinfc_host *host,
					 int *current_sync)
{
	return 0;
}

/*****************************************************************************/

static int hinfc610_hynix_set_sync_param(struct hinfc_host *host, int enable)
{
	return 0;
}
/*****************************************************************************/

struct synchronous_t hinfc610_hynix_sync = {
	.type = HINFC610_SYNC_HYNIX,
	.set_sync_param = hinfc610_hynix_set_sync_param,
	.get_sync_param = hinfc610_hynix_get_sync_param,
};
