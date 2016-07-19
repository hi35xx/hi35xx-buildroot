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

#ifndef __TNKMEM_H
#define __TNKMEM_H

enum skb_from_where {
	SKB_FROM_RECV = 0,
	SKB_FROM_SEND
};

void tnk_mem_proc(struct seq_file *s);
void tnk_mem_write_queued(struct sk_buff *skb);
void tnk_mem_write_dequeued(struct sk_buff *skb);
void tnk_mem_read_queued(struct sk_buff *skb);
void tnk_mem_read_dequeued(struct sk_buff *skb);
void tnk_mem_free_pool_queued(struct sk_buff *skb);
void tnk_mem_free_pool_dequeued(struct sk_buff *skb);
void tnk_alloc_skb_send_static(struct sk_buff *skb);
struct sk_buff *tnk_alloc_skb(int size, int allocation);
void tnk_free_skb(struct sk_buff *skb, int skb_from_where);

#endif
