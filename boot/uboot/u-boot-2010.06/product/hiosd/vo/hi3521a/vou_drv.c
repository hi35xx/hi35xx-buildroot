/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_drv.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/06/28
  Description   :
  History       :
  1.Date        : 2011/06/28
    Author      : n00168968    
    Modification: Created file

******************************************************************************/

#include <common.h>
#include <command.h>
#include <malloc.h>
//#include <devices.h>
#include <version.h>

#include <asm/arch/platform.h>
#include <asm/sizes.h>

#include "hi_type.h"
#include "vou_reg.h"
#include "vou_def.h"
#include "vou_hal.h"
#include "vou_drv.h"
#include "vou_coef_org.h"
//#include "vou_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
#define VOU_DEV_MAX_NUM             2
#define VOU_LAYER_MAX_NUM           7

#define VOU_SD_VTTH_WATERLINE       100
#define VOU_HD_VTTH_WATERLINE       240

#define VOU_BACKGROUD_BLACK         0x8080
#define VOU_BACKGROUD_GREEN         0x804D3A42
#define VOU_BACKGROUD_DEFAULT       VOU_BACKGROUD_GREEN

/* vou interrupt mask type */
typedef enum tagVOU_INT_MASK_E
{
    VOU_INTMSK_NONE      = 0, 
    VOU_INTMSK_DHDVTTHD  = 0x1,
    VOU_INTMSK_DHDUFINT  = 0x8,
    VOU_INTMSK_DHDVTTHD1 = 0x10,
    VOU_INTMSK_DHD1UFINT = 0x80,
    VOU_INTMSK_WBC2TEINT = 0x200,
    
    VOU_INTMSK_DSDVTTHD  = 0x10000,
    VOU_INTMSK_DSDUFINT  = 0x20000,
    VOU_INTMSK_G0WBCVTE = 0x400,
    VOU_INTMSK_G4WBCVTE = 0x800,

    VOU_INTMSK_VSDRRINT = 0x100000,
    VOU_INTMSK_VHDRRINT = 0x400000,
    VOU_INTMSK_G0RRINT  = 0x800000,
    
    VOU_INTMSK_RRERRINT = 0x10000000,    
    VOU_INTMSK_VDAC_CVBS_ICINT = 0x8000,
    VOU_INTMSK_VDAC_VGA_ICINT = 0x7000,        
    VOU_INTMSK_UTENDINT = 0x40000000,
    VOU_INTMSK_BEINT    = 0x80000000,
    
    VOU_INTREPORT_ALL = 0xffffffff
} VOU_INT_MASK_E;


typedef struct hiHAL_DEV_CONFIG_S
{
    HI_BOOL         bEnable;
    HI_U32          u32BkGrd;
    VO_INTF_TYPE_E  enIntfType;
    VO_INTF_SYNC_E  enOutSync;
    HAL_DISP_PIXEL_FORMAT_E enPixelFmt;
} HAL_DEV_CONFIG_S;

typedef struct hiHAL_LAYER_CONFIG_S
{
    HI_U32             u32BkGrd;
} HAL_LAYER_CONFIG_S;

typedef struct hiHAL_CSC_VALUE_S
{
    HI_U32 u32Luma;
    HI_U32 u32Cont;
    HI_U32 u32Hue;
    HI_U32 u32Satu;

} HAL_CSC_VALUE_S;

typedef struct hiHAL_COEF_ADDR_S
{
    HI_U32 u32BasePhys;
    HI_VOID *pBaseVirt;    
    HI_U32 u32Hor;
    HI_U32 u32Ver422;
    HI_U32 u32Ver420;
    HI_U32 u32Lut;
    HI_U32 u32Gam;
    HI_U32 u32Acc;
} HAL_COEF_ADDR_S;

/****************************************************************************
 * EXTERN VARIABLES                                                         *
 ****************************************************************************/


/****************************************************************************
 * GLOBAL VARIABLES                                                         *
 ****************************************************************************/
static const HI_S16 g_stNotchCoef_960h_lowpass[23] = { 2, -3,   4, -4,   5, -5,   6,  -7,  7, -8,  8, 246,  8, -8,  7,  -7,   6, -5,   5, -4,   4, -3,  2};  //lowpass 7.75

static HAL_LAYER_CONFIG_S g_stHalLayerCfg[VOU_LAYER_MAX_NUM] =
{
    {
        .u32BkGrd      = VOU_BACKGROUD_DEFAULT,
    },
    {
        .u32BkGrd      = VOU_BACKGROUD_DEFAULT,
    },
    {
        .u32BkGrd      = VOU_BACKGROUD_DEFAULT,
    },
    {
        .u32BkGrd      = VOU_BACKGROUD_DEFAULT,
    },
    {
        .u32BkGrd      = VOU_BACKGROUD_DEFAULT,
    },
    {
        .u32BkGrd      = VOU_BACKGROUD_DEFAULT,
    },
    {
        .u32BkGrd      = VOU_BACKGROUD_DEFAULT,
    }
};

