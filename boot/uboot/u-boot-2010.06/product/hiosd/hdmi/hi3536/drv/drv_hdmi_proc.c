/******************************************************************************

  Copyright (C), 2014-2024, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : drv_hdmi_proc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2015/02/01
  Description   :
  History       :
  Date          : 2015/02/01
  Author        : t00273561
  Modification  :
*******************************************************************************/

#include "drv_hdmi_proc.h"
#include "drv_hdmi_intf.h"
#include "drv_hdmi_common.h"
#include "drv_hdmi_event.h"
#include "drv_hdmi_edid.h"


/***********************************************************************
    HDMIµÄ°æ±¾ºÅ
 ***********************************************************************/
#define HDMI_VER_MAJOR    2
#define HDMI_VER_MINOR    0
#define HDMI_VER_REVISE   0
#define HDMI_VER_DATE     20150630
#define HDMI_VER_TIMES    0

#define MAKE_VER_BIT(x) # x
#define MAKE_MACRO2STR(exp) MAKE_VER_BIT(exp)
#define MAKE_VERSION \
    MAKE_MACRO2STR(HDMI_VER_MAJOR) "." \
    MAKE_MACRO2STR(HDMI_VER_MINOR) "." \
    MAKE_MACRO2STR(HDMI_VER_REVISE) "." \
    MAKE_MACRO2STR(HDMI_VER_DATE) "." \
    MAKE_MACRO2STR(HDMI_VER_TIMES)

const static HI_CHAR* g_pszVersion   = "HDMI Version:   "MAKE_VERSION;

//#define PROC_PRINT(fmt...)		seq_printf(pFile,fmt)
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)		(sizeof(a)/sizeof(a[0]))
#endif
#define STR_YES				"YES"
#define STR_NO					"NO"
#define STR_NA					"N/A"
#define STR_ERROR				"ERROR"
#define STR_ENABLE				"ENABLE "
#define STR_DISABLE			"DISABLE"

#define 	PROC_FEILD_RATE_DEVIATION	50
#define 	PROC_PIXL_FREQ_DEVIATION	100
#define 	HDMI_VIC_PC_BASE 			((HDMI_VIC_VIRTUAL_BASE)+0x100)




#define CHECK_RET_SUCCESS(ret) \
	do{ 					 \
		if(HI_SUCCESS != ret){\
			HDMI_ERR("Error In %s,Line %d,return not success!\n",__FUNCTION__,__LINE__);	   \
			return ;\
		} \
	}while(0) 


#define CHECK_POINTER(p)		do{	\
	if(p==HI_NULL){					\
		HDMI_ERR("%s %d null pointer!\n",__FUNCTION__,__LINE__);	\
		return ;					\
	}								\
}while(0)

#if 0
typedef struct {
	HI_CHAR *pcOutColorSpace;
	HI_CHAR *pcColorimetry ;
	HI_CHAR *pcPixelRepetition;
	HI_CHAR *pcScanInfo;
	HI_CHAR *pcAspectRatio;
}HDMI_AVIIF_PARSE_S;

#endif	

typedef union
{
    struct
    {
        HI_U8    bzScanInfo            : 2   ; // [0..1] 
        HI_U8    bzBarDataPresent      : 2   ; // [2..3] 
        HI_U8    bzActFmtInfoPresent   : 1   ; // [5] 
        HI_U8    bzColorSpace          : 3   ; // [6..7] 
    } bits;

    HI_U8    u8;

} HDMI_AVIIF_BYTE1_U;

typedef union
{
    struct
    {
        HI_U8    bzActAspRatio         : 4   ; // [0..3] 
        HI_U8    bzPictureAspectRatio  : 2   ; // [4..5] 
        HI_U8    bzColorimetry         : 2   ; // [6..7] 
    } bits;

    HI_U8    u8;

} HDMI_AVIIF_BYTE2_U;


typedef union
{
    struct
    {
        HI_U8    bzPictureScaling     	: 2   ; // [0..1] 
        HI_U8    bzRgbQuantization   	: 2   ; // [2..3] 
        HI_U8    bzExtColorimetry 		: 3   ; // [4..6] 
        HI_U8    bzItContentValid	 	: 1   ; // [7] 
    } bits;

    HI_U8    u8;

} HDMI_AVIIF_BYTE3_U;




typedef union
{
    struct
    {
        HI_U8    bzPixelRepetition     	: 4   ; // [0..3] 
        HI_U8    bzITContentType   	   	: 2   ; // [4..5] 
        HI_U8    bzYCCQuantizationRange	: 2   ; // [6..7] 
    } bits;

    HI_U8    u8;

} HDMI_AVIIF_BYTE5_U;


typedef union
{
    struct
    {
        HI_U8    bzAudioChanelCnt     	: 3   ; // [0..2] 
        HI_U8    bzF13   	   			: 1   ; // [3] 
        HI_U8    bzAduioCodingType		: 4   ; // [4..7] 
    } bits;

    HI_U8    u8;

} HDMI_AUDIO_IF_BYTE1_U;

typedef union
{
    struct
    {
        HI_U8    bzAudioSampingSize     : 2   ; // [0..1] 
        HI_U8    bzAudioSampingFreq  	: 3   ; // [2..4] 
        HI_U8    bzF25toF27				: 3   ; // [5..7] 
    } bits;

    HI_U8    u8;

} HDMI_AUDIO_IF_BYTE2_U;

typedef union
{
	/* EIA-CEA-861-F P66 */
    struct
    {
        HI_U8    bzLFE_PlayBack     	: 2   ; // [0..1] 
        HI_U8    bzF52 	   				: 1   ; // [2] 
        HI_U8    bzLevelShiftValue 		: 4   ; // [3..6] 
        HI_U8    bzDownMixInhibit		: 1   ; // [7] 
    } bits;

    HI_U8    u8;

} HDMI_AUDIO_IF_BYTE5_U;

typedef struct 
{
	HI_U32				u32Vic; 
	HI_U32				u32PixlFreq;				/* KHz */
	HI_U32				u32HorActive;	
	HI_U32				u32VerActive;
	HI_U32				u32FieldRate;				/* 0.01Hz */
	HI_CHAR				*pcFormat;
}HDMI_FMT_NAME_PARAM_S;


const static HI_CHAR *s_aStr_LFE_PlayBack[] = 
{
	/* EIA-CEA-861-F P66 */
	"Un-known",			//0b00
	"0 dB",				//0b01
	"+10 dB",			//0b10
	"Reserved",			//0b11
	
};

const static HI_CHAR *s_aStr_ActAspectRatio[] = 
{
	/* EIA-CEA-861-F P159 */

	"Reserved",			//0000 reserved,
	"Reserved",			//0001 reserved
	"16:9 top",			//0010 box 16:9 (top)
	"14:9 top",			//0011 box 14:9 (top)
	"16:9 BoxCenter",	//0100 box > 16:9 (center)
	"Reserved",			//0101 - 0111 reserved,
	"Reserved",			//0101 - 0111 reserved,
	"Reserved",			//0101 - 0111 reserved,
	"Same as Picture",	//1000 As the coded frame, 		 
	"4:3 Center",		//1001 4:3 (center),			 
	"16:9 Center",		//1010 16:9 (center), 		 
	"14:9 Center",		//1011 14:9 (center),
	"Reserved",			//1100 reserved,
	"4:3 14_9",			//1101 4:3 (with shoot & prot14:9 center),
	"16:9 14_9",		//1110 16:9 (with shoot & protec14:9 center),
	"16:9 4_3",			//1111 16:9 (with shoot & prot4:3 center),
};


const static HI_CHAR *s_aStrScanInfo[] = 
{
	/* EIA-CEA-861-F P43 */
	"No Data",			//0b00
	"overscanned",		//0b01
	"underscanned",		//0b10
	"Reserved",			//0b11
	
};

const static HI_CHAR *s_aStr_Colorimetry[] = 
{
	/* EIA-CEA-861-F P44 */
	"No Data",			//0b00
	"SMPTE 170M",		//0b01
	"ITU-R BT.709",		//0b10
	"Extended",			//0b11	
};

const static HI_CHAR *s_aStr_PictureAspectRatio[] = 
{
	/* EIA-CEA-861-F P44 */
	"No Data",			//0b00
	"4:3",				//0b01
	"16:9",				//0b10
	"Reserved",			//0b11	
};

const static HI_CHAR *s_aStr_BarDataPresent[] = 
{
	/* EIA-CEA-861-F P43*/
	"no",			//0b00 Bar Data not present 
	"HnVp",			//0b01 Vert. BarInfo present 
	"HpVn",			//0b10 Horiz. BarInfo present 
	"HpVp",			//0b11 Vert. and Horiz. BarInfo present 

};


const static HI_CHAR *s_aStr_ITContentType[] = 
{
	/* EIA-CEA-861-F P49*/
	"Graphics",			//0b00 
	"Photo",			//0b01 
	"Cinema",			//0b10 
	"Game",				//0b11 
};
const static HI_CHAR *s_aStr_YCCQuantizationRange[] = 
{
	/* EIA-CEA-861-F P50*/
	"Limited",			//0b00   
	"Full",				//0b01   
	"Reserved",			//0b10  
	"Reserved",			//0b11  
};
const static HI_CHAR *s_aStrPixelRepetition[] = 
{
	"No Repetition",	//0b0000
	"2 times",			//0b0001
	"3 times",			//0b0010
	"4 times",			//0b0011
	"5 times",			//0b0100
	"6 times",			//0b0101
	"7 times",			//0b0110
	"8 times",			//0b0111
	"9 times",			//0b1000
	"10 times",			//0b1001
	"Reserved",			//0x0a
	"Reserved",			//0x0b
	"Reserved",			//0x0c
	"Reserved",			//0x0d
	"Reserved",			//0x0e
	"Reserved",			//0x0f
};


