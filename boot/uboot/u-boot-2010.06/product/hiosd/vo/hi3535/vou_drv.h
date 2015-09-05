/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_drv.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/06/28
  Description   : vou_drv.c header file
  History       :
  1.Date        : 2011/06/28
    Author      : z00185248/n00168968
    Modification: Created file

******************************************************************************/

#ifndef __VOU_DRV_H__
#define __VOU_DRV_H__

#include "hi_type.h"
#include <hi3535_vo.h>
#include "vou_def.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define VOU_WBC2_FINISHED_INT    0          /* 是否使用WBC完成中断控制宏 */

#define VOU_INTCLEAR_ALL        0xffffffff
#define RGB(r,g,b)   (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff))
#define RGB_RED(c)   ((c & 0xff0000) >> 16)
#define RGB_GREEN(c) ((c & 0xff00) >> 8)
#define RGB_BLUE(c)  (c & 0xff)

#define YUV(y,u,v)   (((y & 0xff) << 16) | ((u & 0xff) << 8) | (v & 0xff))
#define YUV_Y(c)     ((c & 0xff0000) >> 16)
#define YUV_U(c)     ((c & 0xff00) >> 8)
#define YUV_V(c)     (c & 0xff)


typedef HI_S32 VO_DEV;
typedef HI_S32 VO_LAYER;
typedef HI_S32 VO_CHN;
typedef HI_S32 VO_WBC;
typedef HI_S32 GRAPHIC_LAYER;
typedef HI_S32 VO_INTF_TYPE_E;


typedef enum hiVOU_INT_MODE_E
{
    VOU_INT_MODE_FRAME      = 0x0,
    VOU_INT_MODE_FIELD      = 0x1,
    VOU_INT_MODE_BUTT
} VOU_INT_MODE_E;

/* vou mixer prio id */
typedef enum tagVOU_MIX_PRIO_E
{
    VOU_MIX_PRIO0 = 0,
    VOU_MIX_PRIO1,
    VOU_MIX_PRIO2,
    VOU_MIX_PRIO3,
    VOU_MIX_PRIO4,
    VOU_MIX_PRIO5,
    VOU_MIX_PRIO_CLR,
    VOU_MIX_BUTT 
}VOU_MIX_PRIO_E;

typedef enum hiVOU_LAYER_PIXERL_FORMAT_E
{
    /* for video surface interface */
    VOU_LAYER_PIXERL_FORMAT_SPYCbCr_420       = 0x3,
    VOU_LAYER_PIXERL_FORMAT_SPYCbCr_422       = 0x4,
    VOU_LAYER_PIXERL_FORMAT_BUTT
} VOU_LAYER_PIXERL_FORMAT_E;

typedef enum hiVOU_INTF_PIXERL_FORMAT_E
{
    /* for device interface */
    VOU_INTF_PIXERL_FORMAT_YCBCR422          = 0x0,
    VOU_INTF_PIXERL_FORMAT_YCBCR444          = 0xC,
    VOU_INTF_PIXERL_FORMAT_RGB888            = 0xC,
    VOU_INTF_PIXERL_FORMAT_BUTT
} VOU_INTF_PIXERL_FORMAT_E;

typedef enum hiVO_CSC_RANGE_E
{
    VO_CSC_DEF_VAL          = 50,
    VO_CSC_LUMA_MAX         = 100,
    VO_CSC_LUMA_MIN         = 0,

    VO_CSC_CONT_MAX         = 100,
    VO_CSC_CONT_MIN         = 0,

    VO_CSC_HUE_MAX         = 100,
    VO_CSC_HUE_MIN         = 0,

    VO_CSC_SAT_MAX         = 100,
    VO_CSC_SAT_MIN         = 0,

}VO_CSC_RANGE_E;


/* 如果后期开放ACM参数配置，则移到hi_comm_vo.h中 */
typedef struct hiVOU_ACM_BLKINFO_S
{
    HI_U32 bAcmBlkEn;
    
    HI_U8  u8UIndex;
    HI_U8  u8VIndex;
    
    HI_S8  s8UOffsetA;
    HI_S8  s8UOffsetB;
    HI_S8  s8UOffsetC;
    HI_S8  s8UOffsetD;
    
    HI_S8  s8VOffsetA;
    HI_S8  s8VOffsetB;
    HI_S8  s8VOffsetC;
    HI_S8  s8VOffsetD;
} VOU_ACM_BLKINFO_S;

