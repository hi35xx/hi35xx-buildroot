/******************************************************************************
*    Copyright (c) 2009-2011 by Hisi.
*    All rights reserved.
* ***
*
******************************************************************************/

#ifndef NAND_LOGIFH
#define NAND_LOGIFH
/******************************************************************************/

#include <nand.h>

/*****************************************************************************/

typedef struct nand_logic_t
{
	unsigned long long address;
	unsigned long long length;
	unsigned long long erasesize; /* fixed for 64bit  */
	nand_info_t *nand;

} nand_logic_t;

/*****************************************************************************/

nand_logic_t *nand_logic_open(unsigned long long address, unsigned long long length);

void nand_logic_close(nand_logic_t *nand_logic);

int nand_logic_erase
(
 nand_logic_t *nand_logic,
 unsigned long long offset,   /* should be alignment with nand block size */
 unsigned long long length    /* should be alignment with nand block size */
 );

int nand_logic_write
(
 nand_logic_t *nand_logic,
 unsigned long long offset,  /* should be alignment with nand page size */
 unsigned int length,        /* should be alignment with nand page size */
 unsigned char *buf,
 int withoob
 );

int nand_logic_read
(
 nand_logic_t *nand_logic,
 unsigned long long offset,  /* should be alignment with nand page size */
 unsigned int length,        /* should be alignment with nand page size */
 unsigned char *buf,
 int withoob
 );

/******************************************************************************/
#endif /* NAND_LOGIFH */

