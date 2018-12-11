#include <linux/proc_fs.h>

#include "hieth.h"
#include "proc.h"

static int hieth_tx_info_read(struct seq_file *m, void *v)
{
#ifdef HIETH_TSO_DEBUG
	{
		int i;
		int id_now;

		if (id_send == 0)
			id_now = MAX_RECORD-1;
		else
			id_now = id_send-1;

		for (i = id_now; i >= 0; i--) {
			seq_printf(m, "%x,%x,%x\n",
				   pkt_rec[i].reg_addr,
				   pkt_rec[i].reg_pkt_info,
				   pkt_rec[i].status);
		}
		for (i = MAX_RECORD-1; i > id_now; i--) {
			seq_printf(m, "%x,%x,%x\n",
				   pkt_rec[i].reg_addr,
				   pkt_rec[i].reg_pkt_info,
				   pkt_rec[i].status);
		}
	}
#endif
	return 0;
}

static struct proc_dir_entry *hieth_proc_root;

#define proc_open(name) \
static int proc_open_##name(struct inode *inode, struct file *file) \
{ \
	return single_open(file, name, PDE_DATA(inode)); \
} \

proc_open(hieth_tx_info_read);

static struct proc_file {
	char *name;
	const struct file_operations ops;
} proc_file[] = {
	{
		.name = "tx_info",
		.ops = {
			.open           = proc_open_hieth_tx_info_read,
			.read           = seq_read,
			.llseek         = seq_lseek,
			.release        = single_release,
		},
	},
};

/* /proc/hieth/
 *      |---tx_info
 */
void hieth_proc_create(void)
{
	int i;

	hieth_proc_root = proc_mkdir("hieth", NULL);
	if (!hieth_proc_root)
		return;

	for (i = 0; i < ARRAY_SIZE(proc_file); i++) {
		struct proc_dir_entry *entry;

		entry = proc_create(proc_file[i].name, 0, hieth_proc_root,
				    &proc_file[i].ops);
		if (!entry)
			pr_err("failed to create %s\n", proc_file[i].name);
	}
}

void hieth_proc_destroy(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(proc_file); i++)
		remove_proc_entry(proc_file[i].name, hieth_proc_root);

	remove_proc_entry("hieth", NULL);
}
