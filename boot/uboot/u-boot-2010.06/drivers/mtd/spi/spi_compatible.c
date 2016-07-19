/******************************************************************************
*    Copyright (c) 2009-2011 by CCC.
*    All rights reserved.
* ***
*    Create by CCC. 2011-12-02
*
******************************************************************************/

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <asm-generic/errno.h>
#include <linux/mtd/mtd.h>

/*****************************************************************************/

#if (CONFIG_HISFC_FLASH_GODNET) || (CONFIG_HISFC_FLASH_GODCUBE) ||\
	(CONFIG_HISFC_FLASH_GODARM)
extern int hisfc300new_spiflash_init(struct spi_flash **spiflash,
		struct mtd_info_ex **spiinfo);
#endif

#ifdef CONFIG_SPI_FLASH_HISFC300
extern int hisfc300_spiflash_init(struct spi_flash **spiflash,
		struct mtd_info_ex **spiinfo);
#endif

#ifdef CONFIG_SPI_FLASH_HISFC350
extern int hisfc350_spiflash_init(struct spi_flash **spiflash,
		struct mtd_info_ex **spiinfo);
#endif

#ifdef CONFIG_HIFMC_SPI_NOR
extern struct spi_flash *hifmc100_spi_nor_probe(struct mtd_info_ex **);
extern struct mtd_info_ex *hifmc100_get_spi_nor_info(struct spi_flash *);
#endif

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
extern unsigned hisfc350_get_spi_lock_info(unsigned char *cmp,
					unsigned char *level);
extern void hisfc350_spi_lock(unsigned char cmp, unsigned char level);
#endif

/*****************************************************************************/

static struct spi_flash *spiflash;
static struct mtd_info_ex *spiinfo_ex;
/*****************************************************************************/

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
	unsigned int max_hz, unsigned int spi_mode)
{
	if (spiflash)
		return spiflash;

#if (CONFIG_HISFC_FLASH_GODNET) || (CONFIG_HISFC_FLASH_GODCUBE) ||\
	(CONFIG_HISFC_FLASH_GODARM)
	hisfc300new_spiflash_init(&spiflash, &spiinfo_ex);
#endif

#ifdef CONFIG_SPI_FLASH_HISFC300
	hisfc300_spiflash_init(&spiflash, &spiinfo_ex);
#endif

#ifdef CONFIG_SPI_FLASH_HISFC350
	hisfc350_spiflash_init(&spiflash, &spiinfo_ex);
#endif

#ifdef CONFIG_HIFMC_SPI_NOR
	spiflash = hifmc100_spi_nor_probe(&spiinfo_ex);
#endif

	return spiflash;
}
/*****************************************************************************/

struct mtd_info_ex *get_spiflash_info(void)
{
	if (spiinfo_ex)
		return spiinfo_ex;

#if (CONFIG_HISFC_FLASH_GODNET) || (CONFIG_HISFC_FLASH_GODCUBE)
	hisfc300new_spiflash_init(&spiflash, &spiinfo_ex);
#endif

#ifdef CONFIG_SPI_FLASH_HISFC300
	hisfc300_spiflash_init(&spiflash, &spiinfo_ex);
#endif

#ifdef CONFIG_SPI_FLASH_HISFC350
	hisfc350_spiflash_init(&spiflash, &spiinfo_ex);
#endif

#ifdef CONFIG_HIFMC_SPI_NOR
	spiinfo_ex = hifmc100_get_spi_nor_info(spiflash);
#endif

	return spiinfo_ex;
}
/*****************************************************************************/

void spi_flash_free(struct spi_flash *flash)
{
}
/*****************************************************************************/

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
void spi_flash_lock(unsigned char cmp, unsigned char level, unsigned char op)
{
#if defined(CONFIG_SPI_FLASH_HISFC350) || defined(CONFIG_HIFMC_SPI_NOR)
	if (BP_OP_GET == op) {
		puts("Get spi lock information\n");
		cmp = BP_CMP_UPDATE_FLAG;
#ifdef CONFIG_SPI_FLASH_HISFC350
		hisfc350_get_spi_lock_info(&cmp, &level);
#endif
#ifdef CONFIG_HIFMC_SPI_NOR
		hifmc100_get_spi_lock_info(&cmp, &level);
#endif
		if (level) {
			if (level == BP_LEVEL_MAX)
				puts("all block is locked.\n");
			else
				printf("level: %d, start of %s address.\n",
					level, (cmp ? "bottom" : "top"));
		} else
			puts("all block is unlocked.\n");

		return;
	}

	if (BP_OP_SET == op) {
		if (level) {
			if (level == BP_LEVEL_MAX) {
				cmp = BP_CMP_TOP;
				puts("lock all block.\n");
			} else
				printf("Set lock level: %d, start of %s " \
					"address\n", level,
					(cmp ? "bottom" : "top"));
		} else {
			cmp = BP_CMP_TOP;
			puts("unlock all block.\n");
		}

#ifdef CONFIG_SPI_FLASH_HISFC350
		hisfc350_spi_lock(cmp, level);
#endif
#ifdef CONFIG_HIFMC_SPI_NOR
		hifmc100_spi_lock(cmp, level);
#endif
		return;
	}

	printf("%s ERROR: Invalid optin argument!", __func__);
#endif
}
/*****************************************************************************/
#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

