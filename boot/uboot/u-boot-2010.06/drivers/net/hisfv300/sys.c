#include <config.h>
#include "hieth.h"
#include "mdio.h"
#include "mac.h"
#include "ctrl.h"
#include "glb.h"
#include "sys.h"

#ifdef CONFIG_NET_HISFV300_GODBOX_V1
#include "sys-godbox-v1.c"
#endif/*CONFIG_NET_HISFV300_GODBOX_V1*/

#ifdef CONFIG_NET_HISFV300_GODBOX
#include "sys-godbox.c"
#endif/*CONFIG_NET_HISFV300_GODBOX*/

#ifdef CONFIG_NET_HISFV300_GODNET
#include "sys-godnet.c"
#endif/*CONFIG_NET_HISFV300_GODNET*/

#ifdef CONFIG_NET_HISFV300_GODCUBE
#include "sys-godcube.c"
#endif/*CONFIG_NET_HISFV300_GODCUBE*/

#ifdef CONFIG_NET_HISFV300_3518
#include "sys-3518.c"
#endif/*CONFIG_NET_HISFV300_3518*/

#ifdef CONFIG_NET_HISFV300_HI3520D
#include "sys-hi3520d.c"
#endif/*CONFIG_NET_HISFV300_HI3520D*/

#ifndef INNER_PHY
void revise_led_shine(void)
{
	return;
}
#endif

#ifndef INNER_PHY
void set_phy_valtage(void)
{
	return;
}
#endif

/**************************************************/
void hieth_sys_startup(void)
{
	hieth_clk_ena();
	/* undo reset */
	hieth_reset(0);
}

void hieth_sys_allstop(void)
{
}

void hieth_sys_init(void)
{
	hieth_funsel_config();
	hieth_sys_allstop();
	hieth_phy_reset();
	hieth_clk_ena();
	hieth_reset(0);
	revise_led_shine();
}

void hieth_sys_exit(void)
{
	hieth_funsel_restore();
	hieth_sys_allstop();
}
