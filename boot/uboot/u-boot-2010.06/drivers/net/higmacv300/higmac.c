#include "util.h"
#include "higmac.h"
#include "ctrl.h"
#include "mdio.h"

/* suppose higmac_board_info[i] was initialed! */
#define for_each_gmac_netdev_local_priv(ld, i)	\
	for (i = 0; i < CONFIG_GMAC_NUMS &&	\
			({ld = &higmac_board_info[i].higmac_ld; 1; });	\
			i++)

/* this driver only support GMAC_NUMS = 2 or 1 */
#define MDIO_BUS_NUMS				(2)
struct higmac_board_info higmac_board_info[] = { /* MAX_GMAC_NUMS */
	{
		{
			.index		= 0,
			.iobase		= HIGMAC0_IOBASE,
			.iobase_phys	= HIGMAC0_IOBASE,
		},
		.mii_name		= "mdio0",
		.phy_intf		= CONFIG_HIGMAC_PHY1_INTERFACE_MODE,
		.phy_addr		= CONFIG_HIGMAC_PHY1_ADDR,
	},
#if (CONFIG_GMAC_NUMS == 2)
	{
		{
			.index		= 1,
			.iobase		= HIGMAC1_IOBASE,
			.iobase_phys	= HIGMAC1_IOBASE,
		},
		.mii_name		= "mdio1",
		.phy_intf		= CONFIG_HIGMAC_PHY2_INTERFACE_MODE,
		.phy_addr		= CONFIG_HIGMAC_PHY2_ADDR,
	}
#endif
};

static struct higmac_netdev_local *current_ld ;
static int phy_link_times;
static int gmac_debug;

static char *phy_intf_str[interface_mode_butt] = {
	"mii",
	"rmii",
	"rgmii",
};
static char *mdio_bus[MDIO_BUS_NUMS] = {
	"mdio0",
	"mdio1"
};

#if defined(CONFIG_MACH_GODEYES)
/*
 *  * 3516(godeyes) port mode def.
 *   * port_mode_butt means not support this mode.
 *    */
static int g_speed_portmode_table[speed_mode_butt][interface_mode_butt] = {
	{port_mode_10_mii,      port_mode_butt,         port_mode_10_rgmii},
	{port_mode_100_mii,     port_mode_butt,         port_mode_100_rgmii},
	{port_mode_butt,        port_mode_butt,         port_mode_1000_rgmii}
};
#else
static int g_speed_portmode_table[speed_mode_butt][interface_mode_butt] = {
	{port_mode_10_mii,      port_mode_10_rmii,      port_mode_10_rgmii},
	{port_mode_100_mii,     port_mode_100_rmii,     port_mode_100_rgmii},
	{port_mode_butt,        port_mode_butt,         port_mode_1000_rgmii}
};
#endif

static int calculate_port_mode(enum speed_mode speed, enum if_mode if_mode,
		int is_duplex_half)
{
	int ret = port_mode_butt ;

	if (speed < speed_mode_butt && if_mode < interface_mode_butt) {
		ret = g_speed_portmode_table[speed][if_mode];
		if (is_duplex_half)
			ret &= ~(1 << 4); /* see mac_if reg def. */
		return ret;
	}

	printf("Invalid speed(%d) & interface(%d) mode.\n", speed, if_mode);
	printf("Please assign which mode our eth will correctly work at.\n"
			"It may support 10M/100M MII, 10M/100M RMII, "
			"10M/100M/1000M RGMII interface.\n"
			"eg. if your board have two NICs, connecting each phy using "
			"mii and rgmii interface, you can use the module param "
			"'port_mode=mii,rgmii' to tell the driver.\n");
	BUG();

	/* can't reach here */
	return -1;
}

/* module parameter
 * 1) phy_intf=mii,rgmii if eth0=mii, eth1=rgmii
 *	phy_intf=mii if eth0=mii, eth1=mii;
 * 2) use_internal_phy=1 if eth1 use internal phy;
 * 3) phy0_addr=1, phy1_addr=2
 * 4) gmac_debug=1
 */
