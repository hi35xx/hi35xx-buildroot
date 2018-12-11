#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/circ_buf.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <net/ipv6.h>
#include <linux/tcp.h>
#include <net/protocol.h>
#include "hieth.h"
#include "mdio.h"
#include "proc.h"
#include "tso.h"

#ifdef HIETH_TSO_SUPPORTED
#define E_MAC_TX_FAIL   2
#define E_MAC_SW_GSO    3
#endif

#define FCS_BYTES       4

/*----------------------------Global variable-------------------------------*/
struct hieth_phy_param_s hieth_phy_param[HIETH_MAX_PORT];

#define FC_ACTIVE_MIN           1
#define FC_ACTIVE_DEFAULT       3
#define FC_ACTIVE_MAX           31
#define FC_DEACTIVE_MIN         1
#define FC_DEACTIVE_DEFAULT     5
#define FC_DEACTIVE_MAX         31

/*----------------------------Local variable-------------------------------*/
static struct net_device *hieth_devs_save[HIETH_MAX_PORT] = { NULL, NULL };
static struct hieth_netdev_priv hieth_priv;

/* real port count */
static int hieth_real_port_cnt;
/* default, eth enable */
static bool hieth_disable;
/* autoeee, enabled by dts */
static bool hieth_enable_autoeee;

static int __init hieth_noeth(char *str)
{
	hieth_disable = true;

	return 0;
}

early_param("noeth", hieth_noeth);

#include "pm.c"

static int hieth_hw_set_macaddress(struct hieth_netdev_priv *priv,
				   unsigned char *mac)
{
	u32 reg;

	if (priv->port == HIETH_PORT_1) {
		reg = hieth_readl(priv->glb_base, HIETH_GLB_DN_HOSTMAC_ENA);
		reg |= HIETH_GLB_DN_HOSTMAC_ENA_BIT;
		hieth_writel(priv->glb_base, reg, HIETH_GLB_DN_HOSTMAC_ENA);
	}

	reg = mac[1] | (mac[0] << 8);
	if (priv->port == HIETH_PORT_0)
		hieth_writel(priv->glb_base, reg, HIETH_GLB_HOSTMAC_H16);
	else
		hieth_writel(priv->glb_base, reg, HIETH_GLB_DN_HOSTMAC_H16);

	reg = mac[5] | (mac[4] << 8) | (mac[3] << 16) | (mac[2] << 24);
	if (priv->port == HIETH_PORT_0)
		hieth_writel(priv->glb_base, reg, HIETH_GLB_HOSTMAC_L32);
	else
		hieth_writel(priv->glb_base, reg, HIETH_GLB_DN_HOSTMAC_L32);

	return 0;
}

static void hieth_irq_enable(struct hieth_netdev_priv *priv, int irqs)
{
	u32 val;

	local_lock(priv);
	val = hieth_readl(priv->glb_base, HIETH_GLB_IRQ_ENA);
	hieth_writel(priv->glb_base, val | irqs, HIETH_GLB_IRQ_ENA);
	local_unlock(priv);
}

static void hieth_irq_disable(struct hieth_netdev_priv *priv, int irqs)
{
	u32 val;

	local_lock(priv);
	val = hieth_readl(priv->glb_base, HIETH_GLB_IRQ_ENA);
	hieth_writel(priv->glb_base, val & (~irqs), HIETH_GLB_IRQ_ENA);
	local_unlock(priv);
}

static void hieth_clear_irqstatus(struct hieth_netdev_priv *priv, int irqs)
{
	local_lock(priv);
	hieth_writel(priv->glb_base, irqs, HIETH_GLB_IRQ_RAW);
	local_unlock(priv);
}

static void hieth_set_flow_ctrl(struct hieth_netdev_priv *priv)
{
	unsigned int pause_en;
	unsigned int tx_flow_ctrl;

	tx_flow_ctrl = hieth_readl(priv->port_base, HIETH_P_GLB_FC_LEVEL);
	tx_flow_ctrl &= ~BITS_THR_MASK;
	tx_flow_ctrl |= priv->tx_pause_deactive_thresh;
	tx_flow_ctrl &= ~(BITS_THR_MASK << BITS_FC_ACTIVE_THR_OFFSET);
	tx_flow_ctrl |= priv->tx_pause_active_thresh <<
				BITS_FC_ACTIVE_THR_OFFSET;

	pause_en = hieth_readl(priv->port_base, HIETH_P_MAC_SET);

	if (priv->tx_pause_en) {
		tx_flow_ctrl |= BIT_FC_EN;
		pause_en |= BIT_PAUSE_EN;
	} else {
		tx_flow_ctrl &= ~BIT_FC_EN;
		pause_en &= ~BIT_PAUSE_EN;
	}

	hieth_writel(priv->port_base, tx_flow_ctrl, HIETH_P_GLB_FC_LEVEL);

	hieth_writel(priv->port_base, pause_en, HIETH_P_MAC_SET);
}

static int hieth_port_reset(struct hieth_netdev_priv *priv)
{
	u32 rst_bit = 0;
	u32 val;

	if (hieth_real_port_cnt == 1) {
		rst_bit = HIETH_GLB_SOFT_RESET_ALL;
	} else {
		if (priv->port == HIETH_PORT_0)
			rst_bit |= HIETH_GLB_SOFT_RESET_P0;
		else if (priv->port == HIETH_PORT_1)
			rst_bit |= HIETH_GLB_SOFT_RESET_P1;
		else
			BUG();
	}

	val = hieth_readl(priv->glb_base, HIETH_GLB_SOFT_RESET);

	val |= rst_bit;
	hieth_writel(priv->glb_base, val, HIETH_GLB_SOFT_RESET);
	usleep_range(1000, 10000);
	val &= ~rst_bit;
	hieth_writel(priv->glb_base, val, HIETH_GLB_SOFT_RESET);
	usleep_range(1000, 10000);
	val |= rst_bit;
	hieth_writel(priv->glb_base, val, HIETH_GLB_SOFT_RESET);
	usleep_range(1000, 10000);
	val &= ~rst_bit;
	hieth_writel(priv->glb_base, val, HIETH_GLB_SOFT_RESET);

	return 0;
}

static void hieth_port_init(struct hieth_netdev_priv *priv)
{
	u32 val;
	int phy_intf = (priv->phy_mode == PHY_INTERFACE_MODE_MII ?
			HIETH_P_MAC_PORTSEL_MII : HIETH_P_MAC_PORTSEL_RMII);

	/* set little endian */
	val = hieth_readl(priv->glb_base, HIETH_GLB_ENDIAN_MOD);
	val |= HIETH_GLB_ENDIAN_MOD_IN;
	val |= HIETH_GLB_ENDIAN_MOD_OUT;
	hieth_writel(priv->glb_base, val, HIETH_GLB_ENDIAN_MOD);

	/* set stat ctrl to cpuset, and MII or RMII mode */
	hieth_writel(priv->port_base, phy_intf | HIETH_P_MAC_PORTSEL_STAT_CPU,
		     HIETH_P_MAC_PORTSEL);

	/*clear all interrupt status */
	hieth_clear_irqstatus(priv, UD_BIT_NAME(HIETH_GLB_IRQ_ENA_BIT));

	/*disable interrupts */
	hieth_irq_disable(priv, UD_BIT_NAME(HIETH_GLB_IRQ_ENA_BIT) |
			  UD_BIT_NAME(HIETH_GLB_IRQ_ENA_IEN));

#ifdef HIETH_TSO_SUPPORTED
	/* enable TSO debug for error handle */
	val = hieth_readl(priv->port_base, HIETH_P_GLB_TSO_DBG_EN);
	val |= BITS_TSO_DBG_EN;
	hieth_writel(priv->port_base, val, HIETH_P_GLB_TSO_DBG_EN);
#endif

	/* disable vlan, enable UpEther<->CPU */
	val = hieth_readl(priv->glb_base, HIETH_GLB_FWCTRL);
	val &= ~HIETH_GLB_FWCTRL_VLAN_ENABLE;
	val |= UD_BIT_NAME(HIETH_GLB_FWCTRL_FW2CPU_ENA);
	val &= ~(UD_BIT_NAME(HIETH_GLB_FWCTRL_FWALL2CPU));
	hieth_writel(priv->glb_base, val, HIETH_GLB_FWCTRL);
	val = hieth_readl(priv->glb_base, HIETH_GLB_MACTCTRL);
	val |= UD_BIT_NAME(HIETH_GLB_MACTCTRL_BROAD2CPU);
	val |= UD_BIT_NAME(HIETH_GLB_MACTCTRL_MACT_ENA);
	hieth_writel(priv->glb_base, val, HIETH_GLB_MACTCTRL);

	/* set pre count limit */
	val = hieth_readl(priv->port_base, HIETH_P_MAC_TX_IPGCTRL);
	val &= ~HIETH_P_MAC_TX_IPGCTRL_PRE_CNT_LMT_MSK;
	val |= 0;
	hieth_writel(priv->port_base, val, HIETH_P_MAC_TX_IPGCTRL);

	/* set max receive length */
	val = hieth_readl(priv->port_base, HIETH_P_MAC_SET);
	val &= ~HIETH_P_MAC_SET_LEN_MAX_MSK;
	val |= HIETH_P_MAC_SET_LEN_MAX(HIETH_MAX_RCV_LEN);
	hieth_writel(priv->port_base, val, HIETH_P_MAC_SET);

	hieth_set_flow_ctrl(priv);
}

