/******************************************************************************
*    NAND Flash Controller V504 Device Driver
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
* ***
*    written by CaiZhiYong. 2010-11-04
*
******************************************************************************/

#  include <linux/proc_fs.h>
#  include <linux/module.h>
#  include <linux/moduleparam.h>
#  include <linux/vmalloc.h>

#include "hinfc504_os.h"
#include "hinfc504.h"

#if defined(CONFIG_HINFC504_DBG_NAND_PROC_FILE) \
	|| defined(CONFIG_HINFC504_DBG_NAND_EC_NOTICE)
#  define DBG_NAND_CORRECT_ERROR
#endif /* defined(CONFIG_HINFC504_DBG_NAND_PROC_FILE)
	  || defined(CONFIG_HINFC504_DBG_NAND_EC_NOTICE) */

#define CMD_WORD_OFFSET             "offset="
#define CMD_WORD_LENGTH             "length="
#define CMD_WORD_CLEAN              "clear"

/*****************************************************************************/
#ifdef DBG_NAND_CORRECT_ERROR

void dbg_nand_ec_init(void)
{
#  ifdef CONFIG_HINFC504_DBG_NAND_PROC_FILE
	printk(KERN_DEBUG "NAND debug: DBG_NAND_CORRECT_ERROR\n");
#  endif /* defined(CONFIG_HINFC504_DBG_NAND_PROC_FILE) */

#  ifdef CONFIG_HINFC504_DBG_NAND_EC_NOTICE
	printk(KERN_DEBUG "NAND debug: CONFIG_HINFC504_DBG_NAND_EC_NOTICE\n");
#  endif /* CONFIG_HINFC504_DBG_NAND_EC_NOTICE */
}
/*****************************************************************************/

void dbg_nand_ec_notice(struct hinfc_host *host)
{
	int nBits = 0;

	if (host->ecctype == et_ecc_none || !host->uc_er)
		return;

	if (!host->is_randomizer) {
		unsigned long  val;
		unsigned long *oob;

		oob = (unsigned long *)(host->buffer + host->pagesize);
		val = (oob[0] & oob[1] & oob[2] & oob[3]
			& oob[4] & oob[5] & oob[6] & oob[7]);
		if (val == 0xFFFFFFFF)
			host->uc_er = 2;
#ifdef CONFIG_HINFC504_DBG_NAND_EC_NOTICE
		else {
			int ix = 0;
			int jx = 0;
			for (ix = 0; ix < 8; ix++) {
				unsigned long val = oob[ix];
				for (jx = 0; jx < 32 && val; jx++) {
					if (!(val & 0x01))
						nBits++;
					val = (val >> 1);
				}
				nBits += (32 - jx);
			}
		}
#endif /* CONFIG_HINFC504_DBG_NAND_EC_NOTICE */
	}

#ifdef CONFIG_HINFC504_DBG_NAND_EC_NOTICE
	if (host->uc_er == 1) {
		if (!host->is_randomizer)
			printk(KERN_INFO "NAND uc ecc, "
				"page:0x%08X, (%d/256 zero)\n",
				GET_PAGE_INDEX(host), nBits);
		else
			printk(KERN_INFO "NAND uc ecc, "
				"page:0x%08X, BBM:0x%02X, EBM:0x%04X\n",
				GET_PAGE_INDEX(host), *host->bbm, *host->epm);
	}
#endif /* CONFIG_HINFC504_DBG_NAND_EC_NOTICE */

}
#endif /* DBG_NAND_CORRECT_ERROR */
/*****************************************************************************/

/*****************************************************************************
*  create file "/proc/nand_logs" show the last 100 operation
******************************************************************************/
#if defined(CONFIG_HINFC504_DBG_NAND_PROC_FILE)
#  ifndef CONFIG_HINFC504_DBG_NAND_NUM_OF_LOGS
#    define CONFIG_HINFC504_DBG_NAND_NUM_OF_LOGS      (80)
#  endif /* CONFIG_HINFC504_DBG_NAND_NUM_OF_LOGS */

#  ifndef CONFIG_HINFC504_DBG_NAND_LOG_LENGTH
#    define CONFIG_HINFC504_DBG_NAND_LOG_LENGTH       (40)
#  endif /* CONFIG_HINFC504_DBG_NAND_LOG_LENGTH */

#  ifndef CONFIG_HINFC504_DBG_NAND_DATA_LENGTH
#    define CONFIG_HINFC504_DBG_NAND_DATA_LENGTH      (8)
#  endif /* CONFIG_HINFC504_DBG_NAND_DATA_LENGTH */

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

	unsigned char  data[CONFIG_HINFC504_DBG_NAND_LOG_LENGTH];
};

struct dbg_nand_logs_root_t {

