#include "hieth.h"
#include "eee_enable.c"


struct hiphy_info *phy_search_ids(unsigned long phy_id)
{
	int i;
	struct hiphy_info *fit_info = NULL;
	for (i = 0; phy_info_table[i].name != NULL; i++) {
		if (phy_id == phy_info_table[i].phy_id)
			fit_info = &phy_info_table[i];
	}

	return fit_info;
}


#if RTL8211EG_MAC
static struct hiphy_driver  hiphy_driver_rtl8211EG_mac = {
	.eee_enable = phy_realtek_b_mac_eee_enable,
};
#else
static struct hiphy_driver  hiphy_driver_rtl8211EG = {
	.eee_enable = phy_realtek_a_eee_enable,
};
#endif

static struct hiphy_driver  hiphy_driver_smsc_lan8740 = {
	.eee_enable = phy_smsc_a_eee_enable,
};

struct hiphy_info phy_info_table[] = {
	/* phy_name		phy_id	  with_eee phy_driver */
/* SMSC */
	{"SMSC LAN8740",	0x7c110,  MAC_EEE, &hiphy_driver_smsc_lan8740},

#if RTL8211EG_MAC
	{"Realtek 8211EG",	0x1cc915, MAC_EEE, &hiphy_driver_rtl8211EG_mac},
#else
	{"Realtek 8211EG",	0x1cc915, PHY_EEE, &hiphy_driver_rtl8211EG},
#endif

/*	{"Realtek 8201",	0x1cc816,  MAC_EEE, &hiphy_driver_SMSC_a},

	{"IC+ IP101G_DS_R01",	0x2430c54,  MAC_EEE, &hiphy_driver_SMSC_a},
	{"Micrel KSZ8091MNX-RNB",   0x220000,  MAC_EEE, &hiphy_driver_SMSC_a},*/
	{0, 0, 0, 0},
};

