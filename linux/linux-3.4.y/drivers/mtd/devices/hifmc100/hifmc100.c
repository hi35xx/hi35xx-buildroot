/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <asm/setup.h>

#include "../../hifmc_common.h"
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
static int hifmc100_reg_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hifmc_host *host = MTD_TO_HOST(mtd);
	struct hifmc_spi *spi = host->spi;

	if ((from + len) > mtd->size) {
		DB_MSG("read area out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DB_MSG("read length is 0.\n");
		return 0;
	}

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_lock(&spi_type_mutex);
	mtd_switch_spi_type(mtd);
#endif
	mutex_lock(&host->lock);

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
	*retlen = (size_t)(ptr - buf);
fail:
	mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_unlock(&spi_type_mutex);
#endif
	return result;
}
#endif /* HIFMC100_SPI_NOR_SUPPORT_REG_READ */

/*****************************************************************************/
static int hifmc100_dma_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hifmc_host *host = MTD_TO_HOST(mtd);
	struct hifmc_spi *spi = host->spi;

	FMC_PR(RD_DBG, "\t|*-Start dma read, from:%#x len:%#x\n", (u_int)from,
			len);

	if ((from + len) > mtd->size) {
		DB_MSG("read area out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DB_MSG("read length is 0.\n");
		return 0;
	}

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_lock(&spi_type_mutex);
	mtd_switch_spi_type(mtd);
#endif
	mutex_lock(&host->lock);

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
			host->set_system_clock(spi->read, ENABLE);
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
	result = 0;
	*retlen = (size_t)(ptr - buf);
fail:
	mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_unlock(&spi_type_mutex);
#endif
	FMC_PR(RD_DBG, "\t|*-End dma read.\n");

	return result;
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
static int hifmc100_dma_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	int num;
	int result = -EIO;

	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = MTD_TO_HOST(mtd);
	struct hifmc_spi *spi = host->spi;

	if (WR_DBG)
		pr_info("\n");
	FMC_PR(WR_DBG, "\t\t* Start dma write, to:%#x len:%#x\n", (u_int)to,
			(u_int)len);

	if ((to + len) > mtd->size) {
		DB_MSG("write data out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DB_MSG("write length is 0.\n");
		return 0;
	}

#ifdef CONFIG_SPI_BLOCK_PROTECT
	if (host->level && (to < host->end_addr)) {
		pr_err("Error: The DMA write area was locked\n");
		return -EINVAL;
	}
#endif

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_lock(&spi_type_mutex);
	mtd_switch_spi_type(mtd);
#endif
	mutex_lock(&host->lock);

	if (spi->driver->wait_ready(spi))
		goto fail;

	spi->driver->write_enable(spi);

	host->set_system_clock(spi->write, ENABLE);

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
			host->set_system_clock(spi->write, ENABLE);
		}
		memcpy(host->buffer, ptr, num);
		FMC_PR(WR_DBG, "\t\t  DMA align write, to:%#x len:%#x.\n",
				(u_int)to, num);
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
			host->set_system_clock(spi->write, ENABLE);
		}

		memcpy(host->buffer, ptr, num);
		FMC_PR(WR_DBG, "\t\t  DMA split write, to:%#x len:%#x.\n",
				(u_int)to, num);
		hifmc100_dma_transfer(host, to, (u_char *)host->dma_buffer,
				RW_OP_WRITE, num);

		to += num;
		ptr += num;
		len -= num;
	}
	*retlen = (size_t)(ptr - buf);
	result = 0;
fail:
	mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_unlock(&spi_type_mutex);
#endif
	FMC_PR(WR_DBG, "\t\t* End dma write.\n");
	return result;
}

/*****************************************************************************/
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
static int hifmc100_reg_write_buf(struct hifmc_host *host,
		struct hifmc_spi *spi, u_int spi_start_addr,
		u_int size, unsigned char *buffer)
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
static int hifmc100_reg_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = MTD_TO_HOST(mtd);
	struct hifmc_spi *spi = host->spi;

	if ((to + len) > mtd->size) {
		DB_MSG("write data out of range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DB_MSG("write length is 0.\n");
		return 0;
	}

#ifdef CONFIG_SPI_BLOCK_PROTECT
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

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_lock(&spi_type_mutex);
	mtd_switch_spi_type(mtd);
#endif
	mutex_lock(&host->lock);

	if (spi->driver->wait_ready(spi))
		goto fail;

	host->set_system_clock(spi->write, ENABLE);

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

			host->set_system_clock(spi->write, ENABLE);
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
			host->set_system_clock(spi->write, ENABLE);
		}
		if (hifmc100_reg_write_buf(host, spi, to, num, ptr))
			goto fail;
		to += num;
		ptr += num;
		len -= num;
	}
	*retlen = (size_t)(ptr - buf);
	result = 0;
fail:
	mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_unlock(&spi_type_mutex);
#endif
	return result;
}
#endif /* HIFMC100_SPI_NOR_SUPPORT_REG_WRITE */

