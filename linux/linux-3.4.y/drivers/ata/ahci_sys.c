#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/io.h>

#ifdef CONFIG_HI_SATA_V100
#include "hisatav100.h"
#endif/*CONFIG_HI_SATA_V100*/

#ifdef CONFIG_HI_SATA_V200
#include "hisatav200.h"
#endif/*CONFIG_HI_SATA_V200*/

#ifdef CONFIG_ARCH_HI3535
#include "ahci_sys_hi3535_defconfig.c"
#endif/*CONFIG_ARCH_HI3535*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon osdrv group");

#ifndef CONFIG_HI_SATA_V200
static int phy_config = CONFIG_HI_SATA_PHY_CONFIG;
#endif
static int n_ports = CONFIG_HI_SATA_PORTS;
static int phy_mode = CONFIG_HI_SATA_MODE;

#ifdef MODULE
module_param(phy_config, uint, 0600);
MODULE_PARM_DESC(phy_config, "sata phy config (default:0x0e180000)");
module_param(n_ports, uint, 0600);
MODULE_PARM_DESC(n_ports, "sata port number (default:2)");
module_param(mode_3g, uint, 0600);
MODULE_PARM_DESC(phy_mode, "sata phy mode (0:1.5G(default);1:3G;2:6G)");
#endif

void hi_sata_init(void __iomem *mmio)
{
	int port_num;

	port_num = n_ports;

#ifdef CONFIG_ARCH_HI3535
	if (!(hi_sata_use_esata()))
		port_num += 1;
#endif
	hi_sata_poweron();
	msleep(20);
	hi_sata_clk_enable();
	hi_sata_phy_clk_sel();
	hi_sata_unreset();
	msleep(20);
	hi_sata_phy_unreset();
	msleep(20);
	hi_sata_phy_reset();
	msleep(20);
	hi_sata_phy_unreset();
	msleep(20);
	hi_sata_clk_unreset();
	msleep(20);
#ifdef CONFIG_HI_SATA_V100
	hisata_v100_init(mmio, phy_mode, prot_num);
#endif
#ifdef CONFIG_HI_SATA_V200
	hisata_v200_init(mmio, phy_mode, port_num);
#endif
}
EXPORT_SYMBOL(hi_sata_init);

void hi_sata_exit(void)
{
	hi_sata_phy_reset();
	msleep(20);
	hi_sata_reset();
	msleep(20);
	hi_sata_clk_reset();
	msleep(20);
	hi_sata_clk_disable();
	hi_sata_poweroff();
	msleep(20);
}
EXPORT_SYMBOL(hi_sata_exit);
