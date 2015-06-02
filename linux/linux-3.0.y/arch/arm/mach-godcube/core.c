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

#define ioaddr_intc(x) IO_ADDRESS(REG_BASE_INTC + (x))

#define do_level_IRQ    handle_level_irq
static void hil_ack_irq(struct irq_data *data)
{
	unsigned long irq = data->irq;
	irq -= INTNR_IRQ_START;
	writel(1<<irq, ioaddr_intc(REG_INTC_INTENCLEAR));
}

static void hil_mask_irq(struct irq_data *data)
{
	unsigned long irq = data->irq;
	irq -= INTNR_IRQ_START;
	writel(1<<irq, ioaddr_intc(REG_INTC_INTENCLEAR));
}

static void hil_unmask_irq(struct irq_data *data)
{
	unsigned long irq = data->irq;
	irq -= INTNR_IRQ_START;
	writel(1<<irq, ioaddr_intc(REG_INTC_INTENABLE));
}

static struct irq_chip hic_chip = {
	.irq_ack	= hil_ack_irq,
	.irq_mask	= hil_mask_irq,
	.irq_unmask	= hil_unmask_irq,
	.irq_disable	= hil_mask_irq,
};


static struct map_desc godcube_io_desc[] __initdata = {
	{
		.virtual        = GODCUBE_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(GODCUBE_IOCH1_PHYS),
		.length         = GODCUBE_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	{
		.virtual        = GODCUBE_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(GODCUBE_IOCH2_PHYS),
		.length         = GODCUBE_IOCH2_SIZE,
		.type           = MT_DEVICE
	}
};

static unsigned long godcube_timer_reload, timer0_clk_hz, timer1_clk_hz,
		     timer0_clk_khz, timer1_clk_khz;

#define REG_CRG1_OFFSET		IO_ADDRESS(CRG_REG_BASE + 0x4)
/*
 * bus clk configuration
 *
 * APLL : ARM9 : DDR : AXI(AHB) : APB
 *  8   :  4   :  4  :  2       : 1
 *
 * default APPL = 24M * 0x9B / 0x3 = 1240M
 *         AXI busclk = APPL / 4 = 310M
 */
#define HW_REG(a) (*(unsigned long *)(a))
#define get_bus_clk() ({\
		unsigned long fbdiv, refdiv;\
		unsigned long tmp_reg, foutvco, busclk;\
		tmp_reg = HW_REG(REG_CRG1_OFFSET);\
		refdiv = (tmp_reg >> 12) & 0x3f;\
		fbdiv = tmp_reg & 0xfff;\
		foutvco = 24000000/refdiv;\
		foutvco *= fbdiv;\
		busclk = foutvco/4;\
		busclk;\
		})

static void early_init(void)
{
	unsigned long busclk;

	edb_trace();
	busclk = get_bus_clk();

	godcube_timer_reload = BUSCLK_TO_TIMER_RELOAD(busclk);
	timer0_clk_hz = BUSCLK_TO_TIMER0_CLK_HZ(busclk);
	timer0_clk_khz = BUSCLK_TO_TIMER0_CLK_KHZ(busclk);
	timer1_clk_hz = BUSCLK_TO_TIMER1_CLK_HZ(busclk);
	timer1_clk_khz = BUSCLK_TO_TIMER1_CLK_KHZ(busclk);
}

void __init godcube_map_io(void)
{
	int i;

	iotable_init(godcube_io_desc, ARRAY_SIZE(godcube_io_desc));

	for (i = 0; i < ARRAY_SIZE(godcube_io_desc); i++) {
		edb_putstr(" V: ");     edb_puthex(godcube_io_desc[i].virtual);
		edb_putstr(" P: ");     edb_puthex(godcube_io_desc[i].pfn);
		edb_putstr(" S: ");     edb_puthex(godcube_io_desc[i].length);
		edb_putstr(" T: ");     edb_putul(godcube_io_desc[i].type);
		edb_putstr("\n");
	}

	early_init();

	edb_trace();
}

