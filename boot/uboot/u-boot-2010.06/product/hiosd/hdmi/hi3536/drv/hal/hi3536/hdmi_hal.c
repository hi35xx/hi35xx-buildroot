#ifndef HDMI_BUILD_IN_BOOT
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
#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#if defined(CONFIG_HDMI_STB_SDK)
#include "hi_drv_disp.h"
#endif
#include "si_drv_tpg_api.h"
#include "si_lib_seq_api.h"
#include "si_drv_vtg_api.h"
#include "si_drv_pll_vo_api.h"
#include "si_drv_cpi_api.h"
#include "si_lib_video_api.h"
#include "drv_hdmi_platform.h"
#endif
#include "si_datatypes.h"
#include "si_lib_time_api.h"
#include "si_drv_tx_api.h"
#include "si_drv_cra_api.h"
#include "si_lib_log_api.h"
#include "si_drv_tx_api.h"
#include "si_drv_tx_regs.h"
#include "si_drv_common.h"
#include "si_lib_malloc_api.h"
#include "hdmi_hal.h"
#if defined(CONFIG_HDMI_STB_SDK)
#include "hi_reg_common.h"
#endif

/***** local macro definitions ***********************************************/
#ifdef HDMI_BUILD_IN_BOOT
#include "boot_hdmi_intf.h"
#endif

typedef enum
{
    HDMI_CLK_FROM_CRG,
    HDMI_CLK_FROM_PHY   
}HDMI_CLK_TYPE_E;

#define CHECK_POINTER(p) \
do{                      \
    if(p == HI_NULL){ \
        HDMI_ERR("Error In %s,Line %d\n",__FUNCTION__,__LINE__);       \
        return 1;\
    } \
}while(0)
/***** local type definitions ************************************************/
static  SiiInst_t instCra = HI_NULL;

static HI_VOID HdmiRegWrite(volatile HI_VOID *pRegAddr, HI_U32 value)
{
    *(volatile HI_U32*)pRegAddr = value;
}

static HI_U32 HdmiRegRead(volatile HI_VOID *pRegAddr)
{
    return *(volatile HI_U32*)pRegAddr;
}

#if defined(CONFIG_HDMI_STB_SDK)
#define PERI_HDMITX_CTRL_ADDR          0xf8a208b0
#define HDMI_AUDIO_SOURCE_SPDIF        (0x1<<0)
#define HDMI_AUDIO_SOURCE_I2S          (0x1<<1)
#define HDMI_AUDIO_SOURCE_DSD          (0x1<<2)
#define HDMI_AUDIO_SOURCE_HBRA         (0x1<<3)
#define HDMI_AUDIO_SOURCE_PARALLEL     (0x1<<4)

#define HDMITX_CTRL_ASCLK_SEL          (0x1<<14) //有SIMG PHY的项目默认选PHY , 有自研 PHY 的项目默认选CRG 。
#define HDMITX_CTRL_SRST_REQ           (0x1<<9)
#define HDMITX_CTRL_BUS_SRST_REQ       (0x1<<8)
#define HDMITX_CTRL_ID_CKSEL           (0x1<<6)
#define HDMITX_CTRL_OSCLK_SEL          (0x1<<5)  //DDC /HDCP2X时钟,有SIMG PHY的项目默认选PHY , 有自研 PHY 的项目默认选CRG 。
#define HDMITX_CTRL_PIXELNX_CKSEL      (0x1<<4)
#elif defined(CONFIG_HDMI_BVT_SDK)
#define PERI_HDMITX_CTRL_ADDR          0x12120150
#define HDMI_AUDIO_SOURCE_SPDIF 	   (0x1<<6)
#define HDMI_AUDIO_SOURCE_I2S 		   (0x1<<7)
#define HDMI_AUDIO_SOURCE_DSD 		   (0x1<<8)
#define HDMI_AUDIO_SOURCE_HBRA 		   (0x1<<9)
#define HDMI_AUDIO_SOURCE_PARALLEL 	   (0x1<<10)

#define HDMI_CRG_BASE_ADDR  			0x1204003c
#define HDMITX_CTRL_PIXELNX_CKSEL		(0x1<<28)
#define HDMITX_CTRL_ID_CKSEL			(0x1<<24)
#define HDMITX_CTRL_ASCLK_SEL			(0x1<<18)
#define HDMITX_CTRL_OSCLK_SEL			(0x1<<17)
#define HDMITX_CTRL_SRST_REQ			(0x1<<13)
#define HDMITX_CTRL_BUS_SRST_REQ		(0x1<<12)
#define HDMITX_CTRL_PIXELNX_CKEN		(0x1<<8) 
#define HDMITX_CTRL_XCLK_CKEN  		    (0x1<<7)  
#define HDMITX_CTRL_PIXEL_CKEN			(0x1<<6) 
#define HDMITX_CTRL_AS_CKEN			    (0x1<<5)
#define HDMITX_CTRL_OS_CKEN			    (0x1<<4)
#define HDMITX_CTRL_MHL_CKEN			(0x1<<3)
#define HDMITX_CTRL_ID_CKEN			    (0x1<<2)
#define HDMITX_CTRL_CEC_CKEN			(0x1<<1)
#define HDMITX_CTRL_BUS_CKEN			(0x1<<0)
#endif

#ifdef HDMI_HDCP_SUPPORT
#define HDMI_HDCP2_CCHK_DONE  (0x1 << 25)
#define HDMI_HDCP2_CUPD_START (0x1 << 15)
#define HDMI_HDCP2_CUPD_HW    (0x1 << 14)
#define HDMI_HDCP2_CUPD_DONE  (0x1 << 13)
#define C8051_PRAM_BASE_ADDR  0xf8cf0000

static HI_S32 Hdcp22ProgramLoad(HDMI_HAL_S *pstHdmiHal, HI_U32 u32Length, HI_U8 u8BinaryCode[]);
static HI_U8 s_u8C8051Hdcp22Program[] =
{
    #include "c8051_hdcp22_program.bin"
};
#endif

#ifndef HDMI_BUILD_IN_BOOT
#if defined(CONFIG_HDMI_STB_SDK)
static HI_S32 CipherHdmiClkSet(HI_DRV_HDCPKEY_TYPE_E enKeyType)
{
    HI_S32 s32Ret = HI_FAILURE;
    CIPHER_EXPORT_FUNC_S* pstCipherExportFuncs = HI_NULL;
    s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_CIPHER, (HI_VOID**)&pstCipherExportFuncs);
    if (s32Ret == HI_FAILURE || pstCipherExportFuncs == HI_NULL || pstCipherExportFuncs->pfnCipherSetHdmiReadClk == HI_NULL)
    {
        HDMI_ERR("Get cipher funcs fail\n");
        return HI_FAILURE;   
    }
    pstCipherExportFuncs->pfnCipherSetHdmiReadClk(enKeyType);
    HDMI_INFO("CipherHdmiClkSet HDMI\n");
    return HI_SUCCESS; 
}
#endif

static HI_VOID HalEventCallback(HI_VOID* data, SiiDrvTxEvent_t event)
{
    HDMI_HAL_S *pstHdmiHal = (HDMI_HAL_S *)data;
    HI_BOOL bHotPlug;
    HDMI_EVENT_E enEvent = 0;

    if (pstHdmiHal)
    {
        switch (event)
        {
            case (SII_DRV_TX_EVENT__HOT_PLUG_CHNG | SII_DRV_TX_EVENT__RSEN_CHNG):
                bHotPlug = SiiDrvTxHotPlugStatusGet(pstHdmiHal->stHalCtx.hHdmiHw);
                if (bHotPlug)
                {
                    enEvent = HDMI_EVENT_HOTPLUG;                   
                }
                else
                {
                    enEvent = HDMI_EVENT_HOTUNPLUG;
                }              
                break;
            case SII_DRV_TX_EVENT__HDMI_STATE_CHNG:
                break;
            case SII_DRV_TX_EVENT__HDCP_STATE_CHNG:
            {
             #ifdef HDMI_HDCP_SUPPORT
                SiiDrvHdcpStatus_t hdcpStatus;
                SiiDrvTxHdcpStateStatusGet(pstHdmiHal->stHalCtx.hHdmiHw, &hdcpStatus);
                if (hdcpStatus == SII_DRV_HDCP_STATUS__SUCCESS_1X || hdcpStatus == SII_DRV_HDCP_STATUS__SUCCESS_22)
                {
                    enEvent = HDMI_EVENT_HDCP_SUCCESS;
                }
                else if (hdcpStatus == SII_DRV_HDCP_STATUS__FAILED)
                {
                    enEvent = HDMI_EVENT_HDCP_FAIL;
                }
            #endif                
            }
                break;
            case SII_DRV_CEC_EVENT:
                break;
            case SII_DRV_TX_EVENT_SCRAMBLE_SUCCESS:
                enEvent = HDMI_EVENT_SCRAMBLE_SUCCESS;
                break;            
            case SII_DRV_TX_EVENT_SCRAMBLE_FAIL:
                enEvent = HDMI_EVENT_SCRAMBLE_FAIL;
                break;
            default:
                break;
        }
        
        if (pstHdmiHal->stHalCtx.pCallback && enEvent != 0)
           pstHdmiHal->stHalCtx.pCallback(pstHdmiHal->stHalCtx.hHdmiDev, enEvent);
    }
}  
#endif

