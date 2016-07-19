#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/cnt32_to_63.h>
#include <linux/io.h>

#include <linux/clkdev.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/leds.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/vic.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/sched_clock.h>
#include <mach/hardware.h>
#include <mach/early-debug.h>
#include <mach/irqs.h>
#include "mach/clock.h"
#include <mach/clkdev.h>
#include <linux/bootmem.h>
#include <linux/delay.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/smp_twd.h>
#include <asm/hardware/timer-sp.h>

/*#define HW_REG(a) (*(volatile unsigned int *)(a))*/
#define HW_REG(a) readl(a)
#define A9_AXI_SCALE_REG   IO_ADDRESS(CRG_REG_BASE + 0x30)
#define REG_PERI_CRG57     IO_ADDRESS(CRG_REG_BASE + 0xe4)
#define get_bus_clk()({\
	unsigned long tmp_reg, busclk;\
	tmp_reg = HW_REG(A9_AXI_SCALE_REG);\
	if ((tmp_reg & 0x1) == 0x1) {\
		busclk = 200000000;\
	} else {\
		busclk = 250000000;\
	} \
	busclk;\
})

void __iomem *hi3535_gic_cpu_base_addr;

unsigned long long hi_sched_clock(void)
{
	return sched_clock();
}
EXPORT_SYMBOL(hi_sched_clock);


void __init hi3535_gic_init_irq(void)
{
	edb_trace();
	hi3535_gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;

#ifndef CONFIG_LOCAL_TIMERS
	gic_init(0, HISI_GIC_IRQ_START, (void __iomem *)
		 CFG_GIC_DIST_BASE, hi3535_gic_cpu_base_addr);
#else
	/* git initialed include Local timer.
	* IRQ_LOCALTIMER is settled IRQ number for local timer interrupt.
	* It is set to 29 by ARM.
	*/
	gic_init(0, IRQ_LOCALTIMER, (void __iomem *)CFG_GIC_DIST_BASE,
		 (void __iomem *)CFG_GIC_CPU_BASE);
#endif
}

static struct map_desc hi3535_io_desc[] __initdata = {
	{
		.virtual        = HI3535_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(HI3535_IOCH1_PHYS),
		.length         = HI3535_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	{
		.virtual        = HI3535_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3535_IOCH2_PHYS),
		.length         = HI3535_IOCH2_SIZE,
		.type           = MT_DEVICE
	},
	{
		.virtual        = HI3535_IOCH3_VIRT,
		.pfn            = __phys_to_pfn(HI3535_IOCH3_PHYS),
		.length         = HI3535_IOCH3_SIZE,
		.type           = MT_DEVICE
	}

};

void __init hi3535_map_io(void)
{
	int i;

	iotable_init(hi3535_io_desc, ARRAY_SIZE(hi3535_io_desc));

	for (i = 0; i < ARRAY_SIZE(hi3535_io_desc); i++) {
		edb_putstr(" V: ");	edb_puthex(hi3535_io_desc[i].virtual);
		edb_putstr(" P: ");	edb_puthex(hi3535_io_desc[i].pfn);
		edb_putstr(" S: ");	edb_puthex(hi3535_io_desc[i].length);
		edb_putstr(" T: ");	edb_putul(hi3535_io_desc[i].type);
		edb_putstr("\n");
	}

	edb_trace();
}
/*****************************************************************************/

#define HIL_AMBADEV_NAME(name) hil_ambadevice_##name

