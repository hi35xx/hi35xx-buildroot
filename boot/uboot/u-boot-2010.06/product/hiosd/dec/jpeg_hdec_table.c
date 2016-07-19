/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : jpeg_hdec_table.c
Version		    : Initial Draft
Author		    : y00181162
Created		    : 2014/06/20
Description	    : set dqt and dht message
                  CNcomment: 设置量化表和哈夫曼表信息 CNend\n
Function List 	:

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162		    Created file      	
******************************************************************************/

/*********************************add include here******************************/
#include "jpeglib.h"
#include "jpeg_hdec_rwreg.h"
#include "hi_drv_jpeg_reg.h"
#include "jpeg_hdec_api.h"

/***************************** Macro Definition ******************************/

/** luminance */
/** CNcomment:亮度 */
#define  LU         0
/** chrominance */
/** CNcomment:色度 */
#define  CH         1
/** luminance and chrominance */
/** CNcomment:亮度和色度表 */
#define  MAX_TAB    2

/** swap the two value */
/** CNcomment:交换两个值 */	
#define CAI_JPEG_SWAP(a,b) do{ a=a+b; b=a-b; a=a-b; } while(0)

/******************** to see which include file we want to use***************/



/*************************** Structure Definition ****************************/



/********************** Global Variable declaration **************************/

#if 0
static const HI_U32 s_ZOrder[DCTSIZE2 + 16] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
    63, 63, 63, 63, 63, 63, 63, 63, /**< extra entries for safety in decoder *//** CNcomment:额外的值 */
    63, 63, 63, 63, 63, 63, 63, 63
};
#endif

/******************************* API forward declarations *******************/

/******************************* API realization *****************************/


/*****************************************************************************
* func			: JPEG_HDEC_SetDqt
* description	: set the dqt
                  CNcomment:  设置量化表信息          CNend\n
* param[in] 	: cinfo        CNcomment:  解码对象   CNend\n
* retval		: NA
* others:		: NA
*****************************************************************************/
HI_VOID	JPEG_HDEC_SetDqt(const struct jpeg_decompress_struct *cinfo)
{

		HI_U16  *QCr = NULL;
		HI_U16  *QCb = NULL;
		HI_U16  *QY  = NULL;
		HI_S32 s32Cnt = 0;
		HI_S32 Q[DCTSIZE2];

		/** four dqt, about different part */
		/** CNcomment:四张量表，针对不同分量  */
		HI_S32 quant_tbl_no[NUM_QUANT_TBLS] = {0};

#ifdef CONFIG_JPEG_MPG_DEC_ENABLE
		/** LU DQT */
		/** CNcomment:亮度量化表 */
		HI_U16  QT_TABLE_Cy[DCTSIZE2]=
		{
		 16, 11, 10, 16, 24, 40, 51, 61,
		 12, 12, 14, 19, 26, 58, 60, 55,
		 14, 13, 16, 24, 40, 57, 69, 56,
		 14, 17, 22, 29, 51, 87, 80, 62,
		 18, 22, 37, 56, 68, 109,103,77,
		 24, 35, 55, 64, 81, 104,113,92,
		 49, 64, 78, 87, 103,121,120,101,
		 72, 92, 95, 98, 112,100,103,99
		};

		/** CH DQT */
		/** CNcomment:色度量化表 */
		HI_U16  QT_TABLE_CbCr[DCTSIZE2]=
		{
		 17, 18, 24, 47, 99, 99, 99, 99,
		 18, 21, 26, 66, 99, 99, 99, 99,
		 24, 26, 56, 99, 99, 99, 99, 99,
		 47, 66, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99
		};
#endif  

		JPEG_HDEC_HANDLE_S_PTR  pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

		jpeg_component_info  *compptr = NULL;

#ifdef CONFIG_JPEG_MPG_DEC_ENABLE
		if(NULL == cinfo->quant_tbl_ptrs[0])
		{
			QY	= (unsigned short *)QT_TABLE_Cy;
			QCb = (unsigned short *)QT_TABLE_CbCr;
			QCr = (unsigned short *)QT_TABLE_CbCr;
		}
		else
#endif
		{
			for (s32Cnt = 0, compptr = cinfo->comp_info; s32Cnt < cinfo->num_components; s32Cnt++, compptr++)
			{
				 /** get dqt number */
				 /** CNcomment:获取量化表号  */
				 quant_tbl_no[s32Cnt] = compptr->quant_tbl_no;
			}
			if(NULL == cinfo->quant_tbl_ptrs[quant_tbl_no[0]])
			{
			    //ERREXIT1(cinfo, JERR_NO_QUANT_TABLE, quant_tbl_no[0]); /*lint !e740 ignore by y00181162, because this is needed */
			}
			QY  = cinfo->quant_tbl_ptrs[quant_tbl_no[0]]->quantval;
			QCb = cinfo->quant_tbl_ptrs[quant_tbl_no[1]] == NULL? QY  : cinfo->quant_tbl_ptrs[quant_tbl_no[1]]->quantval;
			QCr = cinfo->quant_tbl_ptrs[quant_tbl_no[2]] == NULL? QCb : cinfo->quant_tbl_ptrs[quant_tbl_no[2]]->quantval;
		}

		/** translate to YCbCr format */
		/** CNcomment:转换成YCbCr格式  */
		for(s32Cnt = 0; s32Cnt < DCTSIZE2; s32Cnt++)
		{
			Q[s32Cnt] = QY[s32Cnt] + (QCb[s32Cnt]<<8) + (QCr[s32Cnt]<<16);
		}

		/** set dqt */
		/** CNcomment:配置量化表  */
		JPEG_HDEC_CpyData2Reg((HI_CHAR *)pJpegHandle->pJpegRegVirAddr, (HI_VOID *)Q, JPGD_REG_QUANT, sizeof(Q));


}