const static HI_CHAR *s_aStrAudioCodingType[] = 
{
	/* EIA-CEA-861-F P62 */
	"Stream head",		//0b0000
	"L-PCM",			//0b0001
	"AC-3",				//0b0010
	"MPEG-1",			//0b0011
	"MP3",				//0b0100
	"MPEG2",			//0b0101
	"AAC LC",			//0b0110
	"DTS",				//0b0111
	"ATRAC",			//0b1000
	"OneBitAudio",		//0b1001
	"EnhancedAC3",		//0b1010
	"DTS-HD",			//0b1011
	"MAT",				//0b1100
	"DST",				//0b1101
	"WMA Pro",			//0b1110
	"Extension",		//0b1111
};


const static HI_CHAR *s_aStrAudioChannelCnt[] = 
{
	/* EIA-CEA-861-F P62 */
	"Stream header",	//0b000
	"2 channels",		//0b001
	"3 channels",		//0b010
	"4 channels",		//0b011
	"5 channels",		//0b100
	"6 channels",		//0b101
	"7 channels",		//0b110
	"8 channels",		//0b111
};


const static HI_CHAR *s_aStrAudioSampingFreq[] = 
{
	/* EIA-CEA-861-F P62 */
	"Stream header",	//0b000
	"32 kHz",			//0b001
	"44.1 kHz",			//0b010
	"48 kHz",			//0b011
	"88.2 kHz",			//0b100
	"96 kHz",			//0b101
	"176.4 kHz",		//0b110
	"192 kHz",			//0b111
};


const static HI_CHAR *s_aStrAudioSampingSize[] = 
{
	/* EIA-CEA-861-F P62 */
	"Stream header",	//0b00
	"16 bit",			//0b01
	"20 bit",			//0b10
	"24 bit",			//0b11	
};



const static HI_CHAR *s_aStrRunState[] = 
{
	//"NONE",				//HDMI_RUN_STATE_NONE 	= 0x00,
    "INIT",				//HDMI_RUN_STATE_INIT 	= 0x01,
    "OPEN",				//HDMI_RUN_STATE_OPEN 	= 0x02,
    "START",			//HDMI_RUN_STATE_START 	= 0x04,
    "STOP",				//HDMI_RUN_STATE_STOP 	= 0x08,
    "CLOSE",			//HDMI_RUN_STATE_CLOSE 	= 0x10, 
    "DEINIT",			//HDMI_RUN_STATE_DEINIT 	= 0x20,

};
#if 0
const static HI_CHAR *s_aSrt_enTransitionState[]=

{
    "NONE",				//HDMI_TRANSITION_NONE = 0x0,
    "BOOT_MCE",			//HDMI_TRANSITION_BOOT_MCE = 0x1,
    "MCE_BOOT",			//HDMI_TRANSITION_MCE_APP = 0x2,
    "BOOT_APP",			//HDMI_TRANSITION_BOOT_APP = 0x4,
};
#endif

const static HI_CHAR *s_aStrTmdsMode[] =
{
    "NONE",				//HDMI_TMDS_MODE_NONE = 0,
    "DVI",				//HDMI_TMDS_MODE_DVI = 1,
    "HDMI1.4",			//HDMI_TMDS_MODE_HDMI_1_4 = 2,
    "HDMI2.0",			//HDMI_TMDS_MODE_HDMI_2_0 =3,
    //"AUTO",				//HDMI_TMDS_MODE_AUTO = 4,
};
#if defined(CONFIG_HDMI_STB_SDK)
const static HI_CHAR *s_aStr_enHdcpMode[] =
{
	"NONE",				//HDMI_HDCP_MODE_NONE = 0,
    "HDCP1.4",			//HDMI_HDCP_MODE_1_4 = 1,
    "HDCP2.2",			//HDMI_HDCP_MODE_2_2=2,
    "AUTO",				//HDMI_HDCP_MODE_AUTO=3,
};

const static HI_CHAR *s_aStr_u8Network[] =
{
	"TV",				//HDMI_CEC_LOGICALADD_TV               = 0X00, 
    "RECORDDEV_1",		//HDMI_CEC_LOGICALADD_RECORDDEV_1      = 0X01, 
    "RECORDDEV_2",		//HDMI_CEC_LOGICALADD_RECORDDEV_2      = 0X02, 
    "TUNER_1",			//HDMI_CEC_LOGICALADD_TUNER_1          = 0X03, 
    "PLAYDEV_1",		//HDMI_CEC_LOGICALADD_PLAYDEV_1        = 0X04,
    "AUDIOSYSTEM",		//HDMI_CEC_LOGICALADD_AUDIOSYSTEM      = 0X05,
    "TUNER_2",			//HDMI_CEC_LOGICALADD_TUNER_2          = 0X06,
    "TUNER_3",			//HDMI_CEC_LOGICALADD_TUNER_3          = 0X07, 
    "PLAYDEV_2",		//HDMI_CEC_LOGICALADD_PLAYDEV_2        = 0X08, 
    "RECORDDEV_3",		//HDMI_CEC_LOGICALADD_RECORDDEV_3      = 0X09, 
    "TUNER_4",			//HDMI_CEC_LOGICALADD_TUNER_4          = 0X0A, 
    "PLAYDEV_3",		//HDMI_CEC_LOGICALADD_PLAYDEV_3        = 0X0B, 
    "RESERVED_1",		//HDMI_CEC_LOGICALADD_RESERVED_1       = 0X0C, 
    "RESERVED_2",		//HDMI_CEC_LOGICALADD_RESERVED_2       = 0X0D, 
    "SPECIALUSE",		//HDMI_CEC_LOGICALADD_SPECIALUSE       = 0X0E, 
    "BROADCAST",		//HDMI_CEC_LOGICALADD_BROADCAST        = 0X0F, 
						//HDMI_CEC_LOGICALADD_BUTT
};
#endif /*#if defined(CONFIG_HDMI_STB_SDK)*/
const static HI_CHAR *s_aStr_ColorSpace[] =
{
	/* EIA-CEA-861-F P43 */
	"RGB",				//HDMI_COLORSPACE_RGB = 0, 		0b000
	"YCbCr422",			//HDMI_COLORSPACE_YCbCr422 =1 ,	0b001
	"YCbCr444",			//HDMI_COLORSPACE_YCbCr444 = 2,	0b010
	"YCbCr420",			//HDMI_COLORSPACE_YCbCr420 = 3,	0b011
	"Reserved",			//0b100
	"Reserved",			//0b101
	"Reserved",			//0b110
	"IDO-Defined",		//0b111

};
	
const static HI_CHAR *s_aStr_PictureScaling[] = 
{
	"un-known", 			//0b00
	"HpVn", 				//0b01 Picture has been scaled horizontally 
	"HnVp ",				//0b10 Picture has been scaled vertically
	"HpVp", 				//0b11 Picture has been scaled horizontally and vertically
};

const static HI_CHAR *s_aStr_RgbQuantization[] =
{
	"default",				//HDMI_RGB_QUANTIZATION_DEFAULT_RANGE	0b00
	"limited",				//HDMI_RGB_QUANTIZATION_LIMITED_RANGE	0b01
	"full", 				//HDMI_RGB_QUANTIZATION_FULL_RANGE		0b10
	"Reserved", 			//HDMI_RGB_QUANTIZATION_FULL_RESERVED	0b11
};
const static HI_CHAR *s_aStr_ExtColorimetry[] =
{
	"xvYCC601", 			//	0b000
	"xvYCC709" ,			//	0b001
	"sYCC601",				//	0b010 
	"AdobeYCC601",			//	0b011 
	"AdobeRGB", 			//	0b100 
	"BT2020 YCC", 			//	0b101 
	"BT2020 RGB/YCC", 		//	0b110 
	"Reserved"				//	0b111
};

 

const static HI_CHAR *s_aStr_enOutBitDepth[] =
{
	"08 bit",			//HDMI_VIDEO_BITDEPTH_8 = 0,         
	"10 bit",			//HDMI_VIDEO_BITDEPTH_10 =1,        
	"12 bit",			//HDMI_VIDEO_BITDEPTH_12 =2,        
	"16 bit",			//HDMI_VIDEO_BITDEPTH_16 = 3,  
	"Reserved",			//0b100
	"Reserved",			//0b101
	"Reserved",			//0b110
	"IDO-Defined",		//0b111
};

const static HI_CHAR *s_aStr_SoundIntf[] =
{
    "I2S",				//HDMI_AUDIO_INTERFACE__I2S = 0,
    "SPDIF",			//HDMI_AUDIO_INTERFACE__SPDIF = 1, 
    "HBRA",				//HDMI_AUDIO_INTERFACE__HBRA = 2, 
    "ERROR",			//HDMI_AUDIO_INTERFACE__BUTT
};



const static HI_CHAR *s_aStr_HvSyncPolarity[] =
{
	"HPVP",				//HDMI_HV_SYNC_POL_HPVP = 0,
	"HPVN",				//HDMI_HV_SYNC_POL_HPVN = 1,
	"HNVP",				//HDMI_HV_SYNC_POL_HNVP = 2,
	"HNVN",				//HDMI_HV_SYNC_POL_HNVN = 3
};

const static HI_CHAR *s_aStr_Speaker[] =
{

	"FL_FR",			//HDMI_EDID_AUDIO_SPEAKER_FL_FR   ,
    "LFE",				//HDMI_EDID_AUDIO_SPEAKER_LFE     , 
    "FC",				//HDMI_EDID_AUDIO_SPEAKER_FC      , 
    "RL_RR",			//HDMI_EDID_AUDIO_SPEAKER_RL_RR   ,
    "RC",				//HDMI_EDID_AUDIO_SPEAKER_RC      ,
    "FLC_FRC",			//HDMI_EDID_AUDIO_SPEAKER_FLC_FRC , 
    "RLC_RRC",			//HDMI_EDID_AUDIO_SPEAKER_RLC_RRC ,
    "FLW_FRW",			//HDMI_EDID_AUDIO_SPEAKER_FLW_FRW ,
    "FLH_FRH",			//HDMI_EDID_AUDIO_SPEAKER_FLH_FRH ,
    "TC",				//HDMI_EDID_AUDIO_SPEAKER_TC      ,
    "FCH",				//HDMI_EDID_AUDIO_SPEAKER_FCH     ,
						//HDMI_EDID_AUDIO_SPEAKER_BUTT    ,
};

