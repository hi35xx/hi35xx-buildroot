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
#ifdef	CONFIG_HISI_SNAPSHOT_BOOT
#include <linux/suspend.h>
#endif

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

#include <linux/bootmem.h>
#include <linux/delay.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/smp_twd.h>

#define A7_AXI_SCALE_REG   IO_ADDRESS(CRG_REG_BASE + 0x30)
#define REG_PERI_CRG57     IO_ADDRESS(CRG_REG_BASE + 0xe4)

#define get_bus_clk() ({\
		unsigned long tmp_reg, busclk;\
		tmp_reg = readl(A7_AXI_SCALE_REG);\
		if (0x0 == (tmp_reg & 0x8))\
			busclk = 198000000;\
		else \
			busclk = 148500000;\
		busclk;\
		})


void __iomem *hi3516a_gic_cpu_base_addr;
unsigned long long hi_sched_clock(void)
{
	return sched_clock();
}
EXPORT_SYMBOL(hi_sched_clock);

void __init hi3516a_gic_init_irq(void)
{
	edb_trace();
	hi3516a_gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;

#ifndef CONFIG_LOCAL_TIMERS
	gic_init(0, HISI_GIC_IRQ_START, (void __iomem *)
		 CFG_GIC_DIST_BASE, hi3516a_gic_cpu_base_addr);
#else
	/* git initialed include Local timer.
	* IRQ_LOCALTIMER is settled IRQ number for local timer interrupt.
	* It is set to 29 by ARM.
	*/
	gic_init(0, IRQ_LOCALTIMER, (void __iomem *)CFG_GIC_DIST_BASE,
		 (void __iomem *)CFG_GIC_CPU_BASE);
#endif
}

static struct map_desc hi3516a_io_desc[] __initdata = {
	{
		.virtual        = HI3516A_IOCH1_VIRT,
		.pfn            = __phys_to_pfn(HI3516A_IOCH1_PHYS),
		.length         = HI3516A_IOCH1_SIZE,
		.type           = MT_DEVICE
	},
	{
		.virtual        = HI3516A_IOCH2_VIRT,
		.pfn            = __phys_to_pfn(HI3516A_IOCH2_PHYS),
		.length         = HI3516A_IOCH2_SIZE,
		.type           = MT_DEVICE
	}
};

void __init hi3516a_map_io(void)
{
	int i;

	iotable_init(hi3516a_io_desc, ARRAY_SIZE(hi3516a_io_desc));

	for (i = 0; i < ARRAY_SIZE(hi3516a_io_desc); i++) {
		edb_putstr(" V: ");	edb_puthex(hi3516a_io_desc[i].virtual);
		edb_putstr(" P: ");	edb_puthex(hi3516a_io_desc[i].pfn);
		edb_putstr(" S: ");	edb_puthex(hi3516a_io_desc[i].length);
		edb_putstr(" T: ");	edb_putul(hi3516a_io_desc[i].type);
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
 * These are default clocks.
 */
static struct clk uart_clk = {
	.rate = 6000000,
};

static struct clk sp804_clk = {
	.rate = 12500000,
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

static void __init hi3516a_reserve(void)
{
}

void __init hi3516a_init(void)
{
	unsigned long i;
	unsigned long reg = 0;

	edb_trace();

	/* hi3516a ASIC uart use apb bus clk */
	reg = readl(REG_PERI_CRG57);
	reg &= ~UART_CKSEL_APB;
	writel(reg, REG_PERI_CRG57);

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		edb_trace();
		amba_device_register(amba_devs[i], &iomem_resource);
	}

}

/*****************************************************************************/

static void __init hi3516a_init_early(void)
{
	uart_clk.rate = get_bus_clk()/4;
	sp804_clk.rate = get_bus_clk()/4;

	clkdev_add_table(lookups, ARRAY_SIZE(lookups));

	edb_trace();
}
/*****************************************************************************/

void hi3516a_restart(char mode, const char *cmd)
{
	__raw_writel(~0, IO_ADDRESS(SYS_CTRL_BASE) + REG_SC_SYSRES);
}
/*****************************************************************************/
extern struct sys_timer hi3516a_sys_timer;

MACHINE_START(HI3516A, "hi3516a")
	.atag_offset	= 0x100,
	.map_io		= hi3516a_map_io,
	.init_early	= hi3516a_init_early,
	.init_irq	= hi3516a_gic_init_irq,
	.handle_irq	= gic_handle_irq,
	.timer		= &hi3516a_sys_timer,
	.init_machine	= hi3516a_init,
	.reserve	= hi3516a_reserve,
	.restart	= hi3516a_restart,
MACHINE_END
