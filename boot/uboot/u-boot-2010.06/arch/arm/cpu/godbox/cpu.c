#include <common.h>
#include <command.h>
#include <asm/system.h>
#include <asm/cache.h>
#include <asm/io.h>
#include <config.h>
#ifndef CONFIG_L2_OFF
#include <asm/arch/sys_proto.h>
#endif

static void cache_flush(void);

int cleanup_before_linux(void)
{
	unsigned int i;

	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */
	disable_interrupts();

	/* turn off I/D-cache */
	icache_disable();
	dcache_disable();

	/* invalidate I-cache */
	cache_flush();

	i = 0;
	/* mem barrier to sync up things */
	asm("mcr p15, 0, %0, c7, c10, 4": :"r"(i));


	return 0;
}

static void cache_flush(void)
{
	asm ("mcr p15, 0, %0, c7, c5, 0": :"r" (0));
}

void reset_cpu(ulong addr)
{
	/* unclock wdg */
	writel(0x1ACCE551, REG_BASE_WDG + 0x0C00);
	/* wdg load value */
	writel(0x00000100, REG_BASE_WDG + 0x0000);
	/* bit0: int enable bit1: reboot enable */
	writel(0x00000003, REG_BASE_WDG + 0x0008);
	while (1);
}