const static HI_CHAR *s_aStr_3DType[] =
{
	"FPK",				//HDMI_EDID_3D_FRAME_PACKETING				 = 0x00, /**<3d type:Frame Packing*/
	"FIELD_ALTER",		//HDMI_EDID_3D_FIELD_ALTERNATIVE				 = 0x01, /**<3d type:Field alternative*/
	"LINE_ALTER",		//HDMI_EDID_3D_LINE_ALTERNATIVE				 = 0x02, /**<3d type:Line alternative*/
	"SBS_full",			//HDMI_EDID_3D_SIDE_BY_SIDE_FULL				 = 0x03, /**<3d type:Side by side full*/
	"L_DEPTH",			//HDMI_EDID_3D_L_DEPTH						 = 0x04, /**<3d type:L+depth*/
	"LdGGd",			//HDMI_EDID_3D_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH = 0x05, /**<3d type:L+depth+Graphics+Graphics-depth*/
	"TAB",				//HDMI_EDID_3D_TOP_AND_BOTTOM 				 = 0x06, /**<3d type:Top and Bottom*/
	"SBS_half",			//HDMI_EDID_3D_SIDE_BY_SIDE_HALF
};

const static HI_CHAR *s_aStr_EstabTiming[] =
{
	/*VESA Enhanced EDID Standard Release A, Rev.1 Page 16 */
	"V800X600_60",			//HDMI_EDID_ESTABTIMG_VESA_800X600_60 		,0
	"V800X600_56",			//HDMI_EDID_ESTABTIMG_VESA_800X600_56			,
	"V640X480_75",			//HDMI_EDID_ESTABTIMG_VESA_640X480_75			,
	"V640X480_72",			//HDMI_EDID_ESTABTIMG_VESA_640X480_72			,
	"V640X480_67",			//HDMI_EDID_ESTABTIMG_IBM_VGA_640X480_67		,
	"V640X480_60",			//HDMI_EDID_ESTABTIMG_IBM_XGA_640X480_60		,5
	"V720X400_88",			//HDMI_EDID_ESTABTIMG_IBM_XGA2_720X400_88		,
	"V720X400_70",			//HDMI_EDID_ESTABTIMG_IBM_XGA_720X400_70		,
	
	"V1280X1024_75",			//HDMI_EDID_ESTABTIMG_VESA_1280X1024_75		,
	"V1024X768_75",			//HDMI_EDID_ESTABTIMG_VESA_1024X768_75		,
	"V1024X768_70",			//HDMI_EDID_ESTABTIMG_VESA_1024X768_70		,
	"V1024X768_60",			//HDMI_EDID_ESTABTIMG_VESA_1024X768_60		,11
	"V1024X768_87",			//HDMI_EDID_ESTABTIMG_IBM_1024X768_87			,
	"V832X624_75",			//HDMI_EDID_ESTABTIMG_APPLE_MACII_832X624_75	,
	"V800X600_75",			//HDMI_EDID_ESTABTIMG_VESA_800X600_75			,
	"V800X600_72",			//HDMI_EDID_ESTABTIMG_VESA_800X600_72			,

	"V1152X870_75",			//HDMI_EDID_ESTABTIMG_VESA_1152X870_75		,
	
							//HDMI_EDID_ESTABTIMG_BUTT					,

};

static const HDMI_FMT_NAME_PARAM_S s_stFmtParam[]=
{
	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    Fmt************************/
	{						 1, 25175 ,  640 ,480 , 6000 ,	"640X480_60 4:3"		},		// 1. 640 x 480p @ 60 VGA
	{						 2, 27000 ,  720 ,480 , 6000 ,	"480P_60 4:3"			},		// 2,3 720 x 480p
	{						 3, 27000 ,  720 ,480 , 6000 ,	"480P_60 16:9"			},		// 2,3 720 x 480p
	{						 4, 74170 ,  1280,720 , 6000 ,	"720P_60 16:9"			},		// 4   1280 x 720p
	{						 5, 74170 ,  1920,1080, 6000 ,	"1080i_60 16:9" 		},		// 5 1920 x 1080i
	{						 6, 27000 ,  720 ,480 , 6000 ,	"NTSC 4:3" 				},		// 6,7 720 x 480i, pix repl
	{						 7, 27000 ,  720 ,480 , 6000 ,	"NTSC 16:9" 			},		// 6,7 720 x 480i, pix repl

	{						 8, 27000 ,  1440,240 , 6000 ,	"240P_60 4:3" 			},		// 
	{						 9, 27000 ,  1440,240 , 6000 ,	"240P_60 16:9" 			},		// 
	{						10, 54000 ,  720 ,480 , 6000 ,	"2880X480i_60 4:3" 		},		// 
	{						11, 54000 ,  720 ,480 , 6000 ,	"2880X480i_60 16:9" 	},		// 
	{						12, 54000 ,  2880,240 , 6000 ,	"2880X240P_60 4:3" 		},		// 
	{						13, 54000 ,  2880,240 , 6000 ,	"2880X240P_60 16:9" 	},		// 
	{						14, 54000 ,  1440,480 , 6000 ,	"1440X480P_60 4:3" 		},		// 
	{						15, 54000 ,  1440,480 , 6000 ,	"1440X480P_60 16:9" 	},		// 
	
	{						16, 148500,  1920,1080, 6000 ,	"1080P_60 16:9" 		},		// 16 1920 x 1080p
	{						17, 27000 ,  720 ,576 , 5000 ,	"576P_50 4:3"			},		// 17,18 720 x 576p
	{						18, 27000 ,  720 ,576 , 5000 ,	"576P_50 16:9"			},		// 17,18 720 x 576p
	{						19, 74250 ,  1280,720 , 5000 ,	"720P_50 16:9"			},		// 19 1280 x 720p
	{						20, 74250 ,  1920,1080, 5000 ,	"1080i_50 16:9" 		},		// 20 1920 x 1080i
	{						21, 27000 ,  1440,576 , 5000 ,	"PAL 4:3"				},		// 21,22 1440 x 576i
	{						22, 27000 ,  1440,576 , 5000 ,	"PAL 16:9"				},		// 21,22 1440 x 576i
	
	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    Fmt************************/
	{						31, 148500,  1440,576 , 5000 ,	"1080P_50 16:9" 		},		// 31(1) 1920 x 1080p
	{						32, 74170 ,  1920,1080, 2400 ,	"1080P_24 16:9" 		},		// 32(2) 1920 x 1080p
	{						33, 74250 ,  1920,1080, 2500 ,	"1080P_25 16:9" 		},		// 33(3) 1920 x 1080p
	{						34, 74170 ,  1920,1080, 3000 ,	"1080P_30 16:9" 		},		// 34(4) 1920 x 1080p

	{						60, 59400 ,  1280,720 , 2400 ,	"720P_24 16:9"			},		// 60   1280 x 720p
	{						61, 74250 ,  1280,720 , 2500 ,	"720P_25 16:9"			},		// 61   1280 x 720p
	{						62, 74250 ,  1280,720 , 3000 ,	"720P_30 16:9"			},		// 62   1280 x 720p
	/*4K * 2K*/
	{						93,297000 ,  3840,2160, 2400 ,	"3840X2160P24 16:9" 		},		
	{						94,297000 ,  3840,2160, 2500 ,	"3840X2160P25 16:9" 		},		
	{						95,297000 ,  3840,2160, 3000 ,	"3840X2160P30 16:9" 		},		
	{						96,594000 ,  3840,2160, 5000 ,	"3840X2160P50 16:9" 		},		
	{						97,594000 ,  3840,2160, 6000 ,	"3840X2160P60 16:9" 		},		

	{						98,297000 ,  4096,2160, 2400 ,	"4096X2160P24 256:135" 		},		
	{						99,297000 ,  4096,2160, 2500 ,	"4096X2160P25 256:135" 		},		
	{					   100,297000 ,  4096,2160, 3000 ,	"4096X2160P30 256:135" 		},		
	{					   101,594000 ,  4096,2160, 5000 ,	"4096X2160P50 256:135" 		},
	{					   102,594000 ,  4096,2160, 6000 ,	"4096X2160P60 256:135" 		},

	{					   103,297000 ,  3840,2160, 2400 ,	"3840X2160P24 64:27" 			},		
	{					   104,297000 ,  3840,2160, 2500 ,	"3840X2160P25 64:27" 			},		
	{					   105,297000 ,  3840,2160, 3000 ,	"3840X2160P30 64:27" 			},		
	{					   106,594000 ,  3840,2160, 5000 ,	"3840X2160P50 64:27" 			},		
	{					   107,594000 ,  3840,2160, 6000 ,	"3840X2160P60 64:27" 			},
	
#if defined (DVI_SUPPORT)
	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    Fmt************************/
	
	{ HDMI_VIC_PC_BASE+ 	 9, 40000 , 800 ,600 ,	6000 ,	"800X600_60"	},		// 800x600@60.317
	{ HDMI_VIC_PC_BASE+ 	13, 65000 , 1024,768 ,	6000 ,	"1024X768_60"	},		// 1024x768@60
	{ HDMI_VIC_PC_BASE+ 	18, 162000, 1600,1200,	6000 ,	"1600X1200_60"	},		// 1600x1200@60

	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    Fmt************************/
	{ HDMI_VIC_PC_BASE+ 	25, 108000, 1280,1024,	6000 ,	"1280X1024_60"	},		// 1280x1024@60
	{ HDMI_VIC_PC_BASE+ 	28, 85500 , 1360,768 ,	6000 ,	"1366X768_60"	},		// 1360x768@60
	{ HDMI_VIC_PC_BASE+ 	41, 154000, 1920,1200,	6000 ,	"1920X1200_60"	},		// 1920x1200@59.95
#endif
	/*for 2k & 4k ,we use VIRTUAL VIC*/
	{ HDMI_EDID_VIRTUAL_VIC_3840X2160_30, 297000,  3840,2160, 3000 ,	"3840X2160P_30" 		}, 		// 1 2160P@30,3840X2160_30
	{ HDMI_EDID_VIRTUAL_VIC_3840X2160_25, 297000,  3840,2160, 2500 ,	"3840X2160P_25" 		}, 		// 2 2160P@25,3840X2160_25
	{ HDMI_EDID_VIRTUAL_VIC_3840X2160_24, 297000,  3840,2160, 2400 ,	"3840X2160P_24" 		}, 		// 3 2160P@24,3840X2160_24
	{ HDMI_EDID_VIRTUAL_VIC_4096X2160_24, 297000,  4096,2160, 2400 ,	"4096X2160P_24" 		}, 		// 4 4k*2K SMTP 4096*2160_24

};

