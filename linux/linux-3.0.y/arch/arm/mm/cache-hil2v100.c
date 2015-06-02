#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#include <asm/cacheflush.h>
#include <asm/irq.h>

#include <mach/irqs.h>
#include <mach/platform.h>
#include <mach/early-debug.h>
#include "cache-hil2v100.h"

#define CACHE_LINE_SIZE		L2_LINE_SIZE

#define DRIVER_NAME "l2cache"

static void __iomem *l2cache_base;
static int l2cache_flag;
static DEFINE_SPINLOCK(l2cache_lock);

static void l2cache_release(struct device *dev)
{
}

static int l2cache_remove(struct platform_device *pdev)
{
	return 0;
}

static inline void sync_writel(unsigned long val, unsigned long reg,
		unsigned long complete_mask)
{
	writel_relaxed(val, l2cache_base + reg);
}

static inline void cache_sync(void)
{
	unsigned int reg;
	reg = readl_relaxed(l2cache_base + REG_L2_SYNC);
}

/******************************************************************************
 *
 * l2_invalid_auto -  auto invalid l2 cache
 *
 * This routine auto invalid l2 cache
 *
 *****************************************************************************/
static void l2_invalid_auto(unsigned int way_num)   /* invalid cache way num */
{
	unsigned int reg;

	cache_sync();
	reg = (way_num << BIT_L2_MAINT_AUTO_WAYADDRESS)
		| (0x1 << BIT_L2_MAINT_AUTO_START);
	sync_writel(reg, REG_L2_MAINT_AUTO, reg);
	while (!(readl_relaxed(l2cache_base + REG_L2_RINT)
				& (0X1 << BIT_L2_RINT_AUTO_END)))
		;
	reg = readl_relaxed(l2cache_base + REG_L2_RINT);
	sync_writel(reg, REG_L2_INTCLR, reg);
	cache_sync();
}

/******************************************************************************
 *
 * l2_cache_auto-  auto clean l2 cache
 *
 * This routine auto clean l2 cache
 *
 *****************************************************************************/
static void l2_cache_auto(unsigned int way_num)      /* clean cache way num */
{
	unsigned int reg;

	cache_sync();
	reg = (way_num << BIT_L2_MAINT_AUTO_WAYADDRESS)
		| (0x1 << BIT_L2_MAINT_AUTO_START | 0x2);
	sync_writel(reg, REG_L2_MAINT_AUTO, reg);
	while (!(readl_relaxed(l2cache_base + REG_L2_RINT)
				& (0X1 << BIT_L2_RINT_AUTO_END)))
		;
	reg = readl_relaxed(l2cache_base + REG_L2_RINT);
	sync_writel(reg, REG_L2_INTCLR, reg);
	cache_sync();
}
static void __l2cache_inv_all(void)
{
	unsigned int way_num;

	if (!l2cache_flag) {
		printk(KERN_INFO "%s:l2cache_flag = %d, " \
				"L2 cache may not enable!\n",
				__func__, l2cache_flag);
		return;
	}

	cache_sync();
	/*invalidate cache  all-way*/
	for (way_num = 0; way_num < L2_WAY_NUM; way_num++)
		l2_invalid_auto(way_num);

	cache_sync();
}
static void l2cache_inv_all(void)
{
	unsigned long flags;

	spin_lock_irqsave(&l2cache_lock, flags);

	__l2cache_inv_all();

	spin_unlock_irqrestore(&l2cache_lock, flags);
}

static void l2cache_clean_all(void)
{
	unsigned int way_num;

	if (!l2cache_flag) {
		printk(KERN_INFO "%s:l2cache_flag = %d, " \
				"L2 cache may not enable!\n",
				__func__, l2cache_flag);
		return;
	}

	cache_sync();
	/*invalidate cache  all-way*/
	for (way_num = 0; way_num < L2_WAY_NUM; way_num++)
		l2_cache_auto(way_num);

	cache_sync();
}

static void l2cache_flush_all(void)
{
	unsigned long flags;

	if (!l2cache_flag) {
		printk(KERN_INFO "%s:l2cache_flag = %d, " \
				"L2 cache may not enable!\n",
				__func__, l2cache_flag);
		return;
	}

	spin_lock_irqsave(&l2cache_lock, flags);

	l2cache_clean_all();
	__l2cache_inv_all();

	spin_unlock_irqrestore(&l2cache_lock, flags);
}

