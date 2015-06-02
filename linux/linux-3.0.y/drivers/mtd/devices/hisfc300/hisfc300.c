/******************************************************************************
*    Copyright (c) 2009-2010 by czy.
*    All rights reserved.
* ***
*
******************************************************************************/

/*****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/math64.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <asm/setup.h>
#include <linux/io.h>

#include "../spi_ids.h"
#include "hisfc300.h"

#define mtd_has_partitions()             (1)
/*****************************************************************************/

#define HISFC200_SUPPORT_READ             (SPI_IF_READ_STD \
	| SPI_IF_READ_FAST \
	| SPI_IF_READ_DUAL)

#define HISFC200_SUPPORT_MAX_DUMMY        (1)
#define HISFC200_SUPPORT_WRITE            (SPI_IF_WRITE_STD)

/*****************************************************************************/
static char *int_to_size(unsigned long long size)
{
	int ix;
	static char buffer[20];
	char *fmt[] = {"%u", "%uK", "%uM", "%uG", "%uT", "%uT"};

	for (ix = 0; (ix < 5) && !(size & 0x3FF) && size; ix++)
		size = (size >> 10);

	sprintf(buffer, fmt[ix], size);
	return buffer;
}

/*****************************************************************************/
static void hisfc200_config(struct hisfc_host *host, struct hisfc_spi *spi,
	struct spi_operation *op, int clk_en, int writecmd, int readcmd)
{
	unsigned int regval = 0;

	if (spi && op) {
		regval = HISFC200_CONFIG_IF_TYPE(op->iftype)
			| HISFC200_CONFIG_DUMMY_BYTE(op->dummy)
			| HISFC200_CONFIG_WIP_LOCATE(0)
			| (spi->addrcycle == 4 ?
					HISFC200_CONFIG_ADDR_MODE_4B : 0)
			| HISFC200_CONFIG_RD_DELAY(0)
			| HISFC200_CONFIG_PREFETCH_CNT(0)
			| HISFC200_CONFIG_RD_INS(readcmd)
			| HISFC200_CONFIG_WR_INS(writecmd);
	}
	hisfc_write(host, HISFC200_CONFIG, regval);

	hisfc200_set_system_clock(host, op, clk_en);
}

/*****************************************************************************/
static void hisfc200_map_iftype_and_clock(struct hisfc_spi *spi)
{
	int ix, jx;
	int iftype_read[] = {
		SPI_IF_READ_STD,       0,
		SPI_IF_READ_FAST,      0,
		SPI_IF_READ_DUAL,      4,
		SPI_IF_READ_QUAD,      6,
		SPI_IF_READ_QUAD_ADDR, 7,
		0, 0,
	};

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}

	/* std write */
	spi->write->iftype = 0;
	hisfc200_get_best_clock(&(spi->read->clock));
	hisfc200_get_best_clock(&(spi->write->clock));

	DBG_MSG("map: spi->read->iftype:0x%08X\n",  spi->read->iftype);
	DBG_MSG("map: spi->read->clock:0x%08X\n",   spi->read->clock);
	DBG_MSG("map: spi->write->iftype:0x%08X\n", spi->write->iftype);
	DBG_MSG("map: spi->write->clock:0x%08X\n",  spi->write->clock);

	for (jx = 0; (jx < MAX_SPI_OP) && spi->erase[jx].iftype; jx++) {
		hisfc200_get_best_clock(&spi->erase[jx].clock);
		spi->erase[jx].iftype = 0;
		DBG_MSG("map: spi->erase[%d].clock:0x%08X size:%s\n",
			jx, spi->erase[jx].clock,
			int_to_size(spi->erase[jx].size));
	}
}

static int hisfc200_wait_ready(struct hisfc_host *host, struct hisfc_spi *spi)
{
	unsigned long regval;
	unsigned long deadline;

	deadline = jiffies + HISFC300_MAX_READY_WAIT_JIFFIES;
	do {
		hisfc_write(host, HISFC200_INSTR, SPI_CMD_RDSR);
		hisfc_write(host, HISFC200_CMD,
				HISFC200_CMD_CS(spi->chipselect)
				| HISFC200_CMD_DATA_CNT(1)
				| HISFC200_CMD_RDDATA_EN
				| HISFC200_CMD_START);

		while ((hisfc_read(host, HISFC200_CMD) & HISFC200_CMD_START))
			cond_resched();

		regval = hisfc_read(host, HISFC200_DATABUFFER1);
		if (!(regval & SPI_CMD_SR_WIP))
			return 0;

		cond_resched();

	} while (!time_after_eq(jiffies, deadline));

	printk(KERN_ERR "!!! Wait spi flash ready timeout.\n");

	return 1;
}
/*****************************************************************************/

