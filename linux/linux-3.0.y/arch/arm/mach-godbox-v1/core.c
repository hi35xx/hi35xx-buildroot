#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/sysdev.h>
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
#include <asm/localtimer.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/vic.h>
#include <asm/mach-types.h>
#include <linux/leds.h>
#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>
#include <asm/sched_clock.h>
#include <mach/time.h>
#include <mach/hardware.h>
#include <mach/early-debug.h>
#include <mach/irqs.h>
#include "clock.h"

#include <linux/bootmem.h>

void __iomem *godbox_v1_gic_cpu_base_addr;
static unsigned long godbox_timer_reload;
static unsigned long timer0_clk_hz;
static unsigned long timer1_clk_hz;
static unsigned long timer0_clk_khz;
static unsigned long timer1_clk_khz;

static void early_init(void)
{
	unsigned long busclk;

	edb_trace();
	busclk = get_ahbclk_hw();
	printk(KERN_INFO "godbox bus clk is %lu\n", busclk);

	godbox_timer_reload = BUSCLK_TO_TIMER_RELOAD(busclk);
	timer0_clk_hz = BUSCLK_TO_TIMER0_CLK_HZ(busclk);
	timer0_clk_khz = BUSCLK_TO_TIMER0_CLK_KHZ(busclk);
	timer1_clk_hz = BUSCLK_TO_TIMER1_CLK_HZ(busclk);
	timer1_clk_khz = BUSCLK_TO_TIMER1_CLK_KHZ(busclk);
}

void __init godbox_gic_init_irq(void)
{
	edb_trace();
	godbox_v1_gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;
	/* Local timer */
	gic_init(0, 29, (void __iomem *)
			CFG_GIC_DIST_BASE, godbox_v1_gic_cpu_base_addr);
}

static struct map_desc godbox_io_desc[] __initdata = {
	/* GODBOX_IOCH1 */
	{
		.virtual	= GODBOX_IOCH1_VIRT,
		.pfn		= __phys_to_pfn(GODBOX_IOCH1_PHYS),
		.length		= GODBOX_IOCH1_SIZE,
		.type		= MT_DEVICE
	},
	/* GODBOX_IOCH2 */
	{
		.virtual	= GODBOX_IOCH2_VIRT,
		.pfn		= __phys_to_pfn(GODBOX_IOCH2_PHYS),
		.length		= GODBOX_IOCH2_SIZE,
		.type		= MT_DEVICE
	},
	/* GODBOX_IOCH3 */
	{
		.virtual	= GODBOX_IOCH3_VIRT,
		.pfn		= __phys_to_pfn(GODBOX_IOCH3_PHYS),
		.length		= GODBOX_IOCH3_SIZE,
		.type		= MT_DEVICE
	},
	/* GODBOX_IOCH4 */
	{
		.virtual	= GODBOX_IOCH4_VIRT,
		.pfn		= __phys_to_pfn(GODBOX_IOCH4_PHYS),
		.length		= GODBOX_IOCH4_SIZE,
		.type		= MT_DEVICE
	},
	/* GODBOX_IOCH5 */
	{
		.virtual	= GODBOX_IOCH5_VIRT,
		.pfn		= __phys_to_pfn(GODBOX_IOCH5_PHYS),
		.length		= GODBOX_IOCH5_SIZE,
		.type		= MT_DEVICE_STRONGLY_ORDERED
	},
	/* GODBOX_IOCH6 */
	{
		.virtual	= GODBOX_IOCH6_VIRT,
		.pfn		= __phys_to_pfn(GODBOX_IOCH6_PHYS),
		.length		= GODBOX_IOCH6_SIZE,
		.type		= MT_DEVICE
	},
	/* GODBOX_IOCH7 */
	{
		.virtual	= GODBOX_IOCH7_VIRT,
		.pfn		= __phys_to_pfn(GODBOX_IOCH7_PHYS),
		.length		= GODBOX_IOCH7_SIZE,
		.type		= MT_DEVICE
	}
};

void __init godbox_map_io(void)
{
	int i;

	iotable_init(godbox_io_desc, ARRAY_SIZE(godbox_io_desc));

	for (i = 0; i < ARRAY_SIZE(godbox_io_desc); i++) {
		edb_putstr(" V: ");	edb_puthex(godbox_io_desc[i].virtual);
		edb_putstr(" P: ");	edb_puthex(godbox_io_desc[i].pfn);
		edb_putstr(" S: ");	edb_puthex(godbox_io_desc[i].length);
		edb_putstr(" T: ");	edb_putul(godbox_io_desc[i].type);
		edb_putstr("\n");
	}

	early_init();

	edb_trace();
}

