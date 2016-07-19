/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : jpeg_hdec_setpara.c
Version		    : Initial Draft
Author		    : y00181162
Created		    : 2014/06/20
Description	    : set parameter that hard decode need
                  CNcomment: 配置硬件解码需要的参数信息 CNend\n
Function List 	:

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162		    Created file      	
******************************************************************************/
/*********************************add include here******************************/
#include "hi_jpeg_config.h"
#include "jpeg_hdec_rwreg.h"
#include "jpeg_hdec_table.h"
#include "hi_drv_jpeg_reg.h"
#include "jpeg_hdec_api.h"


/***************************** Macro Definition ******************************/


/*************************** Structure Definition ****************************/


/********************** Global Variable declaration **************************/

/******************************* API forward declarations *******************/

/******************************* API realization *****************************/

/*****************************************************************************
 * func 		 : JPEG_HDEC_ImagFmt2HardFmt
 * description	 : convert the input jpeg format to hard register need format
				   CNcomment:  将输入图片的像素格式转换成硬件需要的像素格式 CNend\n
 * param[in]	 : cinfo	   CNcomment: 解码对象	 CNend\n
 * retval		 : NA
 * others:		 : NA
 *****************************************************************************/
static HI_VOID JPEG_HDEC_ImagFmt2HardFmt(const HI_JPEG_FMT_E enJpegFmt,HI_U32 *pu32Fmt)
{

		switch(enJpegFmt)
		{
			case JPEG_FMT_YUV400:
				   *pu32Fmt = 0;
			   break;
			case JPEG_FMT_YUV420:
				   *pu32Fmt = 3;
				   break;
			case JPEG_FMT_YUV422_21:
				   *pu32Fmt = 4;
				   break;
			case JPEG_FMT_YUV422_12:
				   *pu32Fmt = 5;
			   break;
			case JPEG_FMT_YUV444:
				   *pu32Fmt = 6;
			   break;
			default:
				   *pu32Fmt = 7;
				   break;
		}
		
}


/*****************************************************************************
* func			: JPEG_HDEC_SetSof
* description	: set the sof message
				  CNcomment: 设置sof基本信息    	   CNend\n
* param[in] 	: cinfo 		CNcomment: 解码对象    CNend\n
* retval		: HI_SUCCESS	CNcomment: 成功 	   CNend\n
* retval		: HI_FAILURE	CNcomment: 失败 	   CNend\n
* others:		: NA
*****************************************************************************/
static HI_S32 JPEG_HDEC_SetSof(const struct jpeg_decompress_struct *cinfo)
{

		
		HI_S32 s32DefaultScale	= 0;
		HI_U32 u32Stride		= 0;
		HI_U32 u32HardFmt      = 0;
		
#ifdef CONFIG_JPEG_FPGA_TEST_SET_DIFFERENT_OUTSTANDING_VALUE
		HI_S32 s32OutstandingValue = 0;
		HI_BOOL bOutStandingFlag   = HI_FALSE;
#endif
		JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);
		

        /**
		 ** set the picture type
		 ** CNcomment: 设置图片类型 CNend\n
		 **/
		JPEG_HDEC_ImagFmt2HardFmt(pJpegHandle->enImageFmt,&u32HardFmt);
		if(u32HardFmt >= 7)
		{
			return HI_FAILURE;
		}
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_PICTYPE, (HI_S32)u32HardFmt); 

        /**
		 ** set the dec scale
		 ** CNcomment: 设置解码缩放比例 CNend\n
		 **/
		#ifdef CONFIG_JPEG_HARDDEC2ARGB
		if(HI_TRUE == pJpegHandle->bDecARGB)
		{   /**
			 ** do not write data to ddr and no scale
			 ** CNcomment: 不往DDR写YUV数据并且不缩放并且不缩放，默认值为0x34 CNend\n
			 **/
		   /*Enable mem clock: | 0x8*/
		   JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_SCALE, (0x30 | 0x8));
		}
		else
	    #endif
		{
		    /**
			 ** set the scale register
			 ** CNcomment: 设置要解码的缩放比例，看该寄存器的结构来配置
			 **            先获取硬件默认的缩放比例 Scale.struBits.Outstanding = 7 CNend\n
			 **/
		    s32DefaultScale = JPEG_HDEC_ReadReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_SCALE);
			#ifdef CONFIG_JPEG_TEST_CHIP_OPEN_CK
			JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,\
				                  JPGD_REG_SCALE,              \
							      (HI_S32)(((HI_U32)s32DefaultScale & 0xfffffffc) | pJpegHandle->u32ScalRation | 0x8));

			/** himd.l 0xf8c40000 看是否已经写入 **/
			//getchar();
			#else
			JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,\
				                  JPGD_REG_SCALE,              \
							      (HI_S32)(((HI_U32)s32DefaultScale & 0xfffffffc) | pJpegHandle->u32ScalRation | 0x8));
			#endif
		}