static HI_VOID HalHdmiSwReset(HDMI_HAL_S *pstHdmiHal, HI_BOOL bReset)
{
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__SOFTWARE, bReset);
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__AUDIO, bReset);
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__TPI, bReset);
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__HDCP2x, bReset);
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__CEC, bReset);
}

static HI_VOID HalHdmiHwReset(HI_BOOL bReset)
{
    HI_U32 u32NewValue = 0;
    volatile HI_VOID* pRegAddr = HI_NULL;
    HI_U32 u32OldValue = 0;

    #if defined(CONFIG_HDMI_STB_SDK)
    pRegAddr = (volatile HI_VOID*)&(g_pstRegCrg->PERI_CRG67.u32);    
    #elif defined(CONFIG_HDMI_BVT_SDK)
    pRegAddr = (volatile HI_VOID*)IO_ADDRESS(HDMI_CRG_BASE_ADDR);
    #endif
    
    u32OldValue = HdmiRegRead(pRegAddr);
    u32NewValue = u32OldValue;
    if (bReset)
    {
        u32NewValue |= HDMITX_CTRL_SRST_REQ | HDMITX_CTRL_BUS_SRST_REQ;
        HdmiRegWrite(pRegAddr, u32NewValue);
    }
    else
    {
        u32NewValue &= ~(HDMITX_CTRL_SRST_REQ | HDMITX_CTRL_BUS_SRST_REQ);
        HdmiRegWrite(pRegAddr, u32NewValue);
    }  

    return;    
}

static HI_VOID HalHdmiClkSet(HDMI_CLK_TYPE_E enClkType)
{
    HI_U32 u32NewValue1 = 0, u32OldValue1 = 0;
    volatile HI_VOID *pRegAddr = HI_NULL;

#if defined(CONFIG_HDMI_STB_SDK)
    HI_U32 u32NewValue2 = 0, u32OldValue2 = 0;

    pRegAddr = (volatile HI_VOID*)&(g_pstRegCrg->PERI_CRG67.u32);

    u32OldValue1 = HdmiRegRead(pRegAddr);
    u32NewValue1 = u32OldValue1;
    u32NewValue1 |= 0x3f;
    if (enClkType == HDMI_CLK_FROM_CRG)
    {
        u32NewValue1 &= ~HDMITX_CTRL_ASCLK_SEL;
        u32NewValue2 |= HDMITX_CTRL_OSCLK_SEL;
    }
    else if (enClkType == HDMI_CLK_FROM_PHY)
    {
        u32NewValue1 |= HDMITX_CTRL_ASCLK_SEL;
        u32NewValue2 &= ~HDMITX_CTRL_OSCLK_SEL;
    }
    
    HdmiRegWrite(pRegAddr, u32NewValue1);

    pRegAddr = (volatile HI_VOID*)&(g_pstRegCrg->PERI_CRG158.u32);    
    u32OldValue2 = HdmiRegRead(pRegAddr);
    u32NewValue2 |= u32OldValue2;
    u32NewValue2 |= 0x7;
    HdmiRegWrite(pRegAddr, u32NewValue2);

#elif defined(CONFIG_HDMI_BVT_SDK)
    pRegAddr = (volatile HI_VOID *)IO_ADDRESS(HDMI_CRG_BASE_ADDR);

    u32OldValue1 = HdmiRegRead(pRegAddr);
    u32NewValue1 = u32OldValue1;

    u32NewValue1 |= ( HDMITX_CTRL_AS_CKEN
                    | HDMITX_CTRL_OS_CKEN
                    | HDMITX_CTRL_MHL_CKEN
                    | HDMITX_CTRL_ID_CKEN
                    | HDMITX_CTRL_CEC_CKEN
                    | HDMITX_CTRL_BUS_CKEN );

    if (enClkType == HDMI_CLK_FROM_CRG)
    {
        u32NewValue1 &= ~HDMITX_CTRL_ASCLK_SEL;
        u32NewValue1 |= HDMITX_CTRL_OSCLK_SEL;
        
    }
    else if (enClkType == HDMI_CLK_FROM_PHY)
    {
        u32NewValue1 |= HDMITX_CTRL_ASCLK_SEL;
        u32NewValue1 &= ~HDMITX_CTRL_OSCLK_SEL;
    }

    u32NewValue1 |=  ( HDMITX_CTRL_PIXEL_CKEN 
                     | HDMITX_CTRL_PIXELNX_CKEN
                     | HDMITX_CTRL_XCLK_CKEN );
    HdmiRegWrite(pRegAddr, u32NewValue1);
#endif

    return;
}

//when input is YUV420,idclk need div2. when output is YUV420,pixelnx clk need div2
static HI_VOID HalHdmiClkDivSet(HI_BOOL bIdClkDiv, HI_BOOL bPixelnxClkDiv)
{
    HI_U32 u32NewValue = 0, u32OldValue = 0;
    volatile HI_VOID* pRegAddr = HI_NULL;

    #if defined(CONFIG_HDMI_STB_SDK)
    pRegAddr = (volatile HI_VOID*)&(g_pstRegCrg->PERI_CRG158.u32);
    #elif defined(CONFIG_HDMI_BVT_SDK)
    pRegAddr = (volatile HI_VOID*)IO_ADDRESS(HDMI_CRG_BASE_ADDR);
    #endif

    u32NewValue = u32OldValue = HdmiRegRead(pRegAddr);
    if (bIdClkDiv)
    {
        u32NewValue |= HDMITX_CTRL_ID_CKSEL;  
    }
    else
    {
        u32NewValue &= ~HDMITX_CTRL_ID_CKSEL;
    }
    
    if (bPixelnxClkDiv)
    {
        u32NewValue |= HDMITX_CTRL_PIXELNX_CKSEL;
    }
    else
    {
        u32NewValue &= ~HDMITX_CTRL_PIXELNX_CKSEL;
    }
    HdmiRegWrite(pRegAddr, u32NewValue);
}

HI_VOID HAL_HDMI_HardwareInit(HDMI_HAL_S *pstHdmiHal)
{
    volatile HI_VOID *u32PeriHdmiTxCtrl = HI_NULL;
    HI_U32 u32Value = 0;

    u32PeriHdmiTxCtrl = (volatile HI_VOID *)IO_ADDRESS(PERI_HDMITX_CTRL_ADDR);

#if defined(CONFIG_HDMI_STB_SDK)
    u32Value = HdmiRegRead(u32PeriHdmiTxCtrl) & (~0x1f); //clear bit0~bit4
#elif defined(CONFIG_HDMI_BVT_SDK)
    u32Value = HdmiRegRead(u32PeriHdmiTxCtrl) & (~0x7c0);//0xfffff83f
#endif
    
#ifndef HDMI_BUILD_IN_BOOT    
#if defined(CONFIG_HDMI_STB_SDK)
    CipherHdmiClkSet(HI_DRV_HDCPKEY_TX0);
#endif
#endif
    SiiDrvTxSoftwareInit(pstHdmiHal->stHalCtx.hHdmiHw);
    
    HalHdmiHwReset(HI_TRUE);
    HalHdmiClkSet(HDMI_CLK_FROM_PHY);
    HalHdmiClkDivSet(HI_FALSE, HI_FALSE);
    SiiLibTimeMilliDelay(1);
    HalHdmiHwReset(HI_FALSE);
    SiiLibTimeMilliDelay(1);
    
    HalHdmiSwReset(pstHdmiHal, HI_TRUE);
    SiiLibTimeMilliDelay(1);
    HalHdmiSwReset(pstHdmiHal, HI_FALSE);
    SiiLibTimeMilliDelay(1);
    
    SiiDrvTxHardwareInit(pstHdmiHal->stHalCtx.hHdmiHw);

    u32Value |= HDMI_AUDIO_SOURCE_I2S;
    HdmiRegWrite(u32PeriHdmiTxCtrl, u32Value); 

};

HI_VOID HAL_HDMI_IrqEnableSet(HDMI_HAL_S *pstHdmiHal, HDMI_INT_TYPE_E enIntType, HI_BOOL bEnable)
{
    SiiDrvTxInterruptEnableSet(pstHdmiHal->stHalCtx.hHdmiHw, enIntType, bEnable);
}
    
#ifndef HDMI_BUILD_IN_BOOT
HI_VOID HAL_HDMI_SequencerHandlerProcess(HDMI_HAL_S *pstHdmiHal)
{
    SiiSequencerHandler();
}

HI_VOID HAL_HDMI_ScdcStatusGet(HDMI_HAL_S  *pstHdmiHal, HDMI_SCDC_STATUS_S *pstScdcStatus)    
{
    SiiScdcStatus_t scdcStatus;
    
    SiiDrvTxHwScdcStatusGet(pstHdmiHal->stHalCtx.hHdmiHw, &scdcStatus);
    /* scdc status*/

    pstScdcStatus->bSinkScrambleOn   = scdcStatus.bSinkScrambleOn;
    pstScdcStatus->bSourceScrambleOn = scdcStatus.bSourceScrambleOn;
    pstScdcStatus->u8TmdsBitClkRatio = scdcStatus.tmdsBitClkRatio;
}

