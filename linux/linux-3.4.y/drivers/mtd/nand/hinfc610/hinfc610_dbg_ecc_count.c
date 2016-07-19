/******************************************************************************
 *    COPYRIGHT (C) 2013 Czyong. Hisilicon
 *    All rights reserved.
 * ***
 *    Create by Czyong 2013-07-05
 *
******************************************************************************/

#include <linux/moduleparam.h>
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

#include "hinfc610_os.h"
#include "hinfc610.h"
#include "hinfc610_dbg.h"

#ifndef CONFIG_HINFC610_DBG_ECC_COUNT_NUM
#  define CONFIG_HINFC610_DBG_ECC_COUNT_NUM          (100)
#endif /* CONFIG_HINFC610_DBG_ECC_COUNT_NUM */

struct hinfc610_dbg_ecc_count_item_t {
	unsigned int page;
	unsigned int page_status;    /* the same as host->page_status */
	unsigned short hour;
	unsigned short min;
	unsigned short sec;
	unsigned short msec;

	unsigned char ecc[4];
};

struct hinfc610_dbg_ecc_count_t {

	struct dentry *dentry;
	unsigned int index; /* current logs index */
	int count;          /* number of logs */

	struct hinfc610_ecc_inf_t *ecc_inf;
	unsigned int offset;
	unsigned int length;
	unsigned int pagecount;

	unsigned char *item;

	unsigned int read_index;
};

#define GET_ITEM(_ecc_count, _index)     \
	((struct hinfc610_dbg_ecc_count_item_t *)((_ecc_count)->item + \
		((sizeof(struct hinfc610_dbg_ecc_count_item_t) + \
			(_ecc_count)->ecc_inf->section) * (_index))))

static DEFINE_MUTEX(dbg_ecc_count_mutex);
static struct hinfc610_dbg_ecc_count_t *dbg_ecc_count;
/*****************************************************************************/

static void do_gettime(unsigned short *hour, unsigned short *min,
		       unsigned short *sec, unsigned short *msec)
{
	long val;
	struct timeval tv;

	do_gettimeofday(&tv);
	val = tv.tv_sec % 86400; /* the second form 0 hour */

	if (hour)
		*hour = val / 3600;
	val %= 3600;
	if (min)
		*min  = val / 60;
	if (sec)
		*sec  = val % 60;
	if (msec)
		*msec = tv.tv_usec / 1000;
}
/*****************************************************************************/

