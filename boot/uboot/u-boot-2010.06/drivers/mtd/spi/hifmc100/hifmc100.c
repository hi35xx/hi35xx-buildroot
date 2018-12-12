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
static void hifmc100_dma_transfer(struct hifmc_spi *spi,
	unsigned int spi_start_addr, unsigned char *dma_buffer,
	unsigned char rw_op, unsigned int size)
{
	unsigned char if_type = 0, dummy = 0;
	unsigned char w_cmd = 0, r_cmd = 0;
	unsigned int regval;
	struct hifmc_host *host = spi->host;

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

#ifdef CONFIG_DTR_MODE_SUPPORT
	/* DTR MODE disable */
	if (host->dtr_mode_en == DISABLE && host->dtr_training_flag == 1) {
		spi_dtr_dummy_training_set(host, ENABLE);
		hifmc_dtr_mode_ctrl(spi, ENABLE);
		FMC_PR(DTR_DB, "\t|*-Start reg DTR mode read.\n");
	}
#endif

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
#ifdef CONFIG_DTR_MODE_SUPPORT
	/* DTR MODE disable */
	if (host->dtr_mode_en == ENABLE && host->dtr_training_flag == 1) {
		hifmc_dtr_mode_ctrl(spi, DISABLE);
		spi_dtr_dummy_training_set(host, DISABLE);
		FMC_PR(DTR_DB, "\t|*-END reg DTR mode read.\n");
	}
#endif
fail:
	hifmc_ip_user--;
	return result;
}
#endif /* HIFMC100_SPI_NOR_SUPPORT_REG_READ */

/*****************************************************************************/
static int hifmc100_dma_read(struct spi_flash *spiflash, u_int from, size_t len,
				void *buf)
{
	size_t num;
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
#ifdef CONFIG_DTR_MODE_SUPPORT
	/* DTR MODE enable */
	if (host->dtr_mode_en == DISABLE && host->dtr_training_flag == 1) {
		spi_dtr_dummy_training_set(host, ENABLE);
		hifmc_dtr_mode_ctrl(spi, ENABLE);
		FMC_PR(DTR_DB, "\t|*-Start dma DTR mode read.\n");
	}
#endif
	host->set_system_clock(spi->read, ENABLE);

	while (len) {
		num = ((len >= HIFMC100_DMA_RD_MAX_SIZE)
				? HIFMC100_DMA_RD_MAX_SIZE : len);

		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DB_BUG("read memory out of range.\n");

			if (spi->driver->wait_ready(spi)) {
				DB_MSG("Error: Dma read wait ready fail!!\n");
				return -EBUSY;
			}

			host->set_system_clock(spi->read, ENABLE);
		}

		if (from + num > spi->chipsize)
			num = spi->chipsize - from;

		hifmc100_dma_transfer(spi, from, (u_char *)buf,
				RW_OP_READ, num);
		from += num;
		buf  += num;
		len  -= num;
	}

	hifmc_ip_user--;
#ifdef CONFIG_DTR_MODE_SUPPORT
	/* DTR MODE disable */
	if (host->dtr_mode_en == ENABLE && host->dtr_training_flag == 1) {
		hifmc_dtr_mode_ctrl(spi, DISABLE);
		spi_dtr_dummy_training_set(host, DISABLE);
		FMC_PR(DTR_DB, "\t|*-END dma DTR mode read.\n");
	}
#endif
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
	size_t num;
	int result = -EIO;
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

