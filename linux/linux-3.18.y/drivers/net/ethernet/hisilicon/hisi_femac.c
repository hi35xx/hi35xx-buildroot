/*
 * Hisilicon Fast Ethernet MAC Driver
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/circ_buf.h>
#include <linux/clk.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/tcp.h>
#include <net/ipv6.h>
#include <net/protocol.h>

/* MAC control register list */
#define MAC_PORTSEL			0x0200
#define MAC_PORTSEL_STAT_CPU		BIT(0)
#define MAC_PORTSEL_RMII		BIT(1)
#define MAC_PORTSET			0x0208
#define MAC_PORTSET_DUPLEX_FULL		BIT(0)
#define MAC_PORTSET_LINKED		BIT(1)
#define MAC_PORTSET_SPEED_100M		BIT(2)
#define MAC_SET				0x0210
#define MAX_FRAME_SIZE			1600
#define MAX_FRAME_SIZE_MASK		GENMASK(10, 0)
#define BIT_PAUSE_EN			BIT(18)
#define RX_COALESCE_SET			0x0340
#define RX_COALESCED_FRAME_OFFSET	24
#define RX_COALESCED_FRAMES		8
#define RX_COALESCED_TIMER		0x74
#define QLEN_SET			0x0344
#define RX_DEPTH_OFFSET			8
#define MAX_HW_FIFO_DEPTH		64
#define HW_TX_FIFO_DEPTH		12
#define HW_RX_FIFO_DEPTH		(MAX_HW_FIFO_DEPTH - HW_TX_FIFO_DEPTH)
#define FC_LEVEL			0x0348
#define BITS_FC_ACTIVE_THR_OFFSET	8
#define FC_DEACTIVE_THR_MASK		GENMASK(5, 0)
#define FC_ACTIVE_THR_MASK		GENMASK(13, 8)
#define BIT_FC_EN			BIT(14)
#define IQFRM_DES			0x0354
#define RX_FRAME_LEN_MASK		GENMASK(11, 0)
#define BITS_PAYLOAD_ERR_OFFSET		28
#define BITS_PAYLOAD_ERR_MASK		0x1
#define BITS_HEADER_ERR_OFFSET		29
#define BITS_HEADER_ERR_MASK		0x1
#define BITS_PAYLOAD_DONE_OFFSET	30
#define BITS_PAYLOAD_DONE_MASK		0x1
#define BITS_HEADER_DONE_OFFSET		31
#define BITS_HEADER_DONE_MASK		0x1
#define IQ_ADDR				0x0358
#define EQ_ADDR				0x0360
#define EQFRM_LEN			0x0364
#define ADDRQ_STAT			0x036C
#define TX_CNT_INUSE_MASK		GENMASK(5, 0)
#define BIT_TX_READY			BIT(24)
#define BIT_RX_READY			BIT(25)
#define RX_COE_CTRL			0x0380
#define BIT_COE_IPV6_UDP_ZERO_DROP	BIT(13)
#define BIT_COE_PAYLOAD_DROP		BIT(14)
#define BIT_COE_IPHDR_DROP		BIT(15)
#define COE_ERR_DROP			(BIT_COE_IPHDR_DROP | \
					BIT_COE_PAYLOAD_DROP | \
					BIT_COE_IPV6_UDP_ZERO_DROP)
#define TSO_DBG_EN			0x03A4
#define BITS_TSO_DBG_EN			BIT(31)
#define TSO_DBG_STATE			0x03A8
#define TSO_DBG_ADDR			0x03AC
#define TSO_DBG_TX_INFO			0x03B0
#define TSO_DBG_TX_ERR			0x03B4
/* global control register list */
#define GLB_HOSTMAC_L32			0x0000
#define GLB_HOSTMAC_H16			0x0004
#define GLB_SOFT_RESET			0x0008
#define SOFT_RESET_ALL			BIT(0)
#define GLB_FWCTRL			0x0010
#define FWCTRL_VLAN_ENABLE		BIT(0)
#define FWCTRL_FW2CPU_ENA		BIT(5)
#define FWCTRL_FWALL2CPU		BIT(7)
#define GLB_MACTCTRL			0x0014
#define MACTCTRL_UNI2CPU		BIT(1)
#define MACTCTRL_MULTI2CPU		BIT(3)
#define MACTCTRL_BROAD2CPU		BIT(5)
#define MACTCTRL_MACT_ENA		BIT(7)
#define GLB_IRQ_STAT			0x0030
#define GLB_IRQ_ENA			0x0034
#define IRQ_ENA_PORT0_MASK		GENMASK(7, 0)
#define IRQ_ENA_PORT0			BIT(18)
#define IRQ_ENA_ALL			BIT(19)
#define GLB_IRQ_RAW			0x0038
#define IRQ_INT_RX_RDY			BIT(0)
#define IRQ_INT_TX_PER_PACKET		BIT(1)
#define IRQ_INT_TX_FIFO_EMPTY		BIT(6)
#define IRQ_INT_MULTI_RXRDY		BIT(7)
#define INT_TX_ERR			BIT(8)
#define DEF_INT_MASK			(IRQ_INT_MULTI_RXRDY | \
					IRQ_INT_TX_PER_PACKET | \
					IRQ_INT_TX_FIFO_EMPTY)
#define GLB_MAC_L32_BASE		0x0100
#define GLB_MAC_H16_BASE		0x0104
#define MACFLT_HI16_MASK		GENMASK(15, 0)
#define BIT_MACFLT_ENA			BIT(17)
#define BIT_MACFLT_FW2CPU		BIT(21)
#define GLB_MAC_H16(reg)		(GLB_MAC_H16_BASE + ((reg) * 0x8))
#define GLB_MAC_L32(reg)		(GLB_MAC_L32_BASE + ((reg) * 0x8))
#define MAX_MAC_FILTER_NUM		8
#define MAX_UNICAST_ADDRESSES		2
#define MAX_MULTICAST_ADDRESSES		(MAX_MAC_FILTER_NUM - \
					MAX_UNICAST_ADDRESSES)
/* software tx and rx queue number, should be power of 2 */
#define TXQ_NUM				64
#define RXQ_NUM				128
#define FEMAC_POLL_WEIGHT		16
#define HW_CAP_TSO			BIT(0)
#define HW_CAP_RXCSUM			BIT(1)
#define HAS_TSO_CAP(hw_cap)		((hw_cap) & HW_CAP_TSO)
#define HAS_RXCSUM_CAP(hw_cap)		((hw_cap) & HW_CAP_RXCSUM)
#define RXBUF_ADDR_ALIGN_SIZE		64UL
/* UDP header len is 2 word */
#define UDP_HDR_LEN			2
/* IPv6 header len is 10 word */
#define IPV6_HDR_LEN			10
#define WORD_TO_BYTE			4

#define BIT_OFFSET_NFRAGS_NUM		11
#define BIT_OFFSET_PROT_HEADER_LEN	16
#define BIT_OFFSET_IP_HEADER_LEN	20
#define BIT_FLAG_SG			BIT(26)
#define BIT_FLAG_TXCSUM			BIT(27)
#define BIT_FLAG_UDP			BIT(28)
#define BIT_FLAG_IPV6			BIT(29)
#define BIT_FLAG_VLAN			BIT(30)
#define BIT_FLAG_TSO			BIT(31)

#define PHY_RESET_DELAYS_PROPERTY	"hisilicon,phy-reset-delays-us"

