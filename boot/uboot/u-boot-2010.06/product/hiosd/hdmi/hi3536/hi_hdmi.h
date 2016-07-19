/*******************************************************************************
 *              Copyright 2005 - 2006, Hisilicon Tech. Co., Ltd.
 *                           ALL RIGHTS RESERVED
 * FileName:    hi_hdmi.h
 * Description: This is internal include file for irda
 *
 * History:
 * Version      Date         Author       DefectNum    Description
 *  0.01     2010-08-06      luochen      NULL       Create this file
 ******************************************************************************/

#ifndef  __HI_HDMI_H__
#define  __HI_HDMI_H__

#include "hi_type.h"




typedef enum hiHDMI_VIDEO_FMT_E
{
    HI_HDMI_VIDEO_FMT_1080P_60 = 0,
    HI_HDMI_VIDEO_FMT_1080P_50,
    HI_HDMI_VIDEO_FMT_1080P_30,
    HI_HDMI_VIDEO_FMT_1080P_25,
    HI_HDMI_VIDEO_FMT_1080P_24,

    HI_HDMI_VIDEO_FMT_1080i_60,
    HI_HDMI_VIDEO_FMT_1080i_50,

    HI_HDMI_VIDEO_FMT_720P_60,
    HI_HDMI_VIDEO_FMT_720P_50,

    HI_HDMI_VIDEO_FMT_576P_50,
    HI_HDMI_VIDEO_FMT_480P_60,

    HI_HDMI_VIDEO_FMT_PAL,                  /* B D G H I PAL */
    HI_HDMI_VIDEO_FMT_PAL_N,                /* (N)PAL        */
    HI_HDMI_VIDEO_FMT_PAL_Nc,               /* (Nc)PAL       */

    HI_HDMI_VIDEO_FMT_NTSC,                 /* (M)NTSC       */
    HI_HDMI_VIDEO_FMT_NTSC_J,               /* NTSC-J        */
    HI_HDMI_VIDEO_FMT_NTSC_PAL_M,           /* (M)PAL        */

    HI_HDMI_VIDEO_FMT_SECAM_SIN,            /**< SECAM_SIN*/
    HI_HDMI_VIDEO_FMT_SECAM_COS,            /**< SECAM_COS*/

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
    HI_HDMI_VIDEO_FMT_VESA_1680X1050_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1080_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_2048X1152_60,    /* Currently, not support */
    HI_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE, /* Currently, not support */

    HI_HDMI_VIDEO_FMT_BUTT
}HI_HDMI_VIDEO_FMT_E;


typedef enum hiHDMI_VIDEO_MODE
{
    HI_HDMI_VIDEO_MODE_RGB444,
    HI_HDMI_VIDEO_MODE_YCBCR422,
    HI_HDMI_VIDEO_MODE_YCBCR444,
        
    HI_HDMI_VIDEO_MODE_BUTT    
}HI_HDMI_VIDEO_MODE_E;

typedef enum hiHDMI_ID_E
{
    HI_HDMI_ID_0         = 0,               /**< HDMI 0 */
    HI_HDMI_ID_BUTT
} HI_HDMI_ID_E;
typedef HI_HDMI_ID_E HI_UNF_HDMI_ID_E;

/**the config parameter of HDMI interface*/
/**CNcomment:HDMI 接口参数配置 */
typedef struct 
{
	HI_BOOL                  bEnableHdmi;         /**<force to HDMI or DVI,the value must set before HI_UNF_HDMI_Start or behind HI_UNF_HDMI_Stop*//**<CNcomment:是否强制HDMI,否则为DVI.该值必须在 HI_UNF_HDMI_Start之前或者HI_UNF_HDMI_Stop之后设置  */
    //HI_BOOL                  bEnableVideo;        /**<parameter must set HI_TRUE,or the HDMI diver will force to set HI_TRUE*//**<CNcomment:必须是HI_TRUE, 如果是HI_FALSE:HDMI驱动会强制设置为HI_TRUE */
    //HI_BOOL                  bEnableAudio;        /**<Enable flag of Audio*//**CNcomment:是否Enable音频 */
    HI_U32                   u32DispFmt;         /**<video format*//**CNcomment:视频制式 */
    HI_HDMI_VIDEO_MODE_E enVidOutMode;       /**<HDMI output vedio mode VIDEO_MODE_YCBCR,VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_RGB444 *//**<CNcomment:HDMI输出视频模式，VIDEO_MODE_YCBCR444，VIDEO_MODE_YCBCR422，VIDEO_MODE_RGB444 */
    HI_HDMI_VIDEO_MODE_E enVidInMode;        /**<HDMI input vedio mode VIDEO_MODE_YCBCR,VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_RGB444 *//**<CNcomment:HDMI输出视频模式，VIDEO_MODE_YCBCR444，VIDEO_MODE_YCBCR422，VIDEO_MODE_RGB444 */
}HI_DRV_HDMI_ATTR_S;

/**the config parameter of HDMI interface*/
/**CNcomment:HDMI 接口参数配置 */
typedef struct hiUNF_HDMI_ATTR_S
{
	HI_BOOL                  bEnableHdmi;         /**<force to HDMI or DVI,the value must set before HI_UNF_HDMI_Start or behind HI_UNF_HDMI_Stop*//**<CNcomment:是否强制HDMI,否则为DVI.该值必须在 HI_UNF_HDMI_Start之前或者HI_UNF_HDMI_Stop之后设置  */
   // HI_BOOL                  bEnableVideo;        /**<parameter must set HI_TRUE,or the HDMI diver will force to set HI_TRUE*//**<CNcomment:必须是HI_TRUE, 如果是HI_FALSE:HDMI驱动会强制设置为HI_TRUE */
  //  HI_BOOL                  bEnableAudio;        /**<Enable flag of Audio*//**CNcomment:是否Enable音频 */
    HI_U32                   u32DispFmt;         /**<video format*//**CNcomment:视频制式 */
    HI_HDMI_VIDEO_MODE_E enVidOutMode;       /**<HDMI output vedio mode VIDEO_MODE_YCBCR,VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_RGB444 *//**<CNcomment:HDMI输出视频模式，VIDEO_MODE_YCBCR444，VIDEO_MODE_YCBCR422，VIDEO_MODE_RGB444 */
    HI_HDMI_VIDEO_MODE_E enVidInMode;        /**<HDMI input vedio mode VIDEO_MODE_YCBCR,VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_RGB444 *//**<CNcomment:HDMI输出视频模式，VIDEO_MODE_YCBCR444，VIDEO_MODE_YCBCR422，VIDEO_MODE_RGB444 */
}HI_UNF_HDMI_ATTR_S;


HI_S32 HI_DRV_HDMI_Init(void);
HI_S32 HI_DRV_HDMI_DeInit(void);
HI_S32 HI_DRV_HDMI_Open(HI_HDMI_ID_E enHdmiID);
HI_S32 HI_DRV_HDMI_SetAttr(HI_HDMI_ID_E enHdmiID, HI_DRV_HDMI_ATTR_S *pstAttr);
HI_S32 HI_DRV_HDMI_Start(HI_HDMI_ID_E enHdmiID);
HI_S32 HI_DRV_HDMI_Close(HI_HDMI_ID_E enHdmiID);




int hdmi_display(unsigned int vosync, unsigned int input, unsigned int output);
void hdmi_stop(void);

#endif
