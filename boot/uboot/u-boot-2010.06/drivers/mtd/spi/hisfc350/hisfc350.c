/******************************************************************************
 *    Copyright (c) 2009-2010 by Hisilicon
 *    All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>

#include "../spi_ids.h"
#include "hisfc350.h"
#define DEBUG 0
#define THREE_BYTE_ADDR_BOOT 0
/*****************************************************************************/
/* Don't change the follow config */
#define HISFC350_SUPPORT_READ (SPI_IF_READ_STD \
	| SPI_IF_READ_FAST \
	| SPI_IF_READ_DUAL \
	| SPI_IF_READ_DUAL_ADDR \
	| SPI_IF_READ_QUAD \
	| SPI_IF_READ_QUAD_ADDR)

#define HISFC350_SUPPORT_WRITE (SPI_IF_WRITE_STD \
	| SPI_IF_WRITE_DUAL \
	| SPI_IF_WRITE_DUAL_ADDR \
	| SPI_IF_WRITE_QUAD \
	| SPI_IF_WRITE_QUAD_ADDR)

#define HISFC350_SUPPORT_MAX_DUMMY        (7)

/*
#undef HISFCV350_SUPPORT_REG_READ
*/
/*****************************************************************************/
static int  start_up_mode;
static struct hisfc_host hisfc_host;
static struct mtd_info_ex spi_info_ex = {.type = 0, };

extern struct spi_info hisfc350_spi_info_table[];

#ifdef CONFIG_HI3518
#  include "hisfc350_3518.c"
#endif

#ifdef CONFIG_HI3520D
#  include "hisfc350_hi3520d.c"
#endif

#ifdef CONFIG_HI3535
#  include "hisfc350_hi3535.c"
#endif

#ifndef GET_SFC_ADDR_MODE
#define GET_SFC_ADDR_MODE  (0)
#endif
/*****************************************************************************/
#ifdef CONFIG_HISFC350_SHOW_CYCLE_TIMING
static char *hisfc350_get_ifcycle_str(int ifcycle)
{
	static char *ifcycle_str[] = {
		"single",
		"dual",
		"dual-addr",
		"dual-cmd",
		"reserve",
		"quad",
		"quad-addr",
		"quad-cmd",
	};

	return ifcycle_str[(ifcycle & 0x07)];
}
#endif /* CONFIG_HISFC350_SHOW_CYCLE_TIMING */
/*****************************************************************************/