HI_VOID	JPEG_HDEC_Uboot_SetDqt(int *YQtTable,int *UQtTable,int *VQtTable)
{

		int  *QCr = NULL;
		int  *QCb = NULL;
		int  *QY  = NULL;
		HI_S32 s32Cnt = 0;
		HI_S32 Q[DCTSIZE2];

#ifdef CONFIG_JPEG_MPG_DEC_ENABLE
		/** LU DQT */
		/** CNcomment:亮度量化表 */
		int  QT_TABLE_Cy[DCTSIZE2]=
		{
		 16, 11, 10, 16, 24, 40, 51, 61,
		 12, 12, 14, 19, 26, 58, 60, 55,
		 14, 13, 16, 24, 40, 57, 69, 56,
		 14, 17, 22, 29, 51, 87, 80, 62,
		 18, 22, 37, 56, 68, 109,103,77,
		 24, 35, 55, 64, 81, 104,113,92,
		 49, 64, 78, 87, 103,121,120,101,
		 72, 92, 95, 98, 112,100,103,99
		};

		/** CH DQT */
		/** CNcomment:色度量化表 */
		int  QT_TABLE_CbCr[DCTSIZE2]=
		{
		 17, 18, 24, 47, 99, 99, 99, 99,
		 18, 21, 26, 66, 99, 99, 99, 99,
		 24, 26, 56, 99, 99, 99, 99, 99,
		 47, 66, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99,
		 99, 99, 99, 99, 99, 99, 99, 99
		};
#endif  


#ifdef CONFIG_JPEG_MPG_DEC_ENABLE
		if(NULL == YQtTable)
		{
			QY	= QT_TABLE_Cy;
			QCb = QT_TABLE_CbCr;
			QCr = QT_TABLE_CbCr;
		}
		else
#endif
		{
			QY  = YQtTable;
			QCb = UQtTable== NULL? QY  : UQtTable;
			QCr = VQtTable== NULL? QCb : VQtTable;
		}

		/** translate to YCbCr format */
		/** CNcomment:转换成YCbCr格式  */
		for(s32Cnt = 0; s32Cnt < DCTSIZE2; s32Cnt++)
		{
			Q[s32Cnt] = QY[s32Cnt] + (QCb[s32Cnt]<<8) + (QCr[s32Cnt]<<16);
		}

		/** set dqt */
		/** CNcomment:配置量化表  */
		JPEG_HDEC_CpyData2Reg((HI_CHAR *)JPGD_REG_BASEADDR, (HI_VOID *)Q, JPGD_REG_QUANT, sizeof(Q));


}


/*****************************************************************************
* func			: JPEG_HDEC_DecHuff
* description	: the huffman decode
* 				  CNcomment:  哈夫曼解码，计算方法如下
* 	              ibit[]最后一个非零值所在的位置 = 7
* 	                           1   2   3   4   5   6   7   8   9  10  11  ..... 16
* 	              ibit[16] = {0x0,0x2,0x2,0x3,0x1,0x1,0x1,0x0,0x0,0x0,0x0,.....,0x0}
* 	              此时 max_idx = 7;
*                 个数 = 0x2 + 0x2 + 0x3 + 0x1 + 0x1 + 0x1 = 10;
* 			       bit[]只有10个数，从bit[0]~bit[9]
* 			       bit[0] = 0  bit[1] = 1
* 			       bit[2] = 4  bit[3] = 5
* 			       bit[4] = 12 bit[5] = 13 bit[6] = 14
* 			       bit[7] = 30
* 			       bit[8] = 62
* 			       bit[9] = 126
* 			       bit[]二进制排列
* 			       0000
* 			       0001
* 			       =========
* 			       0100
* 			       0101
* 			       =========
* 			       1100
* 			       1101
* 			       1110
* 			       =========
* 			       0001 1110
* 			       =========
* 			       0011 1110
* 			       =========
* 			       0111 1110
* 			       =========                        CNend\n
* param[in] 	: huff_tbl 	  CNcomment:  哈夫曼表	CNend\n
* param[in] 	: bit    	  CNcomment:  bit位值	CNend\n
* retval		: max_idx     CNcomment:  索引值    CNend\n
* others:		: NA
*****************************************************************************/
static HI_U32 JPEG_HDEC_DecHuff(const JHUFF_TBL *huff_tbl, unsigned int bit[256])
{


	      HI_U32 idx     = 0;
		  HI_U32 cnt     = 0;
		  HI_U32 loc     = 0;
		  HI_U32 value   = 0;
		  HI_U32 max_idx = 0;

	      /** initial to zero */
          /** CNcomment:初始化值 */
		  memset(bit,0,sizeof(bit[0]));


	      /** Ln＝16，from one, zero is ignore  */
          /** CNcomment:HT位表 有16个字节，huff_tbl->bits */
	      for(idx = 1; idx < 17; idx++)
	      {
			   /** calc the none zero  */
               /** CNcomment:0 值忽略，这样可以减少计算次数，提高效率 */
	           if(0 != huff_tbl->bits[idx])
	           {
	                  /** remember the temproary max index **/
					  max_idx = idx;
	                  for(cnt = huff_tbl->bits[max_idx]; cnt>0; cnt--)
	                  {
	                      bit[loc] = value;
						  loc++;
						  value++;
	                  }
	           }
               /** CNcomment:下一个ibt[]位的起始值将是上一个ibit[]的结束值 + 1 的两倍
                             比如第3个的起始值将是第2个结束值+1 的两倍 */
	           value<<=1;
	          
	      }
		  
	      return max_idx;
		  
}


