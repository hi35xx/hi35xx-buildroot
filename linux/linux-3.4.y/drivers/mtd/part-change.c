#include <linux/slab.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/err.h>
#include <linux/cmdline-parser.h>
#include "mtdcore.h"

struct mtd_table {
	struct list_head list;
	struct mtd_info *mtd;
};

static LIST_HEAD(mtd_list);
static DEFINE_MUTEX(mtd_list_mutex);

struct mtd_table *find_mtd_partdev_by_mtd(struct mtd_info *mtd)
{
	struct mtd_table *tbl;

	mutex_lock(&mtd_list_mutex);
	list_for_each_entry(tbl, &mtd_list, list) {
		if (tbl->mtd == mtd) {
			mutex_unlock(&mtd_list_mutex);
			return tbl;
		}
	}
	mutex_unlock(&mtd_list_mutex);

	return NULL;
}

struct mtd_info *find_mtd_partdev_by_name(const char *devname)
{
	struct mtd_table *tbl;
	struct mtd_info *mtd;

	mutex_lock(&mtd_list_mutex);
	list_for_each_entry(tbl, &mtd_list, list) {
		mtd = tbl->mtd;
		if (!strncmp(devname, mtd->name, BDEVNAME_SIZE)) {
			mutex_unlock(&mtd_list_mutex);
			return mtd;
		}
	}
	mutex_unlock(&mtd_list_mutex);

	return NULL;
}

int register_mtd_partdev(struct mtd_info *mtd)
{
	struct mtd_table *tbl;

	if (find_mtd_partdev_by_mtd(mtd))
		return -EBUSY;

	tbl = kzalloc(sizeof(struct mtd_table), GFP_KERNEL);
	if (!tbl) {
		pr_err("Can't allocate memory.\n");
		return -ENOMEM;
	}

	tbl->mtd = mtd;

	mutex_lock(&mtd_list_mutex);
	list_add(&tbl->list, &mtd_list);
	mutex_unlock(&mtd_list_mutex);

	return 0;
}

int unregister_mtd_partdev(struct mtd_info *mtd)
{
	struct mtd_table *tbl;

	tbl = find_mtd_partdev_by_mtd(mtd);
	if (!tbl) {
		mutex_lock(&mtd_list_mutex);
		list_del(&tbl->list);
		mutex_unlock(&mtd_list_mutex);
		kfree(tbl);
	}
	return 0;
}

/*****************************************************************************/

static int mtd_add_part(int slot, struct cmdline_subpart *subpart, void *param)
{
	struct mtd_partition *mtdpart = &((struct mtd_partition *)param)[slot];

	mtdpart->name = subpart->name;
	mtdpart->offset = subpart->from;
	mtdpart->size = subpart->size;
	mtdpart->mask_flags = 0;
	mtdpart->ecclayout = 0;

	return 0;
}

int mtd_part_change(struct cmdline_parts *parts)
{
	int ret = -ENODEV;
	struct mtd_info *mtd;
	int nr_parts = parts->nr_subparts;
	struct mtd_partition *mtdparts;

	mtd = find_mtd_partdev_by_name(parts->name);
	if (!mtd)
		return -ENODEV;

	mtdparts = kzalloc(sizeof(*mtdparts) * nr_parts, GFP_KERNEL);
	if (!mtdparts) {
		pr_err("Can't allocate memory.\n");
		return -ENOMEM;
	}

	cmdline_parts_set(parts, mtd->size, 0, mtd_add_part, (void *)mtdparts);

	ret = del_mtd_partitions(mtd);
	pr_notice("Remove mtd device '%s' partitions %s.\n",
		  mtd->name, (!ret ? "success" : "fail"));

	if (!ret) {
		ret = add_mtd_partitions(mtd, mtdparts, nr_parts);
		pr_notice("Add mtd device '%s' partitions %s.\n",
			  mtd->name, (!ret ? "success" : "fail"));
	}

	kfree(mtdparts);
	return ret;
}
