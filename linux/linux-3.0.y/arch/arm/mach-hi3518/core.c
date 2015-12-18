#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/sysdev.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/amba/pl061.h>
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

#define ioaddr_intc(x)	IO_ADDRESS(REG_BASE_INTC + (x))
#define SYS_CTL_VIRT	IO_ADDRESS(SYS_CTRL_BASE)
#define REG_SC_CTRL	0x0

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

static struct map_desc hi3518_io_desc[] __initdata = {
	{
		.virtual        = HI3518_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(HI3518_IOCH1_PHYS),
		.length         = HI3518_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	{
		.virtual        = HI3518_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3518_IOCH2_PHYS),
		.length         = HI3518_IOCH2_SIZE,
		.type           = MT_DEVICE
	}
};

static unsigned long hi3518_timer_reload, timer0_clk_hz, timer1_clk_hz,
		     timer0_clk_khz, timer1_clk_khz;

#define REG_CRG4		IO_ADDRESS(CRG_REG_BASE + 0x10)
#define REG_CRG5		IO_ADDRESS(CRG_REG_BASE + 0x14)

/*
 * bus clk configuration
 *
 * BPLL : DDR : AXI : APB (AHB)
 *      :  4  :  2  : 1
 *
 * AXI busclk comes from BPPL (NOT from APLL)
 *         AXI busclk = (24M * fbdiv) / (2 * refdiv * pstdiv1 * pstdiv2)
 */
#define HW_REG(a) (*(unsigned long *)(a))
#define get_bus_clk() ({\
		unsigned long fbdiv, refdiv, pstdiv1, pstdiv2;\
		unsigned long tmp_reg, foutvco, busclk;\
		tmp_reg = HW_REG(REG_CRG4);\
		pstdiv1 = (tmp_reg >> 24) & 0x7;\
		pstdiv2 = (tmp_reg >> 27) & 0x7;\
		tmp_reg = HW_REG(REG_CRG5);\
		refdiv = (tmp_reg >> 12) & 0x3f;\
		fbdiv = tmp_reg & 0xfff;\
		foutvco = 24000000/refdiv;\
		foutvco *= fbdiv;\
		busclk = foutvco/(2 * pstdiv1 * pstdiv2);\
		busclk;\
		})

static void early_init(void)
{
	unsigned long busclk;

	edb_trace();
	busclk = get_bus_clk();
	printk(KERN_INFO "AXI bus clock %ld.\n", busclk);

	hi3518_timer_reload = BUSCLK_TO_TIMER_RELOAD(busclk);
	timer0_clk_hz = BUSCLK_TO_TIMER0_CLK_HZ(busclk);
	timer0_clk_khz = BUSCLK_TO_TIMER0_CLK_KHZ(busclk);
	timer1_clk_hz = BUSCLK_TO_TIMER1_CLK_HZ(busclk);
	timer1_clk_khz = BUSCLK_TO_TIMER1_CLK_KHZ(busclk);
}

void __init hi3518_map_io(void)
{
	int i;

	iotable_init(hi3518_io_desc, ARRAY_SIZE(hi3518_io_desc));

	for (i = 0; i < ARRAY_SIZE(hi3518_io_desc); i++) {
		edb_putstr(" V: ");     edb_puthex(hi3518_io_desc[i].virtual);
		edb_putstr(" P: ");     edb_puthex(hi3518_io_desc[i].pfn);
		edb_putstr(" S: ");     edb_puthex(hi3518_io_desc[i].length);
		edb_putstr(" T: ");     edb_putul(hi3518_io_desc[i].type);
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

static void notrace hi3518_update_sched_clock(void)
{
	u32 cyc = ~readl(CFG_TIMER01_VABASE + REG_TIMER1_VALUE);
	update_sched_clock(&cd, cyc, (u32)~0);
}

unsigned long long notrace sched_clock(void)
{
	u32 cyc = ~readl(CFG_TIMER01_VABASE + REG_TIMER1_VALUE);

	return cyc_to_sched_clock(&cd, cyc, (u32)~0);
}

static struct clocksource hi3518_clocksource;


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
		writel(hi3518_timer_reload, CFG_TIMER01_VABASE
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
static irqreturn_t hi3518_timer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER01_VABASE+REG_TIMER_RIS)) & 0x1) {
		writel(~0, CFG_TIMER01_VABASE + REG_TIMER_INTCLR);
		timer0_clockevent.event_handler(&timer0_clockevent);
	}

	return IRQ_HANDLED;
}