/*****************************************************************************
* func			: JPEG_HDEC_SetDC
* description	: set the dc
				  CNcomment:  设置哈夫曼DC表信息    CNend\n
* param[in] 	: cinfo 	  CNcomment:  解码对象	CNend\n
* retval		: HI_SUCCESS  CNcomment:  成功		CNend\n
* retval		: HI_FAILURE  CNcomment:  失败		CNend\n
* others:		: NA
*****************************************************************************/
HI_VOID JPEG_HDEC_SetDC(const struct jpeg_decompress_struct *cinfo)
{

		/** 2 Huff tables(DC/AC): 0--luminance,1--chrominance */
		/** CNcomment:差分表，从文件中获取到的数据 */
		
		HI_U32 cnt		  = 0;
		HI_U32 u32Index   = 0;
		HI_U32 sum_syms    =0;
		HI_U32 syms 	  = 0;
		HI_U32 temp 	  = 0;
		HI_U32 index1	  = 0;

		/** dht table,256 is limit value */
		/** CNcomment:dht表，256个值 */
		HI_U32 huffcode[MAX_TAB][256]  = {{0}};
		/** dht lenght，YU table，12 is limit value */
		/** CNcomment:dht表的长度 */
		HI_U32 dc_hufflen[MAX_TAB][12] = {{0}};
		HI_U32 dc_sym[MAX_TAB][12]	    = {{0}};
		HI_U32 max_idx[MAX_TAB] 	    = {0};
		HI_U32 hdc_tab[12]             = {0};
		
		/** calc default value */
		/** CNcomment:计算好的默认DC表的值 */
#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
        HI_U32 u32TmpHdcTab[12] = {0x200200,0x201302,0x202303,0x306304,0x40e305,0x51e306,
			                         0x63e40e,0x77e51e,0x8fe63e,0x9fe77e,0xafe8fe,0xbfe9fe};
#endif

#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
		JHUFF_TBL huff_ptr[MAX_TAB] = { { {0}, {0}, 0} };
		const HI_U8 u8DefaultDCTable[] = {
			/**< for luma DC *//** CNcomment:亮度DC */ 0x00,
			0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			/**< for chrom DC *//** CNcomment:色度DC */ 0x01,
			0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
		};
		HI_U8 *pTmp  = (HI_U8*)u8DefaultDCTable;
#else
		JHUFF_TBL *huff_ptr[MAX_TAB];
#endif

		JPEG_HDEC_HANDLE_S_PTR	pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

		/** DC table decode, the table has three paras as follows
		 ** UINT8 bits[17];
		 ** UINT8 huffval[256];
		 ** boolean sent_table; */
		/** CNcomment: DC表解码，表的几个参数
		 ** 		   UINT8 bits[17];
		 ** 		   UINT8 huffval[256];	从文件读出的huffval的值 
		 ** 		   boolean sent_table; */

#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
		if ((NULL == cinfo->dc_huff_tbl_ptrs[0]) || (NULL != cinfo->dc_huff_tbl_ptrs[2]))
		{
			/** luminance DC */
			/** CNcomment: 亮度DC差分表 */
			pTmp+=1;
			memcpy(&(huff_ptr[LU].bits[1]), pTmp, 16);
			pTmp   += 16;
			memcpy(&(huff_ptr[LU].huffval[0]), pTmp, 12);
			pTmp   += 12;
			
			/** chrominance DC */
			/** CNcomment: 色度DC差分表 */
			pTmp+=1;
			memcpy(&(huff_ptr[CH].bits[1]), pTmp, 16);
			pTmp   += 16;
			memcpy(&(huff_ptr[CH].huffval[0]), pTmp, 12);
			pTmp   += 12;
		}
		else
		{
			/** luminance DC */
			/** CNcomment: 亮度DC差分表 */
			memcpy(&(huff_ptr[LU]), cinfo->dc_huff_tbl_ptrs[0], sizeof(huff_ptr[LU]));
			/** chrominance DC */
			/** CNcomment: 色度DC差分表 */
			if(NULL == cinfo->dc_huff_tbl_ptrs[1])
			{
				memcpy(&(huff_ptr[CH]),cinfo->dc_huff_tbl_ptrs[0],sizeof(huff_ptr[LU]));
			}
			else
			{
				memcpy(&(huff_ptr[CH]),cinfo->dc_huff_tbl_ptrs[1],sizeof(huff_ptr[LU]));
			}
		}
		/**
		** calculate the largest value of iHuffsize, here we can optimize
		** the all zero of iBits[] that after Ln
		**/
		max_idx[LU] = JPEG_HDEC_DecHuff(&huff_ptr[LU],huffcode[LU]);
		/** calculate the largest value of iHuffsize for chrominance **/
		max_idx[CH] = JPEG_HDEC_DecHuff(&huff_ptr[CH],huffcode[CH]);
		
#elif defined(CONFIG_JPEG_MPG_DEC_ENABLE) && defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
		if ((NULL == cinfo->dc_huff_tbl_ptrs[0]) || (NULL != cinfo->dc_huff_tbl_ptrs[2]))
		{/** 这里已经计算好了 **/
			JPEG_HDEC_CpyData2Reg((HI_CHAR *)pJpegHandle->pJpegRegVirAddr,u32TmpHdcTab,JPGD_REG_HDCTABLE,48);
			return;
		}
		
		huff_ptr[LU] = cinfo->dc_huff_tbl_ptrs[0];
		huff_ptr[CH] = (NULL==cinfo->dc_huff_tbl_ptrs[1])?  (cinfo->dc_huff_tbl_ptrs[0]) : (cinfo->dc_huff_tbl_ptrs[1]);
		/**
		 ** calculate the largest value of iHuffsize, here we can optimize
		 ** the all zero of iBits[] that after Ln
		 **/
		max_idx[LU] = JPEG_HDEC_DecHuff(huff_ptr[LU],huffcode[LU]);
		/** calculate the largest value of iHuffsize for chrominance **/
		max_idx[CH] = JPEG_HDEC_DecHuff(huff_ptr[CH],huffcode[CH]);
#else
		huff_ptr[LU] = cinfo->dc_huff_tbl_ptrs[0];
		huff_ptr[CH] = (NULL==cinfo->dc_huff_tbl_ptrs[1])?  (cinfo->dc_huff_tbl_ptrs[0]) : (cinfo->dc_huff_tbl_ptrs[1]);
		/**
		** calculate the largest value of iHuffsize, here we can optimize
		** the all zero of iBits[] that after Ln
		**/
		max_idx[LU] = JPEG_HDEC_DecHuff(huff_ptr[LU],huffcode[LU]);
		/** calculate the largest value of iHuffsize for chrominance **/
		max_idx[CH] = JPEG_HDEC_DecHuff(huff_ptr[CH],huffcode[CH]);
#endif

		/** Store huff length and huff value number **/

		/** two, one is luminance, the other is chrominance */
		/** CNcomment: 两个表，一个是亮度表，一个是色度表 */
		for(cnt = 0; cnt < MAX_TAB; cnt++)
		{

			temp = 0;
			sum_syms = 0;
			/** the largest value of iHuffsize**/
			for(u32Index = 0; u32Index < max_idx[cnt]; u32Index++)
			{/** 最大的非零值 **/
			    #if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
				syms = huff_ptr[cnt].bits[u32Index+1];
				#else
				syms = huff_ptr[cnt]->bits[u32Index+1];
				#endif
				if(syms)
				{
					/** calculate the number of node **/
					sum_syms +=syms; 
					while(syms--)
					{
						/** calclulate the value of iHuffsize **/
						dc_hufflen[cnt][temp] = u32Index + 1;
						/** calculate iHuffVal[] **/
						#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
						dc_sym[cnt][temp] = huff_ptr[cnt].huffval[temp];
						#else
						dc_sym[cnt][temp] = huff_ptr[cnt]->huffval[temp];
						#endif
						temp++;
					}
				}
				
			}

			/** sort */
			/** CNcomment: 排序 */
			/** from less to large about iHuffVal[] **/
			for(u32Index = 0; u32Index < sum_syms; u32Index++)
			{
				for(index1=u32Index+1;index1<sum_syms;index1++)
				{
					if(dc_sym[cnt][u32Index]>dc_sym[cnt][index1])
					{
						CAI_JPEG_SWAP(dc_sym[cnt][u32Index],dc_sym[cnt][index1]);
						CAI_JPEG_SWAP(dc_hufflen[cnt][u32Index],dc_hufflen[cnt][index1]);
						CAI_JPEG_SWAP(huffcode[cnt][u32Index],huffcode[cnt][index1]);
					}
				}

			}
			
		  /** Generate DC table **/
		  /** this table make up with iHuffVal[],iHuffSize,iHuffcode **/
		  if(0 == cnt)
		  {/** LU **/
				/** sum_syms = ibit[] 和 的大小，也就是有多少个Huff值 **/
				for(u32Index = 0;u32Index < sum_syms; u32Index++)
				{

					if(dc_sym[0][u32Index] >= 12)
					{
						//ERREXIT(cinfo, JERR_BAD_HUFF_TABLE); /*lint !e740 ignore by y00181162, because this is needed */  
					}
					/** (~(0xfff)) = 0xfffff000 这里计算好,省了计算的时间 **/
					*(hdc_tab + dc_sym[0][u32Index]) = \
							  ( (*(hdc_tab + dc_sym[0][u32Index])) & 0xfffff000 ) \
							| ( (dc_hufflen[0][u32Index] & 0xf)<<8 ) \
							| ( (huffcode[0][u32Index] & 0xff) );
				}

		  }
		  else
		  {/** CH **/
		  
				for(u32Index = 0;u32Index < sum_syms; u32Index++)
				{
						if(dc_sym[cnt][u32Index] >= 12)
						{
							//ERREXIT(cinfo, JERR_BAD_HUFF_TABLE); /*lint !e740 ignore by y00181162, because this is needed */  
						}
						/** (~(0xfff<<12)) = 0xff000fff 这里计算好,省了计算的时间 **/
						*(hdc_tab + dc_sym[cnt][u32Index]) = \
							  ( (*(hdc_tab + dc_sym[1][u32Index])) & 0xff000fff ) \
							| ( (dc_hufflen[1][u32Index] & 0xf)<<20 ) \
							| ( (huffcode[1][u32Index] & 0xff)<<12 );
				}

		  }

		}
		/** Set HW DC table **/
		JPEG_HDEC_CpyData2Reg((HI_CHAR *)pJpegHandle->pJpegRegVirAddr,hdc_tab,JPGD_REG_HDCTABLE,48);

}

