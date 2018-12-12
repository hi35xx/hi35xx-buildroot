/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/03/18
  Description   :
  History       :
  1.Date        : 2009/03/18
    Author      : x00100808
    Modification: Created file

******************************************************************************/


#ifndef __HI35XX_VO_H__
#define __HI35XX_VO_H__

#define CFG_MAXARGS 10

#define HI_353X_VO  1
#define PIC_MAX_WIDTH   1920
#define PIC_MAX_HEIGHT  1200
#define PIC_MIN_LENTH   16

/* vo inteface type */
#define VO_INTF_CVBS        (0x01L<<0)
#define VO_INTF_YPBPR       (0x01L<<1)
#define VO_INTF_VGA         (0x01L<<2)
#define VO_INTF_BT656       (0x01L<<3)
#define VO_INTF_BT1120      (0x01L<<4)
#define VO_INTF_HDMI        (0x01L<<5)
#define VO_INTF_LCD         (0x01L<<6)
#define VO_INTF_BT656_H     (0x01L<<7)
#define VO_INTF_BT656_L     (0x01L<<8)
#define VO_INTF_LCD_6BIT    (0x01L<<9)
#define VO_INTF_LCD_8BIT    (0x01L<<10)
#define VO_INTF_LCD_16BIT   (0x01L<<11)



#define VO_DEFAULT_CHN      -1          /* use vo buffer as pip buffer */

/*****************************************************************************
 * 3520 ADDed
 *****************************************************************************/

typedef enum hiVO_DEV_E
{
    VOU_DEV_DHD1  = 0,                 /* assistant device */
    VOU_DEV_BUTT
} VO_DEV_E;

typedef enum
{
    VOU_LAYER_VHD1  = 0,
    VOU_LAYER_G2    = 1,   
    VOU_LAYER_BUTT
}VOU_LAYER_E;


typedef enum hiVO_INTF_SYNC_E
{
    VO_OUTPUT_PAL = 0,
    VO_OUTPUT_NTSC,
    
    VO_OUTPUT_1080P24,
    VO_OUTPUT_1080P25,
    VO_OUTPUT_1080P30,
    
    VO_OUTPUT_720P50, 
    VO_OUTPUT_720P60,   
    VO_OUTPUT_1080I50,
    VO_OUTPUT_1080I60,    
    VO_OUTPUT_1080P50,
    VO_OUTPUT_1080P60,            

    VO_OUTPUT_576P50,
    VO_OUTPUT_480P60,

    VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
    VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/
    VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_960H_PAL,              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
    VO_OUTPUT_960H_NTSC,             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/
    VO_OUTPUT_320X240_60,            /* For ota5182 at 60 Hz (8bit) */
    VO_OUTPUT_320X240_50,            /* For ili9342 at 50 Hz (6bit) */
    VO_OUTPUT_240X320_50,            /* For ili9341 at 50 Hz (6bit) */
    VO_OUTPUT_240X320_60,            /* For ili9341 at 60 Hz (16bit) */
    VO_OUTPUT_USER,
    VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;



typedef enum hiVO_CSC_MATRIX_E
{
    VO_CSC_MATRIX_IDENTITY = 0,
    
    VO_CSC_MATRIX_BT601_TO_BT709,
    VO_CSC_MATRIX_BT709_TO_BT601,

    VO_CSC_MATRIX_BT601_TO_RGB_PC,
    VO_CSC_MATRIX_BT709_TO_RGB_PC,

    VO_CSC_MATRIX_RGB_TO_BT601_PC,
    VO_CSC_MATRIX_RGB_TO_BT709_PC,

    VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;





#endif /* End of #ifndef __HI_COMM_VO_H__ */

