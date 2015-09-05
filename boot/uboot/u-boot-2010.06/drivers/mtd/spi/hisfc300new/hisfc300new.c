/******************************************************************************
*    Copyright (c) 2011-2012.
*    All rights reserved.
* ***
*
******************************************************************************/
#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm-generic/errno.h>

#include "../spi_ids.h"
#include "hisfc300new.h"

#define DEBUG 0

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
static struct hisfc_host  hisfc300_host;
static struct mtd_info_ex spi_info_ex = {.type = 0, };


#ifdef CONFIG_HISFC300_SHOW_CYCLE_TIMING
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
#endif

/*****************************************************************************/
static void hisfc300_set_host_addr_mode(struct hisfc_host *host, int enable)
{
	unsigned int regval;
	regval = hisfc_read(host, HISFC300_GLOBAL_CONFIG);
	if (enable)
		regval |= HISFC300_GLOBAL_CONFIG_ADDR_MODE_4B;
	else
		regval &= ~HISFC300_GLOBAL_CONFIG_ADDR_MODE_4B;

	hisfc_write(host, HISFC300_GLOBAL_CONFIG, regval);
}


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
	for (ix = 0; (ix < MAX_SPI_OP) && spi->erase[ix].iftype; ix++) {
		hisfc300_get_best_clock(&(spi->erase[ix].clock));
		spi->erase[ix].iftype = HISFC300_IFCYCLE_STD;
	}
}