#if 0
static HI_CHAR * Proc_Etablish2Format(HI_U32 u32EtablishCode)
{
	HI_U32 i = 0;
	HDMI_ESTAB_FORMAT_S *pstEstabTiming = HI_NULL;

	for (i=0;i<ARRAY_SIZE(s_aStr_EstabTiming);i++)
	{
		pstEstabTiming = (HDMI_ESTAB_FORMAT_S *)&s_aStr_EstabTiming[i];
		if (   pstEstabTiming != HI_NULL
			&& pstEstabTiming->u32EtablishCode == u32EtablishCode)
		{
			return pstEstabTiming->pcFormat;
		}
	}

	return HI_NULL;
	
}

static HI_CHAR * Proc_PreTiming2Format(HDMI_EDID_PRE_TIMING_S *pstPreTiming)
{
	HI_U32 i = 0;
	HDMI_FMT_NAME_PARAM_S *pstFmtParam = HI_NULL;

	if (pstPreTiming == HI_NULL)
	{
		return HI_NULL;
	}

	for (i=0;i<ARRAY_SIZE(s_stFmtParam);i++)
	{
		pstFmtParam = (HDMI_FMT_NAME_PARAM_S *)&s_stFmtParam[i];
		if (   (pstFmtParam != HI_NULL)
			&& (pstFmtParam->u32HorActive == pstPreTiming->u32HACT)
			&& (pstFmtParam->u32VerActive == pstPreTiming->u32VACT)
			&& (pstFmtParam->u32PixlFreq >= pstPreTiming->u32PixelClk)
			&& ((pstFmtParam->u32PixlFreq-PROC_PIXL_FREQ_DEVIATION) <= pstPreTiming->u32PixelClk)  )
		{
			return pstFmtParam->pcFormat;
		}
	}

	return HI_NULL;
	
}

#endif
static HI_CHAR * Proc_Vic2Format(HI_U32 u32Vic)
{
	HI_U32 i = 0;
	HDMI_FMT_NAME_PARAM_S *pstFmtParam = HI_NULL;

	for (i=0;i<ARRAY_SIZE(s_stFmtParam);i++)
	{
		pstFmtParam = (HDMI_FMT_NAME_PARAM_S *)&s_stFmtParam[i];
		if (   (pstFmtParam != HI_NULL)
			&& (pstFmtParam->u32Vic == u32Vic)    )
		{
			return pstFmtParam->pcFormat;
		}
	}

	return HI_NULL;
	
}


static HI_CHAR * Proc_StdTiming2Format(HDMI_EDID_STD_TIMING_S *pstStdTiming)
{
	HI_U32 i = 0;
	HDMI_FMT_NAME_PARAM_S *pstFmtParam = HI_NULL;

	if (pstStdTiming == HI_NULL)
	{
		return HI_NULL;
	}

	for (i=0;i<ARRAY_SIZE(s_stFmtParam);i++)
	{
		pstFmtParam = (HDMI_FMT_NAME_PARAM_S *)&s_stFmtParam[i];
		if (   (pstFmtParam != HI_NULL) 
			&& (pstFmtParam->u32HorActive == pstStdTiming->u32HorActive)
			&& (pstFmtParam->u32VerActive == pstStdTiming->u32VerActive)
			&& (pstFmtParam->u32FieldRate >= pstStdTiming->u32RefreshRate)
			&& ((pstFmtParam->u32FieldRate-PROC_FEILD_RATE_DEVIATION) <= pstStdTiming->u32RefreshRate)  )
		{
			return pstFmtParam->pcFormat;
		}
	}

	return HI_NULL;
	
}




static HI_VOID ProcSrc_GlobleSoftStatusPrint(struct seq_file *pFile,HDMI_DEVICE_ID_E enHdmiID)
{
	HDMI_DEVICE_S *pstHdmiDev = HI_NULL;
	HI_U32 i=0;
#if defined(CONFIG_HDMI_STB_SDK)
	HI_BOOL bTmp = 0;
#endif
	pstHdmiDev = GetHdmiDevice(enHdmiID);

	PROC_PRINT(pFile,"%s\n",g_pszVersion);
	PROC_PRINT(pFile,"-------------------------------- [GlobalSoftStatus] -----------------------------\n");

	CHECK_POINTER(pstHdmiDev);
	
	/* line 1 */
	if (pstHdmiDev->enTmdsMode<ARRAY_SIZE(s_aStrTmdsMode))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |","HDMI Mode",s_aStrTmdsMode[pstHdmiDev->enTmdsMode]);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |","HDMI Mode",STR_ERROR);
	}

    PROC_PRINT(pFile,"%-20s:", "Run State");
	if (pstHdmiDev->enRunState != HDMI_RUN_STATE_NONE)
	{
		for (i=0;i<ARRAY_SIZE(s_aStrRunState);i++)
		{
			if (pstHdmiDev->enRunState & (1<<i))
			{
				PROC_PRINT(pFile," %s", s_aStrRunState[i]);
			}
		}
	}
	else
	{
		PROC_PRINT(pFile," %s", "NONE");
	}
	PROC_PRINT(pFile,"\n");

	/* line 2 */
	PROC_PRINT(pFile,"%-20s: %-15s  |", "Thread Is Run",(pstHdmiDev->stThreadInfo.kThreadTimerSate==HDMI_THREAD_STATE_STATR) ? STR_YES : STR_NO);		
	PROC_PRINT(pFile,"%-20s: %-15s  \n", "KerCallBack",(pstHdmiDev->pKCallback==HI_NULL) ? STR_NO  : STR_YES);		
#if 0
	PROC_PRINT(pFile,"%-20s:", "Transition State");
	if (pstHdmiDev->enTransitionState != HDMI_TRANSITION_NONE)
	{
		for (i=0;i<ARRAY_SIZE(s_aSrt_enTransitionState);i++)
		{
			if (pstHdmiDev->enRunState & (1<<i))
			{
				PROC_PRINT(pFile," %s", s_aSrt_enTransitionState[i]);
			}
		}
	}
	else
	{
		PROC_PRINT(pFile," %s", s_aSrt_enTransitionState[HDMI_TRANSITION_NONE]);
	}
#endif

	/* line 3 */
	PROC_PRINT(pFile,"%-20s: %-15u  |",  "Kernel Open Cnt",pstHdmiDev->u32KernelCnt);
	PROC_PRINT(pFile,"%-20s: %-15u  \n", "User Open Cnt",pstHdmiDev->u32UserCnt);

#if defined(CONFIG_HDMI_STB_SDK)
	
	/* line 4*/
	PROC_PRINT(pFile,"%-20s: %-15s  |",  "Hdcp Is On",pstHdmiDev->stHdcpInfo.bHdcpIsOn ? STR_YES : STR_NO);
	if (pstHdmiDev->stHdcpInfo.enHdcpMode < ARRAY_SIZE(s_aStr_enHdcpMode))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  \n", "Hdcp Mode",s_aStr_enHdcpMode[pstHdmiDev->stHdcpInfo.enHdcpMode]);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  \n", "Hdcp Mode",STR_ERROR);
	}
	/* line 5*/
	PROC_PRINT(pFile,"%-20s: %-15s  |",  "Hdcp1 Prep",pstHdmiDev->stHdcpInfo.bHdcp1Prep ? STR_YES : STR_NO);
	//PROC_PRINT(pFile,"%-20s: %-15s  \n", "bHdcp2Prep",pstHdmiDev->stHdcpInfo.bHdcp2Prep ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"\n");
	/* line 6*/
	PROC_PRINT(pFile,"%-20s: %-15s  |",  "Cec Is On",pstHdmiDev->stCecInfo.bCecIsOn ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s:",  "Cec Network");
	for (i = 0,bTmp = HI_FALSE; i < ARRAY_SIZE(s_aStr_u8Network); i++)
	{
		if (pstHdmiDev->stCecInfo.u8Network[i])
		{
			bTmp = HI_TRUE;
			PROC_PRINT(pFile," %s", s_aStr_u8Network[i]);
		}
	}
	if (bTmp==HI_FALSE)
	{
		PROC_PRINT(pFile," %s", "NONE");
	}
	PROC_PRINT(pFile,"\n");

	/* line 7*/
	PROC_PRINT(pFile,"%-20s: %-15x  |",  "Logical Addr",pstHdmiDev->stCecInfo.u8LogicalAddr);
	PROC_PRINT(pFile,"%-20s: %02x.%02x.%02x.%02x  \n",  "Physical Addr",
							pstHdmiDev->stCecInfo.u8PhysicalAddr[0],
							pstHdmiDev->stCecInfo.u8PhysicalAddr[1],
							pstHdmiDev->stCecInfo.u8PhysicalAddr[2],
							pstHdmiDev->stCecInfo.u8PhysicalAddr[3]);
	
#endif /* #if defined(CONFIG_HDMI_STB_SDK) */	
	
	
}


