#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/clocksource.h>
#include <linux/amba/pl061.h>
#include <linux/clockchips.h>
#include <linux/cnt32_to_63.h>
#include <linux/io.h>

#ifdef	CONFIG_HISI_SNAPSHOT_BOOT
#include <linux/suspend.h>
#endif

#include <linux/clkdev.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/leds.h>
#include <asm/hardware/arm_timer.h>
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

#include <linux/bootmem.h>
#include <linux/delay.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/smp_twd.h>

unsigned long long hi_sched_clock(void)
{
	return sched_clock();
}
EXPORT_SYMBOL(hi_sched_clock);

void __init hi3518ev200_vic_init_irq(void)
{
	edb_trace();

	vic_init((void __iomem *)IO_ADDRESS(REG_BASE_INTC), INTNR_IRQ_START,
			~0, 0);
}

static struct map_desc hi3518ev200_io_desc[] __initdata = {
	/* hi3518ev200_IOCH1 */
	{
		.virtual        = HI3518EV200_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(HI3518EV200_IOCH1_PHYS),
		.length         = HI3518EV200_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	/* hi3518ev200_IOCH1 */
	{
		.virtual        = HI3518EV200_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3518EV200_IOCH2_PHYS),
		.length         = HI3518EV200_IOCH2_SIZE,
		.type           = MT_DEVICE
	}
};

