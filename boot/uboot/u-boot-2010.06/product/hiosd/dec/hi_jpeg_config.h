/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : hi_jpeg_config.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : this file is through set macro to select different funciton,
                  and select different platform
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/

 

#ifndef __HI_JPEG_CONFIG_H__
#define __HI_JPEG_CONFIG_H__


/*****************************************************************************/


#ifdef __cplusplus
      #if __cplusplus
   
extern "C" 
{

      #endif
#endif /* __cplusplus */

    /***************************** Macro Definition ******************************/

	/** \addtogroup 	 JPEG CFG */
    /** @{ */  /** <!-- 【JPEG CFG】 */


    /****************************************************************************
     **宏开关都是在Makefile中定义
     ****************************************************************************/
	/** this macro is add google function and use in android version,define in Makefile */
	/** CNcomment:将android的google版本核入jpeg-6b中，在Makefile中控制该宏开关 CNend */
	#if 0
	#define CONFIG_JPEG_ADD_GOOGLEFUNCTION
	#endif

//是否硬解码
#define HARD_DEC
/****************************************************************************
** function marco definition no depend chip platform
** CNcomment:和芯片平台无关的宏定义
****************************************************************************/
     #ifdef CONFIG_HI_SDK_JPEG_VERSION
	     #ifdef CONFIG_GFX_ANDROID_SDK
			/** use gfx mmz to alloc mem */
			/** CNcomment:使用自己封装的MMZ分配内存 */
			#define CONFIG_JPEG_USE_PRIVATE_MMZ
		    /** use android debug message */
			/** CNcomment:Android版本的调试信息 */
			#define CONFIG_JPEG_ANDROID_DEBUG_ENABLE
		 #elif defined(CONFIG_GFX_BVT_SDK)
		 	/** CNcomment:使用自己封装的MMZ分配内存 */
			#define CONFIG_JPEG_USE_PRIVATE_MMZ
		 #else
			/** the some function realize before the main function */
			/** CNcomment:有些功能在main函数之前实现，也就是程序运行之后调用一次 */
			#define CONFIG_JPEG_REALIZEFUNC_BEFORMAINFUNC
	     #endif
	 #else
	 	/** the some function realize before the main function */
		/** CNcomment:有些功能在main函数之前实现，也就是程序运行之后调用一次 */
	    #define CONFIG_JPEG_REALIZEFUNC_BEFORMAINFUNC
		/** 加载库的时候就要实现，这样测试用例才可以正常使用 **/
	    #ifdef CONFIG_JPEG_USE_PRIVATE_MMZ
			#undef CONFIG_JPEG_USE_PRIVATE_MMZ
	    #endif
	 #endif
	 

	 /** fpga and eda and advca not use this file **/
	 #if defined(CONFIG_HI_SDK_JPEG_VERSION) && !defined(CONFIG_GFX_ADVCA_RELEASE)
	 /** if need proc message */
	 /** CNcomment:是否需要proc信息 */
     #define CONFIG_JPEG_PROC_ENABLE
	 #endif

	/** if need use soft csc to debug whether the hard csc has problem */
	/** CNcomment:使用软件转换来定位硬件转换是否有问题 */
    #define CONFIG_JPEG_SOFTCSC_ENABLE

	/** use hard idct method */
	/** CNcomment:使用硬件得IDCT算法 */
    #define CONFIG_JPEG_USEHWIDCT

	/** support motion jpeg decode */
	/** CNcomment:支持motion jpeg 解码 */
	#define CONFIG_JPEG_MPG_DEC_ENABLE

	/** use the calc default value dqt and dht value */
	/** CNcomment:使用计算好的量化表和哈夫曼表的值，直接配置寄存器 */
	#ifdef CONFIG_JPEG_MPG_DEC_ENABLE
		//#define CONFIG_JPEG_USE_CALC_DEFAULT_VALUE
	#endif

/****************************************************************************
 ** function marco definition depend chip platform
 ** CNcomment:和芯片平台相关的宏定义
 ****************************************************************************/
	/** if support crop */
	/** CNcomment:是否支持裁剪功能 */
	#define CONFIG_JPEG_OUTPUT_CROP
	/** if support suspend */
	/** CNcomment:是否支持待机功能 */
	#define CONFIG_JPEG_SUSPEND
	/** get jpeg dec time */
	/** CNcomment:获取jpeg解码的时间 */
	#define CONFIG_JPEG_GETDECTIME
	/** all jpeg dec output yuv420sp */
	/** CNcomment:统一解码输出yuv420sp,硬件和软件都不支持yuv400 -> yuv420
				　要是需要支持填充0x80 */
	#define CONFIG_JPEG_OUTPUT_YUV420SP
	/** dec jpeg file output lu pixel value sum */
	/** CNcomment:统计亮度值 */
	#define CONFIG_JPEG_OUTPUT_LUPIXSUM
	/** if support jpeg hard dec to argb8888 */
	/** CNcomment:是否支持jpeg硬件解码输出argb8888 */
	#define CONFIG_JPEG_HARDDEC2ARGB
	/** CNcomment:这里由于DDR无法处理跨4K内存的问题，所以JPGD逻辑
	 ** 要对跨4K内存进行处理，否则会有一段段小黑条
	 ** 0x0 - 0x1000 - 0x2000 - 0x3000 内存起始地址+大小要在区间内的问题 */
	/** use sdk CRG write */
	/** CNcomment:使用SDK的CRG操作 */	
	#define CONFIG_JPEG_USE_SDK_CRG_ENABLE

	/** HSCP201405300013 HSCP201405290010 DTS2014061006717**/
	/** CNcomment:hifone之后修改的按照采样因子来解码 **/
	#define CONFIG_JPEG_SET_SAMPLEFACTOR


	/****************************************************************************
     ** 底下根据不同芯片平台将不支持的功能宏开关打开
     ****************************************************************************/
	#ifdef CONFIG_GFX_BVT_SDK
		#undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_SUSPEND
		#undef CONFIG_JPEG_GETDECTIME
		#undef CONFIG_JPEG_HARDDEC2ARGB
		#undef CONFIG_JPEG_USE_SDK_CRG_ENABLE
		#undef CONFIG_JPEG_PROC_ENABLE
		#define CONFIG_JPEG_CSC_DISABLE
		/** 裁剪非yuvsp输出的代码 **/
		#define CONFIG_JPEG_CODE_CUT_RGB
	#endif

	#if   defined(HI_CHIP_TYPE_hi3531) || defined(HI_CHIP_TYPE_hi3532)   \
       || defined(HI_CHIP_TYPE_hi3521) || defined(HI_CHIP_TYPE_hi3520A)  \
       || defined(HI_CHIP_TYPE_hi3520D)
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
	#endif
	
	#if    defined(CHIP_TYPE_hi3716cv200)   \
        || defined(CHIP_TYPE_hi3716mv400)   \
        || defined(CHIP_TYPE_hi3718cv100)   \
        || defined(CHIP_TYPE_hi3718mv100)   \
        || defined(CHIP_TYPE_hi3719cv100)   \
        || defined(CHIP_TYPE_hi3719mv100)   \
        || defined(CHIP_TYPE_hi3719mv100_a)
        #undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
		#undef CONFIG_JPEG_HARDDEC2ARGB         /**CV200 EC之后已经修改**/
		/** the save stream buffer whether 4 byte align */
	    /** CNcomment:存储码流buffer是否需要4字节对齐,这个bug在3535/CV200(EC)之后的芯片解决了**/
	    #define CONFIG_JPEG_STREAMBUF_4ALIGN
	#endif

	#if defined(CHIP_TYPE_hi3712)       \
        || defined(CHIP_TYPE_hi3716c)    \
        || defined(CHIP_TYPE_hi3716m)    \
        || defined(CHIP_TYPE_hi3716h)
        #undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
		#undef CONFIG_JPEG_HARDDEC2ARGB
		#undef CONFIG_JPEG_USE_SDK_CRG_ENABLE
	#endif

	#ifdef CHIP_TYPE_hi3716mv310
        #undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
		#undef CONFIG_JPEG_HARDDEC2ARGB
	#endif

	#ifndef CHIP_TYPE_hifone
		#undef CONFIG_JPEG_SET_SAMPLEFACTOR
	#else
		#undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_HARDDEC2ARGB
	#endif

	#ifndef CHIP_TYPE_hi3716mv310
		/** 只支持mv310的开机动画，其它平台都用ts流做瞬播 **/
		#ifdef HI_MCE_SUPPORT
			#undef HI_MCE_SUPPORT
		#endif
	#endif
	/****************************************************************************
     ** function marco definition use to fpga test and deal with bug by eda
	 ** CNcomment:用来硬件逻辑测试及定位bug使用的宏开关
     ****************************************************************************/
	/** output the need output os memory size */
	/** CNcomment:输出OS需要的内存大小 */
	//#define CONFIG_JPEG_OUTPUT_NEED_OS_MEMSIZE
	
	/** test press */
	/** CNcomment:测试反压 */
	//#define CONFIG_JPEG_TEST_CHIP_PRESS

	/**
	 ** 默认支持跨4K，FPGA测试要测试到不支持跨4K得情况(会有异常)
	 ** 这里的跨4K只的是取地址的时候会有跨4K的情况。打开CBB不支持
	 ** 跨4K
	 **/
	//#define CONFIG_JPEG_4KDDR_DISABLE
    //#define CONFIG_JPEG_4KDDR_DISABLE_SHOWMSG /** 显示信息 **/
	
	/** save the scen information,use it to eda simulation */
	/** CNcomment:导现场使能，用来调试用的，现场给逻辑做EDA仿真 */
	//#define CONFIG_JPEG_FPGA_TEST_SAVE_SCEN_ENABLE
	/** save the data to bmp picture */
	/** CNcomment:保存bmp图片 */
    //#define CONFIG_JPEG_TEST_SAVE_BMP_PIC
    /** save yuv semi-planer data */
	/** CNcomment:保存yuv semi-planer数据 */
    //#define CONFIG_JPEG_TEST_SAVE_YUVSP_DATA
	/** the chip hard decode random reset test */
	/** CNcomment:随机软复位样片测试 */
	//#define CONFIG_JPEG_TEST_CHIP_RANDOM_RESET
	/** run decode reset test */
	/** CNcomment:跑正常解码用例的软复位 */
	//#define CONFIG_JPEG_TEST_ALL_DEC_RANDOM_RESET
	/** the ck_gt_en is open */
	/** CNcomment:时钟门控打开 */
	//#define CONFIG_JPEG_TEST_CHIP_OPEN_CK
	/** test hard decode capa */
	/** CNcomment:测试硬件解码性能 */
	//#define CONFIG_JPEG_TEST_HARD_DEC_CAPA
	/** save the scen information,use it to eda simulation */
	/** CNcomment:测试待机增加的几个接口 */
	//#define CONFIG_JPEG_FPGA_TEST_SUSPEND_ENABLE
	/** test the continue stream ddr change */
	/** CNcomment:测试续码流地址发生变化的情况 */
	//#define CONFIG_JPEG_FPGA_TEST_CONTINUE_STREAM_DDR_CHANGE
	/** test set the different outstanding value */
	/** CNcomment:测试配置不同的outstanding值,自动比较宏开关打开 */
	//#define CONFIG_JPEG_FPGA_TEST_SET_DIFFERENT_OUTSTANDING_VALUE


	#ifdef CONFIG_HI_FPGA_JPEG_VERSION
		#define CONFIG_JPEG_FPGA_TEST_ENABLE
		#define CONFIG_JPEG_CSC_DISABLE
		#undef CONFIG_JPEG_USE_SDK_CRG_ENABLE
	#endif

	
    #ifdef CONFIG_GFX_ADVCA_RELEASE
	    /** this is not has warning message */
	    /** CNcomment:只有带上参数才不会有告警信息 */
        #define JPEG_TRACE( fmt,args...)
	#else
	    #ifdef __KERNEL__
		#define JPEG_TRACE               GFX_Printk
		#else
	 	#define JPEG_TRACE               GFX_Printf
		#endif
	#endif

    /** @} */  /*! <!-- Macro Definition end */

	
    /*************************** Structure Definition ****************************/
	
    /***************************  The enum of Jpeg image format  ******************/


    /********************** Global Variable declaration **************************/
    

    /******************************* API declaration *****************************/
	
	
    #ifdef __cplusplus

        #if __cplusplus



}
      
        #endif
        
   #endif /* __cplusplus */

#endif /* __HI_JPEG_CONFIG_H__*/