static void hisfc200_enable_4byte_mode(struct hisfc_host *host,
		struct hisfc_spi *spi, int enable)
{
	if (spi->addrcycle != 4 && enable) {
		DBG_BUG("spi->addrcycle(%d) != 4\n", spi->addrcycle);
		return;
	}

	if (enable)
		hisfc_write(host, HISFC200_INSTR, SPI_CMD_EN4B);
	else
		hisfc_write(host, HISFC200_INSTR, SPI_CMD_EX4B);

	hisfc_write(host, HISFC200_CMD,
			HISFC200_CMD_CS(spi->chipselect)
			| HISFC200_CMD_START);

	while ((hisfc_read(host, HISFC200_CMD) & HISFC200_CMD_START))
		;
}

/*****************************************************************************/
/* read data used bus mode */
static int hisfc200_bus_read(struct mtd_info *mtd, loff_t from, size_t len,
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

	if (hisfc200_wait_ready(host, spi))
		goto fail;
	hisfc200_config(host, spi, spi->read, 1, 0, spi->read->cmd);

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");

			if (hisfc200_wait_ready(host, spi))
				goto fail;
			hisfc200_config(host, spi,
					spi->read, 1, 0, spi->read->cmd);
		}

		num = ((from + len) >= spi->chipsize) ?
			(spi->chipsize - from) : len;

		while (num >= HISFC200_MEM_READ_SLICE) {
			memcpy(ptr, (char *)spi->iobase + from,
					HISFC200_MEM_READ_SLICE);

			ptr  += HISFC200_MEM_READ_SLICE;
			from += HISFC200_MEM_READ_SLICE;
			len  -= HISFC200_MEM_READ_SLICE;
			num  -= HISFC200_MEM_READ_SLICE;

			cond_resched();
		}

		if (num) {
			memcpy(ptr, (char *)spi->iobase + from, num);
			from += num;
			ptr  += num;
			len  -= num;
		}
	}
	result = 0;
fail:
	mutex_unlock(&host->lock);
	*retlen = (size_t)(ptr - buf);
	return result;
}
/*****************************************************************************/
/* read one buffer from reg */
static int hisfc200_reg_read_one_buffer(struct hisfc_host *host,
		struct hisfc_spi *spi, unsigned int offset,
		unsigned char *buf, int count)
{
	int num = 0;
	int regindex = 0;
	unsigned int regval;

	hisfc_write(host, HISFC200_INSTR, spi->read->cmd);

	hisfc_write(host, HISFC200_ADDR,
			(offset & HISFC200_ADDR_MASK));

	hisfc_write(host, HISFC200_CMD,
			HISFC200_CMD_CS(spi->chipselect)
			| HISFC200_CMD_DUMMY_CNT(spi->read->dummy)
			| HISFC200_CMD_DATA_CNT(count)
			| HISFC200_CMD_ADDR_EN
			| HISFC200_CMD_RDDATA_EN
			| HISFC200_CMD_START);

	while ((hisfc_read(host, HISFC200_CMD) & HISFC200_CMD_START))
		cond_resched();

	while (count > 0) {
		regval = hisfc_read(host, (HISFC200_DATABUFFER1 + regindex));
		num = ((count > 4) ? 4 : count);
		memcpy(buf, &regval, num);

		buf      += num;
		regindex += 4;
		count    -= num;
	}

	return 0;
}
/*****************************************************************************/
/* flash regist read */
static int hisfc200_reg_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	if ((from + len) > mtd->size) {
		DBG_MSG("read area out fo range.\n");
		return -EINVAL;
	}

	*retlen = 0;
	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	mutex_lock(&host->lock);

	if (hisfc200_wait_ready(host, spi))
		goto fail;
	hisfc200_config(host, spi, spi->read, 1, 0, 0);

	if (from & HISFC200_REG_BUF_MASK) {
		num = HISFC200_REG_BUF_SIZE - (from & HISFC200_REG_BUF_MASK);
		if (num > len)
			num = len;

		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");

			if (hisfc200_wait_ready(host, spi))
				goto fail;
			hisfc200_config(host, spi, spi->read, 1, 0, 0);
		}

		hisfc200_reg_read_one_buffer(host, spi, from, ptr, num);
		from += num;
		ptr  += num;
		len  -= num;
	}

	while (len > 0) {
		num = ((len >= HISFC200_REG_BUF_SIZE) ?
				HISFC200_REG_BUF_SIZE : len);

		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");

			if (hisfc200_wait_ready(host, spi))
				goto fail;
			hisfc200_config(host, spi, spi->read, 1, 0, 0);
		}
		hisfc200_reg_read_one_buffer(host, spi, from, ptr, num);
		from += num;
		ptr  += num;
		len  -= num;
	}
	result = 0;
