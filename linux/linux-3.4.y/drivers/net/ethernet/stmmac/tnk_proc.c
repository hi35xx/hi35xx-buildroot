/*****************************************************************************
 *  This is the driver for the CreVinn TOE-NK-2G TCP Offload Engine.
 *  TOE-NK-2G incorporates a Synopsys Ethernet MAC core.
 *
 *  Copyright (C) 2011 Emutex Ltd. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms and conditions of the GNU General Public License,
 *  version 2, as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Authors: Mark Burkley  <mark@emutex.com>
 *           Kai Ji        <kai@emutex.com>
 *           Dan O'Donovan <dan@emutex.com>
 *
 ******************************************************************************/

/*
 *  TNK connection table manager
 */
#include <linux/proc_fs.h>
#include <linux/tcp.h>
#include <net/sock.h>
#include <net/tnkdrv.h>
#include <net/inet_sock.h>

#include "tnkct.h"
#include "tnkhw.h"
#include "tnkinfo.h"
#include "tnkmem.h"
#include "stmmac.h"

#define TNK_PARENT       "tnk"
#define TNK_SUMMARY_PROC "summary"
#define TNK_STATS_PROC   "stats"
#define TNK_CT_PROC      "ct"
#define TNK_BYPASS_PROC  "bypass"

static struct proc_dir_entry *proc_tnk_dir;

static unsigned int tnk_max_connections;
static struct timer_list tnk_rate_timer;

static struct _analysis {
	int preamble;
	int active;
	int last;
	int diffs;
	int matches;
	int bytes;
	int count;
	int curr;
} analysis[TNK_MAX_CONNECTIONS];

static void tnk_rate_timer_handler(unsigned long x)
{
	int i;
	struct tnkentry *t;

	for (i = 2; i < tnk_max_connections; i++) {
		t = tnk_ct_get_entry(i);
		if (t && t->sk) {
			t->rate_sent = t->sent - t->last_sent;
			t->last_sent = t->sent;
			t->rate_recv = t->recv - t->last_recv;
			t->last_recv = t->recv;
		}
	}

	mod_timer(&tnk_rate_timer, jiffies + HZ);
}

void verify_data(struct sk_buff *skb, int index)
{
	int i;
	int x;
	struct _analysis *a = &analysis[index];

	/*  printk ("analyse %d index %d\n", skb->len, index); */
	for (i = 0; i < skb->len; i++) {
		if (!a->active && skb->data[i] == 0xa5)
			a->preamble++;
		else if (a->active && skb->data[i] == 0x5a)
			a->preamble++;
		else
			a->preamble = 0;

		if (a->preamble >= 16 && !a->active) {
			a->preamble = 0;
			a->active = 1;
			a->bytes = 0;
			a->curr = 0;
			continue;
		}

		if (a->preamble >= 16 && a->active) {
			a->preamble = 0;
			a->active = 0;
		}

		if (!a->active)
			continue;

		a->count++;
		x = skb->data[i];
		a->curr |= (x << (a->bytes * 8));
		/*  if (a->count < 32) */
		/*     printk ("x=%d cur=%d\n", x, a->curr); */
		a->bytes++;

		if (a->bytes == 4) {
			if (a->curr != a->last + 1)
				a->diffs++;
			else
				a->matches++;

			a->last = a->curr;
			a->bytes = 0;
			a->curr = 0;
		}
	}
}

static void tnk_conn_header_printout(struct seq_file *s, struct tnkentry *e)
{
	struct inet_sock *inet = inet_sk(e->sk);

	seq_printf(s, "\nTNK Conn[%4d]:\n", e->index);
#ifdef CONFIG_IPV6
	if (e->sk->sk_gso_type == SKB_GSO_TCPV6) {
		struct ipv6_pinfo *inet6 = inet6_sk(e->sk);
		seq_printf(s, "\tS-IP=%pI6\n\tD-IP=%pI6\n",
				&inet6->saddr, &inet6->daddr);
	} else
#endif
	{
		seq_printf(s, "\tS-IP=%pI4 D-IP=%pI4\n",
				&inet->inet_saddr, &inet->inet_daddr);

	}

	seq_printf(s, "\tsport=%d dport=%d\n\tinfo=%s\n",
			ntohs(inet->inet_sport), ntohs(inet->inet_dport),
			e->info);
}

