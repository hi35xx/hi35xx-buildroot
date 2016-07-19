#ifndef __HIETH_SYS_H
#define __HIETH_SYS_H

void hieth_sys_init(void);
void hieth_sys_exit(void);
void hieth_sys_allstop(void);
void hieth_sys_suspend(void);
void hieth_sys_resume(void);
int hieth_port_reset(struct hieth_netdev_local *ld, int port);
void set_phy_valtage(void);

#endif

/* vim: set ts=8 sw=8 tw=78: */
