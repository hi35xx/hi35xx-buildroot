#ifndef __DRV_HDMI_COMMON_H_
#define __DRV_HDMI_COMMON_H_

#include "hi_type.h"
#include "drv_hdmi_infoframe.h"

/***********************************************************************
    enum
 ***********************************************************************/

typedef enum
{
    HDMI_DEVICE_ID0,
    HDMI_DEVICE_ID_BUTT,    
}HDMI_DEVICE_ID_E;

typedef enum
{
    HDMI_THREAD_STATE_STATR,
    HDMI_THREAD_STATE_STOP,    
}HDMI_THREAD_STATE_E;

typedef enum
{
    HDMI_EVENT_HOTPLUG = 0x10,      
    HDMI_EVENT_HOTUNPLUG,            
    HDMI_EVENT_EDID_FAIL,            
    HDMI_EVENT_HDCP_FAIL,            
    HDMI_EVENT_HDCP_SUCCESS,       
    HDMI_EVENT_RSEN_CONNECT,        
    HDMI_EVENT_RSEN_DISCONNECT,      
    HDMI_EVENT_HDCP_USERSETTING,    
    HDMI_EVENT_SCRAMBLE_FAIL,            
    HDMI_EVENT_SCRAMBLE_SUCCESS,     
    HDMI_EVENT_BUTT
}HDMI_EVENT_E;

typedef enum
{
	HDMI_INT_TYPE_NONE,
	HDMI_INT_TYPE_HPD_RSEN,
	HDMI_INT_TYPE_HDCP,
	HDMI_INT_TYPE_HDCP2x,
	HDMI_INT_TYPE_CEC,
} HDMI_INT_TYPE_E;

typedef HI_S32 (*HMDI_CALLBACK)(HI_VOID*, HDMI_EVENT_E);

#define HDMI_EDID_BLOCK_SIZE             128
#define HDMI_EDID_TOTAL_BLOCKS           4
#define HDMI_EDID_SIZE (HDMI_EDID_BLOCK_SIZE * HDMI_EDID_TOTAL_BLOCKS)
typedef struct 
{
    HI_U8                           u8EdidValid;
    HI_U32                          u32EdidLen;
    HI_U8                           u8Edid[HDMI_EDID_SIZE];
}HDMI_EDID_RAWDATA_S;


typedef struct
{
    HI_U8 *pu8InputEncryptedKey;           
    HI_U32 u32KeyLength;  				 
}HDMI_LOAD_KEY_S;


/**HDMI status*/
/**CNcomment: HDMI状态 */
typedef struct 
{
    HI_BOOL             bConnected;             /**<The Device is connected or disconnected *//**<CNcomment:设备是否连接 */
    HI_BOOL             bSinkPowerOn;           /**<The sink is PowerOn or not*//**<CNcomment:Sink设备是否上电 */
    HI_BOOL             bAuthed;                /**<HDCP Authentication *//**<CNcomment:HDCP 是否握手完成 */
    HI_U8               u8Bksv[5];              /**<Bksv of sink 40bits*//**<CNcomment:接收端的Bksv */
}HDMI_STATUS_S;

/*********************video data Structure*******************************************/

/**HDMI Deep color mode*/
typedef enum 
{
    HDMI_DEEP_COLOR_24BIT = 0x00,    
    HDMI_DEEP_COLOR_30BIT,          
    HDMI_DEEP_COLOR_36BIT,          
    HDMI_DEEP_COLOR_48BIT,           
    HDMI_DEEP_COLOR_OFF   = 0xff,
    HDMI_DEEP_COLOR_BUTT
}HDMI_DEEP_COLOR_E;

typedef enum
{
	HDMI_VIDEO_BITDEPTH_8,
	HDMI_VIDEO_BITDEPTH_10,
	HDMI_VIDEO_BITDEPTH_12,
	HDMI_VIDEO_BITDEPTH_16,
    HDMI_VIDEO_BITDEPTH_BUTT
}HDMI_VIDEO_BITDEPTH_E;

