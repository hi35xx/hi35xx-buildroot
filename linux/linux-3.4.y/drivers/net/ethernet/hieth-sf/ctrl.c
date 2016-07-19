#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/skbuff.h>
#include "hieth.h"
#include "ctrl.h"
#include "glb.h"
#include "mac.h"

#ifdef HIETH_TSO_SUPPORTED
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <net/protocol.h>
#include <net/ipv6.h>
#endif
static inline int _hieth_irq_enable(struct hieth_netdev_local *ld, int irqs)
{
	int old;

	old = hieth_readl(ld, GLB_RW_IRQ_ENA);
	hieth_writel(ld, old | irqs, GLB_RW_IRQ_ENA);
	old = hieth_readl(ld, GLB_RW_IRQ_ENA);

	return old;
}

static inline int _hieth_irq_disable(struct hieth_netdev_local *ld, int irqs)
{
	int old;

	old = hieth_readl(ld, GLB_RW_IRQ_ENA);
	hieth_writel(ld, old & (~irqs), GLB_RW_IRQ_ENA);

	return old;
}

static inline int _hieth_read_irqstatus(struct hieth_netdev_local *ld)
{
	int status;

	status = hieth_readl(ld, GLB_RO_IRQ_STAT);

	return status;
}

int hieth_hw_set_macaddress(
		struct hieth_netdev_local *ld,
		int ena,
		unsigned char *mac)
{
	unsigned long reg;

	local_lock(ld);

	if (ld->port == DOWN_PORT)
		hieth_writel_bits(ld, 1, GLB_DN_HOSTMAC_ENA, BITS_DN_HOST_ENA);

	reg = mac[1] | (mac[0] << 8);
	if (ld->port == UP_PORT)
		hieth_writel(ld, reg, GLB_HOSTMAC_H16);
	else
		hieth_writel(ld, reg, GLB_DN_HOSTMAC_H16);

	reg = mac[5] | (mac[4]<<8) | (mac[3]<<16) | (mac[2]<<24);
	if (ld->port == UP_PORT)
		hieth_writel(ld, reg, GLB_HOSTMAC_L32);
	else
		hieth_writel(ld, reg, GLB_DN_HOSTMAC_L32);

	local_unlock(ld);

	return 0;
}

int hieth_hw_get_macaddress(struct hieth_netdev_local *ld, unsigned char *mac)
{
	unsigned long reg;

	local_lock(ld);

	if (ld->port == UP_PORT)
		reg = hieth_readl(ld, GLB_HOSTMAC_H16);
	else
		reg = hieth_readl(ld, GLB_DN_HOSTMAC_H16);
	mac[0] = (reg>>8) & 0xff;
	mac[1] = reg & 0xff;

	if (ld->port == UP_PORT)
		reg = hieth_readl(ld, GLB_HOSTMAC_L32);
	else
		reg = hieth_readl(ld, GLB_DN_HOSTMAC_L32);
	mac[2] = (reg>>24) & 0xff;
	mac[3] = (reg>>16) & 0xff;
	mac[4] = (reg>>8) & 0xff;
	mac[5] = reg & 0xff;

	local_unlock(ld);

	return 0;
}

static inline int _test_xmit_queue_ready(struct hieth_netdev_local *ld)
{
	return hieth_readl_bits(ld,
			 UD_REG_NAME(GLB_RO_QUEUE_STAT), BITS_XMITQ_RDY);
}

static inline int _test_recv_queue_ready(struct hieth_netdev_local *ld)
{
	return hieth_readl_bits(ld,
			 UD_REG_NAME(GLB_RO_QUEUE_STAT), BITS_RECVQ_RDY);
}

int hieth_irq_enable(struct hieth_netdev_local *ld, int irqs)
{
	int old;

	local_lock(ld);
	old = _hieth_irq_enable(ld, irqs);
	local_unlock(ld);

	return old;
}

int hieth_irq_disable(struct hieth_netdev_local *ld, int irqs)
{
	int old;

	local_lock(ld);
	old = _hieth_irq_disable(ld, irqs);
	local_unlock(ld);

	return old;
}

int hieth_read_irqstatus(struct hieth_netdev_local *ld)
{
	int status;

	local_lock(ld);
	status = _hieth_read_irqstatus(ld);
	local_unlock(ld);

	return status;
}

