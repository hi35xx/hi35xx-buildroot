/*
 *
 * Copyright (C) 2011 Novell Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/xattr.h>
#include "overlayfs.h"

int ovl_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct dentry *upperdentry;
	int err;

	if ((attr->ia_valid & ATTR_SIZE) && !ovl_dentry_upper(dentry))
		err = ovl_copy_up_truncate(dentry, attr->ia_size);
	else
		err = ovl_copy_up(dentry);
	if (err)
		return err;

	upperdentry = ovl_dentry_upper(dentry);

	if (attr->ia_valid & (ATTR_KILL_SUID|ATTR_KILL_SGID))
		attr->ia_valid &= ~ATTR_MODE;

	mutex_lock(&upperdentry->d_inode->i_mutex);
	err = notify_change(upperdentry, attr);
	mutex_unlock(&upperdentry->d_inode->i_mutex);

	return err;
}

static int ovl_getattr(struct vfsmount *mnt, struct dentry *dentry,
			 struct kstat *stat)
{
	struct path realpath;

	ovl_path_real(dentry, &realpath);
	return vfs_getattr(realpath.mnt, realpath.dentry, stat);
}

int ovl_permission(struct inode *inode, int mask, unsigned int flags)
{
	struct ovl_entry *oe;
	struct dentry *alias = NULL;
	struct inode *realinode;
	struct dentry *realdentry;
	bool is_upper;
	int err;

	if (S_ISDIR(inode->i_mode)) {
		oe = inode->i_private;
	} else if (mask & IPERM_FLAG_RCU) {
		return -ECHILD;
	} else {
		/*
		 * For non-directories find an alias and get the info
		 * from there.
		 */
		spin_lock(&inode->i_lock);
		if (WARN_ON(list_empty(&inode->i_dentry))) {
			spin_unlock(&inode->i_lock);
			return -ENOENT;
		}
		alias = list_entry(inode->i_dentry.next, struct dentry, d_alias);
		dget(alias);
		spin_unlock(&inode->i_lock);
		oe = alias->d_fsdata;
	}

	realdentry = ovl_entry_real(oe, &is_upper);

	/* Careful in RCU walk mode */
	realinode = ACCESS_ONCE(realdentry->d_inode);
	if (!realinode) {
		WARN_ON(!(mask & IPERM_FLAG_RCU));
		err = -ENOENT;
		goto out_dput;
	}

	if (mask & MAY_WRITE) {
		umode_t mode = realinode->i_mode;

		/*
		 * Writes will always be redirected to upper layer, so
		 * ignore lower layer being read-only.
		 *
		 * If the overlay itself is read-only then proceed
		 * with the permission check, don't return EROFS.
		 * This will only happen if this is the lower layer of
		 * another overlayfs.
		 *
		 * If upper fs becomes read-only after the overlay was
		 * constructed return EROFS to prevent modification of
		 * upper layer.
		 */
		err = -EROFS;
		if (is_upper && !IS_RDONLY(inode) && IS_RDONLY(realinode) &&
		    (S_ISREG(mode) || S_ISDIR(mode) || S_ISLNK(mode)))
			goto out_dput;

		/*
		 * Nobody gets write access to an immutable file.
		 */
		err = -EACCES;
		if (IS_IMMUTABLE(realinode))
			goto out_dput;
	}

	if (realinode->i_op->permission)
		err = realinode->i_op->permission(realinode, mask, flags);
	else
		err = generic_permission(realinode, mask, flags, NULL);
out_dput:
	dput(alias);
	return err;
}


struct ovl_link_data {
	struct dentry *realdentry;
	void *cookie;
};

static void *ovl_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	void *ret;
	struct dentry *realdentry;
	struct inode *realinode;

	realdentry = ovl_dentry_real(dentry);
	realinode = realdentry->d_inode;

	if (WARN_ON(!realinode->i_op->follow_link))
		return ERR_PTR(-EPERM);

	ret = realinode->i_op->follow_link(realdentry, nd);
	if (IS_ERR(ret))
		return ret;

	if (realinode->i_op->put_link) {
		struct ovl_link_data *data;

		data = kmalloc(sizeof(struct ovl_link_data), GFP_KERNEL);
		if (!data) {
			realinode->i_op->put_link(realdentry, nd, ret);
			return ERR_PTR(-ENOMEM);
		}
		data->realdentry = realdentry;
		data->cookie = ret;

		return data;
	} else {
		return NULL;
	}
}

static void ovl_put_link(struct dentry *dentry, struct nameidata *nd, void *c)
{
	struct inode *realinode;
	struct ovl_link_data *data = c;

	if (!data)
		return;

	realinode = data->realdentry->d_inode;
	realinode->i_op->put_link(data->realdentry, nd, data->cookie);
	kfree(data);
}

static int ovl_readlink(struct dentry *dentry, char __user *buf, int bufsiz)
{
	struct path realpath;
	struct inode *realinode;

	ovl_path_real(dentry, &realpath);
	realinode = realpath.dentry->d_inode;

	if (!realinode->i_op->readlink)
		return -EINVAL;

	touch_atime(realpath.mnt, realpath.dentry);

	return realinode->i_op->readlink(realpath.dentry, buf, bufsiz);
}


