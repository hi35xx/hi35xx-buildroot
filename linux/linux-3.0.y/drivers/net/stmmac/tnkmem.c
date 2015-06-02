/*****************************************************************************
  This is the driver for the CreVinn TOE-NK-2G TCP Offload Engine.
  TOE-NK-2G incorporates a Synopsys Ethernet MAC core.

  Copyright (C) 2011 Emutex Ltd. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  Authors: Mark Burkley <mark@emutex.com>

*******************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>

#include <net/tcp.h>
#include <net/tnkdrv.h>

#include "tnkmem.h"

static struct
{
	int alloc_skb;
	int alloc_bytes;
	int write_queue_skb;
	int write_queue_bytes;
	int read_queue_skb;
	int read_queue_bytes;
	int free_pool_queue_skb;
}
	meminfo;

void tnk_mem_proc(struct seq_file *s)
{
	seq_printf(s, "\ntnkmem:\n");
	seq_printf(s, "skbs allocated: %d\n", meminfo.alloc_skb);
	seq_printf(s, "bytes allocated: %d\n", meminfo.alloc_bytes);
	seq_printf(s, "skbs queued write: %d\n", meminfo.write_queue_skb);
	seq_printf(s, "bytes queued write: %d\n", meminfo.write_queue_bytes);
	seq_printf(s, "skbs queued read: %d\n", meminfo.read_queue_skb);
	seq_printf(s, "bytes queued read: %d\n", meminfo.read_queue_bytes);
	seq_printf(s, "skbs queued recycle: %d\n", meminfo.free_pool_queue_skb);
}

void tnk_mem_write_queued(struct sk_buff *skb)
{
	meminfo.write_queue_skb++;
	meminfo.write_queue_bytes += skb->len;
}

void tnk_mem_write_dequeued(struct sk_buff *skb)
{
	meminfo.write_queue_skb--;
	meminfo.write_queue_bytes -= skb->len;
}

void tnk_mem_read_queued(struct sk_buff *skb)
{
	meminfo.read_queue_skb++;
	meminfo.read_queue_bytes += skb->len;
}

void tnk_mem_read_dequeued(struct sk_buff *skb)
{
	meminfo.read_queue_skb--;
	meminfo.read_queue_bytes -= skb->len;
}

void tnk_mem_free_pool_queued(struct sk_buff *skb)
{
	meminfo.free_pool_queue_skb++;
}

void tnk_mem_free_pool_dequeued(struct sk_buff *skb)
{
	meminfo.free_pool_queue_skb--;
}

struct sk_buff *tnk_alloc_skb_fclone(int size, int allocation)
{
	struct sk_buff *skb = alloc_skb_fclone(size, allocation);

	if (skb) {
		meminfo.alloc_skb++;
		meminfo.alloc_bytes += size;
	}

	return skb;
}

struct sk_buff *tnk_alloc_skb(int size, int allocation)
{
	struct sk_buff *skb = alloc_skb(size, allocation);

	if (skb) {
		meminfo.alloc_skb++;
		meminfo.alloc_bytes += size;
	}

	return skb;
}

void tnk_free_skb(struct sk_buff *skb)
{
	meminfo.alloc_skb--;
	meminfo.alloc_bytes -= skb->len;

	kfree_skb(skb);
}
