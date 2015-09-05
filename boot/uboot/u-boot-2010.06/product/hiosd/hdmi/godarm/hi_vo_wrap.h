/******************************************************************************
  Copyright (C), 2001-2021, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : vo_wrap.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2010/02/12
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/
#ifndef __VO_WRAP_H__
#define __VO_WRAP_H__


#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

#define IRQ_HANDLED 0

#define printk      printf
#define HI_ERR_DISP printf
#define HI_PRINT    printf

#ifndef MAX_DAC_NUM
#define MAX_DAC_NUM     6
#endif

/**定义视频制式枚举*/
typedef enum hiHDMI_VIDEO_FMT_E
{
    HI_HDMI_VIDEO_FMT_1080P_60 = 0,     /**<1080p60赫兹*/
    HI_HDMI_VIDEO_FMT_1080P_50,         /**<1080p50赫兹*/
    HI_HDMI_VIDEO_FMT_1080P_30,         /**<1080p30赫兹*/
    HI_HDMI_VIDEO_FMT_1080P_25,         /**<1080p25赫兹*/
    HI_HDMI_VIDEO_FMT_1080P_24,         /**<1080p24赫兹*/

    HI_HDMI_VIDEO_FMT_1080i_60,         /**<1080i60赫兹 */
    HI_HDMI_VIDEO_FMT_1080i_50,         /**<1080i60赫兹 */

    HI_HDMI_VIDEO_FMT_720P_60,          /**<720p60赫兹 */
    HI_HDMI_VIDEO_FMT_720P_50,          /**<720p50赫兹 */

    HI_HDMI_VIDEO_FMT_576P_50,          /**<576p50赫兹 */
    HI_HDMI_VIDEO_FMT_480P_60,          /**<480p60赫兹 */

    HI_HDMI_VIDEO_FMT_PAL,              /* B D G H I PAL */
    HI_HDMI_VIDEO_FMT_PAL_N,            /* (N)PAL        */
    HI_HDMI_VIDEO_FMT_PAL_Nc,           /* (Nc)PAL       */

    HI_HDMI_VIDEO_FMT_NTSC,             /* (M)NTSC       */
    HI_HDMI_VIDEO_FMT_NTSC_J,           /* NTSC-J        */
    HI_HDMI_VIDEO_FMT_NTSC_PAL_M,       /* (M)PAL        */

    HI_HDMI_VIDEO_FMT_SECAM_SIN,        /**< SECAM_SIN*/
    HI_HDMI_VIDEO_FMT_SECAM_COS,        /**< SECAM_COS*/

    HI_HDMI_VIDEO_FMT_861D_640X480_60,
    HI_HDMI_VIDEO_FMT_VESA_800X600_60,
    HI_HDMI_VIDEO_FMT_VESA_1024X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X720_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X800_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X1024_60,
    HI_HDMI_VIDEO_FMT_VESA_1366X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1080_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_2048X1152_60,
    HI_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE,        // for lvds timing

    HI_HDMI_VIDEO_FMT_BUTT
}HI_HDMI_VIDEO_FMT_E;

/**定义宽高比转换方式枚举*/
typedef enum hiHDMI_ASPECT_CVRS_E
{
    HI_HDMI_ASPECT_CVRS_IGNORE = 0x0,   /**< 忽略*/
    HI_HDMI_ASPECT_CVRS_LETTERBOX,      /**< 加黑边*/
    HI_HDMI_ASPECT_CVRS_PANANDSCAN,     /**< 裁减*/
    HI_HDMI_ASPECT_CVRS_COMBINED,       /**< 加黑边和裁减混合*/
    HI_HDMI_ASPECT_CVRS_BUTT
} HI_HDMI_ASPECT_CVRS_E;

typedef enum hiHDMI_DISP_MACROVISION_MODE_E
{
    HI_HDMI_DISP_MACROVISION_MODE_TYPE0,  /**<典型配置0 */
    HI_HDMI_DISP_MACROVISION_MODE_TYPE1,  /**<典型配置1 */
    HI_HDMI_DISP_MACROVISION_MODE_TYPE2,  /**<典型配置2 */
    HI_HDMI_DISP_MACROVISION_MODE_TYPE3,  /**<典型配置3 */
    HI_HDMI_DISP_MACROVISION_MODE_CUSTOM, /**<用户自定义配置 */
    HI_HDMI_DISP_MACROVISION_MODE_BUTT
} HI_HDMI_DISP_MACROVISION_MODE_E;

