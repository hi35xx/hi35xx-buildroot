#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <asm/thread_info.h>
#include <linux/poll.h>
#include <mach/hardware.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include "hdmi_hal.h"
#include "drv_hdmi_edid.h"
#include "drv_hdmi_event.h"
#include "drv_hdmi_common.h"
#include "drv_hdmi_platform.h"
#include "drv_hdmi_intf.h"
#include "drv_hdmi_ioctl.h"
#include "drv_hdmi_proc.h"
#include "drv_hdmi_compatibility.h"

#ifdef HDMI_ANDROID_SUPPORT
#include <linux/switch.h>

static HI_BOOL            s_bIsSwitchOn = HI_FALSE;
static struct switch_dev  s_stSwitchDev =
{
    .name = "hdmi",
};
#endif  

//static struct semaphore s_HdmiMutex;
HI_HDMI_DECLARE_MUTEX(s_HdmiMutex);

#define CHECK_POINTER(p) \
do{                      \
    if(HI_NULL == p){\
        HDMI_ERR("The pointer is null\n");       \
        return HI_FAILURE;\
    } \
}while(0) 

#define CHECK_HDMI_OPEN(state) \
do \
{   \
    if (!(state & HDMI_RUN_STATE_OPEN)) \
    {   \
        HDMI_WARN("The hdmi device not open\n");\
        return HI_FAILURE;\
    } \
}while(0)

#define BITDEPTH_2_DEEPCOLOR(BitDepth, DeepColor) \
do \
{   \
    if (HDMI_VIDEO_BITDEPTH_8 == BitDepth)\
    {\
        DeepColor = HDMI_DEEP_COLOR_24BIT;\
    }\
    else if (HDMI_VIDEO_BITDEPTH_10 == BitDepth)\
    {\
        DeepColor = HDMI_DEEP_COLOR_30BIT;\
    }\
    else if (HDMI_VIDEO_BITDEPTH_12 == BitDepth)\
    {\
        DeepColor = HDMI_DEEP_COLOR_36BIT;\
    }\
    else if (HDMI_VIDEO_BITDEPTH_16 == BitDepth)\
    {\
        DeepColor = HDMI_DEEP_COLOR_48BIT;\
    }\
    else\
    {\
        DeepColor = HDMI_DEEP_COLOR_BUTT;\
    }\
}while(0)

#define DEEPCOLOR_2_BITDEPTH(BitDepth, DeepColor) \
do \
{   \
    if (HDMI_DEEP_COLOR_24BIT == DeepColor)\
    {\
        BitDepth = HDMI_VIDEO_BITDEPTH_8;\
    }\
    else if (HDMI_DEEP_COLOR_30BIT == DeepColor)\
    {\
        BitDepth = HDMI_VIDEO_BITDEPTH_10;\
    }\
    else if (HDMI_DEEP_COLOR_36BIT == DeepColor)\
    {\
        BitDepth = HDMI_VIDEO_BITDEPTH_12;\
    }\
    else if (HDMI_DEEP_COLOR_48BIT == DeepColor)\
    {\
        BitDepth = HDMI_VIDEO_BITDEPTH_16;\
    }\
    else\
    {\
        BitDepth = HDMI_VIDEO_BITDEPTH_BUTT;\
    }\
}while(0)

extern HI_S32 HI_DRV_HDMI_Init(HI_VOID);
extern HI_S32 HI_DRV_HDMI_Deinit(HI_VOID);
extern HI_S32 HI_DRV_HDMI_KernelEventCallback(HI_VOID* data, HDMI_EVENT_E enEvent);

static atomic_t  s_HdmiCount;
static HI_S32 HdmiRelease(HDMI_DEVICE_S* pstHdmiDev); 
static HI_S32 HdmiFileOpen(struct inode *inode, struct file *filp);
static HI_S32 HdmiFileClose(struct inode *inode, struct file *filp);
static long  HdmiFileIoctl(struct file *file,unsigned int cmd, unsigned long arg);
static HI_VOID HdmiSetAndroidState(HDMI_SWITCH_STATE enState);
static HI_S32 HdmiVideoPathSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_VO_ATTR_S* pstVOAttr);
static HI_S32 HdmiAudioPathSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_AO_ATTR_S *pstAOAttr);
static HI_S32 HdmiEventCallback(HI_VOID* pData , HDMI_EVENT_E enEvent);
static HI_BOOL VOAttrIsChanged(HDMI_VO_ATTR_S* pstHwVOAttr, HDMI_VO_ATTR_S* pstSwVOAttr);
static HI_BOOL APPAttrIsChanged(HDMI_APP_ATTR_S* pstHwAPPAttr, HDMI_APP_ATTR_S* pstSwAPPAttr);

DECLARE_HDMI_NODE("HDMI", HdmiFileOpen, HdmiFileClose, HI_NULL, HdmiFileIoctl, HI_NULL, HI_NULL);

static HDMI_DEVICE_S s_stHdmiCtrl[HDMI_DEVICE_ID_BUTT];

HDMI_DEVICE_S* GetHdmiDevice(HDMI_DEVICE_ID_E enHdmiID)
{
    if (enHdmiID < HDMI_DEVICE_ID_BUTT)
    {
        s_stHdmiCtrl[enHdmiID].u32HdmiDevId = enHdmiID;
        return &s_stHdmiCtrl[enHdmiID];
    }
    return HI_NULL;
}

#ifdef HDMI_SCDC_SUPPORT
static HI_S32 HdmiScrambleEnableSet(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bEnable)
{
    HDMI_SCDC_CONFIG_S      stScdcConfig;
    HDMI_SCDC_STATUS_S      stScdcStatus;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    memset(&stScdcStatus, 0, sizeof(HDMI_SCDC_STATUS_S));
    pstHdmiDev->pstHdmiHal->HAL_HDMI_ScdcStatusGet(pstHdmiDev->pstHdmiHal, &stScdcStatus);

    if ((stScdcStatus.bSourceScrambleOn &&
        stScdcStatus.bSinkScrambleOn && 
        stScdcStatus.u8TmdsBitClkRatio == 40 && bEnable) ||
        (!stScdcStatus.bSourceScrambleOn &&
        !stScdcStatus.bSinkScrambleOn && 
        stScdcStatus.u8TmdsBitClkRatio == 10 && !bEnable))
    {
        HDMI_INFO("Scramble status is not change, so no need to config scramble\n");
        return HI_SUCCESS;
    }  

    HDMI_INFO("SourceScrambleOn:%d, SinkScrambleOn:%d, TmdsBitClkRatio:%d\n",stScdcStatus.bSourceScrambleOn,stScdcStatus.bSinkScrambleOn,stScdcStatus.u8TmdsBitClkRatio);
    memset(&stScdcConfig, 0, sizeof(HDMI_SCDC_CONFIG_S));
    stScdcConfig.bScdcEnable = bEnable;
    
    if (bEnable)
    {
        stScdcConfig.u32MaxTmdsCharacterRate = 600 * 1000000;
    }
    else
    {
        stScdcConfig.u32MaxTmdsCharacterRate = 300 * 1000000;
    }
#if 0    
    if (HDMI_EDID_DATA_INVALID == DRV_HDMI_EdidCapabilityGet(&pstHdmiDev->stEdidInfo, &pstSinkCap))
    {
        HDMI_WARN("Get sink capability fail\n");
        stScdcConfig.bScdcPresent            = HI_TRUE;
        stScdcConfig.bRRCapable              = HI_FALSE;
        stScdcConfig.bLTE340McscScramble     = HI_FALSE;
        stScdcConfig.u32MaxTmdsCharacterRate = 600 * 1000000;
    }
    else
    {
        stScdcConfig.bScdcPresent            = pstSinkCap->bSupportSCDC;
        stScdcConfig.bRRCapable              = pstSinkCap->bSupportRRCapable;
        stScdcConfig.bLTE340McscScramble     = pstSinkCap->bSupportLte340McscScrameble;
        stScdcConfig.u32MaxTmdsCharacterRate = pstSinkCap->u32MaxTMDSClock * 1000000;
    }
#else
    stScdcConfig.bScdcPresent            = HI_TRUE;
    stScdcConfig.bRRCapable              = HI_FALSE;
    stScdcConfig.bLTE340McscScramble     = HI_FALSE;

#endif
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyOutputEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);
    HDMI_INFO("ScdcEnable:%d\n",stScdcConfig.bScdcEnable); 
    HDMI_INFO("Scdc Config:ScdcPresent(%d),RRCapable(%d),LTE340McscScramble(%d),MaxTmdsCharacterRate(%d)\n",stScdcConfig.bScdcPresent,stScdcConfig.bRRCapable,stScdcConfig.bLTE340McscScramble,stScdcConfig.u32MaxTmdsCharacterRate);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_ScdcConfig(pstHdmiDev->pstHdmiHal, &stScdcConfig);
    
    return HI_SUCCESS;
}
#endif

static HI_S32 HdmiModeSchedule(HDMI_DEVICE_S* pstHdmiDev)
{
    HDMI_TX_CAPABILITY_E     enTxCapability;
    HDMI_SINK_CAPABILITY_S   *pstSinkCap;
    HDMI_APP_ATTR_S*         pstAppAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_TMDS_MODE_E         enTmdsMode = HDMI_TMDS_MODE_HDMI_1_4;
    
    pstAppAttr   = &pstHdmiDev->stAttr.stAppAttr;
    pstVideoAttr = &pstHdmiDev->stAttr.stVOAttr;

    /* disable YUV420 first*/
    pstAppAttr->bYUV420Enable = HI_FALSE;
    
    if (pstAppAttr->bEnableHdmi == HI_TRUE)//HDMI
    {
        /* 600M*/
        if (pstVideoAttr->enVideoTiming >= HDMI_VIDEO_TIMING_3840X2160P_50000 &&
            pstVideoAttr->enVideoTiming <= HDMI_VIDEO_TIMING_3840X2160P_60000 &&
            pstAppAttr->enOutColorSpace != HDMI_COLORSPACE_YCbCr420)
        {
            /* Max TMDS clock frequency is not more than 600M*/
            if (pstAppAttr->enDeepColorMode > HDMI_DEEP_COLOR_24BIT &&
                pstAppAttr->enDeepColorMode < HDMI_DEEP_COLOR_OFF)
            {
                HDMI_ERR("when fmt is 4K50 or 4K60, do not support deep color\n");
                return HI_FAILURE;
            }
            else
            {
                pstHdmiDev->pstHdmiHal->HAL_HDMI_TxCapabilityGet(pstHdmiDev->pstHdmiHal, &enTxCapability); 
                if ((enTxCapability & HDMI_TX_SUPPORT_HDMI2_0))
                {
                    enTmdsMode = HDMI_TMDS_MODE_HDMI_2_0;
                }
                else 
                {
                    HDMI_ERR("source does not support HDMI2.0\n");
                    return HI_FAILURE;
                }
                
            }
        }
    }
    else//DVI
    {
        enTmdsMode = HDMI_TMDS_MODE_DVI;
    }
    
    if (HDMI_EDID_DATA_INVALID == DRV_HDMI_EdidCapabilityGet(&pstHdmiDev->stEdidInfo, &pstSinkCap))
    {
        HDMI_WARN("Get sink capability fail\n");
    }
    else
    {
        /* when sink does not support HDMI2.0, then switch to HDMI1.4*/
        if (pstSinkCap->bSupportHdmi_2_0 != HI_TRUE && enTmdsMode == HDMI_TMDS_MODE_HDMI_2_0)
        {
            HI_U8 i = pstSinkCap->u32SupportVICNum;
            HI_BOOL bSupport4KP60 = HI_FALSE;

            enTmdsMode = HDMI_TMDS_MODE_HDMI_1_4;
            HDMI_INFO("Sink does not support HDMI2.0, Switch to HDMI1.4\n");

            while (i)
            {
                i--;
                if (HDMI_3840X2160P50_16_9 == pstSinkCap->au32SupportFormat[i] || 
                    HDMI_3840X2160P60_16_9 == pstSinkCap->au32SupportFormat[i])
                {
                    HDMI_INFO("Sink support 4K60 and 4K50\n");
                    bSupport4KP60 = HI_TRUE;
                    pstAppAttr->bYUV420Enable = HI_TRUE; 
                    HDMI_INFO("Enable YUV420\n");
                    break;
                }
            }        

            if (!bSupport4KP60)
            {
                if (pstSinkCap->u32OnlySupportY420VICNum > 0)
                {    
                    HDMI_INFO("The number of VIC that only support YUV420 is %d\n",pstSinkCap->u32OnlySupportY420VICNum);
                    for (i = 0; i < pstSinkCap->u32OnlySupportY420VICNum; i++)
                    {
                        if (HDMI_3840X2160P50_16_9 == pstSinkCap->au32OnlySupportY420Format[i] ||
                            HDMI_3840X2160P60_16_9 == pstSinkCap->au32OnlySupportY420Format[i])
                        {
                            /*when timing is 4K60,50, and sink not support 4K YUV444,but support 4K YUV420, enable YUV420*/
                            pstAppAttr->bYUV420Enable = HI_TRUE; 
                            HDMI_INFO("Enable YUV420\n");
                            break;
                        }
                    }
                }
            }
        }        
    }

    pstHdmiDev->pstHdmiHal->HAL_HDMI_TmdsModeSet(pstHdmiDev->pstHdmiHal, enTmdsMode);
    HDMI_INFO("original tmds mode is %d, new tmds mode is %d\n", pstHdmiDev->enTmdsMode,enTmdsMode);
    pstHdmiDev->enTmdsMode = enTmdsMode;
    
#ifdef HDMI_SCDC_SUPPORT    
    if (pstHdmiDev->enTmdsMode == HDMI_TMDS_MODE_HDMI_2_0)
    {
        HdmiScrambleEnableSet(pstHdmiDev, HI_TRUE);
    }
    else
    {
        HdmiScrambleEnableSet(pstHdmiDev, HI_FALSE);
    }
#endif   

    return HI_SUCCESS;
}