HI_VOID HAL_HDMI_HardwareStatusGet(HDMI_HAL_S  *pstHdmiHal, HDMI_HARDWARE_STATUS_S* pstHwStatus)
{
    SiiHardwareStatus_t hwStatus;
   
    SiiDrvTxHardwareStatusGet(pstHdmiHal->stHalCtx.hHdmiHw, &hwStatus);
    
    pstHwStatus->stCommonStatus.bHotPlug    = hwStatus.hdmiStatus.hotPlug;
    pstHwStatus->stCommonStatus.bRsen       = hwStatus.hdmiStatus.rsen;
    pstHwStatus->stCommonStatus.enTmdsMode  = hwStatus.hdmiStatus.tmdsMode;
    pstHwStatus->stCommonStatus.bAvMute     = hwStatus.hdmiStatus.avMute;

    pstHwStatus->stAudioStatus.bAudioEnable = hwStatus.hdmiStatus.audioEnable;
    pstHwStatus->stAudioStatus.bAudioMute   = hwStatus.hdmiStatus.audioMute;
    pstHwStatus->stAudioStatus.enLayout     = hwStatus.hdmiStatus.audioFmt.layout1;
    pstHwStatus->stAudioStatus.bDownSample  = hwStatus.hdmiStatus.audioFmt.downSample;

    if (hwStatus.hdmiStatus.audioFmt.i2s)
    {
        pstHwStatus->stAudioStatus.enSoundIntf = HDMI_AUDIO_INTERFACE__I2S;
    }
    else if (hwStatus.hdmiStatus.audioFmt.spdif)
    {
        pstHwStatus->stAudioStatus.enSoundIntf = HDMI_AUDIO_INTERFACE__SPDIF;
    }
    else if (hwStatus.hdmiStatus.audioFmt.hbrA)
    {
        pstHwStatus->stAudioStatus.enSoundIntf = HDMI_AUDIO_INTERFACE__HBRA;
    }
    
    switch (hwStatus.hdmiStatus.audioFmt.audioFs)
    {
        case SII_AUDIO_FS__22_05KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_22K;
            break;
        case SII_AUDIO_FS__24KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_24K;
            break;
        case SII_AUDIO_FS__32KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_32K;
            break;
        case SII_AUDIO_FS__44_1KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_44K;
            break;
        case SII_AUDIO_FS__48KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_48K;
            break;
        case SII_AUDIO_FS__88_2KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_88K;
            break;
        case SII_AUDIO_FS__96KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_96K;
            break;
        case SII_AUDIO_FS__176_4KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_176K;
            break;
        case SII_AUDIO_FS__192KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_192K;
            break;
        case SII_AUDIO_FS__768KHZ:
            pstHwStatus->stAudioStatus.enSampleFs = HDMI_SAMPLE_RATE_768K;
            break;
         default:
            break;
    }

    switch (hwStatus.hdmiStatus.channelStatus.i2s_chst4 & 0xf)
    {
		case 0x2:
            pstHwStatus->stAudioStatus.enSampleDepth = HDMI_AUDIO_BIT_DEPTH_16;
            break;

		case 0x4:
            pstHwStatus->stAudioStatus.enSampleDepth = HDMI_AUDIO_BIT_DEPTH_18;
            break;
	
        case 0xb:
            pstHwStatus->stAudioStatus.enSampleDepth = HDMI_AUDIO_BIT_DEPTH_24;
            break;
        
        case 0xa:
            pstHwStatus->stAudioStatus.enSampleDepth = HDMI_AUDIO_BIT_DEPTH_20;
            break;
         default:
		 	pstHwStatus->stAudioStatus.enSampleDepth = HDMI_AUDIO_BIT_DEPTH_16;
            break;
    }
    
    pstHwStatus->stPhyStatus.bPhyPowerOn     = hwStatus.hdmiStatus.phyPower;
    pstHwStatus->stPhyStatus.bPhyOe          = hwStatus.hdmiStatus.phyOutput;
    pstHwStatus->stPhyStatus.enDeepColor     = hwStatus.hdmiStatus.phyBitDepth;
    
	pstHwStatus->stVideoStatus.bVideoMute    = hwStatus.videoStatus.bVideoMute;
    pstHwStatus->stVideoStatus.bIn420Ydemux  = hwStatus.videoStatus.in420Ydemux;
    pstHwStatus->stVideoStatus.bOut420Ydemux = hwStatus.videoStatus.out420Ydemux;
    pstHwStatus->stVideoStatus.bRGB2YCbCr    = hwStatus.videoStatus.RGB2YUV;
    pstHwStatus->stVideoStatus.bYCbCr2RGB    = hwStatus.videoStatus.YUV2RGB;
    pstHwStatus->stVideoStatus.bYCbCr420_422 = hwStatus.videoStatus.YUV420_422;
    pstHwStatus->stVideoStatus.bYCbCr422_444 = hwStatus.videoStatus.YUV422_444;
    pstHwStatus->stVideoStatus.bYCbCr444_422 = hwStatus.videoStatus.YUV444_422;
    pstHwStatus->stVideoStatus.bYCbCr422_420 = hwStatus.videoStatus.YUV422_420; 
    pstHwStatus->stVideoStatus.enHvSyncPol   = hwStatus.videoStatus.hvSyncPol;
    pstHwStatus->stVideoStatus.enOutBitDepth = hwStatus.hdmiStatus.outBitDepth;
    pstHwStatus->stVideoStatus.bDither       = hwStatus.videoStatus.dither;

    pstHwStatus->stInfoFrameStatus.bVSIFEnable  = hwStatus.hdmiStatus.bIfOn[SII_INFO_FRAME_ID__VS];
    pstHwStatus->stInfoFrameStatus.bAVIEnable   = hwStatus.hdmiStatus.bIfOn[SII_INFO_FRAME_ID__AVI]; 
    pstHwStatus->stInfoFrameStatus.bSPDEnable   = hwStatus.hdmiStatus.bIfOn[SII_INFO_FRAME_ID__SPD];   
    pstHwStatus->stInfoFrameStatus.bAUDIOEnable = hwStatus.hdmiStatus.bIfOn[SII_INFO_FRAME_ID__AUDIO]; 
    pstHwStatus->stInfoFrameStatus.bMPEGEnable  = hwStatus.hdmiStatus.bIfOn[SII_INFO_FRAME_ID__MPEG]; 
    pstHwStatus->stInfoFrameStatus.bGBDEnable   = hwStatus.hdmiStatus.bIfOn[SII_INFO_FRAME_ID__GBD];
 
    memcpy(pstHwStatus->stInfoFrameStatus.u8AVI,   hwStatus.hdmiStatus.infoframe[SII_INFO_FRAME_ID__AVI].b, 32);
    memcpy(pstHwStatus->stInfoFrameStatus.u8AUDIO, hwStatus.hdmiStatus.infoframe[SII_INFO_FRAME_ID__AUDIO].b, 32);
    memcpy(pstHwStatus->stInfoFrameStatus.u8VSIF,  hwStatus.hdmiStatus.infoframe[SII_INFO_FRAME_ID__VS].b, 32);
    memcpy(pstHwStatus->stInfoFrameStatus.u8SPD,   hwStatus.hdmiStatus.infoframe[SII_INFO_FRAME_ID__SPD].b, 32);
    memcpy(pstHwStatus->stInfoFrameStatus.u8MPEG,  hwStatus.hdmiStatus.infoframe[SII_INFO_FRAME_ID__MPEG].b, 32);
    memcpy(pstHwStatus->stInfoFrameStatus.u8GDB,   hwStatus.hdmiStatus.infoframe[SII_INFO_FRAME_ID__GBD].b, 32);

    if (pstHwStatus->stInfoFrameStatus.bAVIEnable)
    {
        if (!pstHwStatus->stVideoStatus.bOut420Ydemux)
        {
            pstHwStatus->stVideoStatus.enOutColorSpace = (pstHwStatus->stInfoFrameStatus.u8AVI[4] >> 5) & 0x3;

        }
        else
        {
            pstHwStatus->stVideoStatus.enOutColorSpace = HDMI_COLORSPACE_YCbCr420;
        }
    }
    else
    {
        pstHwStatus->stVideoStatus.enOutColorSpace = HDMI_COLORSPACE_BUTT;
    }
    
    if (pstHwStatus->stCommonStatus.enTmdsMode == HDMI_TMDS_MODE_DVI)
    {
        pstHwStatus->stVideoStatus.enOutColorSpace = HDMI_COLORSPACE_RGB;
    }

}

HI_VOID HAL_HDMI_HotPlugStatusGet(HDMI_HAL_S *pstHdmiHal, HI_BOOL* bHotPlug)
{
    SiiHardwareStatus_t hwStatus;
    SiiDrvTxHardwareStatusGet(pstHdmiHal->stHalCtx.hHdmiHw, &hwStatus);
    /* Some TV sometimes has no hotPlug but has rsen*/
    *bHotPlug = hwStatus.hdmiStatus.hotPlug;
}
#endif

#ifdef HDMI_SCDC_SUPPORT
HI_VOID HAL_HDMI_ScdcConfig(HDMI_HAL_S *pstHdmiHal, HDMI_SCDC_CONFIG_S* pstScdcConfig)
{
    SiiModTxScdcSinKCaps_t scdcSinkCaps;
    scdcSinkCaps.bScdcEnable         = pstScdcConfig->bScdcEnable;
    scdcSinkCaps.b3DOsdDisparity     = pstScdcConfig->b3DOsdDisparity;
    scdcSinkCaps.bDc30bit420         = pstScdcConfig->bDc30bit420;
    scdcSinkCaps.bDc36bit420         = pstScdcConfig->bDc36bit420;
    scdcSinkCaps.bDc48bit420         = pstScdcConfig->bDc48bit420;
    scdcSinkCaps.bDualView           = pstScdcConfig->bDualView;
    scdcSinkCaps.bIndependentView    = pstScdcConfig->bIndependentView;
    scdcSinkCaps.bLTE340MscsScramble = pstScdcConfig->bLTE340McscScramble;
    scdcSinkCaps.bReadReqCapable     = pstScdcConfig->bRRCapable;
    scdcSinkCaps.bScdcPresent        = pstScdcConfig->bScdcPresent;
    scdcSinkCaps.vclk_mb             = pstScdcConfig->u32MaxTmdsCharacterRate / 1000000;
    SiiDrvTxScdcConfig(pstHdmiHal->stHalCtx.hHdmiHw, &scdcSinkCaps);
}
#endif