static unsigned long long gobbox_cycles_2_ns(unsigned long long cyc)
{
	unsigned long cyc2ns_scale = (1000000 << 10)/timer1_clk_khz;
	return (cyc * cyc2ns_scale) >> 10;
}

static unsigned long free_timer_overflows;

unsigned long long sched_clock(void)
{
	unsigned long long ticks64;
	unsigned long ticks2, ticks1;
	ticks2 = 0UL - (unsigned long)
		readl(CFG_TIMER_VABASE + REG_TIMER1_VALUE);
	do {
		ticks1 = ticks2;
		ticks64 = free_timer_overflows;
		ticks2 = 0UL - (unsigned long)
			readl(CFG_TIMER_VABASE + REG_TIMER1_VALUE);
	} while (ticks1 > ticks2);

	/*
	 ** If INT is not cleaned, means the function is called with irq_save.
	 ** The ticks has overflow but 'free_timer_overflows' is not be update.
	 */
	if (readl(CFG_TIMER_VABASE + REG_TIMER1_MIS)) {
		ticks64 += 1;
		ticks2 = 0UL - (unsigned long)
			readl(CFG_TIMER_VABASE + REG_TIMER1_VALUE);
	}

	return gobbox_cycles_2_ns((ticks64 << 32) | ticks2);
}

static DEFINE_CLOCK_DATA(cd);
static void *ctr;
static void godbox_update_sched_clock(void){
	u32 cyc = readl(ctr);
	update_sched_clock(&cd, cyc, (u32)~0);
}
static struct clocksource godbox_clocksource;

static void __init godbox_sched_clock_init(void* reg, unsigned long rate)
{
	ctr = reg;
	init_fixed_sched_clock(&cd, godbox_update_sched_clock,
		32, rate, godbox_clocksource.mult, godbox_clocksource.shift);
}

#define HIL_AMBADEV_NAME(name) hil_ambadevice_##name

#define HIL_AMBA_DEVICE(name, busid, base, platdata)		\
	static struct amba_device HIL_AMBADEV_NAME(name) =  \
{	\
	.dev		= {					\
		.coherent_dma_mask = ~0,			\
		.init_name = busid,				\
		.platform_data = platdata,			\
	},							\
	.res		= {					\
		.start	= REG_BASE_##base,			\
		.end	= REG_BASE_##base + 0x1000 - 1,	\
		.flags	= IORESOURCE_IO,			\
	},							\
	.dma_mask	= ~0,					\
	.irq		= { INTNR_##base, NO_IRQ }		\
}

#define UART0_IRQ	{ INTNR_UART0, NO_IRQ }
#define UART1_IRQ	{ INTNR_UART1, NO_IRQ }

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
	.rate	= 54000000,
};


static struct clk_lookup lookups[] = {
	{	/* UART0 */
		.dev_id		= "uart:0",
		.clk		= &uart_clk,
	}, {	/* UART1 */
		.dev_id		= "uart:1",
		.clk		= &uart_clk,
	}
};

void __init godbox_init(void)
{
	unsigned long i;

	edb_trace();

	for (i = 0; i < ARRAY_SIZE(lookups); i++)
		clkdev_add(&lookups[i]);

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		edb_trace();
		amba_device_register(amba_devs[i], &iomem_resource);
	}

}

