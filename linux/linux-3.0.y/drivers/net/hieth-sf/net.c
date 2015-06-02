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
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/atomic.h>
#include <asm/setup.h>

#include "hieth.h"
#include "mdio.h"
#include "mac.h"
#include "ctrl.h"
#include "glb.h"
#include "sys.h"
#include "mii.h"

static struct net_device *hieth_devs_save[2] = {NULL, NULL};

static struct sockaddr macaddr;

int eee_available;
static int __init hieth_parse_tag(const struct tag *tag)
{
	if (tag->hdr.size != 4) {
		hieth_error("tag error.");
		goto err_out;
	}

	memcpy(macaddr.sa_data, &tag->u, 6);
err_out:
	return 0;
}
__tagtable(CONFIG_HIETH_TAG, hieth_parse_tag);

static void hieth_adjust_link(struct net_device *dev)
{
	int stat = 0;
	struct hieth_netdev_local *ld = netdev_priv(dev);

	stat |= (ld->phy->link) ? HIETH_LINKED : 0;
	stat |= (ld->phy->duplex == DUPLEX_FULL) ? HIETH_DUP_FULL : 0;
	stat |= (ld->phy->speed == SPEED_100) ? HIETH_SPD_100M : 0;

	if (stat != ld->link_stat) {
		if (ld->phy->link == 0) {
			printk(KERN_INFO "***link down status changed***.\n");
			writel(0x0, ld->iobase + 0x490);
			writel(0x0, ld->iobase + 0x484);
		} else {
			writel(0x1, ld->iobase + 0x490);
#ifdef CONFIG_EEE_SUPPORT
			if (eee_available > NO_EEE) {
				int data = ld->driver->eee_enable();
				if (DEBUG)
					printk(KERN_INFO "adjust_link "
						"data:(=2?): %d\n", data);
				if (eee_available == MAC_EEE) {
					writel(0x10, ld->iobase + 0x484);
					/* enable intr */
					writel(0x1f, ld->iobase + 0x480);
				} else
					writel(0x0, ld->iobase + 0x484);

			}
#endif
		}

		hieth_set_linkstat(ld, stat);
		phy_print_status(ld->phy);
		ld->link_stat = stat;
		hieth_set_mii_mode(ld, UD_BIT_NAME(CONFIG_HIETH_MII_RMII_MODE));
	}
}

static int hieth_init_skb_buffers(struct hieth_netdev_local *ld)
{
	int i;
	struct sk_buff *skb;

	for (i = 0; i < CONFIG_HIETH_MAX_RX_POOLS; i++) {
		skb = dev_alloc_skb(SKB_SIZE);
		if (!skb)
			break;

		ld->rx_pool.sk_pool[i] = skb;
	}

	if (i < CONFIG_HIETH_MAX_RX_POOLS) {
		hieth_error("no mem");
		for (i--; i > 0; i--)
			dev_kfree_skb_any(ld->rx_pool.sk_pool[i]);
		return -ENOMEM;
	}

	ld->rx_pool.next_free_skb = 0;
	ld->stat.rx_pool_dry_times = 0;

	return 0;
}

static void hieth_destroy_skb_buffers(struct hieth_netdev_local *ld)
{
	int i;
	for (i = 0; i < CONFIG_HIETH_MAX_RX_POOLS; i++)
		dev_kfree_skb_any(ld->rx_pool.sk_pool[i]);

	ld->rx_pool.next_free_skb = 0;
	ld->stat.rx_pool_dry_times = 0;
}

struct sk_buff *hieth_platdev_alloc_skb(struct hieth_netdev_local *ld)
{
	struct sk_buff *skb;
	int i;

	skb = ld->rx_pool.sk_pool[ld->rx_pool.next_free_skb++];

	if (ld->rx_pool.next_free_skb == CONFIG_HIETH_MAX_RX_POOLS)
		ld->rx_pool.next_free_skb = 0;

