
#ifndef __BOOT_HDMI_INTF_H__
#define __BOOT_HDMI_INTF_H__
#include "hi_type.h"
#include "hdmi_hal.h"
#include "drv_hdmi_common.h"
#if defined(CONFIG_HDMI_STB_SDK) 
#include <uboot.h>
#elif defined(CONFIG_HDMI_BVT_SDK)
#include "ubi_uboot.h"
#endif

#define HDMI_FATAL(fmt...)    
#define HDMI_ERR(fmt...)     
#define HDMI_WARN(fmt...)     
#define HDMI_INFO(fmt...)   
#undef offsetof
#define offsetof(__type, __member) ((unsigned int) &((__type *)0)->__member)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#undef container_of
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

typedef struct 
{
    HI_U32             u32HdmiDevId;
    HDMI_TMDS_MODE_E   enTmdsMode;
    HDMI_ATTR_S        stAttr;
    HDMI_INFOFRAME_S   stInfoFrame;
    HDMI_HAL_S*        pstHdmiHal;
}HDMI_DEVICE_S;

typedef enum hihiVO_INTF_SYNC_E
{
    VO_OUTPUT_PAL = 0,
    VO_OUTPUT_NTSC,
    VO_OUTPUT_960H_PAL,              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
    VO_OUTPUT_960H_NTSC,             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/
    
    VO_OUTPUT_1080P24,
    VO_OUTPUT_1080P25,
    VO_OUTPUT_1080P30,
    
    VO_OUTPUT_720P50, 
    VO_OUTPUT_720P60,   
    VO_OUTPUT_1080I50,
    VO_OUTPUT_1080I60,    
    VO_OUTPUT_1080P50,
    VO_OUTPUT_1080P60,            

    VO_OUTPUT_576P50,
    VO_OUTPUT_480P60,

    VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/    
    VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x2160_30,          /* 1920x2160_30 */
    VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/ 
    VO_OUTPUT_2560x1440_30,          /* 2560x1440_30 */
    VO_OUTPUT_2560x1600_60,          /* 2560x1600_60 */
    VO_OUTPUT_3840x2160_30,          /* 3840x2160_30 */
    VO_OUTPUT_3840x2160_60,          /* 3840x2160_60 */
    
    VO_OUTPUT_USER,
    VO_OUTPUT_BUTT

} HI_VO_INTF_SYNC_E;

#endif//__BOOT_HDMI_INTF_H__