#ifndef __HIETH_TSO_H
#define __HIETH_TSO_H

#define TSO_FLAG	(1 << 31)
#define VLAN_FLAG	(1 << 30)
#define IPV6_FLAG	(1 << 29)
#define UDP_FLAG	(1 << 28)
#define TX_CSUM_FLAG	(1 << 27)
#define SG_FLAG		(1 << 26)

#define PKT_IPV6_HDR_LEN        (10)
#define PKT_UDP_HDR_LEN         (2)
#define WORD_TO_BYTE            (4)

enum {
	PKT_NORMAL,
	PKT_SG
};

enum {
	PKT_IPV4,
	PKT_IPV6
};

enum {
	PKT_TCP,
	PKT_UDP
};

struct frags_info {
	/* Word(2*i+2) */
	u32 addr;
	/* Word(2*i+3) */
	u32 size:16;
	u32 reserved:16;
};

struct dma_tx_desc {
	/* Word0 */
	u32 total_len:17;
	u32 reserv:15;
	/* Word1 */
	u32 ipv6_id;
	/* Word2 */
	u32 linear_addr;
	/* Word3 */
	u32 linear_len:16;
	u32 reserv3:16;
	/* MAX_SKB_FRAGS = 17 */
	struct frags_info frags[30];
	/* struct frags_info frags[MAX_SKB_FRAGS]; */
};

struct tx_pkt_info {
	union {
		struct {
			u32 data_len:11;
			u32 nfrags_num:5;
			u32 prot_hdr_len:4;
			u32 ip_hdr_len:4;
			u32 reserved:2;
			u32 sg_flag:1;
			u32 coe_flag:1;
			u32 prot_type:1;
			u32 ip_ver:1;
			u32 vlan_flag:1;
			u32 tso_flag:1;
		} info;
		u32 val;
	} tx;
	u32 tx_addr; /* normal pkt, skb->data */
	u32 sg_desc_offset; /* TSO pkt, desc addr */
	struct sk_buff *skb;
};

#endif
