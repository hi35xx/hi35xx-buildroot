/*****************************************************************************
 *
 *  Copyright (C) 2011 Emutex Ltd. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms and conditions of the GNU General Public License,
 *  version 2, as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Authors: Mark Burkley  <mark@emutex.com>
 *           Kai Ji        <kai@emutex.com>
 *           Dan O'Donovan <dan@emutex.com>
 *
 ******************************************************************************/

#include <linux/proc_fs.h>
#include <linux/tcp.h>
#include <net/sock.h>
#include <net/inet_sock.h>

#include "higmacv300.h"

#define GMAC_PARENT       "higmacv300"
#define GMAC_PROC  "higmacv300_proc"

static struct proc_dir_entry *proc_gmac_dir;

static struct timer_list gmac_rate_timer;

static void gmac_rate_timer_handler(unsigned long x)
{
	mod_timer(&gmac_rate_timer, jiffies + HZ);
}


/* proc interface setup */
static void *gmac_seq_start(struct seq_file *s, loff_t * pos)
{
	/*   counter is used to tracking multi proc interfaces
	 *  We have only one interface so return zero
	 *  pointer to start the sequence.
	 */
	static unsigned long counter;
	if (*pos == 0) {
		return &counter;
	} else {
		*pos = 0;
		return NULL;
	}
}

/* proc interface next */
static void *gmac_seq_next(struct seq_file *s, void *v, loff_t * pos)
{
	(*pos)++;

	return NULL;
}

static int gmac_bypass_seq_show(struct seq_file *s, void *v)
{
	stmmac_proc(s);

	return 0;
}

/* proc interface stop */
static void gmac_seq_stop(struct seq_file *s, void *v)
{
}


/* proc interface operation */
static const struct seq_operations gmac_seq_ops = {
	.start = gmac_seq_start,
	.next = gmac_seq_next,
	.stop = gmac_seq_stop,
	.show = gmac_bypass_seq_show
};


/* proc file open*/
static int gmac_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &gmac_seq_ops);
};


static const struct file_operations gmac_proc_ops = {
	.owner = THIS_MODULE,
	.open = gmac_proc_open,
	.read = seq_read
};

int gmac_proc_init(void)
{
	struct proc_dir_entry *proc_bypass_entry;

	proc_gmac_dir = proc_mkdir(GMAC_PARENT, NULL);
	if (!proc_gmac_dir) {
		pr_err("%s: failed to create proc file %s\n",
				__func__, GMAC_PARENT);
		return 1;
	}

	proc_bypass_entry = create_proc_entry(GMAC_PROC, 0, proc_gmac_dir);

	if (!proc_bypass_entry) {
		pr_err("%s: failed to create proc file\n", __func__);
		return 1;
	}
	proc_bypass_entry->proc_fops = &gmac_proc_ops;

	setup_timer(&gmac_rate_timer, gmac_rate_timer_handler, 0);
	mod_timer(&gmac_rate_timer, jiffies + HZ);

	return 0;

}

int gmac_proc_shutdown(void)
{
	del_timer(&gmac_rate_timer);

	remove_proc_entry(GMAC_PROC, proc_gmac_dir);
	remove_proc_entry(GMAC_PARENT, NULL);
	proc_gmac_dir = NULL;

	return 0;
}