typedef enum
{
	HDMI_HV_SYNC_POL_HPVP,
	HDMI_HV_SYNC_POL_HPVN,
	HDMI_HV_SYNC_POL_HNVP,
	HDMI_HV_SYNC_POL_HNVN
} HDMI_HVSYNC_POLARITY_E;

typedef enum 
{
    HDMI_PICTURE_NON_UNIFORM__SCALING,      
    HDMI_PICTURE_SCALING__H,                
    HDMI_PICTURE_SCALING__V,                 
    HDMI_PICTURE_SCALING__HV                
 }HDMI_PICTURE_SCALING_E;


typedef struct 
{
    HI_U32                      u32ClkFs; 
    HI_U32                      u32PixelRepeat;  
    HDMI_VIDEO_TIMING_E         enVideoTiming;     
    HDMI_3D_MODE_E              enStereoMode;
    HDMI_COLORSPACE_E           enInColorSpace; 
    HDMI_HVSYNC_POLARITY_E      enHvSyncPol;
    HDMI_COLORIMETRY_E          enColorimetry;
    HDMI_EXTENDED_COLORIMETRY   enExtendedColorimetry;
    HDMI_QUANTIZATION_RANGE     enRGBQuantization;
    HDMI_YCC_QUANTIZATION_RANGE enYCCQuantization;
    HDMI_PICTURE_ASPECT         enPictureAspect;
    HDMI_ACTIVE_ASPECT          enActiveAspect; 
    HDMI_PICTURE_SCALING_E      enPictureScaling;
    HDMI_VIDEO_BITDEPTH_E       enInBitDepth;
    HI_U32                      u32DispFmt;
    //HI_BOOL                     bYUV420Enable;
}HDMI_VO_ATTR_S;

/*********************audio data Structure*******************************************/
typedef enum
{
	HDMI_AUDIO_FORMAT_2CH = 0x02,
	HDMI_AUDIO_FORMAT_3CH,
	HDMI_AUDIO_FORMAT_4CH,
	HDMI_AUDIO_FORMAT_5CH,
	HDMI_AUDIO_FORMAT_6CH,
	HDMI_AUDIO_FORMAT_7CH,
	HDMI_AUDIO_FORMAT_8CH
} HDMI_AUDIO_CH_E;

/*hdmi audio interface */
typedef enum 
{
    HDMI_AUDIO_INTERFACE__I2S,
    HDMI_AUDIO_INTERFACE__SPDIF, 
    HDMI_AUDIO_INTERFACE__HBRA, 
    HDMI_AUDIO_INTERFACE__BUTT
}HDMI_AUDIO_INTERFACE_E;

typedef enum 
{
    HDMI_AUDIO_BIT_DEPTH_UNKNOWN =0,
    HDMI_AUDIO_BIT_DEPTH_8  = 8,    
    HDMI_AUDIO_BIT_DEPTH_16 = 16,   
    HDMI_AUDIO_BIT_DEPTH_18 = 18,  
    HDMI_AUDIO_BIT_DEPTH_20 = 20,  
    HDMI_AUDIO_BIT_DEPTH_24 = 24,  
    HDMI_AUDIO_BIT_DEPTH_32 = 32,   
    HDMI_AUDIO_BIT_DEPTH_BUTT
}HDMI_AUDIO_BIT_DEPTH_E;


typedef enum
{
    HDMI_SAMPLE_RATE_UNKNOWN=0,       
    HDMI_SAMPLE_RATE_8K    = 8000,   
    HDMI_SAMPLE_RATE_11K   = 11025,   
    HDMI_SAMPLE_RATE_12K   = 12000,   
    HDMI_SAMPLE_RATE_16K   = 16000,   
    HDMI_SAMPLE_RATE_22K   = 22050,   
    HDMI_SAMPLE_RATE_24K   = 24000,  
    HDMI_SAMPLE_RATE_32K   = 32000,  
    HDMI_SAMPLE_RATE_44K   = 44100, 
    HDMI_SAMPLE_RATE_48K   = 48000, 
    HDMI_SAMPLE_RATE_88K   = 88200,   
    HDMI_SAMPLE_RATE_96K   = 96000,  
	HDMI_SAMPLE_RATE_176K	 = 176400,	
    HDMI_SAMPLE_RATE_192K  = 192000,  
    HDMI_SAMPLE_RATE_768K  = 768000,  
    HDMI_SAMPLE_RATE_BUTT
}HDMI_SAMPLE_RATE_E;


