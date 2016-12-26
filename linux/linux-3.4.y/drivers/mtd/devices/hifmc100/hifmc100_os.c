/******************************************************************************
 *	Flash Memory Controller v100 Device Driver
 *	Copyright (c) 2014 - 2015 by Hisilicon
 *	All rights reserved.
 * ***
 *	Create by hisilicon
 *
 *****************************************************************************/

/*****************************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include "../../mtdcore.h"
#include "../../hifmc_common.h"
#include "hifmc100.h"
#include "hifmc100_os.h"

/*****************************************************************************/
static inline int mtd_has_partitions(void) { return 1; }

/*****************************************************************************/
static void hifmc100_probe_spi_size(struct hifmc_host *host)
{
	unsigned int ix, total = 0;
	struct hifmc_spi *spi = host->spi;
	struct mtd_info *mtd = host->mtd;
	int erasesize = spi->erasesize;

	FMC_PR(BT_DBG, "\t|*-Start probe SPI nor flash total size\n");

	for (ix = 0; ix < host->num_chip; ix++, spi++) {
		FMC_PR(BT_DBG, "\t||-SPI nor flash[%d]: %dMB\n", ix,
				(u_int)(spi->chipsize >> 20));
		total += spi->chipsize;
	}

	mtd->size = total;
	mtd->erasesize = erasesize;

	FMC_PR(BT_DBG, "\t|*-Probe SPI nor total size: %dMB, chip num: %d\n",
			(u_int)(mtd->size >> 20), host->num_chip);
}

/*****************************************************************************/
static int hifmc100_driver_probe(struct platform_device *plat_dev)
{
	int result = -EIO;
	struct hifmc_host *host;
	struct mtd_info *mtd;
	int nr_parts = 0;
	struct mtd_partition *parts = NULL;

	FMC_PR(BT_DBG, "\t*-Start SPI nor flash driver probe\n");

	/* Hifmc ip version check */
	result = hifmc_ip_ver_check();
	if (result) {
		FMC_PR(BT_DBG, "\t*-IP version unknown, result: %d\n", result);
		return result;
	}

	FMC_PR(BT_DBG, "\t|-Allocate SPI nor driver host space\n");
	host = kmalloc(sizeof(struct hifmc_host), GFP_KERNEL);
	if (!host) {
		DB_MSG("Error: Failed to allocate host structure.\n");
		return -ENOMEM;
	}
	memset((char *)host, 0, sizeof(struct hifmc_host));

	FMC_PR(BT_DBG, "\t|-Save SPI nor driver private data: host\n");
	platform_set_drvdata(plat_dev, host);


	host->regbase = ioremap_nocache(CONFIG_HIFMC_REG_BASE,
			HIFMC_REG_ADDRESS_LEN);
	if (!host->regbase) {
		DB_MSG("Error: SPI nor reg base-address ioremap failed.\n");
		result = -EFAULT;
		goto fail;
	}

	host->iobase = ioremap_nocache(CONFIG_HIFMC_BUFFER_BASE,
			SPI_NOR_BUFFER_LEN);
	if (!host->iobase) {
		DB_MSG("Error: SPI nor buffer base-address ioremap failed.\n");
		result = -EFAULT;
		goto fail;
	}

	FMC_PR(BT_DBG, "\t|-Allocate SPI nor DMA buffer space\n");
	host->buffer = dma_alloc_coherent(host->dev, HIFMC100_DMA_MAX_SIZE,
			&host->dma_buffer, GFP_KERNEL);
	if (!host->buffer) {
		DB_BUG("Error: Can't allocate memory for dma buffer.");
		result = -ENOMEM;
		goto fail;
	}

	mutex_init(&host->lock);

	mtd = host->mtd;
	mtd->name = (char *)plat_dev->name;
	mtd->type = MTD_NORFLASH;
	mtd->writesize = 1;
	mtd->flags = MTD_CAP_NORFLASH;
	mtd->owner = THIS_MODULE;

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	FMC_PR(BT_DBG, "\t|-Switch device type to SPI nor\n");
	mtd_switch_spi_type(mtd);
#endif

	result = hifmc100_spi_nor_init(host);
	if (result) {
		FMC_PR(BT_DBG, "SPI Nor init failed, result: %d\n", result);
		result = -ENODEV;
		goto fail;
	}

	result = hifmc100_spi_nor_scan(host);
	if (!result && host->num_chip) {
		hifmc100_probe_spi_size(host);
		pr_info("SPI Nor total size: %uMB\n", (u_int)(mtd->size >> 20));
	} else
		goto fail;

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	FMC_PR(BT_DBG, "\t|-Switch device type to default\n");
	mtd->type = MTD_ABSENT;
	mtd_switch_spi_type(mtd);
	mtd->type = MTD_NORFLASH;
#endif

#ifdef CONFIG_PM
	host->suspend = hifmc100_suspend;
	host->resume  = hifmc100_resume;
#endif

	if (mtd_has_partitions()) {
		static char const *part_probes[] = {
			"cmdlinepart",
			NULL,
		};

		nr_parts = parse_mtd_partitions(host->mtd,
				part_probes, &parts, 0);
		FMC_PR(BT_DBG, "parse mtd partitions: %d\n", nr_parts);
		if (nr_parts > 0)
			host->add_partition = 1;
	}

	result = mtd_device_register(host->mtd, parts, nr_parts);
	if (result) {
		kfree(parts);
		parts = NULL;
	}

	return (1 == result) ? -ENODEV : 0;

fail:
	if (host->buffer)
		dma_free_coherent(host->dev, HIFMC100_DMA_MAX_SIZE,
			host->buffer, host->dma_buffer);

	if (host->iobase)
		iounmap(host->iobase);
	if (host->regbase)
		iounmap(host->regbase);

	platform_set_drvdata(plat_dev, NULL);
	kfree(host);

	FMC_PR(BT_DBG, "\t*-End SPI nor flash driver probe\n");

	return result;
}

