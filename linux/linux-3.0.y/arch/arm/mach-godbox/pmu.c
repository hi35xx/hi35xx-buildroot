#include <linux/platform_device.h>
#include <asm/pmu.h>
#include <mach/irqs.h>
static struct resource  A9_pmu_resource = {
	.start = INTNR_A9_PMU_INT,
	.end   = INTNR_A9_PMU_INT,
	.flags = IORESOURCE_IRQ,
};

static struct platform_device A9_pmu_device = {
	.name = "arm-pmu",
	.id   = ARM_PMU_DEVICE_CPU,
	.resource = &A9_pmu_resource,
	.num_resources = 1,
};

static int __init pmu_init(void)
{
	platform_device_register(&A9_pmu_device);
	return 0;
};
arch_initcall(pmu_init);