#define HIL_AMBA_DEVICE(name, busid, base, platdata)		\
static struct amba_device HIL_AMBADEV_NAME(name) =		\
{								\
	.dev		= {					\
		.coherent_dma_mask = ~0,			\
		.init_name = busid,				\
		.platform_data = platdata,			\
	},							\
	.res		= {					\
		.start	= REG_BASE_##base,			\
		.end	= REG_BASE_##base + 0x1000 - 1,		\
		.flags	= IORESOURCE_IO,			\
	},							\
	.dma_mask	= ~0,					\
	.irq		= { INTNR_##base, INTNR_##base }	\
}

HIL_AMBA_DEVICE(uart0, "uart:0",  UART0,    NULL);
HIL_AMBA_DEVICE(uart1, "uart:1",  UART1,    NULL);

static struct amba_device *amba_devs[] __initdata = {
	&HIL_AMBADEV_NAME(uart0),
	&HIL_AMBADEV_NAME(uart1),
};

/*
 * These are fixed clocks.
 */
static struct clk uart_clk = {
	.rate = 2000000,
};

static struct clk sp804_clk = {
	.rate = 10000000,
};

static struct clk_lookup lookups[] = {
	{
		/* UART0 */
		.dev_id		= "uart:0",
		.clk		= &uart_clk,
	}, { /* UART1 */
		.dev_id		= "uart:1",
		.clk		= &uart_clk,
	}, { /* SP804 timers */
		.dev_id		= "sp804",
		.clk		= &sp804_clk,
	},
};

static void __init hi3535_reserve(void)
{
}
/*****************************************************************************/


void __init hi3535_init(void)
{
	unsigned long i;

	edb_trace();
	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		edb_trace();
		amba_device_register(amba_devs[i], &iomem_resource);
	}

}
/*****************************************************************************/

#ifdef CONFIG_CACHE_L2X0
static int __init l2_cache_init(void)
{
	u32 val;
	void __iomem *l2x0_base = (void __iomem *)IO_ADDRESS(REG_BASE_L2CACHE);

	/*
	 * Bits  Value Description
	 * [31]    0 : SBZ
	 * [30]    1 : Double linefill enable (L3)
	 * [29]    1 : Instruction prefetching enable
	 * [28]    1 : Data prefetching enabled
	 * [27]    0 : Double linefill on WRAP read enabled (L3)
	 * [26:25] 0 : SBZ
	 * [24]    1 : Prefetch drop enable (L3)
	 * [23]    0 : Incr double Linefill enable (L3)
	 * [22]    0 : SBZ
	 * [21]    0 : Not same ID on exclusive sequence enable (L3)
	 * [20:5]  0 : SBZ
	 * [4:0]   0 : use the Prefetch offset values 0.
	 */
	writel_relaxed(0x71000000, l2x0_base + L2X0_PREFETCH_CTRL);

	val = __raw_readl(l2x0_base + L2X0_AUX_CTRL);
	val |= (1 << 30); /* Early BRESP enabled */
	val |= (1 << 0);  /* Full Line of Zero Enable */
	writel_relaxed(val, l2x0_base + L2X0_AUX_CTRL);


	l2x0_init(l2x0_base, 0x00430000, 0xFFB0FFFF);

	/*
	 * 2. enable L2 prefetch hint                  [1]a
	 * 3. enable write full line of zeros mode.    [3]a
	 *   a: This feature must be enabled only when the slaves
	 *      connected on the Cortex-A9 AXI master port support it.
	 */
	asm volatile (
	"	mrc	p15, 0, r0, c1, c0, 1\n"
	"	orr	r0, r0, #0x0A\n"
	"	mcr	p15, 0, r0, c1, c0, 1\n"
	  :
	  :
	  : "r0", "cc");

	return 0;
}

early_initcall(l2_cache_init);
#endif

/*****************************************************************************/

static void __init hi3535_init_early(void)
{
	uart_clk.rate = get_bus_clk()/4;
	sp804_clk.rate = get_bus_clk()/4;

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	/*
	 * 1. enable L1 prefetch                       [2]
	 * 4. enable allocation in one cache way only. [8]
	 */
	asm volatile (
	"       mrc     p15, 0, r0, c1, c0, 1\n"
	"       orr     r0, r0, #0x104\n"
	"       mcr     p15, 0, r0, c1, c0, 1\n"
		:
		:
		: "r0", "cc");

	edb_trace();
}
/*****************************************************************************/

void hi3535_restart(char mode, const char *cmd)
{
	__raw_writel(~0, IO_ADDRESS(SYS_CTRL_BASE) + REG_SC_SYSRES);
}
/*****************************************************************************/

extern struct sys_timer hi3535_sys_timer;

MACHINE_START(HI3535, "hi3535")
	.atag_offset	= 0x100,
	.map_io		= hi3535_map_io,
	.init_early	= hi3535_init_early,
	.init_irq	= hi3535_gic_init_irq,
	.handle_irq	= gic_handle_irq,
	.timer		= &hi3535_sys_timer,
	.init_machine	= hi3535_init,
	.reserve	= hi3535_reserve,
	.restart	= hi3535_restart,
MACHINE_END