/* The threshold for activing tx flow ctrl.
 * When the left amount of receive queue descriptors is below this threshold,
 * hardware will send pause frame immediately.
 * We advise this value is set between 1 and 10.
 * Too bigger is not a good choice.
 * This value must be smaller than tx flow ctrl deactive threshold.
 */
#define TX_FLOW_CTRL_ACTIVE_THRESHOLD	3
/* The threshold for deactiving tx flow ctrl.
 * When the left amount of receive queue descriptors is
 * above or equal with this threshold,
 * hardware will exit flow control state.
 * We advise this value is set between 1 and 10.
 * Too bigger is not a good choice.
 * This value must be larger than tx flow ctrl active threshold.
 */
#define TX_FLOW_CTRL_DEACTIVE_THRESHOLD	5
#define FC_ACTIVE_MIN			1
#define FC_ACTIVE_DEFAULT		3
#define FC_ACTIVE_MAX			31
#define FC_DEACTIVE_MIN			1
#define FC_DEACTIVE_DEFAULT		5
#define FC_DEACTIVE_MAX			31

enum phy_reset_delays {
	PRE_DELAY,
	PULSE,
	POST_DELAY,
	DELAYS_NUM,
};

struct hisi_femac_queue {
	struct sk_buff **skb;
	dma_addr_t *dma_phys;
	int num;
	unsigned int head;
	unsigned int tail;
};

struct hisi_femac_tx_desc_ring {
	struct tx_desc *desc;
	dma_addr_t dma_phys;
};

struct hisi_femac_priv {
	void __iomem *port_base;
	void __iomem *glb_base;
	struct clk *clk;
	struct reset_control *mac_rst;
	struct reset_control *phy_rst;
	u32 phy_reset_delays[DELAYS_NUM];
	u32 link_status;

	struct device *dev;
	struct net_device *ndev;

	u32 hw_cap;
	struct hisi_femac_queue txq;
	struct hisi_femac_queue rxq;
	struct hisi_femac_tx_desc_ring tx_ring;
	u32 tx_fifo_used_cnt;
	struct napi_struct napi;

	/* 802.3x flow control */
	bool tx_pause_en;
	u32 tx_pause_active_thresh;
	u32 tx_pause_deactive_thresh;
};

struct frags_info {
	/* Word(2*i+2) */
	u32 addr;
	/* Word(2*i+3) */
	u32 size:16;
	u32 reserved:16;
};

struct tx_desc {
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

static void hisi_femac_irq_enable(struct hisi_femac_priv *priv, u32 irqs)
{
	u32 val;

	val = readl(priv->glb_base + GLB_IRQ_ENA);
	writel(val | irqs, priv->glb_base + GLB_IRQ_ENA);
}

static void hisi_femac_irq_disable(struct hisi_femac_priv *priv, u32 irqs)
{
	u32 val;

	val = readl(priv->glb_base + GLB_IRQ_ENA);
	writel(val & (~irqs), priv->glb_base + GLB_IRQ_ENA);
}

static void hisi_femac_set_flow_ctrl(struct hisi_femac_priv *priv)
{
	unsigned int pause_en;
	unsigned int tx_flow_ctrl;

	tx_flow_ctrl = readl(priv->port_base + FC_LEVEL);
	tx_flow_ctrl &= ~FC_DEACTIVE_THR_MASK;
	tx_flow_ctrl |= priv->tx_pause_deactive_thresh;
	tx_flow_ctrl &= ~FC_ACTIVE_THR_MASK;
	tx_flow_ctrl |= priv->tx_pause_active_thresh <<
				BITS_FC_ACTIVE_THR_OFFSET;

	pause_en = readl(priv->port_base + MAC_SET);

	if (priv->tx_pause_en) {
		tx_flow_ctrl |= BIT_FC_EN;
		pause_en |= BIT_PAUSE_EN;
	} else {
		tx_flow_ctrl &= ~BIT_FC_EN;
		pause_en &= ~BIT_PAUSE_EN;
	}

	writel(tx_flow_ctrl, priv->port_base + FC_LEVEL);

	writel(pause_en, priv->port_base + MAC_SET);
}

static void hisi_femac_tx_sg_dma_unmap(struct hisi_femac_priv *priv,
				       struct sk_buff *skb, unsigned int pos)
{
	struct tx_desc *desc_cur;
	dma_addr_t addr;
	u32 len;
	int i;

	desc_cur = priv->tx_ring.desc + pos;

	addr = desc_cur->linear_addr;
	len = desc_cur->linear_len;
	dma_unmap_single(priv->dev, addr, len, DMA_TO_DEVICE);

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		addr = desc_cur->frags[i].addr;
		len = desc_cur->frags[i].size;
		dma_unmap_page(priv->dev, addr, len, DMA_TO_DEVICE);
	}
}

static void hisi_femac_tx_dma_unmap(struct hisi_femac_priv *priv,
				    struct sk_buff *skb, unsigned int pos)
{
	if (!(skb_is_gso(skb) || skb_shinfo(skb)->nr_frags)) {
		dma_addr_t dma_addr;

		dma_addr = priv->txq.dma_phys[pos];
		dma_unmap_single(priv->dev, dma_addr, skb->len, DMA_TO_DEVICE);
	} else {
		hisi_femac_tx_sg_dma_unmap(priv, skb, pos);
	}
}

static void hisi_femac_xmit_reclaim(struct net_device *dev)
{
	struct sk_buff *skb;
	struct hisi_femac_priv *priv = netdev_priv(dev);
	struct hisi_femac_queue *txq = &priv->txq;
	unsigned int bytes_compl = 0, pkts_compl = 0;
	u32 val;

	netif_tx_lock(dev);

	val = readl(priv->port_base + ADDRQ_STAT) & TX_CNT_INUSE_MASK;
	while (val < priv->tx_fifo_used_cnt) {
		skb = txq->skb[txq->tail];
		if (unlikely(!skb)) {
			netdev_err(dev, "xmitq_cnt_inuse=%d, tx_fifo_used=%d\n",
				   val, priv->tx_fifo_used_cnt);
			break;
		}
		hisi_femac_tx_dma_unmap(priv, skb, txq->tail);
		pkts_compl++;
		bytes_compl += skb->len;
		dev_kfree_skb_any(skb);

		priv->tx_fifo_used_cnt--;

		val = readl(priv->port_base + ADDRQ_STAT) & TX_CNT_INUSE_MASK;
		txq->skb[txq->tail] = NULL;
		txq->tail = (txq->tail + 1) % txq->num;
	}

	netdev_completed_queue(dev, pkts_compl, bytes_compl);

	if (unlikely(netif_queue_stopped(dev)) && pkts_compl)
		netif_wake_queue(dev);

	netif_tx_unlock(dev);
}