HI_VOID JPEG_HDEC_Uboot_SetDC(JHUFF_TBL * dat)
{	
		/** 2 Huff tables(DC/AC): 0--luminance,1--chrominance */
		/** CNcomment:差分表，从文件中获取到的数据 */
		
		HI_U32 cnt		  = 0;
		HI_U32 u32Index   = 0;
		HI_U32 sum_syms    =0;
		HI_U32 syms 	  = 0;
		HI_U32 temp 	  = 0;
		HI_U32 index1	  = 0;

		/** dht table,256 is limit value */
		/** CNcomment:dht表，256个值 */
		HI_U32 huffcode[MAX_TAB][256]  = {{0}};
		/** dht lenght，YU table，12 is limit value */
		/** CNcomment:dht表的长度 */
		HI_U32 dc_hufflen[MAX_TAB][12] = {{0}};
		HI_U32 dc_sym[MAX_TAB][12]	    = {{0}};
		HI_U32 max_idx[MAX_TAB] 	    = {0};
		HI_U32 hdc_tab[12]             = {0};
		
		JHUFF_TBL huff_ptr[MAX_TAB] = { { {0}, {0}, 0} };

		/** DC table decode, the table has three paras as follows
		 ** UINT8 bits[17];
		 ** UINT8 huffval[256];
		 ** boolean sent_table; */
		/** CNcomment: DC表解码，表的几个参数
		 ** 		   UINT8 bits[17];
		 ** 		   UINT8 huffval[256];	从文件读出的huffval的值 
		 ** 		   boolean sent_table; */

        //if ((NULL == cinfo->dc_huff_tbl_ptrs[0]) || (NULL != cinfo->dc_huff_tbl_ptrs[2]))
        //{
        //	/** luminance DC */
        //	/** CNcomment: 亮度DC差分表 */
        //	pTmp+=1;
        //	memcpy(&(huff_ptr[LU].bits[1]), pTmp, 16);
        //	pTmp   += 16;
        //	memcpy(&(huff_ptr[LU].huffval[0]), pTmp, 12);
        //	pTmp   += 12;
        //	
        //	/** chrominance DC */
        //	/** CNcomment: 色度DC差分表 */
        //	pTmp+=1;
        //	memcpy(&(huff_ptr[CH].bits[1]), pTmp, 16);
        //	pTmp   += 16;
        //	memcpy(&(huff_ptr[CH].huffval[0]), pTmp, 12);
        //	pTmp   += 12;
        //}
        //else
		{
			/** luminance DC */
			/** CNcomment: 亮度DC差分表 */
			memcpy(&(huff_ptr[LU]), &dat[0], sizeof(huff_ptr[LU]));
			/** chrominance DC */
			/** CNcomment: 色度DC差分表 */
            if(NULL == &dat[1])
            {
            	memcpy(&(huff_ptr[CH]),&dat[0],sizeof(huff_ptr[LU]));
            }
            else
			{
				memcpy(&(huff_ptr[CH]),&dat[1],sizeof(huff_ptr[LU]));
			}
		}
		/**
		** calculate the largest value of iHuffsize, here we can optimize
		** the all zero of iBits[] that after Ln
		**/
		max_idx[LU] = JPEG_HDEC_DecHuff(&huff_ptr[LU],huffcode[LU]);
		/** calculate the largest value of iHuffsize for chrominance **/
		max_idx[CH] = JPEG_HDEC_DecHuff(&huff_ptr[CH],huffcode[CH]);
		
		/** Store huff length and huff value number **/

		/** two, one is luminance, the other is chrominance */
		/** CNcomment: 两个表，一个是亮度表，一个是色度表 */
		for(cnt = 0; cnt < MAX_TAB; cnt++)
		{

			temp = 0;
			sum_syms = 0;
			/** the largest value of iHuffsize**/
			for(u32Index = 0; u32Index < max_idx[cnt]; u32Index++)
			{/** 最大的非零值 **/
				syms = huff_ptr[cnt].bits[u32Index+1];
				if(syms)
				{
					/** calculate the number of node **/
					sum_syms +=syms; 
					while(syms--)
					{
						/** calclulate the value of iHuffsize **/
						dc_hufflen[cnt][temp] = u32Index + 1;
						/** calculate iHuffVal[] **/
						
						dc_sym[cnt][temp] = huff_ptr[cnt].huffval[temp];
						
						temp++;
					}
				}
				
			}

			/** sort */
			/** CNcomment: 排序 */
			/** from less to large about iHuffVal[] **/
			for(u32Index = 0; u32Index < sum_syms; u32Index++)
			{
				for(index1=u32Index+1;index1<sum_syms;index1++)
				{
					if(dc_sym[cnt][u32Index]>dc_sym[cnt][index1])
					{
						CAI_JPEG_SWAP(dc_sym[cnt][u32Index],dc_sym[cnt][index1]);
						CAI_JPEG_SWAP(dc_hufflen[cnt][u32Index],dc_hufflen[cnt][index1]);
						CAI_JPEG_SWAP(huffcode[cnt][u32Index],huffcode[cnt][index1]);
					}
				}

			}
			
		  /** Generate DC table **/
		  /** this table make up with iHuffVal[],iHuffSize,iHuffcode **/
		  if(0 == cnt)
		  {/** LU **/
				/** sum_syms = ibit[] 和 的大小，也就是有多少个Huff值 **/
				for(u32Index = 0;u32Index < sum_syms; u32Index++)
				{

					if(dc_sym[0][u32Index] >= 12)
					{
						//ERREXIT(cinfo, JERR_BAD_HUFF_TABLE); /*lint !e740 ignore by y00181162, because this is needed */  
					}
					/** (~(0xfff)) = 0xfffff000 这里计算好,省了计算的时间 **/
					*(hdc_tab + dc_sym[0][u32Index]) = \
							  ( (*(hdc_tab + dc_sym[0][u32Index])) & 0xfffff000 ) \
							| ( (dc_hufflen[0][u32Index] & 0xf)<<8 ) \
							| ( (huffcode[0][u32Index] & 0xff) );
				}

		  }
		  else
		  {/** CH **/
		  
				for(u32Index = 0;u32Index < sum_syms; u32Index++)
				{
						if(dc_sym[cnt][u32Index] >= 12)
						{
							//ERREXIT(cinfo, JERR_BAD_HUFF_TABLE); /*lint !e740 ignore by y00181162, because this is needed */  
						}
						/** (~(0xfff<<12)) = 0xff000fff 这里计算好,省了计算的时间 **/
						*(hdc_tab + dc_sym[cnt][u32Index]) = \
							  ( (*(hdc_tab + dc_sym[1][u32Index])) & 0xff000fff ) \
							| ( (dc_hufflen[1][u32Index] & 0xf)<<20 ) \
							| ( (huffcode[1][u32Index] & 0xff)<<12 );
				}

		  }

		}
		/** Set HW DC table **/
		JPEG_HDEC_CpyData2Reg((HI_CHAR *)JPGD_REG_BASEADDR,hdc_tab,JPGD_REG_HDCTABLE,48);

}

