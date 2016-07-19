#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/dma-mapping.h>
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/atomic.h>
#include <linux/platform_device.h>
#include <linux/capability.h>
#include <linux/time.h>
#include <asm/setup.h>
#include <linux/proc_fs.h>
#include <linux/module.h>

#include "util.h"
#include "higmac.h"
#include "ctrl.h"
#include "mdio.h"
#include "forward.h"
#include "autoeee/autoeee.h"
#include "sockioctl.h"

#define	mdio_bus_0		"mdio0"
#define	mdio_bus_1		"mdio1"

static unsigned int flow_ctrl_en = FLOW_OFF;
static int tx_flow_ctrl_pause_time = CONFIG_TX_FLOW_CTRL_PAUSE_TIME;
static int tx_flow_ctrl_pause_interval = CONFIG_TX_FLOW_CTRL_PAUSE_INTERVAL;
static int tx_flow_ctrl_active_threshold = CONFIG_TX_FLOW_CTRL_ACTIVE_THRESHOLD;
static int tx_flow_ctrl_deactive_threshold =
				CONFIG_TX_FLOW_CTRL_DEACTIVE_THRESHOLD;

struct higmac_board_info higmac_board_info[MAX_GMAC_NUMS] = {
	{
		.gmac_name	= "gmac0",
		.mii_name	= mdio_bus_0,
		.phy_addr	= CONFIG_HIGMAC_PHY0_ADDR,
		.phy_intf	= CONFIG_HIGMAC_PHY0_INTERFACE_MODE,
	},
#if (CONFIG_GMAC_NUMS > 1)
	{
		.gmac_name	= "gmac1",
		.mii_name	= mdio_bus_0,
		.phy_addr	= CONFIG_HIGMAC_PHY1_ADDR,
		.phy_intf	= CONFIG_HIGMAC_PHY1_INTERFACE_MODE,
	}
#endif
};

const char *mii_to_str[] = {
	[PHY_INTERFACE_MODE_MII] = "mii",
	[PHY_INTERFACE_MODE_RMII] = "rmii",
	[PHY_INTERFACE_MODE_RGMII] = "rgmii"
};
static char *mdio_bus[MAX_GMAC_NUMS] = {
	mdio_bus_0,
	mdio_bus_1
};

/* default, disable autoeee func */
static bool enable_autoeee;
module_param(enable_autoeee, bool, S_IRUGO | S_IWUSR);

/* module parameters
 * phy_intf=rgmii,mii	==> eth0 use rgmii interface, eth1 use mii interface;
 * phy_intf=rmii	==> eth0, eth1 both use rmii interface;
 * phy_addr=1,2		==> eth0's phy addr is 1, eth1's phy addr is 2;
 * use_mdio=0,1		==> eth0 use mdio0, eth1 use mdio1;
 * use_mdio=1		==> eth0 and eth1 both use mdio1;
 */
static int __init hieth_parse_tag(const struct tag *tag)
{
	const char *tags = (const char *)&tag->u.cmdline.cmdline, *name;
	phy_interface_t phy_intf;
	char str[15];
	char *s, *e, *buf;
	int gmac = 0;
	unsigned long tmp;

	printk(KERN_DEBUG "MISC TAGS: %s\n", tags);

	name = "phy_intf=";
	s = strstr(tags, name);
	if (s) {/* s = "phy_intf=mii,rgmii;..." */
		s += strlen(name);/* s = "mii,rgmii;..." */
		memcpy(str, s, sizeof(str));/* copy to a safe place */
		buf = &str[0];	/* buf point to the head of the string */
		s = strsep(&buf, ";");/* s = "mii,rgmii" */
next_mii:
		while (*s == ' ' || *s == ',')
			s++;

		if (!strncmp(s, "mii", 3))
			phy_intf = PHY_INTERFACE_MODE_MII;
		else if (!strncmp(s, "rmii", 4))
			phy_intf = PHY_INTERFACE_MODE_RMII;
		else if (!strncmp(s, "rgmii", 5))
			phy_intf = PHY_INTERFACE_MODE_RGMII;
		else {
			pr_err("ETH%d phy_intf=%s invalid! ", gmac, s);
			pr_err("Set default to mii.\n");
			phy_intf = PHY_INTERFACE_MODE_MII;
		}

		if (!gmac) {/* first time */
			higmac_board_info[0].phy_intf = phy_intf;
			/* in case phy_intf=mii */
			higmac_board_info[1].phy_intf = phy_intf;
			gmac++;
			s = strchr(s, ',');
			if (s)
				goto next_mii;
		} else
			higmac_board_info[1].phy_intf = phy_intf;
	}

	/* use_mdio=0 or use_mdio=1 or use_mdio=0,1 or ... */
	gmac = 0;
	name = "use_mdio=";
	s = strstr(tags, name);
	if (s) {
		s += strlen(name);
		memcpy(str, s, sizeof(str));/* copy to a safe place */
		buf = &str[0];
		s = strsep(&buf, ";");
next_mdio:
		while (*s == ' ' || *s == ',')
			s++;

		tmp = simple_strtoul(s, &e, 10);
		if (tmp >= CONFIG_GMAC_NUMS) {
			pr_info("Invalid use_mdio, Set GMAC%d use mdio%d.\n",
					gmac, gmac);
			tmp = gmac;
		}

		if (!gmac) {/* first time */
			higmac_board_info[0].mii_name = mdio_bus[tmp];
			higmac_board_info[1].mii_name = mdio_bus[tmp];
			gmac++;
			s = e;
			s = strchr(s, ',');
			if (s)
				goto next_mdio;
		} else
			higmac_board_info[1].mii_name = mdio_bus[tmp];
	}

	/* get phy addr */
	gmac = 0;
	name = "phy_addr=";
	s = strstr(tags, name);
	if (s) {
		s += strlen(name);
		memcpy(str, s, sizeof(str));/* copy to a safe place */
		buf = &str[0];
		s = strsep(&buf, ";");
next_phyaddr:
		while (*s == ' ' || *s == ',')
			s++;

		tmp = simple_strtoul(s, &e, 10);

		if (tmp == 0)
			pr_info("Dectected gmac%d phyaddr set to 0, " \
					"is it right?\n", gmac);
		else if (tmp >= 32) {
			pr_info("Dectected gmac%d ", gmac);
			pr_info("phyaddr set to val(%lu) >= 32.", tmp);
			pr_info("This may not correct, ");
			pr_info("use default phyaddr=%u\n", \
					higmac_board_info[gmac].phy_addr);

			tmp = higmac_board_info[gmac].phy_addr;
		}

		if (!gmac) {/* gmac0 */
			higmac_board_info[gmac].phy_addr = tmp;
			gmac++;
			s = e;
			s = strchr(s, ',');
			if (s)
				goto next_phyaddr;
		} else /* gmac1 */
			higmac_board_info[gmac].phy_addr = tmp;
	}

	return 0;
}

__tagtable(CONFIG_HIETH_TAG, hieth_parse_tag);

static int __init hieth_macaddr_parse_tag(const struct tag *tag)
{
	int i;

	if (tag->hdr.size != 4) {
		pr_err("ethaddr tag size error!\n");
		return 0;
	}

	for (i = 0; i < CONFIG_GMAC_NUMS; i++)
		memcpy(higmac_board_info[i].macaddr.sa_data, &tag->u, 6);

	return 0;
}
__tagtable(CONFIG_ETHADDR_TAG, hieth_macaddr_parse_tag);

#if defined(CONFIG_MACH_GODEYES)
/*
 * 3516(godeyes) port mode def.
 * port_mode_butt means not support this mode.
 */
static int g_speed_portmode_table[speed_mode_butt][interface_mode_butt] = {
	{port_mode_10_mii,	port_mode_butt,		port_mode_10_rgmii},
	{port_mode_100_mii,	port_mode_butt,		port_mode_100_rgmii},
	{port_mode_butt,	port_mode_butt,		port_mode_1000_rgmii}
};
#else
static int g_speed_portmode_table[speed_mode_butt][interface_mode_butt] = {
	{port_mode_10_mii,	port_mode_10_rmii,	port_mode_10_rgmii},
	{port_mode_100_mii,	port_mode_100_rmii,	port_mode_100_rgmii},
	{port_mode_butt,	port_mode_butt,		port_mode_1000_rgmii}
};
#endif