HI_VOID HAL_HDMI_TxCapabilityGet(HDMI_HAL_S *pstHdmiHal,  HDMI_TX_CAPABILITY_E *penTxCapability)
{
    *penTxCapability = pstHdmiHal->stHalCtx.enTxCapability;
}


HI_VOID HAL_HDMI_TmdsModeSet(HDMI_HAL_S *pstHdmiHal, HDMI_TMDS_MODE_E enTmdsMode)
{
    SiiDrvTxTmdsModeSet(pstHdmiHal->stHalCtx.hHdmiHw, enTmdsMode);
}


HI_VOID HAL_HDMI_AvMuteSet(HDMI_HAL_S *pstHdmiHal, HI_BOOL bAvMute)
{
    SiiDrvTxAvMuteSet(pstHdmiHal->stHalCtx.hHdmiHw, bAvMute);
}

#ifndef HDMI_BUILD_IN_BOOT
HI_VOID HAL_HDMI_AudioMuteSet(HDMI_HAL_S *pstHdmiHal, HI_BOOL bAudioMute)
{
    SiiDrvTxAudioMuteSet(pstHdmiHal->stHalCtx.hHdmiHw, bAudioMute);
}

HI_VOID HAL_HDMI_VideoMuteSet(HDMI_HAL_S *pstHdmiHal, HI_BOOL bAudioMute)
{
    SiiDrvTxVideoMuteSet(pstHdmiHal->stHalCtx.hHdmiHw, bAudioMute);
}
#endif

static SiiInfoFrameId_t	s_ifId[HDMI_INFOFRAME_TYPE_BUTT - HDMI_INFOFRAME_TYPE_VENDOR] = 
{
    SII_INFO_FRAME_ID__VS,	
    SII_INFO_FRAME_ID__AVI,
    SII_INFO_FRAME_ID__SPD,
    SII_INFO_FRAME_ID__AUDIO,
    SII_INFO_FRAME_ID__MPEG,
    SII_INFO_FRAME_ID__GBD
};

HI_VOID HAL_HDMI_InfoframeSet(HDMI_HAL_S *pstHdmiHal, HDMI_INFOFRAME_ID_E enInfoFrameId, HI_U8 u8InBuffer[])
{
    SiiInfoFrame_t InfoFrame;
    SiiInfoFrameId_t ifId = s_ifId[enInfoFrameId - HDMI_INFOFRAME_TYPE_VENDOR];

    SiiDrvClearInfoFrame(ifId, &InfoFrame);
    memcpy(InfoFrame.b, u8InBuffer, SII_INFOFRAME_MAX_LEN);
    
    SiiDrvTxInfoframeSet(pstHdmiHal->stHalCtx.hHdmiHw, ifId, &InfoFrame);
}

HI_VOID HAL_HDMI_InfoframeEnableSet(HDMI_HAL_S *pstHdmiHal, HDMI_INFOFRAME_ID_E enInfoFrameId, HI_BOOL bEnable)
{
    SiiInfoFrameId_t ifId = s_ifId[enInfoFrameId - HDMI_INFOFRAME_TYPE_VENDOR];
    SiiDrvTxInfoframeOnOffSet(pstHdmiHal->stHalCtx.hHdmiHw, ifId, bEnable);
}

#ifndef HDMI_BUILD_IN_BOOT

static unsigned char s_chst3aTable[SII_AUDIO_FS__768KHZ + 1] = {0x4,0x6,0x3,0x0,0x2,0x8,0xa,0xc,0xe,0x9};
HI_S32 HAL_HDMI_AudioPathSet(HDMI_HAL_S *pstHdmiHal, HDMI_AUDIO_CONFIG_S* pstAudioCfg)
{
    
    SiiAudioFormat_t AudioFormat = {0};
    SiiChannelStatus_t ChannelStatus = {0};
    HI_U32 u32Value = 0;
	volatile HI_VOID *u32PeriHdmiTxCtrl = HI_NULL;

	u32PeriHdmiTxCtrl = (volatile HI_VOID *)IO_ADDRESS(PERI_HDMITX_CTRL_ADDR);
#if defined(CONFIG_HDMI_STB_SDK)
    u32Value = HdmiRegRead(u32PeriHdmiTxCtrl) & (~0x1f); //clear bit0~bit4
#elif defined(CONFIG_HDMI_BVT_SDK)
    u32Value = HdmiRegRead(u32PeriHdmiTxCtrl) & (~0x7c0);//0xfffff83f
#endif

    switch (pstAudioCfg->enSoundIntf)
    {
        case HDMI_AUDIO_INTERFACE__I2S:
            AudioFormat.i2s = HI_TRUE;
            u32Value |= HDMI_AUDIO_SOURCE_I2S;
            HdmiRegWrite(u32PeriHdmiTxCtrl, u32Value);  
            break;
        case HDMI_AUDIO_INTERFACE__SPDIF:
            AudioFormat.spdif = HI_TRUE;
            u32Value |= HDMI_AUDIO_SOURCE_SPDIF;
            HdmiRegWrite(u32PeriHdmiTxCtrl, u32Value); 
            break;            
        case HDMI_AUDIO_INTERFACE__HBRA:
            AudioFormat.hbrA = HI_TRUE;
            u32Value |= HDMI_AUDIO_SOURCE_HBRA;
            HdmiRegWrite(u32PeriHdmiTxCtrl, u32Value);             
            break;
        default : 
            AudioFormat.i2s = HI_TRUE;
            break;
    }

    if (pstAudioCfg->bDownSample)
    {
        AudioFormat.downSample = HI_TRUE;
    }
    
    switch (pstAudioCfg->enSampleFs)
    {
        case HDMI_SAMPLE_RATE_22K:
            AudioFormat.audioFs = SII_AUDIO_FS__22_05KHZ;
            break;
        case HDMI_SAMPLE_RATE_24K:
            AudioFormat.audioFs = SII_AUDIO_FS__24KHZ;
            break;            
        case HDMI_SAMPLE_RATE_32K:
            AudioFormat.audioFs = SII_AUDIO_FS__32KHZ;
            break;
        case HDMI_SAMPLE_RATE_44K:
            AudioFormat.audioFs = SII_AUDIO_FS__44_1KHZ;
            break;
        case HDMI_SAMPLE_RATE_48K:
            AudioFormat.audioFs = SII_AUDIO_FS__48KHZ;
            break;
        case HDMI_SAMPLE_RATE_88K:
            AudioFormat.audioFs = SII_AUDIO_FS__88_2KHZ;
            break;
        case HDMI_SAMPLE_RATE_96K:
            AudioFormat.audioFs = SII_AUDIO_FS__96KHZ;
            break;
        case HDMI_SAMPLE_RATE_176K:
            AudioFormat.audioFs = SII_AUDIO_FS__176_4KHZ;
            break;
        case HDMI_SAMPLE_RATE_192K:
            AudioFormat.audioFs = SII_AUDIO_FS__192KHZ;
            break;
        case HDMI_SAMPLE_RATE_768K:
            AudioFormat.audioFs = SII_AUDIO_FS__768KHZ;
            break;
        default:
            AudioFormat.audioFs = SII_AUDIO_FS__48KHZ;
            break;      
    }
    ChannelStatus.i2s_chst3 = s_chst3aTable[AudioFormat.audioFs];

    switch (pstAudioCfg->enSampleDepth)
    {
        case HDMI_AUDIO_BIT_DEPTH_16:
            ChannelStatus.i2s_chst4 = 0x2;
            break;
        case HDMI_AUDIO_BIT_DEPTH_18:
            ChannelStatus.i2s_chst4 = 0x4;
            break;
        case HDMI_AUDIO_BIT_DEPTH_20:
            ChannelStatus.i2s_chst4 = 0xa;
            break;
        case HDMI_AUDIO_BIT_DEPTH_24:
            ChannelStatus.i2s_chst4 = 0xb;
            break;
        default :
            ChannelStatus.i2s_chst4 = 0x2;
            break;           
    }
    
    AudioFormat.layout1 = pstAudioCfg->enLayout;
    SiiDrvTxAudioEnableSet(pstHdmiHal->stHalCtx.hHdmiHw, HI_FALSE);
    SiiDrvTxChannelStatusSet(pstHdmiHal->stHalCtx.hHdmiHw, &ChannelStatus);
    SiiDrvTxAudioFormatSet(pstHdmiHal->stHalCtx.hHdmiHw, &AudioFormat);
	SiiDrvTxAudioEnableSet(pstHdmiHal->stHalCtx.hHdmiHw, pstAudioCfg->bEnableAudio);
    
    memcpy(&pstHdmiHal->stHalCtx.stAudioCfg, pstAudioCfg, sizeof(HDMI_AUDIO_CONFIG_S));
    return HI_SUCCESS;
    
}
#endif

