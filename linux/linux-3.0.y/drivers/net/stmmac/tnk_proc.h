/*
 *  TNK connection table manager
 */
#ifndef __TNK_PROC_H__
#define __TNK_PROC_H__

#include <linux/proc_fs.h>
#include <net/sock.h>
#include <net/tnkdrv.h>

int tnk_proc_init(unsigned int max_connections);
int tnk_proc_shutdown(void);

#endif /*  __TNK_PROC_H__ */