#ifdef CONFIG_JPEG_FPGA_TEST_SET_DIFFERENT_OUTSTANDING_VALUE
		HI_JPEG_GetOutstandingValue(&s32OutstandingValue,&bOutStandingFlag);
		if(HI_TRUE == bOutStandingFlag)
		{
			s32DefaultScale = JPEG_HDEC_ReadReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_SCALE);
			JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, \
								 JPGD_REG_SCALE,			   \
								(HI_S32)(((s32DefaultScale) & s32OutstandingValue) | 0x8));
			#if 0
			/** himd.l 0xf8c40000 看是否已经写入 **/
			JPEG_TRACE("please enter getchar to next\n");
			getchar();
			#endif
			if( (0x10 != (s32DefaultScale & s32OutstandingValue & 0xf0)) && (0x20 != (s32DefaultScale & s32OutstandingValue & 0xf0)))
			{
				JPEG_TRACE("=====================================================================\n");
				JPEG_TRACE("set the outstanding value failure,please enter getchar to exit\n");
				JPEG_TRACE("s32DefaultScale = 0x%x\n",s32DefaultScale);
				JPEG_TRACE("s32DefaultScale & s32OutstandingValue = 0x%x\n",s32DefaultScale & s32OutstandingValue);
				JPEG_TRACE("=====================================================================\n");
				getchar();
			}
		}
#endif

		/**  set the picture size
	     **  CNcomment: 设置图片原始大小，宽高
		 **  这个值是解码分辨率pstMessagePrivate->stImagMsg.u32PicSize和图片本身的
		 **  的显示分辨率不一定相等，因为做了对齐操作，可能会大于jpeg本身图片大小
		 **  但硬件的限制是根据解码分辨率来着
		 **  ~0x3ff03ff = 0x111111 0000000000(w)	111111 0000000000(h) = 8192 * 8192 CNend\n
		 **/
		if ( 0 != ((pJpegHandle->stJpegSofInfo.u32InWandH & (~0x3ff03ff))) ) 
		{
			return HI_FAILURE;
		}
        JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_PICSIZE, (HI_S32)pJpegHandle->stJpegSofInfo.u32InWandH);
		
		/**
		 ** set lu and ch stride
		 ** CNcomment: 配置亮度和色度的行间距值 CNend\n
		 **/
		u32Stride = (pJpegHandle->stJpegSofInfo.u32CbCrStride << 16 ) | pJpegHandle->stJpegSofInfo.u32YStride;
		
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_STRIDE,(HI_S32)u32Stride);
		
		return HI_SUCCESS;


}
/*****************************************************************************
* func			: JPEG_HDEC_SetSampleFactor
* description	: set the sample factor---hifone has revise this bug
				  CNcomment: 设置采样因子信息   	   CNend\n
* param[in] 	: cinfo 		CNcomment: 解码对象    CNend\n
* retval		: HI_SUCCESS	CNcomment: 成功 	   CNend\n
* retval		: HI_FAILURE	CNcomment: 失败 	   CNend\n
* others:		: NA
*****************************************************************************/
static HI_VOID JPEG_HDEC_SetSampleFactor(const struct jpeg_decompress_struct *cinfo)
{
	JPEG_HDEC_HANDLE_S_PTR	pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);
	HI_U8 u8YFac = 0,u8UFac = 0,u8VFac = 0;
	u8YFac = (HI_U8)(((pJpegHandle->u8Fac[0][0] << 4) | pJpegHandle->u8Fac[0][1]) & 0xff);
	u8UFac = (HI_U8)(((pJpegHandle->u8Fac[1][0] << 4) | pJpegHandle->u8Fac[1][1]) & 0xff);
	u8VFac = (HI_U8)(((pJpegHandle->u8Fac[2][0] << 4) | pJpegHandle->u8Fac[2][1]) & 0xff);
	JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr, JPGD_REG_SAMPLINGFACTOR,(HI_S32)((u8YFac << 16) |(u8UFac << 8) | u8VFac));
}