static void hieth_set_hwq_depth(struct hieth_netdev_priv *priv)
{
	u32 val;

	val = hieth_readl(priv->port_base, HIETH_P_GLB_QLEN_SET);
	val &= ~HIETH_P_GLB_QLEN_SET_TXQ_DEP_MSK;
	val |= HIETH_P_GLB_QLEN_SET_TXQ_DEP(priv->depth.hw_xmitq);
	val &= ~HIETH_P_GLB_QLEN_SET_RXQ_DEP_MSK;
	val |= HIETH_P_GLB_QLEN_SET_RXQ_DEP(HIETH_MAX_QUEUE_DEPTH -
					    priv->depth.hw_xmitq);
	hieth_writel(priv->port_base, val, HIETH_P_GLB_QLEN_SET);
}

static int hieth_hw_xmitq_ready(struct hieth_netdev_priv *priv)
{
	int ret;

	local_lock(priv);
	ret = hieth_readl(priv->port_base, HIETH_P_GLB_RO_QUEUE_STAT);
	ret &= HIETH_P_GLB_RO_QUEUE_STAT_XMITQ_RDY_MSK;
	local_unlock(priv);

	return ret;
}

#ifdef HIETH_TSO_SUPPORTED
#ifdef HIETH_TSO_DEBUG
unsigned int id_send;
unsigned int id_free;
struct send_pkt_info    pkt_rec[MAX_RECORD];
#endif
#endif

#ifdef HIETH_TSO_SUPPORTED
static struct sk_buff *hieth_xmit_release_gso(struct hieth_netdev_priv *priv)
{
	struct sk_buff *skb;
	struct tx_pkt_info *txq_cur;
	int pkt_type;

	txq_cur = priv->txq + priv->txq_tail;

	skb = txq_cur->skb;

	if (txq_cur->tx.info.sg_flag)
		pkt_type = PKT_SG;
	else
		pkt_type = PKT_NORMAL;

	if (pkt_type == PKT_NORMAL) {
		dma_addr_t dma_addr;

		dma_addr = txq_cur->tx_addr;
		dma_unmap_single(priv->dev, dma_addr, skb->len, DMA_TO_DEVICE);
	} else { /* TSO pkt */
		struct dma_tx_desc *desc_cur;
		unsigned int desc_offset;
		int nfrags = skb_shinfo(skb)->nr_frags;
		int i;

		desc_offset = txq_cur->sg_desc_offset;
		BUG_ON(desc_offset != priv->sg_tail);
		desc_cur = priv->dma_tx + desc_offset;

		dma_unmap_single(priv->dev, desc_cur->linear_addr,
				 desc_cur->linear_len, DMA_TO_DEVICE);
		for (i = 0; i < nfrags; i++) {
			dma_unmap_page(priv->dev, desc_cur->frags[i].addr,
				       desc_cur->frags[i].size, DMA_TO_DEVICE);
		}

		priv->sg_tail = (priv->sg_tail + 1) % priv->q_size;
	}

	txq_cur->skb = NULL;
	priv->txq_tail = (priv->txq_tail + 1) % priv->q_size;

#ifdef HIETH_TSO_DEBUG
	pkt_rec[id_free].status = 0;
	id_free++;
	if (id_free == MAX_RECORD)
		id_free = 0;
#endif

	return skb;
}
#endif

static int hieth_xmit_release_skb(struct hieth_netdev_priv *priv)
{
	u32 val;
	int ret = 0;
	struct sk_buff *skb;

	local_lock(priv);

	val = hieth_readl(priv->port_base, HIETH_P_GLB_RO_QUEUE_STAT) &
			  HIETH_P_GLB_RO_QUEUE_STAT_XMITQ_CNT_INUSE_MSK;
	while (val < priv->tx_hw_cnt) {
#ifdef HIETH_TSO_SUPPORTED
		skb = hieth_xmit_release_gso(priv);
#else
		skb = skb_dequeue(&priv->tx_hw);
#endif

		if (!skb) {
			pr_err("hw_xmitq_cnt_inuse=%d, tx_hw_cnt=%d\n",
			       val, priv->tx_hw_cnt);

			BUG();
			ret = -1;
			goto error_exit;
		}
		dev_kfree_skb_any(skb);

		priv->tx_hw_cnt--;

		val = hieth_readl(priv->port_base, HIETH_P_GLB_RO_QUEUE_STAT) &
				HIETH_P_GLB_RO_QUEUE_STAT_XMITQ_CNT_INUSE_MSK;
	}

error_exit:
	local_unlock(priv);
	return ret;
}

#ifdef HIETH_TSO_SUPPORTED
void hieth_get_tso_err_info(struct hieth_netdev_priv *priv)
{
	unsigned int reg_addr, reg_tx_info, reg_tx_err;
	unsigned int sg_index;
	struct dma_tx_desc *sg_desc;
	int *sg_word;
	int i;

	reg_addr = hieth_readl(priv->port_base, HIETH_P_GLB_TSO_DBG_ADDR);
	reg_tx_info = hieth_readl(priv->port_base, HIETH_P_GLB_TSO_DBG_TX_INFO);
	reg_tx_err = hieth_readl(priv->port_base, HIETH_P_GLB_TSO_DBG_TX_ERR);

	WARN(1, "tx err=0x%x, tx_info=0x%x, addr=0x%x\n",
	     reg_tx_err, reg_tx_info, reg_addr);

	sg_index = (reg_addr - priv->dma_tx_phy) / sizeof(struct dma_tx_desc);
	sg_desc = priv->dma_tx + sg_index;
	sg_word = (int *)sg_desc;
	for (i = 0; i < sizeof(struct dma_tx_desc) / sizeof(int); i++)
		pr_err("%s,%d: sg_desc word[%d]=0x%x\n",
		       __func__, __LINE__, i, sg_word[i]);

	/* restart MAC to transmit next packet */
	hieth_irq_disable(priv, UD_BIT_NAME(HIETH_INT_TX_ERR));
#if 0
	hieth_readl(priv, HIETH_P_GLB_TSO_DBG_STATE));
	hieth_irq_enable(priv, UD_BIT_NAME(HIETH_INT_TX_ERR));
#endif
}

static int hieth_xmit_real_send(struct hieth_netdev_priv *priv,
				struct sk_buff *skb);

static int hieth_sw_gso(struct hieth_netdev_priv *priv, struct sk_buff *skb)
{
	struct sk_buff *segs, *curr_skb;
	struct net_device *ndev = hieth_devs_save[priv->port];
	int ret;

	segs = skb_gso_segment(skb, ndev->features &
			~(NETIF_F_ALL_CSUM | NETIF_F_GSO_SOFTWARE));
	if (IS_ERR(segs))
		goto sw_tso_end;

	do {
		curr_skb = segs;
		segs = segs->next;
		curr_skb->next = NULL;
		ret = hieth_xmit_real_send(priv, curr_skb);
		if (ret < 0) {
			dev_kfree_skb(curr_skb);
			while (segs != NULL) {
				curr_skb = segs;
				segs = segs->next;
				curr_skb->next = NULL;
				dev_kfree_skb(curr_skb);
			}
			goto sw_tso_end;
		}
	} while (segs);

sw_tso_end:
	dev_kfree_skb(skb);

	return 0;
}