static void tnk_summary_seq_printout(struct seq_file *s, struct tnkentry *e)
{
	struct tcp_sock *tcp = tcp_sk(e->sk);
	struct sk_buff *skb;

	tnk_conn_header_printout(s, e);

	seq_printf(s,
			"\tsent       %d\n" "\treceived   %d\n"
			"\tsnd_nxt    0x%08x / %d\n"
			"\tsnd_una    0x%08x / %d (%d in queue)\n"
			"\trcv_nxt    0x%08x / %d\n"
			"\tcopied_seq 0x%08x / %d (%d in queue)\n"
			"\tsk-rx-q    %d\n"
			"\ttnk-rx-q   %d\n" "\ttnk-tx-q   %d\n"
			"\ttnk-ofo-q  %d\n"
			"\twmem_queue %d\n"
			"\tsndbuf     %d\n" "\tsock flags 0x%lX\n"
			"\tstate      %d\n"
			"\tsend-rate  %d.%02dM\n" "\trecv-rate  %d.%02dM\n",
			e->sent, e->recv, tcp->snd_nxt,
			tcp->snd_nxt - e->initial_ack, tcp->snd_una,
			tcp->snd_una - e->initial_ack,
			tcp->snd_nxt - tcp->snd_una,
			tcp->rcv_nxt, tcp->rcv_nxt - e->initial_seq,
			tcp->copied_seq,
			tcp->copied_seq - e->initial_seq,
			tcp->rcv_nxt - tcp->copied_seq,
			skb_queue_len(&e->sk->sk_receive_queue),
			skb_queue_len(&e->receive_queue),
			skb_queue_len(&e->transmit_queue),
			skb_queue_len(&e->out_of_order_queue),
			e->sk->sk_wmem_queued,
			e->sk->sk_sndbuf,
			e->sk->sk_socket ? e->sk->sk_socket->flags : (~0UL),
			e->sk->sk_tnkinfo.state,
			e->rate_sent / 1000000, e->rate_sent / 10000,
			e->rate_recv / 1000000, e->rate_recv / 10000);

	skb_queue_walk(&e->out_of_order_queue, skb)
		seq_printf(s, "\nofo_seq=%x, end=%x\n",
				TCP_SKB_CB(skb)->seq, TCP_SKB_CB(skb)->end_seq);
}

