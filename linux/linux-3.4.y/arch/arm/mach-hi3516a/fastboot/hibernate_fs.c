/*
 * Todo
 *  - save & restore inotify kernel space watches
 *  - save & restore eventpolls
 *  - save & restore file locks
 *  - save & restore file mapping
 */

#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/kmod.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/pm.h>
#include <linux/freezer.h>
#include <linux/slab.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/freezer.h>
#include <linux/namei.h>
#include <linux/fs_struct.h>
#include <linux/mnt_namespace.h>
#include <linux/fsnotify_backend.h>
#include <linux/inotify.h>
#include <linux/backing-dev.h>
#include <linux/anon_inodes.h>
#include <linux/security.h>
#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/suspend.h>

#define pr_err_bug_if(condition, fmt, ...)				\
	do {								\
		if (unlikely(condition)) {				\
			printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__);	\
			BUG();						\
		}							\
	} while (0)

#define pr_err_if(condition, fmt, ...)					\
	do {								\
		if (unlikely(condition))				\
			printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__);	\
	} while (0)


struct saved_mountpoint {
	struct list_head node;
	char *dev_name;
	char *dir_name;
	char *type;
	char *data;
#define MNT_MOVED (MNT_INTERNAL	<< 1)
	unsigned long flags;
};

static LIST_HEAD(saved_mountpoints);

static char path_buf_a[PATH_MAX];
static char path_buf_b[PATH_MAX];

static inline char *__d_path_wrapper(struct path *path, bool b)
{
	if (!b)
		return d_path(path, &path_buf_a[0], sizeof(path_buf_a));
	else
		return d_path(path, &path_buf_b[0], sizeof(path_buf_b));
}

static inline char *d_path_wrapper(struct path *path)
{
	return __d_path_wrapper(path, 0);
}

static inline char *d_path_of_file(struct file *file)
{
	return d_path_wrapper(&file->f_path);
}

static inline char *d_path_of_mnt(struct vfsmount *mnt)
{
	struct path mntpath = { .dentry = mnt->mnt_root, .mnt = mnt };
	return d_path_wrapper(&mntpath);
}

static bool is_rw_bdev(struct vfsmount *mnt)
{
	struct super_block *sb;
	bool rw_bdev = false;

	if (!mnt || !mnt->mnt_sb)
		goto out;

	spin_lock(&sb_lock);
	sb = mnt->mnt_sb;

	if (!hlist_unhashed(&sb->s_instances) &&
	    MAJOR(sb->s_dev) != 0 &&
	    sb->s_bdi != &noop_backing_dev_info &&
	    !__mnt_is_readonly(mnt)) {
		rw_bdev = true;
	}
	spin_unlock(&sb_lock);

out:
	return rw_bdev;
}

extern spinlock_t vfsmount_lock;

/* self included */
static bool have_rw_bdev_parent(struct vfsmount *self)
{
	struct vfsmount *parent = self;
	bool have_rw_bdev = true;

	spin_lock(&vfsmount_lock);
repeat:
	have_rw_bdev = is_rw_bdev(parent);
	if (have_rw_bdev)
		goto out;

	if (!IS_ROOT(parent->mnt_mountpoint)) {
		parent = parent->mnt_parent;
		goto repeat;
	}
out:
	spin_unlock(&vfsmount_lock);
	return have_rw_bdev;
}

static inline struct mnt_namespace *get_init_mnt_ns(void)
{
	struct nsproxy *nsp;

	rcu_read_lock();
	nsp = task_nsproxy(&init_task);
	BUG_ON(!(nsp && nsp->mnt_ns));
	get_mnt_ns(nsp->mnt_ns);
	rcu_read_unlock();

	return nsp->mnt_ns;
}

static bool null_check(struct vfsmount *mnt)
{
	return true;
}

static int
iterate_mounts_safe_reverse(int (*f)(struct vfsmount *, void *),
			    bool (*check)(struct vfsmount *), void *arg)
{
	struct mnt_namespace *mnt_ns;
	struct vfsmount *mnt, *tmp;
	int error = 0;

	if (!check)
		check = null_check;

	mnt_ns = get_init_mnt_ns();
	list_for_each_entry_safe_reverse(mnt, tmp, &mnt_ns->list, mnt_list) {
		if (!check(mnt))
			continue;

		mntget(mnt);
		error = f(mnt, arg);
		mntput_no_expire(mnt);
		if (error)
			goto out;
	}

out:
	put_mnt_ns(mnt_ns);
	return error;
}

extern struct dentry *lookup_hash(struct nameidata *nd);

/* copied from do_rmdir */
static long do_kern_rmdir(const char *pathname)
{
	int error = 0;
	struct dentry *dentry;
	struct nameidata nd;

	error = path_lookup(pathname, LOOKUP_PARENT, &nd);
	if (error)
		return error;

	switch(nd.last_type) {
	case LAST_DOTDOT:
		error = -ENOTEMPTY;
		goto exit1;
	case LAST_DOT:
		error = -EINVAL;
		goto exit1;
	case LAST_ROOT:
		error = -EBUSY;
		goto exit1;
	}

	nd.flags &= ~LOOKUP_PARENT;

	mutex_lock_nested(&nd.path.dentry->d_inode->i_mutex, I_MUTEX_PARENT);
	dentry = lookup_hash(&nd);
	error = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto exit2;
	error = mnt_want_write(nd.path.mnt);
	if (error)
		goto exit3;
	error = security_path_rmdir(&nd.path, dentry);
	if (error)
		goto exit4;
	error = vfs_rmdir(nd.path.dentry->d_inode, dentry);
exit4:
	mnt_drop_write(nd.path.mnt);
exit3:
	dput(dentry);
exit2:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
exit1:
	path_put(&nd.path);
	return error;
}

/* move to /dev/, mostly writable and living on ram */
#define SAFE_ROOT "/dev/hib_safe/"

static void mount_saved_mountpoints(void)
{
	struct saved_mountpoint *saved, *tmp;
	bool move = false;

	list_for_each_entry_safe(saved, tmp, &saved_mountpoints, node) {
		int error;
		char *dir;
		char *dev;

		pr_info("PM:FS: restore mount [%s:%s]\n",
			saved->dir_name, saved->dev_name);
		pr_debug("PM:FS:  type   : %s\n", saved->type);
		pr_debug("PM:FS:  flags  : 0x%08lx\n", saved->flags);
		pr_debug("PM:FS:  data   : %s\n",
			 saved->data ? saved->data : "null");

		error = do_mount(saved->dev_name, saved->dir_name,
				 saved->type, saved->flags, saved->data);
		move = saved->flags & MS_MOVE;

		dir = move ? saved->dev_name : saved->dir_name;
		dev = move ? saved->dir_name : saved->dev_name;

		if (error) {
			pr_err("PM:FS: %s failed [%s:%s] (%d)\n",
			       move ? "move mount" : "mount",
			       dir, dev, error);
		} else {
			pr_debug("PM:FS: %s partition [%s:%s]\n",
				 move ? "moved" : "mounted", dir, dev);
		}

		if (move) {
			struct path moved;
			int error = kern_path(dev, 0, &moved);
			if (!error) {
				struct mnt_namespace *ns =
					get_init_mnt_ns();
				list_move_tail(&moved.mnt->mnt_list,
					       &ns->list);
				put_mnt_ns(ns);
				path_put(&moved);
			}
			do_kern_rmdir(saved->dev_name);
		}

		list_del(&saved->node);
		kfree(saved->dir_name);
		kfree(saved->dev_name);
		kfree(saved->type);
		if (saved->data)
			free_page((unsigned long)saved->data);
		kfree(saved);
	}
}

