/******************************************************************************
 *    Copyright (c) 2009-2012.
 *    All rights reserved.
 * ***
 *
 ******************************************************************************/

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
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include "../spi_ids.h"
#include "hisfc300new.h"

#define mtd_has_partitions()             (1)
static struct partition_info ptn_info;
static struct partition_info ptn300_info;
/*****************************************************************************/
/*
 *  Defines for new spi interfaces
 *
 *****************************************************************************/
/* Don't change the follow config */
#define HISFC300_SUPPORT_READ (SPI_IF_READ_STD \
		| SPI_IF_READ_FAST \
		| SPI_IF_READ_DUAL \
		| SPI_IF_READ_DUAL_ADDR \
		| SPI_IF_READ_QUAD \
		| SPI_IF_READ_QUAD_ADDR)

#define HISFC300_SUPPORT_WRITE (SPI_IF_WRITE_STD \
		| SPI_IF_WRITE_DUAL \
		| SPI_IF_WRITE_DUAL_ADDR \
		| SPI_IF_WRITE_QUAD \
		| SPI_IF_WRITE_QUAD_ADDR)

#define HISFC300_SUPPORT_MAX_DUMMY        (7)

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
static int __init parse_spi_partitions(const struct tag *tag)
{
	int i;

	if (tag->hdr.size <= 2) {
		printk(KERN_INFO "%s(%d): tag->hdr.size <= 2\n",
				__func__, __LINE__);
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
#ifdef CONFIG_HISFC300NEW_SHOW_CYCLE_TIMING
static char *hisfc300_get_ifcycle_str(int ifcycle)
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



/*****************************************************************************/
char *hisfc300_get_clock_str(unsigned int clk_reg)
{
	static char buffer[40];

	/* calculate reference PERI_CLKDIV1[31:28] */
	clk_reg = 216 / ((clk_reg >> 28) & 0xF);
	sprintf(buffer, "%dM", clk_reg);

	return buffer;
}
#endif

/*****************************************************************************/
static void hisfc300_map_iftype_and_clock(struct hisfc_spi *spi)
{
	int ix;
	const int iftype_read[] = {
		SPI_IF_READ_STD,       HISFC300_IFCYCLE_STD,
		SPI_IF_READ_FAST,      HISFC300_IFCYCLE_STD,
		SPI_IF_READ_DUAL,      HISFC300_IFCYCLE_DUAL,
		SPI_IF_READ_DUAL_ADDR, HISFC300_IFCYCLE_DUAL_ADDR,
		SPI_IF_READ_QUAD,      HISFC300_IFCYCLE_QUAD,
		SPI_IF_READ_QUAD_ADDR, HISFC300_IFCYCLE_QUAD_ADDR,
		0, 0,
	};
	const int iftype_write[] = {
		SPI_IF_WRITE_STD,       HISFC300_IFCYCLE_STD,
		SPI_IF_WRITE_DUAL,      HISFC300_IFCYCLE_DUAL,
		SPI_IF_WRITE_DUAL_ADDR, HISFC300_IFCYCLE_DUAL_ADDR,
		SPI_IF_WRITE_QUAD,      HISFC300_IFCYCLE_QUAD,
		SPI_IF_WRITE_QUAD_ADDR, HISFC300_IFCYCLE_QUAD_ADDR,
		0, 0,
	};

	for (ix = 0; iftype_write[ix]; ix += 2) {
		if (spi->write->iftype == iftype_write[ix]) {
			spi->write->iftype = iftype_write[ix + 1];
			break;
		}
	}
	hisfc300_get_best_clock(&spi->write->clock);

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}
	hisfc300_get_best_clock(&spi->read->clock);

	hisfc300_get_best_clock(&spi->erase->clock);
	spi->erase->iftype = HISFC300_IFCYCLE_STD;
}
/*****************************************************************************/
#if 0
static char *hisfc300_reg_read_buf(struct hisfc300_host *host,
		struct hisfc_spi *spi, unsigned int spi_start_addr,
		unsigned int size, unsigned char *buffer)
{
	int index = 0;
	char reg_buffer[HISFC300_REG_BUF_SIZE];

	if (size > HISFC300_REG_BUF_SIZE)
		DBG_BUG("reg read out of reg range.\n");

	hisfc300_write(host, HISFC300_CMD_INS, spi->read->cmd);
	hisfc300_write(host, HISFC300_CMD_ADDR,
			(spi_start_addr & HISFC300_CMD_ADDR_MASK));
	hisfc300_write(host, HISFC300_CMD_CONFIG,
			HISFC300_CMD_CONFIG_MEM_IF_TYPE(spi->read->iftype)
			| HISFC300_CMD_CONFIG_DATA_CNT(size)
			| HISFC300_CMD_CONFIG_RW_READ
			| HISFC300_CMD_CONFIG_DATA_EN
			| HISFC300_CMD_CONFIG_DUMMY_CNT(spi->read->dummy)
			| HISFC300_CMD_CONFIG_ADDR_EN
			| HISFC300_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	while (index < size) {
		*(unsigned int *)(reg_buffer + index) = hisfc300_read(host,
				HISFC300_CMD_DATABUF0 + index);
		index    += 4;
	}

	memcpy(buffer, reg_buffer, size);

	return buffer;
}
/*****************************************************************************/
static int hisfc300_reg_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc300_host *host = MTD300_TO_HOST(mtd);
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
		}

			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(host, spi->read, TRUE);

		num = ((from + len) >= spi->chipsize)
			? (spi->chipsize - from) : len;

		while (num >= HISFC300_REG_BUF_SIZE) {
			hisfc300_reg_read_buf(host, spi,
					from, HISFC300_REG_BUF_SIZE, ptr);
			ptr  += HISFC300_REG_BUF_SIZE;
			from += HISFC300_REG_BUF_SIZE;
			len  -= HISFC300_REG_BUF_SIZE;
			num  -= HISFC300_REG_BUF_SIZE;
		}

		if (num) {
			hisfc300_reg_read_buf(host, spi,
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
#endif
/*****************************************************************************/
static unsigned char *hisfc300_read_ids(struct hisfc300_host *host,
		int chipselect, unsigned char *buffer)
{
	int regindex = 0;
	int numread = 8;
	unsigned int *ptr = (unsigned int *)buffer;

	if (numread > HISFC300_REG_BUF_SIZE)
		numread = HISFC300_REG_BUF_SIZE;

	hisfc300_write(host, HISFC300_CMD_INS, SPI_CMD_RDID);
	hisfc300_write(host, HISFC300_CMD_CONFIG,
			HISFC300_CMD_CONFIG_SEL_CS(chipselect)
			| HISFC300_CMD_CONFIG_RW_READ
			| HISFC300_CMD_CONFIG_DATA_EN
			| HISFC300_CMD_CONFIG_DATA_CNT(numread)
			| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	while (numread) {
		*ptr = hisfc300_read(host,
				HISFC300_CMD_DATABUF0 + regindex);
		ptr      += 1;
		regindex += 4;
		numread  -= 4;
	}

	return buffer;
}
/*****************************************************************************/
static int hisfc300_reg_erase_one_block(struct hisfc300_host *host,
	struct hisfc_spi *spi, unsigned int offset)
{
	if (spi->driver->wait_ready(spi))
		return 1;
	spi->driver->write_enable(spi);
	host->set_system_clock(host, spi->erase, TRUE);

	hisfc300_write(host, HISFC300_CMD_INS, spi->erase->cmd);

	hisfc300_write(host, HISFC300_CMD_ADDR,
			(offset & HISFC300_CMD_ADDR_MASK));

	hisfc300_write(host, HISFC300_CMD_CONFIG,
		HISFC300_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC300_CMD_CONFIG_MEM_IF_TYPE(spi->erase->iftype)
		| HISFC300_CMD_CONFIG_DUMMY_CNT(spi->erase->dummy)
		| HISFC300_CMD_CONFIG_ADDR_EN
		| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	return 0;
}
/*****************************************************************************/
static int hisfc300_reg_write_buf(struct hisfc300_host *host,
		struct hisfc_spi *spi, unsigned int spi_start_addr,
		unsigned int size, unsigned char *buffer)
{
	int index = 0;
	char reg_buffer[HISFC300_REG_BUF_SIZE];

	if (size > HISFC300_REG_BUF_SIZE)
		DBG_BUG("reg read out of reg range.\n");

	if (spi->driver->wait_ready(spi))
		return 1;

	memcpy(reg_buffer, buffer, size);

	while (index < size) {
		hisfc300_write(host, HISFC300_CMD_DATABUF0 + index,
				*(unsigned int *)(reg_buffer + index));
		index    += 4;
	}

	spi->driver->write_enable(spi);

	hisfc300_write(host, HISFC300_CMD_INS, spi->write->cmd);
	hisfc300_write(host, HISFC300_CMD_ADDR,
			(spi_start_addr & HISFC300_CMD_ADDR_MASK));
	hisfc300_write(host, HISFC300_CMD_CONFIG,
			HISFC300_CMD_CONFIG_MEM_IF_TYPE(spi->write->iftype)
			| HISFC300_CMD_CONFIG_DATA_CNT(size)
			| HISFC300_CMD_CONFIG_DATA_EN
			| HISFC300_CMD_CONFIG_DUMMY_CNT(spi->write->dummy)
			| HISFC300_CMD_CONFIG_ADDR_EN
			| HISFC300_CMD_CONFIG_SEL_CS(spi->chipselect)
			| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	return 0;
}
/*****************************************************************************/
static int hisfc300_reg_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	int num, onechipsize;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hisfc300_host *host = MTD300_TO_HOST(mtd);
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

	host->set_system_clock(host, spi->write, TRUE);

	if (to & HISFC300_REG_BUF_MASK) {
		num = HISFC300_REG_BUF_SIZE - (to & HISFC300_REG_BUF_MASK);
		if (num > (int)len)
			num = (int)len;

		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");
		}

		if (spi->driver->wait_ready(spi))
			goto fail;

		if (hisfc300_reg_write_buf(host, spi, to, num, ptr))
			goto fail;
		to  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0)	{
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");
		}

			if (spi->driver->wait_ready(spi))
				goto fail;

		onechipsize = ((to+len >= spi->chipsize)
				? spi->chipsize - to : len);

		while (onechipsize > 0) {
			num = ((len >= HISFC300_REG_BUF_SIZE) ?
					HISFC300_REG_BUF_SIZE : len);

			if (hisfc300_reg_write_buf(host, spi, to, num, ptr))
				goto fail;
			to  += num;
			ptr += num;
			len -= num;
			onechipsize -= num;
		}
	}

	*retlen = (size_t)(ptr - buf);
	result = 0;
fail:
	mutex_unlock(&host->lock);
	return result;
}
/*****************************************************************************/
static int hisfc300_reg_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	int cmdindex;
	struct hisfc300_host *host = MTD300_TO_HOST(mtd);
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

		for (cmdindex = 0; cmdindex < MAX_SPI_OP; cmdindex++) {
			if ((spi->chipsize - offset)
					>= spi->erase[cmdindex].size
					&& length >= spi->erase[cmdindex].size)
				break;
		}

		if (hisfc300_reg_erase_one_block(host, spi, offset)) {
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
/*****************************************************************************/
static int hisfc300_bus_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = buf;
	struct hisfc300_host *host = MTD300_TO_HOST(mtd);
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
/*****************************************************************************/
static int hisfc300_map_chipsize(unsigned long long chipsize)
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
static int hisfc_spi_probe(struct hisfc300_host *host)
{

	unsigned int regval = 0;
	unsigned int total = 0;
	unsigned char ids[8];
	struct spi_info *spiinfo;
	struct hisfc_spi *spi = host->spi;
	int chipselect = (CONFIG_HISFC300NEW_CHIP_NUM - 1);

	host->num_chip = 0;

	for (; chipselect >= 0; chipselect--) {


		hisfc300_read_ids(host, chipselect, ids);

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

			/*map out the result if flash support QUAD_READ*/
			spi_search_rw(spiinfo, spi->read,
					HISFC300_SUPPORT_READ,
					HISFC300_SUPPORT_MAX_DUMMY, READ);
			hisfc300_map_iftype_and_clock(spi);

			spi->driver->qe_enable(spi);

			/*reserve search_read to keep same as the old codes*/
			spi_search_rw(spiinfo, spi->read,
					HISFC300_SUPPORT_READ,
					HISFC300_SUPPORT_MAX_DUMMY, READ);

			spi_search_rw(spiinfo, spi->write,
					HISFC300_SUPPORT_WRITE,
					HISFC300_SUPPORT_MAX_DUMMY, WRITE);

			spi_get_erase(spiinfo, spi->erase);
			hisfc300_map_iftype_and_clock(spi);

			regval = hisfc300_read(host, HISFC300_BUS_FLASH_SIZE);
			regval &= ~(HISFC300_BUS_FLASH_SIZE_CS0_MASK
					<< (chipselect << 3));
			regval |= (hisfc300_map_chipsize(spi->chipsize)
					<< (chipselect << 3));
			hisfc300_write(host, HISFC300_BUS_FLASH_SIZE, regval);

			hisfc300_write(host, (HISFC300_BUS_BASE_ADDR_CS0
					+ (chipselect << 2)),
					(CONFIG_HISFC300NEW_BUFFER_BASE_ADDRESS
					+ total));

			spi->iobase = (char *)host->iobase + total;

			spi->driver->entry_4addr(spi, TRUE);

			printk(KERN_INFO "Spi(cs%d): ", spi->chipselect);
			printk(KERN_INFO "Block:%sB ",
					int_to_size(spi->erasesize));
			printk(KERN_INFO "Chip:%sB ",
					int_to_size(spi->chipsize));
			printk(KERN_INFO "Name:\"%s\"\n", spi->name);

#ifdef CONFIG_HISFC300NEW_SHOW_CYCLE_TIMING

			printk(KERN_INFO "Spi(cs%d): ", spi->chipselect);
			if (spi->addrcycle == 4)
				printk(KERN_INFO "4 addrcycle ");
			printk(KERN_INFO "read:%s,%02X,%s ",
				hisfc300_get_ifcycle_str(spi->read->iftype),
				spi->read->cmd,
				hisfc300_get_clock_str(spi->read->clock));
			printk(KERN_INFO "write:%s,%02X,%s ",
				hisfc300_get_ifcycle_str(spi->write->iftype),
				spi->write->cmd,
				hisfc300_get_clock_str(spi->write->clock));
			printk(KERN_INFO "erase:%s,%02X,%s\n",
				hisfc300_get_ifcycle_str(spi->erase[0].iftype),
				spi->erase[0].cmd,
				hisfc300_get_clock_str(spi->erase[0].clock));

#endif /* CONFIG_HISFC300NEW_SHOW_CYCLE_TIMING */
			host->num_chip++;
			total += spi->chipsize;
			spi++;

		} else
			printk(KERN_ERR"Spi(cs%d): find unrecognized spi flash.\n",
					chipselect);
	}

	return host->num_chip;
}
/*****************************************************************************/

static int __init parse300_spi_partitions(const struct tag *tag)
{
	int i;

	if (tag->hdr.size <= 2) {
		printk(KERN_DEBUG "%s(%d): tag->hdr.size <= 2\n",
				__func__, __LINE__);
		return 0;
	}

	ptn300_info.parts_num = (tag->hdr.size - 2) /
		(sizeof(struct partition_entry)/sizeof(int));
	memcpy(ptn300_info.entry, &tag->u,
			ptn300_info.parts_num * sizeof(struct partition_entry));

	for (i = 0; i < ptn300_info.parts_num; i++) {
		ptn300_info.parts[i].name   = ptn300_info.entry[i].name;
		ptn300_info.parts[i].size   = (ptn300_info.entry[i].length);
		ptn300_info.parts[i].offset = (ptn300_info.entry[i].start);
		ptn300_info.parts[i].mask_flags = 0;
	}

	return 0;
}
__tagtable(0x48695370, parse300_spi_partitions);


/*****************************************************************************/
static void hisfc300_probe_spi_size(struct hisfc300_host *host,
		struct mtd_info *mtd)
{
	int ix = 1;
	struct hisfc_spi *spi = host->spi;

	int total     = spi->chipsize;
	int erasesize = spi->erasesize;

	for (++spi; ix < host->num_chip; ix++, spi++) {
		/*	erasesize = mcm(erasesize, spi->erasesize); */
		total += spi->chipsize;
	}

	mtd->size = total;
	mtd->erasesize = erasesize;
	printk(KERN_INFO "spi size: 0x%d\n", (int)mtd->size);
	printk(KERN_INFO "chip num: %x\n", host->num_chip);
}
/*****************************************************************************/
static int hisfc300_probe(struct hisfc300_host *host)
{
	struct mtd_info *mtd = host->mtd;

	host->set_system_clock(host, NULL, TRUE);
	hisfc300_write(host, HISFC300_TIMING,
			HISFC300_TIMING_TCSS(0x6)
			| HISFC300_TIMING_TCSH(0x6)
			| HISFC300_TIMING_TSHSL(0xf));

	if (!hisfc_spi_probe(host))
		return -1;

	mtd->erase = hisfc300_reg_erase;
	mtd->write = hisfc300_reg_write;
	mtd->read  = hisfc300_bus_read;

	return 0;
}
/*****************************************************************************/
#ifdef CONFIG_PM
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
static void hisfc300_set_host_addr_mode(struct hisfc300_host *host, int enable)
{
	unsigned int regval;
	regval = hisfc300_read(host, HISFC300_GLOBAL_CONFIG);
	if (enable)
		regval |= HISFC300_GLOBAL_CONFIG_ADDR_MODE_4B;
	else
		regval &= ~HISFC300_GLOBAL_CONFIG_ADDR_MODE_4B;

	hisfc300_write(host, HISFC300_GLOBAL_CONFIG, regval);
}



static int __devinit hisfc300_driver_probe(struct platform_device * plat_dev)
{
	int result = -EIO;
	struct hisfc300_host *host;
	struct mtd_info   *mtd;
	int nr_parts = 0;
	struct mtd_partition *parts = NULL;
	int ret;

	host = kmalloc(sizeof(struct hisfc300_host), GFP_KERNEL);
	if (!host)
		return -ENOMEM;
	memset(host, 0, sizeof(struct hisfc300_host));

	dev_set_drvdata(&plat_dev->dev, host);

	host->sysreg = ioremap_nocache(CONFIG_HISFC300NEW_PERIPHERY_REGBASE,
			HISFC300NEW_PERIPHERY_REG_LEN);
	if (!host->sysreg) {
		printk(KERN_ERR "spi system reg ioremap failed.\n");
		goto fail;
	}

	host->regbase = ioremap_nocache(CONFIG_HISFC300NEW_REG_BASE_ADDRESS,
			HISFC300NEW_REG_BASE_LEN);
	host->set_system_clock = hisfc300_set_system_clock;
	host->set_host_addr_mode = hisfc300_set_host_addr_mode;
	if (!host->regbase) {
		printk(KERN_ERR "spi base reg ioremap failed.\n");
		goto fail;
	}

	host->iobase = ioremap_nocache(CONFIG_HISFC300NEW_BUFFER_BASE_ADDRESS,
			HISFC300NEW_BUFFER_BASE_LEN);
	if (!host->iobase) {
		printk(KERN_ERR "spi buffer ioremap failed.\n");
		goto fail;
	}

	mutex_init(&host->lock);
	mtd = host->mtd;
	mtd->name = (char *)plat_dev->name;
	mtd->type = MTD_NORFLASH;
	mtd->writesize = 1;
	mtd->flags = MTD_CAP_NORFLASH;
	mtd->owner = THIS_MODULE;

	if (hisfc300_probe(host)) {
		result = -ENODEV;
		goto fail;
	}

	hisfc300_probe_spi_size(host, mtd);

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
	if (host->sysreg)
		iounmap(host->sysreg);
	kfree(host);
	dev_set_drvdata(&plat_dev->dev, NULL);
	return result;
}
/*****************************************************************************/

static int __devexit hisfc300_driver_remove(struct platform_device * pltdev)
{
	struct hisfc300_host *host = platform_get_drvdata(pltdev);


	mtd_device_unregister(host->mtd);

	if (host->regbase)
		iounmap(host->regbase);
	if (host->iobase)
		iounmap(host->iobase);
	if (host->sysreg)
		iounmap(host->sysreg);

	kfree(host);
	platform_set_drvdata(pltdev, NULL);

	return 0;
}
/*****************************************************************************/

static void hisfc300_driver_shutdown(struct platform_device *pltdev)
{
	int ix;
	struct hisfc300_host *host = platform_get_drvdata(pltdev);
	struct hisfc_spi *spi = host->spi;

	for (ix = 0; ix < host->num_chip; ix++, spi++) {
		if (spi->addrcycle == 4)
			spi->driver->wait_ready(spi);
			spi->driver->entry_4addr(spi, 0);
	}
}
/*****************************************************************************/
static void hisfc300_pltdev_release(struct device *dev)
{
}
/*****************************************************************************/

static struct platform_driver hisfc300_driver_pltdrv = {
	.probe      = hisfc300_driver_probe,
	.remove     = hisfc300_driver_remove,
	.shutdown   = hisfc300_driver_shutdown,
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
		.start = CONFIG_HISFC300NEW_REG_BASE_ADDRESS,
		.end   = CONFIG_HISFC300NEW_REG_BASE_ADDRESS
			+ HISFC300NEW_REG_BASE_LEN - 1,
		.flags = IORESOURCE_MEM,
	},

	[1] = {
		.start = CONFIG_HISFC300NEW_BUFFER_BASE_ADDRESS,
		.end   = CONFIG_HISFC300NEW_BUFFER_BASE_ADDRESS
			+ HISFC300NEW_BUFFER_BASE_LEN - 1,
		.flags = IORESOURCE_MEM,

	},
	[2] = {
		.start = CONFIG_HISFC300NEW_PERIPHERY_REGBASE,
		.end   = CONFIG_HISFC300NEW_PERIPHERY_REGBASE
			+ HISFC300NEW_PERIPHERY_REG_LEN - 1,
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
static int hisfc300_version_check(void)
{
	void *regbase;
	unsigned long regval;

	regbase = ioremap_nocache(CONFIG_HISFC300NEW_REG_BASE_ADDRESS,
			HISFC300NEW_REG_BASE_LEN);
	if (!regbase) {
		printk(KERN_ERR "spi base reg ioremap failed.\n");
		return -EIO;
	}
	regval = readl((unsigned)((char *)regbase + HISFC300_VERSION));
	iounmap(regbase);
	return (regval == 0x300);
}
/*****************************************************************************/
static int __init hisfc300_module_init(void)
{
	int result = 0;

	printk(KERN_INFO
			"Hisilicon Spi Flash Controller V300 New Device Driver, "
			"Version 1.10\n");

	printk(KERN_INFO "Check Spi Flash Controller V300 New. ");
	if (!hisfc300_version_check()) {
		printk(KERN_INFO "\n");
		return -ENODEV;
	}
	printk(KERN_INFO "Found\n");

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
MODULE_DESCRIPTION("Hisilicon Spi Flash Controller V300 New Device Driver, "
		"Version 1.10");

