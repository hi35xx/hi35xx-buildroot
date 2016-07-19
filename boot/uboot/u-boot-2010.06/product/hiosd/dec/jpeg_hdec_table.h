/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : jpeg_hdec_table.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : the dqt and dht table realize in this file
                  CNcomment: 量化表和哈夫曼表的实现接口 CNend\n
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/
#ifndef __JPEG_HDEC_TABLE_H__
#define __JPEG_HDEC_TABLE_H__


/*********************************add include here******************************/

#include  "jpeglib.h"
#include  "hi_type.h"


/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" 
{
#endif
#endif /* __cplusplus */


    /***************************** Macro Definition ******************************/
    /** \addtogroup 	 JPEG TABLE MACRO */
    /** @{ */  /** <!-- 【JPEG TABLE MACRO】 */


	 /** @} */	/*! <!-- Macro Definition end */


	 /*************************** Enum Definition ****************************/

	/** \addtogroup      JPEG TABLE ENUM */
    /** @{ */  /** <!-- 【JPEG TABLE ENUM】 */


	
    /** @} */  /*! <!-- enum Definition end */

	/*************************** Structure Definition ****************************/

	/** \addtogroup      JPEG TABLE STRUCTURE */
    /** @{ */  /** <!-- 【JPEG TABLE STRUCTURE】 */

	/** @} */  /*! <!-- Structure Definition end */

	
    /********************** Global Variable declaration **************************/
 
    /******************************* API declaration *****************************/

	/** \addtogroup      JPEG TABLE API */
    /** @{ */  /** <!-- 【JPEG TABLE API】 */
	

	/*****************************************************************************
	* func			: JPEG_HDEC_SetDqt
	* description	: set the dqt
	                  CNcomment:  设置量化表信息          CNend\n
	* param[in] 	: cinfo        CNcomment:  解码对象   CNend\n
	* retval		: NA
	* others:		: NA
	*****************************************************************************/
	HI_VOID	JPEG_HDEC_SetDqt(const struct jpeg_decompress_struct *cinfo);
	
	/*****************************************************************************
	* func			: JPEG_HDEC_SetDht
	* description	: set the dht
	                  CNcomment:  设置哈夫曼表信息      CNend\n
	* param[in] 	: cinfo       CNcomment:  解码对象  CNend\n
	* retval		: NA
	* others:		: NA
	*****************************************************************************/
	HI_VOID JPEG_HDEC_SetDht(const struct jpeg_decompress_struct *cinfo);

	/** @} */  /*! <!-- API declaration end */
	
    /****************************************************************************/



#ifdef __cplusplus
    
#if __cplusplus
   
}
#endif
#endif /* __cplusplus */

#endif /* __JPEG_HDEC_TABLE_H__*/