HAL_DISP_SYNCINFO_S g_stSyncTiming[VO_OUTPUT_BUTT] = 
{
 /* |--INTFACE---||-----TOP-----||----HORIZON--------||----BOTTOM-----||-PULSE-||-INVERSE-| */
  /*Synm,Iop,Itf, Vact, Vbb,Vfb, Hact, Hbb,Hfb,Hmid,Bvact,Bvbb,Bvfb,Hpw,Vpw,Idv,Ihs,Ivs */
    {0,   0,   0,   288,  22,  2,   720, 132,  12,   1,  288,  23,  2, 126,  3,  0,  0,  0},     /* 576I(PAL)  */  // HDMI:hmid 300; YPbPr:hmid 0;
    {0,   0,   0,   240,  18,  4,   720, 119,  19,   1,  240,  19,  4, 124,  3,  0,  0,  0},     /* 480I(NTSC) */  // HDMI:hmid 310; YPbPr:hmid 0;

    {0,   0,   0,   288,  22,  2,   960, 176,  16,   1,  288,  23,  2, 168,  3,  0,  0,  0},    /* 960H(PAL) */
    {0,   0,   0,   240,  18,  4,   960, 163,  21,   1,  240,  19,  4, 168,  3,  0,  0,  0},    /* 960H(NTSC) */
        
    {0,   1,   1,  1080,  41,  4,  1920, 192, 638,   1,    1,   1,  1,  44,  5,  0,  0,  0},    /* 1080P@24Hz */ 
    {0,   1,   1,  1080,  41,  4,  1920, 192, 528,   1,    1,   1,  1,  44,  5,  0,  0,  0},    /* 1080P@25Hz */
    {0,   1,   1,  1080,  41,  4,  1920, 192,  88,   1,    1,   1,  1,  44,  5,  0,  0,  0},    /* 1080P@30Hz */
        
    {0,   1,   1,   720,  25,  5,  1280, 260, 440,   1,    1,   1,  1,  40,  5,  0,  0,  0},    /* 720P@50Hz */ 
    {0,   1,   1,   720,  25,  5,  1280, 260, 110,   1,    1,   1,  1,  40,  5,  0,  0,  0},    /* 720P@60Hz */
    {0,   0,   1,   540,  20,  2,  1920, 192, 528,1128,  540,  21,  2,  44,  5,  0,  0,  0},    /* 1080I@50Hz */
    //{0,   0,   1,   540,  20,  3,  1920, 192,528,1128    540,  20,  2,  44,  5,  0,  0,  0}, /* 1080I@50Hz */
    {0,   0,   1,   540,  20,  2,  1920, 192,  88, 908,  540,  21,  2,  44,  5,  0,  0,  0},    /* 1080I@60Hz */              
    {0,   1,   1,  1080,  41,  4,  1920, 192, 528,   1,    1,   1,  1,  44,  5,  0,  0,  0},    /* 1080P@50Hz */
    {0,   1,   1,  1080,  41,  4,  1920, 192,  88,   1,    1,   1,  1,  44,  5,  0,  0,  0},    /* 1080P@60Hz */
        
    {1,   1,   1,   576,   44, 5,   720, 132, 12,   1,    1,   1,  1,  64,  5,  0,  0,  0},     /* 576P@50Hz */
    {1,   1,   1,   480,   36, 9,   720, 122, 16,   1,    1,   1,  1,  62,  6,  0,  0,  0},     /* 480P@60Hz */ 

    {1,   1,   2,   480,  35, 10,   640, 144,  16,   1,    1,   1,  1,  96,  2,  0,  1,  1},    /* 640*480@60Hz CVT */
    {1,   1,   2,   600,  27,  1,   800, 216,  42,   1,    1,   1,  1, 128,  4,  0,  0,  0},    /* 800*600@60Hz VGA@60Hz*/
    {1,   1,   2,   768,  35,  3,  1024, 296,  26,   1,    1,   1,  1, 136,  6,  0,  1,  1},    /* 1024x768@60Hz */
    {1,   1,   2,  1024,  41,  1,  1280, 360,  50,   1,    1,   1,  1, 112,  3,  0,  0,  0},    /* 1280x1024@60Hz */
    {1,   1,   2,   768,  27,  3,  1366, 356,  70,   1,    1,   1,  1, 143,  3,  0,  0,  0},    /* 1366x768@60Hz */
    {1,   1,   2,   900,  31,  3,  1440, 384,  80,   1,    1,   1,  1, 152,  6,  0,  1,  0},    /* 1440x900@60Hz */
    {1,   1,   2,   800,  28,  3,  1280, 328,  72,   1,    1,   1,  1, 128,  6,  0,  1,  0},    /* 1280*800@60Hz VGA@60Hz*/
    {1,   1,   2,  1050,  36,  3,  1680, 456, 104,   1,    1,   1,  1, 176,  6,  0,  1,  0},    /* 1680*1050@60Hz */
    {0,   1,   1,  2160,  72,  8,  1920, 192,  88,   1,    1,   1,  1,  44,  5,  0,  0,  0},    /* 1920*2160@30Hz*/    
    {1,   1,   2,  1200,  49,  1,  1600, 496,  66,   1,    1,   1,  1, 192,  3,  0,  0,  0},    /* 1600*1200@60Hz */    
    //{1,   1,   2,  1200,  42,  3,  1920, 536, 136,   1,    1,   1,  1, 200,  6,  0,  1,  0},    /* 1920*1200@60Hz */
    {1,   1,   2,  1200,  32,  3,  1920, 112,  48,   1,    1,   1,  1,  32,  6,  0,  1,  0},    /* 1920*1200@60Hz CVT (Reduced Blanking) */

    {0,   1,   1,  1440,  37,  3,  2560, 80,   48,   1,    1,   1,  1,  32,  6,  0,  0,  0},    /* 2560*1440@30Hz*/ 
    {0,   1,   2,  1600,  55,  3,  2560, 94,   24,   1,    1,   1,  1,  35,  6,  0,  0,  0},   /* 2560*1600@60Hz*/
    {0,   1,   1,  2160,  82,  8,  3840, 384,  176,  1,    1,   1,  1,  88,  10, 0,  0,  0},   /* 3840*2160@30Hz*/
    {0,   1,   1,  2160,  82,  8,  3840, 384,  176,  1,    1,   1,  1,  88,  10, 0,  0,  0},   /* 3840*2160@60Hz*/

    {}/* User */
};