static ssize_t dbgfs_ecc_count_read(struct file *filp, char __user *buffer,
				    size_t count, loff_t *ppos)
{
	int len = 0;
	char buf[128] = {0};
	unsigned int read_index;
	char __user *pusrbuf = buffer;
	struct hinfc610_dbg_ecc_count_item_t *item;

	if (*ppos == 0) {

		if (dbg_ecc_count->count
		    < CONFIG_HINFC610_DBG_ECC_COUNT_NUM)
			dbg_ecc_count->read_index = 0;
		else
			dbg_ecc_count->read_index
				= (dbg_ecc_count->index + 1);

		len = snprintf(buf, sizeof(buf),
			"Print parameter: \"offset=%d length=%d\"\n",
			dbg_ecc_count->offset,
			dbg_ecc_count->length);

		if (copy_to_user(pusrbuf, buf, len))
			return -EFAULT;

		pusrbuf += len;

		len = snprintf(buf, sizeof(buf),
			"  UTC Clock    page          ecc data\n");

		if (copy_to_user(pusrbuf, buf, len))
			return -EFAULT;

		pusrbuf += len;

	} else if (dbg_ecc_count->read_index == dbg_ecc_count->index)
		return 0;

	for (read_index = dbg_ecc_count->read_index;
	     (read_index != dbg_ecc_count->index); ++read_index) {

		if (read_index >= CONFIG_HINFC610_DBG_ECC_COUNT_NUM)
			read_index = 0;

		item = GET_ITEM(dbg_ecc_count, read_index);

		if ((count - (pusrbuf - buffer)) < 80)
			break;

		len = snprintf(buf, sizeof(buf),
			"%02d:%02d:%02d.%04d  0x%08X    ",
			item->hour, item->min, item->sec, item->msec,
			item->page);

		if (copy_to_user(pusrbuf, buf, len))
			return -EFAULT;

		pusrbuf += len;

		if (IS_PS_BAD_BLOCK(item) || IS_PS_EMPTY_PAGE(item) ||
		    IS_PS_UN_ECC(item)) {
			char *ptr = buf;

			if (IS_PS_BAD_BLOCK(item))
				ptr += snprintf(ptr, 16, "bb ");
			else if (IS_PS_EMPTY_PAGE(item))
				ptr += snprintf(ptr, 16, "ep ");

			if (IS_PS_UN_ECC(item))
				ptr += snprintf(ptr, 16, "ecc ");

			if (IS_PS_EPM_ERR(item) || IS_PS_BBM_ERR(item))
				ptr += snprintf(ptr, 16, "? ");

			ptr += snprintf(ptr, 16, "\n");
			len = (ptr - buf);

		} else {
			int ix;
			char *ptr = buf;

			for (ix = 0; ix < dbg_ecc_count->ecc_inf->section; ix++)
				ptr += snprintf(ptr, 16, "%d/", item->ecc[ix]);

			if (IS_PS_EPM_ERR(item))
				ptr += snprintf(ptr, 16, " ?");

			ptr += snprintf(ptr, 16, "\n");
			len = (ptr - buf);
		}

		if (copy_to_user(pusrbuf, buf, len))
			return -EFAULT;
		pusrbuf += len;
	}

	dbg_ecc_count->read_index = read_index;

	*ppos += (pusrbuf - buffer);
	return pusrbuf - buffer;
}
/******************************************************************************/
/*
 * echo "offset=8192,length=102400" > ecc_count
 *
 */
static ssize_t dbgfs_ecc_count_write(struct file *filp,
				     const char __user *buffer, size_t count,
				     loff_t *ppos)
{
	char *str;
	char buf[128] = {0};
	unsigned long value;
	unsigned long pos = 0;

	if (count > sizeof(buf))
		count = sizeof(buf);

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	while (pos < count) {
		while (pos < count &&
			(buf[pos] == ' ' || buf[pos] == ',' || buf[pos] == ';'))
			pos++;

		if (pos >= count)
			break;

		switch (buf[pos]) {

		case 'o':

			if (memcmp(&buf[pos], CMD_WORD_OFFSET,
				sizeof(CMD_WORD_OFFSET) - 1))
				break;

			pos += sizeof(CMD_WORD_OFFSET) - 1;
			str = (char *)(buf + pos);
			value = simple_strtoul(str, &str, 10);

			if (value < 0)
				value = 0;
			if (value >= dbg_ecc_count->pagecount)
				value = 0;

			dbg_ecc_count->offset = (value & ~7);

			break;

		case 'l':
			if (memcmp(&buf[pos], CMD_WORD_LENGTH,
				sizeof(CMD_WORD_LENGTH) - 1))
				break;

			pos += sizeof(CMD_WORD_LENGTH) - 1;
			str = (char *)(buf + pos);
			value = simple_strtoul(str, &str, 10);

			if (value < 0)
				value = dbg_ecc_count->pagecount;

			value = ((value + 7) & ~7);

			if (dbg_ecc_count->offset + value >
			    dbg_ecc_count->pagecount)
				value = dbg_ecc_count->pagecount
					- dbg_ecc_count->offset;

			dbg_ecc_count->length = value;

			break;
		}

		while (pos < count &&
			(buf[pos] != ' ' && buf[pos] != ',' && buf[pos] != ';'))
			pos++;
	}

	return count;
}
/******************************************************************************/

