/*****************************************************************************
 * This is the driver for the CreVinn TOE-NK-2G TCP Offload Engine.
 * TOE-NK-2G incorporates a Synopsys Ethernet MAC core.
 *
 * Copyright (C) 2011 Emutex Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Authors: Mark Burkley <mark@emutex.com>
 *          Kai Ji <kai@emutex.com>
 *
 ******************************************************************************/

#ifndef UNIT_TEST
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/slab.h>

#include <net/tcp.h>
#include <net/tnkdrv.h>

#include "tnkhw.h"
#include "tnkct.h"
#include "tnkmem.h"
#include "tnksysctl.h"
#include "stmmac.h"
#endif

#include "tnktcp.h"

/* #define TNK_TCP_DEBUG */
#undef TNK_TCP_DEBUG
#ifdef TNK_TCP_DEBUG
#undef TNK_DBG
#define TNK_DBG(fmt, args...)  printk(fmt, ## args)
#else
#define TNK_DBG(fmt, args...)  do { } while (0)
#endif

#define USER_SNDTIMEO_LEVEL (60*HZ) /* 60s */

struct tnkfuncs funcs = {
	.tcp_prepare = tnk_tcp_prepare,
	.tcp_open = tnk_tcp_open,
	.tcp_close = tnk_tcp_close,
	.tcp_sendpages = tnk_tcp_sendpages,
	.tcp_sendmsg = tnk_tcp_sendmsg,
	.tcp_receive = tnk_tcp_receive,
	.tcp_read_sock = tnk_tcp_read_sock,
	.tcp_update = tnk_tcp_update,
	.tcp_wait_data = tnk_tcp_wait_data,
	.tcp_check_fin = tnk_tcp_check_fin,
	.tcp_check_connect_state = tnk_tcp_check_connect_state
};

static struct timer_list tnk_tcp_persist_timer;

static int free_skb;

static spinlock_t tnk_tcp_lock;

static void tnk_state_change(struct sock *sk, int old, int new)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	char *cpu = "";
	char tnk[5];
	static char *states[] = {
		"OPEN",
		"PREPARED",
		"ESTABLISHED",
		"ACTIVATING",
		"ACTIVE",
		"STOPPING",
		"STOPPED",
		"CLOSED"
	};
	char *old_state;
	char *new_state;

	if (in_irq())
		cpu = "IRQ    ";
	else if (in_softirq())
		cpu = "SoftIRQ";
	else if (in_interrupt())
		cpu = "Interpt";
	else
		cpu = "normal ";

	old_state = states[old];
	new_state = states[new];

	if (t->entry)
		sprintf(tnk, "%d", t->entry->index);
	else
		strcpy(tnk, "none");

	CONTROL_DBG("%s cpu=%s sk=%p tnk=%-4.4s state %s => %s\n",
		    __func__, cpu, sk, tnk, old_state, new_state);

	sk->sk_tnkinfo.state = new;
}

static void tnk_tcp_persist_timer_handler(unsigned long x)
{
	tnkhw_inc_persist();
	mod_timer(&tnk_tcp_persist_timer, jiffies + tnk_tcp_persist_time);
}

/**
 * tnk_tcp_wait_data - wait for data to arrive at receive_queue
 * @sk:    sock to wait on
 * @timeo: for how long
 *
 * Now socket state including sk->sk_err is changed only under lock,
 * hence we may omit checks after joining wait queue.
 * We check receive queue before schedule() only as optimization;
 * it is very likely that release_sock() added new data.
 */
#define TNK_DATA_READY_CHECK						\
	((skb_queue_len(&sk->sk_receive_queue) > 0) ||			\
	 (t->entry != NULL && (skb_queue_len(&t->entry->receive_queue) > 0)))

int tnk_tcp_wait_data(struct sock *sk, long *timeo)
{
	int rc;
	struct tnkinfo *t = &sk->sk_tnkinfo;
	DEFINE_WAIT(wait);

	if (t->entry == NULL)
		return 0;

	if (*timeo == 0)
		return TNK_DATA_READY_CHECK;

	if (sk->sk_err ||
	    sock_flag(sk, SOCK_DONE) || (sk->sk_shutdown & RCV_SHUTDOWN))
		return 0;

	while (*timeo > 0 && !TNK_DATA_READY_CHECK) {
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		set_bit(SOCK_ASYNC_WAITDATA, &sk->sk_socket->flags);
		rc = sk_wait_event(sk, timeo, TNK_DATA_READY_CHECK);
		clear_bit(SOCK_ASYNC_WAITDATA, &sk->sk_socket->flags);
		finish_wait(sk_sleep(sk), &wait);
	}

	if (*timeo > 0) {
		/* If *timeo > 0, then we can assume that we broke out
		 * of the loop above because we have data ready
		 */
		return 1;
	} else {
		TNK_DBG("%s: Timeout expired (%ld)\n", __func__, *timeo);
		return 0;
	}
}

void tnk_tcp_init(void)
{
	tcp_register_tnk_funcs(&funcs);
	setup_timer(&tnk_tcp_persist_timer, tnk_tcp_persist_timer_handler, 0);
	mod_timer(&tnk_tcp_persist_timer, jiffies + tnk_tcp_persist_time);
	spin_lock_init(&tnk_tcp_lock);
}

