#ifndef __HI_MII_H__
#define __HI_MII_H__
#include <linux/platform_device.h>

extern struct hieth_mdio_local hieth_mdio_local_device;
extern int hieth_mdiobus_driver_init(struct platform_device *pdev);
extern void hieth_mdiobus_driver_exit(void);
#endif
