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

void boot_flag_init(void)
{
	unsigned long ret, bootrom, usb_update;

	/*get boot flag*/
	ret = __raw_readl(SYS_CTRL_REG_BASE + REG_SYSSTAT);
	bootrom = (ret >> 31) & 0x1;
	usb_update = (ret >> 23) & 0x1;
	ret >>= 4;
	ret = (ret & 0x3);

	switch (ret) {
	case 0x0:
		boot_media = BOOT_MEDIA_SPIFLASH;
		break;
	case 0x1:
		boot_media = BOOT_MEDIA_NAND;
		break;
	case 0x3:
		if (!bootrom)
			boot_media = BOOT_MEDIA_DDR;
		else if (bootrom && !usb_update)
			boot_media = BOOT_MEDIA_EMMC;
		break;
	default:
		boot_media = BOOT_MEDIA_NAND;
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
	reg &= ~UART_CKSEL_APB;
	writel(reg, CRG_REG_BASE + PERI_CRG57);

	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_arch_number = MACH_TYPE_HI3536;
	gd->bd->bi_boot_params = CFG_BOOT_PARAMS;
	gd->flags = 0;

	boot_flag_init();

	return 0;
}

#define USB_DETECT_PIN_OFF   23
#define UPDATE_FROM_USB_MASK  (0x1)
#define UBOOT_DATA_ADDR		(0x41000000)
#define UBOOT_DATA_SIZE		(0x80000)
#define UBOOT_DATA_FLAG_OFF      (0x3c)
#define UBOOT_DATA_FLAG_VAL      (0x12345678)
int save_bootdata_to_flash(void)
{
	unsigned int val;
	unsigned int start_mode;
	unsigned int  update_from_usb;
	void *buf;
	static struct spi_flash *flash;

	start_mode = readl(REG_BASE_SCTL + REG_SYSSTAT);
	update_from_usb = (start_mode >> USB_DETECT_PIN_OFF)
				& UPDATE_FROM_USB_MASK;
	if (update_from_usb) {
		val = readl(UBOOT_DATA_ADDR + UBOOT_DATA_FLAG_OFF);
		if (UBOOT_DATA_FLAG_VAL == val) {
			flash = spi_flash_probe(0, 0, 1000000, 0x3);
			if (!flash) {
				printf("Failed to initialize SPI flash\n");
				return -1;
			}

			/*
			 * erase the address range.
			 */
			printf("flash erase..., func:%s, line:%d\n",
					__func__, __LINE__);
			val = flash->erase(flash, 0, UBOOT_DATA_SIZE);
			if (val) {
				printf("SPI flash sector erase failed\n");
				return 1;
			}

			buf = map_physmem((unsigned long)UBOOT_DATA_ADDR,
					UBOOT_DATA_SIZE, MAP_WRBACK);
			if (!buf) {
				puts("Failed to map physical memory\n");
				return 1;
			}

			/* copy the data from RAM to FLASH */
			printf("flash write...\n");
			val = flash->write(flash, 0, UBOOT_DATA_SIZE, buf);
			if (val) {
				printf("SPI flash write failed, return %d\n",
						val);
				return 1;
			}

			unmap_physmem(buf, UBOOT_DATA_SIZE);
		}
	}

	return 0;
}

int misc_init_r(void)
{
#ifdef CONFIG_RANDOM_ETHADDR
	random_init_r();
#endif
	setenv("verify", "n");

#ifdef CONFIG_AUTO_UPDATE
	extern int do_auto_update(void);
#ifdef CFG_MMU_HANDLEOK
	dcache_stop();
#endif
	do_auto_update();
#ifdef CFG_MMU_HANDLEOK
	dcache_start();
#endif
#endif
	save_bootdata_to_flash();

	return 0;

}

int dram_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	gd->bd->bi_dram[0].start = CFG_DDR_PHYS_OFFSET;
	gd->bd->bi_dram[0].size = CFG_DDR_SIZE;

	return 0;
}