static HAL_DEV_CONFIG_S g_stHalDevCfg[VOU_DEV_MAX_NUM] =
{
	#if 0
    {
        .bEnable        = HI_FALSE,
        .u32BkGrd       = VOU_BACKGROUD_DEFAULT,
        .enIntfType     = VO_INTF_BT1120 | VO_INTF_HDMI | VO_INTF_VGA,
        .enOutSync      = VO_OUTPUT_1080P60,           /* for debug: VO_OUTPUT_800x600_60 */
        .enPixelFmt     = HAL_INPUTFMT_YCbCr_SEMIPLANAR_422,     /* VOU_PIXERL_FORMAT_RGB888, VOU_PIXERL_FORMAT_YCBCR422*/
    },
    #endif
    {
        .bEnable        = HI_FALSE,
        .u32BkGrd       = VOU_BACKGROUD_DEFAULT,
        .enIntfType     = VO_INTF_VGA,
        .enOutSync      = VO_OUTPUT_1080P60,
        .enPixelFmt     = HAL_INPUTFMT_YCbCr_SEMIPLANAR_422,
    },
    {
        .bEnable        = HI_FALSE,
        .u32BkGrd       = VOU_BACKGROUD_DEFAULT,
        .enIntfType     = VO_INTF_CVBS | VO_INTF_BT656,
        .enOutSync      = VO_OUTPUT_PAL,
        .enPixelFmt     = HAL_INPUTFMT_YCbCr_SEMIPLANAR_422,
    }
};

/****************************************************************************
 * EXTERN FUNCTION DECLARATION                                              *
 ****************************************************************************/

/****************************************************************************
 * INTERNAL FUNCTION DECLARATION                                            *
 ****************************************************************************/

/****************************************************************************
 * VOU FUNCTION                                                             *
 *            ---- Assistant Function Area                                  *
 ****************************************************************************/

HI_VOID VOU_DRV_WriteReg(HI_U32 u32Addr, HI_U32 u32Value)
{
    HAL_WriteReg((HI_U32*)u32Addr, u32Value);
}

HI_VOID VOU_DRV_ReadReg(HI_U32 u32Addr, HI_U32 *pu32Value)
{
    *pu32Value = HAL_ReadReg((HI_U32*)u32Addr);
}

HI_VOID VOU_DRV_DacSwitch(HI_U32 u32DacId, HI_BOOL bOpen)
{
    /* todo */
    return;
}

HI_VOID VOU_DRV_BoardInit(HI_VOID)
{        
    HAL_VOU_Init();
    VOU_DRV_DefaultSetting();

    return;

}

static inline HI_BOOL VouDrvIsMultiIntf(VO_INTF_TYPE_E enMuxIntf)
{
    HI_U32 u32Num, u32Tmp;

    u32Num = 0;
    u32Tmp = (HI_U32)enMuxIntf;
    while (0 != u32Tmp)
    {
        if (u32Tmp & 0x1)
        {
            u32Num++;
        }
        u32Tmp = u32Tmp >> 1;
    }

    return ((u32Num <= 1) ? HI_FALSE : HI_TRUE);
}

/* 中断中视频层寄存器更新方式 */
HI_VOID VOU_DRV_IntRegUpMode(VO_LAYER VoLayer, VOU_INT_MODE_E IntMode)
{
    HAL_VIDEO_SetLayerUpMode(VoLayer, IntMode);
    //HAL_LAYER_SetRegUp (VoLayer);

    return;
}

HI_VOID VOU_DRV_SetDevIntfType(HI_S32 VoDev, VO_INTF_TYPE_E enIntfType)
{
    g_stHalDevCfg[VoDev].enIntfType = enIntfType;

    return;
}

HI_VOID VOU_DRV_SetDevBkGrd(HI_S32 VoDev, HI_U32 u32BgColor)
{
    g_stHalDevCfg[VoDev].u32BkGrd = u32BgColor;

    return;
}

HI_VOID VOU_DRV_SetDevOutSync(HI_S32 VoDev, VO_INTF_SYNC_E enVoOutMode)
{
    g_stHalDevCfg[VoDev].enOutSync = enVoOutMode;

    return;
}

HI_VOID VOU_DRV_DateSetting(VO_DEV VoDev, VO_INTF_SYNC_E enOutSync)
{
    HI_U32 u32Data;
    
    switch (enOutSync)
    {
        case VO_OUTPUT_PAL :
            //u32Data = 0x528414fc;
            u32Data = 0x528414dc;
            /* disable 960H */
            HAL_DISP_SetDateCoeffByIdx(60, 0);
            break;
        case VO_OUTPUT_NTSC :
            //u32Data = 0x108414fc;
            u32Data = 0x108414dc;
            /* disable 960H */
            HAL_DISP_SetDateCoeffByIdx(60, 0);
            break;
        case VO_OUTPUT_960H_PAL :
        case VO_OUTPUT_960H_NTSC :
        {
            if(VO_OUTPUT_960H_PAL == enOutSync)
            {
                u32Data = 0x528414fc;
            }
            else if(VO_OUTPUT_960H_NTSC == enOutSync)
            {
                u32Data = 0x108414fc;
            }
            /* enable 960H date */
            HAL_DISP_SetDateCoeffByIdx(60, 1);
            /* enable notch coef ?? */
            HAL_DISP_SetDateCoeffByIdx(73, 1);
            /* default notch coef */            
            VOU_HAL_DISP_SetDateNotchCoeff(g_stNotchCoef_960h_lowpass);
            break;
        }
        default :
            return;
    }
    HAL_DISP_SetDateCoeff(VoDev, u32Data);
    HAL_DISP_DATE_OutCtrl(VoDev, 0x111111);//针对 DATE V300
    return;
}

//HI_VOID VOU_DRV_DateSetting(VO_DEV VoDev, VO_INTF_SYNC_E enOutSync)
//{
//    HI_U32 u32Data;
//    HAL_DISP_SYNCINV_S stDispSync = {0};
//    switch (enOutSync)
//    {
//        case VO_OUTPUT_PAL :
//            //u32Data = 0x528414fc;
//            u32Data = 0x528414dc;
//            break;
//        case VO_OUTPUT_NTSC :
//            //u32Data = 0x108414fc;
//            u32Data = 0x108414dc;
//            break;
//        default :
//            return;
//    }
//    stDispSync.u32hs_inv = HI_TRUE;
//    HAL_DISP_SetDateCoeff(VoDev, u32Data);
//    HAL_DISP_DATE_OutCtrl(VoDev, 0x111111);//for DATE V300
//    HAL_DISP_SetIntfSyncInv(HAL_DISP_INTF_CVBS,&stDispSync);
//    return;
//}

