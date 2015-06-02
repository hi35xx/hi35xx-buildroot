/******************************************************************************
 * NAND Flash Controller V300 Device Driver
 * Copyright (c) 2009-2010 by Hisilicon.
 * All rights reserved.
 ******************************************************************************/
/* defined(CONFIG_HINFC300_DBG_NAND_PROC_FILE)
 *|| defined(CONFIG_HINFC300_DBG_NAND_EC_NOTICE)
 */
#if defined(CONFIG_HINFC300_DBG_NAND_PROC_FILE)\
	|| defined(CONFIG_HINFC300_DBG_NAND_EC_NOTICE)
#define DBG_NAND_CORRECT_ERROR
#endif
/*****************************************************************************/
#ifdef DBG_NAND_CORRECT_ERROR

static void dbg_nand_ec_init(void)
{
#ifdef CONFIG_HINFC300_DBG_NAND_PROC_FILE
	printk(KERN_DEBUG "nand debug: DBG_NAND_CORRECT_ERROR\n");
#endif /* defined(CONFIG_HINFC300_DBG_NAND_PROC_FILE) */

#ifdef CONFIG_HINFC300_DBG_NAND_EC_NOTICE
	printk(KERN_DEBUG "nand debug: CONFIG_HINFC300_DBG_NAND_EC_NOTICE\n");
#endif /* CONFIG_HINFC300_DBG_NAND_EC_NOTICE */
}
/*****************************************************************************/
static void dbg_nand_ec_notice(struct hinfc_host *host)
{
	unsigned long val;
	unsigned long *oob;

	if (host->ecctype == et_ecc_none || !host->uc_er)
		return;

	oob = (unsigned long *)(host->buffer + host->pagesize);
	val = (oob[0] & oob[1] & oob[2] & oob[3]
		& oob[4] & oob[5] & oob[6] & oob[7]);

	if (val == 0xFFFFFFFF) {
		host->uc_er = 0;
	} else {
#  ifdef CONFIG_HINFC300_DBG_NAND_EC_NOTICE
		int ix = 0;
		int jx = 0;
		int nBits = 0;

		for (ix = 0; ix < 8; ix++) {
			unsigned long val = oob[ix];
			for (jx = 0; jx < 32 && val; jx++) {
				if (!(val & 0x01))
					nBits++;

				val = (val >> 1);
			}
			nBits += (32 - jx);
		}

		/* if exists file "/proc/nand",
		 * this also show in "/proc/nand"
		 */
		printk(KERN_ERR "!!! nand uncorrectable error,"
			" page:0x%08X, (%d/256 bits zero)\n",
			GET_PAGE_INDEX(host), nBits);
#  endif /* CONFIG_HINFC300_DBG_NAND_EC_NOTICE */
	}
}
#endif /* DBG_NAND_CORRECT_ERROR */
/*****************************************************************************/

#define CMD_WORD_OFFSET     "offset="
#define CMD_WORD_LENGTH     "length="
#define CMD_WORD_CLEAN      "clean"

/*****************************************************************************
 *  create file "/proc/nand" show the last 100 operation
 ******************************************************************************/
#if defined(CONFIG_HINFC300_DBG_NAND_PROC_FILE)

#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#ifndef CONFIG_HINFC300_DBG_NAND_NUM_OF_LOGS
#define CONFIG_HINFC300_DBG_NAND_NUM_OF_LOGS   (80)
#endif /* CONFIG_HINFC300_DBG_NAND_NUM_OF_LOGS */

#ifndef CONFIG_HINFC300_DBG_NAND_LOG_LENGTH
#define CONFIG_HINFC300_DBG_NAND_LOG_LENGTH    (40)
#endif /* CONFIG_HINFC300_DBG_NAND_LOG_LENGTH */