static void hisfc350_set_host_addr_mode(struct hisfc_host *host, int enable)
{
	unsigned int regval;
	regval = hisfc_read(host, HISFC350_GLOBAL_CONFIG);
	if (enable)
		regval |= HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;
	else
		regval &= ~HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;

	hisfc_write(host, HISFC350_GLOBAL_CONFIG, regval);
}
/*****************************************************************************/
static void hisfc350_shutdown(void)
{
	if (start_up_mode == THREE_BYTE_ADDR_BOOT) {

		int ix;
		struct hisfc_host *host = &hisfc_host;
		struct hisfc_spi *spi = host->spi;

		for (ix = 0; ix < host->num_chip; ix++, spi++) {
			if (spi->addrcycle == 4) {
				spi->driver->wait_ready(spi);
				spi->driver->entry_4addr(spi, 0);
			}
		}
	}
}
/*****************************************************************************/
static void hisfc350_map_iftype_and_clock(struct hisfc_spi *spi)
{
	int ix;
	const int iftype_read[] = {
		SPI_IF_READ_STD,       HISFC350_IFCYCLE_STD,
		SPI_IF_READ_FAST,      HISFC350_IFCYCLE_STD,
		SPI_IF_READ_DUAL,      HISFC350_IFCYCLE_DUAL,
		SPI_IF_READ_DUAL_ADDR, HISFC350_IFCYCLE_DUAL_ADDR,
		SPI_IF_READ_QUAD,      HISFC350_IFCYCLE_QUAD,
		SPI_IF_READ_QUAD_ADDR, HISFC350_IFCYCLE_QUAD_ADDR,
		0, 0,
	};
	const int iftype_write[] = {
		SPI_IF_WRITE_STD,       HISFC350_IFCYCLE_STD,
		SPI_IF_WRITE_DUAL,      HISFC350_IFCYCLE_DUAL,
		SPI_IF_WRITE_DUAL_ADDR, HISFC350_IFCYCLE_DUAL_ADDR,
		SPI_IF_WRITE_QUAD,      HISFC350_IFCYCLE_QUAD,
		SPI_IF_WRITE_QUAD_ADDR, HISFC350_IFCYCLE_QUAD_ADDR,
		0, 0,
	};

	for (ix = 0; iftype_write[ix]; ix += 2) {
		if (spi->write->iftype == iftype_write[ix]) {
			spi->write->iftype = iftype_write[ix + 1];
			break;
		}
	}
	hisfc350_get_best_clock(&spi->write->clock);

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}
	hisfc350_get_best_clock(&spi->read->clock);

	hisfc350_get_best_clock(&spi->erase->clock);
	spi->erase->iftype = HISFC350_IFCYCLE_STD;
}
/*****************************************************************************/
static void hisfc350_dma_transfer(struct hisfc_host *host,
	unsigned int spi_start_addr, unsigned char *dma_buffer,
	unsigned char is_read, unsigned int size, unsigned char chipselect)
{
	hisfc_write(host, HISFC350_BUS_DMA_MEM_SADDR, dma_buffer);

	hisfc_write(host, HISFC350_BUS_DMA_FLASH_SADDR,
		spi_start_addr);

	hisfc_write(host, HISFC350_BUS_DMA_LEN,
		HISFC350_BUS_DMA_LEN_DATA_CNT(size));

	hisfc_write(host, HISFC350_BUS_DMA_AHB_CTRL,
		HISFC350_BUS_DMA_AHB_CTRL_INCR4_EN
		| HISFC350_BUS_DMA_AHB_CTRL_INCR8_EN
		| HISFC350_BUS_DMA_AHB_CTRL_INCR16_EN);

	hisfc_write(host, HISFC350_BUS_DMA_CTRL,
		HISFC350_BUS_DMA_CTRL_RW(is_read)
		| HISFC350_BUS_DMA_CTRL_CS(chipselect)
		| HISFC350_BUS_DMA_CTRL_START);

	HISFC350_DMA_WAIT_CPU_FINISH(host);

}
/*****************************************************************************/
#ifdef HISFCV350_SUPPORT_REG_READ
static char *hisfc350_reg_read_buf(struct hisfc_host *host,
	struct hisfc_spi *spi, unsigned int spi_start_addr,
	unsigned int size, unsigned char *buffer)
{
	int numread;
	int index = 0;
	unsigned char *ptr = buffer;

	if (size > HISFC350_REG_BUF_SIZE)
		DBG_BUG("reg read out of reg range.\n");

	hisfc_write(host, HISFC350_CMD_INS, spi->read->cmd);
	hisfc_write(host, HISFC350_CMD_ADDR,
		(spi_start_addr & HISFC350_CMD_ADDR_MASK));
	hisfc_write(host, HISFC350_CMD_CONFIG,
		HISFC350_CMD_CONFIG_MEM_IF_TYPE(spi->read->iftype)
		| HISFC350_CMD_CONFIG_DATA_CNT(size)
		| HISFC350_CMD_CONFIG_RW_READ
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_DUMMY_CNT(spi->read->dummy)
		| HISFC350_CMD_CONFIG_ADDR_EN
		| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	numread = (size & ~0x03);
	while (numread) {
		*(unsigned int *)ptr = hisfc_read(host,
			HISFC350_CMD_DATABUF0 + index);
		ptr      += 4;
		index    += 4;
		numread  -= 4;
	}

	numread = (size & 0x03);
	if (numread) {
		char tmp[4];
		*(unsigned int *)tmp = hisfc_read(host,
			HISFC350_CMD_DATABUF0 + index);
		memcpy(ptr, tmp, numread);
	}

	return buffer;
}
/*****************************************************************************/
static int hisfc350_reg_read(struct spi_flash *spiflash, u32 from,
	size_t len, void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hisfc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hisfc_spi *spi = host->spi;


	if ((from + len) > spiflash->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	if (spi->driver->wait_ready(spi))
		goto fail;
	host->set_system_clock(host, spi->read, TRUE);

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");
			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(host, spi->read, TRUE);
		}

		num = ((from + len) >= spi->chipsize)
			? (spi->chipsize - from) : len;

		while (num >= HISFC350_REG_BUF_SIZE) {
			hisfc350_reg_read_buf(host, spi,
				from, HISFC350_REG_BUF_SIZE, ptr);
			ptr  += HISFC350_REG_BUF_SIZE;
			from += HISFC350_REG_BUF_SIZE;
			len  -= HISFC350_REG_BUF_SIZE;
			num  -= HISFC350_REG_BUF_SIZE;
		}

		if (num) {
			hisfc350_reg_read_buf(host, spi,
				from, num, ptr);
			from += num;
			ptr  += num;
			len  -= num;
		}
	}
	result = 0;
fail:

	return result;
}
#endif /* HISFCV350_SUPPORT_REG_READ */
/*****************************************************************************/
static int hisfc350_dma_read(struct spi_flash *spiflash, u32 from,
	size_t len, void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hisfc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hisfc_spi *spi = host->spi;


	if ((from + len) > spiflash->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	if (spi->driver->wait_ready(spi))
		goto fail;
	spi->driver->bus_prepare(spi, READ);

	if (from & HISFC350_DMA_ALIGN_MASK) {
		num = HISFC350_DMA_ALIGN_SIZE -
			(from & HISFC350_DMA_ALIGN_MASK);
		if (num > len)
			num = len;
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");
			if (spi->driver->wait_ready(spi))
				goto fail;
			spi->driver->bus_prepare(spi, READ);
		}
		hisfc350_dma_transfer(host, from,
			(unsigned char *)host->dma_buffer, READ,
			num, spi->chipselect);
		memcpy(ptr, host->buffer, num);
		from  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");
			if (spi->driver->wait_ready(spi))
				goto fail;
			spi->driver->bus_prepare(spi, READ);
		}

		num = ((from + len) >= spi->chipsize)
			? (spi->chipsize - from) : len;
		while (num >= HISFC350_DMA_MAX_SIZE) {
			hisfc350_dma_transfer(host, from,
				(unsigned char *)host->dma_buffer, READ,
				HISFC350_DMA_MAX_SIZE, spi->chipselect);
			memcpy(ptr, host->buffer, HISFC350_DMA_MAX_SIZE);
			ptr  += HISFC350_DMA_MAX_SIZE;
			from += HISFC350_DMA_MAX_SIZE;
			len  -= HISFC350_DMA_MAX_SIZE;
			num  -= HISFC350_DMA_MAX_SIZE;
		}

		if (num) {
			hisfc350_dma_transfer(host, from,
				(unsigned char *)host->dma_buffer, READ,
				num, spi->chipselect);
			memcpy(ptr, host->buffer, num);
			from += num;
			ptr  += num;
			len  -= num;
		}
	}
	result = 0;
