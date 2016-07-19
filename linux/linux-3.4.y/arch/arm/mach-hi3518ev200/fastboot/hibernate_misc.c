#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/nsproxy.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/rwlock.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/err.h>
#include <linux/seq_file.h>
#include <linux/fs_struct.h>
#include <linux/proc_fs.h>
#include <linux/mount.h>
#include <linux/mnt_namespace.h>
#include <asm/uaccess.h>

static DEFINE_MUTEX(file_mutex);

struct sb_files_mount {
	struct seq_file m;
	struct path path;
	struct vfsmount *mnt;
};

extern int kern_path(const char *, unsigned int, struct path *);
extern void path_put(struct path *);
extern char *get_task_comm(char *, struct task_struct *);

static char mnt_path[64] = {0,};

static inline char *__dpath_of_filp(struct file *filp, char *buf, int len)
{
	struct path mnt_path;
	mnt_path.dentry = filp->f_dentry;
	mnt_path.mnt = filp->f_vfsmnt;
	return d_path(&mnt_path, buf, len);
}

static void *sb_files_seq_start(struct seq_file *m, loff_t *pos)
{
	struct sb_files_mount *priv = m->private;
	mutex_lock(&file_mutex);
	return seq_list_start(&priv->mnt->mnt_sb->s_files, *pos);
}

static void *m_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct sb_files_mount *priv = m->private;
	return seq_list_next(v, &priv->mnt->mnt_sb->s_files, pos);
}

static void m_stop(struct seq_file *m, void *v)
{
	mutex_unlock(&file_mutex);
}

static int sb_files_seq_show(struct seq_file *m, void *v)
{
	struct file *filp = list_entry(v, struct file, f_u.fu_list);
	char buf[128], *path;
	unsigned int acc;
	struct vm_area_struct *vma;
	struct address_space *mapping;
	struct prio_tree_iter iter;
	int mmap_count = 0;

	path = __dpath_of_filp(filp, buf, sizeof(buf));
	acc = filp->f_flags & O_ACCMODE;
	mapping = filp->f_mapping;

	vma_prio_tree_foreach(vma, &iter, &mapping->i_mmap, 0, ULONG_MAX) {
		mmap_count++;
	}

	seq_printf(m, "[%s]\n", path);
	seq_printf(m, "\topen count: %ld\n", file_count(filp));
	seq_printf(m, "\topen flags: %s%s%s%s\n",
			(filp->f_mode | filp->f_flags) & FMODE_EXEC ?
			"EXEC " : "   ",
			filp->f_flags & O_DIRECTORY ? "DIR " : "  ",
			acc == O_RDWR ? "RDWR " :
				(acc == O_RDONLY ? "RD   " :
					(acc == O_WRONLY ? "  WR " : "   ")),
			mmap_count ? "MMAP" : "");


#if 0
	/* Useless. the superblock has no process information. */
	{
	struct task_struct *tsk;
	char comm_buf[TASK_COMM_LEN];

	seq_printf(m, "\topen processes(%p):\n\t\t", filp->f_owner.pid);
	do_each_pid_task(filp->f_owner.pid, PIDTYPE_PID, tsk) {
		seq_printf(m, "[%s] ", get_task_comm(comm_buf, tsk));
	} while_each_pid_task(filp->f_owner, PIDTYPE_PID, tsk);
	do_each_pid_task(filp->f_owner.pid, PIDTYPE_PGID, tsk) {
		seq_printf(m, "{%s} ", get_task_comm(comm_buf, tsk));
	} while_each_pid_task(filp->f_owner, PIDTYPE_PGID, tsk);
	do_each_pid_task(filp->f_owner.pid, PIDTYPE_SID, tsk) {
		seq_printf(m, "(%s) ", get_task_comm(comm_buf, tsk));
	} while_each_pid_task(filp->f_owner, PIDTYPE_SID, tsk);
	seq_printf(m, "\n");
	}
#endif

	return 0;
}

static const struct seq_operations sb_files_seq_ops = {
	.start	= sb_files_seq_start,
	.next	= m_next,
	.stop	= m_stop,
	.show	= sb_files_seq_show,
};

static int sb_files_open(struct inode *inode, struct file *file)
{
	struct sb_files_mount *priv;
	int ret = -1;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	if (mnt_path[0] != 0)
		ret = kern_path(mnt_path, 0, &priv->path);

	if (ret)
		ret = kern_path("/", 0, &priv->path);

	priv->mnt = collect_mounts(&priv->path);

	file->private_data = &priv->m;
	ret = seq_open(file, &sb_files_seq_ops);
	if (ret) {
		path_put(&priv->path);
		drop_collected_mounts(priv->mnt);
		kfree(priv);
		return ret;
	}

	priv->m.private = priv;
	return 0;
}

static int sb_files_release(struct inode *inode, struct file *file)
{
	struct sb_files_mount *priv = file->private_data;
	if (priv) {
		path_put(&priv->path);
		drop_collected_mounts(priv->mnt);
		kfree(priv);
	}
	return 0;
}

static ssize_t sb_files_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	size_t c = sizeof(mnt_path)-1 < count ? sizeof(mnt_path)-1 : count;
	int ret;
	ret = copy_from_user(mnt_path, buf, c);
	if (unlikely(ret < 0))
		return -EFAULT;
	mnt_path[c] = 0;
	if (c > 1) {
		if (mnt_path[c-1] == '\n')
			mnt_path[c-1] = '\0';
	}
	*ppos += c;
	return c;
}

static const struct file_operations sb_files_proc_fops = {
	.open           = sb_files_open,
	.release        = sb_files_release,
	.read           = seq_read,
	.llseek         = seq_lseek,

	/* FIXME */
	.write		= sb_files_write,
};

int __init fastboot_hibernation_misc_init(void)
{
	proc_create("dump_sb_files", 0444, NULL, &sb_files_proc_fops);
	return 0;
}

#ifdef MODULE
void __exit fastboot_hibernation_misc_exit(void)
{
	remove_proc_entry("dump_sb_files", NULL);
}
#endif