static inline int get_s_flags_from_mnt(struct vfsmount *mnt)
{
	int mnt_flags = mnt->mnt_flags;
	int s_flags = mnt->mnt_sb->s_flags;

	if (mnt_flags & MNT_RELATIME)
		s_flags &= ~MS_NOATIME;
	if (mnt_flags & MNT_NOSUID)
		s_flags |= MS_NOSUID;
	if (mnt_flags & MNT_NODEV)
		s_flags |=  MS_NODEV;
	if (mnt_flags & MNT_NOEXEC)
		s_flags |= MS_NOEXEC;
	if (mnt_flags & MNT_NOATIME)
		s_flags |= MS_NOATIME;
	if (mnt_flags & MNT_NODIRATIME)
		s_flags |=  MS_NODIRATIME;
	if (mnt_flags & MNT_READONLY)
		s_flags |= MS_RDONLY;
	if (!(mnt_flags & (MNT_RELATIME | MNT_NOATIME)))
		s_flags |= MS_STRICTATIME;
	if (mnt_flags & MNT_MOVED)
		s_flags |= MS_MOVE;

	return s_flags;
}

static int save_mountpoint(struct vfsmount *mnt, const char *dir_name)
{
	struct saved_mountpoint *save;
	const char *options;
	const char *dev_name;
	int error = -ENOMEM;

	/* when moved, need to get old path */
	if (mnt->mnt_flags & MNT_MOVED) {
		dev_name = d_path_of_mnt(mnt);
		mnt->mnt_flags &= ~MNT_MOVED;
	} else {
		dev_name = mnt->mnt_devname;
	}

	save = kzalloc(sizeof(*save), GFP_ATOMIC);
	if (!save)
		return -ENOMEM;

	pr_info("PM:FS: save partition [%s:%s]\n", dir_name, mnt->mnt_devname);
	pr_debug("PM:FS:  type   : %s\n", mnt->mnt_sb->s_type->name);
	pr_debug("PM:FS:  flags  : 0x%08lx\n", mnt->mnt_sb->s_flags);
	pr_debug("PM:FS:  data   : %s\n", mnt->mnt_sb->s_options);

	INIT_LIST_HEAD(&save->node);

	save->flags = get_s_flags_from_mnt(mnt);
	save->dir_name = kstrdup(dir_name, GFP_ATOMIC);
	if (!save->dir_name)
		goto free_save;
	save->dev_name = kstrdup(dev_name, GFP_ATOMIC);
	if (!save->dev_name)
		goto free_dirname;
	save->type = kstrdup(mnt->mnt_sb->s_type->name, GFP_ATOMIC);
	if (!save->type)
		goto free_devname;
	rcu_read_lock();
	options = rcu_dereference(mnt->mnt_sb->s_options);
	if (options && options[0]) {
		save->data = (char *)__get_free_page(GFP_ATOMIC);
		if (!save->data) {
			rcu_read_unlock();
			goto free_type;
		}
		memset(save->data, 0, PAGE_SIZE);
		strncpy(save->data, options, PAGE_SIZE);
	}
	rcu_read_unlock();

	list_add(&save->node, &saved_mountpoints);

	return 0;

free_type:
	kfree(save->type);
free_devname:
	kfree(save->dev_name);
free_dirname:
	kfree(save->dir_name);
free_save:
	kfree(save);
	return error;
}

static bool check_if_task_running(struct super_block *sb)
{
	struct task_struct *task;
	bool running = false;

	/* check if running process */
	read_lock(&tasklist_lock);
	for_each_process(task) {
		struct fs_struct *fs;
		task_lock(task);
		fs = task->fs;
		if (!fs) {
			goto next;
		}
		read_lock(&fs->lock);
		if (sb == fs->root.mnt->mnt_sb) {
			pr_debug("PM:FS: task [%s] is running on [%s] (root)\n",
				 &task->comm[0], d_path_wrapper(&fs->root));
			running = true;
		}
		if (sb == fs->pwd.mnt->mnt_sb) {
			pr_debug("PM:FS: task [%s] is running on [%s] (pwd)\n",
				 &task->comm[0], d_path_wrapper(&fs->pwd));
			running = true;
		}
		read_unlock(&fs->lock);
	next:
		task_unlock(task);
	}
	read_unlock(&tasklist_lock);

	return running;
}

static bool check_if_file_opening(struct super_block *sb)
{
	bool opening = false;

	file_list_lock();
	if (!list_empty(&sb->s_files)) {
#if defined(DEBUG)
		struct file *file;
		list_for_each_entry(file, &sb->s_files, f_u.fu_list) {
			pr_debug("PM:FS: file [%s] is still open\n",
				 d_path_of_file(file));
		}
#endif
		opening = true;
	}
	file_list_unlock();

	return opening;
}

#ifdef CONFIG_INOTIFY_USER

extern struct srcu_struct fsnotify_grp_srcu;
extern struct list_head fsnotify_groups;
extern const struct fsnotify_ops inotify_fsnotify_ops;

static bool check_if_inotify_user_watching(struct super_block *sb)
{
	struct fsnotify_group *group;
	bool watching = false;
	int idx;

	/* check if inotify_user */
	idx = srcu_read_lock(&fsnotify_grp_srcu);
	list_for_each_entry_rcu(group, &fsnotify_groups, group_list) {
		spin_lock(&group->mark_lock);
		if (group->ops == &inotify_fsnotify_ops) {
			struct fsnotify_mark_entry *entry;
			list_for_each_entry(entry,
					    &group->mark_entries, g_list) {
				struct inode *inode = entry->inode;
#if defined(DEBUG)
				struct dentry *dentry;
#endif
				if (sb != inode->i_sb)
					continue;
#if defined(DEBUG)
				dentry = d_find_alias(inode);
				pr_debug("PM:FS: inotify [%s] still watching\n",
					 dentry->d_name.name);
				dput(dentry);
#endif
				watching = true;
			}
		}
		spin_unlock(&group->mark_lock);
	}
	srcu_read_unlock(&fsnotify_grp_srcu, idx);

	return watching;
}
#else
static inline bool check_if_inotify_user_watching(struct super_block *sb)
{
	return false;
}
#endif

extern struct vfsmount *next_mnt(struct vfsmount *p, struct vfsmount *root);

