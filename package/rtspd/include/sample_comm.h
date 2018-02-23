/******************************************************************************
  Hisilicon HI3531 sample programs head file.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include "tw2865.h"
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_isp.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_venc.h"
#include "hi_comm_vpss.h"
#include "hi_comm_vdec.h"
#include "hi_comm_vda.h"
#include "hi_comm_region.h"
#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"
#include "hi_comm_ao.h"
#include "hi_comm_aio.h"
#include "hi_comm_isp.h"
#include "hi_defines.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vpss.h"
#include "mpi_vdec.h"
#include "mpi_vda.h"
#include "mpi_region.h"
#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_isp.h"
#ifdef ISP_V2
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_af.h"
#endif
#include "hi_sns_ctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*******************************************************
    macro define 
*******************************************************/
#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))
#define SAMPLE_SYS_ALIGN_WIDTH      64
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_420

#define TW2865_FILE "/dev/tw2865dev"
#define TW2960_FILE "/dev/tw2960dev"
#define TLV320_FILE "/dev/tlv320aic31"


#if HICHIP == HI3518_V100 //FIXME
#define SAMPLE_VO_DEV_DSD1 0
#define SAMPLE_VO_DEV_DSD0 SAMPLE_VO_DEV_DSD1
#else
#error HICHIP define may be error
#endif

/*** for init global parameter ***/
#define SAMPLE_ENABLE 		    1
#define SAMPLE_DISABLE 		    0
#define SAMPLE_NOUSE 		    -1


#define SAMPLE_AUDIO_HDMI_AO_DEV 3


#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

/*******************************************************
    enum define 
*******************************************************/

typedef enum sample_ispcfg_opt_e
{
    CFG_OPT_NONE    = 0,
    CFG_OPT_SAVE    = 1,
    CFG_OPT_LOAD    = 2,
    CFG_OPT_BUTT
}SAMPLE_CFG_OPT_E;

typedef enum sample_vi_mode_e
{
    APTINA_AR0130_DC_720P_30FPS = 0,
    APTINA_9M034_DC_720P_30FPS,
    SONY_ICX692_DC_720P_30FPS,
    SONY_IMX104_DC_720P_30FPS,
    SONY_IMX138_DC_720P_30FPS,
    SONY_IMX122_DC_1080P_30FPS,
    SONY_IMX236_DC_1080P_30FPS,
    OMNI_OV9712_DC_720P_30FPS,
    APTINA_MT9P006_DC_1080P_30FPS,
    OMNI_OV2710_DC_1080P_30FPS,
    SOI_H22_DC_720P_30FPS,
    HIMAX_1375_DC_720P_30FPS,
    APTINA_AR0330_DC_1080P_30FPS,
    PIXELPLUS_3100K_DC_720P_30FPS,
    SAMPLE_VI_MODE_1_D1,
}SAMPLE_VI_MODE_E;

typedef enum 
{
    VI_DEV_BT656_D1_1MUX = 0,
    VI_DEV_BT656_D1_4MUX,
    VI_DEV_BT656_960H_1MUX,
    VI_DEV_BT656_960H_4MUX,
    VI_DEV_720P_HD_1MUX,
    VI_DEV_1080P_HD_1MUX,
    VI_DEV_BUTT
}SAMPLE_VI_DEV_TYPE_E;

typedef enum sample_vi_chn_set_e
{
    VI_CHN_SET_NORMAL = 0,      /* mirror, flip close */
    VI_CHN_SET_MIRROR,          /* open MIRROR */
    VI_CHN_SET_FLIP,            /* open filp */
    VI_CHN_SET_FLIP_MIRROR      /* mirror, flip */
}SAMPLE_VI_CHN_SET_E;

typedef enum sample_vo_mode_e
{
    VO_MODE_1MUX = 0,
    VO_MODE_BUTT
}SAMPLE_VO_MODE_E;
    
typedef enum sample_rc_e
{
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_FIXQP
}SAMPLE_RC_E;