static void hisi_femac_get_tso_err_info(struct hisi_femac_priv *priv)
{
	unsigned int reg_addr, reg_tx_info, reg_tx_err;
	unsigned int sg_index;
	struct tx_desc *sg_desc;
	int *sg_word;
	int i;

	reg_addr = readl(priv->port_base + TSO_DBG_ADDR);
	reg_tx_info = readl(priv->port_base + TSO_DBG_TX_INFO);
	reg_tx_err = readl(priv->port_base + TSO_DBG_TX_ERR);

	WARN(1, "tx err=0x%x, tx_info=0x%x, addr=0x%x\n",
	     reg_tx_err, reg_tx_info, reg_addr);

	sg_index = (reg_addr - priv->tx_ring.dma_phys) / sizeof(struct tx_desc);
	sg_desc = priv->tx_ring.desc + sg_index;
	sg_word = (int *)sg_desc;
	for (i = 0; i < sizeof(struct tx_desc) / sizeof(int); i++)
		pr_err("%s,%d: sg_desc word[%d]=0x%x\n",
		       __func__, __LINE__, i, sg_word[i]);

	/* restart MAC to transmit next packet */
	hisi_femac_irq_disable(priv, INT_TX_ERR);
	/* The following is recovery code,
	 * allow netcard transmit packet again.
	 * But now we disable it for error debug.
	 */
#if 0
	readl(priv->port_base + TSO_DBG_STATE));
	hisi_femac_irq_enable(priv, INT_TX_ERR);
#endif
}

static netdev_tx_t hisi_femac_net_xmit(struct sk_buff *skb,
				       struct net_device *dev);

static netdev_tx_t hisi_femac_sw_gso(struct sk_buff *skb,
				     struct net_device *dev)
{
	struct sk_buff *segs, *curr_skb;
	netdev_features_t features = dev->features;

	features &= ~(NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
			NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_UFO);
	segs = skb_gso_segment(skb, features);
	if (IS_ERR_OR_NULL(segs))
		goto drop;

	do {
		curr_skb = segs;
		segs = segs->next;
		curr_skb->next = NULL;
		if (hisi_femac_net_xmit(curr_skb, dev)) {
			dev_kfree_skb(curr_skb);
			while (segs) {
				curr_skb = segs;
				segs = segs->next;
				curr_skb->next = NULL;
				dev_kfree_skb_any(curr_skb);
			}
			goto drop;
		}
	} while (segs);

	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;

drop:
	dev_kfree_skb_any(skb);
	dev->stats.tx_dropped++;
	return NETDEV_TX_OK;
}

static void hisi_femac_do_udp_checksum(struct sk_buff *skb)
{
	int offset;
	__wsum csum;
	__sum16 udp_csum;

	offset = skb_checksum_start_offset(skb);
	WARN_ON(offset >= skb_headlen(skb));
	csum = skb_checksum(skb, offset, skb->len - offset, 0);

	offset += skb->csum_offset;
	WARN_ON(offset + sizeof(__sum16) > skb_headlen(skb));

	udp_csum = csum_fold(csum);
	if (udp_csum == 0)
		udp_csum = CSUM_MANGLED_0;

	*(__sum16 *)(skb->data + offset) = udp_csum;

	skb->ip_summed = CHECKSUM_NONE;
}

static inline __be16 hisi_femac_get_l3_proto(struct sk_buff *skb)
{
	__be16 l3_proto;

	l3_proto = skb->protocol;
	if (skb->protocol == htons(ETH_P_8021Q))
		l3_proto = vlan_get_protocol(skb);

	return l3_proto;
}

static inline bool hisi_femac_skb_is_ipv6(struct sk_buff *skb)
{
	return (hisi_femac_get_l3_proto(skb) == htons(ETH_P_IPV6));
}

static int hisi_femac_check_hw_capability_for_ipv6(struct sk_buff *skb)
{
	unsigned int l4_proto = IPPROTO_MAX;

	l4_proto = ipv6_hdr(skb)->nexthdr;

	if ((l4_proto != IPPROTO_TCP) && (l4_proto != IPPROTO_UDP)) {
		/* when IPv6 next header is not tcp or udp,
		 * it means that IPv6 next header is extension header.
		 * Hardware can't deal with this case,
		 * so do checksumming by software or do GSO by software.
		 */
		if (skb_is_gso(skb))
			return -ENOTSUPP;

		if (skb->ip_summed == CHECKSUM_PARTIAL &&
		    skb_checksum_help(skb))
			return -EINVAL;
	}

	return 0;
}

static int hisi_femac_check_hw_capability(struct sk_buff *skb)
{
	/* if tcp_mtu_probe() use (2 * tp->mss_cache) as probe_size,
	 * the linear data length will be larger than 2048,
	 * the MAC can't handle it, so let the software do it.
	 */
	if (skb_is_gso(skb) && (skb_headlen(skb) > 2048))
		return -ENOTSUPP;

	if (hisi_femac_skb_is_ipv6(skb))
		return hisi_femac_check_hw_capability_for_ipv6(skb);

	return 0;
}

static u32 hisi_femac_get_pkt_info(struct sk_buff *skb)
{
	__be16 l3_proto;
	unsigned int l4_proto = IPPROTO_MAX;
	bool do_txcsum = false;
	int max_data_len = skb->len - ETH_HLEN;
	unsigned int max_mss = ETH_DATA_LEN;
	u32 pkt_info = 0;

	if (skb->ip_summed == CHECKSUM_PARTIAL)
		do_txcsum = true;

	l3_proto = skb->protocol;
	if (skb->protocol == htons(ETH_P_8021Q)) {
		l3_proto = vlan_get_protocol(skb);
		max_data_len -= VLAN_HLEN;
		pkt_info |= BIT_FLAG_VLAN;
	}

	if (l3_proto == htons(ETH_P_IP)) {
		struct iphdr *iph = ip_hdr(skb);

		if ((max_data_len >= GSO_MAX_SIZE) &&
		    (ntohs(iph->tot_len) <= (iph->ihl << 2)))
			iph->tot_len = htons(GSO_MAX_SIZE - 1);

		max_mss -= iph->ihl * WORD_TO_BYTE;
		pkt_info |= (iph->ihl << BIT_OFFSET_IP_HEADER_LEN);
		l4_proto = iph->protocol;
	} else if (l3_proto == htons(ETH_P_IPV6)) {
		max_mss -= IPV6_HDR_LEN * WORD_TO_BYTE;
		pkt_info |= BIT_FLAG_IPV6;
		pkt_info |= (IPV6_HDR_LEN << BIT_OFFSET_IP_HEADER_LEN);
		l4_proto = ipv6_hdr(skb)->nexthdr;
	} else {
		do_txcsum = false;
	}

	if (l4_proto == IPPROTO_TCP) {
		max_mss -= tcp_hdr(skb)->doff * WORD_TO_BYTE;
		pkt_info |= (tcp_hdr(skb)->doff << BIT_OFFSET_PROT_HEADER_LEN);
	} else if (l4_proto == IPPROTO_UDP) {
		if (l3_proto == htons(ETH_P_IPV6))
			max_mss -= sizeof(struct frag_hdr);
		pkt_info |= (BIT_FLAG_UDP |
				(UDP_HDR_LEN << BIT_OFFSET_PROT_HEADER_LEN));
	} else {
		do_txcsum = false;
	}

	/* Although netcard support UFO feature, it can't deal with
	 * UDP header checksum.
	 * So the driver will do UDP header checksum and netcard will just
	 * fragment the packet.
	 */
	if (do_txcsum && skb_is_gso(skb) && (l4_proto == IPPROTO_UDP)) {
		hisi_femac_do_udp_checksum(skb);
		do_txcsum = false;
	}

	if (do_txcsum)
		pkt_info |= BIT_FLAG_TXCSUM;

	if (skb_is_gso(skb))
		pkt_info |= (BIT_FLAG_SG | BIT_FLAG_TSO);
	else if (skb_shinfo(skb)->nr_frags)
		pkt_info |= BIT_FLAG_SG;

	pkt_info |= (skb_shinfo(skb)->nr_frags << BIT_OFFSET_NFRAGS_NUM);
	pkt_info |= (skb_is_gso(skb) ?
		((skb_shinfo(skb)->gso_size > max_mss) ? max_mss :
		skb_shinfo(skb)->gso_size) : (skb->len + ETH_FCS_LEN));

	return pkt_info;
}

