#ifndef __HIGMAC_H__
#define __HIGMAC_H__

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/list.h>
#include <linux/phy.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <mach/platform.h>
#include <mach/irqs.h>
#include "phy_fix.h"

#define HIGMAC_DRIVER_NAME	"hi_gmac_v200"

#define HIGMAC_TSO_DEBUG
/* #undef HIGMAC_TSO_DEBUG */
#define HIGMAC_TSO_SUPPORTED
/* #undef HIGMAC_TSO_SUPPORTED */
#ifdef HIGMAC_TSO_SUPPORTED
#include "tso.h"
#endif

#define HIETH_INVALID_RXPKG_LEN(len)	\
		(!((len) >= 42 && (len) <= HIETH_MAX_FRAME_SIZE))

#define HIGMAC_IOSIZE			(0x1000)
#define HIGMAC_OFFSET			(HIGMAC_IOSIZE)

#define RX_BQ_IN_INT			(1<<17)
#define TX_RQ_IN_INT			(1<<19)
#define RX_BQ_IN_TIMEOUT_INT		(1<<28)
#define TX_RQ_IN_TIMEOUT_INT		(1<<29)

#define RX_OUTCFF_WR_DESC_ENA		(1<<3)
#define RX_CFF_RD_DESC_ENA		(1<<2)
#define TX_OUTCFF_WR_DESC_ENA		(1<<1)
#define TX_CFF_RD_DESC_ENA		(1<<0)

#define HIGMAC_SYNC_BARRIER() do { isb(); dsb(); dmb(); } while (0)

enum if_mode {/* interface mode */
	interface_mode_mii,
	interface_mode_rmii,
	interface_mode_rgmii,
	interface_mode_butt
};

enum speed_mode {
	speed_mode_10M,
	speed_mode_100M,
	speed_mode_1000M,
	speed_mode_butt
};

enum {/* DEFAULT: duplex_full */
	port_mode_10_mii = 0x1D,
	port_mode_100_mii = 0x1F,
	port_mode_10_rgmii = 0x3D,
	port_mode_100_rgmii = 0x3F,
	port_mode_1000_rgmii = 0x3C,
	port_mode_10_rmii = 0x9D,
	port_mode_100_rmii = 0x9F,
	port_mode_butt = 0xFF
};

#define HIGMAC_LINKED		(1 << 0)
#define HIGMAC_DUP_FULL		(1 << 1)
#define HIGMAC_SPD_10M		(1 << 2)
#define HIGMAC_SPD_100M		(1 << 3)
#define HIGMAC_SPD_1000M	(1 << 4)

#define DEFAULT_INTEPHY_LINK_STAT	(HIGMAC_DUP_FULL)

#define DEFAULT_LINK_STAT    ((ld->phy->phy_id == HISILICON_PHY_ID_FESTAV200)\
		? DEFAULT_INTEPHY_LINK_STAT : 0)

/* Flow Control defines */
#define FLOW_OFF        0
#define FLOW_RX         1
#define FLOW_TX         2
#define FLOW_AUTO       (FLOW_TX | FLOW_RX)

#define FC_ACTIVE_MIN		1
#define FC_ACTIVE_DEFAULT	16
#define FC_ACTIVE_MAX		127
#define FC_DEACTIVE_MIN		1
#define FC_DEACTIVE_DEFAULT	32
#define FC_DEACTIVE_MAX		127

#define FC_PAUSE_TIME_DEFAULT		0xFFFF
#define FC_PAUSE_INTERVAL_DEFAULT	0xFFFF
#define FC_PAUSE_TIME_MAX		0xFFFF

#define RX_BQ_INT_THRESHOLD	0x40 /* TODO: */
#define TX_RQ_INT_THRESHOLD	0x20 /* TODO: */

#define HIGMAC_MAX_QUEUE_DEPTH	(2048)