typedef enum sample_rgn_change_type_e
{
    RGN_CHANGE_TYPE_FGALPHA = 0,
    RGN_CHANGE_TYPE_BGALPHA,
    RGN_CHANGE_TYPE_LAYER
}SAMPLE_RGN_CHANGE_TYPE_EN;


/*******************************************************
    structure define 
*******************************************************/
typedef struct sample_vi_param_s
{
    HI_S32 s32ViDevCnt;         // VI Dev Total Count
    HI_S32 s32ViDevInterval;    // Vi Dev Interval
    HI_S32 s32ViChnCnt;         // Vi Chn Total Count
    HI_S32 s32ViChnInterval;    // VI Chn Interval
}SAMPLE_VI_PARAM_S;

typedef struct sample_video_loss_s
{
    HI_BOOL bStart;
    pthread_t Pid;
    SAMPLE_VI_MODE_E enViMode;
} SAMPLE_VIDEO_LOSS_S;

typedef struct sample_venc_getstream_s
{
     HI_BOOL bThreadStart;
     HI_S32  s32Cnt;
}SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct sample_vi_config_s
{
    SAMPLE_VI_MODE_E enViMode;
    VIDEO_NORM_E enNorm;           /*DC: VIDEO_ENCODING_MODE_AUTO */    
    ROTATE_E enRotate;
    SAMPLE_VI_CHN_SET_E enViChnSet;    
}SAMPLE_VI_CONFIG_S;


/*******************************************************
    function announce  
*******************************************************/
HI_S32 SAMPLE_COMM_SYS_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize);
HI_U32 SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth);
HI_S32 SAMPLE_COMM_SYS_MemConfig(HI_VOID);
HI_VOID SAMPLE_COMM_SYS_Exit(void);
HI_S32 SAMPLE_COMM_SYS_Init(VB_CONF_S *pstVbConf);
HI_S32 SAMPLE_COMM_SYS_Payload2FilePostfix(PAYLOAD_TYPE_E enPayload, HI_CHAR* szFilePostfix);

HI_S32 SAMPLE_COMM_ISP_Init(void);
HI_VOID SAMPLE_COMM_ISP_Stop(void);
HI_S32 SAMPLE_COMM_ISP_Run(void);
HI_S32 SAMPLE_COMM_ISP_SensorInit(void);
HI_S32 SAMPLE_COMM_ISP_ChangeSensorMode(HI_U8 u8Mode);

