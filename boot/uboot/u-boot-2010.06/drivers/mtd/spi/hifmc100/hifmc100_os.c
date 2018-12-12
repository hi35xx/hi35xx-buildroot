/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
 *
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*****************************************************************************/
#include <common.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <hifmc_common.h>

#include "../../hifmc_spi_ids.h"
#include "hifmc100.h"
#include "hifmc100_os.h"

/*****************************************************************************/
static struct hifmc_host hifmc100_host;
static struct mtd_info_ex hifmc100_spi_nor_info = {.type = 0, };

/*****************************************************************************/
static void hifmc100_driver_shutdown(void)
{
	if (start_up_addr_mode == SPI_NOR_ADDR_MODE_3_BYTES) {
		int ix;
		struct hifmc_host *host = &hifmc100_host;
		struct hifmc_spi *spi = host->spi;
		struct mtd_info_ex *spi_nor_info = &hifmc100_spi_nor_info;

		hifmc_dev_type_switch(FLASH_TYPE_SPI_NOR);

		for (ix = 0; ix < spi_nor_info->numchips; ix++, spi++) {
			if (spi->addrcycle == 4) {
				spi->driver->wait_ready(spi);
				spi->driver->entry_4addr(spi, DISABLE);
			}
		}
	}
}

/*****************************************************************************/
static int hifmc100_driver_probe(void)
{
	int ret = 0;
	struct hifmc_host *host = &hifmc100_host;

	FMC_PR(BT_DBG, "\t|*-Start SPI nor flash driver probe\n");

	/* Hifmc ip version check */
	ret = hifmc_ip_ver_check();
	if (ret) {
		FMC_PR(BT_DBG, "\t|*-IP version unknown, result: %d\n", ret);
		return ret;
	}

	FMC_PR(BT_DBG, "\t||-SPI nor host init\n");
	memset((char *)host, 0, sizeof(struct hifmc_host));
	ret = hifmc100_spi_nor_init(host);
	if (ret) {
		FMC_PR(BT_DBG, "Error: SPI Nor init failed, ret: %d\n", ret);
		goto end;
	}

end:
	FMC_PR(BT_DBG, "\t|*-End SPI nor flash driver probe\n");

	return ret;
}

/*****************************************************************************/
struct mtd_info_ex *hifmc100_get_spi_nor_info(struct spi_flash *spi_nor_flash)
{
	if (hifmc100_spi_nor_info.type == 0) {
		if (hifmc100_spi_nor_probe(NULL) == NULL)
			return NULL;
	}

	return &hifmc100_spi_nor_info;
}

/*****************************************************************************/
static void hifmc100_probe_spi_size(struct spi_flash *spi_nor_flash)
{
	struct hifmc_host *host = &hifmc100_host;
	struct hifmc_spi *spi = host->spi;
	unsigned int ix, total = 0;
	struct mtd_info_ex *spi_nor_info = host->spi_nor_info;

	FMC_PR(BT_DBG, "\t|*-Start probe SPI nor flash total size\n");

	for (ix = 0; ix < spi_nor_info->numchips; ix++, spi++) {
		FMC_PR(BT_DBG, "\t||-SPI nor flash[%d]: %dMB\n", ix,
				(u_int)(spi->chipsize >> 20));
		total += spi->chipsize;
	}

	spi_nor_flash->size = total;

	FMC_PR(BT_DBG, "\t|*-Probe SPI nor total size: %dMB, chip num: %d\n",
		spi_nor_flash->size >> 20, spi_nor_info->numchips);
}

/*****************************************************************************/
struct spi_flash *hifmc100_spi_nor_probe(struct mtd_info_ex **spi_nor_info)
{
	static struct spi_flash *spi_nor_flash;

	FMC_PR(BT_DBG, "\t*-Start SPI Nor flash probe\n");

	if (spi_nor_flash) {
		FMC_PR(BT_DBG, "\t*-SPI Nor flash is initialized.\n");
		return spi_nor_flash;
	}

	/* Check current SPI device type whether SPI nor */
	hifmc_dev_type_switch(FLASH_TYPE_SPI_NOR);

	FMC_PR(BT_DBG, "\t|-SPI Nor flash driver probe\n");
	if (!hifmc100_driver_probe()) {
		struct hifmc_host *host = &hifmc100_host;

		FMC_PR(BT_DBG, "\t|-SPI nor flash scanning\n");
		host->spi_nor_info = &hifmc100_spi_nor_info;
		spi_nor_flash = hifmc100_spi_nor_scan(host);
		if (spi_nor_flash) {
			*spi_nor_info =
				hifmc100_get_spi_nor_info(spi_nor_flash);

			hifmc100_probe_spi_size(spi_nor_flash);
			printf("SPI Nor total size: %uMB\n",
					spi_nor_flash->size >> 20);
			FMC_PR(BT_DBG, "\t|-Add func hook for Reset cmd\n");
			add_shutdown(hifmc100_driver_shutdown);

			goto end;
		}
	}

	spi_nor_flash = NULL;
	hifmc100_spi_nor_info.type = 0;

end:
	/* Change SPI device type to default */
	hifmc_dev_type_switch(FLASH_TYPE_DEFAULT);

	FMC_PR(BT_DBG, "\t*-End SPI Nor flash probe\n");

	return spi_nor_flash;
}

/*****************************************************************************/
#ifdef CONFIG_SPI_BLOCK_PROTECT
void hifmc100_spi_flash_lock(unsigned char cmp, unsigned char level,
		unsigned char op)
{
	struct hifmc_host *host = &hifmc100_host;
	struct spi_flash *nor = host->spi_nor_flash;

	host->cmp = cmp;

	if (BP_OP_GET == op) {
		puts("Get spi lock information\n");
		if (host->level) {
			if (host->level == nor->bp_level_max)
				puts("all blocks are locked.\n");
			else
				printf("level: %d\n", host->level);
			printf("Spi is locked. lock address[0 => %#x]\n",
					host->end_addr);
		} else
			puts("all blocks are unlocked.\n");

		return;
	}

	if (BP_OP_SET == op) {
		if (level) {
			if (level == nor->bp_level_max)
				puts("lock all blocks.\n");
			else
				printf("lock level: %d\n", level);
		} else
			puts("unlock all block.\n");

		hifmc100_spi_lock(host, level);
		return;
	}

	printf("%s ERROR: Invalid optin argument!", __func__);
}
/*****************************************************************************/
#endif /* CONFIG_SPI_BLOCK_PROTECT */
