/******************************************************************************
 *    COPYRIGHT (C) 2013 Czyong. Hisilicon
 *    All rights reserved.
 * ***
 *    Create by Czyong 2013-02-07
 *
******************************************************************************/
#ifndef ASMARM_RESOURCEH
#define ASMARM_RESOURCEH

/*****************************************************************************/

#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

struct device_resource {
	const char *name;
	struct resource *resource;
	int num_resources;
};

int find_cpu_resource(const char *name, struct resource **resource,
		      int *num_resources);

/*****************************************************************************/
#endif /* ASMARM_RESOURCEH */