static void l2cache_inv_range(unsigned long start, unsigned long end)
{
	unsigned long addr;
	unsigned long reg;
	unsigned long flags;
	if (!l2cache_flag) {
		printk(KERN_INFO "%s:l2cache_flag = %d, " \
				"L2 cache may not enable!\n",
				__func__, l2cache_flag);
		return;
	}

#ifdef CONFIG_CACHE_HIL2V100_DEBUG
	printk(KERN_DEBUG "%s: start addr: 0x%x, end addr: 0x%x\n", __func__,
			(unsigned int)(start), (unsigned int)(end));
#endif
	spin_lock_irqsave(&l2cache_lock, flags);

	cache_sync();
	start &= ~(CACHE_LINE_SIZE - 1);

	for (addr = start; addr < end; addr += CACHE_LINE_SIZE) {
		reg = addr | BIT_L2_INVALID_BYADDRESS;
		sync_writel(reg, REG_L2_INVALID, reg);
	}
	cache_sync();

	spin_unlock_irqrestore(&l2cache_lock, flags);
}

static void l2cache_clean_range(unsigned long start, unsigned long end)
{
	unsigned long addr;
	unsigned long reg;
	unsigned long flags;

	if (!l2cache_flag) {
		printk(KERN_INFO "%s:l2cache_flag = %d, " \
				"L2 cache may not enable!\n",
				__func__, l2cache_flag);
		return;
	}

#ifdef CONFIG_CACHE_HIL2V100_DEBUG
	printk(KERN_DEBUG "%s: start addr: 0x%x, end addr: 0x%x\n", __func__,
			(unsigned int)(start), (unsigned int)(end));
#endif
	spin_lock_irqsave(&l2cache_lock, flags);

	cache_sync();
	start &= ~(CACHE_LINE_SIZE - 1);
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE)	{
		reg = addr | BIT_L2_CLEAN_BYADDRESS;
		sync_writel(reg, REG_L2_CLEAN, reg);
	}
	cache_sync();

	spin_unlock_irqrestore(&l2cache_lock, flags);
}