typedef struct 
{
    HI_BOOL                  bDownSample;
	HDMI_SAMPLE_RATE_E       enSampleFs;
    HDMI_AUDIO_CH_E          enChanels;
	HDMI_AUDIO_INTERFACE_E   enSoundIntf;
    HDMI_AUDIO_BIT_DEPTH_E   enSampleDepth;
    HDMI_AUDIO_FORMAT_CODE_E enAudioCode;
}HDMI_AO_ATTR_S;


/*********************APP data Structure*******************************************/
typedef enum
{
    HDMI_TMDS_MODE_NONE,
    HDMI_TMDS_MODE_DVI,
    HDMI_TMDS_MODE_HDMI_1_4,
    HDMI_TMDS_MODE_HDMI_2_0,
    HDMI_TMDS_MODE_AUTO,
}HDMI_TMDS_MODE_E;

typedef enum
{
    HDMI_HDCP_MODE_NONE,
    HDMI_HDCP_MODE_1_4,
    HDMI_HDCP_MODE_2_2,
    HDMI_HDCP_MODE_AUTO,
}HDMI_HDCP_MODE_E;

typedef enum 
{
    HDMI_DEFAULT_ACTION_NULL,   
    HDMI_DEFAULT_ACTION_HDMI,   
    HDMI_DEFAULT_ACTION_DVI,    
    HDMI_DEFAULT_ACTION_BUTT
}HDMI_DEFAULT_ACTION_E;

/*In order to extern ,so we define struct*/
typedef struct hiHDMI_APP_ATTR_S
{
	HI_BOOL                 bEnableHdmi;         /**<force to HDMI or DVI,the value must set before HI_UNF_HDMI_Start or behind HI_UNF_HDMI_Stop*//**<CNcomment:是否强制HDMI,否则为DVI.该值必须在 HI_UNF_HDMI_Start之前或者HI_UNF_HDMI_Stop之后设置  */
    HI_BOOL                 bEnableVideo;        /**<parameter must set HI_TRUE,or the HDMI diver will force to set HI_TRUE*//**<CNcomment:必须是HI_TRUE, 如果是HI_FALSE:HDMI驱动会强制设置为HI_TRUE */
    HI_BOOL                 bEnableAudio;        /**<Enable flag of Audio*//**CNcomment:是否Enable音频 */

    HDMI_COLORSPACE_E       enOutColorSpace;       /**<HDMI output vedio mode VIDEO_MODE_YCBCR,VIDEO_MODE_YCBCR444，VIDEO_MODE_YCBCR422，VIDEO_MODE_RGB444 *//**<CNcomment:HDMI输出视频模式，VIDEO_MODE_YCBCR444，VIDEO_MODE_YCBCR422，VIDEO_MODE_RGB444 */
    HDMI_DEEP_COLOR_E       enDeepColorMode;    /**<Deep Color output mode,defualt: HI_UNF_HDMI_DEEP_COLOR_24BIT *//**<CNcomment:DeepColor输出模式, 默认为HI_UNF_HDMI_DEEP_COLOR_24BIT */
    HI_BOOL                 bxvYCCMode;          /**<the xvYCC output mode,default:HI_FALSE*//**<CNcomment:< xvYCC输出模式，默认为HI_FALSE */

    HI_BOOL                 bEnableAviInfoFrame; /**<Enable flag of AVI InfoFrame,suggestion:enable *//**<CNcomment:是否使能 AVI InfoFrame，建议使能 */
    HI_BOOL                 bEnableSpdInfoFrame; /**<Enable flag of SPD info frame,suggestion:disable*//**<CNcomment:是否使能 SPD InfoFrame， 建议关闭 */
    HI_BOOL                 bEnableMpegInfoFrame;/**<Enable flag of MPEG info frame,suggestion:disable*//**<CNcomment:是否使能 MPEG InfoFrame， 建议关闭 */
    HI_BOOL                 bEnableAudInfoFrame; /**<Enable flag of Audio InfoFrame,suggestion:enable*//**<CNcomment:是否使能 AUDIO InfoFrame，建议使能 */

    HI_U32                  bDebugFlag;          /**<the flag of hdmi dubug,suggestion:disable*//**<CNcomment:< 是否使能 打开hdmi内部debug信息， 建议关闭 */
    HI_BOOL                 bHDCPEnable;         /**<0:HDCP disable mode,1:eable HDCP mode*//**<CNcomment:< 0:HDCP不激活，1:HDCP模式打开 */
    HDMI_DEFAULT_ACTION_E   enHdmiAction;
    HI_BOOL                 bYUV420Enable;
}HDMI_APP_ATTR_S;