	/*current skb is used by kernel or other process,find another skb*/
	if (skb_shared(skb) || (atomic_read(&(skb_shinfo(skb)->dataref)) > 1)) {

		for (i = 0; i < CONFIG_HIETH_MAX_RX_POOLS; i++) {

			skb = ld->rx_pool.sk_pool[ld->rx_pool.next_free_skb++];
			if (ld->rx_pool.next_free_skb ==
						CONFIG_HIETH_MAX_RX_POOLS)
				ld->rx_pool.next_free_skb = 0;

			if ((skb_shared(skb) == 0) &&
				(atomic_read(&(skb_shinfo(skb)->dataref)) <= 1))
				break;
		}

		if (i == CONFIG_HIETH_MAX_RX_POOLS) {
			ld->stat.rx_pool_dry_times++;
			hieth_trace(7, "%ld: no free skb\n",
				ld->stat.rx_pool_dry_times);
			return NULL;
		}
	}

	memset(skb, 0, offsetof(struct sk_buff, tail));

	skb->data = skb->tail = skb->head;

	skb->end = skb->data + (skb->truesize - sizeof(struct sk_buff));
	skb->len = 0;
	skb->data_len = 0;
	skb->cloned = 0;

	atomic_inc(&skb->users);

	return skb;
}

static void hieth_bfproc_recv(unsigned long data)
{
	int ret = 0;
	struct net_device *dev = (void *)data;
	struct hieth_netdev_local *ld = netdev_priv(dev);
	struct sk_buff *skb;

	hieth_hw_recv_tryup(ld);

	while ((skb = skb_dequeue(&ld->rx_head)) != NULL) {

		skb->protocol = eth_type_trans(skb, dev);

		if (HIETH_INVALID_RXPKG_LEN(skb->len)) {
			hieth_error("pkg len error");
			ld->stats.rx_errors++;
			ld->stats.rx_length_errors++;
			dev_kfree_skb_any(skb);
			continue;
		}

		ld->stats.rx_packets++;
		ld->stats.rx_bytes += skb->len;
		dev->last_rx = jiffies;
		skb->dev = dev;

		ret = netif_rx(skb);
		if (ret)
			ld->stats.rx_dropped++;
	}

	return;
}

static void hieth_net_isr_proc(struct hieth_netdev_local *ld, int ints)
{
	if ((ints & UD_BIT_NAME(HIETH_INT_MULTI_RXRDY)) &&
			(hieth_hw_recv_tryup(ld) > 0)) {

		tasklet_schedule(&ld->bf_recv);
	}

	if (ints & UD_BIT_NAME(HIETH_INT_TXQUE_RDY)) {

		hieth_irq_disable(ld, UD_BIT_NAME(HIETH_INT_TXQUE_RDY));
		netif_wake_queue(hieth_devs_save[ld->port]);
	}
}

#define TX_ENTRY_LPI	0x1
#define TX_LEAVE_LPI	0x2
#define RX_ENTRY_LPI	0x4
#define RX_LEAVE_LPI	0x8
#define TX_LPI_COND	0x10

