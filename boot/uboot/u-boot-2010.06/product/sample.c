#include <common.h>
#include <boot/customer.h>
#include <linux/string.h>

extern int ddr_training_second_step(void);

const int product_control(void)
{
#ifdef CONFIG_DDR_TRAINING
	ddr_training_second_step();
#endif

#ifdef CONFIG_DDR_TRAINING_V200
	unsigned long value = 0;
	char *ddrt_print = getenv("ddrt_print");
	if (!ddrt_print)
		goto out;
	value = simple_strtoul(ddrt_print, 0, 10);
	if (value != 0)
		ddr_training_second_step();
out:
#endif
	return 0;
}