static SiiDrvTxClrSpc_t s_clrSpc[HDMI_COLORSPACE_BUTT][HDMI_CONV_STD_BUTT] =
{
    {SII_DRV_TX_CLRSPC__RGB_FULL,SII_DRV_TX_CLRSPC__RGB_FULL,SII_DRV_TX_CLRSPC__RGB_FULL,SII_DRV_TX_CLRSPC__RGB_FULL},
    {SII_DRV_TX_CLRSPC__YC422_709,SII_DRV_TX_CLRSPC__YC422_601,SII_DRV_TX_CLRSPC__YC422_2020,SII_DRV_TX_CLRSPC__YC422_2020},
    {SII_DRV_TX_CLRSPC__YC444_709,SII_DRV_TX_CLRSPC__YC444_601,SII_DRV_TX_CLRSPC__YC444_2020,SII_DRV_TX_CLRSPC__YC444_2020},
    {SII_DRV_TX_CLRSPC__YC420_709,SII_DRV_TX_CLRSPC__YC420_601,SII_DRV_TX_CLRSPC__YC420_2020,SII_DRV_TX_CLRSPC__YC420_2020},
};

HI_S32 HAL_HDMI_VideoPathSet(HDMI_HAL_S *pstHdmiHal, HDMI_VIDEO_CONFIG_S* pstVideoCfg)
{
    HI_BOOL bIdClkDiv = HI_FALSE, bPixelnxClkDiv = HI_FALSE;
    HI_U32  u32TmdsClk = pstVideoCfg->u32PixelClk;
    SiiDrvTxColorInfoCfg_t clrInfo;
    SiiDrvTxClrSpc_t clrSpc;
    SiiHvSyncPol_t hvSyncPol;
    SiiDrvBitDepth_t bitDepth = SII_DRV_BIT_DEPTH__PASSTHOUGH;

    CHECK_POINTER(pstHdmiHal);
    CHECK_POINTER(pstVideoCfg);

    /* when input is YUV420,idclk need div2. when output is YUV420,pixelnx clk need div2*/
    if (pstVideoCfg->enInColorSpace == HDMI_COLORSPACE_YCbCr420)
    {
        bIdClkDiv = HI_TRUE;
    }  
    
    if (pstVideoCfg->enOutColorSpace == HDMI_COLORSPACE_YCbCr420)
    {
        bPixelnxClkDiv = HI_TRUE;
        u32TmdsClk /= 2;
    }
    //printf("pstVideoCfg->enOutColorSpace=%d\n",pstVideoCfg->enOutColorSpace);
    HalHdmiClkDivSet(bIdClkDiv, bPixelnxClkDiv); 
    SiiLibTimeMilliDelay(1);    
    
    hvSyncPol = pstVideoCfg->enHvSyncPol;
    SiiDrvTxHvSyncPolaritySet(pstHdmiHal->stHalCtx.hHdmiHw, &hvSyncPol);
    
    switch (pstVideoCfg->enDeepColor)
    {
        case HDMI_DEEP_COLOR_24BIT:
            bitDepth = SII_DRV_BIT_DEPTH__8_BIT;
            break;
        case HDMI_DEEP_COLOR_30BIT:
            bitDepth = SII_DRV_BIT_DEPTH__10_BIT;
            u32TmdsClk = (u32TmdsClk * 5)/4;
            break;
        case HDMI_DEEP_COLOR_36BIT:
            bitDepth = SII_DRV_BIT_DEPTH__12_BIT;
            u32TmdsClk = (u32TmdsClk * 3)/2;
            break;
        case HDMI_DEEP_COLOR_48BIT:
            bitDepth = SII_DRV_BIT_DEPTH__16_BIT;
            u32TmdsClk = u32TmdsClk * 2;
            break;
        default:
            bitDepth = SII_DRV_BIT_DEPTH__8_BIT;
            break;
    }

    SiiDrvTxPhyConfig(pstHdmiHal->stHalCtx.hHdmiHw, u32TmdsClk);
    
    if (pstVideoCfg->enDeepColor != pstHdmiHal->stHalCtx.stVideoCfg.enDeepColor)
    {
        SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__SOFTWARE, HI_TRUE);
        SiiLibTimeMilliDelay(1);
        SiiDrvTxOutputBitDepthSet(pstHdmiHal->stHalCtx.hHdmiHw, bitDepth);
        SiiLibTimeMilliDelay(1);
        SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__SOFTWARE, HI_FALSE);
    }
    
    clrSpc = s_clrSpc[pstVideoCfg->enOutColorSpace][pstVideoCfg->enConvStd];
    SiiDrvTxOutputColorSpaceSet(pstHdmiHal->stHalCtx.hHdmiHw, &clrSpc);

    clrInfo.inputClrConvStd = (SiiDrvTxConvStd_t)pstVideoCfg->enConvStd;
    clrInfo.inputVidDcDepth = pstVideoCfg->enInBitDepth + 1;//SII_DRV_BIT_DEPTH__10_BIT;
    clrInfo.inputClrSpc = s_clrSpc[pstVideoCfg->enInColorSpace][pstVideoCfg->enConvStd];
    SiiDrvTxColorInfoConfig(pstHdmiHal->stHalCtx.hHdmiHw, &clrInfo);

    memcpy(&pstHdmiHal->stHalCtx.stVideoCfg, pstVideoCfg, sizeof(HDMI_VIDEO_CONFIG_S));
    return HI_SUCCESS;
}
#if 0
HI_S32 HAL_HDMI_DeepColorSet(HDMI_HAL_S *pstHdmiHal, HDMI_DEEP_COLOR_E enDeepColor)
{
    SiiDrvBitDepth_t bitDepth = SII_DRV_BIT_DEPTH__PASSTHOUGH;

    pstHdmiHal->stHalCtx.stVideoCfg.enDeepColor = enDeepColor;
    switch (enDeepColor)
    {
        case HDMI_DEEP_COLOR_24BIT:
            bitDepth = SII_DRV_BIT_DEPTH__8_BIT;
            break;
        case HDMI_DEEP_COLOR_30BIT:
            bitDepth = SII_DRV_BIT_DEPTH__10_BIT;
            break;
        case HDMI_DEEP_COLOR_36BIT:
            bitDepth = SII_DRV_BIT_DEPTH__12_BIT;
            break;
        case HDMI_DEEP_COLOR_48BIT:
            bitDepth = SII_DRV_BIT_DEPTH__16_BIT;
            break;
        default:
            bitDepth = SII_DRV_BIT_DEPTH__8_BIT;
            break;
    }
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__SOFTWARE, HI_TRUE);
    SiiDrvTxOutputBitDepthSet(pstHdmiHal->stHalCtx.hHdmiHw, bitDepth);
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__SOFTWARE, HI_FALSE);
    return 0;
}
#endif

#ifdef HDMI_HDCP_SUPPORT
HI_VOID HAL_HDMI_HdcpEnableSet(HDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable)
{ 
    if (bEnable)
    {
        /* select clk of hdmi*/
        CipherHdmiClkSet(HI_DRV_HDCPKEY_TX0);
    }
    SiiDrvTxHdcpProtectionSet(pstHdmiHal->stHalCtx.hHdmiHw, bEnable);
}

HI_S32 HAL_HDMI_HdcpModeSet(HDMI_HAL_S *pstHdmiHal, HDMI_HDCP_MODE_E enHdcpMode)
{
    SiiModDsHdcpVersion_t hdcpVer = SII_DRV_DS_HDCP_VER__1X;
    
    if (enHdcpMode == HDMI_HDCP_MODE_2_2)
    {
        hdcpVer = SII_DRV_DS_HDCP_VER__22;
        if (HI_SUCCESS != Hdcp22ProgramLoad(pstHdmiHal, sizeof(s_u8C8051Hdcp22Program), s_u8C8051Hdcp22Program))
        {
            HDMI_ERR("Load c8051 Hdcp2.2 program fail!!!\n");
            return HI_FAILURE;
        }
    }
    SiiDrvTxHdcpVersionSet(pstHdmiHal->stHalCtx.hHdmiHw, hdcpVer);
    return HI_SUCCESS;
}

HI_VOID HAL_HDMI_SinkHdcp22CapabilityGet(HDMI_HAL_S *pstHdmiHal, HI_BOOL* bSupport)
{
    SiiDrvTxHdcp22CapabilityGet(pstHdmiHal->stHalCtx.hHdmiHw, bSupport);
}