void __init hisilicon_init_irq(void)
{
	unsigned int i;

	edb_trace();

	writel(~0, ioaddr_intc(REG_INTC_INTENCLEAR));
	writel(0, ioaddr_intc(REG_INTC_INTSELECT));
	writel(~0, ioaddr_intc(REG_INTC_SOFTINTCLEAR));
	writel(1, ioaddr_intc(REG_INTC_PROTECTION));

	for (i = INTNR_IRQ_START; i <= INTNR_IRQ_END; i++) {
		irq_set_chip(i, &hic_chip);
		irq_set_handler(i, do_level_IRQ);
		set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
	}
}

static unsigned long free_timer_overflows;

static DEFINE_CLOCK_DATA(cd);

static void notrace godcube_update_sched_clock(void)
{
	u32 cyc = ~readl(CFG_TIMER01_VABASE + REG_TIMER1_VALUE);

	update_sched_clock(&cd, cyc, (u32)~0);
}

unsigned long long notrace sched_clock(void)
{
	u32 cyc = ~readl(CFG_TIMER01_VABASE + REG_TIMER1_VALUE);

	return cyc_to_sched_clock(&cd, cyc, (u32)~0);
}

static struct clocksource godcube_clocksource;


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
		writel(0, CFG_TIMER01_VABASE + REG_TIMER_CONTROL);
		writel(godcube_timer_reload, CFG_TIMER01_VABASE
				+ REG_TIMER_RELOAD);
		writel(CFG_TIMER_CONTROL, CFG_TIMER01_VABASE
				+ REG_TIMER_CONTROL);
		edb_trace();
		break;
	case CLOCK_EVT_MODE_ONESHOT:
		writel((CFG_TIMER_32BIT | CFG_TIMER_ONESHOT),
				CFG_TIMER01_VABASE + REG_TIMER_CONTROL);
		break;
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	default:
	ctrl = readl(CFG_TIMER01_VABASE + REG_TIMER_CONTROL);
	ctrl &= ~CFG_TIMER_ENABLE;
		writel(ctrl, CFG_TIMER01_VABASE + REG_TIMER_CONTROL);
	}
}

static int timer_set_next_event(unsigned long evt,
		struct clock_event_device *unused)
{
	unsigned long ctrl;
	ctrl = readl(CFG_TIMER01_VABASE + REG_TIMER_CONTROL);
	ctrl &= ~(CFG_TIMER_ENABLE | CFG_TIMER_INTMASK);
	writel(ctrl, CFG_TIMER01_VABASE + REG_TIMER_CONTROL);
	writel(evt, CFG_TIMER01_VABASE + REG_TIMER_RELOAD);
	writel(CFG_TIMER_ONE_CONTROL, CFG_TIMER01_VABASE + REG_TIMER_CONTROL);

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
static irqreturn_t godcube_timer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER01_VABASE+REG_TIMER_RIS)) & 0x1) {
		writel(~0, CFG_TIMER01_VABASE + REG_TIMER_INTCLR);
		timer0_clockevent.event_handler(&timer0_clockevent);
	}

	return IRQ_HANDLED;
}

static irqreturn_t godcube_freetimer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER01_VABASE+REG_TIMER1_RIS)) & 0x1) {
		free_timer_overflows++;
		writel(~0, CFG_TIMER01_VABASE + REG_TIMER1_INTCLR);
	}
	return IRQ_HANDLED;
}

static struct irqaction godcube_timer_irq = {
	.name           = "System Timer Tick",
	.flags          = IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER,
	.handler        = godcube_timer_interrupt,
};

static struct irqaction godcube_freetimer_irq = {
	.name           = "Free Timer",
	.flags          = IRQF_SHARED | IRQF_TIMER,
	.handler        = godcube_freetimer_interrupt,
};
static cycle_t godcube_get_cycles(struct clocksource *cs)
{
	return ~readl(CFG_TIMER01_VABASE + REG_TIMER1_VALUE);
}

