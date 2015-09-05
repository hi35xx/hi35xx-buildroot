/******************************************************************************
 *    COPYRIGHT (C) 2013 Hisi. Hisilicon
 *    All rights reserved.
 *
******************************************************************************/

#ifndef CPU_INFOH
#define CPU_INFOH
/******************************************************************************/

struct cpu_info_t {
	char        *name;
	long long    chipid;
	long long    chipid_mask;
	unsigned int max_ddr_size;

	unsigned int devs;

	int (*boot_media)(char **media);

	void (*get_clock)(unsigned int *cpu, unsigned int *timer);

	int (*get_cpu_version)(char **version);
};

/******************************************************************************/
#endif /* CPU_INFOH */

