#ifndef _FORWARD_H
#define _FORWARD_H

#include "higmac.h"

#define FW_MAC_TBL_L		0x0000
#define	FW_MAC_TBL_H		0x0004

#define FW_CTRL			0x0100
#define MAC_TCTRL		0x0104
#define CRF_TRANSFF		0x0108

#define MAC_TAB_CLEAR		0x0114
#define CRF_TX_PRIORITY0	0x011C
#define CRF_TX_PRIORITY1	0x0120

#define FWD_TBL_ENTRY_NUMS	(32)
/* uc use 4 entries, and mc use the rest */
#define FWD_TBL_UC_START	(0)
#define FWD_TBL_UC_NUMS		(CONFIG_GMAC_NUMS)
#define FWD_TBL_MC_START	(FWD_TBL_UC_NUMS)
#define FWD_TBL_MC_NUMS		(FWD_TBL_ENTRY_NUMS - FWD_TBL_UC_NUMS)
#define FWD_TBL_MC_NUMS_EACH_GMAC \
	(FWD_TBL_MC_NUMS / CONFIG_GMAC_NUMS)

/*
 * STANDALONE -- eth0 and eth1 working in stand alone mode
 */
enum work_mode {
	STANDALONE,
	MODE1,
	MODE2,
	MODE3,
	WORK_MODE_NUMS
};

struct fwd_mac_tbl {
	union {
		unsigned char mac[4];
		unsigned int val;
	} mac_tbl_l;

	union {
		struct {
			unsigned int mac4:8;
			unsigned int mac5:8;
			unsigned int to_other:1;
			unsigned int to_cpu:1;
			unsigned int owner:1;
			unsigned int valid:1;
			unsigned int reserved:12;
		} bits;
		unsigned int val;
	} mac_tbl_h;
};

#define ADD_UC		0
#define ADD_MC		1
#define fwd_uc_tbl_del	fwd_mc_tbl_del

void fwd_suspend(struct higmac_adapter *adapter, int mode);
void fwd_resume(struct higmac_adapter *adapter);
void fwd_setup(struct higmac_adapter *adapter);
void fwd_uc_mc_tbl_add(struct higmac_netdev_local *ld,
	const unsigned char *addr, int entry, int mc);
void fwd_mc_tbl_del(struct higmac_netdev_local *ld, int entry);

#endif