static bool ovl_is_private_xattr(const char *name)
{
	return strncmp(name, "trusted.overlay.", 14) == 0;
}

int ovl_setxattr(struct dentry *dentry, const char *name,
		 const void *value, size_t size, int flags)
{
	int err;
	struct dentry *upperdentry;

	if (ovl_is_private_xattr(name))
		return -EPERM;

	err = ovl_copy_up(dentry);
	if (err)
		return err;

	upperdentry = ovl_dentry_upper(dentry);
	return  vfs_setxattr(upperdentry, name, value, size, flags);
}

ssize_t ovl_getxattr(struct dentry *dentry, const char *name,
		     void *value, size_t size)
{
	if (ovl_path_type(dentry->d_parent) == OVL_PATH_MERGE &&
	    ovl_is_private_xattr(name))
		return -ENODATA;

	return vfs_getxattr(ovl_dentry_real(dentry), name, value, size);
}

ssize_t ovl_listxattr(struct dentry *dentry, char *list, size_t size)
{
	ssize_t res;
	int off;

	res = vfs_listxattr(ovl_dentry_real(dentry), list, size);
	if (res <= 0 || size == 0)
		return res;

	if (ovl_path_type(dentry->d_parent) != OVL_PATH_MERGE)
		return res;

	/* filter out private xattrs */
	for (off = 0; off < res;) {
		char *s = list + off;
		size_t slen = strlen(s) + 1;

		BUG_ON(off + slen > res);

		if (ovl_is_private_xattr(s)) {
			res -= slen;
			memmove(s, s + slen, res - off);
		} else {
			off += slen;
		}
	}

	return res;
}

int ovl_removexattr(struct dentry *dentry, const char *name)
{
	int err;
	struct path realpath;
	enum ovl_path_type type;

	if (ovl_path_type(dentry->d_parent) == OVL_PATH_MERGE &&
	    ovl_is_private_xattr(name))
		return -ENODATA;

	type = ovl_path_real(dentry, &realpath);
	if (type == OVL_PATH_LOWER) {
		err = vfs_getxattr(realpath.dentry, name, NULL, 0);
		if (err < 0)
			return err;

		err = ovl_copy_up(dentry);
		if (err)
			return err;

		ovl_path_upper(dentry, &realpath);
	}

	return vfs_removexattr(realpath.dentry, name);
}

static bool ovl_open_need_copy_up(int flags, enum ovl_path_type type,
				  struct dentry *realdentry)
{
	if (type != OVL_PATH_LOWER)
		return false;

	if (special_file(realdentry->d_inode->i_mode))
		return false;

	if (!(OPEN_FMODE(flags) & FMODE_WRITE) && !(flags & O_TRUNC))
		return false;

	return true;
}

static struct file *ovl_open(struct dentry *dentry, int flags,
			     const struct cred *cred)
{
	int err;
	struct path realpath;
	enum ovl_path_type type;

	type = ovl_path_real(dentry, &realpath);
	if (ovl_open_need_copy_up(flags, type, realpath.dentry)) {
		if (flags & O_TRUNC)
			err = ovl_copy_up_truncate(dentry, 0);
		else
			err = ovl_copy_up(dentry);
		if (err)
			return ERR_PTR(err);

		ovl_path_upper(dentry, &realpath);
	}

	return vfs_open(&realpath, flags, cred);
}

static const struct inode_operations ovl_file_inode_operations = {
	.setattr	= ovl_setattr,
	.permission	= ovl_permission,
	.getattr	= ovl_getattr,
	.setxattr	= ovl_setxattr,
	.getxattr	= ovl_getxattr,
	.listxattr	= ovl_listxattr,
	.removexattr	= ovl_removexattr,
	.open		= ovl_open,
};

static const struct inode_operations ovl_symlink_inode_operations = {
	.setattr	= ovl_setattr,
	.follow_link	= ovl_follow_link,
	.put_link	= ovl_put_link,
	.readlink	= ovl_readlink,
	.getattr	= ovl_getattr,
	.setxattr	= ovl_setxattr,
	.getxattr	= ovl_getxattr,
	.listxattr	= ovl_listxattr,
	.removexattr	= ovl_removexattr,
};

struct inode *ovl_new_inode(struct super_block *sb, umode_t mode,
			    struct ovl_entry *oe)
{
	struct inode *inode;

	inode = new_inode(sb);
	if (!inode)
		return NULL;

	mode &= S_IFMT;

	inode->i_ino = get_next_ino();
	inode->i_mode = mode;
	inode->i_flags |= S_NOATIME | S_NOCMTIME;

	switch (mode) {
	case S_IFDIR:
		inode->i_private = oe;
		inode->i_op = &ovl_dir_inode_operations;
		inode->i_fop = &ovl_dir_operations;
		break;

	case S_IFLNK:
		inode->i_op = &ovl_symlink_inode_operations;
		break;

	case S_IFREG:
	case S_IFSOCK:
	case S_IFBLK:
	case S_IFCHR:
	case S_IFIFO:
		inode->i_op = &ovl_file_inode_operations;
		break;

	default:
		WARN(1, "illegal file type: %i\n", mode);
		inode = NULL;
	}

	return inode;

}