int hieth_get_pkt_info(struct sk_buff *skb, struct tx_pkt_info *txq_cur)
{
	int nfrags = skb_shinfo(skb)->nr_frags;

	__be16 l3_proto; /* level 3 protocol */
	unsigned int l4_proto = IPPROTO_MAX;
	unsigned char coe_enable = 0;

	if (skb->ip_summed == CHECKSUM_PARTIAL)
		coe_enable = 1;

	txq_cur->tx.val = 0;

	if (skb_is_gso(skb)) {
		txq_cur->tx.info.tso_flag = 1;
		txq_cur->tx.info.sg_flag = 1;
	} else if (nfrags) {
		txq_cur->tx.info.sg_flag = 1;
	}

	/* deal with VLAN flag */
	l3_proto = skb->protocol;
	if (skb->protocol == htons(ETH_P_8021Q)) {
		struct vlan_hdr *vhdr;

		vhdr = (struct vlan_hdr *)(skb->data + ETH_HLEN);
		l3_proto = vhdr->h_vlan_encapsulated_proto;
		txq_cur->tx.info.vlan_flag = 1;
	}

	if (l3_proto == htons(ETH_P_IP)) {
		const struct iphdr *iph;

		iph = ip_hdr(skb);
		txq_cur->tx.info.ip_ver = PKT_IPV4;
		txq_cur->tx.info.ip_hdr_len = iph->ihl;

		l4_proto = iph->protocol;
		l4_proto = l4_proto & (MAX_INET_PROTOS - 1);
	} else if (l3_proto == htons(ETH_P_IPV6)) {
		const struct ipv6hdr *hdr;

		txq_cur->tx.info.ip_ver = PKT_IPV6;
		txq_cur->tx.info.ip_hdr_len = PKT_IPV6_HDR_LEN;
		hdr = ipv6_hdr(skb);
		l4_proto = hdr->nexthdr;
	} else {
		coe_enable = 0;
		/* pr_err("unknown level 3 prot=0x%x\n", ntohs(l3_proto)); */
	}

	if (l4_proto == IPPROTO_TCP) {
		const struct tcphdr *th;

		th = tcp_hdr(skb);
		txq_cur->tx.info.prot_type = PKT_TCP;
		txq_cur->tx.info.prot_hdr_len = th->doff;
	} else if (l4_proto == IPPROTO_UDP) {
		txq_cur->tx.info.prot_type = PKT_UDP;
		txq_cur->tx.info.prot_hdr_len = PKT_UDP_HDR_LEN;
	} else {
		coe_enable = 0;
		/* pr_err("unknown level 4 prot=0x%x\n", l4_proto); */
		/* TODO: when l3_proto == ETH_P_IPV6,
		 * we need to deal with IPV6 next header.
		 */
		if (l3_proto == htons(ETH_P_IPV6) &&
		    skb->ip_summed == CHECKSUM_PARTIAL)
			return -E_MAC_SW_GSO;
	}

	if (skb_is_gso(skb))
		txq_cur->tx.info.data_len
			= skb_shinfo(skb)->gso_size;
	else
		txq_cur->tx.info.data_len = skb->len + FCS_BYTES;

	if (coe_enable) {
		if (skb_is_gso(skb) && (l4_proto == IPPROTO_UDP)) {
			/* TODO: self checksum caculate */
			int offset;
			__wsum csum;
			__sum16 udp_csum;

			offset = skb_checksum_start_offset(skb);
			WARN_ON(offset >= skb_headlen(skb));
			csum = skb_checksum(skb, offset, skb->len - offset, 0);

			offset += skb->csum_offset;
			BUG_ON(offset + sizeof(__sum16) > skb_headlen(skb));
			udp_csum = csum_fold(csum);
			if (udp_csum == 0)
				udp_csum = CSUM_MANGLED_0;

			*(__sum16 *)(skb->data + offset) = udp_csum;

			skb->ip_summed = CHECKSUM_NONE;
			coe_enable = 0;
		}
	}

	txq_cur->tx.info.coe_flag = coe_enable;
	txq_cur->tx.info.nfrags_num = nfrags;

	return 0;
}

int hieth_xmit_gso(struct hieth_netdev_priv *priv, struct sk_buff *skb)
{
	int pkt_type = PKT_NORMAL;
	int nfrags = skb_shinfo(skb)->nr_frags;
	struct tx_pkt_info *txq_cur;
	int ret;

	if (((priv->txq_head + 1) % priv->q_size) == priv->txq_tail) {
		/* txq full, stop */
		pr_err("WARNING: txq full.");
		return -E_MAC_TX_FAIL;
	}

	if (skb_is_gso(skb) || nfrags) {
		/* TSO pkt or SG pkt */
		pkt_type = PKT_SG;
	} else { /* Normal pkt */
		pkt_type = PKT_NORMAL;
	}

	txq_cur = priv->txq + priv->txq_head;

	ret = hieth_get_pkt_info(skb, txq_cur);
	if (unlikely(ret < 0))
		return ret;

	if (pkt_type == PKT_NORMAL) {
		txq_cur->tx_addr = dma_map_single(priv->dev, skb->data,
				skb->len, DMA_TO_DEVICE);
	} else { /* TSO pkt */
		struct dma_tx_desc *desc_cur;
		int i;

		if (unlikely(((priv->sg_head + 1) % priv->q_size) ==
		    priv->sg_tail)) {
			/* SG pkt, but sg desc all used */
			pr_err("WARNING: sg desc all used.");
			return -E_MAC_TX_FAIL;
		}

		desc_cur = priv->dma_tx + priv->sg_head;

		/* TODO: deal with ipv6_id */
		if (txq_cur->tx.info.tso_flag &&
		    txq_cur->tx.info.ip_ver == PKT_IPV6 &&
		    txq_cur->tx.info.prot_type == PKT_UDP) {
			desc_cur->ipv6_id = ntohl(skb_shinfo(skb)->ip6_frag_id);
		}

		desc_cur->total_len = skb->len;
		desc_cur->linear_len = skb_headlen(skb);
		desc_cur->linear_addr = dma_map_single(priv->dev, skb->data,
				desc_cur->linear_len, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(priv->dev,
					       desc_cur->linear_addr))) {
			pr_err("DMA Mapping fail.");
			return -E_MAC_TX_FAIL;
		}

		for (i = 0; i < nfrags; i++) {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
			int len = frag->size;

			WARN(len <= 0, "%s,%d: frag %d size %d. desc=%x\n",
			     __func__, __LINE__, i, len, txq_cur->tx.val);

			desc_cur->frags[i].addr =
				skb_frag_dma_map(priv->dev, frag, 0,
						 len, DMA_TO_DEVICE);
			ret = dma_mapping_error(priv->dev,
						desc_cur->frags[i].addr);
			if (unlikely(ret)) {
				pr_err("skb frag DMA Mapping fail.");
				return -E_MAC_TX_FAIL;
			}
			desc_cur->frags[i].size = len;
		}

		txq_cur->tx_addr = priv->dma_tx_phy +
			priv->sg_head * sizeof(struct dma_tx_desc);
		txq_cur->sg_desc_offset = priv->sg_head;

		priv->sg_head = (priv->sg_head + 1) % priv->q_size;

		/* Ensure desc info writen to memory before config hardware */
		wmb();
	}

	txq_cur->skb = skb;
	priv->txq_head = (priv->txq_head + 1) % priv->q_size;

#ifdef HIETH_TSO_DEBUG
	pkt_rec[id_send].reg_addr = txq_cur->tx_addr;
	pkt_rec[id_send].reg_pkt_info = txq_cur->tx.val;
	pkt_rec[id_send].status = 1;
	id_send++;
	if (id_send == MAX_RECORD)
		id_send = 0;
#endif
	hieth_writel(priv->port_base, txq_cur->tx_addr, HIETH_P_GLB_EQ_ADDR);
	hieth_writel(priv->port_base, txq_cur->tx.val, HIETH_P_GLB_EQFRM_LEN);

	return 0;
}
#endif

static int hieth_xmit_real_send(struct hieth_netdev_priv *priv,
				struct sk_buff *skb)
{
	int ret = 0;
	u32 val;

	local_lock(priv);

	val = hieth_readl(priv->port_base, HIETH_P_GLB_RO_QUEUE_STAT);
	val &= HIETH_P_GLB_RO_QUEUE_STAT_XMITQ_RDY_MSK;
	if (!val) {
		pr_err("hw xmit queue is not ready\n");
		ret = -1;
		goto _trans_exit;
	}

#ifdef HIETH_TSO_SUPPORTED
	/* TSO supported */
	ret = hieth_xmit_gso(priv, skb);
	if (unlikely(ret < 0)) {
		if (ret == -E_MAC_SW_GSO) {
			local_unlock(priv);
			return hieth_sw_gso(priv, skb);
		}
		ret = -1;
		goto _trans_exit;
	}
#else
	dma_map_single(priv->dev, skb->data, skb->len, DMA_TO_DEVICE);
	/* for recalc CRC, 4 bytes more is needed */
	hieth_writel(priv->port_base, virt_to_phys(skb->data),
		     HIETH_P_GLB_EQ_ADDR);
	hieth_writel(priv->port_base, skb->len + FCS_BYTES,
		     HIETH_P_GLB_EQFRM_LEN);

