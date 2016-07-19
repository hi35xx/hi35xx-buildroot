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
/* These macroes are for debug only, reg read is slower then dma read */
#undef HIFMC100_SPI_NOR_SUPPORT_REG_READ
/* #define HIFMC100_SPI_NOR_SUPPORT_REG_READ */

#undef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
/* #define HIFMC100_SPI_NOR_SUPPORT_REG_WRITE */

/*****************************************************************************/
#define HIFMC100_DMA_MAX_SIZE			(4096)
#define HIFMC100_DMA_MASK			(HIFMC100_DMA_MAX_SIZE - 1)

/*****************************************************************************/
#define SPI_NOR_CR_SHIFT	8	/* Config Register shift(bit) */

#define SPI_NOR_CR_4BYTE_SHIFT	5
#define SPI_NOR_CR_4BYTE_MASK		(1 << SPI_NOR_CR_4BYTE_SHIFT)
#define SPI_NOR_GET_4BYTE_BY_CR(cr)	(((cr) & SPI_NOR_CR_4BYTE_MASK) \
						>> SPI_NOR_CR_4BYTE_SHIFT)

#define SPI_NOR_CR_QE_SHIFT		1
#define SPI_NOR_CR_QE_MASK		(1 << SPI_NOR_CR_QE_SHIFT)
#define SPI_NOR_GET_QE_BY_CR(cr)	(((cr) & SPI_NOR_CR_QE_MASK) \
						>> SPI_NOR_CR_QE_SHIFT)

#ifdef CONFIG_SPI_BLOCK_PROTECT
#define MID_SPANSION			0x01    /* Spansion Manufacture ID */
#define MID_WINBOND			0xef    /* Winbond  Manufacture ID */
#define MID_MXIC			0xc2    /* MXIC Manufacture ID */
#define MID_MICRON			0x20    /* Micron Manufacture ID */
#define MID_GD				0xc8    /* GD Manufacture ID */
#define MID_ESMT			0x8c    /* ESMT Manufacture ID */
#define MID_CFEON			0x1c    /* CFeon Manufacture ID */

#define BP_NUM_3                        3
#define BP_NUM_4                        4

#define DEBUG_SPI_NOR_BP	0

#define SPI_NOR_SR_SRWD_SHIFT	7
#define SPI_NOR_SR_SRWD_MASK	(1 << SPI_NOR_SR_SRWD_SHIFT)

#define SPI_NOR_SR_BP0_SHIFT    2
#define SPI_NOR_SR_BP_WIDTH_4   0xf
#define SPI_NOR_SR_BP_MASK_4    (SPI_NOR_SR_BP_WIDTH_4 << SPI_NOR_SR_BP0_SHIFT)

#define SPI_NOR_SR_BP_WIDTH_3   0x7
#define SPI_NOR_SR_BP_MASK_3    (SPI_NOR_SR_BP_WIDTH_3 << SPI_NOR_SR_BP0_SHIFT)

#define SPI_NOR_SR_TB_SHIFT	3
#define SPI_NOR_SR_TB_MASK	(1 << SPI_NOR_SR_TB_SHIFT)

#define LOCK_LEVEL_MAX(bp_num)	(((0x01) << bp_num) - 1)

#endif /* CONFIG_SPI_BLOCK_PROTECT */

#define SPI_NOR_SR_WIP_MASK	(1 << 0)

/*****************************************************************************/
struct hifmc_host {
	struct mtd_info mtd[1];
	struct hifmc_spi spi[CONFIG_SPI_NOR_MAX_CHIP_NUM];

	void __iomem *regbase;
	void __iomem *iobase;

	char *buffer;
	unsigned int dma_buffer;

	void (*set_system_clock)(struct spi_op *op, int clk_en);
	void (*set_host_addr_mode)(struct hifmc_host *host, int enable);

	struct device *dev;
	struct mutex lock;

	int add_partition;
	int num_chip;

#ifdef CONFIG_PM
	int (*suspend)(struct platform_device *pltdev, pm_message_t state);
	int (*resume)(struct platform_device *pltdev);
#endif

#ifdef CONFIG_SPI_BLOCK_PROTECT
	unsigned int start_addr;
	unsigned int end_addr;
	unsigned char cmp;
	unsigned char mid;
	unsigned int lock_level_max;
	unsigned char level;
#endif
};

#ifdef CONFIG_PM
void hifmc100_spi_nor_config(struct hifmc_host *host);
#endif

#ifdef CONFIG_SPI_BLOCK_PROTECT
unsigned short hifmc100_get_spi_lock_info(struct hifmc_host *host);
unsigned char hifmc100_bp_to_level(struct hifmc_host *host,
				unsigned int bp_num);
void spi_lock_update_address(struct hifmc_host *host);
extern u_char spi_general_get_flash_register(struct hifmc_spi *spi, u_char cmd);
#endif

#define MTD_TO_HOST(_mtd)		((struct hifmc_host *)(_mtd))

/*****************************************************************************/
void hifmc100_read_ids(struct hifmc_spi *, u_char, u_char *);

extern int hifmc_spi_nor_probe(struct mtd_info *mtd, struct hifmc_spi *spi);

extern void hifmc_set_nor_system_clock(struct spi_op *op, int clk_en);

/*****************************************************************************/

#endif /* End of __HIFMC100_H__ */

