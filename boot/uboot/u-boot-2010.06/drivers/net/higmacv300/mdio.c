#include "mdio.h"
#include "util.h"
#include "higmac.h"

static struct higmac_mdio_local higmac_mdio0;
static struct higmac_mdio_local higmac_mdio1;

static int wait_mdio_ready(struct higmac_mdio_local *ld)
{
	int timeout_us = 1000;

	while (--timeout_us && !test_mdio_ready(ld))
		udelay(1);

	return timeout_us;
}

static int higmac_mdio_read(struct higmac_mdio_local *ld,
		char *devname, unsigned char phy,
		unsigned char reg, unsigned short *value)
{
	int timeout = 1000;

	if (!wait_mdio_ready(ld))
		return -1;

	mdio_start_phyread(ld, phy, reg);

	while (!wait_mdio_ready(ld) && timeout-- > 0)
		udelay(1);

	if (timeout <= 0 || !test_mdio_read_data_done(ld)) {
		*value = 0;
		/* it should return Error(-1), but miiphy_read() will
		 * print error info, it's annoying
		 */
		return 0;
	}

	*value = mdio_get_phyread_val(ld);

	higmac_trace(2, "mdio read phy:%x, reg:%x = %x\n", phy, reg, *value);

	return 0;
}

static int higmac_mdio_write(struct higmac_mdio_local *ld,
		char *devname, unsigned char phy,
		unsigned char reg, unsigned short val)
{
	if (!wait_mdio_ready(ld))
		return -1;

	higmac_trace(2, "mdio write phy:%x, reg:%x = %x\n", phy, reg, val);

	mdio_set_phywrite_val(ld, val);
	mdio_phywrite(ld, phy, reg);

	return 0;
}

static void higmac_mdio_reset(struct higmac_mdio_local *ld)
{
	/* FIXME: mdiobus need a reset? */
}

static void higmac_mdio_init(struct higmac_mdio_local *ld)
{
	higmac_mdio_reset(ld);
}

static void higmac_mdio_exit(struct higmac_mdio_local *ld)
{
}

int higmac0_mdio_read(char *devname, unsigned char phy,
		unsigned char reg, unsigned short *value)
{
	return higmac_mdio_read(&higmac_mdio0, devname, phy, reg, value);
}

int higmac1_mdio_read(char *devname, unsigned char phy,
		unsigned char reg, unsigned short *value)
{
	return higmac_mdio_read(&higmac_mdio1, devname, phy, reg, value);
}

int higmac0_mdio_write(char *devname, unsigned char phy,
		unsigned char reg, unsigned short val)
{
	return higmac_mdio_write(&higmac_mdio0, devname, phy, reg, val);
}

int higmac1_mdio_write(char *devname, unsigned char phy,
		unsigned char reg, unsigned short val)
{
	return higmac_mdio_write(&higmac_mdio1, devname, phy, reg, val);
}

void higmac_mdiobus_driver_init(void)
{
	higmac_mdio0.iobase_phys = HIGMAC0_MDIO_IO_BASE;
	higmac_mdio0.iobase = HIGMAC0_MDIO_IO_BASE;
	higmac_mdio_init(&higmac_mdio0);

#if (CONFIG_GMAC_NUMS == 2)
	higmac_mdio_init(&higmac_mdio1);
	higmac_mdio1.iobase_phys = HIGMAC1_MDIO_IO_BASE;
	higmac_mdio1.iobase = HIGMAC1_MDIO_IO_BASE;
#endif
}

void higmac_mdiobus_driver_exit(void)
{
	if (!miiphy_get_current_dev())
		return;

	higmac_mdio_exit(&higmac_mdio0);
	if (CONFIG_GMAC_NUMS == 2)
		higmac_mdio_exit(&higmac_mdio1);
}