void tnk_tcp_shutdown(void)
{
	TNK_DBG("%s shutting down\n", __func__);
	tcp_register_tnk_funcs(NULL);
	del_timer(&tnk_tcp_persist_timer);
}

/*
 *  TNK TCP connection management
 */

int tnk_tcp_prepare(struct sock *sk, struct sk_buff *skb, int inbound)
{
	int i;
	struct net_device *ndev;
	unsigned long flags;
	int err = 0;
	struct dst_entry *dst;

	TNK_DBG("%s begin\n", __func__);
	CONTROL_DBG("%s: Got sk %p, skb %p\n", __func__, sk, skb);

	spin_lock_irqsave(&tnk_tcp_lock, flags);

	if (likely(sk->sk_tnkinfo.state >= TNKINFO_STATE_PREPARED)) {
		TNK_DBG("%s: already prepared\n", __func__);
		goto out;
	}
	dst = __sk_dst_get(sk);
	if (!dst || !dst->neighbour) {
		TNK_DBG("%s: Neighbour pointer unset\n", __func__);
		goto out;
	}

	ndev = dst->neighbour->dev;
	if (unlikely(!ndev)) {
		pr_err("%s: skb->dev is NULL\n", __func__);
		err = -EINVAL;
		goto out;
	}

	sk->sk_tnkinfo.dev = NULL;
	for (i = 0; i < TNK_NUM_GMACS; i++) {
		if (stmmac_device_list[i] == ndev) {
			sk->sk_tnkinfo.dev = ndev;
			break;
		}
	}
	if (unlikely(!sk->sk_tnkinfo.dev)) {
		TNK_DBG("%s: skb->dev %p not recognised, ignoring connection\n",
		       __func__, ndev);
		err = -EINVAL;
		goto out;
	} else {
		struct stmmac_priv *priv = netdev_priv(sk->sk_tnkinfo.dev);
		if (unlikely(!priv)) {
			pr_err("%s: skb->dev priv data  is uninitialised\n",
			       __func__);
			err = -EINVAL;
			goto out;
		}

		TNK_DBG("%s: GMAC id is %d\n", __func__, priv->id);
		sk->sk_tnkinfo.gmac_id = priv->id;
	}

	memcpy(sk->sk_tnkinfo.rmac, dst->neighbour->ha,
	       sizeof(sk->sk_tnkinfo.rmac));

	TNK_DBG("%s: Remote MAC address is %02x:%02x:%02x:%02x:%02x:%02x\n",
		__func__,
		sk->sk_tnkinfo.rmac[0],
		sk->sk_tnkinfo.rmac[1],
		sk->sk_tnkinfo.rmac[2],
		sk->sk_tnkinfo.rmac[3],
		sk->sk_tnkinfo.rmac[4], sk->sk_tnkinfo.rmac[5]);

	sk->sk_tnkinfo.bytecount = 0;
	/*  sk->sk_tnkinfo.state = TNKINFO_STATE_PREPARED; */
	tnk_state_change(sk, sk->sk_tnkinfo.state, TNKINFO_STATE_PREPARED);
	sk->sk_tnkinfo.entry = NULL;

	TNK_DBG("tcp connection prepared\n");

out:
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);
	return err;
}

int tnk_tcp_open(struct sock *sk)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	unsigned long flags;
	int err = 0;

	CONTROL_DBG("%s begin\n", __func__);

	spin_lock_irqsave(&tnk_tcp_lock, flags);

	if (t->state == TNKINFO_STATE_ESTABLISHED)
		goto out;

	if (t->state != TNKINFO_STATE_PREPARED) {
		TNK_DBG("%s: socket tnkinfo not in prepared state\n",
				__func__);
		err = -EINVAL;
		goto out;
	}

	BUG_ON(t->entry != NULL);
	t->bytecount = 0;
	/*  t->state = TNKINFO_STATE_ESTABLISHED; */
	tnk_state_change(sk, sk->sk_tnkinfo.state, TNKINFO_STATE_ESTABLISHED);
	t->entry = NULL;

	TNK_DBG("tcp connection established\n");

out:
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);
	return err;
}