static void parse_module_parameters(void)
{
	enum if_mode if_mode;
	unsigned long tmp;
	int gmac = 0;
	char *s, *e;

	s = getenv("gmac_debug");
	if (s)
		gmac_debug = simple_strtoul(s, 0, 0);/* debug on */
	else
		gmac_debug = 0;

	s = getenv("phy_link_times");
	if (s)
		phy_link_times = simple_strtoul(s, 0, 0);
	else
		phy_link_times = DEFAULT_PHY_LINK_TIMES;

	/* phy_intf = {mii | rgmii | rmii} [,{mii | rgmii | rmii}]*/
	s = getenv("mdio_intf");
next_mii:
	if (s) {
		while (*s == ' ' || *s == ',')
			s++;

		if (!strncmp(s, "mii", 3))
				if_mode = interface_mode_mii;
		else if (!strncmp(s, "rmii", 4))
				if_mode = interface_mode_rmii;
		else if (!strncmp(s, "rgmii", 5))
				if_mode = interface_mode_rgmii;
		else {
			printf("Invalid phy_intf(mii, rmii, or rgmii)." \
					"Set ETH%d default to mii.\n", gmac);
			if_mode = interface_mode_mii;
		}

		if (!gmac) {/* first time */
			gmac = 1;
			higmac_board_info[0].phy_intf = if_mode;
			/* in case phy_intf=mii */
#if (CONFIG_GMAC_NUMS == 2)
			higmac_board_info[1].phy_intf = if_mode;
#endif
			s = strchr(s, ',');
			goto next_mii;
		} else
#if (CONFIG_GMAC_NUMS == 2)
			higmac_board_info[1].phy_intf = if_mode;
#else
			;
#endif
	}
#if (CONFIG_GMAC_NUMS == 2)
	/* use_mdio=0 or use_mdio=1 or use_mdio=0,1 or ... */
	gmac = 0;
	s = getenv("use_mdio");
next_mdio:
	if (s) {
		while (*s == ' ' || *s == ',')
			s++;

		tmp = simple_strtoul(s, &e, 10);
		if (tmp >= MDIO_BUS_NUMS) {
			printf("Invalid use_mdio, Set GMAC%d use mdio%d.\n",
					gmac, gmac);
			tmp = gmac;
		}

		if (!gmac) {/* first time */
			higmac_board_info[0].mii_name = mdio_bus[tmp];
			higmac_board_info[1].mii_name = mdio_bus[tmp];
			gmac++;
			s = e;
			s = strchr(s, ',');
			goto next_mdio;
		} else
			higmac_board_info[1].mii_name = mdio_bus[tmp];
	}
#endif

	/* get phy addr */
	gmac = 0;
	s = getenv("phy_addr");
next_phyaddr:
	if (s) {
		while (*s == ' ' || *s == ',')
			s++;

		tmp = simple_strtoul(s, &e, 10);
		if (tmp == 0)
			printf("Dectected gmac%d phyaddr set to 0, " \
					"is it right?\n", gmac);
		else if (tmp >= 32) {
			printf("Dectected gmac%d phyaddr " \
					"set to val(%lu) >= 32." \
					"This may not correct, " \
					"use default phyaddr=%u\n",
					gmac, tmp,
					higmac_board_info[gmac].phy_addr);

			tmp = higmac_board_info[gmac].phy_addr;
		}
		if (!gmac) {/* gmac0 */
			higmac_board_info[gmac].phy_addr = tmp;
			gmac++;
			s = e;
			s = strchr(s, ',');
			goto next_phyaddr;
		} else /* gmac1 */
#if (CONFIG_GMAC_NUMS == 2)
			higmac_board_info[gmac].phy_addr = tmp;
#else
			;
#endif
	}

	if (gmac_debug) {
		printf("phy_link_times=%d\n", phy_link_times);
		for (gmac = 0; gmac < CONFIG_GMAC_NUMS; gmac++) {
			printf("ETH%d: port_mode=%s, " \
				"mii_name=%s, phy_addr=%d\n",
				gmac,
				phy_intf_str[higmac_board_info[gmac].phy_intf],
				higmac_board_info[gmac].mii_name,
				higmac_board_info[gmac].phy_addr);
		}
		printf("\n");
	}
}

