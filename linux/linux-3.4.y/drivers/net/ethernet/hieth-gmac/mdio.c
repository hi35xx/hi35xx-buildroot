#include "mdio.h"
#include "util.h"
#include "higmac.h"

static int wait_mdio_ready(struct higmac_netdev_local *ld)
{
	int timeout_us = 1000;

	while (--timeout_us && !test_mdio_ready(ld))
		udelay(1);

	return timeout_us;
}

static int higmac_mdio_read(struct mii_bus *bus, int phy, int reg)
{
	struct higmac_netdev_local *ld = bus->priv;
	int timeout = 1000;
	int val;

	if (!wait_mdio_ready(ld))
		return 0;

	mdio_start_phyread(ld, phy, reg);

	while (!wait_mdio_ready(ld) && timeout-- > 0)
		udelay(1);

	if (timeout <= 0 || !test_mdio_read_data_done(ld))
		return 0;

	val = mdio_get_phyread_val(ld);

	higmac_trace(2, "mdio read phy:%x, reg:%x = %x\n", phy, reg, val);

	return val;
}

static int higmac_mdio_write(struct mii_bus *bus, int phy, int reg, u16 val)
{
	struct higmac_netdev_local *ld = bus->priv;

	if (!wait_mdio_ready(ld))
		return 0;

	higmac_trace(2, "mdio write phy:%x, reg:%x = %x\n", phy, reg, val);

	mdio_set_phywrite_val(ld, val);
	mdio_phywrite(ld, phy, reg);

	return val;
}

static int higmac_mdio_reset(struct mii_bus *bus)
{
	/* mdio clk enable? reset? */
	return 0;
}

static inline int higmac_mdio_init(struct higmac_netdev_local *ld)
{
	/* clk enable? unreset? */
	return 0;
}

static inline void higmac_mdio_exit(struct higmac_netdev_local *ld)
{
}

int higmac_mdiobus_driver_init(struct higmac_netdev_local *ld, char *bus_id)
{
	struct mii_bus *mii_bus;
	int ret;

	higmac_mdio_init(ld);

	/* register MII bus */
	mii_bus = mdiobus_alloc();
	if (mii_bus == NULL) {
		pr_err("alloc mii bus failed!\n");
		ret = -ENOMEM;
		goto _error_bus_alloc;
	}

	mii_bus->name = "higmac_mdio_bus";
	snprintf(mii_bus->id, MII_BUS_ID_SIZE, "%s", bus_id);
	mii_bus->read =  higmac_mdio_read;
	mii_bus->write = higmac_mdio_write;
	mii_bus->reset = higmac_mdio_reset;
	mii_bus->priv = ld;

	ret = mdiobus_register(mii_bus);
	if (ret) {
		pr_err("failed to register MDIO bus\n");
		goto _error_register_bus;
	}

	ld->mii_bus_registered = mii_bus;
	ld->mii_bus_using      = mii_bus;

	return 0;

_error_register_bus:
	mdiobus_free(mii_bus);
_error_bus_alloc:

	return ret;
}

void higmac_mdiobus_driver_exit(struct higmac_netdev_local *ld)
{
	if (ld == NULL || ld->mii_bus_registered == NULL)
		return;

	mdiobus_unregister(ld->mii_bus_registered);

	mdiobus_free(ld->mii_bus_registered);

	higmac_mdio_exit(ld);

	ld->mii_bus_registered = NULL;
	ld->mii_bus_using      = NULL;

	return;
}
