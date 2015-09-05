/******************************************************************************
*    Copyright (c) 2009-2011 by Hisi.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <common.h>
#include <watchdog.h>
#include <asm/errno.h>
#include <malloc.h>

#include <nand_logif.h>

/*****************************************************************************/

static unsigned long long logic_to_phylength
(
 nand_info_t *nand,
 unsigned long long phyaddress, /* should be alignment with block size */
 unsigned long long logiclength /* switch logic length, should be alignment with block size */
 )
{
	unsigned long long len_incl_bad = 0;
	unsigned long long len_excl_bad = 0;

	while (len_excl_bad < logiclength)
	{
		if (!nand_block_isbad(nand, phyaddress))
			len_excl_bad += nand->erasesize;

		len_incl_bad += nand->erasesize;
		phyaddress   += nand->erasesize;

		if (phyaddress >= nand->size)
		{
			printf("operation out of nand flash range.\n");
			break;
		}
	}
	return len_incl_bad;
}
/*****************************************************************************/

nand_logic_t *nand_logic_open(unsigned long long address, unsigned long long length)
{
	nand_info_t  *nand;
	nand_logic_t *nand_logic;

	/* the following commands operate on the current device */
	if (nand_curr_device < 0
		|| nand_curr_device >= CONFIG_SYS_MAX_NAND_DEVICE
		|| !nand_info[nand_curr_device].name)
	{
		printf("no devices available\n");
		return NULL;
	}
	nand = &nand_info[nand_curr_device];

	/* Reject open, which are not block aligned */
	if ((address & (nand->erasesize - 1)) || (length & (nand->erasesize - 1)))
	{
		printf("Attempt to open non block aligned, "
			"nand blocksize: 0x%08x, address: 0x%08llx, length: 0x%08llx\n",
			nand->erasesize, address, length);
		return NULL;
	}

	if ((address > nand->size)
		|| (length > nand->size)
		|| ((address + length) > nand->size))
	{
		printf("Attempt to open outside the flash area, "
			"nand chipsize: 0x%08llx, address: 0x%08llx, length: 0x%08llx\n",
			nand->size, address, length);
		return NULL;
	}

	if ((nand_logic = malloc(sizeof(nand_logic_t))) == NULL)
	{
		printf("no many memory.\n");
		return NULL;
	}

	nand_logic->nand      = nand;
	nand_logic->address   = address;
	nand_logic->length    = length;
	nand_logic->erasesize = nand->erasesize;

	return nand_logic;
}
/*****************************************************************************/

void nand_logic_close(nand_logic_t *nand_logic)
{
	free(nand_logic);
}
/*****************************************************************************/

int nand_logic_erase
(
 nand_logic_t *nand_logic,
 unsigned long long offset,   /* should be alignment with nand block size */
 unsigned long long length    /* should be alignment with nand block size */
 )
{
	struct erase_info erase;
	unsigned long long phylength;
	nand_info_t *nand = nand_logic->nand;

	/* Reject write, which are not page aligned */
	if ((offset & (nand->erasesize - 1)) || (length & (nand->erasesize - 1)))
	{
		printf("Attempt to erase non block aligned, "
			"nand blocksize: 0x%08x, offset: 0x%08llx, length: 0x%08llx\n",
			nand->erasesize, offset, length);
		return -1;
	}

	phylength = logic_to_phylength(nand, nand_logic->address, (offset + length));
	if ((offset > nand_logic->length)
		|| (length > nand_logic->length)
		|| ((offset + length) > nand_logic->length))
	{
		printf("Attempt to erase outside the flash handle area, "
			"flash handle size: 0x%08llx, offset: 0x%08llx, "
			"length: 0x%08llx, phylength:  0x%08llx\n",
			nand_logic->length, offset, length, phylength);
		return -1;
	}
	if (phylength > nand_logic->length)
	{
		printf("Erase physical length out of range. "
			"flash handle size: 0x%08llx, offset: 0x%08llx, "
			"length: 0x%08llx, phylength:  0x%08llx\n",
			nand_logic->length, offset, length, phylength);
	}	
	phylength = logic_to_phylength(nand, nand_logic->address, offset);

	memset(&erase, 0, sizeof(erase));
	erase.mtd  = nand;
	erase.len  = nand->erasesize;
	erase.addr = nand_logic->address + phylength;

	for (; length > 0; erase.addr += nand->erasesize)
	{
		WATCHDOG_RESET ();

		if(erase.addr >= (nand_logic->address + nand_logic->length))
			break;

		int ret = nand->block_isbad (nand, erase.addr);
		if (ret > 0)
		{
			printf("\rSkipping bad block at  "
				"0x%08llx                   "
				"                         \n",
				erase.addr);
			continue;
		}
		else if (ret < 0)
		{
			printf("\n%s: MTD get bad block at 0x%08llx failed: %d\n",
				nand->name, erase.addr, ret);
			return -1;
		}

		if ((ret = nand->erase(nand, &erase)) != 0)
		{
			printf("\n%s: MTD Erase at 0x%08llx failure: %d, ",
				nand->name, erase.addr, ret);
			if (nand->block_markbad && (ret == -EIO)) 
			{
				printf("mark bad block.");
				ret = nand->block_markbad(nand, erase.addr);
				if (ret < 0) 
				{
					printf("\n%s: MTD block_markbad at 0x%08llx failed: %d, aborting\n",
						nand->name, erase.addr, ret);
					return -1;
				}
			}
			printf("\n");
		}

		length -= nand->erasesize;
	}
	return 0;
}
/*****************************************************************************/