/*****************************************************************************/
static int hifmc100_reg_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct hifmc_host *host = MTD_TO_HOST(mtd);
	struct hifmc_spi *spi = host->spi;

	unsigned long long offset = instr->addr;
	unsigned long long length = instr->len;

	if (OP_DBG)
		pr_info("\n");
	FMC_PR(OP_DBG, "\t\t* Start reg erase, offset:%#x len:%#x\n",
			(u_int)offset, (u_int)length);

	if (offset + length > mtd->size) {
		DB_MSG("Error: Erase area out of range of mtd.\n");
		return -EINVAL;
	}

	if ((u_int)offset & (mtd->erasesize-1)) {
		DB_MSG("Error: Erase start address is not alignment.\n");
		return -EINVAL;
	}

	if ((u_int)length & (mtd->erasesize-1)) {
		DB_MSG("Error: Erase length is not alignment.\n");
		return -EINVAL;
	}

#ifdef CONFIG_SPI_BLOCK_PROTECT
	if ((host->level) && (offset < host->end_addr)) {
		pr_err("Error: The erase area was locked\n");
		return -EINVAL;
	}
#endif

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	FMC_PR(OP_DBG, "\t|-Switch device type to SPI nor\n");
	mutex_lock(&spi_type_mutex);
	mtd_switch_spi_type(mtd);
#endif
	mutex_lock(&host->lock);

	while (length) {
		if (spi->chipsize <= offset) {
			offset -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("Error: Erase memory out of range.\n");
		}

		if (hifmc100_reg_erase_one_block(host, spi, offset)) {
			instr->state = MTD_ERASE_FAILED;
			mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
			mutex_unlock(&spi_type_mutex);
#endif
			return -EIO;
		}

		offset += spi->erase->size;
		length -= spi->erase->size;
	}

	instr->state = MTD_ERASE_DONE;
	mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_unlock(&spi_type_mutex);
#endif
	mtd_erase_callback(instr);

	FMC_PR(OP_DBG, "\t\t* End reg erase.\n");

	return 0;
}

/*****************************************************************************/
int hifmc100_spi_nor_scan(struct hifmc_host *host)
{
	unsigned char cs;
	struct mtd_info *mtd = host->mtd;
	struct hifmc_spi *spi = host->spi;

	FMC_PR(BT_DBG, "\t|*-Start Scan SPI nor flash\n");

	mtd->size = 0;

	for (cs = 0; cs < CONFIG_SPI_NOR_MAX_CHIP_NUM; cs++)
		host->spi[cs].host = host;

	if (!hifmc_spi_nor_probe(mtd, spi)) {
#ifndef CONFIG_SPI_NOR_QUIET_TEST
		pr_info("Can't find a valid spi nor flash chip.\n");
#endif
		goto end;
	}

	FMC_PR(BT_DBG, "\t||-Initial mtd structure func hook\n");

#ifdef CONFIG_SPI_BLOCK_PROTECT
	hifmc100_get_spi_lock_info(host);
#endif
	mtd->_erase = hifmc100_reg_erase;
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
	mtd->_write = hifmc100_reg_write;
#else
	mtd->_write = hifmc100_dma_write;
#endif

#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_READ
	mtd->_read = hifmc100_reg_read;
#else
	mtd->_read = hifmc100_dma_read;
#endif

end:
	FMC_PR(BT_DBG, "\t|*-Found SPI nor flash: %s on %s\n", spi->name,
			mtd->name);

	return 0;
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
	unsigned int reg, flash_type;

	FMC_PR(BT_DBG, "\t||*-Start SPI Nor host init\n");

	reg = hifmc_read(host, FMC_CFG);
	FMC_PR(BT_DBG, "\t|||-Get CFG[%#x]%#x\n", FMC_CFG, reg);
	flash_type = (reg & FLASH_SEL_MASK) >> FLASH_SEL_SHIFT;
	if (flash_type != FLASH_TYPE_SPI_NOR) {
		DB_MSG("Error: Flash type isn't SPI Nor. reg: %#x\n", reg);
		return -ENODEV;
	}

	if ((reg & OP_MODE_MASK) == OP_MODE_BOOT) {
		DB_MSG("Error: Current controller mode isn't normal!\n");
		reg |= FMC_CFG_OP_MODE(OP_MODE_NORMAL);
		hifmc_write(host, FMC_CFG, reg);
		FMC_PR(BT_DBG, "\t|||-Set CFG[%#x]%#x\n", FMC_CFG, reg);
	}

	host->set_system_clock = hifmc_set_nor_system_clock;
	host->set_host_addr_mode = hifmc100_set_host_addr_mode;

	reg = TIMING_CFG_TCSH(CS_HOLD_TIME)
		| TIMING_CFG_TCSS(CS_SETUP_TIME)
		| TIMING_CFG_TSHSL(CS_DESELECT_TIME);
	hifmc_write(host, FMC_SPI_TIMING_CFG, reg);
	FMC_PR(BT_DBG, "\t|||-Set TIMING[%#x]%#x\n", FMC_SPI_TIMING_CFG, reg);

	FMC_PR(BT_DBG, "\t||*-End SPI Nor host init\n");

	return 0;
}