static irqreturn_t hi3518_freetimer_interrupt(int irq, void *dev_id)
{
	if ((readl(CFG_TIMER01_VABASE+REG_TIMER1_RIS)) & 0x1) {
		free_timer_overflows++;
		writel(~0, CFG_TIMER01_VABASE + REG_TIMER1_INTCLR);
	}
	return IRQ_HANDLED;
}

static struct irqaction hi3518_timer_irq = {
	.name           = "System Timer Tick",
	.flags          = IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER,
	.handler        = hi3518_timer_interrupt,
};

static struct irqaction hi3518_freetimer_irq = {
	.name           = "Free Timer",
	.flags          = IRQF_SHARED | IRQF_TIMER,
	.handler        = hi3518_freetimer_interrupt,
};
static cycle_t hi3518_get_cycles(struct clocksource *cs)
{
	return ~readl(CFG_TIMER01_VABASE + REG_TIMER1_VALUE);
}

static struct clocksource hi3518_clocksource = {
	.name           = "timer1",
	.rating         = 200,
	.read           = hi3518_get_cycles,
	.mask           = CLOCKSOURCE_MASK(32),
	.flags          = CLOCK_SOURCE_IS_CONTINUOUS,
};

static int __init hi3518_clocksource_init(void)
{
	unsigned long rate = timer1_clk_khz * 1000;
	writel(0, CFG_TIMER01_VABASE + REG_TIMER1_CONTROL);
	writel(0xffffffff, CFG_TIMER01_VABASE + REG_TIMER1_RELOAD);
	writel(0xffffffff, CFG_TIMER01_VABASE + REG_TIMER1_VALUE);
	writel(CFG_TIMER_CONTROL, CFG_TIMER01_VABASE + REG_TIMER1_CONTROL);

	/* caculate the mult/shift by clock rate to gain more accratury */
	if (clocksource_register_hz(&hi3518_clocksource, rate))
		panic("register clocksouce :%s error\n",
				hi3518_clocksource.name);

	/* force check the mult/shift of clocksource */
	init_fixed_sched_clock(&cd, hi3518_update_sched_clock, 32, rate,
			hi3518_clocksource.mult, hi3518_clocksource.shift);

	return 0;
}