void __init hi3518ev200_map_io(void)
{
	int i;

	iotable_init(hi3518ev200_io_desc, ARRAY_SIZE(hi3518ev200_io_desc));

	for (i = 0; i < ARRAY_SIZE(hi3518ev200_io_desc); i++) {
		edb_putstr(" V: ");
		edb_puthex(hi3518ev200_io_desc[i].virtual);
		edb_putstr(" P: ");
		edb_puthex(hi3518ev200_io_desc[i].pfn);
		edb_putstr(" S: ");
		edb_puthex(hi3518ev200_io_desc[i].length);
		edb_putstr(" T: ");
		edb_putul(hi3518ev200_io_desc[i].type);
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
HIL_AMBA_DEVICE(uart2, "uart:2",  UART2,    NULL);

#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
static struct pl061_platform_data gpio0_plat_data = {
	.gpio_base	= 0,
	.irq_base	= INTNR_GPIO0_START,
};

static struct pl061_platform_data gpio1_plat_data = {
	.gpio_base	= 8,
	.irq_base	= INTNR_GPIO1_START,
};

static struct pl061_platform_data gpio2_plat_data = {
	.gpio_base	= 16,
	.irq_base	= INTNR_GPIO2_START,
};

static struct pl061_platform_data gpio3_plat_data = {
	.gpio_base	= 24,
	.irq_base	= INTNR_GPIO3_START,
};

static struct pl061_platform_data gpio4_plat_data = {
	.gpio_base	= 32,
	.irq_base	= INTNR_GPIO4_START,
};

static struct pl061_platform_data gpio5_plat_data = {
	.gpio_base	= 40,
	.irq_base	= INTNR_GPIO5_START,
};

static struct pl061_platform_data gpio6_plat_data = {
	.gpio_base	= 48,
	.irq_base	= INTNR_GPIO6_START,
};

static struct pl061_platform_data gpio7_plat_data = {
	.gpio_base	= 56,
	.irq_base	= INTNR_GPIO7_START,
};

static struct pl061_platform_data gpio8_plat_data = {
	.gpio_base	= 64,
	.irq_base	= INTNR_GPIO8_START,
};

HIL_AMBA_DEVICE(gpio0,  "dev:gpio0",  GPIO0,  &gpio0_plat_data);
HIL_AMBA_DEVICE(gpio1,  "dev:gpio1",  GPIO1,  &gpio1_plat_data);
HIL_AMBA_DEVICE(gpio2,  "dev:gpio2",  GPIO2,  &gpio2_plat_data);
HIL_AMBA_DEVICE(gpio3,  "dev:gpio3",  GPIO3,  &gpio3_plat_data);
HIL_AMBA_DEVICE(gpio4,  "dev:gpio4",  GPIO4,  &gpio4_plat_data);
HIL_AMBA_DEVICE(gpio5,  "dev:gpio5",  GPIO5,  &gpio5_plat_data);
HIL_AMBA_DEVICE(gpio6,  "dev:gpio6",  GPIO6,  &gpio6_plat_data);
HIL_AMBA_DEVICE(gpio7,  "dev:gpio7",  GPIO7,  &gpio7_plat_data);
HIL_AMBA_DEVICE(gpio8,  "dev:gpio8",  GPIO8,  &gpio8_plat_data);
#endif

static struct amba_device *amba_devs[] __initdata = {
	&HIL_AMBADEV_NAME(uart0),
	&HIL_AMBADEV_NAME(uart1),
	&HIL_AMBADEV_NAME(uart2),
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
#endif
};

/*****************************************************************************/
/*
 * These are fixed clocks.
 */
static struct clk uart_clk = {
	.rate = 24000000,
};

static struct clk sp804_clk = {
	.rate = 3000000,
};

#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
static struct clk gpio_clk = {
	.rate = CONFIG_DEFAULT_BUSCLK,
};
#endif

static struct clk_lookup lookups[] = {
	{	/* UART0 */
		.dev_id		= "uart:0",
		.clk		= &uart_clk,
	}, { /* UART1 */
		.dev_id		= "uart:1",
		.clk		= &uart_clk,
	}, { /* UART2 */
		.dev_id		= "uart:2",
		.clk		= &uart_clk,
	}, { /* SP804 timers */
		.dev_id		= "sp804",
		.clk		= &sp804_clk,
	},
#if defined(CONFIG_GPIO_PL061) || defined(CONFIG_GPIO_PL061_MODULE)
	{ /* GPIO0 */
		.dev_id		= "dev:gpio0",
		.clk		= &gpio_clk,
	},
	{ /* GPIO1 */
		.dev_id		= "dev:gpio1",
		.clk		= &gpio_clk,
	},
	{ /* GPIO2 */
		.dev_id		= "dev:gpio2",
		.clk		= &gpio_clk,
	},
	{ /* GPIO3 */
		.dev_id		= "dev:gpio3",
		.clk		= &gpio_clk,
	},
	{ /* GPIO4 */
		.dev_id		= "dev:gpio4",
		.clk		= &gpio_clk,
	},
	{ /* GPIO5 */
		.dev_id		= "dev:gpio5",
		.clk		= &gpio_clk,
	},
	{ /* GPIO6 */
		.dev_id		= "dev:gpio6",
		.clk		= &gpio_clk,
	},
	{ /* GPIO7 */
		.dev_id		= "dev:gpio7",
		.clk		= &gpio_clk,
	},
	{ /* GPIO8 */
		.dev_id		= "dev:gpio8",
		.clk		= &gpio_clk,
	},
#endif
};

static void __init hi3518ev200_reserve(void)
{
}

void __init hi3518ev200_init(void)
{
	unsigned long i;

	edb_trace();
	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		edb_trace();
		amba_device_register(amba_devs[i], &iomem_resource);
	}
}

/*****************************************************************************/

static void __init hi3518ev200_init_early(void)
{
	unsigned long reg = 0;

	/* hi3518ev200 ASIC uart use 24M clk */
	reg = readl(IO_ADDRESS(CRG_REG_BASE + REG_CRG57));
	reg &= ~UART_CLK_MASK;
	reg |= UART_CLK_24M;
	writel(reg, IO_ADDRESS(CRG_REG_BASE + REG_CRG57));

	uart_clk.rate = 24000000;
	sp804_clk.rate = get_bus_clk()/4;

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	edb_trace();
}
/*****************************************************************************/

void hi3518ev200_restart(char mode, const char *cmd)
{
	__raw_writel(~0, IO_ADDRESS(SYS_CTRL_BASE + REG_SC_SYSRES));
}
/*****************************************************************************/
extern struct sys_timer hi3518ev200_sys_timer;

MACHINE_START(HI3518EV200, "hi3518ev200")
	.atag_offset	= 0x100,
	.map_io		= hi3518ev200_map_io,
	.init_early	= hi3518ev200_init_early,
	.init_irq	= hi3518ev200_vic_init_irq,
	.handle_irq	= vic_handle_irq,
	.timer		= &hi3518ev200_sys_timer,
	.init_machine	= hi3518ev200_init,
	.reserve	= hi3518ev200_reserve,
	.restart	= hi3518ev200_restart,
MACHINE_END
