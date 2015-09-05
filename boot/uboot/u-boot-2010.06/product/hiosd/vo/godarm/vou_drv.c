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
#define VOU_DEV_MAX_NUM             3
#define VOU_LAYER_MAX_NUM           17

#define VOU_VTTH_WATERLINE          100

#define VOU_BACKGROUD_BLACK         0x8080
#define VOU_BACKGROUD_GREEN         0x804D3A42
#define VOU_BACKGROUD_DEFAULT       VOU_BACKGROUD_GREEN

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
static HAL_COEF_ADDR_S g_stCoefAddr = {-1,NULL,-1,-1,-1,-1,-1,-1};
#if 0
static HAL_CSC_VALUE_S stCscDefVal[VOU_LAYER_MAX_NUM] = 
{
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50},
    {50,50,50,50}
};
#endif
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
  /*Synm, Iop, Itf,   Vact, Vbb,  Vfb,  Hact,  Hbb,  Hfb, Hmid,Bvact,Bvbb, Bvfb, Hpw, Vpw,Idv, Ihs, Ivs */
    {0,   0,   0,   288,  22,  2,   720, 132,  12,   1,  288,  23,  2, 126,  3,  0,  0,  0},     /* 576I(PAL)  */  // HDMI:hmid 300; YPbPr:hmid 0;
    {0,   0,   0,   240,  18,  4,   720, 119,  19,   1,  240,  19,  4, 124,  3,  0,  0,  0},     /* 480I(NTSC) */  // HDMI:hmid 310; YPbPr:hmid 0;
        
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
        
    {1,   1,   2,   600,  27,  1,   800, 216,  40,   1,    1,   1,  1, 128,  4,  0,  0,  0},    /* 800*600@60Hz VGA@60Hz*/
    {1,   1,   2,   768,  35,  3,  1024, 296,  24,   1,    1,   1,  1, 136,  6,  0,  0,  0},    /* 1024x768@60Hz */
    {1,   1,   2,  1024,  41,  1,  1280, 360,  48,   1,    1,   1,  1, 112,  3,  0,  0,  0},    /* 1280x1024@60Hz */
    {1,   1,   2,   768,  27,  3,  1366, 356,  70,   1,    1,   1,  1, 143,  3,  0,  0,  0},    /* 1366x768@60Hz */
    {1,   1,   2,   900,  23,  3,  1440, 112,  48,   1,    1,   1,  1,  32,  6,  0,  0,  0},    /* 1440x900@60Hz */
    {1,   1,   2,   800,  28,  3,   1280, 328,  72,  1,    1,   1,  1, 128,  6,  0,  0,  0},    /* 1280*800@60Hz VGA@60Hz*/
    {}/* User */
};