int calculate_port_mode(enum speed_mode speed, phy_interface_t phy_intf,
		int is_duplex_half)
{
	enum if_mode if_mode;
	int ret = port_mode_butt;

	switch (phy_intf) {
	case PHY_INTERFACE_MODE_MII:
		if_mode = interface_mode_mii;
		break;
	case PHY_INTERFACE_MODE_RMII:
		if_mode = interface_mode_rmii;
		break;
	case PHY_INTERFACE_MODE_RGMII:
		if_mode = interface_mode_rgmii;
		break;
	default:
		if_mode = interface_mode_butt;
		break;
	}

	if (speed < speed_mode_butt && if_mode < interface_mode_butt) {
		ret = g_speed_portmode_table[speed][if_mode];
		if (is_duplex_half)
			ret &= ~(1 << 4);/* see mac_if ctrl reg */

		return ret;
	}

	pr_err("Invalid speed(%d) & interface(%d) mode.\n", speed, if_mode);
	pr_err("Please assign which mode our eth will correctly work at.\n"
		"It may support 10M/100M MII, 10M/100M RMII, "
		"10M/100M/1000M RGMII interface.\n"
		"eg. if your board have two NICs, connecting each phy using "
		"mii and rgmii interface, you can use the module param "
		"'port_mode=mii,rgmii' to tell the driver.\n");
	BUG();

	/* can't reach here */
	return -1;
}

void higmac_set_flow_ctrl(struct higmac_netdev_local *ld)
{
	unsigned int flow;
	unsigned int flow_rx_q_en;
	unsigned int rx_fq_empty_th;
	unsigned int rx_fq_full_th;
	unsigned int rx_bq_empty_th;
	unsigned int rx_bq_full_th;

	higmac_writel(ld, ld->pause, FC_TX_TIMER);
	higmac_writel(ld, ld->pause_interval, PAUSE_THR);

	rx_fq_empty_th = higmac_readl(ld, RX_FQ_ALEMPTY_TH);
	rx_fq_empty_th &= ~(BITS_Q_PAUSE_TH_MASK << BITS_Q_PAUSE_TH_OFFSET);
	rx_fq_empty_th |= (ld->flow_ctrl_active_threshold <<
			BITS_Q_PAUSE_TH_OFFSET);
	higmac_writel(ld, rx_fq_empty_th, RX_FQ_ALEMPTY_TH);

	rx_fq_full_th = higmac_readl(ld, RX_FQ_ALFULL_TH);
	rx_fq_full_th &= ~(BITS_Q_PAUSE_TH_MASK << BITS_Q_PAUSE_TH_OFFSET);
	rx_fq_full_th |= (ld->flow_ctrl_deactive_threshold <<
			BITS_Q_PAUSE_TH_OFFSET);
	higmac_writel(ld, rx_fq_full_th, RX_FQ_ALFULL_TH);

	rx_bq_empty_th = higmac_readl(ld, RX_BQ_ALEMPTY_TH);
	rx_bq_empty_th &= ~(BITS_Q_PAUSE_TH_MASK << BITS_Q_PAUSE_TH_OFFSET);
	rx_bq_empty_th |= (ld->flow_ctrl_active_threshold <<
			BITS_Q_PAUSE_TH_OFFSET);
	higmac_writel(ld, rx_bq_empty_th, RX_BQ_ALEMPTY_TH);

	rx_bq_full_th = higmac_readl(ld, RX_BQ_ALFULL_TH);
	rx_bq_full_th &= ~(BITS_Q_PAUSE_TH_MASK << BITS_Q_PAUSE_TH_OFFSET);
	rx_bq_full_th |= (ld->flow_ctrl_deactive_threshold <<
			BITS_Q_PAUSE_TH_OFFSET);
	higmac_writel(ld, rx_bq_full_th, RX_BQ_ALFULL_TH);

	higmac_writel(ld, 0, CRF_TX_PAUSE);

	flow_rx_q_en = higmac_readl(ld, RX_PAUSE_EN);
	flow_rx_q_en &= ~(BIT_RX_FQ_PAUSE_EN | BIT_RX_BQ_PAUSE_EN);
	if (ld->flow_ctrl & FLOW_TX)
		flow_rx_q_en |= (BIT_RX_FQ_PAUSE_EN | BIT_RX_BQ_PAUSE_EN);
	higmac_writel(ld, flow_rx_q_en, RX_PAUSE_EN);

	flow = higmac_readl(ld, PAUSE_EN);
	flow &= ~(BIT_RX_FDFC | BIT_TX_FDFC);
	if (ld->flow_ctrl & FLOW_RX)
		flow |= BIT_RX_FDFC;
	if (ld->flow_ctrl & FLOW_TX)
		flow |= BIT_TX_FDFC;
	higmac_writel(ld, flow, PAUSE_EN);

	higmac_writel_bits(ld, 1, REC_FILT_CONTROL, BIT_PAUSE_FRM_PASS);
}

static void higmac_set_flow_ctrl_args(struct higmac_netdev_local *ld)
{
	ld->flow_ctrl = flow_ctrl_en;
	ld->pause = tx_flow_ctrl_pause_time;
	ld->pause_interval = tx_flow_ctrl_pause_interval;
	ld->flow_ctrl_active_threshold = tx_flow_ctrl_active_threshold;
	ld->flow_ctrl_deactive_threshold = tx_flow_ctrl_deactive_threshold;
}

/* set gmac's multicast list, here we setup gmac's mc filter, not fwd tbl's */
static void higmac_gmac_multicast_list(struct net_device *dev)
{
	struct higmac_netdev_local *ld = netdev_priv(dev);
	struct netdev_hw_addr *ha;
	unsigned int d = 0;

	/* when set gmac in promisc mode
	 * a. dev in IFF_PROMISC mode
	 * b. work_mode is not STANDALONE
	 */
	if ((dev->flags & IFF_PROMISC)
			|| (ld->adapter->work_mode != STANDALONE)) {
		/* promisc mode.received all pkgs. */
		higmac_writel_bits(ld, 0, REC_FILT_CONTROL, BIT_BC_DROP_EN);
		higmac_writel_bits(ld, 0, REC_FILT_CONTROL, BIT_MC_MATCH_EN);
		higmac_writel_bits(ld, 0, REC_FILT_CONTROL, BIT_UC_MATCH_EN);
	} else {
		/* drop uc pkgs with field 'DA' not match our's */
		higmac_writel_bits(ld, 1, REC_FILT_CONTROL, BIT_UC_MATCH_EN);

		if (dev->flags & IFF_BROADCAST)	/* no broadcast */
			higmac_writel_bits(ld, 0, REC_FILT_CONTROL,
					BIT_BC_DROP_EN);
		else
			higmac_writel_bits(ld, 1, REC_FILT_CONTROL,
					BIT_BC_DROP_EN);

		if (netdev_mc_empty(dev) || !(dev->flags
					& IFF_MULTICAST)) {
			/* haven't join any mc group */
			higmac_writel(ld, 0, PORT_MC_ADDR_LOW);
			higmac_writel_bits(ld, 0, PORT_MC_ADDR_HIGH,
					BIT_PORT_MC_ADDR_HIGH);
			higmac_writel_bits(ld, 1, REC_FILT_CONTROL,
					BIT_MC_MATCH_EN);
		} else if (1 == netdev_mc_count(dev) && (dev->flags
					& IFF_MULTICAST)) {
			netdev_for_each_mc_addr(ha, dev) {
				d = (ha->addr[0] << 8) | (ha->addr[1]);
				higmac_writel_bits(ld, d, PORT_MC_ADDR_HIGH,
						BIT_PORT_MC_ADDR_HIGH);

				d = (ha->addr[2] << 24) | (ha->addr[3] << 16)
					| (ha->addr[4] << 8) | (ha->addr[5]);
				higmac_writel(ld, d, PORT_MC_ADDR_LOW);
			}
			higmac_writel_bits(ld, 1, REC_FILT_CONTROL,
					BIT_MC_MATCH_EN);
		} else
			higmac_writel_bits(ld, 0, REC_FILT_CONTROL,
					BIT_MC_MATCH_EN);
	}
}