static irqreturn_t hieth_net_isr(int irq, void *dev_id)
{
	int ints;
	struct hieth_netdev_local *ld;

	if (hieth_devs_save[UP_PORT])
		ld = netdev_priv(hieth_devs_save[UP_PORT]);
	else if (hieth_devs_save[DOWN_PORT])
		ld = netdev_priv(hieth_devs_save[DOWN_PORT]);
	else {
		BUG();
		return IRQ_NONE;
	}

	/*mask the all interrupt*/
	hieth_writel_bits(ld, 0, GLB_RW_IRQ_ENA, BITS_IRQS_ENA_ALLPORT);
#ifdef CONFIG_EEE_SUPPORT
	if (eee_available == MAC_EEE) {
		/* what eee intr we get? */
		eee_ints = readl(ld->iobase + 0x480);
		/* get ready for LPI */
		if (eee_ints & TX_LPI_COND) {
			/* clear eee_intr status */
			writel(0x10, ld->iobase + 0x480);
			data = readl(ld->iobase + 0x488);
			/* tx_lpi_assert enable */
			data |= 0x2;
			writel(data, ld->iobase + 0x488);   /*LPI entry*/
		}

		if (eee_ints & TX_ENTRY_LPI) {
			/* clear eee_intr status */
			writel(0x1, ld->iobase + 0x480);
		}

		if (eee_ints & TX_LEAVE_LPI) {
			/* clear eee_intr status */
			writel(0x2, ld->iobase + 0x480);
		}
	}
#endif

	ints = hieth_read_irqstatus(ld);

	if (likely(ints & BITS_IRQS_MASK_U) && hieth_devs_save[UP_PORT]) {
		hieth_net_isr_proc(netdev_priv(hieth_devs_save[UP_PORT]),
				(ints & BITS_IRQS_MASK_U));
		hieth_clear_irqstatus(ld, (ints & BITS_IRQS_MASK_U));
		ints &= ~BITS_IRQS_MASK_U;
	}

	if (likely(ints & BITS_IRQS_MASK_D) && hieth_devs_save[DOWN_PORT]) {
		hieth_net_isr_proc(
				netdev_priv(hieth_devs_save[DOWN_PORT]),
				(ints & BITS_IRQS_MASK_D));
		hieth_clear_irqstatus(ld, (ints & BITS_IRQS_MASK_D));
		ints &= ~BITS_IRQS_MASK_D;
	}

	if (unlikely(ints)) {
		hieth_error("unknown ints=0x%.8x\n", ints);
		hieth_clear_irqstatus(ld, ints);
	}

	/*unmask the all interrupt*/
	hieth_writel_bits(ld, 1, GLB_RW_IRQ_ENA, BITS_IRQS_ENA_ALLPORT);

	return IRQ_HANDLED;
}

static void hieth_monitor_func(unsigned long arg)
{
	struct net_device *dev = (struct net_device *)arg;
	struct hieth_netdev_local *ld = netdev_priv(dev);

	if (!ld || !netif_running(dev)) {
		hieth_trace(7, "network driver is stoped.");
		return;
	}

	hieth_feed_hw(ld);

	hieth_xmit_release_skb(ld);

	ld->monitor.expires =
				jiffies +
				msecs_to_jiffies(CONFIG_HIETH_MONITOR_TIMER);
	add_timer(&ld->monitor);

	return;
}

static unsigned long phy_id_read(void)
{
	unsigned long phy_id;
	unsigned short id1, id2;
	int my_phyaddr = 0x1;

	id1 = hieth_mdio_read(&hieth_mdio_local_device, my_phyaddr, 0x02);
	id2 = hieth_mdio_read(&hieth_mdio_local_device, my_phyaddr, 0x03);

	phy_id = (((id1 & 0xffff) << 16) | (id2 & 0xffff));
	return phy_id;

}

static int hieth_net_open(struct net_device *dev)
{
	struct hieth_netdev_local *ld = netdev_priv(dev);
	unsigned int data;
	unsigned long phy_id;

	try_module_get(THIS_MODULE);

	/* init tasklet */
	ld->bf_recv.next = NULL;
	ld->bf_recv.state = 0;
	ld->bf_recv.func = hieth_bfproc_recv;
	ld->bf_recv.data = (unsigned long)dev;
	atomic_set(&ld->bf_recv.count, 0);

	/* setup hardware */
	hieth_set_hwq_depth(ld);

	hieth_clear_irqstatus(ld, UD_BIT_NAME(BITS_IRQS_MASK));

	netif_carrier_off(dev);

	hieth_feed_hw(ld);

	netif_start_queue(dev);

	ld->link_stat = 0;
	phy_start(ld->phy);

	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_MULTI_RXRDY));
	hieth_writel_bits(ld, 1, GLB_RW_IRQ_ENA, UD_BIT_NAME(BITS_IRQS_ENA));
	hieth_writel_bits(ld, 1, GLB_RW_IRQ_ENA, BITS_IRQS_ENA_ALLPORT);

	init_timer(&ld->monitor);
	ld->monitor.function = hieth_monitor_func;
	ld->monitor.data = (unsigned long)dev;
	ld->monitor.expires = jiffies
		+ msecs_to_jiffies(CONFIG_HIETH_MONITOR_TIMER);
	add_timer(&ld->monitor);

	data = readl(ld->iobase + 0x210);
	data |= 0x40000000;	/* do CRC check in mac*/
	writel(data, ld->iobase + 0x210);


	phy_id = phy_id_read();