fail:
	mutex_unlock(&host->lock);
	*retlen = (size_t)(ptr - buf);
	return result;
}
/*****************************************************************************/

static unsigned char *hisfc200_read_ids(struct hisfc_host *host, int chipselect,
	unsigned char *buffer, int numread)
{
	int regindex = 0;
	unsigned long *ptr = (unsigned long *)buffer;

	if (numread > HISFC200_REG_BUF_SIZE)
		numread = HISFC200_REG_BUF_SIZE;

	hisfc_write(host, HISFC200_INSTR, SPI_CMD_RDID);
	hisfc_write(host, HISFC200_CMD,
			HISFC200_CMD_CS(chipselect)
			| HISFC200_CMD_RDDATA_EN
			| HISFC200_CMD_DATA_CNT(numread)
			| HISFC200_CMD_START);

	while ((hisfc_read(host, HISFC200_CMD) & HISFC200_CMD_START))
		cond_resched();

	numread = ((numread + 0x03) & (~0x03));
	while (numread) {
		*ptr = hisfc_read(host, HISFC200_DATABUFFER1 + regindex);
		ptr      += 1;
		regindex += 4;
		numread  -= 4;
	}

	return buffer;
}
/*****************************************************************************/
/* enable spi write. */
static int hisfc200_write_enable(struct hisfc_host *host, struct hisfc_spi *spi)
{
	hisfc_write(host, HISFC200_INSTR, SPI_CMD_WREN);

	hisfc_write(host, HISFC200_CMD,
		(HISFC200_CMD_CS(spi->chipselect) | HISFC200_CMD_START));

	while ((hisfc_read(host, HISFC200_CMD) & HISFC200_CMD_START))
		cond_resched();

	return 0;
}
/*****************************************************************************/
/* erase one block */
static int hisfc200_reg_erase_one_block(struct hisfc_host *host,
		struct hisfc_spi *spi, unsigned long offset, int cmdindex)
{
	if (hisfc200_wait_ready(host, spi))
		return 1;

	hisfc200_config(host, spi, &spi->erase[cmdindex], 1, 0, 0);

	hisfc200_write_enable(host, spi);

	hisfc_write(host, HISFC200_INSTR, spi->erase[cmdindex].cmd);

	hisfc_write(host, HISFC200_ADDR,
			(offset & HISFC200_ADDR_MASK));

	hisfc_write(host, HISFC200_CMD,
			HISFC200_CMD_CS(spi->chipselect)
			| HISFC200_CMD_DUMMY_CNT(spi->erase[cmdindex].dummy)
			| HISFC200_CMD_ADDR_EN
			| HISFC200_CMD_START);

	while ((hisfc_read(host, HISFC200_CMD) & HISFC200_CMD_START))
		cond_resched();

	return 0;
}
/*****************************************************************************/

static int hisfc200_reg_write_one_buffer(struct hisfc_host *host,
		struct hisfc_spi *spi, unsigned int offset,
		const unsigned char *buf, int count)
{
	int num = 0;
	int regindex = 0;
	int num_write = count;
	unsigned int regval;

	if (hisfc200_wait_ready(host, spi))
		return 1;

	hisfc200_write_enable(host, spi);

	hisfc_write(host, HISFC200_INSTR, spi->write->cmd);

	hisfc_write(host, HISFC200_ADDR,
			(offset & HISFC200_ADDR_MASK));

	while (count > 0) {
		num = ((count > 4) ? 4 : count);
		memcpy(&regval, buf, num);
		hisfc_write(host, (HISFC200_DATABUFFER1 + regindex), regval);

		buf      += num;
		regindex += 4;
		count    -= num;
	}

	hisfc_write(host, HISFC200_CMD,
			HISFC200_CMD_CS(spi->chipselect)
			| HISFC200_CMD_DUMMY_CNT(spi->write->dummy)
			| HISFC200_CMD_DATA_CNT(num_write)
			| HISFC200_CMD_ADDR_EN
			| HISFC200_CMD_WRDATA_EN
			| HISFC200_CMD_START);

	while ((hisfc_read(host, HISFC200_CMD) & HISFC200_CMD_START))
		cond_resched();

	return 0;
}
/*****************************************************************************/