#define HIGMAC_HWQ_RX_FQ_DEPTH	(1024)
#define HIGMAC_HWQ_RX_BQ_DEPTH	(HIGMAC_HWQ_RX_FQ_DEPTH)
#define HIGMAC_HWQ_TX_BQ_DEPTH	(1024)
#define HIGMAC_HWQ_TX_RQ_DEPTH	(HIGMAC_HWQ_TX_BQ_DEPTH)

#define HIGMAC_MONITOR_TIMER	(msecs_to_jiffies(200))

#define MAX_RX_POOLS		SZ_1K
#define HIETH_MAX_FRAME_SIZE	(1600+128)
#define SKB_SIZE		(HIETH_MAX_FRAME_SIZE)

#define DESC_VLD_FREE		0
#define DESC_VLD_BUSY		1

#define DESC_FL_FIRST		2
#define DESC_FL_MID		0
#define DESC_FL_LAST		1
#define DESC_FL_FULL		3

#define DESC_SIZE		32

struct higmac_desc {
	unsigned int data_buff_addr;

#ifdef HIGMAC_TSO_SUPPORTED
	union {
		struct {
			unsigned int prot_hdr_len:4;
			unsigned int ip_hdr_len:4;
			unsigned int prot_type:1;
			unsigned int ip_ver:1;
			unsigned int vlan_flag:1;
			unsigned int nfrags_num:5;
			unsigned int data_len:11;
			unsigned int reservel:1;
			unsigned int tso_flag:1;
			unsigned int coe_flag:1;
			unsigned int sg_flag:1;
			unsigned int hw_own:1;
		} tx;
		struct {
			unsigned int buffer_len:11;
			unsigned int reserve2:5;
			unsigned int data_len:11;
			unsigned int reserve1:2;
			unsigned int fl:2;
			unsigned int descvid:1;
		} rx;
		unsigned int val;
	} desc1;
#else
	unsigned int buffer_len:11;
	unsigned int reserve2:5;
	unsigned int data_len:11;
	unsigned int reserve1:2;
	unsigned int fl:2;
	unsigned int descvid:1;
#endif

	struct sk_buff *skb_buff_addr;

#ifdef HIGMAC_TSO_SUPPORTED
	unsigned int sg_desc_offset;
#else
	unsigned int reserve3;
#endif

#ifdef HIGMAC_TSO_SUPPORTED
	unsigned int tx_err;
#else
	unsigned int reserve4;
#endif
	unsigned int reserve5;
	unsigned int reserve6;
	unsigned int reserve7;

};

struct higmac_adapter {
	void	__iomem *fwdctl_iobase;/* forwarding iobase */

	spinlock_t	lock;
	/* temp value for spin_lock_irqsave(lock, lock_flags) */
	unsigned long	lock_flags;

	int		work_mode;
	/* in MODE1 and MODE2, which port is the master? 0--eth0, 1--eth1 */
	int		master;
	int		forcing_fwd;

	unsigned int	debug_level;
};

#define SKB_MAGIC	((struct sk_buff *)0x5a)

#define QUEUE_NUMS	(4)
struct higmac_netdev_local {
#ifdef HIGMAC_TSO_SUPPORTED
#define HIGMAC_SG_DESC_ADD	(64U)
	struct sg_desc *dma_sg_desc ____cacheline_aligned;
	dma_addr_t dma_sg_phy;
	unsigned int sg_head;
	unsigned int sg_tail;
	unsigned int sg_count;
#endif
	void		__iomem	*gmac_iobase;
	int			index; /* 0 -- mac0, 1 -- mac1 */

	struct {
		struct higmac_desc *desc;
		dma_addr_t	phys_addr;

		/* how many desc in the desc pool */
		unsigned int	count;
		struct sk_buff **skb;

		/* sizeof(desc) * count */
		unsigned int	size;

