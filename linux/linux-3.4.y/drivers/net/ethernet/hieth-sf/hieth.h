#ifndef __HIETH_H
#define __HIETH_H

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/list.h>
#include <linux/phy.h>
#include <linux/io.h>
#include <mach/platform.h>
#include <mach/irqs.h>
#include <linux/interrupt.h>

#define HIETH_TSO_SUPPORTED
#define HIETH_TSO_DEBUG
#define HIETH_RXCSUM_SUPPORTED

#ifdef HIETH_TSO_SUPPORTED
#include "tso.h"
#endif

#define HIETH_MAX_QUEUE_DEPTH	64

#define HIETH_MIIBUS_NAME	"himii"
#define HIETH_DRIVER_NAME	"hieth"

#define HIETH_MAX_RX_HEAD_LEN	(10000)  /* max skbs for rx */
#define HIETH_MAX_FRAME_SIZE	(1600)

#define HIETH_MAX_MAC_FILTER_NUM	8
#define HIETH_MAX_UNICAST_ADDRESSES	2
#define HIETH_MAX_MULTICAST_ADDRESSES	(HIETH_MAX_MAC_FILTER_NUM - \
		HIETH_MAX_UNICAST_ADDRESSES)

#define hieth_trace(level, msg...) do { \
	if ((level) >= CONFIG_HIETH_TRACE_LEVEL) { \
		pr_info("hieth_trace:%s:%d: ", __func__, __LINE__); \
		printk(msg); \
		printk("\n"); \
	} \
} while (0)

#define hieth_error(s...) do { \
	pr_err("hieth:%s:%d: ", __func__, __LINE__); \
	pr_err(s); \
	pr_err("\n"); \
} while (0)

#define hieth_assert(cond) do { \
	if (!(cond)) {\
		pr_err("Assert:hieth:%s:%d\n", \
			__func__, \
			__LINE__); \
		BUG(); \
	} \
} while (0)

#define hieth_dump_buf(level, buf, len) do {\
	int i;\
	char *p = (void *)(buf);\
	if ((level) >= CONFIG_HIETH_TRACE_LEVEL) { \
		pr_info("%s->%d, buf=0x%.8x, len=%d\n", \
				__func__, __LINE__, \
				(int)(buf), (int)(len)); \
		for (i = 0; i < (len); i++) {\
			pr_info("0x%.2x ", *(p+i));\
				if (!((i+1) & 0x07))\
						pr_info("\n");\
		} \
		pr_info("\n");\
	} \
} while (0)

#define FC_ACTIVE_MIN		1
#define FC_ACTIVE_DEFAULT	3
#define FC_ACTIVE_MAX		31
#define FC_DEACTIVE_MIN		1
#define FC_DEACTIVE_DEFAULT	5
#define FC_DEACTIVE_MAX		31

extern int tx_flow_ctrl_en;
extern int tx_flow_ctrl_active_threshold;
extern int tx_flow_ctrl_deactive_threshold;

/* port */
#define UP_PORT		0
#define DOWN_PORT	1
#define NO_EEE		0
#define MAC_EEE		1
#define PHY_EEE		2
#define PARTNER_EEE	2
#define DEBUG		0

struct hiphy_info {
	char *name;
	unsigned long  phy_id;
	unsigned int  with_eee; /*1: eee supported by this phy */
	struct hiphy_driver *driver;
};

struct hiphy_driver {
	int (*eee_enable)(void);
};

struct hieth_netdev_local {
#ifdef HIETH_TSO_SUPPORTED
	struct dma_tx_desc *dma_tx ____cacheline_aligned;
	dma_addr_t dma_tx_phy;
	unsigned int sg_head;
	unsigned int sg_tail;
#endif
	void __iomem *iobase;	       /* virtual io addr */
	unsigned long iobase_phys;	/* physical io addr */
	int port;			/* 0 => up port, 1 => down port */

	struct device *dev;
	struct net_device_stats stats;

	struct sk_buff_head rx_head;	/*received pkgs*/
	struct sk_buff_head rx_hw;	/*rx pkgs in hw*/
#ifdef HIETH_TSO_SUPPORTED
	struct tx_pkt_info *txq;
	unsigned int txq_head;
	unsigned int txq_tail;
	int q_size;
#else
	struct sk_buff_head tx_hw;	/*tx pkgs in hw*/
#endif
	int tx_hw_cnt;

	struct timer_list monitor;

	struct {
		int hw_xmitq;
	} depth;

	struct {
		unsigned long rx_pool_dry_times;
	} stat;

#define SKB_SIZE		(HIETH_MAX_FRAME_SIZE)
	struct rx_skb_pool {
	/*skb pool*/
	struct sk_buff *sk_pool[CONFIG_HIETH_MAX_RX_POOLS];
		int next_free_skb;	/*next free skb*/
	} rx_pool;

	struct tasklet_struct bf_recv;

	char phy_name[MII_BUS_ID_SIZE];
	struct phy_device *phy;
	int link_stat;

	spinlock_t lock;
	unsigned long lockflags;
	struct hiphy_driver *driver;
};