static HI_VOID ProcSrc_HardWareStatusPrint(struct seq_file *pFile,HDMI_DEVICE_ID_E enHdmiID)
{
	HI_U32 i=0;
//	HI_U32 u32Tmp = 0;
	HI_CHAR *pTmpFmt = HI_NULL;
	HI_U8 *paData = HI_NULL;
	HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(enHdmiID);
	HDMI_HAL_S  *pstHdmiHal	= HI_NULL;
	HDMI_HARDWARE_STATUS_S	stHwStatus;
	HDMI_AVIIF_BYTE1_U	unAviByte1 ;
	HDMI_AVIIF_BYTE2_U	unAviByte2 ;
	HDMI_AVIIF_BYTE3_U	unAviByte3 ;
	HDMI_AVIIF_BYTE5_U	unAviByte5 ;

	HDMI_AUDIO_IF_BYTE1_U unAudioByte1 ;
	HDMI_AUDIO_IF_BYTE2_U unAudioByte2 ;
	HDMI_AUDIO_IF_BYTE5_U unAudioByte5 ;
	
	
	PROC_PRINT(pFile,"---------------------------------- [HardWare Status] ----------------------------\n");

	CHECK_POINTER(pstHdmiDev);
	pstHdmiHal = pstHdmiDev->pstHdmiHal;
	CHECK_POINTER(pstHdmiHal);

	pstHdmiHal->HAL_HDMI_HardwareStatusGet(pstHdmiHal,&stHwStatus);
	
	/* line 1 */
	if (stHwStatus.stCommonStatus.enTmdsMode < ARRAY_SIZE(s_aStrTmdsMode))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |", 	"HDMI Mode",s_aStrTmdsMode[stHwStatus.stCommonStatus.enTmdsMode]);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"HDMI Mode",STR_ERROR);
	}
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"HotPlug",stHwStatus.stCommonStatus.bHotPlug ? STR_YES : STR_NO);
	
	/* line 2 */	
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Phy Output Enable",stHwStatus.stPhyStatus.bPhyOe ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n", 	"Rsen",stHwStatus.stCommonStatus.bRsen ? STR_YES : STR_NO);

	/* line 4 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Phy Power On",stHwStatus.stPhyStatus.bPhyPowerOn ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"AvMute Enable",stHwStatus.stCommonStatus.bAvMute ? STR_YES : STR_NO);

	/* Hardware Video/Audio Path */
	unAviByte1.u8 = stHwStatus.stInfoFrameStatus.u8AVI[4];
	unAviByte2.u8 = stHwStatus.stInfoFrameStatus.u8AVI[5];
	unAviByte3.u8 = stHwStatus.stInfoFrameStatus.u8AVI[6];
	unAviByte5.u8 = stHwStatus.stInfoFrameStatus.u8AVI[8];

	unAudioByte1.u8 = stHwStatus.stInfoFrameStatus.u8AUDIO[4];
	unAudioByte2.u8 = stHwStatus.stInfoFrameStatus.u8AUDIO[5];
	unAudioByte5.u8 = stHwStatus.stInfoFrameStatus.u8AUDIO[8];

	/********************************** VIDEO ****************************************************/
	PROC_PRINT(pFile,"------------HW Video Path--------------|--------------- AVI IF Parse-------------\n");
	
#if 0
	if (stHwStatus.stVideoStatus.enInColorSpace < ARRAY_SIZE(s_aStr_ColorSpace))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"enInColorSpace",s_aStr_ColorSpace[stHwStatus.stVideoStatus.enInColorSpace]);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"enInColorSpace",STR_ERROR);
	}
#endif	
	/* line 1  */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Video Mute Enable",stHwStatus.stVideoStatus.bVideoMute ? STR_YES : STR_NO);
	pTmpFmt = Proc_Vic2Format(stHwStatus.stInfoFrameStatus.u8AVI[7]);
	PROC_PRINT(pFile,"%-20s: %s(VIC=%2d)\n",	"Current Format", pTmpFmt==HI_NULL ? "" : pTmpFmt,stHwStatus.stInfoFrameStatus.u8AVI[7]);	

	/* line 2 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"DeepColor Open",stHwStatus.stPhyStatus.enDeepColor ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"IT Content Valid",unAviByte3.bits.bzItContentValid ? STR_YES : STR_NO);
	
	
	/* line 3 */
	if (stHwStatus.stVideoStatus.enOutBitDepth < ARRAY_SIZE(s_aStr_ColorSpace))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"Out BitDepth",s_aStr_enOutBitDepth[stHwStatus.stVideoStatus.enOutBitDepth] );
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"Out BitDepth",STR_ERROR );
	}
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Content Type",s_aStr_ITContentType[unAviByte5.bits.bzITContentType]);
	

	/* line 4 */
	if (pstHdmiDev->stAttr.stVOAttr.enInColorSpace < ARRAY_SIZE(s_aStr_ColorSpace))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"InColorSpace(sw)",s_aStr_ColorSpace[pstHdmiDev->stAttr.stVOAttr.enInColorSpace] );
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"InColorSpace(sw)",STR_ERROR);	
	}
	PROC_PRINT(pFile,"%-20s: %-15s \n",	"Bar Info",s_aStr_BarDataPresent[unAviByte1.bits.bzBarDataPresent]);

	/* line 5 */
	if (stHwStatus.stVideoStatus.enOutColorSpace < ARRAY_SIZE(s_aStr_ColorSpace))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"OutColorSpace",s_aStr_ColorSpace[stHwStatus.stVideoStatus.enOutColorSpace]);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"OutColorSpace",STR_ERROR);
	}
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"ColorSpace",s_aStr_ColorSpace[unAviByte1.bits.bzColorSpace]);

	/* line 6 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Dither",stHwStatus.stVideoStatus.bDither ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Scan Info",s_aStrScanInfo[unAviByte1.bits.bzScanInfo]);
	
	

	/* line 7 */
	if (stHwStatus.stVideoStatus.enOutBitDepth < ARRAY_SIZE(s_aStr_HvSyncPolarity))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"HV Sync Polarity",s_aStr_HvSyncPolarity[stHwStatus.stVideoStatus.enHvSyncPol]);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"HV Sync Polarity",STR_ERROR);
	}
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Colorimetry",s_aStr_Colorimetry[unAviByte2.bits.bzColorimetry]);

	/* line 8 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"InYCbCr420Demux",stHwStatus.stVideoStatus.bIn420Ydemux ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s \n",	"Active Aspect Ratio",s_aStr_ActAspectRatio[unAviByte2.bits.bzActAspRatio]);

	/* line 9 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"OutYCbCr420Demux",stHwStatus.stVideoStatus.bOut420Ydemux ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Picture Aspect Ratio",s_aStr_PictureAspectRatio[unAviByte2.bits.bzPictureAspectRatio]);
	
	
	/* line 10 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"RGB_YCbCr444",stHwStatus.stVideoStatus.bRGB2YCbCr ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Ext Colorimetry",s_aStr_ExtColorimetry[unAviByte3.bits.bzExtColorimetry]);

	/* line 11 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"YCbCr444_422",stHwStatus.stVideoStatus.bYCbCr444_422? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"RGB Quantization",s_aStr_RgbQuantization[unAviByte3.bits.bzRgbQuantization]);

	/* line 12 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"YCbCr422_420",stHwStatus.stVideoStatus.bYCbCr422_420? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"YCC Quantization",s_aStr_YCCQuantizationRange[unAviByte5.bits.bzYCCQuantizationRange]);
	

	/* line 11 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"YCbCr420_422",stHwStatus.stVideoStatus.bYCbCr420_422? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s \n",	"Picture Scaling",s_aStr_PictureScaling[unAviByte3.bits.bzPictureScaling]);

	/* line 12 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"YCbCr422_444",stHwStatus.stVideoStatus.bYCbCr422_444? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Active Info Present",unAviByte1.bits.bzActFmtInfoPresent ? STR_YES : STR_NO);

	/* line 13 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"YCbCr444_RGB",stHwStatus.stVideoStatus.bYCbCr2RGB? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Pixel Repetition",s_aStrPixelRepetition[unAviByte5.bits.bzPixelRepetition]);

	/********************************** AUDIO ****************************************************/

	PROC_PRINT(pFile,"------------HW Audio Path--------------|-------------- Audio IF Parse-------------\n");
	/* line 1 */
	if (stHwStatus.stVideoStatus.enOutBitDepth < ARRAY_SIZE(s_aStr_SoundIntf))
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"Audio Intf Type",s_aStr_SoundIntf[stHwStatus.stAudioStatus.enSoundIntf]);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"Audio Intf Type",STR_ERROR);	
	}
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Aduio Coding Type",s_aStrAudioCodingType[unAudioByte1.bits.bzAduioCodingType]);

	/* line 2 */
	if (  (stHwStatus.stAudioStatus.enLayout <= HDMI_AUDIO_FORMAT_8CH)
		&&(stHwStatus.stAudioStatus.enLayout >= HDMI_AUDIO_FORMAT_2CH)	)
	{
		PROC_PRINT(pFile,"%-20s: %-15d  |",	"Audio Channel Cnt",stHwStatus.stAudioStatus.enLayout);
	}
	else
	{
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"Audio Channel Cnt", STR_ERROR);
	}
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Audio Channel Cnt",s_aStrAudioChannelCnt[unAudioByte1.bits.bzAudioChanelCnt]);
	
	/* line 3 */
	PROC_PRINT(pFile,"%-20s: %-15d  |",	"Sampling Freq(Hz)",stHwStatus.stAudioStatus.enSampleFs);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Audio Sampling Freq",s_aStrAudioSampingFreq[unAudioByte2.bits.bzAudioSampingFreq]);

	/* line 4 */
	PROC_PRINT(pFile,"%-20s: %-15d  |", 	"Sampling Depth",stHwStatus.stAudioStatus.enSampleDepth);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Audio Sampling Size",s_aStrAudioSampingSize[unAudioByte2.bits.bzAudioSampingSize]);
	
	/* line 5 */		
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"DownSample",stHwStatus.stAudioStatus.bDownSample ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s \n",	"DownmixInhibit",unAudioByte5.bits.bzDownMixInhibit ? STR_YES : STR_NO);

	/* line 6 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"N value","");
	PROC_PRINT(pFile,"%-20s: 0x%02x (%3d)\n",	"Channel/SpeakerAlloc",stHwStatus.stInfoFrameStatus.u8AUDIO[7],stHwStatus.stInfoFrameStatus.u8AUDIO[7]);
	
	/* line 7 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"CTS value","");
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"LFE PlayBack",s_aStr_LFE_PlayBack[unAudioByte5.bits.bzLFE_PlayBack]);

	/* line 8 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Audio Mute",stHwStatus.stAudioStatus.bAudioMute ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-5d dB\n",	"Level Shift",unAudioByte5.bits.bzLevelShiftValue);

	/* line 9 */	
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Audio Enable",stHwStatus.stAudioStatus.bAudioEnable ? STR_YES : STR_NO);

	/* Info Frame Raw Data */
	PROC_PRINT(pFile,"\n------------------------------ [Info Frame RawData] ----------------------------\n");

	/* line 1 */
	PROC_PRINT(pFile,"[HEX]%s%10s:",	"AVI  ",stHwStatus.stInfoFrameStatus.bAVIEnable ? STR_ENABLE : STR_DISABLE);
	paData = stHwStatus.stInfoFrameStatus.u8AVI;
	for (i = 0;i < paData[2]+4 && i < ARRAY_SIZE(stHwStatus.stInfoFrameStatus.u8AVI) ; i++)
	{
		if ( i == 20 )
		{
			PROC_PRINT(pFile,"\n%21s","");
		}
		if ( (i%4) == 0 )
		{
			if (i==0)
			{
				PROC_PRINT(pFile,"[");	
			}
			else
			{
				PROC_PRINT(pFile,"  ");	
			}
		}
		
		PROC_PRINT(pFile," %02x",stHwStatus.stInfoFrameStatus.u8AVI[i]);
		if (i==3)
		{
			PROC_PRINT(pFile,"]");	
		}
	}
	PROC_PRINT(pFile,"\n"); 

	/* line 2 */
	PROC_PRINT(pFile,"[HEX]%s%10s:",	"AUDIO",stHwStatus.stInfoFrameStatus.bAUDIOEnable ? STR_ENABLE : STR_DISABLE);
	paData = stHwStatus.stInfoFrameStatus.u8AUDIO;
	for (i = 0;i < paData[2]+4 && i < ARRAY_SIZE(stHwStatus.stInfoFrameStatus.u8AUDIO) ; i++)
	{
		if ( i == 20 )
		{
			PROC_PRINT(pFile,"\n%21s","");
		}
		if ( (i%4) == 0 )
		{
			if (i==0)
			{
				PROC_PRINT(pFile,"[");	
			}
			else
			{
				PROC_PRINT(pFile,"  ");	
			}
		}
		
		PROC_PRINT(pFile," %02x",stHwStatus.stInfoFrameStatus.u8AUDIO[i]);
		if (i==3)
		{
			PROC_PRINT(pFile,"]");	
		}
	}
	PROC_PRINT(pFile,"\n");

	/* line 3 */
	PROC_PRINT(pFile,"[HEX]%s%10s:",	"VS   ",stHwStatus.stInfoFrameStatus.bVSIFEnable ? STR_ENABLE : STR_DISABLE);
	paData = stHwStatus.stInfoFrameStatus.u8VSIF;
	for (i = 0;i < paData[2]+4 && i < ARRAY_SIZE(stHwStatus.stInfoFrameStatus.u8VSIF) ; i++)
	{
		if ( i == 20 )
		{
			PROC_PRINT(pFile,"\n%21s","");
		}
		if ( (i%4) == 0 )
		{
			if (i==0)
			{
				PROC_PRINT(pFile,"[");	
			}
			else
			{
				PROC_PRINT(pFile,"  ");	
			}
		}
		
		PROC_PRINT(pFile," %02x",stHwStatus.stInfoFrameStatus.u8VSIF[i]);
		if (i==3)
		{
			PROC_PRINT(pFile,"]");	
		}
	}
	PROC_PRINT(pFile,"\n");