#ifndef CONFIG_HINFC300_DBG_NAND_DATA_LENGTH
#define CONFIG_HINFC300_DBG_NAND_DATA_LENGTH   (8)
#endif /* CONFIG_HINFC300_DBG_NAND_DATA_LENGTH */

struct dbg_nand_logs_t {
	unsigned short hour;
	unsigned short min;
	unsigned short sec;
	unsigned short msec;

	unsigned short cycle;

	unsigned long  page;
	unsigned long  offset;
	unsigned long  length;

	char error;
	char op;

	unsigned char  data[CONFIG_HINFC300_DBG_NAND_LOG_LENGTH];
};

struct dbg_nand_proc_t {
	struct proc_dir_entry *entry;
	int IsInit;
	int index;
	int NumOfLogs;

	int offset;
	int length;

	struct dbg_nand_logs_t logs[CONFIG_HINFC300_DBG_NAND_NUM_OF_LOGS];

	int read_index;
};

static struct dbg_nand_proc_t dbg_nand_proc = {
	.length = CONFIG_HINFC300_DBG_NAND_DATA_LENGTH,
};
/*****************************************************************************/
/*
 * echo "offset=2048 length=8" > /proc/nand
 */
static int dbg_nand_write_proc(struct file *file, const char *buf,
	unsigned long count, void *data)
{
	char *p;
	int value;
	unsigned long pos = 0;
	struct dbg_nand_proc_t *nand_proc = (struct dbg_nand_proc_t *)data;

	while (pos < count) {
		while (pos < count
			&& (buf[pos] == ' '
				|| buf[pos] == ','
				|| buf[pos] == ';'))
			pos++;

		if (pos >= count)
			break;

		switch (buf[pos]) {
		case 'o':
			if (!memcmp(&buf[pos], CMD_WORD_OFFSET,
				sizeof(CMD_WORD_OFFSET) - 1)) {
				pos += sizeof(CMD_WORD_OFFSET) - 1;
				p = (char *)(buf + pos);
				/*value = simple_strtoul(p, &p, 10);*/
				kstrtoul(p, 10, &value);
				if (value < 0)
					value = 0;
				nand_proc->offset = value;
			}
			break;

		case 'l':
			if (!memcmp(&buf[pos], CMD_WORD_LENGTH,
				sizeof(CMD_WORD_LENGTH) - 1)) {
				pos += sizeof(CMD_WORD_LENGTH) - 1;
				p = (char *)(buf + pos);
				/*value = simple_strtoul(p, &p, 10);*/
				kstrtoul(p, 10, &value);
				if (value < 0)
					value = 0;
				nand_proc->length = value;
			}
			break;
		}

		while (pos < count
			&& (buf[pos] != ' '
				&& buf[pos] != ','
				&& buf[pos] != ';'))
			pos++;
	}

	return count;
}
/*****************************************************************************/

static int dbg_nand_read_proc(char *page, char **start, off_t off, int count,
	int *eof, void *data)
{
	int read_index;
	char *ppage = page;
	struct dbg_nand_logs_t *logs;
	struct dbg_nand_proc_t *nand_proc = (struct dbg_nand_proc_t *)data;

	*(int *)start = 1;
	*eof = 1;

	if (off == 0) {
		if (dbg_nand_proc.NumOfLogs
			< CONFIG_HINFC300_DBG_NAND_NUM_OF_LOGS)
			nand_proc->read_index = 0;
		else
			nand_proc->read_index = dbg_nand_proc.index + 1;

		ppage += sprintf(ppage, "\"offset=%d length=%d\"\n",
			nand_proc->offset, nand_proc->length);
		ppage += sprintf(ppage,
			"UTC Clock op cylce page-offset  data\n");
	} else if (nand_proc->read_index == dbg_nand_proc.index) {
		return 0;
	}

	for (read_index = nand_proc->read_index;
		(read_index != nand_proc->index);
		++read_index) {
		if (read_index >= CONFIG_HINFC300_DBG_NAND_NUM_OF_LOGS)
			read_index = 0;

		logs = &nand_proc->logs[read_index];

		if ((count - (ppage - page)) < (50 + logs->length * 3))
			break;

		ppage += sprintf(ppage,
			"%02d:%02d:%02d.%04d  %c  %-2u 0x%08lX-%04lX",
			logs->hour, logs->min, logs->sec,
			logs->msec, logs->op,
			logs->cycle, logs->page, logs->offset);

		if (logs->op == 'E') {
			ppage += sprintf(ppage, "---");
		} else {
			int ix;
			ppage += sprintf(ppage, " %c ", logs->error);

			for (ix = 0; ix < logs->length; ix++) {
				if ((ix % 16) == 15)
					ppage += sprintf(ppage,
						"%02X-",
						logs->data[ix]);
				else
					ppage += sprintf(ppage,
						"%02X ",
						logs->data[ix]);
			}
		}
		ppage += sprintf(ppage, "\n");
	}

	nand_proc->read_index = read_index;

	return ppage - page;
}
/*****************************************************************************/

