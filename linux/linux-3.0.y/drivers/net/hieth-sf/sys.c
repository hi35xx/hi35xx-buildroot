#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#include "hieth.h"
#include "mdio.h"
#include "mac.h"
#include "ctrl.h"
#include "glb.h"
#include "sys.h"

#ifdef CONFIG_ARCH_GODVIEW
#include "sys-godview.c"
#endif/*CONFIG_ARCH_GODVIEW*/

#ifdef CONFIG_ARCH_GODBOX_V1
#include "sys-godbox-v1.c"
#endif/*CONFIG_ARCH_GODBOX_V1*/

#ifdef CONFIG_ARCH_GODBOX
#include "sys-godbox.c"
#endif/*CONFIG_ARCH_GODBOX_V1*/

#ifdef CONFIG_ARCH_GODNET
#include "sys-godnet.c"
#endif/*CONFIG_ARCH_GODNET*/

#ifdef CONFIG_ARCH_GODCUBE
#include "sys-godcube.c"
#endif/*CONFIG_ARCH_GODCUBE*/

#ifdef CONFIG_ARCH_PHOENIX
#include "sys-phoenix.c"
#endif/*CONFIG_ARCH_PHOENIX*/

#ifdef CONFIG_ARCH_HI3518
#include "sys-3518.c"
#endif/*CONFIG_ARCH_HI3518*/

#ifdef CONFIG_ARCH_HI3520D
#include "sys-hi3520d.c"
#endif/*CONFIG_ARCH_HI3520D*/
/**************************************************/

#ifndef CONFIG_HIETH_INNER_PHY
void revise_led_shine(void)
{
	return;
}
#endif

#ifndef CONFIG_HIETH_INNER_PHY
void set_phy_valtage()
{
	return;
}
#endif
void hieth_sys_allstop(void)
{
	hieth_reset(1);
	hieth_clk_dis();
}

void hieth_sys_suspend(void)
{
	hieth_phy_suspend();

	hieth_clk_dis();
}

void hieth_sys_resume(void)
{
	hieth_phy_resume();

	hieth_clk_ena();
}

void hieth_sys_init(void)
{
	hieth_funsel_config();
	hieth_sys_allstop();
	hieth_phy_reset();
	hieth_clk_ena();
	hieth_reset(1);
	hieth_reset(0);
	revise_led_shine();
}

void hieth_sys_exit(void)
{
	hieth_funsel_restore();
	hieth_sys_allstop();
}

/* vim: set ts=8 sw=8 tw=78: */
