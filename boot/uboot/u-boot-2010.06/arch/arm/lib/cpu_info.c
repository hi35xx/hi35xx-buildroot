/******************************************************************************
 *    COPYRIGHT (C) 2013 Hisi. Hisilicon
 *    All rights reserved.
 *
******************************************************************************/

#include <common.h>
#include <command.h>
#include <asm/system.h>
#include <asm/cache.h>
#include <asm/io.h>
#include <config.h>
#include <asm/cpu_info.h>

/*****************************************************************************/

extern struct cpu_info_t cpu_info[];

extern long long get_chipid_reg(void);

extern unsigned int get_ca_vendor_reg(void);
/*****************************************************************************/

static struct cpu_info_t *get_cpuinfo(void)
{
	static struct cpu_info_t *info;
	long long chipid = get_chipid_reg();

	if (info)
		return info;

	for (info = cpu_info; info->name; info++) {
		if ((info->chipid & info->chipid_mask)
			== (chipid & info->chipid_mask))
			return info;
	}
	return cpu_info;
}
/*****************************************************************************/

long long get_chipid(void)
{
	static long long chipid;

	if (!chipid) {
		struct cpu_info_t *info = get_cpuinfo();
		chipid = (info->chipid & info->chipid_mask);
	}

	return chipid;
}
/*****************************************************************************/

int get_bootmedia(char **media, void *arg)
{
	static char *media_name;
	static unsigned int media_type;

	if (!media_name)
		media_type = get_cpuinfo()->boot_media(&media_name);
	if (media)
		(*media) = media_name;

	return media_type;
}
/*****************************************************************************/

unsigned int get_timer_clock(void)
{
	unsigned int timer_clk = 0;
	if (get_cpuinfo()->get_clock)
		get_cpuinfo()->get_clock(NULL, &timer_clk);
	return timer_clk;
}
/*****************************************************************************/

const char *get_cpu_name(void)
{
	return get_cpuinfo()->name;
}
/*****************************************************************************/

unsigned int get_max_ddr_size(void)
{
	return get_cpuinfo()->max_ddr_size;
}
/******************************************************************************/

unsigned int get_ca_vendor(void)
{
	static unsigned long ca_vendor;
	if (ca_vendor)
		return ca_vendor;
	ca_vendor = get_ca_vendor_reg();
	return ca_vendor;
}
/******************************************************************************/

const int get_cpu_version(char **version)
{
	static char *cpu_ver_str;
	static int cpu_ver;

	if (cpu_ver_str)
		goto exit;

	if (get_cpuinfo()->get_cpu_version)
		cpu_ver = get_cpuinfo()->get_cpu_version(&cpu_ver_str);
	else
		cpu_ver_str = "";

exit:
	if (version)
		(*version) = cpu_ver_str;

	return cpu_ver;
}
/*****************************************************************************/

int get_cpudevs(void)
{
	static unsigned int devs;
	if (!devs)
		devs = get_cpuinfo()->devs;
	return (int) devs;
}

