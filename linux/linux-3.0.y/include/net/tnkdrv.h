/*
 *  COPYRIGHT NOTICE
 *
 *  Copyright 2011 Emutex Ltd. All rights reserved.
 *
 *  tnk info structure
 */

#ifndef TNKDRV_H
#define TNKDRV_H

#include <linux/fs.h>

typedef int (*tnk_read_actor_t)(read_descriptor_t *, struct sk_buff *,
				unsigned int, size_t);

#ifndef CONFIG_TNK
#error "You must enable the CONFIG_TNK kernel configuration option"
#endif

/* Forward declarations required below */
struct tnkentry;
struct net_device;
struct sk_buff;
struct sock;

#define TNKINFO_STATE_OPEN        0 /* Socket is created but not connected or accelerated    */
#define TNKINFO_STATE_PREPARED    1 /* Socket is created and has a connection but not accelerated    */
#define TNKINFO_STATE_ESTABLISHED 2 /* Socket is created and has a connection but not accelerated    */
#define TNKINFO_STATE_ACTIVATING  3 /* Socket is connected and acceleration handover has begun       */
#define TNKINFO_STATE_ACTIVE      4 /* Socket is connected and accelerated              */
#define TNKINFO_STATE_STOPPING    5 /* Socket is connected but no longer accelerated              */
#define TNKINFO_STATE_STOPPED     6 /* Socket is connected but no longer accelerated              */
#define TNKINFO_STATE_CLOSED      7

struct tnkinfo
{
	int state;
	int bytecount;
	struct tnkentry *entry;
	struct net_device *dev;
	uint8_t rmac[6];
	int gmac_id;
	struct timer_list idle_timer;
	int finflag;
	int rstflag;
};

struct tnkfuncs
{
	int (*tcp_prepare) (struct sock *sk, struct sk_buff *skb, int inbound);
	int (*tcp_open) (struct sock *sk);
	int (*tcp_close) (struct sock *sk, int graceful);
	int (*tcp_sendpages) (struct sock *s, struct page **pages,
			int poffset, size_t psize, int flags);
	int (*tcp_sendmsg) (struct sock *s, struct iovec *iov,
			int iovlen, int flags);
	int (*tcp_receive) (struct sock *sk, uint32_t *seq, long *timeo,
			int target, struct iovec *to, int *has_copied,
			size_t len, int fin_flag);
	int (*tcp_read_sock) (struct sock *sk, read_descriptor_t* desc,
			tnk_read_actor_t recv_actor);
	int (*tcp_update) (struct sock *s, int len);
	int (*tcp_wait_data) (struct sock *sk, long *timeo);
	int (*tcp_check_fin) (struct sock *sk);
	int (*tcp_check_connect_state) (struct sock *sk);
};

void tcp_register_tnk_funcs(struct tnkfuncs *f);

#endif