int hieth_read_raw_irqstatus(struct hieth_netdev_local *ld)
{
	int status;

	local_lock(ld);
	status = hieth_readl(ld, GLB_RO_IRQ_STAT);
	local_unlock(ld);

	return status;
}

int hieth_clear_irqstatus(struct hieth_netdev_local *ld, int irqs)
{
	int status;

	local_lock(ld);
	hieth_writel(ld, irqs, GLB_RW_IRQ_RAW);
	status = _hieth_read_irqstatus(ld);
	local_unlock(ld);

	return status;
}

int hieth_set_endian_mode(struct hieth_netdev_local *ld, int mode)
{
	int old;

	local_lock(ld);
	old = hieth_readl_bits(ld, GLB_ENDIAN_MOD, BITS_ENDIAN);
	hieth_writel_bits(ld, mode, GLB_ENDIAN_MOD, BITS_ENDIAN);
	local_unlock(ld);

	return old;
}

void hieth_set_flow_ctrl(struct hieth_netdev_local *ld)
{
	int pause_en;

	local_lock(ld);

	hieth_writel_bits(ld, tx_flow_ctrl_active_threshold,
			UD_REG_NAME(GLB_FC_LEVEL), BITS_FC_ACTIVE_THR);
	hieth_writel_bits(ld, tx_flow_ctrl_deactive_threshold,
			UD_REG_NAME(GLB_FC_LEVEL), BITS_FC_DEACTIVE_THR);

	pause_en = tx_flow_ctrl_en ? 1 : 0;

	hieth_writel_bits(ld, pause_en, UD_REG_NAME(GLB_FC_LEVEL), BITS_FC_EN);
	hieth_writel_bits(ld, pause_en, UD_REG_NAME(MAC_SET), BITS_PAUSE_EN);

	local_unlock(ld);
}

int hieth_set_hwq_depth(struct hieth_netdev_local *ld)
{
	hieth_assert(ld->depth.hw_xmitq > 0  &&
			ld->depth.hw_xmitq  <= HIETH_MAX_QUEUE_DEPTH);

	local_lock(ld);

	if ((ld->depth.hw_xmitq) > HIETH_MAX_QUEUE_DEPTH) {
		local_unlock(ld);
		BUG();
		return -1;
	}

	hieth_writel_bits(ld, ld->depth.hw_xmitq, UD_REG_NAME(GLB_QLEN_SET),
			BITS_TXQ_DEP);
	hieth_writel_bits(ld, HIETH_MAX_QUEUE_DEPTH - ld->depth.hw_xmitq,
			UD_REG_NAME(GLB_QLEN_SET), BITS_RXQ_DEP);

	local_unlock(ld);

	return 0;
}

int hieth_hw_xmitq_ready(struct hieth_netdev_local *ld)
{
	int ret;

	local_lock(ld);
	ret = _test_xmit_queue_ready(ld);
	local_unlock(ld);

	return ret;
}

#ifdef HIETH_TSO_SUPPORTED
#ifdef HIETH_TSO_DEBUG
unsigned int id_send;
unsigned int id_free;
struct send_pkt_info	pkt_rec[MAX_RECORD];
#endif
#endif

#ifdef HIETH_TSO_SUPPORTED
struct sk_buff *hieth_xmit_release_gso(struct hieth_netdev_local *ld)
{
	struct sk_buff *skb;
	struct tx_pkt_info *txq_cur;
	int pkt_type;

	txq_cur = ld->txq + ld->txq_tail;

	skb = txq_cur->skb;

	if (txq_cur->tx.info.sg_flag)
		pkt_type = PKT_SG;
	else
		pkt_type = PKT_NORMAL;

	if (pkt_type == PKT_NORMAL) {
		dma_addr_t dma_addr;

		dma_addr = txq_cur->tx_addr;
		dma_unmap_single(ld->dev, dma_addr, skb->len, DMA_TO_DEVICE);
	} else { /* TSO pkt */
		struct dma_tx_desc *desc_cur;
		unsigned int desc_offset;
		int nfrags = skb_shinfo(skb)->nr_frags;
		int i;

		desc_offset = txq_cur->sg_desc_offset;
		BUG_ON(desc_offset != ld->sg_tail);
		desc_cur = ld->dma_tx + desc_offset;

		dma_unmap_single(ld->dev, desc_cur->linear_addr,
				desc_cur->linear_len, DMA_TO_DEVICE);
		for (i = 0; i < nfrags; i++) {
			dma_unmap_page(ld->dev, desc_cur->frags[i].addr,
					desc_cur->frags[i].size, DMA_TO_DEVICE);
		}

		ld->sg_tail = (ld->sg_tail + 1) % ld->q_size;
	}