static int hisi_femac_fill_sg_desc(struct hisi_femac_priv *priv,
				   struct sk_buff *skb, unsigned int pos)
{
	struct tx_desc *desc_cur;
	dma_addr_t addr;
	int ret;
	int i;

	desc_cur = priv->tx_ring.desc + pos;

	desc_cur->ipv6_id = ntohl(skb_shinfo(skb)->ip6_frag_id);

	desc_cur->total_len = skb->len;
	addr = dma_map_single(priv->dev, skb->data, skb_headlen(skb),
			      DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(priv->dev, addr)))
		return -EINVAL;
	desc_cur->linear_addr = addr;
	desc_cur->linear_len = skb_headlen(skb);

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		int len = frag->size;

		addr = skb_frag_dma_map(priv->dev, frag, 0, len, DMA_TO_DEVICE);
		ret = dma_mapping_error(priv->dev, addr);
		if (unlikely(ret))
			return -EINVAL;
		desc_cur->frags[i].addr = addr;
		desc_cur->frags[i].size = len;
	}

	return 0;
}

static void hisi_femac_adjust_link(struct net_device *dev)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);
	struct phy_device *phy = dev->phydev;
	u32 status = 0;

	if (phy->link)
		status |= MAC_PORTSET_LINKED;
	if (phy->duplex == DUPLEX_FULL)
		status |= MAC_PORTSET_DUPLEX_FULL;
	if (phy->speed == SPEED_100)
		status |= MAC_PORTSET_SPEED_100M;

	if ((status != priv->link_status) &&
	    ((status | priv->link_status) & MAC_PORTSET_LINKED)) {
		writel(status, priv->port_base + MAC_PORTSET);
		priv->link_status = status;
		phy_print_status(phy);

		priv->tx_pause_en = phy->pause;
		hisi_femac_set_flow_ctrl(priv);
	}
}

static void hisi_femac_rx_refill(struct hisi_femac_priv *priv)
{
	struct hisi_femac_queue *rxq = &priv->rxq;
	struct sk_buff *skb;
	u32 pos;
	u32 len = MAX_FRAME_SIZE;
	dma_addr_t addr;
	u32 alloc_rxbuf_align = 0;
	int reserve_room = 0;

	pos = rxq->head;
	while (readl(priv->port_base + ADDRQ_STAT) & BIT_RX_READY) {
		if (!CIRC_SPACE(pos, rxq->tail, rxq->num))
			break;
		if (unlikely(rxq->skb[pos])) {
			netdev_err(priv->ndev, "err skb[%d]=%p\n",
				   pos, rxq->skb[pos]);
			break;
		}
		len = MAX_FRAME_SIZE + RXBUF_ADDR_ALIGN_SIZE;
		skb = netdev_alloc_skb_ip_align(priv->ndev, len);
		if (unlikely(!skb))
			break;

		alloc_rxbuf_align = ((unsigned long)skb->data - NET_IP_ALIGN) &
						(RXBUF_ADDR_ALIGN_SIZE - 1);
		if (alloc_rxbuf_align) {
			reserve_room = RXBUF_ADDR_ALIGN_SIZE - alloc_rxbuf_align;
			len -= reserve_room;
			skb_reserve(skb, reserve_room);
		}

		addr = dma_map_single(priv->dev, skb->data, len,
				      DMA_FROM_DEVICE);
		if (dma_mapping_error(priv->dev, addr)) {
			dev_kfree_skb_any(skb);
			break;
		}
		rxq->dma_phys[pos] = addr;
		rxq->skb[pos] = skb;
		writel(addr, priv->port_base + IQ_ADDR);
		pos = (pos + 1) % rxq->num;
	}
	rxq->head = pos;
}

static u32 hisi_femac_rx(struct net_device *dev, int limit)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);
	struct hisi_femac_queue *rxq = &priv->rxq;
	struct sk_buff *skb;
	dma_addr_t addr;
	u32 rx_pkt_info, pos, len, rx_pkts_num = 0;
	int hdr_csum_done, hdr_csum_err;
	int payload_csum_done, payload_csum_err;

	pos = rxq->tail;
	while (readl(priv->glb_base + GLB_IRQ_RAW) & IRQ_INT_RX_RDY) {
		rx_pkt_info = readl(priv->port_base + IQFRM_DES);
		len = rx_pkt_info & RX_FRAME_LEN_MASK;
		len -= ETH_FCS_LEN;

		/* tell hardware we will deal with this packet */
		writel(IRQ_INT_RX_RDY, priv->glb_base + GLB_IRQ_RAW);

		rx_pkts_num++;

		skb = rxq->skb[pos];
		if (unlikely(!skb)) {
			netdev_err(dev, "rx skb NULL. pos=%d\n", pos);
			break;
		}
		rxq->skb[pos] = NULL;

		addr = rxq->dma_phys[pos];
		dma_unmap_single(priv->dev, addr, MAX_FRAME_SIZE,
				 DMA_FROM_DEVICE);
		skb_put(skb, len);
		if (unlikely(skb->len > MAX_FRAME_SIZE)) {
			netdev_err(dev, "rcv len err, len = %d\n", skb->len);
			dev->stats.rx_errors++;
			dev->stats.rx_length_errors++;
			dev_kfree_skb_any(skb);
			goto next;
		}

		skb->ip_summed = CHECKSUM_NONE;
		if (dev->features & NETIF_F_RXCSUM) {
			hdr_csum_done =
				(rx_pkt_info >> BITS_HEADER_DONE_OFFSET) &
				BITS_HEADER_DONE_MASK;
			payload_csum_done =
				(rx_pkt_info >> BITS_PAYLOAD_DONE_OFFSET) &
				BITS_PAYLOAD_DONE_MASK;
			hdr_csum_err =
				(rx_pkt_info >> BITS_HEADER_ERR_OFFSET) &
				BITS_HEADER_ERR_MASK;
			payload_csum_err =
				(rx_pkt_info >> BITS_PAYLOAD_ERR_OFFSET) &
				BITS_PAYLOAD_ERR_MASK;

			if (hdr_csum_done && payload_csum_done) {
				if (unlikely(hdr_csum_err)) {
					dev->stats.rx_errors++;
					dev->stats.rx_crc_errors++;
					dev_kfree_skb_any(skb);
					goto next;
				} else if (!payload_csum_err) {
					skb->ip_summed = CHECKSUM_UNNECESSARY;
				}
			}
		}

		skb->protocol = eth_type_trans(skb, dev);
		napi_gro_receive(&priv->napi, skb);
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += skb->len;
next:
		pos = (pos + 1) % rxq->num;
		if (rx_pkts_num >= limit)
			break;
	}
	rxq->tail = pos;

	hisi_femac_rx_refill(priv);

	return rx_pkts_num;
}