static struct clocksource godcube_clocksource = {
	.name           = "timer1",
	.rating         = 200,
	.read           = godcube_get_cycles,
	.mask           = CLOCKSOURCE_MASK(32),
	.flags          = CLOCK_SOURCE_IS_CONTINUOUS,
};

static int __init godcube_clocksource_init(void)
{
	unsigned long rate = timer1_clk_khz * 1000;
	writel(0, CFG_TIMER01_VABASE + REG_TIMER1_CONTROL);
	writel(0xffffffff, CFG_TIMER01_VABASE + REG_TIMER1_RELOAD);
	writel(0xffffffff, CFG_TIMER01_VABASE + REG_TIMER1_VALUE);
	writel(CFG_TIMER_CONTROL, CFG_TIMER01_VABASE + REG_TIMER1_CONTROL);

	/* caculate the mult/shift by clock rate to gain more accratury */
	if (clocksource_register_hz(&godcube_clocksource, rate))
		panic("register clocksouce :%s error\n",
				godcube_clocksource.name);

	/* force check the mult/shift of clocksource */
	init_fixed_sched_clock(&cd, godcube_update_sched_clock, 32, rate,
			godcube_clocksource.mult, godcube_clocksource.shift);

	return 0;
}

static void __init godcube_timer_init(void)
{
	edb_trace();

	setup_irq(TIMER01_IRQ, &godcube_timer_irq);
	setup_irq(TIMER01_IRQ, &godcube_freetimer_irq);

	godcube_clocksource_init();
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

struct sys_timer godcube_timer = {
	.init           = godcube_timer_init,
};

#define HIL_AMBADEV_NAME(name) hil_ambadevice_##name

#define HIL_AMBA_DEVICE(name, busid, base, platdata)			\
	static struct amba_device HIL_AMBADEV_NAME(name) =		\
	{\
		.dev            = {                                     \
			.coherent_dma_mask = ~0,                        \
			.init_name = busid,                             \
			.platform_data = platdata,                      \
		},                                                      \
		.res            = {                                     \
			.start  = base##_BASE,				\
			.end    = base##_BASE + 0x10000 - 1,		\
			.flags  = IORESOURCE_IO,                        \
		},                                                      \
		.dma_mask       = ~0,                                   \
		.irq            = { base##_IRQ, NO_IRQ }		\
	}

HIL_AMBA_DEVICE(uart0, "uart:0",  UART0,    NULL);

static struct amba_device *amba_devs[] __initdata = {
	&HIL_AMBADEV_NAME(uart0),
};

/*
 * These are fixed clocks.
 */
static struct clk uart_clk;
static struct clk_lookup lookups[2];

static void  uart_clk_init(unsigned long busclk)
{
	uart_clk.rate = busclk/2;

	lookups[0].dev_id = "uart:0";
	lookups[0].clk = &uart_clk;

}

void __init godcube_init(void)
{
	unsigned long i;
	unsigned long reg;
	unsigned long busclk;
	edb_trace();

	/* godcube uart use apb bus clk */
	reg = readl(IO_ADDRESS(CRG_REG_BASE + 0xe4));
	reg &= ~UART_CKSEL_APB;
	writel(reg, IO_ADDRESS(CRG_REG_BASE + 0xe4));

	busclk = get_bus_clk();
	uart_clk_init(busclk);
	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		edb_trace();
		amba_device_register(amba_devs[i], &iomem_resource);
	}
}

MACHINE_START(GODCUBE, "godcube")
	.boot_params    = PLAT_PHYS_OFFSET + 0x100,
	.map_io         = godcube_map_io,
	.init_irq       = hisilicon_init_irq,
	.timer          = &godcube_timer,
	.init_machine   = godcube_init,
MACHINE_END
