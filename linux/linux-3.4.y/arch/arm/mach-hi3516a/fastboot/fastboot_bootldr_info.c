#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

/* error message prefix */
#define ERRP "bootinfo: "

/* debug macro */
#if 0
#define dbg(x) do { printk("BOOTINFO-CMDLINE-PART: "); printk x; } while(0)
#else
#define dbg(x)
#endif

struct boot_partition {
	char *name;
	uint64_t size;
	uint64_t offset;
};

struct cmdline_boot_partition {
	int num_parts;
	struct boot_partition *parts;
};

static char *bootpart_cmdline;

static struct cmdline_boot_partition *boot_partitions;

int __init fastboot_find_bootpartition(const char *name,
		unsigned long long *offset, unsigned long long *size)
{
	struct boot_partition *part;
	int i;

	if (!boot_partitions) {
		pr_err("%s: could not found partinfo\n", __func__);
		return -EFAULT;
	}

	for (i = 0; i < boot_partitions->num_parts; i++) {
		part = boot_partitions->parts + i;
		if (strcmp(name, part->name) == 0) {
			*offset = part->offset;
			*size = part->size;
			return 0;
		}
	}

	pr_err("%s: could not found %s\n", __func__, name);
	return -ENOENT;
}

/* strongly based on drivers/mtd/cmdlinepart.c */
static struct boot_partition __init *newbootpart(char *s,
		char **retptr,
		int *num_parts,
		int this_part,
		unsigned char **extra_mem_ptr,
		int extra_mem_size)
{
	struct boot_partition *parts;
	unsigned long long size;
	unsigned long long offset = 0;
	char *name;
	int name_len;
	unsigned char *extra_mem;
	char delim;

	size = memparse(s, &s);

	/* check for offset */
	if (*s == '@') {
		s++;
		offset = memparse(s, &s);
	}
	/* now look for name */
	delim = 0;
	if (*s == '(')
		delim = ')';

	if (delim) {
		char *p;
		name = ++s;
		p = strchr(name, delim);
		if (!p) {
			pr_err(ERRP "no closing %c found\n", delim);
			return NULL;
		}
		name_len = p - name;
		s = p + 1;
	} else {
		name = NULL;
		name_len = 13; /* Partition_000 */
	}

	/* record name length for memory allocation later */
	extra_mem_size += name_len + 1;

	/* test if more partitions are following */
	if (*s == ',') {
		/* more partitions follow, parse them */
		parts = newbootpart(s + 1, &s, num_parts, this_part + 1,
				&extra_mem, extra_mem_size);
		if (!parts)
			return NULL;
	} else {
		/* this is the last partition: allocate space for all */
		int alloc_size;

		*num_parts = this_part + 1;
		alloc_size = *num_parts * sizeof(struct boot_partition)
				+ extra_mem_size;
		parts = kzalloc(alloc_size, GFP_KERNEL);
		if (!parts) {
			pr_err(ERRP "out of memory\n");
			return NULL;
		}
		extra_mem = (unsigned char *)(parts + *num_parts);
	}

	/* enter this partition (offset will be calculated later if it is zero at this point) */
	parts[this_part].size = size;
	parts[this_part].offset = offset;
	if (name)
		strlcpy(extra_mem, name, name_len + 1);
	else
		sprintf(extra_mem, "Partition_%03d", this_part);
	parts[this_part].name = extra_mem;
	extra_mem += name_len + 1;

	dbg(("partition %d: name <%s>, offset %llx, size %llx\n",
		this_part,
		parts[this_part].name,
		parts[this_part].offset,
		parts[this_part].size));

	/* return (updated) pointer to extra_mem memory */
	if (extra_mem_ptr)
		*extra_mem_ptr = extra_mem;

	/* return (updated) pointer command line string */
	*retptr = s;

	/* return partition table */
	return parts;
}

/* strongly based on drivers/mtd/cmdlinepart.c */
static int __init bootpart_cmdline_parse(char *s)
{
	struct cmdline_boot_partition *this_bootpart = NULL;
	struct boot_partition *parts = NULL;
	int num_parts;

	if (!s)
		return 0;

	parts = newbootpart(s,		/* cmdline */
			&s,		/* out: updated cmdline ptr */
			&num_parts,	/* out: number of parts */
			0,		/* first partition */
			(unsigned char**)&this_bootpart, /* out: extra mem */
			sizeof(*this_bootpart) + sizeof(void*)-1 /*alignment*/);
	if(!parts) {
		/*
		 * An error occurred. We're either:
		 * a) out of memory, or
		 * b) in the middle of the partition spec
		 * Either way, this part is hosed and we're
		 * unlikely to succeed in parsing any more
		 */
		 return 0;
	 }

	/* align this_bootpart */
	this_bootpart = (struct cmdline_boot_partition *)
			ALIGN((unsigned long)this_bootpart, sizeof(void*));
	/* enter results */
	this_bootpart->parts = parts;
	this_bootpart->num_parts = num_parts;

	/* does another spec follow? */
	if (*s != '\0')
		pr_err(ERRP "bad character after partition (%c)\n", *s);

	boot_partitions = this_bootpart;

	return 0;
}

static void *bootpartinfo_seq_start(struct seq_file *file, loff_t *index)
{
	if (!boot_partitions)
		return NULL;
	if (*index >= boot_partitions->num_parts)
		return NULL;
	return boot_partitions->parts + *index;
}

static void *bootpartinfo_seq_next(struct seq_file *file, void *data,
		loff_t *index)
{
	if (data == NULL)
		return NULL;
	if (!boot_partitions)
		return NULL;
	(*index)++;
	if (*index >= boot_partitions->num_parts)
		return NULL;
	return boot_partitions->parts + *index;
}

static void bootpartinfo_seq_stop(struct seq_file *file, void *data)
{
}

static int bootpartinfo_seq_show(struct seq_file *file, void *data)
{
	struct boot_partition *bpart = data;
	return seq_printf(file, "0x%016llx@0x%016llx: %s\n",
			bpart->size, bpart->offset, bpart->name);
}

static const struct seq_operations bootpartinfo_seq_ops = {
	.start = bootpartinfo_seq_start,
	.next = bootpartinfo_seq_next,
	.stop = bootpartinfo_seq_stop,
	.show = bootpartinfo_seq_show,
};

static int bootpartinfo_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &bootpartinfo_seq_ops);
}

static const struct file_operations bootpartinfo_proc_fops = {
	.open = bootpartinfo_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int __init fastboot_bootloader_info_init(void)
{
	bootpart_cmdline_parse(bootpart_cmdline);
	proc_create("bootpartinfo", 0444, NULL, &bootpartinfo_proc_fops);
	return 0;
}
module_init(fastboot_bootloader_info_init);

static int __init bootpart_setup(char *s)
{
	bootpart_cmdline = s;
	return 1;
}
__setup("bootparts=", bootpart_setup);