static int hisi_femac_poll(struct napi_struct *napi, int budget)
{
	struct hisi_femac_priv *priv = container_of(napi,
					struct hisi_femac_priv, napi);
	struct net_device *dev = priv->ndev;
	int work_done = 0, task = budget;
	u32 ints, num;

	do {
		hisi_femac_xmit_reclaim(dev);
		num = hisi_femac_rx(dev, task);
		work_done += num;
		task -= num;
		if (work_done >= budget)
			break;

		ints = readl(priv->glb_base + GLB_IRQ_RAW);
		writel(ints & DEF_INT_MASK,
		       priv->glb_base + GLB_IRQ_RAW);
	} while (ints & DEF_INT_MASK);

	if (work_done < budget) {
		napi_complete(napi);
		hisi_femac_irq_enable(priv, DEF_INT_MASK &
					(~IRQ_INT_TX_PER_PACKET));
	}

	return work_done;
}

static irqreturn_t hisi_femac_interrupt(int irq, void *dev_id)
{
	u32 ints;
	struct net_device *dev = (struct net_device *)dev_id;
	struct hisi_femac_priv *priv = netdev_priv(dev);

	ints = readl(priv->glb_base + GLB_IRQ_RAW);

	if (likely(ints & DEF_INT_MASK)) {
		writel(ints & DEF_INT_MASK,
		       priv->glb_base + GLB_IRQ_RAW);
		hisi_femac_irq_disable(priv, DEF_INT_MASK);
		napi_schedule(&priv->napi);
	}

	if (HAS_TSO_CAP(priv->hw_cap) &&
	    unlikely(ints & INT_TX_ERR))
		hisi_femac_get_tso_err_info(priv);

	return IRQ_HANDLED;
}

static int hisi_femac_init_tx_descriptor_ring(struct hisi_femac_priv *priv)
{
	priv->tx_ring.desc = (struct tx_desc *)dma_zalloc_coherent(priv->dev,
			TXQ_NUM * sizeof(struct tx_desc),
			&priv->tx_ring.dma_phys,
			GFP_KERNEL);
	if (!priv->tx_ring.desc)
		return -ENOMEM;

	return 0;
}

static void hisi_femac_destroy_tx_descriptor_ring(struct hisi_femac_priv *priv)
{
	if (priv->tx_ring.desc)
		dma_free_coherent(priv->dev,
				  TXQ_NUM * sizeof(struct tx_desc),
				  priv->tx_ring.desc, priv->tx_ring.dma_phys);
	priv->tx_ring.desc = NULL;
}

static int hisi_femac_init_queue(struct device *dev,
				 struct hisi_femac_queue *queue,
				 unsigned int num)
{
	queue->skb = devm_kcalloc(dev, num, sizeof(struct sk_buff *),
				  GFP_KERNEL);
	if (!queue->skb)
		return -ENOMEM;

	queue->dma_phys = devm_kcalloc(dev, num, sizeof(dma_addr_t),
				       GFP_KERNEL);
	if (!queue->dma_phys)
		return -ENOMEM;

	queue->num = num;
	queue->head = 0;
	queue->tail = 0;

	return 0;
}

static int hisi_femac_init_tx_and_rx_queues(struct hisi_femac_priv *priv)
{
	int ret;

	ret = hisi_femac_init_queue(priv->dev, &priv->txq, TXQ_NUM);
	if (ret)
		return ret;

	ret = hisi_femac_init_queue(priv->dev, &priv->rxq, RXQ_NUM);
	if (ret)
		return ret;

	priv->tx_fifo_used_cnt = 0;

	return 0;
}

static void hisi_femac_free_skb_rings(struct hisi_femac_priv *priv)
{
	struct hisi_femac_queue *txq = &priv->txq;
	struct hisi_femac_queue *rxq = &priv->rxq;
	struct sk_buff *skb;
	dma_addr_t dma_addr;
	u32 pos;

	pos = rxq->tail;
	while (pos != rxq->head) {
		skb = rxq->skb[pos];
		if (unlikely(!skb)) {
			netdev_err(priv->ndev, "NULL rx skb. pos=%d, head=%d\n",
				   pos, rxq->head);
			continue;
		}

		dma_addr = rxq->dma_phys[pos];
		dma_unmap_single(priv->dev, dma_addr, MAX_FRAME_SIZE,
				 DMA_FROM_DEVICE);

		dev_kfree_skb_any(skb);
		rxq->skb[pos] = NULL;
		pos = (pos + 1) % rxq->num;
	}
	rxq->tail = pos;

	pos = txq->tail;
	while (pos != txq->head) {
		skb = txq->skb[pos];
		if (unlikely(!skb)) {
			netdev_err(priv->ndev, "NULL tx skb. pos=%d, head=%d\n",
				   pos, txq->head);
			continue;
		}
		hisi_femac_tx_dma_unmap(priv, skb, pos);
		dev_kfree_skb_any(skb);
		txq->skb[pos] = NULL;
		pos = (pos + 1) % txq->num;
	}
	txq->tail = pos;
	priv->tx_fifo_used_cnt = 0;
}

static int hisi_femac_set_hw_mac_addr(struct hisi_femac_priv *priv,
				      unsigned char *mac)
{
	u32 reg;

	reg = mac[1] | (mac[0] << 8);
	writel(reg, priv->glb_base + GLB_HOSTMAC_H16);

	reg = mac[5] | (mac[4] << 8) | (mac[3] << 16) | (mac[2] << 24);
	writel(reg, priv->glb_base + GLB_HOSTMAC_L32);

	return 0;
}

static int hisi_femac_port_reset(struct hisi_femac_priv *priv)
{
	u32 val;

	val = readl(priv->glb_base + GLB_SOFT_RESET);
	val |= SOFT_RESET_ALL;
	writel(val, priv->glb_base + GLB_SOFT_RESET);

	usleep_range(500, 800);

	val &= ~SOFT_RESET_ALL;
	writel(val, priv->glb_base + GLB_SOFT_RESET);

	return 0;
}

static int hisi_femac_net_open(struct net_device *dev)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);

	hisi_femac_set_hw_mac_addr(priv, dev->dev_addr);
	/* clear interrupts will drop the first packet MAC have received,
	 * so do it before refill the rx free skbs.
	 */
	writel(IRQ_ENA_PORT0_MASK, priv->glb_base + GLB_IRQ_RAW);
	hisi_femac_rx_refill(priv);

	netif_carrier_off(dev);
	netdev_reset_queue(dev);
	netif_start_queue(dev);
	napi_enable(&priv->napi);

	priv->link_status = 0;
	if (dev->phydev)
		phy_start(dev->phydev);

	hisi_femac_irq_enable(priv, IRQ_ENA_ALL | IRQ_ENA_PORT0 | DEF_INT_MASK);
	if (HAS_TSO_CAP(priv->hw_cap))
		hisi_femac_irq_enable(priv, INT_TX_ERR);

	return 0;
}

static void hisi_femac_port_init(struct hisi_femac_priv *priv);

static int hisi_femac_net_close(struct net_device *dev)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);

	hisi_femac_irq_disable(priv, IRQ_ENA_PORT0);

	if (dev->phydev)
		phy_stop(dev->phydev);

	netif_stop_queue(dev);
	napi_disable(&priv->napi);

	/* reset MAC port first before free skb rings
	 * to prevent potential risk of use-after-free.
	 */
	hisi_femac_port_reset(priv);
	hisi_femac_port_init(priv);

	priv->tx_pause_en = false;
	hisi_femac_set_flow_ctrl(priv);
	hisi_femac_free_skb_rings(priv);

	return 0;
}