static void __init hi3518_timer_init(void)
{
	unsigned int val;

	edb_trace();

	/* set timer clock source */
	val = readl(SYS_CTL_VIRT + REG_SC_CTRL);
	val |= ((1<<16) | (1<<18) | (1<<20));
	writel(val, SYS_CTL_VIRT + REG_SC_CTRL);

	setup_irq(TIMER01_IRQ, &hi3518_timer_irq);
	setup_irq(TIMER01_IRQ, &hi3518_freetimer_irq);

	hi3518_clocksource_init();
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

struct sys_timer hi3518_timer = {
	.init           = hi3518_timer_init,
};

#define HIL_AMBADEV_NAME(name) hil_ambadevice_##name

#define HIL_AMBA_DEVICE(name, busid, base, platdata)			\
	static struct amba_device HIL_AMBADEV_NAME(name) =		\
	{								\
		.dev            = {                                     \
			.coherent_dma_mask = ~0,                        \
			.init_name = busid,                             \
			.platform_data = platdata,                      \
		},                                                      \
		.res            = {                                     \
			.start  = base##_BASE,				\
			.end    = base##_BASE + 0x1000 - 1,		\
			.flags  = IORESOURCE_IO,                        \
		},                                                      \
		.dma_mask       = ~0,                                   \
		.irq            = { base##_IRQ, NO_IRQ }		\
	}

#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
static struct pl061_platform_data gpio0_plat_data = {
	.gpio_base	= 0,
	.irq_base	= GPIO0_IRQ_START,
};

static struct pl061_platform_data gpio1_plat_data = {
	.gpio_base	= 8,
	.irq_base	= GPIO1_IRQ_START,
};

static struct pl061_platform_data gpio2_plat_data = {
	.gpio_base	= 16,
	.irq_base	= GPIO2_IRQ_START,
};

static struct pl061_platform_data gpio3_plat_data = {
	.gpio_base	= 24,
	.irq_base	= GPIO3_IRQ_START,
};

static struct pl061_platform_data gpio4_plat_data = {
	.gpio_base	= 32,
	.irq_base	= GPIO4_IRQ_START,
};

static struct pl061_platform_data gpio5_plat_data = {
	.gpio_base	= 40,
	.irq_base	= GPIO5_IRQ_START,
};

static struct pl061_platform_data gpio6_plat_data = {
	.gpio_base	= 48,
	.irq_base	= GPIO6_IRQ_START,
};

static struct pl061_platform_data gpio7_plat_data = {
	.gpio_base	= 56,
	.irq_base	= GPIO7_IRQ_START,
};

static struct pl061_platform_data gpio8_plat_data = {
	.gpio_base	= 64,
	.irq_base	= GPIO8_IRQ_START,
};

static struct pl061_platform_data gpio9_plat_data = {
	.gpio_base	= 72,
	.irq_base	= GPIO9_IRQ_START,
};

static struct pl061_platform_data gpio10_plat_data = {
	.gpio_base	= 80,
	.irq_base	= GPIO10_IRQ_START,
};

static struct pl061_platform_data gpio11_plat_data = {
	.gpio_base	= 88,
	.irq_base	= GPIO11_IRQ_START,
};
#endif

HIL_AMBA_DEVICE(uart0,  "uart:0",     UART0,  NULL);
HIL_AMBA_DEVICE(uart1,  "uart:1",     UART1,  NULL);
#if defined(CONFIG_ARM_SP805_WATCHDOG) || defined(CONFIG_ARM_SP805_WATCHDOG_MODULE)
HIL_AMBA_DEVICE(wdog,   "dev:wdog",   WDG,    NULL);
#endif
#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
HIL_AMBA_DEVICE(gpio0,  "dev:gpio0",  GPIO0,  &gpio0_plat_data);
HIL_AMBA_DEVICE(gpio1,  "dev:gpio1",  GPIO1,  &gpio1_plat_data);
HIL_AMBA_DEVICE(gpio2,  "dev:gpio2",  GPIO2,  &gpio2_plat_data);
HIL_AMBA_DEVICE(gpio3,  "dev:gpio3",  GPIO3,  &gpio3_plat_data);
HIL_AMBA_DEVICE(gpio4,  "dev:gpio4",  GPIO4,  &gpio4_plat_data);
HIL_AMBA_DEVICE(gpio5,  "dev:gpio5",  GPIO5,  &gpio5_plat_data);
HIL_AMBA_DEVICE(gpio6,  "dev:gpio6",  GPIO6,  &gpio6_plat_data);
HIL_AMBA_DEVICE(gpio7,  "dev:gpio7",  GPIO7,  &gpio7_plat_data);
HIL_AMBA_DEVICE(gpio8,  "dev:gpio8",  GPIO8,  &gpio8_plat_data);
HIL_AMBA_DEVICE(gpio9,  "dev:gpio9",  GPIO9,  &gpio9_plat_data);
HIL_AMBA_DEVICE(gpio10, "dev:gpio10", GPIO10, &gpio10_plat_data);
HIL_AMBA_DEVICE(gpio11, "dev:gpio11", GPIO11, &gpio11_plat_data);
#endif

static struct amba_device *amba_devs[] __initdata = {
	&HIL_AMBADEV_NAME(uart0),
	&HIL_AMBADEV_NAME(uart1),
#if defined(CONFIG_ARM_SP805_WATCHDOG) || defined(CONFIG_ARM_SP805_WATCHDOG_MODULE)
	&HIL_AMBADEV_NAME(wdog),
#endif
#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
	&HIL_AMBADEV_NAME(gpio0),
	&HIL_AMBADEV_NAME(gpio1),
	&HIL_AMBADEV_NAME(gpio2),
	&HIL_AMBADEV_NAME(gpio3),
	&HIL_AMBADEV_NAME(gpio4),
	&HIL_AMBADEV_NAME(gpio5),
	&HIL_AMBADEV_NAME(gpio6),
	&HIL_AMBADEV_NAME(gpio7),
	&HIL_AMBADEV_NAME(gpio8),
	&HIL_AMBADEV_NAME(gpio9),
	&HIL_AMBADEV_NAME(gpio10),
	&HIL_AMBADEV_NAME(gpio11),
#endif
};

/*
 * These are fixed clocks.
 */
static struct clk uart_clk = {
	.rate   = 3000000,
};

#if defined(CONFIG_ARM_SP805_WATCHDOG) || defined(CONFIG_ARM_SP805_WATCHDOG_MODULE)
static struct clk wdog_clk = {
	.rate	= 3000000,
};
#endif

#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
static struct clk gpio_clk = {
	.rate	= CONFIG_DEFAULT_BUSCLK,
};
#endif

static struct clk_lookup lookups[] = {
	{       /* UART0 */
		.dev_id         = "uart:0",
		.clk            = &uart_clk,
	},
	{       /* UART1 */
		.dev_id         = "uart:1",
		.clk            = &uart_clk,
	},
#if defined(CONFIG_ARM_SP805_WATCHDOG) || defined(CONFIG_ARM_SP805_WATCHDOG_MODULE)
	{	/* WDOG */
		.dev_id		= "dev:wdog",
		.clk		= &wdog_clk,
	},
#endif
#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
	{	/* GPIO0 */
		.dev_id		= "dev:gpio0",
		.clk		= &gpio_clk,
	},
	{	/* GPIO1 */
		.dev_id		= "dev:gpio1",
		.clk		= &gpio_clk,
	},
	{	/* GPIO2 */
		.dev_id		= "dev:gpio2",
		.clk		= &gpio_clk,
	},
	{	/* GPIO3 */
		.dev_id		= "dev:gpio3",
		.clk		= &gpio_clk,
	},
	{	/* GPIO4 */
		.dev_id		= "dev:gpio4",
		.clk		= &gpio_clk,
	},
	{	/* GPIO5 */
		.dev_id		= "dev:gpio5",
		.clk		= &gpio_clk,
	},
	{	/* GPIO6 */
		.dev_id		= "dev:gpio6",
		.clk		= &gpio_clk,
	},
	{	/* GPIO7 */
		.dev_id		= "dev:gpio7",
		.clk		= &gpio_clk,
	},
	{	/* GPIO8 */
		.dev_id		= "dev:gpio8",
		.clk		= &gpio_clk,
	},
	{	/* GPIO9 */
		.dev_id		= "dev:gpio9",
		.clk		= &gpio_clk,
	},
	{	/* GPIO10 */
		.dev_id		= "dev:gpio10",
		.clk		= &gpio_clk,
	},
	{	/* GPIO11 */
		.dev_id		= "dev:gpio11",
		.clk		= &gpio_clk,
	},
#endif
};

void __init hi3518_init(void)
{
	unsigned long i;

	edb_trace();

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		edb_trace();
		amba_device_register(amba_devs[i], &iomem_resource);
	}
}

MACHINE_START(HI3518, "hi3518")
	.boot_params    = PLAT_PHYS_OFFSET + 0x100,
	.map_io         = hi3518_map_io,
	.init_irq       = hisilicon_init_irq,
	.timer          = &hi3518_timer,
	.init_machine   = hi3518_init,
MACHINE_END
