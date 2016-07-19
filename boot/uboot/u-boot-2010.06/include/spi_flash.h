/*
 * Interface to SPI flash
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <spi.h>
#include <linux/types.h>

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
#define BP_OP_SET	0
#define BP_OP_GET	1

#define BP_CMP_TOP	0
#define BP_CMP_BOTTOM	1
#define BP_CMP_UPDATE_FLAG	0xff

enum block_protection_level {
	BP_LEVEL_0	= 0,
	BP_LEVEL_1	= 1,
	BP_LEVEL_2	= 2,
	BP_LEVEL_3	= 3,
	BP_LEVEL_4	= 4,
	BP_LEVEL_5	= 5,
	BP_LEVEL_6	= 6,
	BP_LEVEL_7	= 7,
	BP_LEVEL_8	= 8,
	BP_LEVEL_9	= 9,
	BP_LEVEL_10	= 10,
	BP_LEVEL_END,
};

#define BP_LEVEL_MAX	(BP_LEVEL_END - 1)

void spi_flash_lock(unsigned char cmp, unsigned char level, unsigned char op);

#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

struct spi_flash_region {
	unsigned int	count;
	unsigned int	size;
};

struct spi_flash {
	struct spi_slave *spi;

	const char	*name;

	u32		size;

	int		(*read)(struct spi_flash *flash, u32 offset,
				size_t len, void *buf);
	int		(*write)(struct spi_flash *flash, u32 offset,
				size_t len, const void *buf);
	int		(*erase)(struct spi_flash *flash, u32 offset,
				size_t len);
};

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode);
void spi_flash_free(struct spi_flash *flash);

static inline int spi_flash_read(struct spi_flash *flash, u32 offset,
		size_t len, void *buf)
{
	return flash->read(flash, offset, len, buf);
}

static inline int spi_flash_write(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf)
{
	return flash->write(flash, offset, len, buf);
}

static inline int spi_flash_erase(struct spi_flash *flash, u32 offset,
		size_t len)
{
	return flash->erase(flash, offset, len);
}

#endif /* _SPI_FLASH_H_ */
