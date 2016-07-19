/******************************************************************************
 *	Flash Memory Controller Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/mtd/mtd.h>

#include <mach/platform.h>

#include "hifmc_common.h"

/*****************************************************************************/
static unsigned int hifmc_ip_ver;

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
static unsigned char hifmc_current_dev_type = FLASH_TYPE_DEFAULT;
static unsigned char hifmc_boot_dev_type = FLASH_TYPE_DEFAULT;
#endif

/*****************************************************************************/
unsigned char hifmc_cs_user[CONFIG_HIFMC_MAX_CS_NUM];

/*****************************************************************************/
int hifmc_ip_ver_check(void)
{
	unsigned int base = IO_ADDRESS(CONFIG_HIFMC_REG_BASE);
	char buffer[50];

	if (hifmc_ip_ver == HIFMC_VER_100)
		return 0;

	sprintf(buffer, "Check Flash Memory Controller v100 ... ");
	hifmc_ip_ver = readl((void *)(base + FMC_VERSION));
	if (hifmc_ip_ver != HIFMC_VER_100) {
		pr_info("%s\n", buffer);
		return -EFAULT;
	}
	pr_info("%s Found.\n", buffer);

	return 0;
}

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
/*****************************************************************************/
unsigned char mtd_get_spi_type(struct mtd_info *mtd)
{
	unsigned char type = DEVICE_TYPE_UNKNOWN;

	switch (mtd->type) {
	case MTD_NORFLASH:
		type = FLASH_TYPE_SPI_NOR;
		break;
	case MTD_NANDFLASH:
		type = FLASH_TYPE_SPI_NAND;
		break;
	case MTD_ABSENT:
		type = FLASH_TYPE_DEFAULT;
	default:
		break;
	}

	return type;
}

/*****************************************************************************/
void mtd_dev_type_switch(unsigned char type)
{
	unsigned int reg, spi_device_type, flash_type;
	static const char * const str[] = {"SPI nor", "SPI nand", "Nand",
						"Boot"};

	if (hifmc_current_dev_type == type)
		return;

	FMC_PR(BT_DBG, "\t|*-Start switch current device type\n");

	if (type > FLASH_TYPE_DEFAULT) {
		FMC_PR(BT_DBG, "\t||-Switch unknown device type %d\n", type);
		return;
	}

	if (hifmc_boot_dev_type == FLASH_TYPE_DEFAULT) {
		reg = readl(__io_address(SYS_CTRL_BASE + REG_SC_STAT));
		FMC_PR(BT_DBG, "\t||-Get system STATUS[%#x]%#x\n",
				SYS_CTRL_BASE + REG_SC_STAT, reg);
		hifmc_boot_dev_type = GET_SPI_DEVICE_TYPE(reg);
		FMC_PR(BT_DBG, "\t||-Init boot device type to %s flash\n",
				str[hifmc_boot_dev_type]);
	}

	if (type == FLASH_TYPE_DEFAULT)
		spi_device_type = hifmc_boot_dev_type;
	else
		spi_device_type = type;

	FMC_PR(BT_DBG, "\t||-Switch type to %s flash\n", str[type]);

	reg = readl(__io_address(CONFIG_HIFMC_REG_BASE + FMC_CFG));
	FMC_PR(BT_DBG, "\t||-Get HIFMC CFG[%#x]%#x\n", FMC_CFG, reg);
	flash_type = (reg & FLASH_SEL_MASK) >> FLASH_SEL_SHIFT;
	if (spi_device_type != flash_type) {
		reg &= ~FLASH_SEL_MASK;
		reg |= FMC_CFG_FLASH_SEL(spi_device_type);
		writel(reg, __io_address(CONFIG_HIFMC_REG_BASE + FMC_CFG));
		FMC_PR(BT_DBG, "\t||-Set HIFMC CFG[%#x]%#x\n", FMC_CFG, reg);
	}
	hifmc_current_dev_type = spi_device_type;

	FMC_PR(BT_DBG, "\t|*-End switch current device type\n");
}
#endif /* End of CONFIG_HIFMC_SWITCH_DEV_TYPE */

/*****************************************************************************/
char *ulltostr(unsigned long long size)
{
	int ix;
	static char buffer[20];
	char *fmt[] = {"%u", "%uK", "%uM", "%uG", "%uT"};

	for (ix = 0; (ix < 5) && !(size & 0x3FF) && size; ix++)
		size = (size >> 10);

	sprintf(buffer, fmt[ix], size);
	return buffer;
}

/*****************************************************************************/
void debug_register_dump(void)
{
	int ix;
	unsigned int base = IO_ADDRESS(CONFIG_HIFMC_REG_BASE);

	pr_info("Register dump:");
	for (ix = 0; ix <= 0x98; ix += 0x04) {
		if (!(ix & 0x0F))
			pr_info("\n0x%08X: ", base + ix);
		pr_info("%08X ", readl((void __iomem *)(base + ix)));
	}
	pr_info("\n");
}

