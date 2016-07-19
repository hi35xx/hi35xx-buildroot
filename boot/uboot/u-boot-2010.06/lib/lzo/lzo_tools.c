/*****************************************************************************
* Copyright (C), 1988-2012, Huawei Tech. Co,.Ltd
* File name: cmd_bootss.h
* Author:     Version: 0.0.1    Date: 2012.09.05
* Description: upgrade module header file.
*
* History:
* Date          Who          Case                Description
* 2013.07.31    y00175887    DTS2013072205801    fastboot feature add
******************************************************************************/

#include <common.h>
#include <linux/lzo.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>

#include "lzodefs.h"
#include "lzo_tools.h"

int lzo1x_compress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, int unused, void *wrkmem)
{
	size_t tmp_slen, tmp_dlen;
	int ret;

	tmp_slen = srclen;
	tmp_dlen = *dstlen;
	ret = lzo1x_1_compress(src, tmp_slen, dst, &tmp_dlen, wrkmem);
	if (ret != LZO_E_OK) {
		printf("lzo1x_compress(): failed %d\n", ret);
		return -1;
	}
	*dstlen = tmp_dlen;
	return 0;
}


int lzo1x_decompress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, void *unused)
{
	size_t tmp_slen, tmp_dlen;
	int ret;
	tmp_slen = srclen;
	tmp_dlen = *dstlen;
	ret = lzo1x_decompress_safe(src, tmp_slen, dst, &tmp_dlen);
	if (ret != LZO_E_OK) {
		printf("lzo1x_decompress(): failed %d\n", ret);
		return -1;
	}
	*dstlen = tmp_dlen;
	return 0;
}

