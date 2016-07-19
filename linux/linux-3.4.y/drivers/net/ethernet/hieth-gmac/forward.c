#include <linux/atomic.h>
#include "forward.h"
#include "util.h"
#include "ctrl.h"

#if (CONFIG_GMAC_NUMS == 2)/* forwarding is only meaningful when mac nums = 2 */

#define ETH0		0
#define ETH1		1
#define ETH0_IS_MASTER	0
#define ETH1_IS_MASTER	1

struct fwd_hw_config {
	union {
		unsigned int fw_ctrl_config;
		struct {
			unsigned int reserved_bits_0_3:4;
			unsigned int eth1_permit_fwd_all_to_other:1;
			unsigned int eth1_permit_fwd_all_to_cpu:1;
			unsigned int eth1_fwd_all_to_other:1;
			unsigned int eth1_fwd_all_to_cpu:1;

			unsigned int eth0_permit_fwd_all_to_other:1;
			unsigned int eth0_permit_fwd_all_to_cpu:1;
			unsigned int eth0_fwd_all_to_other:1;
			unsigned int eth0_fwd_all_to_cpu:1;
			unsigned int reserved_bits_12_31:20;
		} fw_ctrl_bits;
	} fw_ctrl;

	union {
		unsigned int mac_tctrl_config;
		struct {
			unsigned int eth1_uc_tbl_miss_to_other:1;
			unsigned int eth1_uc_tbl_miss_to_cpu:1;
			unsigned int eth1_mc_tbl_miss_to_other:1;
			unsigned int eth1_mc_tbl_miss_to_cpu:1;
			unsigned int eth1_bc_to_other:1;
			unsigned int eth1_bc_to_cpu:1;
			unsigned int reserved_bit_6:1;
			unsigned int eth1_uc_tbl_enable:1;
			unsigned int eth1_mc_tbl_enable:1;

			unsigned int eth0_uc_tbl_miss_to_other:1;
			unsigned int eth0_uc_tbl_miss_to_cpu:1;
			unsigned int eth0_mc_tbl_miss_to_other:1;
			unsigned int eth0_mc_tbl_miss_to_cpu:1;
			unsigned int eth0_bc_to_other:1;
			unsigned int eth0_bc_to_cpu:1;
			unsigned int reserved_bit_15:1;
			unsigned int eth0_uc_tbl_enable:1;
			unsigned int eth0_mc_tbl_enable:1;

			unsigned int reserved_bits_18_31:14;
		} mac_tctrl_bits;
	} mac_tctrl;
};

union fwd_usr_config {
	unsigned int port[CONFIG_GMAC_NUMS];/* eth0 and eth1 */
	struct {
		/* level 1 control */
		unsigned int fwd_all_to_cpu:1;
		unsigned int fwd_all_to_other:1;

		/* level 2 control */
		unsigned int uc_tbl_enable:1;
		unsigned int uc_tbl_match_to_cpu:1;
		unsigned int uc_tbl_match_to_other:1;

		/* same bit for uc_to_cpu */
		unsigned int uc_tbl_miss_to_cpu:1;
		/* same bit for uc_to_other */
		unsigned int uc_tbl_miss_to_other:1;
/* when uc_tbl_enable = 0*/
#define uc_to_cpu	uc_tbl_miss_to_cpu
#define uc_to_other	uc_tbl_miss_to_other

		unsigned int mc_tbl_enable:1;
		unsigned int mc_tbl_match_to_cpu:1;
		unsigned int mc_tbl_match_to_other:1;
		/* same bit for mc_to_cpu */
		unsigned int mc_tbl_miss_to_cpu:1;
		/* same bit for mc_to_other */
		unsigned int mc_tbl_miss_to_other:1;
/* when mc_tbl_enable = 0*/
#define mc_to_cpu	mc_tbl_miss_to_cpu
#define mc_to_other	mc_tbl_miss_to_other

		unsigned int bc_to_cpu:1;
		unsigned int bc_to_other:1;
	} usr_config[CONFIG_GMAC_NUMS];
};

