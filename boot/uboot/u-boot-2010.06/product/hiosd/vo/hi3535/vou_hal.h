/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_hal.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/06/28
  Description   : vou_hal.c header file
  History       :
  1.Date        : 2011/06/28
    Author      : z00185248/n00168968
    Modification: Created file

******************************************************************************/

#ifndef __VOU_HAL_H__
#define __VOU_HAL_H__

#include "vou_reg.h"
#include "vou_def.h"
#include "vou_coef_org.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

HI_VOID HAL_VOU_Init(HI_VOID);
HI_VOID HAL_VOU_Exit(HI_VOID);

HI_VOID HAL_WriteReg(HI_U32 *pAddress, HI_U32 Value);
HI_U32 HAL_ReadReg(HI_U32 *pAddress);

HI_S32 SYS_HAL_DDRConifg(HI_VOID);
HI_S32 SYS_HAL_SelVoVgaPinConifg(HI_VOID);
HI_S32 SYS_HAL_SelVoHdmiPinConifg(HI_VOID);
HI_S32 SYS_HAL_SetVoHdmiReset(HI_BOOL bReset);
HI_S32 SYS_HAL_VouHdDacClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_SelVoHdDacClk(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_SelVoHDMIClk(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_VouSdDacClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_SelVoClk(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_SelVoSdClkDiv(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_SelVoSdClkDiv(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_VouBusResetSel(HI_BOOL bReset);
HI_S32 SYS_HAL_VouDevClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn);
HI_S32 SYS_HAL_VouHdOutClkSel(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_VouBusClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_HdmiResetSel(HI_BOOL bReset);
HI_S32 SYS_HAL_SetVoPllFrac(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllPostdiv1(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllPostdiv2(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllRefdiv(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllFbdiv(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_VouBusClkEn(HI_BOOL pbClkEn);
HI_VOID HAL_SYS_Control(HI_VOID);

/*****************************************************************************
 Prototype       : sys Relative
 Description     :

*****************************************************************************/
HI_VOID HAL_SYS_SetOutstanding(HI_VOID);
HI_VOID HAL_SYS_SetArbMode(HI_U32 bMode);
HI_VOID HAL_SYS_SetRdBusId(HI_U32 bMode);
HI_VOID HAL_SYS_VdpResetClk(HI_U32 sel);





/*****************************************************************************
 Prototype       : device Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_DISP_SetIntfEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL enIntf);
HI_BOOL HAL_DISP_SetIntfSync(HAL_DISP_OUTPUTCHANNEL_E enChan,
                                     HAL_DISP_SYNCINFO_S *pstSyncInfo);
HI_BOOL HAL_DISP_SetIntfDacEnable(HAL_DISP_INTF_E enIntf, HI_BOOL bDacEnable);

HI_BOOL HAL_DISP_SetIntfSyncInv(HAL_DISP_INTF_E enIntf, HAL_DISP_SYNCINV_S *pstInv);
HI_BOOL  HAL_DISP_SetIntfMuxSel(HAL_DISP_OUTPUTCHANNEL_E enChan,HAL_DISP_INTF_E enIntf);
HI_BOOL HAL_DISP_SetBt1120Sel(HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_DISP_VgaDacEn(HI_BOOL bEn);
HI_BOOL HAL_DISP_CvbsDacEn(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL bEn);
HI_BOOL HAL_DISP_SetVgaGc(HI_U32 u32Value);
HI_BOOL HAL_DISP_SetCvbsGc(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Value);
HI_BOOL HAL_DISP_SetIntfCSCEn(HAL_DISP_INTF_E enIntf,HI_BOOL bCscEn);
HI_BOOL HAL_DISP_SetIntfCscCoef(HAL_DISP_INTF_E enIntf,CscCoef_S *pstCoef);
HI_BOOL HAL_DISP_SetIntfClip(HAL_DISP_INTF_E enIntf, HI_BOOL enClip, HAL_DISP_CLIP_S *pstClipData);
HI_BOOL HAL_DISP_SetVtThdMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 uFieldMode);
HI_BOOL HAL_DISP_SetVtThd(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 vtthd);

HI_BOOL HAL_DISP_SetIntMask(HI_U32 u32MaskEn);
HI_BOOL HAL_DISP_ClrIntMask(HI_U32 u32MaskEn);
HI_BOOL HAL_DISP_ClearIntStatus(HI_U32 u32IntMsk);
HI_BOOL HAL_DISP_SetClkGateEnable(HI_U32 u32Data);
HI_VOID HAL_DISP_DATE_OutCtrl(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32OutCtrl);

//HI_BOOL HAL_DISP_HDATE_Enable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bEnable);
//HI_BOOL HAL_DISP_HDATE_VideoFmt(HAL_DISP_OUTPUTCHANNEL_E enChan, U_HDATE_VIDEO_FORMAT* pVFormat);
//HI_BOOL HAL_DISP_HDATE_OutCtrl(HAL_DISP_OUTPUTCHANNEL_E enChan, U_HDATE_OUT_CTRL* pOutCtrl);
//HI_BOOL HAL_DISP_HDATE_PolaCtrl(HAL_DISP_OUTPUTCHANNEL_E enChan, U_HDATE_POLA_CTRL* PolaCtrl);
HI_BOOL HAL_DISP_SetDateCoeff(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data);
HI_VOID HAL_DISP_SetRegUp (HAL_DISP_OUTPUTCHANNEL_E enChan);




/*****************************************************************************
 Prototype       : video layer Relative
 Description     :

*****************************************************************************/

HI_BOOL HAL_VIDEO_SetLayerUpMode(HAL_DISP_LAYER_E enLayer, HI_U32 bUpMode);
HI_BOOL HAL_VIDEO_SetIfirMode(HAL_DISP_LAYER_E enLayer, HAL_IFIRMODE_E enMode);
HI_BOOL HAL_VIDEO_SetIfirCoef(HAL_DISP_LAYER_E enLayer, HI_S32 * s32Coef);
HI_BOOL HAL_VIDEO_SetLayerDispRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect);
HI_BOOL HAL_VIDEO_SetLayerVideoRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect);

/*****************************************************************************
 Prototype       : layer Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_LAYER_EnableLayer(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable);
HI_BOOL HAL_LAYER_SetLayerDataFmt(HAL_DISP_LAYER_E enLayer,
                                            HAL_DISP_PIXEL_FORMAT_E enDataFmt);
HI_BOOL HAL_LAYER_SetCscCoef(HAL_DISP_LAYER_E enLayer, CscCoef_S *pstCscCoef);
HI_BOOL HAL_LAYER_SetCscEn(HAL_DISP_LAYER_E enLayer, HI_BOOL bCscEn);
HI_BOOL HAL_LAYER_SetLayerInRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect);
HI_BOOL HAL_LAYER_SetLayerGalpha(HAL_DISP_LAYER_E enLayer,
                                     HI_U8 u8Alpha0);

HI_BOOL  HAL_LAYER_SetRegUp(HAL_DISP_LAYER_E enLayer);


/*****************************************************************************
 Prototype       : graphic layer Relative
 Description     :

*****************************************************************************/
#if 1
HI_BOOL HAL_GRAPHIC_SetGfxAddr(HAL_DISP_LAYER_E enLayer, HI_U32 u32LAddr);
HI_BOOL HAL_GRAPHIC_SetGfxStride(HAL_DISP_LAYER_E enLayer, HI_U16 u16pitch);
HI_BOOL HAL_GRAPHIC_SetGfxExt(HAL_DISP_LAYER_E enLayer,
                                HAL_GFX_BITEXTEND_E enMode);
HI_BOOL HAL_GRAPHIC_SetGfxPreMult(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable);
HI_BOOL HAL_GRAPHIC_SetGfxPalpha(HAL_DISP_LAYER_E enLayer,
                                   HI_U32 bAlphaEn,HI_U32 bArange,
                                   HI_U8 u8Alpha0,HI_U8 u8Alpha1);
HI_BOOL HAL_GRAPHIC_GetGfxPalpha(HAL_DISP_LAYER_E enLayer, HI_U32 *pbAlphaEn, 
                         HI_U8 *pu8Alpha0, HI_U8 *pu8Alpha1);

HI_BOOL HAL_GRAPHIC_SetGfxPalphaRange(HAL_DISP_LAYER_E enLayer, HI_U32 bArange);

HI_BOOL HAL_GRAPHIC_SetGfxMskThd(HAL_DISP_LAYER_E enLayer, HAL_GFX_MASK_S stMsk);
#endif
/*****************************************************************************
 Prototype       : cbm layer Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_CBM_SetCbmAttr(HAL_DISP_LAYER_E enLayer, HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_CBM_SetCbmBkg(HI_U32 bMixerId, HAL_DISP_BKCOLOR_S *pstBkg);
HI_BOOL HAL_CBM_SetCbmMixerPrio(HAL_DISP_LAYER_E enLayer, HI_U8 u8Prio, HI_U8 u8MixerId);
HI_BOOL HAL_DISP_SetVgaCSCEn(HAL_DISP_LAYER_E enLayer, HI_BOOL bCscEn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* End of __VOU_HAL_H__ */

