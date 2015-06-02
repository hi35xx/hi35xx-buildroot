/*****************************************************************************
 *    Copyright (c) 2009-2011 by HiC
 *    All rights reserved.
 *****************************************************************************/

/*****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <asm/setup.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include "../spi_ids.h"
#include "hisfc350.h"

static inline int mtd_has_partitions(void) { return 1; }
#define THREE_BIT_START_UP 0

#ifdef CONFIG_ARCH_HI3518
#include "hisfc350_3518.c"
#endif

#ifdef CONFIG_ARCH_HI3520D
#include "hisfc350_hi3520d.c"
#endif

#ifdef CONFIG_ARCH_PHOENIX
#include "hisfc350_phoenix.c"
#endif

#ifndef GET_SFC_ADDR_MODE
#define GET_SFC_ADDR_MODE  (0)
#endif

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

static int  start_up_mode;
static char *ultohstr(unsigned long long size)
{
	int ix;
	static char buffer[20];
	char *fmt[] = {"%u", "%uK", "%uM", "%uG", "%uT", "%uT"};

	for (ix = 0; (ix < 5) && !(size & 0x3FF) && size; ix++)
		size = (size >> 10);

	sprintf(buffer, fmt[ix], size);
	return buffer;
}

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
#endif

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

static void hisfc350_driver_shutdown(struct platform_device *pltdev)
{
	if (start_up_mode == THREE_BIT_START_UP) {
		int ix;
		struct hisfc_host *host = platform_get_drvdata(pltdev);
		struct hisfc_spi *spi = host->spi;

		for (ix = 0; ix < host->num_chip; ix++, spi++) {
			if (spi->addrcycle == 4)
				spi->driver->wait_ready(spi);
			spi->driver->entry_4addr(spi, 0);
		}
	}
}
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

#ifdef HISFCV350_SUPPORT_REG_READ
static char *hisfc350_reg_read_buf(struct hisfc_host *host,
	struct hisfc_spi *spi, unsigned int spi_start_addr,
	unsigned int size, unsigned char *buffer)
{
	int index = 0;

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

	while (index < size) {
		*(unsigned int *)(host->reg_buffer + index) = hisfc_read(host,
			HISFC350_CMD_DATABUF0 + index);
		index    += 4;
	}

	memcpy(buffer, host->reg_buffer, size);

	return buffer;
}

static int hisfc350_reg_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	if ((from + len) > mtd->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}
	mutex_lock(&host->lock);

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
	*retlen = (size_t)(ptr - buf);
fail:
	mutex_unlock(&host->lock);
	return result;
}
#endif /* HISFCV350_SUPPORT_REG_READ */

static int hisfc350_dma_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	if ((from + len) > mtd->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	mutex_lock(&host->lock);

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
	*retlen = (size_t)(ptr - buf);
fail:
	mutex_unlock(&host->lock);
	return result;
}

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

static int hisfc350_dma_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	int num;
	int result = -EIO;

	unsigned char *ptr = (unsigned char *)buf;
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	if ((to + len) > mtd->size) {
		DBG_MSG("write data out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DBG_MSG("write length is 0.\n");
		return 0;
	}

	mutex_lock(&host->lock);

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
	*retlen = (size_t)(ptr - buf);
	result = 0;
fail:
	mutex_unlock(&host->lock);
	return result;
}