/* interrupt relative                                                       */
HI_VOID VOU_DRV_DevIntEnable(VO_DEV VoDev, HI_BOOL Enable)
{
    VOU_INT_MASK_E IntType;

    switch ( VoDev )
    {
		#if 0
        case VOU_DEV_DHD0 :
            #if VOU_WBC2_FINISHED_INT
            IntType = VOU_INTMSK_DHDVTTHD | VOU_INTMSK_WBC2TEINT;
            #else
            IntType = VOU_INTMSK_DHDVTTHD;
            #endif
            break;
		#endif
        case VOU_DEV_DHD1 :
            #if VOU_WBC2_FINISHED_INT
            IntType = VOU_INTMSK_DHDVTTHD1 | VOU_INTMSK_WBC2TEINT;
            #else
            IntType = VOU_INTMSK_DHDVTTHD1| VOU_INTMSK_VDAC_VGA_ICINT;
            #endif
            break;
        case VOU_DEV_DSD0 :
            IntType = VOU_INTMSK_DSDVTTHD| VOU_INTMSK_VDAC_CVBS_ICINT;
            break;

        default:
            return;
    }

    if (HI_TRUE == Enable)
    {
        HAL_DISP_SetIntMask(IntType);
    }
    else
    {
        HAL_DISP_ClrIntMask(IntType);
    }

    return;
}

HI_VOID VOU_DRV_IntSetMode(HI_S32 VoDev, VOU_INT_MODE_E IntMode)
{
    HAL_DISP_SetVtThdMode(VoDev, IntMode);

    return;
}

HI_VOID VOU_DRV_LayerEnable(VO_LAYER VoLayer, HI_BOOL Enable)
{
    HAL_LAYER_EnableLayer(VoLayer, Enable);
    //HAL_LAYER_SetRegUp(VoLayer);

    return;
}

HI_VOID VOU_DRV_SetLayerBkGrd(VO_LAYER VoLayer, HI_U32 u32BkGrd)
{
    g_stHalLayerCfg[VoLayer].u32BkGrd = u32BkGrd;
    return;
}

HI_VOID VOU_DRV_SetLayerDataFmt(VO_LAYER VoLayer, VOU_LAYER_PIXERL_FORMAT_E enDataFmt)
{
    HAL_DISP_LAYER_E EnVoLayer = VoLayer;
    HAL_LAYER_SetLayerDataFmt(EnVoLayer, enDataFmt);
    //HAL_LAYER_SetRegUp(VoLayer);
    return;
}

HI_S32 VOU_DRV_CalcCscMatrix(HI_U32 u32Luma, HI_U32 u32Contrast,
    HI_U32 u32Hue, HI_U32 u32Satuature, HAL_CSC_MODE_E enCscMode, CscCoef_S *pstCstCoef)
{
    HI_S32 s32Luma     = 0;
    HI_S32 s32Contrast = 0;
    HI_S32 s32Hue      = 0;
    HI_S32 s32Satu     = 0;
    const CscCoef_S *pstCscTmp = NULL;
    
    s32Luma     = (HI_S32)u32Luma * 64 / 100 - 32;
    s32Contrast = ((HI_S32)u32Contrast - 50) * 2 + 100;
    s32Hue      = (HI_S32)u32Hue * 60 / 100;
    s32Satu     = ((HI_S32)u32Satuature - 50) * 2 + 100;    

    /* 选择色彩空间转换的常系数矩阵 */
    switch (enCscMode)
    {
        case HAL_CSC_MODE_BT601_TO_BT601:
        case HAL_CSC_MODE_BT709_TO_BT709:
        case HAL_CSC_MODE_RGB_TO_RGB:
            pstCscTmp = &g_stCSC_Init;
            break;
        case HAL_CSC_MODE_BT709_TO_BT601:
            pstCscTmp = &g_stCSC_YUV2YUV_709_601;
            break;
        case HAL_CSC_MODE_BT601_TO_BT709:
            pstCscTmp = &g_stCSC_YUV2YUV_601_709;
            break;
        case HAL_CSC_MODE_BT601_TO_RGB_PC:
            pstCscTmp = &g_stCSC_YUV6012RGB_pc;
            break;
        case HAL_CSC_MODE_BT709_TO_RGB_PC:
            pstCscTmp = &g_stCSC_YUV7092RGB_pc;
            break;
        case HAL_CSC_MODE_RGB_TO_BT601_PC:
            pstCscTmp = &g_stCSC_RGB2YUV601_pc;
            break;
        case HAL_CSC_MODE_RGB_TO_BT709_PC:
            pstCscTmp = &g_stCSC_RGB2YUV709_pc;
            break;
        default:            
            return HI_FAILURE;
    }

    pstCstCoef->csc_in_dc0 = pstCscTmp->csc_in_dc0;
    pstCstCoef->csc_in_dc1 = pstCscTmp->csc_in_dc1;
    pstCstCoef->csc_in_dc2 = pstCscTmp->csc_in_dc2;
    pstCstCoef->csc_out_dc0 = pstCscTmp->csc_out_dc0;
    pstCstCoef->csc_out_dc1 = pstCscTmp->csc_out_dc1;
    pstCstCoef->csc_out_dc2 = pstCscTmp->csc_out_dc2;

    /* C_ratio的调节范围一般是0～1.99, C_ratio=s32Contrast/100
    *  S的调节范围一般为0~1.99,S=s32Satu/100
    *  色调调节参数的范围一般为-30°~30°,通过查表法求得COS和SIN值并/1000
    */
    if ((HAL_CSC_MODE_BT601_TO_RGB_PC == enCscMode) || (HAL_CSC_MODE_BT709_TO_RGB_PC == enCscMode)
        || (HAL_CSC_MODE_BT601_TO_RGB_TV == enCscMode) || (HAL_CSC_MODE_BT709_TO_RGB_TV == enCscMode))
    {
        /* 此公式仅用于YUV->RGB转换，RGB->YUV转换不可用此公式 */
        pstCstCoef->csc_coef00 = (s32Contrast * pstCscTmp->csc_coef00) / 100;
        pstCstCoef->csc_coef01 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef01*COS_TABLE[s32Hue] - pstCscTmp->csc_coef02*SIN_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef02 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef01*SIN_TABLE[s32Hue] + pstCscTmp->csc_coef02*COS_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef10 = (s32Contrast * pstCscTmp->csc_coef10) / 100;
        pstCstCoef->csc_coef11 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef11*COS_TABLE[s32Hue] - pstCscTmp->csc_coef12*SIN_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef12 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef11*SIN_TABLE[s32Hue] + pstCscTmp->csc_coef12*COS_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef20 = (s32Contrast * pstCscTmp->csc_coef20) / 100;
        pstCstCoef->csc_coef21 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef21*COS_TABLE[s32Hue] - pstCscTmp->csc_coef22*SIN_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef22 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef21*SIN_TABLE[s32Hue] + pstCscTmp->csc_coef22*COS_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_in_dc0 += (0 != s32Contrast) ? (s32Luma * 100 / s32Contrast) : s32Luma * 100;
    }
    else
    {    
        /* 此公式仅用于RGB->YUV转换，YUV->RGB转换不可用此公式，
        *  YUV->YUV仅调节图像效果可用此公式，因为常量矩阵为单位矩阵 */
        pstCstCoef->csc_coef00 = (s32Contrast * pstCscTmp->csc_coef00) / 100;
        pstCstCoef->csc_coef01 = (s32Contrast * pstCscTmp->csc_coef01) / 100;
        pstCstCoef->csc_coef02 = (s32Contrast * pstCscTmp->csc_coef02) / 100;
        pstCstCoef->csc_coef10 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef10*COS_TABLE[s32Hue] + pstCscTmp->csc_coef20*SIN_TABLE[s32Hue]) / 1000)) / 10000;
        pstCstCoef->csc_coef11 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef11*COS_TABLE[s32Hue] + pstCscTmp->csc_coef21*SIN_TABLE[s32Hue]) / 1000)) / 10000;
        pstCstCoef->csc_coef12 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef12*COS_TABLE[s32Hue] + pstCscTmp->csc_coef22*SIN_TABLE[s32Hue]) / 1000)) / 10000;
        pstCstCoef->csc_coef20 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef20*COS_TABLE[s32Hue] - pstCscTmp->csc_coef10*SIN_TABLE[s32Hue]) / 1000)) / 10000;
        pstCstCoef->csc_coef21 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef21*COS_TABLE[s32Hue] - pstCscTmp->csc_coef11*SIN_TABLE[s32Hue]) / 1000)) / 10000;
        pstCstCoef->csc_coef22 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef22*COS_TABLE[s32Hue] - pstCscTmp->csc_coef12*SIN_TABLE[s32Hue]) / 1000)) / 10000;
        pstCstCoef->csc_out_dc0 += s32Luma;
    }    
    return HI_SUCCESS;
}

