/*
 * (Hisilicon's SoC based) flattened device tree enabled machine
 *
 * Copyright (c) 2012-2013 Hisilicon Ltd.
 * Copyright (c) 2012-2013 Linaro Ltd.
 *
 * Author: Haojian Zhuang <haojian.zhuang@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#include <linux/irqchip.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <mach/io.h>

#define HI3620_SYSCTRL_PHYS_BASE		0xfc802000
#define HI3620_SYSCTRL_VIRT_BASE		0xfe802000

/*
 * This table is only for optimization. Since ioremap() could always share
 * the same mapping if it's defined as static IO mapping.
 *
 * Without this table, system could also work. The cost is some virtual address
 * spaces wasted since ioremap() may be called multi times for the same
 * IO space.
 */
static struct map_desc hi3620_io_desc[] __initdata = {
	{
		/* sysctrl */
		.pfn		= __phys_to_pfn(HI3620_SYSCTRL_PHYS_BASE),
		.virtual	= HI3620_SYSCTRL_VIRT_BASE,
		.length		= 0x1000,
		.type		= MT_DEVICE,
	},
};

static void __init hi3620_map_io(void)
{
	debug_ll_io_init();
	iotable_init(hi3620_io_desc, ARRAY_SIZE(hi3620_io_desc));
}