/*****************************************************************************/
int hifmc100_spi_nor_init(struct hifmc_host *host)
{
	int ret = 0;

	FMC_PR(BT_DBG, "\t|*-Start hifmc100 SPI Nor init\n");

	FMC_PR(BT_DBG, "\t||-Hifmc100 host structure init\n");
	ret = hifmc100_host_init(host);
	if (ret) {
		DB_MSG("Error: SPI Nor host init failed, result: %d\n", ret);
		return ret;
	}

	FMC_PR(BT_DBG, "\t||-Set default system clock, Enable controller\n");
	if (host->set_system_clock)
		host->set_system_clock(NULL, ENABLE);

	FMC_PR(BT_DBG, "\t|*-End hifmc100 SPI Nor init\n");

	return ret;
}


#ifdef CONFIG_SPI_BLOCK_PROTECT
/*****************************************************************************/
unsigned short hifmc100_get_spi_lock_info(struct hifmc_host *host)
{
	struct hifmc_spi *spi = host->spi;

	FMC_PR(BP_DBG, "Get manufacturer ID: [%#x]\n", host->mid);

	switch (host->mid) {
	case MID_GD:
	case MID_ESMT:
	case MID_CFEON:
	case MID_SPANSION:
		host->level = hifmc100_bp_to_level(host, BP_NUM_3);
		break;
	case MID_WINBOND:
		if (spi->chipsize <= _16M)
			host->level = hifmc100_bp_to_level(host, BP_NUM_3);
		else
			host->level = hifmc100_bp_to_level(host, BP_NUM_4);
		break;
	case MID_MXIC:
		if (spi->chipsize <= _8M)
			host->level = hifmc100_bp_to_level(host, BP_NUM_3);
		else
			 host->level = hifmc100_bp_to_level(host, BP_NUM_4);
		break;
	default:
		goto usage;
	}

	spi_lock_update_address(host);
	if (host->end_addr)
		pr_info("Spi is locked. lock address[0 => %#x]\n",
				host->end_addr);
	return 0;
usage:
	DB_MSG("Error:The ID: %#x isn't in the BP table,\n", host->mid);
	DB_MSG("Current device can't not protect\n");
	return 1;
}
/*****************************************************************************/
unsigned char hifmc100_bp_to_level(struct hifmc_host *host,
			unsigned int bp_num)
{
	unsigned char val;
	unsigned char level;
	struct hifmc_spi *spi = host->spi;

	spi->driver->wait_ready(spi);
	val = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(BP_DBG, "Get Status Register[%#x]\n", val);

	if (bp_num == BP_NUM_3)
		level = (val & SPI_NOR_SR_BP_MASK_3) >> SPI_NOR_SR_BP0_SHIFT;
	else
		level = (val & SPI_NOR_SR_BP_MASK_4) >> SPI_NOR_SR_BP0_SHIFT;

	FMC_PR(BP_DBG, "the current level[%d]\n", level);

	if (bp_num == BP_NUM_4) {
		host->lock_level_max = LOCK_LEVEL_MAX(bp_num) - 5;
		if ((host->mid == MID_MXIC) && (spi->chipsize == _16M))
			host->lock_level_max--;
	} else
		host->lock_level_max = LOCK_LEVEL_MAX(bp_num);
	FMC_PR(BP_DBG, "Get the max bp level: [%d]\n", host->lock_level_max);

	return level;
}
/*****************************************************************************/
void spi_lock_update_address(struct hifmc_host *host)
{
	unsigned int lock_level_max, erasesize, chipsize;
	struct hifmc_spi *spi = host->spi;

	if (!host->level) {
		host->end_addr = 0;
		FMC_PR(BP_DBG, "all blocks is unlocked.\n");
		return;
	}

	chipsize = spi->chipsize;
	erasesize = spi->erasesize;
	lock_level_max = host->lock_level_max;

	switch (host->mid) {
	case MID_MXIC:
		if (spi->chipsize == _2M) {
			if ((host->level != lock_level_max)
					&& (host->level != 1))
				host->end_addr = chipsize - (erasesize <<
					(lock_level_max - host->level - 1));
			else
				host->end_addr = chipsize;
			return;
		}

		if (spi->chipsize != _8M)
			break;
	case MID_ESMT:
		/* this case is for ESMT and MXIC 8M devices */
		if (host->level != lock_level_max)
			host->end_addr = chipsize - (erasesize
					<< (lock_level_max - host->level));
		else
			host->end_addr = chipsize;
		return;
	case MID_CFEON:
		if (host->level != lock_level_max)
			host->end_addr = chipsize - (erasesize
					<< (host->level - 1));
		else
			host->end_addr = chipsize;
		return;
	default:
		break;
	}

	/* general case */
	host->end_addr = chipsize >> (lock_level_max - host->level);
}
/*****************************************************************************/
#endif
