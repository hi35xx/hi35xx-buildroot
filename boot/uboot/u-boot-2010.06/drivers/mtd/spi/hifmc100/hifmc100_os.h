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
extern int start_up_addr_mode;

/*****************************************************************************/
struct spi_flash *hifmc100_spi_nor_probe(struct mtd_info_ex **spi_nor_info);

extern int hifmc_ip_ver_check(void);

extern int hifmc100_spi_nor_init(struct hifmc_host *);

extern struct spi_flash *hifmc100_spi_nor_scan(struct hifmc_host *host);

#endif /* End of __HIFMC100_OS_H__ */