/* CGMS type select */
typedef enum hiHDMI_DISP_CGMS_TYPE_E
{
    HI_HDMI_DISP_CGMS_TYPE_A = 0x00,
    HI_HDMI_DISP_CGMS_TYPE_B,

    HI_HDMI_DISP_CGMS_TYPE_BUTT
}HI_HDMI_DISP_CGMS_TYPE_E;


/* definition of CGMS mode */
typedef enum hiHDMI_DISP_CGMS_MODE_E
{
   HI_HDMI_DISP_CGMS_MODE_COPY_FREELY    = 0,    /* copying is permitted without restriction */
   HI_HDMI_DISP_CGMS_MODE_COPY_NO_MORE   = 0x01, /* No more copies are allowed (one generation copy has been made) */
   HI_HDMI_DISP_CGMS_MODE_COPY_ONCE      = 0x02, /* One generation of copies may be made */
   HI_HDMI_DISP_CGMS_MODE_COPY_NEVER     = 0x03, /* No copying is permitted */
      
   HI_HDMI_DISP_CGMS_MODE_BUTT
}HI_HDMI_DISP_CGMS_MODE_E;


/** definition of CGMS configuration */
typedef struct hiHDMI_DISP_CGMS_CFG_S
{
    HI_BOOL                  bEnable;    /** HI_TRUE:CGMS is enabled; HI_FALSE:CGMS is disabled */
    HI_HDMI_DISP_CGMS_TYPE_E  enType;     /** type-A or type-B or None(BUTT) */
    HI_HDMI_DISP_CGMS_MODE_E  enMode;     /** CGMS mode. */
     
}HI_HDMI_DISP_CGMS_CFG_S;


typedef enum hiHDMI_DISP_DAC_MODE_E
{
    HI_HDMI_DISP_DAC_MODE_SILENCE = 0,    /**<无输出*/
    HI_HDMI_DISP_DAC_MODE_CVBS,           /**<CVBS输出*/
    HI_HDMI_DISP_DAC_MODE_Y,              /**<Y亮度分量输出*/
    HI_HDMI_DISP_DAC_MODE_PB,             /**<蓝色色差分量输出*/
    HI_HDMI_DISP_DAC_MODE_PR,             /**<红色色差分量输出*/
    HI_HDMI_DISP_DAC_MODE_R,              /**<红色分量输出*/
    HI_HDMI_DISP_DAC_MODE_G,              /**<绿色分量输出*/
    HI_HDMI_DISP_DAC_MODE_B,              /**<蓝色分量输出*/
    HI_HDMI_DISP_DAC_MODE_SVIDEO_Y,       /**<SVIDEO_Y分量输出*/
    HI_HDMI_DISP_DAC_MODE_SVIDEO_C,       /**<SVIDEO_C分量输出*/
    HI_HDMI_DISP_DAC_MODE_HD_Y,           /**<高清Y亮度分量输出*/
    HI_HDMI_DISP_DAC_MODE_HD_PB,          /**<高清蓝色色差分量输出*/
    HI_HDMI_DISP_DAC_MODE_HD_PR,          /**<高清红色色差分量输出*/
    HI_HDMI_DISP_DAC_MODE_HD_R,           /**<高清红色分量输出*/
    HI_HDMI_DISP_DAC_MODE_HD_G,           /**<高清绿色分量输出*/
    HI_HDMI_DISP_DAC_MODE_HD_B,           /**<高清蓝色分量输出*/
    HI_HDMI_DISP_DAC_MODE_BUTT
}HI_HDMI_DISP_DAC_MODE_E;

/**定义支持的显示输出接口 */
typedef struct  hiHDMI_DISP_INTERFACE_S
{
    HI_BOOL                      bScartEnable;                /**<是否输出Scart信号*/
    HI_BOOL                      bBt1120Enable;               /**<是否输出Bt1120数字信号*/
    HI_BOOL                      bBt656Enable;                /**<是否输出Bt656数字信号*/
    HI_HDMI_DISP_DAC_MODE_E       enDacMode[MAX_DAC_NUM];      /**<各个DAC的输出信号*/
}HI_HDMI_DISP_INTERFACE_S ;