#ifdef CONFIG_EEE_SUPPORT
	hiphyinfo = phy_search_ids(phy_id);

	if (hiphyinfo) {
		eee_available = hiphyinfo->with_eee;
		if (DEBUG)
			printk(KERN_INFO "fit_phy_id : %x, phy_name: %s,"
				"eee :%x\n", (unsigned int)hiphyinfo->phy_id,
				hiphyinfo->name, (unsigned int)eee_available);
		ld->driver = hiphyinfo->driver;

		if (eee_available > NO_EEE) {
			data = 0x0;
			data_tmp = ld->driver->eee_enable();

			if (eee_available == MAC_EEE) {
				/* init EEE timer */
				/* 0x35 only for FPGA(54M)*/
				writel(0x35, ld->iobase + 0x494);
				writel(0x1e03e8, ld->iobase + 0x48c);
				writel(0xf42400, ld->iobase + 0x488);
				data = readl(ld->iobase + 0x490);
				data |= 0x2;
				writel(data, ld->iobase + 0x490);

				/* enable EEE */
				writel(0x10, ld->iobase + 0x484);
				if (DEBUG)
					printk(KERN_INFO "mac eee mode start "
							"to enable, eee :%x\n",
							eee_available);
				data = readl(ld->iobase + 0x488);
				data |= 0x1;    /* mac-eee_enable */
				writel(data, ld->iobase + 0x488);
			}

		}

	} else {
		printk(KERN_INFO "non-EEE mode\n");
		eee_available = NO_EEE;
	}
#endif
	return 0;
}

static int hieth_net_close(struct net_device *dev)
{
	struct hieth_netdev_local *ld = netdev_priv(dev);

	hieth_irq_disable(ld, UD_BIT_NAME(HIETH_INT_MULTI_RXRDY));

	phy_stop(ld->phy);

	del_timer_sync(&ld->monitor);

	/* reset and init port */
	hieth_port_reset(ld, ld->port);
	hieth_port_init(ld, ld->port);

	skb_queue_purge(&ld->rx_head);
	skb_queue_purge(&ld->rx_hw);
	skb_queue_purge(&ld->tx_hw);
	ld->tx_hw_cnt = 0;

	module_put(THIS_MODULE);

	return 0;
}

static void hieth_net_timeout(struct net_device *dev)
{
	hieth_error("tx timeout");
}

static int hieth_net_hard_start_xmit(
		struct sk_buff *skb,
		struct net_device *dev)
{
	int ret;
	struct hieth_netdev_local *ld = netdev_priv(dev);
	int tx_lpi_assert = 0x2;
	unsigned int data;
	data = readl(ld->iobase + 0x488);

	data &= ~tx_lpi_assert;
	writel(data, ld->iobase + 0x488);

	hieth_xmit_release_skb(ld);

	dma_map_single(ld->dev, skb->data, skb->len, DMA_TO_DEVICE);

	ret = hieth_xmit_real_send(ld, skb);
	if (ret < 0) {
		ld->stats.tx_dropped++;
		hieth_error("tx bug, drop packet");
		BUG();
		return NETDEV_TX_BUSY;
	}

	dev->trans_start = jiffies;

	ld->stats.tx_packets++;
	ld->stats.tx_bytes += skb->len;

	hieth_clear_irqstatus(ld, UD_BIT_NAME(HIETH_INT_TXQUE_RDY));
	if (!hieth_hw_xmitq_ready(ld)) {
		netif_stop_queue(dev);
		hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_TXQUE_RDY));
	}

	return NETDEV_TX_OK;
}