#ifdef HDMI_HDCP_SUPPORT
static HI_S32 HdcpSchedule(HDMI_DEVICE_S* pstHdmiDev)
{  
    HI_S32  s32Ret = HI_FAILURE;
    HI_BOOL bHotPlug;
    HDMI_APP_ATTR_S* pstAppAttr;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    pstAppAttr = &pstHdmiDev->stAttr.stAppAttr;

    /* HDCP mode select*/
    if (pstHdmiDev->stHdcpInfo.bHdcpIsOn)
    {
        HI_BOOL bHdcp22Support;
        HDMI_TX_CAPABILITY_E     enTxCapability;
        pstHdmiDev->pstHdmiHal->HAL_HDMI_TxCapabilityGet(pstHdmiDev->pstHdmiHal, &enTxCapability);  
        
        pstHdmiDev->stHdcpInfo.enHdcpMode = HDMI_HDCP_MODE_NONE;       
        pstHdmiDev->pstHdmiHal->HAL_HDMI_SinkHdcp22CapabilityGet(pstHdmiDev->pstHdmiHal, &bHdcp22Support);
        HDMI_INFO("Sink hdcp2.2 support(%d)\n",bHdcp22Support);
        if ((enTxCapability & HDMI_TX_SUPPORT_HDCP2_2) && bHdcp22Support)
        {
            pstHdmiDev->stHdcpInfo.enHdcpMode = HDMI_HDCP_MODE_2_2; 
        }            
        else if ((enTxCapability & HDMI_TX_SUPPORT_HDCP1_4) && !bHdcp22Support)
        {
            if (!pstHdmiDev->stHdcpInfo.bHdcp1Prep)
            {
                HDMI_ERR("Do not load hdcp1.4 key\n");
                return HI_FAILURE;
            }
            pstHdmiDev->stHdcpInfo.enHdcpMode = HDMI_HDCP_MODE_1_4;
        }               
        else
        {
            HDMI_WARN("Do not support HDCP\n");
            pstHdmiDev->stHdcpInfo.bHdcpIsOn = HI_FALSE;  
            return HI_FAILURE;
        }                 

        if (HDMI_HDCP_MODE_NONE != pstHdmiDev->stHdcpInfo.enHdcpMode)
        {
            HDMI_INFO("The HDCP mode is %d\n", pstHdmiDev->stHdcpInfo.enHdcpMode);
            s32Ret = pstHdmiDev->pstHdmiHal->HAL_HDMI_HdcpModeSet(pstHdmiDev->pstHdmiHal, pstHdmiDev->stHdcpInfo.enHdcpMode);
            if (s32Ret != HI_SUCCESS)
            {
                HDMI_ERR("Set Hdcp2.2 mode fail\n");
                return HI_FAILURE;
            }
        }
    }

    pstHdmiDev->pstHdmiHal->HAL_HDMI_HotPlugStatusGet(pstHdmiDev->pstHdmiHal, &bHotPlug);
    HDMI_INFO("Hdcp schedule, Hotplug:%d, HdcpIsOn:%d, HdcpWasOn:%d\n", bHotPlug,pstHdmiDev->stHdcpInfo.bHdcpIsOn, pstHdmiDev->stHdcpInfo.bHdcpWasOn);

    if (bHotPlug)// hot plug in
    {
        if (pstHdmiDev->stHdcpInfo.bHdcpIsOn && !pstHdmiDev->stHdcpInfo.bHdcpWasOn)
        {
            pstHdmiDev->stHdcpInfo.bHdcpWasOn = HI_TRUE;
            pstHdmiDev->pstHdmiHal->HAL_HDMI_HdcpEnableSet(pstHdmiDev->pstHdmiHal, HI_TRUE);
            HDMI_INFO("Start hdcp\n");
        }
        else if (!pstHdmiDev->stHdcpInfo.bHdcpIsOn && pstHdmiDev->stHdcpInfo.bHdcpWasOn)
        {
            pstHdmiDev->stHdcpInfo.bHdcpWasOn = HI_FALSE;
            pstHdmiDev->pstHdmiHal->HAL_HDMI_HdcpEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);
            HDMI_INFO("Stop hdcp\n");
        }
    }
    else// hot plug out
    {
        if (pstHdmiDev->stHdcpInfo.bHdcpWasOn)
        {
            pstHdmiDev->stHdcpInfo.bHdcpWasOn = HI_FALSE;
            pstHdmiDev->pstHdmiHal->HAL_HDMI_HdcpEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);
            HDMI_WARN("Hotplug is out,so stop hdcp\n");
        }
        else if (pstHdmiDev->stHdcpInfo.bHdcpIsOn)
        {
            HDMI_WARN("Hotplug is out,so do not start hdcp\n");
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;     
}
#endif 

#ifdef HDMI_CEC_SUPPORT
static HI_S32 CecSchedule(HDMI_DEVICE_S* pstHdmiDev)
{   
    HI_S32                  s32Ret;      
    HI_BOOL                 bHotPlug;
    HI_U16                  u16PhyAddr = 0;
    HDMI_CEC_NETWORK_S      stCecNetwork;
    HDMI_SINK_CAPABILITY_S  *pstSinkCap;
       
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
        
    pstHdmiDev->pstHdmiHal->HAL_HDMI_HotPlugStatusGet(pstHdmiDev->pstHdmiHal, &bHotPlug);
    if (bHotPlug)
    {
        if (pstHdmiDev->stCecInfo.bCecIsOn && !pstHdmiDev->stCecInfo.bCecWasOn)
        { 
            s32Ret = DRV_HDMI_EdidCapabilityGet(&pstHdmiDev->stEdidInfo, &pstSinkCap);  
            if (s32Ret != HI_SUCCESS)
            {
                HDMI_WARN("Invalid EdidCapability!\n");
            }
            
            if (pstSinkCap->stCECAddr.bPhyAddrValid)
            {
                 u16PhyAddr = (pstSinkCap->stCECAddr.u8PhyAddrA & 0xf) | ((pstSinkCap->stCECAddr.u8PhyAddrA & 0xf) << 4) \
                            | ((pstSinkCap->stCECAddr.u8PhyAddrC & 0xf) << 8) | ((pstSinkCap->stCECAddr.u8PhyAddrD & 0xf) << 12);
                 pstHdmiDev->stCecInfo.u8PhysicalAddr[0] = pstSinkCap->stCECAddr.u8PhyAddrA;
                 pstHdmiDev->stCecInfo.u8PhysicalAddr[1] = pstSinkCap->stCECAddr.u8PhyAddrB;
                 pstHdmiDev->stCecInfo.u8PhysicalAddr[2] = pstSinkCap->stCECAddr.u8PhyAddrC;
                 pstHdmiDev->stCecInfo.u8PhysicalAddr[3] = pstSinkCap->stCECAddr.u8PhyAddrD;
            }
            else
            {
                HDMI_ERR("Get cec physics address fail\n");
                return HI_FAILURE;
            }
            HDMI_INFO("Start Cec\n");
            pstHdmiDev->stCecInfo.bCecWasOn = HI_TRUE;
            pstHdmiDev->pstHdmiHal->HAL_HDMI_CecEnableSet(pstHdmiDev->pstHdmiHal, HI_TRUE);
            pstHdmiDev->pstHdmiHal->HAL_HDMI_CecAutoPing(pstHdmiDev->pstHdmiHal, u16PhyAddr, &stCecNetwork);
            pstHdmiDev->stCecInfo.u8LogicalAddr = stCecNetwork.logicalAddr;
            memcpy(pstHdmiDev->stCecInfo.u8Network, stCecNetwork.network, sizeof(stCecNetwork.network));
        }
        else if (!pstHdmiDev->stCecInfo.bCecIsOn && pstHdmiDev->stCecInfo.bCecWasOn)
        {
            HDMI_INFO("Stop Cec\n");
            pstHdmiDev->pstHdmiHal->HAL_HDMI_CecEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);
            pstHdmiDev->stCecInfo.bCecWasOn = HI_FALSE;
            memset(pstHdmiDev->stCecInfo.u8Network, 0, sizeof(stCecNetwork.network));
        }
    }
    else
    {
        if (pstHdmiDev->stCecInfo.bCecWasOn)
        {
            HDMI_INFO("Stop Cec\n");
            pstHdmiDev->pstHdmiHal->HAL_HDMI_CecEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);
            pstHdmiDev->stCecInfo.bCecWasOn = HI_FALSE;
            memset(pstHdmiDev->stCecInfo.u8Network, 0, sizeof(stCecNetwork.network));
        }
    }

    return HI_SUCCESS; 
}
#endif  

