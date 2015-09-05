
#include "hieth.h"
#include "mdio.h"
#include <config.h>

/* MDIO Bus Interface */
static struct hieth_mdio_local mdio_bus_ld;
extern unsigned char U_PHY_ADDR;
extern unsigned char D_PHY_ADDR;
extern char U_PHY_NAME[MAX_PHY_NAME_LEN];
extern char D_PHY_NAME[MAX_PHY_NAME_LEN];
static int hieth_mdiobus_read(char *devname, unsigned char addr, 
		unsigned char reg, unsigned short *value)
{
	*value = hieth_mdio_read(&mdio_bus_ld, addr, reg);

	return 0;
}

static int hieth_mdiobus_write(char *devname, unsigned char addr, 
		unsigned char reg, unsigned short value)
{
	hieth_mdio_write(&mdio_bus_ld, addr, reg, value);
	
	return 0;
}

//added by wzh 2009-4-15
unsigned int get_phy_device(char *devname, unsigned char phyaddr)
{
	u32 phy_id;
	u16 id1, id2;
	
	miiphy_read (devname, phyaddr, PHY_PHYIDR1, &id1);
	miiphy_read (devname, phyaddr, PHY_PHYIDR2, &id2);

	phy_id = (id1 & 0xffff) << 16;
	phy_id |= (id2 & 0xffff);

	/* If the phy_id is all Fs, there is no device there */
	if (0xffffffff == phy_id || 0 == phy_id
		|| phy_id == 0xFFFF || phy_id == 0xFFFF0000)
	{
		return -1;
	}

	/* run this at RMII mode */
	if (HIETH_MII_RMII_MODE_U == 1)
	{
		/* PHY-KSZ8051RNL */
		if ((phy_id & 0xFFFFFFF0) == 0x221550) {
			unsigned short reg;
			miiphy_read(devname, phyaddr, 0x1F, &reg);
			reg |= (1 << 7); /* set phy RMII 50MHz clk;*/
			miiphy_write(devname, phyaddr, 0x1F, reg);

			miiphy_read(devname, phyaddr, 0x16, &reg);
			reg |= (1 << 1); /* set phy RMII override; */
			miiphy_write(devname, phyaddr, 0x16, reg);
		}
	}
	return 0;
}

int hieth_mdiobus_driver_init(void)
{
	mdio_bus_ld.iobase_phys = ETH_IO_ADDRESS_BASE;
	mdio_bus_ld.mdio_frqdiv = ETH_MDIO_FRQDIV;

	hieth_mdio_init(&mdio_bus_ld);
	
	/* UpEther PHY init */
	miiphy_register(U_PHY_NAME, hieth_mdiobus_read, hieth_mdiobus_write);
	
	
	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR))
	{
		miiphy_reset(U_PHY_NAME, U_PHY_ADDR);
	    miiphy_set_current_dev(U_PHY_NAME);
	}
	/* DownEther PHY init */
	miiphy_register(D_PHY_NAME, hieth_mdiobus_read, hieth_mdiobus_write);

	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR))
	{	
		miiphy_reset(D_PHY_NAME, D_PHY_ADDR);
	    miiphy_set_current_dev(D_PHY_NAME);
	}
	return 0;
}

void hieth_mdiobus_driver_exit(void)
{
    /*add this to avoid the first time to use eth will print 'No such device: XXXXX' message.*/
    if (!miiphy_get_current_dev())
        return;

	/* UpEther PHY exit */
	if(!get_phy_device(U_PHY_NAME,U_PHY_ADDR))
	{
		miiphy_reset(U_PHY_NAME, U_PHY_ADDR);
	}
	
	/* DownEther PHY exit */
	if(!get_phy_device(D_PHY_NAME,D_PHY_ADDR))
	{	
		miiphy_reset(D_PHY_NAME, D_PHY_ADDR);
	}
		
	hieth_mdio_exit(&mdio_bus_ld);
}

