#ifndef __STMMAC_H__
#define __STMMAC_H__

#include <common.h>
#include <command.h>
#include <linux/list.h>
#include <asm/io.h>
#include <malloc.h>         /* malloc, free, realloc*/
#include <net.h>
#include <miiphy.h>

#define print_mac(mac)	 do { int i;\
	printf("MAC:  ");\
	for (i = 0; i < MAC_LEN; i++)\
		printf("%c%02X", i ? '-' : ' ', *(((unsigned char *)mac)+i));\
	printf("\n");\
} while (0)

#define MAC_LEN 6
#define MAX_PHY_NAME_LEN 6

#define MIN_PKG_LEN (42)
#define MAX_PKG_LEN	(1600)

#define STMMAC_INVALID_RXPKG_LEN(len) \
	(!(((len) >= MIN_PKG_LEN) && ((len) <= MAX_PKG_LEN)))

#define PORT_MOD_10M_MII	0
#define PORT_MOD_100M_MII	1
#define PORT_MOD_1000M_GMII	2
#define PORT_MOD_10M_RGMII	3
#define PORT_MOD_100M_RGMII	4
#define PORT_MOD_1000M_RGMII	5

#define STMMAC_LINKED		(1 << 0)
#define STMMAC_DUP_FULL	(1 << 1)
#define STMMAC_SPD_10M		(1 << 2)
#define STMMAC_SPD_100M	(1 << 3)
#define STMMAC_SPD_1000M	(1 << 4)

#define GMAC0_PORT 0
#ifndef STMMAC_SINGLE_MAC
#define GMAC1_PORT 1
#endif

#define STMMAC_IOBASE		STMMAC_GMACADDR
#define STMMAC_IOBASE_DMA	STMMAC_DMAADDR

#define STMMAC_PHYID STMMAC_PHYADDR

#define SZ_1K 1024

#define STMMAC_HW_QUEUE_DEPTH	1

#define STMMAC_MAX_FRAME_SIZE	(SZ_1K*2)
#define SKB_SIZE		(STMMAC_MAX_FRAME_SIZE)

struct dma_desc_tx {
  unsigned int volatile status;
  unsigned int volatile length;
  unsigned int volatile buffer1;
  unsigned int volatile buffer2;
};

struct dma_desc_rx {
  unsigned int volatile status_0;
  unsigned int volatile length_0;
  unsigned int volatile buffer1_0;
  unsigned int volatile buffer2_0;
  unsigned int volatile status_1;
  unsigned int volatile length_1;
  unsigned int volatile buffer1_1;
  unsigned int volatile buffer2_1;
};

struct stmmac_netdev_local {
/* gmac addr */
	unsigned long iobase_gmac;
/* dma addr */
	unsigned long iobase_dma;

/* 0 => GMAC0, 1 => GMAC1 */
	int port;

	struct dma_desc_rx *rx_addr;
	struct dma_desc_tx *tx_addr;

	char *phy_name;
	int link_stat;

};

#endif