static netdev_tx_t hisi_femac_net_xmit(struct sk_buff *skb,
				       struct net_device *dev)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);
	struct hisi_femac_queue *txq = &priv->txq;
	dma_addr_t addr;
	int ret;
	u32 pkt_info;
	u32 val;

	val = readl(priv->port_base + ADDRQ_STAT);
	val &= BIT_TX_READY;
	if (!val) {
		hisi_femac_irq_enable(priv, IRQ_INT_TX_PER_PACKET);
		dev->stats.tx_dropped++;
		dev->stats.tx_fifo_errors++;
		netif_stop_queue(dev);
		return NETDEV_TX_BUSY;
	}

	if (unlikely(!CIRC_SPACE(txq->head, txq->tail,
				 txq->num))) {
		hisi_femac_irq_enable(priv, IRQ_INT_TX_PER_PACKET);
		dev->stats.tx_dropped++;
		dev->stats.tx_fifo_errors++;
		netif_stop_queue(dev);
		return NETDEV_TX_BUSY;
	}

	ret = hisi_femac_check_hw_capability(skb);
	if (unlikely(ret)) {
		if (ret == -ENOTSUPP)
			return hisi_femac_sw_gso(skb, dev);

		dev_kfree_skb_any(skb);
		dev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	pkt_info = hisi_femac_get_pkt_info(skb);

	if (!(skb_is_gso(skb) || skb_shinfo(skb)->nr_frags)) {
		addr = dma_map_single(priv->dev, skb->data,
				      skb->len, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(priv->dev, addr))) {
			dev_kfree_skb_any(skb);
			dev->stats.tx_dropped++;
			return NETDEV_TX_OK;
		}
	} else {
		ret = hisi_femac_fill_sg_desc(priv, skb, txq->head);
		if (unlikely(ret)) {
			dev_kfree_skb_any(skb);
			dev->stats.tx_dropped++;
			return NETDEV_TX_OK;
		}

		addr = priv->tx_ring.dma_phys +
			txq->head * sizeof(struct tx_desc);

		/* Ensure desc info writen to memory before config hardware */
		wmb();
	}
	txq->dma_phys[txq->head] = addr;

	txq->skb[txq->head] = skb;
	txq->head = (txq->head + 1) % txq->num;

	writel(addr, priv->port_base + EQ_ADDR);
	writel(pkt_info, priv->port_base + EQFRM_LEN);

	priv->tx_fifo_used_cnt++;

	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;
	netdev_sent_queue(dev, skb->len);

	return NETDEV_TX_OK;
}

static int hisi_femac_set_mac_address(struct net_device *dev, void *p)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);
	struct sockaddr *skaddr = p;

	if (!is_valid_ether_addr(skaddr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, skaddr->sa_data, dev->addr_len);
	dev->addr_assign_type &= ~NET_ADDR_RANDOM;

	hisi_femac_set_hw_mac_addr(priv, dev->dev_addr);

	return 0;
}

static void hisi_femac_enable_hw_addr_filter(struct hisi_femac_priv *priv,
					     unsigned int reg_n, bool enable)
{
	u32 val;

	val = readl(priv->glb_base + GLB_MAC_H16(reg_n));
	if (enable)
		val |= BIT_MACFLT_ENA;
	else
		val &= ~BIT_MACFLT_ENA;
	writel(val, priv->glb_base + GLB_MAC_H16(reg_n));
}

static void hisi_femac_set_hw_addr_filter(struct hisi_femac_priv *priv,
					  unsigned char *addr,
					  unsigned int reg_n)
{
	unsigned int high, low;
	u32 val;

	high = GLB_MAC_H16(reg_n);
	low = GLB_MAC_L32(reg_n);

	val = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5];
	writel(val, priv->glb_base + low);

	val = readl(priv->glb_base + high);
	val &= ~MACFLT_HI16_MASK;
	val |= ((addr[0] << 8) | addr[1]);
	val |= (BIT_MACFLT_ENA | BIT_MACFLT_FW2CPU);
	writel(val, priv->glb_base + high);
}

static void hisi_femac_set_promisc_mode(struct hisi_femac_priv *priv,
					bool promisc_mode)
{
	u32 val;

	val = readl(priv->glb_base + GLB_FWCTRL);
	if (promisc_mode)
		val |= FWCTRL_FWALL2CPU;
	else
		val &= ~FWCTRL_FWALL2CPU;
	writel(val, priv->glb_base + GLB_FWCTRL);
}

/* Handle multiple multicast addresses (perfect filtering)*/
static void hisi_femac_set_mc_addr_filter(struct hisi_femac_priv *priv)
{
	struct net_device *dev = priv->ndev;
	u32 val;

	val = readl(priv->glb_base + GLB_MACTCTRL);
	if ((netdev_mc_count(dev) > MAX_MULTICAST_ADDRESSES) ||
	    (dev->flags & IFF_ALLMULTI)) {
		val |= MACTCTRL_MULTI2CPU;
	} else {
		int reg = MAX_UNICAST_ADDRESSES;
		int i;
		struct netdev_hw_addr *ha;

		for (i = reg; i < MAX_MAC_FILTER_NUM; i++)
			hisi_femac_enable_hw_addr_filter(priv, i, false);

		netdev_for_each_mc_addr(ha, dev) {
			hisi_femac_set_hw_addr_filter(priv, ha->addr, reg);
			reg++;
		}
		val &= ~MACTCTRL_MULTI2CPU;
	}
	writel(val, priv->glb_base + GLB_MACTCTRL);
}

/* Handle multiple unicast addresses (perfect filtering)*/
static void hisi_femac_set_uc_addr_filter(struct hisi_femac_priv *priv)
{
	struct net_device *dev = priv->ndev;
	u32 val;

	val = readl(priv->glb_base + GLB_MACTCTRL);
	if (netdev_uc_count(dev) > MAX_UNICAST_ADDRESSES) {
		val |= MACTCTRL_UNI2CPU;
	} else {
		int reg = 0;
		int i;
		struct netdev_hw_addr *ha;

		for (i = reg; i < MAX_UNICAST_ADDRESSES; i++)
			hisi_femac_enable_hw_addr_filter(priv, i, false);

		netdev_for_each_uc_addr(ha, dev) {
			hisi_femac_set_hw_addr_filter(priv, ha->addr, reg);
			reg++;
		}
		val &= ~MACTCTRL_UNI2CPU;
	}
	writel(val, priv->glb_base + GLB_MACTCTRL);
}

static void hisi_femac_net_set_rx_mode(struct net_device *dev)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);

	if (dev->flags & IFF_PROMISC) {
		hisi_femac_set_promisc_mode(priv, true);
	} else {
		hisi_femac_set_promisc_mode(priv, false);
		hisi_femac_set_mc_addr_filter(priv);
		hisi_femac_set_uc_addr_filter(priv);
	}
}

static int hisi_femac_net_ioctl(struct net_device *dev,
				struct ifreq *ifreq, int cmd)
{
	if (!netif_running(dev))
		return -EINVAL;

	if (!dev->phydev)
		return -EINVAL;

	return phy_mii_ioctl(dev->phydev, ifreq, cmd);
}

static void hisi_femac_get_pauseparam(struct net_device *dev,
				      struct ethtool_pauseparam *pause)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);

	pause->autoneg = dev->phydev->autoneg;
	pause->rx_pause = 1;
	if (priv->tx_pause_en)
		pause->tx_pause = 1;
}

