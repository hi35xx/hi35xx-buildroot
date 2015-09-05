#include "mdio.h"
#include "ctrl.h"
#include "stmmac.h"

#ifdef CONFIG_GODNET
#include "sys_godnet.h"
#endif

#ifdef CONFIG_GODARM
#include "sys_godarm.h"
#endif

#ifdef CONFIG_HI3535
#include "sys_hi3535.h"
#endif

#define MII_BUSY 0x00000001
#define MII_WRITE 0x00000002

struct stmmac_mdio_local stmmac_mdio_local_device;

void stmmac_mdio_clk_init(struct stmmac_mdio_local *ld)
{
	unsigned long busclk;
	unsigned long tnkclk;

	busclk = CFG_CLK_BUS;
#ifdef CONFIG_TNK
	tnkclk = get_tnkclk(busclk);
#else
	tnkclk = busclk >> 1;
#endif

	if((tnkclk >= 60000000) && (tnkclk <= 100000000))
		ld->csr_clk = 0;
	else if((tnkclk >= 100000000) && (tnkclk <= 150000000))
		ld->csr_clk = 1;
	else if((tnkclk >= 20000000) && (tnkclk <= 35000000))
		ld->csr_clk = 2;
	else if((tnkclk >= 35000000) && (tnkclk <= 60000000))
		ld->csr_clk = 3;
	else if((tnkclk >= 150000000) && (tnkclk <= 250000000))
		ld->csr_clk = 4;
	else if((tnkclk >= 250000000) && (tnkclk <= 300000000))
		ld->csr_clk = 5;
}

int stmmac_mdio_read(struct stmmac_mdio_local *ld, int phyaddr, int phyreg)
{
	unsigned long ioaddr = ld->iobase_phys;
	unsigned int mii_address = GMAC_GMII_ADDR;
	unsigned int mii_data = GMAC_GMII_DATA;

	int data;

	u16 regValue = (((phyaddr << 11) & (0x0000F800)) |
			((phyreg << 6) & (0x000007C0))) | 
		((ld->csr_clk & 7) << 2);
	regValue |= MII_BUSY;	/* in case of GMAC */

	do {} while (((readl(ioaddr + mii_address)) & MII_BUSY) == 1);
	writel(regValue, ioaddr + mii_address);
	do {} while (((readl(ioaddr + mii_address)) & MII_BUSY) == 1);

	/*Read the data from the MII data register */
	data = (int)readl(ioaddr + mii_data);

	return data;
}

int stmmac_mdio_write(struct stmmac_mdio_local *ld, int phyaddr, int phyreg,
		unsigned phydata)
{
	unsigned long ioaddr = ld->iobase_phys;
	unsigned int mii_address = GMAC_GMII_ADDR;
	unsigned int mii_data = GMAC_GMII_DATA;

	u16 value =
		(((phyaddr << 11) & (0x0000F800)) | ((phyreg << 6)
		& (0x000007C0))) | ((ld->csr_clk & 7) << 2) | MII_WRITE;

	value |= MII_BUSY;

	/*Wait until any existing MII operation is complete */
	do {} while (((readl(ioaddr + mii_address)) & MII_BUSY) == 1);

	/*Set the MII address register to write */
	writel(phydata, ioaddr + mii_data);
	writel(value, ioaddr + mii_address);

	/*Wait until any existing MII operation is complete */
	do {} while (((readl(ioaddr + mii_address)) & MII_BUSY) == 1);

	return 0;
}

static int stmmac_mdiobus_read(char *devname, unsigned char addr,
		unsigned char reg, unsigned short *value)
{
	*value = stmmac_mdio_read(&stmmac_mdio_local_device, addr, reg);

	return 0;
}

static int stmmac_mdiobus_write(char *devname, unsigned char addr,
		unsigned char reg, unsigned short val)
{
	stmmac_mdio_write(&stmmac_mdio_local_device, addr, reg, val);
	return 0;
}

unsigned int get_phy_device(char *devname, unsigned char phyaddr)
{
	u32 phy_id;
	u16 id1, id2;

	miiphy_read(devname, phyaddr, PHY_PHYIDR1, &id1);
	miiphy_read(devname, phyaddr, PHY_PHYIDR2, &id2);

	phy_id = (id1 & 0xffff) << 16;
	phy_id |= (id2 & 0xffff);

	/* If the phy_id is all Fs, there is no device there */
	if (0xffffffff == phy_id || 0 == phy_id
			|| phy_id == 0xFFFF || phy_id == 0xFFFF0000)
		return 1;

	/* PHY-KSZ8051MNL */
	if ((phy_id & 0xFFFFFFF0) == 0x221550) {
		unsigned short reg;
		miiphy_read(devname, phyaddr, 0x1F, &reg);
		/* set phy RMII 50MHz clk; */
		reg |= (1 << 7);
		miiphy_write(devname, phyaddr, 0x1F, reg);

		/* set phy RMII override; */
		miiphy_read(devname, phyaddr, 0x16, &reg);
		reg |= (1 << 1);
		miiphy_write(devname, phyaddr, 0x16, reg);
	}

	return 0;
}

int stmmac_mdiobus_driver_init(void)
{
	stmmac_mdio_local_device.iobase_phys = STMMAC_MDIO_IO_BASE;
	stmmac_mdio_local_device.iobase = STMMAC_MDIO_IO_BASE;

	stmmac_mdio_clk_init(&stmmac_mdio_local_device);

	/* GMAC0 PHY init */
	miiphy_register(GMAC0_PHY_NAME, stmmac_mdiobus_read,
			stmmac_mdiobus_write);

	if (!get_phy_device(GMAC0_PHY_NAME, GMAC0_PHY_ADDR))
		miiphy_set_current_dev(GMAC0_PHY_NAME);

	return 0;
}

void stmmac_mdiobus_driver_exit(void)
{
	if (!miiphy_get_current_dev())
		return;
}