#ifdef HIETH_TSO_DEBUG
#define MAX_RECORD      (100)
struct send_pkt_info {
	u32 reg_addr;
	u32 reg_pkt_info;
	u32 status;
};
#endif

/* ***********************************************************
 *
 * Only for internal used!
 *
 * ***********************************************************
 */

/* read/write IO */
#define hieth_readl(ld, ofs) \
		({ unsigned long reg = readl((ld)->iobase + (ofs)); \
		hieth_trace(2, "readl(0x%04X) = 0x%08lX", (ofs), reg); \
		reg; })
#define hieth_writel(ld, v, ofs) do { writel(v, (ld)->iobase + (ofs)); \
	hieth_trace(2, "writel(0x%04X) = 0x%08lX", (ofs), (unsigned long)(v)); \
} while (0)

#define MK_BITS(shift, nbits)	((((shift)&0x1F)<<16) | ((nbits)&0x3F))

#define hieth_writel_bits(ld, v, ofs, bits_desc) do { \
	unsigned long _bits_desc = bits_desc; \
	unsigned long _shift = (_bits_desc)>>16; \
	unsigned long _reg = hieth_readl(ld, ofs); \
	unsigned long _mask = ((_bits_desc & 0x3F) < 32) ? \
				(((1<<(_bits_desc & 0x3F)) - 1)<<(_shift)) :\
				 0xffffffff; \
	hieth_writel(ld, (_reg & (~_mask)) | (((v)<<(_shift)) & _mask), ofs); \
} while (0)
#define hieth_readl_bits(ld, ofs, bits_desc) ({ \
		unsigned long _bits_desc = bits_desc; \
		unsigned long _shift = (_bits_desc)>>16; \
		unsigned long _mask = ((_bits_desc & 0x3F) < 32) ?\
				 (((1 << (_bits_desc & 0x3F)) - 1)<<(_shift)) :\
				 0xffffffff; \
		(hieth_readl(ld, ofs)&_mask)>>(_shift); })

#define hieth_trace_level 8
#define hireg_trace(level, msg...) do { \
	if ((level) >= hieth_trace_level) { \
		pr_info("hireg_trace:%s:%d: ", __func__, __LINE__); \
		printk(msg); \
		printk("\n"); \
	} \
} while (0)

#define hireg_readl(base, ofs) ({ unsigned long reg = readl((base) + (ofs)); \
		hireg_trace(2, "_readl(0x%04X) = 0x%08lX", (ofs), reg); \
		reg; })

#define hireg_writel(base, v, ofs) do { writel((v), (base) + (ofs)); \
	hireg_trace(2, "_writel(0x%04X) = 0x%08lX",\
		 (ofs), (unsigned long)(v)); \
} while (0)


#define hireg_writel_bits(base, v, ofs, bits_desc) do { \
	unsigned long _bits_desc = bits_desc; \
	unsigned long _shift = (_bits_desc)>>16; \
	unsigned long _reg = hireg_readl(base, ofs); \
	unsigned long _mask = ((_bits_desc & 0x3F) < 32) ?\
		(((1<<(_bits_desc & 0x3F)) - 1)<<(_shift)) :\
		0xffffffff; \
	hireg_writel(base, (_reg & (~_mask)) |\
		 (((v)<<(_shift)) & _mask), ofs); \
} while (0)

#define hireg_readl_bits(base, ofs, bits_desc) ({ \
		unsigned long _bits_desc = bits_desc; \
		unsigned long _shift = (_bits_desc)>>16; \
		unsigned long _mask = ((_bits_desc & 0x3F) < 32) ? \
		(((1<<(_bits_desc & 0x3F)) - 1)<<(_shift)) :\
		0xffffffff; \
		(hireg_readl(base, ofs)&_mask)>>(_shift); })

#define local_lock_init(ld)	spin_lock_init(&(ld)->lock)
#define local_lock_exit(ld)
#define local_lock(ld)		spin_lock_irqsave(&(ld)->lock, (ld)->lockflags)
#define local_unlock(ld)	spin_unlock_irqrestore(&(ld)->lock, \
					(ld)->lockflags)


#define UD_REG_NAME(name)       ((ld->port == UP_PORT) ? U_##name : D_##name)
#define UD_BIT_NAME(name)       ((ld->port == UP_PORT) ? name##_U : name##_D)
#define UD_PHY_NAME(name)       ((ld->port == UP_PORT) ? name##_U : name##_D)

#define UD_BIT(port, name)	(((port) == UP_PORT) ? name##_U : name##_D)

#define GLB_MAC_H16(port, reg)	((((port) == UP_PORT) ? GLB_MAC_H16_BASE \
					: GLB_MAC_H16_BASE_D) + (reg * 0x8))
#define GLB_MAC_L32(port, reg)	((((port) == UP_PORT) ? GLB_MAC_L32_BASE \
					: GLB_MAC_L32_BASE_D) + (reg * 0x8))

extern struct sk_buff *hieth_platdev_alloc_skb(struct hieth_netdev_local *ld);
struct hiphy_info *phy_search_ids(unsigned long phy_id);
extern struct hiphy_info phy_info_table[];

#endif

/* vim: set ts=8 sw=8 tw=78: */
