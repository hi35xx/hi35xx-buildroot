/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_def.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/06/28
  Description   :
  History       :
  1.Date        : 2011/06/28
    Author      : n00168968/z00185248/l00181524
    Modification: Created file

******************************************************************************/
#ifndef __VOU_DEF_H__
#define __VOU_DEF_H__

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define   VO_BASE_ADDR  0x205c0000

#define VO_VHD_BASE_ADDR   0x800
#define VO_GFX_BASE_ADDR   0x6000
#define VO_HC_BASE_ADDR    0x9800
#define VO_WBC0_BASE_ADDR  0xac00
#define VO_WBC1_BASE_ADDR  0xa400
#define VO_DHD_BASE_ADDR   0xc000
#define VO_DSD_BASE_ADDR   0xc800
#define VHD_REG_LEN        0x800
#define VSD_REG_LEN        0x800
#define GFX_REG_LEN        0x800
#define HC_REG_LEN         0x800
#define WBC_REG_LEN        0x400
#define DHD_REG_LEN        0x400
#define DSD_REG_LEN        0x400
#define INTF_REG_LEN       0x100


//for driver macro define,refer to enum
#define CHANNEL_DHD_START  0 //DHD0
#define CHANNEL_DHD_END    1 //DHD0
#define CHANNEL_DSD_START  2 //DSD0
#define CHANNEL_DSD_END    2 //DSD1



//for multi area
#define MULTI_AREA_CFG_LEN 4*8

/* 输出通道号*/
typedef enum tagHAL_DISP_OUTPUTCHANNEL_E
{
    HAL_DISP_CHANNEL_DHD0 = 0,       
    HAL_DISP_CHANNEL_DHD1 = 1,       
    HAL_DISP_CHANNEL_DSD0 = 2,       
    //HAL_DISP_CHANNEL_DSD1 = 2,       
    //HAL_DISP_CHANNEL_DSD2 = 4,       
    //HAL_DISP_CHANNEL_DSD3 = 5,       
    //HAL_DISP_CHANNEL_DSD4 = 6,       
    //HAL_DISP_CHANNEL_DSD5 = 7,       
    //HAL_DISP_CHANNEL_WBC2 = 8,   //没用到
    //HAL_DISP_CHANNEL_NONE = 9,
    HAL_DISP_CHANNEL_BUTT
} HAL_DISP_OUTPUTCHANNEL_E;


typedef enum tagHAL_DISP_LAYER_E
{
    HAL_DISP_LAYER_VHD0 = 0 ,
    HAL_DISP_LAYER_VHD1 = 1 ,
    //HAL_DISP_LAYER_VHD2 = 2 ,    
    HAL_DISP_LAYER_VP   = 2,
    HAL_DISP_LAYER_VSD0 = 3 ,
    //HAL_DISP_LAYER_VSD1 = 2 ,
    //HAL_DISP_LAYER_VSD2 = 5 ,
    //HAL_DISP_LAYER_VSD3 = 6 ,
    //HAL_DISP_LAYER_VSD4 = 7 ,
    HAL_DISP_LAYER_GFX0 = 4 ,    /* fb0 */  
    HAL_DISP_LAYER_GFX1 = 5 ,    /* fb2 */ 
    HAL_DISP_LAYER_HC0  = 6,     /* G2 is HC0  --- fb3 */  
    HAL_DISP_LAYER_GFX4 = 7,     /* fb1 and no G3 */ 
    HAL_DISP_LAYER_WBC_G0 = 8,
    HAL_DISP_LAYER_WBC_G4 = 9,
    HAL_DISP_LAYER_WBC_D = 10,
    HAL_DISP_LAYER_TT   = 11,

    HAL_DISP_LAYER_BUTT
} HAL_DISP_LAYER_E;

#define HAL_DISP_LAYER_GFX2  HAL_DISP_LAYER_HC0


#define LAYER_VHD_START  HAL_DISP_LAYER_VHD0  //VHD0
#define LAYER_VHD_END    HAL_DISP_LAYER_VHD1  //VHD1
#define LAYER_VSD_START  HAL_DISP_LAYER_VSD0  //VSD0
#define LAYER_VSD_END    HAL_DISP_LAYER_VSD0  //VSD0
#define LAYER_GFX_START  HAL_DISP_LAYER_GFX0  //GFX0
#define LAYER_GFX_END    HAL_DISP_LAYER_GFX4  //GFX4
#define LAYER_WBC_START  HAL_DISP_LAYER_WBC_D //WBC
#define LAYER_WBC_END    HAL_DISP_LAYER_WBC_D //WBC