/*****************************************************************************
* func			: JPEG_HDEC_SetStreamBuf
* description	: set stream buffer message
				  CNcomment: 设置码流buffer寄存器 	   CNend\n
* param[in] 	: cinfo 		CNcomment: 解码对象    CNend\n
* retval		: NA
* others:		: NA
*****************************************************************************/
HI_VOID JPEG_HDEC_SetStreamBuf(const struct jpeg_decompress_struct *cinfo)
{

		HI_U32 u32Align       = 64;  /**< the stream buffer should 64 bytes align *//**<CNcomment:码流buffer需要64字节对齐 */
		HI_U32 u32Offset      = 100; /**< you can set >=0,insure include the save stream buffer *//**<CNcomment:大于等于0的值，保证能够包含存储码流buffer的区域 */
		HI_U64 u64StreamSize  = 0;
		HI_CHAR *pStartStreamPhy  = NULL;
		HI_CHAR *pEndStreamPhy    = NULL;
		JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

		/**
		** if use user buffer, this buffer can not sure is 64 bytes align
		** CNcomment: 如果是使用用户的连续码流buffer，并不一定是64字节对齐
		**            硬件buffer只有有两个值即可，真正使用的存储码流的buffer，
		**            必须在这两个值之间 CNend\n
		**/
		if(HI_TRUE == pJpegHandle->stHDecDataBuf.bUserPhyMem && HI_FALSE == pJpegHandle->stHDecDataBuf.bNeedStreamReturn)
		{
			u64StreamSize = pJpegHandle->stHDecDataBuf.u64DataSize + u32Offset;
			/**
			** this can insure the start buffer is before the stream address
			** after align.
			** CNcomment: 这个能确保对齐之后在码流地址之前 CNend\n
			**/
			pStartStreamPhy = pJpegHandle->stHDecDataBuf.pDataPhyBuf - u32Align;
			pStartStreamPhy = (HI_CHAR*)(((HI_U32)pStartStreamPhy + u32Align - 1) & (~(u32Align - 1)));
			pStartStreamPhy = pStartStreamPhy - u32Offset;
			pEndStreamPhy   = (HI_CHAR*)(pJpegHandle->stHDecDataBuf.pDataPhyBuf + u64StreamSize);
		}
		else if(HI_TRUE == pJpegHandle->stHDecDataBuf.bNeedStreamReturn)
		{/** 码流回绕 **/
			pStartStreamPhy = pJpegHandle->stHDecDataBuf.pStartBufPhy;
			pEndStreamPhy   = pStartStreamPhy + pJpegHandle->stHDecDataBuf.s32BufLen;
		}
		else
		{
			u64StreamSize   = JPGD_HARD_BUFFER;
			pStartStreamPhy = pJpegHandle->stHDecDataBuf.pSaveStreamPhyBuf;
			pEndStreamPhy   = (HI_CHAR*)(pJpegHandle->stHDecDataBuf.pSaveStreamPhyBuf + u64StreamSize);
		}
		

		/**
		 ** the end address [0:d] invalid. just ok
		 ** CNcomment: 结束地址回读的时候[0:5]无效，为零，所以会看到和实际不一致，正常的 CNend\n
		 **/		
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,    \
						    JPGD_REG_STADDR, 				  \
						    (HI_S32 )pStartStreamPhy);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,    \
						    JPGD_REG_ENDADDR,				  \
						    (HI_S32)pEndStreamPhy);
		
}


