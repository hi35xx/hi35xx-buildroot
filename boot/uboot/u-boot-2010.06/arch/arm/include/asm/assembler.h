/***********************************************************************
* Copyright (C), 1988-2012, Huawei Tech. Co,.Ltd
* File name: cmd_bootss.h
* Author:     Version: 0.0.1    Date: 2012.09.05
* Description: upgrade module header file.
*
* History:
* Date          Who          Case                Description
* 2013.07.31    y00175887    DTS2013072205801    fastboot feature add
************************************************************************/

/*
 * Endian independent macros for shifting bytes within registers.
 */
#ifndef __ARMEB__
#define pull		lsr
#define push		lsl
#define get_byte_0	(lsl #0)
#define get_byte_1	(lsr #8)
#define get_byte_2	(lsr #16)
#define get_byte_3	(lsr #24)
#define put_byte_0	(lsl #0)
#define put_byte_1	(lsl #8)
#define put_byte_2	(lsl #16)
#define put_byte_3	(lsl #24)
#else
#define pull		lsl
#define push		lsr
#define get_byte_0	(lsr #24)
#define get_byte_1	(lsr #16)
#define get_byte_2	(lsr #8)
#define get_byte_3      (lsl #0)
#define put_byte_0	(lsl #24)
#define put_byte_1	(lsl #16)
#define put_byte_2	(lsl #8)
#define put_byte_3      (lsl #0)
#endif

/*
 * Data preload for architectures that support it
 */
#if defined(__ARM_ARCH_5E__) || defined(__ARM_ARCH_5TE__) || \
	defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || \
	defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6Z__) || \
	defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_7A__) || \
	defined(__ARM_ARCH_7R__)
#define PLD(code...)	code
#else
#define PLD(code...)
#endif

/*
 * Cache alligned
 */
#define CALGN(code...) code