	skb_queue_tail(&priv->tx_hw, skb);
#endif

	priv->tx_hw_cnt++;

_trans_exit:
	local_unlock(priv);

	return ret;
}

static struct sk_buff *hieth_platdev_alloc_skb(struct hieth_netdev_priv *priv)
{
	struct sk_buff *skb;

	skb = priv->rx_pool.sk_pool[priv->rx_pool.next_free_skb++];

	if (priv->rx_pool.next_free_skb == CONFIG_HIETH_MAX_RX_POOLS)
		priv->rx_pool.next_free_skb = 0;

	/*current skb is used by kernel or other process,find another skb*/
	if (skb_shared(skb) || (atomic_read(&(skb_shinfo(skb)->dataref)) > 1)) {
		int i;

		for (i = 0; i < CONFIG_HIETH_MAX_RX_POOLS; i++) {
			skb = priv->rx_pool.sk_pool[priv->
						    rx_pool.next_free_skb++];
			if (priv->rx_pool.next_free_skb ==
			    CONFIG_HIETH_MAX_RX_POOLS)
				priv->rx_pool.next_free_skb = 0;

			if ((skb_shared(skb) == 0) &&
			    (atomic_read(&(skb_shinfo(skb)->dataref)) <= 1))
				break;
		}

		if (i == CONFIG_HIETH_MAX_RX_POOLS) {
			priv->stat.rx_pool_dry_times++;
			pr_debug("%ld: no free skb\n",
				 priv->stat.rx_pool_dry_times);
			skb = dev_alloc_skb(SKB_SIZE);
			return skb;
		}
	}
	memset(skb, 0, offsetof(struct sk_buff, tail));

	skb->data = skb->head;
	skb->tail = skb->head;

	skb_reserve(skb, NET_SKB_PAD);
	skb->len = 0;
	skb->data_len = 0;
	skb->cloned = 0;
	atomic_inc(&skb->users);
	return skb;
}

static int hieth_feed_hw(struct hieth_netdev_priv *priv)
{
	struct sk_buff *skb;
	int cnt = 0;
	int rx_head_len;

	/* if skb occupied too much, then do not alloc any more. */
	rx_head_len = skb_queue_len(&priv->rx_head);
	if (rx_head_len > HIETH_MAX_RX_HEAD_LEN)
		return 0;

	local_lock(priv);

	while (hieth_readl(priv->port_base, HIETH_P_GLB_RO_QUEUE_STAT) &
		HIETH_P_GLB_RO_QUEUE_STAT_RECVQ_RDY_MSK) {
		skb = hieth_platdev_alloc_skb(priv);
		if (!skb)
			break;

		dma_map_single(priv->dev, skb->data, HIETH_MAX_FRAME_SIZE,
			       DMA_FROM_DEVICE);
		hieth_writel(priv->port_base, virt_to_phys(skb->data + 2),
			     HIETH_P_GLB_IQ_ADDR);
		skb_queue_tail(&priv->rx_hw, skb);
		cnt++;
	}

	local_unlock(priv);
	return cnt;
}

static int hieth_hw_recv_tryup(struct hieth_netdev_priv *priv)
{
	struct sk_buff *skb;
	u32 rlen;
	int cnt = 0;
	uint32_t rx_pkt_info;
#ifdef HIETH_RXCSUM_SUPPORTED
	struct net_device *ndev = hieth_devs_save[priv->port];
	int hdr_csum_done, hdr_csum_err;
	int payload_csum_done, payload_csum_err;
#endif

	local_lock(priv);

	while ((hieth_readl(priv->glb_base, HIETH_GLB_IRQ_RAW) &
		(UD_BIT_NAME(HIETH_GLB_IRQ_INT_RX_RDY)))) {
		rx_pkt_info = hieth_readl(priv->port_base,
					  HIETH_P_GLB_RO_IQFRM_DES);
		rlen = rx_pkt_info & HIETH_P_GLB_RO_IQFRM_DES_FDIN_LEN_MSK;
		rlen -= 4; /* remove FCS 4Bytes */

		/* hw set rx pkg finish */
		hieth_writel(priv->glb_base,
			     UD_BIT_NAME(HIETH_GLB_IRQ_INT_RX_RDY),
			     HIETH_GLB_IRQ_RAW);

		skb = skb_dequeue(&priv->rx_hw);
		if (!skb) {
			pr_err("chip told receive pkg, but no packet find!\n");
			BUG();
			break;
		}

		dma_map_single(priv->dev, skb->data, HIETH_MAX_FRAME_SIZE,
			       DMA_FROM_DEVICE);
		skb_reserve(skb, 2);
		skb_put(skb, rlen);
#ifdef HIETH_RXCSUM_SUPPORTED
		skb->ip_summed = CHECKSUM_NONE;
		if (ndev->features & NETIF_F_RXCSUM) {
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
				if (unlikely(hdr_csum_err ||
					     payload_csum_err)) {
					priv->stats.rx_errors++;
					priv->stats.rx_crc_errors++;
					dev_kfree_skb_any(skb);
					continue;
				} else {
					skb->ip_summed = CHECKSUM_UNNECESSARY;
				}
			}
		}
#endif
		skb_queue_tail(&priv->rx_head, skb);
		cnt++;
	}

	local_unlock(priv);

	/* fill hardware receive queue again */
	hieth_feed_hw(priv);
	return cnt;
}

static void hieth_adjust_link(struct net_device *dev)
{
	int stat = 0;
	struct hieth_netdev_priv *priv = netdev_priv(dev);

	stat |= (priv->phy->link) ? HIETH_P_MAC_PORTSET_LINKED : 0;
	stat |= (priv->phy->duplex == DUPLEX_FULL) ?
		HIETH_P_MAC_PORTSET_DUP_FULL : 0;
	stat |= (priv->phy->speed == SPEED_100) ?
		HIETH_P_MAC_PORTSET_SPD_100M : 0;

	if ((stat != priv->link_stat) &&
	    ((stat | priv->link_stat) & HIETH_P_MAC_PORTSET_LINKED)) {
		hieth_writel(priv->port_base, stat, HIETH_P_MAC_PORTSET);
		phy_print_status(priv->phy);
		priv->link_stat = stat;

		priv->tx_pause_en = priv->phy->pause;
		hieth_set_flow_ctrl(priv);

		if (hieth_enable_autoeee)
			hieth_autoeee_init(priv, stat);
	}
}

static int hieth_init_skb_buffers(struct hieth_netdev_priv *priv)
{
	int i;
	struct sk_buff *skb;

	for (i = 0; i < CONFIG_HIETH_MAX_RX_POOLS; i++) {
		skb = dev_alloc_skb(SKB_SIZE);
		if (!skb)
			break;
		priv->rx_pool.sk_pool[i] = skb;
	}

	if (i < CONFIG_HIETH_MAX_RX_POOLS) {
		pr_err("no mem\n");
		for (i--; i > 0; i--)
			dev_kfree_skb_any(priv->rx_pool.sk_pool[i]);
		return -ENOMEM;
	}

	priv->rx_pool.next_free_skb = 0;
	priv->stat.rx_pool_dry_times = 0;
	return 0;
}

static void hieth_destroy_skb_buffers(struct hieth_netdev_priv *priv)
{
	int i;

	for (i = 0; i < CONFIG_HIETH_MAX_RX_POOLS; i++)
		dev_kfree_skb_any(priv->rx_pool.sk_pool[i]);

	priv->rx_pool.next_free_skb = 0;
	priv->stat.rx_pool_dry_times = 0;
}

static void hieth_bfproc_recv(unsigned long data)
{
	int ret = 0;
	struct net_device *dev = (void *)data;
	struct hieth_netdev_priv *priv = netdev_priv(dev);
	struct sk_buff *skb;

	hieth_hw_recv_tryup(priv);

	while ((skb = skb_dequeue(&priv->rx_head)) != NULL) {
		skb->protocol = eth_type_trans(skb, dev);
		if (HIETH_INVALID_RXPKG_LEN(skb->len)) {
			pr_err("pkg len error\n");
			priv->stats.rx_errors++;
			priv->stats.rx_length_errors++;
			dev_kfree_skb_any(skb);
			continue;
		}

		priv->stats.rx_packets++;
		priv->stats.rx_bytes += skb->len;
		dev->last_rx = jiffies;
		skb->dev = dev;

		ret = netif_rx(skb);
		if (ret)
			priv->stats.rx_dropped++;
	}
}