static unsigned long rstate = 1;
/* trivial congruential random generators. from glibc. */
void srandom(unsigned long seed)
{
	rstate = seed ? seed : 1;  /* dont allow a 0 seed */
}

unsigned long random(void)
{
	unsigned int next = rstate;
	int result;

	next *= 1103515245;
	next += 12345;
	result = (unsigned int) (next / 65536) % 2048;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int) (next / 65536) % 1024;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int) (next / 65536) % 1024;

	rstate = next;

	return result;
}

void random_ether_addr(unsigned char *mac)
{
	unsigned long ethaddr_low, ethaddr_high;

	srandom(get_timer(0));
	/*
	 * setting the 2nd LSB in the most significant byte of
	 * the address makes it a locally administered ethernet
	 * address
	 */
	ethaddr_high = (random() & 0xfeff) | 0x0200;
	ethaddr_low = random();

	mac[0] = ethaddr_high >> 8;
	mac[1] = ethaddr_high & 0xff;
	mac[2] = ethaddr_low >> 24;
	mac[3] = (ethaddr_low >> 16) & 0xff;
	mac[4] = (ethaddr_low >> 8) & 0xff;
	mac[5] = ethaddr_low & 0xff;

	mac[0] &= 0xfe;    /* clear multicast bit */
	mac[0] |= 0x02;    /* set local assignment bit (IEEE802) */

}