fail:

	return result;
}
/*****************************************************************************/
static unsigned char *hisfc350_read_ids(struct hisfc_host *host,
	int chipselect, unsigned char *buffer)
{
	int regindex = 0;
	int numread = 8;
	unsigned int *ptr = (unsigned int *)buffer;

	if (numread > HISFC350_REG_BUF_SIZE)
		numread = HISFC350_REG_BUF_SIZE;

	hisfc_write(host, HISFC350_CMD_INS, SPI_CMD_RDID);
	hisfc_write(host, HISFC350_CMD_CONFIG,
		HISFC350_CMD_CONFIG_SEL_CS(chipselect)
		| HISFC350_CMD_CONFIG_RW_READ
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_DATA_CNT(numread)
		| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	while (numread) {
		*ptr = hisfc_read(host,
			HISFC350_CMD_DATABUF0 + regindex);
		ptr      += 1;
		regindex += 4;
		numread  -= 4;
	}

	return buffer;
}
/*****************************************************************************/
static int hisfc350_reg_erase_one_block(struct hisfc_host *host,
	struct hisfc_spi *spi, unsigned int offset)
{
	if (spi->driver->wait_ready(spi))
		return 1;
	spi->driver->write_enable(spi);
	host->set_system_clock(host, spi->erase, TRUE);

	hisfc_write(host, HISFC350_CMD_INS, spi->erase->cmd);

	hisfc_write(host, HISFC350_CMD_ADDR,
		(offset & HISFC350_CMD_ADDR_MASK));

	hisfc_write(host, HISFC350_CMD_CONFIG,
		HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_MEM_IF_TYPE(spi->erase->iftype)
		| HISFC350_CMD_CONFIG_DUMMY_CNT(spi->erase->dummy)
		| HISFC350_CMD_CONFIG_ADDR_EN
		| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	return 0;
}
/*****************************************************************************/
static int hisfc350_dma_write(struct spi_flash *spiflash, u32 to, size_t len,
	const void *buf)
{
	int num;
	int result = -EIO;

	unsigned char *ptr = (unsigned char *)buf;
	struct hisfc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hisfc_spi *spi = host->spi;

	if ((to + len) > spiflash->size) {
		DBG_MSG("write data out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("write length is 0.\n");
		return 0;
	}

	if (spi->driver->wait_ready(spi))
		goto fail;

	spi->driver->write_enable(spi);
	spi->driver->bus_prepare(spi, WRITE);

	if (to & HISFC350_DMA_ALIGN_MASK) {
		num = HISFC350_DMA_ALIGN_SIZE - (to & HISFC350_DMA_ALIGN_MASK);
		if (num > len)
			num = len;
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");
			if (spi->driver->wait_ready(spi))
				goto fail;
			spi->driver->write_enable(spi);
			spi->driver->bus_prepare(spi, WRITE);
		}
		memcpy(host->buffer, ptr, num);
		hisfc350_dma_transfer(host, to,
			(unsigned char *)host->dma_buffer, WRITE,
			num, spi->chipselect);

		to  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0) {
		num = ((len >= HISFC350_DMA_MAX_SIZE)
			? HISFC350_DMA_MAX_SIZE : len);
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");
			if (spi->driver->wait_ready(spi))
				goto fail;
			spi->driver->write_enable(spi);
			spi->driver->bus_prepare(spi, WRITE);
		}

		memcpy(host->buffer, ptr, num);
		hisfc350_dma_transfer(host, to,
			(unsigned char *)host->dma_buffer, WRITE,
			num, spi->chipselect);

		to  += num;
		ptr += num;
		len -= num;
	}

	result = 0;
fail:
	return result;
}
/*****************************************************************************/
#ifdef HISFCV350_SUPPORT_REG_WRITE
static int hisfc350_reg_write_buf(struct hisfc_host *host,
	struct hisfc_spi *spi, u32 spi_start_addr,
	size_t size, unsigned char *buffer)
{
	int index = 0;

	if (size > HISFC350_REG_BUF_SIZE)
		DBG_BUG("reg read out of reg range.\n");

	if (spi->driver->wait_ready(spi))
		return 1;

	memcpy(host->buffer, buffer, size);

	while (index < size) {
		hisfc_write(host, HISFC350_CMD_DATABUF0 + index,
			*(unsigned int *)(host->buffer + index));
		index    += 4;
	}

	spi->driver->write_enable(spi);

	hisfc_write(host, HISFC350_CMD_INS, spi->write->cmd);
	hisfc_write(host, HISFC350_CMD_ADDR,
		(spi_start_addr & HISFC350_CMD_ADDR_MASK));
	hisfc_write(host, HISFC350_CMD_CONFIG,
		HISFC350_CMD_CONFIG_MEM_IF_TYPE(spi->write->iftype)
		| HISFC350_CMD_CONFIG_DATA_CNT(size)
		| HISFC350_CMD_CONFIG_DATA_EN
		| HISFC350_CMD_CONFIG_DUMMY_CNT(spi->write->dummy)
		| HISFC350_CMD_CONFIG_ADDR_EN
		| HISFC350_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC350_CMD_CONFIG_START);

	HISFC350_CMD_WAIT_CPU_FINISH(host);

	return 0;
}

/*****************************************************************************/
static int hisfc350_reg_write(struct spi_flash *spiflash, u32 to, size_t len,
	const void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hisfc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hisfc_spi *spi = host->spi;


	if ((to + len) > spiflash->size) {
		DBG_MSG("write data out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("write length is 0.\n");
		return 0;
	}

	if (spi->driver->wait_ready(spi))
		goto fail;
	host->set_system_clock(host, spi->write, TRUE);

	if (to & HISFC350_REG_BUF_MASK) {
		num = HISFC350_REG_BUF_SIZE - (to & HISFC350_REG_BUF_MASK);
		if (num > (int)len)
			num = (int)len;

		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(host, spi->write, TRUE);
		}
		if (hisfc350_reg_write_buf(host, spi, to, num, ptr))
			goto fail;
		to  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0)	{
		num = ((len >= HISFC350_REG_BUF_SIZE) ?
				HISFC350_REG_BUF_SIZE : len);
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(host, spi->write, TRUE);
		}
		if (hisfc350_reg_write_buf(host, spi, to, num, ptr))
			goto fail;
		to  += num;
		ptr += num;
		len -= num;
	}
	result = 0;
fail:
	return result;
}
#endif /* HISFCV350_SUPPORT_REG_WRITE */
/*****************************************************************************/
static int hisfc350_reg_erase(struct spi_flash *spiflash, u32 offset,
	size_t length)
{
	struct hisfc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hisfc_spi *spi = host->spi;

	if (offset + length > spiflash->size) {
		DBG_MSG("erase area out of range of mtd.\n");
		return -EINVAL;
	}

	if (offset & (host->erasesize-1)) {
		DBG_MSG("erase start address is not alignment.\n");
		return -EINVAL;
	}

	if (length & (host->erasesize-1)) {
		DBG_MSG("erase length is not alignment.\n");
		return -EINVAL;
	}

	while (length) {
		if (spi->chipsize <= offset) {
			offset -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("erase memory out of range.\n");
		}
		if (hisfc350_reg_erase_one_block(host, spi, offset))
			return -1;

		offset += spi->erase->size;
		length -= spi->erase->size;
	}

	return 0;
}
/*****************************************************************************/
#ifdef HISFCV350_SUPPORT_BUS_READ
static int hisfc350_bus_read(struct spi_flash *spiflash, u32 from,
		size_t len, void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hisfc_spi *spi = host->spi;

	if ((from + len) > spiflash->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	if (spi->driver->wait_ready(spi))
		goto fail;
	spi->driver->bus_prepare(spi, READ);

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;
			spi->driver->bus_prepare(spi, READ);
		}

		num = ((from + len) >= spi->chipsize)
				? (spi->chipsize - from) : len;

		if (num) {
			memcpy(ptr, (char *)spi->iobase + from, num);
			from += num;
			ptr  += num;
			len  -= num;
		}
	}
	result = 0;