/*****************************************************************************
* func			: JPEG_HDEC_SetMidderBuf
* description	: set middle buffer register
				  CNcomment: 设置JPEG硬件解码输出寄存器   CNend\n
* param[in] 	: cinfo 		CNcomment: 解码对象       CNend\n
* retval		: NA
* others:		: NA
*****************************************************************************/
static HI_VOID JPEG_HDEC_SetMidderBuf(const struct jpeg_decompress_struct * cinfo)
{

		JPEG_HDEC_HANDLE_S_PTR	  pJpegHandle = NULL;
		pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);


		/**
		 ** set y address,if jpeg hard decode out to argb,is also argb address
		 ** CNcomment: 设置亮度输出地址，要是解码输出ARGB，则也是ARGB输出地址 CNend\n
		 **/
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,   \
				             JPGD_REG_YSTADDR, 				 \
				             (HI_S32)pJpegHandle->stMiddleSurface.pMiddlePhy[0]);

#ifdef CONFIG_JPEG_HARDDEC2ARGB
		if(HI_FALSE == pJpegHandle->bDecARGB)
#endif
		{
			JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,	   \
								  JPGD_REG_UVSTADDR, 			   \
								  (HI_S32)pJpegHandle->stMiddleSurface.pMiddlePhy[1]);

		}

}


#ifdef CONFIG_JPEG_HARDDEC2ARGB


