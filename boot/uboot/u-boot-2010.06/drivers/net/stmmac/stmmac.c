#include "ctrl.h"
#include "mdio.h"

#ifdef CONFIG_GODNET
#include "sys_godnet.h"
#endif

#ifdef CONFIG_GODARM
#include "sys_godarm.h"
#endif

#ifdef CONFIG_HI3535
#include "sys_hi3535.h"
#endif

#ifndef STMMAC_SINGLE_MAC
static struct stmmac_netdev_local g_stmmac_ld[2];
#else
static struct stmmac_netdev_local g_stmmac_ld[1];
#endif

struct stmmac_netdev_local *stmmac_curr;

char GMAC0_PHY_NAME[MAX_PHY_NAME_LEN];
#ifndef STMMAC_SINGLE_MAC
char GMAC1_PHY_NAME[MAX_PHY_NAME_LEN];
#endif

unsigned char GMAC0_PHY_ADDR = STMMAC_PHYADDR0;
#ifndef STMMAC_SINGLE_MAC
unsigned char GMAC1_PHY_ADDR = STMMAC_PHYADDR1;
#endif

unsigned int FLAG;

#define INTERFACE_MODE_RGMII 0x20
#define INTERFACE_MODE_MII 0x0
#define INTERFACE_MODE_RMII 0x80
static unsigned int g_interface_mode = INTERFACE_MODE_RGMII;

struct dma_desc_rx rd __attribute__ ((aligned(16)));
struct dma_desc_tx td __attribute__ ((aligned(16)));