int tnk_tcp_close(struct sock *sk, int graceful)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	struct tcp_sock *tcp = tcp_sk(sk);
	int err = 0;
	unsigned long flags;

	TNK_DBG("%s begin\n", __func__);

	/*  This function is not re-entrant.  To prevent re-entrancy
	 *  we must lock with interrupts disabled.  Since this prevents
	 *  some functions we must unlock and relock before calling
	 *  sensitive functions.
	 *  */
	spin_lock_irqsave(&tnk_tcp_lock, flags);

	/*  If we had not yet reached active state then
	 *  just return, no need for us to do anything
	 *  on close.
	 */
	if (t->state < TNKINFO_STATE_ACTIVE)
		goto out;

	BUG_ON(t->entry == NULL);

	/* If FINs have been exchanged, don't wait for graceful shutdown */
	if (graceful &&
	    (sk->sk_state == TCP_LAST_ACK || sk->sk_state == TCP_CLOSING))
		graceful = 0;

	TNK_DBG("%s: sk %p chan %d graceful %d, sk_state %d\n",
		__func__, sk, t->entry->index, graceful, sk->sk_state);

	/*  TODO ensure this covers all end cases where data could
	 *  be in progress receive or transmit.
	 */
	if (t->state == TNKINFO_STATE_ACTIVE) {
		int tx_done_timeout = 0;
		/*  t->state = TNKINFO_STATE_STOPPING; */
		tnk_state_change(sk, sk->sk_tnkinfo.state,
				 TNKINFO_STATE_STOPPING);
		TNK_DBG("%s active, closing connection %d\n", __func__,
			t->entry->index);

		/*  Only attempt a graceful shutdown if the link is up */
		if (graceful
		    && netif_carrier_ok(stmmac_device_list[t->gmac_id]))
			tx_done_timeout = TNK_TCP_CLOSE_TIMEOUT;

		spin_unlock_irqrestore(&tnk_tcp_lock, flags);
		err = tnk_ct_stop(t->entry, tx_done_timeout);
		spin_lock_irqsave(&tnk_tcp_lock, flags);
		/*  t->state = TNKINFO_STATE_STOPPED; */
		tnk_state_change(sk, sk->sk_tnkinfo.state,
				 TNKINFO_STATE_STOPPED);
	}

	if (t->state == TNKINFO_STATE_STOPPED) {
		TNK_DBG("%s queue empty or abrupt, removing connection %d\n",
			__func__, t->entry->index);
		sk->sk_allocation = t->entry->sk_allocation;
		tnk_ct_remove(t->entry);

		/*
		 *  EXPERIMENTAL
		 *
		 *  If the transmit queue is not empty and we are closing
		 *  gracefully, then attempt to hand it back to linux.  The
		 *  presumtion is if it has not drained normally and we are
		 *  closing / idle timer has gone off then we should try to
		 *  keep the data if possible.
		 *
		 *  TODO - we should also hand back unack'd TTX DMA ring buffers
		 */

#if 0
		if (graceful && skb_queue_len(&t->entry->transmit) > 0) {
			pr_info("%s splice write queue, qlen=%d wmem=%d\n",
			       __func__, skb_queue_len(&sk->sk_transmit_queue),
			       sk->sk_wmem_queued);
			tcp->packets_out =
			    skb_queue_len(t->entry->transmit_queue);

			skb_queue_splice_init(&t->entry->transmit_queue,
					      &sk->sk_write_queue);
			sk->sk_send_head = sk->sk_write_queue;
		}
#endif

		/*  Even though the connection is closed as far as
		 *  TOE is concerned, it is still an active bypass
		 *  connection so set the state to established
		 *
		 *  This means that tnk_tcp_update can re-open
		 *  this connection later.  This may occur if,
		 *  for example, the connection was closed by
		 *  the idle timer handler due to a temporary
		 *  lack of activity on the connection
		 */
		t->entry = NULL;
		/*  t->state = TNKINFO_STATE_ESTABLISHED; */

		if (tcp->packets_out) {
			pr_warn("%s WARN - packets out is %d\n", __func__,
				tcp->packets_out);
			tcp->packets_out = 0;
		}

		tnk_state_change(sk, sk->sk_tnkinfo.state,
				 TNKINFO_STATE_ESTABLISHED);
		t->bytecount = 0;

		/*  Tell TCP to resume keep alive timer
		 */
		inet_csk_reset_keepalive_timer(sk,
					       keepalive_time_when(tcp_sk(sk)));

		/*  Unlock here, in case we need to call sk_data_ready below */
		spin_unlock_irqrestore(&tnk_tcp_lock, flags);

		if (!(sk->sk_socket))
			return err;

		/*  If the socket is waiting for Rx data, wake it up */
		if (test_bit(SOCK_ASYNC_WAITDATA, &sk->sk_socket->flags))
			sk->sk_data_ready(sk, 0);

		TNK_DBG("%s done\n", __func__);

		return err;
	}

out:
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);

	return err;
}

/*  Try to send data via the TOE.  If connection is
 *  not accelerated but is eligable for acceleration
 *  then try to create a new connection.  If we have
 *  a connection then send data to the TOE and return
 *  0 otherwise return EINVAL.
 */