static HI_S32 Hdcp22ProgramLoad(HDMI_HAL_S *pstHdmiHal, HI_U32 u32Length, HI_U8 u8BinaryCode[])
{
    HI_S32 timeout = 3000;
    HI_U32 value = 0, u32Len = 0;
    HI_S32 ret = HI_SUCCESS;
    volatile HI_U32 *pu32VirAddr = HI_NULL;
 
    volatile HI_U32 *u32PeriHdmiTxCtrl = (HI_U32 *)IO_ADDRESS(PERI_HDMITX_CTRL_ADDR);
    HDMI_PRINT_IN;
    CHECK_POINTER(pstHdmiHal);
    
    /* reset hdcp2.2*/
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__HDCP2x, HI_TRUE);
    SiiLibTimeMilliDelay(5);
    SiiDrvTxReset(pstHdmiHal->stHalCtx.hHdmiHw, SII_RESET_TYPE__HDCP2x, HI_FALSE);

    /* hdcp2tx_cupd_start high*/
    value = HdmiRegRead(u32PeriHdmiTxCtrl);
    value |= HDMI_HDCP2_CUPD_START;
    HdmiRegWrite(u32PeriHdmiTxCtrl, value);
    SiiLibTimeMilliDelay(2);
    /* hdcp2tx_cupd_hw   high*/
    value = HdmiRegRead(u32PeriHdmiTxCtrl);
    value |= HDMI_HDCP2_CUPD_HW;
    HdmiRegWrite(u32PeriHdmiTxCtrl, value);
    SiiLibTimeMilliDelay(2);
    
    /* write PRAM*/
    /********************************************************/
    pu32VirAddr = (HI_U32 *)IO_ADDRESS(C8051_PRAM_BASE_ADDR);
    while (u32Len < u32Length)
    {
        HdmiRegWrite(pu32VirAddr + u32Len, u8BinaryCode[u32Len]);
        u32Len++;
    }

    /********************************************************/
    
    /* hdcp2tx_cupd_hw   low */
    value = HdmiRegRead(u32PeriHdmiTxCtrl);
    value &= ~HDMI_HDCP2_CUPD_HW;
    HdmiRegWrite(u32PeriHdmiTxCtrl, value);
    SiiLibTimeMilliDelay(2);
    /* hdcp2tx_cupd_done  high*/
    value = HdmiRegRead(u32PeriHdmiTxCtrl);
    value |= HDMI_HDCP2_CUPD_DONE;
    HdmiRegWrite(u32PeriHdmiTxCtrl, value);
    SiiLibTimeMilliDelay(2);
    
    /* hdcp2tx_cchk_done  high*/
    while (!(HDMI_HDCP2_CCHK_DONE & HdmiRegRead(u32PeriHdmiTxCtrl)) && --timeout > 0)
    {
        SiiLibTimeMilliDelay(1);   
    }

    if (!timeout && !(HDMI_HDCP2_CCHK_DONE & HdmiRegRead(u32PeriHdmiTxCtrl)))
    {
       ret = HI_FAILURE;
       HDMI_ERR("Hdcp2.2 check done error\n");
    }
    else
    {
        SiiDrvHdcp2xCupdChkStat_t hdcp2xCupdStat;
        SiiDrvTxHdcp2xCupdStatusGet(pstHdmiHal->stHalCtx.hHdmiHw, &hdcp2xCupdStat);
        if (SII_DRV_HDCP2X_CUPD_CHK__FAIL == hdcp2xCupdStat)
        {
            HDMI_ERR("Hdcp2.2 cupd check fail\n");
            ret = HI_FAILURE;
        }
    }
    
    /* hdcp2tx_cupd_done low*/
    value = HdmiRegRead(u32PeriHdmiTxCtrl);
    value &= ~HDMI_HDCP2_CUPD_DONE;
    HdmiRegWrite(u32PeriHdmiTxCtrl, value);
    SiiLibTimeMilliDelay(2);
    /* hdcp2tx_cupd_start low*/
    value = HdmiRegRead(u32PeriHdmiTxCtrl);
    value &= ~HDMI_HDCP2_CUPD_START;
    HdmiRegWrite(u32PeriHdmiTxCtrl, value); 
    HDMI_PRINT_OUT;
    return ret;
}
#endif

//HI_S32 (*HAL_HDMI_HdcpKsvListGet)(HDMI_HAL_S *pstHdmiHal, HDMI_HDCP_KSVLIST_S* pstKsvList);
HI_VOID HAL_HDMI_PhyOutputEnableSet(HDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable)
{
    SiiDrvTxPhyOutputEnableSet(pstHdmiHal->stHalCtx.hHdmiHw, bEnable);
}


HI_VOID HAL_HDMI_PhyPowerEnableSet(HDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable)
{
    SiiDrvTxPhyPowerOnOffSet(pstHdmiHal->stHalCtx.hHdmiHw, bEnable);
}

#ifndef HDMI_BUILD_IN_BOOT
HI_VOID HAL_HDMI_PhyOutputEnableGet(struct hiHDMI_HAL_S *pstHdmiHal, HI_BOOL* bEnable)
{
   *bEnable =  SiiDrvTxPhyOutputEnableGet(pstHdmiHal->stHalCtx.hHdmiHw);
}

HI_U32 HAL_HDMI_EdidRawDataRead(HDMI_HAL_S *pstHdmiHal, HI_U32 u32Size, HI_U8 u8OutBuffer[])
{
    SiiEdid_t Edid;
    memset(&Edid, 0, sizeof(SiiEdid_t));
    if (SiiDrvTxEdidGet(pstHdmiHal->stHalCtx.hHdmiHw, &Edid))
    {
        memcpy(u8OutBuffer, Edid.b, u32Size);
        return Edid.validLen;
    }
    return 0;
}
#endif

#ifdef HDMI_CEC_SUPPORT
HI_VOID HAL_HDMI_CecEnableSet(HDMI_HAL_S *pstHdmiHal, HI_BOOL bEnable)
{
    HDMI_HAL_CEC_S* pstCec = pstHdmiHal->stHalCtx.pstCec;

    SiiCecEnable(pstCec, bEnable);
}

HI_S32 HAL_HDMI_CecAutoPing(HDMI_HAL_S *pstHdmiHal, HI_U16 u16PhyAddr, HDMI_CEC_NETWORK_S* pstCecNetwork)
{
    HI_S32 s32Ret;
    HDMI_HAL_CEC_S* pstCec = pstHdmiHal->stHalCtx.pstCec;
    pstCec->physicalAddr = u16PhyAddr;
    //pstCec->physicalAddr = SiiDrvTxCecPhysicalAddrGet(pstHdmiHal->stHalCtx.hHdmiHw);
    s32Ret = SiiCecAutoPing(pstCec);
    if (s32Ret != HI_SUCCESS)
    {
        return HI_FAILURE;
    }
    pstCecNetwork->logicalAddr = pstCec->logicalAddr;
    memcpy(pstCecNetwork->network, pstCec->network, sizeof(pstCec->network));
    return HI_SUCCESS;
}

HI_S32 HAL_HDMI_CecMsgSend(HDMI_HAL_S *pstHdmiHal, HDMI_CEC_CMD_S* sptMsg)
{
    HDMI_HAL_CEC_S* pstCec = pstHdmiHal->stHalCtx.pstCec;
    SiiCpiData_t cecFrame;
    memset(&cecFrame, 0, sizeof(SiiCpiData_t));
    cecFrame.opcode = sptMsg->u8Opcode;
    cecFrame.srcDestAddr = MAKE_SRCDEST(sptMsg->enSrcAdd, sptMsg->enDstAdd);
    cecFrame.argCount = sptMsg->stOperand.stRawData.u8Length;
    memcpy((cecFrame.args), sptMsg->stOperand.stRawData.u8Data, cecFrame.argCount);
    return SiiCecMsgSend(pstCec, &cecFrame);
}

HI_S32 HAL_HDMI_CecMsgReceive(HDMI_HAL_S *pstHdmiHal, HDMI_CEC_CMD_S* pstCecCmd)
{
    HI_S32 s32Ret = HI_FAILURE;
    HDMI_HAL_CEC_S* pstCec = pstHdmiHal->stHalCtx.pstCec;
    HDMI_CEC_MSG_S  msg;
    
    memset(&msg, 0, sizeof(HDMI_CEC_MSG_S));
    s32Ret = SiiCecMsgRead(pstCec, &msg);
    if (s32Ret == HI_SUCCESS)
    {
        pstCecCmd->enSrcAdd = GET_CEC_SRC(msg.srcDestAddr);
        pstCecCmd->enDstAdd = GET_CEC_DEST(msg.srcDestAddr);
        pstCecCmd->u8Opcode = msg.opcode;
        pstCecCmd->stOperand.stRawData.u8Length = msg.argCount;
        memcpy(pstCecCmd->stOperand.stRawData.u8Data, msg.args, msg.argCount);
    }

    return s32Ret;
}
#endif

#ifndef HDMI_BUILD_IN_BOOT
#if defined(CONFIG_HDMI_STB_SDK)
static HI_DRV_DISP_FMT_E DISP_TVHDFmtU2V(HI_UNF_ENC_FMT_E U)
{
    if (U <= HI_UNF_ENC_FMT_480P_60)
    {
        return (HI_DRV_DISP_FMT_E)(HI_DRV_DISP_FMT_1080P_60 + (U - HI_UNF_ENC_FMT_1080P_60));
    }
    else
    {
        return HI_DRV_DISP_FMT_1080i_50;
    }
}

static HI_DRV_DISP_FMT_E DISP_TVSDFmtU2V(HI_UNF_ENC_FMT_E U)
{
    switch (U)
    {
        case HI_UNF_ENC_FMT_PAL:
            return HI_DRV_DISP_FMT_PAL;
        case HI_UNF_ENC_FMT_PAL_N:
            return HI_DRV_DISP_FMT_PAL_N;
        case HI_UNF_ENC_FMT_PAL_Nc:
            return HI_DRV_DISP_FMT_PAL_Nc;
        case HI_UNF_ENC_FMT_NTSC:
            return HI_DRV_DISP_FMT_NTSC;
        case HI_UNF_ENC_FMT_NTSC_J:
            return HI_DRV_DISP_FMT_NTSC_J;
        case HI_UNF_ENC_FMT_NTSC_PAL_M:
            return HI_DRV_DISP_FMT_PAL_M;
        case HI_UNF_ENC_FMT_SECAM_SIN:
            return HI_DRV_DISP_FMT_SECAM_SIN;
        case HI_UNF_ENC_FMT_SECAM_COS:
            return HI_DRV_DISP_FMT_SECAM_COS;
        default:
            return HI_DRV_DISP_FMT_PAL;
    }
}