void string_to_mac(char *mac, char *s)
{
	int i;
	char *e;

	for (i = 0; i < MAC_LEN; ++i) {
		mac[i] = s ? simple_strtoul(s, &e, 16) : 0;
		if (s)
			s = (*e) ? e+1 : e;
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

void random_ether_addr(char *mac)
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

	mac[5] = ethaddr_high >> 8;
	mac[4] = ethaddr_high & 0xff;
	mac[3] = ethaddr_low >> 24;
	mac[2] = (ethaddr_low >> 16) & 0xff;
	mac[1] = (ethaddr_low >> 8) & 0xff;
	mac[0] = ethaddr_low & 0xff;

	mac[0] &= 0xfe;    /* clear multicast bit */
	mac[0] |= 0x02;    /* set local assignment bit (IEEE802) */

}

static int set_random_mac_address(char *mac, char *ethaddr)
{
	random_ether_addr(mac);

	sprintf((char *)ethaddr, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	setenv("ethaddr", (char *)ethaddr);

	stmmac_hw_set_macaddress(&g_stmmac_ld[GMAC0_PORT], mac);
#ifndef STMMAC_SINGLE_MAC
	stmmac_hw_set_macaddress(&g_stmmac_ld[GMAC1_PORT], mac);
#endif

	return 0;
}

static int stmmac_net_set_mac_address(struct stmmac_netdev_local *ld)
{
	char *s;
	char mac[MAC_LEN];
	char ethaddr[20];

	s = getenv("ethaddr");
	if (!s)
		set_random_mac_address(mac, ethaddr);

	string_to_mac(mac, s);

	if (!is_valid_ether_addr((u8 *)mac))
		set_random_mac_address(mac, ethaddr);

	print_mac(mac);

	stmmac_hw_set_macaddress(ld, mac);

	return 0;
}

static int stmmac_init_hw_desc_queue(struct stmmac_netdev_local *ld)
{
	int count = 0;

	count = STMMAC_HW_QUEUE_DEPTH;

	memset((void *)&td, 0, (count * sizeof(struct dma_desc_tx)));
	td.status = TXDESCENDOFRING;
	writel((unsigned int)&td, ld->iobase_dma + DMA_TX_BASE_ADDR);

	/* init rx queue */
	memset((void *)&rd, 0, (count * sizeof(struct dma_desc_rx)));
	rd.length_1 = RXDESCENDOFRING;
	writel((unsigned int)&rd, ld->iobase_dma + DMA_RCV_BASE_ADDR);

	return 0;
}

static void stmmac_destroy_hw_desc_queue(struct stmmac_netdev_local *ld)
{
	ld->tx_addr = NULL;
	ld->rx_addr = NULL;
}

static void stmmac_net_adjust_link(struct stmmac_netdev_local *ld)
{
	int stat = 0;
	int newval = 0;

	unsigned char phyaddr;
	char *phy_name;

#ifndef STMMAC_SINGLE_MAC
	phyaddr = (ld->port == GMAC0_PORT) ? GMAC0_PHY_ADDR : GMAC1_PHY_ADDR;
#else
	phyaddr = GMAC0_PHY_ADDR;
#endif
	phy_name = GMAC0_PHY_NAME;

	stat |= miiphy_link(phy_name, phyaddr) ? STMMAC_LINKED : 0;

	if (miiphy_link(phy_name, phyaddr) ==  STMMAC_LINKED) {
		if (miiphy_duplex(phy_name, phyaddr) == 44) {
#ifdef CONFIG_TNK
			newval |= 0x10;
#endif
			if (miiphy_speed(phy_name, phyaddr) == 1000)
				writel(0x280c, ld->iobase_gmac + MAC_CTRL_REG);
			else if (miiphy_speed(phy_name, phyaddr) == 100) {
#ifdef CONFIG_TNK
				newval |= 0x3;
#endif
				writel(0xe80c, ld->iobase_gmac + MAC_CTRL_REG);
			} else if (miiphy_speed(phy_name, phyaddr) == 10) {
#ifdef CONFIG_TNK
				newval |= 0x1;
#endif
				writel(0xa80c, ld->iobase_gmac + MAC_CTRL_REG);
			}
		} else if (miiphy_duplex(phy_name, phyaddr) == 22) {
			if (miiphy_speed(phy_name, phyaddr) == 100) {
#ifdef CONFIG_TNK
				newval |= 0x3;
#endif
				writel(0xe00c, ld->iobase_gmac + MAC_CTRL_REG);
			} else if (miiphy_speed(phy_name, phyaddr) == 10) {
#ifdef CONFIG_TNK
				newval |= 0x1;
#endif
				writel(0xa00c, ld->iobase_gmac + MAC_CTRL_REG);
			}
		}
	} else if (miiphy_link(phy_name, phyaddr) == 0)
		return;

#ifdef CONFIG_TNK
	newval |= 0xc;

	/* set mac interface mode mii, rmii, rgmii */
	newval |= g_interface_mode;

	if (ld->port)
		newval |= newval << 16;

#ifdef STMMAC_SINGLE_MAC
	reset_mac_interface_single(newval);
#else
	reset_mac_interface_dual(newval);
#endif
#endif

	ld->link_stat = stat;
	return;
}

int eth_rx(void)
{
	struct stmmac_netdev_local *ld = stmmac_curr;
	int timeout_us = 100000;
	int lenth = 0;
	unsigned int rx_status_0 = 0, rx_status_1 = 0;

	if (FLAG == 0) {
		rd.length_0 |= 0x7d0;
		rd.buffer1_0 = (unsigned int)NetRxPackets[0];
		rd.status_0 = DESCOWNBYDMA;
		goto start_rx;
	} else {
		rd.length_1 |= 0x7d0;
		rd.buffer1_1 = (unsigned int)NetRxPackets[0];
		rd.status_1 = DESCOWNBYDMA;
		goto start_rx;
	}

start_rx:
	/* Start RCV */
#ifdef CONFIG_TNK
	writel(DMA_CONTROL_SR | 0x3200000, ld->iobase_dma + DMA_CONTROL);
#else
	writel(DMA_CONTROL_SR, ld->iobase_dma + DMA_CONTROL);
#endif

	if (FLAG == 0) {
		while ((--timeout_us) &&
			((rd.status_0 & DESCOWNBYDMA) == DESCOWNBYDMA)) {
			/*Enable DMA receive poll demand */
			writel(0x1, ld->iobase_dma + DMA_RCV_POLL_DEMAND);
		}

		if (!timeout_us) {
			memset((void *)&rd, 0, 16);

			return -1;
		}

		rx_status_0 = rd.status_0;
		lenth = ((rx_status_0 & DESCRXLENTHMASK)
				>> DESCRXLENTHSHIFT) - 4;

		NetReceive((uchar *)NetRxPackets[0], lenth);

		/* Reset desc */
		memset((void *)(&rd), 0, 16);

		FLAG = 1;

		return 0;
	} else {
		while ((--timeout_us) && ((rd.status_1 & DESCOWNBYDMA)
							== DESCOWNBYDMA)) {
			/*Enable DMA receive poll demand */
			writel(0x1, ld->iobase_dma + DMA_RCV_POLL_DEMAND);
		}

		if (!timeout_us) {
			memset(((void *)&rd + 16), 0, 16);
			rd.length_1 = RXDESCENDOFRING;

			return -1;
		}

		rx_status_1 = rd.status_1;
		lenth = ((rx_status_1 & DESCRXLENTHMASK)
					>> DESCRXLENTHSHIFT) - 4;

		NetReceive((uchar *)NetRxPackets[0], lenth);

		/* Reset desc */
		memset((void *)((&rd) + 16), 0, 16);

		rd.length_1 = RXDESCENDOFRING;

		FLAG = 0;

		return 0;
	}
}

int eth_send(volatile void *packet, int length)
{
	struct stmmac_netdev_local *ld = stmmac_curr;
	int timeout_us = 100000;

	td.length = ((length << DESCTXSIZESHIFT) & DESCTXSIZEMASK);
	td.status |= (DESCTXFIRST | DESCTXLAST);
	td.status |= DESCOWNBYDMA;
	td.buffer1 = (unsigned int)packet;

	/* Start transmit */
#ifdef CONFIG_TNK
	writel(DMA_CONTROL_ST | 0x3200000, ld->iobase_dma + DMA_CONTROL);
#else
	writel(DMA_CONTROL_ST, ld->iobase_dma + DMA_CONTROL);
	writel(1, ld->iobase_dma + DMA_XMT_POLL_DEMAND);
#endif

	while (--timeout_us && ((td.status & DESCOWNBYDMA) == DESCOWNBYDMA)) {}

	memset((void *)(&td), 0, sizeof(struct dma_desc_tx));
	td.status = TXDESCENDOFRING;

	if (!timeout_us)
		return -1;

	return 0;
}

static int stmmac_dev_probe_init(int port)
{
	struct stmmac_netdev_local *ld = &g_stmmac_ld[port];

	int ret = 0;

#ifndef STMMAC_SINGLE_MAC
	if ((GMAC0_PORT != port) && (GMAC1_PORT != port))
		return -1;
#else
	if (GMAC0_PORT != port)
		return -1;
#endif

	ld->port = port;

	ld->iobase_gmac = STMMAC_IOBASE + ld->port * 0x4000;
	ld->iobase_dma = STMMAC_IOBASE_DMA + ld->port * 0x100;

#ifndef STMMAC_SINGLE_MAC
	ld->phy_name = (ld->port == GMAC0_PORT)
		? GMAC0_PHY_NAME : GMAC1_PHY_NAME;
#else
	ld->phy_name = GMAC0_PHY_NAME;
#endif

	stmmac_sys_init(ld);

	ret = stmmac_init_hw_desc_queue(ld);

	FLAG = 0;

	if (ret) {
		printf("init hw desc queue fail!\n");

		return 1;
	}

	return 0;
}

static int stmmac_init(void)
{
	int ret = 0;
	char *phyaddr;
	char *mdio_intf;

	/* get mdio interface from env.FORMAT: mdio_intf=mii or mdio_intf=rgmii
	   or mdio_intf=rmii */
	mdio_intf = getenv("mdio_intf");
	if (mdio_intf) {
		if (strncmp(mdio_intf, "mii", 3)
				&& strncmp(mdio_intf, "rgmii", 5)
				&& strncmp(mdio_intf, "rmii", 4)) {
			printf("Invalid mdio intface assignment");
			printf("mii, rgmii or rmii ). Set default to rgmii\n");
		} else if (!strncmp(mdio_intf, "mii", 3))
			g_interface_mode = INTERFACE_MODE_MII;
		else if (!strncmp(mdio_intf, "rgmii", 5))
			g_interface_mode = INTERFACE_MODE_RGMII;
		else if (!strncmp(mdio_intf, "rmii", 4))
			g_interface_mode = INTERFACE_MODE_RMII;
	}

	/*get phy addr of GMAC0 port*/
	phyaddr = getenv("phyaddr0");
	if (phyaddr) {
		unsigned long tmp = simple_strtoul(phyaddr, 0, 10);
		/*phy addr == 0, notify user to check wether its ok.*/
		if (tmp == 0) {
			printf("Detected env 'phyaddr0' had been set"
				"to 0,is it right?\n");
		}
		/*check phyaddr > 0xff*/
		else if (tmp >= (unsigned long)(~((unsigned char)0))) {
			printf("Detected env 'phyaddr0' had been set greater"
				"than 0x%lx,this may not correct.\n",
				(unsigned long)(~((unsigned char)0)));
			return -1;
		}
		GMAC0_PHY_ADDR = (unsigned char)tmp;
		sprintf(GMAC0_PHY_NAME, "0:%d", GMAC0_PHY_ADDR);
	} else
		sprintf(GMAC0_PHY_NAME, "0:%d", GMAC0_PHY_ADDR);

#ifndef STMMAC_SINGLE_MAC
	/*get phy addr of GMAC1 port*/
	phyaddr = getenv("phyaddr1");
	if (phyaddr) {
		unsigned long tmp = (int)simple_strtoul(phyaddr, 0, 10);
		/*phy addr == 0, notify user to check wether its ok.*/
		if (tmp == 0) {
			printf("Detected env 'phyaddr1' set to 0,"
				"is it right?\n");
		}
		/*check phyaddr > 0xff*/
		else if (tmp >= (unsigned long)(~((unsigned char)0))) {
			printf("Detected env 'phyaddr1' had been set greater"
				"than 0x%lx,this may not correct.\n",
				(unsigned long)(~((unsigned char)0)));
			return -1;
		}
		GMAC1_PHY_ADDR = (unsigned char)tmp;
		sprintf(GMAC1_PHY_NAME, "0:%d", GMAC1_PHY_ADDR);
	} else
		sprintf(GMAC1_PHY_NAME, "0:%d", GMAC1_PHY_ADDR);
#endif

	memset(g_stmmac_ld, 0, sizeof(g_stmmac_ld));

	ret = stmmac_mdiobus_driver_init();
	if (ret) {
		printf("Init mdio bus fail!\n ");
		return -1;
	}

	if (!get_phy_device(GMAC0_PHY_NAME, GMAC0_PHY_ADDR)) {

		ret = stmmac_dev_probe_init(GMAC0_PORT);
		if (ret) {
			printf("register GMAC0 netdevice driver failed!");
			return -1;
		}
	}

#ifndef STMMAC_SINGLE_MAC
	if (!get_phy_device(GMAC1_PHY_NAME, GMAC1_PHY_ADDR)) {

		ret = stmmac_dev_probe_init(GMAC1_PORT);
		if (ret) {
			printf("register GMAC1 netdevice driver failed!");
			return -1;
		}
	}
#endif

	return 0;
}

static void stmmac_exit(int port)
{
	struct stmmac_netdev_local *ld = &g_stmmac_ld[port];
	unsigned long data;

	data = readl(ld->iobase_dma + DMA_CONTROL);
	data &= (~DMA_CONTROL_ST);
	data &= (~DMA_CONTROL_SR);
	writel(data, ld->iobase_dma + DMA_CONTROL);

	data = readl(ld->iobase_gmac + MAC_CTRL_REG);
	data &= (~MAC_ENABLE_TX);
	writel(data, ld->iobase_gmac + MAC_CTRL_REG);
	data &= (~MAC_RNABLE_RX);
	writel(data, ld->iobase_gmac + MAC_CTRL_REG);

	stmmac_destroy_hw_desc_queue(ld);
	stmmac_mdiobus_driver_exit();
}

static int stmmac_net_open(struct stmmac_netdev_local *ld)
{
	ld->link_stat = 0;
	stmmac_net_adjust_link(ld);

	return 0;
}


int eth_init(bd_t *bd)
{
	int ret;
	int count = 10;
	char *timeout;
	unsigned int data;

	timeout = getenv("phy_link_time");

	if (timeout)
		count = simple_strtoul(timeout, 0, 10);

	ret = stmmac_init();

	if (ret) {
		printf("gmac:Init fail!\n");
		return ret;
	}

retry:
	if (!get_phy_device(GMAC0_PHY_NAME, GMAC0_PHY_ADDR)) {
		/* open UpEther net dev */
		stmmac_net_open(&g_stmmac_ld[GMAC0_PORT]);
		if (g_stmmac_ld[GMAC0_PORT].link_stat & STMMAC_LINKED) {
			stmmac_curr = &g_stmmac_ld[GMAC0_PORT];
			goto link_on;
		}
	}

#ifndef STMMAC_SINGLE_MAC
	if (!get_phy_device(GMAC1_PHY_NAME, GMAC1_PHY_ADDR)) {
		/* open DownEther net dev */
		stmmac_net_open(&g_stmmac_ld[GMAC1_PORT]);
		if (g_stmmac_ld[GMAC1_PORT].link_stat & STMMAC_LINKED) {
			stmmac_curr = &g_stmmac_ld[GMAC1_PORT];
			goto link_on;
		}
	}
#endif

	if (--count)
		goto retry;

	printf("PHY not link!\n");
	eth_halt();
	return -1;

link_on:
	stmmac_net_set_mac_address(stmmac_curr);

	writel(MMC_COUNTER_ENA, stmmac_curr->iobase_gmac + MMC_CTRL);
	writel(0x60f, stmmac_curr->iobase_gmac + GMAC_INT_MASK);
	writel(0xffffffff, stmmac_curr->iobase_gmac + MMC_INTR_MASK_RX);
	writel(0xffffffff, stmmac_curr->iobase_gmac + MMC_INTR_MASK_TX);
	writel(0xffffffff, stmmac_curr->iobase_dma + MMC_IPC_INTR_MASK_RX);
	writel(0x00000000, stmmac_curr->iobase_dma + DMA_INTR_ENA);
	writel(0x1ffff, stmmac_curr->iobase_dma + DMA_STATUS);

	data = readl(stmmac_curr->iobase_dma + DMA_CONTROL);
	data |= DMA_CONTROL_ST;
	data |= DMA_CONTROL_SR;
	writel(data, stmmac_curr->iobase_dma + DMA_CONTROL);

	return 0;
}

void eth_halt(void)
{
	if (g_stmmac_ld[GMAC0_PORT].iobase_gmac)
		stmmac_exit(GMAC0_PORT);
#ifndef STMMAC_SINGLE_MAC
	if (g_stmmac_ld[GMAC1_PORT].iobase_gmac)
		stmmac_exit(GMAC1_PORT);
#endif
}
