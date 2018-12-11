#include <mach/io.h>
#include <linux/io.h>

#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#include "hi_reg_user.h"

static int hi_reg_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int hi_reg_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int hi_reg_read(struct file *file, char __user *buf,
		size_t count, loff_t *f_pos)
{
	return 0;
}

static int hi_reg_write(struct file *file, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	return 0;
}


static long hi_reg_ioctl(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	void __iomem *virt_addr;
	struct hi_reg_handle handle;
	int valid_addr = 0;
	unsigned int new_value = 0;

	if (copy_from_user((void *)&handle, (void *)arg,
				sizeof(struct hi_reg_handle))) {
		printk(KERN_ERR"hi reg copy from user error!\n");
		return -EFAULT;
	}

	/* phys addr must be aligned by 4 bytes */
    if (!handle.phys_addr || (handle.phys_addr & (sizeof(u32) - 1))) {
        printk(KERN_ERR"phys addr not aligned by 4 bytes,which is [0x%08x]!\n",
                handle.phys_addr);
		return -EFAULT;
	}

#ifdef CONFIG_ARCH_HI3516AV200
	/* phys addr must be in IO_ADDRESS */
    if ((handle.phys_addr >= HI3516AV200_IOCH3_PHYS)
			&& (handle.phys_addr < HI3516AV200_IOCH3_PHYS + HI3516AV200_IOCH3_SIZE)) {
		valid_addr = 1;
    } else if ((handle.phys_addr >= HI3516AV200_IOCH2_PHYS)
			&& (handle.phys_addr < HI3516AV200_IOCH2_PHYS + HI3516AV200_IOCH2_SIZE)) {
		valid_addr = 1;
    } else if ((handle.phys_addr >= HI3516AV200_IOCH1_PHYS)
			&& (handle.phys_addr < HI3516AV200_IOCH1_PHYS + HI3516AV200_IOCH1_SIZE)) {
		valid_addr = 1;
    }
#endif

#if defined(CONFIG_ARCH_HI3519) || defined(CONFIG_ARCH_HI3519V101)
	/* phys addr must be in IO_ADDRESS */
    if ((handle.phys_addr >= HI3519_IOCH3_PHYS)
			&& (handle.phys_addr < HI3519_IOCH3_PHYS + HI3519_IOCH3_SIZE)) {
		valid_addr = 1;
    } else if ((handle.phys_addr >= HI3519_IOCH2_PHYS)
			&& (handle.phys_addr < HI3519_IOCH2_PHYS + HI3519_IOCH2_SIZE)) {
		valid_addr = 1;
    } else if ((handle.phys_addr >= HI3519_IOCH1_PHYS)
			&& (handle.phys_addr < HI3519_IOCH1_PHYS + HI3519_IOCH1_SIZE)) {
		valid_addr = 1;
    }
#endif

	if (!valid_addr) {
        printk(KERN_ERR"invalid addr, which is [0x%08x]!\n",
                handle.phys_addr);
		return -EFAULT;
	}
	if ('R' == _IOC_TYPE(cmd)) {
		switch (_IOC_NR(cmd)) {
			case _IOC_NR(HI_REG_READ):
				virt_addr = (void *)IO_ADDRESS(handle.phys_addr);
				new_value = hi_readl(virt_addr);
				handle.data = (void *)new_value;
				if (copy_to_user((void *)arg, (void *)&handle,
							sizeof(struct hi_reg_handle))) {
					printk(KERN_ERR"hi reg copy to user error!\n");
					return -EFAULT;
				}
				break;

			case _IOC_NR(HI_REG_WRITE):
				virt_addr = (void *)IO_ADDRESS(handle.phys_addr);

				if (sizeof(u32) == handle.size) {
					hi_writel((u32)handle.data, virt_addr);
				} else if (sizeof(u16) == handle.size) {
					hi_writew(((u32)handle.data & 0xFFFF), virt_addr);
				} else if (sizeof(u8) == handle.size) {
					hi_writeb(((u32)handle.data & 0xFF), virt_addr);
				} else {
					printk(KERN_ERR"%d, write size should be 1, 2 or 4!\n", __LINE__);
				}
				break;

			default:
				printk(KERN_ERR"%d, hi reg unknow cmd!\n", __LINE__);
				break;
		}
	} else {
		printk(KERN_ERR"%d, hi reg unknow cmd!\n", __LINE__);
	}
	return 0;
}

static const struct file_operations hi_reg_fops = {
	.owner          = THIS_MODULE,
	.open           = hi_reg_open,
	.release        = hi_reg_release,
	.unlocked_ioctl = hi_reg_ioctl,
	.write          = hi_reg_write,
	.read           = hi_reg_read
};

static struct miscdevice hi_reg_dev = {
	.minor    = MISC_DYNAMIC_MINOR,
	.fops     = &hi_reg_fops,
	.name     = "hi_reg"
};

static int __init hi_reg_init(void)
{
	int ret;
	ret = misc_register(&hi_reg_dev);
	if (ret) {
		printk("register hi_reg device failed!");
	}
	return ret;
}

void __exit hi_reg_exit(void)
{
	misc_deregister(&hi_reg_dev);
}
#ifndef MODULE
subsys_initcall(hi_reg_init);
#else
module_init(hi_reg_init);
#endif
module_exit(hi_reg_exit);

MODULE_DESCRIPTION("Driver for himd/himm/himc");
MODULE_AUTHOR("ou jinsong");
MODULE_LICENSE("GPL");