static int hisi_femac_set_pauseparam(struct net_device *dev,
				     struct ethtool_pauseparam *pause)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);
	struct phy_device *phy = dev->phydev;
	int ret = 0;

	if (pause->rx_pause == 0)
		return -EINVAL;

	if (pause->tx_pause != priv->tx_pause_en) {
		priv->tx_pause_en = pause->tx_pause;
		hisi_femac_set_flow_ctrl(priv);
	}

	if (phy->autoneg) {
		if (netif_running(dev)) {
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

static void hisi_femac_enable_rxcsum_drop(struct hisi_femac_priv *priv,
					  bool drop)
{
	unsigned int val;

	val = readl(priv->port_base + RX_COE_CTRL);
	val &= ~COE_ERR_DROP;
	if (drop)
		val |= (BIT_COE_IPHDR_DROP | BIT_COE_IPV6_UDP_ZERO_DROP);
	writel(val, priv->port_base + RX_COE_CTRL);
}

static int hisi_femac_set_features(struct net_device *dev,
				   netdev_features_t features)
{
	struct hisi_femac_priv *priv = netdev_priv(dev);
	netdev_features_t changed = dev->features ^ features;

	if (changed & NETIF_F_RXCSUM) {
		if (features & NETIF_F_RXCSUM)
			hisi_femac_enable_rxcsum_drop(priv, true);
		else
			hisi_femac_enable_rxcsum_drop(priv, false);
	}

	return 0;
}

static int hisi_femac_get_settings(struct net_device *ndev,
				   struct ethtool_cmd *cmd)
{
	if (!ndev->phydev)
		return -EINVAL;

	return phy_ethtool_gset(ndev->phydev, cmd);
}

static int hisi_femac_set_settings(struct net_device *ndev,
				   struct ethtool_cmd *cmd)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (!ndev->phydev)
		return -EINVAL;

	return phy_ethtool_sset(ndev->phydev, cmd);
}

static struct ethtool_ops hisi_femac_ethtools_ops = {
	.get_link		= ethtool_op_get_link,
	.get_settings		= hisi_femac_get_settings,
	.set_settings		= hisi_femac_set_settings,
	.get_pauseparam		= hisi_femac_get_pauseparam,
	.set_pauseparam		= hisi_femac_set_pauseparam,
};

static const struct net_device_ops hisi_femac_netdev_ops = {
	.ndo_open		= hisi_femac_net_open,
	.ndo_stop		= hisi_femac_net_close,
	.ndo_start_xmit		= hisi_femac_net_xmit,
	.ndo_do_ioctl		= hisi_femac_net_ioctl,
	.ndo_set_mac_address	= hisi_femac_set_mac_address,
	.ndo_set_rx_mode	= hisi_femac_net_set_rx_mode,
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_set_features	= hisi_femac_set_features,
};

static void hisi_femac_verify_flow_ctrl_args(struct hisi_femac_priv *priv)
{
	if (priv->tx_pause_active_thresh < FC_ACTIVE_MIN ||
	    priv->tx_pause_active_thresh > FC_ACTIVE_MAX)
		priv->tx_pause_active_thresh = FC_ACTIVE_DEFAULT;

	if (priv->tx_pause_deactive_thresh < FC_DEACTIVE_MIN ||
	    priv->tx_pause_deactive_thresh > FC_DEACTIVE_MAX)
		priv->tx_pause_deactive_thresh = FC_DEACTIVE_DEFAULT;

	if (priv->tx_pause_active_thresh >= priv->tx_pause_deactive_thresh) {
		priv->tx_pause_active_thresh = FC_ACTIVE_DEFAULT;
		priv->tx_pause_deactive_thresh = FC_DEACTIVE_DEFAULT;
	}
}

static void hisi_femac_core_reset(struct hisi_femac_priv *priv)
{
	reset_control_assert(priv->mac_rst);
	reset_control_deassert(priv->mac_rst);
}

static void hisi_femac_sleep_us(u32 time_us)
{
	u32 time_ms;

	if (!time_us)
		return;

	time_ms = DIV_ROUND_UP(time_us, 1000);
	if (time_ms < 20)
		usleep_range(time_us, time_us + 500);
	else
		msleep(time_ms);
}

static void hisi_femac_phy_reset(struct hisi_femac_priv *priv)
{
	/* To make sure PHY hardware reset success,
	 * we must keep PHY in deassert state first and
	 * then complete the hardware reset operation
	 */
	reset_control_deassert(priv->phy_rst);
	hisi_femac_sleep_us(priv->phy_reset_delays[PRE_DELAY]);

	reset_control_assert(priv->phy_rst);
	/* delay some time to ensure reset ok,
	 * this depends on PHY hardware feature
	 */
	hisi_femac_sleep_us(priv->phy_reset_delays[PULSE]);
	reset_control_deassert(priv->phy_rst);
	/* delay some time to ensure later MDIO access */
	hisi_femac_sleep_us(priv->phy_reset_delays[POST_DELAY]);
}

static void hisi_femac_port_init(struct hisi_femac_priv *priv)
{
	u32 val;

	/* MAC gets link status info and phy mode by software config */
	val = MAC_PORTSEL_STAT_CPU;
	if (priv->ndev->phydev->interface == PHY_INTERFACE_MODE_RMII)
		val |= MAC_PORTSEL_RMII;
	writel(val, priv->port_base + MAC_PORTSEL);

	/*clear all interrupt status */
	writel(IRQ_ENA_PORT0_MASK, priv->glb_base + GLB_IRQ_RAW);
	hisi_femac_irq_disable(priv, IRQ_ENA_PORT0_MASK | IRQ_ENA_PORT0);

	if (HAS_TSO_CAP(priv->hw_cap)) {
		/* enable TSO debug for error handle */
		val = readl(priv->port_base + TSO_DBG_EN);
		val |= BITS_TSO_DBG_EN;
		writel(val, priv->port_base + TSO_DBG_EN);
	}

	val = readl(priv->glb_base + GLB_FWCTRL);
	val &= ~(FWCTRL_VLAN_ENABLE | FWCTRL_FWALL2CPU);
	val |= FWCTRL_FW2CPU_ENA;
	writel(val, priv->glb_base + GLB_FWCTRL);

	val = readl(priv->glb_base + GLB_MACTCTRL);
	val |= (MACTCTRL_BROAD2CPU | MACTCTRL_MACT_ENA);
	writel(val, priv->glb_base + GLB_MACTCTRL);

	val = readl(priv->port_base + MAC_SET);
	val &= ~MAX_FRAME_SIZE_MASK;
	val |= MAX_FRAME_SIZE;
	writel(val, priv->port_base + MAC_SET);

	val = RX_COALESCED_TIMER |
		(RX_COALESCED_FRAMES << RX_COALESCED_FRAME_OFFSET);
	writel(val, priv->port_base + RX_COALESCE_SET);

	val = (HW_RX_FIFO_DEPTH << RX_DEPTH_OFFSET) | HW_TX_FIFO_DEPTH;
	writel(val, priv->port_base + QLEN_SET);

	hisi_femac_set_flow_ctrl(priv);
}

static int hisi_femac_drv_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	struct resource *res;
	struct net_device *ndev;
	struct hisi_femac_priv *priv;
	struct phy_device *phy;
	const char *mac_addr;
	int ret;

	ndev = alloc_etherdev(sizeof(*priv));
	if (!ndev)
		return -ENOMEM;

	platform_set_drvdata(pdev, ndev);

	priv = netdev_priv(ndev);
	priv->dev = dev;
	priv->ndev = ndev;

	if (of_device_is_compatible(node, "hisilicon,hisi-femac-v2"))
		priv->hw_cap |= HW_CAP_TSO | HW_CAP_RXCSUM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->port_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->port_base)) {
		ret = PTR_ERR(priv->port_base);
		goto out_free_netdev;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	priv->glb_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->glb_base)) {
		ret = PTR_ERR(priv->glb_base);
		goto out_free_netdev;
	}

	priv->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(priv->clk)) {
		dev_err(dev, "failed to get clk\n");
		ret = -ENODEV;
		goto out_free_netdev;
	}

	ret = clk_prepare_enable(priv->clk);
	if (ret) {
		dev_err(dev, "failed to enable clk %d\n", ret);
		goto out_free_netdev;
	}

	priv->mac_rst = devm_reset_control_get(dev, "mac");
	if (IS_ERR(priv->mac_rst)) {
		ret = PTR_ERR(priv->mac_rst);
		goto out_disable_clk;
	}
	hisi_femac_core_reset(priv);

	priv->phy_rst = devm_reset_control_get(dev, "phy");
	if (IS_ERR(priv->phy_rst)) {
		priv->phy_rst = NULL;
	} else {
		ret = of_property_read_u32_array(node,
						 PHY_RESET_DELAYS_PROPERTY,
						 priv->phy_reset_delays,
						 DELAYS_NUM);
		if (ret)
			goto out_disable_clk;
		hisi_femac_phy_reset(priv);
	}

	phy = of_phy_get_and_connect(ndev, node, hisi_femac_adjust_link);
	if (!phy) {
		dev_err(dev, "connect to PHY failed!\n");
		ret = -ENODEV;
		goto out_disable_clk;
	}

	phy->advertising |= ADVERTISED_Pause;
	phy->supported |= ADVERTISED_Pause;

	dev_info(dev, "phy_id=0x%.8lx, phy_addr=%d, phy_mode=%s\n",
			(unsigned long)phy->phy_id,
			phy->addr,
			phy_modes(phy->interface));

	mac_addr = of_get_mac_address(node);
	if (mac_addr)
		ether_addr_copy(ndev->dev_addr, mac_addr);
	if (!is_valid_ether_addr(ndev->dev_addr)) {
		eth_hw_addr_random(ndev);
		dev_warn(dev, "using random MAC address %pM\n",
			 ndev->dev_addr);
	}

	ndev->watchdog_timeo = 6 * HZ;
	ndev->priv_flags |= IFF_UNICAST_FLT;
	ndev->netdev_ops = &hisi_femac_netdev_ops;
	ndev->ethtool_ops = &hisi_femac_ethtools_ops;
	netif_napi_add(ndev, &priv->napi, hisi_femac_poll, FEMAC_POLL_WEIGHT);
	SET_NETDEV_DEV(ndev, &pdev->dev);

	if (HAS_TSO_CAP(priv->hw_cap))
		ndev->hw_features |= NETIF_F_SG |
			NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
			NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_UFO;

	if (HAS_RXCSUM_CAP(priv->hw_cap))
		ndev->hw_features |= NETIF_F_RXCSUM;
	ndev->features |= ndev->hw_features;
	ndev->vlan_features |= ndev->features;

	device_set_wakeup_capable(priv->dev, true);
	device_set_wakeup_enable(priv->dev, true);

	priv->tx_pause_en = true;
	priv->tx_pause_active_thresh = TX_FLOW_CTRL_ACTIVE_THRESHOLD;
	priv->tx_pause_deactive_thresh = TX_FLOW_CTRL_DEACTIVE_THRESHOLD;

	hisi_femac_verify_flow_ctrl_args(priv);

	hisi_femac_port_init(priv);

	if (HAS_RXCSUM_CAP(priv->hw_cap))
		hisi_femac_enable_rxcsum_drop(priv, true);

	ret = hisi_femac_init_tx_and_rx_queues(priv);
	if (ret)
		goto out_disconnect_phy;

	if (HAS_TSO_CAP(priv->hw_cap)) {
		ret = hisi_femac_init_tx_descriptor_ring(priv);
		if (ret)
			goto out_disconnect_phy;
	}

	ndev->irq = platform_get_irq(pdev, 0);
	if (ndev->irq <= 0) {
		dev_err(dev, "No irq resource\n");
		ret = -ENODEV;
		goto out_destroy_descriptor;
	}

	ret = devm_request_irq(dev, ndev->irq, hisi_femac_interrupt,
			       IRQF_SHARED, pdev->name, ndev);
	if (ret) {
		dev_err(dev, "devm_request_irq %d failed!\n", ndev->irq);
		goto out_destroy_descriptor;
	}

	ret = register_netdev(ndev);
	if (ret) {
		dev_err(dev, "register_netdev failed!\n");
		goto out_destroy_descriptor;
	}

	return ret;