/* copied from may_umount_tree */
static bool check_if_mnt_holding(struct vfsmount *mnt)
{
	bool holding = false;
	int actual_refs = 0;
	int minimum_refs = 0;
	struct vfsmount *p;

	spin_lock(&vfsmount_lock);
	for (p = mnt; p; p = next_mnt(p, mnt)) {
		actual_refs += atomic_read(&p->mnt_count);
		minimum_refs += 2;
	}
	spin_unlock(&vfsmount_lock);

	if (actual_refs > minimum_refs) {
		pr_debug("PM:FS: [%s] actual_refs:%d,minimum_refs:%d\n",
			 d_path_of_mnt(mnt), actual_refs, minimum_refs);
		holding = true;
	}

	return holding;
}

static bool check_if_unmountable(struct vfsmount *mnt)
{
	struct super_block *sb = mnt->mnt_sb;
	bool opening, running, watching, holding;
	char *p = d_path_of_mnt(mnt);

	if (!sb) {
		return false;
	}

	holding = check_if_mnt_holding(mnt);
	watching = check_if_inotify_user_watching(sb);
	opening = check_if_file_opening(sb);
	running = check_if_task_running(sb);

	pr_err_if(holding, "PM:FS: Still Holding [%s:%s]\n",
		  p, mnt->mnt_devname);
	pr_err_if(watching, "PM:FS: Still Watching [%s:%s]\n",
		  p, mnt->mnt_devname);
	pr_err_if(opening, "PM:FS: Still Opening [%s:%s]\n",
		  p, mnt->mnt_devname);
	pr_err_if(running, "PM:FS: Still Running [%s:%s]\n",
		  p, mnt->mnt_devname);

	return !holding && !opening && !running && !watching;
}

static inline void thaw_flusher(void)
{
	struct task_struct *task, *g;
	read_lock(&tasklist_lock);
	do_each_thread(g, task) {
		if (task->flags & PF_FLUSHER)
			thaw_process(task);
	} while_each_thread(g, task);
	read_unlock(&tasklist_lock);
}

static inline void freeze_flusher(void)
{
	struct task_struct *task, *g;
	read_lock(&tasklist_lock);
	do_each_thread(g, task) {
		if (task->flags & PF_FLUSHER)
			freeze_task(task, false);
	} while_each_thread(g, task);
	read_unlock(&tasklist_lock);
}

/* copied from sys_mkdirat */
static int kern_mkdirat(const char *name, int mode)
{
	struct dentry *dentry;
	struct nameidata nd;
	int error;

	error = path_lookup(name, LOOKUP_PARENT, &nd);
	if (error)
		goto out_err;

	dentry = lookup_create(&nd, 1);
	error = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out_unlock;

	error = mnt_want_write(nd.path.mnt);
	if (error)
		goto out_dput;
	error = security_path_mkdir(&nd.path, dentry, mode);
	if (error)
		goto out_drop_write;
	error = vfs_mkdir(nd.path.dentry->d_inode, dentry, mode);
out_drop_write:
	mnt_drop_write(nd.path.mnt);
out_dput:
	dput(dentry);
out_unlock:
	mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
	path_put(&nd.path);
out_err:
	return error;
}

static inline int mkdir_safe(void)
{
	int error = kern_mkdirat(SAFE_ROOT, 755);
	if (error != -EEXIST)
		return error;

	return 0;
}

static int mkdir_on_safe(const char *name, int mode, char **safe_dir)
{
	static char safe_path[PATH_MAX];
	int error;

	error = mkdir_safe();
	if (error)
		return error;

	snprintf(&safe_path[0], sizeof(safe_path), "%s/%s", SAFE_ROOT, name);

	error = kern_mkdirat(&safe_path[0], 755);
	if (error)
		return error;

	if (safe_dir)
		*safe_dir = &safe_path[0];

	return error;
}

static int do_move_mount_to_safe(struct vfsmount *mnt, char *from)
{
	char *to;
	char temp[9];
	int error;

	/* FIXME - means nothing */
	sprintf(&temp[0], "%p", mnt);
	error = mkdir_on_safe(temp, 755, &to);
	if (error)
		return error;

	error = do_mount(from, to, NULL, MS_MOVE, NULL);
	if (error)
		return error;

	/* MNT_MOVED set here to pick it up during resumption and cleared. */
	mnt->mnt_flags |= MNT_MOVED;
	return error;
}

/* FIXME - Is it enough ? */
static inline bool is_on_ram(struct vfsmount *mnt)
{
	return MAJOR(mnt->mnt_sb->s_dev) == 0 ? true : false;
}

struct umount_mnt {
	struct list_head node;
	struct vfsmount *mnt;
};

extern int do_umount(struct vfsmount *mnt, int flags);

static int unmount_rw_backing_dev(struct vfsmount *mnt, void *unused,
				  int is_on_ram)
{
	struct path path = { .dentry = mnt->mnt_root, .mnt = mnt };
	char *orig = __d_path_wrapper(&path, true /* b */);
	bool move = false;
	int error;

	/*
	 * need to move a mount, it is not neccessary to check_if_unmountable()
	 */
	if (is_on_ram) {
		move = true;
		error = do_move_mount_to_safe(mnt, orig);
		/* d_path_mnt() gives different path after here */
	} else {
		if (check_if_unmountable(mnt))
			error = do_umount(mnt, 0);
		else {
			pr_err("PM:FS: [%s:%s] is unmountable\n",
			       d_path_of_mnt(mnt), mnt->mnt_devname);
			error = -EBUSY;
		}
	}

	if (error) {
		pr_err("PM:FS: %s failed [%s:%s] (%d)\n",
		       move ? "move mount" : "unmount",
		       orig,
		       move ? d_path_of_mnt(mnt) : mnt->mnt_devname,
		       error);
	} else {
#if defined(DEBUG)
		pr_debug("PM:FS: %s partition [%s:%s]\n",
			 move ? "moved" : "unmounted",
			 orig,
			 move ? d_path_of_mnt(mnt) : mnt->mnt_devname);
#endif
		error = save_mountpoint(mnt, orig);
	}

	return error;
}

static int collect_mnt_on_rw_backing_devs(struct vfsmount *mnt, void *list)
{
	struct list_head *umount_list = list;
	struct umount_mnt *umount;

	umount = kmalloc(sizeof(*umount), GFP_KERNEL);
	if (unlikely(!umount))
		return -ENOMEM;

	INIT_LIST_HEAD(&umount->node);
	umount->mnt = mntget(mnt);

	list_add_tail(&umount->node, umount_list);
	return 0;
}

static int unmount_rw_backing_devs(void)
{
	LIST_HEAD(umount_list);
	struct umount_mnt *umount, *tmp;
	int error;

	error = iterate_mounts_safe_reverse(collect_mnt_on_rw_backing_devs,
					    have_rw_bdev_parent, &umount_list);

	thaw_flusher();

	list_for_each_entry_safe(umount, tmp, &umount_list, node) {
		struct vfsmount *mnt = umount->mnt;
		int is_ram = 0;

		list_del(&umount->node);
		kfree(umount);

		if (is_on_ram(mnt)) is_ram = 1;
		error = unmount_rw_backing_dev(mnt, NULL, is_ram);
		if (!error && !is_ram /* !-EBUSY */) {
			pr_err_bug_if(atomic_read(&mnt->mnt_count) != 1,
				      "PM:FS: "
				      "mount count should be 1 but %d\n",
				      atomic_read(&mnt->mnt_count));
		}
		/* mnt will be destroyed if no error */
		mntput_no_expire(mnt);
	}

	freeze_flusher();
	return error;
}