		/* debug info */
		unsigned int	use_cnt;
	} pool[QUEUE_NUMS];
#define rx_fq		pool[0]
#define rx_bq		pool[1]
#define tx_bq		pool[2]
#define tx_rq		pool[3]

	struct device		*dev;
	struct net_device	*netdev;

	struct higmac_adapter	*adapter;

	struct timer_list	monitor;

	char			phy_name[MII_BUS_ID_SIZE];
	struct phy_device	*phy;
	int (*eee_init)(struct phy_device *phy_dev);

	unsigned int flow_ctrl;
	unsigned int pause;
	unsigned int pause_interval;
	unsigned int flow_ctrl_active_threshold;
	unsigned int flow_ctrl_deactive_threshold;

	int			link_stat;
	struct mii_bus		*mii_bus_registered;
	struct mii_bus		*mii_bus_using;

	spinlock_t		rxlock;
	spinlock_t		txlock;
	spinlock_t		pmtlock;

	struct tasklet_struct	bf_recv;
	int			dev_state;	/* INIT/OPEN/CLOSE */
#define INIT			(0) /* power off gmac */
#define OPEN			(1) /* power on gmac */
#define CLOSE			(2) /* power off gmac */
};

#ifdef HIGMAC_TSO_SUPPORTED
enum tso_version {
	VER_NO_TSO = 0x0,
	VER_BYTE_SPLICE = 0x100,
	VER_SG_COE = 0x200,
	VER_TSO = 0x300,
};
#endif

struct higmac_board_info {
	struct net_device *netdev;
	char *gmac_name;
	char *mii_name;
	int phy_addr;
	phy_interface_t phy_intf;
	struct sockaddr macaddr;
#ifdef HIGMAC_TSO_SUPPORTED
	unsigned int	tso_ver;
#endif
};

#ifdef HIGMAC_TSO_DEBUG
#define MAX_RECORD	(100)
struct send_pkt_info {
	struct higmac_desc	desc;
	int			status;
};
#endif

#define MAX_GMAC_NUMS		(2)

#ifdef CONFIG_ARCH_S40
#define CONFIG_GMAC_NUMS	(2)
#else
#define CONFIG_GMAC_NUMS	(1)
#endif
/* describe board configuration: phy_addr, phy_intf and mdio usage */
extern struct higmac_board_info higmac_board_info[MAX_GMAC_NUMS];

static inline struct higmac_adapter *get_adapter(void)
{
	struct higmac_netdev_local *ld;

	ld = netdev_priv(higmac_board_info[0].netdev);

	return ld->adapter;
}

static inline struct higmac_netdev_local *get_netdev_local(int index)
{
	struct higmac_netdev_local *ld = NULL;

	if (index < CONFIG_GMAC_NUMS)
		ld = netdev_priv(higmac_board_info[index].netdev);

	return ld;
}

#ifdef HIGMAC_TSO_SUPPORTED
int higmac_tx_avail(struct higmac_netdev_local *ld);
#endif

/* suppose higmac_board_info[i] was initialed! */
#define for_each_gmac_netdev_local_priv(ld, i)	\
	for (i = 0; i < CONFIG_GMAC_NUMS && \
		({ld = netdev_priv(higmac_board_info[i].netdev); 1; }); \
		i++)

#define for_each_gmac_netdev(netdev, i)		\
	for (i = 0; i < CONFIG_GMAC_NUMS &&	\
			({netdev = higmac_board_info[i].netdev; 1; }); \
		i++)

/* trace rx desc's feed and reclaim action */
#define DEBUG_HW_RX_DESC	(1 << 0)
#define DEBUG_HW_TX_DESC	(1 << 1)
#define DEBUG_HW_IRQ		(1 << 2)
#define DEBUG_AUTOEEE		(1 << 3)
#define DEBUG_DEFAULT		(0)/* TODO: change it to 0 */
/* TODO: add more debug option here */
#define debug(name)	(get_adapter()->debug_level & DEBUG_##name)

#endif