static int dbg_nand_proc_init(void)
{
	int res = -ENOMEM;

	if (dbg_nand_proc.IsInit++)
		return 0;

	dbg_nand_proc.entry = create_proc_entry("nand",
		S_IFREG | S_IRUSR | S_IWUSR, NULL);
	if (!dbg_nand_proc.entry)
		goto out;

	dbg_nand_proc.entry->read_proc  = dbg_nand_read_proc;
	dbg_nand_proc.entry->write_proc = dbg_nand_write_proc;
	dbg_nand_proc.entry->data       = (void *)&dbg_nand_proc;

	printk(KERN_DEBUG "nand debug: CONFIG_HINFC300_DBG_NAND_PROC_FILE\n");

	res = 0;
out:
	return res;
}
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
static int dbg_nand_proc_save_logs(struct hinfc_host *host, char *op)
{
	struct dbg_nand_logs_t *logs =
		&dbg_nand_proc.logs[dbg_nand_proc.index];

	dbg_nand_proc.NumOfLogs++;

	do_gettime(&logs->hour, &logs->min, &logs->sec, &logs->msec);
	logs->error = ' ';

#  ifdef DBG_NAND_CORRECT_ERROR
	if (host->uc_er)
		logs->error = '*';

#endif /* DBG_NAND_CORRECT_ERROR */

	logs->op = *op;
	logs->cycle = host->addr_cycle;
	logs->length = dbg_nand_proc.length;

	if (*op != 'E') {
		unsigned long buflen = (host->pagesize + host->oobsize);

		logs->offset = (host->addr_value[0] & 0xFFFF);
		logs->page   = GET_PAGE_INDEX(host);

		if (!logs->offset)
			logs->offset = dbg_nand_proc.offset;

		if (logs->offset >= buflen)
			logs->offset = 0;

		if (logs->length > (buflen - logs->offset))
			logs->length = (buflen - logs->offset);

		if (logs->length > CONFIG_HINFC300_DBG_NAND_LOG_LENGTH)
			logs->length = CONFIG_HINFC300_DBG_NAND_LOG_LENGTH;

		memcpy(logs->data, host->buffer + logs->offset, logs->length);
	} else {
		logs->offset = 0;
		logs->page  = host->addr_value[0];
		logs->length = 0;
	}

	if (++dbg_nand_proc.index >= CONFIG_HINFC300_DBG_NAND_NUM_OF_LOGS)
		dbg_nand_proc.index	= 0;

	return 0;
}
/*****************************************************************************/
#endif /* defined(CONFIG_HINFC300_DBG_NAND_PROC_FILE) */

/*****************************************************************************
 *  create file "/proc/nand_stat" statistics nand read/write/erase operation
 ******************************************************************************/

#ifdef CONFIG_HINFC300_DBG_NAND_STAT_PROC_FILE

