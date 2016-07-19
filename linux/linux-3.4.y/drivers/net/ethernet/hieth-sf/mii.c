#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#include <linux/io.h>
#include <asm/irq.h>
#include <linux/uaccess.h>

#include "hieth.h"
#include "mdio.h"

static struct mii_bus *hieth_mdiobus;
struct hieth_mdio_local hieth_mdio_local_device;

/* MDIO Bus Interface */

static int hieth_mdiobus_read(struct mii_bus *bus, int phy_addr, int regnum)
{
	return hieth_mdio_read(bus->priv, phy_addr, regnum);
}

static int hieth_mdiobus_write(
		struct mii_bus *bus,
		int phy_addr,
		int regnum,
		u16 val)
{
	return hieth_mdio_write(bus->priv, phy_addr, regnum, val);
}

static int hieth_mdiobus_reset(struct mii_bus *bus)
{
	return hieth_mdio_reset(bus->priv);
}

int hieth_mdiobus_driver_init(struct platform_device *pdev)
{
	int phy, ret = 0;
	struct hieth_mdio_local *ld = &hieth_mdio_local_device;

	ld->iobase = (unsigned long)ioremap_nocache(
			CONFIG_HIETH_IOBASE, CONFIG_HIETH_IOSIZE);
	if (!ld->iobase) {
		hieth_error("ioremap_nocache err, base=0x%.8x, size=0x%.8x\n",
				CONFIG_HIETH_IOBASE, CONFIG_HIETH_IOSIZE);
		ret = -EFAULT;
		goto _error_exit;
	}
	ld->iobase_phys = CONFIG_HIETH_IOBASE;
	ld->mdio_frqdiv = CONFIG_HIETH_MDIO_FRQDIV;

	hieth_mdio_init(ld);

	/* register MII bus */
	hieth_mdiobus = mdiobus_alloc();
	if (!hieth_mdiobus) {
		hieth_error("get ioresource failed!");
		ret = -ENOMEM;
		goto _error_iounmap;
	}

	hieth_mdiobus->name = HIETH_MIIBUS_NAME;
	snprintf(hieth_mdiobus->id, MII_BUS_ID_SIZE, "%s", hieth_mdiobus->name);
	hieth_mdiobus->read =  hieth_mdiobus_read;
	hieth_mdiobus->write = hieth_mdiobus_write;
	hieth_mdiobus->reset = hieth_mdiobus_reset;
	hieth_mdiobus->priv = ld;
	hieth_mdiobus->parent = &pdev->dev;/*for Power Management*/

	hieth_mdiobus->irq = kmalloc(sizeof(int) * PHY_MAX_ADDR, GFP_KERNEL);
	if (!hieth_mdiobus->irq) {
		hieth_error("get ioresource failed!");
		ret = -ENOMEM;
		goto _error_free_mdiobus;
	}

	for (phy = 0; phy < PHY_MAX_ADDR; phy++)
		hieth_mdiobus->irq[phy] = PHY_POLL;

	ret = mdiobus_register(hieth_mdiobus);
	if (ret) {
		hieth_error("failed to register MDIO bus\n");
		goto _error_free_mdiobus;
	}

	return 0;

_error_free_mdiobus:
	kfree(hieth_mdiobus->irq);
	mdiobus_free(hieth_mdiobus);

_error_iounmap:
	iounmap((void *)ld->iobase);

_error_exit:
	return ret;
}

void hieth_mdiobus_driver_exit(void)
{
	struct hieth_mdio_local *ld = &hieth_mdio_local_device;

	mdiobus_unregister(hieth_mdiobus);

	kfree(hieth_mdiobus->irq);

	mdiobus_free(hieth_mdiobus);

	iounmap((void *)ld->iobase);

	hieth_mdio_exit(ld);

	return;
}

/* vim: set ts=8 sw=8 tw=78: */