HI_S32 DRV_HDMI_Init(HI_VOID)
{
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_DeInit(HI_VOID)
{
    return HI_SUCCESS;
}

static HI_S32 HdmiAttrConstruct(HDMI_DEVICE_S * pstHdmiDev, HDMI_ATTR_S *pstAttr)
{   
    HDMI_HARDWARE_STATUS_S   stHwStatus;
    HDMI_AO_ATTR_S*          pstAudioAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_APP_ATTR_S*         pstAppAttr;
    
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    pstAppAttr   = &pstAttr->stAppAttr;
    pstAudioAttr = &pstAttr->stAOAttr;
    pstVideoAttr = &pstAttr->stVOAttr;  

    memset(&stHwStatus, 0, sizeof(HDMI_HARDWARE_STATUS_S));
    pstHdmiDev->pstHdmiHal->HAL_HDMI_HardwareStatusGet(pstHdmiDev->pstHdmiHal, &stHwStatus);
    /* user attribute construct*/
    pstHdmiDev->enTmdsMode = stHwStatus.stCommonStatus.enTmdsMode;
    switch (pstHdmiDev->enTmdsMode)
    {
        case HDMI_TMDS_MODE_HDMI_1_4:
        case HDMI_TMDS_MODE_HDMI_2_0:  
        {
            pstAppAttr->bEnableHdmi  = HI_TRUE;
            pstAppAttr->bEnableVideo = HI_TRUE;
            break;
        }
        case HDMI_TMDS_MODE_DVI:
        {
            pstAppAttr->bEnableHdmi  = HI_FALSE;
            pstAppAttr->bEnableVideo = HI_TRUE;
            break;
        } 
        default:
        {
            pstAppAttr->bEnableHdmi  = HI_FALSE;
            pstAppAttr->bEnableVideo = HI_FALSE;
            break;
        } 
    }

    pstAppAttr->bEnableAudio     = (stHwStatus.stAudioStatus.bAudioEnable && 
                                   stHwStatus.stInfoFrameStatus.bAUDIOEnable);
    pstAppAttr->enOutColorSpace  = stHwStatus.stVideoStatus.enOutColorSpace;
    BITDEPTH_2_DEEPCOLOR(stHwStatus.stVideoStatus.enOutBitDepth, pstAppAttr->enDeepColorMode);

	pstAppAttr->bEnableAviInfoFrame  = stHwStatus.stInfoFrameStatus.bAVIEnable;
	pstAppAttr->bEnableAudInfoFrame  = stHwStatus.stInfoFrameStatus.bAUDIOEnable;
    pstAppAttr->bxvYCCMode          = stHwStatus.stInfoFrameStatus.bGBDEnable;
    /* video attribute construct*/
    pstVideoAttr->enInColorSpace = pstAppAttr->enOutColorSpace; 

    if (stHwStatus.stVideoStatus.bYCbCr444_422 && stHwStatus.stVideoStatus.bYCbCr422_420)
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_YCbCr444;
    }
    
    if (stHwStatus.stVideoStatus.bRGB2YCbCr && 
        stHwStatus.stVideoStatus.enOutColorSpace != HDMI_COLORSPACE_RGB)
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_RGB;
    }

    if (stHwStatus.stVideoStatus.bYCbCr2RGB && 
        stHwStatus.stVideoStatus.enOutColorSpace == HDMI_COLORSPACE_RGB)
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_YCbCr444;
    }
    
    pstVideoAttr->enHvSyncPol    = stHwStatus.stVideoStatus.enHvSyncPol;
    if (stHwStatus.stInfoFrameStatus.bAVIEnable)
    {
        HDMI_VIDEO_CODE_E  enVideoCode  = stHwStatus.stInfoFrameStatus.u8AVI[7];
        pstVideoAttr->enPictureAspect   = (stHwStatus.stInfoFrameStatus.u8AVI[5] >> 4) & 0x3;
        pstVideoAttr->enActiveAspect    = stHwStatus.stInfoFrameStatus.u8AVI[5] & 0xf;
        pstVideoAttr->enColorimetry     = (stHwStatus.stInfoFrameStatus.u8AVI[5] >> 6) & 0x3;
        pstVideoAttr->enRGBQuantization = (stHwStatus.stInfoFrameStatus.u8AVI[6] >> 2) & 0x3;
        pstVideoAttr->u32PixelRepeat    = (stHwStatus.stInfoFrameStatus.u8AVI[8] & 0xf) + 1;
        pstVideoAttr->enVideoTiming     = HDMI_VIDEO_TIMING_UNKNOWN;
        pstVideoAttr->enVideoTiming     =  DRV_HDMI_VideoTimingLookup(enVideoCode, pstVideoAttr->enPictureAspect, HI_FALSE);
    }
    pstVideoAttr->enStereoMode = HDMI_3D_BUTT;
    if (stHwStatus.stInfoFrameStatus.bVSIFEnable)
    {
        enum hdmi_video_format format = (stHwStatus.stInfoFrameStatus.u8VSIF[7] >> 5) & 0x3;
        if (format == HDMI_VIDEO_FORMAT_4K)
        {
            HDMI_VIDEO_CODE_E  enVideoCode = stHwStatus.stInfoFrameStatus.u8VSIF[8];
            pstVideoAttr->enVideoTiming = DRV_HDMI_VideoTimingLookup(enVideoCode, pstVideoAttr->enPictureAspect, HI_TRUE);
            
        }
        else if (format == HDMI_VIDEO_FORMAT_3D) 
        {
            pstVideoAttr->enStereoMode    = (stHwStatus.stInfoFrameStatus.u8VSIF[8] >> 4) & 0xf;
        }
       
    }

    /* audio attrbute construct*/
    pstAudioAttr->bDownSample   = stHwStatus.stAudioStatus.bDownSample;
    pstAudioAttr->enChanels     = stHwStatus.stAudioStatus.enLayout;
    pstAudioAttr->enSampleDepth = stHwStatus.stAudioStatus.enSampleDepth;
    pstAudioAttr->enSampleFs    = stHwStatus.stAudioStatus.enSampleFs;
    pstAudioAttr->enSoundIntf   = stHwStatus.stAudioStatus.enSoundIntf;
    if (stHwStatus.stInfoFrameStatus.bAUDIOEnable)
    {
        pstAudioAttr->enAudioCode = (stHwStatus.stInfoFrameStatus.u8AUDIO[4] >> 4) & 0xf;
    }
    
    return HI_SUCCESS;
}
#if 0
static HI_S32 HdmiTransitionProcess(HDMI_DEVICE_S * pstHdmiDev)
{
    HI_BOOL bHdmiOn  = HI_FALSE;
    HI_BOOL bHotPLug = HI_FALSE;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyOutputEnableGet(pstHdmiDev->pstHdmiHal, &bHdmiOn);
    if (!bHdmiOn)
    {
         pstHdmiDev->enTransitionState = HDMI_TRANSITION_NONE;
         pstHdmiDev->pstHdmiHal->HAL_HDMI_HardwareInit(pstHdmiDev->pstHdmiHal); 
    }
    else
    {
        if (pstHdmiDev->u32UserCnt)
        {
            if (pstHdmiDev->u32KernelCnt)//HDMI has opened in kernel
            {
                pstHdmiDev->enTransitionState = HDMI_TRANSITION_MCE_APP;
     
                pstHdmiDev->pstHdmiHal->HAL_HDMI_HotPlugStatusGet(pstHdmiDev->pstHdmiHal, &bHotPLug);  
                if (bHotPLug)
                {
                    HdmiEventCallback(pstHdmiDev, HDMI_EVENT_HOTPLUG);
                }
                else
                {
                    HdmiEventCallback(pstHdmiDev, HDMI_EVENT_HOTUNPLUG);
                }                
            }
            else
            {
                pstHdmiDev->enTransitionState = HDMI_TRANSITION_BOOT_APP;
            }
        }
        else
        {
            pstHdmiDev->enTransitionState = HDMI_TRANSITION_BOOT_MCE;
        }
    }
    /* only debug,delete it in future*/
    //pstHdmiDev->pstHdmiHal->HAL_HDMI_HardwareInit(pstHdmiDev->pstHdmiHal); 

    pstHdmiDev->pstHdmiHal->HAL_HDMI_IrqEnableSet(pstHdmiDev->pstHdmiHal, HDMI_INT_TYPE_HPD_RSEN, HI_TRUE);
    HDMI_INFO("TransitionState = %d\n",pstHdmiDev->enTransitionState);
    return HI_SUCCESS;
}
#endif
/*
static HDMI_TRANSITION_STATE_E HdmiTransitionStateGet(HDMI_DEVICE_S * pstHdmiDev)
{
    CHECK_POINTER(pstHdmiDev);
    return pstHdmiDev->enTransitionState;
}
*/
static HI_S32 HdmiEventCallback(HI_VOID* pData , HDMI_EVENT_E enEvent)
{
    HI_S32          s32Ret = 0;
    HI_U32          u32TimeOutCnt = 5;
    HDMI_DEVICE_S* pstHdmiDev = (HDMI_DEVICE_S*) pData;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    switch (enEvent)
    {
        case HDMI_EVENT_HOTPLUG:
            {
                /* update edid from sink*/
                do 
                {               
                    /*  1.for some TV ,EDID need to read more times ; 
                        2.for some repeater ,EDID is readable after HPD assert 250ms. */
                    s32Ret = DRV_HDMI_EdidUpdate(&pstHdmiDev->stEdidInfo, HDMI_EDID_UPDATE_SINK);
                    if (s32Ret == HI_SUCCESS)
                    {
                        break;
                    }
                    msleep(50);
                    
                }while(u32TimeOutCnt--);

                if (s32Ret != HI_SUCCESS)
                {
                    HDMI_ERR("Update EDID fail,u32TimeOutCnt=%\n",u32TimeOutCnt);
                }
                
                HdmiSetAndroidState(STATE_HOTPLUGIN);
                break;
            }
        case HDMI_EVENT_HOTUNPLUG:
            {
                HdmiSetAndroidState(STATE_HOTPLUGOUT); 
                break;
            }
        case HDMI_EVENT_HDCP_FAIL:
            break;   
        case HDMI_EVENT_HDCP_SUCCESS:
            break;  
        case HDMI_EVENT_SCRAMBLE_FAIL:
            break;   
        case HDMI_EVENT_SCRAMBLE_SUCCESS:
            break;  
        default :
            break;
    }
    
    if (enEvent <= HDMI_EVENT_HDCP_USERSETTING)
    {
        if (!pstHdmiDev->pKCallback)//Notify to user
        {
            HDMI_INFO("Notify event(0x%x) to user\n", enEvent);
            DRV_HDMI_EventPoolWrite(&pstHdmiDev->stEventInfo, enEvent);
        }
        else//Notify to kernel 
        {
            HDMI_INFO("Notify event(0x%x) to kernel\n", enEvent);
			HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
			if(pstHdmiDev->pKCallback)
			{
				pstHdmiDev->pKCallback((HI_VOID*)&(pstHdmiDev->u32HdmiDevId), enEvent); 
			}
			HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
        }
    }
    return HI_SUCCESS; 
}

static HI_S32 HdmiKThreadTimer(void* data)
{
    HDMI_DEVICE_S* pstHdmiDev = (HDMI_DEVICE_S*)data;
    CHECK_POINTER(pstHdmiDev);
    
    while ( 1 )
    {
        if (kthread_should_stop())
        {
            break;
        }

        if (pstHdmiDev->stThreadInfo.kThreadTimerSate == HDMI_THREAD_STATE_STOP)
        {
            msleep(90);
            continue;
        }
  
        if (pstHdmiDev->pstHdmiHal)
        {       
            if (pstHdmiDev->pstHdmiHal->HAL_HDMI_SequencerHandlerProcess)
            {
                pstHdmiDev->pstHdmiHal->HAL_HDMI_SequencerHandlerProcess((struct hiHDMI_HAL_S *)pstHdmiDev);
            }
        }
        else
        {
            HDMI_ERR("HDMI_KThreadTimer's input param is NULL\n");
        }
        msleep(90);
    }

    return HI_SUCCESS;
}


HDMI_VIDEO_TIMING_E DispFmt2HdmiTiming(HDMI_DEVICE_S* pstHdmiDev, HI_U32 u32DispFmt)
{
    if (pstHdmiDev->pstHdmiHal && pstHdmiDev->pstHdmiHal->HAL_HDMI_DispFmt2HdmiTiming)
    {
        return pstHdmiDev->pstHdmiHal->HAL_HDMI_DispFmt2HdmiTiming(pstHdmiDev->pstHdmiHal, u32DispFmt);
    }
    
    return HDMI_VIDEO_TIMING_1280X720P_50000;
}

#if 1
static HI_VOID HdmiVOAttrInit(HDMI_VO_ATTR_S *pstVideoAttr)
{
    //pstVideoAttr->enVideoTiming = DispFmt2HdmiTiming(u32DispFmt);

    if (pstVideoAttr->enVideoTiming > HDMI_VIDEO_TIMING_720X576P_50000)
    {
        pstVideoAttr->enColorimetry   = HDMI_COLORIMETRY__ITU_709;
        pstVideoAttr->enPictureAspect = HDMI_PICTURE_ASPECT_16_9;
        pstVideoAttr->enHvSyncPol     = HDMI_HV_SYNC_POL_HPVP;
    }
    else
    {
        pstVideoAttr->enColorimetry   = HDMI_COLORIMETRY__ITU_601;
        pstVideoAttr->enPictureAspect = HDMI_PICTURE_ASPECT_4_3;
        pstVideoAttr->enHvSyncPol     = HDMI_HV_SYNC_POL_HNVN;
    }

    pstVideoAttr->u32PixelRepeat = 1;
    if (pstVideoAttr->enVideoTiming == HDMI_VIDEO_TIMING_720X480I_60000 ||
        pstVideoAttr->enVideoTiming == HDMI_VIDEO_TIMING_720X576I_50000)
    {
        pstVideoAttr->u32PixelRepeat = 2;
    }

    if (pstVideoAttr->enVideoTiming <= HDMI_VIDEO_TIMING_640X480P_60000)
    {
        pstVideoAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_FULL;
    }
    else
    {
        pstVideoAttr->enYCCQuantization = HDMI_YCC_QUANTIZATION_RANGE_LIMITED;
    }

#if 0    
    if (pstVideoAttr->enVideoTiming != HDMI_VIDEO_TIMING_UNKNOWN &&
        pstVideoAttr->enVideoTiming != HDMI_VIDEO_TIMING_640X480P_60000 )
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_YCbCr444;
    }
    else
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_RGB;
    }
#else    
    pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_YCbCr444;
#endif

    pstVideoAttr->enStereoMode    = HDMI_3D_BUTT;
    pstVideoAttr->enInBitDepth    = HDMI_VIDEO_BITDEPTH_10;
    pstVideoAttr->enActiveAspect  = HDMI_ACTIVE_ASPECT_PICTURE;
}
#endif

static HI_S32 HdmiDeviceInit(HDMI_DEVICE_S * pstHdmiDev)
{
    HI_U32                   u32DispFmt = 0;
    HDMI_AO_ATTR_S*          pstAudioAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_APP_ATTR_S*         pstAppAttr;
    CHECK_POINTER(pstHdmiDev);

	HDMI_INIT_MUTEX(pstHdmiDev->stMutexThread);

    pstAppAttr   = &pstHdmiDev->stAttr.stAppAttr;
    pstAudioAttr = &pstHdmiDev->stAttr.stAOAttr;
    pstVideoAttr = &pstHdmiDev->stAttr.stVOAttr; 

    DRV_HDMI_DefaultActionSet(pstHdmiDev, HDMI_DEFAULT_ACTION_HDMI);

    /* user attribute init*/
    pstAppAttr->bEnableHdmi     = HI_TRUE;
    pstAppAttr->bEnableVideo    = HI_TRUE;
    pstAppAttr->bEnableAudio    = HI_TRUE;
    pstAppAttr->enDeepColorMode = HDMI_DEEP_COLOR_24BIT;
    pstAppAttr->enOutColorSpace = HDMI_COLORSPACE_YCbCr444;

    /* video attribute init*/
    pstVideoAttr->enVideoTiming = HDMI_VIDEO_TIMING_1280X720P_50000;

    if (pstHdmiDev->pstHdmiHal && pstHdmiDev->pstHdmiHal->HAL_HDMI_DispFmtGet)
    {
        pstHdmiDev->pstHdmiHal->HAL_HDMI_DispFmtGet(pstHdmiDev->pstHdmiHal, &u32DispFmt);
    }
    
    pstVideoAttr->enVideoTiming = DispFmt2HdmiTiming(pstHdmiDev, u32DispFmt);
  
    HdmiVOAttrInit(pstVideoAttr);

    /* audio attribute init*/
    pstAudioAttr->enSoundIntf   = HDMI_AUDIO_INTERFACE__I2S;
    pstAudioAttr->enSampleFs    = HDMI_SAMPLE_RATE_48K;
    pstAudioAttr->enSampleDepth = HDMI_AUDIO_BIT_DEPTH_24;
    pstAudioAttr->enChanels     = HDMI_AUDIO_FORMAT_2CH;
    HDMI_INFO("VideoTiming:%d\n",pstVideoAttr->enVideoTiming );
    //DRV_HDMI_EventInit(&pstHdmiDev->stEventInfo);
    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_Open(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bUser)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_BOOL bHdmiOn  = HI_FALSE;
    HI_BOOL bHotPLug = HI_FALSE;
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);
    
    /* create hdmi task ,every hdmi device only create once*/
    if (pstHdmiDev->stThreadInfo.kThreadTimer == NULL)
    {
        /* open Hal module*/
        s32Ret = HAL_HDMI_Open(HdmiEventCallback, &pstHdmiDev->pstHdmiHal);
        if (!pstHdmiDev->pstHdmiHal || s32Ret != HI_SUCCESS)
        {
             HDMI_ERR("HAL_HDMI_Open fail\n");
             return HI_FAILURE;
        }  

        pstHdmiDev->pstHdmiHal->stHalCtx.hHdmiDev = pstHdmiDev;
        pstHdmiDev->pstHdmiHal->stHalCtx.u32HdmiID = pstHdmiDev->u32HdmiDevId;
        
        HDMI_INFO("Create HDMI Timer Thread \n");
        pstHdmiDev->stThreadInfo.kThreadTimer = kthread_run(HdmiKThreadTimer, pstHdmiDev, "HI_HDMI_kThread");
        if (HI_NULL == pstHdmiDev->stThreadInfo.kThreadTimer)
        {
            HDMI_ERR("Create HDMI Timer Thread fail\n");
            return HI_FAILURE;
        }
        HdmiDeviceInit(pstHdmiDev);
        pstHdmiDev->enRunState = HDMI_RUN_STATE_OPEN;
       
    }

    DRV_HDMI_EventInit(&pstHdmiDev->stEventInfo);
   
    if (bUser)
    {
        pstHdmiDev->u32UserCnt++;
        /* Must Create Event Queue first */
        s32Ret = DRV_HDMI_EventPoolMalloc(&pstHdmiDev->stEventInfo, HI_NULL);
        if (s32Ret != HI_SUCCESS)
        {
            if (s32Ret == HDMI_EVENT_ID_EXIST)
            {
                pstHdmiDev->u32UserCnt--;
            }
            else
            {
                HDMI_ERR("DRV_HDMI_EventPoolMalloc fail\n");
                return HI_FAILURE;
           }
        }
        HDMI_INFO("Create Event Queue for process:%d \n",get_current()->tgid);

    }
    else
    {
        pstHdmiDev->u32KernelCnt++;
    }

    if (pstHdmiDev->u32UserCnt > 0)
    {
    	HDMI_MUTEX_UNLOCK(s_HdmiMutex);
		HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
        pstHdmiDev->pKCallback = HI_NULL;
		HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
		HDMI_MUTEX_LOCK(s_HdmiMutex);
    }
    else if (pstHdmiDev->u32KernelCnt > 0)
    {
		HDMI_MUTEX_UNLOCK(s_HdmiMutex);
		HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
		pstHdmiDev->pKCallback = HI_DRV_HDMI_KernelEventCallback;
		HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
		HDMI_MUTEX_LOCK(s_HdmiMutex);
    }

    /*20150513, l232354 STB boot have done it. But BVT have not done it in boot.
	  BVT work in boot need equa SBT work in boot later.
	*/