void fwd_hw_config(struct higmac_adapter *adapter, union fwd_usr_config *cfg)
{
	struct fwd_hw_config hw_cfg;

	memset(&hw_cfg, 0, sizeof(hw_cfg));

#define eth0		(cfg->usr_config[ETH0])
#define eth1		(cfg->usr_config[ETH1])
#define fwctrl		(hw_cfg.fw_ctrl.fw_ctrl_bits)
	/* level 1 config: FW_CTRL */
	fwctrl.eth1_permit_fwd_all_to_other	= 1;
	fwctrl.eth1_permit_fwd_all_to_cpu	= 1;
	fwctrl.eth0_permit_fwd_all_to_other	= 1;
	fwctrl.eth0_permit_fwd_all_to_cpu	= 1;

	fwctrl.eth1_fwd_all_to_other	= eth1.fwd_all_to_other;
	fwctrl.eth1_fwd_all_to_cpu	= eth1.fwd_all_to_cpu;
	fwctrl.eth0_fwd_all_to_other	= eth0.fwd_all_to_other;
	fwctrl.eth0_fwd_all_to_cpu	= eth0.fwd_all_to_cpu;
#undef fwctrl

#define tctrl		(hw_cfg.mac_tctrl.mac_tctrl_bits)
	tctrl.eth1_uc_tbl_miss_to_other	= eth1.uc_tbl_miss_to_other;
	tctrl.eth1_uc_tbl_miss_to_cpu	= eth1.uc_tbl_miss_to_cpu;
	tctrl.eth1_mc_tbl_miss_to_other	= eth1.mc_tbl_miss_to_other;
	tctrl.eth1_mc_tbl_miss_to_cpu	= eth1.mc_tbl_miss_to_cpu;
	tctrl.eth1_bc_to_other		= eth1.bc_to_other;
	tctrl.eth1_bc_to_cpu		= eth1.bc_to_cpu;
	tctrl.eth1_uc_tbl_enable	= eth1.uc_tbl_enable;
	tctrl.eth1_mc_tbl_enable	= eth1.mc_tbl_enable;

	tctrl.eth0_uc_tbl_miss_to_other	= eth0.uc_tbl_miss_to_other;
	tctrl.eth0_uc_tbl_miss_to_cpu	= eth0.uc_tbl_miss_to_cpu;
	tctrl.eth0_mc_tbl_miss_to_other	= eth0.mc_tbl_miss_to_other;
	tctrl.eth0_mc_tbl_miss_to_cpu	= eth0.mc_tbl_miss_to_cpu;
	tctrl.eth0_bc_to_other		= eth0.bc_to_other;
	tctrl.eth0_bc_to_cpu		= eth0.bc_to_cpu;
	tctrl.eth0_uc_tbl_enable	= eth0.uc_tbl_enable;
	tctrl.eth0_mc_tbl_enable	= eth0.mc_tbl_enable;
#undef tctrl
#undef eth0
#undef eth1

	writel(hw_cfg.fw_ctrl.fw_ctrl_config,
			adapter->fwdctl_iobase + FW_CTRL);
	writel(hw_cfg.mac_tctrl.mac_tctrl_config,
			adapter->fwdctl_iobase + MAC_TCTRL);
}

/*
 *	work_mode
 * ---------------------
 *  fwd_usr_config[][]
 *  --------------------
 *  fwd_hw_config()
 */