out_destroy_descriptor:
	if (HAS_TSO_CAP(priv->hw_cap))
		hisi_femac_destroy_tx_descriptor_ring(priv);
out_disconnect_phy:
	netif_napi_del(&priv->napi);
	phy_disconnect(phy);
out_disable_clk:
	clk_disable_unprepare(priv->clk);
out_free_netdev:
	free_netdev(ndev);

	return ret;
}

static int hisi_femac_drv_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct hisi_femac_priv *priv = netdev_priv(ndev);

	netif_napi_del(&priv->napi);
	unregister_netdev(ndev);
	if (HAS_TSO_CAP(priv->hw_cap))
		hisi_femac_destroy_tx_descriptor_ring(priv);

	phy_disconnect(ndev->phydev);
	clk_disable_unprepare(priv->clk);
	free_netdev(ndev);

	return 0;
}

#ifdef CONFIG_PM
int hisi_femac_drv_suspend(struct platform_device *pdev,
			   pm_message_t state)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct hisi_femac_priv *priv = netdev_priv(ndev);

	disable_irq(ndev->irq);
	if (netif_running(ndev)) {
		hisi_femac_net_close(ndev);
		netif_device_detach(ndev);
	}

	clk_disable_unprepare(priv->clk);

	return 0;
}

int hisi_femac_drv_resume(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct hisi_femac_priv *priv = netdev_priv(ndev);

	clk_prepare_enable(priv->clk);
	if (priv->phy_rst)
		hisi_femac_phy_reset(priv);

	if (netif_running(ndev)) {
		hisi_femac_port_init(priv);
		hisi_femac_net_open(ndev);
		netif_device_attach(ndev);
	}
	enable_irq(ndev->irq);

	return 0;
}
#endif

static const struct of_device_id hisi_femac_match[] = {
	{.compatible = "hisilicon,hisi-femac-v1",},
	{.compatible = "hisilicon,hisi-femac-v2",},
	{.compatible = "hisilicon,hi3516cv300-femac",},
	{},
};

MODULE_DEVICE_TABLE(of, hisi_femac_match);

static struct platform_driver hisi_femac_driver = {
	.driver = {
		.name = "hisi-femac",
		.of_match_table = hisi_femac_match,
	},
	.probe = hisi_femac_drv_probe,
	.remove = hisi_femac_drv_remove,
#ifdef CONFIG_PM
	.suspend = hisi_femac_drv_suspend,
	.resume = hisi_femac_drv_resume,
#endif
};

module_platform_driver(hisi_femac_driver);

MODULE_DESCRIPTION("Hisilicon Fast Ethernet MAC driver");
MODULE_AUTHOR("Dongpo Li <lidongpo@hisilicon.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:hisi-femac");