static LIST_HEAD(saved_task_fs_list);

struct saved_task_fs {
	struct list_head list;

	struct task_struct *task;
	char *root;
	char *pwd;
};

static struct saved_task_fs *alloc_saved_task_fs(struct task_struct *task,
						 bool move_root, bool move_pwd)
{
	struct fs_struct *fs = task->fs;
	int root_len = 0;
	int pwd_len = 0;
	int null_len = 0;
	struct saved_task_fs *saved;

	read_lock(&fs->lock);

	if (move_root) {
		root_len = strlen(d_path_wrapper(&fs->root));
		null_len++;
	}
	if (move_pwd) {
		pwd_len = strlen(d_path_wrapper(&fs->pwd));
		null_len++;
	}

	saved = kzalloc(sizeof(*saved) + root_len + pwd_len + null_len,
			GFP_ATOMIC);
	if (!saved)
		goto unlock;

	INIT_LIST_HEAD(&saved->list);
	saved->task = task;

	if (move_root) {
		saved->root = (char *)saved + sizeof(*saved);
		strcpy(saved->root, d_path_wrapper(&fs->root));
	}
	if (move_pwd) {
		if (saved->root)
			saved->pwd = saved->root + root_len + 1;
		else
			saved->pwd = (char *)saved + sizeof(*saved);
		strcpy(saved->pwd, d_path_wrapper(&fs->pwd));
	}

unlock:
	read_unlock(&fs->lock);
	return saved;
}

static int move_task_fs_to(struct task_struct *task,
			   const char *root, const char *pwd)
{
	struct path root_path, *old_root = NULL;
	struct path pwd_path;
	int error;

	if (root) {
		error = kern_path(root, LOOKUP_FOLLOW, &root_path);
		if (error) {
			pr_err("PM:FS: failed to get root path [%s]\n", root);
			return error;
		}

		/* to rollback root when moving pwd fails */
		old_root = &task->fs->root;
		path_get(old_root);

		set_fs_root(task->fs, &root_path);
		path_put(&root_path);
	}
	if (pwd) {
		error = kern_path(pwd, LOOKUP_FOLLOW, &pwd_path);
		if (error) {
			pr_err("PM:FS: failed to get pwd path [%s]\n", pwd);
			goto out;
		}

		set_fs_pwd(task->fs, &pwd_path);
		path_put(&pwd_path);
	}

out:
	if (old_root) {
		if (error)
			set_fs_root(task->fs, old_root);
		path_put(old_root);
	}

	return error;
}

static int move_task_fs_to_safe(struct task_struct *task,
				bool move_root, bool move_pwd)
{
	struct saved_task_fs *saved;
	char *root = NULL;
	char *pwd = NULL;
	int error;

	if (!move_root && !move_pwd)
		return 0;

	/* sleeping function called from invalid context */
	error = mkdir_safe();
	if (error)
		return error;

	error = -ENOMEM;
	saved = alloc_saved_task_fs(task, move_root, move_pwd);
	if (!saved)
		goto rm_safe;

	if (move_root) {
		root = SAFE_ROOT;
		pr_info("PM:FS: move task [%s] on root:%s to %s\n",
			&task->comm[0],
			d_path_wrapper(&task->fs->root), root);

	}
	if (move_pwd) {
		pwd = SAFE_ROOT;
		pr_info("PM:FS: move task [%s] on pwd:%s to %s\n",
			&task->comm[0],
			d_path_wrapper(&task->fs->pwd), pwd);
	}

	error = move_task_fs_to(task, root, pwd);
	if (error)
		goto free_saved;

	list_add(&saved->list, &saved_task_fs_list);
	return 0;

free_saved:
	kfree(saved);
rm_safe:
	do_kern_rmdir(SAFE_ROOT);
	return error;
}

static int save_task_fs_on_rw_backing_devs(void)
{
	struct task_struct *task;
	int error = 0;

	for_each_process(task) {
		struct fs_struct *fs;
		bool move_root = false;
		bool move_pwd = false;

		fs = task->fs;
		if (!fs)
			goto next;

		if (have_rw_bdev_parent(fs->root.mnt))
			move_root = true;
		if (have_rw_bdev_parent(fs->pwd.mnt))
			move_pwd = true;

		if (move_root || move_pwd)
			error = move_task_fs_to_safe(task, move_root, move_pwd);

	next:
		if (error)
			break;
	}

	return error;
}

static void restore_saved_task_fs(void)
{
	struct saved_task_fs *saved, *n;

	list_for_each_entry_safe(saved, n, &saved_task_fs_list, list) {
		int error;

		if (saved->root)
			pr_debug("PM:FS: restore task [%s] on root:%s\n",
				&saved->task->comm[0], saved->root);
		if (saved->pwd)
			pr_debug("PM:FS: restore task [%s] on pwd:%s\n",
				 &saved->task->comm[0], saved->pwd);

		error = move_task_fs_to(saved->task, saved->root, saved->pwd);
		if (unlikely(error)) {
			pr_err("PM:FS: failed to restore fs for task [%s] "
			       "root:%s pwd:%s (%d)\n",
			       &saved->task->comm[0],
			       saved->root ? saved->root : "null",
			       saved->pwd ? saved->pwd : "null",
			       error);
		}
		list_del(&saved->list);
		kfree(saved);
	}

	do_kern_rmdir(SAFE_ROOT);
}

/*
 * FIXME
 *  How can inode-like one be used instead of file_path ?
 *  in other words, use alternative inode structure and file_path as a dentry
 */
static LIST_HEAD(file_path_list);

struct file_path {
	struct list_head node; /* file_path_list */
	struct list_head fobj_list;

	char *path;

	//counter
};

/* one per struct file instance */
struct file_object {
	struct list_head same_fpath; /* fobj_list */
	struct list_head fdesc_list;

	struct file	*file; /* make it null when f_count is zero */

	atomic_long_t	count;
	unsigned int 	flags;
	fmode_t		mode;
	loff_t		pos;
	loff_t		size;
};

struct file_descriptor {
	struct list_head same_fobj; /* fdesc_list */

	int fd;
	struct task_struct *task;
};

static loff_t noentry_lseek(struct file *file, loff_t offset, int orig)
{
	pr_err("PM:FS: [%s] lseek(%lld, %d) on [%s]",
	       &current->comm[0], offset, orig, d_path_of_file(file));
	return -EBADF;
}

static ssize_t noentry_read(struct file *file,
			    char __user *buf, size_t count, loff_t *ppos)
{
	pr_err("PM:FS: [%s] read(%p, %d, %lld) on [%s]\n",
	       &current->comm[0], buf, count, *ppos, d_path_of_file(file));
	return -EBADF;
}

static ssize_t noentry_write(struct file *file,
			     const char __user *buf, size_t count,
			     loff_t *ppos)
{
	pr_err("PM:FS: [%s] write(%p, %d, %lld) on [%s]\n",
	       &current->comm[0], buf, count, *ppos, d_path_of_file(file));
	return -EBADF;
}