static HI_DRV_DISP_FMT_E DISP_3DFmtU2V(HI_UNF_ENC_FMT_E U)
{
    if (U <= HI_UNF_ENC_FMT_720P_50_FRAME_PACKING)
    {
        return (HI_DRV_DISP_FMT_E)(HI_DRV_DISP_FMT_1080P_24_FP + (U - HI_UNF_ENC_FMT_1080P_24_FRAME_PACKING));
    }
    else
    {
        return HI_DRV_DISP_FMT_1080P_24_FP;
    }
}

static HI_DRV_DISP_FMT_E DISP_4KFmtU2V(HI_UNF_ENC_FMT_E U)
{
    if (U <= HI_UNF_ENC_FMT_4096X2160_60)
    {
        return (HI_DRV_DISP_FMT_E)(HI_DRV_DISP_FMT_3840X2160_24 + (U - HI_UNF_ENC_FMT_3840X2160_24));
    }
    else
    {
        return HI_DRV_DISP_FMT_3840X2160_24;
    }
}

static HI_DRV_DISP_FMT_E DISP_DecimalFmtU2V(HI_UNF_ENC_FMT_E U)
{
    if (U <= HI_UNF_ENC_FMT_1080i_59_94)
    {
        return (HI_DRV_DISP_FMT_E)(HI_DRV_DISP_FMT_3840X2160_23_976 + (U - HI_UNF_ENC_FMT_3840X2160_23_976));
    }
    else
    {
        return HI_DRV_DISP_FMT_3840X2160_23_976;
    }
}


static HI_DRV_DISP_FMT_E DISP_DVIFmtU2V(HI_UNF_ENC_FMT_E U)
{
    if (U <= HI_UNF_ENC_FMT_VESA_2560X1600_60_RB)
    {
        return (HI_DRV_DISP_FMT_E)(HI_DRV_DISP_FMT_861D_640X480_60 + (U - HI_UNF_ENC_FMT_861D_640X480_60));
    }
    else
    {
        return HI_DRV_DISP_FMT_861D_640X480_60;
    }
}

static HI_DRV_DISP_FMT_E DISP_GetEncFmt(HI_UNF_ENC_FMT_E enUnFmt)
{
    if (enUnFmt <= HI_UNF_ENC_FMT_480P_60)
    {
        return DISP_TVHDFmtU2V(enUnFmt);
    }
    else if (enUnFmt <= HI_UNF_ENC_FMT_SECAM_COS)
    {
        return DISP_TVSDFmtU2V(enUnFmt);
    }
    else if (enUnFmt <= HI_UNF_ENC_FMT_720P_50_FRAME_PACKING)
    {
        return DISP_3DFmtU2V(enUnFmt);
    }
    else if (enUnFmt <= HI_UNF_ENC_FMT_VESA_2560X1600_60_RB)
    {
        return DISP_DVIFmtU2V(enUnFmt);
    }
    else if (enUnFmt <= HI_UNF_ENC_FMT_4096X2160_24)
    {
        return DISP_4KFmtU2V(enUnFmt);
    }
    else if (enUnFmt <= HI_UNF_ENC_FMT_1080i_59_94)
    {
        return DISP_DecimalFmtU2V(enUnFmt);
    }
    else if (enUnFmt == HI_UNF_ENC_FMT_BUTT)
    {
        return HI_DRV_DISP_FMT_CUSTOM;
    }
    else
    {
        return HI_DRV_DISP_FMT_PAL;
    }
}

HI_S32 HAL_HDMI_DispFmtGet(HDMI_HAL_S *pstHdmiHal, HI_U32 *u32DispFmt)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_DISP_PARAM_S   stDispParam;
    PDM_EXPORT_FUNC_S   *pstPdmFuncs  = HI_NULL;
    
    *u32DispFmt = HI_UNF_ENC_FMT_720P_50;
    s32Ret = HI_DRV_MODULE_GetFunction(HI_ID_PDM, (HI_VOID**)&pstPdmFuncs);
    if (s32Ret == HI_FAILURE || NULL == pstPdmFuncs
        || NULL == pstPdmFuncs->pfnPDM_GetDispParam)
    {
        HDMI_ERR("get pdm module function failed\r\n");
        return HI_FAILURE;
    }

    s32Ret = pstPdmFuncs->pfnPDM_GetDispParam(HI_DRV_DISPLAY_1, &stDispParam);
    if (s32Ret == HI_FAILURE)
    {
        HDMI_ERR("PDM_GetDispParam failed\r\n");
        return HI_FAILURE;
    }
    *u32DispFmt = DISP_GetEncFmt(stDispParam.enFormat);
    HDMI_INFO("DispFmt from BaseParam is %d(UnfFmt %d)\n",*u32DispFmt,stDispParam.enFormat);
    return s32Ret;
}
#endif
#endif

#if defined(CONFIG_HDMI_STB_SDK)
HDMI_VIDEO_TIMING_E HAL_HDMI_DispFmt2HdmiTiming(HDMI_HAL_S *pstHdmiHal, HI_U32 u32DispFmt)
{
    HDMI_VIDEO_TIMING_E VideoTimingMode;
    switch (u32DispFmt)
    {
        case HI_DRV_DISP_FMT_1080P_60:
        case HI_DRV_DISP_FMT_1080P_59_94:
            VideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_60000;
            break;
        case HI_DRV_DISP_FMT_1080P_50:
            VideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_50000;
            break;
        case HI_DRV_DISP_FMT_1080P_30:
        case HI_DRV_DISP_FMT_1080P_29_97:
            VideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_30000;
            break;
        case HI_DRV_DISP_FMT_1080P_25:
            VideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_25000;
            break;
        case HI_DRV_DISP_FMT_1080P_24:
        case HI_DRV_DISP_FMT_1080P_23_976:
            VideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_24000;
            break;
        case HI_DRV_DISP_FMT_1080i_60:
        case HI_DRV_DISP_FMT_1080i_59_94:
            VideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_60000;
            break;
        case HI_DRV_DISP_FMT_1080i_50:
            VideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_50000;
            break;
        case HI_DRV_DISP_FMT_720P_60:
        case HI_DRV_DISP_FMT_720P_59_94:
            VideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_60000;
            break;
        case HI_DRV_DISP_FMT_720P_50:
            VideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_50000;
            break;
        case HI_DRV_DISP_FMT_576P_50:
            VideoTimingMode = HDMI_VIDEO_TIMING_720X576P_50000;
            break;
        case HI_DRV_DISP_FMT_480P_60:
            VideoTimingMode = HDMI_VIDEO_TIMING_720X480P_60000;
            break;
        case HI_DRV_DISP_FMT_3840X2160_24:
        case HI_DRV_DISP_FMT_3840X2160_23_976:
            VideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_24000;
            break;            
        case HI_DRV_DISP_FMT_3840X2160_25:
            VideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_25000;
            break;  
        case HI_DRV_DISP_FMT_3840X2160_30:
        case HI_DRV_DISP_FMT_3840X2160_29_97:
            VideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_30000;
            break;  
        case HI_DRV_DISP_FMT_4096X2160_24:    
            VideoTimingMode = HDMI_VIDEO_TIMING_4096X2160P_24000;
            break;
        case HI_DRV_DISP_FMT_4096X2160_25:    
            VideoTimingMode = HDMI_VIDEO_TIMING_4096X2160P_25000;
            break;
        case HI_DRV_DISP_FMT_4096X2160_30:    
            VideoTimingMode = HDMI_VIDEO_TIMING_4096X2160P_30000;
            break;
        case HI_DRV_DISP_FMT_3840X2160_50:
            VideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_50000;
            break;  
        case HI_DRV_DISP_FMT_3840X2160_60:
            VideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_60000;
            break; 
        case HI_DRV_DISP_FMT_4096X2160_50:    
            VideoTimingMode = HDMI_VIDEO_TIMING_4096X2160P_50000;
            break;
        case HI_DRV_DISP_FMT_4096X2160_60:    
            VideoTimingMode = HDMI_VIDEO_TIMING_4096X2160P_60000;
            break;
        case HI_DRV_DISP_FMT_1440x576i_50:
        case HI_DRV_DISP_FMT_PAL:
        case HI_DRV_DISP_FMT_PAL_B:
        case HI_DRV_DISP_FMT_PAL_B1:
        case HI_DRV_DISP_FMT_PAL_D:
        case HI_DRV_DISP_FMT_PAL_D1:
        case HI_DRV_DISP_FMT_PAL_G:
        case HI_DRV_DISP_FMT_PAL_H:
        case HI_DRV_DISP_FMT_PAL_K:
        case HI_DRV_DISP_FMT_PAL_I:
        case HI_DRV_DISP_FMT_PAL_M:
        case HI_DRV_DISP_FMT_PAL_N:
        case HI_DRV_DISP_FMT_PAL_Nc:
        case HI_DRV_DISP_FMT_PAL_60:

        case HI_DRV_DISP_FMT_SECAM_SIN:
        case HI_DRV_DISP_FMT_SECAM_COS:
        case HI_DRV_DISP_FMT_SECAM_L:
        case HI_DRV_DISP_FMT_SECAM_B:
        case HI_DRV_DISP_FMT_SECAM_G:
        case HI_DRV_DISP_FMT_SECAM_D:
        case HI_DRV_DISP_FMT_SECAM_K:
        case HI_DRV_DISP_FMT_SECAM_H:
            VideoTimingMode = HDMI_VIDEO_TIMING_720X576I_50000;
            break;
        case HI_DRV_DISP_FMT_1440x480i_60:
        case HI_DRV_DISP_FMT_NTSC:
        case HI_DRV_DISP_FMT_NTSC_J:
        case HI_DRV_DISP_FMT_NTSC_443:
            VideoTimingMode = HDMI_VIDEO_TIMING_720X480I_60000;
            break;
        case HI_DRV_DISP_FMT_861D_640X480_60:
            VideoTimingMode = HDMI_VIDEO_TIMING_640X480P_60000;
            break;
        default:
            HDMI_INFO("Not CEA video timing:%d\n", u32DispFmt);
            //  vesa
            VideoTimingMode = HDMI_VIDEO_TIMING_UNKNOWN;
            break;
    }
    return VideoTimingMode;
}
#endif

