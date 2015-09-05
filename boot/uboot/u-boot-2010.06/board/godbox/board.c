
#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/sizes.h>
#include <asm/arch/platform.h>

static int boot_media = BOOT_MEDIA_UNKNOW;

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0" (loops));
}

/* get uboot start media. */
int get_boot_media(void)
{
	return boot_media;
}

void boot_flag_init(void)
{
	unsigned long ret;

	/*get boot flag*/
	ret = __raw_readl(REG_BASE_PERI_CTRL + REG_START_MODE);
	ret >>= 9;
        ret = (ret & 0x3);

	switch (ret) {
	case 0x0:
		boot_media = BOOT_MEDIA_SPIFLASH;
		break;
	case 0x1:
		boot_media = BOOT_MEDIA_NAND;
		break;
	case 0x3:
		boot_media = BOOT_MEDIA_EMMC;
		break;
	}
}

/*
 * Miscellaneous platform dependent initialisations
 */
int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	
	gd->bd->bi_arch_number = MACH_TYPE_GODBOX;
	gd->bd->bi_boot_params = CFG_BOOT_PARAMS;
	gd->flags = 0;

	boot_flag_init();	
	return 0;
}

int misc_init_r (void)
{
#ifdef CONFIG_RANDOM_ETHADDR
	random_init_r();
#endif

	/*
	 * optimize startup time, remove the follow codes.
	 * modify by CaiZhiYong 2011.04.25
	 * 
	udelay(300000);
	setenv("verify", "n");
	 */
	
	return (0);
}

static void display_info(void)
{
	printf("ETH FastBoot:  V3.0\n");
	printf("Machine ID:    %d v%d\n",MACH_TYPE_GODBOX,0); 
	printf("Build Date:    "__DATE__", "__TIME__"\n\n");
	printf("Serial Number: %s\n\n", CONFIG_PRODUCTNAME);
}

int dram_init (void)
{
	  DECLARE_GLOBAL_DATA_PTR;
         gd->bd->bi_dram[0].start = CFG_DDR_PHYS_OFFSET;
         gd->bd->bi_dram[0].size = CFG_DDR_SIZE;

	display_info();
	return 0;
}