/*****************************************************************************
* func			: JPEG_HDEC_SetAC
* description	: set the ac
				  CNcomment:  设置哈夫曼AC表信息    CNend\n
* param[in] 	: cinfo 	  CNcomment:  解码对象	CNend\n
* retval		: HI_SUCCESS  CNcomment:  成功		CNend\n
* retval		: HI_FAILURE  CNcomment:  失败		CNend\n
* others:		: NA
*****************************************************************************/
HI_VOID JPEG_HDEC_SetAC(const struct jpeg_decompress_struct *cinfo)
{
		/** 2 Huff tables(DC/AC): 0--luminance,1--chrominance */
		/** CNcomment:差分表，从文件中获取到的数据 */	
		HI_U32 cnt		  = 0;
		HI_U32 u32Index   = 0;
		HI_U32 pre_index  = 0;
		HI_U32 sum_syms    =0;
		HI_U32 syms 	  = 0;
		
		/** dht table,256 is limit value */
		/** CNcomment:dht表，256个值 */
		HI_U32 huffcode[MAX_TAB][256]  = {{0}};
		/** dht lenght，YU table，12 is limit value */
		/** CNcomment:dht表的长度 */
		HI_U32 min_tab[MAX_TAB][16]    = {{0}};
		HI_U32 base_tab[MAX_TAB][16]   = {{0}};
		HI_U32 max_idx[MAX_TAB] 	   = {0};

		HI_U32 hac_min_tab[8]   = {0};
		HI_U32 hac_base_tab[8]	 = {0};
		HI_U32 hac_symbol_tab[256]	  = {0};

#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
		HI_U32 u32TmpHacMinTab[8]   = {0x0,0xa040a04,0x38183a1a,0xf678f878,0xf6f4f6f6,0xf4f6f4f6,0xe0f0e0f0,0x88c282c0};
		HI_U32 u32TmpHacBaseTab[8]	  = {0x0,0xf9fef9fe,0xd1edcfec,0x1a951793,0x2520211c,0x302a2c26,0x48000000,0xa367a364};
		HI_U32 u32TmpHacSymblTab[256]	  = 
			  {
				0x1,      0x102,	0x203,	  0x300,   0x1104,	  0x411,   0x505,    0x2112,	
				0x3121,   0x631,   0x1241,	  0x4106,  0x5113,	  0x751,   0x6161,	  0x7107,   
				0x1322,   0x2271,	0x3214,   0x8132,	0x881,	  0x1491,  0x42a1,	  0x9108,   
				0xa123,   0xb142,	0xc1b1,   0x9c1,   0x2315,	  0x3352,  0x52d1,	  0xf0f0,   
				0x1524,   0x6233,	0x7262,   0xd172,	0xa82,	  0x1609,  0x240a,	  0x3416,   
				0xe117,   0x2518,	0xf119,   0x171a,	0x1825,   0x1926,	0x1a27,   0x2628,	
				0x2729,   0x282a,	0x2934,   0x2a35,	0x3536,   0x3637,	0x3738,   0x3839,	
				0x393a,   0x3a43,	0x4344,   0x4445,	0x4546,   0x4647,	0x4748,   0x4849,	
				0x494a,   0x4a53,	0x5354,   0x5455,	0x5556,   0x5657,	0x5758,   0x5859,	
				0x595a,   0x5a63,	0x6364,   0x6465,	0x6566,   0x6667,	0x6768,   0x6869,	
				0x696a,   0x6a73,	0x7374,   0x7475,	0x7576,   0x7677,	0x7778,   0x7879,	
				0x797a,   0x7a83,	0x8284,   0x8385,	0x8486,   0x8587,	0x8688,   0x8789,	
				0x888a,   0x8992,	0x8a93,   0x9294,	0x9395,   0x9496,	0x9597,   0x9698,	
				0x9799,   0x989a,	0x99a2,   0x9aa3,	0xa2a4,   0xa3a5,	0xa4a6,   0xa5a7,	
				0xa6a8,   0xa7a9,	0xa8aa,   0xa9b2,	0xaab3,   0xb2b4,	0xb3b5,   0xb4b6,	
				0xb5b7,   0xb6b8,	0xb7b9,   0xb8ba,	0xb9c2,   0xbac3,	0xc2c4,   0xc3c5,	
				0xc4c6,   0xc5c7,	0xc6c8,   0xc7c9,	0xc8ca,   0xc9d2,	0xcad3,   0xd2d4,	
				0xd3d5,   0xd4d6,	0xd5d7,   0xd6d8,	0xd7d9,   0xd8da,	0xd9e1,   0xdae2,	
				0xe2e3,   0xe3e4,	0xe4e5,   0xe5e6,	0xe6e7,   0xe7e8,	0xe8e9,   0xe9ea,	
				0xeaf1,   0xf2f2,	0xf3f3,   0xf4f4,	0xf5f5,   0xf6f6,	0xf7f7,   0xf8f8,	
				0xf9f9,   0xfafa,	0x0,      0x0,      0x0,	   0x0,     0x0,      0x0,   
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0,	
				0x0,   0x0,   0x0,	 0x0,	0x0,   0x0,   0x0,	 0x0
			  };
#endif

#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
        JHUFF_TBL huff_ptr[MAX_TAB] = { { {0}, {0}, 0} };

		const HI_U8 u8DefaultACTable[] = {
			/**< for luma AC *//** CNcomment:亮度AC */ 0x10,
			0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
			0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
			0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
			0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
			0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
			0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
			0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
			0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
			0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
			0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
			0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
			0xF9, 0xFA,
			/**< for chrom AC *//** CNcomment:色度AC */ 0x11,
			0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
			0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
			0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
			0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
			0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
			0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
			0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
			0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
			0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
			0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
			0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
			0xF9, 0xFA
		};
		HI_U8 *pTmp  = (HI_U8*)u8DefaultACTable;
#else
		JHUFF_TBL *huff_ptr[MAX_TAB];
#endif

		JPEG_HDEC_HANDLE_S_PTR	pJpegHandle = (JPEG_HDEC_HANDLE_S_PTR)(cinfo->client_data);

		/** AC Decode Begin */
		/** CNcomment: 开始AC表解码 */
		/** Generate hac_min_table, hac_base_table **/
#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
		if ((NULL == cinfo->ac_huff_tbl_ptrs[0]) || (NULL != cinfo->ac_huff_tbl_ptrs[2]))
		{
			pTmp+=1;
			memset(&(huff_ptr[LU].bits[0]),0,17);
			memcpy(&(huff_ptr[LU].bits[1]), pTmp, 16);
			pTmp   += 16;
			memset(&(huff_ptr[LU].huffval[0]),0,256);
			memcpy(&(huff_ptr[LU].huffval[0]), pTmp, 162);
			pTmp   += 162;

			pTmp+=1;
			memset(&(huff_ptr[CH].bits[0]),0,17);
			memcpy(&(huff_ptr[CH].bits[1]), pTmp, 16);
			pTmp   += 16;
			memset(&(huff_ptr[CH].huffval[0]),0,256);
			memcpy(&(huff_ptr[CH].huffval[0]), pTmp, 162);
		}
		else
		{
			/** 亮度AC系数表 **/
			memset(&(huff_ptr[LU].bits[0]),0,17);
			memset(&(huff_ptr[LU].huffval[0]),0,256);
			memset(&(huff_ptr[CH].bits[0]),0,17);
			memset(&(huff_ptr[CH].huffval[0]),0,256);
			memcpy(&(huff_ptr[LU]), cinfo->ac_huff_tbl_ptrs[0], sizeof(huff_ptr[LU]));
			/** 色度AC系数表 **/
			if(NULL == cinfo->ac_huff_tbl_ptrs[1])
			{
				memcpy(&(huff_ptr[CH]),cinfo->ac_huff_tbl_ptrs[0],sizeof(huff_ptr[LU]));
			}
			else
			{
				memcpy(&(huff_ptr[CH]),cinfo->ac_huff_tbl_ptrs[1],sizeof(huff_ptr[LU]));
			}
		}
		max_idx[LU] = JPEG_HDEC_DecHuff(&huff_ptr[0],huffcode[LU]);
		max_idx[CH] = JPEG_HDEC_DecHuff(&huff_ptr[1],huffcode[CH]);

#elif defined(CONFIG_JPEG_MPG_DEC_ENABLE) && defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
		if ((NULL == cinfo->ac_huff_tbl_ptrs[0]) || (NULL != cinfo->ac_huff_tbl_ptrs[2]))
		{
			JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, u32TmpHacMinTab,   JPGD_REG_HACMINTABLE,  32);
			JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, u32TmpHacBaseTab,  JPGD_REG_HACBASETABLE, 32);
			JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, u32TmpHacSymblTab, JPGD_REG_HACSYMTABLE,  0x2c0);
			return;
		}
		huff_ptr[LU] = cinfo->ac_huff_tbl_ptrs[0];
		huff_ptr[CH] = (NULL==cinfo->ac_huff_tbl_ptrs[1])? (cinfo->ac_huff_tbl_ptrs[0]) : (cinfo->ac_huff_tbl_ptrs[1]);
		max_idx[LU]  = JPEG_HDEC_DecHuff(huff_ptr[0],huffcode[LU]);
		max_idx[CH]  = JPEG_HDEC_DecHuff(huff_ptr[1],huffcode[CH]);