fail:
	return result;
}
#endif /* HISFCV350_SUPPORT_BUS_READ */


#ifdef HISFCV350_SUPPORT_BUS_WRITE
static int hisfc350_bus_write(struct spi_flash *spiflash, u32 to, size_t len,
		const void *buf)
{
		int num;
		int result = -EIO;
		unsigned char *ptr = (unsigned char *)buf;
		struct hisfc_host *host = SPIFLASH_TO_HOST(spiflash);
		struct hisfc_spi *spi = host->spi;

		if ((to + len) > spiflash->size) {
				DBG_MSG("write data out of range.\n");
				return -EINVAL;
		}

		if (!len) {
				DBG_MSG("write length is 0.\n");
				return 0;
		}

		if (spi->driver->wait_ready(spi))
				goto fail;
		spi->driver->bus_prepare(spi, WRITE);

		while (len > 0) {
				while (to >= spi->chipsize) {
					to -= spi->chipsize;
					spi++;
					if (!spi->name)
						DBG_BUG(
						"write spi space out of range.\n");

					if (spi->driver->wait_ready(spi))
						goto fail;
					spi->driver->bus_prepare(spi, WRITE);
				}

				num = ((to + len) >= spi->chipsize)
					? (spi->chipsize - to) : len;

				if (num) {
					memcpy((char *)spi->iobase + to,
						ptr, num);
					ptr += num;
					to += num;
					len -= num;
				}
		}

		result = 0;
fail:
		return result;

}
#endif /* HISFCV350_SUPPORT_BUS_WRITE */

