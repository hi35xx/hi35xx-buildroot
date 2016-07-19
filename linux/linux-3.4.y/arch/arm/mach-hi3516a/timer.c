/******************************************************************************
 *
 *    COPYRIGHT (C) 2013 Hisilicon
 *    All rights reserved.
 *
******************************************************************************/

#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/clockchips.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/percpu.h>

#include <asm/hardware/gic.h>
#include <asm/localtimer.h>

#include <mach/platform.h>
#include <mach/irqs.h>
#include <asm/hardware/timer-sp.h>
#include <asm/mach/time.h>
#include <asm/sched_clock.h>
#include <asm/hardware/arm_timer.h>
#include <asm/smp_twd.h>

#if !defined(CONFIG_LOCAL_TIMERS)
#  undef CONFIG_HAVE_ARM_LOCAL_TIMER_HI3516A
#  undef CONFIG_HAVE_SP804_LOCAL_TIMER_HI3516A
#endif
/*****************************************************************************/

struct hi_timer_t {
	int init;          /* timer initialize times */
	int index;         /* timer index */
	char *name;
	unsigned int addr; /* timer control base address */
	struct irqaction irq;
	void *priv;
};

#define HI_TIMER(_index, _irq, _addr) { \
	.init = 0, \
	.index = _index, \
	.name = "timer"#_index, \
	.addr = IO_ADDRESS(_addr), \
	.priv = NULL, \
	.irq.irq  = _irq,\
	.irq.name = "timer"#_index, \
	.irq.flags = (IRQF_TIMER | IRQF_NOBALANCING | IRQF_PERCPU), \
}

static struct hi_timer_t hi_timer[] = {
	HI_TIMER(0, INTNR_TIMER_0, REG_BASE_TIMER0),
	HI_TIMER(1, INTNR_TIMER_0, REG_BASE_TIMER0 + 0x20),
	HI_TIMER(2, INTNR_TIMER_1, REG_BASE_TIMER1),
	HI_TIMER(3, INTNR_TIMER_1, REG_BASE_TIMER1 + 0x20),
};

#define TIMER(_index)        (&hi_timer[_index])

#define GET_SMP_TIMER(_cpu)  (&hi_timer[((_cpu) << 1) + 2])

#ifdef CONFIG_HAVE_SP804_LOCAL_TIMER_HI3516A
/*****************************************************************************/
static long __init sp804_get_clock_rate(const char *name)
{
	struct clk *clk;
	long rate;
	int err;

	clk = clk_get_sys("sp804", name);
	if (IS_ERR(clk)) {
		pr_err("sp804: %s clock not found: %d\n", name,
			(int)PTR_ERR(clk));
		return PTR_ERR(clk);
	}

	err = clk_prepare(clk);
	if (err) {
		pr_err("sp804: %s clock failed to prepare: %d\n", name, err);
		clk_put(clk);
		return err;
	}

	err = clk_enable(clk);
	if (err) {
		pr_err("sp804: %s clock failed to enable: %d\n", name, err);
		clk_unprepare(clk);
		clk_put(clk);
		return err;
	}

	rate = clk_get_rate(clk);
	if (rate < 0) {
		pr_err("sp804: %s clock failed to get rate: %ld\n", name, rate);
		clk_disable(clk);
		clk_unprepare(clk);
		clk_put(clk);
	}

	return rate;
}

static unsigned long local_timer_rate;

static void sp804_set_mode(enum clock_event_mode mode,
			   struct clock_event_device *evt)
{
	struct hi_timer_t *timer = GET_SMP_TIMER(smp_processor_id());
	unsigned long ctrl = TIMER_CTRL_32BIT | TIMER_CTRL_IE;
	unsigned int clkevt_base = timer->addr;

	writel(ctrl, clkevt_base + TIMER_CTRL);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		writel(DIV_ROUND_CLOSEST(local_timer_rate, HZ),
			clkevt_base + TIMER_LOAD);
		ctrl |= TIMER_CTRL_PERIODIC | TIMER_CTRL_ENABLE;
		break;

	case CLOCK_EVT_MODE_ONESHOT:
		/* period set, and timer enabled in 'next_event' hook */
		ctrl |= TIMER_CTRL_ONESHOT;
		break;

	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	default:
		break;
	}

	writel(ctrl, clkevt_base + TIMER_CTRL);
}

