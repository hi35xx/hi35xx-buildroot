/*
 *  COPYRIGHT NOTICE
 *
 *  Copyright 2011 Emutex Ltd. All rights reserved.
 *
 *  tnk info structure
 */

#ifndef TNKINFO_H
#define TNKINFO_H

#include <net/tcp.h>

#ifndef UNIT_TEST
#include <net/tnkdrv.h>
#endif

#define TNK_MAX_CONNECTIONS 2048
#define TNK_TCP_CLOSE_TIMEOUT TCP_TIMEWAIT_LEN /* 60s */
#define TNK_TIME_MS_TO_NS		(1000000ULL)
#define TNK_SHUTDOWN_WAIT_TIME	(2000*TNK_TIME_MS_TO_NS)
#define TNK_TCP_PERSIST_TIME  (1*HZ)
#define TNK_TCP_RTT_SAMPLE_INTERVAL (10*HZ)
#define TNK_THRESHOLD   0
#define TNK_POLL_LIMIT  64
#define TNK_TX_TIMEOUT_MIN  300
#define TNK_TX_TIMEOUT_MAX  120000
#define TNK_TX_RTT_FILITER  200

#define TNK_RX_NAPI_LIMIT	16
#define TNK_RX_CHANNEL_FLOW_CONTROL
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
#define TNK_RX_Q_LIMIT (65535)
#else
#define TNK_RX_SW_Q_SIZE 2048
#endif

#define TNK_SEND_MIN_MSGSIZE	(1024)
#define TNK_SEND_MAX_MSGSIZE	(4 * 1024)
#define TNK_REQUST_SIZE(X)	SKB_DATA_ALIGN(SKB_WITH_OVERHEAD(X))
#define TNK_REAL_SIZE(X)	(SKB_DATA_ALIGN(X) + \
				SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

struct tnkentry {
	struct tnkentry *next;
	struct tnkentry *prev;
	struct sock *sk;
	struct sk_buff_head receive_queue;
	struct sk_buff_head transmit_queue;
	struct sk_buff_head out_of_order_queue;
	struct timer_list zero_window_probe_timer;
	unsigned int last_ack;
	unsigned int last_snd_nxt;
	int index;
	int data_ready;
	unsigned initial_seq;
	unsigned initial_ack;
	int txremain;
	int timer;
	uint32_t mac_h;
	uint32_t mac_l;
	unsigned long next_rtt_poll;
	unsigned txtimeout;
	int sent;
	int recv;
	int last_sent;
	int last_recv;
	int rate_sent;
	int rate_recv;
	char *info;
	spinlock_t list_lock;
	spinlock_t tx_q_lock;
	spinlock_t adv_wnd_lock;
	int sk_allocation;
	long max_retries_exceeded;
	/* Waitqueue for sleeping during graceful tx shutdown */
	wait_queue_head_t waitqueue;
	/* Flag indicating receipt of RST packet for this connection. */
	int rst_received;
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
	atomic_t rx_q_bytes;
	unsigned adv_wnd;
#endif
	int sk_rcv_disable;
	int local_ip_pos;
	int ct_hw_removed;
};

#endif