HI_S32 VOU_DRV_GraphicsSetCscCoef(VO_LAYER VoLayer)
{
    CscCoef_S stCscCoef;
    HAL_CSC_MODE_E enCscMode;
    
    if((VoLayer < LAYER_GFX_START) || (VoLayer > LAYER_GFX_END))
    {
        return HI_FAILURE;
    }

    if (VoLayer == HAL_DISP_LAYER_GFX4)
    {
        enCscMode = HAL_CSC_MODE_RGB_TO_BT709_PC;
    }
    else if (VoLayer == HAL_DISP_LAYER_GFX1)
    {
        enCscMode = HAL_CSC_MODE_RGB_TO_BT601_PC;
    }
    else
    {
        return HI_FAILURE;
    }

    VOU_DRV_CalcCscMatrix(50, 50, 50, 50, enCscMode, &stCscCoef);
    HAL_LAYER_SetCscCoef(VoLayer, &stCscCoef);

    return HI_SUCCESS;
}

HI_VOID VOU_DRV_VGACscConfig(VO_CSC_S *pstVgaCSC)
{
    HAL_CSC_MODE_E enCscMode;
    CscCoef_S stCscCoef;

    switch (pstVgaCSC->enCscMatrix)
    {
        case VO_CSC_MATRIX_BT601_TO_RGB_PC:
            enCscMode = HAL_CSC_MODE_BT601_TO_RGB_PC;
            break;    
        case VO_CSC_MATRIX_BT709_TO_RGB_PC :
            enCscMode = HAL_CSC_MODE_BT709_TO_RGB_PC;
            break;
        default :
            enCscMode = HAL_CSC_MODE_BT601_TO_RGB_PC;
            break;
    }

    VOU_DRV_CalcCscMatrix(pstVgaCSC->u32Luma, pstVgaCSC->u32Contrast, pstVgaCSC->u32Hue, pstVgaCSC->u32Satuature,
        enCscMode, &stCscCoef);

    HAL_DISP_SetIntfCscCoef(VO_INTF_VGA,&stCscCoef);    
    return;
}