static HAL_DEV_CONFIG_S g_stHalDevCfg[VOU_DEV_MAX_NUM] =
{
    {
        .bEnable        = HI_FALSE,
        .u32BkGrd       = VOU_BACKGROUD_DEFAULT,
        .enIntfType     = VO_INTF_BT1120 | VO_INTF_HDMI,
        .enOutSync      = VO_OUTPUT_1080P60,           /* for debug: VO_OUTPUT_800x600_60 */
        .enPixelFmt     = HAL_INPUTFMT_YCbCr_SEMIPLANAR_422,     /* VOU_PIXERL_FORMAT_RGB888, VOU_PIXERL_FORMAT_YCBCR422*/
    },
    {
        .bEnable        = HI_FALSE,
        .u32BkGrd       = VOU_BACKGROUD_DEFAULT,
        .enIntfType     = VO_INTF_CVBS | VO_INTF_BT656,
        .enOutSync      = VO_OUTPUT_PAL,
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
    int i;

    HAL_VOU_Init();
    
    HAL_SYS_SetRdBusId(1);
    
    for (i=HAL_DISP_LAYER_GFX0; i<=HAL_DISP_LAYER_HC0; i++)
    {
        HAL_GRAPHIC_SetGfxPalphaRange(i, 1);
    }
 
    for (i=0; i<=HAL_DISP_LAYER_VSD1; i++)
    {
        HAL_VIDEO_SetIfirMode(i, HAL_IFIRMODE_6TAPFIR);
    }
    HAL_VIDEO_SetIfirMode(HAL_DISP_LAYER_VSD0, HAL_IFIRMODE_COPY);
  
    HAL_DISP_SetIntfDfirEn(HAL_DISP_CHANNEL_DHD0,0);
    HAL_DISP_SetIntfDfirEn(HAL_DISP_CHANNEL_DSD0,0);
    HAL_DISP_SetIntfDfirEn(HAL_DISP_CHANNEL_DSD1,0);

    VOU_DRV_DefLayerBindDev();
   
    HAL_DISP_SetClkGateEnable(1);

    return;

}

HI_VOID VOU_DRV_BoardDeInit(HI_VOID)
{
    HI_U32 i;

    for (i = 0; i < HAL_DISP_CHANNEL_BUTT; i++)
    {
        #if 0
        VOU_DRV_Close(i);
        #endif
    }

    HAL_VOU_Exit();
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

HI_VOID VOU_DRV_IntRegUpMode(VO_LAYER VoLayer, VOU_INT_MODE_E IntMode)
{
    HAL_VIDEO_SetLayerUpMode(VoLayer, IntMode);
    HAL_LAYER_SetRegUp (VoLayer);

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

HI_BOOL VOU_DRV_GetDevEnable(HI_S32 VoDev)
{
    HI_BOOL bIntfEn;
    
    HAL_DISP_GetIntfEnable(VoDev, &bIntfEn);
    
    return bIntfEn;
}

HI_VOID VOU_DRV_DateSetting(HI_S32 VoDev, VO_INTF_SYNC_E enOutSync)
{
    HI_U32 u32Data;

    switch (enOutSync)
    {
        case VO_OUTPUT_PAL :
            u32Data = 0x528414dc;
            break;
        case VO_OUTPUT_NTSC :
            u32Data = 0x108414dc;
            break;
        default :
            return;
    }
    
    HAL_DISP_SetDateCoeff(VoDev, u32Data);
    HAL_DISP_DATE_OutCtrl(VoDev, 1);
    
    return;
}

HI_VOID VOU_DRV_HDateSetting(HI_S32 VoDev, VO_INTF_SYNC_E enOutSync)
{
    return;
}

HI_VOID VOU_DRV_GammaConfig(HI_S32 VoDev)
{
    if (-1 == g_stCoefAddr.u32Gam)
    {
        return;
    }
    HAL_DISP_SetGammaAddr(VoDev, g_stCoefAddr.u32Gam);
    HAL_DISP_SetDispParaUpd(VoDev, HAL_DISP_COEFMODE_GAM);

    return;
}

HI_VOID VOU_DRV_GammaEnable(HI_S32 VoDev, HI_BOOL bEnable)
{
    HAL_DISP_SetGammaEnable(VoDev, bEnable);

    return;
}


HI_VOID VOU_DRV_DevIntEnable(HI_S32 VoDev, HI_BOOL Enable)
{
    VOU_INT_MASK_E IntType;

    switch ( VoDev )
    {
        case VOU_DEV_DHD0 :
            IntType = VOU_INTMSK_DHDVTTHD;
            break;
        case VOU_DEV_DSD0 :
            IntType = VOU_INTMSK_DSDVTTHD;
            break;
        case VOU_DEV_DSD1 :
            IntType = VOU_INTMSK_DSDVTTHD1;
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

HI_VOID VOU_DRV_IntClear(VOU_INT_MASK_E IntType)
{
    HAL_DISP_ClearIntStatus(IntType);
    
    return;
}

HI_VOID VOU_DRV_IntDisableAll(HI_VOID)
{
    HAL_DISP_ClrIntMask(VOU_INTREPORT_ALL);

    return;
}

HI_VOID VOU_DRV_IntSetMode(HI_S32 VoDev, VOU_INT_MODE_E IntMode)
{
    HAL_DISP_SetVtThdMode(VoDev, IntMode);

    return;
}

HI_VOID VOU_DRV_IntSetVTTH(HI_S32 VoDev, HI_U32 Threshold)
{
    HAL_DISP_SetVtThd(VoDev, Threshold);

    return;
}

HI_VOID VOU_DRV_LayerEnable(VO_LAYER VoLayer, HI_BOOL Enable)
{
    HAL_LAYER_EnableLayer(VoLayer, Enable);
    HAL_LAYER_SetRegUp(VoLayer);

    return;
}

HI_VOID VOU_DRV_SetLayerDdr(VO_LAYER VoLayer, VOU_LAYER_DDR_E enDdrSelId)
{
    return;
}

HI_VOID VOU_DRV_SetLayerBkGrd(VO_LAYER VoLayer, HI_U32 u32BkGrd)
{
    g_stHalLayerCfg[VoLayer].u32BkGrd = u32BkGrd;
    return;
}

HI_VOID VOU_DRV_SetLayerDataFmt(VO_LAYER VoLayer, VOU_LAYER_PIXERL_FORMAT_E enDataFmt)
{
    HAL_LAYER_SetLayerDataFmt(VoLayer, enDataFmt);
    HAL_LAYER_SetRegUp(VoLayer);

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

    s32Luma     = (HI_S32)u32Luma - 50;
    s32Contrast = ((HI_S32)u32Contrast - 50) * 2 + 100;
    s32Hue      = (HI_S32)u32Hue * 60 / 100;
    s32Satu     = ((HI_S32)u32Satuature - 50) * 2 + 100;    

    switch (enCscMode)
    {
        case HAL_CSC_MODE_BT601_TO_BT601:
        case HAL_CSC_MODE_BT709_TO_BT709:
        case HAL_CSC_MODE_RGB_TO_RGB:
            pstCscTmp = &g_stCSC_Init;
            break;
        case HAL_CSC_MODE_BT709_TO_BT601:
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

    if ((HAL_CSC_MODE_BT601_TO_RGB_PC == enCscMode) || (HAL_CSC_MODE_BT709_TO_RGB_PC == enCscMode)
        || (HAL_CSC_MODE_BT601_TO_RGB_TV == enCscMode) || (HAL_CSC_MODE_BT709_TO_RGB_TV == enCscMode))
    {
        pstCstCoef->csc_coef00 = (s32Contrast * pstCscTmp->csc_coef00) / 100;
        pstCstCoef->csc_coef01 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef01*COS_TABLE[s32Hue] - pstCscTmp->csc_coef02*SIN_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef02 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef01*SIN_TABLE[s32Hue] + pstCscTmp->csc_coef02*COS_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef10 = (s32Contrast * pstCscTmp->csc_coef10) / 100;
        pstCstCoef->csc_coef11 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef11*COS_TABLE[s32Hue] - pstCscTmp->csc_coef12*SIN_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef12 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef11*SIN_TABLE[s32Hue] + pstCscTmp->csc_coef12*COS_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef20 = (s32Contrast * pstCscTmp->csc_coef20) / 100;
        pstCstCoef->csc_coef21 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef21*COS_TABLE[s32Hue] - pstCscTmp->csc_coef22*SIN_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_coef22 = (s32Contrast * s32Satu * ((pstCscTmp->csc_coef21*SIN_TABLE[s32Hue] + pstCscTmp->csc_coef22*COS_TABLE[s32Hue]) /1000)) / 10000;
        pstCstCoef->csc_in_dc0 += s32Luma * 100 / s32Contrast;
    }
    else
    {    
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

    if (VoLayer == HAL_DISP_LAYER_GFX0)
    {
        enCscMode = HAL_CSC_MODE_RGB_TO_BT709_PC;
    }
    else if ((VoLayer == HAL_DISP_LAYER_GFX2) || (VoLayer == HAL_DISP_LAYER_GFX3))
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

HI_VOID VOU_DRV_LayerCSCEnable(VO_LAYER VoLayer, HI_BOOL bCscEn)
{
    HAL_LAYER_SetCscEn(VoLayer, bCscEn);
    HAL_LAYER_SetRegUp(VoLayer);

    return;
}

HI_VOID VOU_DRV_LayerCSCConfig(VO_LAYER VoLayer, HI_S32 u32Luma, HI_U32 u32Contrast, HI_U32 u32Hue, HI_U32 u32Satuature)
{
    HAL_CSC_MODE_E enCscMode;
    CscCoef_S stCscCoef;    

    if (VoLayer <= VOU_LAYER_VHD0)
    {
        if (VO_INTF_BT656 & g_stHalDevCfg[VoLayer].enIntfType)
        {
            enCscMode = HAL_CSC_MODE_BT601_TO_BT601;
        }
        else
        {
            enCscMode = HAL_CSC_MODE_BT601_TO_BT709;
        }        
    }
    else if (VoLayer == VOU_LAYER_VSD0)
    {
        enCscMode = HAL_CSC_MODE_BT601_TO_BT709;
    }
    else if ((VoLayer > VOU_LAYER_VSD0) && (VoLayer <= VOU_LAYER_VSD1))
    {
        enCscMode = HAL_CSC_MODE_BT601_TO_BT601;
    }
    else
    {
        enCscMode = HAL_CSC_MODE_BT601_TO_BT601;
    }

    VOU_DRV_CalcCscMatrix(u32Luma, u32Contrast, u32Hue, u32Satuature, enCscMode, &stCscCoef);

    HAL_LAYER_SetCscCoef(VoLayer, &stCscCoef);
    
    return;
}


HI_VOID VOU_DRV_VGACscConfig(HI_S32 u32Luma, HI_U32 u32Contrast, HI_U32 u32Hue, HI_U32 u32Satuature)
{
    CscCoef_S stCscCoef;

    VOU_DRV_CalcCscMatrix(u32Luma, u32Contrast, u32Hue, u32Satuature,
        HAL_CSC_MODE_BT601_TO_RGB_PC, &stCscCoef);

    HAL_DISP_SetVgaCscCoef(&stCscCoef);
    
    return;
}

HI_VOID VOU_DRV_VGACscEn(HI_BOOL bVgaCscEn)
{
    HAL_DISP_SetVgaCSCEn(bVgaCscEn);
    
    return;
}

HI_VOID VOU_DRV_DefLayerBindDev(HI_VOID)
{
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_VHD0, VOU_MIX_PRIO0, VOU_DEV_DHD0);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_VSD0, VOU_MIX_PRIO1, VOU_DEV_DHD0);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_G0, VOU_MIX_PRIO2, VOU_DEV_DHD0);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_HC0, VOU_MIX_PRIO3, VOU_DEV_DHD0);

    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_VSD0, VOU_MIX_PRIO0, VOU_DEV_DSD0);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_G1, VOU_MIX_PRIO1, VOU_DEV_DSD0);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_HC0, VOU_MIX_PRIO2, VOU_DEV_DSD0);

    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_VSD1, VOU_MIX_PRIO0, VOU_DEV_DSD1);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_G2, VOU_MIX_PRIO1, VOU_DEV_DSD1);
    HAL_CBM_SetCbmMixerPrio(VOU_LAYER_HC0, VOU_MIX_PRIO2, VOU_DEV_DSD1);

    return;
}