#if defined(CONFIG_HDMI_STB_SDK)
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyOutputEnableGet(pstHdmiDev->pstHdmiHal, &bHdmiOn);
#endif
    if (!bHdmiOn)
    {
         pstHdmiDev->enTransitionState = HDMI_TRANSITION_NONE;
         pstHdmiDev->pstHdmiHal->HAL_HDMI_HardwareInit(pstHdmiDev->pstHdmiHal); 
    }  
    else
    {
        if (bUser)
        {
            if (pstHdmiDev->u32KernelCnt > 0)//HDMI has opened in kernel
            {
                pstHdmiDev->enTransitionState = HDMI_TRANSITION_MCE_APP;
            }
            else
            {
                pstHdmiDev->enTransitionState = HDMI_TRANSITION_BOOT_APP;
            }
        }
        else
        {
            pstHdmiDev->enTransitionState = HDMI_TRANSITION_BOOT_MCE;
        }

        pstHdmiDev->pstHdmiHal->HAL_HDMI_HotPlugStatusGet(pstHdmiDev->pstHdmiHal, &bHotPLug);  
        if (bHotPLug)
        {
            HdmiEventCallback(pstHdmiDev, HDMI_EVENT_HOTPLUG);
        }
        else
        {
            HdmiEventCallback(pstHdmiDev, HDMI_EVENT_HOTUNPLUG);
        }  
        
    }
    
    /* only debug,delete it in future*/
    //pstHdmiDev->pstHdmiHal->HAL_HDMI_HardwareInit(pstHdmiDev->pstHdmiHal); 

    pstHdmiDev->pstHdmiHal->HAL_HDMI_IrqEnableSet(pstHdmiDev->pstHdmiHal, HDMI_INT_TYPE_HPD_RSEN, HI_TRUE);
    HDMI_INFO("TransitionState: %d\n",pstHdmiDev->enTransitionState);

    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}

static HI_S32 HdmiRelease(HDMI_DEVICE_S* pstHdmiDev) 
{
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);

    if (pstHdmiDev->enRunState & HDMI_RUN_STATE_START) 
    {
        DRV_HDMI_Stop(pstHdmiDev);
    }

    if (pstHdmiDev->u32UserCnt == 0)
    {
        DRV_HDMI_EventDeInit(&pstHdmiDev->stEventInfo);
    
        if (pstHdmiDev->u32KernelCnt == 0)
        {
            if (pstHdmiDev->stThreadInfo.kThreadTimer) 
            {
                HDMI_INFO("Stop hdmi kernel thread timer\n");
                kthread_stop(pstHdmiDev->stThreadInfo.kThreadTimer);
                pstHdmiDev->stThreadInfo.kThreadTimer = HI_NULL;
                DRV_HDMI_ThreadStateSet(pstHdmiDev, HDMI_THREAD_STATE_STOP);
            }
           
            if (pstHdmiDev->pstHdmiHal)
            {
                HAL_HDMI_Close(pstHdmiDev->pstHdmiHal);
            }    
            memset(pstHdmiDev, 0, sizeof(HDMI_DEVICE_S));
        }  
        else
        {
        //#if defined(CONFIG_HDMI_STB_SDK)  
         	HDMI_MUTEX_UNLOCK(s_HdmiMutex);
        	HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
            pstHdmiDev->pKCallback = HI_DRV_HDMI_KernelEventCallback;
			HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
            HDMI_MUTEX_LOCK(s_HdmiMutex);
        //#endif
        }
    }
    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_Close(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bUser)
{
    HI_S32 s32Ret = HI_FAILURE;
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);
    CHECK_HDMI_OPEN(pstHdmiDev->enRunState);
    
    if (bUser && pstHdmiDev->u32UserCnt > 0)
    {
        HDMI_INFO("Delete Event Queue for process:%d \n",get_current()->tgid);
        s32Ret = DRV_HDMI_EventPoolFree(&pstHdmiDev->stEventInfo, get_current()->tgid);
        if (s32Ret != HI_SUCCESS)
        {
            HDMI_ERR("DRV_HDMI_EventQueueAdd fail\n");
            return HI_FAILURE;
        }
        pstHdmiDev->u32UserCnt--;
//#if defined(CONFIG_HDMI_STB_SDK)
        if (pstHdmiDev->u32UserCnt == 0 && pstHdmiDev->u32KernelCnt > 0)
        {
            HDMI_MUTEX_UNLOCK(s_HdmiMutex);
        	HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
            pstHdmiDev->pKCallback = HI_DRV_HDMI_KernelEventCallback;
			HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
            HDMI_MUTEX_LOCK(s_HdmiMutex);
        }
		else
		{
			 HDMI_MUTEX_UNLOCK(s_HdmiMutex);
			 HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
			 pstHdmiDev->pKCallback = HI_NULL;
			 HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
             HDMI_MUTEX_LOCK(s_HdmiMutex);
                   			 
		}

//#endif        
    }
    else if (!bUser && pstHdmiDev->u32KernelCnt > 0)
    {
        pstHdmiDev->u32KernelCnt--;
    }
    
    //printk("u32UserCnt %d,u32KernelCnt %d\n",pstHdmiDev->u32UserCnt,pstHdmiDev->u32KernelCnt);
    if (pstHdmiDev->u32UserCnt == 0 && pstHdmiDev->u32KernelCnt == 0)
    {
        HdmiRelease(pstHdmiDev);
        pstHdmiDev->enRunState = HDMI_RUN_STATE_CLOSE;
    }
    HDMI_PRINT_OUT;
    return HI_SUCCESS;   
}

HI_S32 DRV_HDMI_Start(HDMI_DEVICE_S* pstHdmiDev)
{
    HDMI_APP_ATTR_S* pstAppAttr;
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    CHECK_HDMI_OPEN(pstHdmiDev->enRunState);
#if 0
    if ((pstHdmiDev->enRunState & HDMI_RUN_STATE_START)) 
    {
        return HI_SUCCESS;
    }
#endif
    pstAppAttr = &pstHdmiDev->stAttr.stAppAttr;
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyOutputEnableSet(pstHdmiDev->pstHdmiHal, HI_TRUE);
    DRV_HDMI_AVMuteSet(pstHdmiDev, HI_FALSE);
#ifdef HDMI_CEC_SUPPORT      
    CecSchedule(pstHdmiDev);
#endif 
#ifdef HDMI_HDCP_SUPPORT  
    pstHdmiDev->stHdcpInfo.bHdcpIsOn = pstHdmiDev->stAttr.stAppAttr.bHDCPEnable;
    if (HI_SUCCESS != HdcpSchedule(pstHdmiDev))
    {
        DRV_HDMI_AVMuteSet(pstHdmiDev, HI_TRUE);
        HdmiEventCallback(pstHdmiDev, HDMI_EVENT_HDCP_FAIL); 
    }
#endif    

    pstHdmiDev->enRunState &= ~HDMI_RUN_STATE_STOP;
    pstHdmiDev->enRunState |= HDMI_RUN_STATE_START;

               

    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_Stop(HDMI_DEVICE_S* pstHdmiDev)
{
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    if (!(pstHdmiDev->enRunState & HDMI_RUN_STATE_START) || 
        (pstHdmiDev->enRunState & HDMI_RUN_STATE_STOP)) 
    {
        return HI_SUCCESS;
    }
    
#ifdef HDMI_HDCP_SUPPORT  
    pstHdmiDev->stHdcpInfo.bHdcpIsOn = HI_FALSE;
    HdcpSchedule(pstHdmiDev);
    msleep(200);
#endif
#ifdef HDMI_CEC_SUPPORT      
    CecSchedule(pstHdmiDev);
#endif 
    DRV_HDMI_AVMuteSet(pstHdmiDev, HI_TRUE);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyOutputEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);

    pstHdmiDev->enRunState &= ~HDMI_RUN_STATE_START;
    pstHdmiDev->enRunState |= HDMI_RUN_STATE_STOP;

    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_AVMuteSet(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bAvMute)
{
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    /* fix compatibility problem*/
    if (!bAvMute)
    {   
        HI_U32 u32Delay = 0;
        FormatDelayGet(pstHdmiDev, &u32Delay);
        msleep(u32Delay);
    }
    
    pstHdmiDev->pstHdmiHal->HAL_HDMI_AvMuteSet(pstHdmiDev->pstHdmiHal, bAvMute);

    /* fix compatibility problem*/
    if (bAvMute)
    {
        HI_U32 u32Delay = 0;
        AvMuteDelayGet(pstHdmiDev, &u32Delay);
        msleep(u32Delay);   
    }
    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_DeepColorSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_DEEP_COLOR_E enDeepColor)
{
    HDMI_APP_ATTR_S*         pstAppAttr;
    
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal); 

    pstAppAttr =  &pstHdmiDev->stAttr.stAppAttr;
    pstAppAttr->enDeepColorMode = enDeepColor; 
    DRV_HDMI_AVMuteSet(pstHdmiDev, HI_TRUE);  
    DRV_HDMI_AttrSet(pstHdmiDev, &pstHdmiDev->stAttr);
    DRV_HDMI_AVMuteSet(pstHdmiDev, HI_FALSE);
    
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_DeepColorGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_DEEP_COLOR_E* enDeepColor)
{
    HDMI_APP_ATTR_S* pstAppAttr;
    CHECK_POINTER(pstHdmiDev);
    
    pstAppAttr =  &pstHdmiDev->stAttr.stAppAttr;
    *enDeepColor = pstAppAttr->enDeepColorMode;
    return HI_SUCCESS;
}

static HI_S32 CheckAudioAttr(HDMI_AO_ATTR_S *pstAOAttr)
{
    if (HDMI_AUDIO_INTERFACE__I2S != pstAOAttr->enSoundIntf && 
        HDMI_AUDIO_INTERFACE__SPDIF != pstAOAttr->enSoundIntf && 
        HDMI_AUDIO_INTERFACE__HBRA != pstAOAttr->enSoundIntf)
    {
        HDMI_ERR("The Audio interface(%d) is invalid\n",pstAOAttr->enSoundIntf);
        return HI_FAILURE;
    }

    if (pstAOAttr->enChanels < HDMI_AUDIO_FORMAT_2CH || 
        pstAOAttr->enChanels > HDMI_AUDIO_FORMAT_8CH)
    {
        HDMI_ERR("The Audio chanel number(%d) is invalid\n",pstAOAttr->enChanels);
        return HI_FAILURE;
    }
    
    switch (pstAOAttr->enSampleFs)
    {
        case HDMI_SAMPLE_RATE_32K:
        case HDMI_SAMPLE_RATE_44K:
        case HDMI_SAMPLE_RATE_48K:
        case HDMI_SAMPLE_RATE_88K:
        case HDMI_SAMPLE_RATE_96K:
        case HDMI_SAMPLE_RATE_176K:
        case HDMI_SAMPLE_RATE_192K:
        case HDMI_SAMPLE_RATE_768K:
            break;
        default:
            HDMI_ERR("The input Audio Frequency(%d) is invalid\n",pstAOAttr->enSampleFs);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 HdmiAudioPathSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_AO_ATTR_S *pstAOAttr)
{
    HDMI_AUDIO_CONFIG_S      stAudioCfg;
    
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstAOAttr);   
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    
    stAudioCfg.bDownSample   = pstAOAttr->bDownSample;
    stAudioCfg.enLayout      = pstAOAttr->enChanels;
    stAudioCfg.enSampleDepth = pstAOAttr->enSampleDepth;
    stAudioCfg.enSampleFs    = pstAOAttr->enSampleFs;
    stAudioCfg.enSoundIntf   = pstAOAttr->enSoundIntf;
	stAudioCfg.bEnableAudio  = pstHdmiDev->stAttr.stAppAttr.bEnableAudio;
    pstHdmiDev->pstHdmiHal->HAL_HDMI_AudioPathSet(pstHdmiDev->pstHdmiHal, &stAudioCfg);

    return HI_SUCCESS;
    
}