	txq_cur->skb = NULL;
	ld->txq_tail = (ld->txq_tail + 1) % ld->q_size;

#ifdef HIETH_TSO_DEBUG
	pkt_rec[id_free].status = 0;
	id_free++;
	if (id_free == MAX_RECORD)
		id_free = 0;
#endif

	return skb;
}
#endif

int hieth_xmit_release_skb(struct hieth_netdev_local *ld)
{
	int ret = 0;
	struct sk_buff *skb;

	local_lock(ld);

	while (hw_xmitq_cnt_inuse(ld) < ld->tx_hw_cnt) {

		hieth_assert(ld->tx_hw_cnt);

#ifdef HIETH_TSO_SUPPORTED
		skb = hieth_xmit_release_gso(ld);
#else
		skb = skb_dequeue(&ld->tx_hw);
#endif

		if (!skb) {
			hieth_error("hw_xmitq_cnt_inuse=%d, tx_hw_cnt=%d\n",
			(int)hw_xmitq_cnt_inuse(ld), (int)ld->tx_hw_cnt);

			BUG();
			ret = -1;
			goto error_exit;
		}
		dev_kfree_skb_any(skb);

		ld->tx_hw_cnt--;
	}

error_exit:
	local_unlock(ld);
	return ret;
}

#ifdef HIETH_TSO_SUPPORTED
void hieth_get_tso_err_info(struct hieth_netdev_local *ld)
{
	unsigned int reg_addr, reg_tx_info, reg_tx_err;
	unsigned int sg_index;
	struct dma_tx_desc *sg_desc;
	int *sg_word;
	int i;

	reg_addr = hieth_readl(ld, UD_REG_NAME(GLB_TSO_DBG_ADDR));
	reg_tx_info = hieth_readl(ld, UD_REG_NAME(GLB_TSO_DBG_TX_INFO));
	reg_tx_err = hieth_readl(ld, UD_REG_NAME(GLB_TSO_DBG_TX_ERR));

	WARN(1, "tx err=0x%x, tx_info=0x%x, addr=0x%x\n",
			reg_tx_err, reg_tx_info, reg_addr);

	sg_index = (reg_addr - ld->dma_tx_phy) / sizeof(struct dma_tx_desc);
	sg_desc = ld->dma_tx + sg_index;
	sg_word = (int *)sg_desc;
	for (i = 0; i < sizeof(struct dma_tx_desc) / sizeof(int); i++)
		pr_err("%s,%d: sg_desc word[%d]=0x%x\n",
				__func__, __LINE__, i, sg_word[i]);

	/* restart MAC to transmit next packet */
	hieth_irq_disable(ld, UD_BIT_NAME(HIETH_INT_TX_ERR));
#if 0
	hieth_readl(ld, UD_REG_NAME(GLB_TSO_DBG_STATE));
	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_TX_ERR));
#endif
}

