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

struct tnkcb {
	unsigned magic;
	int type;
	dma_addr_t dma;
	unsigned flag;
};
#define TNK_MAGIC 0x6ac169f3

enum tnkhw_cb_flags {
	TNKHW_CB_FLAG_NONE = 0,
	TNKHW_CB_FLAG_PREMATURE_EOP = (1 << 0),
	TNKHW_CB_FLAG_BAD_ETHERTYPE = (1 << 1),
	TNKHW_CB_FLAG_BAD_DST_MACADDR = (1 << 2),
	TNKHW_CB_FLAG_TCP_CSUM_ERR = (1 << 3),
	TNKHW_CB_FLAG_IPHDR_CSUM_ERR = (1 << 4),
	TNKHW_CB_FLAG_NON_TCP_PROT = (1 << 5),
	TNKHW_CB_FLAG_DST_IP_MISMATCH = (1 << 6),
	TNKHW_CB_FLAG_CONNECTION_ERR = (1 << 7),
	TNKHW_CB_FLAG_RX_SEQNUM_ERR = (1 << 8),
	TNKHW_CB_FLAG_PKT_DROPPED = (1 << 9),
	TNKHW_CB_FLAG_RESERVED = (1 << 10),
	TNKHW_CB_FLAG_FULL_PKT = (1 << 11),
	TNKHW_CB_FLAG_TOE_ERR = (1 << 12),
	TNKHW_CB_FLAG_URG = (1 << 13),
	TNKHW_CB_FLAG_ACK = (1 << 14),
	TNKHW_CB_FLAG_PSH = (1 << 15),
	TNKHW_CB_FLAG_RST = (1 << 16),
	TNKHW_CB_FLAG_SYN = (1 << 17),
	TNKHW_CB_FLAG_FIN = (1 << 18),
};

#define TNK_TYPE_DATA   0
#define TNK_TYPE_FIN    1
#define TNK_TYPE_RST    2
#define TNK_TYPE_FREE   3

/* Socket is created but not connected or accelerated */
#define TNKINFO_STATE_OPEN        0
/* Socket is created and has a connection but not accelerated */
#define TNKINFO_STATE_PREPARED    1
/* Socket is created and has a connection but not accelerated */
#define TNKINFO_STATE_ESTABLISHED 2
/* Socket is connected and acceleration handover has begun */
#define TNKINFO_STATE_ACTIVATING  3
 /* Socket is connected and accelerated */
#define TNKINFO_STATE_ACTIVE      4
/* Socket is connected but no longer accelerated */
#define TNKINFO_STATE_STOPPING    5
/* Socket is connected but no longer accelerated */
#define TNKINFO_STATE_STOPPED     6
#define TNKINFO_STATE_CLOSED      7

#define TNK_DESTROY_CLOSE	0
#define TNK_DESTROY_SHUTDOWN	1

enum tnk_update_path {
	UPDATE_PATH_UNKNOWN = 0,
	UPDATE_PATH_SEND,
	UPDATE_PATH_RECV
};

struct tnkinfo {
	int state;
	int bytecount;
	unsigned int snd_initial_seq;
	/* used by kernel path to decide whether allowed to use toe */
	int enable;
	struct tnkentry *entry;
	struct net_device *dev;
	uint8_t rmac[6];
	int gmac_id;
	struct timer_list idle_timer;
	int finflag;
	int rstflag;
	int reetrant;
	int howto_destroy;
	struct tnk_tcp_close_work *close_work;
	int update_path;
	/* used by user settings , some socket setting use special
	   function, but toe can't afford it , so it's not allowed
	   to use toe, for example,when traffic control is set,toe
	   can not be used, the initial value is zero .
	 */
	int not_capable;
};

struct tnkfuncs {
	int (*tcp_prepare) (struct sock *sk, struct sk_buff *skb, int inbound);
	int (*tcp_open) (struct sock *sk);
	int (*tcp_close) (struct sock *sk, int graceful);
	int (*tcp_reset) (struct sock *sk);
	int (*tcp_sendpages) (struct sock *s, struct page **pages,
			int poffset, size_t psize, int flags);
	int (*tcp_sendmsg) (struct sock *s, struct iovec *iov, int iovlen,
			int flags);
	int (*tcp_receive) (struct sock *sk, uint32_t *seq, long *timeo,
			int target, struct iovec *to, int *has_copied,
			size_t *len, int fin_flag, int sk_flags);
	int (*tcp_read_sock) (struct sock *sk, read_descriptor_t *desc,
			tnk_read_actor_t recv_actor, int *has_copied);
	int (*tcp_update) (struct sock *s, int len);
	int (*tcp_wait_data) (struct sock *sk, long *timeo);
	int (*tcp_check_fin) (struct sock *sk);
	int (*tcp_check_connect_state) (struct sock *sk);
	void (*tcp_disable_rcv) (struct sock *sk);
	int (*tcp_recv_queue_data_size) (struct sock *sk);
	void (*tcp_sync_mss) (struct sock *sk);
	void (*config_tcp_retries2) (int tcp_retries2);
};

void tcp_register_tnk_funcs(struct tnkfuncs *f);

#endif