typedef struct 
{
    HDMI_AO_ATTR_S  stAOAttr;
    HDMI_VO_ATTR_S  stVOAttr;
    HDMI_APP_ATTR_S stAppAttr;
}HDMI_ATTR_S;


/***************************CEC data Structure *******************************************/
typedef enum 
{
    HDMI_CEC_LOGICALADD_TV               = 0X00, 
    HDMI_CEC_LOGICALADD_RECORDDEV_1      = 0X01, 
    HDMI_CEC_LOGICALADD_RECORDDEV_2      = 0X02, 
    HDMI_CEC_LOGICALADD_TUNER_1          = 0X03, 
    HDMI_CEC_LOGICALADD_PLAYDEV_1        = 0X04,
    HDMI_CEC_LOGICALADD_AUDIOSYSTEM      = 0X05,
    HDMI_CEC_LOGICALADD_TUNER_2          = 0X06,
    HDMI_CEC_LOGICALADD_TUNER_3          = 0X07, 
    HDMI_CEC_LOGICALADD_PLAYDEV_2        = 0X08, 
    HDMI_CEC_LOGICALADD_RECORDDEV_3      = 0X09, 
    HDMI_CEC_LOGICALADD_TUNER_4          = 0X0A, 
    HDMI_CEC_LOGICALADD_PLAYDEV_3        = 0X0B, 
    HDMI_CEC_LOGICALADD_RESERVED_1       = 0X0C, 
    HDMI_CEC_LOGICALADD_RESERVED_2       = 0X0D, 
    HDMI_CEC_LOGICALADD_SPECIALUSE       = 0X0E, 
    HDMI_CEC_LOGICALADD_BROADCAST        = 0X0F, 
    HDMI_CEC_LOGICALADD_BUTT
}HDMI_CEC_LOGICALADD_S;

