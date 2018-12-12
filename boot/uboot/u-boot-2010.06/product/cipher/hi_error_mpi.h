/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_error_mpi.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2008/04/24
  Description   : error code of MPI
  History       :
  1.Date        : 2008/04/24
    Author      : q46153
    Modification: Created file

******************************************************************************/
/** 
\file
\brief Describes the information about common error codes. CNcomment: 提供公共错误码的相关信息
*/
/* add include here */
#ifndef __HI_UNF_MPI_ERRORCODE_H__
#define __HI_UNF_MPI_ERRORCODE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* End of #ifdef __cplusplus */

/***************************** Macro Definition ******************************/
/** \addtogroup      H_3_1 */
/** @{ */  /** <!-- 【Error codes related to media processing CNcomment: 媒体处理错误码】 */

#define  HI_ERR_CIPHER_NOT_INIT                     (HI_S32)(0x804D0001)
#define  HI_ERR_CIPHER_INVALID_HANDLE               (HI_S32)(0x804D0002)
#define  HI_ERR_CIPHER_INVALID_POINT                (HI_S32)(0x804D0003)
#define  HI_ERR_CIPHER_INVALID_PARA                 (HI_S32)(0x804D0004)
#define  HI_ERR_CIPHER_FAILED_INIT                  (HI_S32)(0x804D0005)
#define  HI_ERR_CIPHER_FAILED_GETHANDLE             (HI_S32)(0x804D0006)
#define  HI_ERR_CIPHER_FAILED_RELEASEHANDLE         (HI_S32)(0x804D0007)
#define  HI_ERR_CIPHER_FAILED_CONFIGAES             (HI_S32)(0x804D0008)
#define  HI_ERR_CIPHER_FAILED_CONFIGDES             (HI_S32)(0x804D0009)
#define  HI_ERR_CIPHER_FAILED_ENCRYPT               (HI_S32)(0x804D000A)
#define  HI_ERR_CIPHER_FAILED_DECRYPT               (HI_S32)(0x804D000B)
#define  HI_ERR_CIPHER_BUSY                         (HI_S32)(0x804D000C)
#define  HI_ERR_CIPHER_NO_AVAILABLE_RNG             (HI_S32)(0x804D000D)

/** @} */  /*! <!-- Macro Definition end */



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __HI_UNF_MPI_ERRORCODE_H__ */