static struct net_device_stats *hieth_net_get_stats(struct net_device *dev)
{
	struct hieth_netdev_local *ld = netdev_priv(dev);

	return &ld->stats;
}

static int hieth_net_set_mac_address(struct net_device *dev, void *p)
{
	struct hieth_netdev_local *ld = netdev_priv(dev);
	struct sockaddr *skaddr = p;

	if (netif_running(dev))
		return -EBUSY;
	if (!is_valid_ether_addr(skaddr->sa_data))
		return -EADDRNOTAVAIL;

	local_lock(ld);

	if (hieth_devs_save[UP_PORT])
		memcpy(hieth_devs_save[UP_PORT]->dev_addr,
				skaddr->sa_data, dev->addr_len);
	if (hieth_devs_save[DOWN_PORT])
		memcpy(hieth_devs_save[DOWN_PORT]->dev_addr,
				skaddr->sa_data, dev->addr_len);

	local_unlock(ld);

	hieth_hw_set_macaddress(ld, 1, dev->dev_addr);

	return 0;
}

static void print_mac_address(
		const char *pre_msg,
		const unsigned char *mac,
		const char *post_msg)
{
	int i;

	if (pre_msg)
		printk(pre_msg);

	for (i = 0; i < 6; i++)
		printk("%02X%s", mac[i], i == 5 ? "" : ":");

	if (post_msg)
		printk(post_msg);
}

static int hieth_net_ioctl(struct net_device *net_dev, \
		struct ifreq *ifreq, int cmd)
{
	struct hieth_netdev_local *ld = netdev_priv(net_dev);

	if (!netif_running(net_dev))
		return -EINVAL;

	if (!ld->phy)
		return -EINVAL;

	return phy_mii_ioctl(ld->phy, ifreq, cmd);
}

static void hieth_ethtools_get_drvinfo(struct net_device *net_dev, \
		struct ethtool_drvinfo *info)
{
	strcpy(info->driver, "hieth driver");
	strcpy(info->version, "hieth sfv200/sfv300");
	strcpy(info->bus_info, "platform");
}

static u32 hieth_ethtools_get_link(struct net_device *net_dev)
{
	u32 ret = 0;
	struct hieth_netdev_local *ld = netdev_priv(net_dev);
	ret = (ld->phy->link) ? HIETH_LINKED : 0;
	return ret;
}

static int hieth_ethtools_get_settings(struct net_device *net_dev, \
		struct ethtool_cmd *cmd)
{
	struct hieth_netdev_local *ld = netdev_priv(net_dev);

	if (ld->phy)
		return phy_ethtool_gset(ld->phy, cmd);

	return -EINVAL;
}