static int hisfc200_reg_write(struct mtd_info *mtd, loff_t to, size_t len,
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

	if (hisfc200_wait_ready(host, spi))
		goto fail;
	hisfc200_config(host, spi, spi->write, 1, 0, 0);

	if (to & HISFC200_REG_BUF_MASK) {
		num = HISFC200_REG_BUF_SIZE - (to & HISFC200_REG_BUF_MASK);
		if (num > (int)len)
			num = (int)len;

		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (hisfc200_wait_ready(host, spi))
				goto fail;
			hisfc200_config(host, spi, spi->write, 1, 0, 0);
		}
		if (hisfc200_reg_write_one_buffer(host, spi, to, ptr, num))
			goto fail;
		to  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0) {
		num = ((len >= HISFC200_REG_BUF_SIZE)
				? HISFC200_REG_BUF_SIZE : len);

		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (hisfc200_wait_ready(host, spi))
				goto fail;
			hisfc200_config(host, spi, spi->write, 1, 0, 0);
		}
		if (hisfc200_reg_write_one_buffer(host, spi, to, ptr, num))
			goto fail;
		to  += num;
		ptr += num;
		len -= num;
	}
	result = 0;
fail:
	mutex_unlock(&host->lock);
	*retlen = (size_t)(ptr - buf);
	return result;
}
/*****************************************************************************/

static int hisfc200_reg_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct hisfc_host *host = MTD_TO_HOST(mtd);
	struct hisfc_spi *spi = host->spi;

	int cmdindex;
	unsigned long long offset = instr->addr;
	unsigned long long len    = instr->len;

	if (instr->addr + instr->len > mtd->size) {
		DBG_MSG("erase area out of range of mtd.\n");
		return -EINVAL;
	}

	if ((unsigned int)instr->addr & (mtd->erasesize-1)) {
		DBG_MSG("erase start address is not alignment.\n");
		return -EINVAL;
	}

	if ((unsigned int)instr->len & (mtd->erasesize-1)) {
		DBG_MSG("erase length is not alignment.\n");
		return -EINVAL;
	}

	mutex_lock(&host->lock);
	while (len) {
		if (spi->chipsize <= offset) {
			offset -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("erase memory out of range.\n");

		}

		for (cmdindex = 0; cmdindex < MAX_SPI_OP; cmdindex++) {
			if ((spi->chipsize - offset)
					>= spi->erase[cmdindex].size
					&& len >= spi->erase[cmdindex].size)
				break;
		}

		if (hisfc200_reg_erase_one_block(host, spi,
					(unsigned long)offset, cmdindex)) {
			instr->state = MTD_ERASE_FAILED;
			mutex_unlock(&host->lock);
			return -EIO;
		}

		offset += spi->erase[cmdindex].size;
		len    -= spi->erase[cmdindex].size;
	}

	instr->state = MTD_ERASE_DONE;
	mutex_unlock(&host->lock);
	mtd_erase_callback(instr);

	return 0;
}
/*****************************************************************************/

static int hisfc200_map_chipsize(unsigned long long chipsize)
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

