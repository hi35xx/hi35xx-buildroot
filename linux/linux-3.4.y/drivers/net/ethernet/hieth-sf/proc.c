#include <linux/proc_fs.h>

#include "hieth.h"
#include "ctrl.h"
#include "proc.h"

static int hieth_tx_info_read(char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	int len = 0;

	if (off)
		return 0;

#ifdef HIETH_TSO_DEBUG
	{
		int i;
		int id_now;

		if (id_send == 0)
			id_now = MAX_RECORD-1;
		else
			id_now = id_send-1;

		for (i = id_now; i >= 0; i--) {
			len += snprintf(&page[len], PAGE_SIZE - len,
					"%x,%x,%x\n",
					pkt_rec[i].reg_addr,
					pkt_rec[i].reg_pkt_info,
					pkt_rec[i].status);
		}
		for (i = MAX_RECORD-1; i > id_now; i--) {
			len += snprintf(&page[len], PAGE_SIZE - len,
					"%x,%x,%x\n",
					pkt_rec[i].reg_addr,
					pkt_rec[i].reg_pkt_info,
					pkt_rec[i].status);
		}
	}
#endif
	return len;
}

static struct proc_dir_entry *hieth_proc_root;

static struct proc_file {
	char *name;
	read_proc_t *read;
	write_proc_t *write;
} proc_file[] = {
	{
		.name = "tx_info",
		.read = hieth_tx_info_read,
	},
};

/* /proc/hieth/
 *      |---tx_info
 */
void hieth_proc_create(void)
{
	struct proc_dir_entry *entry;
	int i;

	hieth_proc_root = proc_mkdir("hieth", NULL);
	if (!hieth_proc_root)
		return;

	for (i = 0; i < ARRAY_SIZE(proc_file); i++) {
		entry = create_proc_entry(proc_file[i].name, 0,
				hieth_proc_root);
		if (entry) {
			entry->read_proc = proc_file[i].read;
			entry->write_proc = proc_file[i].write;
			entry->data = NULL;
		} else
			pr_err("Cann't create proc file:%s!\n",
					proc_file[i].name);
	}
}

void hieth_proc_destroy(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(proc_file); i++)
		remove_proc_entry(proc_file[i].name, hieth_proc_root);

	remove_proc_entry("hieth", NULL);
}

