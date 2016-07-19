/* linux/drivers/spi/spidev_info-hi3516a.c
 *
 * HISILICON SPI Controller driver
 *
 * Copyright (c) 2015 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * History:
 *      31-July-2015 create this file
 */

#include <linux/module.h>
#include <linux/spi/spi.h>

#define DEFAULT_SPEED_HZ	2000000

/*
 * SPI device information
 */
static struct spi_board_info hi3516a_spidev_info[] = {
	{
		.modalias       = "spidev",
		.bus_num        = 0,
		.chip_select    = 0,
		.max_speed_hz   = DEFAULT_SPEED_HZ,
		.mode           = SPI_MODE_0,
	},
	{
		.modalias       = "spidev",
		.bus_num        = 1,
		.chip_select    = 0,
		.max_speed_hz   = DEFAULT_SPEED_HZ,
		.mode           = SPI_MODE_0,
	},
	{
		.modalias       = "spidev",
		.bus_num        = 1,
		.chip_select    = 1,
		.max_speed_hz   = DEFAULT_SPEED_HZ,
		.mode           = SPI_MODE_0,
	},
	{
		.modalias       = "spidev",
		.bus_num        = 1,
		.chip_select    = 2,
		.max_speed_hz   = DEFAULT_SPEED_HZ,
		.mode           = SPI_MODE_0,
	}
};

int hi_spidev_get_info(struct spi_board_info **info)
{

	*info = hi3516a_spidev_info;

	return ARRAY_SIZE(hi3516a_spidev_info);
}
EXPORT_SYMBOL(hi_spidev_get_info);
