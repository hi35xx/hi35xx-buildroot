#define HIETH_SFV300
#include "hieth.h"
#include "mdio.h"
#include "mac.h"
#include "ctrl.h"
#include "glb.h"
#include "sys.h"
#include <config.h>
#include <miiphy.h>
#include <net.h>
#define CONFIG_RANDOM_ETHADDR
/*************************************************************************/
int hieth_mdiobus_driver_init(void);
void hieth_mdiobus_driver_exit(void);
extern unsigned int get_phy_device(char *devname, unsigned char phyaddr);
static struct hieth_netdev_local hieth_devs[2];

static struct hieth_netdev_local *hieth_curr=NULL;

/* Used when trying to connect to a specific phy (mii bus id:phy device id) */
/* refer linux-2.6.23 ./include/linux/phy.h line 76 */
char U_PHY_NAME[MAX_PHY_NAME_LEN];
char D_PHY_NAME[MAX_PHY_NAME_LEN];

/* only use for uboot */
unsigned char U_PHY_ADDR = HISFV_PHY_U;
unsigned char D_PHY_ADDR = HISFV_PHY_D;

#define INTERFACE_MODE_MII 0x0
#define INTERFACE_MODE_RMII 0x1
static unsigned int g_interface_mode = INTERFACE_MODE_MII;

#ifndef INNER_PHY
void set_efuse_unread(void)
{
}

void set_inner_phy_addr(unsigned long phyaddr)
{
}
#endif

#define MAC_LEN 6

#define print_mac(mac)	 do{ int i;\
	printf("MAC:  ");\
	for (i = 0; i < MAC_LEN; i++)\
		printf("%c%02X", i ? '-' : ' ', *(((unsigned char*)mac)+i));\
	printf("\n");\
}while(0)

void string_to_mac(unsigned char *mac, char* s)
{
	int i;
	char *e;

	for (i=0; i<MAC_LEN; ++i) {
		mac[i] = s ? simple_strtoul(s, &e, 16) : 0;
		if (s) {
			s = (*e) ? e+1 : e;
		}
	}
}
 
static void print_mac_address(const char *pre_msg, const unsigned char *mac, const char *post_msg)
{
	int i;
	
	if(pre_msg)
		printf(pre_msg);

	for(i=0; i<6; i++)
		printf("%02X%s", mac[i], i==5 ? "" : ":");

	if(post_msg)
		printf(post_msg);
}

#ifdef CONFIG_RANDOM_ETHADDR
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

//void random_ether_addr(char *mac)
void random_ether_addr(unsigned char *mac)
{
    unsigned long ethaddr_low, ethaddr_high;

    srandom(get_timer(0) );

    /*
     * setting the 2nd LSB in the most significant byte of
     * the address makes it a locally administered ethernet
     * address
     */
    ethaddr_high = (random() & 0xfeff) | 0x0200;
    ethaddr_low = random();

    mac[0] = ethaddr_high >> 8;
    mac[1] = ethaddr_high & 0xff;
    mac[2] = ethaddr_low >>24;
    mac[3] = (ethaddr_low >> 16) & 0xff;
    mac[4] = (ethaddr_low >> 8) & 0xff;
    mac[5] = ethaddr_low & 0xff;    

    mac [0] &= 0xfe;	/* clear multicast bit */
    mac [0] |= 0x02;	/* set local assignment bit (IEEE802) */
    
 }
#endif

