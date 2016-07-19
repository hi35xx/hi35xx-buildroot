/*
 * hisilicon clock Management framework Routines
 *
 * Author: wangjian <stand.wang@huawei.com>
 *
 * Copyright (C) 2012 Hisilicon Instruments, Inc.
 * wangjian <stand.wang@huawei.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include <asm/clkdev.h>
#include <linux/io.h>
#include <mach/io.h>
#include <mach/platform.h>
#include <mach/early-debug.h>
#include <mach/clock.h>

static DEFINE_MUTEX(clock_list_lock);
static DEFINE_MUTEX(clocks_mutex);
static LIST_HEAD(hiclocks);
static DEFINE_SPINLOCK(clocks_lock);

static inline void clk_lock_init(struct clk *c)
{
	mutex_init(&c->mutex);
	spin_lock_init(&c->spinlock);
}

struct clk *hi_get_clock_by_name(const char *name)
{
	struct clk *c;
	struct clk *ret = NULL;
	mutex_lock(&clock_list_lock);
	list_for_each_entry(c, &hiclocks, node) {
		if (strcmp(c->name, name) == 0) {
			ret = c;
			break;
		}
	}
	mutex_unlock(&clock_list_lock);
	return ret;
}
EXPORT_SYMBOL(hi_get_clock_by_name);

unsigned long clk_get_rate(struct clk *c)
{
	unsigned long flags;
	unsigned long rate;

	spin_lock_irqsave(&clocks_lock, flags);

	rate = c->rate;

	spin_unlock_irqrestore(&clocks_lock, flags);

	return rate;
}
EXPORT_SYMBOL(clk_get_rate);

int clk_enable(struct clk *c)
{
	int ret = 0;

	unsigned long flags;
	/*
	 * if c->refcnt == 0,clk hasn't been enabled ,
	 * shoud enable if ,else just refcnt ++
	 */
	if (c->refcnt == 0) {
		if (c->parent) {
			ret = clk_enable(c->parent); /* enable parent */
			if (ret) {
				WARN(1, "enable clock:%s failed", c->name);
				goto out;
			}
		}

		if (c->ops && c->ops->enable) {
			ret = c->ops->enable(c);
			if (ret) { /* if enable faild ,disable the parent */
				if (c->parent)
					clk_disable(c->parent);
				WARN(1, "enable clock:%s failed", c->name);
				goto out;
			}
		}
		spin_lock_irqsave(&clocks_lock, flags);
		c->state = ON;
		c->set = true;
		spin_unlock_irqrestore(&clocks_lock, flags);
	}
	spin_lock_irqsave(&clocks_lock, flags);
	c->refcnt++;
	spin_unlock_irqrestore(&clocks_lock, flags);
out:
	return ret;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *c)
{
	unsigned long flags;

	if (c->refcnt == 0) {
		WARN(1, "Attempting to disable clock %s with refcnt 0",
			c->name);
		return;
	}
	if (c->refcnt == 1) {
		if (c->ops && c->ops->disable)
			c->ops->disable(c);

		if (c->parent)
			clk_disable(c->parent);
		spin_lock_irqsave(&clocks_lock, flags);
		c->state = OFF;
		spin_unlock_irqrestore(&clocks_lock, flags);
	}
	spin_lock_irqsave(&clocks_lock, flags);
	c->refcnt--;
	spin_unlock_irqrestore(&clocks_lock, flags);
}
EXPORT_SYMBOL(clk_disable);

int clk_set_parent(struct clk *c, struct clk *parent)
{
	int ret = 0;

	if (!c->ops || !c->ops->set_parent) {
		ret = -ENOSYS;
		goto out;
	}

	ret = c->ops->set_parent(c, parent);
out:
	return ret;
}
EXPORT_SYMBOL(clk_set_parent);

struct clk *clk_get_parent(struct clk *c)
{
	return c->parent;
}
EXPORT_SYMBOL(clk_get_parent);

int clk_set_rate(struct clk *c, unsigned long rate)
{
	int ret = 0;

	if (!c) {
		printk(KERN_INFO "%s->%d\n", __func__, __LINE__);
		return 0;
	}
	printk(KERN_INFO "%s rate=%ld\n", __func__, rate);

	if (!c->ops || !c->ops->set_rate) {
		ret = -ENOSYS;
		goto out;
	}

	if (rate > c->max_rate)
		rate = c->max_rate;

	ret = c->ops->set_rate(c, rate);
	if (ret)
		goto out;

out:
	return ret;
}
EXPORT_SYMBOL(clk_set_rate);

long clk_round_rate(struct clk *c, unsigned long rate)
{
	long ret;

	if (!c->ops || !c->ops->round_rate) {
		ret = -ENOSYS;
		goto out;
	}

	if (rate > c->max_rate)
		rate = c->max_rate;

	ret = c->ops->round_rate(c, rate);

out:
	return ret;
}
EXPORT_SYMBOL(clk_round_rate);

void clk_init(struct clk *c)
{
	clk_lock_init(c);

	if (c->ops && c->ops->init)
		c->ops->init(c);

	if (!c->ops || !c->ops->enable) {

		c->refcnt++;
		c->set = true;

		if (c->parent)
			c->state = c->parent->state;
		else
			c->state = ON;
	}

	mutex_lock(&clock_list_lock);
	list_add(&c->node, &hiclocks);
	mutex_unlock(&clock_list_lock);
}
EXPORT_SYMBOL(clk_init);

void clk_exit(struct clk *c)
{
	if (!c) {
		c->refcnt--;
		c->set = true;
	}

	mutex_lock(&clock_list_lock);
	list_del(&c->node);
	mutex_unlock(&clock_list_lock);
	return;
}
EXPORT_SYMBOL(clk_exit);