#if 0
	/* line 4 */
	PROC_PRINT(pFile,"[HEX]%s%10s:",	"MPEG ",stHwStatus.stInfoFrameStatus.bMPEGEnable? STR_ENABLE : STR_DISABLE);
	paData = stHwStatus.stInfoFrameStatus.u8MPEG;
	for (i = 0;i < paData[2]+4 &&  i < ARRAY_SIZE(stHwStatus.stInfoFrameStatus.u8MPEG) ; i++)
	{
		if ( i == 20 )
		{
			PROC_PRINT(pFile,"\n%21s","");
		}
		if ( (i%4) == 0 )
		{
			if (i==0)
			{
				PROC_PRINT(pFile,"[");	
			}
			else
			{
				PROC_PRINT(pFile,"  ");	
			}
		}
		
		PROC_PRINT(pFile," %02x",stHwStatus.stInfoFrameStatus.u8MPEG[i]);
		if (i==3)
		{
			PROC_PRINT(pFile,"]");	
		}
	}
	PROC_PRINT(pFile,"\n");

	/* line 5 */
	PROC_PRINT(pFile,"[HEX]%s%10s:",	"SPD  ",stHwStatus.stInfoFrameStatus.bSPDEnable ? STR_ENABLE : STR_DISABLE);
	paData = stHwStatus.stInfoFrameStatus.u8SPD;
	for (i = 0;i < paData[2]+4 && i < ARRAY_SIZE(stHwStatus.stInfoFrameStatus.u8SPD) ; i++)
	{
		if ( i == 20 )
		{
			PROC_PRINT(pFile,"\n%21s","");
		}
		if ( (i%4) == 0 )
		{
			if (i==0)
			{
				PROC_PRINT(pFile,"[");	
			}
			else
			{
				PROC_PRINT(pFile,"  ");	
			}
		}
		PROC_PRINT(pFile," %02x",stHwStatus.stInfoFrameStatus.u8SPD[i]);
		if (i==3)
		{
			PROC_PRINT(pFile,"]");	
		}
	}
	PROC_PRINT(pFile,"\n");
#endif		

}


static HI_VOID ProcSrc_EventInfoPrint(struct seq_file *pFile,HDMI_DEVICE_ID_E enHdmiID)
{
	HI_U32 					i=0,j=0;
	HI_U32 					u32TotalPool = 0;
 	HI_S32 					s32Ret = HI_SUCCESS;
	HDMI_DEVICE_S 			*pstHdmiDev = GetHdmiDevice(enHdmiID);
	HDMI_EVENT_INFO_S 		*pstEvtInfo = HI_NULL;
	HDMI_EVENT_RUN_CNT_S 	stRunCnt;
	HDMI_EVENT_RUN_CTRL_S 	stCtrl;
	
	CHECK_POINTER(pstHdmiDev);
	pstEvtInfo = &pstHdmiDev->stEventInfo;
	CHECK_POINTER(pstEvtInfo);

	s32Ret = DRV_HDMI_EventPoolTotalGet(pstEvtInfo,&u32TotalPool);
	CHECK_RET_SUCCESS(s32Ret);

	for (i = 0; i < u32TotalPool; i++)
	{
		if(DRV_HDMI_EventPoolStatusGet(pstEvtInfo,i+1,&stCtrl,&stRunCnt)==HI_SUCCESS)
		{
			PROC_PRINT(pFile,"------------------------ [TaskID=%u Event Pool[%u] Status] ------------------------\n",stCtrl.u32PoolID,i);
			/* line1 */
			PROC_PRINT(pFile,"CNT|%-8s|%-8s|%-8s|%-8s|%-8s|%-8s|%-8s|%-8s|%-8s\n",
												"ErrTotal",
												"HPD",
												"UnHPD",
												"EdidFail",
												"HdcpFail",
												"HdcpSucc",
												"RsenCon",
												"RsenDis",
												"HdcpUsr");
			/* line2 */
			PROC_PRINT(pFile,"WR:|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u\n",
												stRunCnt.u32ErrWdCnt,
												stRunCnt.u32HPDWrCnt,	
												stRunCnt.u32UnHPDWrCnt,	
												stRunCnt.u32EdidFailWrCnt,
												stRunCnt.u32HdcpFailWrCnt,
												stRunCnt.u32HdcpSuccWrCnt,
												stRunCnt.u32RsenConWrCnt,
												stRunCnt.u32RsenDisWrCnt,
												stRunCnt.u32HdcpUserWrCnt);
			/* line3 */
			PROC_PRINT(pFile,"RD:|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u|%-8u\n",
												stRunCnt.u32ErrRdCnt,
												stRunCnt.u32HPDRdCnt,		
												stRunCnt.u32UnHPDRdCnt,
												stRunCnt.u32EdidFailRdCnt,
												stRunCnt.u32HdcpFailRdCnt,
												stRunCnt.u32HdcpSuccRdCnt,
												stRunCnt.u32RsenConRdCnt,
												stRunCnt.u32RsenDisRdCnt,
												stRunCnt.u32HdcpUserRdCnt);
			/* line4 */
			PROC_PRINT(pFile,"Memory[WkFlg=%1d |RdAble=%2d| RdPtr=%-2d| WrPtr=%-2d]:",
												stCtrl.bWakeUpFlag,
												stCtrl.u32RdAbleCnt,
												stCtrl.u32ReadPtr,
												stCtrl.u32WritePtr);
			
			for (j = 0; j < ARRAY_SIZE(stCtrl.enEventPool); j++)
			{
				if ( j == 5 )
				{
					PROC_PRINT(pFile,"  ");
				}
				PROC_PRINT(pFile," %02x",stCtrl.enEventPool[j] );
			}
			
			PROC_PRINT(pFile,"\n");
		}
	}

	return ;
}