/*****************************************************************************/
static int hifmc100_driver_remove(struct platform_device *plat_dev)
{
	struct hifmc_host *host = platform_get_drvdata(plat_dev);

	host->set_system_clock(NULL, DISABLE);

	mtd_device_unregister(host->mtd);

	if (host->buffer)
		dma_free_coherent(host->dev, HIFMC100_DMA_MAX_SIZE,
			host->buffer, host->dma_buffer);

	if (host->iobase)
		iounmap(host->iobase);
	if (host->regbase)
		iounmap(host->regbase);

	platform_set_drvdata(plat_dev, NULL);
	kfree(host);

	return 0;
}

/*****************************************************************************/
static void hifmc100_driver_shutdown(struct platform_device *pltdev)
{
	if (start_up_addr_mode == SPI_NOR_ADDR_MODE_3_BYTES) {
		int ix;
		struct hifmc_host *host = platform_get_drvdata(pltdev);
		struct mtd_info *mtd = host->mtd;
		struct hifmc_spi *spi;

		if (host)
			spi = host->spi;
		else
			return;

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
		mutex_lock(&spi_type_mutex);
		mtd_switch_spi_type(mtd);
#endif
		mutex_lock(&host->lock);
		for (ix = 0; ix < host->num_chip; ix++, spi++) {
			if (spi->addrcycle == 4) {
				spi->driver->wait_ready(spi);
				spi->driver->entry_4addr(spi, DISABLE);
			}
		}
		mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
		mutex_unlock(&spi_type_mutex);
#endif
	}
}