static HI_BOOL AOAttrIsChanged(HDMI_AO_ATTR_S* pstHwAOAttr, HDMI_AO_ATTR_S* pstSwAOAttr)
{
    if (pstHwAOAttr == HI_NULL || pstSwAOAttr == HI_NULL)
    {
        HDMI_ERR("The input pointer is null\n");
        return HI_FALSE;
    }

    if (pstHwAOAttr->enSoundIntf != pstSwAOAttr->enSoundIntf)
    {
        HDMI_INFO("audio interface change,old(%d)->new(%d)\n",pstHwAOAttr->enSoundIntf, pstSwAOAttr->enSoundIntf);
        return HI_TRUE;
    }
    
    if (pstHwAOAttr->enSampleFs != pstSwAOAttr->enSampleFs)
    {
        HDMI_INFO("audio SampleFs change,old(%d)->new(%d)\n",pstHwAOAttr->enSampleFs, pstSwAOAttr->enSampleFs);
        return HI_TRUE;
    }
    
    if (pstHwAOAttr->enSampleDepth != pstSwAOAttr->enSampleDepth)
    {
        HDMI_INFO("audio enSampleDepth change,old(%d)->new(%d)\n",pstHwAOAttr->enSampleDepth, pstSwAOAttr->enSampleDepth);
        return HI_TRUE;
    }   
    
    if (pstHwAOAttr->enAudioCode != pstSwAOAttr->enAudioCode)
    {
        HDMI_INFO("audio enAudioCode change,old(%d)->new(%d)\n",pstHwAOAttr->enAudioCode, pstSwAOAttr->enAudioCode);
        return HI_TRUE;
    }  
    
    if (pstHwAOAttr->enChanels != pstSwAOAttr->enChanels)
    {
        HDMI_INFO("audio enChanels change,old(%d)->new(%d)\n",pstHwAOAttr->enChanels, pstSwAOAttr->enChanels);
        return HI_TRUE;
    }
    
    if (pstHwAOAttr->bDownSample != pstSwAOAttr->bDownSample)
    {
        HDMI_INFO("audio bDownSample change,old(%d)->new(%d)\n",pstHwAOAttr->bDownSample, pstSwAOAttr->bDownSample);
        return HI_TRUE;
    }
    return HI_FALSE;
}

HI_S32 DRV_HDMI_AOAttrSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_AO_ATTR_S *pstAOAttr)
{
    HDMI_APP_ATTR_S*         pstAppAttr;
    HDMI_AO_ATTR_S*          pstAudioAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_ATTR_S              stHwAttr;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstAOAttr);
   
    if (HI_SUCCESS != CheckAudioAttr(pstAOAttr))
    {
        HDMI_ERR("HDMI_CheckAudioAttr fail\n");
        return HI_FAILURE;
    }
    pstAppAttr   = &pstHdmiDev->stAttr.stAppAttr;
    pstVideoAttr = &pstHdmiDev->stAttr.stVOAttr;
    pstAudioAttr = &pstHdmiDev->stAttr.stAOAttr;
    memcpy(pstAudioAttr, pstAOAttr, sizeof(HDMI_AO_ATTR_S));

    memset(&stHwAttr, 0, sizeof(HDMI_ATTR_S));
    HdmiAttrConstruct(pstHdmiDev, &stHwAttr);

    if (!AOAttrIsChanged(&stHwAttr.stAOAttr, pstAOAttr) &&
        !VOAttrIsChanged(&stHwAttr.stVOAttr, pstVideoAttr) &&
        !APPAttrIsChanged(&stHwAttr.stAppAttr, pstAppAttr) &&
        pstAppAttr->bEnableAudio == stHwAttr.stAppAttr.bEnableAudio &&
        pstAppAttr->bEnableAudInfoFrame == stHwAttr.stAppAttr.bEnableAudInfoFrame)
    {
        HDMI_INFO("The audio attr is not changed\n");
        return HI_SUCCESS;
    }
    
    //if (pstAppAttr->bEnableAudio)
    {
        HdmiAudioPathSet(pstHdmiDev, pstAOAttr);
    }
    
    DRV_HDMI_AudioInfoFrameSend(&pstHdmiDev->stInfoFrame, (pstAppAttr->bEnableAudio && pstAppAttr->bEnableAudInfoFrame));
 
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_AOAttrGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_AO_ATTR_S* pstAOAttr)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstAOAttr);  
    
    memcpy(pstAOAttr, &pstHdmiDev->stAttr.stAOAttr, sizeof(HDMI_AO_ATTR_S));
    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_AudioOutputEnableSet(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bEnable)
{
    HI_BOOL bMute = bEnable ? HI_FALSE : HI_TRUE;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    
    pstHdmiDev->pstHdmiHal->HAL_HDMI_AudioMuteSet(pstHdmiDev->pstHdmiHal, bMute);
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_ThreadStateSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_THREAD_STATE_E enState)
{
    CHECK_POINTER(pstHdmiDev);
    pstHdmiDev->stThreadInfo.kThreadTimerSate = enState;
    return HI_SUCCESS;
}

static HI_S32 CheckVideoAttr(HDMI_VO_ATTR_S* pstVOAttr)
{
    CHECK_POINTER(pstVOAttr);
    
    if (pstVOAttr->enVideoTiming >= HDMI_VIDEO_TIMING_BUTT)
    {
        HDMI_ERR("Hdmi video timing(%d) is wrong\n", pstVOAttr->enVideoTiming);
        return HI_FAILURE;
    }
    
    if (pstVOAttr->enHvSyncPol > HDMI_HV_SYNC_POL_HNVN)
    {
        HDMI_ERR("Hdmi video HvSyncPol(%d) is wrong\n", pstVOAttr->enHvSyncPol);
        return HI_FAILURE;
    }

    if (pstVOAttr->enInColorSpace >= HDMI_COLORSPACE_BUTT)
    {
        HDMI_ERR("Hdmi video InColorSpace(%d) is wrong\n", pstVOAttr->enInColorSpace);
        return HI_FAILURE;
    }

    if (pstVOAttr->enColorimetry <= HDMI_COLORIMETRY__NONE ||
        pstVOAttr->enColorimetry > HDMI_COLORIMETRY__EXTENDED)
    {
        HDMI_ERR("Hdmi video Colorimetry(%d) is wrong\n", pstVOAttr->enColorimetry);
        return HI_FAILURE;
    }

    if (pstVOAttr->enRGBQuantization > HDMI_QUANTIZATION_RANGE_FULL)
    {
        HDMI_ERR("Hdmi video RGBQuantization(%d) is wrong\n", pstVOAttr->enRGBQuantization);
        return HI_FAILURE;
    }

    if (pstVOAttr->enPictureAspect < HDMI_PICTURE_ASPECT_4_3 ||
        pstVOAttr->enPictureAspect > HDMI_PICTURE_ASPECT_16_9)
    {
        HDMI_ERR("Hdmi video PictureAspect(%d) is wrong\n", pstVOAttr->enPictureAspect);
        return HI_FAILURE;
    }

    if (pstVOAttr->enInBitDepth < HDMI_VIDEO_BITDEPTH_8 ||
        pstVOAttr->enInBitDepth > HDMI_VIDEO_BITDEPTH_12)
    {
        HDMI_ERR("Hdmi video InBitDepth(%d) is wrong\n", pstVOAttr->enInBitDepth);
        return HI_FAILURE;
    }
        
    return HI_SUCCESS;
}

static HI_S32 HdmiVideoPathSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_VO_ATTR_S* pstVOAttr)
{
    HDMI_VIDEO_CONFIG_S stVideoCfg = {0};
    HDMI_APP_ATTR_S*    pstAppAttr;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstVOAttr);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    
    pstAppAttr   = &pstHdmiDev->stAttr.stAppAttr;
    
    stVideoCfg.u32PixelClk     = pstVOAttr->u32ClkFs;
    stVideoCfg.enInBitDepth    = pstVOAttr->enInBitDepth;
    stVideoCfg.enHvSyncPol     = pstVOAttr->enHvSyncPol;
    stVideoCfg.enQuantization  = pstVOAttr->enRGBQuantization;
    stVideoCfg.enInColorSpace  = /*pstVOAttr->bYUV420Enable ? HDMI_COLORSPACE_YCbCr420 : */pstVOAttr->enInColorSpace;
    stVideoCfg.enDeepColor     = pstAppAttr->enDeepColorMode;
    stVideoCfg.enOutColorSpace = pstAppAttr->bYUV420Enable ? HDMI_COLORSPACE_YCbCr420 : pstAppAttr->enOutColorSpace;
    stVideoCfg.enOutColorSpace = pstAppAttr->bEnableHdmi ? stVideoCfg.enOutColorSpace : HDMI_COLORSPACE_RGB;
    HDMI_INFO("InColorSpace is %d,OutColorSpace is %d\n",stVideoCfg.enInColorSpace,stVideoCfg.enOutColorSpace);
    switch (pstVOAttr->enColorimetry)
    {
        case HDMI_COLORIMETRY__ITU_601:
            stVideoCfg.enConvStd = HDMI_CONV_STD_BT_601;
            break;
        case HDMI_COLORIMETRY__ITU_709:
            stVideoCfg.enConvStd = HDMI_CONV_STD_BT_709;
            break;              
        case HDMI_COLORIMETRY__EXTENDED:
            if (pstVOAttr->enExtendedColorimetry == HDMI_EXTENDED_COLORIMETRY_2020_non_const_luminous)
                stVideoCfg.enConvStd = HDMI_CONV_STD_BT_2020_non_const_luminous;
            else if (pstVOAttr->enExtendedColorimetry == HDMI_EXTENDED_COLORIMETRY_2020_const_luminous)
                stVideoCfg.enConvStd = HDMI_CONV_STD_BT_2020_const_luminous;
            break;   
        default:
            stVideoCfg.enConvStd = HDMI_CONV_STD_BT_709;
            break;      
    }
    
    pstHdmiDev->pstHdmiHal->HAL_HDMI_VideoPathSet(pstHdmiDev->pstHdmiHal, &stVideoCfg);
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_VideoOutputEnableSet(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bEnable)
{
    HI_BOOL bMute = bEnable ? HI_FALSE : HI_TRUE;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    
    pstHdmiDev->pstHdmiHal->HAL_HDMI_VideoMuteSet(pstHdmiDev->pstHdmiHal, bMute);
    return HI_SUCCESS;
}

static HI_BOOL APPAttrIsChanged(HDMI_APP_ATTR_S* pstHwAPPAttr, HDMI_APP_ATTR_S* pstSwAPPAttr)
{
    if (pstHwAPPAttr == HI_NULL || pstSwAPPAttr == HI_NULL)
    {
        HDMI_ERR("The input pointer is null\n");
        return HI_FALSE;
    }
    
    if (pstHwAPPAttr->bEnableHdmi != pstSwAPPAttr->bEnableHdmi)
    {
        HDMI_INFO("tmds mode change\n");
        return HI_TRUE;
    }  
    
    if (pstSwAPPAttr->bYUV420Enable)       
    {
        if (pstHwAPPAttr->enOutColorSpace != HDMI_COLORSPACE_YCbCr420)
        {
            HDMI_INFO("out colorSpace change, out colorSpace switch to YUV420\n");
            return HI_TRUE;
        }

    }
    else if (pstHwAPPAttr->enOutColorSpace != pstSwAPPAttr->enOutColorSpace)
    {
        HDMI_INFO("out colorSpace change,old(%d)->new(%d)\n",pstHwAPPAttr->enOutColorSpace, pstSwAPPAttr->enOutColorSpace);
        return HI_TRUE;
    }

    if (pstHwAPPAttr->enDeepColorMode != pstSwAPPAttr->enDeepColorMode)
    {
        HDMI_INFO("deepcolor mode change,old(%d)->new(%d)\n",pstHwAPPAttr->enDeepColorMode, pstSwAPPAttr->enDeepColorMode);
        return HI_TRUE;
    }

    if (pstHwAPPAttr->bxvYCCMode != pstSwAPPAttr->bxvYCCMode)
    {
        HDMI_INFO("bxvYCCMode mode change,old(%d)->new(%d)\n",pstHwAPPAttr->bxvYCCMode, pstSwAPPAttr->bxvYCCMode);
        return HI_TRUE;
    }

    return HI_FALSE;
}

