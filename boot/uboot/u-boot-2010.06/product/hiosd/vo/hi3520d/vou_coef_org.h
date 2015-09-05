/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_coef_org.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/07/24
  Description   : 
  History       :
  1.Date        : 2011/07/24
    Author      : n00168968
    Modification: Created file

******************************************************************************/

#ifndef __VOU_COEF_ORG_H__
#define __VOU_COEF_ORG_H__

#include "vou_coef.h"

extern const CscCoef_S g_stCSC_RGB2YUV601_tv;
extern const CscCoef_S g_stCSC_RGB2YUV601_pc;
extern const CscCoef_S g_stCSC_RGB2YUV709_tv;
extern const CscCoef_S g_stCSC_RGB2YUV709_pc;
extern const CscCoef_S g_stCSC_YUV6012RGB_pc;
extern const CscCoef_S g_stCSC_YUV7092RGB_pc;
extern const CscCoef_S g_stCSC_YUV2YUV_601_709;
extern const CscCoef_S g_stCSC_Init;


extern const int SIN_TABLE[61];
extern const int COS_TABLE[61];


#endif