static int hisfc200_spi_probe(struct hisfc_host *host)
{
	int chipselect;
	unsigned int total = 0;
	unsigned char ids[8];
	struct spi_info *spiinfo;
	struct hisfc_spi *spi = host->spi;

	host->num_chip = 0;
	for (chipselect = (CONFIG_HISFC300_CHIP_NUM - 1);
			chipselect >= 0; chipselect--) {
		hisfc200_read_ids(host, chipselect, ids, 8);

		if (!(ids[0] | ids[1] | ids[2])
				|| ((ids[0] & ids[1] & ids[2]) == 0xFF))
			continue;

		printk(KERN_INFO "Spi(cs%d) ID: "
				"0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
				chipselect,
				ids[0], ids[1], ids[2], ids[3], ids[4], ids[5]);

		spiinfo = spi_serach_ids(ids);
		if (spiinfo) {
			spi->name = spiinfo->name;
			spi->chipselect = chipselect;
			spi->chipsize   = spiinfo->chipsize;
			spi->erasesize  = spiinfo->erasesize;
			spi->addrcycle  = spiinfo->addrcycle;

			spi_search_rw(spiinfo, spi->read,
					HISFC200_SUPPORT_READ,
					HISFC200_SUPPORT_MAX_DUMMY, 1);

			spi_search_rw(spiinfo, spi->write,
					HISFC200_SUPPORT_WRITE,
					HISFC200_SUPPORT_MAX_DUMMY, 0);

			DBG_MSG("spi->read->iftype:0x%02X, cmd:0x%02X, "
					"clock:0x%02X\n", spi->read->iftype,
					spi->read->cmd, spi->read->clock);
			DBG_MSG("spi->write->iftype:0x%02X, cmd:0x%02X\n, "
					"clock:0x%02X", spi->write->iftype,
					spi->write->cmd, spi->write->clock);

			spi_get_erase(spiinfo, spi->erase, &spi->erasesize);
			hisfc200_map_iftype_and_clock(spi);

			/* set chip select.*/
			hisfc_write(host,
				(HISFC200_CS0CONFIG + (chipselect << 2)),
				HISFC200_CSxCONFIG_CHIPSIZE(
					hisfc200_map_chipsize(spi->chipsize)));

			/* set memory map address. */
			hisfc_write(host,
				(HISFC200_CS0BASEADDR + (chipselect << 2)),
				(CONFIG_HISFC300_BUFFER_BASE_ADDRESS + total));

			DBG_MSG("0x%08X: 0x%08X\n",
				(HISFC200_CS0CONFIG + (chipselect << 2)),
				hisfc_read(host,
				(HISFC200_CS0CONFIG + (chipselect << 2))));
			DBG_MSG("0x%08X: 0x%08X\n",
				(HISFC200_CS0BASEADDR + (chipselect << 2)),
				hisfc_read(host,
				(HISFC200_CS0BASEADDR + (chipselect << 2))));

			spi->iobase = (char *)host->iobase + total;

			if (spi->addrcycle == 4) {
				DBG_MSG("entry 4 bytes modes.\n");
				hisfc200_enable_4byte_mode(host, spi, 1);
			}

			printk(KERN_CONT "Spi(cs%d): ", spi->chipselect);
			printk(KERN_CONT "Block:%sB ",
					int_to_size(spi->erasesize));
			printk(KERN_CONT "Chip:%sB ",
					int_to_size(spi->chipsize));
			printk(KERN_CONT "(Name:%s)\n", spi->name);

			host->num_chip++;
			total += spi->chipsize;
			spi++;
		} else
			printk(KERN_ERR "Spi(cs%d): Unknow\n", chipselect);
	}

	return host->num_chip;
}
/*****************************************************************************/

static int hisfc200_probe(struct hisfc_host *host)
{
	struct mtd_info *mtd = host->mtd;

	hisfc_write(host, HISFC200_TIMING,
			HISFC200_TIMING_TCSS(0)
			| HISFC200_TIMING_TCSH(0)
			| HISFC200_TIMING_TRPD(0)
			| HISFC200_TIMING_TSHSL(4)
			| HISFC200_TIMING_TSHWL(4));

	if (!hisfc200_spi_probe(host))
		return -1;

	mtd->erase = hisfc200_reg_erase;
	mtd->write = hisfc200_reg_write;
	if (1)
		mtd->read  = hisfc200_bus_read;
	else
		mtd->read  = hisfc200_reg_read;
	return 0;
}
/*****************************************************************************/
#define MAX_MTD_PARTITIONS         (32)

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
/*****************************************************************************/
static struct partition_info ptn_info;

static int __init parse_spi_partitions(const struct tag *tag)
{
	int i;

	if (tag->hdr.size <= 2) {
		printk(KERN_INFO
			"%s(%d): tag->hdr.size <= 2\n", __func__, __LINE__);
		return 0;
	}
	ptn_info.parts_num = (tag->hdr.size - 2)
		/ (sizeof(struct partition_entry)/sizeof(int));
	memcpy(ptn_info.entry, &tag->u,
			ptn_info.parts_num * sizeof(struct partition_entry));

	for (i = 0; i < ptn_info.parts_num; i++) {
		ptn_info.parts[i].name   = ptn_info.entry[i].name;
		ptn_info.parts[i].size   = (ptn_info.entry[i].length);
		ptn_info.parts[i].offset = (ptn_info.entry[i].start);
		ptn_info.parts[i].mask_flags = 0;
		ptn_info.parts[i].ecclayout  = 0;
	}

	return 0;
}
__tagtable(0x48695370, parse_spi_partitions);
/*****************************************************************************/