HI_VOID VOU_DRV_IntfCscConfig(VO_INTF_TYPE_E enIntfType, VO_CSC_S *pstIntfCSC)
{
    HAL_CSC_MODE_E enCscMode;
    CscCoef_S stCscCoef;

    if ((VO_INTF_VGA != enIntfType) && (VO_INTF_HDMI != enIntfType))
    {
        printf("Interface Type %d err!\n", enIntfType);
        return;
    }

    if (VO_INTF_VGA == enIntfType)
    {
        switch (pstIntfCSC->enCscMatrix)
        {
            case VO_CSC_MATRIX_BT601_TO_RGB_PC:
                enCscMode = HAL_CSC_MODE_BT601_TO_RGB_PC;
                break;    
            case VO_CSC_MATRIX_BT709_TO_RGB_PC :
                enCscMode = HAL_CSC_MODE_BT709_TO_RGB_PC;
                break;
            default :
                enCscMode = HAL_CSC_MODE_BT601_TO_RGB_PC;
                break;
        }
    }
    else
    {
        switch (pstIntfCSC->enCscMatrix)
        {
            case VO_CSC_MATRIX_IDENTITY :
                enCscMode = HAL_CSC_MODE_BT601_TO_BT601;    
                break;
            #if 0
            case VO_CSC_MATRIX_BT601_TO_RGB_PC:
                enCscMode = HAL_CSC_MODE_BT601_TO_RGB_PC;
                break;    
            case VO_CSC_MATRIX_BT709_TO_RGB_PC :
                enCscMode = HAL_CSC_MODE_BT709_TO_RGB_PC;
                break;
			#endif
            case VO_CSC_MATRIX_BT601_TO_BT709 :
                enCscMode = HAL_CSC_MODE_BT601_TO_BT709;
                break;    
            case VO_CSC_MATRIX_BT709_TO_BT601 :
                enCscMode = HAL_CSC_MODE_BT709_TO_BT601;
                break;
			#if 0
            case VO_CSC_MATRIX_RGB_TO_BT601_PC :
                enCscMode = HAL_CSC_MODE_RGB_TO_BT601_PC;
                break;    
            case VO_CSC_MATRIX_RGB_TO_BT709_PC :
                enCscMode = HAL_CSC_MODE_RGB_TO_BT709_PC;    
                break;
			#endif
            default :
                enCscMode = HAL_CSC_MODE_BT601_TO_BT601;
                break;
        }
    }

    VOU_DRV_CalcCscMatrix(pstIntfCSC->u32Luma, pstIntfCSC->u32Contrast, pstIntfCSC->u32Hue, pstIntfCSC->u32Satuature,
        enCscMode, &stCscCoef);
    if (VO_INTF_VGA & enIntfType)
    {
        HAL_DISP_SetIntfCscCoef(VO_INTF_VGA,&stCscCoef); 
    }
    if (VO_INTF_HDMI & enIntfType)
    {
        HAL_DISP_SetIntfCscCoef(VO_INTF_HDMI,&stCscCoef); 
    }
        
    return;
}


HI_VOID VOU_DRV_DefLayerBindDev(HI_VOID)
{
    /* MUX3可混合VHD2/G4*/
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_VHD1, VOU_MIX_PRIO0, VOU_DEV_DHD1);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_VP, VOU_MIX_PRIO1, VOU_DEV_DHD1);

    /* MUX4可混合VSD0/G1/HC0 */
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_VSD0, VOU_MIX_PRIO0, VOU_DEV_DSD0);

    HAL_CBM_SetCbmMixerPrio(HAL_DISP_LAYER_GFX4, VOU_MIX_PRIO2, HAL_CBMMIX2);
    HAL_CBM_SetCbmMixerPrio(HAL_DISP_LAYER_GFX1, VOU_MIX_PRIO1, HAL_CBMMIX3);
    return;
}

