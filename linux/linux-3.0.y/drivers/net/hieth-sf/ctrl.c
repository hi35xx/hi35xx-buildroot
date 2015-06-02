#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/skbuff.h>
#include "hieth.h"
#include "ctrl.h"
#include "glb.h"

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
	old = hieth_readl(ld, GLB_RW_IRQ_ENA);

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

	reg = mac[1] | (mac[0] << 8);
	hieth_writel(ld, reg, GLB_HOSTMAC_H16);

	reg = mac[5] | (mac[4]<<8) | (mac[3]<<16) | (mac[2]<<24);
	hieth_writel(ld, reg, GLB_HOSTMAC_L32);

	local_unlock(ld);

	return 0;
}

int hieth_hw_get_macaddress(struct hieth_netdev_local *ld, unsigned char *mac)
{
	unsigned long reg;

	local_lock(ld);

	reg = hieth_readl(ld, GLB_HOSTMAC_H16);
	mac[0] = (reg>>8) & 0xff;
	mac[1] = reg & 0xff;

	reg = hieth_readl(ld, GLB_HOSTMAC_L32);
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

	hieth_writel_bits(ld, ld->depth.hw_xmitq, UD_REG_NAME(GLB_QLEN_SET), \
			BITS_TXQ_DEP);
	hieth_writel_bits(ld, HIETH_MAX_QUEUE_DEPTH - ld->depth.hw_xmitq, \
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

int hieth_xmit_release_skb(struct hieth_netdev_local *ld)
{
	int ret = 0;
	struct sk_buff *skb;

	local_lock_need(ld);

	while (hw_xmitq_cnt_inuse(ld) < ld->tx_hw_cnt) {

		hieth_assert(ld->tx_hw_cnt);

		skb = skb_dequeue(&ld->tx_hw);

		if (!skb) {
			hieth_error("hw_xmitq_cnt_inuse=%d, tx_hw_cnt=%d\n",
			(int)hw_xmitq_cnt_inuse(ld), (int)ld->tx_hw_cnt);

			BUG();
			ret = -1;
			goto error_exit;
		}

		kfree_skb(skb);

		ld->tx_hw_cnt--;
	}

error_exit:
	local_unlock_need(ld);
	return ret;
}

int hieth_xmit_real_send(struct hieth_netdev_local *ld, struct sk_buff *skb)
{
	int ret = 0;

	local_lock(ld);

	if (!_test_xmit_queue_ready(ld)) {
		hieth_error("hw xmit queue is not ready");
		ret = -1;
		goto _trans_exit;
	}

	/* for recalc CRC, 4 bytes more is needed */
	hw_xmitq_pkg(ld, virt_to_phys(skb->data), skb->len+4);

	skb_queue_tail(&ld->tx_hw, skb);

	ld->tx_hw_cnt++;

_trans_exit:
	local_unlock(ld);

	return ret;
}

int hieth_feed_hw(struct hieth_netdev_local *ld)
{
	struct sk_buff *skb;
	int cnt = 0;

	local_lock_need(ld);

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

	local_unlock_need(ld);

	return cnt;
}

int hieth_hw_recv_tryup(struct hieth_netdev_local *ld)
{
	struct sk_buff *skb;
	uint32_t rlen;
	int cnt = 0;

	local_lock_need(ld);

	while (is_recv_packet(ld)) {

		rlen = hw_get_rxpkg_len(ld);
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

		skb_queue_tail(&ld->rx_head, skb);
		cnt++;
	}

	local_unlock_need(ld);

	/* fill hardware receive queue again */
	hieth_feed_hw(ld);

	return cnt;
}

/* vim: set ts=8 sw=8 tw=78: */