/*
 * the func stop the hw desc and relaim the software skb resource
 * before reusing the gmac, you'd better reset the gmac
 */
void higmac_reclaim_rx_tx_resource(struct higmac_netdev_local *ld)
{
	unsigned long rxflags, txflags;
	int rd_offset, wr_offset;
	int i;

	higmac_irq_disable(ld, RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT
				| TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT);
	higmac_hw_desc_disable(ld);
	higmac_port_disable(ld);
	higmac_writel(ld, STOP_RX_TX, STOP_CMD);

	spin_lock_irqsave(&ld->rxlock, rxflags);
	/* rx_bq: logic write pointer */
	wr_offset = readl(ld->gmac_iobase + RX_BQ_WR_ADDR);
	/* rx_bq: software read pointer */
	rd_offset = readl(ld->gmac_iobase + RX_BQ_RD_ADDR);
	/* FIXME: prevent to reclaim skb in rx bottom half */
	writel(wr_offset, ld->gmac_iobase + RX_BQ_RD_ADDR);

	/* rx_fq: software write pointer */
	wr_offset = readl(ld->gmac_iobase + RX_FQ_WR_ADDR);
	/* rx_fq: logic read pointer */
	rd_offset = readl(ld->gmac_iobase + RX_FQ_RD_ADDR);
	if (!rd_offset)
		rd_offset = (HIGMAC_HWQ_RX_FQ_DEPTH - 1) << 5;
	else
		rd_offset -= DESC_SIZE;
	/* FIXME: stop to feed hw desc */
	writel(rd_offset, ld->gmac_iobase + RX_FQ_WR_ADDR);

	for (i = 0; i < ld->rx_fq.count; i++) {
		if (!ld->rx_fq.skb[i])
			ld->rx_fq.skb[i] = SKB_MAGIC;
	}
	spin_unlock_irqrestore(&ld->rxlock, rxflags);

	/* no need to wait pkts in tx_rq finish to free all skb,
	 * because higmac_xmit_release_skb is in the tx_lock,
	 */
	spin_lock_irqsave(&ld->txlock, txflags);
	/* tx_rq: logic write */
	wr_offset = readl(ld->gmac_iobase + TX_RQ_WR_ADDR);
	/* tx_rq: software read */
	rd_offset = readl(ld->gmac_iobase + TX_RQ_RD_ADDR);
	/* FIXME: stop to reclaim tx skb */
	writel(wr_offset, ld->gmac_iobase + TX_RQ_RD_ADDR);

	/* tx_bq: logic read */
	rd_offset = readl(ld->gmac_iobase + TX_BQ_RD_ADDR);
	if (!rd_offset)
		rd_offset = (HIGMAC_HWQ_TX_BQ_DEPTH - 1) << 5;
	else
		rd_offset -= DESC_SIZE;
	/* FIXME: stop software tx skb */
	writel(rd_offset, ld->gmac_iobase + TX_BQ_WR_ADDR);

	for (i = 0; i < ld->tx_bq.count; i++) {
		if (!ld->tx_bq.skb[i])
			ld->tx_bq.skb[i] = SKB_MAGIC;
	}
	spin_unlock_irqrestore(&ld->txlock, txflags);
}

static void higmac_monitor_func(unsigned long arg);
void pmt_reg_restore(struct higmac_netdev_local *ld);
static void higmac_set_multicast_list(struct net_device *dev);
/*
 * reset and re-config gmac
 * call fwd_resume() before this fun
 * because some config depend on the work_mode
 */
void higmac_restart(struct higmac_netdev_local *ld)
{
	unsigned long rxflags, txflags;
	struct sk_buff *skb = NULL;
	int i;

	/* restart hw engine now */
	spin_lock_irqsave(&ld->rxlock, rxflags);
	spin_lock_irqsave(&ld->txlock, txflags);
	higmac_hw_mac_core_reset(ld);

	for (i = 0; i < ld->rx_fq.count; i++) {
		skb = ld->rx_fq.skb[i];
		if (skb) {
			ld->rx_fq.skb[i] = NULL;
			if (skb == SKB_MAGIC)
				continue;
			dev_kfree_skb_any(skb);
			ld->rx_bq.use_cnt++;
			/* TODO: need to unmap the skb here
			 * but there is no way to get the dma_addr here,
			 * and unmap(TO_DEVICE) ops do nothing in fact,
			 * so we ignore to call
			 * dma_unmap_single(dev, dma_addr, skb->len,
			 *	DMA_TO_DEVICE)
			 */
		}
	}

	for (i = 0; i < ld->tx_bq.count; i++) {
		skb = ld->tx_bq.skb[i];
		if (skb) {
			ld->tx_bq.skb[i] = NULL;
			if (skb == SKB_MAGIC)
				continue;
			dev_kfree_skb_any(skb);
			ld->tx_rq.use_cnt++;
			/* TODO: unmap the skb */
		}
	}

	pmt_reg_restore(ld);
	/* fwd will not reset with the gmac, so no need to reconfig it */
	higmac_hw_mac_core_init(ld);
	/* TODO: work_mode */
	higmac_hw_set_mac_addr(ld, ld->netdev->dev_addr);
	higmac_hw_set_desc_queue_addr(ld);

	/* we don't set macif here, it will be set in adjust_link */
	higmac_feed_hw(ld);
	higmac_set_multicast_list(ld->netdev);

	higmac_hw_desc_enable(ld);
	higmac_port_enable(ld);
	higmac_irq_enable(ld, RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT
				| TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT);
	spin_unlock_irqrestore(&ld->txlock, txflags);
	spin_unlock_irqrestore(&ld->rxlock, rxflags);
}

static void higmac_adjust_link(struct net_device *dev)
{
	struct higmac_netdev_local *ld = netdev_priv(dev);
	int stat = 0, port_mode;
	enum speed_mode speed_mode = speed_mode_100M;

	if (ld->phy == NULL)
		return;

	stat |= ld->phy->link ? HIGMAC_LINKED : 0;
	stat |= (ld->phy->duplex == DUPLEX_FULL) ? HIGMAC_DUP_FULL : 0;

	if (ld->phy->link) {
		switch (ld->phy->speed) {
		case SPEED_10:
			stat |= HIGMAC_SPD_10M;
			speed_mode = speed_mode_10M;
			break;
		default:
			pr_err("wired, phy link speed!\n");
		case SPEED_100:
			stat |= HIGMAC_SPD_100M;
			speed_mode = speed_mode_100M;
			break;
		case SPEED_1000:
			stat |= HIGMAC_SPD_1000M;
			speed_mode = speed_mode_1000M;
			break;
		}
	}

	if (ld->link_stat != stat) {
		phy_print_status(ld->phy);

		if (stat & HIGMAC_LINKED) {
			port_mode = calculate_port_mode(speed_mode,
					higmac_board_info[ld->index].phy_intf,
					ld->phy->duplex == DUPLEX_HALF);
			higmac_set_macif(ld, port_mode, speed_mode);

			/* phy half duplex: for collision detect
			 * and retransmission */
			if (ld->phy->duplex == DUPLEX_HALF)
				higmac_writel(ld, DUPLEX_HALF, \
						MAC_DUPLEX_HALF_CTRL);
			else if (ld->phy->duplex == DUPLEX_FULL)
				higmac_writel(ld, DUPLEX_FULL, \
						MAC_DUPLEX_HALF_CTRL);
			else
				pr_err("unknown, phy link duplex!\n");
		}


		/* TODO: what'll happened when linkup twice */
		if (enable_autoeee)
			init_autoeee(ld, stat);
		ld->link_stat = stat;
	}
}

static void higmac_destroy_hw_desc_queue(struct higmac_netdev_local *ld)
{
	int i;

	for (i = 0; i < QUEUE_NUMS; i++) {
		if (ld->pool[i].desc) {
			dma_free_coherent(ld->dev, ld->pool[i].size,
					ld->pool[i].desc,
					ld->pool[i].phys_addr);

			ld->pool[i].desc = NULL;
		}
	}

	kfree(ld->rx_fq.skb);
	kfree(ld->tx_bq.skb);
	ld->rx_fq.skb = NULL;
	ld->tx_bq.skb = NULL;
}

