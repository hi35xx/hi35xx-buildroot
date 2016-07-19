/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

#ifndef __HIFMC100_OS_H__
#define __HIFMC100_OS_H__

/*****************************************************************************/
int board_nand_init(struct nand_chip *chip);

/*****************************************************************************/
extern int hifmc_ip_ver_check(void);

#endif /* End of __HIFMC100_OS_H__ */