	struct proc_dir_entry *entry;
	int is_init;
	int index;
	int num_of_logs;

	int offset;
	int length;

	struct dbg_nand_logs_t logs[CONFIG_HINFC504_DBG_NAND_NUM_OF_LOGS];

	int read_index;
};

static struct dbg_nand_logs_root_t dbg_nand_logs_root = {
	.length = CONFIG_HINFC504_DBG_NAND_DATA_LENGTH,
};
/*****************************************************************************/
/*
 * echo "offset=2048 length=8" > /proc/nand_logs
 */
static int dbg_nand_write_proc(struct file *file, const char *buf,
	unsigned long count, void *data)
{
	char *p;
	int value;
	unsigned long pos = 0;
	struct dbg_nand_logs_root_t *nand_logs_root
		= (struct dbg_nand_logs_root_t *)data;

	while (pos < count) {
		while (pos < count
			&& (buf[pos] == ' '
				|| buf[pos] == ',' || buf[pos] == ';'))
			pos++;

		if (pos >= count)
			break;

		switch (buf[pos]) {
		case 'o':
			if (!memcmp(&buf[pos], CMD_WORD_OFFSET,
				sizeof(CMD_WORD_OFFSET) - 1)) {

				pos += sizeof(CMD_WORD_OFFSET) - 1;
				p = (char *)(buf + pos);
				value = simple_strtoul(p, &p, 10);
				if (value < 0)
					value = 0;
				nand_logs_root->offset = value;
			}
			break;

		case 'l':
			if (!memcmp(&buf[pos], CMD_WORD_LENGTH,
				sizeof(CMD_WORD_LENGTH) - 1)) {

				pos += sizeof(CMD_WORD_LENGTH) - 1;
				p = (char *)(buf + pos);
				value = simple_strtoul(p, &p, 10);
				if (value < 0)
					value = 0;
				nand_logs_root->length = value;
			}
			break;
		}

		while (pos < count
			&& (buf[pos] != ' '
				&& buf[pos] != ',' && buf[pos] != ';'))
			pos++;
	}

	return count;
}
/*****************************************************************************/

static int dbg_nand_read_proc
(
	char *page,
	char **start,
	off_t off,
	int count,
	int *eof,
	void *data
)
{
	int read_index;
	char *ppage = page;
	struct dbg_nand_logs_t *logs;
	struct dbg_nand_logs_root_t *nand_logs_root
		= (struct dbg_nand_logs_root_t *)data;

	*(int *)start = 1;
	*eof = 1;

	if (off == 0) {

		if (dbg_nand_logs_root.num_of_logs
			< CONFIG_HINFC504_DBG_NAND_NUM_OF_LOGS)
			nand_logs_root->read_index = 0;
		else
			nand_logs_root->read_index
				= (dbg_nand_logs_root.index + 1);

		ppage += sprintf(ppage,
			"Print parameter: \"offset=%d length=%d\"\n",
			nand_logs_root->offset,
			nand_logs_root->length);

		ppage += sprintf(ppage,
			"  UTC Clock   op cylce  page-offset     data\n");

	} else if (nand_logs_root->read_index
		== dbg_nand_logs_root.index)
		return 0;

	for (read_index = nand_logs_root->read_index;
		(read_index != nand_logs_root->index);
		++read_index) {

		if (read_index >= CONFIG_HINFC504_DBG_NAND_NUM_OF_LOGS)
			read_index = 0;

		logs = &nand_logs_root->logs[read_index];

		if ((count - (ppage - page)) < (50 + logs->length * 3))
			break;

		ppage += sprintf(ppage,
			"%02d:%02d:%02d.%04d  %c  %-2u  0x%08lX-%04lX",
			logs->hour, logs->min,
			logs->sec, logs->msec, logs->op,
			logs->cycle, logs->page, logs->offset);

		if (logs->op == 'E')
			ppage += sprintf(ppage, "   ---");
		else {
			int ix;
			ppage += sprintf(ppage, " %c ", logs->error);

			for (ix = 0; ix < logs->length; ix++) {
				if ((ix % 16) == 15)
					ppage += sprintf(ppage,
						"%02X-", logs->data[ix]);
				else
					ppage += sprintf(ppage,
						"%02X ", logs->data[ix]);
			}
		}
		ppage += sprintf(ppage, "\n");
	}

	nand_logs_root->read_index = read_index;

	return ppage - page;
}
/*****************************************************************************/