static void tnk_stats_seq_printout(struct seq_file *s, struct tnkentry *e)
{
	struct tnkhw_conn_stats cstats;

	tnkhw_conn_stats_get(e->index, &cstats);

	tnk_conn_header_printout(s, e);

	/* Print connection stats */
	seq_printf(s,
			"\ttx_desc_sent: %u\n"
			"\ttx_desc_ackd: %u\n"
			"\ttx_bytes_sent: %u\n"
			"\ttx_bytes_ackd: %u\n"
			"\ttx_overflow: %u\n"
			"\trx_pkts: %u\n"
			"\trx_dropped: %u\n"
			"\ttx_dfetch_desc: %u\n"
			"\ttx_dfetch_data: %u\n"
			"\ttx_dfetch_own: %u\n"
			"\ttx_retry_desc: %u\n"
			"\ttx_retry_data: %u\n"
			"\ttx_ackd_read: %u\n"
			"\ttx_ackd_size: %u\n"
			"\ttx_ackd_write: %u\n"
			"\trx_premature_eop: %u\n"
			"\trx_bad_ethertype: %u\n"
			"\trx_bad_dst_macaddr: %u\n"
			"\trx_tcp_csum_err: %u\n"
			"\trx_iphdr_csum_err: %u\n"
			"\trx_non_tcp_prot: %u\n"
			"\trx_bad_dst_ipaddr: %u\n"
			"\trx_connection_err: %u\n"
			"\trx_bad_seq_num: %u\n"
			"\trx_busy_pkt_drop: %u\n"
			"\trx_toe_err: %u\n"
			"\ttx_acked_count: %u\n"
			"\ttx_retry_count: %u\n"
			"\trxed_byte_count: %u\n"
			"\tttx_pipelined_bytes: %u\n"
			"\ttx_block_num: %u\n"
			"\ttx_max_data_size: %u\n",
		cstats.tx_desc_sent,
		cstats.tx_desc_ackd,
		cstats.tx_bytes_sent,
		cstats.tx_bytes_ackd,
		cstats.tx_overflow,
		cstats.rx_pkts,
		cstats.rx_dropped,
		cstats.tx_dfetch_desc,
		cstats.tx_dfetch_data,
		cstats.tx_dfetch_own,
		cstats.tx_retry_desc,
		cstats.tx_retry_data,
		cstats.tx_ackd_read,
		cstats.tx_ackd_size,
		cstats.tx_ackd_write,
		cstats.rx_premature_eop,
		cstats.rx_bad_ethertype,
		cstats.rx_bad_dst_macaddr,
		cstats.rx_tcp_csum_err,
		cstats.rx_iphdr_csum_err,
		cstats.rx_non_tcp_prot,
		cstats.rx_bad_dst_ipaddr,
		cstats.rx_connection_err,
		cstats.rx_bad_seq_num,
		cstats.rx_busy_pkt_drop,
		cstats.rx_toe_err,
		cstats.tx_acked_count,
		cstats.tx_retry_count,
		cstats.rxed_byte_count,
		cstats.ttx_pipelined_bytes,
		cstats.tx_block_num,
		cstats.tx_max_data_size);
}