//added by wzh 2009-4-9 begin
#define ETH_GLB_REG( n )    (GLB_HOSTMAC_L32 + ETH_IO_ADDRESS_BASE + n*4)  
#if 0
static int is_valid_ether_addr(const u8 * addr)
{
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}
#endif
static int set_mac_address(char * mac)
{
	unsigned char t[4] = {0};
	
	t[0] = mac[1];
	t[1] = mac[0];
	*(unsigned int *)ETH_GLB_REG(1) = *((unsigned int*)t);
	
	t[0] = mac[5];
	t[1] = mac[4];
	t[2] = mac[3];
	t[3] = mac[2];
	*(unsigned int *)ETH_GLB_REG(0) = *((unsigned int*)t);
	
	return 0;
}
static int set_random_mac_address(unsigned char * mac, unsigned char * ethaddr)
{
	random_ether_addr(mac);
	print_mac_address( "Set Random MAC address: ", mac, "\n");

	sprintf ((char *)ethaddr, "%02X:%02X:%02X:%02X:%02X:%02X",
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	setenv("ethaddr",(char *)ethaddr);

	set_mac_address((char *)mac);
    return 0;
}

int eth_set_host_mac_address(void)
{
	char* s;
	unsigned char mac[MAC_LEN];
	unsigned char ethaddr[20];
	
	s=getenv("ethaddr");
	if(!s){
		printf("none ethaddr. \n"); 
		set_random_mac_address(mac, ethaddr);
		return 0;
	}
	
	string_to_mac(mac,s);
	if(!is_valid_ether_addr(mac))
	{
		printf("MAC address invalid!\n");
		set_random_mac_address(mac, ethaddr);
		return 0;
	}
	
	print_mac(mac);
	set_mac_address((char *)mac);

	return 0;
}

//modified by wzh 2009-4-14 end


static void phy_print_status(struct hieth_netdev_local *ld, int stat)
{
	printf("%s : phy status change : LINK=%s : DUPLEX=%s : SPEED=%s\n", \
			(ld->port==UP_PORT) ? "UP_PORT" : "DOWN_PORT", \
			(stat & HIETH_LINKED) ? "UP" : "DOWN", \
			(stat & HIETH_DUP_FULL) ? "FULL" : "HALF", \
			(stat & HIETH_SPD_100M) ? "100M" : "10M");
}

static void hieth_adjust_link(struct hieth_netdev_local *ld)
{
	int stat = 0;
	int timeout_us = 1000;
    /*this env phy_link_time used to solve the difference phy auto-negotiation time of  various phys*/
    char* timeout = getenv("phy_link_time");
    if (timeout){
        timeout_us = simple_strtol(timeout,0, 10);
        if(timeout_us < 0)
            timeout_us = 1000;
    }
    {
        u16 reg = 0;
        miiphy_read(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR), PHY_ANAR, &reg);
        reg |= 0x1E0;
        miiphy_write(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR), PHY_ANAR, reg);
    }
retry:
	udelay(1);

	stat |= miiphy_link(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR)) ? HIETH_LINKED : 0;
	stat |= miiphy_duplex(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR)) == FULL ? HIETH_DUP_FULL : 0;
	stat |= miiphy_speed(UD_REG_NAME(PHY_NAME), UD_REG_NAME(PHY_ADDR)) == _100BASET ? HIETH_SPD_100M : 0;

	if( --timeout_us && !(stat & HIETH_LINKED) )
		goto retry;

	if(stat != ld->link_stat) {
		hieth_set_linkstat(ld, stat);
		phy_print_status(ld, stat);
		ld->link_stat = stat;
		hieth_set_mii_mode(ld, g_interface_mode);
	}

	set_phy_valtage();
}

static int hieth_net_open(struct hieth_netdev_local *ld)
{
	/* enable sys-ctrl-en and clk-en  */
	hieth_sys_startup();

	/* setup hardware tx dep */
	hieth_writel_bits(ld, HIETH_HW_TXQ_DEPTH, UD_REG_NAME(GLB_QLEN_SET), BITS_TXQ_DEP);

	/* setup hardware rx dep */
	hieth_writel_bits(ld, HIETH_HW_RXQ_DEPTH, UD_REG_NAME(GLB_QLEN_SET), BITS_RXQ_DEP);

	ld->link_stat = 0;
	hieth_adjust_link(ld);

	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_RX_RDY));

	return 0;
}

static int hieth_net_close(struct hieth_netdev_local *ld)
{
	hieth_glb_preinit_dummy(ld);

	hieth_sys_allstop();

	return 0;
}

static int hieth_dev_probe_init(int port)
{
	struct hieth_netdev_local *ld = &hieth_devs[port];

	if( (UP_PORT != port) && (DOWN_PORT != port) )
		return -1;//-ENODEV

	ld->iobase_phys = ETH_IO_ADDRESS_BASE;
	
	ld->port = port;

	ld->phy_name = UD_REG_NAME(PHY_NAME);

	hieth_glb_preinit_dummy(ld);

	hieth_sys_allstop();

	return 0;
}