/* vou zoom mode */
typedef enum 
{
    VOU_DISP_ZMEMODE_HORL = 0,  
    VOU_DISP_ZMEMODE_HORC,  
    VOU_DISP_ZMEMODE_VERL,
    VOU_DISP_ZMEMODE_VERC,
    
    VOU_DISP_ZMEMODE_HOR,
    VOU_DISP_ZMEMODE_VER,
    VOU_DISP_ZMEMODE_ALPHA,
    VOU_DISP_ZMEMODE_ALPHAV,
    VOU_DISP_ZMEMODE_VERT,
    VOU_DISP_ZMEMODE_VERB,
    
    VOU_DISP_ZMEMODE_ALL,
    VOU_DISP_ZMEMODE_NONL,
    VOU_DISP_ZMEMODE_BUTT
      
}VOU_DISP_ZMEMODE_E;

typedef struct hiVOU_ACM_S
{
    VOU_ACM_BLKINFO_S stAcmBlk[4];  // 4这个值的宏暂时被定义在vou_chn.c中
} VOU_ACM_S;

typedef enum hiVOU_DEV_TYPE_E
{
    VOU_HD_HW_DEV = 0,
    VOU_SD_HW_DEV,
    VOU_CAS_DEV,
    VOU_VIRT_DEV,
    VOU_DEV_TYPE_BUTT,
} VOU_DEV_TYPE_E;

typedef struct hiVOU_DEV_CAPABILITY_S
{
    VOU_DEV_TYPE_E  enVoDevType;             /* VO设备类型 */
    HI_BOOL         bHdVpssRciv;             /* 接收图像策略是否需要通过VPSS接收 */
    HI_BOOL         bSdTdeDisp;              /* 显示图像策略是否需要通过TDE拼接 */
    HI_BOOL         bSupportWbc;             /* 设置支持wbc功能 */
    HI_BOOL         bSptGetScreenFrm;        /* 是否支持获取屏幕接口 */
} VOU_DEV_CAPABILITY_S;

/* 将Dev、LAYER的枚举放在此处是为了不让mkp层的代码直接使用枚举变量，而必须通过调用相应的能力集接口来实现 */
/* vou device enumeration */
/* 物理显示通路枚举，需要与HAL层的显示通路枚举一一对应 */

typedef enum
{
    VOU_SW_LAYER_VHD0  = 0,
    VOU_SW_LAYER_VHD1  = 1,    
    VOU_SW_LAYER_VP    = 2,
    //VOU_SW_LAYER_VHD2  = 2,     /* 暂时软件层面上，HD2和SD5一致，和硬件特性保持一致，以后再改 */
    VOU_SW_LAYER_VSD0  = 3,
    //VOU_SW_LAYER_VSD1  = 2,
    //VOU_SW_LAYER_VSD2  = 5,
    //VOU_SW_LAYER_VSD3  = 6,
    //VOU_SW_LAYER_VSD4  = 7,
    //VOU_SW_LAYER_CAS1  = 8,
    //VOU_SW_LAYER_CAS2  = 9,
    VOU_SW_LAYER_VIRT0 = 4,
    VOU_SW_LAYER_VIRT1 = 5,
    VOU_SW_LAYER_VIRT2 = 6,
    VOU_SW_LAYER_VIRT3 = 7,
    VOU_SW_LAYER_BUTT
}VOU_SW_LAYER_E;

typedef struct tagVO_SYNC_INFO_S
{
    HI_BOOL  bSynm;     /* sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    HI_BOOL  bIop;      /* interlaced or progressive display(0:i; 1:p) */
    HI_U8    u8Intfb;   /* interlace bit width while output */

    HI_U16   u16Vact ;  /* vertical active area */
    HI_U16   u16Vbb;    /* vertical back blank porch */
    HI_U16   u16Vfb;    /* vertical front blank porch */

    HI_U16   u16Hact;   /* herizontal active area */
    HI_U16   u16Hbb;    /* herizontal back blank porch */
    HI_U16   u16Hfb;    /* herizontal front blank porch */
    HI_U16   u16Hmid;   /* bottom herizontal active area */

    HI_U16   u16Bvact;  /* bottom vertical active area */
    HI_U16   u16Bvbb;   /* bottom vertical back blank porch */
    HI_U16   u16Bvfb;   /* bottom vertical front blank porch */

    HI_U16   u16Hpw;    /* horizontal pulse width */
    HI_U16   u16Vpw;    /* vertical pulse width */

    HI_BOOL  bIdv;      /* inverse data valid of output */
    HI_BOOL  bIhs;      /* inverse horizontal synch signal */
    HI_BOOL  bIvs;      /* inverse vertical synch signal */

} VO_SYNC_INFO_S;

