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
#include <malloc.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>
#include <asm/arch/platform.h>

#include <hifmc_common.h>
#include <spi_flash.h>
#include "../../hifmc_spi_ids.h"
#include "hifmc100.h"

/*****************************************************************************/
static void hifmc100_dma_transfer(struct hifmc_host *host,
	unsigned int spi_start_addr, unsigned char *dma_buffer,
	unsigned char rw_op, unsigned int size)
{
	unsigned char if_type = 0, dummy = 0;
	unsigned char w_cmd = 0, r_cmd = 0;
	unsigned int regval;
	struct hifmc_spi *spi = host->spi;

	FMC_PR(DMA_DB, "\t\t *-Start dma transfer => [%#x], len[%#x].\n",
			spi_start_addr, size);

	regval = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, regval);
	FMC_PR(DMA_DB, "\t\t   Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, regval);

	regval = spi_start_addr;
	hifmc_write(host, FMC_ADDRL, regval);
	FMC_PR(DMA_DB, "\t\t   Set ADDRL[%#x]%#x\n", FMC_ADDRL, regval);

	if (rw_op == RW_OP_WRITE) {
		if_type = spi->write->iftype;
		dummy = spi->write->dummy;
		w_cmd = spi->write->cmd;
	} else if (rw_op == RW_OP_READ) {
		if_type = spi->read->iftype;
		dummy = spi->read->dummy;
		r_cmd = spi->read->cmd;
	}

	regval = OP_CFG_FM_CS(spi->chipselect)
		| OP_CFG_MEM_IF_TYPE(if_type)
		| OP_CFG_ADDR_NUM(spi->addrcycle)
		| OP_CFG_DUMMY_NUM(dummy);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(DMA_DB, "\t\t   Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DMA_LEN_SET(size);
	hifmc_write(host, FMC_DMA_LEN, regval);
	FMC_PR(DMA_DB, "\t\t   Set DMA_LEN[%#x]%#x\n", FMC_DMA_LEN, regval);

	regval = (unsigned int)dma_buffer;
	hifmc_write(host, FMC_DMA_SADDR_D0, regval);
	FMC_PR(DMA_DB, "\t\t   Set DMA_SADDR_D0[%#x]%#x\n", FMC_DMA_SADDR_D0,
			regval);

	regval = OP_CTRL_RD_OPCODE(r_cmd)
		| OP_CTRL_WR_OPCODE(w_cmd)
		| OP_CTRL_RW_OP(rw_op)
		| OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, regval);
	FMC_PR(DMA_DB, "\t\t   Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, regval);

	FMC_DMA_WAIT_INT_FINISH(host);

	FMC_PR(DMA_DB, "\t\t *-End dma transfer.\n");

	return;
}

/*****************************************************************************/
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_READ
static char *hifmc100_reg_read_buf(struct hifmc_host *host,
	struct hifmc_spi *spi, unsigned int spi_start_addr,
	unsigned int size, unsigned char *buffer)
{
	unsigned int regval;

	FMC_PR(DMA_DB, "* Start reg read, from:%#x len:%#x\n", spi_start_addr,
			size);

	if (size > HIFMC100_DMA_MAX_SIZE)
		DB_BUG("reg read out of reg range.\n");

	hifmc_write(host, FMC_ADDRL, spi_start_addr);
	FMC_PR(DMA_DB, "  Set ADDRL[%#x]%#x\n", FMC_ADDRL, spi_start_addr);

	regval = FMC_DATA_NUM_CNT(size);
	hifmc_write(host, FMC_DATA_NUM, regval);
	FMC_PR(DMA_DB, "  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = OP_CTRL_RD_OPCODE(spi->read->cmd)
		| OP_CTRL_DMA_OP(OP_TYPE_REG)
		| OP_CTRL_RW_OP(RW_OP_READ)
		| OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, regval);
	FMC_PR(DMA_DB, "  Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	memcpy(buffer, host->iobase, size);

	FMC_PR(DMA_DB, "*-End reg page read.\n");

	return buffer;
}

/*****************************************************************************/
static int hifmc100_reg_read(struct spi_flash *spiflash, u_int from,
	size_t len, void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (u_char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if ((from + len) > spiflash->size) {
		DB_MSG("read area out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DB_MSG("read length is 0.\n");
		return 0;
	}

	if (hifmc_ip_user) {
		printf("Warning: Hifmc IP is busy, Please try again.\n");
		udelay(100);
		return;
	} else {
		hifmc_dev_type_switch(FLASH_TYPE_SPI_NOR);
		hifmc_ip_user++;
	}

	if (spi->driver->wait_ready(spi)) {
		DB_MSG("Error: Dma read wait ready fail!\n");
		result = -EBUSY;
		goto fail;
	}

	host->set_system_clock(spi->read, ENABLE);

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("read memory out of range.\n");
			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(spi->read, ENABLE);
		}

		num = ((from + len) >= spi->chipsize) ?
			(spi->chipsize - from) : len;

		while (num >= HIFMC100_DMA_MAX_SIZE) {
			hifmc100_reg_read_buf(host, spi,
				from, HIFMC100_DMA_MAX_SIZE, ptr);
			ptr += HIFMC100_DMA_MAX_SIZE;
			from += HIFMC100_DMA_MAX_SIZE;
			len -= HIFMC100_DMA_MAX_SIZE;
			num -= HIFMC100_DMA_MAX_SIZE;
		}

		if (num) {
			hifmc100_reg_read_buf(host, spi,
				from, num, ptr);
			from += num;
			ptr += num;
			len -= num;
		}
	}
	result = 0;
fail:
	hifmc_ip_user--;
	return result;
}
#endif /* HIFMC100_SPI_NOR_SUPPORT_REG_READ */

/*****************************************************************************/
static int hifmc100_dma_read(struct spi_flash *spiflash, u_int from, size_t len,
				void *buf)
{
	int num;
	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	FMC_PR(RD_DBG, "\t|*-Start dma read, from:%#x len:%#x\n", from, len);

	if ((from + len) > spiflash->size) {
		DB_MSG("read area out of range[%#x].\n", spiflash->size);
		return -EINVAL;
	}

	if (!len) {
		DB_MSG("read length is 0.\n");
		return 0;
	}

	if (hifmc_ip_user) {
		printf("Warning: Hifmc IP is busy, Please try again.\n");
		udelay(100);
		return -EBUSY;
	} else {
		hifmc_dev_type_switch(FLASH_TYPE_SPI_NOR);
		hifmc_ip_user++;
	}

	if (spi->driver->wait_ready(spi)) {
		DB_MSG("Error: Dma read wait ready fail!\n");
		hifmc_ip_user--;
		return -EBUSY;
	}

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("read memory out of range.\n");
		}

		num = ((from + len) >= spi->chipsize)
			? (spi->chipsize - from) : len;
		while (num >= HIFMC100_DMA_MAX_SIZE) {
			hifmc100_dma_transfer(host, from,
				(u_char *)host->dma_buffer, RW_OP_READ,
				HIFMC100_DMA_MAX_SIZE);
			memcpy(ptr, host->buffer, HIFMC100_DMA_MAX_SIZE);
			ptr += HIFMC100_DMA_MAX_SIZE;
			from += HIFMC100_DMA_MAX_SIZE;
			len -= HIFMC100_DMA_MAX_SIZE;
			num -= HIFMC100_DMA_MAX_SIZE;
		}

		if (num) {
			hifmc100_dma_transfer(host, from,
				(u_char *)host->dma_buffer, RW_OP_READ, num);
			memcpy(ptr, host->buffer, num);
			from += num;
			ptr += num;
			len -= num;
		}
	}

	hifmc_ip_user--;

	FMC_PR(RD_DBG, "\t|*-End dma read.\n");

	return 0;
}