/*****************************************************************************/
static int hisfc350_map_chipsize(unsigned long long chipsize)
{
	int shift = 0;
	chipsize >>= (19 - 3); /* 19: 512K; 3: Bytes -> bit */

	while (chipsize) {
		chipsize >>= 1;
		shift++;
	}
	return shift;
}
/*****************************************************************************/
static int hisfc350_spi_probe(struct hisfc_host *host)
{
	unsigned int regval = 0;
	unsigned int total = 0;
	unsigned char ids[8];
	struct spi_info *spiinfo;
	struct hisfc_spi *spi = host->spi;
	int chipselect = (CONFIG_HISFC350_CHIP_NUM - 1);

	host->num_chip = 0;

	for (; chipselect >= 0; chipselect--) {

		hisfc350_read_ids(host, chipselect, ids);

		/* can't find spi flash device. */
		if (!(ids[0] | ids[1] | ids[2])
			|| ((ids[0] & ids[1] & ids[2]) == 0xFF))
			continue;

		printf("Spi(cs%d) ID: 0x%02X 0x%02X 0x%02X"
			" 0x%02X 0x%02X 0x%02X\n",
			chipselect,
			ids[0], ids[1], ids[2], ids[3], ids[4], ids[5]);

		spiinfo = spi_serach_ids(hisfc350_spi_info_table, ids);

		if (spiinfo) {
			spi->name = spiinfo->name;
			spi->chipselect = chipselect;
			spi->chipsize   = spiinfo->chipsize;
			spi->erasesize  = spiinfo->erasesize;
			spi->addrcycle  = spiinfo->addrcycle;
			spi->driver     = spiinfo->driver;
			spi->host       = host;

			spi_search_rw(spiinfo, spi->read,
				HISFC350_SUPPORT_READ,
				HISFC350_SUPPORT_MAX_DUMMY, READ);
			hisfc350_map_iftype_and_clock(spi);

			spi->driver->qe_enable(spi);

			spi_search_rw(spiinfo, spi->read,
				HISFC350_SUPPORT_READ,
				HISFC350_SUPPORT_MAX_DUMMY, READ);

			spi_search_rw(spiinfo, spi->write,
				HISFC350_SUPPORT_WRITE,
				HISFC350_SUPPORT_MAX_DUMMY, WRITE);

			spi_get_erase(spiinfo, spi->erase);
			hisfc350_map_iftype_and_clock(spi);

			regval = hisfc_read(host, HISFC350_BUS_FLASH_SIZE);
			regval &= ~(HISFC350_BUS_FLASH_SIZE_CS0_MASK
				<< (chipselect << 3));
			regval |= (hisfc350_map_chipsize(spi->chipsize)
				<< (chipselect << 3));
			hisfc_write(host, HISFC350_BUS_FLASH_SIZE, regval);

			hisfc_write(host,
				(HISFC350_BUS_BASE_ADDR_CS0
					+ (chipselect << 2)),
				(CONFIG_HISFC350_BUFFER_BASE_ADDRESS + total));

			spi->iobase = (char *)host->iobase + total;

			/* auto check sfc_addr_mode 3 bytes or 4 bytes */
			start_up_mode = GET_SFC_ADDR_MODE;

			if (start_up_mode == THREE_BYTE_ADDR_BOOT) {
				if (DEBUG)
					printf("start up mode is 3 bytes\n");
				spi->driver->entry_4addr(spi, TRUE);
			} else {
				if (DEBUG)
					printf("start up mode is 4 bytes\n");
			}

			printf("Spi(cs%d): ", spi->chipselect);
			printf("Block:%sB ",  ultohstr(spi->erasesize));
			printf("Chip:%sB ",   ultohstr(spi->chipsize));
			printf("Name:\"%s\"\n", spi->name);
	if (DEBUG) {
			printf("Spi(cs%d): ", spi->chipselect);
			if (spi->addrcycle == 4)
				printf("4 addr SPI ");
			else
				printf("3 addr SPI ");

			printf("read:0x%x,%02X,0x%x ",
				spi->read->iftype,
				spi->read->cmd,
				spi->read->clock);
			printf("write:0x%x,%02X,0x%x ",
				spi->write->iftype,
				spi->write->cmd,
				spi->write->clock);
			printf("erase:0x%x,%02X,0x%x\n",
				spi->erase[0].iftype,
				spi->erase[0].cmd,
				spi->erase[0].clock);
	}
			if (spi_info_ex.type == 0) {
				spi_info_ex.type      = MTD_NORFLASH;
				spi_info_ex.chipsize  = spi->chipsize;
				spi_info_ex.erasesize = spi->erasesize;
				spi_info_ex.pagesize  = 1;
				spi_info_ex.addrcycle = spi->addrcycle;

				if (spiinfo->id_len > sizeof(spi_info_ex.ids)) {
					printf(
					"BUG!! id length out of range.\n");
					BUG();
				}

				spi_info_ex.id_length = spiinfo->id_len;
				memcpy(spi_info_ex.ids, spiinfo->id,
					spiinfo->id_len);

				strncpy(spi_info_ex.name, spi->name,
					sizeof(spi_info_ex.name));
				spi_info_ex.name[sizeof(spi_info_ex.name)-1] =
					'\0';
			}
#ifdef CONFIG_HISFC350_SHOW_CYCLE_TIMING

			printf("Spi(cs%d): ", spi->chipselect);
			if (spi->addrcycle == 4)
				printf("4 addrcycle\n");
			printf("read:%s,%02X,%s ",
				hisfc350_get_ifcycle_str(spi->read->iftype),
				spi->read->cmd,
				hisfc350_get_clock_str(spi->read->clock));
			printf("write:%s,%02X,%s ",
				hisfc350_get_ifcycle_str(spi->write->iftype),
				spi->write->cmd,
				hisfc350_get_clock_str(spi->write->clock));
			printf("erase:%s,%02X,%s\n",
				hisfc350_get_ifcycle_str(spi->erase[0].iftype),
				spi->erase[0].cmd,
				hisfc350_get_clock_str(spi->erase[0].clock));

#endif /* CONFIG_HISFC350_SHOW_CYCLE_TIMING */
			host->num_chip++;
			total += spi->chipsize;
			spi++;
		} else
			printf("Spi(cs%d): find unrecognized spi flash.\n",
				chipselect);
	}
	spi_info_ex.numchips = host->num_chip;
	return host->num_chip;
}