static void l2cache_flush_range(unsigned long start, unsigned long end)
{
	unsigned long addr;
	unsigned long flags;

	if (!l2cache_flag) {
		printk(KERN_INFO "%s:l2cache_flag = %d, " \
				"L2 cache may not enable!\n",
				__func__, l2cache_flag);
		return;
	}

#ifdef CONFIG_CACHE_HIL2V100_DEBUG
	printk(KERN_DEBUG "%s: start addr: 0x%x, end addr: 0x%x\n", __func__,
			(unsigned int)(start), (unsigned int)(end));
#endif
	spin_lock_irqsave(&l2cache_lock, flags);

	cache_sync();
	start &= ~(CACHE_LINE_SIZE - 1);
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE)	{
		sync_writel(addr | 1, REG_L2_CLEAN, 1);
		sync_writel(addr | 1, REG_L2_INVALID, 1);
	}
	cache_sync();

	spin_unlock_irqrestore(&l2cache_lock, flags);
}
#ifdef CONFIG_OUTER_CACHE_SYNC
static void l2cache_sync(void)
{
	unsigned long flags;
	spin_lock_irqsave(&l2cache_lock, flags);

	cache_sync();

	spin_unlock_irqrestore(&l2cache_lock, flags);
}
#endif
static irqreturn_t l2cache_handle(int irq, void *dev_id)
{
	unsigned int ret1, ret2, ret3;

	ret1 = readl_relaxed(l2cache_base + REG_L2_RINT);
	ret2 = readl_relaxed(l2cache_base + REG_L2_SPECIAL_CHECK0);
	ret3 = readl_relaxed(l2cache_base + REG_L2_SPECIAL_CHECK1);
	writel_relaxed(0, l2cache_base + REG_L2_INTCLR);/*clear inner INT*/
	printk(KERN_INFO "L2cache interrupt,REG_L2_RINT is 0x%x,", ret1);
	printk(KERN_INFO "REG_L2_SPECIAL_CHECK0 is 0x%x,", ret2);
	printk(KERN_INFO "REG_L2_SPECIAL_CHECK1 is 0x%x!\n", ret3);
	return IRQ_HANDLED;
}
static void l2cache_disable(void)
{
	unsigned long flags;
	spin_lock_irqsave(&l2cache_lock, flags);

	/* disable L2cache */
	writel_relaxed(0, l2cache_base + REG_L2_CTRL);

	spin_unlock_irqrestore(&l2cache_lock, flags);
}
static void l2cache_init(void)
{
	int ret1, ret2, ret3, aux, aux_val = 0;
	unsigned long flags;

	spin_lock_irqsave(&l2cache_lock, flags);

	l2cache_base = (void __iomem *)IO_ADDRESS(REG_BASE_L2CACHE);

	/* disable L2cache */
	writel_relaxed(0, l2cache_base + REG_L2_CTRL);

	aux_val |= (0x1 << BIT_L2_AUCTRL_EVENT_BUS_EN)
				| (0x1 << BIT_L2_AUCTRL_MONITOR_EN);

	aux = readl_relaxed(l2cache_base + REG_L2_AUCTRL);
	aux |= aux_val;
	writel_relaxed(aux, l2cache_base + REG_L2_AUCTRL);

	/* clean last error int */
	writel_relaxed(0, l2cache_base + REG_L2_INTCLR);
	writel_relaxed(0, l2cache_base + REG_L2_SPECIAL_CHECK0);
	writel_relaxed(0, l2cache_base + REG_L2_SPECIAL_CHECK1);

	/* enable inner INT except BIT_L2_INTMASK_AUTO_END */
	writel_relaxed(0x3fff, l2cache_base + REG_L2_INTMASK);

	l2cache_flag = 1;

	__l2cache_inv_all();

	/* enable L2cache */
	writel_relaxed(1, l2cache_base + REG_L2_CTRL);


	outer_cache.inv_range = l2cache_inv_range;
	outer_cache.clean_range = l2cache_clean_range;
	outer_cache.flush_range = l2cache_flush_range;
#ifdef CONFIG_OUTER_CACHE_SYNC
	outer_cache.sync = l2cache_sync;
#endif
	outer_cache.flush_all = l2cache_flush_all;
	outer_cache.inv_all = l2cache_inv_all;
	outer_cache.disable = l2cache_disable;

	spin_unlock_irqrestore(&l2cache_lock, flags);

	ret1 = request_irq(INTNR_L2CACHE_CHK0_INT, l2cache_handle,
			0, "L2 chk0", NULL);
	ret2 = request_irq(INTNR_L2CACHE_CHK1_INT, l2cache_handle,
			0, "L2 chk1", NULL);
	ret3 = request_irq(INTNR_L2CACHE_INT_COMB, l2cache_handle,
			0, "L2 com", NULL);
	if (ret1 || ret2 || ret3)
		printk(KERN_ERR "request l2 cache irq error!\n");

	printk(KERN_INFO "L2cache cache controller enabled\n");
}

static int l2cache_probe(struct platform_device *pdev)
{
	l2cache_init();
	return 0;
}

#ifdef CONFIG_PM
static int l2cache_suspend(struct platform_device *dev, pm_message_t state)
{
	int ret = 0;
	unsigned long flags;

	/* disable L2cache */
	spin_lock_irqsave(&l2cache_lock, flags);

	writel_relaxed(0, l2cache_base + REG_L2_CTRL);

	l2cache_clean_all();
	__l2cache_inv_all();

	l2cache_flag = 0;

	spin_unlock_irqrestore(&l2cache_lock, flags);

	free_irq(INTNR_L2CACHE_CHK0_INT, NULL);
	free_irq(INTNR_L2CACHE_CHK1_INT, NULL);
	free_irq(INTNR_L2CACHE_INT_COMB, NULL);

	outer_cache.inv_range = NULL;
	outer_cache.clean_range = NULL;
	outer_cache.flush_range = NULL;
#ifdef CONFIG_OUTER_CACHE_SYNC
	outer_cache.sync = NULL;
#endif
	outer_cache.flush_all = NULL;
	outer_cache.inv_all = NULL;
	outer_cache.disable = NULL;

	return ret;
}