HI_VOID VOU_DRV_SetDevClk(VO_DEV VoDev)
{
    HI_S32 s32Pll;
    /* set vpll */
    if (VoDev < 1)
    {
        HI_U32 u32Frac, u32Postdiv1, u32Postdiv2, u32Fbdiv, u32Refdiv;

        s32Pll = VoDev;
        switch (g_stHalDevCfg[VoDev].enOutSync)
        {
            case VO_OUTPUT_PAL :
            case VO_OUTPUT_NTSC :
            {
                //27MHz
                u32Fbdiv = 81;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 6;
                u32Postdiv2 = 3;
                break;
            }
            case VO_OUTPUT_1080P24 :
            case VO_OUTPUT_1080P25 :
            case VO_OUTPUT_1080P30 :
            case VO_OUTPUT_720P50 : 
            case VO_OUTPUT_720P60 :   
            case VO_OUTPUT_1080I50 :
            case VO_OUTPUT_1080I60 :
            {
                //74.25MHz
                u32Fbdiv = 99;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 4;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_1080P50 :
            case VO_OUTPUT_1080P60 :
            {
                //148.5MHz
                u32Fbdiv = 99;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 4;
                u32Postdiv2 = 1;
                break;
            }
            case VO_OUTPUT_576P50 :
            case VO_OUTPUT_480P60 :
            {
                //27MHz
                u32Fbdiv = 81;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 6;
                u32Postdiv2 = 3;
                break;
            }
            case VO_OUTPUT_800x600_60:
            {
                //40MHz
                u32Fbdiv = 80;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 6;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_1024x768_60:
            {
                //65MHz
                u32Fbdiv = 65;
                u32Frac = 0;
                u32Refdiv = 1;
                u32Postdiv1 = 6;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_1280x1024_60:
            {
                //108MHz
                u32Fbdiv = 72;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 4;
                u32Postdiv2 = 1;
                break;
            }
            case VO_OUTPUT_1366x768_60:
            {
                //85.5MHz
                u32Fbdiv = 171;
                u32Frac = 0;
                u32Refdiv = 4;
                u32Postdiv1 = 6;
                u32Postdiv2 = 1;
                break;
            }
            case VO_OUTPUT_1440x900_60:
            {
                //106.5MHz
                u32Fbdiv = 213;
                u32Frac = 0;
                u32Refdiv = 4;
                u32Postdiv1 = 6;
                u32Postdiv2 = 1;
                break;
            }
            case VO_OUTPUT_1280x800_60:
            {
                //83.5MHz
                u32Fbdiv = 167;
                u32Frac = 0;
                u32Refdiv = 4;
                u32Postdiv1 = 6;
                u32Postdiv2 = 1;
                break;
            }
            case VO_OUTPUT_1600x1200_60:
            {
                //162MHz
                u32Fbdiv = 81;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 3;
                u32Postdiv2 = 1;
                break;
            }
            case VO_OUTPUT_1680x1050_60:
            {
                //146.25MHz
                u32Fbdiv = 585;
                u32Frac = 0;
                u32Refdiv = 8;
                u32Postdiv1 = 3;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_1920x1200_60:
            {
                #if 0
                //193.25MHz
                u32Fbdiv = 773;
                u32Frac = 0;
                u32Refdiv = 16;
                u32Postdiv1 = 3;
                u32Postdiv2 = 1;
                #else
                u32Fbdiv = 154;
                u32Frac = 0;
                u32Refdiv = 4;
                u32Postdiv1 = 3;
                u32Postdiv2 = 1;
                #endif
                break;
            }            
            case VO_OUTPUT_640x480_60:
            {
                //25.175MHz
                u32Fbdiv = 113;
                u32Frac = 4823450;
                u32Refdiv = 3;
                u32Postdiv1 = 6;
                u32Postdiv2 = 3;
                break;
            }
            default :
            {
                return ;
            }
        }
       
        SYS_HAL_SetVoPllFbdiv(s32Pll, u32Fbdiv);
        SYS_HAL_SetVoPllFrac(s32Pll, u32Frac);
        SYS_HAL_SetVoPllRefdiv(s32Pll, u32Refdiv);
        SYS_HAL_SetVoPllPostdiv1(s32Pll, u32Postdiv1);        
        SYS_HAL_SetVoPllPostdiv2(s32Pll, u32Postdiv2);
    }
    else
    {
        HI_U32 u32ClkSel = 0x1;
    
        switch (g_stHalDevCfg[VoDev].enOutSync)
        {
            case VO_OUTPUT_PAL :
            case VO_OUTPUT_NTSC :
            {
                u32ClkSel= 0x1;
                break;
            }
            case VO_OUTPUT_960H_PAL :
            case VO_OUTPUT_960H_NTSC :
            {
                u32ClkSel= 0x0;
                break;
            }
            default:
            {
                u32ClkSel = 0x1;
            }
        }
        
        SYS_HAL_VouSdClkSel(u32ClkSel);
    }

    return;
}

HI_VOID VOU_DRV_SetVgaGc(HI_U32 u32Value)
{
    HAL_DISP_SetVgaGc(u32Value);
    return;
}

HI_VOID VOU_DRV_Open(HI_S32 VoDev)
{
    HAL_DISP_BKCOLOR_S stBkg;
    HAL_DISP_SYNCINFO_S stSyncInfo;
    HI_U16 u16VtthLine = 0;
    VO_CSC_S stCSC;
    HAL_DISP_SYNCINV_S stInv = {0};

	printf("OpenDev  vo:%d intf_type:%d out_sync:%d\n",VoDev, g_stHalDevCfg[VoDev].enIntfType,g_stHalDevCfg[VoDev].enOutSync);
    //HAL_DISP_SetIntfEnable(VoDev, HI_FALSE);
    if (VO_INTF_CVBS & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouSdDacClkEn(HI_TRUE);
        VOU_DRV_DateSetting(VoDev, g_stHalDevCfg[VoDev].enOutSync);
    }
    if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
    {        
        SYS_HAL_SelVoHdDacClk(VoDev);
        SYS_HAL_VouHdDacClkEn(HI_TRUE);
    }
    if (VO_INTF_HDMI & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_SelVoHDMIClk(VoDev);
    }
    stBkg.u8Bkg_y = YUV_Y(g_stHalDevCfg[VoDev].u32BkGrd);
    stBkg.u8Bkg_cb = YUV_U(g_stHalDevCfg[VoDev].u32BkGrd);
    stBkg.u8Bkg_cr = YUV_V(g_stHalDevCfg[VoDev].u32BkGrd);
    if ((VoDev >= VOU_DEV_DHD1) && (VoDev <= VOU_DEV_DSD0))
    {
        HAL_CBM_SetCbmBkg(VoDev, &stBkg);
    } 

    /* set interface property */
    memcpy(&stSyncInfo, &g_stSyncTiming[g_stHalDevCfg[VoDev].enOutSync], sizeof(stSyncInfo));    
    stInv.u32hs_inv = stSyncInfo.bIhs ? 1:0;    
    stInv.u32vs_inv = stSyncInfo.bIvs ? 1:0;

    if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
    {
        stCSC.enCscMatrix = VO_CSC_MATRIX_BT709_TO_RGB_PC;
        stCSC.u32Contrast = 50;
        stCSC.u32Hue = 50;
        stCSC.u32Luma = 50;
        stCSC.u32Satuature = 59;
        VOU_DRV_VGACscConfig(&stCSC);
        VOU_DRV_SetVgaGc(0xc);
        HAL_DISP_SetIntfMuxSel(VoDev,VO_INTF_VGA);
        HAL_DISP_SetIntfCSCEn(VO_INTF_VGA, HI_TRUE);

		HAL_DISP_VgaDacEn(HI_TRUE);        
        HAL_DISP_SetIntfDacEnable(VO_INTF_VGA, HI_TRUE);/*enable dac*/

        HAL_DISP_SetIntfSyncInv(VO_INTF_VGA,&stInv);
    }

    if (VO_INTF_HDMI & g_stHalDevCfg[VoDev].enIntfType)
    {
        HAL_DISP_SetIntfMuxSel(VoDev,VO_INTF_HDMI);
        //HAL_DISP_SetIntfCSCEn(VO_INTF_HDMI,HI_TRUE);
        //HAL_DISP_SetIntfDataFmt(VO_INTF_HDMI);

        if ((VO_OUTPUT_576P50 == g_stHalDevCfg[VoDev].enOutSync)
            || (VO_OUTPUT_480P60 == g_stHalDevCfg[VoDev].enOutSync))
        {
            stInv.u32hs_inv = 1 - stInv.u32hs_inv;
            stInv.u32vs_inv = 1 - stInv.u32vs_inv;
            HAL_DISP_SetIntfSyncInv(VO_INTF_HDMI,&stInv);
        }
        else
        {
             HAL_DISP_SetIntfSyncInv(VO_INTF_HDMI,&stInv); 
        }        
    }

    if (VO_INTF_CVBS & g_stHalDevCfg[VoDev].enIntfType)
    {        
        stInv.u32hs_inv = 1;
        HAL_DISP_SetIntfMuxSel(VoDev,VO_INTF_CVBS);        
        HAL_DISP_CvbsDacEn(VoDev, HI_TRUE);        
        HAL_DISP_SetIntfDacEnable(VO_INTF_CVBS, HI_TRUE);
        HAL_DISP_SetCvbsGc(VoDev, 0x26);            
        HAL_DISP_SetIntfSyncInv(HAL_DISP_INTF_CVBS,&stInv);
    }

    if (HI_TRUE == VouDrvIsMultiIntf(g_stHalDevCfg[VoDev].enIntfType))
    {
        if (VO_INTF_BT656 & g_stHalDevCfg[VoDev].enIntfType)
        {
            stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
            stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
        }
        else
        {
            stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_2;
        }
    }
    else
    {
        switch(g_stHalDevCfg[VoDev].enIntfType)
        {
            case VO_INTF_BT1120:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_2;
                break;
            case VO_INTF_BT656:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
                break;
            case VO_INTF_BT656_H:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
                break;
            case VO_INTF_BT656_L:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
                break;
            case VO_INTF_VGA:
                break;
            case VO_INTF_YPBPR:
                break;
            default:
                break;
        }
    }    

    HAL_DISP_SetIntfSync(VoDev, &stSyncInfo);

    /* set clip */ 
    if (VO_INTF_HDMI & g_stHalDevCfg[VoDev].enIntfType)        
    {   
        HAL_DISP_CLIP_S stClipData = {0x4,0x4,0x4,0x3F8,0x3F8,0x3F8};
        HAL_DISP_SetIntfClip(VO_INTF_HDMI, HI_TRUE, &stClipData);
    }    
    if(VO_INTF_BT1120& g_stHalDevCfg[VoDev].enIntfType)          
    {       
        HAL_DISP_CLIP_S stClipData = {0x10,0x10,0x10,0xeb,0xeb,0xeb};
        HAL_DISP_SetIntfClip(VO_INTF_BT1120, HI_TRUE, &stClipData);
    }
    if(VO_INTF_CVBS & g_stHalDevCfg[VoDev].enIntfType)          
    {       
        HAL_DISP_CLIP_S stClipData = {0x40,0x40,0x40,0x3ac,0x3c0,0x3c0};
        HAL_DISP_SetIntfClip(VO_INTF_CVBS, HI_TRUE, &stClipData);
    }

    #if 0
    if ((0 == stSyncInfo.bIop) && VOU_IS_HD_HW_DEV(VoDev)
        && !(VO_INTF_BT656 & g_stHalDevCfg[VoDev].enIntfType))  //HD BT656采取逐行报中断，不过也可以隔行报
    #else
    if ((0 == stSyncInfo.bIop) && (/*(VOU_DEV_DHD0==(VoDev)) || */(VOU_DEV_DHD1==(VoDev))))
    #endif
    {
        VOU_DRV_IntSetMode(VoDev, VOU_INT_MODE_FIELD);
        VOU_DRV_IntRegUpMode(VoDev, VOU_INT_MODE_FIELD);

        u16VtthLine = stSyncInfo.u16Vact - VOU_HD_VTTH_WATERLINE;
    }
    else
    {
        VOU_DRV_IntSetMode(VoDev, VOU_INT_MODE_FRAME);        
        VOU_DRV_IntRegUpMode(VoDev, VOU_INT_MODE_FRAME);

        if (0 == stSyncInfo.bIop)
        {
            u16VtthLine = stSyncInfo.u16Vact*2 - VOU_SD_VTTH_WATERLINE;
        }
        else
        {            
            u16VtthLine = stSyncInfo.u16Vact - VOU_SD_VTTH_WATERLINE;
        }
    }

    HAL_DISP_SetVtThd(VoDev, u16VtthLine);
    HAL_DISP_SetIntfEnable(VoDev, HI_TRUE);
    VOU_DRV_DevIntEnable(VoDev, HI_TRUE);
    HAL_DISP_SetRegUp(VoDev);
    g_stHalDevCfg[VoDev].bEnable = HI_TRUE;
    
    return;
}

HI_VOID VOU_DRV_Close(HI_S32 VoDev)
{    
    HAL_DISP_SetIntfEnable(VoDev, HI_FALSE);
    HAL_DISP_SetRegUp(VoDev);
    /* wait for finishing refresh */
    udelay(25 * 1000);

    if (VO_INTF_CVBS & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouSdDacClkEn(HI_FALSE);
    }
    if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouHdDacClkEn(HI_FALSE);
    }

    g_stHalDevCfg[VoDev].bEnable = HI_FALSE;
    
    return;
}

HI_VOID VOU_DRV_DefaultSetting(HI_VOID)
{
    HI_U32 i;
    
    HAL_SYS_SetRdBusId(1);

    //set each layer global alpha
    for (i=0; i<=HAL_DISP_LAYER_VSD0; i++)
    {
        HAL_LAYER_SetLayerGalpha(i, 255);
    }
    for (i=HAL_DISP_LAYER_GFX1; i<HAL_DISP_LAYER_GFX4; i++)
    {
        HAL_GRAPHIC_SetGfxPalphaRange(i, 1);
    }
    //set each cross bar default PRI
    VOU_DRV_DefLayerBindDev();
    //Vou Clock gate ctrl enable
    HAL_DISP_SetClkGateEnable(1);
    /* outstanding */
    HAL_SYS_SetOutstanding();
    HAL_DISP_ClearIntStatus(VOU_INTREPORT_ALL);
    for (i = 0; i < HAL_DISP_LAYER_GFX1; i++)
    {
        VOU_DRV_LayerEnable(i, HI_FALSE);
        VOU_DRV_SetLayerDataFmt(i, VOU_LAYER_PIXERL_FORMAT_SPYCbCr_422);
        VOU_DRV_SetLayerBkGrd(i, VOU_BACKGROUD_DEFAULT);
    }

    for (i = 0; i < VOU_DEV_MAX_NUM; i++)
    {
        VOU_DRV_IntSetMode(i, VOU_INT_MODE_FRAME);
        VOU_DRV_IntRegUpMode(i, VOU_INT_MODE_FIELD);
    }
    
    return;
}


#ifdef __cplusplus
#if __cplusplus
 }
#endif
#endif /* End of #ifdef __cplusplus */