#ifdef HISFCV350_SUPPORT_REG_WRITE
static int hisfc350_reg_write_buf(struct hisfc_host *host,
	struct hisfc_spi *spi, unsigned int spi_start_addr,
	unsigned int size, unsigned char *buffer)
{
	int index = 0;

	if (size > HISFC350_REG_BUF_SIZE)
		DBG_BUG("reg read out of reg range.\n");

	if (spi->driver->wait_ready(spi))
		return 1;

	memcpy(host->reg_buffer, buffer, size);

	while (index < size) {
		hisfc_write(host, HISFC350_CMD_DATABUF0 + index,
			*(unsigned int *)(host->reg_buffer + index));
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

static int hisfc350_reg_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	if ((to + len) > mtd->size) {
		DBG_MSG("write data out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DBG_MSG("write length is 0.\n");
		return 0;
	}

	mutex_lock(&host->lock);

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
	*retlen = (size_t)(ptr - buf);
	result = 0;
fail:
	mutex_unlock(&host->lock);
	return result;
}
#endif /* HISFCV350_SUPPORT_REG_WRITE */

static int hisfc350_reg_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	unsigned long long offset = instr->addr;
	unsigned long long length = instr->len;

	if (offset + length > mtd->size) {
		DBG_MSG("erase area out of range of mtd.\n");
		return -EINVAL;
	}

	if ((unsigned int)offset & (mtd->erasesize-1)) {
		DBG_MSG("erase start address is not alignment.\n");
		return -EINVAL;
	}

	if ((unsigned int)length & (mtd->erasesize-1)) {
		DBG_MSG("erase length is not alignment.\n");
		return -EINVAL;
	}

	mutex_lock(&host->lock);
	while (length) {
		if (spi->chipsize <= offset) {
			offset -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("erase memory out of range.\n");
		}
		if (hisfc350_reg_erase_one_block(host, spi, offset)) {
			instr->state = MTD_ERASE_FAILED;
			mutex_unlock(&host->lock);
			return -EIO;
		}

		offset += spi->erase->size;
		length -= spi->erase->size;
	}

	instr->state = MTD_ERASE_DONE;
	mutex_unlock(&host->lock);
	mtd_erase_callback(instr);

	return 0;
}

#ifdef HISFCV350_SUPPORT_BUS_READ
static int hisfc350_bus_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	if ((from + len) > mtd->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	mutex_lock(&host->lock);

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
	*retlen = (size_t)(ptr - buf);
	result = 0;
fail:
	mutex_unlock(&host->lock);
	return result;
}
#endif /* HISFCV350_SUPPORT_BUS_READ */

#ifdef HISFCV350_SUPPORT_BUS_WRITE
static int hisfc350_bus_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	if ((to + len) > mtd->size) {
		DBG_MSG("write data out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DBG_MSG("write length is 0.\n");
		return 0;
	}

	mutex_lock(&host->lock);

	if (spi->driver->wait_ready(spi))
		goto fail;
	spi->driver->bus_prepare(spi, WRITE);

	while (len > 0) {
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;
			spi->driver->bus_prepare(spi, WRITE);
		}

		num = ((to + len) >= spi->chipsize)
				? (spi->chipsize - to) : len;

		if (num) {
			memcpy((char *)spi->iobase + to, ptr, num);
			ptr += num;
			to += num;
			len -= num;
		}
	}

	*retlen = (size_t)(ptr - buf);
	result = 0;
fail:
	mutex_unlock(&host->lock);
	return result;

}
#endif

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

		printk(KERN_INFO "Spi(cs%d) ID: 0x%02X 0x%02X 0x%02X"
			" 0x%02X 0x%02X 0x%02X\n",
			chipselect,
			ids[0], ids[1], ids[2], ids[3], ids[4], ids[5]);

		spiinfo = spi_serach_ids(ids);

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

			/* auto check sfc_addr_mode 3byte or 4 byte */
			start_up_mode = GET_SFC_ADDR_MODE;

			if (start_up_mode == THREE_BIT_START_UP) {
				printk(KERN_INFO "SPI FLASH start_up_mode is 3 Bytes\n");
				spi->driver->entry_4addr(spi, TRUE);
			} else {
				printk(KERN_INFO "SPI FLASH start_up_mode is 4 Bytes\n");
			}
			printk(KERN_INFO "Spi(cs%d): ", spi->chipselect);
			printk(KERN_INFO "Block:%sB ",
				ultohstr(spi->erasesize));
			printk(KERN_INFO "Chip:%sB ", ultohstr(spi->chipsize));
			printk(KERN_INFO "Name:\"%s\"\n", spi->name);