/*****************************************************************************
* func		   : JPEG_HDEC_SetDecARGBPara
* description  : set the argb parameter when jpeg hard decode output argb
				 CNcomment: 当解码输出ARGB时候需要设置的一些参数	 CNend\n
* param[in]	   : cinfo		   CNcomment: 解码对象		             CNend\n
* retval 	   : NA
* others:	   : NA
*****************************************************************************/
static HI_VOID JPEG_HDEC_SetDecARGBPara(const struct jpeg_decompress_struct *cinfo)
{


		HI_S32 s32CropX          = 0;
		HI_S32 s32CropY          = 0;
		HI_S32 s32CropStargPos  = 0;
		HI_S32 s32CropEndX       = 0;
		HI_S32 s32CropEndY       = 0;
		HI_S32 s32CropEndPos     = 0;
		HI_S32 s32HorFliter      = 0x0;
		HI_S32 s32VerFliter      = 0x0;
		HI_S32 s32OutType        = 0x0;
		HI_S32 s32OutPut         = 0x0;

		JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);


		/**
		** calc crop or no crop position and size
		** CNcomment: 计算裁剪或非裁剪的位置和大小,原图裁剪和真裁剪 CNend\n
		**/
		s32CropX    = pJpegHandle->stOutDesc.stCropRect.x;
		s32CropY    = pJpegHandle->stOutDesc.stCropRect.y;
		s32CropEndX = (s32CropX + pJpegHandle->stOutDesc.stCropRect.w - 1);
		s32CropEndY = (s32CropY + pJpegHandle->stOutDesc.stCropRect.h - 1);

		s32CropStargPos = (HI_S32)((HI_U32)(s32CropX) | ((HI_U32)s32CropY << 16));
		s32CropEndPos   = (HI_S32)(((HI_U32)s32CropEndX) | ((HI_U32)s32CropEndY << 16));

		if(HI_TRUE == pJpegHandle->stOutDesc.stOutSurface.bUserPhyMem)
		{
			JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_ARGBOUTSTRIDE,pJpegHandle->stOutDesc.stOutSurface.u32OutStride[0]);
		}
		else
		{
			JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_ARGBOUTSTRIDE,pJpegHandle->stJpegSofInfo.u32DisplayStride);
		}
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTSTARTPOS,s32CropStargPos);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTENDPOS,s32CropEndPos);


		/**
		** set alpha value,default is zero
		** CNcomment: 设置alpha值，寄存器默认值为0 CNend\n
		**/
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_ALPHA,(HI_S32)pJpegHandle->u32Alpha);

		/**
		** set the min buffer
		** CNcomment: 设置ARGB的行buffer地址 CNend\n
		**/
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_MINADDR,(int)(pJpegHandle->pMinPhyBuf));
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_MINADDR1,(int)(pJpegHandle->pMinPhyBuf  + pJpegHandle->stJpegSofInfo.u32MINSize));  


		/**
		** set output type register
		** CNcomment: 设置输出类型以及是否滤波寄存器 CNend\n
		**/
		if(HI_TRUE == pJpegHandle->stDecCoef.bEnHorMedian)
		{
			s32HorFliter = 0x8; /** 1000 **/
		}

		if(HI_TRUE == pJpegHandle->stDecCoef.bEnVerMedian)
		{
			s32VerFliter = 0x10; /** 10000 **/
		}
		#ifdef CONFIG_JPEG_ADD_GOOGLEFUNCTION
		if(JCS_ARGB_8888 == cinfo->out_color_space || JCS_RGBA_8888 == cinfo->out_color_space)
		#else
        if(JCS_ARGB_8888 == cinfo->out_color_space)
		#endif
		{
			s32OutType  =  0x1;
		}

		if(JCS_ABGR_8888 == cinfo->out_color_space)
		{
			s32OutType  =  0x0;
		}

		s32OutPut = s32HorFliter | s32VerFliter | s32OutType;

		/**
		** if output argb, the JPGD_REG_SCALE jidct_emar_en register value should set zero
		** default is one dec yuv
		** CNcomment: 设置输出类型以及是否滤波寄存器，还由缩放比例寄存器的jidct_emar_en
		**            位有关，这个配置为0才是ARGB输出，默认是1为YUV输出 CNend\n
		**/
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTTYPE,s32OutPut);


}

/*****************************************************************************
* func		   : JPEG_HDEC_SetHorSampleCoef
* description  : set the hor sample coef
				 CNcomment:  设置水平采样滤波系数    CNend\n
* param[in]	    : *pJpegHandle   CNcomment: 解码器句柄 CNend\n
* retval	   : NA
* others:	   : NA
*****************************************************************************/
static HI_VOID  JPEG_HDEC_SetHorSampleCoef(const JPEG_HDEC_HANDLE_S *pJpegHandle)
{

		HI_S32 s324C = 0;
		HI_S32 s3250 = 0;
		HI_S32 s3254 = 0;
		HI_S32 s3258 = 0;
		//HI_S32 s325C = 0;
		//HI_S32 s3260 = 0;
		//HI_S32 s3264 = 0;
		//HI_S32 s3268 = 0;

		HI_S32 s326C = 0;
		HI_S32 s3270 = 0;
		HI_S32 s3274 = 0;
		HI_S32 s3278 = 0;
		//HI_S32 s327C = 0;
		//HI_S32 s3280 = 0;
		//HI_S32 s3284 = 0;
		//HI_S32 s3288 = 0;

		s324C = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[0][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[0][1] & 0x3fff) << 16));
		s3250 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[0][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[0][3] & 0x3fff) << 16));
		s3254 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[0][4] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[0][5] & 0x3fff) << 16));
		s3258 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[0][6] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[0][7] & 0x3fff) << 16));
		//s325C = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[1][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[1][1] & 0x3fff) << 16));
		//s3260 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[1][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[1][3] & 0x3fff) << 16));
		//s3264 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[1][4] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[1][5] & 0x3fff) << 16));
		//s3268 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[1][6] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[1][7] & 0x3fff) << 16));

		s326C = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[2][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[2][1] & 0x3fff) << 16));
		s3270 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[2][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[2][3] & 0x3fff) << 16));
		s3274 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[2][4] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[2][5] & 0x3fff) << 16));
		s3278 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[2][6] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[2][7] & 0x3fff) << 16));
		//s327C = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[3][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[3][1] & 0x3fff) << 16));
		//s3280 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[3][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[3][3] & 0x3fff) << 16));
		//s3284 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[3][4] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[3][5] & 0x3fff) << 16));
		//s3288 = (HI_S32)((pJpegHandle->stDecCoef.s16HorCoef[3][6] & 0x3fff) | ((pJpegHandle->stDecCoef.s16HorCoef[3][7] & 0x3fff) << 16));


		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF00_01,s324C);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF02_03,s3250);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF04_05,s3254);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF06_07,s3258);

		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF20_21,s326C);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF22_23,s3270);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF24_25,s3274);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_HORCOEF26_27,s3278);


}


