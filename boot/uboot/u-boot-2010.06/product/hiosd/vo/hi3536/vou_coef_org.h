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

/* 每一组缩放参数的大小，取8阶的最大的进行设置 */
#if 0
#define VOU_ZOOM_COEF_SIZE  (VOU_ZOOM_COEF_ITEM*VOU_ZOOM_COEF_MAXTAP * 2)  

/* 8tap*/
extern const int coefficient_lanczos3[18][8];//normalized ok
extern const int coefficient8_cubic[18][8];//normalized ok
extern const int coefficient8_lanczos2_8tap[18][8];//normalized ok
extern const int coefficient8_3M_a19[18][8];//normalized ok
extern const int coefficient8_2M_a05[18][8];
extern const HI_S32 coefficient8_1_5M_a05[18][8]; 

/* 6tap*/
extern const int coefficient6_6M_a25[18][6];//normalized ok
extern const int coefficient6_cubic[18][6];//normalized ok
extern const int coefficient6_5M_a25[18][6];//normalized ok
extern const int coefficient6_4M_a20[18][6];//normalized ok
extern const int coefficient6_3M_a15[18][6];//normalized ok
extern const HI_S32 coefficient6_2M_a05[18][6];
extern const HI_S32 coefficient6_1_5M_a05[18][6]; 

/* 4tap*/
extern const int coefficient4_5M_a15[18][4];//normalized ok
extern const int coefficient4_cubic[18][4];//normalized ok
extern const int coefficient4_4M_a15[18][4];//normalized ok
extern const HI_S32 coefficient4_2M_a05[18][4];
extern const HI_S32 coefficient4_1_5M_a05[18][4];

/* 色度垂直方向原来可以选择2阶，现在都同一为4阶 */
extern const HI_S32 *g_pOrgZoomCoef[VOU_ZOOM_COEF_BUTT][VOU_ZOOM_TAP_BUTT];
#endif
/* RGB->YUV601 常量系数矩阵 */
//extern const CscCoef_S g_stCSC_RGB2YUV601_tv;
/* RGB->YUV601 常量系数矩阵 */
extern const CscCoef_S g_stCSC_RGB2YUV601_pc;
/* RGB->YUV709 常量系数矩阵 */
//extern const CscCoef_S g_stCSC_RGB2YUV709_tv;
/* RGB->YUV709 常量系数矩阵 */
extern const CscCoef_S g_stCSC_RGB2YUV709_pc;
/* YUV601->RGB 常量系数矩阵 */
extern const CscCoef_S g_stCSC_YUV6012RGB_pc;
/* YUV709->RGB 常量系数矩阵 */
extern const CscCoef_S g_stCSC_YUV7092RGB_pc;
/* YUV601->YUV709 常量系数矩阵 */
extern const CscCoef_S g_stCSC_YUV2YUV_601_709;
/* YUV709->YUV601 常量系数矩阵 */
extern const CscCoef_S g_stCSC_YUV2YUV_709_601;
/* YUV601->YUV709 常量系数矩阵 */
extern const CscCoef_S g_stCSC_Init;


extern const int SIN_TABLE[61];
extern const int COS_TABLE[61];


#endif