/*****************************************************************************/
static void hisfc_probe_spi_size(struct hisfc_host *host,
	struct spi_flash *spiflash)
{
	int ix = 1;
	struct hisfc_spi *spi = host->spi;

	int total = spi->chipsize;
	int erasesize = spi->erasesize;
	if (DEBUG)
		printf("num_chip of Spi: %d\n", host->num_chip);

	for (++spi; ix < host->num_chip; ix++, spi++)
		total += spi->chipsize;

	spiflash->size = total;
	host->erasesize = erasesize;
}

/*****************************************************************************/
static int hisfc350_probe(struct hisfc_host *host)
{
	struct spi_flash *spiflash = host->spiflash;

	host->set_system_clock(host, NULL, TRUE);
	hisfc_write(host, HISFC350_TIMING,
		HISFC350_TIMING_TCSS(0x6)
		| HISFC350_TIMING_TCSH(0x6)
		| HISFC350_TIMING_TSHSL(0xf));

	if (!hisfc350_spi_probe(host))
		return -1;

	spiflash->erase = hisfc350_reg_erase;
#ifdef HISFCV350_SUPPORT_REG_WRITE
	spiflash->write = hisfc350_reg_write;
#elif defined HISFCV350_SUPPORT_BUS_WRITE
	spiflash->write = hisfc350_bus_write;
#else
	spiflash->write = hisfc350_dma_write;
#endif
#ifdef HISFCV350_SUPPORT_REG_READ
	spiflash->read  = hisfc350_reg_read;
#elif defined HISFCV350_SUPPORT_BUS_READ
	spiflash->read  = hisfc350_bus_read;
#else
	spiflash->read  = hisfc350_dma_read;
#endif

	return 0;
}
/*****************************************************************************/

