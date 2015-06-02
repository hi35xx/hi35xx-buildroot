#ifndef __PLATSMP__H__
#define __PLATSMP__H__

#define SMP_COREX_START_ADDR_REG        0x20050134/* see bootloader */

extern void godnet_secondary_startup(void);
extern int __cpuinitdata pen_release;

#endif

