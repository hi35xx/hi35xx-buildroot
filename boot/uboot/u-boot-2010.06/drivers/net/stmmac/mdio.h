#ifndef __STMMAC_MDIO_H__
#define __STMMAC_MDIO_H__

#include "stmmac.h"

extern unsigned char GMAC0_PHY_ADDR;
#ifndef STMMAC_SINGLE_MAC
extern unsigned char GMAC1_PHY_ADDR;
#endif
extern char GMAC0_PHY_NAME[MAX_PHY_NAME_LEN];
#ifndef STMMAC_SINGLE_MAC
extern char GMAC1_PHY_NAME[MAX_PHY_NAME_LEN];
#endif

struct stmmac_mdio_local {
	unsigned long iobase;
	unsigned long iobase_phys;
	unsigned long csr_clk;
	int port;

	union {
		unsigned long w32_bits_flags;
		unsigned int mdio_frqdiv;
	};
};

#define STMMAC_MDIO_IO_BASE STMMAC_GMACADDR
#define STMMAC_MDIO_IO_SIZe STMMAC_IOSIZE
#define STMMAC_MDIO_FRQDIV STMMAC_FRQDIV

unsigned int get_phy_device(char *devname, unsigned char phyaddr);
int stmmac_mdio_read(struct stmmac_mdio_local *ld, int phy_addr, int reg);
int stmmac_mdio_write(struct stmmac_mdio_local *ld,
		int phyaddr, int phyreg, unsigned phydata);
int stmmac_mdiobus_driver_init(void);
void stmmac_mdiobus_driver_exit(void);
#endif