/*****************************************************************************
* func		   : JPEG_HDEC_SetVerSampleCoef
* description  : set the ver sample coef
				 CNcomment:  设置垂直采样滤波系数    CNend\n
* param[in]	    : *pJpegHandle   CNcomment: 解码器句柄 CNend\n
* retval	   : NA
* others:	   : NA
*****************************************************************************/
static HI_VOID  JPEG_HDEC_SetVerSampleCoef(const JPEG_HDEC_HANDLE_S *pJpegHandle)
{

		HI_S32 s328C = 0;
		HI_S32 s3290 = 0;
		HI_S32 s3294 = 0;
		HI_S32 s3298 = 0;
		HI_S32 s329C = 0;
		HI_S32 s32A0 = 0;
		HI_S32 s32A4 = 0;
		HI_S32 s32A8 = 0;
		
        s328C =  (pJpegHandle->stDecCoef.s16VerCoef[0][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[0][1] & 0x3fff) << 16);
	    s3290 =  (pJpegHandle->stDecCoef.s16VerCoef[0][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[0][3] & 0x3fff) << 16);
	    s3294 =  (pJpegHandle->stDecCoef.s16VerCoef[1][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[1][1] & 0x3fff) << 16);
	    s3298 =  (pJpegHandle->stDecCoef.s16VerCoef[1][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[1][3] & 0x3fff) << 16);
	    s329C =  (pJpegHandle->stDecCoef.s16VerCoef[2][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[2][1] & 0x3fff) << 16);
	    s32A0 =  (pJpegHandle->stDecCoef.s16VerCoef[2][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[2][3] & 0x3fff) << 16);
	    s32A4 =  (pJpegHandle->stDecCoef.s16VerCoef[3][0] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[3][1] & 0x3fff) << 16);
	    s32A8 =  (pJpegHandle->stDecCoef.s16VerCoef[3][2] & 0x3fff) | ((pJpegHandle->stDecCoef.s16VerCoef[3][3] & 0x3fff) << 16);
	  
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF00_01,s328C);
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF02_03,s3290);
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF10_11,s3294);
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF12_13,s3298);
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF20_21,s329C);
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF22_23,s32A0);
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF30_31,s32A4);
	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_VERCOEF32_33,s32A8);


}