#else
		huff_ptr[LU] = cinfo->ac_huff_tbl_ptrs[0];
		huff_ptr[CH] = (NULL==cinfo->ac_huff_tbl_ptrs[1])? (cinfo->ac_huff_tbl_ptrs[0]) : (cinfo->ac_huff_tbl_ptrs[1]);
		max_idx[LU]  = JPEG_HDEC_DecHuff(huff_ptr[0],huffcode[LU]);
		max_idx[CH]  = JPEG_HDEC_DecHuff(huff_ptr[1],huffcode[CH]);
#endif

		/** Luminance and Chrominance: LU=0,CH=1;**/
		for(cnt = 0; cnt < MAX_TAB; cnt++)
		{
			sum_syms=0;
			for(u32Index = 0; u32Index < 16; u32Index++)
			{
				/** the ibit[] has sixteen value,read from jpeg file */
				/** CNcomment: ibit[]有16个，从jpeg文件读到16个值 */
				#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
				syms = huff_ptr[cnt].bits[u32Index+1];
		        #else
				syms = huff_ptr[cnt]->bits[u32Index+1];
				#endif
				pre_index = u32Index?(u32Index-1):(0);
		
				if(u32Index < max_idx[cnt])
				{

					#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
					min_tab[cnt][u32Index] = (min_tab[cnt][pre_index]+huff_ptr[cnt].bits[u32Index])<<1;
                    #else
					min_tab[cnt][u32Index] = (min_tab[cnt][pre_index]+huff_ptr[cnt]->bits[u32Index])<<1;
					#endif
					if(syms)
					{
						base_tab[cnt][u32Index]=sum_syms-min_tab[cnt][u32Index];
					}
		            #if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
					sum_syms += huff_ptr[cnt].bits[u32Index+1];
					#else
					sum_syms += huff_ptr[cnt]->bits[u32Index+1];
					#endif
		
				}
				else
				{
					min_tab[cnt][u32Index]=~0;
				}
				
			}
		
			if(0 == cnt)
			{/** Create LU symbol table **/
				for(u32Index=0;u32Index<sum_syms;u32Index++)
				{
					#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
					hac_symbol_tab[u32Index] |= ( (unsigned int)(huff_ptr[0].huffval[u32Index]) );
					#else
					hac_symbol_tab[u32Index] |= ( (unsigned int)(huff_ptr[0]->huffval[u32Index]) );
					#endif
				}
			}
			else
			{/** Create CH symbol table **/
				for(u32Index=0;u32Index<sum_syms;u32Index++)
				{
				    #if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
					hac_symbol_tab[u32Index] |= ( (unsigned int)(huff_ptr[1].huffval[u32Index])<<8 );
					#else
					hac_symbol_tab[u32Index] |= ( (unsigned int)(huff_ptr[1]->huffval[u32Index])<<8 );
					#endif
				}
			}
			
		}
		
		/** Conbine Luminnance and Chrominance */
		/** CNcomment: 组合亮度和色度表 */
		for(u32Index=0;u32Index<8;u32Index++)
		{
		
			hac_min_tab[u32Index] =   ( (min_tab[CH][2*u32Index+1] & 0xff)<<24 )  \
								 | ( (min_tab[CH][2*u32Index] & 0xff)<<16	)  \
								 | ( (min_tab[LU][2*u32Index+1] & 0xff)<<8	)  \
								 | (min_tab[LU][2*u32Index] & 0xff);
		
			hac_base_tab[u32Index] =   ((base_tab[CH][2*u32Index+1] & 0xff)<<24 ) \
								  | ((base_tab[CH][2*u32Index] & 0xff)<<16	 ) \
								  | ((base_tab[LU][2*u32Index+1] & 0xff)<<8  ) \
								  | (base_tab[LU][2*u32Index] & 0xff);
			
		}
		
		/** Write hac_min/base/symbol_table into  HW reisters */
		/** CNcomment: 将hac_min/base值配给硬件寄存器 */
		JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, hac_min_tab,   JPGD_REG_HACMINTABLE,  32);
		JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, hac_base_tab,  JPGD_REG_HACBASETABLE, 32);
		/** 0x2c0 = ac_max_sum_syms * 4 */
		/** CNcomment: 值0x2c0 = ac_max_sum_syms * 4计算得来 */
		JPEG_HDEC_CpyData2Reg(pJpegHandle->pJpegRegVirAddr, hac_symbol_tab,JPGD_REG_HACSYMTABLE,  0x2c0);

}