typedef enum 
{
    HDMI_CEC_UICMD_SELECT                      = 0x00,
    HDMI_CEC_UICMD_UP                          = 0x01,
    HDMI_CEC_UICMD_DOWN                        = 0x02,
    HDMI_CEC_UICMD_LEFT                        = 0x03,
    HDMI_CEC_UICMD_RIGHT                       = 0x04,
    HDMI_CEC_UICMD_RIGHT_UP                    = 0x05,
    HDMI_CEC_UICMD_RIGHT_DOWN                  = 0x06,
    HDMI_CEC_UICMD_LEFT_UP                     = 0x07,
    HDMI_CEC_UICMD_LEFT_DOWN                   = 0x08,
    HDMI_CEC_UICMD_ROOT_MENU                   = 0x09,
    HDMI_CEC_UICMD_SETUP_MENU                  = 0x0A,
    HDMI_CEC_UICMD_CONTENTS_MENU               = 0x0B,
    HDMI_CEC_UICMD_FAVORITE_MENU               = 0x0C,
    HDMI_CEC_UICMD_EXIT                        = 0x0D,
    HDMI_CEC_UICMD_NUM_0                       = 0x20,
    HDMI_CEC_UICMD_NUM_1                       = 0x21,
    HDMI_CEC_UICMD_NUM_2                       = 0x22,
    HDMI_CEC_UICMD_NUM_3                       = 0x23,
    HDMI_CEC_UICMD_NUM_4                       = 0x24,
    HDMI_CEC_UICMD_NUM_5                       = 0x25,
    HDMI_CEC_UICMD_NUM_6                       = 0x26,
    HDMI_CEC_UICMD_NUM_7                       = 0x27,
    HDMI_CEC_UICMD_NUM_8                       = 0x28,
    HDMI_CEC_UICMD_NUM_9                       = 0x29,
    HDMI_CEC_UICMD_DOT                         = 0x2A,
    HDMI_CEC_UICMD_ENTER                       = 0x2B,
    HDMI_CEC_UICMD_CLEAR                       = 0x2C,
    HDMI_CEC_UICMD_NEXT_FAVORITE               = 0x2F,
    HDMI_CEC_UICMD_CHANNEL_UP                  = 0x30,
    HDMI_CEC_UICMD_CHANNEL_DOWN                = 0x31,
    HDMI_CEC_UICMD_PREVIOUS_CHANNEL            = 0x32,
    HDMI_CEC_UICMD_SOUND_SELECT                = 0x33,
    HDMI_CEC_UICMD_INPUT_SELECT                = 0x34,
    HDMI_CEC_UICMD_DISPLAY_INFORMATION         = 0x35,
    HDMI_CEC_UICMD_HELP                        = 0x36,
    HDMI_CEC_UICMD_PAGE_UP                     = 0x37,
    HDMI_CEC_UICMD_PAGE_DOWN                   = 0x38,
    HDMI_CEC_UICMD_POWER                       = 0x40,
    HDMI_CEC_UICMD_VOLUME_UP                   = 0x41,
    HDMI_CEC_UICMD_VOLUME_DOWN                 = 0x42,
    HDMI_CEC_UICMD_MUTE                        = 0x43,
    HDMI_CEC_UICMD_PLAY                        = 0x44,
    HDMI_CEC_UICMD_STOP                        = 0x45,
    HDMI_CEC_UICMD_PAUSE                       = 0x46,
    HDMI_CEC_UICMD_RECORD                      = 0x47,
    HDMI_CEC_UICMD_REWIND                      = 0x48,
    HDMI_CEC_UICMD_FAST_FORWARD                = 0x49,
    HDMI_CEC_UICMD_EJECT                       = 0x4A,
    HDMI_CEC_UICMD_FORWARD                     = 0x4B,
    HDMI_CEC_UICMD_BACKWARD                    = 0x4C,
    HDMI_CEC_UICMD_STOP_RECORD                 = 0x4D,
    HDMI_CEC_UICMD_PAUSE_RECORD                = 0x4E,
    HDMI_CEC_UICMD_ANGLE                       = 0x50,
    HDMI_CEC_UICMD_SUBPICTURE                  = 0x51,
    HDMI_CEC_UICMD_VIDEO_ON_DEMAND             = 0x52,
    HDMI_CEC_UICMD_ELECTRONIC_PROGRAM_GUIDE    = 0x53,
    HDMI_CEC_UICMD_TIMER_PROGRAMMING           = 0x54,
    HDMI_CEC_UICMD_INITIAL_CONFIGURATION       = 0x55,
    HDMI_CEC_UICMD_PLAY_FUNCTION               = 0x60,
    HDMI_CEC_UICMD_PAUSE_PLAY_FUNCTION         = 0x61,
    HDMI_CEC_UICMD_RECORD_FUNCTION             = 0x62,
    HDMI_CEC_UICMD_PAUSE_RECORD_FUNCTION       = 0x63,
    HDMI_CEC_UICMD_STOP_FUNCTION               = 0x64,
    HDMI_CEC_UICMD_MUTE_FUNCTION               = 0x65,
    HDMI_CEC_UICMD_RESTORE_VOLUME_FUNCTION     = 0x66,
    HDMI_CEC_UICMD_TUNE_FUNCTION               = 0x67,
    HDMI_CEC_UICMD_SELECT_MEDIA_FUNCTION       = 0x68,
    HDMI_CEC_UICMD_SELECT_AV_INPUT_FUNCTION    = 0x69,
    HDMI_CEC_UICMD_SELECT_AUDIO_INPUT_FUNCTION = 0x6A,
    HDMI_CEC_UICMD_POWER_TOGGLE_FUNCTION       = 0x6B,
    HDMI_CEC_UICMD_POWER_OFF_FUNCTION          = 0x6C,
    HDMI_CEC_UICMD_POWER_ON_FUNCTION           = 0x6D,
    HDMI_CEC_UICMD_F1_BLUE                     = 0x71,
    HDMI_CEC_UICMD_F2_RED                      = 0x72,
    HDMI_CEC_UICMD_F3_GREEN                    = 0x73,
    HDMI_CEC_UICMD_F4_YELLOW                   = 0x74,
    HDMI_CEC_UICMD_F5                          = 0x75,
    HDMI_CEC_UICMD_DATA                        = 0x76
}HDMI_CEC_UICMD_E;


