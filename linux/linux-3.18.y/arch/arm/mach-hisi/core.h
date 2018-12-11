#ifndef __HISILICON_CORE_H
#define __HISILICON_CORE_H

#include <linux/reboot.h>

extern void hi3xxx_set_cpu_jump(int cpu, void *jump_addr);
extern int hi3xxx_get_cpu_jump(int cpu);
extern void secondary_startup(void);
extern struct smp_operations hi3xxx_smp_ops;

extern void hi3xxx_cpu_die(unsigned int cpu);
extern int hi3xxx_cpu_kill(unsigned int cpu);
extern void hi3xxx_set_cpu(int cpu, bool enable);

extern void hix5hd2_secondary_startup(void);
extern struct smp_operations hix5hd2_smp_ops;
extern void hix5hd2_set_cpu(int cpu, bool enable);
extern void hix5hd2_cpu_die(unsigned int cpu);

extern void hi3519_secondary_startup(void);
extern void hi3519_cpu_die(unsigned int cpu);
extern int  hi3519_cpu_kill(unsigned int cpu);

extern void hi3516av200_secondary_startup(void);
extern void hi3516av200_cpu_die(unsigned int cpu);
extern int  hi3516av200_cpu_kill(unsigned int cpu);

extern void hi3559_secondary_startup(void);
extern void hi3559_cpu_die(unsigned int cpu);
extern int  hi3559_cpu_kill(unsigned int cpu);

extern void hi3536c_secondary_startup(void);
extern void hi3536c_cpu_die(unsigned int cpu);
extern int  hi3536c_cpu_kill(unsigned int cpu);
void hi3536c_scu_power_up(int cpu);

extern void hi_pmc_power_up(void);
extern void hi_pmc_power_up_done(void);
extern void hi_pmc_set_ac_inactive(void);
extern void hi_pmc_power_down(void);
#ifdef CONFIG_ARCH_HI3519
extern void hi_pmc_kill_cpu(unsigned int cpu);
#endif
#endif
