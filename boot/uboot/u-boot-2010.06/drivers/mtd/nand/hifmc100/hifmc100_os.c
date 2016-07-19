/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <common.h>
#include <linux/mtd/nand.h>
#include <hifmc_common.h>
#include <hinfc_common.h>

#include "../../hifmc_spi_ids.h"
#include "hifmc100_os.h"
#include "hifmc100.h"

/*****************************************************************************/
static struct hifmc_host hifmc100_host = {
	.chip = NULL,
};

/*****************************************************************************/
static void hifmc100_driver_probe(struct nand_chip *chip, unsigned char cs)
{
	int ret = 0;
	struct hifmc_host *host = &hifmc100_host;

	FMC_PR(BT_DBG, "\t*-Start SPI Nand flash driver probe\n");

	if (!host->chip) {
		/* Hifmc ip version check */
		if (hifmc_ip_ver_check())
			DB_BUG("Error: hifmc IP version unknown!\n");

		/* Hifmc current SPI device type check */
		hifmc_dev_type_switch(FLASH_TYPE_SPI_NAND);

		/* Hifmc SPI nand init */
		memset((char *)host, 0, sizeof(struct hifmc_host));
		ret = hifmc100_host_init(host);
		if (ret) {
			DB_MSG("Error: Host init failed, result: %d\n", ret);
			/* Change SPI device type to default */
			hifmc_dev_type_switch(FLASH_TYPE_DEFAULT);
			return;
		}
	} else
		FMC_PR(BT_DBG, "\t*-SPI Nand host is initialized.\n");

	host->cmd_op.cs = cs;
	host->chip = chip;
	hifmc100_spi_nand_init(host);

	FMC_PR(BT_DBG, "\t*-End SPI Nand flash driver probe.\n");

	return;
}

/*****************************************************************************/
int board_nand_init(struct nand_chip *chip)
{
	unsigned char chip_num = CONFIG_SPI_NAND_MAX_CHIP_NUM;
	static unsigned char cs;

	for (cs = 0; chip_num && (cs < CONFIG_HIFMC_MAX_CS_NUM); cs++) {
		if (hifmc_cs_user[cs]) {
			FMC_PR(BT_DBG, "\t\t*-Current CS(%d) is occupied.\n",
					cs);
			continue;
		}

		hifmc100_driver_probe(chip, cs);
		chip_num--;
	}

	return 0;
}

/*****************************************************************************/
static int hifmc100_spi_nand_get_ecctype(void)
{
	struct hifmc_host *host = &hifmc100_host;

	if (!host->chip) {
		printf("SPI Nand flash uninitialized.\n");
		return -1;
	}

	return match_ecc_type_to_yaffs(hifmc100_host.ecctype);
}

/*****************************************************************************/
int nand_get_ecctype(void)
{
#ifdef CONFIG_HIFMC_SPI_NAND
	return hifmc100_spi_nand_get_ecctype();
#endif

	return -1;
}