static int higmac_init_hw_desc_queue(struct higmac_netdev_local *ld)
{
	struct device *dev = ld->dev;
	struct higmac_desc *virt_addr;
	dma_addr_t phys_addr;
	int size, i;

	/* queue depth */
	ld->rx_fq.count = HIGMAC_HWQ_RX_FQ_DEPTH;
	ld->rx_bq.count = HIGMAC_HWQ_RX_BQ_DEPTH;
	ld->tx_bq.count = HIGMAC_HWQ_TX_BQ_DEPTH;
	ld->tx_rq.count = HIGMAC_HWQ_TX_RQ_DEPTH;

	for (i = 0; i < QUEUE_NUMS; i++) {
		size	   = ld->pool[i].count * sizeof(struct higmac_desc);
		virt_addr  = dma_alloc_coherent(dev, size, &phys_addr,
				GFP_KERNEL);
		if (virt_addr == NULL) {
			pr_err("alloc desc pool[%d] error!\n", i);
			goto error_free_pool;
		}

		memset(virt_addr, 0, size);
		ld->pool[i].size = size;
		ld->pool[i].desc = virt_addr;
		ld->pool[i].phys_addr = phys_addr;
		/* pr_info("pool[i]=%p, phys=0x%x\n", virt_addr, phys_addr); */
	}
	ld->rx_fq.skb = kzalloc(ld->rx_fq.count
			* sizeof(struct sk_buff *), GFP_KERNEL);
	if (ld->rx_fq.skb == NULL) {
		pr_err("alloc gmac%d rx_fq skb array failed!\n", ld->index);
		goto error_free_pool;
	}

	ld->tx_bq.skb = kzalloc(ld->tx_bq.count
			* sizeof(struct sk_buff *), GFP_KERNEL);
	if (ld->tx_bq.skb == NULL) {
		pr_err("alloc gmac%d tx_bq skb array failed!\n", ld->index);
		goto error_free_pool;
	}
	higmac_hw_set_desc_queue_addr(ld);

	return 0;

error_free_pool:
	higmac_destroy_hw_desc_queue(ld);

	return -ENOMEM;
}

#ifdef HIGMAC_TSO_SUPPORTED
int higmac_tx_avail(struct higmac_netdev_local *ld)
{
	int tx_bq_wr_offset, tx_bq_rd_offset;

	tx_bq_wr_offset = higmac_readl_bits(ld, TX_BQ_WR_ADDR,
			BITS_TX_BQ_WR_ADDR);/* software write pointer */
	tx_bq_rd_offset = higmac_readl_bits(ld, TX_BQ_RD_ADDR,
			BITS_TX_BQ_RD_ADDR);/* logic read pointer */

	return (tx_bq_rd_offset >> 5) + HIGMAC_HWQ_TX_BQ_DEPTH
			- (tx_bq_wr_offset >> 5) - 1;
}

static int higmac_init_sg_desc_queue(struct higmac_netdev_local *ld)
{
	ld->sg_count = ld->tx_bq.count + HIGMAC_SG_DESC_ADD;
	ld->dma_sg_desc = (struct sg_desc *)dma_alloc_coherent(ld->dev,
				ld->sg_count * sizeof(struct sg_desc),
				&ld->dma_sg_phy, GFP_KERNEL);

	if (ld->dma_sg_desc == NULL) {
		pr_err("alloc sg desc dma error!\n");
		return -ENOMEM;
	}

#ifdef HIGMAC_TSO_DEBUG
	pr_info("Higmac dma_sg_phy: 0x%x\n", ld->dma_sg_phy);
#endif

	ld->sg_head = ld->sg_tail = 0;

	return 0;
}

static void higmac_destroy_sg_desc_queue(struct higmac_netdev_local *ld)
{
	if (ld->dma_sg_desc) {
		dma_free_coherent(ld->dev,
			ld->sg_count * sizeof(struct sg_desc),
			ld->dma_sg_desc, ld->dma_sg_phy);
		ld->dma_sg_desc = NULL;
	}
}
#endif

static void higmac_bfproc_recv(unsigned long data)
{
	struct net_device *dev = (void *)data;
	struct higmac_netdev_local *ld = netdev_priv(dev);
	struct sk_buff *skb = NULL;
	int rx_bq_wr_offset = 0;
	int rx_bq_rd_offset = 0;
	struct higmac_desc *rx_bq_desc;
	dma_addr_t dma_addr;
	unsigned int rlen;
	unsigned long flags;
	int ret = 0;
	int start, end;

	spin_lock_irqsave(&ld->txlock, flags);
	higmac_xmit_release_skb(ld);
	spin_unlock_irqrestore(&ld->txlock, flags);

	spin_lock_irqsave(&ld->rxlock, flags);

	rx_bq_wr_offset = higmac_readl_bits(ld, RX_BQ_WR_ADDR,
			BITS_RX_BQ_WR_ADDR);/* logic write pointer */
	rx_bq_rd_offset = higmac_readl_bits(ld, RX_BQ_RD_ADDR,
			BITS_RX_BQ_RD_ADDR);/* software read pointer */

	higmac_trace(5, "rx_bq_wr_offset is %x, rx_bq_rd_offset is %x\n",
			rx_bq_wr_offset, rx_bq_rd_offset);

	start = rx_bq_rd_offset >> 5;
	while (rx_bq_wr_offset != rx_bq_rd_offset) {
		int pos = rx_bq_rd_offset >> 5;

		rx_bq_desc = ld->rx_bq.desc + pos;

		skb = rx_bq_desc->skb_buff_addr;
		while (!skb) {
			pr_err("rx_bq: desc consistent warning:");
			pr_err("rx_bq_wr_offset = 0x%x, ", rx_bq_wr_offset);
			pr_err("rx_bq_rd_offset = 0x%x, ", rx_bq_rd_offset);
			pr_err("rx_fq.skb[%d](0x%p)\n", pos, \
					ld->rx_fq.skb[pos]);
			/*
			 * logic bug, cause it update rx_bq_wr pointer first
			 * before loading the desc from fifo into rx_bq.
			 * so try to read it again until desc reached the rx_bq.
			 * FIXME: use volatile or __iomem to avoid compiler
			 * optimize?
			 */
			rx_bq_desc = ld->rx_bq.desc + pos;

			skb = rx_bq_desc->skb_buff_addr;
		}

		/* data consistent check */
		if (unlikely(skb != ld->rx_fq.skb[pos])) {
			pr_err("desc->skb(0x%p) != rx_fq.skb[%d](0x%p)!\n",
					skb, pos, ld->rx_fq.skb[pos]);
			if (ld->rx_fq.skb[pos] == SKB_MAGIC)
				goto next;
			BUG_ON(1);
		} else
			ld->rx_fq.skb[pos] = NULL;

		ld->rx_bq.use_cnt++;
		if (ld->rx_bq.use_cnt != rx_bq_desc->reserve5) {
			pr_err("desc pointer ERROR!!! ld->rx_bq_count is %x, ",
					ld->rx_bq.use_cnt);
			pr_err("rx_bq_desc->reserve5 is %x\n",
					rx_bq_desc->reserve5);
		}
#ifdef HIGMAC_TSO_SUPPORTED
		rlen = rx_bq_desc->desc1.rx.data_len;
#else
		rlen = rx_bq_desc->data_len;
#endif
		dma_addr = rx_bq_desc->data_buff_addr;

		dma_unmap_single(ld->dev, dma_addr, HIETH_MAX_FRAME_SIZE,
				DMA_FROM_DEVICE);

		skb_put(skb, rlen);
		skb->protocol = eth_type_trans(skb, dev);

		if (HIETH_INVALID_RXPKG_LEN(skb->len)) {
			pr_err("rcv pkt len error, len = %d\n", skb->len);
			dev->stats.rx_errors++;
			dev->stats.rx_length_errors++;
			dev_kfree_skb_any(skb);
			goto next;
		}

		dev->stats.rx_packets++;
		dev->stats.rx_bytes += skb->len;
		dev->last_rx = jiffies;

		skb->dev = dev;
		ret = netif_rx(skb);
		if (ret)
			dev->stats.rx_dropped++;
next:
		rx_bq_desc->skb_buff_addr = 0;

		rx_bq_rd_offset += DESC_SIZE;
		if (rx_bq_rd_offset >= (HIGMAC_HWQ_RX_BQ_DEPTH << 5))
			rx_bq_rd_offset = 0;

		higmac_writel_bits(ld, rx_bq_rd_offset, RX_BQ_RD_ADDR,
				BITS_RX_BQ_RD_ADDR);
	}
	end = rx_bq_rd_offset >> 5;
	if (debug(HW_RX_DESC))
		pr_info("gmac%d reclaim skb[%d-%d)\n", ld->index, start, end);
	higmac_feed_hw(ld);
	spin_unlock_irqrestore(&ld->rxlock, flags);
}