typedef enum hiVOU_LAYER_DDR_E
{
    VOU_LAYER_DDR0 = 0,
    VOU_LAYER_DDR1 = 1,
    VOU_LAYER_DDR_BUTT
}VOU_LAYER_DDR_E;

typedef struct hiVO_CSC_S
{
    VO_CSC_MATRIX_E enCscMatrix;
    HI_U32 u32Luma;                     /* luminance:   0 ~ 100 default: 50 */
    HI_U32 u32Contrast;                 /* contrast :   0 ~ 100 default: 50 */
    HI_U32 u32Hue;                      /* hue      :   0 ~ 100 default: 50 */
    HI_U32 u32Satuature;                /* satuature:   0 ~ 100 default: 50 */
} VO_CSC_S;

typedef struct hiVO_VGA_PARAM_S
{
    VO_CSC_S stCSC;
    HI_U32 u32Gain;                     /* current gain of VGA signals. [0, 64). default:0x30 */
} VO_VGA_PARAM_S;


typedef struct hiSIZE_S
{
    HI_U32 u32Width;
    HI_U32 u32Height;
} SIZE_S;

typedef struct hiVO_PUB_ATTR_S
{
    HI_U32                   u32BgColor;          /* Background color of a device, in RGB format. */
    VO_INTF_TYPE_E           enIntfType;          /* Type of a VO interface */
    VO_INTF_SYNC_E           enIntfSync;          /* Type of a VO interface timing */
    VO_SYNC_INFO_S           stSyncInfo;          /* Information about VO interface timings */
} VO_PUB_ATTR_S;

HI_VOID VOU_DRV_WriteReg(HI_U32 u32Addr, HI_U32 u32Value);
HI_VOID VOU_DRV_ReadReg(HI_U32 u32Addr, HI_U32 *pu32Value);
HI_VOID VOU_DRV_DacSwitch(HI_U32 u32DacId, HI_BOOL bOpen);
HI_VOID VOU_DRV_BoardInit(HI_VOID);
HI_VOID VOU_DRV_IntRegUpMode(VO_LAYER VoLayer, VOU_INT_MODE_E IntMode);
HI_VOID VOU_DRV_SetDevIntfType(VO_DEV VoDev, VO_INTF_TYPE_E enIntfType);
HI_VOID VOU_DRV_SetDevBkGrd(VO_DEV VoDev, HI_U32 u32BgColor);
HI_VOID VOU_DRV_SetDevOutSync(VO_DEV VoDev, VO_INTF_SYNC_E enVoOutMode);
HI_VOID VOU_DRV_DateSetting(VO_DEV VoDev, VO_INTF_SYNC_E enOutSync);

/* interrupt relative                                                       */
HI_VOID VOU_DRV_DevIntEnable(VO_DEV VoDev, HI_BOOL Enable);
HI_VOID VOU_DRV_IntSetMode(VO_DEV VoDev, VOU_INT_MODE_E IntMode);

/* surface relative                                                         */
HI_VOID VOU_DRV_LayerEnable(VO_LAYER VoLayer, HI_BOOL Enable);
HI_VOID VOU_DRV_SetLayerBkGrd(VO_LAYER VoLayer, HI_U32 u32BkGrd);
HI_VOID VOU_DRV_SetLayerDataFmt(VO_LAYER VoLayer, VOU_LAYER_PIXERL_FORMAT_E enDataFmt);
HI_S32 VOU_DRV_GraphicsSetCscCoef(VO_LAYER VoLayer);
HI_VOID VOU_DRV_VGACscConfig(VO_CSC_S *pstVgaCsc);
/* mixer relative                                                           */
HI_VOID VOU_DRV_DefLayerBindDev(HI_VOID);

HI_VOID VOU_DRV_Open(VO_DEV VoDev);
HI_VOID VOU_DRV_Close(VO_DEV VoDev);
HI_VOID VOU_DRV_DefaultSetting(HI_VOID);
	
HI_VOID  VOU_DRV_SetDevClk(VO_DEV VoDev);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of __VOU_DRV_H__ */