static int l2cache_resume(struct platform_device *dev)
{
	unsigned long aux, aux_val = 0;
	unsigned long flags, ret, ret1, ret2;

	spin_lock_irqsave(&l2cache_lock, flags);

	/* disable L2cache */
	writel_relaxed(0, l2cache_base + REG_L2_CTRL);

	/* monitor */
	aux_val |= (0x1 << BIT_L2_AUCTRL_EVENT_BUS_EN)
		| (0x1 << BIT_L2_AUCTRL_MONITOR_EN);

	aux = readl_relaxed(l2cache_base + REG_L2_AUCTRL);
	aux |= aux_val;
	writel_relaxed(aux, l2cache_base + REG_L2_AUCTRL);

	/* clean last error int */
	writel_relaxed(0, l2cache_base + REG_L2_INTCLR);
	writel_relaxed(0, l2cache_base + REG_L2_SPECIAL_CHECK0);
	writel_relaxed(0, l2cache_base + REG_L2_SPECIAL_CHECK1);

	/* enable inner INT except BIT_L2_INTMASK_AUTO_END */
	writel_relaxed(0x3fff, l2cache_base + REG_L2_INTMASK);

	l2cache_flag = 1;
	__l2cache_inv_all();

	/* enable L2cache */
	writel_relaxed(1, l2cache_base + REG_L2_CTRL);

	outer_cache.inv_range = l2cache_inv_range;
	outer_cache.clean_range = l2cache_clean_range;
	outer_cache.flush_range = l2cache_flush_range;
#ifdef CONFIG_OUTER_CACHE_SYNC
	outer_cache.sync = l2cache_sync;
#endif
	outer_cache.flush_all = l2cache_flush_all;
	outer_cache.inv_all = l2cache_inv_all;
	outer_cache.disable = l2cache_disable;

	spin_unlock_irqrestore(&l2cache_lock, flags);

	ret  = request_irq(INTNR_L2CACHE_CHK0_INT, l2cache_handle,
			0, "L2 chk0", NULL);
	ret1 = request_irq(INTNR_L2CACHE_CHK1_INT, l2cache_handle,
			0, "L2 chk1", NULL);
	ret2 = request_irq(INTNR_L2CACHE_INT_COMB, l2cache_handle,
			0, "L2 com", NULL);
	if (ret || ret1 || ret2)
		printk(KERN_ERR "request l2 cache irq error!\n");

	return 0;
}
#else
#define l2cache_suspend	NULL
#define l2cache_resume	NULL
#endif

static struct resource l2cache_resources[] = {
	[0] = {
		.start          = REG_BASE_L2CACHE,
		.end            = REG_BASE_L2CACHE + 0x10000 - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = INTNR_L2CACHE_CHK0_INT,
		.end            = INTNR_L2CACHE_CHK0_INT,
		.flags          = IORESOURCE_IRQ,
	},

};

static struct platform_device l2cache_device = {
	.name           = "l2cache",
	.id             = 0,
	.dev = {
		.release  = l2cache_release,
	},
	.num_resources  = ARRAY_SIZE(l2cache_resources),
	.resource       = l2cache_resources,
};

static struct platform_driver l2cache_driver = {
	.probe         = l2cache_probe,
	.remove        = l2cache_remove,
	.suspend       = l2cache_suspend,
	.resume        = l2cache_resume,
	.driver        = {
		.name          = DRIVER_NAME,
	},
};

static int __init l2cache_driver_init(void)
{
	int ret = 0;

	ret = platform_device_register(&l2cache_device);
	if (ret) {
		printk(KERN_INFO "Platform device register is failed!");
		return ret;
	}

	ret = platform_driver_register(&l2cache_driver);
	if (ret) {
		platform_device_unregister(&l2cache_device);
		printk(KERN_INFO "Platform driver register is failed!");
		return ret;
	}
	return ret;
}

static void __exit l2cache_driver_exit(void)
{
	platform_driver_unregister(&l2cache_driver);
	platform_device_unregister(&l2cache_device);
}

module_init(l2cache_driver_init);
module_exit(l2cache_driver_exit);

#ifdef MODULE
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("L2cache driver for the Hisilicon L2cache Controller");
MODULE_LICENSE("GPL");
#endif
