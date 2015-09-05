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

#define VOU_ACC_COEF_NUM (256)

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

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif  /* End of #ifndef __VOU_COEF_H__ */