/*****************************************************************************
* func		   : JPEG_HDEC_SetCSCCoef
* description  : set the CSC coef
				 CNcomment:  设置CSC转换系数    	 CNend\n
* param[in]	    : *pJpegHandle   CNcomment: 解码器句柄 CNend\n
* retval	   : NA
* others:	   : NA
*****************************************************************************/
static HI_VOID  JPEG_HDEC_SetCSCCoef(const JPEG_HDEC_HANDLE_S *pJpegHandle)
{


		/**
		 ** the dc coef is changeless, and the ac coef is change
		 ** CNcomment: 直流系数DC保持固定不变，交流系数AC保可变 CNend\n
		 **/
		 
		HI_S32 s32B4 = 0;
		HI_S32 s32B8 = 0;
		HI_S32 s32BC = 0;
		HI_S32 s32C0 = 0;
		HI_S32 s32C4 = 0;
		
		s32B4 = (pJpegHandle->stDecCoef.s16CSCCoef[0][0] & 0x7fff) | ((pJpegHandle->stDecCoef.s16CSCCoef[0][1] & 0x7fff )<<16);
		s32B8 = (pJpegHandle->stDecCoef.s16CSCCoef[0][2] & 0x7fff) | ((pJpegHandle->stDecCoef.s16CSCCoef[1][0] & 0x7fff )<<16);
		s32BC = (pJpegHandle->stDecCoef.s16CSCCoef[1][1] & 0x7fff) | ((pJpegHandle->stDecCoef.s16CSCCoef[1][2] & 0x7fff )<<16);
		s32C0 = (pJpegHandle->stDecCoef.s16CSCCoef[2][0] & 0x7fff) | ((pJpegHandle->stDecCoef.s16CSCCoef[2][1] & 0x7fff )<<16);
		s32C4 = pJpegHandle->stDecCoef.s16CSCCoef[2][2]  & 0x7fff;

	    JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_CSC00_01,s32B4);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_CSC02_10,s32B8);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_CSC11_12,s32BC);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_CSC20_21,s32C0);
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_CSC22,   s32C4);
		
		
}

/*****************************************************************************
* func		   : JPEG_HDEC_SetCropAndARGBInfo
* description  : set about dec out argb register
				 CNcomment: 设置解码输出为ARGB相关信息	 CNend\n
* param[in]	   : cinfo		   CNcomment: 解码对象		 CNend\n
* retval 	   : NA
* others:	   : NA
*****************************************************************************/
static HI_VOID JPEG_HDEC_SetCropAndARGBInfo(const struct jpeg_decompress_struct *cinfo)
{

		JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

		JPEG_HDEC_SetDecARGBPara(cinfo);

		if(HI_TRUE == pJpegHandle->stDecCoef.bSetHorSampleCoef)
		{
			JPEG_HDEC_SetHorSampleCoef(pJpegHandle);
		}
		if(HI_TRUE == pJpegHandle->stDecCoef.bSetVerSampleCoef)
		{
			JPEG_HDEC_SetVerSampleCoef(pJpegHandle);
		}
		if(HI_TRUE == pJpegHandle->stDecCoef.bSetCSCCoef)
		{
			JPEG_HDEC_SetCSCCoef(pJpegHandle);
		}
}
#endif


/*****************************************************************************
* func		   : JPEG_HDEC_SetCropAndARGBInfo
* description  : set all decode output yuv420sp information
				 CNcomment: 设置解码统一输出yuvsp420sp寄存器	 CNend\n
* param[in]	   : cinfo		   CNcomment: 解码对象		         CNend\n
* retval 	   : NA
* others:	   : NA
*****************************************************************************/
#ifdef CONFIG_JPEG_OUTPUT_YUV420SP
static HI_VOID JPEG_HDEC_SetYUV420SPInfo(const struct jpeg_decompress_struct *cinfo)
{

	 JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

     JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_OUTTYPE,0x4);

}
#endif


/*****************************************************************************
* func		   : JPEG_HDEC_SetCropAndARGBInfo
* description  : set count lu pixle sum value
				 CNcomment: 设置统计亮度值寄存器 	 CNend\n
* param[in]	   : cinfo		   CNcomment: 解码对象	 CNend\n
* retval 	   : NA
* others:	   : NA
*****************************************************************************/