static int noentry_release(struct inode *inode, struct file *file)
{
	pr_err("PM:FS: [%s] close() on [%s]\n",
	       &current->group_leader->comm[0], d_path_of_file(file));
	return 0;
}

/*
 * FIXME
 *  - check whether more callbacks are necessary.
 */
static const struct file_operations noentry_fops = {
	.llseek		= noentry_lseek,
	.read		= noentry_read,
	.write		= noentry_write,
	.release	= noentry_release,
};

static inline bool is_noentry_file(const struct file *file)
{
	return file->f_op == &noentry_fops;
}

/*
 * NOTE
 *   It's not sufficient to get file path from anon_inodefs for debugging.
 *  It just gives us short "anon_inode:[kind]". We override anon_inodefs's
 *  d_dname() so we can get full file path pointed by noentry file.
 *  This means all d_op is overriden and could be a problem if anon_inodefs
 *  implements other dentry operations. But it does not at this time.
 */
/* copied from dynamic_dname but it uses bigger static buffer */
static char *__dynamic_dname(struct dentry *dentry,
			     char *buffer, int buflen, const char *fmt, ...)
{
	va_list args;
	int sz;

	va_start(args, fmt);
	sz = vsnprintf(path_buf_b, sizeof(path_buf_b), fmt, args) + 1;
	va_end(args);

	if (sz > sizeof(path_buf_b) || sz > buflen)
		return ERR_PTR(-ENAMETOOLONG);

	buffer += buflen - sz;
	return memcpy(buffer, path_buf_b, sz);
}

static char *noentry_dentry_dname(struct dentry *dentry,
				  char *buffer, int buflen)
{
	char *p =  __dynamic_dname(dentry, buffer, buflen, "noentry:%s",
				   dentry->d_name.name);
	if (IS_ERR(p))
		return "noentry:nofilenameavailble";
	else
		return p;
}

static const struct dentry_operations noentry_dentry_operations = {
	.d_dname	= noentry_dentry_dname,
};

static struct file *get_noentry_file(const char *name)
{
	struct file *file;

	file = anon_inode_getfile(name, &noentry_fops, NULL, 0);
	if (IS_ERR(file))
		goto out;

	/* override d_op to get full filename */
	file->f_dentry->d_op = &noentry_dentry_operations;

out:
	return file;
}

static inline bool is_task_fd_slot_empty(struct task_struct *task, unsigned fd)
{
	struct files_struct *files = task->files;
	struct fdtable *fdt = files_fdtable(files);
	bool is_empty = true;

	if (fd >= fdt->max_fds) {
		is_empty = false;
		goto out;
	}
	if (rcu_dereference_raw(fdt->fd[fd]) != NULL) {
		is_empty = false;
		goto out;
	}
	if (fd <= files->next_fd)
		files->next_fd = fd + 1;
out:
	return is_empty;
}

static void task_fd_install(struct task_struct *task,
			    unsigned int fd, int flags, struct file *file)
{
	struct files_struct *files = task->files;
	struct fdtable *fdt;

	spin_lock(&files->file_lock);

	fdt = files_fdtable(files);
	BUG_ON(!is_task_fd_slot_empty(task, fd));

	FD_SET(fd, fdt->open_fds);
	if (flags & O_CLOEXEC)
		FD_SET(fd, fdt->close_on_exec);
	else
		FD_CLR(fd, fdt->close_on_exec);
	rcu_assign_pointer(fdt->fd[fd], file);

	spin_unlock(&files->file_lock);
}

static off_t filp_lseek(struct file *file, off_t offset, unsigned int origin)
{
	off_t retval;

	if (!file)
		return -EBADF;

	retval = -EINVAL;
	if (origin <= SEEK_MAX) {
		loff_t res = vfs_llseek(file, offset, origin);
		retval = res;
		if (res != (loff_t)retval)
			retval = -EOVERFLOW;
	}

	return retval;
}

static inline char *get_file_flags_readable(unsigned int flags, fmode_t mode)
{
	static char open_flags[64];
	snprintf(&open_flags[0], sizeof(open_flags),
		 "%s%s%s",
		 (flags & O_ACCMODE) == O_RDWR ? "RDWR ":
		 ((flags & O_ACCMODE) == O_RDONLY ? "RD   ":
		  ((flags & O_ACCMODE) == O_WRONLY ? "WR ":"     ")),
		 (mode | flags) & FMODE_EXEC ? "EXEC ":"     ",
		 flags & O_DIRECTORY ? "DIR ":"");
	return &open_flags[0];
}

static void restore_file_descriptors(struct file_object *fobj)
{
	struct file_descriptor *fdesc, *tmp;

	list_for_each_entry_safe(fdesc, tmp, &fobj->fdesc_list, same_fobj) {
		/*
		 * Failed to open both of the original file and noentry file.
		 * leave the fd and fdslot empty so that the process's syscalls
		 * can fail with -EBADF.
		 */
		if (unlikely(!fobj->file)) {
			pr_err("PM:FS: "
			       "have no file to restore [%s(%d)][fd:%d]\n",
				&fdesc->task->comm[0], fdesc->task->pid,
			       fdesc->fd);
			goto free;
		}
		/* install fd and get file instance */
		task_fd_install(fdesc->task,
				fdesc->fd, fobj->flags, fobj->file);
		get_file(fobj->file);

		pr_info("PM:FS: restore fd [%s(%d)][fd:%d]\n",
			&fdesc->task->comm[0], fdesc->task->pid, fdesc->fd);

free:
		list_del(&fdesc->same_fobj);
		kfree(fdesc);
	}
}

