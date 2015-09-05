/*
 * (C) Copyright 2009-2010
 * Kevin Hsu, Hisilicon inc, xujilin@huawei.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/************************************************
 * NAME	    : hi35xx_vo.h
 *
 * Based on hi35xx User's manual Rev 0.0
 ************************************************/


#ifndef __HI35XX_VO_H__
#define __HI35XX_VO_H__

#define CFG_MAXARGS 10

#define HI_353X_VO  1
#define PIC_MAX_WIDTH   1920
#define PIC_MAX_HEIGHT  1200
#define PIC_MIN_LENTH   16

/* vo inteface type */
#define VO_INTF_CVBS     (0x01L<<0)
#define VO_INTF_YPBPR    (0x01L<<1)
#define VO_INTF_VGA      (0x01L<<2)
#define VO_INTF_BT656    (0x01L<<3)
#define VO_INTF_BT1120   (0x01L<<4)
#define VO_INTF_HDMI     (0x01L<<5)
#define VO_INTF_LCD      (0x01L<<6)
#define VO_INTF_BT656_H  (0x01L<<7)
#define VO_INTF_BT656_L  (0x01L<<8)

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
    VO_OUTPUT_USER,
    VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;


typedef enum hiVO_DEV_E
{
    VOU_DEV_DHD0  = 0,                 /* high definition device */
    VOU_DEV_DHD1  = 1,                 /* assistant device */
    VOU_DEV_DSD0  = 2,                 /* spot device */
    //VOU_DEV_DSD1  = 3,
    //VOU_DEV_DSD2  = 4,
    //VOU_DEV_DSD3  = 5,
    //VOU_DEV_DSD4  = 6,
    //VOU_DEV_DSD5  = 7,
    VOU_DEV_BUTT
} VO_DEV_E;

typedef enum hiVO_GRAPHIC_E
{
    VO_GRAPHC_G0    = 0,
    VO_GRAPHC_G1    = 1,
    VO_GRAPHC_G2    = 2,
    //VO_GRAPHC_G3    = 3,
    VO_GRAPHC_G4    = 3,
    VO_GRAPHC_BUTT
} VO_GRAPHIC_E;

/*  所有物理资源层的枚举，需要与HAL层的资源枚举一一对应*/
typedef enum
{
    VOU_LAYER_VHD0  = 0,
    //VOU_LAYER_VHD1  = 1,
    //VOU_LAYER_VHD2  = 2,
    VOU_LAYER_VHD1  = 1,    
    VOU_LAYER_VP    = 2,
    VOU_LAYER_VSD0  = 3,
    //VOU_LAYER_VSD2  = 5,
    //VOU_LAYER_VSD3  = 6,
    //VOU_LAYER_VSD4  = 7,
    VOU_LAYER_G0    = 4,   /* fb0 */
    VOU_LAYER_G1    = 5,   /* fb2 */ 
    VOU_LAYER_HC0   = 6,   /* G2 is HC0  --- fb3 */ 
    VOU_LAYER_G4    = 7,   /* fb1 and no G3*/ 
    //VOU_LAYER_HC1   = 14,
    VOU_LAYER_WBC_G0 = 8,
    VOU_LAYER_WBC_G4 = 9,
    VOU_LAYER_WBC_D = 10,
    VOU_LAYER_BUTT
}VOU_LAYER_E;

#define VOU_LAYER_G2 VOU_LAYER_HC0

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

#endif

