/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2003
 * Texas Instruments, <www.ti.com>
 * Kshitij Gupta <Kshitij@ti.com>
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/sizes.h>
#include <asm/arch/platform.h>
#include <spi_flash.h>

static int boot_media = BOOT_MEDIA_UNKNOW;

#if defined(CONFIG_SHOW_BOOT_PROGRESS)
void show_boot_progress(int progress)
{
	printf("Boot reached stage %d\n", progress);
}
#endif

#define COMP_MODE_ENABLE ((unsigned int)0x0000EAEF)

static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b" : "=r" (loops) : "0" (loops));
}

/* get uboot start media. */
int get_boot_media(void)
{
	return boot_media;
}

int get_text_base(void)
{
	return TEXT_BASE;
}

#define PERI_CRG48  0xc0
#define PERI_CRG49  0xc4

#define FMC_CLKEN       (1<<1)
#define EMMC_CLKEN      (1<<17)

void close_emmc_clock(void)
{
    unsigned int regval;

    regval = readl(CRG_REG_BASE + PERI_CRG49);
    regval &= ~EMMC_CLKEN;
    writel(regval,CRG_REG_BASE + PERI_CRG49);
}

void close_flash_clock(void)
{
    unsigned int regval;

    regval = readl(CRG_REG_BASE + PERI_CRG48);
    regval &= ~FMC_CLKEN;
    writel(regval,CRG_REG_BASE + PERI_CRG48);
}

static void boot_flag_init(void)
{
	unsigned int regval, boot_mode;

	/* get boot mode */
	regval = __raw_readl(SYS_CTRL_REG_BASE + REG_SYSSTAT);
	boot_mode = GET_SYS_BOOT_MODE(regval);

	switch (boot_mode) {
	/* [5:4] 00b - boot from spi device */
	case BOOT_FROM_SPI:
		/* get SPI device type, (0: SPI nor | 1: SPI nand) */
		if (GET_SPI_DEVICE_TYPE(regval))
			boot_media = BOOT_MEDIA_NAND;
		else
			boot_media = BOOT_MEDIA_SPIFLASH;
		close_emmc_clock();
		break;
	/* [5:4] 01b - boot from Nand device */
	case BOOT_FROM_NAND:
		boot_media = BOOT_MEDIA_NAND;
		close_emmc_clock();
		break;
	/* [5:4] 10b - boot from emmc */
	case BOOT_FROM_EMMC:
		boot_media = BOOT_MEDIA_EMMC;
		close_flash_clock();
		break;
	/* [5:4] 11b - boot from PCI-E slave */
	case BOOT_FROM_DDR:
		boot_media = BOOT_MEDIA_DDR;
		close_flash_clock();
		close_emmc_clock();
		break;
	default:
		boot_media = BOOT_MEDIA_UNKNOW;
		break;
	}
}

/*
 * Miscellaneous platform dependent initialisations
 */
int board_init(void)
{
	unsigned long reg;
	/* set uart clk from apb bus */
	reg = readl(CRG_REG_BASE + PERI_CRG57);
	reg |= UART_CKSEL_24M;
	writel(reg, CRG_REG_BASE + PERI_CRG57);

	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_arch_number = MACH_TYPE_HI3559;
	gd->bd->bi_boot_params = CFG_BOOT_PARAMS;
	gd->flags = 0;

	boot_flag_init();

	return 0;
}

int auto_update_flag=0;
int bare_chip_program=0;

#define BIT_UPDATE_FROM_SDIO0   23
#define UPDATE_FROM_SDIO0_MASK  (0x1)

#define GPIO1_1_ADDR 0x12141008
#define GPIO1_1_MASK 0x1<<1

int is_bare_program(void)
{
	unsigned int start_mode;
	unsigned int  val;
	start_mode = readl(REG_BASE_SCTL + REG_SYSSTAT);
	val = (start_mode >> BIT_UPDATE_FROM_SDIO0)
				& UPDATE_FROM_SDIO0_MASK;
	if (val) 
		return 1;
	else
		return 0;
}

int is_auto_update(void)
{
#ifdef CONFIG_AUTO_SD_UPDATE
	unsigned int  val[3];
	val[0] = readl(GPIO1_1_ADDR) & GPIO1_1_MASK;
	if(val[0])
		return 0;
	udelay(10000);
	val[1] = readl(GPIO1_1_ADDR) & GPIO1_1_MASK;
	udelay(10000);
	val[2] = readl(GPIO1_1_ADDR) & GPIO1_1_MASK;
	udelay(10000);

	if (val[0] == val[1] && val[1] == val[2] && val[0] == 0) 
		return 1;//update
	else
		return 0;
#else
	return 0;
#endif
}

int misc_init_r(void)
{
#ifdef CONFIG_RANDOM_ETHADDR
	random_init_r();
#endif
	setenv("verify", "n");

extern int do_auto_update(void);
#ifdef CONFIG_AUTO_UPDATE
#ifdef CFG_MMU_HANDLEOK
	dcache_stop();
#endif
	//do_auto_update();
#ifdef CFG_MMU_HANDLEOK
	dcache_start();
#endif
#endif

    if(auto_update_flag || bare_chip_program)
        do_auto_update();
	//save_bootdata_to_flash();

	return 0;

}

int dram_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	gd->bd->bi_dram[0].start = CFG_DDR_PHYS_OFFSET;
	gd->bd->bi_dram[0].size = CFG_DDR_SIZE;

	return 0;
}