static int hieth_ethtools_set_settings(struct net_device *net_dev, \
		struct ethtool_cmd *cmd)
{
	struct hieth_netdev_local *ld = netdev_priv(net_dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (ld->phy)
		return phy_ethtool_sset(ld->phy, cmd);

	return -EINVAL;
}

static struct ethtool_ops hieth_ethtools_ops = {
	.get_drvinfo		= hieth_ethtools_get_drvinfo,
	.get_link		= hieth_ethtools_get_link,
	.get_settings		= hieth_ethtools_get_settings,
	.set_settings		= hieth_ethtools_set_settings,
};

static const struct net_device_ops hieth_netdev_ops = {
	.ndo_open		= hieth_net_open,
	.ndo_stop		= hieth_net_close,
	.ndo_start_xmit		= hieth_net_hard_start_xmit,
	.ndo_tx_timeout		= hieth_net_timeout,
	.ndo_do_ioctl		= hieth_net_ioctl,
	.ndo_set_mac_address	= hieth_net_set_mac_address,
	.ndo_get_stats		= hieth_net_get_stats,
};

static int hieth_platdev_probe_port(struct platform_device *pdev, int port)
{
	int ret = -1;
	struct net_device *netdev = NULL;
	struct hieth_netdev_local *ld;

	if ((UP_PORT != port) && (DOWN_PORT != port)) {
		hieth_error("port error!");
		ret = -ENODEV;
		goto _error_exit;
	}

	netdev = alloc_etherdev(sizeof(*ld));
	if (netdev == NULL) {
		hieth_error("alloc_etherdev fail!");
		ret = -ENOMEM;
		goto _error_alloc_etherdev;
	}
	SET_NETDEV_DEV(netdev, &pdev->dev);
	netdev->irq = CONFIG_HIETH_IRQNUM;
	netdev->watchdog_timeo	= 3*HZ;
	netdev->netdev_ops	= &hieth_netdev_ops;
	netdev->ethtool_ops	= &hieth_ethtools_ops;

	/* init hieth_global somethings... */
	hieth_devs_save[port] = netdev;

	/* init hieth_local_driver */
	ld = netdev_priv(netdev);
	memset(ld, 0, sizeof(*ld));

	local_lock_init(ld);

	ld->iobase = (unsigned long)ioremap_nocache(CONFIG_HIETH_IOBASE, \
					CONFIG_HIETH_IOSIZE);
	if (!ld->iobase) {
		hieth_error("ioremap_nocache err, base=0x%.8x, size=0x%.8x\n",
				CONFIG_HIETH_IOBASE, CONFIG_HIETH_IOSIZE);
		ret = -EFAULT;
		goto _error_ioremap_nocache;
	}
	ld->iobase_phys = CONFIG_HIETH_IOBASE;

	ld->port = port;

	ld->dev = &(pdev->dev);

	/* reset and init port */
	hieth_port_reset(ld, ld->port);
	hieth_port_init(ld, ld->port);

	ld->depth.hw_xmitq = CONFIG_HIETH_HWQ_XMIT_DEPTH;

	memset(ld->phy_name, 0, sizeof(ld->phy_name));
	snprintf(ld->phy_name, MII_BUS_ID_SIZE, PHY_ID_FMT, \
		HIETH_MIIBUS_NAME, UD_PHY_NAME(CONFIG_HIETH_PHYID));

	ld->phy = phy_connect(netdev, ld->phy_name, hieth_adjust_link, 0, \
			UD_BIT_NAME(CONFIG_HIETH_MII_RMII_MODE) ? \
			PHY_INTERFACE_MODE_RMII : PHY_INTERFACE_MODE_MII);
	if (IS_ERR(ld->phy)) {
		hieth_error("connect to phy_device %s failed!", ld->phy_name);
		ld->phy = NULL;
		goto _error_phy_connect;
	}

	set_phy_valtage();

	skb_queue_head_init(&ld->rx_head);
	skb_queue_head_init(&ld->rx_hw);
	skb_queue_head_init(&ld->tx_hw);
	ld->tx_hw_cnt = 0;

	ret = hieth_init_skb_buffers(ld);
	if (ret) {
		hieth_error("hieth_init_skb_buffers failed!");
		goto _error_init_skb_buffers;
	}

	ret = register_netdev(netdev);
	if (ret) {
		hieth_error("register_netdev %s failed!", netdev->name);
		goto _error_register_netdev;
	}

	return ret;

_error_register_netdev:
	hieth_destroy_skb_buffers(ld);

_error_init_skb_buffers:
	phy_disconnect(ld->phy);
	ld->phy = NULL;

_error_phy_connect:
	iounmap((void *)ld->iobase);

_error_ioremap_nocache:
	local_lock_exit();
	hieth_devs_save[port] = NULL;
	free_netdev(netdev);

_error_alloc_etherdev:

_error_exit:
	return ret;
}

static int hieth_platdev_remove_port(struct platform_device *pdev, int port)
{
	struct net_device *ndev;
	struct hieth_netdev_local *ld;

	ndev = hieth_devs_save[port];

	if (!ndev)
		goto _ndev_exit;

	ld = netdev_priv(ndev);

	unregister_netdev(ndev);
	hieth_destroy_skb_buffers(ld);

	phy_disconnect(ld->phy);
	ld->phy = NULL;

	iounmap((void *)ld->iobase);

	local_lock_exit();

	hieth_devs_save[port] = NULL;
	free_netdev(ndev);

_ndev_exit:
	return 0;
}

static void phy_quirk(struct hieth_mdio_local *mdio, int phyaddr)
{
	unsigned long phy_id;
	unsigned short id1, id2;
	unsigned short reg;

	id1 = hieth_mdio_read(mdio, phyaddr, 0x02);
	id2 = hieth_mdio_read(mdio, phyaddr, 0x03);

	phy_id = (((id1 & 0xffff) << 16) | (id2 & 0xffff));
	if (CONFIG_HIETH_MII_RMII_MODE_U == 1) {
		/* PHY-KSZ8051RNL */
		if ((phy_id & 0xFFFFFFF0) == 0x221550) {
			reg = hieth_mdio_read(mdio, phyaddr, 0x1F);
			reg |= (1 << 7); /* set phy RMII 50MHz clk;*/
			hieth_mdio_write(mdio, phyaddr, 0x1F, reg);

			reg = hieth_mdio_read(mdio, phyaddr, 0x16);
			reg |= (1 << 1); /* set phy RMII override; */
			hieth_mdio_write(mdio, phyaddr, 0x16, reg);
		}
	}

}
static int hieth_plat_driver_probe(struct platform_device *pdev)
{
	int ret = -1;
	struct net_device *ndev = NULL;

	memset(hieth_devs_save, 0, sizeof(hieth_devs_save));

	hieth_sys_init();

	if (hieth_mdiobus_driver_init(pdev)) {
		hieth_error("mdio bus init error!\n");
		ret = -ENODEV;
		goto _error_mdiobus_driver_init;
	}

	hieth_platdev_probe_port(pdev, UP_PORT);
#ifdef CONFIG_HIETH_DOWNPORT_EN
	hieth_platdev_probe_port(pdev, DOWN_PORT);
#endif

	phy_quirk(&hieth_mdio_local_device, CONFIG_HIETH_PHYID_U);
	phy_quirk(&hieth_mdio_local_device, CONFIG_HIETH_PHYID_D);

	if (hieth_devs_save[UP_PORT])
		ndev = hieth_devs_save[UP_PORT];
	else if (hieth_devs_save[DOWN_PORT])
		ndev = hieth_devs_save[DOWN_PORT];

	if (!ndev) {
		hieth_error("no dev probed!\n");
		ret = -ENODEV;
		goto _error_nodev_exit;
	}

	if (!is_valid_ether_addr(macaddr.sa_data)) {
		print_mac_address(KERN_WARNING "Invalid HW-MAC Address: ", \
				macaddr.sa_data, "\n");
		random_ether_addr(macaddr.sa_data);
		print_mac_address(KERN_WARNING "Set Random MAC address: ", \
				macaddr.sa_data, "\n");
	}
	hieth_net_set_mac_address(ndev, (void *)&macaddr);

	ret = request_irq(CONFIG_HIETH_IRQNUM, hieth_net_isr, IRQF_SHARED, \
				"hieth", hieth_devs_save);
	if (ret) {
		hieth_error("request_irq %d failed!", CONFIG_HIETH_IRQNUM);
		goto _error_request_irq;
	}

	return ret;

_error_request_irq:
	hieth_platdev_remove_port(pdev, UP_PORT);
	hieth_platdev_remove_port(pdev, DOWN_PORT);

_error_nodev_exit:
	hieth_mdiobus_driver_exit();

_error_mdiobus_driver_init:
	hieth_sys_exit();

	return ret;
}

static int hieth_plat_driver_remove(struct platform_device *pdev)
{
	hieth_assert(hieth_devs_save[UP_PORT] || hieth_devs_save[DOWN_PORT]);

	free_irq(CONFIG_HIETH_IRQNUM, hieth_devs_save);

	hieth_platdev_remove_port(pdev, UP_PORT);
	hieth_platdev_remove_port(pdev, DOWN_PORT);

	hieth_mdiobus_driver_exit();

	hieth_sys_exit();

	memset(hieth_devs_save, 0, sizeof(hieth_devs_save));

	return 0;
}

#ifdef CONFIG_PM
static int hieth_plat_driver_suspend_port(struct platform_device *pdev, \
		pm_message_t state,
		int port)
{
	struct net_device *ndev = hieth_devs_save[port];

	if (ndev) {
		hieth_net_close(ndev);
		netif_device_detach(ndev);
	}

	return 0;
}

static int hieth_plat_driver_suspend(struct platform_device *pdev, \
		pm_message_t state)
{
	hieth_plat_driver_suspend_port(pdev, state, UP_PORT);
	hieth_plat_driver_suspend_port(pdev, state, DOWN_PORT);

	hieth_sys_suspend();

	return 0;
}

static int hieth_plat_driver_resume_port(struct platform_device *pdev, \
		int port)
{
	struct hieth_netdev_local *ld;
	struct net_device *ndev = hieth_devs_save[port];

	if (ndev) {
		ld = netdev_priv(ndev);

		/* restore local host mac */
		hieth_hw_set_macaddress(ld, 1, ndev->dev_addr);

		/* reset and init port */
		hieth_port_reset(ld, ld->port);
		hieth_port_init(ld, ld->port);

		netif_device_attach(ndev);
		hieth_net_open(ndev);
	}

	return 0;
}

static int hieth_plat_driver_resume(struct platform_device *pdev)
{
	hieth_sys_resume();

	phy_quirk(&hieth_mdio_local_device, CONFIG_HIETH_PHYID_U);
	phy_quirk(&hieth_mdio_local_device, CONFIG_HIETH_PHYID_D);

	hieth_plat_driver_resume_port(pdev, UP_PORT);
	hieth_plat_driver_resume_port(pdev, DOWN_PORT);

	return 0;
}
#else
#define hieth_plat_driver_suspend	NULL
#define hieth_plat_driver_resume	NULL
#endif

static struct platform_driver hieth_platform_driver = {
	.probe		= hieth_plat_driver_probe,
	.remove		= hieth_plat_driver_remove,
	.suspend	= hieth_plat_driver_suspend,
	.resume		= hieth_plat_driver_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= HIETH_DRIVER_NAME,
		.bus	= &platform_bus_type,
	},
};