static void tnk_ct_seq_printout(struct seq_file *s, struct tnkentry *e)
{
	struct tnkhw_connection conn;

	tnkhw_connection_get(e->index , &conn);

	tnk_conn_header_printout(s, e);

	seq_printf(s, "\tr_macaddr_u16: 0x%04x\n", conn.r_macaddr_u16);
	seq_printf(s, "\tl_port: %u\n", conn.l_port);
	seq_printf(s, "\tr_macaddr_l32: 0x%08x\n", conn.r_macaddr_l32);
	seq_printf(s, "\tr_ipaddr: 0x%08x\n", conn.r_ipaddr);
	seq_printf(s, "\tr_port: %u\n", conn.r_port);
	seq_printf(s, "\tttl: %u\n", conn.ttl);
	seq_printf(s, "\ttos: %u\n", conn.tos);
	seq_printf(s, "\tword4_flag: %u\n", conn.word4_flag);
	seq_printf(s, "\tword4_count: %u\n", conn.word4_count);
	seq_printf(s, "\tword5_recoverseqnum: 0x%08x\n",
						conn.word5_recoverseqnum);
	seq_printf(s, "\trx_ack_num: 0x%08x\n", conn.rx_ack_num);
	seq_printf(s, "\tnext_tx_seq_num: 0x%08x\n", conn.next_tx_seq_num);
	seq_printf(s, "\tnext_rx_seq_num: 0x%08x\n", conn.next_rx_seq_num);
	seq_printf(s, "\tdup_ack_cnt: %u\n", conn.dup_ack_cnt);
	seq_printf(s, "\trx_adv_wnd: %u\n", conn.rx_adv_wnd);
	seq_printf(s, "\ttimer_tx_retry: %u\n", conn.timer_tx_retry);
	seq_printf(s, "\tretry_count: %u\n", conn.retry_count);
	seq_printf(s, "\ttx_mss: %u\n", conn.tx_mss);
	seq_printf(s, "\tadv_wnd_scale: %u\n", conn.adv_wnd_scale);
	seq_printf(s, "\tflag_conn_enable: %u\n", conn.flag_conn_enable);
	seq_printf(s, "\tflag_mac_sel: %u\n", conn.flag_mac_sel);
	seq_printf(s, "\tflag_ipv6: %u\n", conn.flag_ipv6);
	seq_printf(s, "\tflag_disable_nagle: %u\n", conn.flag_disable_nagle);
	seq_printf(s, "\tflag_use_local_wnd: %u\n", conn.flag_use_local_wnd);
	seq_printf(s, "\tflag_use_local_scl: %u\n", conn.flag_use_local_scl);
	seq_printf(s, "\tflag_full_pkt: %u\n", conn.flag_full_pkt);
	seq_printf(s, "\ttx_adv_wnd: %u\n", conn.tx_adv_wnd);
	seq_printf(s, "\tcong_wnd: %u\n", conn.cong_wnd);
	seq_printf(s, "\ttimer_timestamp: %u\n", conn.timer_timestamp);
	seq_printf(s, "\ttimer_active: %u\n", conn.timer_active);
	seq_printf(s, "\ttimer_seq_num: 0x%08x\n", conn.timer_seq_num);
	seq_printf(s, "\ttimeout_value: %u\n", conn.timeout_value);
	seq_printf(s, "\tsampled_ack_num: 0x%08x\n", conn.sampled_ack_num);
	seq_printf(s, "\ttimer_rx_fast_retry: %u\n", conn.timer_rx_fast_retry);
	seq_printf(s, "\tretry_seq_num: 0x%08x\n", conn.retry_seq_num);
	seq_printf(s, "\ten_rtt_calc: %u\n", conn.en_rtt_calc);
	seq_printf(s, "\tss_threshold: %u\n", conn.ss_threshold);
	seq_printf(s, "\ttx_timestamp: %u\n", conn.tx_timestamp);
	seq_printf(s, "\ttimer_rtt: %u\n", conn.timer_rtt);
	seq_printf(s, "\ttimer_rtt_valid: %u\n", conn.timer_rtt_valid);
	seq_printf(s, "\tlast_retry_seq_num: 0x%08x\n",
						conn.last_retry_seq_num);
	seq_printf(s, "\tr_ipv6_addr_31_0: %u\n", conn.r_ipv6_addr_31_0);
	seq_printf(s, "\tr_ipv6_addr_63_32: %u\n", conn.r_ipv6_addr_63_32);
	seq_printf(s, "\tr_ipv6_addr_95_64: %u\n", conn.r_ipv6_addr_95_64);
	seq_printf(s, "\tr_ipv6_addr_127_96: %u\n", conn.r_ipv6_addr_127_96);
	seq_printf(s, "\tr_ipv6_flow_label: %u\n", conn.r_ipv6_flow_label);
	seq_printf(s, "\tword30_updated_tx_ack_num: 0x%08x\n",
						conn.word30_updated_tx_ack_num);
	seq_printf(s, "\tword31_updated_flag: %u\n", conn.word31_updated_flag);
}

/* proc interface setup */
static void *tnk_seq_start(struct seq_file *s, loff_t *pos)
{
	/*   counter is used to tracking multi proc interfaces
	 *  We have only one interface so return zero
	 *  pointer to start the sequence.
	 */
	static unsigned long counter;
	if (*pos == 0) {
		return &counter;
	} else {
		*pos = 0;
		return NULL;
	}
}

/* proc interface next */
static void *tnk_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;

	return NULL;
}