static void restore_file_objects(struct file_path *fpath)
{
	const char *filename = fpath->path;
	struct file_object *fobj, *tmp;

	list_for_each_entry_safe(fobj, tmp, &fpath->fobj_list, same_fpath) {
		struct file *file;

		/* in case of error on suspension, file object is alive */
		if (unlikely(fobj->file))
			goto restore_fdesc;

		/* we must put the file after all fds are restored */
		file = filp_open(filename, fobj->flags, fobj->mode);
		if (unlikely(IS_ERR(file))) {
			pr_err("PM:FS: opening [%s] failed (%d)\n",
			       filename, (int)PTR_ERR(file));

			file = get_noentry_file(filename);
			if (IS_ERR(file)) {
				pr_err("PM:FS: getting noentry file failed "
				       "(%ld) not recoverable\n",
				       PTR_ERR(file));
				/*
				 * make it null so we can recognize it during
				 * fd restoration.
				 */
				file = NULL;
			} else {
				pr_err("PM:FS: "
				       "got noentry file instead of [%s]\n",
				       filename);
			}
		} else {
			/*
			 * sanity check - if oops, something's wroing, fix !
			 */
			pr_err_bug_if(fobj->flags != file->f_flags,
				      "PM:FS: mismatch file flags, should be "
				      "%x (but %x)\n",
				      fobj->flags, file->f_flags);
			pr_err_bug_if(fobj->mode != file->f_mode,
				      "PM:FS: mismatch file mode, should be "
				      "%x (but %x)\n",
				      fobj->mode, file->f_mode);

			/* size truncated ? */
			if (unlikely(fobj->pos >
				     i_size_read(file->f_dentry->d_inode))) {
				pr_err("PM:FS: file size (%lld) smaller than "
				       "offset (%lld)\n",
				       i_size_read(file->f_dentry->d_inode),
				       fobj->pos);
				/* set pos with inode's size */
				fobj->pos =
					i_size_read(file->f_dentry->d_inode);
			}

			/* seek offset */
			BUG_ON(filp_lseek(file, fobj->pos, 0) < 0);

			pr_debug("PM:FS: restore file object [%p:%s]\n",
				 file, filename);
			pr_debug("PM:FS:  open flags : %s\n",
				 get_file_flags_readable(fobj->flags,
							 fobj->mode));
			pr_debug("PM:FS:  open mode  : 0x%08x\n", fobj->mode);
			pr_debug("PM:FS:  offset     : %lld\n", fobj->pos);
			pr_debug("PM:FS:  size       : %lld\n",
				 i_size_read(file->f_dentry->d_inode));
		}
		fobj->file = file;

	restore_fdesc:
		/* task's file descriptors */
		restore_file_descriptors(fobj);

		/* original file or noentry file */
		if (likely(fobj->file)) {
			/*
			 * sanity check - if oops, something's wroing, fix !
			 * file_count can be 1 (in case someting's wrong), so
			 * check it before dropping reference.
			 */
			pr_err_bug_if(file_count(fobj->file) < 2,
				      "PM:FS: file count is %ld\n",
				      file_count(fobj->file));
			pr_err_bug_if((file_count(fobj->file) - 1) !=
				      atomic_long_read(&fobj->count),
				      "PM:FS: mismatch file count, it should "
				      "be %ld (but %ld)\n",
				      atomic_long_read(&fobj->count),
				      file_count(fobj->file) - 1);
		}

		/*
		 * we should put the file because additional count is held in
		 * filp_open().
		 */
		if (likely(fobj->file))
			fput(fobj->file);

		list_del(&fobj->same_fpath);
		kfree(fobj);
	}
}

static void open_saved_files(void)
{
	struct file_path *fpath, *tmp;

	list_for_each_entry_safe(fpath, tmp, &file_path_list, node) {
		pr_info("PM:FS: restore file [%s]", fpath->path);
		restore_file_objects(fpath);
		list_del(&fpath->node);
		kfree(fpath);
	}
}

static struct file_path *get_file_path(struct file *file)
{
	const char *filename = d_path_of_file(file);
	struct file_path *fpath;

	/* search first */
	list_for_each_entry(fpath, &file_path_list, node) {
		if (!strcmp(fpath->path, filename))
			return fpath;
	}

	/* new a file_path */
	fpath = kmalloc(sizeof(*fpath) + strlen(filename) + 1, GFP_ATOMIC);
	if (!fpath)
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(&fpath->node);
	INIT_LIST_HEAD(&fpath->fobj_list);
	fpath->path = (char *)fpath + sizeof(*fpath);
	strcpy(fpath->path, filename);

	list_add_tail(&fpath->node, &file_path_list);

	pr_info("PM:FS: save file [%s]\n", filename);
	return fpath;
}

static struct file_object *get_file_object(struct file *file)
{
	struct file_path *fpath;
	struct file_object *fobj;

	fpath = get_file_path(file);
	if (IS_ERR(fpath))
		return ERR_PTR(-ENOMEM);

	/* search first */
	list_for_each_entry(fobj, &fpath->fobj_list, same_fpath) {
		if (fobj->file && (fobj->file == file))
			return fobj;
	}

	/* new a file_object */
	fobj = kzalloc(sizeof(*fobj), GFP_ATOMIC);
	if (!fobj)
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(&fobj->same_fpath);
	INIT_LIST_HEAD(&fobj->fdesc_list);
	fobj->file = file;
	atomic_long_set(&fobj->count, file_count(file));
	fobj->flags = file->f_flags & ~(O_CREAT | O_TRUNC | O_EXCL);
	fobj->mode = file->f_mode;
	fobj->pos = file->f_pos;
	fobj->size = i_size_read(file->f_dentry->d_inode);

	list_add_tail(&fobj->same_fpath, &fpath->fobj_list);

	pr_debug("PM:FS: save file object [%p:%s]\n", file, fpath->path);
	pr_debug("PM:FS:  open count: %ld\n", file_count(file));
	pr_debug("PM:FS:  open flags: %s\n",
		 get_file_flags_readable(file->f_flags, file->f_mode));
	pr_debug("PM:FS:  open mode : 0x%08x\n", file->f_mode);
	pr_debug("PM:FS:  offset    : %lld\n", file->f_pos);
	pr_debug("PM:FS:  size      : %lld\n",
		 i_size_read(file->f_dentry->d_inode));

	/*
	 * grab additional 1 ref to prevent it from being destroyed in filp_cl-
	 * ose() which may sleep.
	 * file object will be destoryed at the end of close_files_on_rw_backi-
	 * ng_devs().
	 */
	get_file(fobj->file);

	return fobj;
}

static void __put_unused_fd(struct files_struct *files, unsigned int fd)
{
	struct fdtable *fdt = files_fdtable(files);
	__FD_CLR(fd, fdt->open_fds);
	if (fd < files->next_fd)
		files->next_fd = fd;
}

static int check_file_busy(struct task_struct *task, struct file *file)
{
	struct list_head *l;
	int nr_epolls = 0;
	struct inode *inode = file->f_path.dentry->d_inode;
	struct file_lock *flock;
	int nr_flocks = 0;
	struct vm_area_struct *vma;
	struct address_space *mapping;
	struct prio_tree_iter iter;
	int nr_mappings = 0;
	int busy = 0;

	/* FIXME - remove this and save & restore eventpolls */
	spin_lock(&file->f_lock);
	list_for_each(l, &file->f_ep_links)
		nr_epolls++;
	spin_unlock(&file->f_lock);

	/* lock_kernel(); */
	for (flock = inode->i_flock; flock != NULL; flock = flock->fl_next)
		nr_flocks++;
	/* unlock_kernel(); */

	mapping = file->f_mapping;
	spin_lock(&mapping->i_mmap_lock);
	vma_prio_tree_foreach(vma, &iter, &mapping->i_mmap, 0, ULONG_MAX) {
		nr_mappings++;
	}
	spin_unlock(&mapping->i_mmap_lock);

	if (unlikely(nr_epolls)) {
		busy = 1;
		pr_err("PM:FS: file [%s] (opened by [%s:%d]) has eventpolls "
		       "(%d)\n", d_path_of_file(file),
		       &task->comm[0], task->pid, nr_epolls);
	}
	if (unlikely(nr_flocks)) {
		busy = 1;
		pr_err("PM:FS: file [%s] (opened by [%s:%d]) has locks (%d)\n",
		       d_path_of_file(file),
		       &task->comm[0], task->pid, nr_flocks);
	}
	if (unlikely(nr_mappings)) {
		busy = 1;
		pr_err("PM:FS: "
		       "file [%s] (opened by [%s:%d]) has been mmapped (%d)\n",
		       d_path_of_file(file),
		       &task->comm[0], task->pid, nr_mappings);
	}

	return busy;
}

