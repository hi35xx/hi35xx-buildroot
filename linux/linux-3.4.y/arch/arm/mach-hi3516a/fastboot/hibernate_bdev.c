#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/spinlock.h>
#include <linux/hdreg.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>

struct hibernate_bdev {
	const char *path;
	struct mutex lock;
	int devidx;
	struct gendisk *disk;
	struct request_queue *rq;
	spinlock_t queue_lock;
	struct task_struct *thread;
	struct file *filp;
	atomic_t opened;
	void *priv;
};

static int HB_BDEV_MAJOR = 0;
static const char *HB_BDEV_NAME = "hb_bdev";

static sector_t HB_OFFSET;
static sector_t HB_SIZE;

extern int __init fastboot_find_bootpartition(const char *name,
		unsigned long long *offset, unsigned long long *size);

static int hb_bdev_thread(void *arg);

static inline int __hb_init_env(void)
{
	unsigned long long ofs, sz;
	int n = fastboot_find_bootpartition("hibernate", &ofs, &sz);
	if (n < 0)
		return n;
	HB_OFFSET = ofs >> 9;
	HB_SIZE = sz >> 9;

	if (!HB_BDEV_MAJOR)
		HB_BDEV_MAJOR = register_blkdev(HB_BDEV_MAJOR, HB_BDEV_NAME);

	return 0;
}

static int hb_bdev_open(struct block_device *bdev, fmode_t mode)
{
	struct hibernate_bdev *hb = bdev->bd_disk->private_data;
	
	int devidx = MINOR(bdev->bd_dev);
	if (!hb)
		return -ERESTARTSYS;
	
	if (IS_ERR_OR_NULL(hb->filp)) {
		hb->filp = filp_open(hb->path, O_RDWR|O_SYNC | O_LARGEFILE, 0);
		if (!hb->filp)
			hb->filp = ERR_PTR(-ERESTARTSYS);
		if (!IS_ERR(hb->filp)) {
			hb->thread = kthread_run(hb_bdev_thread, hb,
					"%s%d", HB_BDEV_NAME, devidx);
			if (IS_ERR(hb->thread)) {
				int ret = PTR_ERR(hb->thread);
				filp_close(hb->filp, current->files);
				hb->filp = ERR_PTR(-ERESTARTSYS);
				return ret;
			}
		}
	}

	if (IS_ERR_OR_NULL(hb->filp))
		return PTR_ERR(hb->filp);

	atomic_inc(&hb->opened);
	return 0;
}

static int hb_bdev_release(struct gendisk *disk, fmode_t mode)
{
	struct hibernate_bdev *hb = disk->private_data;
	if (!hb)
		return -ENXIO;
	if (atomic_dec_and_test(&hb->opened)) {
		kthread_stop(hb->thread);
		if (!IS_ERR_OR_NULL(hb->filp))
			filp_close(hb->filp, current->files);
		hb->filp = ERR_PTR(-ENODEV);
	}
	return 0;
}

static const struct block_device_operations hb_bdev_ops = {
	.owner		= THIS_MODULE,
	.open		= hb_bdev_open,
	.release	= hb_bdev_release,
};

static int hb_bdev_do_request(struct hibernate_bdev *hb,
			       struct request *req)
{
	unsigned long long offset_tmp;
	unsigned long long offset, len;
	char *buf;
	int ret;

	if (!hb->filp)
		return -ENODEV;
    /*modify because linux3.x change, --liucan*/
#if 0
	if (!blk_fs_request(req))
		return -EIO;
#else
    if (req->cmd_type != REQ_TYPE_FS)
        return 0;
#endif

	if (blk_rq_pos(req) + blk_rq_cur_sectors(req) >
			get_capacity(req->rq_disk))
		return -ENOSPC;

	offset = (blk_rq_pos(req) + HB_OFFSET) << 9;
	len = blk_rq_cur_bytes(req);
	buf = req->buffer;

	offset_tmp = offset;

	switch(rq_data_dir(req)) {
	case READ:
		ret = vfs_read(hb->filp, (char __user*)buf, len, &offset_tmp);
		rq_flush_dcache_pages(req);
		break;
	case WRITE:
		rq_flush_dcache_pages(req);
		ret = vfs_write(hb->filp, (char __user*)buf, len, &offset_tmp);
		break;
	default:
		pr_err("%s: Unknown request %u\n", __func__, rq_data_dir(req));
		return -EIO;
	}

	if (ret < 0)
		return ret;

	return 0;
}

static int hb_bdev_thread(void *arg)
{
	struct hibernate_bdev *hb = arg;
	struct request_queue *rq = hb->rq;
	struct request *req = NULL;

	set_fs(get_ds());

	spin_lock_irq(rq->queue_lock);

	while (!kthread_should_stop()) {
		int res;

		if (!req && !(req = blk_fetch_request(rq))) {
			set_current_state(TASK_INTERRUPTIBLE);
			spin_unlock_irq(rq->queue_lock);
			schedule();
			spin_lock_irq(rq->queue_lock);
			continue;
		}

		spin_unlock_irq(rq->queue_lock);

		mutex_lock(&hb->lock);
		res = hb_bdev_do_request(hb, req);
		mutex_unlock(&hb->lock);

		spin_lock_irq(rq->queue_lock);

		if (!__blk_end_request_cur(req, res))
			req = NULL;
	}

	if (req)
		__blk_end_request_all(req, -EIO);

	spin_unlock_irq(rq->queue_lock);

	return 0;
}

static void hb_bdev_request(struct request_queue *rq)
{
	struct hibernate_bdev *hb;
	struct request *req = NULL;

	hb = rq->queuedata;

	if (!hb)
		while ((req = blk_fetch_request(rq)) != NULL)
			__blk_end_request_all(req, -ENODEV);
	else
		wake_up_process(hb->thread);
}

static int hibernate_add_blkdev(int devidx, const char *path)
{
	struct hibernate_bdev *hb;
	struct gendisk *gd;

	hb = kzalloc(sizeof(*hb), GFP_KERNEL);
	if (!hb)
		return -ENOMEM;
	hb->path = path;
	mutex_init(&hb->lock);
	atomic_set(&hb->opened, 0);
	hb->filp = ERR_PTR(-ENODEV);

	gd = alloc_disk(1);
	if (!gd) {
		kfree(hb);
		return -ENOMEM;
	}

	hb->disk = gd;
	gd->private_data = hb;
	gd->major = HB_BDEV_MAJOR;
	gd->first_minor = devidx;
	gd->fops = &hb_bdev_ops;
	snprintf(gd->disk_name, sizeof(gd->disk_name), "%s%d",
			HB_BDEV_NAME, devidx);

	set_capacity(gd, HB_SIZE);

	spin_lock_init(&hb->queue_lock);
	hb->rq = blk_init_queue(hb_bdev_request, &hb->queue_lock);
	if (!hb->rq) {
		kfree(gd);
		kfree(hb);
		return -ENOMEM;
	}
	hb->rq->queuedata = hb;
	blk_queue_logical_block_size(hb->rq, 4096);
	gd->queue = hb->rq;

	add_disk(gd);

	return 0;
}

static int __init hibernate_bdev_init(void)
{
	if (__hb_init_env() < 0)
		return 0;
	hibernate_add_blkdev(0, "/dev/block/mmcblk0");
	return 0;
}
late_initcall(hibernate_bdev_init);