static int hieth_dev_remove(struct hieth_netdev_local *ld)
{
	return 0;
}

static int hieth_init(void)
{
	int ret = 0;
	char *phyaddr = NULL;
	char *mdio_intf = NULL;

	/*get mdio interface from env.FORMAT: mdio_intf=mii or mdio_intf=rmii*/
	mdio_intf = getenv("mdio_intf");
	if (mdio_intf) {
		if (strncmp(mdio_intf, "mii", 3)
				&& strncmp(mdio_intf, "rmii", 4)) {
			printf("Invalid mdio intface assignment");
			printf("mii, rmii ). Set default to mii\n");
		} else if (!strncmp(mdio_intf, "mii", 3))
			g_interface_mode = INTERFACE_MODE_MII;
		else if (!strncmp(mdio_intf, "rmii", 4))
			g_interface_mode = INTERFACE_MODE_RMII;
	}

	/*get phy addr of up port*/
	phyaddr = getenv("phyaddru");
	if (phyaddr) {
		unsigned long tmp = simple_strtoul(phyaddr, 0, 10);
		/*check phyaddr > 0xff*/
		if (tmp >= (unsigned long)(~((unsigned char)0))) {
			printf("Detected env 'phyaddru' had been set greater"
					"than 0x%lx,this may not correct.\n",
					(unsigned long)(~((unsigned char)0)));
			return -1;
		}
		U_PHY_ADDR = (unsigned char)tmp;
	} else {
		#if defined(INNER_PHY) && defined(INNER_PHY_ADDR_U)
		U_PHY_ADDR = INNER_PHY_ADDR_U;
		#endif
	}

	#ifdef INNER_PHY
	set_inner_phy_addr(U_PHY_ADDR);
	#endif

	sprintf(U_PHY_NAME, "0:%d", U_PHY_ADDR);

	/*get phy addr of down port*/
	phyaddr = getenv("phyaddrd");
	if (phyaddr) {
		unsigned long tmp = (int)simple_strtoul(phyaddr, 0, 10);
		/*check phyaddr > 0xff*/
		if (tmp >= (unsigned long)(~((unsigned char)0))) {
			printf("Detected env 'phyaddrd' had been set greater "
					"than 0x%lx,this may not correct.\n",
					(unsigned long)(~((unsigned char)0)));
			return -1;
		}
		D_PHY_ADDR = (unsigned char)tmp;
	} else {
		#if defined(INNER_PHY) && defined(INNER_PHY_ADDR_D)
		D_PHY_ADDR = INNER_PHY_ADDR_D;
		#endif
	}
	sprintf(D_PHY_NAME, "0:%d", D_PHY_ADDR);

	printf(OSDRV_MODULE_VERSION_STRING"\n");

	memset(hieth_devs, 0, sizeof(hieth_devs));

	set_efuse_unread();

	/* sys-func-sel */
	hieth_sys_init();

	/* register MDIO bus to uboot */
	if (hieth_mdiobus_driver_init())
		goto _error_mdiobus_driver_init;

	if (!get_phy_device(U_PHY_NAME, U_PHY_ADDR)) {

		ret = hieth_dev_probe_init(UP_PORT);
		if (ret) {
			hieth_error("register UpEther netdevice"
					" driver failed!");
			goto _error_register_driver;
		}
	}

	if (!get_phy_device(D_PHY_NAME, D_PHY_ADDR)) {
		ret = hieth_dev_probe_init(DOWN_PORT);
		if (ret) {
			hieth_error("register DownEther netdevice"
					" driver failed!");
			goto _error_register_driver;
		}
	}

	return ret;

_error_register_driver:
	hieth_mdiobus_driver_exit();

_error_mdiobus_driver_init:

	return -1;
}

static void hieth_exit(void)
{

	hieth_mdiobus_driver_exit();

	hieth_sys_exit();
}

