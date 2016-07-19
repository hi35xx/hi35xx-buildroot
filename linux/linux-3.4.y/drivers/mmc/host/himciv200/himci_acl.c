/******************************************************************************
 *    Copyright (C) 2014 Hisilicon STB Development Dept
 *    All rights reserved.
 * ***
 *    Create by Czyong
 *
******************************************************************************/
#define pr_fmt(fmt) "mmcacl: " fmt

#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <linux/sched.h>
#include <linux/cmdline-parser.h>
#include <linux/delay.h>

#include "himci_acl.h"

struct part_t {
	u32 start;
	u32 end;
	int part;
};

static int mmcacl_devid = -1;

static struct part_t ro_parts[64];
static int nr_ro_parts;

static int mmcadd_part(int slot, struct cmdline_subpart *subpart, void *param)
{
	u32 start = (u32)(subpart->from >> 9);
	u32 end = start + (u32)(subpart->size >> 9);
	struct part_t *part = (struct part_t *)param;

	if (subpart->flags & PF_RDONLY) {
		if (nr_ro_parts < 64) {
			part[nr_ro_parts].start = start;
			part[nr_ro_parts].end   =  end;
			part[nr_ro_parts].part  = slot;
			nr_ro_parts++;
		} else {
			pr_err("too many ro partition. please increase ro_parts\n");
		}
	}

	return 0;
}
/*****************************************************************************/

static int mmcpart_part(char *cmdline)
{
	struct cmdline_parts *bdev_parts = NULL;

	if (cmdline_parts_parse(&bdev_parts, cmdline))
		return -1;

	cmdline_parts_set(bdev_parts, (sector_t)0x1000000000ULL, 0, mmcadd_part,
			  (void *)ro_parts);

	cmdline_parts_free(&bdev_parts);

	return 0;
}
/*****************************************************************************/

int himci_acl_init(int devid)
{
	char *cmdline = NULL;

	if (mmcacl_devid != -1)
		return 0;

	mmcacl_devid = devid;

	cmdline = get_blkdevparts();
	if (!cmdline)
		return 0;

	mmcpart_part(cmdline);

	return 0;
}
/*****************************************************************************/

struct part_t *part_find(struct part_t *part,
		u32 nr_part, u32 ofblk, u32 endblk)
{
	u32 min = 0;
	u32 max = nr_part;
	u32 pos = (min + max) >> 1;

	do {
		if (endblk <= part[pos].start) {
			max = pos;
			goto next;
		}

		if (part[pos].end <= ofblk) {
			min = pos;
			goto next;
		}

		break;
next:
		pos = (min + max) >> 1;
	} while (pos != min && pos != max);

	return (endblk <= part[pos].start || part[pos].end <= ofblk) ?
		NULL : &part[pos];
}
/*****************************************************************************/

int himci_acl_rw(int devid, int is_write, u32 ofblk, u32 nrblk)
{
	struct part_t *part;

	if (mmcacl_devid != devid)
		return 0;

	if (!is_write)
		return 0;

	if (!nr_ro_parts)
		return 0;

	part = part_find(ro_parts, nr_ro_parts, ofblk, ofblk + nrblk);
	if (part) {
		pr_warn("attempt to write ro area, offset:0x%08x, count:0x%08x.\n",
			ofblk, nrblk);
		return 1;
	}

	return 0;
}