static struct file_operations dbgfs_ecc_count_fops = {
	.owner = THIS_MODULE,
	.read  = dbgfs_ecc_count_read,
	.write = dbgfs_ecc_count_write,
};
/*****************************************************************************/

static int dbgfs_ecc_count_init(struct dentry *root, struct hinfc_host *host)
{
	unsigned int size;
	unsigned int pagesize;
	unsigned int chipsize;
	struct hinfc610_ecc_inf_t *ecc_inf;
	struct hinfc610_dbg_ecc_count_t *ecc_count;

	if (dbg_ecc_count)
		return 0;

	ecc_inf = hinfc610_get_ecc_inf(host, host->pagesize, host->ecctype);
	if (!ecc_inf) {
		printk(KERN_WARNING
		       "ecc_count: The NAND not support this interface.\n");
		return -1;
	}

	size = sizeof(struct hinfc610_dbg_ecc_count_t);
	size += CONFIG_HINFC610_DBG_ECC_COUNT_NUM *
		(sizeof(struct hinfc610_dbg_ecc_count_item_t)
		 + ecc_inf->section);

	ecc_count = vmalloc(size);
	if (!ecc_count) {
		PR_ERR("Can't allocate memory.\n");
		return -ENOMEM;
	}
	memset(ecc_count, 0, size);

	ecc_count->item = (char *)ecc_count +
		sizeof(struct hinfc610_dbg_ecc_count_t);
	ecc_count->ecc_inf = ecc_inf;

	pagesize  = (host->pagesize >> 10);
	chipsize = (unsigned int)(host->chip->chipsize >> 10);
	ecc_count->pagecount = (chipsize / pagesize);
	ecc_count->length = ecc_count->pagecount;

	ecc_count->dentry = debugfs_create_file("ecc_count",
		S_IFREG | S_IRUSR | S_IWUSR,
		root, NULL, &dbgfs_ecc_count_fops);
	if (!ecc_count->dentry) {
		PR_ERR("Can't create 'ecc_count' file.\n");
		vfree(ecc_count);
		return -ENOENT;
	}

	dbg_ecc_count = ecc_count;

	return 0;
}
/*****************************************************************************/

static int dbgfs_ecc_count_uninit(void)
{
	if (!dbg_ecc_count)
		return 0;

	mutex_lock(&dbg_ecc_count_mutex);

	debugfs_remove(dbg_ecc_count->dentry);

	vfree(dbg_ecc_count);
	dbg_ecc_count = NULL;

	mutex_unlock(&dbg_ecc_count_mutex);

	return 0;
}
/*****************************************************************************/

static void dbg_ecc_count_read(struct hinfc_host *host)
{
	unsigned int page;
	struct hinfc610_dbg_ecc_count_item_t *item;

	mutex_lock(&dbg_ecc_count_mutex);

	if (!dbg_ecc_count)
		goto exit;

	page = GET_PAGE_INDEX(host);

	if (page < dbg_ecc_count->offset ||
	    page > (dbg_ecc_count->offset + dbg_ecc_count->length))
		goto exit;

	item = GET_ITEM(dbg_ecc_count, dbg_ecc_count->index);

	dbg_ecc_count->count++;

	do_gettime(&item->hour, &item->min, &item->sec, &item->msec);

	item->page = page;
	item->page_status = host->page_status;

	if (!IS_PS_UN_ECC(host))
		dbg_ecc_count->ecc_inf->ecc_inf(host, item->ecc);

	if (++dbg_ecc_count->index >= CONFIG_HINFC610_DBG_ECC_COUNT_NUM)
		dbg_ecc_count->index = 0;

exit:
	mutex_unlock(&dbg_ecc_count_mutex);
}
/*****************************************************************************/

struct hinfc610_dbg_inf_t hinfc610_dbg_inf_ecc_count = {
	"ecc_count", 0,
	dbgfs_ecc_count_init,
	dbgfs_ecc_count_uninit,
	dbg_ecc_count_read,
	NULL,
	NULL,
	NULL,
};
/*****************************************************************************/
