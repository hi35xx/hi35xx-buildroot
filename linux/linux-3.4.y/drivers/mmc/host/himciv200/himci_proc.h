/*
 *  MCI connection table manager
 */
#ifndef __MCI_PROC_H__
#define __MCI_PROC_H__

#include <linux/proc_fs.h>

#define MAX_CARD_TYPE	4
#define MAX_SPEED_MODE	5

#ifdef CONFIG_ARCH_HI3518EV200
	#define HIMCI_SLOT_NUM 3
#endif

int mci_proc_init(unsigned int max_connections);
int mci_proc_shutdown(void);

#endif /*  __MCI_PROC_H__ */