static int hisfc350_version_check(void)
{
	unsigned long regval = readl(CONFIG_HISFC350_REG_BASE_ADDRESS
		+ HISFC350_VERSION);
	return (regval == 0x350);
}
/*****************************************************************************/

static struct spi_flash *hisfc350_init(void)
{
	struct hisfc_host *host = &hisfc_host;
	static struct spi_flash *spiflash;

	if (spiflash)
		return spiflash;

	memset(host, 0, sizeof(struct hisfc_host));
	memset(&spi_info_ex, 0, sizeof(struct mtd_info_ex));

	host->sysreg  = (void *)CRG_REG_BASE;
	host->regbase = (void *)CONFIG_HISFC350_REG_BASE_ADDRESS;
	host->iobase  = (void *)CONFIG_HISFC350_BUFFER_BASE_ADDRESS;
	host->dma_buffer = (unsigned int)host->buffer;
	host->set_system_clock = hisfc350_set_system_clock;
	host->set_host_addr_mode = hisfc350_set_host_addr_mode;

	spiflash = host->spiflash;

	spiflash->name = "hi_sfc";

	if (hisfc350_probe(host)) {
		printf("Can't find a valid spi flash chip.\n");
		goto fail;
	}
	hisfc_probe_spi_size(host, spiflash);

success:
	return spiflash;

fail:
	if (spiflash)
		spiflash = NULL;
	goto success;
}
/*****************************************************************************/

static struct mtd_info_ex *hisfc350_getinfo(void)
{
	if (spi_info_ex.type == 0) {
		if (hisfc350_init() == NULL)
			return NULL;
	}
	return &spi_info_ex;
}
/*****************************************************************************/

int hisfc350_spiflash_init(struct spi_flash **spiflash,
	struct mtd_info_ex **spiinfo)
{
	printf("Check spi flash controller v350... ");
	if (!hisfc350_version_check()) {
		printf("\n");
		return 0;
	}
	printf("Found\n");

	(*spiflash) = hisfc350_init();
	(*spiinfo) = hisfc350_getinfo();
	add_shutdown(hisfc350_shutdown);

	return 1;
}