/*****************************************************************************/
void hifmc100_read_ids(struct hifmc_spi *spi, u_char cs, u_char *id)
{
	unsigned int reg;
	struct hifmc_host *host = spi->host;

	FMC_PR(BT_DBG, "\t|||*-Start Read SPI(cs:%d) ID.\n", cs);

	reg = FMC_CMD_CMD1(SPI_CMD_RDID);
	hifmc_write(host, FMC_CMD, reg);
	FMC_PR(BT_DBG, "\t||||-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(cs);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(BT_DBG, "\t||||-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_DATA_NUM_CNT(MAX_SPI_NOR_ID_LEN);
	hifmc_write(host, FMC_DATA_NUM, reg);
	FMC_PR(BT_DBG, "\t||||-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_READ_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(BT_DBG, "\t||||-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	memcpy(id, host->iobase, MAX_SPI_NOR_ID_LEN);

	FMC_PR(BT_DBG, "\t|||*-Read CS: %d ID: %#X %#X %#X %#X %#X %#X\n",
			cs, id[0], id[1], id[2], id[3], id[4], id[5]);
}

/*****************************************************************************/
static int hifmc100_reg_erase_one_block(struct hifmc_host *host,
	struct hifmc_spi *spi, unsigned int offset)
{
	unsigned int regval;

	FMC_PR(OP_DBG, "\t\t * Start erase one block, offset:%#x\n", offset);

	regval = spi->driver->wait_ready(spi);
	if (regval) {
		DB_MSG("Error: Erase wait ready fail! reg:%#x\n", regval);
		return 1;
	}

	spi->driver->write_enable(spi);

	host->set_system_clock(spi->erase, ENABLE);

	regval = FMC_CMD_CMD1(spi->erase->cmd);
	hifmc_write(host, FMC_CMD, regval);
	FMC_PR(OP_DBG, "\t\t   Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = offset;
	hifmc_write(host, FMC_ADDRL, regval);
	FMC_PR(OP_DBG, "\t\t   Set ADDRL[%#x]%#x\n", FMC_ADDRL, regval);

	regval = OP_CFG_FM_CS(spi->chipselect)
		| OP_CFG_MEM_IF_TYPE(spi->erase->iftype)
		| OP_CFG_ADDR_NUM(spi->addrcycle)
		| OP_CFG_DUMMY_NUM(spi->erase->dummy);
	hifmc_write(host, FMC_OP_CFG, regval);
	FMC_PR(OP_DBG, "\t\t   Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	FMC_PR(OP_DBG, "\t\t   Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	FMC_PR(OP_DBG, "\t\t * End erase one block.\n");

	return 0;
}

/*****************************************************************************/
static int hifmc100_dma_write(struct spi_flash *spiflash, u_int to, size_t len,
	const void *buf)
{
	int num;
	int result = -EIO;

	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if (WR_DBG)
		printf("\n");
	FMC_PR(WR_DBG, "\t\t* Start dma write, to:%#x len:%#x\n", to, len);

	if ((to + len) > spiflash->size) {
		DB_MSG("write data out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DB_MSG("write length is 0.\n");
		return 0;
	}

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	if (host->level) {
		if ((BP_CMP_TOP == host->cmp)
			&& ((to + len) > host->start_addr)) {
			puts("\n");
			DB_MSG("DMA Write area[%#x => %#x] was locked\n",
					host->start_addr, (to + len));
			return -EINVAL;
		}

		if ((BP_CMP_BOTTOM == host->cmp) && (to < host->end_addr)) {
			unsigned int end = ((to + len) > host->end_addr) ?
				host->end_addr : (to + len);

			puts("\n");
			DB_MSG("DMA Write area[%#x => %#x] was locked\n", to,
					end);
			return -EINVAL;
		}
	}
#endif

	if (hifmc_ip_user) {
		printf("Warning: Hifmc IP is busy, Please try again.\n");
		udelay(100);
		return -EBUSY;
	} else {
		hifmc_dev_type_switch(FLASH_TYPE_SPI_NOR);
		hifmc_ip_user++;
	}

	if (spi->driver->wait_ready(spi))
		goto fail;

	spi->driver->write_enable(spi);

	if (to & HIFMC100_DMA_MASK) {
		num = HIFMC100_DMA_MAX_SIZE - (to & HIFMC100_DMA_MASK);
		if (num > len)
			num = len;
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

			spi->driver->write_enable(spi);
		}
		memcpy(host->buffer, ptr, num);
		FMC_PR(WR_DBG, "\t\t  DMA align write, to:%#x len:%#x.\n", to,
				num);
		hifmc100_dma_transfer(host, to, (u_char *)host->dma_buffer,
				RW_OP_WRITE, num);

		to += num;
		ptr += num;
		len -= num;
	}

	while (len > 0) {
		num = ((len >= HIFMC100_DMA_MAX_SIZE)
			? HIFMC100_DMA_MAX_SIZE : len);
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

			spi->driver->write_enable(spi);
		}

		memcpy(host->buffer, ptr, num);
		FMC_PR(WR_DBG, "\t\t  DMA split write, to:%#x len:%#x.\n", to,
				num);
		hifmc100_dma_transfer(host, to, (u_char *)host->dma_buffer,
				RW_OP_WRITE, num);

		to += num;
		ptr += num;
		len -= num;
	}

	result = 0;
fail:
	hifmc_ip_user--;
	FMC_PR(WR_DBG, "\t\t* End dma write.\n");
	return result;
}

/*****************************************************************************/
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
static int hifmc100_reg_write_buf(struct hifmc_host *host,
		struct hifmc_spi *spi, u_int spi_start_addr,
		size_t size, unsigned char *buffer)
{
	if (size > HIFMC100_DMA_MAX_SIZE)
		DB_BUG("reg read out of reg range.\n");

	if (spi->driver->wait_ready(spi))
		return 1;

	memcpy((char *)host->iobase, buffer, size);

	spi->driver->write_enable(spi);

	hifmc_write(host, FMC_INT_CLR, FMC_INT_CLR_ALL);

	hifmc_write(host, FMC_CMD, FMC_CMD_CMD1(spi->write->cmd));

	hifmc_write(host, FMC_OP_CFG, OP_CFG_FM_CS(spi->chipselect)
				| OP_CFG_MEM_IF_TYPE(spi->write->iftype));

	hifmc_write(host, FMC_ADDRL, spi_start_addr);

	hifmc_write(host, FMC_OP_CTRL, (OP_CTRL_WR_OPCODE(spi->write->cmd)
			| OP_CTRL_DMA_OP(OP_TYPE_REG)
			| OP_CTRL_RW_OP(RW_OP_WRITE)
			| OP_CTRL_DMA_OP_READY));

	FMC_DMA_WAIT_CPU_FINISH(host);

	return 0;
}

/*****************************************************************************/
static int hifmc100_reg_write(struct spi_flash *spiflash, u_int to, size_t len,
	const void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if ((to + len) > spiflash->size) {
		DB_MSG("write data out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DB_MSG("write length is 0.\n");
		return 0;
	}

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	if (host->level) {
		if ((BP_CMP_TOP == host->cmp)
			&& ((to + len) > host->start_addr)) {
			puts("\n");
			DB_MSG("Reg write area[%#x => %#x] was locked\n",
					host->start_addr, (to + len));
			return -EINVAL;
		}

		if ((BP_CMP_BOTTOM == host->cmp) && (to < host->end_addr)) {
			unsigned end = ((to + len) > host->end_addr) ?
				host->end_addr : (to + len);

			puts("\n");
			DB_MSG("Reg write area[%#x => %#x] was locked\n", to,
					end);
			return -EINVAL;
		}
	}
#endif

	if (hifmc_ip_user) {
		printf("Warning: Hifmc IP is busy, Please try again.\n");
		udelay(100);
		return;
	} else {
		hifmc_dev_type_switch(FLASH_TYPE_SPI_NOR);
		hifmc_ip_user++;
	}

	if (spi->driver->wait_ready(spi))
		goto fail;

	host->set_system_clock(host, spi->write, ENABLE);

	if (to & HIFMC100_DMA_MASK) {
		num = HIFMC100_DMA_MAX_SIZE - (to & HIFMC100_DMA_MASK);
		if (num > (int)len)
			num = (int)len;

		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

			host->set_system_clock(host, spi->write, ENABLE);
		}

		if (hifmc100_reg_write_buf(host, spi, to, num, ptr))
			goto fail;

		to += num;
		ptr += num;
		len -= num;
	}

	while (len > 0)	{
		num = ((len >= HIFMC100_DMA_MAX_SIZE) ?
				HIFMC100_DMA_MAX_SIZE : len);
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(host, spi->write, ENABLE);
		}
		if (hifmc100_reg_write_buf(host, spi, to, num, ptr))
			goto fail;
		to += num;
		ptr += num;
		len -= num;
	}
fail:
	hifmc_ip_user--;
	return 0;
}
#endif /* HIFMC100_SPI_NOR_SUPPORT_REG_WRITE */

/*****************************************************************************/
static int hifmc100_reg_erase(struct spi_flash *spiflash, u_int offset,
	size_t length)
{
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if (OP_DBG)
		printf("\n");
	FMC_PR(OP_DBG, "\t\t* Start reg erase, offset:%#x len:%#x\n",
			(u_int)offset, (u_int)length);

	if (offset + length > spiflash->size) {
		DB_MSG("Error: Erase area out of range of mtd.\n");
		return -EINVAL;
	}

	if (offset & (spi->erasesize - 1)) {
		DB_MSG("Error: Erase start address is not alignment.\n");
		return -EINVAL;
	}

	if (length & (spi->erasesize - 1)) {
		DB_MSG("Error: Erase length is not alignment.\n");
		return -EINVAL;
	}

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	if (host->level) {
		if ((BP_CMP_TOP == host->cmp)
			&& ((offset + length) > host->start_addr)) {
			puts("\n");
			DB_MSG("Erase area[%#x => %#x] was locked\n",
					host->start_addr, (offset + length));
			return -EINVAL;
		}

		if ((BP_CMP_BOTTOM == host->cmp) && (offset < host->end_addr)) {
			u_int end = ((offset + length) > host->end_addr) ?
				host->end_addr : (offset + length);

			puts("\n");
			DB_MSG("Erase area[%#x => %#x] was locked\n", offset,
					end);
			return -EINVAL;
		}
	}
#endif

	if (hifmc_ip_user) {
		printf("Warning: Hifmc IP is busy, Please try again.\n");
		udelay(100);
		return -EBUSY;
	} else {
		hifmc_dev_type_switch(FLASH_TYPE_SPI_NOR);
		hifmc_ip_user++;
	}

	while (length) {
		if (spi->chipsize <= offset) {
			offset -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("Error: Erase memory out of range.\n");
		}

		if (hifmc100_reg_erase_one_block(host, spi, offset)) {
			hifmc_ip_user--;
			return -1;
		}

		offset += spi->erase->size;
		length -= spi->erase->size;
	}

	hifmc_ip_user--;

	FMC_PR(OP_DBG, "\t\t* End reg erase.\n");

	return 0;
}

/*****************************************************************************/
struct spi_flash *hifmc100_spi_nor_scan(struct hifmc_host *host)
{
	unsigned char cs;
	struct spi_flash *spi_nor_flash = host->spi_nor_flash;
	struct hifmc_spi *spi = host->spi;
	struct mtd_info_ex *spi_nor_info = host->spi_nor_info;

	FMC_PR(BT_DBG, "\t|*-Start Scan SPI nor flash\n");

	spi_nor_flash->size = 0;

	for (cs = 0; cs < CONFIG_SPI_NOR_MAX_CHIP_NUM; cs++)
		host->spi[cs].host = host;

	FMC_PR(BT_DBG, "\t||-Initial mtd_info_ex structure\n");
	memset(spi_nor_info, 0, sizeof(struct mtd_info_ex));

	if (!hifmc_spi_nor_probe(spi_nor_info, spi)) {
#ifndef CONFIG_SPI_NOR_QUIET_TEST
		printf("Can't find a valid spi nor flash chip.\n");
#endif
		return NULL;
	}

	FMC_PR(BT_DBG, "\t||-Initial spi_flash structure\n");
	spi_nor_flash->name = "hi_fmc";
	spi_nor_flash->erase = hifmc100_reg_erase;
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
	spi_nor_flash->write = hifmc100_reg_write;
#else
	spi_nor_flash->write = hifmc100_dma_write;
#endif

#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_READ
	spi_nor_flash->read = hifmc100_reg_read;
#else
	spi_nor_flash->read = hifmc100_dma_read;
#endif

	FMC_PR(BT_DBG, "\t|*-Found SPI nor flash: %s\n", spi_nor_info->name);

	return spi_nor_flash;
}

/*****************************************************************************/
static void hifmc100_set_host_addr_mode(struct hifmc_host *host, int enable)
{
	unsigned int regval = hifmc_read(host, FMC_CFG);

	/* 1: SPI_NOR_ADDR_MODE_4_BYTES	0: SPI_NOR_ADDR_MODE_3_BYTES */
	if (enable)
		regval |= SPI_NOR_ADDR_MODE_MASK;
	else
		regval &= ~SPI_NOR_ADDR_MODE_MASK;

	hifmc_write(host, FMC_CFG, regval);
}

/*****************************************************************************/
static int hifmc100_host_init(struct hifmc_host *host)
{
	unsigned int reg, flash_type, align_mask;

	FMC_PR(BT_DBG, "\t|||*-Start SPI Nor host init\n");

	host->regbase = (void *)CONFIG_HIFMC_REG_BASE;
	host->iobase = (void *)CONFIG_HIFMC_BUFFER_BASE;

	reg = hifmc_read(host, FMC_CFG);
	FMC_PR(BT_DBG, "\t||||-Get CFG[%#x]%#x\n", FMC_CFG, reg);
	flash_type = (reg & FLASH_SEL_MASK) >> FLASH_SEL_SHIFT;
	if (flash_type != FLASH_TYPE_SPI_NOR) {
		DB_MSG("Error: Flash type isn't SPI Nor. reg: %#x\n", reg);
		return -ENODEV;
	}

	if ((reg & OP_MODE_MASK) == OP_MODE_BOOT) {
		reg |= FMC_CFG_OP_MODE(OP_MODE_NORMAL);
		FMC_PR(BT_DBG, "\t||||-Controller enter normal mode\n");
		hifmc_write(host, FMC_CFG, reg);
		FMC_PR(BT_DBG, "\t||||-Set CFG[%#x]%#x\n", FMC_CFG, reg);
	}

	host->buforg = kmalloc((HIFMC100_DMA_MAX_SIZE + FMC_DMA_ALIGN),
				GFP_KERNEL);
	if (!host->buforg) {
		FMC_PR(BT_DBG, "Error: Can't malloc memory for host\n");
		return -ENOMEM;
	}

	align_mask = FMC_DMA_ALIGN - 1;
	host->dma_buffer = (u_int)(host->buforg + align_mask) & ~align_mask;
	host->buffer = (char *)host->dma_buffer;

	host->set_system_clock = hifmc_set_fmc_system_clock;
	host->set_host_addr_mode = hifmc100_set_host_addr_mode;

	reg = TIMING_CFG_TCSH(CS_HOLD_TIME)
		| TIMING_CFG_TCSS(CS_SETUP_TIME)
		| TIMING_CFG_TSHSL(CS_DESELECT_TIME);
	hifmc_write(host, FMC_SPI_TIMING_CFG, reg);
	FMC_PR(BT_DBG, "\t||||-Set TIMING[%#x]%#x\n", FMC_SPI_TIMING_CFG, reg);

	FMC_PR(BT_DBG, "\t|||*-End SPI Nor host init\n");

	return 0;
}

/*****************************************************************************/
int hifmc100_spi_nor_init(struct hifmc_host *host)
{
	int ret = 0;

	FMC_PR(BT_DBG, "\t||*-Start hifmc100 SPI Nor init\n");

	FMC_PR(BT_DBG, "\t|||-Hifmc100 host structure init\n");
	ret = hifmc100_host_init(host);
	if (ret) {
		DB_MSG("Error: SPI Nor host init failed, result: %d\n", ret);
		return ret;
	}

	FMC_PR(BT_DBG, "\t|||-Set default system clock, Enable controller\n");
	if (host->set_system_clock)
		host->set_system_clock(NULL, ENABLE);

	FMC_PR(BT_DBG, "\t||*-End hifmc100 SPI Nor init\n");

	return ret;
}

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
/*****************************************************************************/
void spi_lock_update_address(unsigned char cmp, unsigned char level,
			unsigned int *start_addr, unsigned int *end_addr)
{
	unsigned int lock_len, erasesize, chipsize;

	erasesize = *start_addr;
	chipsize = *end_addr;

	*start_addr = 0;

	if (level) {
		lock_len = erasesize << (level - 1);
		if (lock_len > chipsize)
			lock_len = chipsize;

		if (BP_CMP_BOTTOM == cmp)
			*end_addr = lock_len;
		else if (BP_CMP_TOP == cmp)
			*start_addr = chipsize - lock_len;

		printf("Spi is locked. lock address[%#x => %#x]\n",
				*start_addr, *end_addr);
	} else {
		if (BP_CMP_BOTTOM == cmp)
			*end_addr = 0;
		else if (BP_CMP_TOP == cmp)
			*start_addr = chipsize;
	}
}

/*****************************************************************************/
unsigned hifmc100_get_spi_lock_info(unsigned char *cmp, unsigned char *level)
{
	unsigned char status, config, update_flag = *cmp;
	struct hifmc_host *host = &fmc_host;
	struct hifmc_spi *spi = host->spi;

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	BP_MSG("Get Status Register[%#x]\n", status);
	*level = (status & SPI_NOR_SR_BP_MASK) >> SPI_NOR_SR_BP0_SHIFT;

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	BP_MSG("Get Config Register[%#x]\n", config);
	*cmp = (config & SPI_NOR_SR_TB_MASK) >> SPI_NOR_SR_TB_SHIFT;

	if (update_flag == BP_CMP_UPDATE_FLAG) {
		host->start_addr = host->erasesize;
		host->end_addr = host->spiflash[0].size;
		spi_lock_update_address(*cmp, *level, &(host->start_addr),
				&(host->end_addr));
	}

	return (config << SPI_NOR_CR_SHIFT) | status;
}

/*****************************************************************************/
static void hifmc100_set_BP_level(struct hifmc_host *host, unsigned char cmp,
					unsigned char level)
{
	unsigned char old_level, old_cmp = 0;
	unsigned int val, reg;
	struct hifmc_spi *spi = host->spi;

	BP_MSG("* Start BP %s level %d\n", (cmp ? "bottom" : "top"), level);

	val = hifmc100_get_spi_lock_info(&old_cmp, &old_level);
	BP_MSG("  Read CR:SR[%#x]\n", val);

	if ((old_cmp == cmp) && (old_level == level))
		return;

	spi->driver->write_enable(spi);

	if ((old_cmp != cmp) && level && (level != BP_LEVEL_MAX)) {
		val &= ~(SPI_NOR_SR_TB_MASK << SPI_NOR_CR_SHIFT);
		val |= cmp << (SPI_NOR_SR_TB_SHIFT + SPI_NOR_CR_SHIFT);
	}

	if (old_level != level) {
		val &= ~SPI_NOR_SR_BP_MASK;
		val |= level << SPI_NOR_SR_BP0_SHIFT;
	}

	reg = hifmc_read(host, FMC_GLOBAL_CFG);
	if (reg & FMC_GLOBAL_CFG_WP_ENABLE) {
		BP_MSG("  Hardware protected enable!, reg[%#x]\n", reg);
		reg &= ~FMC_GLOBAL_CFG_WP_ENABLE;
		hifmc_write(host, FMC_GLOBAL_CFG, reg);
		val &= ~SPI_NOR_SR_SRWD_MASK;
		BP_MSG("Disable SR[%d]:SRWD and WP#\n", SPI_NOR_SR_SRWD_SHIFT);
	}

	writeb(val, host->iobase);
	BP_MSG(" WriAte IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned char *)host->iobase);

	reg = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, reg);
	BP_MSG(" Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	BP_MSG(" Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN);
	hifmc_write(host, FMC_DATA_NUM, reg);
	BP_MSG(" Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	BP_MSG(" Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	BP_MSG("* Set BP level end.\n");
}

/*****************************************************************************/
void hifmc100_spi_lock(unsigned char cmp, unsigned char level)
{
	unsigned char old_level, old_cmp = 0;
	struct hifmc_host *host = &fmc_host;

	/* Set lock start of top to bottom, warning */
	if ((cmp == BP_CMP_BOTTOM) && (host->cmp == BP_CMP_TOP)) {
		puts("Warning: Set bottom option will change a OTP(One Time ");
		puts("Program) bit!\nThere is no way to recover if them ");
		puts("are changed.\nAre sure of what you are doing!\n");
		puts("Really set start of bottom address? <y/n>\n");
		if (getc() != 'y') {
			puts("Set start of bottom address aborted\n");
			return;
		}
	}

	/* Set lock start of bottom to top, error */
	if ((level != BP_LEVEL_0) && (level != BP_LEVEL_MAX)
	    && (cmp == BP_CMP_TOP) && (host->cmp == BP_CMP_BOTTOM)) {
		DB_MSG("Error: Set top option will change a OTP(One Time ");
		DB_MSG("Program) bit when it is changed!\n");
		return;
	}

	hifmc100_set_BP_level(host, cmp, level);

	hifmc100_get_spi_lock_info(&old_cmp, &old_level);
	if ((old_cmp != cmp) && level && (level != BP_LEVEL_MAX)) {
		DB_MSG("Error: Current lock start of %s address, but set ",
				(old_cmp ? "bottom" : "top"));
		DB_MSG("value: %s\n", (cmp ? "bottom" : "top"));
		return;
	}

	if (old_level != level) {
		DB_MSG("Error: Current lock level: %d, but set value: %d\n",
			old_level, level);
		return;
	}

	host->start_addr = host->erasesize;
	host->end_addr = host->spiflash[0].size;
	spi_lock_update_address(cmp, level, &(host->start_addr),
				&(host->end_addr));

	if ((level != BP_LEVEL_0) && (level != BP_LEVEL_MAX))
		host->cmp = cmp;
	host->level = level;

	return;
}

/*****************************************************************************/
#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