static HI_VOID ProcSrc_EdidInfoPrint(struct seq_file *pFile,HDMI_DEVICE_ID_E enHdmiID)
{
	HI_S32 					i = 0,j=0;
 	HI_U32			 		s32Ret = HI_SUCCESS;
	HDMI_EDID_DATA_E 		enGetCapRet = HI_SUCCESS;
	HI_S32					s32RawLen = 0;
	HDMI_EDID_STATUS_S		stStatus;					
	HI_U8					*pu8EdidRaw = HI_NULL;
	HI_CHAR					*pTmpFmt = HI_NULL;
	HI_CHAR					aTmpArray[25];
	HDMI_SINK_CAPABILITY_S	*pstCap = HI_NULL;
	HDMI_EDID_INFO_S		*pstEdidInfo = HI_NULL;
	HDMI_DEVICE_S 			*pstHdmiDev = GetHdmiDevice(enHdmiID);

	CHECK_POINTER(pstHdmiDev);
	pstEdidInfo = &pstHdmiDev->stEdidInfo;
	CHECK_POINTER(pstEdidInfo);

	pu8EdidRaw = (HI_U8 *)HDMI_KMALLOC(HDMI_EDID_TOTAL_SIZE);
	CHECK_POINTER(pu8EdidRaw);

	s32Ret = DRV_HDMI_EdidStatusGet(pstEdidInfo,&stStatus);
	if (s32Ret != HI_SUCCESS)
	{
		HDMI_ERR("Get edid stat!\n");
		HDMI_KFREE(pu8EdidRaw);
		return ;
	}

	memset(aTmpArray,'\0',sizeof(aTmpArray));
	PROC_PRINT(pFile,"--------------------------------- Capability SW Info --------------------------------------\n");

	/* line 1 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Capability From Sink",stStatus.bCapSink ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15d  \n",	"Raw UpdateErrCnt",stStatus.u32RawUpdateErrCnt);	
	/* line 2 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Capability Is Valid",stStatus.bCapValid ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15d  \n",	"Parse Error",stStatus.enParseErr);
	
	/* line 3 */
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Raw Is Valid",stStatus.bRawValid ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: 0x%08x  \n","Parse Warn",stStatus.u32ParseWarn);
	/* line 4 */
	PROC_PRINT(pFile,"%-20s: %-15d  |",	"Raw GetErrCnt",stStatus.u32RawGetErrCnt);
	PROC_PRINT(pFile,"%-20s: %-15d   \n",  "Raw Length",stStatus.u32RawLen);
	
	/* line 5 */
	//PROC_PRINT(pFile,"%-20s: %-15d  |",  "Capability GetErrCnt",stStatus.u32CapGetErrCnt);
	//PROC_PRINT(pFile,"%-20s: %-15s  \n",	"CapValid",stStatus.bCapValid ? STR_YES : STR_NO);

#if 0
	s32RawLen = HDMI_EDID_TOTAL_SIZE;
	s32RawLen = DRV_HDMI_EdidRawForceGet(pstEdidInfo,pu8EdidRaw,s32RawLen);
	if (s32RawLen < 0)
	{
		HDMI_ERR("RawLen=%d\n",s32RawLen);
		PROC_PRINT(pFile,"RawLen=%d\n",s32RawLen);
		HDMI_KFREE(pu8EdidRaw);
		return ;
	}