#ifdef CONFIG_HISFC350_SHOW_CYCLE_TIMING

			printk(KERN_INFO "Spi(cs%d): ", spi->chipselect);
			if (spi->addrcycle == 4)
				printk(KERN_INFO "4 addrcycle ");
			printk(KERN_INFO "read:%s,%02X,%s ",
				hisfc350_get_ifcycle_str(spi->read->iftype),
				spi->read->cmd,
				hisfc350_get_clock_str(spi->read->clock));
			printk(KERN_INFO "write:%s,%02X,%s ",
				hisfc350_get_ifcycle_str(spi->write->iftype),
				spi->write->cmd,
				hisfc350_get_clock_str(spi->write->clock));
			printk(KERN_INFO "erase:%s,%02X,%s\n",
				hisfc350_get_ifcycle_str(spi->erase[0].iftype),
				spi->erase[0].cmd,
				hisfc350_get_clock_str(spi->erase[0].clock));

#endif /* CONFIG_HISFC350_SHOW_CYCLE_TIMING */

			host->num_chip++;
			total += spi->chipsize;
			spi++;

		} else
			printk(KERN_ERR"Spi(cs%d): find unrecognized spi flash.\n",
				chipselect);
	}

	return host->num_chip;
}

#define MAX_MTD_PARTITIONS        (32)

struct partition_entry {
	char name[16];
	unsigned long long start;
	unsigned long long length;
	unsigned int flags;
};

struct partition_info {
	int parts_num;
	struct partition_entry entry[MAX_MTD_PARTITIONS];
	struct mtd_partition parts[MAX_MTD_PARTITIONS];
};

static struct partition_info ptn_info;

static int __init parse_spi_partitions(const struct tag *tag)
{
	int i;

	if (tag->hdr.size <= 2) {
		printk(KERN_DEBUG "%s(%d): tag->hdr.size <= 2\n",
			__func__, __LINE__);
		return 0;
	}

	ptn_info.parts_num = (tag->hdr.size - 2) /
		(sizeof(struct partition_entry)/sizeof(int));
	memcpy(ptn_info.entry, &tag->u,
		ptn_info.parts_num * sizeof(struct partition_entry));

	for (i = 0; i < ptn_info.parts_num; i++) {
		ptn_info.parts[i].name   = ptn_info.entry[i].name;
		ptn_info.parts[i].size   = (ptn_info.entry[i].length);
		ptn_info.parts[i].offset = (ptn_info.entry[i].start);
		ptn_info.parts[i].mask_flags = 0;
	}

	return 0;
}
__tagtable(0x48695370, parse_spi_partitions);

static void hisfc_probe_spi_size(struct hisfc_host *host,
	struct mtd_info *mtd)
{
	int ix = 1;
	struct hisfc_spi *spi = host->spi;

	int total     = spi->chipsize;
	int erasesize = spi->erasesize;

	for (++spi; ix < host->num_chip; ix++, spi++) {
		total += spi->chipsize;
	}

	mtd->size = total;
	mtd->erasesize = erasesize;

	printk(KERN_INFO "spi size: %sB\n", ultohstr(mtd->size));
	printk(KERN_INFO "chip num: %x\n", host->num_chip);
}