static HI_BOOL VOAttrIsChanged(HDMI_VO_ATTR_S* pstHwVOAttr, HDMI_VO_ATTR_S* pstSwVOAttr)
{
    if (pstHwVOAttr == HI_NULL || pstSwVOAttr == HI_NULL)
    {
        HDMI_ERR("The input pointer is null\n");
        return HI_FALSE;
    }
    
    if (pstHwVOAttr->enVideoTiming != pstSwVOAttr->enVideoTiming)
    {
        HDMI_INFO("video timing change,old(%d)->new(%d)\n",pstHwVOAttr->enVideoTiming,pstSwVOAttr->enVideoTiming);
        return HI_TRUE;
    }

    if (pstHwVOAttr->enInColorSpace != pstSwVOAttr->enInColorSpace)
    {
        HDMI_INFO("input colorspace change,old(%d)->new(%d)\n",pstHwVOAttr->enInColorSpace,pstSwVOAttr->enInColorSpace);
        return HI_TRUE;
    }

    if (pstHwVOAttr->enStereoMode != pstSwVOAttr->enStereoMode)
    {
        HDMI_INFO("3d mode change,old(%d)->new(%d)\n",pstHwVOAttr->enStereoMode, pstSwVOAttr->enStereoMode);
        return HI_TRUE;
    }
    
    if (pstHwVOAttr->u32PixelRepeat != pstSwVOAttr->u32PixelRepeat)
    {
        HDMI_INFO("pixel repeation change,old(%d)->new(%d)\n",pstHwVOAttr->u32PixelRepeat, pstSwVOAttr->u32PixelRepeat);
        return HI_TRUE;
    }

    if (pstHwVOAttr->enHvSyncPol != pstSwVOAttr->enHvSyncPol)
    {
        HDMI_INFO("hsync sync polarity change,old(%d)->new(%d)\n",pstHwVOAttr->enHvSyncPol, pstSwVOAttr->enHvSyncPol);
        return HI_TRUE;
    }
    
    if (pstHwVOAttr->enColorimetry != pstSwVOAttr->enColorimetry)
    {
        HDMI_INFO("colorimetry change,old(%d)->new(%d)\n",pstHwVOAttr->enColorimetry, pstSwVOAttr->enColorimetry);
        return HI_TRUE;
    }

    if (pstHwVOAttr->enRGBQuantization != pstSwVOAttr->enRGBQuantization)
    {
        HDMI_INFO("RGB quantization change,old(%d)->new(%d)\n",pstHwVOAttr->enRGBQuantization, pstSwVOAttr->enRGBQuantization);
        return HI_TRUE;
    }
    
    if (pstHwVOAttr->enYCCQuantization != pstSwVOAttr->enYCCQuantization)
    {
        HDMI_INFO("RGB quantization change,old(%d)->new(%d)\n",pstHwVOAttr->enYCCQuantization, pstSwVOAttr->enYCCQuantization);
        return HI_TRUE;
    }

    if (pstHwVOAttr->enPictureAspect != pstSwVOAttr->enPictureAspect)
    {
        HDMI_INFO("picture aspect change,old(%d)->new(%d)\n",pstHwVOAttr->enPictureAspect, pstSwVOAttr->enPictureAspect);
        return HI_TRUE;
    }
   
    return HI_FALSE;
}


HI_S32 DRV_HDMI_VOAttrSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_VO_ATTR_S* pstVOAttr)
{
    HDMI_APP_ATTR_S*         pstAppAttr;
    HDMI_ATTR_S              stHwAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    //HDMI_TMDS_MODE_E         enTmdsMode;
    
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstVOAttr);

    pstAppAttr =  &pstHdmiDev->stAttr.stAppAttr;
       
    if (HI_SUCCESS != CheckVideoAttr(pstVOAttr))
    {
        HDMI_ERR("HDMI_CheckVideoAttr fail\n");
        return HI_FAILURE;
    }
    
    pstVideoAttr = &pstHdmiDev->stAttr.stVOAttr;
    memcpy(pstVideoAttr, pstVOAttr, sizeof(HDMI_VO_ATTR_S));

    //YUV420Schedule(pstHdmiDev);
    HdmiModeSchedule(pstHdmiDev);
    
    memset(&stHwAttr, 0, sizeof(HDMI_ATTR_S));
    HdmiAttrConstruct(pstHdmiDev, &stHwAttr);
    if (!VOAttrIsChanged(&stHwAttr.stVOAttr, pstVideoAttr) &&
        !APPAttrIsChanged(&stHwAttr.stAppAttr, pstAppAttr))
    {
        HDMI_INFO("The video and app attr is not changed\n");
        return HI_SUCCESS;
    }  

    if (pstAppAttr->bEnableVideo)
    {   
        HdmiVideoPathSet(pstHdmiDev, pstVideoAttr);
    }

    DRV_HDMI_GBDPackSend(&pstHdmiDev->stInfoFrame,(pstAppAttr->bEnableHdmi && pstAppAttr->bxvYCCMode));
    DRV_HDMI_AVIInfoFrameSend(&pstHdmiDev->stInfoFrame,(pstAppAttr->bEnableHdmi && pstAppAttr->bEnableAviInfoFrame));//pstAppAttr->bEnableAviInfoFrame);
    DRV_HDMI_VendorInfoFrameSend(&pstHdmiDev->stInfoFrame, pstAppAttr->bEnableHdmi);  
  
   
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_VOAttrGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_VO_ATTR_S* pstVOAttr)
{
	CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstVOAttr);  
    
    memcpy(pstVOAttr, &pstHdmiDev->stAttr.stVOAttr, sizeof(HDMI_VO_ATTR_S));
    return HI_SUCCESS;
}



HI_S32 DRV_HDMI_AttrGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_ATTR_S *pstAttr)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstAttr);
    
    memcpy(pstAttr, &pstHdmiDev->stAttr, sizeof(HDMI_ATTR_S));
    return HI_SUCCESS;
}

static HI_S32 CheckAppAttr(HDMI_DEVICE_S* pstHdmiDev, HDMI_APP_ATTR_S* pstAppAttr)
{
    HDMI_TX_CAPABILITY_E     enTxCapability;
    CHECK_POINTER(pstAppAttr);

    pstHdmiDev->pstHdmiHal->HAL_HDMI_TxCapabilityGet(pstHdmiDev->pstHdmiHal, &enTxCapability);  
    
    if (pstAppAttr->enOutColorSpace >= HDMI_COLORSPACE_BUTT)
    {
        HDMI_ERR("Hdmi OutColorSpace(%d) is wrong\n", pstAppAttr->enOutColorSpace);
        return HI_FAILURE;
    }
    else if (pstAppAttr->enOutColorSpace == HDMI_COLORSPACE_YCbCr420)
    {
       if (!(enTxCapability & HDMI_TX_SUPPORT_YUV420))
       {
            HDMI_ERR("Hdmi TX not support YUV420\n");
            return HI_FAILURE;
       }
    }
           
    return HI_SUCCESS;
    
}

HI_S32 DRV_HDMI_AttrSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_ATTR_S *pstAttr)
{
    HDMI_AO_ATTR_S*          pstAudioAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_APP_ATTR_S*         pstAppAttr; 
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    CHECK_POINTER(pstAttr);

    if (HI_SUCCESS != CheckAppAttr(pstHdmiDev, &pstAttr->stAppAttr))
    {
        HDMI_ERR("Check hdmi attr fail\n");
        return HI_FAILURE;
    }
   
    pstAudioAttr = &pstHdmiDev->stAttr.stAOAttr;
    pstVideoAttr = &pstHdmiDev->stAttr.stVOAttr;
    pstAppAttr   = &pstHdmiDev->stAttr.stAppAttr;
    memcpy(&pstHdmiDev->stAttr.stAppAttr, &pstAttr->stAppAttr, sizeof(HDMI_APP_ATTR_S)); 
   
#ifdef HDMI_HDCP_SUPPORT
    pstHdmiDev->stHdcpInfo.bHdcpIsOn = pstAppAttr->bHDCPEnable;
#endif    

    if (HI_SUCCESS != DRV_HDMI_AOAttrSet(pstHdmiDev, &pstAttr->stAOAttr))
    {
        HDMI_ERR("DRV_HDMI_AOAttrSet fail\n");
        return HI_FAILURE;
    }

    if (HI_SUCCESS != DRV_HDMI_VOAttrSet(pstHdmiDev, &pstAttr->stVOAttr))
    {
        HDMI_ERR("DRV_HDMI_VOAttrSet fail\n");
        return HI_FAILURE;
    }
  
    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_InfoFrameGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_INFOFRAME_ID_E enInfoFrameId, HDMI_INFOFRAME_U *puInfoFrame)
{
    CHECK_POINTER(pstHdmiDev);
    switch (enInfoFrameId)
    {
        case HDMI_INFOFRAME_TYPE_VENDOR:
            memcpy(&puInfoFrame->stVendorInfoFrame, &pstHdmiDev->stInfoFrame.stVendorInfoFrame, sizeof(HDMI_VENDOR_INFOFRAME_S));
            break;
        case HDMI_INFOFRAME_TYPE_AVI:
            memcpy(&puInfoFrame->stAVIInfoFrame, &pstHdmiDev->stInfoFrame.stAVIInfoFrame, sizeof(HDMI_AVI_INFOFRAME_S));
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO:
            memcpy(&puInfoFrame->stAudioInfoFrame, &pstHdmiDev->stInfoFrame.stAudioInfoFrame, sizeof(HDMI_AUDIO_INFOFRAME_S));
            break;
        default:
            return HI_FAILURE;           
    }
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_InfoFrameSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_INFOFRAME_ID_E enInfoFrameId, HDMI_INFOFRAME_U *puInfoFrame)
{
    CHECK_POINTER(pstHdmiDev);
    switch (enInfoFrameId)
    {
        case HDMI_INFOFRAME_TYPE_VENDOR:
            memcpy(&pstHdmiDev->stInfoFrame.stVendorInfoFrame, &puInfoFrame->stVendorInfoFrame, sizeof(HDMI_VENDOR_INFOFRAME_S));
            break;
        case HDMI_INFOFRAME_TYPE_AVI:
            memcpy(&pstHdmiDev->stInfoFrame.stAVIInfoFrame, &puInfoFrame->stAVIInfoFrame, sizeof(HDMI_AVI_INFOFRAME_S));
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO:
            memcpy(&pstHdmiDev->stInfoFrame.stAudioInfoFrame, &puInfoFrame->stAudioInfoFrame, sizeof(HDMI_AUDIO_INFOFRAME_S));
            break;
        default:
            return HI_FAILURE;           
    }
    DRV_HDMI_InfoFrameSend(&pstHdmiDev->stInfoFrame, enInfoFrameId, puInfoFrame);
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_StatusGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_STATUS_S *pHdmiStatus)
{
    HDMI_HARDWARE_STATUS_S stHwStatus;
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiDev);  
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    
    pstHdmiDev->pstHdmiHal->HAL_HDMI_HardwareStatusGet(pstHdmiDev->pstHdmiHal, &stHwStatus);
    /* Some TV sometimes has no hotPlug but has rsen*/
    pHdmiStatus->bConnected = (stHwStatus.stCommonStatus.bHotPlug || stHwStatus.stCommonStatus.bRsen);
    pHdmiStatus->bSinkPowerOn = stHwStatus.stCommonStatus.bRsen;
    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}

#ifdef HDMI_CEC_SUPPORT
HI_S32 DRV_HDMI_CecCmdSend(HDMI_DEVICE_S* pstHdmiDev,  HDMI_CEC_CMD_S  *pstCecCmd)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_CecMsgSend(pstHdmiDev->pstHdmiHal, pstCecCmd);
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_CecCmdReceive(HDMI_DEVICE_S* pstHdmiDev, HDMI_CEC_CMD_S  *pstCecCmd)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    return pstHdmiDev->pstHdmiHal->HAL_HDMI_CecMsgReceive(pstHdmiDev->pstHdmiHal, pstCecCmd); 
}

HI_S32 DRV_HDMI_CecStatusGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_CEC_INFO_S  *pstCecStatus)
{
    CHECK_POINTER(pstHdmiDev);  
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    memcpy(pstCecStatus, &pstHdmiDev->stCecInfo, sizeof(HDMI_CEC_INFO_S));
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_CecEnableSet(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bEnable)
{
    CHECK_POINTER(pstHdmiDev);  
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    pstHdmiDev->stCecInfo.bCecIsOn = bEnable;
    return CecSchedule(pstHdmiDev);
}

#endif


#ifdef HDMI_HDCP_SUPPORT
HI_S32 DRV_HDMI_KeyLoad(HDMI_DEVICE_S* pstHdmiDev, HDMI_LOAD_KEY_S *pstLoadKey)
{
    HI_S32 s32Ret = HI_SUCCESS;    
    HI_DRV_CIPHER_FLASH_ENCRYPT_HDCPKEY_S stFlashEncrytedHdcpKey;
    CIPHER_EXPORT_FUNC_S *g_stCipherExtFunc = HI_NULL;

    CHECK_POINTER(pstHdmiDev);
    s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_CIPHER, (HI_VOID**)&g_stCipherExtFunc);
    if((NULL == g_stCipherExtFunc) || (s32Ret != HI_SUCCESS))
    {
        HDMI_ERR("Get cipher functions failed!\n");
        return HI_FAILURE;
    }

    if((HI_NULL != g_stCipherExtFunc)
     &&(HI_NULL != g_stCipherExtFunc->pfnCipherLoadHdcpKey))
    {
        /*load hdcp key */
        memcpy(stFlashEncrytedHdcpKey.u8Key, pstLoadKey->pu8InputEncryptedKey, pstLoadKey->u32KeyLength);
#if 1
        stFlashEncrytedHdcpKey.u32KeyLen = pstLoadKey->u32KeyLength;
        stFlashEncrytedHdcpKey.enHDCPKeyType = HI_DRV_HDCPKEY_TX0;
        stFlashEncrytedHdcpKey.enHDCPVersion = HI_DRV_HDCP_VERIOSN_1x;
#endif       
        s32Ret = (g_stCipherExtFunc->pfnCipherLoadHdcpKey)(&stFlashEncrytedHdcpKey);
        if( HI_SUCCESS != s32Ret)
        {
            HDMI_ERR("Load hdcp key error!\n");
            return HI_FAILURE;
        }
        HDMI_INFO("Load hdcp key successful!\n");
    }
    pstHdmiDev->stHdcpInfo.bHdcp1Prep = HI_TRUE;
    //HdcpSchedule(pstHdmiDev);
    return s32Ret;

}

#endif


HI_S32 DRV_HDMI_PlayStatusGet(HDMI_DEVICE_S* pstHdmiDev, HI_U32 *pu32PlayStatus)
{
    CHECK_POINTER(pstHdmiDev);

    if (pstHdmiDev->enRunState & HDMI_RUN_STATE_START)
    {
        *pu32PlayStatus = 1;
    }
    else
    {
        *pu32PlayStatus = 0;
    }

    return HI_SUCCESS;
}

HDMI_DEFAULT_ACTION_E DRV_HDMI_DefaultActionGet(HDMI_DEVICE_S* pstHdmiDev)
{
    CHECK_POINTER(pstHdmiDev);
    return pstHdmiDev->stAttr.stAppAttr.enHdmiAction;
}