typedef struct 
{
    HI_U8                              u8Length;  
    HI_U8                              u8Data[15];    
}HDMI_CEC_RAWDATA_S;

typedef union 
{
    HDMI_CEC_RAWDATA_S               stRawData;   
    HDMI_CEC_UICMD_E                 enUIOpcode; 
}HDMI_CEC_Operand_S;


typedef struct 
{
    HDMI_CEC_LOGICALADD_S  enSrcAdd;     
    HDMI_CEC_LOGICALADD_S  enDstAdd;    
    HI_U8                  u8Opcode;     
    HDMI_CEC_Operand_S     stOperand;  
}HDMI_CEC_CMD_S;


typedef struct 
{
    HI_BOOL bCecIsOn; 
    HI_BOOL bCecWasOn;    
    HI_U8   u8PhysicalAddr[4];                    
    HI_U8   u8LogicalAddr;                         
    HI_U8   u8Network[HDMI_CEC_LOGICALADD_BUTT];  
}HDMI_CEC_INFO_S;


typedef struct
{
    HDMI_THREAD_STATE_E  kThreadTimerSate;
    struct task_struct  *kThreadTimer;    /*timer thread*//*CNcomment:定时器线程 */
   // struct task_struct  *kThreadCec;      /*CEC thread*//*CNcomment: CEC线程 */
}HDMI_THREAD_INFO_S;

typedef enum
{
    HDMI_TRANSITION_NONE = 0x0,
    HDMI_TRANSITION_BOOT_MCE = 0x1,
    HDMI_TRANSITION_MCE_APP = 0x2,
    HDMI_TRANSITION_BOOT_APP = 0x4,
}HDMI_TRANSITION_STATE_E;

typedef enum
{
	HDMI_RUN_STATE_NONE 	= 0x00,
    HDMI_RUN_STATE_INIT 	= 0x01,
    HDMI_RUN_STATE_OPEN 	= 0x02,
    HDMI_RUN_STATE_START 	= 0x04,
    HDMI_RUN_STATE_STOP 	= 0x08,
    HDMI_RUN_STATE_CLOSE 	= 0x10, 
    HDMI_RUN_STATE_DEINIT 	= 0x20,    
}HDMI_RUN_STATE_E;


#if 0
typedef struct
{
	uint8_t         deviceCount;		//!< Total number of attached HDCP devices.
	uint8_t         depth;				//!< Number of attached repeater levels.
	uint8_t         maxDevsExceeded;	//!< \c 1: more than supported number of devices attached.
										//!< \c 0: \c deviceCount is valid.
	uint8_t         maxCascadeExceeded; //!< \c 1: more than supported number of levels attached.
										//!< \c 0: \c depth is valid.
	uint8_t         hdcp20RepeaterDs;	//!< \c 1: there is an HDCP 2.0 compliant repeater in the topology.
										//!< \c 0: there is no HDCP 2.0 compliant repeaters in the topology.
	uint8_t         hdcp1xRepeaterDs;	//!< \c 1: there is an HDCP 1.x compliant repeater in the topology.
										//!< \c 0: there is no HDCP 1.x compliant repeaters in the topology.
} SiiDrvHdcpTopology_t;
#endif


