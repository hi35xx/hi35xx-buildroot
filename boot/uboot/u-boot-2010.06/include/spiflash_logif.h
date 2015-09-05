/******************************************************************************
*    Copyright (c) 2009-2011 by Hisi.
*    All rights reserved.
* ***
*
******************************************************************************/

#ifndef SPIFLASH_LOGIFH
#define SPIFLASH_LOGIFH
/******************************************************************************/

#include <spi_flash.h>

/*****************************************************************************/

typedef struct spiflash_logic_t
{
	unsigned long long address;
	unsigned long long length;
	unsigned long long erasesize;
	struct spi_flash *spiflash;
} spiflash_logic_t;

/*****************************************************************************/

spiflash_logic_t *spiflash_logic_open(unsigned long long address, unsigned long long length);

void spiflash_logic_close(spiflash_logic_t *spiflash_logic);

int spiflash_logic_erase
(
 spiflash_logic_t *spiflash_logic,
 unsigned long long offset,   /* should be alignment with spi flash block size */
 unsigned long long length    /* should be alignment with spi flash block size */
 );

int spiflash_logic_write
(
 spiflash_logic_t *spiflash_logic,
 unsigned long long offset,
 unsigned int length,
 unsigned char *buf
 );

int spiflash_logic_read
(
 spiflash_logic_t *spiflash_logic,
 unsigned long long offset,
 unsigned int length,
 unsigned char *buf
 );

/******************************************************************************/
#endif /* SPIFLASH_LOGIFH */

