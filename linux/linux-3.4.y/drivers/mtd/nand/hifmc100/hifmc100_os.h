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
#ifndef CONFIG_SPI_NAND_MAX_CHIP_NUM
	#define CONFIG_SPI_NAND_MAX_CHIP_NUM		(1)
	#warning NOT config CONFIG_SPI_NAND_MAX_CHIP_NUM, \
		used default value, maybe invalid.
#endif

/*****************************************************************************/
#define SPI_NAND_MAX_PAGESIZE			4096
#define SPI_NAND_MAX_OOBSIZE			256

#define SPI_NAND_BUFFER_LEN	(SPI_NAND_MAX_PAGESIZE + SPI_NAND_MAX_OOBSIZE)

#endif /* End of __HIFMC100_OS_H__ */