HI_S32 DRV_HDMI_DefaultActionSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_DEFAULT_ACTION_E enAction)
{
    CHECK_POINTER(pstHdmiDev);
    pstHdmiDev->stAttr.stAppAttr.enHdmiAction = enAction;
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_DelayGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_DELAY_S *pHdmiDelay)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pHdmiDelay);

    memcpy(pHdmiDelay, &pstHdmiDev->stDelay, sizeof(HDMI_DELAY_S));
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_DelaySet(HDMI_DEVICE_S* pstHdmiDev, HDMI_DELAY_S *pHdmiDelay)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pHdmiDelay);

    memcpy(&pstHdmiDev->stDelay, pHdmiDelay, sizeof(HDMI_DELAY_S));
    return HI_SUCCESS;
}

static HI_VOID HdmiSetAndroidState(HDMI_SWITCH_STATE enState)
{
#ifdef HDMI_ANDROID_SUPPORT
    if(s_bIsSwitchOn == HI_TRUE) 
    {
        switch_set_state(&s_stSwitchDev, enState);
    }
#endif
}


static HI_S32 HdmiFileOpen(struct inode *inode, struct file *filp)
{
#if (HDMI_CHIP_HI3536 != HICHIP)
    atomic_inc_return(&s_HdmiCount);
    HDMI_INFO("s_HdmiCount:%d\n", s_HdmiCount);

#endif
    return HI_SUCCESS;
}

static HI_S32 HdmiFileClose(struct inode *inode, struct file *filp)
{
#if (HDMI_CHIP_HI3536 != HICHIP)
    HI_U32 u32HdmiId = 0;
    HDMI_DEVICE_S* pstHdmiDev;

    if (atomic_dec_and_test(&s_HdmiCount))
    {
        for(; u32HdmiId < HDMI_DEVICE_ID_BUTT; u32HdmiId++)
        {
            pstHdmiDev = GetHdmiDevice(u32HdmiId);
            if (pstHdmiDev != HI_NULL)
            {
                pstHdmiDev->u32UserCnt = 0;
                if (pstHdmiDev->enRunState & HDMI_RUN_STATE_OPEN)
                {
                    HdmiRelease(pstHdmiDev);
                }
                else
                {
                    HDMI_INFO("enRunState:%d of u32HdmiId[%d] is not open!\n", pstHdmiDev->enRunState, u32HdmiId);
                }
            }
            else
            {
                HDMI_ERR("pstHdmiDev[%d] is null!\n", u32HdmiId);
            }
        }
    }
    else
    {
        HDMI_INFO("s_HdmiCount:%d\n", s_HdmiCount);
    }
#endif
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_CmdProcess(unsigned int cmd, HI_VOID *arg, HI_BOOL bUser)
{
    HI_S32       s32Ret = HI_FAILURE; 
    HI_VOID __user *argp = (HI_VOID __user *)arg;
    HDMI_PRINT_IN;
    //printk("cmd=0x%x,proc_id=%d\n",cmd,get_current()->tgid);

    HDMI_MUTEX_LOCK(s_HdmiMutex);
           

    switch (cmd)
    {
        case CMD_HDMI_INIT:
        {
            s32Ret = DRV_HDMI_Init();
            break;
        }

        case CMD_HDMI_DEINIT:
        {
            s32Ret = DRV_HDMI_DeInit();
            break;
        }

        case CMD_HDMI_OPEN:
        {       
            HDMI_OPEN_S* pstOpen = (HDMI_OPEN_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstOpen->enHdmiID); 
            DRV_HDMI_DefaultActionSet(pstHdmiDev, pstOpen->enDefaultMode);
            HDMI_MUTEX_UNLOCK(s_HdmiMutex);
            s32Ret = DRV_HDMI_Open(pstHdmiDev, bUser);
            HDMI_MUTEX_LOCK(s_HdmiMutex);
            break;
        }

        case CMD_HDMI_CLOSE:
        {     
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(*(HDMI_DEVICE_ID_E*)argp); 
            s32Ret = DRV_HDMI_Close(pstHdmiDev, bUser);
            break;
        }

        case CMD_HDMI_START:
        {
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(*(HDMI_DEVICE_ID_E*)argp);           
            s32Ret = DRV_HDMI_Start(pstHdmiDev);
            break;
        }

        case CMD_HDMI_STOP:
        {
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(*(HDMI_DEVICE_ID_E*)argp);            
            s32Ret = DRV_HDMI_Stop(pstHdmiDev);
            break;
        }

        case CMD_HDMI_GET_AUDIO_CAPABILITY:
        {
            HI_U32                      i = 0;
            HI_U32                      u32AudFmt = 0;
            HDMI_SINK_CAPABILITY_S     *pstSinkCap;
            DRV_HDMI_AUDIO_CAPABILITY_S* pstDrvAudioCap = (DRV_HDMI_AUDIO_CAPABILITY_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvAudioCap->enHdmiID);        
            s32Ret = DRV_HDMI_EdidCapabilityGet(&pstHdmiDev->stEdidInfo, &pstSinkCap);
            
            HDMI_MEMSET(pstDrvAudioCap,0,sizeof(DRV_HDMI_AUDIO_CAPABILITY_S));

            if (s32Ret == HDMI_EDID_DATA_VALIDSINK)
            {
                for (i = 0 ;i < pstSinkCap->u32AudioInfoNum ; i++)
                {
                    u32AudFmt = pstSinkCap->stAudioInfo[i].enAudFmtCode;
                    
                    if (u32AudFmt < HDMI_ARRAY_SIZE(pstDrvAudioCap->stAudioCap.bAudioFmtSupported))
                    {
                        pstDrvAudioCap->stAudioCap.bAudioFmtSupported[u32AudFmt] = HI_TRUE;
                    }
                    
                    if ( HDMI_AUDIO_CODING_TYPE_PCM == u32AudFmt )
                    {
                        HDMI_MEMCPY(pstDrvAudioCap->stAudioCap.u32AudioSampleRateSupported,
                                    pstSinkCap->stAudioInfo[i].enSupportSampleRate,
                                    sizeof(pstDrvAudioCap->stAudioCap.u32AudioSampleRateSupported) );
                        pstDrvAudioCap->stAudioCap.u32MaxPcmChannels = pstSinkCap->stAudioInfo[i].u8AudChannel;
                    }
                }
                s32Ret = HI_SUCCESS;
            }
            else
            {
                HDMI_WARN("No HPD,Get Auido capabity fail\n");
                s32Ret = HI_FAILURE;
            }

            break;
        }
        case CMD_HDMI_GET_SINK_CAPABILITY:
        {
            HDMI_SINK_CAPABILITY_S     *pstSinkCap;
            DRV_HDMI_SINK_CAPABILITY_S* pstDrvSinkCap = (DRV_HDMI_SINK_CAPABILITY_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvSinkCap->enHdmiID);        
            s32Ret = DRV_HDMI_EdidCapabilityGet(&pstHdmiDev->stEdidInfo, &pstSinkCap); 
            if (s32Ret == HDMI_EDID_DATA_VALIDSINK)
            {
                HDMI_MEMCPY(&pstDrvSinkCap->stCap, pstSinkCap, sizeof(HDMI_SINK_CAPABILITY_S));
                s32Ret = HI_SUCCESS;
            }
            else
            {
                HDMI_WARN("No HPD,Get sink capabity fail\n");
                s32Ret = HI_FAILURE;
            }
            break;
        }

        case CMD_HDMI_POLL_EVENT:
        {
            DRV_HDMI_EVENT_S *pstEvent = (DRV_HDMI_EVENT_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstEvent->enHdmiID); 
            HDMI_MUTEX_UNLOCK(s_HdmiMutex);
            s32Ret = DRV_HDMI_EventPoolRead(&pstHdmiDev->stEventInfo, get_current()->tgid, &pstEvent->enEvent); 
 
            HDMI_MUTEX_LOCK(s_HdmiMutex);
                   
            break;
        }

        case CMD_HDMI_GET_ATTR:
        {
            HDMI_ATTR_S      stHDMIAttr;
            DRV_HDMI_APP_ATTR_S *pstDrvAppAttr = (DRV_HDMI_APP_ATTR_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvAppAttr->enHdmiID); 
            memset((void*)&stHDMIAttr, 0, sizeof(HDMI_ATTR_S));
            s32Ret = DRV_HDMI_AttrGet(pstHdmiDev, &stHDMIAttr);
            memcpy(&pstDrvAppAttr->stAPPAttr,&stHDMIAttr.stAppAttr,sizeof(HDMI_APP_ATTR_S));
            break;
        }

        case CMD_HDMI_SET_ATTR:
        {
            HDMI_ATTR_S      stHDMIAttr;
            DRV_HDMI_APP_ATTR_S *pstDrvAppAttr = (DRV_HDMI_APP_ATTR_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvAppAttr->enHdmiID); 

            s32Ret = DRV_HDMI_AttrGet(pstHdmiDev, &stHDMIAttr);

            memcpy(&stHDMIAttr.stAppAttr, &pstDrvAppAttr->stAPPAttr, sizeof(HDMI_APP_ATTR_S));

            //s32Ret = DRV_HDMI_SetAttr(phdmiattr->enHdmi, &phdmiattr->stHdmiAppAttr);
            s32Ret |= DRV_HDMI_AttrSet(pstHdmiDev, &stHDMIAttr);
            break;
        }

        case CMD_HDMI_GET_INFOFRAME:
        {
            DRV_HDMI_INFOFRAME_S *pInfoframe = (DRV_HDMI_INFOFRAME_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pInfoframe->enHdmiID); 

            s32Ret = DRV_HDMI_InfoFrameGet(pstHdmiDev, pInfoframe->enInfoFrameId, &(pInfoframe->uInfoFrame));
            break;
        }

        case CMD_HDMI_SET_INFOFRAME:
        {
            DRV_HDMI_INFOFRAME_S *pInfoframe = (DRV_HDMI_INFOFRAME_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pInfoframe->enHdmiID); 

            s32Ret = DRV_HDMI_InfoFrameSet(pstHdmiDev, pInfoframe->enInfoFrameId, &(pInfoframe->uInfoFrame));
   
            break;
        }
        
        case CMD_HDMI_SET_AVMUTE:
        {
            DRV_HDMI_AVMUTE_S* pstAvmute = (DRV_HDMI_AVMUTE_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstAvmute->enHdmiID); 

            s32Ret = DRV_HDMI_AVMuteSet(pstHdmiDev, pstAvmute->bAvMute);
            break;
        }
		case CMD_HDMI_GET_VO_ATTR:
        {
            DRV_HDMI_VO_ATTR_S *pstDrvVOAttr = (DRV_HDMI_VO_ATTR_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvVOAttr->enHdmiID); 
           
            s32Ret = DRV_HDMI_VOAttrGet(pstHdmiDev, &pstDrvVOAttr->stVOAttr);
            break;
        }
        case CMD_HDMI_SET_VO_ATTR:
        {
            DRV_HDMI_VO_ATTR_S *pstDrvVOAttr = (DRV_HDMI_VO_ATTR_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvVOAttr->enHdmiID); 

            s32Ret = DRV_HDMI_VOAttrSet(pstHdmiDev, &pstDrvVOAttr->stVOAttr);
            break;
        }
#if 0        
        case CMD_HDMI_PREVTIMING:
        {
            HDMI_VO_ATTR_S *pstPreVideoTiming;

            pstPreVideoTiming = (HDMI_VO_ATTR_S*)argp;
            s32Ret = DRV_HDMI_PreFormat(pstHdmiDev, pstPreVideoTiming);
            break;
        }
#endif        
        case CMD_HDMI_GET_DEEPCOLOR:
        {
            DRV_HDMI_DEEP_COLOR_S *pstDrvDeepColor = (DRV_HDMI_DEEP_COLOR_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvDeepColor->enHdmiID); 

            s32Ret = DRV_HDMI_DeepColorGet(pstHdmiDev, &pstDrvDeepColor->enDeepColorMode);
            break;
        }
        case CMD_HDMI_SET_DEEPCOLOR:
        {
            DRV_HDMI_DEEP_COLOR_S *pstDrvDeepColor = (DRV_HDMI_DEEP_COLOR_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvDeepColor->enHdmiID); 

            s32Ret = DRV_HDMI_DeepColorSet(pstHdmiDev, pstDrvDeepColor->enDeepColorMode);
            break;
        }
        case CMD_HDMI_SET_XVYCC:
        {

            break;
        }

#if defined(HDMI_CEC_SUPPORT)
        case CMD_HDMI_SET_CEC:
        {
            DRV_HDMI_CEC_CMD_S *pstCecCmd = (DRV_HDMI_CEC_CMD_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstCecCmd->enHdmiID); 
 
            s32Ret = DRV_HDMI_CecCmdSend(pstHdmiDev, &pstCecCmd->CECCmd);
            break;
        }
        case CMD_HDMI_GET_CEC:
        {
            DRV_HDMI_CEC_CMD_S *pstCecCmd = (DRV_HDMI_CEC_CMD_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstCecCmd->enHdmiID); 

            HDMI_MUTEX_UNLOCK(s_HdmiMutex);

            /* may sleep in this function,so release the lock first*/
            s32Ret = DRV_HDMI_CecCmdReceive(pstHdmiDev, &pstCecCmd->CECCmd);
            HDMI_MUTEX_LOCK(s_HdmiMutex);
                   
            break;
        }
        case CMD_HDMI_GET_CECSTATUS:
        {
            DRV_HDMI_CEC_INFO_S *pstCecStatus = (DRV_HDMI_CEC_INFO_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstCecStatus->enHdmiID); 
            
            s32Ret = DRV_HDMI_CecStatusGet(pstHdmiDev, &pstCecStatus->stCecInfo);
            break;
        }
        case CMD_HDMI_CEC_ENABLE:
        {
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(*(HDMI_DEVICE_ID_E*)argp); 
            s32Ret = DRV_HDMI_CecEnableSet(pstHdmiDev,HI_TRUE);
            break;
        }
        case CMD_HDMI_CEC_DISABLE:
        {
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(*(HDMI_DEVICE_ID_E*)argp);            
            s32Ret = DRV_HDMI_CecEnableSet(pstHdmiDev,HI_FALSE);
            break;
        }
#endif
        case CMD_HDMI_UPDATE_EDID:
        {
            DRV_HDMI_EDID_RAWDATA_S *pstDrvEdidRaw = (DRV_HDMI_EDID_RAWDATA_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvEdidRaw->enHdmiID); 
           
            s32Ret = DRV_HDMI_EdidUpdate(&pstHdmiDev->stEdidInfo, HDMI_EDID_UPDATE_SINK);
            if (s32Ret == HI_SUCCESS)
            {
                pstDrvEdidRaw->stEdidRaw.u32EdidLen = DRV_HDMI_EdidRawGet(&pstHdmiDev->stEdidInfo, pstDrvEdidRaw->stEdidRaw.u8Edid, HDMI_EDID_SIZE);
                pstDrvEdidRaw->stEdidRaw.u8EdidValid = 1;
            }        
            break;
        }
        case CMD_HDMI_GET_HDMI_PLAYSTATUS:
        {
            DRV_HDMI_PLAYSTATUS_S* pstPlayStatus = (DRV_HDMI_PLAYSTATUS_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstPlayStatus->enHdmiID); 
              
            s32Ret = DRV_HDMI_PlayStatusGet(pstHdmiDev, &pstPlayStatus->u32Status);
            break;
        }
#if defined(HDMI_HDCP_SUPPORT)
		case CMD_HDMI_LOAD_KEY:
		{
			DRV_HDMI_LOAD_KEY_S *pstLoadKey = (DRV_HDMI_LOAD_KEY_S*)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstLoadKey->enHdmiID); 

            s32Ret = DRV_HDMI_KeyLoad(pstHdmiDev, &pstLoadKey->stKey);
			break;
		}
               
#endif
        case CMD_HDMI_GET_AO_ATTR:
        {
            DRV_HDMI_AO_ATTR_S *pstDrvAOAttr = (DRV_HDMI_AO_ATTR_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvAOAttr->enHdmiID); 

            s32Ret = DRV_HDMI_AOAttrGet(pstHdmiDev, &pstDrvAOAttr->stAOAttr);
            break;
        }
        case CMD_HDMI_SET_AO_ATTR:
        {
            DRV_HDMI_AO_ATTR_S *pstDrvAOAttr = (DRV_HDMI_AO_ATTR_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDrvAOAttr->enHdmiID); 

            s32Ret = DRV_HDMI_AOAttrSet(pstHdmiDev, &pstDrvAOAttr->stAOAttr);
            break;
        }
        case CMD_HDMI_GET_STATUS:
        {
            DRV_HDMI_STATUS_S *pstStatus = (DRV_HDMI_STATUS_S *)argp;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstStatus->enHdmiID); 

            s32Ret = DRV_HDMI_StatusGet(pstHdmiDev, &pstStatus->stHdmiStatus);
            break;
        }
#if defined (HDMI_DEFUALT_CALLBACK_SUPPORT)	
		case CMD_HDMI_REGISTER_CALLBACK:
        {
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(*(HDMI_DEVICE_ID_E*)argp); 
			if (pstHdmiDev)
			{
				HDMI_MUTEX_UNLOCK(s_HdmiMutex);
				HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
            	pstHdmiDev->pKCallback = HI_NULL;
				HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
				HDMI_MUTEX_LOCK(s_HdmiMutex);
				s32Ret= HI_SUCCESS;
			}
			else
			{
				s32Ret= HI_FAILURE;
			}
			
            break;
        }

		case CMD_HDMI_UNREGISTER_CALLBACK:
        {
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(*(HDMI_DEVICE_ID_E*)argp); 
			if (pstHdmiDev)
			{
				HDMI_MUTEX_UNLOCK(s_HdmiMutex);
				HDMI_MUTEX_LOCK(pstHdmiDev->stMutexThread);
				pstHdmiDev->pKCallback = HI_DRV_HDMI_KernelEventCallback;
				HDMI_MUTEX_UNLOCK(pstHdmiDev->stMutexThread);
				HDMI_MUTEX_LOCK(s_HdmiMutex);
				s32Ret= HI_SUCCESS;
			}
			else
			{
				s32Ret= HI_FAILURE;
			}
            break;
        }
#endif
        case CMD_HDMI_GET_DELAY:
        {
            DRV_HDMI_DELAY_S *pstDelay = (DRV_HDMI_DELAY_S *)arg;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDelay->enHdmiID); 
            s32Ret= DRV_HDMI_DelayGet(pstHdmiDev,&pstDelay->stDelay);
            break;
        }
        case CMD_HDMI_SET_DELAY:
        {
            DRV_HDMI_DELAY_S *pstDelay = (DRV_HDMI_DELAY_S *)arg;
            HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(pstDelay->enHdmiID); 
            s32Ret= DRV_HDMI_DelaySet(pstHdmiDev,&pstDelay->stDelay);
            break;
        }
        default:
        {
            HDMI_ERR("unkonw cmd:0x%x\n", cmd);
            HDMI_MUTEX_UNLOCK(s_HdmiMutex);
            return -ENOIOCTLCMD;
        }
    }
    HDMI_MUTEX_UNLOCK(s_HdmiMutex);
    HDMI_PRINT_OUT;
    return s32Ret;

}

