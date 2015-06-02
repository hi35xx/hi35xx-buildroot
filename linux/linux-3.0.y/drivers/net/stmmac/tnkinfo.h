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
#define TNK_TCP_PERSIST_TIME  (10*HZ)
#define TNK_TCP_RTT_SAMPLE_INTERVAL (10*HZ)
#define TNK_THRESHOLD   10240
#define TNK_POLL_LIMIT  8
#define TNK_TX_TIMEOUT_MIN  300
#define TNK_TX_TIMEOUT_MAX  120000

#define TNK_RX_CHANNEL_FLOW_CONTROL
#ifdef TNK_RX_CHANNEL_FLOW_CONTROL
#define TNK_RX_Q_LIMIT 65535
#else
#define TNK_RX_SW_Q_SIZE 2048
#endif

struct tnkentry {
	struct tnkentry *next;
	struct tnkentry *prev;
	struct sock *sk;
	struct sk_buff_head receive_queue;
	struct sk_buff_head transmit_queue;
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
};

#endif