static int hieth_sw_gso(struct hieth_netdev_local *ld, struct sk_buff *skb)
{
	struct sk_buff *segs, *curr_skb;
	struct net_device *ndev = hieth_devs_save[ld->port];
	int ret;

	segs = skb_gso_segment(skb, ndev->features &
			~(NETIF_F_ALL_CSUM | NETIF_F_GSO_SOFTWARE));
	if (IS_ERR(segs))
		goto sw_tso_end;

	do {
		curr_skb = segs;
		segs = segs->next;
		curr_skb->next = NULL;
		ret = hieth_xmit_real_send(ld, curr_skb);
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
	unsigned int mtu = 0;
	unsigned char coe_enable = 0;

	if (skb->ip_summed == CHECKSUM_PARTIAL)
		coe_enable = 1;

	txq_cur->tx.val = 0;

	mtu = skb_shinfo(skb)->gso_size;

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

		mtu += iph->ihl * WORD_TO_BYTE;
		l4_proto = iph->protocol;
		l4_proto = l4_proto & (MAX_INET_PROTOS - 1);
	} else if (l3_proto == htons(ETH_P_IPV6)) {
		const struct ipv6hdr *hdr;

		txq_cur->tx.info.ip_ver = PKT_IPV6;
		txq_cur->tx.info.ip_hdr_len = PKT_IPV6_HDR_LEN;
		mtu += PKT_IPV6_HDR_LEN * WORD_TO_BYTE;
		hdr = ipv6_hdr(skb);
		l4_proto = hdr->nexthdr;
	} else {
		coe_enable = 0;
		/* pr_err("unknow level 3 prot=0x%x\n", ntohs(l3_proto)); */
	}

	if (l4_proto == IPPROTO_TCP) {
		const struct tcphdr *th;

		th = tcp_hdr(skb);
		txq_cur->tx.info.prot_type = PKT_TCP;
		txq_cur->tx.info.prot_hdr_len = th->doff;
		mtu += th->doff * WORD_TO_BYTE;
	} else if (l4_proto == IPPROTO_UDP) {
		txq_cur->tx.info.prot_type = PKT_UDP;
		txq_cur->tx.info.prot_hdr_len = PKT_UDP_HDR_LEN;
		if (l3_proto == htons(ETH_P_IPV6))
			mtu += sizeof(struct frag_hdr);
	} else {
		coe_enable = 0;
		/* pr_err("unknow level 4 prot=0x%x\n", l4_proto); */
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

int hieth_xmit_gso(struct hieth_netdev_local *ld, struct sk_buff *skb)
{
	int pkt_type = PKT_NORMAL;
	int nfrags = skb_shinfo(skb)->nr_frags;
	struct tx_pkt_info *txq_cur;
	int ret;

	if (((ld->txq_head + 1) % ld->q_size) == ld->txq_tail) {
		/* txq full, stop */
		hieth_error("WARNING: txq full.");
		return -E_MAC_TX_FAIL;
	}

	if (skb_is_gso(skb) || nfrags) {
		/* TSO pkt or SG pkt */
		pkt_type = PKT_SG;
	} else { /* Normal pkt */
		pkt_type = PKT_NORMAL;
	}

	txq_cur = ld->txq + ld->txq_head;

	ret = hieth_get_pkt_info(skb, txq_cur);
	if (unlikely(ret < 0))
		return ret;

	if (pkt_type == PKT_NORMAL) {
		txq_cur->tx_addr = dma_map_single(ld->dev, skb->data,
					skb->len, DMA_TO_DEVICE);
	} else { /* TSO pkt */
		struct dma_tx_desc *desc_cur;
		int i;

		if (unlikely(((ld->sg_head + 1) % ld->q_size) == ld->sg_tail)) {
			/* SG pkt, but sg desc all used */
			hieth_error("WARNING: sg desc all used.");
			return -E_MAC_TX_FAIL;
		}

		desc_cur = ld->dma_tx + ld->sg_head;

		/* TODO: deal with ipv6_id */
		if (txq_cur->tx.info.tso_flag &&
				txq_cur->tx.info.ip_ver == PKT_IPV6 &&
				txq_cur->tx.info.prot_type == PKT_UDP) {
			desc_cur->ipv6_id = ntohl(skb_shinfo(skb)->ip6_frag_id);
		}

		desc_cur->total_len = skb->len;
		desc_cur->linear_len = skb_headlen(skb);
		desc_cur->linear_addr = dma_map_single(ld->dev, skb->data,
				desc_cur->linear_len, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(ld->dev,
						desc_cur->linear_addr))) {
			hieth_error("DMA Mapping fail.");
			return -E_MAC_TX_FAIL;
		}

		for (i = 0; i < nfrags; i++) {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
			int len = frag->size;

			WARN(len <= 0,
				"%s,%d: frag %d size %d. desc=%x\n",
				__func__, __LINE__, i, len, txq_cur->tx.val);

			desc_cur->frags[i].addr =
				skb_frag_dma_map(ld->dev, frag, 0,
						len, DMA_TO_DEVICE);
			if (unlikely(dma_mapping_error(ld->dev,
						desc_cur->frags[i].addr))) {
				hieth_error("skb frag DMA Mapping fail.");
				return -E_MAC_TX_FAIL;
			}
			desc_cur->frags[i].size = len;
		}

		txq_cur->tx_addr = ld->dma_tx_phy +
			ld->sg_head * sizeof(struct dma_tx_desc);
		txq_cur->sg_desc_offset = ld->sg_head;

		ld->sg_head = (ld->sg_head + 1) % ld->q_size;

		wmb();
	}

	txq_cur->skb = skb;
	ld->txq_head = (ld->txq_head + 1) % ld->q_size;

#ifdef HIETH_TSO_DEBUG
	pkt_rec[id_send].reg_addr = txq_cur->tx_addr;
	pkt_rec[id_send].reg_pkt_info = txq_cur->tx.val;
	pkt_rec[id_send].status = 1;
	id_send++;
	if (id_send == MAX_RECORD)
		id_send = 0;
#endif

	hw_xmitq_pkg(ld, txq_cur->tx_addr, txq_cur->tx.val);

	return 0;
}
#endif

int hieth_xmit_real_send(struct hieth_netdev_local *ld, struct sk_buff *skb)
{
	int ret = 0;

	local_lock(ld);

	if (!_test_xmit_queue_ready(ld)) {
		hieth_error("hw xmit queue is not ready");
		ret = -1;
		goto _trans_exit;
	}

#ifdef HIETH_TSO_SUPPORTED
	/* TSO supported */
	ret = hieth_xmit_gso(ld, skb);
	if (unlikely(ret < 0)) {
		if (ret == -E_MAC_SW_GSO) {
			local_unlock(ld);
			return hieth_sw_gso(ld, skb);
		} else {
			ret = -1;
			goto _trans_exit;
		}
	}
#else
	dma_map_single(ld->dev, skb->data, skb->len, DMA_TO_DEVICE);
	/* for recalc CRC, 4 bytes more is needed */
	hw_xmitq_pkg(ld, virt_to_phys(skb->data), skb->len+4);

	skb_queue_tail(&ld->tx_hw, skb);
#endif

	ld->tx_hw_cnt++;

_trans_exit:
	local_unlock(ld);

	return ret;
}

int hieth_feed_hw(struct hieth_netdev_local *ld)
{
	struct sk_buff *skb;
	int cnt = 0;
	int rx_head_len;

	/* if skb occupied too much, then do not alloc any more. */
	rx_head_len = skb_queue_len(&ld->rx_head);
	if (rx_head_len > HIETH_MAX_RX_HEAD_LEN)
		return 0;

	local_lock(ld);

	while (hieth_readl_bits(ld, UD_REG_NAME(GLB_RO_QUEUE_STAT),
				 BITS_RECVQ_RDY)) {

		skb = hieth_platdev_alloc_skb(ld);

		if (!skb)
			break;

		dma_map_single(ld->dev, skb->data,
				 HIETH_MAX_FRAME_SIZE, DMA_FROM_DEVICE);

		hieth_writel(ld, virt_to_phys(skb->data + 2),
				 UD_REG_NAME(GLB_IQ_ADDR));

		skb_queue_tail(&ld->rx_hw, skb);

		cnt++;
	}

	local_unlock(ld);

	return cnt;
}

int hieth_hw_recv_tryup(struct hieth_netdev_local *ld)
{
	struct sk_buff *skb;
	uint32_t rlen;
	int cnt = 0;
	uint32_t rx_pkt_info;
#ifdef HIETH_RXCSUM_SUPPORTED
	struct net_device *ndev = hieth_devs_save[ld->port];
	int hdr_csum_done, hdr_csum_err;
	int payload_csum_done, payload_csum_err;
#endif

	local_lock(ld);

	while (is_recv_packet(ld)) {
		rx_pkt_info = hw_get_rxpkg_info(ld);
		rlen = (rx_pkt_info >> BITS_RXPKG_LEN_OFFSET)
				& BITS_RXPKG_LEN_MASK;
		rlen -= 4;
		hw_set_rxpkg_finish(ld);

		skb = skb_dequeue(&ld->rx_hw);

		if (!skb) {
			hieth_error(
				"chip told us to receive pkg, but no more can be received!\n");
			BUG();
			break;
		}

		dma_map_single(ld->dev, skb->data,
				HIETH_MAX_FRAME_SIZE, DMA_FROM_DEVICE);
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
					ld->stats.rx_errors++;
					ld->stats.rx_crc_errors++;
					dev_kfree_skb_any(skb);
					continue;
				} else
					skb->ip_summed = CHECKSUM_UNNECESSARY;
			}
		}
#endif
		skb_queue_tail(&ld->rx_head, skb);
		cnt++;
	}

	local_unlock(ld);

	/* fill hardware receive queue again */
	hieth_feed_hw(ld);

	return cnt;
}

/* vim: set ts=8 sw=8 tw=78: */
