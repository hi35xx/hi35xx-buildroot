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
#ifndef CONFIG_SPI_NOR_MAX_CHIP_NUM
#define CONFIG_SPI_NOR_MAX_CHIP_NUM			2
	#warning NOT config CONFIG_SPI_NOR_MAX_CHIP_NUM, \
	used default value, maybe invalid.
#endif

/*****************************************************************************/
#define SPI_NOR_BUFFER_LEN			(0x2000000) /* 32MB */

/*****************************************************************************/
extern int start_up_addr_mode;

/*****************************************************************************/
extern int hifmc100_spi_nor_init(struct hifmc_host *host);

extern int hifmc100_spi_nor_scan(struct hifmc_host *host);

#endif /* End of __HIFMC100_OS_H__ */

#ifdef CONFIG_PM
static int hifmc100_suspend(struct platform_device *dev, pm_message_t state);
static int hifmc100_resume(struct platform_device *dev);
#endif