typedef enum hiHDMI_DISP_INTF_TYPE_E
{
    HI_HDMI_DISP_INTF_TYPE_TV = 0,    /**<输出接口为TV*/
    HI_HDMI_DISP_INTF_TYPE_LCD,       /**<输出接口为LCD*/
    HI_HDMI_DISP_INTF_TYPE_BUTT
}HI_HDMI_DISP_INTF_TYPE_E;

/**定义LCD输出的数据位宽*/
typedef enum hiHDMI_DISP_LCD_DATA_WIDTH_E
{
    HI_HDMI_DISP_LCD_DATA_WIDTH8 = 0,       /**<8位*/
    HI_HDMI_DISP_LCD_DATA_WIDTH16,          /**<16位*/
    HI_HDMI_DISP_LCD_DATA_WIDTH24,          /**<24位*/
    HI_HDMI_DISP_LCD_DATA_WIDTH_BUTT
}HI_HDMI_DISP_LCD_DATA_WIDTH_E;


/**定义LCD的数据格式*/
typedef enum hiHDMI_DISP_LCD_FORMAT_E
{
    HI_HDMI_DISP_LCD_FORMAT_YUV422 = 0,         /**<YUV422，位宽为16*/
    HI_HDMI_DISP_LCD_FORMAT_RGB565 = 0x8,       /**<RGB565，位宽为16*/
    HI_HDMI_DISP_LCD_FORMAT_RGB444 = 0xa,       /**<RGB444，位宽为16*/
    HI_HDMI_DISP_LCD_FORMAT_RGB666 = 0xb,       /**<RGB666，位宽为24*/
    HI_HDMI_DISP_LCD_FORMAT_RGB888 = 0xc,       /**<RGB888，位宽为24*/
    HI_HDMI_DISP_LCD_FORMAT_BUTT
}HI_HDMI_DISP_LCD_FORMAT_E;

/**定义LCD的数据格式*/
typedef enum hiHI_HDMI_DISP_LCD_DATA_FMT
{
    HI_HDMI_DISP_LCD_DATA_FMT_YUV422 = 0,         /**<YUV422，位宽为16*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB565 = 0x8,       /**<RGB565，位宽为16*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB444 = 0xa,       /**<RGB444，位宽为16*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB666 = 0xb,       /**<RGB666，位宽为24*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB888 = 0xc,       /**<RGB888，位宽为24*/
    HI_HDMI_DISP_LCD_DATA_FMT_BUTT
}HI_HDMI_DISP_LCD_DATA_FMT_E;


/**定义LCD的时序参数*/
typedef struct hiHDMI_DISP_LCD_PARA_S
{
    HI_U32                    VFB;                 /**<垂直前消隐*/
    HI_U32                    VBB;                 /**<垂直后消隐*/
    HI_U32                    VACT;                /**<垂直有效区*/
    HI_U32                    HFB;                 /**<水平前消隐*/
    HI_U32                    HBB;                 /**<水平后消隐*/
    HI_U32                    HACT;                /**<水平有效区*/
    HI_U32                    VPW;                 /**<垂直脉冲宽度*/
    HI_U32                    HPW;                 /**<水平脉冲宽度*/
    HI_BOOL                   IDV;                 /**<有效数据信号是否翻转*/
    HI_BOOL                   IHS;                 /**<水平同步脉冲信号是否翻转*/
    HI_BOOL                   IVS;                 /**<垂直同步脉冲信号是否翻转*/
    HI_BOOL                   ClockReversal;       /**<时钟是否翻转*/
    HI_HDMI_DISP_LCD_DATA_WIDTH_E   DataWidth;      /**<数据位宽*/
    HI_HDMI_DISP_LCD_FORMAT_E       ItfFormat;      /**<数据格式.*/
    HI_BOOL                   DitherEnable;        /**<是否使能Dither*/
    HI_U32                    ClkPara0;            /**<PLL  register SC_VPLL1FREQCTRL0  value */
    HI_U32                    ClkPara1;            /**<PLL  register SC_VPLL1FREQCTRL1 value*/
    HI_U32                    InRectWidth;
    HI_U32                    InRectHeight;
} HI_HDMI_DISP_LCD_PARA_S;



/**定义显示颜色的结构体 */
typedef struct  hiHDMI_DISP_BG_COLOR_S
{
    HI_U8 u8Red ;            /**<红色分量*/
    HI_U8 u8Green ;          /**<绿色分量*/
    HI_U8 u8Blue ;           /**<蓝色分量*/
} HI_HDMI_DISP_BG_COLOR_S;


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /* __VO_WRAP_H__ */