static int higmac_net_set_mac_address(struct higmac_netdev_local *ld)
{
	unsigned char mac[MAC_LEN];
	int ret;

	ret = eth_getenv_enetaddr("ethaddr", mac);
	if (!ret) {
		unsigned char ethaddr[20];

		random_ether_addr(mac);
		sprintf((char *)ethaddr, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

		setenv("ethaddr", (char *)ethaddr);
	}

	higmac_hw_set_macaddress(ld, mac);
	print_mac(mac);

	return 0;
}

static int higmac_init_hw_desc_queue(struct higmac_netdev_local *ld)
{
	higmac_desc *queue_phy_addr = NULL;
	int queue_count = 0;

	/* init rx fq */
	queue_count = HIGMAC_HWQ_RX_FQ_DEPTH;

	queue_phy_addr
		= (higmac_desc *)malloc(queue_count * sizeof(higmac_desc));
	if (queue_phy_addr == NULL) {
		printf("alloc rx fq error!\n");
		goto _error_alloc_rx_fq;
	}

	memset((void *)queue_phy_addr, 0, queue_count * sizeof(higmac_desc));
	ld->rx_fq_addr = queue_phy_addr;
	higmac_set_rx_fq_hwq_addr(ld, (unsigned int)queue_phy_addr);

	/* init rx bq */
	queue_count = HIGMAC_HWQ_RX_BQ_DEPTH;

	queue_phy_addr
		= (higmac_desc *)malloc(queue_count * sizeof(higmac_desc));
	if (queue_phy_addr == NULL) {
		printf("alloc rx bq error!\n");
		goto _error_alloc_rx_bq;
	}

	memset((void *)queue_phy_addr, 0, queue_count * sizeof(higmac_desc));
	ld->rx_bq_addr = queue_phy_addr;
	higmac_set_rx_bq_hwq_addr(ld, (unsigned int)queue_phy_addr);

	/* init tx bq */
	queue_count = HIGMAC_HWQ_TX_BQ_DEPTH;

	queue_phy_addr
		= (higmac_desc *)malloc(queue_count * sizeof(higmac_desc));
	if (queue_phy_addr == NULL) {
		printf("alloc tx bq error!\n");
		goto _error_alloc_tx_bq;
	}

	memset((void *)queue_phy_addr, 0, queue_count * sizeof(higmac_desc));
	ld->tx_bq_addr = queue_phy_addr;
	higmac_set_tx_bq_hwq_addr(ld, (unsigned int)queue_phy_addr);

	/* init tx rq */
	queue_count = HIGMAC_HWQ_TX_RQ_DEPTH;

	queue_phy_addr
		= (higmac_desc *)malloc(queue_count * sizeof(higmac_desc));
	if (queue_phy_addr == NULL) {
		printf("alloc tx rq error!\n");
		goto _error_alloc_tx_rq;
	}

	memset((void *)queue_phy_addr, 0, queue_count * sizeof(higmac_desc));
	ld->tx_rq_addr = queue_phy_addr;
	higmac_set_tx_rq_hwq_addr(ld, (unsigned int)queue_phy_addr);

	return 0;

_error_alloc_tx_rq:
	free(ld->tx_bq_addr);

_error_alloc_tx_bq:
	free(ld->rx_bq_addr);

_error_alloc_rx_bq:
	free(ld->rx_fq_addr);

_error_alloc_rx_fq:
	return -1;
}

#if 0
static void higmac_destroy_hw_desc_queue(struct higmac_netdev_local *ld)
{
	free(ld->rx_fq_addr);
	free(ld->rx_bq_addr);
	free(ld->tx_bq_addr);
	free(ld->tx_rq_addr);

	ld->rx_fq_addr = NULL;
	ld->rx_bq_addr = NULL;
	ld->tx_bq_addr = NULL;
	ld->tx_rq_addr = NULL;
}
#endif

static int higmac_net_adjust_link(struct higmac_netdev_local *ld)
{
	char *mii_name = higmac_board_info[ld->index].mii_name;
	int phy_addr = higmac_board_info[ld->index].phy_addr;
	int stat = 0, speed = 0, is_duplex_half = 1, port_mode, phy_duplex;
	enum speed_mode speed_mode;

	stat |= miiphy_link(mii_name, phy_addr) ? HIGMAC_LINKED : 0;

	phy_duplex = miiphy_duplex(mii_name, phy_addr);
	if (phy_duplex == FULL) {
		stat |= HIGMAC_DUP_FULL;
		is_duplex_half = 0;
	}

	speed = miiphy_speed(mii_name, phy_addr);
	switch (speed) {
	case _10BASET:
		stat |= HIGMAC_SPD_10M;
		speed_mode = speed_mode_10M;
		break;
	default:
		printf("wired, phy speed!\n");
	case _100BASET:
		stat |= HIGMAC_SPD_100M;
		speed_mode = speed_mode_100M;
		break;
	case _1000BASET:
		stat |= HIGMAC_SPD_1000M;
		speed_mode = speed_mode_1000M;
		break;
	}

	if (ld->link_stat != stat) {
		if (stat & HIGMAC_LINKED) {
			port_mode = calculate_port_mode(speed_mode,
				higmac_board_info[ld->index].phy_intf,
				is_duplex_half);

			higmac_set_macif(ld, port_mode, speed_mode);

			if (is_duplex_half)
				higmac_writel(ld, 0x0, MAC_DUPLEX_HALF_CTRL);
			else
				higmac_writel(ld, 0x1, MAC_DUPLEX_HALF_CTRL);

		}
		ld->link_stat = stat;
	}

	return stat;
}

static int select_current_linked_phy(void)
{
	int count = phy_link_times ? : DEFAULT_PHY_LINK_TIMES;
	struct higmac_netdev_local *ld;
	int status, i;

	for (; count > 0; count--) {
		if (ctrlc()) {
			puts("\nAbort\n");
			goto link_failed;
		}

		for (i = 0; i < CONFIG_GMAC_NUMS; i++) {
			ld = &higmac_board_info[i].higmac_ld;

			status = higmac_net_adjust_link(ld);
			if (status & HIGMAC_LINKED)
				goto link_on;
		}
		udelay(1);
	}

link_failed:
	for (i = 0; i < CONFIG_GMAC_NUMS; i++) {
		ld = &higmac_board_info[i].higmac_ld;
		printf("ETH%d: PHY(phyaddr=%d, %s) not link!\n",
			i, higmac_board_info[i].phy_addr,
			phy_intf_str[higmac_board_info[i].phy_intf]);
	}

	return -1;

link_on:
	printf("ETH%d: PHY(phyaddr=%d, %s) link UP: DUPLEX=%s : SPEED=%s\n",
			ld->index, higmac_board_info[ld->index].phy_addr,
			phy_intf_str[higmac_board_info[ld->index].phy_intf],
			(status & HIGMAC_DUP_FULL) ? "FULL" : "HALF",
			(status & HIGMAC_SPD_100M) ? "100M" :
			((status & HIGMAC_SPD_10M) ? "10M" : "1000M"));

	higmac_net_set_mac_address(ld);
	miiphy_set_current_dev(higmac_board_info[ld->index].mii_name);
	current_ld = ld;

	return 0;
}

int eth_rx(void)
{
	struct higmac_netdev_local *ld = current_ld;
	int timeout_us = 100000;
	int rx_fq_wr_offset = 0;
	int rx_fq_rd_offset = 0;
	int rx_bq_wr_offset = 0;
	int rx_bq_rd_offset = 0;
	int len = 0;
	int wr_rd_dist;
	int i;
	higmac_desc *rx_fq_desc = ld->rx_fq_addr;
	higmac_desc *rx_bq_desc = ld->rx_bq_addr;

	rx_fq_wr_offset = higmac_readl_bits(ld, RX_FQ_WR_ADDR,
				BITS_RX_FQ_WR_ADDR);
	rx_fq_rd_offset = higmac_readl_bits(ld, RX_FQ_RD_ADDR,
				BITS_RX_FQ_RD_ADDR);

	if (rx_fq_wr_offset >= rx_fq_rd_offset)
		wr_rd_dist = (HIGMAC_HWQ_RX_FQ_DEPTH << DESC_BYTE_SHIFT)
			- (rx_fq_wr_offset - rx_fq_rd_offset);
	else
		wr_rd_dist = rx_fq_rd_offset - rx_fq_wr_offset;

	/* offset was counted on bytes, desc size = 2^5 */
	wr_rd_dist >>= DESC_BYTE_SHIFT;

	/* wr_rd_dist - 1 for logic reason.
	 * Logic think the desc pool is full filled, ...?
	 */
	for (i = 0; i < wr_rd_dist - 1; i++) {
		rx_fq_desc = ld->rx_fq_addr +
				(rx_fq_wr_offset >> DESC_BYTE_SHIFT);
		rx_fq_desc->data_buff_addr
			= (unsigned int)malloc(HIETH_BUFFER_SIZE);
		if (rx_fq_desc->data_buff_addr == (unsigned int)NULL)
			break;
		rx_fq_desc->descvid = DESC_VLD_FREE;
		rx_fq_desc->buffer_len = (HIETH_MAX_FRAME_SIZE - 1);
		rx_fq_desc->data_len = 8;

		rx_fq_wr_offset += DESC_SIZE;
		if (rx_fq_wr_offset >=
			(HIGMAC_HWQ_RX_FQ_DEPTH << DESC_BYTE_SHIFT))
			rx_fq_wr_offset = 0;
		higmac_writel_bits(ld, rx_fq_wr_offset, RX_FQ_WR_ADDR,
			BITS_RX_FQ_WR_ADDR);
	}

	/* enable Rx */
	/*
	   higmac_desc_enable(ld, RX_OUTCFF_WR_DESC_ENA|RX_CFF_RD_DESC_ENA);
	   higmac_writel_bits(ld, 1, PORT_EN, BITS_RX_EN);
	*/
	rx_bq_wr_offset = higmac_readl_bits(ld, RX_BQ_WR_ADDR,
				BITS_RX_BQ_WR_ADDR);
	rx_bq_rd_offset = higmac_readl_bits(ld, RX_BQ_RD_ADDR,
				BITS_RX_BQ_RD_ADDR);
	rx_bq_desc += (rx_bq_rd_offset >> DESC_BYTE_SHIFT);

	while (--timeout_us && (rx_bq_wr_offset == rx_bq_rd_offset)) {
		udelay(1);
		rx_bq_wr_offset = higmac_readl_bits(ld,
				RX_BQ_WR_ADDR, BITS_RX_BQ_WR_ADDR);
	}

	if (!timeout_us) {
		/* printf("rx time out!\n"); */
		return -1;
	}

	rx_bq_rd_offset += DESC_SIZE;
	if (rx_bq_rd_offset >= (HIGMAC_HWQ_RX_BQ_DEPTH << DESC_BYTE_SHIFT))
		rx_bq_rd_offset = 0;

	len = rx_bq_desc->data_len;
	if (HIGMAC_INVALID_RXPKG_LEN(len)) {
		higmac_writel_bits(ld, rx_bq_rd_offset, RX_BQ_RD_ADDR,
			BITS_RX_BQ_RD_ADDR);
		free((void *)rx_bq_desc->data_buff_addr);
		return -1;
	}
	if (gmac_debug)
		printf("got packet!\n");

	memcpy((void *)NetRxPackets[0],
			(const void *)rx_bq_desc->data_buff_addr, len);
	free((void *)rx_bq_desc->data_buff_addr);

	higmac_writel_bits(ld, rx_bq_rd_offset, RX_BQ_RD_ADDR,
		BITS_RX_BQ_RD_ADDR);

	/*NetRecive*/
	NetReceive(NetRxPackets[0], len);

	/* disable Rx */
	/*
	   higmac_writel_bits(ld, 0, PORT_EN, BITS_RX_EN);
	   higmac_desc_disable(ld, RX_OUTCFF_WR_DESC_ENA|RX_CFF_RD_DESC_ENA);
	*/
	return 0;
}

int eth_send(volatile void *packet, int length)
{
	struct higmac_netdev_local *ld = current_ld;
	int timeout_us = 1000;
	int tx_bq_wr_offset = 0;
	int tx_bq_rd_offset = 0;
	int tx_rq_wr_offset = 0;
	int tx_rq_rd_offset = 0;
	unsigned int tso_ver = 0;
	higmac_desc *tx_bq_desc = ld->tx_bq_addr;

	tx_bq_wr_offset = higmac_readl_bits(ld,
			TX_BQ_WR_ADDR, BITS_TX_BQ_WR_ADDR);
	tx_bq_rd_offset = higmac_readl_bits(ld,
			TX_BQ_RD_ADDR, BITS_TX_BQ_RD_ADDR);

	if (tx_bq_rd_offset != tx_bq_wr_offset) {
		higmac_writel_bits(ld, tx_bq_rd_offset,
				TX_BQ_WR_ADDR, BITS_TX_BQ_WR_ADDR);
		/*printf("tx bq is error!\n"); */
		return -1;
	}

	tso_ver = higmac_readl_bits(ld, CRF_MIN_PACKET, BIT_TSO_VERSION);

	tx_bq_desc += (tx_bq_wr_offset >> DESC_BYTE_SHIFT);
	tx_bq_desc->data_buff_addr = (unsigned int)packet;
	tx_bq_desc->descvid = DESC_VLD_BUSY;

	if (tso_ver)
		tx_bq_desc->fl = 0;
	else
		tx_bq_desc->fl = DESC_FL_FULL;

	tx_bq_desc->reserve1 = 0;
	tx_bq_desc->data_len = length;
	tx_bq_desc->buffer_len = HIETH_MAX_FRAME_SIZE - 1;

	tx_bq_wr_offset += DESC_SIZE;
	if (tx_bq_wr_offset >= (HIGMAC_HWQ_TX_BQ_DEPTH << DESC_BYTE_SHIFT))
		tx_bq_wr_offset = 0;

	higmac_writel_bits(ld, tx_bq_wr_offset,
			TX_BQ_WR_ADDR, BITS_TX_BQ_WR_ADDR);

	/* enable tx */
	/*
	   higmac_writel_bits(ld, 1, PORT_EN, BITS_TX_EN);
	   higmac_desc_enable(ld, TX_OUTCFF_WR_DESC_ENA|TX_CFF_RD_DESC_ENA);
	*/
	tx_rq_wr_offset = higmac_readl_bits(ld,
			TX_RQ_WR_ADDR, BITS_TX_RQ_WR_ADDR);
	tx_rq_rd_offset = higmac_readl_bits(ld,
			TX_RQ_RD_ADDR, BITS_TX_RQ_RD_ADDR);

	tx_rq_rd_offset += DESC_SIZE;
	if (tx_rq_rd_offset >= (HIGMAC_HWQ_TX_RQ_DEPTH << DESC_BYTE_SHIFT))
		tx_rq_rd_offset = 0;

	while (--timeout_us && (tx_rq_rd_offset != tx_rq_wr_offset)) {
		udelay(1);
		tx_rq_wr_offset = higmac_readl_bits(ld,
				TX_RQ_WR_ADDR, BITS_TX_RQ_WR_ADDR);
	}

	if (!timeout_us) {
		/* printf("send time out!\n"); */
		return -1;
	}

	higmac_writel_bits(ld, tx_rq_rd_offset,
			TX_RQ_RD_ADDR, BITS_TX_RQ_RD_ADDR);
	if (gmac_debug)
		printf("send packet!\n");
	/* disable tx */
	/*
	   higmac_desc_disable(ld, TX_OUTCFF_WR_DESC_ENA|TX_CFF_RD_DESC_ENA);
	   higmac_writel_bits(ld, 0, PORT_EN, BITS_TX_EN);
	*/
	return 0;
}

static int gmac_inited;
static int mdio_register;
static int higmac_init(void)
{
	int ret = 0;
	struct higmac_netdev_local *ld;
	int i;

	/* init once to save time */
	if (!gmac_inited) {
		gmac_inited = 1;
		higmac_sys_init();
		higmac_mdiobus_driver_init();

		if (!mdio_register) {
			mdio_register = 1;
			miiphy_register(mdio_bus[0], higmac0_mdio_read,
					higmac0_mdio_write);

			if (CONFIG_GMAC_NUMS == 2) {
				miiphy_register(mdio_bus[1], higmac1_mdio_read,
						higmac1_mdio_write);
			}
		}
#if 0
		/* disable PHY 1000M Mode */
		miiphy_write(higmac_board_info[0].mii_name,
				higmac_board_info[0].phy_addr, PHY_1000BTCR, 0);

		if (CONFIG_GMAC_NUMS == 2)
			miiphy_write(higmac_board_info[1].mii_name,
					higmac_board_info[1].phy_addr,
					PHY_1000BTCR, 0);
#endif

		miiphy_reset(higmac_board_info[0].mii_name,
				higmac_board_info[0].phy_addr);

		if (CONFIG_GMAC_NUMS == 2)
			miiphy_reset(higmac_board_info[1].mii_name,
					higmac_board_info[1].phy_addr);

		for_each_gmac_netdev_local_priv(ld, i) {
			higmac_glb_preinit_dummy(ld);

			ret = higmac_set_hwq_depth(ld);
			if (ret) {
				printf("init eth%d hw desc depth fail!\n", i);
				goto glb_init;
			}

			ret = higmac_init_hw_desc_queue(ld);
			if (ret) {
				printf("init eth%d hw desc queue fail!\n", i);
				goto hw_desc_init;
			}
		}
	}

	ret = select_current_linked_phy();
	if (ret)
		goto mdiobus_init;

	higmac_desc_enable(current_ld,
			RX_OUTCFF_WR_DESC_ENA|RX_CFF_RD_DESC_ENA);
	higmac_writel_bits(current_ld, 1, PORT_EN, BITS_RX_EN);

	higmac_writel_bits(current_ld, 1, PORT_EN, BITS_TX_EN);
	higmac_desc_enable(current_ld,
			TX_OUTCFF_WR_DESC_ENA|TX_CFF_RD_DESC_ENA);

	return 0;

hw_desc_init:
glb_init:
	higmac_mdiobus_driver_exit();

mdiobus_init:
	higmac_sys_exit();

	return ret;
}

static void higmac_exit(void)
{
	higmac_writel_bits(current_ld, 0, PORT_EN, BITS_RX_EN);
	higmac_desc_disable(current_ld,
			RX_OUTCFF_WR_DESC_ENA|RX_CFF_RD_DESC_ENA);

	higmac_desc_disable(current_ld,
			TX_OUTCFF_WR_DESC_ENA|TX_CFF_RD_DESC_ENA);
	higmac_writel_bits(current_ld, 0, PORT_EN, BITS_TX_EN);

	/* higmac_glb_preinit_dummy(current_ld); */
#if 0
	higmac_destroy_hw_desc_queue(current_ld);
	higmac_mdiobus_driver_exit();
	higmac_sys_exit();
#endif
}

int eth_init(bd_t *bd)
{
	int ret;

	parse_module_parameters();

	ret = higmac_init();
	if (ret)
		printf("higmac init fail!\n");

	return ret;
}

void eth_halt(void)
{
	if (current_ld && current_ld->iobase)
		higmac_exit();
}