static int hisfc350_probe(struct hisfc_host *host)
{
	struct mtd_info *mtd = host->mtd;

	host->set_system_clock(host, NULL, TRUE);
	hisfc_write(host, HISFC350_TIMING,
		HISFC350_TIMING_TCSS(0x6)
		| HISFC350_TIMING_TCSH(0x6)
		| HISFC350_TIMING_TSHSL(0xf));

	if (!hisfc350_spi_probe(host))
		return -1;

	mtd->erase = hisfc350_reg_erase;
#ifdef HISFCV350_SUPPORT_REG_WRITE
	mtd->write = hisfc350_reg_write;
#elif defined HISFCV350_SUPPORT_BUS_WRITE
	mtd->write = hisfc350_bus_write;
#else
	mtd->write = hisfc350_dma_write;
#endif
#ifdef HISFCV350_SUPPORT_REG_READ
	mtd->read  = hisfc350_reg_read;
#elif defined HISFCV350_SUPPORT_BUS_READ
	mtd->read  = hisfc350_bus_read;
#else
	mtd->read  = hisfc350_dma_read;
#endif

	return 0;
}

static int __devinit hisfc350_driver_probe(struct platform_device * plat_dev)
{
	int result = -EIO;
	struct hisfc_host *host;
	struct mtd_info   *mtd;
	int nr_parts = 0;
	struct mtd_partition *parts = NULL;
	int ret;

	host = kmalloc(sizeof(struct hisfc_host), GFP_KERNEL);
	if (!host)
		return -ENOMEM;
	memset(host, 0, sizeof(struct hisfc_host));

	platform_set_drvdata(plat_dev, host);

	host->sysreg = ioremap_nocache(CONFIG_HISFC350_SYSCTRL_ADDRESS,
		HISFC350_SYSCTRL_LENGTH);
	if (!host->sysreg) {
		printk(KERN_ERR "spi system reg ioremap failed.\n");
		goto fail;
	}

	host->regbase = ioremap_nocache(CONFIG_HISFC350_REG_BASE_ADDRESS,
		HISFC350_REG_BASE_LEN);
	host->set_system_clock = hisfc350_set_system_clock;
	host->set_host_addr_mode = hisfc350_set_host_addr_mode;
	if (!host->regbase) {
		printk(KERN_ERR "spi base reg ioremap failed.\n");
		goto fail;
	}

#ifdef HISFCV350_SUPPORT_BUS_READ
	host->iobase = ioremap_nocache(CONFIG_HISFC350_BUFFER_BASE_ADDRESS,
		HISFC350_BUFFER_BASE_LEN);
	if (!host->iobase) {
		printk(KERN_ERR "spi buffer ioremap failed.\n");
		goto fail;
	}
#endif

	host->buffer = dma_alloc_coherent(host->dev, HISFC350_DMA_MAX_SIZE,
		&host->dma_buffer, GFP_KERNEL);
	if (host->buffer == NULL) {
		printk(KERN_ERR "spi alloc dma buffer failed.\n");
		goto fail;
	}

	mutex_init(&host->lock);
	mtd = host->mtd;
	mtd->name = (char *)plat_dev->name;
	mtd->type = MTD_NORFLASH;
	mtd->writesize = 1;
	mtd->flags = MTD_CAP_NORFLASH;
	mtd->owner = THIS_MODULE;

	if (hisfc350_probe(host)) {
		result = -ENODEV;
		goto fail;
	}

	hisfc_probe_spi_size(host, mtd);

	if (mtd_has_partitions()) {

		if (mtd_has_cmdlinepart()) {
			static char const *part_probes[] = {
				"cmdlinepart",
				NULL,
			};

			nr_parts = parse_mtd_partitions(host->mtd,
				part_probes, &parts, 0);
		}

		if (!nr_parts) {
			nr_parts = ptn_info.parts_num;
			parts    = ptn_info.parts;
		}

		if (nr_parts > 0) {
			int i;
			for (i = 0; i < nr_parts; i++) {
				DEBUG(MTD_DEBUG_LEVEL2, "partitions[%d] = "
					"{.name = %s, .offset = 0x%.8x, "
					".size = 0x%08x (%uKiB) }\n",
					i, parts[i].name,
					(unsigned int)parts[i].offset,
					(unsigned int)parts[i].size,
					(unsigned int)parts[i].size/1024);
			}
			host->add_partition = 1;
		}
	}
	ret = mtd_device_register(host->mtd, parts, nr_parts);

	if (nr_parts > 0)
		kfree(parts);

	return (1 == ret) ? -ENODEV : 0;

fail:
	if (host->regbase)
		iounmap(host->regbase);
	if (host->iobase)
		iounmap(host->iobase);
	if (host->buffer)
		dma_free_coherent(host->dev, HISFC350_DMA_MAX_SIZE,
			host->buffer, host->dma_buffer);
	if (host->sysreg)
		iounmap(host->sysreg);
	kfree(host);
	platform_set_drvdata(plat_dev, NULL);
	return result;
}