extern unsigned long long notrace sched_clock(void);
unsigned long long hi_sched_clock(void)
{
	return sched_clock();
}
EXPORT_SYMBOL(hi_sched_clock);

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
static struct map_desc hi3519_io_desc[] __initdata = {
	/* hi3519_IOCH1 */
	{
		.virtual        = HI3519_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(HI3519_IOCH1_PHYS),
		.length         = HI3519_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3519_IOCH2 */
	{
		.virtual        = HI3519_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3519_IOCH2_PHYS),
		.length         = HI3519_IOCH2_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3519_IOCH2 */
	{
		.virtual        = HI3519_IOCH3_VIRT,
		.pfn            = __phys_to_pfn(HI3519_IOCH3_PHYS),
		.length         = HI3519_IOCH3_SIZE,
		.type           = MT_DEVICE
	},

};

static void __init hi3519_reserve(void)
{
#ifdef CONFIG_DMA_CMA
	extern int hisi_declare_heap_memory(void);
	hisi_declare_heap_memory();
#endif
}
#endif

#ifdef CONFIG_ARCH_HI3516AV200
static struct map_desc hi3516av200_io_desc[] __initdata = {
	/* hi3516av200_IOCH1 */
	{
		.virtual        = HI3516AV200_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(HI3516AV200_IOCH1_PHYS),
		.length         = HI3516AV200_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3516av200_IOCH2 */
	{
		.virtual        = HI3516AV200_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3516AV200_IOCH2_PHYS),
		.length         = HI3516AV200_IOCH2_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3516av200_IOCH2 */
	{
		.virtual        = HI3516AV200_IOCH3_VIRT,
		.pfn            = __phys_to_pfn(HI3516AV200_IOCH3_PHYS),
		.length         = HI3516AV200_IOCH3_SIZE,
		.type           = MT_DEVICE
	},

};

static void __init hi3516av200_reserve(void)
{
#ifdef CONFIG_DMA_CMA
	extern int hisi_declare_heap_memory(void);
	hisi_declare_heap_memory();
#endif
}
#endif
#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
static struct map_desc hi3559_io_desc[] __initdata = {
	/* hi3559_IOCH1 */
	{
		.virtual        = HI3559_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(HI3559_IOCH1_PHYS),
		.length         = HI3559_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3559_IOCH2 */
	{
		.virtual        = HI3559_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3559_IOCH2_PHYS),
		.length         = HI3559_IOCH2_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3559_IOCH2 */
	{
		.virtual        = HI3559_IOCH3_VIRT,
		.pfn            = __phys_to_pfn(HI3559_IOCH3_PHYS),
		.length         = HI3559_IOCH3_SIZE,
		.type           = MT_DEVICE
	},

};
#endif

#ifdef CONFIG_ARCH_HI3536C
static struct map_desc hi3536c_io_desc[] __initdata = {
	/* hi3536c_IOCH1 */
	{
		.virtual    = HI3536C_IOCH1_VIRT,
		.pfn        = __phys_to_pfn(HI3536C_IOCH1_PHYS),
		.length     = HI3536C_IOCH1_SIZE,
		.type       = MT_DEVICE
	},
	/* hi3536c_IOCH2 */
	{
		.virtual        = HI3536C_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3536C_IOCH2_PHYS),
		.length         = HI3536C_IOCH2_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3536c_IOCH3 */
	{
		.virtual        = HI3536C_IOCH3_VIRT,
		.pfn            = __phys_to_pfn(HI3536C_IOCH3_PHYS),
		.length         = HI3536C_IOCH3_SIZE,
		.type           = MT_DEVICE
	},
};

static void __init hi3536c_map_io(void)
{
	iotable_init(hi3536c_io_desc, ARRAY_SIZE(hi3536c_io_desc));
}

static void __init hi3536c_init_early(void)
{

}
#endif /* CONFIG_ARCH_HI3536C */

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
static void __init hi3519_map_io(void)
{
	/* debug_ll_io_init(); */
	iotable_init(hi3519_io_desc, ARRAY_SIZE(hi3519_io_desc));
}
#endif

#ifdef CONFIG_ARCH_HI3516AV200
static void __init hi3516av200_map_io(void)
{
	/* debug_ll_io_init(); */
	iotable_init(hi3516av200_io_desc, ARRAY_SIZE(hi3516av200_io_desc));
}
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
static void __init hi3559_map_io(void)
{
	/* debug_ll_io_init(); */
	iotable_init(hi3559_io_desc, ARRAY_SIZE(hi3559_io_desc));
}

static void __init hi3559_reserve(void)
{
#ifdef CONFIG_DMA_CMA
	extern int hisi_declare_heap_memory(void);
	hisi_declare_heap_memory();
#endif
}
#endif

static const char *hi3xxx_compat[] __initconst = {
	"hisilicon,hi3620-hi4511",
	NULL,
};

DT_MACHINE_START(HI3620, "Hisilicon Hi3620 (Flattened Device Tree)")
	.map_io		= hi3620_map_io,
	.dt_compat	= hi3xxx_compat,
MACHINE_END

static const char *hix5hd2_compat[] __initconst = {
	"hisilicon,hix5hd2",
	NULL,
};

DT_MACHINE_START(HIX5HD2_DT, "Hisilicon HIX5HD2 (Flattened Device Tree)")
	.dt_compat	= hix5hd2_compat,
MACHINE_END

static const char *hip04_compat[] __initconst = {
	"hisilicon,hip04-d01",
	NULL,
};

DT_MACHINE_START(HIP04, "Hisilicon HiP04 (Flattened Device Tree)")
	.dt_compat	= hip04_compat,
MACHINE_END

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
static const char *hi3519_compat[] __initconst = {
	"hisilicon,hi3519",
	"hisilicon,hi3519v101",
	NULL,
};
#endif

#ifdef CONFIG_ARCH_HI3516AV200
static const char *hi3516av200_compat[] __initconst = {
	"hisilicon,hi3516av200",
	NULL,
};
#endif
#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
static const char *hi3559_compat[] __initconst = {
	"hisilicon,hi3559",
	"hisilicon,hi3556",
	NULL,
};
#endif

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
DT_MACHINE_START(HI3519_DT, "Hisilicon Hi3519 (Flattened Device Tree)")
	.map_io		= hi3519_map_io,
	.dt_compat	= hi3519_compat,
	.reserve	= hi3519_reserve,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_HI3516AV200
DT_MACHINE_START(HI3516AV200_DT, "Hisilicon Hi3516av200 (Flattened Device Tree)")
	.map_io		= hi3516av200_map_io,
	.dt_compat	= hi3516av200_compat,
	.reserve	= hi3516av200_reserve,
MACHINE_END
#endif
#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
DT_MACHINE_START(HI3559_DT, "Hisilicon Hi3559/Hi3556 (Flattened Device Tree)")
	.map_io		= hi3559_map_io,
	.dt_compat	= hi3559_compat,
	.reserve	= hi3559_reserve,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_HI3516CV300
static const char *hi3516cv300_compat[] __initconst = {
	"hisilicon,hi3516cv300",
	NULL,
};

DT_MACHINE_START(HI3516CV300_DT,
		"Hisilicon Hi3516cv300 (Flattened Device Tree)")
	.dt_compat	= hi3516cv300_compat,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_HI3536C
static const char *hi3536c_compat[] __initconst = {
	"hisilicon,hi3536c",
	NULL,
};

DT_MACHINE_START(HI3536C_DT, "Hisilicon Hi3536C (Flattened Device Tree)")
	.map_io		= hi3536c_map_io,
	.dt_compat	= hi3536c_compat,
	.init_early	= hi3536c_init_early,
MACHINE_END
#endif
