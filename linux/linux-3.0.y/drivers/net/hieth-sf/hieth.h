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

#define HIETH_MAX_QUEUE_DEPTH	64

#define HIETH_MIIBUS_NAME	"himii"
#define HIETH_DRIVER_NAME	"hieth"

#define HIETH_MAX_FRAME_SIZE	(SZ_1K*2)

#define hieth_trace(level, msg...) do { \
	if ((level) >= CONFIG_HIETH_TRACE_LEVEL) { \
		printk(KERN_INFO "hieth_trace:%s:%d: ", __func__, __LINE__); \
		printk(msg); \
		printk("\n"); \
	} \
} while (0)

#define hieth_error(s...) do { \
	printk(KERN_ERR "hieth:%s:%d: ", __func__, __LINE__); \
	printk(KERN_ERR s); \
	printk("\n"); \
} while (0)

#define hieth_assert(cond) do { \
	if (!(cond)) {\
		printk(KERN_ERR "Assert:hieth:%s:%d\n", \
			__func__, \
			__LINE__); \
		BUG(); \
	} \
} while (0)

#define hieth_dump_buf(level, buf, len) do {\
	int i;\
	char *p = (void *)(buf);\
	if ((level) >= CONFIG_HIETH_TRACE_LEVEL) { \
		printk(KERN_INFO "%s->%d, buf=0x%.8x, len=%d\n", \
				__func__, __LINE__, \
				(int)(buf), (int)(len)); \
		for (i = 0; i < (len); i++) {\
			printk(KERN_INFO "0x%.2x ", *(p+i));\
				if (!((i+1) & 0x07))\
						printk(KERN_INFO "\n");\
		} \
		printk(KERN_INFO "\n");\
	} \
} while (0)

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
	unsigned long iobase;		/* virtual io addr */
	unsigned long iobase_phys;	/* physical io addr */
	int port;			/* 0 => up port, 1 => down port */

	struct device *dev;
	struct net_device_stats stats;

	struct sk_buff_head rx_head;	/*received pkgs*/
	struct sk_buff_head rx_hw;	/*rx pkgs in hw*/
	struct sk_buff_head tx_hw;	/*tx pkgs in hw*/
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
		printk(KERN_INFO "hireg_trace:%s:%d: ", __func__, __LINE__); \
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

#if 0
#define local_lock_init(ld)	spin_lock_init(&(ld)->lock)
#define local_lock_exit(ld)
#define local_lock(ld)		spin_lock_irqsave(&(ld)->lock, (ld)->lockflags)
#define local_unlock(ld) \
				spin_unlock_irqrestore(\
								&(ld)->lock,\
								(ld)->lockflags)
#else
#define local_lock_init(ld)	spin_lock_init(&(ld)->lock)
#define local_lock_exit(ld)
#define local_lock(ld)
#define local_unlock(ld)
#define local_lock_need(ld)     spin_lock_irqsave(&(ld)->lock, (ld)->lockflags)
#define local_unlock_need(ld) \
	spin_unlock_irqrestore(\
			&(ld)->lock,\
			(ld)->lockflags)
#endif


#define UD_REG_NAME(name)       ((ld->port == UP_PORT) ? U_##name : D_##name)
#define UD_BIT_NAME(name)       ((ld->port == UP_PORT) ? name##_U : name##_D)
#define UD_PHY_NAME(name)       ((ld->port == UP_PORT) ? name##_U : name##_D)

extern struct sk_buff *hieth_platdev_alloc_skb(struct hieth_netdev_local *ld);
struct hiphy_info *phy_search_ids(unsigned long phy_id);
extern struct hiphy_info phy_info_table[];

#endif

/* vim: set ts=8 sw=8 tw=78: */