#ifdef CONFIG_JPEG_OUTPUT_LUPIXSUM
static HI_VOID JPEG_HDEC_SetLuPixSumInfo(const struct jpeg_decompress_struct *cinfo)
{

	JPEG_HDEC_HANDLE_S_PTR pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

	JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_LPIXSUM1,(HI_S32)0x80000000);

}
#endif

/*****************************************************************************
* func			: JPEG_HDEC_SetPara
* description	: set the parameter that hard decode need
				  CNcomment: 配置硬件解码需要的参数信息 	   CNend\n
* param[in] 	: cinfo 		CNcomment: 解码对象    CNend\n
* retval		: HI_SUCCESS	CNcomment: 成功 	   CNend\n
* retval		: HI_FAILURE	CNcomment: 失败 	   CNend\n
* others:		: NA
*****************************************************************************/
HI_S32 JPEG_HDEC_SetPara(const struct jpeg_decompress_struct *cinfo)
{


		JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

		HI_S32 s32RetVal = HI_SUCCESS;

		/**
		 ** turn on interrupt
		 ** CNcomment: 打开中断 CNend\n
		 **/
		JPEG_HDEC_WriteReg(pJpegHandle->pJpegRegVirAddr,JPGD_REG_INTMASK,0x0);
				  
		/**
		 ** set dqt table register
		 ** CNcomment: 配置量化表，21us CNend\n
		 **/
		JPEG_HDEC_SetDqt(cinfo);
		
		/**
		 ** set dht table register
		 ** CNcomment: 配置哈夫曼表，100us ~ 120us CNend\n
		 **/
		JPEG_HDEC_SetDht(cinfo);

		/**
		 ** set sof message
		 ** CNcomment: 设置SOF信息 CNend\n
		 **/
		s32RetVal = JPEG_HDEC_SetSof(cinfo);
		if(HI_SUCCESS != s32RetVal)
		{
		   return HI_FAILURE;
		}

		/**
		 ** set sample factor message
		 ** CNcomment: 设置采样因子信息 CNend\n
		 **/
		JPEG_HDEC_SetSampleFactor(cinfo);

		/**
		 ** set stream buffer message
		 ** CNcomment: 设置码流buffer寄存器 CNend\n
		 **/
		JPEG_HDEC_SetStreamBuf(cinfo);

		/**
		 ** set middle buffer register
		 ** CNcomment: 设置JPEG硬件解码输出寄存器 CNend\n
		 **/
		JPEG_HDEC_SetMidderBuf(cinfo);
				

#ifdef CONFIG_JPEG_HARDDEC2ARGB
		/**
		 ** set about dec out argb register
		 ** CNcomment: 设置解码输出为ARGB相关信息 CNend\n
		 **/
		if(HI_TRUE == pJpegHandle->bDecARGB)
		{
		   JPEG_HDEC_SetCropAndARGBInfo(cinfo);
		}
#endif

#ifdef CONFIG_JPEG_OUTPUT_YUV420SP
		/**
		 ** set all decode output yuv420sp information
		 ** CNcomment: 设置解码统一输出yuvsp420sp寄存器 CNend\n
		 **/
		if(HI_TRUE == pJpegHandle->bOutYUV420SP)
		{
		   JPEG_HDEC_SetYUV420SPInfo(cinfo);
		}
#endif

#ifdef CONFIG_JPEG_OUTPUT_LUPIXSUM
		/**
		 ** set count lu pixle sum value
		 ** CNcomment: 设置统计亮度值寄存器 CNend\n
		 **/
		if(HI_TRUE == pJpegHandle->bLuPixSum)
		{
		   JPEG_HDEC_SetLuPixSumInfo(cinfo);
		}
#endif

#ifdef CONFIG_JPEG_TEST_CHIP_PRESS
		JPEG_HDEC_SetPress(cinfo);
#endif

		return HI_SUCCESS;

}
