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
#ifndef HDMI_BUILD_IN_BOOT
#include "hi_debug.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif


/******************************************************************************/
/* Macro type for hdmi module                                                 */
/******************************************************************************/
#define HDMI_CHIP_HI3536  0x35360100 

#define HDMI_CTRL_BASE_ADDR 0x13010000

#define HDMI_VMALLOC(size)     vmalloc(size, GFP_KERNEL)
#define HDMI_KMALLOC(size)     kmalloc(size, GFP_KERNEL)

#define HDMI_KFREE(viraddr)    kfree(viraddr)
#define HDMI_VFREE(viraddr)    vfree(viraddr)

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