static int __devexit hisfc350_driver_remove(struct platform_device * plat_dev)
{
	struct hisfc_host *host = platform_get_drvdata(plat_dev);

	mtd_device_unregister(host->mtd);

	if (host->regbase)
		iounmap(host->regbase);
	if (host->iobase)
		iounmap(host->iobase);
	if (host->buffer)
		dma_free_coherent(host->dev, HISFC350_DMA_MAX_SIZE,
			host->buffer, host->dma_buffer);
	if (host->sysreg)
		iounmap(host->sysreg);

	kfree(host);
	platform_set_drvdata(plat_dev, NULL);

	return 0;
}


static void hisfc350_pltdev_release(struct device *dev)
{
}

#ifdef CONFIG_PM
static int hisfc350_driver_suspend(struct platform_device *dev,
		pm_message_t state)
{
	return 0;
}
static int hisfc350_driver_resume(struct platform_device *dev)
{
	return 0;
}
#endif

static struct platform_driver hisfc350_driver_pltdrv = {
	.probe		= hisfc350_driver_probe,
	.remove		= hisfc350_driver_remove,
	.shutdown	= hisfc350_driver_shutdown,
#ifdef CONFIG_PM
	.suspend	= hisfc350_driver_suspend,
	.resume		= hisfc350_driver_resume,
#endif
	.driver.name	= "hi_sfc",
	.driver.owner	= THIS_MODULE,
	.driver.bus	= &platform_bus_type,
};

static struct resource hisfc350_device_resources[] = {
	[0] = {
		.start = CONFIG_HISFC350_REG_BASE_ADDRESS,
		.end   = CONFIG_HISFC350_REG_BASE_ADDRESS
			+ HISFC350_REG_BASE_LEN - 1,
		.flags = IORESOURCE_MEM,
	},

	[1] = {
		.start = CONFIG_HISFC350_BUFFER_BASE_ADDRESS,
		.end   = CONFIG_HISFC350_BUFFER_BASE_ADDRESS
			+ HISFC350_BUFFER_BASE_LEN - 1,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device hisfc350_device_pltdev = {
	.name           = "hi_sfc",
	.id             = -1,

	.dev.release    = hisfc350_pltdev_release,
	.dev.coherent_dma_mask = ~0,
	.num_resources  = ARRAY_SIZE(hisfc350_device_resources),
	.resource       = hisfc350_device_resources,
};

static int __init hisfc350_module_init(void)
{
	int result = 0;

	printk(KERN_DEBUG "Hisilicon Spi Flash Controller V350 Device Driver,"
		" Version 1.10\n");

	result = platform_driver_register(&hisfc350_driver_pltdrv);
	if (result < 0)
		return result;

	result = platform_device_register(&hisfc350_device_pltdev);
	if (result < 0) {
		platform_driver_unregister(&hisfc350_driver_pltdrv);
		return result;
	}

	return result;
}

static void __exit hisfc350_module_exit(void)
{
	platform_device_unregister(&hisfc350_device_pltdev);
	platform_driver_unregister(&hisfc350_driver_pltdrv);
}

module_init(hisfc350_module_init);
module_exit(hisfc350_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HiC");
MODULE_DESCRIPTION("Hisilicon Spi Flash Controller V350 Device Driver, "
	"Version 1.00");