#ifdef CONFIG_PM
/*****************************************************************************/
static int hifmc100_suspend(struct platform_device *dev, pm_message_t state)
{
	unsigned char cs, chip_num = CONFIG_SPI_NOR_MAX_CHIP_NUM;
	struct hifmc_host *host = platform_get_drvdata(dev);
	struct mtd_info *mtd = host->mtd;
	struct hifmc_spi *spi = host->spi;

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_lock(&spi_type_mutex);
	mtd_switch_spi_type(mtd);
#endif
	mutex_lock(&host->lock);
	for (cs = 0; chip_num && (cs < CONFIG_HIFMC_MAX_CS_NUM); cs++, spi++) {
		spi->driver->wait_ready(spi);
		chip_num--;
	}

	if (host->set_system_clock)
		host->set_system_clock(0, DISABLE);
	FMC_PR(PM_DBG, "\t|-disable system clock\n");
	mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_unlock(&spi_type_mutex);
#endif

	return 0;
}
/*****************************************************************************/
static int hifmc100_resume(struct platform_device *dev)
{
	struct hifmc_host *host = platform_get_drvdata(dev);
	struct mtd_info *mtd = host->mtd;

#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_lock(&spi_type_mutex);
	mtd_switch_spi_type(mtd);
#endif
	mutex_lock(&host->lock);
	hifmc100_spi_nor_config(host);
	mutex_unlock(&host->lock);
#ifdef CONFIG_HIFMC_SWITCH_DEV_TYPE
	mutex_unlock(&spi_type_mutex);
#endif

	return 0;
}
/*****************************************************************************/
static int hifmc100_driver_suspend(struct platform_device *dev,
		pm_message_t state)
{
	struct hifmc_host *host = platform_get_drvdata(dev);

	if (host && host->suspend)
		return (host->suspend)(dev, state);

	return 0;
}

/*****************************************************************************/
static int hifmc100_driver_resume(struct platform_device *dev)
{
	struct hifmc_host *host = platform_get_drvdata(dev);

	if (host && host->resume)
		return (host->resume)(dev);

	return 0;
}
#endif /* End of CONFIG_PM */

/*****************************************************************************/
static void hifmc100_pltdev_release(struct device *dev)
{
}

/*****************************************************************************/
static struct resource hifmc100_device_resources[] = {
	[0] = {
		.start	= CONFIG_HIFMC_REG_BASE,
		.end	= CONFIG_HIFMC_REG_BASE + HIFMC_REG_ADDRESS_LEN - 1,
		.flags	= IORESOURCE_MEM,
		.name	= "base"
	},
	[1] = {
		.start	= CONFIG_HIFMC_BUFFER_BASE,
		.end	= CONFIG_HIFMC_BUFFER_BASE + SPI_NOR_BUFFER_LEN - 1,
		.flags	= IORESOURCE_MEM,
		.name	= "buffer"
	},
};

/*****************************************************************************/
static struct platform_driver hifmc100_driver_pltdrv = {
	.driver.name	= "hi_sfc",
	.probe		= hifmc100_driver_probe,
	.remove		= hifmc100_driver_remove,
	.shutdown	= hifmc100_driver_shutdown,
#ifdef CONFIG_PM
	.suspend	= hifmc100_driver_suspend,
	.resume		= hifmc100_driver_resume,
#endif
	.driver.owner	= THIS_MODULE,
	.driver.bus	= &platform_bus_type,
};

/*****************************************************************************/
static u64 hifmc_dmamask = DMA_BIT_MASK(32);

/*****************************************************************************/
static struct platform_device hifmc100_device_pltdev = {
	.name			= "hi_sfc",
	.id			= -1,

	.dev.platform_data	= NULL,
	.dev.dma_mask		= &hifmc_dmamask,
	.dev.coherent_dma_mask	= DMA_BIT_MASK(32),
	.dev.release		= hifmc100_pltdev_release,

	.num_resources		= ARRAY_SIZE(hifmc100_device_resources),
	.resource		= hifmc100_device_resources,
};

/*****************************************************************************/
static int __init hifmc100_module_init(void)
{
	int result = 0;

	result = platform_driver_register(&hifmc100_driver_pltdrv);
	if (result < 0)
		return result;

	result = platform_device_register(&hifmc100_device_pltdev);
	if (result < 0) {
		platform_driver_unregister(&hifmc100_driver_pltdrv);
		return result;
	}

	return result;
}

/*****************************************************************************/
static void __exit hifmc100_module_exit(void)
{
	platform_device_unregister(&hifmc100_device_pltdev);
	platform_driver_unregister(&hifmc100_driver_pltdrv);
}

/*****************************************************************************/
module_init(hifmc100_module_init);
module_exit(hifmc100_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BVT_BSP");
MODULE_DESCRIPTION("Hisilicon Flash Memory Controller V100 SPI Nor Driver");