static irqreturn_t hieth_net_isr(int irq, void *dev_id)
{
	/* TODO: remove debug code */
	struct higmac_netdev_local *ld = get_netdev_local((int)dev_id);
	int ints;

	higmac_irq_disable(ld, RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT
				| TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT);

	ints = higmac_read_irqstatus(ld);

	if (debug(HW_IRQ))
		pr_info("gmac%d irq status=0x%x\n", ld->index, ints);

	if (likely(ints & (RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT |
			TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT))) {
		tasklet_schedule(&ld->bf_recv);
		higmac_clear_irqstatus(ld, ints);
		ints &= ~(RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT
				| TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT);
	}

	if (unlikely(ints)) {
		higmac_trace(7, "unknown ints=0x%.8x\n", ints);
		higmac_clear_irqstatus(ld, ints);
	}

	higmac_irq_enable(ld, RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT
				| TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT);

#if	0	/* debug_pmt and eee */
	ints = readl(ld->gmac_iobase + 0xa00);
	if (ints & (1 << 3)) {/* interrupt enable bit */
		if (ints & (1 << 5))
			pr_info("got magic packet!\n");
		if (ints & (1 << 6))
			pr_info("got wake-up frame!\n");
		/* clear interrupt status */
		writel(ints, ld->gmac_iobase + 0xa00);
	}

	/* debug eee interrupt */
#endif

	return IRQ_HANDLED;
}

static void higmac_monitor_func(unsigned long arg)
{
	struct net_device *dev = (struct net_device *)arg;
	struct higmac_netdev_local *ld = netdev_priv(dev);
	unsigned long flags;

	if (!ld || !netif_running(dev)) {
		higmac_trace(7, "network driver is stoped.");
		return;
	}

	spin_lock_irqsave(&ld->rxlock, flags);
	higmac_feed_hw(ld);
	spin_unlock_irqrestore(&ld->rxlock, flags);

	ld->monitor.expires = jiffies + HIGMAC_MONITOR_TIMER;
	mod_timer(&ld->monitor, ld->monitor.expires);
}

static int higmac_net_open(struct net_device *dev)
{
	struct higmac_netdev_local *ld = netdev_priv(dev);
	unsigned long flags;

	ld->link_stat = DEFAULT_LINK_STAT;
	phy_start(ld->phy);

	netif_carrier_off(dev);

	higmac_hw_desc_enable(ld);
	higmac_port_enable(ld);
	higmac_irq_enable(ld, RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT
				| TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT);

	spin_lock_irqsave(&ld->rxlock, flags);
	higmac_feed_hw(ld);
	spin_unlock_irqrestore(&ld->rxlock, flags);

	ld->monitor.expires = jiffies + HIGMAC_MONITOR_TIMER;
	mod_timer(&ld->monitor, ld->monitor.expires);

	netif_start_queue(dev);

	return 0;
}

static int higmac_net_close(struct net_device *dev)
{
	struct higmac_netdev_local *ld = netdev_priv(dev);

	higmac_irq_disable(ld, RX_BQ_IN_INT | RX_BQ_IN_TIMEOUT_INT
				| TX_RQ_IN_INT | TX_RQ_IN_TIMEOUT_INT);
	higmac_hw_desc_disable(ld);

	netif_carrier_off(dev);
	netif_stop_queue(dev);

	phy_stop(ld->phy);
	del_timer_sync(&ld->monitor);

	return 0;
}

static void higmac_net_timeout(struct net_device *dev)
{
	dev->stats.tx_errors++;

	printk(KERN_DEBUG "tx timeout!\n");
}

static int higmac_net_hard_start_xmit(struct sk_buff *skb,
		struct net_device *dev)
{
	struct higmac_netdev_local *ld = netdev_priv(dev);
	int ret;

	/*
	 * if adding higmac_xmit_release_skb here, iperf tcp client
	 * performance will be affected, from 550M(avg) to 513M~300M
	 */

	ret = higmac_xmit_real_send(ld, skb);
	if (ret) {
		dev->stats.tx_dropped++;
		dev->stats.tx_fifo_errors++;

		netif_stop_queue(dev);
		ret = NETDEV_TX_BUSY;
		goto out;
	}

	dev->trans_start = jiffies;
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;

	ret = NETDEV_TX_OK;

out:
	return ret;
}

static void higmac_set_multicast_list(struct net_device *dev)
{
	struct higmac_netdev_local *ld = netdev_priv(dev);
	struct netdev_hw_addr *ha;
	int entry, end, i;

	higmac_gmac_multicast_list(dev);

	entry = FWD_TBL_MC_START + FWD_TBL_MC_NUMS_EACH_GMAC * ld->index;
	end = entry + FWD_TBL_MC_NUMS_EACH_GMAC;

	netdev_for_each_mc_addr(ha, dev) {
		if (entry < end) {
			fwd_uc_mc_tbl_add(ld, ha->addr, entry, ADD_MC);
			entry++;
		} else {
			/* shall we switch to promiscous mode? */
			pr_err("Cann't filter so many mc group!\n");
			return;
		}
	}
	/* clear the other mc entry */
	for (i = entry; i < end; i++)
		fwd_mc_tbl_del(ld, entry);
}

static struct net_device_stats *higmac_net_get_stats(struct net_device *dev)
{
	return &dev->stats;
}

static int higmac_net_set_mac_address(struct net_device *dev, void *p)
{
	struct higmac_netdev_local *ld = netdev_priv(dev);
	int ret;

	ret = eth_mac_addr(dev, p);
	if (!ret)
		higmac_hw_set_mac_addr(ld, dev->dev_addr);

	return ret;
}

static void higmac_get_drvinfo(struct net_device *net_dev,
		struct ethtool_drvinfo *info)
{
	strncpy(info->driver, "higmac driver", 15);
	strncpy(info->version, "higmac v200", 15);
	strncpy(info->bus_info, "platform", 15);
}

static unsigned int higmac_get_link(struct net_device *net_dev)
{
	struct higmac_netdev_local *ld = netdev_priv(net_dev);
	return ld->phy->link ? HIGMAC_LINKED : 0;
}

static int higmac_get_settings(struct net_device *net_dev,
		struct ethtool_cmd *cmd)
{
	struct higmac_netdev_local *ld = netdev_priv(net_dev);

	if (ld->phy)
		return phy_ethtool_gset(ld->phy, cmd);

	return -EINVAL;
}

static int higmac_set_settings(struct net_device *net_dev,
		struct ethtool_cmd *cmd)
{
	struct higmac_netdev_local *ld = netdev_priv(net_dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (ld->phy)
		return phy_ethtool_sset(ld->phy, cmd);

	return -EINVAL;
}

static void higmac_get_pauseparam(struct net_device *net_dev,
		struct ethtool_pauseparam *pause)
{
	struct higmac_netdev_local *ld = netdev_priv(net_dev);

	pause->rx_pause = 0;
	pause->tx_pause = 0;
	pause->autoneg = ld->phy->autoneg;

	if (ld->flow_ctrl & FLOW_RX)
		pause->rx_pause = 1;
	if (ld->flow_ctrl & FLOW_TX)
		pause->tx_pause = 1;
}

static int higmac_set_pauseparam(struct net_device *net_dev,
		struct ethtool_pauseparam *pause)
{
	struct higmac_netdev_local *ld = netdev_priv(net_dev);
	struct phy_device *phy = ld->phy;
	int new_pause = FLOW_OFF;
	int ret = 0;

