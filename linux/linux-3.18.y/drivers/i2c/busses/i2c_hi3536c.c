#include <mach/platform.h>

unsigned int get_apb_clk(void)
{
	unsigned int apb_clk;

	apb_clk = get_bus_clk() / CFG_TIMER_PER ;

	return apb_clk;
}

static struct resource hi_i2c0_resources[] = {
	[0] = {
		.start          = CONFIG_HI_I2C0_IO_BASE,
		.end            = CONFIG_HI_I2C0_IO_BASE
			+ CONFIG_HI_I2C0_IO_SIZE  - 1,
		.flags          = IORESOURCE_MEM,
	},
};

struct hi_platform_i2c hi_i2c0_platform_data = {
	.clk_limit      = CONFIG_HI_I2C0_CLK_LIMIT,
	.i2c_class      = I2C_CLASS_DDC,
};

struct platform_device hi_i2c0_device = {
	.name           = HI_I2C,
	.id             = 0,
	.resource       = hi_i2c0_resources,
	.num_resources  = ARRAY_SIZE(hi_i2c0_resources),
	.dev            = {
		.platform_data  = &hi_i2c0_platform_data,
	}
};

static struct platform_device *hi_i2c_devices[] __initdata = {
	&hi_i2c0_device,
};

static int __init hi_i2c_module_init(void)
{
	int ret;


	ret = platform_add_devices(hi_i2c_devices, ARRAY_SIZE(hi_i2c_devices));
	if (ret) {
		hi_err("i2c device register failed!\n");
		return ret;
	}

	ret = platform_driver_register(&hi_i2c_driver);
	if (ret) {
		platform_device_unregister(&hi_i2c0_device);
		hi_err("i2c driver register failed!\n");
		return ret;
	}

	return ret;
}

static void __exit hi_i2c_module_exit(void)
{
	platform_driver_unregister(&hi_i2c_driver);

	platform_device_unregister(&hi_i2c0_device);
}
