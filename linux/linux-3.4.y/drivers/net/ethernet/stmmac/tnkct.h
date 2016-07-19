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

  Author: Dan O'Donovan <dan@emutex.com>

*******************************************************************************/

/*
 *  TNK connection table manager
 */

#ifndef __TNKCT_H__
#define __TNKCT_H__

/* #define TNK_HW_DEBUG */
#undef TNK_CT_DEBUG
/*#define STMMAC_XMIT_DEBUG*/
#ifdef TNK_CT_DEBUG
#define TNKCT_DBG(fmt, args...)  printk(fmt, ## args)
#else
#define TNKCT_DBG(fmt, args...)  do { } while (0)
#endif


#include "tnkinfo.h"

struct sock *tnk_ct_get_sock(int index);
int tnk_ct_init(unsigned int max_connections);
void tnk_ct_tx_backlog_enqueue(int cindex, struct sk_buff *skb, bool flush);
void tnk_ct_tx_backlog_flush_all(int cindex);
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
int tnk_ct_rx_release(int cindex, int len);
#else
void tnk_ct_rx_release(int count);
#endif
void tnk_ct_update_rx_seq(struct tnkentry *e);
void tnk_ct_shutdown(void);
void tnk_ct_prepare(struct sk_buff *skb, struct tnkinfo *tnkinfo,
		    struct net_device *dev, int gmac_id);

struct tnkentry *tnk_ct_create(struct sock *s, struct tnkentry *e);
void tnk_ct_enable(unsigned int cindex);
struct tnkentry *tnk_get_free_entry(struct sock *sk);
int tnk_ct_stop(struct tnkentry *e, int tx_done_timeout);
void tnk_ct_remove(struct tnkentry *e);
struct tnkentry *tnk_ct_get_entry(int i);
struct tnkentry *tnk_ct_get_next_used(struct tnkentry *e);
void tnk_ct_link_state_update(int gmac_id, int link_up);

void tnk_ct_proc(struct seq_file *s);

#endif /*  __TNKCT_H__ */