static int sp804_set_next_event(unsigned long next,
				struct clock_event_device *evt)
{
	struct hi_timer_t *timer = GET_SMP_TIMER(smp_processor_id());
	unsigned int clkevt_base = timer->addr;
	unsigned long ctrl = readl(clkevt_base + TIMER_CTRL);

	writel(next, clkevt_base + TIMER_LOAD);
	writel(ctrl | TIMER_CTRL_ENABLE, clkevt_base + TIMER_CTRL);

	return 0;
}

static irqreturn_t sp804_timer_isr(int irq, void *dev_id)
{
	struct hi_timer_t *timer = (struct hi_timer_t *)dev_id;
	unsigned int clkevt_base = timer->addr;
	struct clock_event_device *evt
		= (struct clock_event_device *)timer->priv;

	/* clear the interrupt */
	writel(1, clkevt_base + TIMER_INTCLR);

	evt->event_handler(evt);

	return IRQ_HANDLED;
}
/*****************************************************************************/

/* every should have it's timer irq. */
static int __cpuinit hi3516a_local_timer_setup(struct clock_event_device *evt)
{
	unsigned int cpu = smp_processor_id();
	struct hi_timer_t *timer = GET_SMP_TIMER(cpu);
	struct irqaction *irq = &timer->irq;

	evt->name = timer->name;
	evt->irq  = irq->irq;
	evt->features = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT
		| CLOCK_EVT_FEAT_C3STOP;

	evt->set_mode = sp804_set_mode;
	evt->set_next_event = sp804_set_next_event;
	evt->rating = 350;

	timer->priv = (void *)evt;

	clockevents_config_and_register(evt, local_timer_rate, 0xf, 0xffffffff);
	irq_set_affinity(evt->irq, evt->cpumask);
	enable_irq(evt->irq);

	return 0;
}

static void hi3516a_local_timer_stop(struct clock_event_device *evt)
{
	evt->set_mode(CLOCK_EVT_MODE_UNUSED, evt);
	disable_irq(evt->irq);
}
/*****************************************************************************/

static struct local_timer_ops hi3516a_timer_tick_ops __cpuinitdata = {
	.setup	= hi3516a_local_timer_setup,
	.stop	= hi3516a_local_timer_stop,
};

static void __init hi3516a_local_timer_init(void)
{
	struct hi_timer_t *cpu0_timer = GET_SMP_TIMER(0);
	local_timer_rate = sp804_get_clock_rate("sp804");

	cpu0_timer->irq.handler = sp804_timer_isr;
	cpu0_timer->irq.dev_id = (void *)cpu0_timer;
	setup_irq(cpu0_timer->irq.irq, &cpu0_timer->irq);
	disable_irq(cpu0_timer->irq.irq);

	local_timer_register(&hi3516a_timer_tick_ops);
}
#endif /* CONFIG_HAVE_SP804_LOCAL_TIMER_HI3516A */

/*****************************************************************************/
#ifdef CONFIG_HAVE_ARM_LOCAL_TIMER_HI3516A
DEFINE_TWD_LOCAL_TIMER(twd_localtimer,
		      (resource_size_t)(REG_BASE_A7_PERI
			      + REG_A7_PERI_PRI_TIMER_WDT),
		      (resource_size_t)IRQ_LOCALTIMER);
#endif
static void __init hi3516a_timer_init(void)
{
	/* set the bus clock for all timer */
	writel(readl(IO_ADDRESS(SYS_CTRL_BASE)) |
		(1<<16) | (1<<18) | (1<<20) | (1<<22),
		IO_ADDRESS(SYS_CTRL_BASE));
#ifdef CONFIG_HAVE_ARM_LOCAL_TIMER_HI3516A
	twd_local_timer_register(&twd_localtimer);
#endif

#ifdef CONFIG_HAVE_SP804_LOCAL_TIMER_HI3516A
	hi3516a_local_timer_init();
#endif
	sp804_clocksource_and_sched_clock_init((void *)TIMER(0)->addr,
		TIMER(0)->name);
	sp804_clockevents_init((void *)TIMER(1)->addr,
		TIMER(1)->irq.irq, TIMER(1)->name);
}
/*****************************************************************************/

struct sys_timer hi3516a_sys_timer = {
	.init = hi3516a_timer_init,
};