typedef struct
{
	HI_U16      length;
	HI_U8*  	pList;
	HI_U8*		pListStart;
}HDMI_HDCP_KSVLIST_S;


typedef struct 
{
    HI_BOOL             bHdcpIsOn;
    HI_BOOL             bHdcpWasOn;
    HI_BOOL             bHdcp1Prep;
    //HI_BOOL             bHdcp2Prep;
    HDMI_HDCP_MODE_E    enHdcpMode;
}HDMI_HDCP_INFO_S;

typedef enum
{
    HDMI_TX_SUPPORT_HDMI1_4    = 0x1,
    HDMI_TX_SUPPORT_HDMI2_0    = 0x2,
    HDMI_TX_SUPPORT_HDCP1_4    = 0x4,
    HDMI_TX_SUPPORT_HDCP2_2    = 0x8,
    HDMI_TX_SUPPORT_YUV420     = 0x10,
    HDMI_TX_SUPPORT_YUV420_422 = 0x20,
    HDMI_TX_SUPPORT_YUV422_444 = 0x40,
    HDMI_TX_SUPPORT_YUV444_422 = 0x80,
    HDMI_TX_SUPPORT_YUV422_420 = 0x100,
    HDMI_TX_SUPPORT_YUV2RGB    = 0x200,
    HDMI_TX_SUPPORT_RGB2YUV    = 0x400,
    HDMI_TX_SUPPORT_SCRAMBLE   = 0x800,
}HDMI_TX_CAPABILITY_E;


typedef enum
{
	HDMI_CONV_STD_BT_709,
	HDMI_CONV_STD_BT_601,
	HDMI_CONV_STD_BT_2020_non_const_luminous,
	HDMI_CONV_STD_BT_2020_const_luminous,
	HDMI_CONV_STD_BUTT
}HDMI_CONVERSION_STD_E;

typedef struct 
{
    HI_U32                  u32PixelClk;
    HDMI_HVSYNC_POLARITY_E  enHvSyncPol;    
    HDMI_CONVERSION_STD_E   enConvStd;
    HDMI_QUANTIZATION_RANGE enQuantization;
    HDMI_COLORSPACE_E       enInColorSpace;   
    HDMI_COLORSPACE_E       enOutColorSpace; 
    HDMI_DEEP_COLOR_E       enDeepColor;
    HDMI_VIDEO_BITDEPTH_E   enInBitDepth;
}HDMI_VIDEO_CONFIG_S;

typedef struct 
{
	HI_BOOL					 bEnableAudio;
    HI_BOOL                  bDownSample;
	HDMI_SAMPLE_RATE_E       enSampleFs;
    HDMI_AUDIO_CH_E          enLayout;
	HDMI_AUDIO_INTERFACE_E   enSoundIntf;
    HDMI_AUDIO_BIT_DEPTH_E   enSampleDepth;
    //HDMI_AUDIO_FORMAT_CODE_E enAudioCode;
}HDMI_AUDIO_CONFIG_S;

typedef struct 
{                           
    HI_U8   logicalAddr;                          
    HI_U8   network[16]; 
}HDMI_CEC_NETWORK_S;

typedef struct
{
    HI_BOOL                 bPhyOe;
    HI_BOOL                 bPhyPowerOn;
    HDMI_VIDEO_BITDEPTH_E   enDeepColor;
}HDMI_PHY_STATUS_S;

typedef struct
{
	HI_BOOL                     bVideoMute; 
    HI_BOOL                     bYCbCr2RGB;     
    HI_BOOL                     bRGB2YCbCr;     
    HI_BOOL                     bYCbCr444_422;     
    HI_BOOL                     bYCbCr422_420;  
    HI_BOOL                     bYCbCr420_422;     
    HI_BOOL                     bYCbCr422_444;
    HI_BOOL                     bIn420Ydemux;    
    HI_BOOL                     bOut420Ydemux;
    HI_BOOL                     bDither;
    HDMI_COLORSPACE_E           enInColorSpace;   
    HDMI_COLORSPACE_E           enOutColorSpace; 
    HDMI_VIDEO_BITDEPTH_E       enOutBitDepth;
    HDMI_HVSYNC_POLARITY_E      enHvSyncPol; 
}HDMI_VIDEO_STATUS_S;