/*视频数据读取模式*/
typedef enum tagHAL_DISP_DATARMODE_E
{
    HAL_DISP_INTERLACE = 0,
    HAL_DISP_PROGRESSIVE = 1,
    HAL_DISP_TOP,
    HAL_DISP_BOTTOM,
    
    HAL_DISP_DATARMODE_BUTT
} HAL_DISP_DATARMODE_E;


/*视频帧图像格式*/
typedef enum tagHAL_DISP_PIXEL_FORMAT_E
{
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_400     = 0x1,
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_411_1X4 = 0x2,
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_420     = 0x3,
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_422     = 0x4,
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_422_1X2 = 0x4,
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_444     = 0x5,   
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_411_4X1 = 0x6,
    HAL_INPUTFMT_YCbCr_SEMIPLANAR_422_2X1 = 0x7,

    HAL_INPUTFMT_CbYCrY_PACKAGE_422       = 0x9,    
    HAL_INPUTFMT_YCbYCr_PACKAGE_422       = 0xa,    
    HAL_INPUTFMT_YCrYCb_PACKAGE_422       = 0xb,    
    HAL_INPUTFMT_YCbCr_PACKAGE_444        = 0x1000,    

    HAL_INPUTFMT_CLUT_1BPP                = 0x00,
    HAL_INPUTFMT_CLUT_2BPP                = 0x10,
    HAL_INPUTFMT_CLUT_4BPP                = 0x20,
    HAL_INPUTFMT_CLUT_8BPP                = 0x30,
    HAL_INPUTFMT_ACLUT_44                 = 0x38,    

    HAL_INPUTFMT_RGB_444                  = 0x40,
    HAL_INPUTFMT_RGB_555                  = 0x41,
    HAL_INPUTFMT_RGB_565                  = 0x42,
    HAL_INPUTFMT_CbYCrY_PACKAGE_422_GRC   = 0x43,
    HAL_INPUTFMT_YCbYCr_PACKAGE_422_GRC   = 0x44,
    HAL_INPUTFMT_YCrYCb_PACKAGE_422_GRC   = 0x45,
    HAL_INPUTFMT_ACLUT_88                 = 0x46,
    HAL_INPUTFMT_ARGB_4444                = 0x48,
    HAL_INPUTFMT_ARGB_1555                = 0x49,

    HAL_INPUTFMT_RGB_888                  = 0x50,
    HAL_INPUTFMT_YCbCr_888                = 0x51,
    HAL_INPUTFMT_ARGB_8565                = 0x5a,
    HAL_INPUTFMT_ARGB_6666                = 0x5b,

    HAL_INPUTFMT_KRGB_888                 = 0x60,
    HAL_INPUTFMT_ARGB_8888                = 0x68,
    HAL_INPUTFMT_AYCbCr_8888              = 0x69,

    HAL_INPUTFMT_RGBA_4444                = 0xc8,
    HAL_INPUTFMT_RGBA_5551                = 0xc9,

    HAL_INPUTFMT_RGBA_6666                = 0xd8,
    HAL_INPUTFMT_RGBA_5658                = 0xda,

    HAL_INPUTFMT_RGBA_8888                = 0xe8,
    HAL_INPUTFMT_YCbCrA_8888              = 0xe9,

    HAL_DISP_PIXELFORMAT_BUTT        
    
}HAL_DISP_PIXEL_FORMAT_E;

/*接口输出数据格式*/
typedef enum tagHAL_DISP_INTFDATAFMT_E
{
    // WBC2 output data format.    
    
    HAL_DISP_INTFDATAFMT_SPCBYCRY_422   = 0x0,
    HAL_DISP_INTFDATAFMT_SPYCBYCR_422   = 0x1,
    HAL_DISP_INTFDATAFMT_SPYCRYCB_422   = 0x2,
    
    HAL_DISP_INTFDATAFMT_ARGB8888   = 0x3,
    HAL_DISP_INTFDATAFMT_YCBCR420   = 0x4,    
    HAL_DISP_INTFDATAFMT_YCBCR422   = 0x5,
    
    HAL_DISP_INTFDATAFMT_BUTT          
} HAL_DISP_INTFDATAFMT_E;


