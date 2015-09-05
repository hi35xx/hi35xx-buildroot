/******************************************************************************
*    Copyright (c) 2009-2011 by Hisi.
*    All rights reserved.
* ***
*
******************************************************************************/
#include <common.h>
#include <malloc.h>
#include <linux/mtd/mtd.h>

#include <spiflash_logif.h>

/*****************************************************************************/

spiflash_logic_t *spiflash_logic_open(unsigned long long address, unsigned long long length)
{
	struct spi_flash  *spiflash;
	struct mtd_info_ex *mtd_info_ex;
	spiflash_logic_t *spiflash_logic;
	
	spiflash = spi_flash_probe(0,0,0,0);
	if (spiflash == NULL)
	{
		printf("no devices available\n");
		return NULL;
	}
	
	mtd_info_ex = get_spiflash_info();
	if (mtd_info_ex == NULL)
	{
		printf("get spi flash info failed\n");
		return NULL;
	}
	
	/* Reject open, which are not block aligned */
	if ((address & (mtd_info_ex->erasesize - 1)) || (length & (mtd_info_ex->erasesize - 1)))
	{
		printf("Attempt to open non block aligned, "
			"spi flash blocksize: 0x%08x, address: 0x%08llx, length: 0x%08llx\n",
			mtd_info_ex->erasesize, address, length);
		return NULL;
	}

	if ((address > spiflash->size)
		|| (length > spiflash->size)
		|| ((address + length) > spiflash->size))
	{
		printf("Attempt to open outside the flash area, "
			"spi flash chipsize: 0x%08x, address: 0x%08llx, length: 0x%08llx\n",
			spiflash->size, address, length);
		return NULL;
	}

	if ((spiflash_logic = malloc(sizeof(spiflash_logic_t))) == NULL)
	{
		printf("no many memory.\n");
		return NULL;
	}

	spiflash_logic->spiflash  = spiflash;
	spiflash_logic->address   = address;
	spiflash_logic->length    = length;
	spiflash_logic->erasesize = mtd_info_ex->erasesize;

	return spiflash_logic;
}
/*****************************************************************************/

void spiflash_logic_close(spiflash_logic_t *spiflash_logic)
{
	free(spiflash_logic);
}
/*****************************************************************************/

int spiflash_logic_erase
(
 spiflash_logic_t *spiflash_logic,
 unsigned long long offset,   /* should be alignment with spi flash block size */
 unsigned long long length    /* should be alignment with spi flash block size */
 )
{
	/* Reject write, which are not block aligned */
	if ((offset & (spiflash_logic->erasesize - 1)) || (length & (spiflash_logic->erasesize - 1)))
	{
		printf("Attempt to erase non block aligned, "
			"spi flash blocksize: 0x%08llx, offset: 0x%08llx, length: 0x%08llx\n",
			spiflash_logic->erasesize, offset, length);
		return -1;
	}

	if ((offset > spiflash_logic->length)
		|| (length > spiflash_logic->length)
		|| ((offset + length) > spiflash_logic->length))
	{
		printf("Attempt to erase outside the flash handle area, "
			"flash handle size: 0x%08llx, offset: 0x%08llx, "
			"length: 0x%08llx, phylength:  0x%08llx\n",
			spiflash_logic->length, offset, length, offset + length);
		return -1;
	}
	
	return spi_flash_erase(spiflash_logic->spiflash, spiflash_logic->address + offset, length);
}
/*****************************************************************************/

int spiflash_logic_write
(
 spiflash_logic_t *spiflash_logic,
 unsigned long long offset,
 unsigned int length,
 unsigned char *buf
 )
{
	if ((offset > spiflash_logic->length)
		|| (length > spiflash_logic->length)
		|| ((offset + length) > spiflash_logic->length))
	{
		printf("Attempt to write outside the flash handle area, "
			"flash handle size: 0x%08llx, offset: 0x%08llx, "
			"length: 0x%08x, phylength:  0x%08llx\n",
			spiflash_logic->length, offset, length, offset + length);
		return -1;
	}
	
	return spi_flash_write(spiflash_logic->spiflash, spiflash_logic->address + offset, length, buf);
}
/*****************************************************************************/

int spiflash_logic_read
(
 spiflash_logic_t *spiflash_logic,
 unsigned long long offset,
 unsigned int length,
 unsigned char *buf
)
{
	if ((offset > spiflash_logic->length)
		|| (length > spiflash_logic->length)
		|| ((offset + length) > spiflash_logic->length))
	{
		printf("Attempt to read outside the flash handle area, "
			"flash handle size: 0x%08llx, offset: 0x%08llx, "
			"length: 0x%08x, phylength:  0x%08llx\n",
			spiflash_logic->length, offset, length, offset + length);
		return -1;
	}
	
	return spi_flash_read(spiflash_logic->spiflash, spiflash_logic->address + offset, length, buf);
}
/*****************************************************************************/
