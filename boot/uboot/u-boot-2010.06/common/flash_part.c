/******************************************************************************
*    Copyright (c) 2009-2011 by Hisi.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <common.h>
#include <linux/types.h>
#include <linux/string.h>
#include <nand.h>

/*****************************************************************************/

static unsigned long long memparse(const char *ptr, char **retptr);

static int find_part(char *s, char **retptr, char *ptnname,
					 uint64_t *start, uint64_t *length);

/*****************************************************************************/

int find_flash_part /* 1 - find, 0 - no find */
(
 char *mtdparts,
 char *media_name,  /* hi_sfc, hinand */
 char *ptn_name,
 uint64_t *start,
 uint64_t *length
 )
{
	int rel;
	char *s = (char *)mtdparts;

	for( ; s != NULL; )
	{
		char *p;
		(*start) = 0;

		/* fetch <mtd-id> */
		if (!(p = strchr(s, ':')))
		{
			printf("no mtd-id\n");
			return 0;
		}
		else
		{
			*p = '\0';
			rel = strcmp(s, media_name);
			*p = ':';
			if (rel)
			{
				if (!(p = strchr(s, ';')))
					return 0;
				s = p + 1;
				continue;
			}
		}

		/*
		* parse one mtd. have it reserve memory for the
		* struct cmdline_mtd_partition and the mtd-id string.
		*/
		rel =find_part(p + 1, &s, ptn_name, start, length);

		if (rel == 1)
			return 1;
		else if (rel == -1)
			return 0;

		/* EOS - we're done */
		if (*s == 0)
			break;

		/* does another spec follow? */
		if (*s != ';')
			return 0;
		s++;
	}
	return 0;
}
/*****************************************************************************/

static int find_part /* 0 - not find, 1 - find, -1 - error. */
(
 char *s,
 char **retptr,
 char *ptnname,
 uint64_t *start,
 uint64_t *length
 )
{
	int isfind = 0;
	char delim = 0;

	/* fetch the partition size */
	if (*s == '-')
	{   /* assign all remaining space to this partition */
		(*length) = (uint64_t)(-1);
		s++;
	}
	else
	{
		(*length) = (uint64_t)memparse(s, &s);
		if ((*length) < 8192)
		{
			printf("partition size too small (%llx)\n", (*length));
			return -1;
		}
	}

	/* check for offset */
	if (*s == '@')
	{
		s++;
		memparse(s, &s);
	}

	/* now look for name */
	if (*s == '(')
		delim = ')';

	if (delim)
	{
		char *p;
		char *name;

		name = ++s;
		p = strchr(name, delim);
		if (!p)
		{
			printf("no closing %c found in partition name\n", delim);
			return -1;
		}

		*p = '\0';
		if (!strcmp(name, ptnname))
			isfind = 1;
		*p = ')';
		s = p + 1;
	}

	/* test for options */
	if (strncmp(s, "ro", 2) == 0)
	{
		s += 2;
	}

	/* if lk is found do NOT unlock the MTD partition*/
	if (strncmp(s, "lk", 2) == 0)
	{
		s += 2;
	}

	/* test if more partitions are following */
	if (*s == ',')
	{
		int ret;
		if ((*length) == (uint64_t)(-1))
		{
			printf("no partitions allowed after a fill-up partition\n");
			return -1;
		}

		if (isfind)
			return 1;
		(*start) += (*length);
		ret = find_part(s + 1, &s, ptnname, start, length);
		*retptr = s;
		return ret;
	}
	*retptr = s;
	return isfind;
}
/*****************************************************************************/

static unsigned long long memparse(const char *ptr, char **retptr)
{
	char *endptr;   /* local pointer to end of parsed string */

	unsigned long long ret = simple_strtoull(ptr, &endptr, 0);

	switch (*endptr) {
	case 'G':
	case 'g':
		ret <<= 10;
	case 'M':
	case 'm':
		ret <<= 10;
	case 'K':
	case 'k':
		ret <<= 10;
		endptr++;
	default:
		break;
	}

	if (retptr)
		*retptr = endptr;

	return ret;
}