int tnk_tcp_sendmsg(struct sock *sk, struct iovec *iov, int iovlen, int flags)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	int i;
	int total = 0;
	struct sk_buff *skb = NULL;
	struct tcp_sock *tcp = tcp_sk(sk);
	int copied = 0, err = 0;

	TNK_DBG("%s begin\n", __func__);

	for (i = 0; i < iovlen; i++)
		total += iov[i].iov_len;

	TNK_DBG("%s sending %d bytes\n", __func__, total);
	tnk_tcp_update(sk, total);

	if (t->state != TNKINFO_STATE_ACTIVE)
		return -EINVAL;

	BUG_ON(t->entry == NULL);

	CONTROL_DBG("%s: sk %p chan %d\n", __func__, sk, t->entry->index);

	/* Transmit throttling mechanism
	 * Stall here if we have a backlog (unless MSG_DONTWAIT is set)
	 */
	if (!sk_stream_memory_free(sk)) {
		long timeo = sock_sndtimeo(sk, flags & MSG_DONTWAIT);
		/*Avoid congestion causing return EAGAIN in short time*/
		if (timeo < USER_SNDTIMEO_LEVEL)
			timeo <<= 3;
		/* NOTE - SOCK_NOSPACE bit will be cleared when
		 * sk->sk_write_space() is called in tnkct.c */
		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
		err = sk_stream_wait_memory(sk, &timeo);
		if (err != 0) {
			if (err != -EAGAIN)
				return sk_stream_error(sk, flags, err);

			if (!sk_stream_memory_free(sk)) {
				/*  Don't return an error here, just return the
				 *  number of bytes copied so far.  If this is
				 *  zero, so be it.  A zero return will flag
				 *  rpc caller to sleep on write space
				 */
				CONTROL_DBG("%s: No write space on sk %p\n",
					    __func__, sk);
				tnk_conn_stats[t->entry->index].tx_block_num++;

				goto out;
			}
		}
	}

	skb = tnk_alloc_skb_fclone(total, sk->sk_allocation | __GFP_HIGH);

	if (!skb) {
		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
		pr_err("%s failed to alloc skb for %d bytes of data\n",
		       __func__, total);
		goto out;
	}

	if (total > tnk_conn_stats[t->entry->index].tx_max_data_size)
		tnk_conn_stats[t->entry->index].tx_max_data_size = total;

	if (unlikely(flags & MSG_OOB))
		TNK_DBG("%s: OOB flag was set\n", __func__);

	/* Ok commence sending. */
	copied = 0;

	while (--iovlen >= 0) {
		size_t seglen = iov->iov_len;
		unsigned char __user *from = iov->iov_base;

		TNK_DBG("%s proc iov seg %d len is %d\n",
			__func__, iovlen, (int)seglen);

		iov++;

		if (skb->truesize < seglen) {
			WARN(1, "%s seglen=%d, skb-len=%d, skb-headlen=%d"
			     " nfrags=%d truesize=%d total=%d\n",
			     __func__, seglen, skb->len, skb_headlen(skb),
			     skb_shinfo(skb)->nr_frags, skb->truesize, total);
		}

		if (copy_from_user(skb->data + copied, from, seglen)) {
			pr_err("%s failed to copy data\n", __func__);
			goto out;
		}
		skb_put(skb, seglen);

		copied += seglen;
		total -= seglen;
	}

	if (skb_shinfo(skb)->nr_frags > 0) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[0];

		pr_info("%s nfrag=%d headlen=%d truesize=%d\n",
			__func__, skb_shinfo(skb)->nr_frags,
			skb_headlen(skb), skb->truesize);
		pr_info("%s off=%d len=%d\n", __func__,
			frag->page_offset, frag->size);
	}
out:
	/*  TODO MGB this looks wrong but seems it cannot be hit anyway
	 */
	if (total)
		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);

	if (skb) {
		if (copied > 0) {
			TNK_DBG("%s transmitting...\n", __func__);

			tnk_ct_tx_backlog_enqueue(t->entry->index, skb);
		} else {
			tnk_free_skb(skb);
			CONTROL_DBG("%s aborting\n", __func__);
		}
	}

	if (copied > 0) {

		CONTROL_DBG("%s returning %d\n", __func__, copied);

		tcp->write_seq += copied;
		t->entry->sent += copied;
	}

	if (copied > 0)
		return copied;
	else
		return err;
}

/*  Try to send data via the TOE.  If connection is
 *  not accelerated return -EINVAL.  If it is
 *  accelerated but we have no space return 0.
 *  Otherwise return bytes queued.
 */
int tnk_tcp_sendpages(struct sock *sk, struct page **pages, int poffset,
		      size_t psize, int flags)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	struct sk_buff *skb = NULL;
	struct tcp_sock *tcp = tcp_sk(sk);
	int copied = 0;

	TNK_DBG("%s begin\n", __func__);

	tnk_tcp_update(sk, psize);

	if (t->state != TNKINFO_STATE_ACTIVE)
		return -EINVAL;

	BUG_ON(t->entry == NULL);

	CONTROL_DBG("%s: sk %p chan %d\n", __func__, sk, t->entry->index);

alloc_skb:
	/* Transmit throttling mechanism
	 * Stall here if we have a backlog (unless MSG_DONTWAIT is set)
	 */
	if (!sk_stream_memory_free(sk)) {
		int err;
		long timeo = sock_sndtimeo(sk, flags & MSG_DONTWAIT);
		/* NOTE - SOCK_NOSPACE bit will be cleared when
		 * sk->sk_write_space() is called in tnkct.c */
		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
		err = sk_stream_wait_memory(sk, &timeo);
		if (err != 0) {
			if (copied)
				goto out;

			if (err != -EAGAIN)
				return sk_stream_error(sk, flags, err);

			if (!sk_stream_memory_free(sk)) {
				/*  Don't return an error here, just return the
				 *  number of bytes copied so far.  If this is
				 *  zero, so be it.  A zero return will flag
				 *  rpc caller to sleep on write space
				 */
				CONTROL_DBG("%s: No write space on sk %p\n",
					    __func__, sk);
				goto out;
			}
		}
	}

	if (t->state != TNKINFO_STATE_ACTIVE)
		return -EINVAL;

	skb = tnk_alloc_skb_fclone(0, sk->sk_allocation);
	if (!skb) {
		pr_err("%s: failed to allocate skb\n", __func__);
		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
		/*  return 0; */
		goto out;
	}

	if (unlikely(flags & MSG_OOB))
		pr_warn("%s: OOB flag was set\n", __func__);

	TNK_DBG("%s psize %lu, poffset %d\n", __func__, (long unsigned)psize,
		poffset);

	while (psize > 0) {
		struct page *page = pages[poffset / PAGE_SIZE];
		int i;
		int offset = poffset % PAGE_SIZE;
		int size = min_t(size_t, psize, PAGE_SIZE - offset);

		TNK_DBG("%s offset = %d, size = %d\n", __func__, offset, size);
		i = skb_shinfo(skb)->nr_frags;
		if (i >= MAX_SKB_FRAGS) {
			TNK_DBG("%s: Hit MAX_SKB_FRAGS, allocating a new skb\n",
				__func__);
			tnk_ct_tx_backlog_enqueue(t->entry->index, skb);
			goto alloc_skb;
		}
		skb_fill_page_desc(skb, i, page, offset, size);
		get_page(page);

		skb->len += size;
		skb->data_len += size;
		skb->truesize += size;
		skb->ip_summed = CHECKSUM_PARTIAL;
		skb_shinfo(skb)->gso_segs = 0;

		copied += size;
		poffset += size;
		psize -= size;
	}