static struct resource hieth_resources[] = {
	[0] = {
		.start	= CONFIG_HIETH_IOBASE,
		.end	= CONFIG_HIETH_IOBASE + CONFIG_HIETH_IOSIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= CONFIG_HIETH_IRQNUM,
		.end	= CONFIG_HIETH_IRQNUM,
		.flags	= IORESOURCE_IRQ,
	}
};

static void hieth_platform_device_release(struct device *dev)
{
}

static struct platform_device hieth_platform_device = {
	.name = HIETH_DRIVER_NAME,
	.id   = 0,
	.dev = {
		.platform_data	= NULL,
		.dma_mask = (u64 *)~0,
		.coherent_dma_mask = (u64)~0,
		.release = hieth_platform_device_release,
	},
	.num_resources = ARRAY_SIZE(hieth_resources),
	.resource = hieth_resources,
};

static int hieth_init(void)
{
	int ret = 0;

	ret = platform_device_register(&hieth_platform_device);
	if (ret) {
		hieth_error("register platform device failed!");
		goto _error_register_device;
	}

	ret = platform_driver_register(&hieth_platform_driver);
	if (ret) {
		hieth_error("register platform driver failed!");
		goto _error_register_driver;
	}

	return ret;

_error_register_driver:
	platform_device_unregister(&hieth_platform_device);

_error_register_device:

	return -1;
}

static void hieth_exit(void)
{
	platform_driver_unregister(&hieth_platform_driver);

	platform_device_unregister(&hieth_platform_device);
}

module_init(hieth_init);
module_exit(hieth_exit);

MODULE_DESCRIPTION("Hisilicon ETH driver whith MDIO support");
MODULE_LICENSE("GPL");

/* vim: set ts=8 sw=8 tw=78: */
