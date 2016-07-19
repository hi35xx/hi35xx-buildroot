#include <linux/platform_device.h>
#include <asm/pmu.h>
#include <mach/irqs.h>

#define INTNR_A7_PMU_INT0 64

static struct resource  pmu_resource[] = {
	[0] = {
		.start = INTNR_A7_PMU_INT0,
		.end   = INTNR_A7_PMU_INT0,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device A7_pmu_device = {
	.name = "arm-pmu",
	.id   = ARM_PMU_DEVICE_CPU,
	.resource = pmu_resource,
	.num_resources = ARRAY_SIZE(pmu_resource),
};

static int __init pmu_init(void)
{
	platform_device_register(&A7_pmu_device);
	return 0;
};

arch_initcall(pmu_init);