static int task_save_close_file(struct task_struct *task, int fd)
{
	struct files_struct *files = task->files;
	struct fdtable *fdt = files_fdtable(files);
	struct file *file = fcheck_files(files, fd);
	struct file_object *fobj;
	struct file_descriptor *fdesc;

	if (check_file_busy(task, file))
		return -EBUSY;

	fobj = get_file_object(file);
	if (IS_ERR(fobj))
		return PTR_ERR(fobj);

	fdesc = kmalloc(sizeof(*fdesc), GFP_ATOMIC);
	if (!fdesc)
		return -ENOMEM;

	pr_info("PM:FS: save fd [%s(%d)][fd:%d]\n",
		&task->comm[0], task->pid, fd);

	/* backup fd */
	INIT_LIST_HEAD(&fdesc->same_fobj);
	fdesc->fd = fd;
	fdesc->task = task;

	/*
	 * decrease count (close),
	 * it should be 2 at least, 1 held in get_file_object().
	 */
	BUG_ON(file_count(file) == 1);
	fput(file);

	/* free fd slot */
	rcu_assign_pointer(fdt->fd[fd], NULL);
	FD_CLR(fd, fdt->close_on_exec);
	__put_unused_fd(files, fd);

	/* link to fobj */
	list_add_tail(&fdesc->same_fobj, &fobj->fdesc_list);

	return 0;
}

static bool is_on_rw_backing_dev(struct file *file)
{
	struct vfsmount *mnt = file->f_vfsmnt;
	return is_rw_bdev(mnt);
}

static int task_close_file_on_rw_backing_dev(struct task_struct *task, int fd)
{
	struct files_struct *files = task->files;
	struct file *file = fcheck_files(files, fd);
	int error = 0;

	if (!file) {
		pr_err("PM:FS: no file structure\n");
		error = -EBADF;
		goto out;
	}

	if (!is_on_rw_backing_dev(file))
		goto out;

	if (!frozen(task)) {
		pr_err("PM:FS: task [%s] that opened file [%s] not frozen\n",
		       &task->comm[0], d_path_of_file(file));
		error = -EBUSY;
		goto out;
	}

	error = task_save_close_file(task, fd);
	if (error) {
		pr_err("PM:FS: failed to close file %s on process %s\n",
		       &task->comm[0], d_path_of_file(file));
		goto out;
	}

out:
	return error;
}

static int task_close_files_on_rw_backing_devs(struct task_struct *task)
{
	struct files_struct *files = task->files;
	struct fdtable *fdt;
	int i = 0;
	int error = 0;

	if (!frozen(task) || !files)
		return 0;

	spin_lock(&files->file_lock);
	fdt = files_fdtable(files);
	for (i = 0; i < fdt->max_fds; i++) {
		struct file *file = fcheck_files(files, i);
		if (!file)
			continue;
		error = task_close_file_on_rw_backing_dev(task, i);
		if (error)
			goto out;
	}
out:
	spin_unlock(&files->file_lock);
	return error;
}

static int close_files_on_rw_backing_devs(void)
{
	struct task_struct *task;
	struct file_path *fpath;
	int error = 0;

	read_lock(&tasklist_lock);
	for_each_process(task) {
		task_lock(task);
		error = task_close_files_on_rw_backing_devs(task);
		task_unlock(task);
		if (error) {
			goto unlock;
		}
	}
unlock:
	read_unlock(&tasklist_lock);

	/*
	 * On error, do not free files on file_path_list here.
	 * They will be restored by simply increasing files' counts not to
	 * bother closing and reopening files.
	 */
	if (unlikely(error))
		goto out;

	/* free files */
	list_for_each_entry(fpath, &file_path_list, node) {
		struct file_object *fobj;
		list_for_each_entry(fobj, &fpath->fobj_list, same_fpath) {
			/* sanity check - if oops, something's wroing, fix ! */
			pr_err_bug_if(file_count(fobj->file) != 1,
				      "PM:FS: "
				      "file count is %ld, should be 1\n",
				      file_count(fobj->file));
			/* free and nullify, reinstantiated on resumption. */
			BUG_ON(filp_close(fobj->file, task->files) < 0);
			fobj->file = NULL;
		}
	}

out:
	return error;
}

#ifdef CONFIG_INOTIFY_USER

struct saved_inotify_user {
	struct list_head node;
	struct fsnotify_group *group;
	int wd;
	u32 mask;
	char *path;
};

static LIST_HEAD(saved_inotify_users);

extern void inotify_free_mark(struct fsnotify_mark_entry *entry);
extern struct kmem_cache *inotify_inode_mark_cachep __read_mostly;
extern int inotify_max_user_watches __read_mostly;
extern void inotify_remove_from_idr(struct fsnotify_group *group,
				    struct inotify_inode_mark_entry *ientry);

/* copied from inotify_new_watch() */
static int restore_saved_inotify_user(struct saved_inotify_user *saved,
				      struct path *path)
{
	struct fsnotify_group *group = saved->group;
	const char *filepath = saved->path;
	int wd = saved->wd;
	u32 mask = saved->mask;
	struct inotify_inode_mark_entry *tmp_ientry;
	struct inode *inode;
	int ret;

	/* don't allow invalid bits: we don't want flags set */
	if (unlikely(!mask))
		return -EINVAL;

	inode = path->dentry->d_inode;

	tmp_ientry = kmem_cache_alloc(inotify_inode_mark_cachep, GFP_KERNEL);
	if (unlikely(!tmp_ientry))
		return -ENOMEM;

	tmp_ientry->path = kstrdup(filepath, GFP_KERNEL);
	if (unlikely(!tmp_ientry->path)) {
		kmem_cache_free(inotify_inode_mark_cachep, tmp_ientry);
		return -ENOMEM;
	}

	fsnotify_init_mark(&tmp_ientry->fsn_entry, inotify_free_mark);
	tmp_ientry->fsn_entry.mask = mask;
	tmp_ientry->wd = -1;

	ret = -ENOSPC;
	if (atomic_read(&group->inotify_data.user->inotify_watches) >=
	    inotify_max_user_watches)
		goto out_err;
retry:
	ret = -ENOMEM;
	if (unlikely(!idr_pre_get(&group->inotify_data.idr, GFP_KERNEL)))
		goto out_err;

	/* we are putting the mark on the idr, take a reference */
	fsnotify_get_mark(&tmp_ientry->fsn_entry);

	spin_lock(&group->inotify_data.idr_lock);
	ret = idr_get_new_above(&group->inotify_data.idr, &tmp_ientry->fsn_entry,
				wd, &tmp_ientry->wd);
	spin_unlock(&group->inotify_data.idr_lock);
	if (ret) {
		/* we didn't get on the idr, drop the idr reference */
		fsnotify_put_mark(&tmp_ientry->fsn_entry);

		/* idr was out of memory allocate and try again */
		if (ret == -EAGAIN)
			goto retry;
		goto out_err;
	}