static void hieth_net_isr_proc(struct net_device *ndev, int ints)
{
	struct hieth_netdev_priv *priv = netdev_priv(ndev);

	if ((ints & UD_BIT_NAME(HIETH_GLB_IRQ_INT_MULTI_RXRDY)) &&
	    (hieth_hw_recv_tryup(priv) > 0)) {
		tasklet_schedule(&priv->bf_recv);
	}

	if (ints & UD_BIT_NAME(HIETH_GLB_IRQ_INT_TXQUE_RDY)) {
		hieth_irq_disable(priv,
				  UD_BIT_NAME(HIETH_GLB_IRQ_INT_TXQUE_RDY));
		netif_wake_queue(ndev);
	}
}

static irqreturn_t hieth_net_isr(int irq, void *dev_id)
{
	int ints;
	struct net_device *dev = (struct net_device *)dev_id;
	struct hieth_netdev_priv *priv = netdev_priv(dev);

	/*mask the all interrupt */
	hieth_irq_disable(priv, HIETH_GLB_IRQ_ENA_IEN_A);

	ints = hieth_readl(priv->glb_base, HIETH_GLB_IRQ_STAT);

	if ((HIETH_PORT_0 == priv->port) &&
	    likely(ints & HIETH_GLB_IRQ_ENA_BIT_U)) {
		hieth_net_isr_proc(dev, (ints & HIETH_GLB_IRQ_ENA_BIT_U));
		hieth_clear_irqstatus(priv, (ints & HIETH_GLB_IRQ_ENA_BIT_U));
		ints &= ~HIETH_GLB_IRQ_ENA_BIT_U;
	}

	if ((HIETH_PORT_1 == priv->port) &&
	    likely(ints & HIETH_GLB_IRQ_ENA_BIT_D)) {
		hieth_net_isr_proc(dev, (ints & HIETH_GLB_IRQ_ENA_BIT_D));
		hieth_clear_irqstatus(priv, (ints & HIETH_GLB_IRQ_ENA_BIT_D));
		ints &= ~HIETH_GLB_IRQ_ENA_BIT_D;
	}
#ifdef HIETH_TSO_SUPPORTED
	if (unlikely(ints & HIETH_INT_TX_ERR_U)) {
		priv = netdev_priv(hieth_devs_save[HIETH_PORT_0]);

		hieth_get_tso_err_info(priv);
		ints &= ~HIETH_INT_TX_ERR_U;
	}

	if (unlikely(ints & HIETH_INT_TX_ERR_D)) {
		priv = netdev_priv(hieth_devs_save[HIETH_PORT_1]);

		hieth_get_tso_err_info(priv);
		ints &= ~HIETH_INT_TX_ERR_D;
	}
#endif

	/*unmask the all interrupt */
	hieth_irq_enable(priv, HIETH_GLB_IRQ_ENA_IEN_A);

	return IRQ_HANDLED;
}

static void hieth_monitor_func(unsigned long arg)
{
	struct net_device *dev = (struct net_device *)arg;
	struct hieth_netdev_priv *priv = netdev_priv(dev);

	if (!priv || !netif_running(dev)) {
		pr_debug("network driver is stopped.\n");
		return;
	}

	hieth_feed_hw(priv);
	hieth_xmit_release_skb(priv);

	priv->monitor.expires =
	    jiffies + msecs_to_jiffies(HIETH_MONITOR_TIMER);
	add_timer(&priv->monitor);
}

static int hieth_net_open(struct net_device *dev)
{
	int ret = 0;
	struct cpumask cpumask;
	struct hieth_netdev_priv *priv = netdev_priv(dev);

	ret = request_irq(dev->irq, hieth_net_isr, IRQF_SHARED,
			  dev->name, dev);
	if (ret) {
		pr_err("request_irq %d failed!\n", dev->irq);
		return ret;
	}

	/* set irq affinity */
	if ((num_online_cpus() > 1) && cpu_online(HIETH_IRQ_AFFINITY_CPU)) {
		cpumask_clear(&cpumask);
		cpumask_set_cpu(HIETH_IRQ_AFFINITY_CPU, &cpumask);
		irq_set_affinity(dev->irq, &cpumask);
	}

	if (!is_valid_ether_addr(dev->dev_addr))
		random_ether_addr(dev->dev_addr);

	hieth_hw_set_macaddress(priv, dev->dev_addr);

	/* init tasklet */
	priv->bf_recv.next = NULL;
	priv->bf_recv.state = 0;
	priv->bf_recv.func = hieth_bfproc_recv;
	priv->bf_recv.data = (unsigned long)dev;
	atomic_set(&priv->bf_recv.count, 0);

	/* setup hardware */
	hieth_set_hwq_depth(priv);
	hieth_clear_irqstatus(priv, UD_BIT_NAME(HIETH_GLB_IRQ_ENA_BIT));

	netif_carrier_off(dev);
	hieth_feed_hw(priv);
	netif_start_queue(dev);

	priv->link_stat = 0;
	if (priv->phy)
		phy_start(priv->phy);

	hieth_irq_enable(priv, UD_BIT_NAME(HIETH_GLB_IRQ_INT_MULTI_RXRDY) |
			UD_BIT_NAME(HIETH_GLB_IRQ_ENA_IEN) |
			HIETH_GLB_IRQ_ENA_IEN_A);
#ifdef HIETH_TSO_SUPPORTED
	hieth_irq_enable(priv, UD_BIT_NAME(HIETH_INT_TX_ERR));
#endif

	priv->monitor.expires =
	    jiffies + msecs_to_jiffies(HIETH_MONITOR_TIMER);
	add_timer(&priv->monitor);

	return 0;
}

static int hieth_net_close(struct net_device *dev)
{
	struct hieth_netdev_priv *priv = netdev_priv(dev);
#ifdef HIETH_TSO_SUPPORTED
	struct sk_buff *skb = NULL;
#endif

	hieth_irq_disable(priv, UD_BIT_NAME(HIETH_GLB_IRQ_INT_MULTI_RXRDY));
#ifdef HIETH_TSO_SUPPORTED
	hieth_irq_disable(priv, UD_BIT_NAME(HIETH_INT_TX_ERR));
#endif

	if (priv->phy)
		phy_stop(priv->phy);

	del_timer_sync(&priv->monitor);

	/* delete tasklet  */
	tasklet_kill(&priv->bf_recv);

	/* reset and init port */
	hieth_port_reset(priv);

	skb_queue_purge(&priv->rx_head);
	skb_queue_purge(&priv->rx_hw);
#ifdef HIETH_TSO_SUPPORTED
	while (priv->txq_tail != priv->txq_head) {
		skb = hieth_xmit_release_gso(priv);
		BUG_ON(skb == NULL);
		kfree_skb(skb);
	}
#else
	skb_queue_purge(&priv->tx_hw);
#endif
	priv->tx_hw_cnt = 0;

	free_irq(dev->irq, dev);
	return 0;
}

static void hieth_net_timeout(struct net_device *dev)
{
	pr_err("tx timeout\n");
}

static int hieth_net_hard_start_xmit(struct sk_buff *skb,
				     struct net_device *dev)
{
	int ret;
	struct hieth_netdev_priv *priv = netdev_priv(dev);

	hieth_xmit_release_skb(priv);

	ret = hieth_xmit_real_send(priv, skb);
	if (ret < 0) {
		priv->stats.tx_dropped++;
		return NETDEV_TX_BUSY;
	}

	dev->trans_start = jiffies;
	priv->stats.tx_packets++;
	priv->stats.tx_bytes += skb->len;
	hieth_clear_irqstatus(priv, UD_BIT_NAME(HIETH_GLB_IRQ_INT_TXQUE_RDY));

	if (!hieth_hw_xmitq_ready(priv)) {
		netif_stop_queue(dev);
		hieth_irq_enable(priv,
				 UD_BIT_NAME(HIETH_GLB_IRQ_INT_TXQUE_RDY));
	}

	return NETDEV_TX_OK;
}

static struct net_device_stats *hieth_net_get_stats(struct net_device *dev)
{
	struct hieth_netdev_priv *priv = netdev_priv(dev);

	return &priv->stats;
}

