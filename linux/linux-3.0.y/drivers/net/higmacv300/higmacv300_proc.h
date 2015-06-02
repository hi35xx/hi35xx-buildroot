#ifndef __GMAC_PROC_H__
#define __GMAC_PROC_H__

#include <linux/proc_fs.h>
#include <net/sock.h>

int gmac_proc_init(void);
int gmac_proc_shutdown(void);

#endif /*  __GMAC_PROC_H__ */