#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#define DBG_NAND_STAT_START_ADDR           (0)
#define DBG_NAND_STAT_NUM_OF_PAGE          (640)

struct dbg_nand_stat_proc_t {
	struct proc_dir_entry *entry;
	int IsInit;

	struct dbg_nand_stat_page {
		unsigned short read;
		unsigned short write;

	} page[DBG_NAND_STAT_NUM_OF_PAGE];

	unsigned short *block;

	unsigned long start_page;
	int page_num;

	unsigned long start_block;
	int block_num;

	int page_size;
	int block_size;
	int num_page_block;

	int read_index;
};

static struct dbg_nand_stat_proc_t dbg_nand_stat_proc = {
	.page_num = DBG_NAND_STAT_NUM_OF_PAGE,
};
/*****************************************************************************/
/*
 * echo "offset=0x0000,clean" > /proc/nand_stat
 */
static int dbg_nand_stat_write_proc(struct file *file, const char *buf,
	unsigned long count, void *data)
{
	char *p;
	unsigned long value;
	unsigned long pos = 0;
	struct dbg_nand_stat_proc_t *nand_stat =
		(struct dbg_nand_stat_proc_t *)data;

	while (pos < count) {
		while (pos < count
			&& (buf[pos] == ' '
				|| buf[pos] == ','
				|| buf[pos] == ';'))
			pos++;

		if (pos >= count)
			break;

		switch (buf[pos]) {
		case 'o':
			if (!memcmp(&buf[pos],
				CMD_WORD_OFFSET,
				sizeof(CMD_WORD_OFFSET) - 1)) {
				pos += sizeof(CMD_WORD_OFFSET) - 1;
				p = (char *)(buf + pos);
				/*value = simple_strtoul(p, &p, 16);*/
				kstrtoul(p, 16, &value);
				if (value < 0)
					value = 0;
				value /= nand_stat->page_size;
				nand_stat->start_block = value
					/ nand_stat->num_page_block;
				nand_stat->start_page  =
					nand_stat->start_block
					* nand_stat->num_page_block;
				memset(nand_stat->page,
					0,
					sizeof(nand_stat->page));
				memset(nand_stat->block, 0,
					nand_stat->block_num
						* sizeof(unsigned short));
			}
			break;
		case 'c':
			if (!memcmp(&buf[pos],
				CMD_WORD_CLEAN,
				sizeof(CMD_WORD_CLEAN) - 1)) {
				memset(nand_stat->page, 0,
					sizeof(nand_stat->page));
				memset(nand_stat->block, 0,
					nand_stat->block_num
						* sizeof(unsigned short));
			}
			break;
		}

		while (pos < count
			&& (buf[pos] != ' '
				&& buf[pos] != ','
				&& buf[pos] != ';'))
			pos++;
	}

	return count;
}
/*****************************************************************************/
static int dbg_nand_stat_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int flash_page;
	char *ppage = page;
	struct dbg_nand_stat_proc_t *nand_stat =
		(struct dbg_nand_stat_proc_t *)data;
	int ret;
	*(int *)start = 1;
	*eof = 1;

	if (off == 0) {
		nand_stat->read_index = 0;
		ppage += sprintf(ppage, "\"offset=0x%lX,clean\"\n",
			nand_stat->start_page * nand_stat->page_size);
		ppage += sprintf(ppage,
			"Statistic address: 0x%08lX - 0x%08lX\n",
			(nand_stat->start_page * nand_stat->page_size),
			((nand_stat->start_page + nand_stat->page_num)
			 * nand_stat->page_size));
	}

	for (flash_page = nand_stat->read_index;
		(flash_page < nand_stat->page_num)
		&& ((ppage - page) < (count - 100));
		flash_page++) {
		if (!(flash_page % nand_stat->num_page_block)) {
			ppage += sprintf(ppage,
				"address page read write erase\n");
			int blocktmp = flash_page/nand_stat->num_page_block;
			ppage += sprintf(ppage,
				"0x%08lX %-5lu %-5u %-5u %-5u\n",
				((nand_stat->start_page + flash_page)
				 * nand_stat->page_size),
				(nand_stat->start_page + flash_page),
				nand_stat->page[flash_page].read,
				nand_stat->page[flash_page].write,
				nand_stat->block[blocktmp]);
		} else {
			ppage += sprintf(ppage, "0x%08lX %-5lu %-5u %-5u\n",
				((nand_stat->start_page + flash_page)
				 * nand_stat->page_size),
				(nand_stat->start_page + flash_page),
				nand_stat->page[flash_page].read,
				nand_stat->page[flash_page].write);
		}
	}

	nand_stat->read_index = flash_page;

	ret = ppage - page;
	return ret;
}
/*****************************************************************************/
static int dbg_nand_stat_proc_init(int page_size, int block_size)
{
	int res = -ENOMEM;
	int num_page_block = block_size / page_size;

	if (dbg_nand_stat_proc.IsInit++)
		return 0;

	dbg_nand_stat_proc.entry = create_proc_entry("nand_stat",
		S_IFREG | S_IRUSR | S_IWUSR, NULL);
	if (!dbg_nand_stat_proc.entry)
		goto out;

	dbg_nand_stat_proc.start_page
		= (DBG_NAND_STAT_START_ADDR / page_size);
	dbg_nand_stat_proc.num_page_block = num_page_block;
	dbg_nand_stat_proc.start_block
		= (dbg_nand_stat_proc.start_page / num_page_block);
	dbg_nand_stat_proc.start_page
		= (dbg_nand_stat_proc.start_block * num_page_block);

	dbg_nand_stat_proc.page_size = page_size;
	dbg_nand_stat_proc.block_size = block_size;

	dbg_nand_stat_proc.block_num =
		(dbg_nand_stat_proc.page_num / num_page_block);
	dbg_nand_stat_proc.page_num =
		(dbg_nand_stat_proc.block_num * num_page_block);

	dbg_nand_stat_proc.block = (unsigned short *)
		kmalloc(dbg_nand_stat_proc.block_num * sizeof(unsigned short),
			GFP_KERNEL);

	if (!dbg_nand_stat_proc.block) {
		remove_proc_entry("nand_stat", NULL);
		goto out;
	}
	memset(dbg_nand_stat_proc.block, 0,
		dbg_nand_stat_proc.block_num * sizeof(unsigned short));

	dbg_nand_stat_proc.entry->read_proc  = dbg_nand_stat_read_proc;
	dbg_nand_stat_proc.entry->write_proc = dbg_nand_stat_write_proc;
	dbg_nand_stat_proc.entry->data       = (void *)&dbg_nand_stat_proc;

	printk(KERN_DEBUG "nand debug:"
		" CONFIG_HINFC300_DBG_NAND_STAT_PROC_FILE\n");

	res = 0;
out:
	return res;
}
/*****************************************************************************/
static int dbg_nand_stat_operation(struct hinfc_host *host, char *op)
{
	unsigned long page;

	if (*op == 'R' || *op == 'W')
		page = GET_PAGE_INDEX(host);
	else if (*op == 'E')
		page = host->addr_value[0];
	else
		return -1;

	if (page < dbg_nand_stat_proc.start_page)
		return 0;

	page -= dbg_nand_stat_proc.start_page;

	if (page >= dbg_nand_stat_proc.page_num)
		return 0;

	if (*op == 'R')
		dbg_nand_stat_proc.page[page].read++
	else if (*op == 'W')
		dbg_nand_stat_proc.page[page].write++
	else {
		page /= dbg_nand_stat_proc.num_page_block;
		dbg_nand_stat_proc.block[page]++;
	}

	return 0;
}

#endif /* CONFIG_HINFC300_DBG_NAND_STAT_PROC_FILE */