/* define parameters where showed in proc file */
static int tnk_summary_seq_show(struct seq_file *s, void *v)
{
	struct tnkentry *t;
	int i;
	int rate_sent = 0;
	int rate_recv = 0;
	unsigned long flags;

	TNK_DBG("%s: the number of max connection: %d\n", __func__,
			tnk_max_connections);

	for (i = 2; i < tnk_max_connections; i++) {
		t = tnk_ct_get_entry(i);
		spin_lock_irqsave(&t->list_lock, flags);
		if (t && t->sk) {
#if 0
			struct _analysis *a = &analysis[i];

			seq_printf(s, "file analysis:-\n\n");
			seq_printf(s, "preamble= %d\n", a->preamble);
			seq_printf(s, "active=   %d\n", a->active);
			seq_printf(s, "last=     %d\n", a->last);
			seq_printf(s, "bytes=    %d\n", a->bytes);
			seq_printf(s, "diffs=    %d\n", a->diffs);
			seq_printf(s, "matches=  %d\n", a->matches);

			a->diffs = 0;
			a->matches = 0;
#endif

			tnk_summary_seq_printout(s, t);
			rate_sent += t->rate_sent;
			rate_recv += t->rate_recv;
		}
		spin_unlock_irqrestore(&t->list_lock, flags);

	}

	seq_printf(s, "total send-rate  %d.%02dM\n"
			"total recv-rate  %d.%02dM\n",
			rate_sent / 1000000,
			rate_sent / 10000, rate_recv / 1000000,
			rate_recv / 10000);
#if 0
	tnk_mem_proc(s);
#endif
	tnk_ct_proc(s);
	tnk_hw_proc(s);
	TNK_DBG(KERN_ALERT "%s\n", __func__);

	return 0;
}

static int tnk_stats_seq_show(struct seq_file *s, void *v)
{
	struct tnkentry *t;
	int i;
	unsigned long flags;

	for (i = 2; i < tnk_max_connections; i++) {
		t = tnk_ct_get_entry(i);
		spin_lock_irqsave(&t->list_lock, flags);
		if (t && t->sk)
			tnk_stats_seq_printout(s, t);
		spin_unlock_irqrestore(&t->list_lock, flags);
	}

	return 0;
}

static int tnk_ct_seq_show(struct seq_file *s, void *v)
{
	struct tnkentry *t;
	int i;
	unsigned long flags;

	for (i = 2; i < tnk_max_connections; i++) {
		t = tnk_ct_get_entry(i);
		spin_lock_irqsave(&t->list_lock, flags);
		if (t && t->sk)
			tnk_ct_seq_printout(s, t);
		spin_unlock_irqrestore(&t->list_lock, flags);
	}

	return 0;
}

static int tnk_bypass_seq_show(struct seq_file *s, void *v)
{
	stmmac_proc(s);

	return 0;
}

/* proc interface stop */
static void tnk_seq_stop(struct seq_file *s, void *v)
{
}

/* proc interface operation */
static const struct seq_operations tnk_summary_seq_ops = {
	.start = tnk_seq_start,
	.next = tnk_seq_next,
	.stop = tnk_seq_stop,
	.show = tnk_summary_seq_show
};

/* proc interface operation */
static const struct seq_operations tnk_stats_seq_ops = {
	.start = tnk_seq_start,
	.next = tnk_seq_next,
	.stop = tnk_seq_stop,
	.show = tnk_stats_seq_show
};

/* proc interface operation */
static const struct seq_operations tnk_ct_seq_ops = {
	.start = tnk_seq_start,
	.next = tnk_seq_next,
	.stop = tnk_seq_stop,
	.show = tnk_ct_seq_show
};

/* proc interface operation */
static const struct seq_operations tnk_bypass_seq_ops = {
	.start = tnk_seq_start,
	.next = tnk_seq_next,
	.stop = tnk_seq_stop,
	.show = tnk_bypass_seq_show
};

/* proc file open*/
static int tnk_summary_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &tnk_summary_seq_ops);
};

static int tnk_stats_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &tnk_stats_seq_ops);
};

static int tnk_ct_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &tnk_ct_seq_ops);
};

/* proc file open*/
static int tnk_bypass_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &tnk_bypass_seq_ops);
};