//色彩空间
typedef enum hiHAL_CS_E
{
    HAL_CS_UNKNOWN = 0,
    HAL_CS_BT601,
    HAL_CS_BT709,
    HAL_CS_RGB,
    HAL_CS_BUTT
} HAL_CS_E;

//色彩空间转换模式
typedef enum hiHAL_CSC_MODE_E
{
    HAL_CSC_MODE_NONE = 0,

    HAL_CSC_MODE_BT601_TO_BT601,
    HAL_CSC_MODE_BT709_TO_BT709,
    HAL_CSC_MODE_RGB_TO_RGB,
    
    HAL_CSC_MODE_BT601_TO_BT709,
    HAL_CSC_MODE_BT709_TO_BT601,
    
    HAL_CSC_MODE_BT601_TO_RGB_PC,   
    HAL_CSC_MODE_BT709_TO_RGB_PC,
    HAL_CSC_MODE_RGB_TO_BT601_PC,
    HAL_CSC_MODE_RGB_TO_BT709_PC,
    
    HAL_CSC_MODE_BT601_TO_RGB_TV, 
    HAL_CSC_MODE_BT709_TO_RGB_TV,
    HAL_CSC_MODE_RGB_TO_BT601_TV,
    HAL_CSC_MODE_RGB_TO_BT709_TV,
    
    HAL_CSC_MODE_BUTT
} HAL_CSC_MODE_E;

typedef enum hiHAL_DISP_BIT_WIDTH_E
{
    HAL_DISP_BIT_WIDTH_1 = 0x0,
    HAL_DISP_BIT_WIDTH_2 = 0x1,
    HAL_DISP_BIT_WIDTH_3 = 0x2,
    HAL_DISP_BIT_WIDTH_BUTT
} HAL_DISP_BIT_WIDTH_E;

typedef enum tagHAL_CCD_IMGID_E
{
    HAL_CCD_IMGID_INVALID, 
    HAL_CCD_IMGID_1080P60, 
    HAL_CCD_IMGID_1080P30_ODD, 
    HAL_CCD_IMGID_1080P30_EVEN, 
    
}HAL_CCD_IMGID_E;

/*视频数据读取模式*/
typedef enum tagHAL_VHD_FOD_E
{
    HAL_T_FIRST = 0,
    HAL_B_FIRST = 1,
    
    HAL_VHD_FOD_BUTT
} HAL_VHD_FOD_E;

/* vou coef load mode */
typedef enum 
{
    HAL_DISP_COEFMODE_HORL  = 0,        
    HAL_DISP_COEFMODE_HORC,        
    HAL_DISP_COEFMODE_VERL,
    HAL_DISP_COEFMODE_VERC,
    HAL_DISP_COEFMODE_LUT,
    HAL_DISP_COEFMODE_GAM,
    HAL_DISP_COEFMODE_ACC,
    HAL_DISP_COEFMODE_ALL
}HAL_DISP_COEFMODE_E;


/* vou zoom mode */
typedef enum 
{
    HAL_DISP_ZMEMODE_HORL = 0,  
    HAL_DISP_ZMEMODE_HORC,  
    HAL_DISP_ZMEMODE_VERL,
    HAL_DISP_ZMEMODE_VERC,
    
    HAL_DISP_ZMEMODE_HOR,
    HAL_DISP_ZMEMODE_VER,
    HAL_DISP_ZMEMODE_ALPHA,
    HAL_DISP_ZMEMODE_ALPHAV,
    HAL_DISP_ZMEMODE_VERT,
    HAL_DISP_ZMEMODE_VERB,
    
    HAL_DISP_ZMEMODE_ALL,
    HAL_DISP_ZMEMODE_NONL,
    HAL_DISP_ZMEMODE_BUTT
      
}HAL_DISP_ZMEMODE_E;

/* vou CBM MIXER */
typedef enum 
{
    HAL_CBMMIX1 = 0,  
    HAL_CBMMIX2    ,
    HAL_CBMMIX3    ,
    
    HAL_CBMMIX1_BUTT
}HAL_CBMMIX_E;

/* vou graphic layer data extend mode */
typedef enum 
{
    HAL_GFX_BITEXTEND_1ST =   0,  
    HAL_GFX_BITEXTEND_2ND = 0x2,
    HAL_GFX_BITEXTEND_3RD = 0x3,
    
    HAL_GFX_BITEXTEND_BUTT
}HAL_GFX_BITEXTEND_E;