static int hieth_net_set_mac_address(struct net_device *dev, void *p)
{
	struct hieth_netdev_priv *priv = netdev_priv(dev);
	struct sockaddr *skaddr = p;

	if (!is_valid_ether_addr(skaddr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, skaddr->sa_data, dev->addr_len);
	dev->addr_assign_type &= ~NET_ADDR_RANDOM;

	hieth_hw_set_macaddress(priv, dev->dev_addr);

	return 0;
}

static inline void hieth_enable_mac_addr_filter(struct hieth_netdev_priv *priv,
						unsigned int reg_n, int enable)
{
	u32 val;

	val = hieth_readl(priv->glb_base, GLB_MAC_H16(priv->port, reg_n));
	if (enable)
		val |= UD_BIT_NAME(HIETH_GLB_MACFLT_ENA);
	else
		val &= ~(UD_BIT_NAME(HIETH_GLB_MACFLT_ENA));
	hieth_writel(priv->glb_base, val, GLB_MAC_H16(priv->port, reg_n));
}

static void hieth_set_mac_addr(struct hieth_netdev_priv *priv, u8 addr[6],
			       unsigned int high, unsigned int low)
{
	u32 val;
	u32 data;

	val = hieth_readl(priv->glb_base, high);
	val |= UD_BIT_NAME(HIETH_GLB_MACFLT_ENA);
	hieth_writel(priv->glb_base, val, high);

	val &= ~HIETH_GLB_MACFLT_HI16;
	val |= ((addr[0] << 8) | addr[1]);
	hieth_writel(priv->glb_base, val, high);

	data = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5];
	hieth_writel(priv->glb_base, data, low);

	val |= UD_BIT_NAME(HIETH_GLB_MACFLT_FW2CPU);
	hieth_writel(priv->glb_base, val, high);
}

static inline void hieth_set_mac_addr_filter(struct hieth_netdev_priv *priv,
					     unsigned char *addr,
					     unsigned int reg_n)
{
	hieth_set_mac_addr(priv, addr, GLB_MAC_H16(priv->port, reg_n),
			   GLB_MAC_L32(priv->port, reg_n));
}

static void hieth_net_set_rx_mode(struct net_device *dev)
{
	u32 val;
	struct hieth_netdev_priv *priv = netdev_priv(dev);

	local_lock(priv);

	val = hieth_readl(priv->glb_base, HIETH_GLB_FWCTRL);
	if (dev->flags & IFF_PROMISC) {
		val |= ((priv->port == HIETH_PORT_0) ?
			HIETH_GLB_FWCTRL_FWALL2CPU_U :
			HIETH_GLB_FWCTRL_FWALL2CPU_D);
		hieth_writel(priv->glb_base, val, HIETH_GLB_FWCTRL);
	} else {
		val &= ~((priv->port == HIETH_PORT_0) ?
			HIETH_GLB_FWCTRL_FWALL2CPU_U :
			HIETH_GLB_FWCTRL_FWALL2CPU_D);
		hieth_writel(priv->glb_base, val, HIETH_GLB_FWCTRL);

		val = hieth_readl(priv->glb_base, HIETH_GLB_MACTCTRL);
		if ((netdev_mc_count(dev) > HIETH_MAX_MULTICAST_ADDRESSES) ||
		    (dev->flags & IFF_ALLMULTI)) {
			val |= UD_BIT_NAME(HIETH_GLB_MACTCTRL_MULTI2CPU);
		} else {
			int reg = HIETH_MAX_UNICAST_ADDRESSES;
			int i;
			struct netdev_hw_addr *ha;

			for (i = reg; i < HIETH_MAX_MAC_FILTER_NUM; i++)
				hieth_enable_mac_addr_filter(priv, i, 0);

			netdev_for_each_mc_addr(ha, dev) {
				hieth_set_mac_addr_filter(priv, ha->addr, reg);
				reg++;
			}

			val &= ~(UD_BIT_NAME(HIETH_GLB_MACTCTRL_MULTI2CPU));
		}

		/* Handle multiple unicast addresses (perfect filtering)*/
		if (netdev_uc_count(dev) > HIETH_MAX_UNICAST_ADDRESSES) {
			val |= UD_BIT_NAME(HIETH_GLB_MACTCTRL_UNI2CPU);
		} else {
			int reg = 0;
			int i;
			struct netdev_hw_addr *ha;

			for (i = reg; i < HIETH_MAX_UNICAST_ADDRESSES; i++)
				hieth_enable_mac_addr_filter(priv, i, 0);

			netdev_for_each_uc_addr(ha, dev) {
				hieth_set_mac_addr_filter(priv, ha->addr, reg);
				reg++;
			}

			val &= ~(UD_BIT_NAME(HIETH_GLB_MACTCTRL_UNI2CPU));
		}
		hieth_writel(priv->glb_base, val, HIETH_GLB_MACTCTRL);
	}

	local_unlock(priv);
}

static int hieth_net_ioctl(struct net_device *net_dev,
			   struct ifreq *ifreq, int cmd)
{
	struct hieth_netdev_priv *priv = netdev_priv(net_dev);
	struct hieth_pm_config pm_config;

	switch (cmd) {
	case SIOCSETPM:
		if (copy_from_user(&pm_config, ifreq->ifr_data,
				   sizeof(pm_config)))
			return -EFAULT;
		return hieth_pmt_config(&pm_config);

	default:
		if (!netif_running(net_dev))
			return -EINVAL;

		if (!priv->phy)
			return -EINVAL;

		return phy_mii_ioctl(priv->phy, ifreq, cmd);
	}

	return 0;
}

static void hieth_ethtools_get_drvinfo(struct net_device *net_dev,
				       struct ethtool_drvinfo *info)
{
	strcpy(info->driver, "hieth driver");
	strcpy(info->version, "v300");
	strcpy(info->bus_info, "platform");
}

static u32 hieth_ethtools_get_link(struct net_device *net_dev)
{
	struct hieth_netdev_priv *priv = netdev_priv(net_dev);

	return ((priv->phy->link) ? HIETH_P_MAC_PORTSET_LINKED : 0);
}

static int hieth_ethtools_get_settings(struct net_device *net_dev,
				       struct ethtool_cmd *cmd)
{
	struct hieth_netdev_priv *priv = netdev_priv(net_dev);

	if (priv->phy)
		return phy_ethtool_gset(priv->phy, cmd);

	return -EINVAL;
}

static int hieth_ethtools_set_settings(struct net_device *net_dev,
				       struct ethtool_cmd *cmd)
{
	struct hieth_netdev_priv *priv = netdev_priv(net_dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (priv->phy)
		return phy_ethtool_sset(priv->phy, cmd);

	return -EINVAL;
}

static void hieth_get_pauseparam(struct net_device *net_dev,
				 struct ethtool_pauseparam *pause)
{
	struct hieth_netdev_priv *priv = netdev_priv(net_dev);

	pause->autoneg = priv->phy->autoneg;
	pause->rx_pause = 1;
	if (priv->tx_pause_en)
		pause->tx_pause = 1;
}

static int hieth_set_pauseparam(struct net_device *net_dev,
				struct ethtool_pauseparam *pause)
{
	struct hieth_netdev_priv *priv = netdev_priv(net_dev);
	struct phy_device *phy = priv->phy;
	int ret = 0;

	if (pause->rx_pause == 0)
		return -EINVAL;

