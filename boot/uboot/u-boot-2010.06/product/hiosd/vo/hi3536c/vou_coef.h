/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_coef.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/04/14
  Description   :
  History       :
  1.Date        : 2009/04/14
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __VOU_COEF_H__
#define __VOU_COEF_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#define VOU_GAMM_COEF_ROW   32
#define VOU_GAMM_COEF_COL   16

#define VOU_ZOOM_COEF_ITEM   18 /* 每组缩放系数有18项 */
#define VOU_ZOOM_COEF_MAXTAP 8  /* 每项最大阶数是8阶 */

#define VOU_ACC_COEF_NUM (256)

/*
    严重注意:

    寄存器上获取的比率 ratio = 输入分辨率/输出分辨率
    而算法给出的比率   ratio = 输出分辨率/输入分辨率    (HERE USE)

    SO UGLY!!!
*/
typedef enum hiVOU_ZOOM_COEF_E
{
    VOU_ZOOM_COEF_1  = 0,
    VOU_ZOOM_COEF_EQU_1,
    VOU_ZOOM_COEF_075,
    VOU_ZOOM_COEF_05,
    VOU_ZOOM_COEF_033,
    VOU_ZOOM_COEF_025,
    VOU_ZOOM_COEF_0,
    VOU_ZOOM_COEF_BUTT
} VOU_ZOOM_COEF_E;

typedef enum hiVOU_ZOOM_TAP_E
{
    VOU_ZOOM_TAP_8LH = 0,        
    VOU_ZOOM_TAP_4CH ,
    VOU_ZOOM_TAP_6LV ,
    VOU_ZOOM_TAP_4LV ,
    VOU_ZOOM_TAP_4CV ,
    //VOU_ZOOM_TAP_2CV ,
    VOU_ZOOM_TAP_BUTT
} VOU_ZOOM_TAP_E;

/* 对位域的更小拆分，一方面是为了满足10bit内存排布；
   另一方面，位域如果进行跨int操作，会出现错误。
 */
typedef struct hiVO_ZOOM_BIT_S
{
    HI_S32	bits_0	:	10	;
    HI_S32	bits_1	:	10	;
    HI_S32	bits_2	:	10	;
    HI_S32	bits_32	:	2	;
    HI_S32	bits_38	:	8	;
    HI_S32	bits_4	:	10	;
    HI_S32	bits_5	:	10	;
    HI_S32	bits_64	:	4	;
    HI_S32	bits_66	:	6	;
    HI_S32	bits_7	:	10	;
    HI_S32	bits_8	:	10	;
    HI_S32	bits_96	:	6	;
    HI_S32	bits_94	:	4	;
    HI_S32	bits_10	:	10	;
    HI_S32	bits_11	:	10	;
    HI_S32	bits_12	:	8	;
} VO_ZOOM_BIT_S;

typedef struct hiVO_ZOOM_BITARRAY_S
{
    HI_U32        u32Size;
    VO_ZOOM_BIT_S stBit[12];
} VO_ZOOM_BITARRAY_S;


/*************************************
 *  COLOR SPACE CONVERT DEFINITION   *
 *************************************/
typedef struct 
{
    HI_S32 csc_coef00;
    HI_S32 csc_coef01;
    HI_S32 csc_coef02;

    HI_S32 csc_coef10;
    HI_S32 csc_coef11;
    HI_S32 csc_coef12;

    HI_S32 csc_coef20;
    HI_S32 csc_coef21;
    HI_S32 csc_coef22;

    HI_S32 csc_in_dc0;
    HI_S32 csc_in_dc1;
    HI_S32 csc_in_dc2;

    HI_S32 csc_out_dc0;
    HI_S32 csc_out_dc1;
    HI_S32 csc_out_dc2;
} CscCoef_S;

/*************************************
 * Vga Sharpen HF Coefficient  *
 *************************************/
 typedef struct
{
    HI_S32 vga_hsp_tmp0;
    HI_S32 vga_hsp_tmp1;
    HI_S32 vga_hsp_tmp2;
    HI_S32 vga_hsp_tmp3;
    HI_U32 vga_hsp_coring;
    HI_S32 vga_hsp_gainneg;
    HI_S32 vga_hsp_gainpos;
    HI_S32 vga_hsp_adpshooten;
    HI_U32 vga_hsp_winsize;
    HI_U32 vga_hsp_mixratio;    
    HI_U32 vga_hsp_underth;
    HI_U32 vga_hsp_overth;
}HspHfCoef_S;

typedef struct
{
    HI_U32 vga_hsp_hf_shootdiv;
    HI_U32 vga_hsp_lti_ratio;
    HI_U32 vga_hsp_ldti_gain;
    HI_U32 vga_hsp_cdti_gain;
    HI_U32 vga_hsp_peak_ratio;
    HI_U32 vga_hsp_glb_overth;
    HI_U32 vga_hsp_glb_unferth;
}HspCoef_S;


#define VO_CSC_DC_MAGIC 0x07c30180


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif  /* End of #ifndef __VOU_COEF_H__ */

