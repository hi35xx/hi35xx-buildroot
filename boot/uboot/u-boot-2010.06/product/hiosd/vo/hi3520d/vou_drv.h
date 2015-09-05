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
    Author      : n00168968
    Modification: Created file

******************************************************************************/

#ifndef __VOU_DRV_H__
#define __VOU_DRV_H__

#include "hi_type.h"
//#include <hi35xx_vo.h>
#include <hi3520d_vo.h>
#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define RGB(r,g,b)   (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff))
#define RGB_RED(c)   ((c & 0xff0000) >> 16)
#define RGB_GREEN(c) ((c & 0xff00) >> 8)
#define RGB_BLUE(c)  (c & 0xff)

#define YUV(y,u,v)   (((y & 0xff) << 16) | ((u & 0xff) << 8) | (v & 0xff))
#define YUV_Y(c)     ((c & 0xff0000) >> 16)
#define YUV_U(c)     ((c & 0xff00) >> 8)
#define YUV_V(c)     (c & 0xff)

typedef enum tagVOU_INT_MASK_E
{
    VOU_INTMSK_NONE      = 0, 
    
    VOU_INTMSK_DSDVTTHD  = 0x1,
    VOU_INTMSK_DSDVTTHD1 = 0x2,
    VOU_INTMSK_DSDVTTHD2 = 0x4,
    VOU_INTMSK_DSDUFINT  = 0x8,
    
    VOU_INTMSK_DHDVTTHD  = 0x10,
    VOU_INTMSK_DHDVTTHD1 = 0x20,
    VOU_INTMSK_DHDVTTHD2 = 0x40,
    VOU_INTMSK_DHDUFINT  = 0x80,

    VOU_INTMSK_DSDVTTHD3 = 0x100,
    VOU_INTMSK_DSDVTTHD4 = 0x200,
    VOU_INTMSK_DSDVTTHD5 = 0x400,

    VOU_INTMSK_WBC2TEINT = 0x800,
    
    VOU_INTMSK_LNKTEINT  = 0x1000,
    VOU_INTMSK_DHD1UFINT = 0x2000,
    VOU_INTMSK_DSD1UFINT = 0x4000,
    VOU_INTMSK_DSD2UFINT = 0x8000,

    VOU_INTMSK_DSD3UFINT = 0x10000,
    VOU_INTMSK_DSD4UFINT = 0x20000,
    VOU_INTMSK_DSD5UFINT = 0x40000,

    VOU_INTMSK_VSDRRINT = 0x100000,
    VOU_INTMSK_VADRRINT = 0x200000,
    VOU_INTMSK_VHDRRINT = 0x400000,
    VOU_INTMSK_G0RRINT  = 0x800000,
    
    VOU_INTMSK_G1RRINT = 0x1000000,
    
    VOU_INTMSK_RRERRINT = 0x10000000,
    VOU_INTMSK_UTENDINT = 0x40000000,
    VOU_INTMSK_BEINT    = 0x80000000,
    
    VOU_INTREPORT_ALL = 0xffffffff
} VOU_INT_MASK_E;

typedef enum hiVOU_INT_MODE_E
{
    VOU_INT_MODE_FRAME      = 0x0,
    VOU_INT_MODE_FIELD      = 0x1,
    VOU_INT_MODE_BUTT
} VOU_INT_MODE_E;

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
    VOU_LAYER_PIXERL_FORMAT_SPYCbCr_420       = 0x3,
    VOU_LAYER_PIXERL_FORMAT_SPYCbCr_422       = 0x4,
    VOU_LAYER_PIXERL_FORMAT_BUTT
} VOU_LAYER_PIXERL_FORMAT_E;

typedef enum hiVOU_INTF_PIXERL_FORMAT_E
{
    VOU_INTF_PIXERL_FORMAT_YCBCR422          = 0x0,
    VOU_INTF_PIXERL_FORMAT_YCBCR444          = 0xC,
    VOU_INTF_PIXERL_FORMAT_RGB888            = 0xC,
    VOU_INTF_PIXERL_FORMAT_BUTT
} VOU_INTF_PIXERL_FORMAT_E;

typedef struct hiRECT_S
{
    HI_S32 s32X;
    HI_S32 s32Y;
    HI_U32 u32Width;
    HI_U32 u32Height;
}RECT_S;

typedef HI_S32 VO_DEV;
typedef HI_S32 VO_LAYER;
typedef HI_S32 VO_INTF_TYPE_E;
typedef HI_S32 VO_WBC_CHN;

