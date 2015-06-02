#include <linux/delay.h>
#include <linux/kernel.h>
#include "mdio.h"
#include "mii.h"

#define RTL8211EG_MAC	0

int phyaddr = 0x1;
static int flag = 1;
int data_tmp;


static int phy_smsc_a_eee_enable(void)
{

	if (flag == 1) {
		/* Realtek LAN 8740 start to enable eee */
		int eee_lan = hieth_mdio_read(&hieth_mdio_local_device,
			phyaddr, 0x10);
		eee_lan |= 0x4;
		hieth_mdio_write(&hieth_mdio_local_device,
			phyaddr, 0x10, eee_lan);
		eee_lan = hieth_mdio_read(&hieth_mdio_local_device,
			phyaddr, 0x10);
		if (DEBUG)
			printk(KERN_INFO "eee enable bit[45?] :%x\n", eee_lan);
		/* auto negotiate after enable eee*/
		eee_lan = hieth_mdio_read(&hieth_mdio_local_device,
				phyaddr, 0x0);
		eee_lan |= 0x200;
		hieth_mdio_write(&hieth_mdio_local_device,
				phyaddr, 0x0, eee_lan);

		flag = 0;
	}


	/* read linkpart */
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xd, 0x7);
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xe, 0x3d);
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xd, 0x4007);
	data_tmp = hieth_mdio_read(&hieth_mdio_local_device, phyaddr, 0xe);

	data_tmp &= 0x2;
	if (DEBUG)
		printk(KERN_INFO "phy_smsc_a_eee_enable()  return: %x\n",
			data_tmp);
	return data_tmp;
}

#if RTL8211EG_MAC
static int phy_realtek_b_mac_eee_enable(void)
{
	/* Realtek 8211EG start reset to change eee to mac */
	if (flag == 1) {
		int phyaddr = 0x1;
		int tmp = 0;
		hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0x1f, 0x0);
		hieth_mdio_write(&hieth_mdio_local_device,
			phyaddr, 0x0, 0x8000);
		do {
			/* wait phy restart over */
			udelay(1000);
			tmp = hieth_mdio_read(&hieth_mdio_local_device,
				phyaddr, 0x1);
			tmp &= 40;
		} while (!tmp);

		hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0x1f, 0x7);
		hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0x1e, 0x20);
		hieth_mdio_write(&hieth_mdio_local_device, phyaddr,
			0x1b, 0xa03a);
		hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0x1f, 0x0);
		flag = 0;
	}

	/* configure phy reg */
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xd, 0x7);
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xe, 0x3d);
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xd, 0x4007);
	data_tmp = hieth_mdio_read(&hieth_mdio_local_device, phyaddr, 0xe);
	if (DEBUG)
		printk(KERN_INFO "eee_enable() link parter is[2?]:%x\n",
			data_tmp);
	data_tmp &= 0x2;

	if (DEBUG)
		printk(KERN_INFO "phy_realtek_a_eee_enable() return: %x\n",
			data_tmp);
	return data_tmp;

}
#else
static int phy_realtek_a_eee_enable(void)
{
	/* configure phy reg */
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xd, 0x7);
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xe, 0x3d);
	hieth_mdio_write(&hieth_mdio_local_device, phyaddr, 0xd, 0x4007);
	data_tmp = hieth_mdio_read(&hieth_mdio_local_device, phyaddr, 0xe);
	data_tmp &= 0x2;
	if (DEBUG)
		printk(KERN_INFO "phy_realtek_a_eee_enable()   return: %x\n",
			data_tmp);
	return data_tmp;

}
#endif
