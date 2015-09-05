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
#include "vou_coef.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define HI3521_V100  0x35210100
#define HI3520A_V100 0x3520A0100

#ifndef HICHIP
    #define HICHIP HI3521_V100
#endif

HI_VOID HAL_VOU_Init(HI_VOID);
HI_VOID HAL_VOU_Exit(HI_VOID);

HI_VOID HAL_WriteReg(HI_U32 *pAddress, HI_U32 Value);
HI_U32 HAL_ReadReg(HI_U32 *pAddress);

HI_S32 HDMI_AV_Config_sep(HI_VOID);
HI_S32 SYS_HAL_SelVoBt1120PinConifg(HI_VOID);
HI_S32 SYS_HAL_SelVoVgaPinConifg(HI_VOID);
HI_S32 SYS_HAL_SelVoHdmiPinConifg(HI_VOID);
HI_S32 SYS_HAL_SetVoHdmiReset(HI_BOOL bReset);
HI_S32 SYS_HAL_SelVoBt1120Clk(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_SelVoHdDacClk(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_SelVoHddateClk(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_SelVoHDMIClk(HI_U32 u32ClkSel);
HI_S32 SYS_HAL_SelVoHdClk(HI_S32 VoDev, HI_U32 u32Pll);

HI_S32 SYS_HAL_SetVoPllFrac(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllPostdiv1(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllPostdiv2(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllRefdiv(HI_S32 s32Pll, HI_U32 u32BitsSet);
HI_S32 SYS_HAL_SetVoPllFbdiv(HI_S32 s32Pll, HI_U32 u32BitsSet);

HI_S32 SYS_HAL_VouSdResetSel(HI_BOOL bReset);
HI_S32 SYS_HAL_VouHdResetSel(HI_S32 s32VoDev, HI_BOOL bReset);
HI_S32 SYS_HAL_VouHdDateResetSel(HI_BOOL bReset);
HI_S32 SYS_HAL_VouBusResetSel(HI_BOOL bReset);
HI_S32 SYS_HAL_VouDevClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn);
HI_S32 SYS_HAL_VouHdOutClkSel(HI_S32 s32Dev, HI_U32 u32ClkSel);
HI_S32 SYS_HAL_VouSdDateClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn);
HI_S32 SYS_HAL_VouHdDateClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_VouBusClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_VouSdDacClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_VouHdDacClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_HdmiResetSel(HI_BOOL bReset);
HI_S32 SYS_HAL_HdmiPixelClkEn(HI_BOOL pbClkEn);
HI_S32 SYS_HAL_HdmiBusClkEn(HI_BOOL pbClkEn);

/*****************************************************************************
 Prototype       : sys Relative
 Description     :

*****************************************************************************/
HI_VOID HAL_SYS_Control(HI_VOID);
HI_VOID HAL_SYS_SetArbMode(HI_U32 bMode);
HI_VOID HAL_SYS_SetRdBusId(HI_U32 bMode);

/*****************************************************************************
 Prototype       : device Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_DISP_SetIntfEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL enIntf);
HI_BOOL HAL_DISP_GetIntfEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL *pbIntfEn);
HI_BOOL HAL_DISP_GetIntState(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL *pbBottom);
HI_BOOL HAL_DISP_SetIntfSync(HAL_DISP_OUTPUTCHANNEL_E enChan,
                                     HAL_DISP_SYNCINFO_S *pstSyncInfo);
HI_BOOL HAL_DISP_GetDispIoP(HAL_DISP_OUTPUTCHANNEL_E enChan,HI_U32 *pu32iop);
HI_BOOL HAL_DISP_SetIntfDataFmt(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_DISP_INTFDATAFMT_E stIntfDataFmt);
HI_BOOL HAL_DISP_SetHdmiSel(HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_DISP_SetHdmiVid(HI_U32 u32Value);
HI_BOOL HAL_DISP_SetHddateInSel(HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_DISP_SetVgaInSel(HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_DISP_SetBt1120Sel(HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_DISP_SetBt656MuxHdSel(HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_DISP_SetBt656SelYc(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL bBt656H);
HI_BOOL HAL_DISP_SetBt656MuxSdSel(HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_DISP_SetDac1Sel(HAL_DAC_SEL_E DacSel);
HI_BOOL HAL_DISP_VgaDacEn(HI_BOOL bEn);
HI_BOOL HAL_DISP_CvbsDacEn(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL bEn);
HI_BOOL HAL_DISP_SetVgaGc(HI_U32 u32Value);
HI_BOOL HAL_DISP_SetCvbsGc(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Value);
HI_BOOL HAL_DISP_SetVgaCSCEn(HI_BOOL bCscEn);
HI_BOOL HAL_DISP_SetVgaCscCoef(CscCoef_S *pstCoef);
HI_BOOL HAL_DISP_SetIntfDfirEn(HAL_DISP_OUTPUTCHANNEL_E enChan,HI_U32 dfir_en);
HI_BOOL HAL_DISP_SetIntfClip(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL enClip, HAL_DISP_CLIP_S *pstClipData);
HI_BOOL HAL_DISP_SetVtThdMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 uFieldMode);
HI_BOOL HAL_DISP_GetVtThdMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 *pu32FieldMode);
HI_BOOL HAL_DISP_SetVtThd(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 vtthd);
HI_BOOL HAL_DISP_SetGammaEnable(HAL_DISP_OUTPUTCHANNEL_E enChn, HI_U32 uGmmEn);
HI_BOOL HAL_DISP_SetGammaCoef(HAL_DISP_OUTPUTCHANNEL_E enChn, HI_U32 *upTable);
HI_BOOL HAL_DISP_SetGammaAddr(HAL_DISP_OUTPUTCHANNEL_E enChn, HI_U32 uGmmAddr);
HI_BOOL HAL_DISP_SetDispParaUpd(HAL_DISP_OUTPUTCHANNEL_E enChan,HAL_DISP_COEFMODE_E enMode);

HI_BOOL HAL_DISP_SetIntMask(HI_U32 u32MaskEn);
HI_BOOL HAL_DISP_ClrIntMask(HI_U32 u32MaskEn);
HI_U32 HAL_DISP_GetIntStatus(HI_U32 u32IntMsk);
HI_BOOL HAL_DISP_ClearIntStatus(HI_U32 u32IntMsk);

HI_BOOL HAL_DISP_SetIntfClipEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bEnable);
HI_BOOL HAL_DISP_SetVSync(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 VFB, HI_U32 VBB, HI_U32 VACT);
HI_BOOL HAL_DISP_SetVSyncPlus(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 BVFB, HI_U32 BVBB, HI_U32 BVACT);
HI_BOOL HAL_DISP_SetHSync(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 HFB, HI_U32 HBB, HI_U32 HACT);
HI_BOOL HAL_DISP_SetPlusWidth(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 HPW, HI_U32 VPW);
HI_BOOL HAL_DISP_SetPlusPhase(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 IHS, HI_U32 IVS, HI_U32 IDV);
HI_BOOL HAL_DISP_SetClkGateEnable(HI_U32 u32Data);
HI_BOOL HAL_DISP_SetIntfSyncMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_DISP_SYNC_MODE_E Mode);
HI_BOOL HAL_DISP_SetIntfBitWidth(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_DISP_BIT_WIDTH_E Width);

HI_VOID HAL_DISP_DATE_OutCtrl(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32OutCtrl);
HI_BOOL HAL_DISP_SetDateScart(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bScartEna);
HI_BOOL HAL_DISP_SetDateCgain(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 Cgain);
HI_BOOL HAL_DISP_GetDateCoeff(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 index, HI_U32 *pu32DateCoeff);
HI_BOOL HAL_DISP_SetDateCoeff(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data);

HI_BOOL HAL_DISP_SetCcdMulti32Mode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 b32Rgn);
HI_BOOL HAL_DISP_SetCcdSlaveMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bSlave);
HI_BOOL HAL_DISP_SetCcdEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bEnable);
HI_BOOL HAL_DISP_SetCcdOutImgId(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_CCD_IMGID_E enImgId);
HI_BOOL HAL_DISP_SetCcdOutImgMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 ImgMode);
HI_BOOL HAL_DISP_GetCcdInImgId(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_CCD_IMGID_E *penImgId);
//HI_BOOL HAL_DISP_SetCcdInImgMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 ImgMode);
HI_BOOL HAL_DISP_SetCcdOutAreaEnable (HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 AreaNum, HI_U32 bEnable);
HI_BOOL HAL_DISP_SetCcdOutAllAreaDisable (HAL_DISP_OUTPUTCHANNEL_E enChan);
//HI_BOOL HAL_DISP_SetCcdInAreaEnable (HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 AreaNum, HI_U32 bEnable);
//HI_BOOL HAL_DISP_SetCcdAreaFPos(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 AreaNum, HI_U32 xfpos, HI_U32 yfpos);
//HI_BOOL HAL_DISP_SetCcdAreaLPos(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 AreaNum, HI_U32 xlpos, HI_U32 ylpos);
HI_BOOL HAL_DISP_SetCcdImgRight(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bEnable);
HI_BOOL HAL_DISP_SetAllCcdAreaDisable (HAL_DISP_OUTPUTCHANNEL_E enChan);


/*****************************************************************************
 Prototype       : video layer Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_VIDEO_SetLayerAddr(HAL_DISP_LAYER_E enLayer,
                                         HI_U32 u32LAddr,HI_U32 u32CAddr, 
                                         HI_U16 u16LStr, HI_U16 u16CStr);

HI_BOOL HAL_VIDEO_SetLayerUpMode(HAL_DISP_LAYER_E enLayer, HI_U32 bUpMode);
HI_BOOL HAL_VIDEO_SetReadMode(HAL_DISP_LAYER_E enLayer,
                                        HAL_DISP_DATARMODE_E enLRMode,
                                        HAL_DISP_DATARMODE_E enCRMode);
HI_BOOL HAL_VIDEO_GetReadMode(HAL_DISP_LAYER_E enLayer, HI_U32 *pu32ReadMode);
HI_BOOL HAL_VIDEO_SetIfirMode(HAL_DISP_LAYER_E enLayer, HAL_IFIRMODE_E enMode);
HI_BOOL HAL_VIDEO_SetIfirCoef(HAL_DISP_LAYER_E enLayer, HI_S32 * s32Coef);
HI_BOOL HAL_VIDEO_SetLayerDispRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect);
HI_BOOL HAL_VIDEO_SetLayerBkg(HAL_DISP_LAYER_E enLayer,HAL_DISP_BKCOLOR_S *pstBkg);
HI_BOOL HAL_VIDEO_SetFieldOrder(HAL_DISP_LAYER_E enLayer, HAL_VHD_FOD_E uFieldOrder);
HI_BOOL HAL_VIEDO_SetAcmEnable(HAL_DISP_LAYER_E enLayer, HAL_ACMBLK_ID_E enAcmId, HI_U32 bAcmEn);
HI_BOOL HAL_VIEDO_SetAcmCoef(HAL_DISP_LAYER_E enLayer, HAL_ACMBLK_ID_E enAcmId, HAL_DISP_ACMBLKINFO_S *pstCoef);
HI_BOOL HAL_VIEDO_SetAcmTestEnable(HAL_DISP_LAYER_E enLayer, HI_U32 bAcmEn);
HI_BOOL HAL_VIDEO_SetLayerAccThd(HAL_DISP_LAYER_E enLayer, ACCTHD_S *pstAccThd);
HI_BOOL HAL_VIDEO_SetLayerAccTab(HAL_DISP_LAYER_E enLayer, HI_U32 *upTable);
HI_BOOL HAL_VIDEO_SetLayerAccCtrl(HAL_DISP_LAYER_E enLayer, HI_U32 uAccEn, HI_U32 uAccMode);
HI_BOOL HAL_VIDEO_SetLayerAccRst(HAL_DISP_LAYER_E enLayer, HI_U32 uAccRst);
HI_BOOL HAL_VIDEO_SetAccWeightAddr(HI_U32 uAccAddr);
HI_BOOL HAL_VIDEO_SetAccParaUpd(HAL_DISP_COEFMODE_E enMode);

HI_BOOL HAL_VIDEO_SetMultiAreaLAddr  (HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 u32LAddr, HI_U16 stride);
HI_BOOL HAL_VIDEO_SetMultiAreaCAddr  (HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 u32CAddr, HI_U16 stride);
HI_BOOL HAL_VIDEO_SetMultiAreaEnable(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 bEnable);
HI_BOOL HAL_VIDEO_GetMultiAreaEnable(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 *pu32Enable);
HI_BOOL HAL_VIDEO_SetAllAreaDisable  (HAL_DISP_LAYER_E enLayer);
HI_BOOL HAL_VIDEO_SetMultiAreaReso(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,
                                              HI_U32 u32Width, HI_U32 u32Height);
HI_BOOL HAL_VIDEO_SetMultiAreaRect(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_RECT_S *pstVideoAreaRect);


/*****************************************************************************
 Prototype       : layer Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_LAYER_SetVouDdrSel(HAL_DISP_LAYER_E enLayer, HAL_LAYER_DDR_SEL_E DdrNo);
HI_BOOL HAL_LAYER_EnableLayer(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable);
HI_BOOL HAL_LAYER_GetLayerEnable(HAL_DISP_LAYER_E enLayer,HI_U32 *pu32Enable);
HI_BOOL HAL_LAYER_SetLayerDataFmt(HAL_DISP_LAYER_E enLayer,
                                            HAL_DISP_PIXEL_FORMAT_E enDataFmt);
HI_BOOL HAL_LAYER_GetLayerDataFmt(HAL_DISP_LAYER_E enLayer,HI_U32 *pu32Fmt);
HI_BOOL HAL_LAYER_SetCscCoef(HAL_DISP_LAYER_E enLayer, CscCoef_S *pstCscCoef);
HI_BOOL HAL_LAYER_SetCscEn(HAL_DISP_LAYER_E enLayer, HI_BOOL bCscEn);
HI_BOOL HAL_LAYER_SetLayerInRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect);
HI_BOOL HAL_LAYER_SetLayerOutRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect);
HI_BOOL HAL_LAYER_GetLayerOutRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect);
HI_BOOL HAL_LAYER_SetLayerRect(HAL_DISP_LAYER_E enLayer,
                                         HAL_DISP_RECT_S  stVideoRect);
HI_BOOL HAL_LAYER_SetLayerGalpha(HAL_DISP_LAYER_E enLayer,
                                     HI_U8 u8Alpha0);
HI_BOOL HAL_LAYER_GetLayerGalpha(HAL_DISP_LAYER_E enLayer,HI_U8 *pu8Alpha0);

HI_BOOL HAL_LAYER_SetCoefAddr(HAL_DISP_LAYER_E enLayer,
                                   HAL_DISP_COEFMODE_E enMode,
                                   HI_U32 u32Addr);
HI_BOOL HAL_LAYER_GetCoefAddr(HAL_DISP_LAYER_E enLayer,
                                   HAL_DISP_COEFMODE_E enMode,
                                   HI_U32 *pu32Addr);
HI_BOOL HAL_LAYER_SetLayerParaUpd(HAL_DISP_LAYER_E enLayer,
                                  HAL_DISP_COEFMODE_E enMode);
HI_BOOL HAL_LAYER_SetZmeEnable(HAL_DISP_LAYER_E enLayer,
                                    HAL_DISP_ZMEMODE_E enMode,
                                    HI_U32 bEnable);
HI_BOOL HAL_LAYER_SetZmeFirEnable(HAL_DISP_LAYER_E enLayer, HAL_DISP_ZMEMODE_E enMode, HI_U32 bEnable);
HI_BOOL HAL_LAYER_SetZmeMscEnable(HAL_DISP_LAYER_E enLayer, HAL_DISP_ZMEMODE_E enMode, HI_U32 bEnable);
HI_BOOL  HAL_LAYER_SetMidEnable(HAL_DISP_LAYER_E enLayer,
                                    HAL_DISP_ZMEMODE_E enMode,
                                    HI_U32 bEnable);
HI_BOOL HAL_LAYER_SetHfirOrder(HAL_DISP_LAYER_E enLayer, HI_U32 uHfirOrder);
HI_BOOL HAL_LAYER_SetZmeVerTap(HAL_DISP_LAYER_E enLayer, HAL_DISP_ZMEMODE_E enMode, HI_U32 uVerTap);
HI_BOOL HAL_LAYER_SetZmeVerType(HAL_DISP_LAYER_E enLayer, HI_U32 uVerType);
HI_BOOL  HAL_LAYER_SetZmePhase(HAL_DISP_LAYER_E enLayer,
                                   HAL_DISP_ZMEMODE_E enMode,
                                   HI_S32 s32Phase);
HI_BOOL HAL_LAYER_SetZmeIReso(HAL_DISP_LAYER_E enLayer, HI_U32 u32Width, HI_U32 u32Height);
HI_BOOL HAL_LAYER_GetZmeIReso(HAL_DISP_LAYER_E enLayer,HI_U32 *pu32Width, HI_U32 *pu32Height);
HI_BOOL HAL_LAYER_SetZmeOReso(HAL_DISP_LAYER_E enLayer, HI_U32 u32Width, HI_U32 u32Height);
HI_BOOL HAL_LAYER_GetZmeOReso(HAL_DISP_LAYER_E enLayer, HI_U32 *pu32Width, HI_U32 *pu32Height);
HI_BOOL HAL_LAYER_SetVerRatio(HAL_DISP_LAYER_E enLayer, HI_U32 uRatio);
HI_BOOL HAL_LAYER_SetHorRatio(HAL_DISP_LAYER_E enLayer, HI_U32 uRatio);
HI_BOOL HAL_LAYER_SetZmeRatio(HAL_DISP_LAYER_E enLayer);

HI_BOOL  HAL_LAYER_SetRegUp(HAL_DISP_LAYER_E enLayer);


/*****************************************************************************
 Prototype       : graphic layer Relative
 Description     :

*****************************************************************************/
#if 1
HI_BOOL HAL_GRAPHIC_SetGfxAddr(HAL_DISP_LAYER_E enLayer, HI_U32 u32LAddr);
HI_BOOL HAL_GRAPHIC_GetGfxAddr(HAL_DISP_LAYER_E enLayer, HI_U32 *pu32GfxAddr);
HI_BOOL HAL_GRAPHIC_SetGfxStride(HAL_DISP_LAYER_E enLayer, HI_U16 u16pitch);
HI_BOOL HAL_GRAPHIC_GetGfxStride(HAL_DISP_LAYER_E enLayer, HI_U32 *pu32GfxStride);
HI_BOOL HAL_GRAPHIC_SetGfxExt(HAL_DISP_LAYER_E enLayer,
                                HAL_GFX_BITEXTEND_E enMode);
HI_BOOL HAL_GRAPHIC_SetGfxPreMult(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable);
HI_BOOL HAL_GRAPHIC_SetGfxPalpha(HAL_DISP_LAYER_E enLayer,
                                   HI_U32 bAlphaEn,HI_U32 bArange,
                                   HI_U8 u8Alpha0,HI_U8 u8Alpha1);
HI_BOOL HAL_GRAPHIC_GetGfxPalpha(HAL_DISP_LAYER_E enLayer, HI_U32 *pbAlphaEn, HI_U32 *pbArange,
                         HI_U8 *pu8Alpha0, HI_U8 *pu8Alpha1);

HI_BOOL HAL_GRAPHIC_SetGfxUpdMode(HAL_DISP_LAYER_E enLayer, HI_U32 u32Data);

HI_BOOL HAL_GRAPHIC_SetGfxPalphaRange(HAL_DISP_LAYER_E enLayer, HI_U32 bArange);

HI_BOOL HAL_GRAPHIC_SetGfxMskThd(HAL_DISP_LAYER_E enLayer, HAL_GFX_MASK_S stMsk);
HI_BOOL HAL_GRAPHIC_SetGfxReadMode(HAL_DISP_LAYER_E enLayer, HI_U32 u32Data);

HI_BOOL HAL_GRAPHIC_SetGfxDcmpEnable(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable);
HI_BOOL HAL_GRAPHIC_SetGfxDcmpLineType(HAL_DISP_LAYER_E enLayer, HAL_GFX_LINETYPE_E LineType);
HI_BOOL HAL_GRAPHIC_SetGfxDcmpLineLen(HAL_DISP_LAYER_E enLayer, HAL_GFX_LINELEN_E LineLen);
HI_BOOL HAL_GRAPHIC_SetGfxDcmpBank(HAL_DISP_LAYER_E enLayer, HI_U32 bank);
HI_BOOL HAL_GRAPHIC_SetGfxDcmpStride(HAL_DISP_LAYER_E enLayer, HI_U32 stride);
HI_BOOL HAL_GRAPHIC_SetGfxDcmpAddr(HAL_DISP_LAYER_E enLayer, HI_U32 addr_a, HI_U32 addr_r, HI_U32 addr_g, HI_U32 addr_b);
HI_BOOL HAL_GRAPHIC_SetGfxDcmpInterModeEn(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable);

HI_BOOL HAL_GRAPHIC_SetGfxKeyEn(HAL_DISP_LAYER_E enLayer, HI_U32 u32KeyEnable);
HI_BOOL HAL_GRAPHIC_SetGfxKeyMode(HAL_DISP_LAYER_E enLayer, HI_U32 u32KeyOut);
HI_BOOL HAL_GRAPHIC_SetColorKeyValue(HAL_DISP_LAYER_E enLayer, 
        HAL_GFX_KEY_MAX_S stKeyMax, HAL_GFX_KEY_MIN_S stKeyMin);
HI_BOOL HAL_GRAPHIC_SetColorKeyMask(HAL_DISP_LAYER_E enLayer, HAL_GFX_MASK_S stMsk);

HI_BOOL HAL_GRAPHIC_GetGfxPreMult(HAL_DISP_LAYER_E enLayer, HI_U32 *pbEnable);
HI_BOOL HAL_CBM_ReleaseCbmAttr(HAL_DISP_LAYER_E enLayer);

#endif
/*****************************************************************************
 Prototype       : cbm layer Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_CBM_SetCbmAttr(HAL_DISP_LAYER_E enLayer, HAL_DISP_OUTPUTCHANNEL_E enChan);
HI_BOOL HAL_CBM_SetCbmBkg(HI_U32 bMixerId, HAL_DISP_BKCOLOR_S *pstBkg);
HI_BOOL HAL_CBM_SetCbmMixerPrio(HAL_DISP_LAYER_E enLayer, HI_U8 u8Prio, HI_U8 u8MixerId);


/*****************************************************************************
 Prototype       : wbc layer Relative
 Description     :

*****************************************************************************/
HI_BOOL HAL_WBC_SetWbcEnable(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable);
HI_BOOL HAL_WBC_SetWbcAddr(HAL_DISP_LAYER_E enLayer, HI_U32 u32Addr,HI_U16 u16Str);
HI_BOOL HAL_WBC_SetWbcCAddr(HAL_DISP_LAYER_E enLayer, HI_U32 u32CAddr,HI_U16 u16CStr);
HI_BOOL HAL_WBC_SetWbcOReso(HI_U32 width,  HI_U32 height);
HI_BOOL HAL_WBC_SetWbcSpd(HAL_DISP_LAYER_E enLayer, HI_U32 u16ReqSpd);
HI_BOOL HAL_WBC_SetWbcOutFmt(HAL_DISP_LAYER_E enLayer, HAL_DISP_INTFDATAFMT_E enIntfFmt);
HI_BOOL HAL_WBC_GetWbcOutFmt(HAL_DISP_LAYER_E enLayer, HAL_DISP_INTFDATAFMT_E *penIntfFmt);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* End of __VOU_HAL_H__ */