int dbg_nand_proc_init(void)
{
	if (dbg_nand_logs_root.is_init++)
		return 0;

	dbg_nand_logs_root.entry = create_proc_entry("nand_logs",
		S_IFREG | S_IRUSR | S_IWUSR, NULL);

	if (!dbg_nand_logs_root.entry) {
		printk(KERN_ERR "Can't create proc file.\n");
		return -1;
	}

	dbg_nand_logs_root.entry->read_proc  = dbg_nand_read_proc;
	dbg_nand_logs_root.entry->write_proc = dbg_nand_write_proc;
	dbg_nand_logs_root.entry->data       = (void *)&dbg_nand_logs_root;

	printk(KERN_DEBUG "NAND debug: CONFIG_HINFC504_DBG_NAND_PROC_FILE\n");

	return 0;
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

int dbg_nand_proc_save_logs(struct hinfc_host *host, char *op)
{
	struct dbg_nand_logs_t *logs
		= &dbg_nand_logs_root.logs[dbg_nand_logs_root.index];

	dbg_nand_logs_root.num_of_logs++;

	do_gettime(&logs->hour, &logs->min, &logs->sec, &logs->msec);
	logs->error = ' ';

#  ifdef DBG_NAND_CORRECT_ERROR
	if (host->uc_er)
		logs->error = host->uc_er + '*' - 1;
#  endif /* DBG_NAND_CORRECT_ERROR */

	logs->op = *op;
	logs->cycle = host->addr_cycle;
	logs->length = dbg_nand_logs_root.length;

	if (*op != 'E') {

		unsigned long buflen = (host->pagesize + host->oobsize);

		logs->offset = (host->addr_value[0] & 0xFFFF);
		logs->page   = GET_PAGE_INDEX(host);

		if (!logs->offset)
			logs->offset = dbg_nand_logs_root.offset;

		if (logs->offset >= buflen)
			logs->offset = 0;

		if (logs->length > (buflen - logs->offset))
			logs->length = (buflen - logs->offset);

		if (logs->length > CONFIG_HINFC504_DBG_NAND_LOG_LENGTH)
			logs->length = CONFIG_HINFC504_DBG_NAND_LOG_LENGTH;

		memcpy(logs->data,
			(host->buffer + logs->offset), logs->length);

	} else {

		logs->offset = 0;
		logs->page   = host->addr_value[0];
		logs->length = 0;
	}

	if (++dbg_nand_logs_root.index
		>= CONFIG_HINFC504_DBG_NAND_NUM_OF_LOGS)
		dbg_nand_logs_root.index = 0;

	return 0;
}
/*****************************************************************************/
#endif /* defined(CONFIG_HINFC504_DBG_NAND_PROC_FILE) */

/*****************************************************************************
 *
 *  Create file /proc/nand_erase to statictics nand block erase operation
 *
 ******************************************************************************/

#ifdef CONFIG_HINFC504_DBG_NAND_PE_PROC_FILE

struct gdb_nand_pe_t {
	unsigned int count;
};

struct gdb_nand_pe_root_t {

	int index;
	int print_start_index;
	int print_end_index;

	struct proc_dir_entry *entry;

	unsigned int num_of_blocks;
	unsigned int page_per_block;

	struct gdb_nand_pe_t pe[1];
};

static struct gdb_nand_pe_root_t *gdb_nand_pe_root;

/*****************************************************************************/

static int dbg_nand_pe_read_proc
(
	char *page,
	char **start,
	off_t off,
	int count,
	int *eof,
	void *data
)
{
	int index;
	int value;
	char *ppage = page;
	struct gdb_nand_pe_t *pe;
	struct gdb_nand_pe_root_t *nand_pe_root
		= (struct gdb_nand_pe_root_t *)data;

	*(int *)start = 1;
	*eof = 1;

	if (off == 0) {
		nand_pe_root->index = nand_pe_root->print_start_index;

		ppage += sprintf(ppage,
			"Print parameter: \"offset=%d length=%d\"\n",
			nand_pe_root->print_start_index,
			(nand_pe_root->print_end_index
				- nand_pe_root->print_start_index));

		ppage += sprintf(ppage,
			"Block Index  ---------------- Erase count from system startup ----------------\n");

	}

	for (index = nand_pe_root->index;
		index < nand_pe_root->print_end_index
			&& ((ppage - page) < (count - 100));
		index += 8) {

		pe = &nand_pe_root->pe[index];
		ppage += sprintf(ppage,
				"%4d: %8u %8u %8u %8u %8u %8u %8u %8u\n",
				index, pe[0].count, pe[1].count, pe[2].count,
				pe[3].count, pe[4].count, pe[5].count,
				pe[6].count, pe[7].count);
	}

	nand_pe_root->index = index;
	value = ppage - page;

	return value;
}

/*****************************************************************************/
/*
 * echo "offset=48,length=78" > /proc/nand_erase
 * echo "clear" > /proc/nand_erase
 */
static int dbg_nand_pe_write_proc(struct file *file, const char *buf,
	unsigned long count, void *data)
{
	char *str;
	unsigned long value;
	unsigned long pos = 0;
	struct gdb_nand_pe_root_t *nand_pe_root
		= (struct gdb_nand_pe_root_t *)data;

	while (pos < count) {
		while (pos < count
			&& (buf[pos] == ' '
			|| buf[pos] == ',' || buf[pos] == ';'))
			pos++;

		if (pos >= count)
			break;

		switch (buf[pos]) {

		case 'o':
			if (!memcmp(&buf[pos], CMD_WORD_OFFSET,
				sizeof(CMD_WORD_OFFSET) - 1)) {

				pos += sizeof(CMD_WORD_OFFSET) - 1;
				str = (char *)(buf + pos);
				value = simple_strtoul(str, &str, 10);

				if (value < 0)
					value = 0;
				if (value >= nand_pe_root->num_of_blocks)
					value = 0;
				value &= ~7;

				nand_pe_root->print_start_index = value;

				break;
			}

		case 'l':
			if (!memcmp(&buf[pos], CMD_WORD_LENGTH,
				sizeof(CMD_WORD_LENGTH) - 1)) {

				pos += sizeof(CMD_WORD_LENGTH) - 1;
				str = (char *)(buf + pos);
				value = simple_strtoul(str, &str, 10);

				if (value < 0)
					value = nand_pe_root->num_of_blocks;

				value = ((value + 7) & ~7);

				nand_pe_root->print_end_index
					= nand_pe_root->print_start_index
					+ value;

				if (nand_pe_root->print_end_index
					> nand_pe_root->num_of_blocks)
					nand_pe_root->print_end_index
						= nand_pe_root->num_of_blocks;

				break;
			}
		case 'c':
			if (!memcmp(&buf[pos], CMD_WORD_CLEAN,
				sizeof(CMD_WORD_CLEAN) - 1)) {

				memset(nand_pe_root->pe, 0,
					nand_pe_root->num_of_blocks
					* sizeof(struct gdb_nand_pe_t));

				return count;
			}
		}

		while (pos < count
			&& (buf[pos] != ' ' && buf[pos] != ','
				&& buf[pos] != ';'))
			pos++;
	}

	return count;
}
/*****************************************************************************/

int dbg_nand_pe_proc_init(unsigned long long chipsize,
			  unsigned int blocksize,
			  unsigned int pagesize)
{
	unsigned int len;
	unsigned int num_of_block;

	if (gdb_nand_pe_root)
		return 0;

	chipsize = (chipsize >> 10);
	blocksize = (blocksize >> 10);
	pagesize = (pagesize >> 10);
	num_of_block = ((unsigned int)chipsize / blocksize);

	len = (num_of_block * sizeof(struct gdb_nand_pe_t))
		+ sizeof(struct gdb_nand_pe_root_t);
	gdb_nand_pe_root = vmalloc(len);

	if (!gdb_nand_pe_root) {
		printk(KERN_ERR "Can't allocate memory.\n");
		return -1;
	}

	memset(gdb_nand_pe_root, 0, len);

	gdb_nand_pe_root->num_of_blocks = num_of_block;
	gdb_nand_pe_root->page_per_block = (blocksize / pagesize);
	gdb_nand_pe_root->print_end_index = num_of_block;

	gdb_nand_pe_root->entry = create_proc_entry("nand_erase",
		S_IFREG | S_IRUSR | S_IWUSR, NULL);
	if (!gdb_nand_pe_root->entry) {
		printk(KERN_ERR "Can't create proc file.\n");
		vfree(gdb_nand_pe_root);
		return -1;
	}

	gdb_nand_pe_root->entry->read_proc  = dbg_nand_pe_read_proc;
	gdb_nand_pe_root->entry->write_proc = dbg_nand_pe_write_proc;
	gdb_nand_pe_root->entry->data       = (void *)gdb_nand_pe_root;

	printk(KERN_DEBUG "NAND: CONFIG_HINFC504_DBG_NAND_PE_PROC_FILE\n");

	return 0;
}
/*****************************************************************************/

int dbg_nand_pe_erase(struct hinfc_host *host)
{
	unsigned int block_index;

	if (!gdb_nand_pe_root) {
		printk(KERN_ERR "NAND erase statistics not initiation.\n");
		return 0;
	}

	block_index = (host->addr_value[0]
		/ gdb_nand_pe_root->page_per_block);

	if (block_index > gdb_nand_pe_root->num_of_blocks) {
		printk(KERN_ERR "Block out of range.\n");
		return 0;
	}

	gdb_nand_pe_root->pe[block_index].count++;

	return 0;
}

#endif /* CONFIG_HINFC504_DBG_NAND_PE_PROC_FILE */