typedef struct tagVO_SYNC_INFO_S
{
    HI_BOOL  bSynm;     
    HI_BOOL  bIop;      
    HI_U8    u8Intfb;   

    HI_U16   u16Vact ;  
    HI_U16   u16Vbb;    
    HI_U16   u16Vfb;    

    HI_U16   u16Hact;   
    HI_U16   u16Hbb;    
    HI_U16   u16Hfb;    

    HI_U16   u16Bvact;  
    HI_U16   u16Bvbb;   
    HI_U16   u16Bvfb;   

    HI_U16   u16Hpw;    
    HI_U16   u16Vpw;    

    HI_BOOL  bIdv;      
    HI_BOOL  bIhs;      
    HI_BOOL  bIvs;
} VO_SYNC_INFO_S;

typedef enum hiVOU_LAYER_DDR_E
{
    VOU_LAYER_DDR0 = 0,
    VOU_LAYER_DDR1 = 1,
    VOU_LAYER_DDR_BUTT
}VOU_LAYER_DDR_E;

HI_VOID VOU_DRV_WriteReg(HI_U32 u32Addr, HI_U32 u32Value);
HI_VOID VOU_DRV_ReadReg(HI_U32 u32Addr, HI_U32 *pu32Value);
HI_VOID VOU_DRV_DacSwitch(HI_U32 u32DacId, HI_BOOL bOpen);
HI_VOID VOU_DRV_BoardInit(HI_VOID);
HI_VOID VOU_DRV_BoardDeInit(HI_VOID);
HI_VOID VOU_DRV_IntRegUpMode(VO_LAYER VoLayer, VOU_INT_MODE_E IntMode);
HI_VOID VOU_DRV_SetDevIntfType(VO_DEV VoDev, VO_INTF_TYPE_E enIntfType);
HI_VOID VOU_DRV_SetDevBkGrd(VO_DEV VoDev, HI_U32 u32BgColor);
HI_VOID VOU_DRV_SetDevOutSync(VO_DEV VoDev, VO_INTF_SYNC_E enVoOutMode);
HI_BOOL VOU_DRV_GetDevEnable(VO_DEV VoDev);
HI_VOID VOU_DRV_DateSetting(VO_DEV VoDev, VO_INTF_SYNC_E enOutSync);
HI_VOID VOU_DRV_HDateSetting(VO_DEV VoDev, VO_INTF_SYNC_E enOutSync);
HI_VOID VOU_DRV_GammaConfig(VO_DEV VoDev);
HI_VOID VOU_DRV_GammaEnable(VO_DEV VoDev, HI_BOOL bEnable);


/* interrupt relative                                                       */
HI_VOID VOU_DRV_DevIntEnable(VO_DEV VoDev, HI_BOOL Enable);
HI_VOID VOU_DRV_IntClear(VOU_INT_MASK_E IntType);
HI_VOID VOU_DRV_IntDisableAll(HI_VOID);
HI_VOID VOU_DRV_IntSetMode(VO_DEV VoDev, VOU_INT_MODE_E IntMode);
HI_VOID VOU_DRV_IntSetVTTH(VO_DEV VoDev, HI_U32 Threshold);


/* surface relative                                                         */
HI_VOID VOU_DRV_SetLayerDdr(VO_LAYER VoLayer, VOU_LAYER_DDR_E enDdrSelId);
HI_VOID VOU_DRV_LayerEnable(VO_LAYER VoLayer, HI_BOOL Enable);
HI_VOID VOU_DRV_SetLayerBkGrd(VO_LAYER VoLayer, HI_U32 u32BkGrd);
HI_VOID VOU_DRV_SetLayerDataFmt(VO_LAYER VoLayer, VOU_LAYER_PIXERL_FORMAT_E enDataFmt);
HI_S32 VOU_DRV_GraphicsSetCscCoef(VO_LAYER VoLayer);
HI_VOID VOU_DRV_LayerCSCEnable(VO_LAYER VoLayer, HI_BOOL bCscEn);
HI_VOID VOU_DRV_LayerCSCConfig(VO_LAYER VoLayer, HI_S32 u32Luma, HI_U32 u32Contrast, HI_U32 u32Hue, HI_U32 u32Satuature);
HI_VOID VOU_DRV_VGACscConfig(VO_CSC_MATRIX_E enCscMatrix,HI_S32 u32Luma, HI_U32 u32Contrast, HI_U32 u32Hue, HI_U32 u32Satuature);
HI_VOID VOU_DRV_VGACscEn(HI_BOOL bVgaCscEn);

HI_VOID VOU_DRV_DefLayerBindDev(HI_VOID);

HI_VOID VOU_DRV_SetDevClk(VO_DEV VoDev);
HI_VOID VOU_DRV_Open(VO_DEV VoDev);
HI_VOID VOU_DRV_Close(VO_DEV VoDev);
HI_VOID VOU_DRV_DefaultSetting(HI_VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of __VOU_DRV_H__ */