#endif
	
	enGetCapRet = DRV_HDMI_EdidCapabilityGet(pstEdidInfo, &pstCap);
	if (enGetCapRet == HDMI_EDID_DATA_INVALID)
	{
		HDMI_ERR("Current capability INVALID,force to get Raw!!!\n");
		PROC_PRINT(pFile,"Current capability INVALID,force to get Raw!!!\n");
		s32RawLen = DRV_HDMI_EdidRawForceGet(pstEdidInfo,pu8EdidRaw,s32RawLen);
		if (s32RawLen < 0)
		{
			HDMI_ERR("DRV_HDMI_EdidRawForceGet fail,ReadRawLen=%d,Please make sure HDMI cable connected & start!\n",s32RawLen);
			PROC_PRINT(pFile,"DRV_HDMI_EdidRawForceGet fail,ReadRawLen=%d,Please make sure HDMI cable connected & start!\n",s32RawLen);
			HDMI_KFREE(pu8EdidRaw);
			return ;
		}
	}
    
	s32RawLen = HDMI_EDID_TOTAL_SIZE;
	s32RawLen = DRV_HDMI_EdidRawGet(pstEdidInfo,pu8EdidRaw,s32RawLen);

	/* Raw data */
	PROC_PRINT(pFile,"------------------------------------- EDID Raw Data ---------------------------------------");
	for (i = 0; i<s32RawLen && i<HDMI_EDID_TOTAL_SIZE ;i++)
	{
		if ((i % 4) == 0)
		{
			PROC_PRINT(pFile,"  ");
		}
		if ((i % 16) == 0)
		{
			PROC_PRINT(pFile,"\n/*%02xH:*/ ",i);
		}
		PROC_PRINT(pFile,"0x%02x,",pu8EdidRaw[i]);
	}
	PROC_PRINT(pFile,"\n");

    if (pstCap == HI_NULL)
    {
		HDMI_ERR("pstCap is NULL!!!\n");
        return ;
    }
        
	/* Detail Timing */
	if (pstCap->u32PerferTimingNum > 0)
	{
		
		/* line1 */
		PROC_PRINT(pFile,"--------------------------------- Support Detail Timing -----------------------------------\n");
		/* line2 */
		PROC_PRINT(pFile,"%-5s:%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s|%-4s\n",
				"[NO.]","HACT","VACT","P/I","PClk","AspW","AspH","HFB","HPW","HBB","VFB","VPW","VBB","ImgW","ImgH","IHS","IVS","IDV");
		for (i=0;i<pstCap->u32PerferTimingNum;i++)
		{
			
			
			PROC_PRINT(pFile,"[%3u]:%-4u|%-4u|%-4c|%-3uM|%-4u|%-4u|%-4u|%-4u|%-4u|%-4u|%-4u|%-4u|%-4u|%-4u|%-4s|%-4s|%-4s\n",
					i,
					pstCap->stPerferTiming[i].u32HACT,
					pstCap->stPerferTiming[i].u32VACT,
					pstCap->stPerferTiming[i].bInterlace ? 'I' : 'P',
					pstCap->stPerferTiming[i].u32PixelClk / 1000,
					pstCap->stPerferTiming[i].u32AspectRatioW,
					pstCap->stPerferTiming[i].u32AspectRatioH,
					pstCap->stPerferTiming[i].u32HFB,
					pstCap->stPerferTiming[i].u32HPW,
					pstCap->stPerferTiming[i].u32HBB,
					pstCap->stPerferTiming[i].u32VFB,
					pstCap->stPerferTiming[i].u32VPW,
					pstCap->stPerferTiming[i].u32VBB,
					pstCap->stPerferTiming[i].u32ImageWidth,
					pstCap->stPerferTiming[i].u32ImageHeight,
					pstCap->stPerferTiming[i].bIHS ? STR_YES : STR_NO,
					pstCap->stPerferTiming[i].bIVS ? STR_YES : STR_NO,
					pstCap->stPerferTiming[i].bIDV ? STR_YES : STR_NO );
			
			
		}

	}
	

	/* format */
	PROC_PRINT(pFile,"-------------------------------- Support Video Format -------------------------------------\n");

	for (i=0;i<pstCap->u32SupportVICNum;i++)
	{


		pTmpFmt = Proc_Vic2Format(pstCap->au32SupportFormat[i]);


		if (pTmpFmt != HI_NULL)
		{
			PROC_PRINT(pFile,"|%-20s ",pTmpFmt);
		}
		else
		{
			PROC_PRINT(pFile,"|(VIC)%-15u ",pstCap->au32SupportFormat[i]);
		}

		j++;
		if ((j % 4)==0)
		{
			PROC_PRINT(pFile,"\n");
		}
			
	}

	for (i=0;i<ARRAY_SIZE(pstCap->stStdTiming);i++)
	{
		pTmpFmt = Proc_StdTiming2Format(&pstCap->stStdTiming[i]);
		if (pTmpFmt != HI_NULL)
		{
			PROC_PRINT(pFile,"|%-20s ",pTmpFmt);
			j++;
			if ((j % 4)==0)
			{
				PROC_PRINT(pFile,"\n");
			}
		}
		else if (pstCap->stStdTiming[i].u32HorActive != 0
			&& pstCap->stStdTiming[i].u32VerActive != 0)
		{
			int n = 0;
			memset(aTmpArray,'\0',sizeof(aTmpArray));
			n = snprintf(aTmpArray,21,"%dX%d",pstCap->stStdTiming[i].u32HorActive,				
											pstCap->stStdTiming[i].u32VerActive);
			if ((pstCap->stStdTiming[i].u32RefreshRate / 100) != 0)
			{
				snprintf(aTmpArray+n,21-n,"_%d",(pstCap->stStdTiming[i].u32RefreshRate / 100));
			}
			PROC_PRINT(pFile,"|%-21s",aTmpArray);
			j++;
			if ((j % 4)==0)
			{
				PROC_PRINT(pFile,"\n");
			}
		}
		
	}
	for (i=0;i<pstCap->u32EstabNum && pstCap->au32EstabTiming[i]<ARRAY_SIZE(s_aStr_EstabTiming);i++)
	{
		PROC_PRINT(pFile,"|%-20s ",s_aStr_EstabTiming[i]);
		j++;
		if ((j % 4)==0)
		{
			PROC_PRINT(pFile,"\n");
		}
	}
	PROC_PRINT(pFile,"\n");

	PROC_PRINT(pFile,"------------------------------------ Support 3D -------------------------------------------\n");
	
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Support 3D",pstCap->st3DInfo.bSupport3D ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"3D Osd Disparity",pstCap->bSupport3dOsdDisparity ? STR_YES : STR_NO);

	PROC_PRINT(pFile,"%-20s: %-15s  |",	"3D Dual View",pstCap->bSupport3dDualView ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"3D Independent View",pstCap->bSupport3dIndependentView ? STR_YES : STR_NO);

	PROC_PRINT(pFile,"%-20s:",	"Support 3D Type");
	for (i = 0 ;i < ARRAY_SIZE(pstCap->st3DInfo.bSupport3DType); i++)
	{
		if (pstCap->st3DInfo.bSupport3DType[i] && i<ARRAY_SIZE(s_aStr_3DType))
		PROC_PRINT(pFile," %s", s_aStr_3DType[i]);	
	}
	PROC_PRINT(pFile,"\n");

	
	/* video info */
	PROC_PRINT(pFile,"--------------------------------- Support video -------------------------------------------\n");
	pTmpFmt = Proc_Vic2Format(pstCap->u32NativeFormat);
	PROC_PRINT(pFile,"%-20s: %-s(VIC %2d)",	"Native Format", pTmpFmt==HI_NULL ? "" : pTmpFmt,pstCap->u32NativeFormat);	
	PROC_PRINT(pFile,"\n");

	/* ColorSpace */
	PROC_PRINT(pFile,"%-20s:",	"ColorSpace");
	if (pstCap->stColorSpace.bRGB444)
	{
		PROC_PRINT(pFile," %-s","RGB444");	
	}
	if (pstCap->stColorSpace.bYCbCr444)
	{
		PROC_PRINT(pFile," %-s","YCbCr444");	
	}
	if (pstCap->stColorSpace.bYCbCr422)
	{
		PROC_PRINT(pFile," %-s","YCbCr422");	
	}
	if (pstCap->u32OnlySupportY420VICNum || pstCap->u32SupportY420VICNum)
	{
		PROC_PRINT(pFile," %-s","YCbCr420");	
	}
	PROC_PRINT(pFile,"\n");

	/* Colorimetry  */
	PROC_PRINT(pFile,"%-20s:",	"Colorimetry");
	if (pstCap->stColorMetry.bxvYCC601)
	{
		PROC_PRINT(pFile," %-s","xvYCC601");	
	}
	if (pstCap->stColorMetry.bxvYCC709)
	{
		PROC_PRINT(pFile," %-s","xvYCC709");	
	}
	if (pstCap->stColorMetry.bsYCC601)
	{
		PROC_PRINT(pFile," %-s","sYCC601");	
	}
	if (pstCap->stColorMetry.bAdobleYCC601)
	{
		PROC_PRINT(pFile," %-s","AdobleYCC601");	
	}
	if (pstCap->stColorMetry.bAdobleRGB)
	{
		PROC_PRINT(pFile," %-s","AdobleRGB");	
	}
	if (pstCap->stColorMetry.bBT2020cYCC)
	{
		PROC_PRINT(pFile," %-s","BT2020cYCC");	
	}
	if (pstCap->stColorMetry.bBT2020YCC)
	{
		PROC_PRINT(pFile," %-s","BT2020YCC");	
	}
	if (pstCap->stColorMetry.bBT2020RGB)
	{
		PROC_PRINT(pFile," %-s","BT2020RGB");	
	}
	PROC_PRINT(pFile,"\n");
	
	/* DeepColor */
	PROC_PRINT(pFile,"%-20s:",	"Deep Color");
	if (pstCap->stDeepColor.bDeepColor30Bit)
	{
		PROC_PRINT(pFile," %-s","RGB_30Bit"); 
	}	
	if (pstCap->stDeepColor.bDeepColor36Bit)
	{
		PROC_PRINT(pFile," %-s","RGB_36Bit");	
	}
	if (pstCap->stDeepColor.bDeepColor48Bit)
	{
		PROC_PRINT(pFile," %-s","RGB_48Bit");	
	}
	if (pstCap->stDeepColor.bDeepColorY444)
	{
		PROC_PRINT(pFile," %-s","YCbCr444_SameRGB");	
	}	
	PROC_PRINT(pFile,"\n");
	
	/* DeepColorY420 */
	PROC_PRINT(pFile,"%-20s:",	"YCbCr420 DeepColor");
	if (pstCap->stDeepColorY420.bDeepColor30Bit)
	{
		PROC_PRINT(pFile," %-s","30Bit"); 
	}	
	if (pstCap->stDeepColorY420.bDeepColor36Bit)
	{
		PROC_PRINT(pFile," %-s","36Bit");	
	}
	if (pstCap->stDeepColorY420.bDeepColor48Bit)
	{
		PROC_PRINT(pFile," %-s","48Bit");	
	}
	PROC_PRINT(pFile,"\n");

	/* Y420Format[Also] */
	PROC_PRINT(pFile,"%-20s:",	"YCbCr420 VIC[Also]");
	for (i=0;i<pstCap->u32SupportY420VICNum;i++)
	{
		PROC_PRINT(pFile," %u",pstCap->au32SupportY420Format[i]);
	}
	PROC_PRINT(pFile,"\n");
	
	/* Y420Format[Olny] */
	PROC_PRINT(pFile,"%-20s:",	"YCbCr420 VIC[Olny]");
	for (i=0;i<pstCap->u32OnlySupportY420VICNum;i++)
	{
		PROC_PRINT(pFile," %u",pstCap->au32OnlySupportY420Format[i]);
	}
	PROC_PRINT(pFile,"\n");

	
  	/* audio info */
	for (i=0;i<pstCap->u32AudioInfoNum;i++)
	{
		PROC_PRINT(pFile,"---------------------------------- Support Audio Type[%u] ----------------------------------\n",i);
		
		
		PROC_PRINT(pFile,"%-20s: %-15s  |",	"Format Type",	pstCap->stAudioInfo[i].enAudFmtCode < ARRAY_SIZE(s_aStrAudioCodingType) ? 
										s_aStrAudioCodingType[pstCap->stAudioInfo[i].enAudFmtCode]:STR_ERROR);
		PROC_PRINT(pFile,"%-20s: %-15d \n",	"Max Channel Number",pstCap->stAudioInfo[i].u8AudChannel);
		if (pstCap->stAudioInfo[i].u32MaxBitRate)
		{
			PROC_PRINT(pFile,"%-20s: %-15d  |",	"Max Bit Rate(KHz)",	 pstCap->stAudioInfo[i].u32MaxBitRate );
		}
		else
		{
			PROC_PRINT(pFile,"%-20s: %-15s  |",	"Max Bit Rate(KHz)",	 STR_NA );
		}

		PROC_PRINT(pFile,"%-20s:","Bit Depth");
		if (pstCap->stAudioInfo[i].u32SupportBitDepthNum==0)
		{
			PROC_PRINT(pFile," %s",STR_NA);
		}
		for (j=0;j<pstCap->stAudioInfo[i].u32SupportBitDepthNum;j++)
		{
			PROC_PRINT(pFile," %d",pstCap->stAudioInfo[i].enSupportBitDepth[j]);
		}
		PROC_PRINT(pFile,"\n");
		PROC_PRINT(pFile,"%-20s:","Sample Rate(Hz)");	
		for (j=0;j<pstCap->stAudioInfo[i].u32SupportSampleRateNum;j++)
		{
			PROC_PRINT(pFile," %d",pstCap->stAudioInfo[i].enSupportSampleRate[j]);
		}
		PROC_PRINT(pFile,"\n");
	}

	/* basic */
	PROC_PRINT(pFile,"------------------------------------ Basic Capability -------------------------------------\n");
	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Support Hdmi_1_4",pstCap->bSupportHdmi ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-1d.%-1d\n","1st Block Version",pstCap->u8Version,pstCap->u8Revision);

	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Support Hdmi_2_0",pstCap->bSupportHdmi_2_0 ? STR_YES : STR_NO);
	memset(aTmpArray,'\0',sizeof(aTmpArray));
	strncpy(aTmpArray,pstCap->stMfrsInfo.u8MfrsName,4);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"Manufacturer Name",aTmpArray);
	
	PROC_PRINT(pFile,"%-20s: %-15d  |",	"Max TMDS Clock(MHz)",pstCap->u32MaxTMDSClock);
	PROC_PRINT(pFile,"%-20s: %-15u  \n",	"Product Code",pstCap->stMfrsInfo.u32ProductCode );

	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Support HDCP_2_2",pstCap->bSupportHDCP_2_2 ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15u  \n",	"Serial Number",pstCap->stMfrsInfo.u32SerialNumber );

	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Support SCDC",pstCap->bSupportSCDC ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15u  \n",	"Week of Manufacture",pstCap->stMfrsInfo.u32Week );

	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Support DVI Dual",pstCap->bSupportDVIDual ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15u  \n",	"Year of Manufacture",pstCap->stMfrsInfo.u32Year );

	PROC_PRINT(pFile,"%-20s: %-15s  |",	"Support AI",pstCap->bSupportsAI ? STR_YES : STR_NO);
	PROC_PRINT(pFile,"%-20s: %-15s  \n",	"CEC Addr Is Valid",pstCap->stCECAddr.bPhyAddrValid ? STR_YES : STR_NO);


	PROC_PRINT(pFile,"%-20s: %-15u  |",	"Ext Block Cnt",pstCap->u8ExtBlockNum );
	PROC_PRINT(pFile,"%-20s: %02x.%02x.%02x.%02x\n",	"CEC Addr",pstCap->stCECAddr.u8PhyAddrA,pstCap->stCECAddr.u8PhyAddrB,
												pstCap->stCECAddr.u8PhyAddrC,pstCap->stCECAddr.u8PhyAddrD );
	/* speaker */
	PROC_PRINT(pFile,"%-20s:","Support Speaker");
	for (i=0;i<ARRAY_SIZE(pstCap->bSupportAudioSpeaker) && i < ARRAY_SIZE(s_aStr_Speaker);i++)
	{
		PROC_PRINT(pFile," %s",pstCap->bSupportAudioSpeaker[i] ? s_aStr_Speaker[i] : "");
	}
	PROC_PRINT(pFile,"\n");

	HDMI_KFREE(pu8EdidRaw);
	return ;
}

HI_S32 DRV_HDMI_ProcSource0Read(struct seq_file *pFile, HI_VOID *pArg)
{

    PROC_PRINT(pFile,"\n%s\n\n",VERSION_STRING);

	ProcSrc_GlobleSoftStatusPrint(pFile,HDMI_DEVICE_ID0);
	ProcSrc_HardWareStatusPrint(pFile,HDMI_DEVICE_ID0);
	ProcSrc_EventInfoPrint(pFile,HDMI_DEVICE_ID0);

	
	return HI_SUCCESS;
}

HI_S32 DRV_HDMI_ProcSink0Read(struct seq_file *pFile, HI_VOID *pArg)
{

	PROC_PRINT(pFile,"\n%s\n\n",VERSION_STRING);

	ProcSrc_EdidInfoPrint(pFile,HDMI_DEVICE_ID0);
	
	return HI_SUCCESS;
}


