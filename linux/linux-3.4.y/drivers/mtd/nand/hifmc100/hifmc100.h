/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

#ifndef __HIFMC100_H__
#define __HIFMC100_H__

/*****************************************************************************/
#include <linux/platform_device.h>
#include "../../hifmc_common.h"
#include "../../hifmc_spi_ids.h"

/*****************************************************************************/
/* These macroes are for debug only, reg option is slower then dma option */
#undef HIFMC100_SPI_NAND_SUPPORT_REG_READ
/* #define HIFMC100_SPI_NAND_SUPPORT_REG_READ */

#undef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
/* #define HIFMC100_SPI_NAND_SUPPORT_REG_WRITE */
/*****************************************************************************/
#define HIFMC100_ECC_ERR_NUM0_BUF0		0xc0

#define GET_ECC_ERR_NUM(_i, _reg)		(((_reg) >> ((_i) * 8)) & 0xff)

/*****************************************************************************/
#define REG_CNT_HIGH_BLOCK_NUM_SHIFT		10

#define REG_CNT_BLOCK_NUM_MASK			0x3ff
#define REG_CNT_BLOCK_NUM_SHIFT			22

#define REG_CNT_PAGE_NUM_MASK			0x3f
#define REG_CNT_PAGE_NUM_SHIFT			16

#define REG_CNT_WRAP_MASK			0xf
#define REG_CNT_WRAP_SHIFT			12

#define REG_CNT_ECC_OFFSET_MASK			0xfff
#define REG_CNT_ECC_8BIT_OFFSET			1054
#define REG_CNT_ECC_16BIT_OFFSET		1056
#define REG_CNT_ECC_24BIT_OFFSET		1082

#define ERR_STR_DRIVER "Driver does not support this configure "
#define ERR_STR_CHECK "Please make sure the hardware configuration is correct"

/*****************************************************************************/
#define HIFMC100_ADDR_CYCLE_MASK		0x2

/*****************************************************************************/
struct hifmc_host {
	struct mtd_info *mtd;
	struct nand_chip *chip;
	struct hifmc_spi spi[CONFIG_SPI_NAND_MAX_CHIP_NUM];
	struct hifmc_cmd_op cmd_op;

	void __iomem *iobase;
	void __iomem *regbase;

	unsigned int fmc_cfg;
	unsigned int fmc_cfg_ecc0;

	unsigned int offset;

	struct device *dev;

	/* This is maybe an un-aligment address, only for malloc or free */
	char *buforg;
	char *buffer;

	unsigned int dma_buffer;
	unsigned int dma_oob;

	unsigned int addr_cycle;
	unsigned int addr_value[2];
	unsigned int cache_addr_value[2];

	unsigned int column;
	unsigned int block_page_mask;

	unsigned int ecctype;
	unsigned int pagesize;
	unsigned int oobsize;

	int add_partition;

	int  need_rr_data;
#define HIFMC100_READ_RETRY_DATA_LEN         128
	char rr_data[HIFMC100_READ_RETRY_DATA_LEN];
	struct read_retry_t *read_retry;

	int version;

	/* BOOTROM read two bytes to detect the bad block flag */
#define HIFMC_BAD_BLOCK_POS		0
	unsigned char *bbm;	/* nand bad block mark */
	unsigned short *epm;	/* nand empty page mark */

	unsigned int uc_er;

	void (*send_cmd_pageprog)(struct hifmc_host *host);
	void (*send_cmd_status)(struct hifmc_host *host);
	void (*send_cmd_readstart)(struct hifmc_host *host);
	void (*send_cmd_erase)(struct hifmc_host *host);
	void (*send_cmd_readid)(struct hifmc_host *host);
	void (*send_cmd_reset)(struct hifmc_host *host);
	void (*set_system_clock)(struct spi_op *op, int clk_en);
#ifdef CONFIG_PM
	int (*suspend)(struct platform_device *pltdev, pm_message_t state);
	int (*resume)(struct platform_device *pltdev);
#endif
};

/*****************************************************************************/
void hifmc100_ecc0_switch(struct hifmc_host *host, unsigned char op);

int hifmc100_spi_nand_init(struct nand_chip *chip);

/*****************************************************************************/
extern void hifmc_spi_nand_ids_register(void);

extern void hifmc_set_nand_system_clock(struct spi_op *op, int clk_en);

/*****************************************************************************/
#ifdef CONFIG_PM
int hifmc100_spi_nand_suspend(struct platform_device *pltdev,
		pm_message_t state);
int hifmc100_spi_nand_resume(struct platform_device *pltdev);
void hifmc100_spi_nand_config(struct hifmc_host *host);
#endif

#endif /* End of __HIFMC100_H__ */