int nand_logic_write
(
 nand_logic_t *nand_logic,
 unsigned long long offset,    /* should be alignment with nand page size */
 unsigned int length,          /* should be alignment with nand page size */
 unsigned char *buf,
 int withoob
 )
{
	unsigned long long phylength;
	unsigned long long phyaddress;
	nand_info_t *nand = nand_logic->nand;

	/* Reject write, which are not page aligned */
	if ((offset & (nand->writesize - 1)) || (length & (nand->writesize - 1)))
	{
		printf("Attempt to write non page aligned data, "
			"nand page size: 0x%08x, offset: 0x%08llx, length: 0x%08x\n",
			nand->writesize, offset, length);
		return -1;
	}

	phylength = logic_to_phylength(nand, nand_logic->address,
		(offset + length + nand->erasesize - 1) & (~(nand_logic->erasesize - 1)));
	if ((offset > nand_logic->length)
		|| (length > nand_logic->length)
		|| (phylength > nand_logic->length))
	{
		printf("Attempt to write outside the flash handle area, "
			"flash handle size: 0x%08llx, offset: 0x%08llx, "
			"length: 0x%08x, phylength:  0x%08llx\n",
			nand_logic->length, offset, length, phylength);
		return -1;
	}

	phylength = logic_to_phylength(nand, nand_logic->address,
		(offset + nand->erasesize - 1) & (~(nand_logic->erasesize - 1)));
	if(offset & (nand_logic->erasesize - 1))
	{
		phyaddress = phylength - nand->erasesize +
			(offset & (nand_logic->erasesize - 1)) + nand_logic->address;
	}
	else
	{
		phyaddress = phylength + nand_logic->address;
	}

	if (withoob)
	{
		length = length / nand->writesize * (nand->writesize + nand->oobsize);
		return nand_write_yaffs_skip_bad(nand_logic->nand, phyaddress, &length, buf);
	}
	else
	{
		return nand_write_skip_bad(nand_logic->nand, phyaddress, &length, buf);
	}
}
/*****************************************************************************/

int nand_logic_read
(
 nand_logic_t *nand_logic,
 unsigned long long offset,    /* should be alignment with nand page size */
 unsigned int length,          /* should be alignment with nand page size */
 unsigned char *buf,
 int withoob
)
{
	unsigned long long phylength;
	unsigned long long phyaddress;
	nand_info_t *nand = nand_logic->nand;

	/* Reject read, which are not page aligned */
	if ((offset & (nand->writesize - 1)) || (length & (nand->writesize - 1)))
	{
		printf("Attempt to read non page aligned data, "
			"nand page size: 0x%08x, offset: 0x%08llx, length: 0x%08x\n",
			nand->writesize, offset, length);
		return -1;
	}

	phylength = logic_to_phylength(nand, nand_logic->address,
		(offset + length + nand->erasesize - 1) & (~(nand_logic->erasesize - 1)));
	if ((offset > nand_logic->length)
		|| (length > nand_logic->length)
		|| (phylength > nand_logic->length))
	{
		printf("Attempt to read outside the flash handle area, "
			"flash handle size: 0x%08llx, offset: 0x%08llx, "
			"length: 0x%08x, phylength:  0x%08llx\n",
			nand_logic->length, offset, length, phylength);
		return -1;
	}

	phylength = logic_to_phylength(nand, nand_logic->address,
		(offset + nand->erasesize - 1) & (~(nand_logic->erasesize - 1)));
	if(offset & (nand_logic->erasesize - 1))
		phyaddress = phylength - nand->erasesize +
		(offset & (nand_logic->erasesize - 1)) + nand_logic->address;
	else
		phyaddress = phylength + nand_logic->address;

	if (withoob)
	{
		unsigned long long block_offset;
		unsigned long long read_length;

		while (length > 0)
		{
			block_offset = phyaddress & (nand->erasesize - 1);

			WATCHDOG_RESET ();

			if (nand_block_isbad (nand, phyaddress & ~(nand_logic->erasesize - 1)))
			{
				printf("Skipping bad block 0x%08llx\n",
					phyaddress & ~(nand_logic->erasesize - 1));
				phyaddress += nand->erasesize - block_offset;
				continue;
			}

			if (length < (nand->erasesize - block_offset))
				read_length = length;
			else
				read_length = nand->erasesize - block_offset;

			while (read_length > 0)
			{
				int ret;
				struct mtd_oob_ops ops;

				memset(&ops, 0, sizeof(ops));
				ops.datbuf = buf;
				ops.oobbuf = buf + nand->writesize;
				ops.len = nand->writesize;
				ops.ooblen = nand->oobsize;
				ops.mode = MTD_OOB_RAW;

				if ((ret = nand->read_oob(nand, phyaddress, &ops)) < 0)
				{
					printf("Error (%d) reading page 0x%08llx\n", ret, phyaddress);
					return -1;
				}
				phyaddress  += nand->writesize;
				read_length -= nand->writesize;
				length      -= nand->writesize;
				buf += nand->writesize + nand->oobsize;
			}
		}
		return 0;
	}
	else
	{
		return nand_read_skip_bad(nand, phyaddress, &length, buf);
	}
}
/*****************************************************************************/