/* vou acm block id information */
typedef enum tagHAL_ACMBLK_ID_E
{
    HAL_ACMBLK_ID0,
    HAL_ACMBLK_ID1,
    HAL_ACMBLK_ID2,
    HAL_ACMBLK_ID3,
    
    HAL_ACMBLK_ALL,

    HAL_ACMBLK_BUTT
    
}HAL_ACMBLK_ID_E;

// vou dac1 sel    
typedef enum 
{
    HAL_DAC_SEL_HDDATE = 0x0,  
    HAL_DAC_SEL_VGA    = 0x01, 
}HAL_DAC_SEL_E;

typedef enum hiHAL_DISP_SYNC_MODE_E
{
    HAL_DISP_SYNC_MODE_TIMING = 0x0,
    HAL_DISP_SYNC_MODE_SIGNAL = 0x1,
    HAL_DISP_SYNC_MODE_BUTT
} HAL_DISP_SYNC_MODE_E;

//for gfx decompress
typedef enum tagHAL_GFX_LINETYPE_E
{
    HAL_GFX_LINE_TYPE_HYBRID, 
    HAL_GFX_LINE_TYPE_VLC, 
    HAL_GFX_LINE_TYPE_FLC, 
                    
    HAL_GFX_LINE_TYPE_BUTT, 
}HAL_GFX_LINETYPE_E;

typedef enum tagHAL_GFX_LINELEN_E
{
    HAL_GFX_LINE_LEN_128, 
    HAL_GFX_LINE_LEN_256, 
    HAL_GFX_LINE_LEN_512, 
    HAL_GFX_LINE_LEN_ALLLINE, 
    
    HAL_GFX_LINE_LEN_BUTT, 
}HAL_GFX_LINELEN_E;

/* RM frame or filed infomation */
typedef enum tagHAL_IFIRMODE_E
{
    HAL_IFIRMODE_DISEN = 0,
    HAL_IFIRMODE_COPY,
    HAL_IFIRMODE_DOUBLE,
    HAL_IFIRMODE_6TAPFIR,

    HAL_IFIRMODE_BUTT
}HAL_IFIRMODE_E;

/* vou mixer prio id */
typedef enum tagHAL_DISP_MIX_PRIO_E
{
    HAL_DISP_MIX_PRIO0 = 0,
    HAL_DISP_MIX_PRIO1,
    HAL_DISP_MIX_PRIO2,
    HAL_DISP_MIX_PRIO3,
    //HAL_DISP_MIX_PRIO4,
    //HAL_DISP_MIX_PRIO5,
    //HAL_DISP_MIX_PRIO6,
    HAL_DISP_MIX_BUTT 
}HAL_DISP_MIX_PRIO_E;


/* vou layer select ddr id */
typedef enum tagHAL_LAYER_DDR_SEL_E
{
    HAL_LAYER_DDR0 = 0,
    //HAL_LAYER_DDR1 = 1,
    HAL_LAYER_DDR_BUTT
}HAL_LAYER_DDR_SEL_E;

/* LTI/CTI mode */
typedef enum tagHAL_DISP_TIMODE_E
{
    HAL_DISP_TIMODE_LUM = 0,
    HAL_DISP_TIMODE_CHM,

    HAL_DISP_TIMODE_ALL,
    HAL_DISP_TIMODE_NON,
    HAL_DISP_TIMODE_BUTT,
} HAL_DISP_TIMODE_E;

/*视频帧同步信息*/
typedef struct tagHAL_DISP_SYNCINFO_S
{
    HI_U32  bSynm;
    HI_U32  bIop;
    HI_U8    u8Intfb; 
    
    HI_U16    u16Vact ;
    HI_U16    u16Vbb;
    HI_U16    u16Vfb;

    HI_U16    u16Hact;
    HI_U16    u16Hbb;
    HI_U16    u16Hfb;
    HI_U16    u16Hmid;

    HI_U16    u16Bvact;
    HI_U16    u16Bvbb;
    HI_U16    u16Bvfb;

    HI_U16    u16Hpw;
    HI_U16    u16Vpw;

    HI_U32  bIdv;
    HI_U32  bIhs;
    HI_U32  bIvs;
} HAL_DISP_SYNCINFO_S;

