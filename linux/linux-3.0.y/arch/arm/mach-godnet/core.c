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
#include <asm/sched_clock.h>
#include <asm/smp_twd.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <linux/leds.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/vic.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>

#include <mach/time.h>
#include <mach/hardware.h>
#include <mach/early-debug.h>
#include <mach/irqs.h>
#include "clock.h"
#include <linux/bootmem.h>

static struct map_desc godnet_io_desc[] __initdata = {
	{
		.virtual        = GODNET_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(GODNET_IOCH1_PHYS),
		.length         = GODNET_IOCH1_SIZE,
		.type           = MT_DEVICE_STRONGLY_ORDERED
	},
	{
		.virtual        = GODNET_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(GODNET_IOCH2_PHYS),
		.length         = GODNET_IOCH2_SIZE,
		.type           = MT_DEVICE
	}
};

static unsigned long godnet_timer_reload, timer0_clk_hz, timer0_clk_khz;
static unsigned long timer1_clk_hz, timer1_clk_khz;

#define HW_REG(a) (*(unsigned long *)(a))
#define A9_AXI_SCALE_REG   IO_ADDRESS(CRG_BASE+0x28)
#define TXIN_OSC_FRE       24000000
#define REG_CRG0_OFFSET    IO_ADDRESS(CRG_BASE + 0x0)
#define REG_CRG1_OFFSET    IO_ADDRESS(CRG_BASE + 0x4)
#define get_bus_clk()({\
	unsigned long fbdiv, pstdiv1, pstdiv2, refdiv;\
	unsigned long tmp_reg, foutvco, busclk;\
	tmp_reg = HW_REG(REG_CRG0_OFFSET);\
	pstdiv1 = (tmp_reg >> 24) & 0x7;\
	pstdiv2 = (tmp_reg >> 27) & 0x7;\
	tmp_reg = HW_REG(REG_CRG1_OFFSET);\
	refdiv = (tmp_reg >> 12) & 0x3f;\
	fbdiv = tmp_reg & 0xfff;\
	foutvco = TXIN_OSC_FRE/refdiv;\
	foutvco *= fbdiv;\
	tmp_reg = HW_REG(A9_AXI_SCALE_REG);\
	if ((tmp_reg & 0x3) == 0) {\
		busclk = foutvco/4;\
	} else if ((tmp_reg & 0x3) == 3) {\
		busclk = foutvco/6;\
	} else {\
		busclk = 0;\
		printk(KERN_ERR "system bus clk is0!\n");\
	} \
	busclk;\
})

static void early_init(void)
{
	unsigned long busclk;

	edb_trace();
	busclk = get_bus_clk();

	godnet_timer_reload = BUSCLK_TO_TIMER_RELOAD(busclk);
	timer0_clk_hz = BUSCLK_TO_TIMER0_CLK_HZ(busclk);
	timer0_clk_khz = BUSCLK_TO_TIMER0_CLK_KHZ(busclk);
	timer1_clk_hz = BUSCLK_TO_TIMER1_CLK_HZ(busclk);
	timer1_clk_khz = BUSCLK_TO_TIMER1_CLK_KHZ(busclk);
}

void __init godnet_map_io(void)
{
	int i;

	iotable_init(godnet_io_desc, ARRAY_SIZE(godnet_io_desc));

	for (i = 0; i < ARRAY_SIZE(godnet_io_desc); i++) {
		edb_putstr(" V: ");     edb_puthex(godnet_io_desc[i].virtual);
		edb_putstr(" P: ");     edb_puthex(godnet_io_desc[i].pfn);
		edb_putstr(" S: ");     edb_puthex(godnet_io_desc[i].length);
		edb_putstr(" T: ");     edb_putul(godnet_io_desc[i].type);
		edb_putstr("\n");
	}

	early_init();

	edb_trace();
}
void __iomem *godnet_gic_cpu_base_addr;

void __init godnet_gic_init_irq(void)
{
	edb_trace();

	godnet_gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;
#ifndef CONFIG_LOCAL_TIMERS
	gic_init(0, GODNET_IRQ_START, (void __iomem *)CFG_GIC_DIST_BASE,
		(void __iomem *)CFG_GIC_CPU_BASE);
#else
	/* git initialed include Local timer.
	 * IRQ_LOCALTIMER is settled IRQ number for local timer interrupt.
	 * It is set to 29 by ARM.
	 */
	gic_init(0, IRQ_LOCALTIMER, (void __iomem *)CFG_GIC_DIST_BASE,
		(void __iomem *)CFG_GIC_CPU_BASE);
#endif
}

static unsigned long free_timer_overflows;


static DEFINE_CLOCK_DATA(cd);
static DEFINE_SPINLOCK(sched_clock_lock);

static void notrace godnet_update_sched_clock(void)
{
       	u32 cyc;
	unsigned long flags;
	spin_lock_irqsave(&sched_clock_lock, flags);
	cyc = ~readl(CFG_TIMER_VABASE + REG_TIMER1_VALUE);

	update_sched_clock(&cd, cyc, (u32)~0);
	spin_unlock_irqrestore(&sched_clock_lock, flags);
}

unsigned long long notrace sched_clock(void)
{
        u32 cyc;
	unsigned long long t_clk;
	unsigned long flags;
	spin_lock_irqsave(&sched_clock_lock, flags);
        cyc = ~readl(CFG_TIMER_VABASE + REG_TIMER1_VALUE);
	t_clk = cyc_to_sched_clock(&cd, cyc, (u32)~0);
	spin_unlock_irqrestore(&sched_clock_lock, flags);

	return t_clk;

}