HI_S32 HAL_HDMI_Open(HMDI_CALLBACK pCallback, HDMI_HAL_S** pstHalHandle)
{
    SiiInst_t instTx = HI_NULL;
	SiiDrvCraConfig_t craConfig;
    SiiDrvTxConfig_t  txConfig;
    HDMI_HAL_S* pstHdmiHal = HI_NULL;

    pstHdmiHal = (HDMI_HAL_S *)SiiLibMallocCreate(sizeof(HDMI_HAL_S));
    if (pstHdmiHal == HI_NULL)
    {
        HDMI_ERR("Alloc HDMI_HAL_S struct memory fail\n");
        return HI_FAILURE;
    }   

    pstHdmiHal->HAL_HDMI_HardwareInit           = HAL_HDMI_HardwareInit;  
    pstHdmiHal->HAL_HDMI_AvMuteSet               = HAL_HDMI_AvMuteSet;
    pstHdmiHal->HAL_HDMI_TmdsModeSet             = HAL_HDMI_TmdsModeSet;
    pstHdmiHal->HAL_HDMI_InfoframeSet            = HAL_HDMI_InfoframeSet;
    pstHdmiHal->HAL_HDMI_InfoframeEnableSet      = HAL_HDMI_InfoframeEnableSet;    
    pstHdmiHal->HAL_HDMI_VideoPathSet            = HAL_HDMI_VideoPathSet;
    pstHdmiHal->HAL_HDMI_PhyPowerEnableSet       = HAL_HDMI_PhyPowerEnableSet;
    pstHdmiHal->HAL_HDMI_PhyOutputEnableSet      = HAL_HDMI_PhyOutputEnableSet; 
    pstHdmiHal->HAL_HDMI_TxCapabilityGet         = HAL_HDMI_TxCapabilityGet;
#if defined(CONFIG_HDMI_STB_SDK)
    pstHdmiHal->HAL_HDMI_DispFmt2HdmiTiming      = HAL_HDMI_DispFmt2HdmiTiming;
#endif
#ifndef HDMI_BUILD_IN_BOOT
    pstHdmiHal->HAL_HDMI_ScdcStatusGet           = HAL_HDMI_ScdcStatusGet;
    pstHdmiHal->HAL_HDMI_HardwareStatusGet       = HAL_HDMI_HardwareStatusGet;
    pstHdmiHal->HAL_HDMI_SequencerHandlerProcess = HAL_HDMI_SequencerHandlerProcess;
    pstHdmiHal->HAL_HDMI_AudioMuteSet            = HAL_HDMI_AudioMuteSet;
    pstHdmiHal->HAL_HDMI_AudioPathSet            = HAL_HDMI_AudioPathSet;
    pstHdmiHal->HAL_HDMI_EdidRawDataRead         = HAL_HDMI_EdidRawDataRead;  
    pstHdmiHal->HAL_HDMI_PhyOutputEnableGet      = HAL_HDMI_PhyOutputEnableGet;
    pstHdmiHal->HAL_HDMI_HotPlugStatusGet        = HAL_HDMI_HotPlugStatusGet;
    pstHdmiHal->HAL_HDMI_IrqEnableSet            = HAL_HDMI_IrqEnableSet;
    pstHdmiHal->HAL_HDMI_VideoMuteSet            = HAL_HDMI_VideoMuteSet;
#if defined(CONFIG_HDMI_STB_SDK)
    pstHdmiHal->HAL_HDMI_DispFmtGet              = HAL_HDMI_DispFmtGet;
#endif
#endif
#ifdef HDMI_HDCP_SUPPORT
    pstHdmiHal->HAL_HDMI_HdcpEnableSet           = HAL_HDMI_HdcpEnableSet;
    pstHdmiHal->HAL_HDMI_HdcpModeSet             = HAL_HDMI_HdcpModeSet;
    pstHdmiHal->HAL_HDMI_SinkHdcp22CapabilityGet = HAL_HDMI_SinkHdcp22CapabilityGet;
#endif
#ifdef HDMI_CEC_SUPPORT
    pstHdmiHal->HAL_HDMI_CecEnableSet            = HAL_HDMI_CecEnableSet;
    pstHdmiHal->HAL_HDMI_CecAutoPing             = HAL_HDMI_CecAutoPing;
    pstHdmiHal->HAL_HDMI_CecMsgSend              = HAL_HDMI_CecMsgSend;  
    pstHdmiHal->HAL_HDMI_CecMsgReceive           = HAL_HDMI_CecMsgReceive; 
#endif    
#ifdef HDMI_SCDC_SUPPORT
    pstHdmiHal->HAL_HDMI_ScdcConfig              = HAL_HDMI_ScdcConfig;
#endif

    
    pstHdmiHal->stHalCtx.pCallback = pCallback;
        
    *pstHalHandle = pstHdmiHal;

	//Configure Cra
	craConfig.baseAddr  = BASE_ADDRESS;
	instCra	= SiiDrvCraCreate(&craConfig);
    if (instCra == HI_NULL)
    {
        HDMI_ERR("SiiDrvCraCreate fail\n");
        return HI_FAILURE;
    }  

    //Configure & Create Tx
    txConfig.instCra     = instCra;
    txConfig.baseAddr    = BASE_ADDRESS;
	txConfig.bHdcp2xEn	 = HI_FALSE;
	txConfig.bMhlen		 = HI_FALSE;
	txConfig.bMhl3en     = HI_FALSE;
	txConfig.bEsmcEn	 = HI_FALSE;
	txConfig.bMDTEn		 = HI_FALSE;
    txConfig.bCpiEn      = HI_FALSE;
    txConfig.bProgramDino  = HI_FALSE;
    txConfig.bVidPathEn	   = HI_TRUE;
    
	instTx = SiiDrvTxCreate("Hisilicon", &txConfig);
    if (instTx == HI_NULL)
    {
        SiiDrvCraDelete(instCra);
        HDMI_ERR("SiiDrvTxCreate fail!\n"); 
        return HI_FAILURE;
    }

    pstHdmiHal->stHalCtx.hHdmiHw = instTx;
    pstHdmiHal->stHalCtx.enTxCapability = HDMI_TX_SUPPORT_HDMI1_4 | HDMI_TX_SUPPORT_HDMI2_0 | \
                                          HDMI_TX_SUPPORT_HDCP1_4 | HDMI_TX_SUPPORT_HDCP2_2 | \
                                          HDMI_TX_SUPPORT_YUV420 | HDMI_TX_SUPPORT_YUV420_422 | \
                                          HDMI_TX_SUPPORT_YUV422_444 | HDMI_TX_SUPPORT_YUV444_422 | \
                                          HDMI_TX_SUPPORT_YUV422_420 | HDMI_TX_SUPPORT_YUV2RGB | \
                                          HDMI_TX_SUPPORT_RGB2YUV | HDMI_TX_SUPPORT_SCRAMBLE;
    
#ifndef HDMI_BUILD_IN_BOOT
    SiiDrvTxRegisterCallBack(instTx, HalEventCallback, pstHdmiHal);
#endif

#ifdef HDMI_CEC_SUPPORT
    pstHdmiHal->stHalCtx.pstCec = SiiCecDeviceCreate(instCra, CEC_BASE_ADDRESS);
    if (pstHdmiHal->stHalCtx.pstCec == HI_NULL)
    {
        SiiDrvTxDelete(instTx);
        SiiDrvCraDelete(instCra);
        HDMI_ERR("SiiCecDeviceCreate fail!\n"); 
        return HI_FAILURE;
    }
#endif    

    HDMI_INFO("Open hdmi hal device success\n");

    return HI_SUCCESS;
}

void HAL_HDMI_Close(HDMI_HAL_S* pstHdmiHal)
{
    if (pstHdmiHal != HI_NULL)
    {
    #ifdef HDMI_CEC_SUPPORT
        SiiCecDeviceDelete(pstHdmiHal->stHalCtx.pstCec);
    #endif
        SiiDrvTxDelete(pstHdmiHal->stHalCtx.hHdmiHw);
        SiiDrvCraDelete(instCra);
        SiiLibMallocDelete(pstHdmiHal);
        HDMI_INFO("\nClose hdmi hal device success\n");
    }   
}


