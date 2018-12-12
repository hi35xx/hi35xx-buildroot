/*
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/sizes.h>
#include <asm/arch/platform.h>
#include <spi_flash.h>
#include <mmc.h>

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

void boot_flag_init(void)
{
	unsigned int reg, boot_mode, spi_device_mode;

	/* get boot flag */
	reg = __raw_readl(SYS_CTRL_REG_BASE + REG_SYSSTAT);
	boot_mode = (reg & BOOT_MODE_MASK) >> BOOT_MODE_SHIFT;

	switch (boot_mode) {
	case BOOT_FROM_FMC:	/* spi mode   0:spi nor  1:spi nand */
		spi_device_mode = GET_SPI_DEVICE_TYPE(reg);
		if (spi_device_mode)
			boot_media = BOOT_MEDIA_NAND;
		else
			boot_media = BOOT_MEDIA_SPIFLASH;
		break;
	case BOOT_FROM_EMMC:	/* emmc mode */
		boot_media = BOOT_MEDIA_EMMC;
		break;
	default:
		boot_media = BOOT_MEDIA_UNKNOW;
		break;
	}
}

static void sys_clk_rest_init(void)
{
	unsigned int reg;

	reg = readl(CRG_REG_BASE + PERI_CRG57);

	if (CONFIG_CONS_INDEX == 0) {
		reg &= ~UART0_SRST;
		reg |= UART0_CLK_EN;
	} else if (CONFIG_CONS_INDEX == 1) {
		reg &= ~UART1_SRST;
		reg |= UART1_CLK_EN;
	} else {
		reg &= ~UART2_SRST;
		reg |= UART2_CLK_EN;
	}

	/* set uart clk to 24M */
	reg &= ~UART_CLK_SEL_MASK;
	if (CONFIG_PL011_CLOCK == CFG_CLK_FIXED_24M)
		reg |= UART_CLK_SEL_24M << UART_CLK_SEL_SHIFT;
	else
		reg |= UART_CLK_SEL_6M << UART_CLK_SEL_SHIFT;

	writel(reg, CRG_REG_BASE + PERI_CRG57);
}
/*
 * Miscellaneous platform dependent initialisations
 */
int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	sys_clk_rest_init();

	gd->bd->bi_arch_number = MACH_TYPE_HI3516CV300;
	gd->bd->bi_boot_params = CFG_BOOT_PARAMS;
	gd->flags = 0;

	boot_flag_init();

	return 0;
}

#define UBOOT_DATA_ADDR		(0x81000000)
#define UBOOT_DATA_SIZE		(0x80000)
int save_bootdata_to_flash(void)
{
	unsigned int val;
	unsigned int sd_update_flag;
	void *buf;
	static struct spi_flash *flash;

	sd_update_flag = readl(REG_BASE_SCTL + REG_SC_GEN4);
	if (sd_update_flag == CONFIG_START_MAGIC) {
		if (boot_media == BOOT_MEDIA_SPIFLASH) {
			flash = spi_flash_probe(0, 0, 1000000, 0x3);
			if (!flash) {
				printf("Failed to initialize SPI flash\n");
				return -1;
			}

			/*
			 * erase the address range.
			 */
			printf("flash erase...\n");
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
#if defined(CONFIG_EMMC_SUPPORT)
		else if (boot_media == BOOT_MEDIA_EMMC) {
			struct mmc *mmc = find_mmc_device(0);

			if (!mmc)
				return 1;

			printf("MMC write...\n");

			mmc_init(mmc);

			buf = map_physmem((unsigned long)UBOOT_DATA_ADDR,
					UBOOT_DATA_SIZE, MAP_WRBACK);
			if (!buf) {
				puts("Failed to map physical memory\n");
				return 1;
			}
			mmc->block_dev.block_write(0, 0,
					(UBOOT_DATA_SIZE >> 9), buf);
			unmap_physmem(buf, UBOOT_DATA_SIZE);
		}
#endif

		val = readl(MISC_REG_BASE + MISC_CTRL47);
		val |= 0x1;
		printf("update success! clear update mode!\n");
		writel(val, MISC_REG_BASE + MISC_CTRL47);
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