out:
	if (skb) {
		if (copied > 0) {
			TNK_DBG("%s transmitting...\n", __func__);

			tnk_ct_tx_backlog_enqueue(t->entry->index, skb);
		} else {
			tnk_free_skb(skb);
			CONTROL_DBG("%s aborting\n", __func__);
			return 0;
		}
	}

	if (copied > 0) {

		CONTROL_DBG("%s returning %d\n", __func__, copied);

		tcp->write_seq += copied;
		t->entry->sent += copied;
	}
	return copied;
}

/*  Try to receive data from the TOE.  If connection is
 *  not accelerated but is eligable for acceleration
 *  then try to create a new connection.  If we have
 *  a connection then recieve data to the TOE and return
 *  0 otherwise return 1.  If we are accelerated but no
 *  data is available return -EAGAIN (caller should block)
 */
int tnk_tcp_receive(struct sock *sk, uint32_t * seq,
		    long *timeo, int target, struct iovec *iov,
		    int *has_copied, size_t len, int fin_flag)
{
	struct tnkinfo *t;
	struct sk_buff *skb = NULL;
	int err = 0;
	int copied = 0;
	size_t seglen;
	unsigned consumed_skbs = 0;
	struct tnkcb *cb = NULL;
	unsigned long flags;
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
	int wnd_update_flag = 0;
#endif
	TNK_DBG("%s begin\n", __func__);

	if (sk == NULL) {
		pr_info("%s:%d: sk struct is NULL.\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (seq == NULL) {
		pr_info("%s:%d: seq is NULL.\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (iov == NULL) {
		pr_info("%s:%d: iov is NULL.\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (len < 1) {
		pr_info("%s:%d: length is too short.\n",  __func__, __LINE__);
		return -EINVAL;
	}

	t = &sk->sk_tnkinfo;
	seglen = len;

	CONTROL_DBG("%s tnkinfo state = %d, sk=%p\n", __func__, t->state, sk);

	if (t->state < TNKINFO_STATE_ACTIVATING) {
		TNK_DBG("%s not active, do slowpath\n", __func__);
		return -EINVAL;
	}

	BUG_ON(t->entry == NULL);

	TNK_DBG("%s: sk %p chan %d\n", __func__, sk, t->entry->index);

	TNK_DBG("%s start to receive - len=%d\n", __func__, (int)len);
	TNK_DBG("%s seglen=%d\n", __func__, seglen);

	while (seglen > 0) {
		int size;

		/*
		TNK_DBG("%s waiting for receive data\n", __func__);
		if (copied == 0 && !tnk_tcp_wait_data(sk, &timeo))
			goto out;
*/

		/*  Entry was set to NULL while we were asleep,
		 *  return -EINVAL to resume slow path processing
		 */
		spin_lock_irqsave(&tnk_tcp_lock, flags);

		if ((t->entry == NULL) ||
		    ((t->state != TNKINFO_STATE_ACTIVATING) &&
		     (t->state != TNKINFO_STATE_ACTIVE))) {
			TNK_DBG("%s not active, do slowpath\n", __func__);
			err = -EINVAL;
		} else if (!skb_queue_empty(&sk->sk_receive_queue)
				&& (!fin_flag)) {
			TNK_DBG
			    ("%s sk_receive_queue not yet empty, do slowpath\n",
			     __func__);
			err = -EINVAL;
		}

		spin_unlock_irqrestore(&tnk_tcp_lock, flags);

		if (err)
			goto out;

		if (skb == NULL)
			skb = skb_dequeue(&t->entry->receive_queue);

		if (skb == NULL) {
			TNK_DBG("%s no more recv bufs for connection %d\n",
				__func__, t->entry->index);
			if ((*has_copied + copied) >= target)
				goto out;
			if (fin_flag) {
				err = -EINVAL;
				goto out;
			}
			if (!tnk_tcp_wait_data(sk, timeo))
				goto out;
			continue;
		}

		cb = (struct tnkcb *)skb->cb;
		BUG_ON(cb == NULL || cb->magic != TNK_MAGIC);

		/*  Full packet received.  Either a FIN or a RST.  Hand
		 *  it off to the slow path for handling and break out.
		 */
		if (cb->type != TNK_TYPE_DATA) {
			/* If we have already copied data here,
			 * we need to return it to the caller BEFORE
			 * sending this full packet up the stack.
			 * otherwise it will be effectively processed
			 * out-of-order
			 */
			if (copied)
				goto out;

			TNK_DBG("%s packet type %d, cindex %d, sk %p\n",
				__func__, cb->type, t->entry->index, sk);

			TNK_DBG("%s non data (type %d) packet found, send to"
				" stack, rcv_nxt=%x -copied_seq=%d rx_q %d\n",
				__func__, cb->type, tcp_sk(sk)->rcv_nxt,
				tcp_sk(sk)->rcv_nxt - tcp_sk(sk)->copied_seq,
				skb_queue_len(&t->entry->receive_queue));
			tnk_mem_read_dequeued(skb);
			consumed_skbs++;
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
			wnd_update_flag
				= tnk_ct_rx_release(t->entry->index, skb->len);
#endif
			/*  tnk_ct_update_rx_seq (t->entry); */
			if (cb->type == TNK_TYPE_FIN)
				tnkhw_skb_recycle(skb);
			else
				netif_receive_skb(skb);
			skb = NULL;
			/* lock_sock (sk); */
			err = -EINVAL;
			goto out;
		}

		size = min_t(size_t, seglen, skb->len);
		TNK_DBG("%s size = %d\n", __func__, size);
		TNK_DBG("%s skb->len:  %d\n", __func__, skb->len);

		err = skb_copy_datagram_iovec(skb, 0, iov, size);
		TNK_DBG("%s copy with size=%d\n", __func__, size);

		if (err)
			goto out;

		*seq += size;	/*  This is usually tcp->copied_seq */
		seglen -= size;
		copied += size;

		skb_pull(skb, size);

		if (skb->len == 0) {
			TNK_DBG("%s fully consumed, unlinking skb\n", __func__);
			/*  (TODO - flags & MSG_PEEK not supported here yet) */
			tnk_mem_read_dequeued(skb);
			consumed_skbs++;
			free_skb++;
			tnkhw_skb_recycle(skb);
			skb = NULL;
			TNK_DBG("%s: free_skb counter: %d\n", __func__,
				free_skb);
		} else {
			TNK_DBG("%s skb not fully consumed, size now %d\n",
				__func__, skb->len);
		}
	}

out:
	spin_lock_irqsave(&tnk_tcp_lock, flags);
	/*  If we've sent a FIN or RST up the stack, or if we've been
	 *  interrupted, our state may have changed so check it again
	 *  with locks held */
	if (t->entry &&
	    ((t->state == TNKINFO_STATE_ACTIVATING) ||
	     (t->state == TNKINFO_STATE_ACTIVE))) {
		if (likely(copied)) {
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
			wnd_update_flag
				= tnk_ct_rx_release(t->entry->index, copied);
#endif
		}
		t->entry->recv += copied;

		/*  skb not fully consumed, connection still active, so
		 *  requeue it */
		if (skb)
			skb_queue_head(&t->entry->receive_queue, skb);
	} else if (skb) {
		/*  skb not fully consumed, but connection not active, so
		 *  drop it */
		TNK_DBG("%s: skb not fully consumed (%d left)\n", __func__,
			skb->len);
		tnkhw_skb_recycle(skb);
		consumed_skbs++;
	}
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);

#ifndef TNK_RX_CHANNEL_FLOW_CONTROL
	if (consumed_skbs)
		tnk_ct_rx_release(consumed_skbs);
#endif
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
	if (wnd_update_flag) {
		struct tnkhw_connection conn;
		tnkhw_connection_get(t->entry->index, &conn);
		tnk_send_ack(sk, conn.next_rx_seq_num, copied);
	}
#endif
	*has_copied += copied;
		return err;
}

int tnk_tcp_read_sock(struct sock *sk, read_descriptor_t * desc,
		      tnk_read_actor_t recv_actor)
{
	struct tnkinfo *t;
	struct sk_buff *skb = NULL;
	struct tcp_sock *tp;
	u32 seq;
	int offset = 0;
	int copied = 0;
	unsigned consumed_skbs = 0;
	struct tnkcb *cb = NULL;
	int err = 0;
	unsigned long flags;
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
	int wnd_update_flag = 0;
#endif

	TNK_DBG("%s begin\n", __func__);
	BUG_ON(sk == NULL);
	BUG_ON(desc == NULL);

	tp = tcp_sk(sk);
	BUG_ON(tp == NULL);
	seq = tp->copied_seq;
	t = &sk->sk_tnkinfo;

	TNK_DBG("%s tnkinfo state = %d, sk=%p\n", __func__, t->state, sk);

	spin_lock_irqsave(&tnk_tcp_lock, flags);

	if ((t->state != TNKINFO_STATE_ACTIVATING) &&
	    (t->state != TNKINFO_STATE_ACTIVE)) {
		TNK_DBG("%s not active, do slowpath\n", __func__);
		err = -EINVAL;
	} else if (!skb_queue_empty(&sk->sk_receive_queue)) {
		TNK_DBG("%s sk_receive_queue not yet empty, do slowpath\n",
			__func__);
		err = -EINVAL;
	}

	spin_unlock_irqrestore(&tnk_tcp_lock, flags);

	if (err)
		return err;

	BUG_ON(t->entry == NULL);

	TNK_DBG("%s: sk %p chan %d\n", __func__, sk, t->entry->index);

	while ((skb != NULL) ||
	       (skb = skb_dequeue(&t->entry->receive_queue))) {
		int used;

		TNK_DBG("%s dequeued %d bytes\n", __func__, skb->len);

		cb = (struct tnkcb *)skb->cb;
		BUG_ON(cb == NULL || cb->magic != TNK_MAGIC);

		/*  Full packet received.  Either a FIN or a RST.  Hand
		 *  it off to the slow path for handling and break out.
		 */
		if (cb->type != TNK_TYPE_DATA) {
			/* If we have already copied data here,
			 * we need to return it to the caller BEFORE
			 * sending this full packet up the stack.
			 * otherwise it will be effectively processed
			 * out-of-order
			 */
			if (copied)
				goto out;

			TNK_DBG("%s packet type %d, cindex %d, sk %p,"
				" rcv_nxt %x\n",
				__func__, cb->type, t->entry->index, sk,
				tp->rcv_nxt);

			TNK_DBG("%s non data packet found, send to stack\n",
				__func__);
			tnk_mem_read_dequeued(skb);
			consumed_skbs++;
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
			wnd_update_flag
				= tnk_ct_rx_release(t->entry->index, skb->len);
#endif
			netif_receive_skb(skb);
			skb = NULL;    /*  Linux stack will free the SKB */
			err = -EINVAL;
			goto out;
		}

		used = recv_actor(desc, skb, 0, skb->len);
		TNK_DBG("%s actor with len=%d used %d\n",
			__func__, skb->len, used);

		if (used < 0) {
			err = used;
			break;
		} else if (used <= skb->len) {
			seq += used;
			copied += used;
			offset += used;
			skb_pull(skb, used);
		} else {
			pr_warn("%s used more than a full skb??\n", __func__);
			skb_pull(skb, skb->len);
		}

		if (skb->len == 0) {
			TNK_DBG("%s fully consumed, unlinking skb\n", __func__);
			tnk_mem_read_dequeued(skb);
			consumed_skbs++;
			tnkhw_skb_recycle(skb);
			skb = NULL;
			free_skb++;
		} else {
			TNK_DBG("%s skb not fully consumed, size now %d\n",
				__func__, skb->len);
		}

		if (!desc->count) {
			TNK_DBG("%s no more space\n", __func__);
			break;
		}

		tp->copied_seq = seq;
	}
	tp->copied_seq = seq;

out:
	spin_lock_irqsave(&tnk_tcp_lock, flags);
	/*  If we've sent a FIN or RST up the stack, or if we've been
	 *  interrupted, Our state may have changed so check it again
	 *  with locks held */
	if (t->entry &&
	    ((t->state == TNKINFO_STATE_ACTIVATING) ||
	     (t->state == TNKINFO_STATE_ACTIVE))) {
		if (likely(copied)) {
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
			wnd_update_flag
				= tnk_ct_rx_release(t->entry->index, copied);
#endif
			t->entry->recv += copied;
		}

		/*  skb not fully consumed, connection still active, so
		 *  requeue it */
		if (skb)
			skb_queue_head(&t->entry->receive_queue, skb);
	} else if (skb) {
		/*  skb not fully consumed, but connection not active,
		 *  so drop it */
		tnkhw_skb_recycle(skb);
		consumed_skbs++;
	}
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);

#ifndef TNK_RX_CHANNEL_FLOW_CONTROL
	if (consumed_skbs)
		tnk_ct_rx_release(consumed_skbs);
#endif
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
	if (wnd_update_flag) {
		struct tnkhw_connection conn;
		tnkhw_connection_get(t->entry->index, &conn);
		tnk_send_ack(sk, conn.next_rx_seq_num, copied);
	}
#endif
	if (copied)
		return copied;
	else
		return err;
}

int tnk_tcp_update(struct sock *sk, int len)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	struct tcp_sock *tcp = tcp_sk(sk);
	unsigned long flags;
	struct sk_buff *skb;

	TNK_DBG("%s begin\n", __func__);
	TNK_DBG("%s: Got sk %p\n", __func__, sk);

	if (!hitoe)
		return 0;

	/*  Update can be called by either socket or splice read
	 *  functions.  Make sure to lock with irq off to prevent
	 *  nested locks.
	 *
	 *  NOTE a better way to fix this problem would be to maintain
	 *  a lock per socket
	 */

	spin_lock_irqsave(&tnk_tcp_lock, flags);

	t->bytecount += len;
	TNK_DBG("%s sk_info update %d len\n", __func__, len);

	if (t->state != TNKINFO_STATE_ESTABLISHED ||
	    t->entry != NULL || t->bytecount < tnk_threshold) {
		goto out;
	}

	t->entry = tnk_get_free_entry(sk);
	if (!t->entry)
		goto out;

	/* At this point, we need to take a snapshot of the connection state
	 * for handover to TOE.  This includes the next rx sequence number,
	 * so we use this state to show that this process has begun and the
	 * kernel must drop any subsequent packets received via the slow-path
	 * for this connection.  See tcp_rcv_established()
	 * in linux/net/ipv4/tcp_input.c */
	/*  t->state = TNKINFO_STATE_ACTIVATING; */
	tnk_state_change(sk, sk->sk_tnkinfo.state, TNKINFO_STATE_ACTIVATING);

	/* Make sure there's no packets in the out-of-order queue at this
	 * point, or they'll give us a nasty suprise later when the kernel
	 * digests them! */
	skb_queue_purge(&tcp->out_of_order_queue);

	/* Same with the prequeue (although it's less harmful) */
	skb_queue_purge(&tcp->ucopy.prequeue);
	tcp->ucopy.memory = 0;

	/* And same with the backlog queue.  This isn't a regular
	 * skb queue, so needs to be handled differently */

	if (sk->sk_lock.owned)
		spin_lock(&sk->sk_lock.slock);

	if (sk->sk_backlog.tail) {
		skb = sk->sk_backlog.head;
		sk->sk_backlog.head = sk->sk_backlog.tail = NULL;

		while (skb) {
			struct sk_buff *next = skb->next;

			skb->next = NULL;
			kfree_skb(skb);
			skb = next;
		}
		sk->sk_backlog.len = 0;
	}
	if (sk->sk_lock.owned)
		spin_unlock(&sk->sk_lock.slock);
	/*  Stop any outstanding timers on this socket, such
	 *  as retransmit.  This is now done by TOE.
	 *
	 *  Stop timers *before* create connection to make sure
	 *  none go off while we are creating.
	 */
	TNK_DBG("%s clear tcp timer\n", __func__);
	inet_csk_clear_xmit_timers(sk);

	/* Update the send sequence number */
	if (!skb_queue_empty(&sk->sk_write_queue)) {
		skb = skb_peek(&sk->sk_write_queue);
		tcp->snd_nxt = TCP_SKB_CB(skb)->seq;
		tcp->snd_una = TCP_SKB_CB(skb)->seq;
	}
	/* Reset tcp protocol stack some infomation,avoid warning */
	tcp->fackets_out = tcp->sacked_out = 0;
	tcp->retrans_out = 0;
	tcp->lost_out = 0;
	tcp->undo_marker = 0;
	tcp->undo_retrans = 0;
	tcp_clear_all_retrans_hints(tcp);

	TNK_DBG("%s threshold seen count=%d, create entry\n", __func__,
		t->bytecount);
	t->entry = tnk_ct_create(sk, t->entry);

	/*  Increase sndbuf significantly to allow TOE to function better
	 */
	sk->sk_sndbuf <<= 3;

	if (t->entry == NULL) {
		TNK_DBG("%s no space for accel connection\n", __func__);
		/*  t->state = TNKINFO_STATE_ESTABLISHED; */
		tnk_state_change(sk, sk->sk_tnkinfo.state,
				 TNKINFO_STATE_ESTABLISHED);
		goto out;
	}

	CONTROL_DBG("%s entry %d created\n", __func__, t->entry->index);

	t->entry->sent = t->entry->recv = 0;

	/*  We over-ride default allocation scheme for TOE connections
	 *  with atomic but remember the previous setting for when
	 *  we hand back to linux
	 */
	t->entry->sk_allocation = sk->sk_allocation;
	/* sk->sk_allocation = GFP_ATOMIC; */

	/*  Any transmitted data that is outstanding needs to be
	 *  retransmitted by the TOE.  This is because from now
	 *  on acks (or lack of) will go to the TOE and so the
	 *  TOE must be the one to free the skbs or do retransmission.
	 */
	if (!skb_queue_empty(&sk->sk_write_queue)) {
		TNK_DBG("%s transfer write queue %d\n", __func__,
			skb_queue_len(&sk->sk_write_queue));
	}

	tcp->packets_out = 0;

	/*  skb_queue_splice_init(&sk->sk_write_queue,
	    &t->entry->transmit_queue); */
	while ((skb = skb_dequeue(&sk->sk_write_queue)) != NULL) {
		unsigned long flags;

		/* Lock here because tnk_ct_tx_backlog_flush() may already
		 * be draining the TX queue and updating the same variables
		 */
		spin_lock_irqsave(&t->entry->tx_q_lock, flags);
		sk->sk_wmem_queued -= skb->truesize;
		sk_mem_uncharge(sk, skb->truesize);
		spin_unlock_irqrestore(&t->entry->tx_q_lock, flags);

		tnk_ct_tx_backlog_enqueue(t->entry->index, skb);
		tnk_mem_write_queued(skb);
	}

	if (tcp_send_head(sk)) {
		TNK_DBG("%s: Setting tcp_send_head to NULL\n", __func__);
		sk->sk_send_head = NULL;
	}

	/*  t->state = TNKINFO_STATE_ACTIVE; */
	tnk_state_change(sk, sk->sk_tnkinfo.state, TNKINFO_STATE_ACTIVE);

 out:
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);

	return 0;
}

int tnk_tcp_check_fin(struct sock *sk)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	struct tnkentry *e = t->entry;
	unsigned long flags;
	int tnk_state = 0;

	spin_lock_irqsave(&tnk_tcp_lock, flags);
	if (t->state != TNKINFO_STATE_ACTIVE)
		goto out;

	if (skb_queue_empty(&e->receive_queue))
		goto out;

	tnk_state = 1;
out:
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);
	return tnk_state;
}

int tnk_tcp_check_connect_state(struct sock *sk)
{
	struct tnkinfo *t = &sk->sk_tnkinfo;
	unsigned long flags;
	int tnk_state = 0;

	spin_lock_irqsave(&tnk_tcp_lock, flags);
	if (t->state != TNKINFO_STATE_ACTIVE)
		goto out;

	tnk_state = 1;
out:
	spin_unlock_irqrestore(&tnk_tcp_lock, flags);
	return tnk_state;
}