	if (pause->tx_pause != priv->tx_pause_en) {
		priv->tx_pause_en = pause->tx_pause;
		hieth_set_flow_ctrl(priv);
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

static inline void hieth_enable_rxcsum_drop(struct hieth_netdev_priv *priv,
					    bool drop)
{
	unsigned int val;

	val = hieth_readl(priv->port_base, HIETH_P_GLB_RX_COE_CTRL);
	if (drop)
		val |= COE_ERR_DROP;
	else
		val &= ~COE_ERR_DROP;
	hieth_writel(priv->port_base, val, HIETH_P_GLB_RX_COE_CTRL);
}

static int hieth_set_features(struct net_device *dev,
			      netdev_features_t features)
{
	struct hieth_netdev_priv *priv = netdev_priv(dev);
	netdev_features_t changed = dev->features ^ features;

	if (changed & NETIF_F_RXCSUM) {
		if (features & NETIF_F_RXCSUM)
			hieth_enable_rxcsum_drop(priv, true);
		else
			hieth_enable_rxcsum_drop(priv, false);
	}

	return 0;
}

static struct ethtool_ops hieth_ethtools_ops = {
	.get_drvinfo = hieth_ethtools_get_drvinfo,
	.get_link = hieth_ethtools_get_link,
	.get_settings = hieth_ethtools_get_settings,
	.set_settings = hieth_ethtools_set_settings,
	.get_pauseparam = hieth_get_pauseparam,
	.set_pauseparam = hieth_set_pauseparam,
};

static const struct net_device_ops hieth_netdev_ops = {
	.ndo_open = hieth_net_open,
	.ndo_stop = hieth_net_close,
	.ndo_start_xmit = hieth_net_hard_start_xmit,
	.ndo_tx_timeout = hieth_net_timeout,
	.ndo_do_ioctl = hieth_net_ioctl,
	.ndo_set_mac_address = hieth_net_set_mac_address,
	.ndo_set_rx_mode	= hieth_net_set_rx_mode,
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_get_stats = hieth_net_get_stats,
	.ndo_set_features       = hieth_set_features,
};

static void hieth_verify_flow_ctrl_args(struct hieth_netdev_priv *priv)
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

static int hieth_platdev_probe_port(struct platform_device *pdev,
				    struct hieth_netdev_priv *com_priv,
				    int port)
{
	int ret = -1;
	struct net_device *netdev = NULL;
	struct device *dev = &pdev->dev;
	struct hieth_netdev_priv *priv;

	if ((HIETH_PORT_0 != port) && (HIETH_PORT_1 != port)) {
		pr_err("port error!\n");
		ret = -ENODEV;
		goto _error_exit;
	}

	netdev = alloc_etherdev(sizeof(*priv));
	if (netdev == NULL) {
		pr_err("alloc_etherdev fail!\n");
		ret = -ENOMEM;
		goto _error_exit;
	}

	platform_set_drvdata(pdev, netdev);
	SET_NETDEV_DEV(netdev, &pdev->dev);

	netdev->irq = com_priv->irq;

	netdev->watchdog_timeo = 3 * HZ;
	netdev->netdev_ops = &hieth_netdev_ops;
	netdev->ethtool_ops = &hieth_ethtools_ops;

#ifdef HIETH_TSO_SUPPORTED
	netdev->hw_features |= NETIF_F_SG | NETIF_F_TSO | NETIF_F_TSO6 |
		NETIF_F_UFO | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM;
#endif
#ifdef HIETH_RXCSUM_SUPPORTED
	netdev->hw_features |= NETIF_F_RXCSUM;
#endif
	netdev->features |= netdev->hw_features;
	netdev->vlan_features |= netdev->features;

	netdev->priv_flags |= IFF_UNICAST_FLT;

	if (hieth_phy_param[port].macaddr)
		ether_addr_copy(netdev->dev_addr,
				hieth_phy_param[port].macaddr);

	if (!is_valid_ether_addr(netdev->dev_addr))
		eth_hw_addr_random(netdev);

	/* init hieth_global somethings... */
	hieth_devs_save[port] = netdev;

	/* init hieth_local_driver */
	priv = netdev_priv(netdev);
	memset(priv, 0, sizeof(*priv));
	memcpy(priv, com_priv, sizeof(*priv));

	local_lock_init(priv);

	priv->port = port;

	if (port == HIETH_PORT_0)
		priv->port_base = priv->glb_base;
	else
		priv->port_base = priv->glb_base + 0x2000;

	priv->dev = dev;

	init_timer(&priv->monitor);
	priv->monitor.function = hieth_monitor_func;
	priv->monitor.data = (unsigned long)netdev;
	priv->monitor.expires =
	    jiffies + msecs_to_jiffies(HIETH_MONITOR_TIMER);

	/* wol need */
	device_set_wakeup_capable(priv->dev, 1);
	/* TODO: when we can let phy powerdown?
	 * In forcing fwd mode, we don't want phy powerdown,
	 * so I set wakeup enable all the time
	 */
	device_set_wakeup_enable(priv->dev, 1);

#ifdef CONFIG_TX_FLOW_CTRL_SUPPORT
	priv->tx_pause_en = 1;
#endif
	priv->tx_pause_active_thresh = CONFIG_TX_FLOW_CTRL_ACTIVE_THRESHOLD;
	priv->tx_pause_deactive_thresh = CONFIG_TX_FLOW_CTRL_DEACTIVE_THRESHOLD;

	hieth_verify_flow_ctrl_args(priv);

	/* reset and init port */
	hieth_port_init(priv);

#ifdef HIETH_RXCSUM_SUPPORTED
	hieth_enable_rxcsum_drop(priv, true);
#endif
	priv->depth.hw_xmitq = HIETH_HWQ_XMIT_DEPTH;

	priv->phy = of_phy_connect(netdev, priv->phy_node,
				   hieth_adjust_link, 0, priv->phy_mode);
	if (!(priv->phy) || IS_ERR(priv->phy)) {
		pr_info("connect to port[%d] PHY failed!\n", port);
		priv->phy = NULL;
		goto _error_phy_connect;
	}

	priv->phy->advertising |= ADVERTISED_Pause;
	priv->phy->supported |= ADVERTISED_Pause;

	pr_info("attached port %d PHY %d to driver %s, phy_mode=%s\n",
		port, priv->phy->addr, priv->phy->drv->name,
		phy_modes(priv->phy_mode));

	if (hieth_enable_autoeee)
		hieth_autoeee_init(priv, 0);

	skb_queue_head_init(&priv->rx_head);
	skb_queue_head_init(&priv->rx_hw);
#ifdef HIETH_TSO_SUPPORTED
	priv->q_size = 2 * (priv->depth.hw_xmitq);

	priv->dma_tx = (struct dma_tx_desc *)dma_alloc_coherent(priv->dev,
			priv->q_size * sizeof(struct dma_tx_desc),
			&priv->dma_tx_phy,
			GFP_KERNEL);
	if (priv->dma_tx == NULL) {
		pr_err("dma_alloc_coherent fail!\n");
		goto _error_alloc_dma_tx;
	}
	priv->sg_head = 0;
	priv->sg_tail = 0;

	priv->txq = kmalloc_array(priv->q_size, sizeof(struct tx_pkt_info),
			GFP_KERNEL);
	if (priv->txq == NULL)
		goto _error_alloc_txq;
	priv->txq_head = 0;
	priv->txq_tail = 0;
#else
	skb_queue_head_init(&priv->tx_hw);
#endif
	priv->tx_hw_cnt = 0;

	ret = hieth_init_skb_buffers(priv);
	if (ret) {
		pr_err("hieth_init_skb_buffers failed!\n");
		goto _error_init_skb_buffers;
	}

	ret = register_netdev(netdev);
	if (ret) {
		pr_err("register_netdev %s failed!\n", netdev->name);
		goto _error_register_netdev;
	}

	return ret;

_error_register_netdev:
	hieth_destroy_skb_buffers(priv);

_error_init_skb_buffers:
#ifdef HIETH_TSO_SUPPORTED
	kfree(priv->txq);
_error_alloc_txq:
	dma_free_coherent(priv->dev,
			  priv->q_size * sizeof(struct dma_tx_desc),
			  priv->dma_tx, priv->dma_tx_phy);
_error_alloc_dma_tx:
#endif
	phy_disconnect(priv->phy);
	priv->phy = NULL;

_error_phy_connect:
	local_lock_exit();
	hieth_devs_save[port] = NULL;
	free_netdev(netdev);

_error_exit:
	return ret;
}

static int hieth_platdev_remove_port(struct platform_device *pdev, int port)
{
	struct net_device *ndev;
	struct hieth_netdev_priv *priv;

	ndev = hieth_devs_save[port];

	if (!ndev)
		goto _ndev_exit;

	priv = netdev_priv(ndev);

	unregister_netdev(ndev);
#ifdef HIETH_TSO_SUPPORTED
	kfree(priv->txq);
	dma_free_coherent(priv->dev,
			  priv->q_size * sizeof(struct dma_tx_desc),
			  priv->dma_tx, priv->dma_tx_phy);
#endif
	hieth_destroy_skb_buffers(priv);

	phy_disconnect(priv->phy);
	priv->phy = NULL;

	iounmap((void *)priv->glb_base);

	local_lock_exit();

	hieth_devs_save[port] = NULL;
	free_netdev(ndev);

_ndev_exit:
	return 0;
}

static int hieth_of_get_param(struct device_node *node)
{
	struct device_node *child = NULL;
	int idx = 0;
	int data;

	/* get auto eee */
	hieth_enable_autoeee = of_property_read_bool(node, "autoeee");

	for_each_available_child_of_node(node, child) {
		/* get phy-addr */
		if (of_property_read_u32(child, "reg", &data))
			return -EINVAL;
		if ((data < 0) || (data >= PHY_MAX_ADDR)) {
			pr_info("%s has invalid PHY address\n",
				child->full_name);
			data = HIETH_INVALID_PHY_ADDR;
		}

		hieth_phy_param[idx].phy_addr = data;
		if (data != HIETH_INVALID_PHY_ADDR)
			hieth_phy_param[idx].isvalid = true;

		/* get phy_mode */
		hieth_phy_param[idx].phy_mode = of_get_phy_mode(child);

		/* get mac */
		hieth_phy_param[idx].macaddr = of_get_mac_address(child);

		/* get gpio_base and bit */
		of_property_read_u32(child, "phy-gpio-base",
				     (u32 *)(&hieth_phy_param[idx].gpio_base));
		of_property_read_u32(child, "phy-gpio-bit",
				     &hieth_phy_param[idx].gpio_bit);

		/* get internal flag */
		hieth_phy_param[idx].isinternal =
			of_property_read_bool(child, "internal-phy");

		if (++idx >= HIETH_MAX_PORT)
			break;
	}

	return 0;
}

static void hieth_mac_core_reset(struct hieth_netdev_priv *priv)
{
	/* undo reset */
	reset_control_deassert(priv->mac_rst);
	usleep_range(50, 60);

	/* soft reset mac port */
	reset_control_assert(priv->mac_rst);
	usleep_range(50, 60);
	/* undo reset */
	reset_control_deassert(priv->mac_rst);
}

void hieth_phy_get_reset_controller(struct device *dev)
{
	int i;
	struct hieth_phy_param_s *phy_param;
	char *rst_name;

	for (i = 0; i < HIETH_MAX_PORT; i++) {
		phy_param = &hieth_phy_param[i];

		if (!phy_param->isvalid)
			continue;

		if (i == 0)
			rst_name = HIETH_PHY0_RST_NAME;
		else
			rst_name = HIETH_PHY1_RST_NAME;

		phy_param->phy_rst = devm_reset_control_get(dev, rst_name);
		if (IS_ERR(phy_param->phy_rst))
			phy_param->phy_rst = NULL;
	}
}

static int hieth_plat_driver_probe(struct platform_device *pdev)
{
	int ret = 0;
	int irq;
	struct net_device *ndev = NULL;
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	struct resource *res;
	struct hieth_netdev_priv *priv = &hieth_priv;
	struct device_node *child = NULL;
	int port = -1;

	memset(hieth_devs_save, 0, sizeof(hieth_devs_save));
	memset(hieth_phy_param, 0, sizeof(hieth_phy_param));
	memset(priv, 0, sizeof(*priv));

	if (hieth_of_get_param(node)) {
		pr_err("of get parameter fail\n");
		ret = -ENODEV;
		goto exit;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->glb_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->glb_base)) {
		ret = PTR_ERR(priv->glb_base);
		goto exit;
	}

	priv->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(priv->clk)) {
		pr_err("failed to get clk\n");
		ret = -ENODEV;
		goto exit;
	}