HI_VOID VOU_DRV_SetDevClk(VO_DEV VoDev)
{
    HI_S32 s32Pll;

    if (VOU_DEV_DHD0 == VoDev)
    {
        HI_U32 u32Frac, u32Postdiv1, u32Postdiv2, u32Fbdiv, u32Refdiv;

        s32Pll = 0;
        switch (g_stHalDevCfg[VoDev].enOutSync)
        {
            case VO_OUTPUT_PAL :
            case VO_OUTPUT_NTSC :
            {
                u32Fbdiv = 81;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 6;
                u32Postdiv2 = 6;
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
                u32Fbdiv = 99;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 4;
                u32Postdiv2 = 4;
                break;
            }
            case VO_OUTPUT_1080P50 :
            case VO_OUTPUT_1080P60 :
            {
                u32Fbdiv = 99;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 4;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_576P50 :
            case VO_OUTPUT_480P60 :
            {
                u32Fbdiv = 81;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 6;
                u32Postdiv2 = 6;
                break;
            }
            case VO_OUTPUT_800x600_60:
            {
                u32Fbdiv = 80;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 6;
                u32Postdiv2 = 4;
                break;
            }
            case VO_OUTPUT_1024x768_60:
            {
                u32Fbdiv = 65;
                u32Frac = 0;
                u32Refdiv = 1;
                u32Postdiv1 = 6;
                u32Postdiv2 = 4;
                break;
            }
            case VO_OUTPUT_1280x1024_60:
            {
                u32Fbdiv = 72;
                u32Frac = 0;
                u32Refdiv = 2;
                u32Postdiv1 = 4;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_1366x768_60:
            {
                u32Fbdiv = 171;
                u32Frac = 0;
                u32Refdiv = 4;
                u32Postdiv1 = 6;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_1440x900_60:
            {
                u32Fbdiv = 213;
                u32Frac = 0;
                u32Refdiv = 4;
                u32Postdiv1 = 6;
                u32Postdiv2 = 2;
                break;
            }
            case VO_OUTPUT_1280x800_60:
            {
                u32Fbdiv = 167;
                u32Frac = 0;
                u32Refdiv = 4;
                u32Postdiv1 = 6;
                u32Postdiv2 = 2;
                break;
            }
            default :
            {
                return;
            }
        }

        //printf("s32Pll %d, ", s32Pll);
        //printf("u32Fbdiv %d, ", u32Fbdiv);
        //printf("u32Frac %d, ", u32Frac);
        //printf("u32Refdiv %d, ", u32Refdiv);
        //printf("u32Postdiv1 %d, ", u32Postdiv1);
        //printf("u32Postdiv2 %d\n", u32Postdiv2);
        
        SYS_HAL_SetVoPllFbdiv(s32Pll, u32Fbdiv);
        SYS_HAL_SetVoPllFrac(s32Pll, u32Frac);
        SYS_HAL_SetVoPllRefdiv(s32Pll, u32Refdiv);
        SYS_HAL_SetVoPllPostdiv1(s32Pll, u32Postdiv1);        
        SYS_HAL_SetVoPllPostdiv2(s32Pll, u32Postdiv2);
    }
    else
    {
    }

    return;
}

HI_VOID VOU_DRV_Open(HI_S32 VoDev)
{
    HAL_DISP_BKCOLOR_S stBkg;
    HAL_DISP_SYNCINFO_S stSyncInfo;
    VOU_INTF_PIXERL_FORMAT_E enPixFmt;
    HI_U16 u16VtthLine = 0;  
    
    HAL_DISP_SetIntfEnable(VoDev, HI_FALSE);

    if (VO_INTF_YPBPR & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouHdDateResetSel(HI_FALSE);
        SYS_HAL_VouHdDateClkEn(HI_TRUE);
        SYS_HAL_VouHdDacClkEn(HI_TRUE);
        VOU_DRV_HDateSetting(VoDev, g_stHalDevCfg[VoDev].enOutSync);
    }
    else if (VO_INTF_CVBS & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouSdDateClkEn(VoDev, HI_TRUE);
        SYS_HAL_VouSdDacClkEn(HI_TRUE);
        VOU_DRV_DateSetting(VoDev, g_stHalDevCfg[VoDev].enOutSync);
    }
    else if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouHdDacClkEn(HI_TRUE);
    }
    else
    {
    }

    stBkg.u8Bkg_y = YUV_Y(g_stHalDevCfg[VoDev].u32BkGrd);
    stBkg.u8Bkg_cb = YUV_U(g_stHalDevCfg[VoDev].u32BkGrd);
    stBkg.u8Bkg_cr = YUV_V(g_stHalDevCfg[VoDev].u32BkGrd);
    if ((VoDev >= VOU_DEV_DHD0) && (VoDev <= VOU_DEV_DSD1))
    {
        HAL_CBM_SetCbmBkg(VoDev, &stBkg);
    }
    
    if ((VO_INTF_BT1120 & g_stHalDevCfg[VoDev].enIntfType)
        || ((VO_INTF_BT656_H | VO_INTF_BT656_L) & g_stHalDevCfg[VoDev].enIntfType))
    {
        HAL_DISP_SetBt656MuxHdSel(VoDev);

        if (VO_INTF_BT656_H & g_stHalDevCfg[VoDev].enIntfType)
        {
            HAL_DISP_SetBt656SelYc(VoDev, HI_TRUE);
        }
        if (VO_INTF_BT656_L & g_stHalDevCfg[VoDev].enIntfType)
        {
            HAL_DISP_SetBt656SelYc(VoDev, HI_FALSE);
        }
    }
    if (VO_INTF_LCD & g_stHalDevCfg[VoDev].enIntfType)
    {
        VOU_DRV_VGACscConfig(50, 50, 50, 50);
        HAL_DISP_SetVgaCSCEn(HI_TRUE);
    }
    if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
    {
        HAL_DISP_SetVgaInSel(VoDev);
        HAL_DISP_SetDac1Sel(HAL_DAC_SEL_VGA);
    }
    if (VO_INTF_YPBPR & g_stHalDevCfg[VoDev].enIntfType)
    {
        HAL_DISP_SetHddateInSel(VoDev);
        HAL_DISP_SetDac1Sel(HAL_DAC_SEL_HDDATE);
    }
    if (VO_INTF_HDMI & g_stHalDevCfg[VoDev].enIntfType)
    {
        HAL_DISP_SetHdmiSel(VoDev);
        HAL_DISP_SetHdmiVid(0x0);
        //HDMI_AV_Config_sep();
    }

    if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
    {
        VOU_DRV_VGACscConfig(50, 50, 50, 59);
        HAL_DISP_SetVgaCSCEn(HI_TRUE);
        HAL_DISP_VgaDacEn(HI_TRUE);
        HAL_DISP_SetVgaGc(0xA);
    }

    if (VO_INTF_CVBS & g_stHalDevCfg[VoDev].enIntfType)
    {
        HAL_DISP_CvbsDacEn(VoDev, HI_TRUE);        
        HAL_DISP_SetCvbsGc(VoDev, 0x3c);
    }
    if (VO_INTF_BT656 & g_stHalDevCfg[VoDev].enIntfType)
    {
        HAL_DISP_SetBt656MuxSdSel(VoDev);
    }
    
    memcpy(&stSyncInfo, &g_stSyncTiming[g_stHalDevCfg[VoDev].enOutSync], sizeof(stSyncInfo));
    if (HI_TRUE == VouDrvIsMultiIntf(g_stHalDevCfg[VoDev].enIntfType))
    {
        if (VO_INTF_BT656 & g_stHalDevCfg[VoDev].enIntfType)
        {
            stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
            stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
            enPixFmt           = VOU_INTF_PIXERL_FORMAT_YCBCR422;
        }
        else
        {
            stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_2;
            enPixFmt           = VOU_INTF_PIXERL_FORMAT_RGB888;
        }
    }
    else
    {
        switch(g_stHalDevCfg[VoDev].enIntfType)
        {
            case VO_INTF_BT1120:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_2;
                enPixFmt           = VOU_INTF_PIXERL_FORMAT_YCBCR422;
                break;
            case VO_INTF_BT656:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
                enPixFmt           = VOU_INTF_PIXERL_FORMAT_YCBCR422;
                break;
            case VO_INTF_BT656_H:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
                enPixFmt           = VOU_INTF_PIXERL_FORMAT_YCBCR422;
                break;
            case VO_INTF_BT656_L:
                stSyncInfo.bSynm   = HAL_DISP_SYNC_MODE_TIMING;
                stSyncInfo.u8Intfb = HAL_DISP_BIT_WIDTH_1;
                enPixFmt           = VOU_INTF_PIXERL_FORMAT_YCBCR422;
                break;
            case VO_INTF_VGA:
                enPixFmt           = VOU_INTF_PIXERL_FORMAT_RGB888;
                break;
            case VO_INTF_YPBPR:
                enPixFmt           = VOU_INTF_PIXERL_FORMAT_YCBCR444;
                break;
            default:
                enPixFmt = VOU_INTF_PIXERL_FORMAT_YCBCR422;
                break;
        }
    }    

    HAL_DISP_SetIntfDataFmt(VoDev, enPixFmt);
    HAL_DISP_SetIntfSync(VoDev, &stSyncInfo);

    if (HI_TRUE == VouDrvIsMultiIntf(g_stHalDevCfg[VoDev].enIntfType))        
    {
        if (VOU_DEV_DHD0==(VoDev))
        {
            HAL_DISP_CLIP_S stClipData = {0x4,0x4,0x4,0x3F8,0x3F8,0x3F8};
            HAL_DISP_SetIntfClip(VoDev, HI_TRUE, &stClipData);
        }
        else
        {
            HAL_DISP_CLIP_S stClipData = {0x40,0x40,0x40,0x3ac,0x3c0,0x3c0};
            HAL_DISP_SetIntfClip(VoDev, HI_TRUE, &stClipData);
        }
    }
    else
    {
        switch (g_stHalDevCfg[VoDev].enIntfType)
        {
            case VO_INTF_BT656 :
            case VO_INTF_CVBS :
            case VO_INTF_BT1120 :
            case VO_INTF_YPBPR :
            {
                HAL_DISP_CLIP_S stClipData = {0x40,0x40,0x40,0x3ac,0x3c0,0x3c0};
                HAL_DISP_SetIntfClip(VoDev, HI_TRUE, &stClipData);
                break;
            }
            default:
            {
                HAL_DISP_CLIP_S stClipData = {0x40,0x40,0x40,0x3ac,0x3c0,0x3c0};
                HAL_DISP_SetIntfClip(VoDev, HI_FALSE, &stClipData);
            }
        }
    }

    if (VOU_DEV_DHD0==(VoDev))
    {
        if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
        {
            //VOU_DRV_GammaConfig(VoDev);
            //VOU_DRV_GammaEnable(VoDev, HI_TRUE);
        }
    } 

    #if 0
    if ((0 == stSyncInfo.bIop) && VOU_IS_HD_HW_DEV(VoDev)
        && !(VO_INTF_BT656 & g_stHalDevCfg[VoDev].enIntfType))  //HD BT656采取逐行报中断，不过也可以隔行报
    #else
    if ((0 == stSyncInfo.bIop) && (VOU_DEV_DHD0==(VoDev)))
    #endif
    {
        VOU_DRV_IntSetMode(VoDev, VOU_INT_MODE_FIELD);
        VOU_DRV_IntRegUpMode(VoDev, VOU_INT_MODE_FIELD);
        //HAL_VIDEO_SetReadMode(VoDev, HAL_DISP_INTERLACE, HAL_DISP_INTERLACE);

        u16VtthLine = stSyncInfo.u16Vact - VOU_VTTH_WATERLINE;
    }
    else
    {
        VOU_DRV_IntSetMode(VoDev, VOU_INT_MODE_FRAME);        
        VOU_DRV_IntRegUpMode(VoDev, VOU_INT_MODE_FRAME);
        //HAL_VIDEO_SetReadMode(VoDev, HAL_DISP_PROGRESSIVE, HAL_DISP_PROGRESSIVE);

        if (0 == stSyncInfo.bIop)
        {
            u16VtthLine = stSyncInfo.u16Vact*2 - VOU_VTTH_WATERLINE;
        }
        else
        {            
            u16VtthLine = stSyncInfo.u16Vact - VOU_VTTH_WATERLINE;
        }
    }

    HAL_DISP_SetVtThd(VoDev, u16VtthLine);

    HAL_DISP_SetIntfEnable(VoDev, HI_TRUE);

    VOU_DRV_DevIntEnable(VoDev, HI_TRUE);
    
    g_stHalDevCfg[VoDev].bEnable = HI_TRUE;
    
    return;
}

HI_VOID VOU_DRV_Close(HI_S32 VoDev)
{    
    HAL_DISP_SetIntfEnable(VoDev, HI_FALSE);

    if (VO_INTF_YPBPR & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouHdDateResetSel(HI_TRUE);
        SYS_HAL_VouHdDateClkEn(HI_FALSE);
        SYS_HAL_VouHdDacClkEn(HI_FALSE);
    }
    else if (VO_INTF_CVBS & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouSdDateClkEn(VoDev, HI_FALSE);
        SYS_HAL_VouSdDacClkEn(HI_FALSE);
    }
    else if (VO_INTF_VGA & g_stHalDevCfg[VoDev].enIntfType)
    {
        SYS_HAL_VouHdDacClkEn(HI_FALSE);
    }
    else
    {
    }

    g_stHalDevCfg[VoDev].bEnable = HI_FALSE;
    
    return;
}

HI_VOID VOU_DRV_DefaultSetting(HI_VOID)
{
    HI_U32 i;

    HAL_DISP_ClearIntStatus(VOU_INTREPORT_ALL);

    for (i=0; i<VOU_DEV_MAX_NUM; i++)
    {
        VOU_DRV_LayerEnable(i, HI_FALSE);
        VOU_DRV_SetLayerDataFmt(i, VOU_LAYER_PIXERL_FORMAT_SPYCbCr_422);
        VOU_DRV_SetLayerBkGrd(i, VOU_BACKGROUD_DEFAULT); 
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