static int mcm(int m, int n)
{
	unsigned int total = m * n;
	unsigned int tt;
	if (m < n)
		tt = m; m = n; n = tt;

	while (n) {
		m = (m % n);
		if (m < n) {
			tt = m; m = n; n = tt;
		}
	}

	return (int)(total / m);
}
/*****************************************************************************/

static void hisfc300_probe_spi_size(struct hisfc_host *host,
		struct mtd_info *mtd)
{
	int ix = 1;
	struct hisfc_spi *spi = host->spi;

	int total     = spi->chipsize;
	int erasesize = spi->erasesize;

	for (++spi; ix < host->num_chip; ix++, spi++) {
		erasesize = mcm(erasesize, spi->erasesize);
		total += spi->chipsize;
	}

	mtd->size = total;
	mtd->erasesize = erasesize;
}
/*****************************************************************************/
#ifdef CONFIG_PM

static int hisfc200_suspend(struct platform_device *pltdev, pm_message_t state)
{
	int ix;
	struct hisfc_host *host = platform_get_drvdata(pltdev);

	for (ix = 0; ix < host->num_chip; ix++)
		hisfc200_wait_ready(host, &host->spi[ix]);

	hisfc200_config(host, 0, 0, 0, 0, 0);

	return 0;
}
/*****************************************************************************/

static int hisfc200_resume(struct platform_device *pltdev)
{
	struct hisfc_host *host = platform_get_drvdata(pltdev);

	hisfc200_config(host, 0, 0, 1, 0, 0);

	return 0;
}
/*****************************************************************************/

static int hisfc300_driver_suspend(struct platform_device *pltdev,
		pm_message_t state)
{
	struct hisfc_host *host = platform_get_drvdata(pltdev);
	if (host->suspend)
		return (host->suspend)(pltdev, state);
	return 0;
}
/*****************************************************************************/

static int hisfc300_driver_resume(struct platform_device *pltdev)
{
	struct hisfc_host *host = platform_get_drvdata(pltdev);
	if (host->resume)
		return (host->resume)(pltdev);
	return 0;
}
#endif /* CONFIG_PM */
/*****************************************************************************/

static int __devinit hisfc300_driver_probe(struct platform_device * pltdev)
{
	int result = -EIO;
	unsigned long regval;
	struct hisfc_host *host;
	struct mtd_info   *mtd;
	int nr_parts = 0;
	struct mtd_partition *parts = NULL;

	host = kmalloc(sizeof(struct hisfc_host),
			GFP_KERNEL);
	if (!host)
		return -ENOMEM;
	memset(host, 0, sizeof(struct hisfc_host));

	platform_set_drvdata(pltdev, host);

	host->regbase = ioremap_nocache(pltdev->resource[0].start,
		pltdev->resource[0].end - pltdev->resource[0].start + 1);
	if (!host->regbase) {
		printk(KERN_ERR "spi reg ioremap failed.\n");
		goto fail;
	}

	host->iobase = ioremap_nocache(pltdev->resource[1].start,
		pltdev->resource[1].end - pltdev->resource[1].start + 1);
	if (!host->iobase) {
		printk(KERN_ERR "spi buffer ioremap failed.\n");
		goto fail;
	}

	host->cfgreg = ioremap_nocache(pltdev->resource[2].start,
		pltdev->resource[2].end - pltdev->resource[2].start + 1);
	if (!host->cfgreg) {
		printk(KERN_ERR "spi cfgreg ioremap failed.\n");
		goto fail;
	}

	mutex_init(&host->lock);

	mtd = host->mtd;
	mtd->name = (char *)pltdev->name;
	mtd->type = MTD_NORFLASH;
	mtd->writesize = 1;
	mtd->flags = MTD_CAP_NORFLASH;
	mtd->owner = THIS_MODULE;

	hisfc200_config(host, 0, 0, 1, 0, 0);
#if 0
	regval = hisfc_read(host, HISFC300_VERSION);
#else
	regval = 0x00;
#endif
	switch (regval) {
	case 0x00:
		if (hisfc200_probe(host)) {
			result = -ENODEV;
			goto fail;
		}
#ifdef CONFIG_PM
		host->suspend = hisfc200_suspend;
		host->resume  = hisfc200_resume;
#endif
		break;
	case 0x300:

		break;
	default:
		printk(KERN_ERR "can't find spi controller version.\n");
		result = -ENODEV;
		goto fail;
	}

	hisfc300_probe_spi_size(host, mtd);

#ifdef CONFIG_MTD_CMDLINE_PARTS
	if (mtd_has_cmdlinepart()) {
		static char const *part_probes[] = {
			"cmdlinepart", NULL, };
		nr_parts = parse_mtd_partitions(host->mtd,
				part_probes, &parts, 0);
	}
#endif
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

	return mtd_device_register(host->mtd, parts, nr_parts) == 1 ?
		-ENODEV : 0;

fail:
	if (host->regbase)
		iounmap(host->regbase);
	if (host->iobase)
		iounmap(host->iobase);
	if (host->cfgreg)
		iounmap(host->cfgreg);

	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return result;
}
/*****************************************************************************/