#ifdef CONFIG_SPI_BLOCK_PROTECT
	if (host->level && (to < host->end_addr)) {
		puts("\n");
		printf("ERROR: The DMA write area was locked.");
		return -EINVAL;
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
	host->set_system_clock(spi->write, ENABLE);

	while (len) {
		num = ((len >= HIFMC100_DMA_WR_MAX_SIZE)
			? HIFMC100_DMA_WR_MAX_SIZE : len);

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

		if (to + num > spi->chipsize)
			num = spi->chipsize - to;

		hifmc100_dma_transfer(spi, to, (u_char *)buf, RW_OP_WRITE, num);
		to  += num;
		buf += num;
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

#ifdef CONFIG_SPI_BLOCK_PROTECT
	if ((host->level) && (offset < host->end_addr)) {
		puts("\n");
		printf("ERROR: The erase area was locked.\n");
		return -EINVAL;
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
#ifdef CONFIG_SPI_BLOCK_PROTECT
	host->cmp = BP_CMP_UPDATE_FLAG;
	hifmc100_get_bp_lock_level(host);
	spi_nor_flash->lock = hifmc100_spi_flash_lock;
#endif
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
#ifdef CONFIG_DTR_MODE_SUPPORT
	if (spi->dtr_mode_support) {
		int ret;

		host->dtr_training_flag = 0;
		/* setting DTR mode dummy and traning */
		ret = spi_dtr_dummy_training_set(host, ENABLE);

		/* switch DTR mode to SDR mode */
		hifmc_dtr_mode_ctrl(spi, DISABLE);
		spi_dtr_dummy_training_set(host, DISABLE);
		if (ret) {
			printf(" Reset to SDR mode.\n");
			spi_dtr_to_sdr_switch(spi);
		}
	}
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
	unsigned int reg, flash_type;

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

#ifdef CONFIG_SPI_BLOCK_PROTECT
/*****************************************************************************/
void spi_lock_update_address(struct hifmc_host *host)
{
	unsigned int lock_level_max, erasesize, chipsize;
	unsigned char mid = host->spi_nor_info->ids[0];
	struct hifmc_spi *spi = host->spi;

	if (!host->level) {
		host->end_addr = 0;
		FMC_PR(BP_DBG, "all blocks is unlocked.\n");
		return;
	}

	chipsize = spi->chipsize;
	erasesize = spi->erasesize;
	lock_level_max = host->spi_nor_flash[0].bp_level_max;

	switch (mid) {
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
void hifmc100_get_bp_lock_level(struct hifmc_host *host)
{
	struct hifmc_spi *spi = host->spi;
	unsigned char mid = host->spi_nor_info->ids[0];
	unsigned int lock_level_max;

	FMC_PR(BP_DBG, "Get manufacturer ID: [%#x]\n", mid);

	/* match the manufacture ID to get the block protect info */
	switch (mid) {
	case MID_GD:
	case MID_ESMT:
	case MID_CFEON:
	case MID_SPANSION:
		host->bp_num = BP_NUM_3;
		host->level = hifmc100_bp_to_level(host);
		break;
	case MID_WINBOND:
		if (spi->chipsize <= _16M) {
			host->bp_num = BP_NUM_3;
			host->level = hifmc100_bp_to_level(host);
		} else {
			host->bp_num = BP_NUM_4;
			host->level = hifmc100_bp_to_level(host);
		}
		break;
	case MID_MXIC:
		if (spi->chipsize <= _8M) {
			host->bp_num = BP_NUM_3;
			host->level = hifmc100_bp_to_level(host);
		} else {
			host->bp_num = BP_NUM_4;
			host->level = hifmc100_bp_to_level(host);
		}
		break;
	case MID_MICRON:
	case MID_PARAGON:
		return;
	default:
		goto usage;
	}

	/* this branch only for initialization */
	if (host->cmp == BP_CMP_UPDATE_FLAG) {
		/* get the max block protect level of current manufacture ID */
		if (host->bp_num == BP_NUM_4) {
			lock_level_max = LOCK_LEVEL_MAX(host->bp_num) - 5;
			/* just for MXIC(16M), the max lock level is 9 */
			if ((mid == MID_MXIC) && (spi->chipsize == _16M))
				lock_level_max--;
		} else
			lock_level_max = LOCK_LEVEL_MAX(host->bp_num);

		host->spi_nor_flash[0].bp_level_max = lock_level_max;
		FMC_PR(BP_DBG, "Get the max bp level: [%d]\n", lock_level_max);

		spi_lock_update_address(host);
		if (host->end_addr)
			printf("Spi is locked. lock address[0 => %#x]\n",
					host->end_addr);
	}
	return;
usage:
	DB_MSG("Error:The ID: %#x isn't in the BP table,\n", mid);
	DB_MSG("Current device can't not protect\n");
}
/*****************************************************************************/
unsigned short hifmc100_set_spi_lock_info(struct hifmc_host *host)
{
	unsigned short val;
	unsigned char mid = host->spi_nor_info->ids[0];
	struct hifmc_spi *spi = host->spi;

	FMC_PR(BP_DBG, "Get manufacturer ID: [%#x]\n", mid);

	/* match the manufacture ID to get the block protect set info */
	switch (mid) {
	case MID_SPANSION:
		val = hifmc100_handle_bp_rdcr_info(host, SPI_CMD_RDCR);
		break;
	case MID_MXIC:
		if (spi->chipsize < _16M)
			val = hifmc100_handle_bp_rdsr_info(host, SPI_CMD_RDSR);
		else
			val = hifmc100_handle_bp_rdcr_info(host,
					SPI_CMD_RDCR_MX);
		break;
	case MID_GD:
	case MID_ESMT:
	case MID_CFEON:
	case MID_WINBOND:
		val = hifmc100_handle_bp_rdsr_info(host, SPI_CMD_RDSR);
		break;
	default:
		goto usage;
	}

	return val;
usage:
	DB_MSG("Error: The manufacture ID is change,\n Pleaer check!!\n");
	DB_MSG("Error: The ID: %#x isn't in the BP table,\n", mid);
	return 1;
}

/*****************************************************************************/
unsigned short hifmc100_handle_bp_rdcr_info(struct hifmc_host *host, u_char cmd)
{
	unsigned char status, config;
	struct hifmc_spi *spi = host->spi;
	unsigned char mid = host->spi_nor_info->ids[0];

	/* this macro definition is for determining the writing length */
	host->bt_loc = BT_LOC_RDCR;
	spi->driver->wait_ready(spi);

	/* get the block protect B/P info in config register */
	config = spi_general_get_flash_register(spi, cmd);
	FMC_PR(BP_DBG, "Get Config Register[%#x]\n", config);

	/* the location of T/B bit in config register is different.
	 SPANSION is 5th and the MXIC(16/32M) is 3th */
	if (mid == MID_SPANSION) {
		config = SPI_BP_BOTTOM_RDCR_SET_S(config);
		FMC_PR(BP_DBG, "Set Config Register[%#x]\n", config);
	} else {
		config = SPI_BP_BOTTOM_RDCR_SET(config);
		FMC_PR(BP_DBG, "Set Config Register[%#x]\n", config);
	}

	/* get the block protect level info in status register */
	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(BP_DBG, "Get Status Register[%#x]\n", status);

	return ((unsigned short)config << SPI_NOR_CR_SHIFT) | status;
}
/*****************************************************************************/
unsigned short hifmc100_handle_bp_rdsr_info(struct hifmc_host *host,
					u_char cmd)
{
	unsigned char val;
	struct hifmc_spi *spi = host->spi;
	unsigned char mid = host->spi_nor_info->ids[0];

	/* this macro definition is for determining the writing length */
	host->bt_loc = BT_LOC_RDSR;
	spi->driver->wait_ready(spi);

	/* get the block protect level and B/T info in status register */
	val = spi_general_get_flash_register(spi, cmd);
	FMC_PR(BP_DBG, "Get Status Register[%#x]\n", val);
	if (mid == MID_CFEON)
		val &= SPI_BP_BOTTOM_RDSR_SET_0(host->bp_num);
	else
		val |= SPI_BP_BOTTOM_RDSR_SET_1(host->bp_num);
	FMC_PR(BP_DBG, "Set Config Register[%#x]\n", val);

	return val;
}

/*****************************************************************************/
static void hifmc100_set_bp_level(struct hifmc_host *host, unsigned char level)
{
	unsigned char old_level;
	unsigned short val;
	unsigned int reg;
	struct hifmc_spi *spi = host->spi;
	unsigned char mid = host->spi_nor_info->ids[0];

	FMC_PR(BP_DBG, "* Start BP bottom level %d\n", level);

	val = hifmc100_set_spi_lock_info(host);
	old_level = host->level;
	FMC_PR(BP_DBG, "  Read CR:SR[%#x]\n", val);

	if (old_level != level) {
		if (host->bp_num == BP_NUM_3)
			val &= ~SPI_NOR_SR_BP_MASK_3;
		else
			val &= ~SPI_NOR_SR_BP_MASK_4;
		val |= level << SPI_NOR_SR_BP0_SHIFT;
		FMC_PR(BP_DBG, "Set Status Register[%#x]\n", val);
	} else {
		FMC_PR(BP_DBG, "NOTES: old_level[%#x] = level[%#x]\n",
				old_level, level);
		return;
	}

	if (((mid == MID_ESMT) || ((mid == MID_MXIC)
				&& (spi->chipsize < _16M))) && (level == 0)) {
		val &= SPI_BP_BOTTOM_RDSR_SET_0(host->bp_num);
		FMC_PR(BP_DBG, "set level = 0[PB3 = 0]:[%#x]\n", val);
	}

	spi->driver->write_enable(spi);
	reg = hifmc_read(host, FMC_GLOBAL_CFG);
	if (reg & FMC_GLOBAL_CFG_WP_ENABLE) {
		FMC_PR(BP_DBG, " Hardware protected enable!, reg[%#x]\n", reg);
		reg &= ~FMC_GLOBAL_CFG_WP_ENABLE;
		hifmc_write(host, FMC_GLOBAL_CFG, reg);
		val &= ~SPI_NOR_SR_SRWD_MASK;
		FMC_PR(BP_DBG, "Disable SR[%d]:SRWD and WP#\n",
				SPI_NOR_SR_SRWD_SHIFT);
	}

	if (host->bt_loc == BT_LOC_RDSR) {
		writeb(val, host->iobase);
		FMC_PR(BP_DBG, "Write IO[%#x]%#x\n", (unsigned int)host->iobase,
				*(unsigned char *)host->iobase);
	} else {
		writew(val, host->iobase);
		FMC_PR(BP_DBG, "Write IO[%#x]%#x\n", (unsigned int)host->iobase,
				*(unsigned short *)host->iobase);
	}

	reg = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, reg);
	FMC_PR(BP_DBG, " Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	FMC_PR(BP_DBG, " Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	if (host->bt_loc == BT_LOC_RDSR)
		reg = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN);
	else
		reg = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN + SPI_NOR_CR_LEN);

	hifmc_write(host, FMC_DATA_NUM, reg);
	FMC_PR(BP_DBG, " Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATA_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	FMC_PR(BP_DBG, " Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	FMC_PR(BP_DBG, "* Set BP level end.\n");
}

/*****************************************************************************/
void hifmc100_spi_lock(struct hifmc_host *host, unsigned char level)
{
	unsigned char current_level;

	hifmc100_set_bp_level(host, level);

	/* check if we have set successfully or not */
	current_level = hifmc100_bp_to_level(host);
	if (current_level != level) {
		DB_MSG("Error: Current lock level: %d, but set value: %d\n",
			current_level, level);
		return;
	}

	host->level = level;
	spi_lock_update_address(host);

	if (host->end_addr)
		printf("Spi is locked. lock address[0 => %#x]\n",
				host->end_addr);

	return;
}

/*****************************************************************************/
unsigned char hifmc100_bp_to_level(struct hifmc_host *host)
{
	unsigned char val;
	unsigned char level;
	struct hifmc_spi *spi = host->spi;

	spi->driver->wait_ready(spi);
	val = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	FMC_PR(BP_DBG, "Get Status Register[%#x]\n", val);

	FMC_PR(BP_DBG, "the bp_num[%d]\n", host->bp_num);

	if (host->bp_num == BP_NUM_3)
		level = (val & SPI_NOR_SR_BP_MASK_3) >> SPI_NOR_SR_BP0_SHIFT;
	else
		level = (val & SPI_NOR_SR_BP_MASK_4) >> SPI_NOR_SR_BP0_SHIFT;

	FMC_PR(BP_DBG, "the current level[%d]\n", level);

	return level;
}
/*****************************************************************************/
#endif /* CONFIG_SPI_BLOCK_PROTECT */

#ifdef CONFIG_DTR_MODE_SUPPORT
/*****************************************************************************/
int spi_dtr_dummy_training_set(struct hifmc_host *host, int dtr_en)
{
	struct hifmc_spi *spi = host->spi;
	int ret;

	switch (spi->dtr_cookie) {
	case DTR_MODE_SET_ODS:
		if (spi->driver->dtr_set_device)
			spi->driver->dtr_set_device(spi, dtr_en);
		break;
	case DTR_MODE_SET_NONE:
	default:
		break;
	}

	/* disable DTR mode without training */
	/* dtr dummy training is done, return it */
	if ((host->dtr_training_flag == 1) || (dtr_en == DISABLE))
		return 0;

	/* enable DTR mode and set sample point */
	hifmc_dtr_mode_ctrl(spi, ENABLE);

	/* set training */
	ret = spi_dtr_training(host);
	if (ret) {
		printf("* Set dtr training fail.\n");
		return 1;
	}
	FMC_PR(DTR_DB, "* Set dtr and dummy end.\n");
	return 0;
}
/*****************************************************************************/
static int dtr_training_handle(struct hifmc_host *host)
{
	int ret_tmp = 0, ret, ix;
	unsigned int reg = 0, regval, p_count = 0, p_temp = 0;
	unsigned char *buf, status[DTR_TRAINING_POINT_NUM] = {0};
	struct hifmc_spi *spi = host->spi;

	/* set div 4 clock */
	host->set_system_clock(spi->read, ENABLE);

	buf = malloc(DTR_TRAINING_CMP_LEN);
	if (!buf)
		return -1;

	/* start training to check every sample point */
	regval = hifmc_read(host, FMC_GLOBAL_CFG);
	for (ix = 0; ix < DTR_TRAINING_POINT_NUM; ix++) {
		regval = DTR_TRAINING_POINT_CLR(regval);
		regval |= (ix << DTR_TRAINING_POINT_MASK);
		FMC_PR(DTR_DB, " setting the dtr training point:%d\n", ix);
		hifmc_write(host, FMC_GLOBAL_CFG, regval);
		FMC_PR(DTR_DB, " Set dtr_training[%#x]%#x\n",
				FMC_GLOBAL_CFG, regval);
		/* read */
		spi->driver->wait_ready(spi);
		hifmc100_dma_transfer(spi, DTR_TRAINING_CMP_ADDR_SHIFT, buf,
				RW_OP_READ, DTR_TRAINING_CMP_LEN);
		ret = memcmp((const void *)buf,
				(const void *)DTR_TRAINING_CMP_ADDR_S,
				DTR_TRAINING_CMP_LEN);
		if (ret == 0) {
			ret_tmp = 1;
			status[ix] = 1; /* like */
			FMC_PR(DTR_DB, " status[%d] = 1\n", ix);
		}
		if (!ret_tmp && (ix == DTR_TRAINING_POINT_NUM - 1))
			goto fail_training;
	}

	kfree(buf);

	/* select the best smaple point */
	for (ix = 0; ix < DTR_TRAINING_POINT_NUM;) {
		if (status[ix] == 1) {
			p_count++;
			ix++;
			if ((DTR_TRAINING_POINT_NUM == ix)
					&& (p_count > p_temp)) {
				p_temp = p_count;
				p_count = 0;
				reg = ix - ((p_temp + 1) >> 1);
				FMC_PR(DTR_DB, "the sample point choice: %#x\n",
						reg);
				break;
			}
			if ((status[ix] == 0) && (p_count > p_temp)) {
				p_temp = p_count;
				p_count = 0;
				reg = ix - ((p_temp + 1) >> 1);
				FMC_PR(DTR_DB, "the sample point choice: %#x\n",
						reg);
				break;
			}
			continue;
		}
		ix++;
	}

	/* to set the best sample point */
	regval = DTR_TRAINING_POINT_CLR(regval);
	regval |= (reg << DTR_TRAINING_POINT_MASK);
	FMC_PR(DTR_DB, " set the sample point[%#x]%#x\n",
			FMC_GLOBAL_CFG, regval);
	hifmc_write(host, FMC_GLOBAL_CFG, regval);

	/* training handle end */
	return regval;

fail_training:
	printf("Cannot find an useful sample point.\n");
	kfree(buf);
	return -1;
}
/*****************************************************************************/
unsigned int spi_dtr_training(struct hifmc_host *host)
{
	int reg, cur_reg;

	FMC_PR(DTR_DB, "DTR traiining start ...\n");

	/* DTR traiining start */
	reg = dtr_training_handle(host);
	if (reg == -1) {
		host->dtr_training_flag = 0;
		printf("DTR traiining fail.\n");
		return 1;
	}
	FMC_PR(DTR_DB, "* DTR traiining end.\n");
	cur_reg = hifmc_read(host, FMC_GLOBAL_CFG);

	/* to check whether training is done */
	if (cur_reg == reg) {
		host->dtr_training_flag = 1;
		FMC_PR(DTR_DB, "* Set dtr_training seccess.\n");
		return 0;
	}
	return 1;
}
/*****************************************************************************/
void hifmc_dtr_mode_ctrl(struct hifmc_spi *spi, int dtr_en)
{
	unsigned int regval;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	host->dtr_mode_en = dtr_en;
	regval = hifmc_read(host, FMC_GLOBAL_CFG);
	if (dtr_en == ENABLE) {
		/* enable DTR mode and set the DC value */
		regval |= (1 << DTR_MODE_REQUEST_SHIFT);
		hifmc_write(host, FMC_GLOBAL_CFG, regval);
		FMC_PR(DTR_DB, " enable dtr mode[%#x]%#x\n",
				FMC_GLOBAL_CFG, regval);
	} else {
		/* disable DTR mode */
		regval &= (~(1 << DTR_MODE_REQUEST_SHIFT));
		hifmc_write(host, FMC_GLOBAL_CFG, regval);
		FMC_PR(DTR_DB, " disable dtr mode[%#x]%#x\n",
				FMC_GLOBAL_CFG, regval);
	}
}
/*****************************************************************************/
void hifmc_check_spi_dtr_support(struct hifmc_spi *spi, u_char *ids)
{
	unsigned manu_id = ids[0], dev_id = ids[1];

	spi->dtr_mode_support = 0;
	spi->dtr_cookie = DTR_MODE_SET_NONE;

	switch (manu_id) {
	case MID_MXIC:
		if (spi_mxic_check_spi_dtr_support(spi)) {
			spi->dtr_cookie = DTR_MODE_SET_ODS;
			goto dtr_on;
		}
		break;
	case MID_WINBOND:
		/* Device ID: 0x70 means support DTR Mode for Winbond */
		if (dev_id == DEVICE_ID_SUPPORT_DTR_WINBOND) {
			spi->dtr_mode_support = 1;
			goto dtr_on;
		}
		break;
	default:
		break;
	}

	FMC_PR(DTR_DB, "The Double Transfer Rate Read Mode isn't supported.\n");
	return;

dtr_on:
	printf("The Double Transfer Rate Read Mode is supported.\n");
}
#endif /* CONFIG_DTR_MODE_SUPPORT */