	BUG_ON(wd != tmp_ientry->wd);

	/* we are on the idr, now get on the inode */
	ret = fsnotify_add_mark(&tmp_ientry->fsn_entry, group, inode);
	if (ret) {
		/* we failed to get on the inode, get off the idr */
		inotify_remove_from_idr(group, tmp_ientry);
		goto out_err;
	}

	/* update the idr hint, who cares about races, it's just a hint */
	group->inotify_data.last_wd = tmp_ientry->wd;

	/* increment the number of watches the user has */
	atomic_inc(&group->inotify_data.user->inotify_watches);

	/* return the watch descriptor for this tmp_ientry entry */
	ret = tmp_ientry->wd;

	/* if this mark added a new event update the group mask */
	if (mask & ~group->mask)
		fsnotify_recalc_group_mask(group);

	pr_info("PM:FS: restore inotify user [%s]\n", filepath);
	pr_debug("PM:FS:  user   : %p\n", group->inotify_data.user);
	pr_debug("PM:FS:  watches: %d\n",
		 atomic_read(&group->inotify_data.user->inotify_watches));
	pr_debug("PM:FS:  group  : %p\n", group);
	pr_debug("PM:FS:  mask   : 0x%08x\n", mask);
	pr_debug("PM:FS:  wd     : %d\n", wd);

out_err:
	/* match the ref from fsnotify_init_markentry() */
	fsnotify_put_mark(&tmp_ientry->fsn_entry);

	return ret;
}

static void restore_saved_inotify_users(void)
{
	struct saved_inotify_user *saved, *tmp;
	struct path path;

	list_for_each_entry_safe(saved, tmp, &saved_inotify_users, node) {
		int error;

		error = kern_path(saved->path, 0, &path);
		if (error /* -ENOENT */) {
			struct file *file;

			pr_err("PM:FS: not found path [%s] (%d)\n",
			       saved->path, error);

			file = get_noentry_file(saved->path);
			if (IS_ERR(file)) {
				error = PTR_ERR(file);
				pr_err("PM:FS: failed to get noentry file, "
				       "not recoverable (%d)\n", error);
				goto next;
			}
			pr_err("PM:FS: got noentry file and put inotify user "
			       "on it\n");
			path = file->f_path;
			path_get(&path);
			/*
			 * inotify registeration needs only f_path, not file
			 * itself.
			 */
			filp_close(file, NULL);
		}

		error = restore_saved_inotify_user(saved, &path);
#if 0
		if (!error) {
			/*
			 * In real world, it's right to regard restored files
			 * as modified in most cases ?
			 */
			fsnotify_enable();
			fsnotify_modify(path.dentry);
			fsnotify_disable();
		}
#endif
		path_put(&path);
	next:
		list_del(&saved->node);
		kfree(saved);
	}
}

static int save_inotify_user_group_on_mnt(struct fsnotify_group *group,
					  struct vfsmount *mnt)
{
	struct fsnotify_mark_entry *entry, *tmp;
	int error = 0;

	spin_lock(&group->mark_lock);
	list_for_each_entry_safe(entry, tmp, &group->mark_entries, g_list) {
		struct inode *inode = entry->inode;
		struct inotify_inode_mark_entry *ientry =
			container_of(entry, struct inotify_inode_mark_entry,
				     fsn_entry);
		struct user_struct *user = group->inotify_data.user;
		struct saved_inotify_user *save;
		char *path_str = ientry->path;

		if (inode->i_sb != mnt->mnt_sb)
			continue;

		save = kzalloc(sizeof(*save) + strlen(path_str) + 1,
			       GFP_ATOMIC);
		if (!save) {
			error = -ENOMEM;
			goto unlock;
		}
		INIT_LIST_HEAD(&save->node);
		save->path = (char *)save + sizeof(*save);

		strcpy(save->path, path_str);
		save->group = group;
		save->wd = ientry->wd;
		save->mask = entry->mask;
		list_add(&save->node, &saved_inotify_users);
		pr_info("PM:FS: save inotify user [%s]\n", save->path);
		pr_debug("PM:FS:  user   : %p\n", user);
		pr_debug("PM:FS:  watches: %d\n",
			 atomic_read(&user->inotify_watches));
		pr_debug("PM:FS:  group  : %p\n", group);
		pr_debug("PM:FS:  mask   : 0x%08x\n", entry->mask);
		pr_debug("PM:FS:  wd     : %d\n", ientry->wd);

		fsnotify_get_mark(entry);
		spin_unlock(&group->mark_lock);

		fsnotify_destroy_mark_by_entry(entry);
		fsnotify_put_mark(entry);

		spin_lock(&group->mark_lock);
	}

unlock:
	spin_unlock(&group->mark_lock);
	return error;
}

static int save_inotify_users_on_mnt(struct vfsmount *mnt, void *unused)
{
	struct fsnotify_group *group;
	int idx;
	int ret = 0;

	idx = srcu_read_lock(&fsnotify_grp_srcu);
	list_for_each_entry_rcu(group, &fsnotify_groups, group_list) {
		if (group->ops == &inotify_fsnotify_ops) {
			ret = save_inotify_user_group_on_mnt(group, mnt);
			if (ret)
				goto unlock;
		}
	}
unlock:
	srcu_read_unlock(&fsnotify_grp_srcu, idx);
	return ret;
}

static int save_inotify_users_on_rw_backing_devs(void)
{
	return iterate_mounts_safe_reverse(save_inotify_users_on_mnt,
					   is_rw_bdev, NULL);
}

#else
inline static int save_inotify_users_on_rw_partitions(void) { return 0; }
inline static int restore_saved_inotify_users(void) { return 0; }

#endif /* CONFIG_INOTIFY_USER */

int fastboot_hibernate_fs_suspend(void)
{
	int error;

	pr_debug("PM:FS: Got PM_POST_FREEZE_PROCESS event\n");

	error = save_inotify_users_on_rw_backing_devs();
	if (error)
		goto restore_inotify_users;
	error = close_files_on_rw_backing_devs();
	if (error)
		goto open;
	error = save_task_fs_on_rw_backing_devs();
	if (error)
		goto restore_task_fs;
	error = unmount_rw_backing_devs();
	if (error)
		goto mount;

	pr_debug("PM:FS: Done with PM_POST_FREEZE_PROCESS event\n");
	return 0;

mount:
	mount_saved_mountpoints();
restore_task_fs:
	restore_saved_task_fs();
open:
	open_saved_files();
restore_inotify_users:
	restore_saved_inotify_users();

	pr_err("PM:FS: failure in suspending filesystem (%d)\n", error);
	return error;
}

int fastboot_hibernate_fs_resume(void)
{
	pr_debug("PM:FS: Got PM_THAW_PROCESS_PREPARE event\n");

	/* no error check for task'fs */
	mount_saved_mountpoints();
	restore_saved_task_fs();
	open_saved_files();
	restore_saved_inotify_users();

	pr_debug("PM:FS: Done with PM_THAW_PROCESS_PREPARE event\n");
	return 0;
}