int eth_init(bd_t * bd)
{
	int res;
	int count = 30;
	res = hieth_init();
	if(res)
		goto _error_hieth_init;

	eth_set_host_mac_address();
retry:
	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR)){

		/* open UpEther net dev */
		hieth_net_open(&hieth_devs[UP_PORT]);
		if(hieth_devs[UP_PORT].link_stat & HIETH_LINKED) {
			hieth_curr = &hieth_devs[UP_PORT];
			goto _link_ok;
		}
	}

	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR)){

		/* open DownEther net dev */
		hieth_net_open(&hieth_devs[DOWN_PORT]);
		if(hieth_devs[DOWN_PORT].link_stat & HIETH_LINKED) {
			hieth_curr = &hieth_devs[DOWN_PORT];
			
			goto _link_ok;
		}
	}
	if( --count )
		goto retry;

	printf("Up/Down PHY not link.\n");

_error_hieth_init:

	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR))
		hieth_net_close(&hieth_devs[UP_PORT]);

	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR))
		hieth_net_close(&hieth_devs[DOWN_PORT]);

	return -1;

_link_ok:

	return 0;
}

int eth_rx(void)
{
	int recvq_ready, timeout_us = 10000;
	struct hieth_frame_desc fd;
	struct hieth_netdev_local *ld = hieth_curr;

	/* check this we can add a Rx addr */
	recvq_ready = hieth_readl_bits(ld, UD_REG_NAME(GLB_RO_QUEUE_STAT), BITS_RECVQ_RDY);
	if(!recvq_ready) {
		hieth_trace(7, "hw can't add a rx addr.");
	}

	/* enable rx int */
	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_RX_RDY));

	/* fill rx hwq fd */
	fd.frm_addr = (unsigned long)NetRxPackets[0];
	fd.frm_len = 0;
	hw_recvq_setfd(ld, fd);

	/* receive packed, loop in NetLoop */
	while(--timeout_us && !is_recv_packet(ld))
		udelay(1);
		
	if( is_recv_packet(ld) ) {

		/*hwid = hw_get_rxpkg_id(ld);*/
		fd.frm_len = hw_get_rxpkg_len(ld);
		hw_set_rxpkg_finish(ld);

		if(HIETH_INVALID_RXPKG_LEN(fd.frm_len)) {
			hieth_error("frm_len invalid (%d).", fd.frm_len);
			goto _error_exit;
		}

		/* Pass the packet up to the protocol layers. */
		NetReceive ((void*)fd.frm_addr, fd.frm_len);

		return 0;
	}
	else {
		hieth_trace(7, "hw rx timeout.");
	}

_error_exit:
	return -1;
}

int eth_send(volatile void *packet, int length)
{
	int ints, xmitq_ready, timeout_us = 3000;
	struct hieth_frame_desc fd;
	struct hieth_netdev_local *ld = hieth_curr;

	/* check this we can add a Tx addr */
	xmitq_ready = hieth_readl_bits(ld, UD_REG_NAME(GLB_RO_QUEUE_STAT), BITS_XMITQ_RDY);
	if(!xmitq_ready) {
		hieth_error("hw can't add a tx addr.");
		goto _error_exit;
	}

	/* enable tx int */
	hieth_irq_enable(ld, UD_BIT_NAME(HIETH_INT_TXQUE_RDY));

	/* fill tx hwq fd */
	fd.frm_addr = (unsigned long)packet;
	fd.frm_len = length + 4;
	hw_xmitq_setfd(ld, fd);

	do{
		udelay(1);
		ints = hieth_read_irqstatus(ld);
	}while( --timeout_us && !(ints & UD_BIT_NAME(HIETH_INT_TXQUE_RDY)) );

	hieth_clear_irqstatus(ld, ints);
		
	if( !timeout_us ) {
		hieth_error("hw tx timeout.");
		goto _error_exit;
	}

	return 0;
		
_error_exit:
	return -1;
}

void eth_halt(void)
{
	if(hieth_devs[UP_PORT].iobase_phys)
		hieth_net_close(&hieth_devs[UP_PORT]);
	if(hieth_devs[DOWN_PORT].iobase_phys)
		hieth_net_close(&hieth_devs[DOWN_PORT]);

	hieth_dev_remove(&hieth_devs[UP_PORT]);
	hieth_dev_remove(&hieth_devs[DOWN_PORT]);
	
	hieth_exit();
}