static struct clocksource godnet_clocksource;


unsigned long long hi_sched_clock(void)
{
	return sched_clock();
}
EXPORT_SYMBOL(hi_sched_clock);

static void timer_set_mode(enum clock_event_mode mode,
		struct clock_event_device *clk)
{
	unsigned long ctrl;
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		writel(0, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
		writel(godnet_timer_reload,
			CFG_TIMER_VABASE + REG_TIMER_RELOAD);
		writel(CFG_TIMER_CONTROL,
			CFG_TIMER_VABASE + REG_TIMER_CONTROL);
		edb_trace();
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

static struct clock_event_device timer0_clockevent = {
	.name           = "timer0",
	.shift          = 32,
	.features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.set_mode       = timer_set_mode,
	.set_next_event = timer_set_next_event,
};

/*
 * IRQ handler for the timer
 */
static irqreturn_t godnet_timer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER_VABASE+REG_TIMER_RIS)) & 0x1) {
		writel(~0, CFG_TIMER_VABASE + REG_TIMER_INTCLR);
		timer0_clockevent.event_handler(&timer0_clockevent);
	}

	return IRQ_HANDLED;
}

static irqreturn_t godnet_freetimer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER_VABASE+REG_TIMER1_RIS)) & 0x1) {
		free_timer_overflows++;
		writel(~0, CFG_TIMER_VABASE + REG_TIMER1_INTCLR);
	}
	return IRQ_HANDLED;
}

static struct irqaction godnet_timer_irq = {
	.name           = "System Timer Tick",
	.flags          = IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER,
	.handler        = godnet_timer_interrupt,
};

static struct irqaction godnet_freetimer_irq = {
	.name           = "Free Timer",
	.flags          = IRQF_SHARED | IRQF_TIMER,
	.handler        = godnet_freetimer_interrupt,
};
static cycle_t godnet_get_cycles(struct clocksource *cs)
{
	return ~readl(CFG_TIMER_VABASE + REG_TIMER1_VALUE);
}

static struct clocksource godnet_clocksource = {
	.name           = "timer1",
	.rating         = 200,
	.read           = godnet_get_cycles,
	.mask           = CLOCKSOURCE_MASK(32),
	.flags          = CLOCK_SOURCE_IS_CONTINUOUS,
};

static int __init godnet_clocksource_init(void)
{
	unsigned long rate = timer1_clk_khz * 1000;

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
	writel(CFG_TIMER_CONTROL, CFG_TIMER_VABASE + REG_TIMER1_CONTROL);

	/* caculate the mult/shift by clock rate to gain more accratury */
	if (clocksource_register_hz(&godnet_clocksource, rate))
		panic("register clocksouce :%s error\n",
				godnet_clocksource.name);

	/* force check the mult/shift of clocksource */
	init_fixed_sched_clock(&cd, godnet_update_sched_clock, 32, rate,
			godnet_clocksource.mult, godnet_clocksource.shift);

	return 0;
}

static void __init godnet_timer_init(void){

	edb_trace();

	setup_irq(TIMER01_IRQ, &godnet_timer_irq);
	setup_irq(TIMER01_IRQ, &godnet_freetimer_irq);

	godnet_clocksource_init();
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

struct sys_timer godnet_timer = {
	.init           = godnet_timer_init,
};

#define HIL_AMBADEV_NAME(name) hil_ambadevice_##name

#define HIL_AMBA_DEVICE(name, busid, base, platdata)               \
	static struct amba_device HIL_AMBADEV_NAME(name) =              \
	{	.dev            = {                                     \
			.coherent_dma_mask = ~0,                        \
			.init_name = busid,                              \
			.platform_data = platdata,                      \
		},                                                      \
		.res            = {                                     \
			.start  = base##_BASE,                      \
			.end    = base##_BASE + 0x10000 - 1,  \
			.flags  = IORESOURCE_IO,                        \
		},                                                      \
		.dma_mask       = ~0,                                   \
		.irq            = { base##_IRQ, NO_IRQ }              \
	}


HIL_AMBA_DEVICE(uart0, "uart:0",  UART0,    NULL);
HIL_AMBA_DEVICE(uart1, "uart:1",  UART1,    NULL);

static struct amba_device *amba_devs[] __initdata = {
	&HIL_AMBADEV_NAME(uart0),
	&HIL_AMBADEV_NAME(uart1),
};


static struct clk uart_clk;
static struct clk_lookup lookups[2];

static void  uart_clk_init(unsigned long busclk)
{
	uart_clk.rate = busclk/2;

	lookups[0].dev_id = "uart:0";
	lookups[0].clk = &uart_clk;

	lookups[1].dev_id = "uart:1";
	lookups[1].clk = &uart_clk;
}

void __init godnet_init(void)
{
	unsigned long i;
	unsigned long reg;
	unsigned long busclk;
	edb_trace();

	/* godnet uart use apb bus clk */
	reg = readl(IO_ADDRESS(CRG_BASE + 0xe4));
	reg &= ~UART_CKSEL_APB;
	writel(reg, IO_ADDRESS(CRG_BASE + 0xe4));

	busclk = get_bus_clk();
	uart_clk_init(busclk);


	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		edb_trace();
		amba_device_register(amba_devs[i], &iomem_resource);
	}
}

MACHINE_START(GODNET, "godnet")
	.boot_params    = PLAT_PHYS_OFFSET + 0x100,
	.map_io         = godnet_map_io,
	.init_irq       = godnet_gic_init_irq,
	.timer          = &godnet_timer,
	.init_machine   = godnet_init,
MACHINE_END

