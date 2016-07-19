/*****************************************************************************
* Copyright (C), 1988-2012, Huawei Tech. Co,.Ltd
* File name: cmd_bootss.h
* Author:     Version: 0.0.1    Date: 2012.09.05
* Description: upgrade module header file.
*
* History:
* Date          Who          Case                Description
* 2013.07.31    y00175887    DTS2013072205801    fastboot feature add
*******************************************************************************/
#ifndef __FASTBOOT_HIBERNATE_LZO_H__
#define __FASTBOOT_HIBERNATE_LZO_H__

extern int lzo1x_compress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, int unused, void *wrkmem);

extern int lzo1x_decompress(unsigned char *dst, int *dstlen,
		unsigned char *src, int srclen, void *unused);
#endif