	ret = clk_prepare_enable(priv->clk);
	if (ret < 0) {
		pr_err("failed to enable clk %d\n", ret);
		goto exit;
	}

	priv->mac_rst = devm_reset_control_get(dev, HIETH_MAC_RST_NAME);
	if (IS_ERR(priv->mac_rst)) {
		ret = PTR_ERR(priv->mac_rst);
		goto exit_clk_disable;
	}

	hieth_mac_core_reset(priv);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		pr_err("no IRQ defined!\n");
		ret = -ENODEV;
		goto exit_clk_disable;
	}
	priv->irq = irq;

	hieth_phy_get_reset_controller(dev);
	hieth_phy_reset();

	if (hieth_mdiobus_driver_init(pdev, priv)) {
		pr_err("mdio bus init error!\n");
		ret = -ENODEV;
		goto exit_clk_disable;
	}

	/* phy param */
	hieth_phy_register_fixups();

	for_each_available_child_of_node(node, child) {
		if (++port >= HIETH_MAX_PORT)
			break;

		if (!hieth_phy_param[port].isvalid)
			continue;

		priv->phy_node = of_parse_phandle(node, "phy-handle", port);
		if (!priv->phy_node) {
			pr_err("not find phy-handle [%d]\n", port);
			continue;
		}

		priv->phy_mode = hieth_phy_param[port].phy_mode;

		if (!hieth_platdev_probe_port(pdev, priv, port))
			hieth_real_port_cnt++;
	}

	if (hieth_devs_save[HIETH_PORT_0])
		ndev = hieth_devs_save[HIETH_PORT_0];
	else if (hieth_devs_save[HIETH_PORT_1])
		ndev = hieth_devs_save[HIETH_PORT_1];

	if (!ndev) {
		pr_err("no dev probed!\n");
		ret = -ENODEV;
		goto exit_mdiobus;
	}

	return ret;

exit_mdiobus:
	hieth_mdiobus_driver_exit(priv);

exit_clk_disable:
	clk_disable_unprepare(priv->clk);

exit:

	return ret;
}

static int hieth_plat_driver_remove(struct platform_device *pdev)
{
	int i;
	struct net_device *ndev = NULL;
	struct hieth_netdev_priv *priv = netdev_priv(ndev);

	if (hieth_devs_save[HIETH_PORT_0])
		ndev = hieth_devs_save[HIETH_PORT_0];
	else if (hieth_devs_save[HIETH_PORT_1])
		ndev = hieth_devs_save[HIETH_PORT_1];

	free_irq(ndev->irq, hieth_devs_save);

	for (i = 0; i < HIETH_MAX_PORT; i++)
		hieth_platdev_remove_port(pdev, i);

	hieth_mdiobus_driver_exit(priv);

	clk_disable_unprepare(priv->clk);

	memset(hieth_devs_save, 0, sizeof(hieth_devs_save));

	return 0;
}

#ifdef CONFIG_PM
static int hieth_plat_driver_suspend_port(struct platform_device *pdev,
					  pm_message_t state, int port)
{
	struct net_device *ndev = hieth_devs_save[port];

	if (ndev) {
		if (netif_running(ndev)) {
			hieth_net_close(ndev);
			netif_device_detach(ndev);
		}
	}

	return 0;
}

int hieth_plat_driver_suspend(struct platform_device *pdev,
			      pm_message_t state)
{
	int i;
	bool power_off = true;
	struct hieth_netdev_priv *priv = NULL;

	for (i = 0; i < HIETH_MAX_PORT; i++)
		hieth_plat_driver_suspend_port(pdev, state, i);

	if (hieth_pmt_enter())
		power_off = false;

	if (power_off) {
		for (i = 0; i < HIETH_MAX_PORT; i++) {
			if (hieth_devs_save[i]) {
				priv = netdev_priv(hieth_devs_save[i]);
				genphy_suspend(priv->phy);/* power down phy */
			}
		}

		/* need some time before phy suspend finished. */
		usleep_range(1000, 10000);

		if (priv)
			clk_disable_unprepare(priv->clk);
	}

	return 0;
}

static int hieth_plat_driver_resume_port(struct platform_device *pdev, int port)
{
	struct net_device *ndev = hieth_devs_save[port];
	struct hieth_netdev_priv *priv = netdev_priv(ndev);

	if (ndev) {
		if (netif_running(ndev)) {
			hieth_port_init(priv);
			hieth_net_open(ndev);
			netif_device_attach(ndev);
		}
	}

	return 0;
}

int hieth_plat_driver_resume(struct platform_device *pdev)
{
	int i;
	struct hieth_netdev_priv *priv = &hieth_priv;

	/* enable clk */
	clk_prepare_enable(priv->clk);
	hieth_phy_reset();

	for (i = 0; i < HIETH_MAX_PORT; i++)
		hieth_plat_driver_resume_port(pdev, i);

	hieth_pmt_exit();
	return 0;
}
#else
#  define hieth_plat_driver_suspend	NULL
#  define hieth_plat_driver_resume	NULL
#endif

static const struct of_device_id hieth_of_match[] = {
	{.compatible = "hisilicon,hieth", },
	{},
};

MODULE_DEVICE_TABLE(of, hieth_of_match);

static struct platform_driver hieth_platform_driver = {
	.probe = hieth_plat_driver_probe,
	.remove = hieth_plat_driver_remove,
	.suspend = hieth_plat_driver_suspend,
	.resume = hieth_plat_driver_resume,
	.driver = {
		   .owner = THIS_MODULE,
		   .name = HIETH_DRIVER_NAME,
		   .bus = &platform_bus_type,
		   .of_match_table = of_match_ptr(hieth_of_match),
		   },
};

static int hieth_mod_init(void)
{
	int ret = 0;

	if (hieth_disable)
		return 0;

	ret = platform_driver_register(&hieth_platform_driver);
	if (ret)
		pr_err("register platform driver failed!\n");

	hieth_proc_create();

	return ret;
}

static void hieth_mod_exit(void)
{
	if (hieth_disable)
		return;

	hieth_proc_destroy();

	platform_driver_unregister(&hieth_platform_driver);
}

module_init(hieth_mod_init);
module_exit(hieth_mod_exit);

MODULE_DESCRIPTION("Hisilicon ETH driver whith MDIO support");
MODULE_LICENSE("GPL");

/* vim: set ts=8 sw=8 tw=78: */