/* proc file operation */
static const struct file_operations tnk_summary_proc_ops = {
	.owner = THIS_MODULE,
	.open = tnk_summary_proc_open,
	.read = seq_read,
	.release = seq_release
};

static const struct file_operations tnk_stats_proc_ops = {
	.owner = THIS_MODULE,
	.open = tnk_stats_proc_open,
	.read = seq_read,
	.release = seq_release
};

static const struct file_operations tnk_ct_proc_ops = {
	.owner = THIS_MODULE,
	.open = tnk_ct_proc_open,
	.read = seq_read,
	.release = seq_release
};


static const struct file_operations tnk_bypass_proc_ops = {
	.owner = THIS_MODULE,
	.open = tnk_bypass_proc_open,
	.read = seq_read,
	.release = seq_release
};

int tnk_proc_init(unsigned int max_connections)
{
	struct proc_dir_entry *proc_summary_entry;
	struct proc_dir_entry *proc_stats_entry;
	struct proc_dir_entry *proc_ct_entry;
	struct proc_dir_entry *proc_bypass_entry;

	tnk_max_connections = max_connections;

	TNK_DBG("%s: the number of max connection: %d\n", __func__,
			tnk_max_connections);
	TNK_DBG("%s: create proc file!\n", __func__);

	proc_tnk_dir = proc_mkdir(TNK_PARENT, NULL);
	if (!proc_tnk_dir) {
		pr_err("%s: failed to create proc file %s\n",
				__func__, TNK_PARENT);
		return 1;
	}

	if (tnk_max_connections > 0) {
		proc_summary_entry = create_proc_entry(TNK_SUMMARY_PROC,
				0, proc_tnk_dir);
		if (!proc_summary_entry) {
			pr_err("%s: failed to create proc file %s\n",
					__func__, TNK_SUMMARY_PROC);
			return 1;
		}
		proc_summary_entry->proc_fops = &tnk_summary_proc_ops;

		proc_stats_entry = create_proc_entry(TNK_STATS_PROC,
				0, proc_tnk_dir);
		if (!proc_stats_entry) {
			pr_err("%s: failed to create proc file %s\n",
					__func__, TNK_STATS_PROC);
			return 1;
		}
		proc_stats_entry->proc_fops = &tnk_stats_proc_ops;

		proc_ct_entry = create_proc_entry(TNK_CT_PROC,
				0, proc_tnk_dir);
		if (!proc_ct_entry) {
			pr_err("%s: failed to create proc file %s\n",
					__func__, TNK_CT_PROC);
			return 1;
		}
		proc_ct_entry->proc_fops = &tnk_ct_proc_ops;
	}

	proc_bypass_entry = create_proc_entry(TNK_BYPASS_PROC, 0, proc_tnk_dir);

	if (!proc_bypass_entry) {
		pr_err("%s: failed to create proc file\n", __func__);
		return 1;
	}
	proc_bypass_entry->proc_fops = &tnk_bypass_proc_ops;

	setup_timer(&tnk_rate_timer, tnk_rate_timer_handler, 0);
	/* The tnk_rate_timer start work after 50 seconds,
	   it can make sure all variables init over */
	mod_timer(&tnk_rate_timer, jiffies + (50 * HZ));

	return 0;

}

int tnk_proc_shutdown(void)
{
	del_timer(&tnk_rate_timer);
	if (proc_tnk_dir) {
		if (tnk_max_connections > 0) {
			remove_proc_entry(TNK_SUMMARY_PROC, proc_tnk_dir);
			remove_proc_entry(TNK_STATS_PROC, proc_tnk_dir);
			remove_proc_entry(TNK_CT_PROC, proc_tnk_dir);
		}
		remove_proc_entry(TNK_BYPASS_PROC, proc_tnk_dir);
		remove_proc_entry(TNK_PARENT, NULL);
		proc_tnk_dir = NULL;
	}

	return 0;
}
