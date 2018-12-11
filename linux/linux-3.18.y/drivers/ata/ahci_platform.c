/*
 * AHCI SATA platform driver
 *
 * Copyright 2004-2005  Red Hat, Inc.
 *   Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2010  MontaVista Software, LLC.
 *   Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/libata.h>
#include <linux/ahci_platform.h>
#include "ahci.h"

static unsigned int ncq_en = CONFIG_HI_SATA_NCQ;
module_param(ncq_en, uint, 0600);
MODULE_PARM_DESC(ncq_en, "ahci ncq flag (default:1)");
extern unsigned int sata_port_map;

static const struct ata_port_info ahci_port_info = {
	.flags		= AHCI_FLAG_COMMON,
	.pio_mask	= ATA_PIO4,
	.udma_mask	= ATA_UDMA6,
	.port_ops	= &ahci_platform_ops,
};

static int ahci_probe(struct platform_device *pdev)
{
#if !defined(CONFIG_ARCH_HI3536C)
	struct device *dev = &pdev->dev;
#endif
	struct ahci_host_priv *hpriv;
	int rc;

	hpriv = ahci_platform_get_resources(pdev);
	if (IS_ERR(hpriv))
		return PTR_ERR(hpriv);

	rc = ahci_platform_enable_resources(hpriv);
	if (rc)
		return rc;

#if defined(CONFIG_ARCH_HI3536C)
	hpriv->type = ORI_AHCI;
	hpriv->force_port_map = sata_port_map;
	if (!ncq_en)
		 hpriv->flags |= AHCI_HFLAG_NO_NCQ;
#else
	if (of_device_is_compatible(dev->of_node, "hisilicon,hisi-ahci"))
		hpriv->flags |= AHCI_HFLAG_NO_FBS | AHCI_HFLAG_NO_NCQ;
#endif

	rc = ahci_platform_init_host(pdev, hpriv, &ahci_port_info);
	if (rc)
		goto disable_resources;

	return 0;
disable_resources:
	ahci_platform_disable_resources(hpriv);
	return rc;
}

static SIMPLE_DEV_PM_OPS(ahci_pm_ops, ahci_platform_suspend,
			 ahci_platform_resume);

static const struct of_device_id ahci_of_match[] = {
	{ .compatible = "generic-ahci", },
	/* Keep the following compatibles for device tree compatibility */
	{ .compatible = "snps,spear-ahci", },
	{ .compatible = "snps,exynos5440-ahci", },
	{ .compatible = "ibm,476gtr-ahci", },
	{ .compatible = "snps,dwc-ahci", },
	{ .compatible = "hisilicon,hisi-ahci", },
	{},
};
MODULE_DEVICE_TABLE(of, ahci_of_match);

static struct platform_driver ahci_driver = {
	.probe = ahci_probe,
	.remove = ata_platform_remove_one,
	.driver = {
		.name = "ahci",
		.owner = THIS_MODULE,
		.of_match_table = ahci_of_match,
		.pm = &ahci_pm_ops,
	},
};
module_platform_driver(ahci_driver);

MODULE_DESCRIPTION("AHCI SATA platform driver");
MODULE_AUTHOR("Anton Vorontsov <avorontsov@ru.mvista.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ahci");