union fwd_usr_config fwd_usr_config[WORK_MODE_NUMS][CONFIG_GMAC_NUMS];
void fwd_usr_config_init(void)
{
	union fwd_usr_config *cfg;

	memset(&fwd_usr_config, 0, sizeof(fwd_usr_config));

	/* STANDALONE */
	/* FWCTRL = 0xbb0, MAC_TCTRL = 0 */
	cfg = &fwd_usr_config[STANDALONE][ETH0];
	cfg->usr_config[ETH0].fwd_all_to_cpu = 1;
	cfg->usr_config[ETH1].fwd_all_to_cpu = 1;

	/* MODE1 */
	/* FWCTRL = 0x770, MAC_TCTRL = 0x1a0 */
	cfg = &fwd_usr_config[MODE1][ETH0_IS_MASTER]; /* master--eth0 */
	cfg->usr_config[ETH0].fwd_all_to_other = 1;
	cfg->usr_config[ETH0].bc_to_cpu = 1;
	cfg->usr_config[ETH0].uc_tbl_enable = 1;
	cfg->usr_config[ETH0].uc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH0].mc_tbl_enable = 1;
	cfg->usr_config[ETH0].mc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH1].fwd_all_to_other = 1;

	/* FWCTRL = 0x770, MAC_TCTRL = 0x34000 */
	cfg = &fwd_usr_config[MODE1][ETH1_IS_MASTER]; /* master--eth1 */
	cfg->usr_config[ETH1].fwd_all_to_other = 1;
	cfg->usr_config[ETH1].bc_to_cpu = 1;
	cfg->usr_config[ETH1].uc_tbl_enable = 1;
	cfg->usr_config[ETH1].uc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH1].mc_tbl_enable = 1;
	cfg->usr_config[ETH1].mc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH0].fwd_all_to_other = 1;

	/* MODE2 */
	/* FWCTRL = 0x370, MAC_TCTRL = 0x36a00 */
	cfg = &fwd_usr_config[MODE2][ETH0_IS_MASTER]; /* master--eth0 */
	cfg->usr_config[ETH0].bc_to_cpu = 1;
	cfg->usr_config[ETH0].bc_to_other = 1;
	cfg->usr_config[ETH0].uc_tbl_enable = 1;
	cfg->usr_config[ETH0].uc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH0].uc_tbl_miss_to_other = 1;
	cfg->usr_config[ETH0].mc_tbl_enable = 1;
	cfg->usr_config[ETH0].mc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH0].mc_tbl_miss_to_other = 1;
	cfg->usr_config[ETH1].fwd_all_to_other = 1;

	/* FWCTRL = 0x730, MAC_TCTRL = 0x1b5 */
	cfg = &fwd_usr_config[MODE2][ETH1_IS_MASTER]; /* master--eth1 */
	cfg->usr_config[ETH1].bc_to_cpu = 1;
	cfg->usr_config[ETH1].bc_to_other = 1;
	cfg->usr_config[ETH1].uc_tbl_enable = 1;
	cfg->usr_config[ETH1].uc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH1].uc_tbl_miss_to_other = 1;
	cfg->usr_config[ETH1].mc_tbl_enable = 1;
	cfg->usr_config[ETH1].mc_tbl_match_to_cpu = 1;
	cfg->usr_config[ETH1].mc_tbl_miss_to_other = 1;
	cfg->usr_config[ETH0].fwd_all_to_other = 1;

	/* MODE3 */
	/* FWCTRL = 0x770, MAC_TCTRL = 0x0 */
	cfg = &fwd_usr_config[MODE3][ETH0];
	cfg->usr_config[ETH0].fwd_all_to_other = 1;
	cfg->usr_config[ETH1].fwd_all_to_other = 1;
}

/* clear all */
void fwd_uc_mc_tbl_clear(struct higmac_adapter *adapter)
{
	writel(1, adapter->fwdctl_iobase + MAC_TAB_CLEAR);
	writel(0, adapter->fwdctl_iobase + MAC_TAB_CLEAR);
}

/*
 * init or re-init forward module
 * clear mc filter table!
 */
static union fwd_usr_config *cur_cfg;
void fwd_setup(struct higmac_adapter *adapter)
{
	static int inited;

	if (!inited) {
		inited = 1;
		fwd_usr_config_init();
	}

	fwd_uc_mc_tbl_clear(adapter);

	cur_cfg = &fwd_usr_config[adapter->work_mode][adapter->master];
	fwd_hw_config(adapter, cur_cfg);
}

#define WORK_MODE_BACKUP_INIT	(-1)
static int work_mode_backup = WORK_MODE_BACKUP_INIT;
void fwd_suspend(struct higmac_adapter *adapter, int work_mode)
{
	work_mode_backup = adapter->work_mode;

	if (work_mode != adapter->work_mode) {
		adapter->work_mode = work_mode;
		fwd_setup(adapter);
	}
}

