//#include "hdmi_drv_common.h"

#ifndef __HDMI_HAL_H__
#define __HDMI_HAL_H__
/**
* @brief KSV data object
*/

#ifdef HDMI_CEC_SUPPORT
#include "hal_cec.h"
#endif
#include "drv_hdmi_common.h"

typedef HI_VOID* HDMI_HANDLE;

typedef struct
{
    HDMI_HANDLE          hHdmiHw;
    HDMI_HANDLE          hHdmiDev;
    HI_U32               u32HdmiID;
    HDMI_TX_CAPABILITY_E enTxCapability;
    HMDI_CALLBACK        pCallback;
    HDMI_VIDEO_CONFIG_S  stVideoCfg;
    HDMI_AUDIO_CONFIG_S  stAudioCfg;
#ifdef HDMI_CEC_SUPPORT
    HDMI_HAL_CEC_S*      pstCec;
#endif
}HDMI_HAL_CONTEXT_S;

typedef struct hiHDMI_HAL_S
{
    HDMI_HAL_CONTEXT_S stHalCtx;
    HI_VOID (*HAL_HDMI_HardwareInit)(struct hiHDMI_HAL_S *pstHdmiHal);
    HI_VOID (*HAL_HDMI_TmdsModeSet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_TMDS_MODE_E enTmdsMode);
    HI_VOID (*HAL_HDMI_AvMuteSet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL bAvMute);
    HI_VOID (*HAL_HDMI_InfoframeSet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_INFOFRAME_ID_E enInfoFrameId, HI_U8 u8InBuffer[]);
    HI_VOID (*HAL_HDMI_InfoframeEnableSet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_INFOFRAME_ID_E enInfoFrameId, HI_BOOL bEnable);
    HI_S32  (*HAL_HDMI_VideoPathSet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_VIDEO_CONFIG_S* pstVideoCfg);
    HI_VOID (*HAL_HDMI_PhyOutputEnableSet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable);
    HI_VOID (*HAL_HDMI_PhyPowerEnableSet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable);
    HI_VOID (*HAL_HDMI_TxCapabilityGet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_TX_CAPABILITY_E *penTxCapability); 
    HDMI_VIDEO_TIMING_E (*HAL_HDMI_DispFmt2HdmiTiming)(struct hiHDMI_HAL_S *pstHdmiHal, HI_U32 u32DispFmt);

#ifndef HDMI_BUILD_IN_BOOT
    HI_S32  (*HAL_HDMI_DispFmtGet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_U32 *u32DispFmt);
    HI_VOID (*HAL_HDMI_SequencerHandlerProcess)(struct hiHDMI_HAL_S *pstHdmiHal);
    HI_VOID (*HAL_HDMI_ScdcStatusGet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_SCDC_STATUS_S* pstScdcStatus);
    HI_VOID (*HAL_HDMI_HardwareStatusGet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_HARDWARE_STATUS_S* pstHwStatus);
    HI_VOID (*HAL_HDMI_HotPlugStatusGet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL* bHotPlug);
    HI_VOID (*HAL_HDMI_AudioMuteSet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL bAudioMute);
    HI_S32 (*HAL_HDMI_AudioPathSet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_AUDIO_CONFIG_S* pstAudioCfg);
    HI_U32 (*HAL_HDMI_EdidRawDataRead)(struct hiHDMI_HAL_S *pstHdmiHal, HI_U32 u32Size, HI_U8 u8OutBuffer[]);
    HI_VOID (*HAL_HDMI_PhyOutputEnableGet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL* bEnable);
    HI_VOID (*HAL_HDMI_IrqEnableSet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_INT_TYPE_E enIntType, HI_BOOL bEnable);
    HI_VOID (*HAL_HDMI_VideoMuteSet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL bAudioMute);
#endif

#ifdef HDMI_HDCP_SUPPORT
    HI_VOID (*HAL_HDMI_HdcpEnableSet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable);
    HI_S32 (*HAL_HDMI_HdcpModeSet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_HDCP_MODE_E enHdcpMode);
    HI_S32 (*HAL_HDMI_HdcpKsvListGet)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_HDCP_KSVLIST_S* pstKsvList);
    HI_VOID (*HAL_HDMI_SinkHdcp22CapabilityGet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL* bSupport);
#endif

#ifdef HDMI_CEC_SUPPORT
    HI_VOID (*HAL_HDMI_CecEnableSet)(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable);
    HI_S32 (*HAL_HDMI_CecAutoPing)(struct hiHDMI_HAL_S *pstHdmiHal, HI_U16 u16PhyAddr, HDMI_CEC_NETWORK_S* pstCecNetwork);
    HI_S32 (*HAL_HDMI_CecMsgSend)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_CEC_CMD_S* pstMsg);
    HI_S32 (*HAL_HDMI_CecMsgReceive)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_CEC_CMD_S* pstCecCmd);
#endif
#ifdef HDMI_SCDC_SUPPORT
    HI_VOID (*HAL_HDMI_ScdcConfig)(struct hiHDMI_HAL_S *pstHdmiHal, HDMI_SCDC_CONFIG_S* pstScdcConfig);
#endif
}HDMI_HAL_S;


/*******************************define_start**********************************************************
Function:      
int HAL_HDMI_Open(HMDI_CALLBACK_FUNC pCallback, HDMI_HAL_S** pstHdmiHalCtrl)
* Description:  打开HDMI HAL模块
* Input:        pCallback:上层驱动事件处理回调          
* Output:       **pstHdmiHalCtrl: hal层全局结构体指针
* Return:        成功 / 错误码
* Others:        无
************************************end****************************************************************/
HI_S32 HAL_HDMI_Open(HMDI_CALLBACK pCallback, HDMI_HAL_S** pstHalHandle);

/*******************************define_start**********************************************************
Function:      
void HAL_HDMI_Close(HDMI_HAL_S* pstHdmiHal)
* Description:  释放HDMI HAL模块
* Input:        *pstHdmiHal:hal层全局结构体指针         
* Output:       无
* Return:        无
* Others:        无
************************************end****************************************************************/
void HAL_HDMI_Close(HDMI_HAL_S* pstHdmiHal);
#endif