HI_S32 SAMPLE_COMM_VI_Mode2Param(SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_PARAM_S *pstViParam);
HI_S32 SAMPLE_COMM_VI_Mode2Size(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm, SIZE_S *pstSize);
VI_DEV SAMPLE_COMM_VI_GetDev(SAMPLE_VI_MODE_E enViMode, VI_CHN ViChn);
HI_S32 SAMPLE_COMM_VI_StartDev(VI_DEV ViDev, SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S *pstCapRect, SIZE_S *pstTarSize, SAMPLE_VI_CONFIG_S* pstViConfig);
HI_S32 SAMPLE_COMM_VI_StartBT656(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm);
HI_S32 SAMPLE_COMM_VI_StopBT656(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_BindVpss(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_UnBindVpss(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_BindVenc(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_UnBindVenc(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_MemConfig(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_GetVFrameFromYUV(FILE *pYUVFile, HI_U32 u32Width, HI_U32 u32Height,HI_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo);
HI_S32 SAMPLE_COMM_VI_ChangeCapSize(VI_CHN ViChn, HI_U32 u32CapWidth, HI_U32 u32CapHeight,HI_U32 u32Width, HI_U32 u32Height);
HI_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig);
HI_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig);
	
HI_S32 SAMPLE_COMM_VPSS_MemConfig();
HI_S32 SAMPLE_COMM_VPSS_Start(HI_S32 s32GrpCnt, SIZE_S *pstSize, HI_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr);
HI_S32 SAMPLE_COMM_VPSS_Stop(HI_S32 s32GrpCnt, HI_S32 s32ChnCnt) ;
HI_S32 SAMPLE_COMM_VPSS_StartGroup(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstVpssGrpAttr);
HI_S32 SAMPLE_COMM_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
                                                  VPSS_CHN_ATTR_S *pstVpssChnAttr,
                                                  VPSS_CHN_MODE_S *pstVpssChnMode,
                                                  VPSS_EXT_CHN_ATTR_S *pstVpssExtChnAttr);
HI_S32 SAMPLE_COMM_VPSS_StopGroup(VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_DisableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize);
HI_S32 SAMPLE_COMM_EnableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize);


HI_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, HI_CHAR *pcMmzName);
HI_S32 SAMPLE_COMM_VO_StartDevLayer(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr, HI_U32 u32SrcFrmRate);
HI_S32 SAMPLE_COMM_VO_StopDevLayer(VO_DEV VoDev);
HI_S32 SAMPLE_COMM_VO_StartChn(VO_DEV VoDev,VO_PUB_ATTR_S *pstPubAttr,SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_StopChn(VO_DEV VoDev,SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_BindVi(VO_DEV VoDev, VO_CHN VoChn, VI_CHN ViChn);
HI_S32 SAMPLE_COMM_VO_UnBindVi(VO_DEV VoDev, VO_CHN VoChn);

HI_S32 SAMPLE_COMM_VENC_MemConfig(HI_VOID);
HI_S32 SAMPLE_COMM_VENC_Start(VENC_GRP VencGrp,VENC_CHN VencChn, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode);
HI_S32 SAMPLE_COMM_VENC_Stop(VENC_GRP VencGrp,VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_SnapStart(VENC_GRP VencGrp,VENC_CHN VencChn, SIZE_S *pstSize);
HI_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_GRP VencGrp, VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_SnapStop(VENC_GRP VencGrp,VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_StartGetStream(HI_S32 s32Cnt);
HI_S32 SAMPLE_COMM_VENC_StopGetStream();
HI_S32 SAMPLE_COMM_VENC_BindVpss(VENC_GRP GrpChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VENC_UnBindVpss(VENC_GRP GrpChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);

HI_S32 SAMLE_COMM_VDEC_BindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp);
HI_S32 SAMLE_COMM_VDEC_UnBindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp);
HI_S32 SAMLE_COMM_VDEC_BindVo(VDEC_CHN VdChn, VO_DEV VoDev, VO_CHN VoChn);
HI_S32 SAMLE_COMM_VDEC_UnBindVo(VDEC_CHN VdChn, VO_DEV VoDev, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VDEC_MemConfig(HI_VOID);

HI_S32 SAMPLE_COMM_VDA_MdStart(VDA_CHN VdaChn, VI_CHN ViChn, SIZE_S *pstSize);
//HI_S32 SAMPLE_COMM_VDA_MdStart(VDA_CHN VdaChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, SIZE_S *pstSize);
HI_S32 SAMPLE_COMM_VDA_OdStart(VDA_CHN VdaChn, VI_CHN ViChn, SIZE_S *pstSize);
HI_VOID SAMPLE_COMM_VDA_MdStop(VDA_CHN VdaChn, VI_CHN ViChn);
HI_VOID SAMPLE_COMM_VDA_OdStop(VDA_CHN VdaChn, VI_CHN ViChn);

HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE *pAdcFd);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S *pstAioAttr, HI_BOOL bMacIn);
HI_S32 SAMPLE_COMM_AUDIO_DisableAcodec();
HI_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
        AIO_ATTR_S *pstAioAttr, HI_BOOL bAnrEn, AUDIO_RESAMPLE_ATTR_S *pstAiReSmpAttr);
HI_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
        HI_BOOL bAnrEn, HI_BOOL bResampleEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, AO_CHN AoChn,
        AIO_ATTR_S *pstAioAttr, AUDIO_RESAMPLE_ATTR_S *pstAiReSmpAttr);
HI_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, AO_CHN AoChn, HI_BOOL bResampleEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAenc(HI_S32 s32AencChnCnt, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAenc(HI_S32 s32AencChnCnt);
HI_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