static int __devexit hisfc300_driver_remove(struct platform_device * pltdev)
{
	struct hisfc_host *host = platform_get_drvdata(pltdev);

	mtd_device_unregister(host->mtd);

	if (host->regbase)
		iounmap(host->regbase);
	if (host->iobase)
		iounmap(host->iobase);
	if (host->cfgreg)
		iounmap(host->cfgreg);

	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return 0;
}
/*****************************************************************************/

static void hisfc300_pltdev_release(struct device *dev)
{
}
/*****************************************************************************/

static struct platform_driver hisfc300_driver_pltdrv = {
	.probe      = hisfc300_driver_probe,
	.remove     = hisfc300_driver_remove,
#ifdef CONFIG_PM
	.suspend    = hisfc300_driver_suspend,
	.resume     = hisfc300_driver_resume,
#endif /* CONFIG_PM */
	.driver.name  = "hi_sfc",
	.driver.owner = THIS_MODULE,
	.driver.bus   = &platform_bus_type,
};
/*****************************************************************************/

static struct resource hisfc300_device_resources[] = {
	[0] = {
		.start = CONFIG_HISFC300_REG_BASE_ADDRESS,
		.end   = CONFIG_HISFC300_REG_BASE_ADDRESS
			+ HISFC300_REG_BASE_LEN - 1,
		.flags = IORESOURCE_MEM,
	},

	[1] = {
		.start = CONFIG_HISFC300_BUFFER_BASE_ADDRESS,
		.end   = CONFIG_HISFC300_BUFFER_BASE_ADDRESS
			+ HISFC300_BUFFER_BASE_LEN - 1,
		.flags = IORESOURCE_MEM,

	},
	[2] = {
		.start = CONFIG_HISFC300_PERIPHERY_REGBASE,
		.end   = CONFIG_HISFC300_PERIPHERY_REGBASE
			+ HISFC300_PERIPHERY_REG_LEN - 1,
		.flags = IORESOURCE_MEM,
	},
};
/*****************************************************************************/

static struct platform_device hisfc300_device_pltdev = {
	.name           = "hi_sfc",
	.id             = -1,

	.dev.release    = hisfc300_pltdev_release,

	.num_resources  = ARRAY_SIZE(hisfc300_device_resources),
	.resource       = hisfc300_device_resources,
};
/*****************************************************************************/

static int __init hisfc300_module_init(void)
{
	int result = 0;

	printk(KERN_INFO
		"Hisilicon Spi Flash Controller V300 Device Driver, "
		"Version 1.10\n");

	result = platform_driver_register(&hisfc300_driver_pltdrv);
	if (result < 0)
		return result;

	result = platform_device_register(&hisfc300_device_pltdev);
	if (result < 0) {
		platform_driver_unregister(&hisfc300_driver_pltdrv);
		return result;
	}

	return result;
}
/*****************************************************************************/

static void __exit hisfc300_module_exit(void)
{
	platform_device_unregister(&hisfc300_device_pltdev);
	platform_driver_unregister(&hisfc300_driver_pltdrv);
}
/*****************************************************************************/

module_init(hisfc300_module_init);
module_exit(hisfc300_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cai ZhiYong");
MODULE_DESCRIPTION("Hisilicon Spi Flash Controller V300 Device Driver, "
		"Version 1.10");