	if (pause->rx_pause)
		new_pause |= FLOW_RX;
	if (pause->tx_pause)
		new_pause |= FLOW_TX;

	if (new_pause != ld->flow_ctrl) {
		ld->flow_ctrl = new_pause;
		higmac_set_flow_ctrl(ld);
	}

	if (phy->autoneg) {
		if (netif_running(net_dev)) {
			struct ethtool_cmd cmd;
			/* auto-negotiation automatically restarted */
			cmd.cmd = ETHTOOL_NWAY_RST;
			cmd.supported = phy->supported;
			cmd.advertising = phy->advertising;
			cmd.autoneg = phy->autoneg;
			cmd.speed = phy->speed;
			cmd.duplex = phy->duplex;
			cmd.phy_address = phy->addr;
			ret = phy_ethtool_sset(phy, &cmd);
		}
	}

	return ret;
}

static struct ethtool_ops hieth_ethtools_ops = {
	.get_drvinfo		= higmac_get_drvinfo,
	.get_link		= higmac_get_link,
	.get_settings		= higmac_get_settings,
	.set_settings		= higmac_set_settings,
	.get_pauseparam		= higmac_get_pauseparam,
	.set_pauseparam		= higmac_set_pauseparam,
};

static const struct net_device_ops hieth_netdev_ops = {
	.ndo_open		= higmac_net_open,
	.ndo_stop		= higmac_net_close,
	.ndo_start_xmit		= higmac_net_hard_start_xmit,
	.ndo_tx_timeout		= higmac_net_timeout,
	.ndo_set_rx_mode	= higmac_set_multicast_list,
	.ndo_do_ioctl		= higmac_ioctl,
	.ndo_set_mac_address	= higmac_net_set_mac_address,
	.ndo_change_mtu         = eth_change_mtu,
	.ndo_get_stats		= higmac_net_get_stats,
};

/*
 * if gmac0 use internal phy
 *	gmac0 -- mdio0 --  internal phy0
 *
 *	gmac1 -- mdio1 --  external phy1
 * else
 *	gmac0		   external phy0, gmac0 use mdio1(gmac1) to ctrl phy0
 *	     \           /
 *	gmac1 -- mdio1 --  external phy1
 *
 *	reseting gmac1--mdio1 may affect gmac0's phy_state_machine!
 */
static int higmac_mdio_register(void)
{
	struct higmac_netdev_local *ld[CONFIG_GMAC_NUMS];
	int ret;

	ld[0] = get_netdev_local(0);/* gmac0 */
#if (CONFIG_GMAC_NUMS > 1)
	ld[1] = get_netdev_local(1);/* gmac1 */
#endif

	/* register mdio0 and mdio1 */
	ret = higmac_mdiobus_driver_init(ld[0], mdio_bus[0]);
	if (ret) {
		pr_err("register mdio bus%d failed!\n", ld[0]->index);
		return ret;
	}

#if (CONFIG_GMAC_NUMS > 1)
	ret = higmac_mdiobus_driver_init(ld[1], mdio_bus[1]);
	if (ret) {
		pr_err("register mdio bus%d failed!\n", ld[1]->index);
		higmac_mdiobus_driver_exit(ld[0]);
		return ret;
	}
#endif

#if (CONFIG_GMAC_NUMS > 1)
	/* gmac0 may use gmac1's mdio and vice versa */
	if (strncmp(higmac_board_info[0].mii_name, mdio_bus[0], 10))
		ld[0]->mii_bus_using = ld[1]->mii_bus_registered;
	if (strncmp(higmac_board_info[1].mii_name, mdio_bus[1], 10))
		ld[1]->mii_bus_using = ld[0]->mii_bus_registered;
#endif

	return 0;
}

static void higmac_mdio_unregister(void)
{
	struct higmac_netdev_local *ld;
	int i;

	for_each_gmac_netdev_local_priv(ld, i)
		higmac_mdiobus_driver_exit(ld);
}

static void higmac_verify_flow_ctrl_args(void)
{
#ifdef CONFIG_TX_FLOW_CTRL_SUPPORT
	flow_ctrl_en |= FLOW_TX;
#endif
#ifdef CONFIG_RX_FLOW_CTRL_SUPPORT
	flow_ctrl_en |= FLOW_RX;
#endif
	if (tx_flow_ctrl_active_threshold < FC_ACTIVE_MIN ||
			tx_flow_ctrl_active_threshold > FC_ACTIVE_MAX)
		tx_flow_ctrl_active_threshold = FC_ACTIVE_DEFAULT;

	if (tx_flow_ctrl_deactive_threshold < FC_DEACTIVE_MIN ||
			tx_flow_ctrl_deactive_threshold > FC_DEACTIVE_MAX)
		tx_flow_ctrl_deactive_threshold = FC_DEACTIVE_DEFAULT;

	if (tx_flow_ctrl_active_threshold >= tx_flow_ctrl_deactive_threshold) {
		tx_flow_ctrl_active_threshold = FC_ACTIVE_DEFAULT;
		tx_flow_ctrl_deactive_threshold = FC_DEACTIVE_DEFAULT;
	}

	if (tx_flow_ctrl_pause_time < 0 ||
			tx_flow_ctrl_pause_time > FC_PAUSE_TIME_MAX)
		tx_flow_ctrl_pause_time = FC_PAUSE_TIME_DEFAULT;

	if (tx_flow_ctrl_pause_interval < 0 ||
			tx_flow_ctrl_pause_interval > FC_PAUSE_TIME_MAX)
		tx_flow_ctrl_pause_interval = FC_PAUSE_INTERVAL_DEFAULT;

	/* pause interval should not bigger than pause time,
	 * but should not too smaller to avoid sending too many pause frame.
	 */
	if ((tx_flow_ctrl_pause_interval > tx_flow_ctrl_pause_time) ||
		(tx_flow_ctrl_pause_interval < (tx_flow_ctrl_pause_time >> 1)))
		tx_flow_ctrl_pause_interval = tx_flow_ctrl_pause_time;
}

static int __devinit higmac_dev_probe(struct platform_device *platdev)
{
	struct device *device = &platdev->dev;
	struct net_device *netdev[CONFIG_GMAC_NUMS] = { 0 };
	struct higmac_netdev_local *ld;
	struct higmac_adapter *adapter;
	struct phy_device *phydev = NULL;
	struct resource *iores;
	int ret = -1, i;
	int mac_index = 0;

	/* 1) init glb adapter */
	adapter = kzalloc(sizeof(struct higmac_adapter), GFP_KERNEL);
	BUG_ON(!adapter);

	/* get these soc specific iobase */
	adapter->fwdctl_iobase = soc_fwdctl_iobase();

	spin_lock_init(&adapter->lock);

	higmac_verify_flow_ctrl_args();

	/* 2) init netdev */
	for (i = 0; i < CONFIG_GMAC_NUMS; i++) {
		netdev[i] = alloc_etherdev(sizeof(struct higmac_netdev_local));
		if (netdev[i] == NULL) {
			pr_err("alloc netdev[%d] failed!\n", i);
			ret = -ENOMEM;
			goto _error_alloc_netdev;
		}

		/* init private ld */
		ld = netdev_priv(netdev[i]);
		iores = platform_get_resource(platdev, IORESOURCE_MEM, i);
		if (iores == NULL) {
			pr_err("get iomem resource failed!");
			goto _error_alloc_netdev;
		}

		ld->gmac_iobase = ioremap(iores->start,
				iores->end - iores->start + 1);
		if (ld->gmac_iobase == NULL) {
			pr_err("ioremap iomem err: base=0x%.8x, size=0x%.8x\n",
					iores->start, HIGMAC_IOSIZE);
			ret = -EFAULT;
			goto _error_alloc_netdev;
		}

		ld->index	= i;	/* mac0, mac1 ... */
		ld->dev		= device;/* struct device */
		ld->netdev	= netdev[i];
		ld->adapter	= adapter;/* glb sys adapter */
		spin_lock_init(&ld->rxlock);
		spin_lock_init(&ld->txlock);
		spin_lock_init(&ld->pmtlock);

		/* init netdevice */
		SET_NETDEV_DEV(netdev[i], device);/* init netdev */
		netdev[i]->irq			= platform_get_irq(platdev, i);
		netdev[i]->watchdog_timeo	= 3 * HZ;
		netdev[i]->netdev_ops		= &hieth_netdev_ops;
		netdev[i]->ethtool_ops		= &hieth_ethtools_ops;

#if 0
#ifdef HIGMAC_TSO_SUPPORTED
		netdev[i]->hw_features |= NETIF_F_SG |
			NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM;

		netdev[i]->features |= netdev[i]->hw_features;
		netdev[i]->features |= NETIF_F_HIGHDMA;

		ndev->vlan_features |= ndev->features;
#endif
#endif

		/* borad info */
		higmac_board_info[i].netdev	= netdev[i];
		/*
		 * bootloadr may set the mac addr already,
		 * grab the MAC from the board somehow
		 */
		memcpy(netdev[i]->dev_addr,
				higmac_board_info[i].macaddr.sa_data,
				netdev[i]->addr_len);

		/* init tasklet */
		ld->bf_recv.next = NULL;
		ld->bf_recv.state = 0;
		ld->bf_recv.func = higmac_bfproc_recv;
		ld->bf_recv.data = (unsigned long)netdev[i];
		atomic_set(&ld->bf_recv.count, 0);

		init_timer(&ld->monitor);
		ld->monitor.function = higmac_monitor_func;
		ld->monitor.data = (unsigned long)netdev[i];
		ld->monitor.expires = jiffies + HIGMAC_MONITOR_TIMER;

		device_set_wakeup_capable(device, 1);
		/* TODO: when we can let phy powerdown?
		 * In forcing fwd mode, we don't want phy powerdown,
		 * so I set wakeup enable all the time */
		device_set_wakeup_enable(ld->dev, 1);
	}

	/* 3) init hw controller */
	for_each_gmac_netdev_local_priv(ld, i) {
		higmac_hw_mac_core_reset(ld);
		higmac_hw_mac_core_init(ld);
	}

	/* phy reset */
	higmac_hw_internal_fephy_reset(adapter);
	higmac_hw_external_phy_reset();

	adapter->debug_level = DEBUG_DEFAULT;
	adapter->work_mode = STANDALONE;
	fwd_setup(adapter);

	/* 6) init hw desc queue */
	for_each_gmac_netdev_local_priv(ld, i) {
		ret = higmac_init_hw_desc_queue(ld);
		if (ret)
			goto _error_hw_desc_queue;

#ifdef HIGMAC_TSO_SUPPORTED
		ret = higmac_init_sg_desc_queue(ld);
		if (ret)
			goto _error_sg_desc_queue;
#endif
	}

	/* 7) register mdio bus */
	ret = higmac_mdio_register();
	if (ret)
		goto _error_mdio_register;

	/* 8) connect phy */
	phy_register_fixups();

	for_each_gmac_netdev_local_priv(ld, i) {
		snprintf(ld->phy_name, MII_BUS_ID_SIZE, PHY_ID_FMT,
				higmac_board_info[i].mii_name,
				higmac_board_info[i].phy_addr);

		phydev = phy_connect(netdev[i], ld->phy_name,
				higmac_adjust_link, 0,
				higmac_board_info[i].phy_intf);
		if (IS_ERR(phydev)) {
			pr_warn("connect to phy_dev %s failed!", ld->phy_name);
			continue;
		}
		/* If the phy_id is mostly Fs, there is no device there */
		if ((phydev->phy_id & 0x1fffffff) == 0x1fffffff
				|| phydev->phy_id == 0) {
			pr_info("PHY %s not found\n", ld->phy_name);
			continue;
		}

		/* Stop Advertising 1000BASE Capability if intf is not RGMII */
		if ((higmac_board_info[i].phy_intf == PHY_INTERFACE_MODE_MII) ||
		  (higmac_board_info[i].phy_intf == PHY_INTERFACE_MODE_RMII)) {
			phydev->advertising &= ~(SUPPORTED_1000baseT_Half |
					SUPPORTED_1000baseT_Full);

			/*
			 * Internal FE phy's reg BMSR bit8 is wrong, make the
			 * kernel believe it has the 1000base Capability, so
			 * fix it here
			 */
			if (phydev->phy_id == HISILICON_PHY_ID_FESTAV200)
				phydev->supported &= ~(ADVERTISED_1000baseT_Full
						| ADVERTISED_1000baseT_Half);
		}

		phydev->advertising |= ADVERTISED_Pause;
		phydev->supported |= ADVERTISED_Pause;

		ld->phy = phydev;

		higmac_set_flow_ctrl_args(ld);
		higmac_set_flow_ctrl(ld);

		/* TODO: code reconstruct */
		if (enable_autoeee)
			init_autoeee(ld, 0);
	}

	/* setup mac_addr after fwd_setup() */
	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;
		if (!is_valid_ether_addr(netdev[i]->dev_addr))
			random_ether_addr(netdev[i]->dev_addr);
		(netdev[i]->dev_addr)[5] += mac_index;
		higmac_hw_set_mac_addr(ld, netdev[i]->dev_addr);
		mac_index++;
	}