HI_VOID JPEG_HDEC_Uboot_SetAC(JHUFF_TBL * dat)
{
            /** 2 Huff tables(DC/AC): 0--luminance,1--chrominance */
            /** CNcomment:差分表，从文件中获取到的数据 */   
            HI_U32 cnt        = 0;
            HI_U32 u32Index   = 0;
            HI_U32 pre_index  = 0;
            HI_U32 sum_syms    =0;
            HI_U32 syms       = 0;
            
            /** dht table,256 is limit value */
            /** CNcomment:dht表，256个值 */
            HI_U32 huffcode[MAX_TAB][256]  = {{0}};
            /** dht lenght，YU table，12 is limit value */
            /** CNcomment:dht表的长度 */
            HI_U32 min_tab[MAX_TAB][16]    = {{0}};
            HI_U32 base_tab[MAX_TAB][16]   = {{0}};
            HI_U32 max_idx[MAX_TAB]        = {0};
    
            HI_U32 hac_min_tab[8]   = {0};
            HI_U32 hac_base_tab[8]   = {0};
            HI_U32 hac_symbol_tab[256]    = {0};
    
#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
            HI_U32 u32TmpHacMinTab[8]   = {0x0,0xa040a04,0x38183a1a,0xf678f878,0xf6f4f6f6,0xf4f6f4f6,0xe0f0e0f0,0x88c282c0};
            HI_U32 u32TmpHacBaseTab[8]    = {0x0,0xf9fef9fe,0xd1edcfec,0x1a951793,0x2520211c,0x302a2c26,0x48000000,0xa367a364};
            HI_U32 u32TmpHacSymblTab[256]     = 
                  {
                    0x1,      0x102,    0x203,    0x300,   0x1104,    0x411,   0x505,    0x2112,    
                    0x3121,   0x631,   0x1241,    0x4106,  0x5113,    0x751,   0x6161,    0x7107,   
                    0x1322,   0x2271,   0x3214,   0x8132,   0x881,    0x1491,  0x42a1,    0x9108,   
                    0xa123,   0xb142,   0xc1b1,   0x9c1,   0x2315,    0x3352,  0x52d1,    0xf0f0,   
                    0x1524,   0x6233,   0x7262,   0xd172,   0xa82,    0x1609,  0x240a,    0x3416,   
                    0xe117,   0x2518,   0xf119,   0x171a,   0x1825,   0x1926,   0x1a27,   0x2628,   
                    0x2729,   0x282a,   0x2934,   0x2a35,   0x3536,   0x3637,   0x3738,   0x3839,   
                    0x393a,   0x3a43,   0x4344,   0x4445,   0x4546,   0x4647,   0x4748,   0x4849,   
                    0x494a,   0x4a53,   0x5354,   0x5455,   0x5556,   0x5657,   0x5758,   0x5859,   
                    0x595a,   0x5a63,   0x6364,   0x6465,   0x6566,   0x6667,   0x6768,   0x6869,   
                    0x696a,   0x6a73,   0x7374,   0x7475,   0x7576,   0x7677,   0x7778,   0x7879,   
                    0x797a,   0x7a83,   0x8284,   0x8385,   0x8486,   0x8587,   0x8688,   0x8789,   
                    0x888a,   0x8992,   0x8a93,   0x9294,   0x9395,   0x9496,   0x9597,   0x9698,   
                    0x9799,   0x989a,   0x99a2,   0x9aa3,   0xa2a4,   0xa3a5,   0xa4a6,   0xa5a7,   
                    0xa6a8,   0xa7a9,   0xa8aa,   0xa9b2,   0xaab3,   0xb2b4,   0xb3b5,   0xb4b6,   
                    0xb5b7,   0xb6b8,   0xb7b9,   0xb8ba,   0xb9c2,   0xbac3,   0xc2c4,   0xc3c5,   
                    0xc4c6,   0xc5c7,   0xc6c8,   0xc7c9,   0xc8ca,   0xc9d2,   0xcad3,   0xd2d4,   
                    0xd3d5,   0xd4d6,   0xd5d7,   0xd6d8,   0xd7d9,   0xd8da,   0xd9e1,   0xdae2,   
                    0xe2e3,   0xe3e4,   0xe4e5,   0xe5e6,   0xe6e7,   0xe7e8,   0xe8e9,   0xe9ea,   
                    0xeaf1,   0xf2f2,   0xf3f3,   0xf4f4,   0xf5f5,   0xf6f6,   0xf7f7,   0xf8f8,   
                    0xf9f9,   0xfafa,   0x0,      0x0,      0x0,       0x0,     0x0,      0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   
                    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0
                  };
#endif
    
#if defined(CONFIG_JPEG_MPG_DEC_ENABLE) && !defined(CONFIG_JPEG_USE_CALC_DEFAULT_VALUE)
            JHUFF_TBL huff_ptr[MAX_TAB] = { { {0}, {0}, 0} };
    
#else
            JHUFF_TBL *huff_ptr[MAX_TAB];
#endif
    
      
            /** AC Decode Begin */
            /** CNcomment: 开始AC表解码 */
            /** Generate hac_min_table, hac_base_table **/
    //        if ((NULL == &dat[0]) || (NULL != cinfo->ac_huff_tbl_ptrs[2]))
    //        {
    //            pTmp+=1;
    //            memset(&(huff_ptr[LU].bits[0]),0,17);
    //            memcpy(&(huff_ptr[LU].bits[1]), pTmp, 16);
    //            pTmp   += 16;
    //            memset(&(huff_ptr[LU].huffval[0]),0,256);
    //            memcpy(&(huff_ptr[LU].huffval[0]), pTmp, 162);
    //            pTmp   += 162;
    //
    //            pTmp+=1;
    //            memset(&(huff_ptr[CH].bits[0]),0,17);
    //            memcpy(&(huff_ptr[CH].bits[1]), pTmp, 16);
    //            pTmp   += 16;
    //            memset(&(huff_ptr[CH].huffval[0]),0,256);
    //            memcpy(&(huff_ptr[CH].huffval[0]), pTmp, 162);
    //        }
    //        else
            {
                /** 亮度AC系数表 **/
                memset(&(huff_ptr[LU].bits[0]),0,17);
                memset(&(huff_ptr[LU].huffval[0]),0,256);
                memset(&(huff_ptr[CH].bits[0]),0,17);
                memset(&(huff_ptr[CH].huffval[0]),0,256);
                memcpy(&(huff_ptr[LU]), &dat[0], sizeof(huff_ptr[LU]));
                /** 色度AC系数表 **/
                if(NULL == &dat[1])
                {
                    memcpy(&(huff_ptr[CH]),&dat[0],sizeof(huff_ptr[LU]));
                }
                else
                {
                    memcpy(&(huff_ptr[CH]),&dat[1],sizeof(huff_ptr[LU]));
                }
            }

            max_idx[LU] = JPEG_HDEC_DecHuff(&huff_ptr[0],huffcode[LU]);
            max_idx[CH] = JPEG_HDEC_DecHuff(&huff_ptr[1],huffcode[CH]);
        
            /** Luminance and Chrominance: LU=0,CH=1;**/
            for(cnt = 0; cnt < MAX_TAB; cnt++)
            {
                sum_syms=0;
                for(u32Index = 0; u32Index < 16; u32Index++)
                {
                    /** the ibit[] has sixteen value,read from jpeg file */
                    /** CNcomment: ibit[]有16个，从jpeg文件读到16个值 */
	                 syms = huff_ptr[cnt].bits[u32Index+1];
                    pre_index = u32Index?(u32Index-1):(0);
            
                    if(u32Index < max_idx[cnt])
                    {
    
				        min_tab[cnt][u32Index] = (min_tab[cnt][pre_index]+huff_ptr[cnt].bits[u32Index])<<1;

                        if(syms)
                        {
                            base_tab[cnt][u32Index]=sum_syms-min_tab[cnt][u32Index];
                        }

                        sum_syms += huff_ptr[cnt].bits[u32Index+1];
            
                    }
                    else
                    {
                        min_tab[cnt][u32Index]=~0;
                    }
                    
                }
            
                if(0 == cnt)
                {/** Create LU symbol table **/
                    for(u32Index=0;u32Index<sum_syms;u32Index++)
                    {
			              hac_symbol_tab[u32Index] |= ( (unsigned int)(huff_ptr[0].huffval[u32Index]) );

                    }
                }
                else
                {/** Create CH symbol table **/
                    for(u32Index=0;u32Index<sum_syms;u32Index++)
                    {
			            hac_symbol_tab[u32Index] |= ( (unsigned int)(huff_ptr[1].huffval[u32Index])<<8 );

                    }
                }
                
            }
            
            /** Conbine Luminnance and Chrominance */
            /** CNcomment: 组合亮度和色度表 */
            for(u32Index=0;u32Index<8;u32Index++)
            {
            
                hac_min_tab[u32Index] =   ( (min_tab[CH][2*u32Index+1] & 0xff)<<24 )  \
                                     | ( (min_tab[CH][2*u32Index] & 0xff)<<16   )  \
                                     | ( (min_tab[LU][2*u32Index+1] & 0xff)<<8  )  \
                                     | (min_tab[LU][2*u32Index] & 0xff);
            
                hac_base_tab[u32Index] =   ((base_tab[CH][2*u32Index+1] & 0xff)<<24 ) \
                                      | ((base_tab[CH][2*u32Index] & 0xff)<<16   ) \
                                      | ((base_tab[LU][2*u32Index+1] & 0xff)<<8  ) \
                                      | (base_tab[LU][2*u32Index] & 0xff);
                
            }
            
            /** Write hac_min/base/symbol_table into  HW reisters */
            /** CNcomment: 将hac_min/base值配给硬件寄存器 */
            JPEG_HDEC_CpyData2Reg((HI_CHAR *)JPGD_REG_BASEADDR, hac_min_tab,   JPGD_REG_HACMINTABLE,  32);
            JPEG_HDEC_CpyData2Reg((HI_CHAR *)JPGD_REG_BASEADDR, hac_base_tab,  JPGD_REG_HACBASETABLE, 32);
            /** 0x2c0 = ac_max_sum_syms * 4 */
            /** CNcomment: 值0x2c0 = ac_max_sum_syms * 4计算得来 */
            JPEG_HDEC_CpyData2Reg((HI_CHAR *)JPGD_REG_BASEADDR, hac_symbol_tab,JPGD_REG_HACSYMTABLE,  0x2c0);

}

/*****************************************************************************
* func			: JPEG_HDEC_SetDht
* description	: set the dht
              	  CNcomment:  设置哈夫曼表信息,这个地方标准表是固定的，
							  可以提前计算好，直接配置寄存器  CNend\n
* param[in] 	: cinfo       CNcomment:  解码对象  CNend\n
* retval		: NA
* others:		: NA
*****************************************************************************/
HI_VOID JPEG_HDEC_SetDht(const struct jpeg_decompress_struct *cinfo)
{
		JPEG_HDEC_SetDC(cinfo);
		JPEG_HDEC_SetAC(cinfo);
}