static void timer_set_mode(enum clock_event_mode mode,
		struct clock_event_device *clk)
{
	unsigned long ctrl;
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
			writel(0, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
			writel(godbox_timer_reload,
					CFG_TIMER_VABASE + REG_TIMER_RELOAD);
			writel(CFG_TIMER_CONTROL,
					CFG_TIMER_VABASE + REG_TIMER_CONTROL);
			break;
	case CLOCK_EVT_MODE_ONESHOT:
			writel((CFG_TIMER_32BIT | CFG_TIMER_ONESHOT),
					CFG_TIMER_VABASE + REG_TIMER_CONTROL);
			break;
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	default:
			ctrl = readl(CFG_TIMER_VABASE + REG_TIMER_CONTROL);
			ctrl &= ~CFG_TIMER_ENABLE;
			writel(ctrl, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
	}
}

static int timer_set_next_event(unsigned long evt,
		struct clock_event_device *unused)
{
	unsigned long ctrl;
	ctrl = readl(CFG_TIMER_VABASE + REG_TIMER_CONTROL);
	ctrl &= ~(CFG_TIMER_ENABLE | CFG_TIMER_INTMASK);
	writel(ctrl, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
	writel(evt, CFG_TIMER_VABASE + REG_TIMER_RELOAD);
	writel(CFG_TIMER_ONE_CONTROL, CFG_TIMER_VABASE + REG_TIMER_CONTROL);

	return 0;
}

static struct clock_event_device timer0_clockevent =	 {
	.name		= "timer0",
	.shift		= 32,
	.features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.set_mode	= timer_set_mode,
	.set_next_event	= timer_set_next_event,
};

/*
 * IRQ handler for the timer
 */
static irqreturn_t godbox_timer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER_VABASE+REG_TIMER_RIS))&0x1) {
		writel(~0, CFG_TIMER_VABASE + REG_TIMER_INTCLR);
		timer0_clockevent.event_handler(&timer0_clockevent);
	}

	return IRQ_HANDLED;
}

static irqreturn_t godbox_freetimer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER_VABASE+REG_TIMER1_RIS))&0x1) {
		free_timer_overflows++;
		writel(~0, CFG_TIMER_VABASE + REG_TIMER1_INTCLR);
	}
	return IRQ_HANDLED;
}

static struct irqaction godbox_timer_irq = {
	.name		= "Godbox System Timer Tick",
	.flags		= IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER,
	.handler	= godbox_timer_interrupt,
};

static struct irqaction godbox_freetimer_irq = {
	.name		= "Free Timer",
	.flags		= IRQF_SHARED | IRQF_TIMER,
	.handler	= godbox_freetimer_interrupt,
};

static cycle_t godbox_get_cycles(struct clocksource *cs)
{
	return ~readl(CFG_TIMER_VABASE + REG_TIMER1_VALUE);
}

static struct clocksource godbox_clocksource = {
	.name		= "timer1",
	.rating		= 200,
	.read		= godbox_get_cycles,
	.mask		= CLOCKSOURCE_MASK(32),
	.shift		= 20,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

static int __init godbox_clocksource_init(void)
{
#ifdef CONFIG_LOCAL_TIMERS
	twd_base = (void *)IO_ADDRESS(REG_BASE_A9_PERI) + REG_SC_LOCAL_TIMER;
	edb_trace();
	edb_putstr("twd_base :\n");
	edb_puthex((int)twd_base);
	edb_putstr("\n");
#endif

	writel(0, CFG_TIMER_VABASE + REG_TIMER1_CONTROL);
	writel(0xffffffff, CFG_TIMER_VABASE + REG_TIMER1_RELOAD);
	writel(0xffffffff, CFG_TIMER_VABASE + REG_TIMER1_VALUE);
	writel(CFG_TIMER_CONTROL,
			CFG_TIMER_VABASE + REG_TIMER1_CONTROL);

	godbox_clocksource.mult =
		clocksource_khz2mult(timer1_clk_khz, godbox_clocksource.shift);
	godbox_sched_clock_init((void *)CFG_TIMER_VABASE + REG_TIMER1_VALUE,
						 timer1_clk_khz * 1000);
	clocksource_register(&godbox_clocksource);

	return 0;
}

static void __init godbox_timer_init(void)
{
	edb_trace();
	setup_irq(INTNR_TIMER_0_1, &godbox_timer_irq);
	setup_irq(INTNR_TIMER_0_1, &godbox_freetimer_irq);
	godbox_clocksource_init();
	timer0_clockevent.mult =
		div_sc(timer0_clk_hz, NSEC_PER_SEC, timer0_clockevent.shift);
	timer0_clockevent.max_delta_ns =
		clockevent_delta2ns(0xffffffff, &timer0_clockevent);
	timer0_clockevent.min_delta_ns =
		clockevent_delta2ns(0xf, &timer0_clockevent);
	timer0_clockevent.cpumask = cpumask_of(0);
	clockevents_register_device(&timer0_clockevent);
	edb_trace();
}

struct sys_timer godbox_timer = {
	.init		= godbox_timer_init,
};

MACHINE_START(GODBOX_V1, "godbox-v1")
	.boot_params	= PLAT_PHYS_OFFSET + 0x100,
	.map_io		= godbox_map_io,
	.init_irq	= godbox_gic_init_irq,
	.timer		= &godbox_timer,
	.init_machine	= godbox_init,
	MACHINE_END