	/* 9) request irq */
	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;

		ret = request_irq(netdev[i]->irq, hieth_net_isr, 0,
				higmac_board_info[ld->index].gmac_name,
				(void *)ld->index);
		if (ret) {
			pr_err("request irq for gmac%d failed!\n", ld->index);
			goto _error_request_irq;
		}
	}

#if 1
#ifdef HIGMAC_TSO_SUPPORTED
	for_each_gmac_netdev_local_priv(ld, i) {
		struct net_device *ndev = ld->netdev;
		unsigned int tso_ver;

		tso_ver = higmac_readl_bits(ld, CRF_MIN_PACKET,
				BIT_TSO_VERSION);

		if (tso_ver == VER_SG_COE) {
			ndev->hw_features |= NETIF_F_SG
				| NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM;
		} else if (tso_ver == VER_TSO) {
			ndev->hw_features |= NETIF_F_SG | NETIF_F_TSO
				| NETIF_F_TSO6 | NETIF_F_UFO
				| NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM;
		}

		ndev->features |= ndev->hw_features;
		ndev->features |= NETIF_F_HIGHDMA | NETIF_F_GSO;
		ndev->vlan_features |= ndev->features;

		higmac_board_info[i].tso_ver = tso_ver;
	}
#endif
#endif

	/* 10) register netdevice */
	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;

		ret = register_netdev(ld->netdev);
		if (ret) {
			pr_err("register_netdev[%d] failed!", i);
			goto _error_register_netdev;
		}
	}

	return ret;

_error_register_netdev:
	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;

		unregister_netdev(ld->netdev);
	}

_error_request_irq:
	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;

		free_irq(netdev[i]->irq, (void *)ld->index);
		phy_disconnect(ld->phy);
	}

_error_mdio_register:
	higmac_mdio_unregister();

#ifdef HIGMAC_TSO_SUPPORTED
_error_sg_desc_queue:
	for_each_gmac_netdev_local_priv(ld, i)
		higmac_destroy_sg_desc_queue(ld);
#endif

_error_hw_desc_queue:
	for_each_gmac_netdev_local_priv(ld, i)
		higmac_destroy_hw_desc_queue(ld);

_error_alloc_netdev:
	for (i = 0; i < CONFIG_GMAC_NUMS; i++) {
		if (netdev[i]) {
			ld = netdev_priv(netdev[i]);
			if (ld->gmac_iobase)
				iounmap(ld->gmac_iobase);

			free_netdev(netdev[i]);
			higmac_board_info[i].netdev = NULL;
		}
	}

	kfree(adapter);

	return ret;
}