void fwd_resume(struct higmac_adapter *adapter)
{
	if (work_mode_backup == WORK_MODE_BACKUP_INIT) {
		pr_info("wired! call suspend() func before resume()!\n");
		return;
	}
	adapter->work_mode = work_mode_backup;
	fwd_setup(adapter);
	work_mode_backup = WORK_MODE_BACKUP_INIT;
}

/* delete one entry */
void fwd_mc_tbl_del(struct higmac_netdev_local *ld, int entry)
{
	struct higmac_adapter *adapter = ld->adapter;

	entry = entry * 8;/* each entry size = 8Bytes */
	/* clear valid bit */
	writel(0, adapter->fwdctl_iobase + FW_MAC_TBL_H + entry);
}

void fwd_uc_mc_tbl_add(struct higmac_netdev_local *ld,
		const unsigned char *addr, int entry, int mc)
{
	struct higmac_adapter *adapter = ld->adapter;
	unsigned int val = 0, mac;
	int match_to_cpu, match_to_other;

	fwd_mc_tbl_del(ld, entry);/* clear the previous */

	if (mc) {
		match_to_cpu = cur_cfg->usr_config[ld->index].
			mc_tbl_match_to_cpu;
		match_to_other = cur_cfg->usr_config[ld->index].
			mc_tbl_match_to_other;
	} else {/* uc */
		match_to_cpu = cur_cfg->usr_config[ld->index].
			uc_tbl_match_to_cpu;
		match_to_other = cur_cfg->usr_config[ld->index].
			uc_tbl_match_to_other;
	}

	/*
	 * match but won't forward to cpu and other port, so forget it
	 * however it has some side effect: it will goto miss action now
	 */
	if (!match_to_cpu && !match_to_other)
		return;

	val |= 1 << 19;/* valid entry */
	val |= ld->index << 18;/* eth0 or eth1 use this entry */
	val |= match_to_cpu << 17;/* to cpu */
	val |= match_to_other << 16;/* fwd to the other port or not */

	entry = entry * 8;/* each entry size = 8Bytes */

	mac = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5];
	writel(mac, adapter->fwdctl_iobase + FW_MAC_TBL_L + entry);

	val |= addr[0] << 8 | addr[1];
	writel(val, adapter->fwdctl_iobase + FW_MAC_TBL_H + entry);
}

void fwd_enter_promisc_mode(struct higmac_netdev_local *ld)
{
	struct higmac_adapter *adapter = ld->adapter;
	unsigned int val;

	if (adapter->master == ld->index) {
		switch (adapter->work_mode) {
		case MODE1:
		case MODE2:
			val = readl(adapter->fwdctl_iobase + FW_CTRL);
			if (ld->index) /* gmac1 */
				val |= 1 << 7;/* gmac1 fwd all to cpu */
			else
				val |= 1 << 11;/* gmac0 fwd all to cpu */
			writel(val, adapter->fwdctl_iobase + FW_CTRL);
			break;
		case STANDALONE:
		case MODE3:
		default:
			break;
		}
	}
}

void fwd_exit_promisc_mode(struct higmac_netdev_local *ld)
{
	struct higmac_adapter *adapter = ld->adapter;
	unsigned int val;

	if (adapter->master == ld->index) {
		switch (adapter->work_mode) {
		case MODE1:
		case MODE2:
			val = readl(adapter->fwdctl_iobase + FW_CTRL);
			if (ld->index) /* gmac1 */
				val &= ~(1 << 7);/* gmac1 fwd all to cpu */
			else
				val &= ~(1 << 11);/* gmac0 fwd all to cpu */
			writel(val, adapter->fwdctl_iobase + FW_CTRL);
			break;
		case STANDALONE:
		case MODE3:
		default:
			break;
		}
	}
}
#else
void fwd_suspend(struct higmac_adapter *adapter, int mode) {};
void fwd_resume(struct higmac_adapter *adapter) {};
void fwd_setup(struct higmac_adapter *adapter) {};
void fwd_uc_mc_tbl_add(struct higmac_netdev_local *ld,
	const unsigned char *addr, int entry, int mc) {};
void fwd_mc_tbl_del(struct higmac_netdev_local *ld, int entry) {};
#endif