typedef enum tagHAL_DISP_INTF_E
{
    
    HAL_DISP_INTF_CVBS   = (0x01L<<0),        
    HAL_DISP_INTF_HDDATE = (0x01L<<1),    
    HAL_DISP_INTF_VGA    = (0x01L<<2),    
    HAL_DISP_INTF_BT656 = (0x01L<<3),    
    HAL_DISP_INTF_BT1120 = (0x01L<<4),    
    HAL_DISP_INTF_HDMI   = (0x01L<<5),
    HAL_DISP_INTF_LCD    = (0x01L<<6), 
    HAL_DISP_INTF_DATE   = (0x01L<<7),
    HAL_DISP_INTF_BUTT   = (0x01L<<8),
} HAL_DISP_INTF_E;

typedef struct tagHAL_DISP_SYNCINV_S 
{    
    HI_U32  u32f_inv;
    HI_U32  u32vs_inv;    
    HI_U32  u32hs_inv;
    HI_U32  u32dv_inv; 

} HAL_DISP_SYNCINV_S;

/* vou background color */
typedef struct tagHAL_DISP_BKCOLOR_S
{
    HI_U8 u8Bkg_a;  
    HI_U8 u8Bkg_y;
    HI_U8 u8Bkg_cb;
    HI_U8 u8Bkg_cr;
} HAL_DISP_BKCOLOR_S; 

typedef struct tagHAL_DISP_CLIP_S
{
    HI_U16 u16ClipLow_y;  
    HI_U16 u16ClipLow_cb;  
    HI_U16 u16ClipLow_cr;  
    
    HI_U16 u16ClipHigh_y;  
    HI_U16 u16ClipHigh_cb;  
    HI_U16 u16ClipHigh_cr;  

} HAL_DISP_CLIP_S;

typedef struct
{
    HI_U32 acc_multi    ;
    HI_U32 thd_med_high ;
    HI_U32 thd_med_low  ;
    HI_U32 thd_high     ;
    HI_U32 thd_low      ;
} ACCTHD_S;

/*矩形大小坐标信息*/
typedef struct tagHAL_DISP_RECT_S
{
    HI_S32 s32SX;       /*source horizontal start position*/
    HI_S32 s32SY;       /*source vertical start position*/
    
    HI_S32 s32DXS;       /*dispaly horizontal start position*/
    HI_S32 s32DYS;       /*display vertical start position*/

    HI_S32 s32DXL;       /*dispaly horizontal end position*/
    HI_S32 s32DYL;       /*display vertical end position*/
    
    HI_S32 s32VX;       /*video horizontal start position*/
    HI_S32 s32VY;       /*video vertical start position*/    
    
    HI_U32 u32InWidth;   /*input width*/
    HI_U32 u32InHeight;  /*input height*/
    HI_U32 u32OutWidth;   /*output width*/
    HI_U32 u32OutHeight;  /*output height*/
} HAL_DISP_RECT_S;


typedef struct
{
    HI_U8  u8UIndex;
    HI_U8  u8VIndex;
    
    HI_S8  s8UOffsetA;
    HI_S8  s8UOffsetB;
    HI_S8  s8UOffsetC;
    HI_S8  s8UOffsetD;
    
    HI_S8  s8VOffsetA;
    HI_S8  s8VOffsetB;
    HI_S8  s8VOffsetC;
    HI_S8  s8VOffsetD;
} HAL_DISP_ACMBLKINFO_S;

/* vou graphic layer mask  */
typedef struct tagHAL_GFX_MASK_S
{
    HI_U8 u8Mask_r;
    HI_U8 u8Mask_g;
    HI_U8 u8Mask_b;

} HAL_GFX_MASK_S;

typedef struct tagHAL_GFX_KEY_MAX_S
{
    HI_U8 u8KeyMax_R;
    HI_U8 u8KeyMax_G;
    HI_U8 u8KeyMax_B;

} HAL_GFX_KEY_MAX_S;

typedef struct tagHAL_GFX_KEY_MIN_S
{
    HI_U8 u8KeyMin_R;
    HI_U8 u8KeyMin_G;
    HI_U8 u8KeyMin_B;

} HAL_GFX_KEY_MIN_S;

typedef struct hiRECT_S
{
    HI_S32 s32X;
    HI_S32 s32Y;
    HI_U32 u32Width;
    HI_U32 u32Height;
}RECT_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* End of __VOU_DEF_H__ */