typedef struct
{
    HI_BOOL                  bAudioMute; 
    HI_BOOL                  bAudioEnable;    
    HI_BOOL                  bDownSample;
	HDMI_SAMPLE_RATE_E       enSampleFs;
    HDMI_AUDIO_CH_E          enLayout;
	HDMI_AUDIO_INTERFACE_E   enSoundIntf;
    HDMI_AUDIO_BIT_DEPTH_E   enSampleDepth;
}HDMI_AUDIO_STATUS_S;

typedef struct
{
    HI_BOOL                     bHotPlug;   
    HI_BOOL                     bRsen;     
    HI_BOOL                     bAvMute; 
    HDMI_TMDS_MODE_E            enTmdsMode;
}HDMI_COMMON_STATUS_S;

typedef struct
{
    HI_BOOL                     bSourceScrambleOn;   
    HI_BOOL                     bSinkScrambleOn;     
    HI_U8                       u8TmdsBitClkRatio;
}HDMI_SCDC_STATUS_S;


#define HDMI_INFO_FRAME_MAX_SIZE			32

typedef struct
{
	HI_BOOL        bAVIEnable;
    HI_BOOL        bAUDIOEnable;
	HI_BOOL        bVSIFEnable;
    HI_BOOL        bSPDEnable;    
    HI_BOOL        bMPEGEnable; 
    HI_BOOL        bGBDEnable;
	HI_U8          u8AVI[HDMI_INFO_FRAME_MAX_SIZE];
    HI_U8          u8AUDIO[HDMI_INFO_FRAME_MAX_SIZE];
    HI_U8          u8VSIF[HDMI_INFO_FRAME_MAX_SIZE];
    HI_U8          u8SPD[HDMI_INFO_FRAME_MAX_SIZE];
    HI_U8          u8MPEG[HDMI_INFO_FRAME_MAX_SIZE];
    HI_U8          u8GDB[HDMI_INFO_FRAME_MAX_SIZE];
}HDMI_INFOFRAME_STATUS_S;

typedef struct
{
    HI_BOOL        bHdcp2_2Enable;
    HI_BOOL        bHdcp1_4Enable;
    HI_BOOL        bRepeaterOn;
    HI_U8          u8BKSVs[5];
    HI_U8          u8AKSVs[5];
    HI_U8          hdcpStatus;
}HDMI_HDCP_STATUS_S;

typedef struct
{
    HDMI_COMMON_STATUS_S        stCommonStatus;
    HDMI_PHY_STATUS_S           stPhyStatus;
    HDMI_VIDEO_STATUS_S         stVideoStatus;
    HDMI_AUDIO_STATUS_S         stAudioStatus;  
    HDMI_INFOFRAME_STATUS_S     stInfoFrameStatus;
    HDMI_HDCP_STATUS_S          stHdcpStatus;
}HDMI_HARDWARE_STATUS_S;

typedef struct
{
    HI_U32    u32MaxTmdsCharacterRate;        
	HI_BOOL   bScdcPresent; 
    HI_BOOL   bRRCapable;  
    HI_BOOL   bLTE340McscScramble;   
	HI_BOOL   b3DOsdDisparity;        
	HI_BOOL   bDualView;              
    HI_BOOL   bIndependentView;      
	HI_BOOL   bDc30bit420;            
	HI_BOOL   bDc36bit420;            
	HI_BOOL   bDc48bit420; 
    HI_BOOL   bScdcEnable;
} HDMI_SCDC_CONFIG_S;

/**HDMI Delay struct*/
typedef struct 
{
    HI_U32  u32MuteDelay;           /**<delay for avmute */
    HI_U32  u32FmtDelay;  			/**<delay for setformat */
    HI_BOOL bForceFmtDelay;         /**<force setformat delay mode */
    HI_BOOL bForceMuteDelay;        /**<force avmute delay mode */
}HDMI_DELAY_S;


#endif //__DRV_HDMI_COMMON_H_