static int __devexit higmac_dev_remove(struct platform_device *pdev)
{
	struct higmac_adapter *adapter = get_adapter();
	struct higmac_netdev_local *ld;
	struct net_device *netdev;
	int i;

	/* TODO: stop the gmac and free all resource */
	higmac_mdio_unregister();

	for_each_gmac_netdev(netdev, i) {
		ld = netdev_priv(netdev);

		del_timer_sync(&ld->monitor);
		tasklet_disable(&ld->bf_recv);

		if (ld->phy) {
			free_irq(netdev->irq, (void *)ld->index);
			unregister_netdev(netdev);
			phy_disconnect(ld->phy);
		}

		higmac_reclaim_rx_tx_resource(ld);
		higmac_mdiobus_driver_exit(ld);
#ifdef HIGMAC_TSO_SUPPORTED
		higmac_destroy_sg_desc_queue(ld);
#endif
		higmac_destroy_hw_desc_queue(ld);

		if (ld->gmac_iobase)
			iounmap(ld->gmac_iobase);

		free_netdev(netdev);
		higmac_board_info[i].netdev = NULL;
	}

	kfree(adapter);

	return 0;
}

static inline int need_forcing_fwd(void)
{
	struct higmac_adapter *adapter = get_adapter();
	return adapter->forcing_fwd;
}

#include "pm.c"
#ifdef CONFIG_PM

#define RESUME		(0)
#define SUSPEND		(1)
static int suspend_state = RESUME;

static int higmac_dev_suspend(struct platform_device *dev, pm_message_t state)
{
	struct higmac_adapter *adapter = get_adapter();
	struct higmac_netdev_local *ld;
	int mode = adapter->work_mode;
	int i, val, power_off = true;

	if (suspend_state == RESUME)
		suspend_state = SUSPEND;
	else {
		pr_err("%s() is called incorrectly\n", __func__);
		return 0;
	}

	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;
		/* suspend -> linkup
		 * we stop tx_queue in suspend and reclaim resource,
		 * but phy_adjust_link will resume the queue occasionally
		 * it will print tx_queue is full
		 */
		disable_irq(ld->netdev->irq);
		phy_disconnect(ld->phy);

		/* del_timer_sync shoule be excuted before netif_device_detach
		 * because timer monitor may wake up
		 * the stopped queue occasionally */
		del_timer_sync(&ld->monitor);
		tasklet_disable(&ld->bf_recv);
		netif_device_detach(ld->netdev);

		netif_carrier_off(ld->netdev);
		higmac_reclaim_rx_tx_resource(ld);
	}
	if (need_forcing_fwd()) {
		for_each_gmac_netdev_local_priv(ld, i) {
			if (!ld->phy)
				continue;

			/* set gmac in promisc mode.received all pkgs. */
			val = readl(ld->gmac_iobase + REC_FILT_CONTROL);
			val &= ~(BIT_BC_DROP_EN | BIT_MC_MATCH_EN |
					BIT_UC_MATCH_EN);
			higmac_writel(ld, val, REC_FILT_CONTROL);
		}
		mode = MODE3;
		power_off = false;
	}

	fwd_suspend(adapter, mode);

	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;

		if (pmt_enter(ld))
			power_off = false;
	}

	if (power_off) {/* if no forcing_fwd and WOL, then poweroff */
		/* pr_info("power off gmac.\n"); */
		for_each_gmac_netdev_local_priv(ld, i) {
			if (!ld->phy)
				continue;
			/* no need to call genphy_resume() in resume,
			 * because we reset everything */
			genphy_suspend(ld->phy);/* power down phy */
		}
		mdelay(20);
		higmac_hw_all_clk_disable();
	}

	return 0;
}

static int higmac_dev_resume(struct platform_device *dev)
{
	struct higmac_netdev_local *ld;
	struct higmac_adapter *adapter = get_adapter();
	int i;

	if (suspend_state == SUSPEND)
		suspend_state = RESUME;
	else {
		pr_err("%s() is called incorrectly\n", __func__);
		return 0;
	}

	/* internal FE_PHY: enable clk and reset  */
	higmac_hw_internal_fephy_reset(adapter);
	higmac_hw_external_phy_reset();

	fwd_resume(adapter);
	/* power on gmac */
	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;

		/* TODO: why we need this phy code? phy_fix execute here */
		phy_connect_direct(ld->netdev, ld->phy, higmac_adjust_link, 0,
				higmac_board_info[i].phy_intf);
		higmac_restart(ld);
		ld->monitor.expires = jiffies + HIGMAC_MONITOR_TIMER;
		mod_timer(&ld->monitor, ld->monitor.expires);
		ld->link_stat = DEFAULT_LINK_STAT;
		tasklet_enable(&ld->bf_recv);
		netif_device_attach(ld->netdev);
		phy_start(ld->phy);
		enable_irq(ld->netdev->irq);
	}

	for_each_gmac_netdev_local_priv(ld, i) {
		if (!ld->phy)
			continue;

		pmt_exit(ld);
	}

	return 0;
}
#else
#define higmac_dev_suspend	NULL
#define higmac_dev_resume	NULL
#endif

static struct platform_driver higmac_dev_driver = {
	.probe		= higmac_dev_probe,
	.remove		= higmac_dev_remove,
	.suspend	= higmac_dev_suspend,
	.resume		= higmac_dev_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= HIGMAC_DRIVER_NAME,
	},
};

static struct resource higmac_resources[] = {
	[0] = {/* eth0 io space */
		.start	= CONFIG_HIGMAC_IOBASE,
		.end	= CONFIG_HIGMAC_IOBASE + HIGMAC_IOSIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {/* eth1 io space */
		.start	= CONFIG_HIGMAC_IOBASE + HIGMAC_OFFSET,
		.end	= CONFIG_HIGMAC_IOBASE + HIGMAC_OFFSET
				+ HIGMAC_IOSIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[2] = {/* gmac0 irq */
		.start	= CONFIG_HIGMAC_IRQNUM,
		.end	= CONFIG_HIGMAC_IRQNUM,
		.flags	= IORESOURCE_IRQ,
	},
	[3] = {/* gmac1 irq */
		.start	= CONFIG_HIGMAC_IRQNUM + 1,
		.end	= CONFIG_HIGMAC_IRQNUM + 1,
		.flags	= IORESOURCE_IRQ,
	}
};

static u64 higmac_dmamask = DMA_BIT_MASK(32);

static void higmac_platform_dev_release(struct device *dev)
{
}

static struct platform_device higmac_platform_device = {
	.name		= HIGMAC_DRIVER_NAME,
	.id		= 0,
	.dev		= {
		.dma_mask = &higmac_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.release = higmac_platform_dev_release,
	},
	.num_resources	= ARRAY_SIZE(higmac_resources),
	.resource	= higmac_resources,
};

#include "proc-dev.c"

static int miscdev_registered;

static int __init higmac_init(void)
{
	int ret = 0, i;

	ret = platform_device_register(&higmac_platform_device);
	if (ret) {
		pr_err("register netdevice device failed!");
		goto _error_register_device;
	}

	ret = platform_driver_register(&higmac_dev_driver);
	if (ret) {
		pr_err("register netdevice driver failed!");
		goto _error_register_driver;
	}
	ret = misc_register(&gmac_dev);
	if (ret) {
		pr_err("register misc device failed!");
		goto _error_register_misc_dev;
	} else
		miscdev_registered = 1;

	higmac_proc_create();

	for (i = 0; i < CONFIG_GMAC_NUMS; i++) {
		pr_info("ETH%d: %s, phy_addr=%d, mii_name=%s\n", i,
				mii_to_str[higmac_board_info[i].phy_intf],
				higmac_board_info[i].phy_addr,
				higmac_board_info[i].mii_name);
	}

	return ret;
_error_register_misc_dev:
	platform_driver_unregister(&higmac_dev_driver);
_error_register_driver:
	platform_device_unregister(&higmac_platform_device);
_error_register_device:

	return -1;
}

static void __exit higmac_exit(void)
{
	if (miscdev_registered)
		misc_deregister(&gmac_dev);

	platform_driver_unregister(&higmac_dev_driver);

	platform_device_unregister(&higmac_platform_device);

	higmac_proc_destroy();
}

module_init(higmac_init);
module_exit(higmac_exit);

MODULE_AUTHOR("ZMJUN");
MODULE_DESCRIPTION("Hisilicon double GMAC driver, base on driver higmacv200 by CHH");
MODULE_LICENSE("GPL");