/*****************************************************************************/
static unsigned char *hisfc300_read_ids(struct hisfc_host *host,
	int chipselect, unsigned char *buffer)
{
	int regindex = 0;
	int numread = 8;
	unsigned int *ptr = (unsigned int *)buffer;

	if (numread > HISFC300_REG_BUF_SIZE)
		numread = HISFC300_REG_BUF_SIZE;

	hisfc_write(host, HISFC300_CMD_INS, SPI_CMD_RDID);
	hisfc_write(host, HISFC300_CMD_CONFIG,
		HISFC300_CMD_CONFIG_SEL_CS(chipselect)
		| HISFC300_CMD_CONFIG_RW_READ
		| HISFC300_CMD_CONFIG_DATA_EN
		| HISFC300_CMD_CONFIG_DATA_CNT(numread)
		| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	while (numread) {
		*ptr = hisfc_read(host,
			HISFC300_CMD_DATABUF0 + regindex);
		ptr      += 1;
		regindex += 4;
		numread  -= 4;
	}

	return buffer;
}

/*****************************************************************************/
static int hisfc300_reg_erase_one_block(struct hisfc_host *host,
	struct hisfc_spi *spi, unsigned long offset, int cmdindex)
{
	if (spi->driver->wait_ready(spi))
		return 1;
	spi->driver->write_enable(spi);

	host->set_system_clock(host, &spi->erase[cmdindex], TRUE);

	hisfc_write(host, HISFC300_CMD_INS,
		spi->erase[cmdindex].cmd);

	hisfc_write(host, HISFC300_CMD_ADDR,
		(offset & HISFC300_CMD_ADDR_MASK));

	hisfc_write(host, HISFC300_CMD_CONFIG,
		HISFC300_CMD_CONFIG_SEL_CS(spi->chipselect)
		| HISFC300_CMD_CONFIG_MEM_IF_TYPE(spi->erase[cmdindex].iftype)
		| HISFC300_CMD_CONFIG_DUMMY_CNT(spi->erase[cmdindex].dummy)
		| HISFC300_CMD_CONFIG_ADDR_EN
		| HISFC300_CMD_CONFIG_START);

	HISFC300_CMD_WAIT_CPU_FINISH(host);

	return 0;
}

/*****************************************************************************/
static int hisfc300_reg_write_buf(struct hisfc_host *host,
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
		hisfc_write(host, HISFC300_CMD_DATABUF0 + index,
			*(unsigned int *)(reg_buffer + index));
		index    += 4;
	}

	spi->driver->write_enable(spi);

	hisfc_write(host, HISFC300_CMD_INS, spi->write->cmd);
	hisfc_write(host, HISFC300_CMD_ADDR,
		(spi_start_addr & HISFC300_CMD_ADDR_MASK));
	hisfc_write(host, HISFC300_CMD_CONFIG,
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
static int hisfc300_reg_write(struct spi_flash *spiflash, u32 to, size_t len,
	const void *buf)
{
	int num, onechipsize;
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

	if (to & HISFC300_REG_BUF_MASK) {
		num = HISFC300_REG_BUF_SIZE - (to & HISFC300_REG_BUF_MASK);
		if (num > (int)len)
			num = (int)len;

		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

		}

		if (hisfc300_reg_write_buf(host, spi, to, num, ptr))
			goto fail;
		to  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0) {
		while (to >= spi->chipsize)  {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

		}

		onechipsize = ((to+len >= spi->chipsize) ?
		spi->chipsize - to : len);

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

	result = 0;
fail:
	return result;
}
/*****************************************************************************/
static int hisfc300_reg_erase(struct spi_flash *spiflash, u32 offset, size_t length)
{
	int cmdindex;
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

		for (cmdindex = 0; cmdindex < MAX_SPI_OP; cmdindex++) {
			if ((spi->chipsize - offset)
				>= spi->erase[cmdindex].size
				&& length >= spi->erase[cmdindex].size)
				break;
		}

		if (hisfc300_reg_erase_one_block(host, spi,
			(unsigned long)offset, cmdindex)) {
			return -EIO;
		}

		offset += spi->erase[cmdindex].size;
		length -= spi->erase[cmdindex].size;
	}


	return 0;
}

/*****************************************************************************/
static int hisfc300_bus_read(struct spi_flash *spiflash, u32 from, size_t len,
	void *buf)
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
static int hisfc300_spi_probe(struct hisfc_host *host)
{
	unsigned int regval = 0;
	unsigned int total = 0;
	unsigned char ids[8];
	struct spi_info *spiinfo;
	struct hisfc_spi *spi = host->spi;
	int chipselect = (CONFIG_HISFC300_CHIP_NUM - 1);

	host->num_chip = 0;

	for (; chipselect >= 0; chipselect--) {

		hisfc300_read_ids(host, chipselect, ids);

		/* can't find spi flash device. */
		if (!(ids[0] | ids[1] | ids[2])
			|| ((ids[0] & ids[1] & ids[2]) == 0xFF))
			continue;

		printf("Spi(cs%d) ID: 0x%02X 0x%02X 0x%02X"
			" 0x%02X 0x%02X 0x%02X\n",
			chipselect,
			ids[0], ids[1], ids[2], ids[3], ids[4], ids[5]);

		spiinfo = spi_serach_ids(hisfc300new_spi_info_table, ids);

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

			regval = hisfc_read(host, HISFC300_BUS_FLASH_SIZE);
			regval &= ~(HISFC300_BUS_FLASH_SIZE_CS0_MASK
				<< (chipselect << 3));
			regval |= (hisfc300_map_chipsize(spi->chipsize)
				<< (chipselect << 3));
			hisfc_write(host, HISFC300_BUS_FLASH_SIZE, regval);

			hisfc_write(host,
				(HISFC300_BUS_BASE_ADDR_CS0
					+ (chipselect << 2)),
				(CONFIG_HISFC300_BUFFER_BASE_ADDRESS + total));

			spi->iobase = (char *)host->iobase + total;

			spi->driver->entry_4addr(spi, TRUE);

			printf("Spi(cs%d): ", spi->chipselect);
			printf("Block:%sB ",
				ultohstr((unsigned long long)spi->erasesize));
			printf("Chip:%sB ",   ultohstr(spi->chipsize));
			printf("Name:\"%s\"\n", spi->name);
			if (DEBUG) {
					printf("Spi(cs%d): ", spi->chipselect);
					if (spi->addrcycle == 4)
						printf("4 addrcycle ");
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
#ifdef CONFIG_HISFC300_SHOW_CYCLE_TIMING

			printf("Spi(cs%d): ", spi->chipselect);
			if (spi->addrcycle == 4)
				printf("4 addrcycle ");
			printf("read:%s,%02X,%s ",
				hisfc300_get_ifcycle_str(spi->read->iftype),
				spi->read->cmd,
				hisfc350_get_clock_str(spi->read->clock));
			printf("write:%s,%02X,%s ",
				hisfc300_get_ifcycle_str(spi->write->iftype),
				spi->write->cmd,
				hisfc300_get_clock_str(spi->write->clock));
			printf("erase:%s,%02X,%s\n",
				hisfc300_get_ifcycle_str(spi->erase[0].iftype),
				spi->erase[0].cmd,
				hisfc300_get_clock_str(spi->erase[0].clock));

#endif /* CONFIG_HISFC300_SHOW_CYCLE_TIMING */

			if (spi_info_ex.type == 0) {
				spi_info_ex.type      = MTD_NORFLASH;
				spi_info_ex.chipsize  = spi->chipsize;
				spi_info_ex.erasesize = spi->erasesize;
				spi_info_ex.pagesize  = 1;
				spi_info_ex.addrcycle = spi->addrcycle;

				if (spiinfo->id_len > sizeof(spi_info_ex.ids)) {
					printf("BUG!id length out of range.\n");
					BUG();
				}

				spi_info_ex.id_length = spiinfo->id_len;
				memcpy(spi_info_ex.ids, spiinfo->id,
					spiinfo->id_len);

				strncpy(spi_info_ex.name, spi->name,
					sizeof(spi_info_ex.name));
				spi_info_ex.name[sizeof(spi_info_ex.name)-1]
					= '\0';
			}

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
static void hisfc300_shutdown(void)
{
	int ix;
	struct hisfc_host *host = &hisfc300_host;
	struct hisfc_spi *spi = host->spi;

	for (ix = 0; ix < host->num_chip; ix++, spi++) {
		spi->driver->wait_ready(spi);
		if (spi->addrcycle == 4)
			spi->driver->entry_4addr(spi, 0);
	}
}

/*****************************************************************************/
static int hisfc300_probe(struct hisfc_host *host)
{
	struct spi_flash *spiflash = host->spiflash;

	/* new version */
	hisfc_write(host, HISFC300_VERSION_SEL, HISFC300_VERSION_SEL_SEL(0x01));
	host->set_system_clock(host, NULL, TRUE);
	hisfc_write(host, HISFC300_TIMING,
		HISFC300_TIMING_TCSS(0x6)
		| HISFC300_TIMING_TCSH(0x6)
		| HISFC300_TIMING_TSHSL(0xf));

	if (!hisfc300_spi_probe(host))
		return -1;

	spiflash->erase = hisfc300_reg_erase;
	spiflash->write = hisfc300_reg_write;
	spiflash->read  = hisfc300_bus_read;

	return 0;
}
/*****************************************************************************/
static void hisfc300_probe_spi_size(struct hisfc_host *host, struct spi_flash *spiflash)
{
	int ix = 1;
	struct hisfc_spi *spi = host->spi;

	int total     = spi->chipsize;
	int erasesize = spi->erasesize;

	for (++spi; ix < host->num_chip; ix++, spi++) {
		/*	erasesize = mcm(erasesize, spi->erasesize); */
		total += spi->chipsize;
	}

	spiflash->size  = total;
	host->erasesize = erasesize;
}

/*****************************************************************************/

static int hisfc300_version_check(void)
{
	unsigned long regval = readl(CONFIG_HISFC300_REG_BASE_ADDRESS
		+ HISFC300_VERSION);
	return (regval == 0x300);
}
/*****************************************************************************/

static struct spi_flash *hisfc300_init(void)
{
	unsigned int regval;
	struct hisfc_host *host = &hisfc300_host;
	static struct spi_flash *spiflash;

	if (spiflash)
		return spiflash;

	memset(host, 0, sizeof(struct hisfc_host));
	memset(&spi_info_ex, 0, sizeof(struct mtd_info_ex));

	host->regbase = (void *)CONFIG_HISFC300_REG_BASE_ADDRESS;
	host->iobase  = (void *)CONFIG_HISFC300_BUFFER_BASE_ADDRESS;
	host->cfgreg  = (void *)CONFIG_HISFC300_PERIPHERY_REGBASE;

	host->set_system_clock = hisfc300_set_system_clock;
	host->set_host_addr_mode = hisfc300_set_host_addr_mode;

	spiflash = host->spiflash;
	spiflash->name = "hi_sfc";

	regval = hisfc_read(host, HISFC300_VERSION);

	switch (regval) {
	case 0x000:
			printf("Can't find v200 spi flash chip.\n");
		break;
	case 0x300:
		if (hisfc300_probe(host)) {
			printf("Can't find a valid spi flash chip.\n");
			goto probe_fail;
		}

		break;
	default:
		printf("Can't find spi controller version.\n");
		goto probe_fail;
	}

	hisfc300_probe_spi_size(host, spiflash);
	return spiflash;

probe_fail:
	spiflash = NULL;
	return NULL;
}
/*****************************************************************************/

static struct mtd_info_ex *hisfc300_getinfo(void)
{
	if (spi_info_ex.type == 0) {
		if (hisfc300_init() == NULL)
			return NULL;
	}
	return &spi_info_ex;
}
/*****************************************************************************/

int hisfc300new_spiflash_init(struct spi_flash **spiflash,
	struct mtd_info_ex **spiinfo)
{
	printf("Check spi flash controller v300. ");
	if (!hisfc300_version_check())  {
		printf("not found\n");
		return 0;
	}
	printf("found\n");

	(*spiflash) = hisfc300_init();
	(*spiinfo) = hisfc300_getinfo();

	add_shutdown(hisfc300_shutdown);
	return 1;
}