#if defined(CONFIG_HDMI_STB_SDK)
static HI_S32 HdmiIoctl(struct inode *inode, struct file *file,
                           unsigned int cmd, HI_VOID *arg)
{
    HI_S32   s32Ret = HI_FAILURE;

    HDMI_PRINT_IN;
    
    s32Ret = DRV_HDMI_CmdProcess(cmd, arg, HI_TRUE);

    HDMI_PRINT_OUT;
    return s32Ret;
}

#elif defined(CONFIG_HDMI_BVT_SDK)
static HI_S32 HdmiIoctl(struct file *file,
                           unsigned int cmd,long unsigned int arg)
{
    HI_S32   s32Ret = HI_FAILURE;

    HDMI_PRINT_IN;
    s32Ret = DRV_HDMI_CmdProcess(cmd, (HI_VOID*)arg, HI_TRUE);

    HDMI_PRINT_OUT;
    return s32Ret;
}
#endif

#if (HDMI_CHIP_HI3536 == HICHIP)
HI_S32 Hdmi_Init(HI_VOID* pArgs)
{
    atomic_inc_return(&s_HdmiCount);
    HDMI_INFO("s_HdmiCount:%d\n", s_HdmiCount);
    
    return 0;
}

HI_VOID Hdmi_Exit(HI_VOID)
{
    HI_U32 u32HdmiId = 0;
    HDMI_DEVICE_S* pstHdmiDev;

    if (atomic_dec_and_test(&s_HdmiCount))
    {
        for(u32HdmiId = 0; u32HdmiId < HDMI_DEVICE_ID_BUTT; u32HdmiId++)
        {
            pstHdmiDev = GetHdmiDevice(u32HdmiId);
            if (pstHdmiDev != HI_NULL)
            {
                pstHdmiDev->u32UserCnt = 0;
                if (pstHdmiDev->enRunState & HDMI_RUN_STATE_OPEN)
                {
                    HdmiRelease(pstHdmiDev);
                }
                else
                {
                    HDMI_INFO("enRunState:%d of u32HdmiId[%d] is not open!\n", pstHdmiDev->enRunState, u32HdmiId);
                }
            }
            else
            {
                HDMI_ERR("pstHdmiDev[%d] is null!\n", u32HdmiId);
            }
        }
    }
    else
    {
        HDMI_INFO("s_HdmiCount:%d\n", s_HdmiCount);
    }

    return;
}

static HI_VOID Hdmi_Notify(MOD_NOTICE_ID_E enNotice) 
{
    return ;
}

static HI_VOID Hdmi_QueryState(MOD_STATE_E *pstState)
{
    return ;
}

static HI_U32 Hdmi_GetVerMagic(HI_VOID)
{
	return VERSION_MAGIC;
}

static UMAP_MODULE_S s_stModule =
{
    .pstOwner = THIS_MODULE,
    .enModId = HI_ID_HDMI,
    .aModName = "hdmi",

    .pfnInit = Hdmi_Init,
    .pfnExit = Hdmi_Exit,
    .pfnQueryState = Hdmi_QueryState,
    .pfnNotify = Hdmi_Notify,
    .pfnVerChecker = Hdmi_GetVerMagic,
    
    .pstExportFuncs = HI_NULL,
    .pData = HI_NULL,
};
#endif

static long  HdmiFileIoctl(struct file *file,unsigned int cmd, unsigned long arg)
{
    return (long) HI_HDMI_UserCopy(file, cmd, arg, HdmiIoctl);
}

HDMI_PROC_ITEM_S s_pHdmiProcItem = 
{
	.fnRead 	= DRV_HDMI_ProcSource0Read,
	.fnWrite	= HI_NULL,
	.fnIoctl	= HI_NULL,	
};

HDMI_PROC_ITEM_S s_pSinkProcItem = 
{
	.fnRead 	= DRV_HDMI_ProcSink0Read,
	.fnWrite	= HI_NULL,
	.fnIoctl	= HI_NULL,	
};

HI_S32 HDMI_DRV_ModInit(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32HdmiId = 0;
    HI_CHAR procName[16];

	//sema_init(&s_HdmiMutex, 1);

    /* clear*/
    //memset(&s_stHdmiCtrl[0], 0, sizeof(HDMI_DEVICE_S)*HDMI_DEVICE_ID_BUTT);
    atomic_set(&s_HdmiCount, 0);
    
#if defined(CONFIG_HDMI_BVT_SDK)
    if (CMPI_RegisterModule(&s_stModule))
    {
        printk("CMPI_RegisterModule hdmi fail \n");
        return HI_FAILURE;
    }
#endif

    /* register hdmi device*/
    snprintf(s_stHdmiDev.devfs_name, sizeof(s_stHdmiDev.devfs_name), "%s", DEVNAME_HDMI);
    s32Ret = HI_HDMI_DEV_Register(s_stHdmiDev);
    if (s32Ret != HI_SUCCESS)
    {
        HDMI_WARN("s_stHdmiDev register fail!\n");
    }

    /* register hdmi proc*/
    for(; u32HdmiId < HDMI_DEVICE_ID_BUTT; u32HdmiId++)
    {
        memset(procName, 0, sizeof(procName));
        snprintf(procName, sizeof(procName), "%s%d", "hdmi",u32HdmiId);
        HI_HDMI_PROC_AddModule(procName, &s_pHdmiProcItem, HI_NULL);
        memset(procName, 0, sizeof(procName));
        snprintf(procName, sizeof(procName), "%s%d%s", "hdmi",u32HdmiId,"_sink");
        HI_HDMI_PROC_AddModule(procName, &s_pSinkProcItem, HI_NULL); 
    }	

#ifndef HI_MCE_SUPPORT
    HI_DRV_HDMI_Init();
#endif 

#ifdef HDMI_ANDROID_SUPPORT
    //android only
    if (switch_dev_register(&s_stSwitchDev))
    {
        s_bIsSwitchOn = HI_FALSE;
    }
    else
    {
        s_bIsSwitchOn = HI_TRUE;
        HdmiSetAndroidState(STATE_PLUG_UNKNOWN);
    }
#endif
#if (defined(MODULE) && defined(CONFIG_HDMI_STB_SDK))
    HI_PRINT("Load hi_hdmi.ko success.  \t(%s)\n", VERSION_STRING);
#endif
    return HI_SUCCESS;
}

HI_VOID HDMI_DRV_ModExit(HI_VOID)
{
    HI_U32 u32HdmiId = 0;
    HDMI_DEVICE_S *pstHdmiDev;
    HI_CHAR procName[16];
    
#ifdef HDMI_ANDROID_SUPPORT
    //android only
    if(s_bIsSwitchOn == HI_TRUE)
    {
        switch_dev_unregister(&s_stSwitchDev);
    }
#endif


#ifndef HI_MCE_SUPPORT
    HI_DRV_HDMI_Deinit();
#endif


    for(; u32HdmiId < HDMI_DEVICE_ID_BUTT; u32HdmiId++)
    {   
        pstHdmiDev = GetHdmiDevice(u32HdmiId);
        if (pstHdmiDev != HI_NULL)
        {
            if (pstHdmiDev->enRunState & HDMI_RUN_STATE_OPEN)
            {
                pstHdmiDev->u32UserCnt = pstHdmiDev->u32KernelCnt = 0;
                HdmiRelease(pstHdmiDev);
                memset(pstHdmiDev, 0, sizeof(HDMI_DEVICE_S));     
            }
        }
 
        memset(procName, 0, sizeof(procName));
        snprintf(procName, sizeof(procName), "%s%d", "hdmi",u32HdmiId);    
        HI_HDMI_PROC_RemoveModule(procName);
        memset(procName, 0, sizeof(procName));
        snprintf(procName, sizeof(procName), "%s%d%s", "hdmi",u32HdmiId,"_sink");
        HI_HDMI_PROC_RemoveModule(procName);
    }

    HI_HDMI_DEV_UnRegister(s_stHdmiDev);

#if defined(CONFIG_HDMI_BVT_SDK)
    CMPI_UnRegisterModule(HI_ID_HDMI);
#endif
}

#ifdef MODULE
module_init(HDMI_DRV_ModInit);
module_exit(HDMI_DRV_ModExit);
#endif
MODULE_LICENSE("GPL");
