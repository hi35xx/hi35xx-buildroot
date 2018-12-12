/******************************************************************************

  Copyright (C), 2010-2026, Hisilicon. Co., Ltd.

 ******************************************************************************
 * File Name     : hdmi_platform.h
 * Version       : 1.0
 * Author        : Hisilicon multimedia software group
 * Created       : 2015/6/27
 * Description   :
 * History       :
 * Date          : 2015/6/27
 * Author        : l00232354
 * Modification  :
 *******************************************************************************
 */
#ifndef __HDMI_PLATFORM_H__
#define __HDMI_PLATFORM_H__
#include "hi_type.h"
#ifdef CONFIG_HDMI_STB_SDK
#include "hi_module.h"
#include "hi_debug.h"
#ifndef HDMI_BUILD_IN_BOOT
#include "hi_drv_mem.h"
#include <asm/io.h>
#endif

#ifdef HI_MINIBOOT_SUPPORT
#include <malloc.h>
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/******************************************************************************/
/* Macro type for hdmi module                                                 */
/******************************************************************************/
#define HDMI_CHIP_HI3536C               (0x3536c100)

#define HDMI_CTRL_BASE_ADDR             (0x13000000)
#define HDMI_ADDR_BASE_IO_CFG           (0x120F0000)
#define HDMI_ADDR_CRG                   (0x12040000)
#define BASE_ADDR_tx_dphy               (0x13030000)
/* the mux IO reg should not config on FPGA board */
#define HDMI_ADDR_IO_CFG_HOTPLUG        (HDMI_ADDR_BASE_IO_CFG + 0x174)
#define HDMI_ADDR_IO_CFG_SDA            (HDMI_ADDR_BASE_IO_CFG + 0x17c)
#define HDMI_ADDR_IO_CFG_SCL            (HDMI_ADDR_BASE_IO_CFG + 0x180)
#define HDMI_ADDR_IO_CFG_CEC            (HDMI_ADDR_BASE_IO_CFG + 0x178) /* CEC will not be used on 21D */

#define HDMI_COLOR_BAR_MASK             (0x70000000)
#define HDMI_COLOR_BAR_UPDATE_MASK      (0x00000001)
#define HDMI_COLOR_BAR_BASE             (0x1302c000)

#define HDMI_VMALLOC(size)              vmalloc(size, GFP_KERNEL)
#define HDMI_KMALLOC(size)              kmalloc(size, GFP_KERNEL)

#define HDMI_KFREE(viraddr)             kfree(viraddr)
#define HDMI_VFREE(viraddr)             vfree(viraddr)

#ifndef HDMI_BUILD_IN_BOOT
#define HDMI_IO_MAP(phys_addr, len)     (ioremap_nocache(phys_addr, len))
#define HDMI_IO_UNMAP(virt_addr)        if(virt_addr){iounmap(virt_addr);}
#else
#define HDMI_IO_MAP(phys_addr, len)     (phys_addr)
#define HDMI_IO_UNMAP(virt_addr)
#endif
/******************************************************************************/
/* Data Structures                                                            */
/******************************************************************************/

/******************************************************************************/
/* Gloabl Data                                                                */
/******************************************************************************/

/******************************************************************************/
/* Public Interface                                                           */
/******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

