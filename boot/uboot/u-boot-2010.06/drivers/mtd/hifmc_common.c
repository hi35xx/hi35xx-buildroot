/*
 * The Flash Memory Controller v100 Device Driver for hisilicon
 *
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
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

#include <common.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <hifmc_common.h>

/*****************************************************************************/
static unsigned char hifmc_current_dev_type = FLASH_TYPE_DEFAULT;
static unsigned char hifmc_boot_dev_type = FLASH_TYPE_DEFAULT;

/*****************************************************************************/
unsigned int hifmc_ip_ver;
unsigned char hifmc_ip_user;
unsigned char hifmc_cs_user[CONFIG_HIFMC_MAX_CS_NUM];

/*****************************************************************************/
int hifmc_ip_ver_check(void)
{
	if (hifmc_ip_ver == HIFMC_VER_100)
		return 0;

	printf("Check Flash Memory Controller v100 ...");
	hifmc_ip_ver = readl(CONFIG_HIFMC_REG_BASE + FMC_VERSION);
	if (hifmc_ip_ver != HIFMC_VER_100) {
		printf("\n");
		return -EFAULT;
	}
	printf(" Found\n");

	return 0;
}

/*****************************************************************************/
void hifmc_dev_type_switch(unsigned char type)
{
	unsigned int reg, spi_device_type, flash_type;
	const char *str[] = {"SPI nor", "SPI nand", "Nand", "Boot"};

	if (hifmc_current_dev_type == type)
		return;

	FMC_PR(BT_DBG, "\t|*-Start switch current device type\n");

	if (type > FLASH_TYPE_DEFAULT) {
		FMC_PR(BT_DBG, "\t||-Switch unknown device type %d\n", type);
		return;
	}

	if (hifmc_boot_dev_type == FLASH_TYPE_DEFAULT) {
		reg = readl((void *)(SYS_CTRL_REG_BASE + REG_SYSSTAT));
		FMC_PR(BT_DBG, "\t||-Get system STATUS[%#x]%#x\n",
				SYS_CTRL_REG_BASE + REG_SYSSTAT, reg);
		hifmc_boot_dev_type = GET_SPI_DEVICE_TYPE(reg);
		FMC_PR(BT_DBG, "\t||-Init boot device type to %s flash\n",
				str[hifmc_boot_dev_type]);
	}

	if (type == FLASH_TYPE_DEFAULT)
		spi_device_type = hifmc_boot_dev_type;
	else
		spi_device_type = type;

	FMC_PR(BT_DBG, "\t||-Switch type to %s flash\n", str[type]);

	reg = readl((void *)(CONFIG_HIFMC_REG_BASE + FMC_CFG));
	FMC_PR(BT_DBG, "\t||-Get HIFMC CFG[%#x]%#x\n", FMC_CFG, reg);
	flash_type = (reg & FLASH_SEL_MASK) >> FLASH_SEL_SHIFT;
	if (spi_device_type != flash_type) {
		reg &= ~FLASH_SEL_MASK;
		reg |= FMC_CFG_FLASH_SEL(spi_device_type);
		writel(reg, (void *)(CONFIG_HIFMC_REG_BASE + FMC_CFG));
		FMC_PR(BT_DBG, "\t||-Set HIFMC CFG[%#x]%#x\n", FMC_CFG, reg);
	}
	hifmc_current_dev_type = spi_device_type;

	FMC_PR(BT_DBG, "\t|*-End switch current device type\n");
}

/*****************************************************************************/
char *ulltostr(unsigned long long size)
{
	int ix;
	static char buffer[20];
	char *fmt[] = {"%u", "%uK", "%uM", "%uG", "%uT"};

	for (ix = 0; (ix < 4) && !(size & 0x3FF) && size; ix++)
		size = (size >> 10);

	sprintf(buffer, fmt[ix], size);
	return buffer;
}

/*****************************************************************************/
void debug_register_dump(void)
{
	int ix;
	unsigned int base = CONFIG_HIFMC_REG_BASE;

	printf("Register dump:");
	for (ix = 0; ix <= 0x98; ix += 0x04) {
		if (!(ix & 0x0F))
			printf("\n0x%08X: ", base + ix);
		printf("%08X ", readl((void *)(base + ix)));
	}
	printf("\n");
}

