
#include "drv_hdmi_common.h"
#include "hi_hdmi.h"

#include "boot_hdmi_intf.h"
/*#include "hi3536_vo.h"*/

typedef struct{
	HI_U32				u32Vic;
    HI_U32				u32PixlFreq;				/* KHz */
	HI_U32				u32HorActive;
    HI_U32				u32VerActive;
	HI_U32				u32FieldRate;				/* 0.01Hz */
	HI_U32				u32Format;
} HDMI_FMT_PARAM_S;


#define HDMI_BOOT_ERR   printf("HDMI_ERR :%s[%d] ,", __func__, __LINE__); printf
#define HDMI_BOOT_INFO  printf("HDMI_INFO:%s[%d] ,", __func__, __LINE__); printf

#define CHECK_POINTER(p) \
do{                      \
    if(HI_NULL == p){\
        HDMI_BOOT_ERR("null pointer!\n"); \
        return HI_FAILURE;\
    } \
}while(0)

#define CHECK_FAILURE_RET(s32Ret) \
do{                      \
    if (HI_SUCCESS != s32Ret)\
    {\
        printf("ERROR:%s[%d] ,return fail!\n",__func__,__LINE__);\
        return HI_FAILURE;\
    }\
}while(0) 

#define	HDMI_INVALID_PIXFREQ    0xffffffff
#define	HDMI_VIC_VIRTUAL_BASE   255U
#define	HDMI_VIC_PC_BASE        ((HDMI_VIC_VIRTUAL_BASE)+0x100)
#define	HDMI_ARRAY_SIZE(a)      (sizeof(a)/sizeof(a[0]))


static HDMI_DEVICE_S s_stHdmiDev[HDMI_DEVICE_ID_BUTT];


static const HDMI_FMT_PARAM_S s_stFmtParamTable[] = {
	/********************VIC,PixFreq  , HACT ,VACT,FldRate,    EncFmt************************/
    {						 0, 88750 ,   1440,  900,  6000 ,	VO_OUTPUT_1440x900_60	    },		/* no std, vesa define */
    {						 0, 71000 ,   1280,  800,  6000 ,	VO_OUTPUT_1280x800_60	    },		/* no std, vesa define */
    {						 0, 119000 ,  1680,  1560, 6000 ,	VO_OUTPUT_1680x1050_60	    },		/* no std, vesa define */
    {						 0, 162000 ,  1600,  1200, 6000 ,	VO_OUTPUT_1600x1200_60	    },		/* no std, vesa define */
    {						 0, 268500 ,  2560,  1600, 6000 ,	VO_OUTPUT_2560x1600_60	    },		/* no std, vesa define */
    {						 0, 130000 ,  2560,  1440, 3000 ,	VO_OUTPUT_2560x1440_30	    },		/* no std, BVT define ,user defined */
    {                                            0, 238750 ,  2560,  1440, 6000 ,       VO_OUTPUT_2560x1440_60      },          /* no std, BVT define ,user defined */
    {						 0, 170000 ,  1920,  2160, 3000 ,   VO_OUTPUT_1920x2160_30      },      /* no std, BVT define ,user defined */

    
	{						 1, 25175 ,   640 ,  480 , 6000 ,	VO_OUTPUT_640x480_60	    },		/* 1. 640 x 480p @ 60 VGA */
	{						 2, 27000 ,   720 ,  480 , 6000 ,	VO_OUTPUT_480P60			},		/* 2,3 720 x 480p */
	{						 3, 27000 ,   720 ,  480 , 6000 ,	VO_OUTPUT_480P60			},		/* 2,3 720 x 480p */
	{						 4, 74170 ,   1280,  720 , 6000 ,	VO_OUTPUT_720P60			},		/* 4   1280 x 720p */
	{						 5, 74170 ,   1920,  1080, 6000 ,	VO_OUTPUT_1080I60			},		/* 5 1920 x 1080i */
	{						 6, 27000 ,   720 ,  480 , 6000 ,	VO_OUTPUT_NTSC				},		/* 6,7 720 x 480i, pix repl */
	{						 7, 27000 ,   720 ,  480 , 6000 ,	VO_OUTPUT_NTSC				},		/* 6,7 720 x 480i, pix repl */
	{						16, 148500,   1920,  1080, 6000 ,	VO_OUTPUT_1080P60			},		/* 16 1920 x 1080p */
	{						17, 27000 ,   720 ,  576 , 5000 ,	VO_OUTPUT_576P50			},		/* 17,18 720 x 576p */
	{						18, 27000 ,   720 ,  576 , 5000 ,	VO_OUTPUT_576P50			},		/* 17,18 720 x 576p */
	{						19, 74250 ,   1280,  720 , 5000 ,	VO_OUTPUT_720P50			},		/* 19 1280 x 720p */
	{						20, 74250 ,   1920,  1080, 5000 ,	VO_OUTPUT_1080I50			},		/* 20 1920 x 1080i */
	{						21, 27000 ,   1440,  576 , 5000 ,	VO_OUTPUT_PAL				},		/* 21,22 1440 x 576i */
	{						22, 27000 ,   1440,  576 , 5000 ,	VO_OUTPUT_PAL				},		/* 21,22 1440 x 576i */

	/********************VIC,PixFreq  , HACT ,VACT,FldRate,    EncFmt************************/
	{						31, 148500,   1920,  1080, 5000 ,	VO_OUTPUT_1080P50			},		/* 31(1) 1920 x 1080p */
	{						32, 74170 ,   1920,  1080, 2400 ,	VO_OUTPUT_1080P24			},		/* 32(2) 1920 x 1080p */
	{						33, 74250 ,   1920,  1080, 2500 ,	VO_OUTPUT_1080P25			},		/* 33(3) 1920 x 1080p */
	{						34, 74170 ,   1920,  1080, 3000 ,	VO_OUTPUT_1080P30			},		/* 34(4) 1920 x 1080p */

	
	
 /* {					    93, 297000 ,  3840,  2160, 2400 ,	HI_HDMI_VIDEO_FMT_3840X2160P_24 		}, */
    {					    94, 297000 ,  3840,  2160, 2500 ,	VO_OUTPUT_3840x2160_25 		}, 
	{						95, 297000 ,  3840,  2160, 3000 ,	VO_OUTPUT_3840x2160_30 		            },
    {                        96, 594000 ,  3840,  2160, 5000 ,   VO_OUTPUT_3840x2160_50        }, 
    {						97, 594000 ,  3840,  2160, 6000 ,	VO_OUTPUT_3840x2160_60 		            },
 /* {						98, 297000 ,  4096,  2160, 2400 ,	HI_HDMI_VIDEO_FMT_4096X2160P_24 		}, */
 /* {						99, 297000 ,  4096,  2160, 2500 ,	HI_HDMI_VIDEO_FMT_4096X2160P_25 		}, */
 /* {					   100, 297000 ,  4096,  2160, 3000 ,	HI_HDMI_VIDEO_FMT_4096X2160P_30 		}, */
 /* {					   101, 594000 ,  4096,  2160, 5000 ,	HI_HDMI_VIDEO_FMT_4096X2160P_50 		}, */
 /* {					   102, 594000 ,  4096,  2160, 6000 ,	HI_HDMI_VIDEO_FMT_4096X2160P_60 		}, */
 /* {					   103, 297000 ,  3840,  2160, 2400 ,	HI_HDMI_VIDEO_FMT_3840X2160P_24 		}, */
 /* {					   104, 297000 ,  3840,  2160, 2500 ,	HI_HDMI_VIDEO_FMT_3840X2160P_25 		}, */
 /* {					   105, 297000 ,  3840,  2160, 3000 ,	HI_HDMI_VIDEO_FMT_3840X2160P_30 		}, */
 /* {					   106, 594000 ,  3840,  2160, 5000 ,	HI_HDMI_VIDEO_FMT_3840X2160P_50 		}, */
 /* {					   107, 594000 ,  3840,  2160, 6000 ,	HI_HDMI_VIDEO_FMT_3840X2160P_60 	    },

	/********************VIC,PixFreq  , HACT ,VACT,FldRate,    EncFmt************************/
	{ HDMI_VIC_PC_BASE+ 	 9, 37879 , 800 ,600 ,	6000 ,	VO_OUTPUT_800x600_60	},	/* 800x600@60.317 */
	{ HDMI_VIC_PC_BASE+ 	13, 65000 , 1024,768 ,	6000 ,	VO_OUTPUT_1024x768_60	},	/* 1024x768@60 */
	/* { HDMI_VIC_PC_BASE+ 	18, 162000, 1600,1200,	6000 ,	HI_HDMI_VIDEO_FMT_VESA_1600X1200_60 }, */  /* 1600x1200@60 */
	{ HDMI_VIC_PC_BASE+ 	25, 108000, 1280,1024,	6000 ,	VO_OUTPUT_1280x1024_60  },	/* 1280x1024@60 */
	{ HDMI_VIC_PC_BASE+ 	28, 85500 , 1360,768 ,	6000 ,	VO_OUTPUT_1366x768_60	},	/* 1360x768@60 */
	{ HDMI_VIC_PC_BASE+ 	41, 154000, 1920,1200,	6000 ,	VO_OUTPUT_1920x1200_60  },	/* 1920x1200@59.95 */

	/*for 2k & 4k ,we use VIRTUAL VIC*/
	/********************VIC,PixFreq  , HACT ,VACT,FldRate,    EncFmt************************/
	/* { HDMI_VIC_VIRTUAL_BASE+ 1, 297000,  3840,2160, 3000 ,	HI_HDMI_VIDEO_FMT_3840X2160P_30 	}, */  /* 1 2160P@30 */
	/* { HDMI_VIC_VIRTUAL_BASE+ 2, 297000,  3840,2160, 2500 ,	HI_HDMI_VIDEO_FMT_3840X2160P_25 	}, */  /* 2 2160P@25 */
	/* { HDMI_VIC_VIRTUAL_BASE+ 3, 297000,  3840,2160, 2400 ,	HI_HDMI_VIDEO_FMT_3840X2160P_24 	}, */  /* 3 2160P@24 */
	/* { HDMI_VIC_VIRTUAL_BASE+ 4, 297000,  4096,2160, 2400 ,	HI_HDMI_VIDEO_FMT_4096X2160P_24 	}, */  /* 4 4k*2K SMTP 4096*2160@24 */

};

static HI_U32 Hdmi_PixelFreqSearch(HI_U32 u32Fmt)
{
	HI_U32 i = 0;
	HDMI_FMT_PARAM_S *pstFmtParam = HI_NULL;

    HDMI_BOOT_INFO("u32Fmt%d. \n", u32Fmt);

	if (u32Fmt >= VO_OUTPUT_BUTT){
		return HDMI_INVALID_PIXFREQ;
	}

	for (i = 0; i < HDMI_ARRAY_SIZE(s_stFmtParamTable); i++){
		pstFmtParam = (HDMI_FMT_PARAM_S *)&s_stFmtParamTable[i];
		if ((pstFmtParam != HI_NULL) && (pstFmtParam->u32Format == u32Fmt))
		{
			return pstFmtParam->u32PixlFreq;
		}
	}

	HDMI_BOOT_INFO("PixFreq search fail,invalid format=%u\n", u32Fmt);

	return HDMI_INVALID_PIXFREQ;
	
}


static HDMI_VIDEO_TIMING_E DispFmt2HdmiTiming(HI_U32 u32Fmt)
{
    HDMI_VIDEO_TIMING_E enVideoTimingMode;
#if defined(CONFIG_HDMI_STB_SDK)
    switch (u32Fmt)
    {
        case HI_DRV_DISP_FMT_1080P_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_60000;
            break;
        case HI_DRV_DISP_FMT_1080P_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_50000;
            break;
        case HI_DRV_DISP_FMT_1080P_30:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_30000;
            break;
        case HI_DRV_DISP_FMT_1080P_25:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_25000;
            break;
        case HI_DRV_DISP_FMT_1080P_24:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_24000;
            break;
        case HI_DRV_DISP_FMT_1080i_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_60000;
            break;
        case HI_DRV_DISP_FMT_1080i_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_50000;
            break;
        case HI_DRV_DISP_FMT_720P_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_60000;
            break;
        case HI_DRV_DISP_FMT_720P_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_50000;
            break;
        case HI_DRV_DISP_FMT_576P_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X576P_50000;
            break;
        case HI_DRV_DISP_FMT_480P_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X480P_60000;
            break;
        case HI_DRV_DISP_FMT_3840X2160_24:
            enVideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_24000;
            break;            
        case HI_DRV_DISP_FMT_3840X2160_25:
            enVideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_25000;
            break;  
        case HI_DRV_DISP_FMT_3840X2160_30:
            enVideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_30000;
            break;  
        case HI_DRV_DISP_FMT_4096X2160_24:    
            enVideoTimingMode = HDMI_VIDEO_TIMING_4096X2160P_24000;
            break;
#if 0
        case HI_DRV_DISP_FMT_3840X2160_50:
            VideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_50000;
            break;  
        case HI_DRV_DISP_FMT_3840X2160_60:
            VideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_60000;
            break; 
#endif
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
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X576I_50000;
            break;
        case HI_DRV_DISP_FMT_NTSC:
        case HI_DRV_DISP_FMT_NTSC_J:
        case HI_DRV_DISP_FMT_NTSC_443:
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X480I_60000;
            break;
        case HI_DRV_DISP_FMT_861D_640X480_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_640X480P_60000;
            break;
        default:
            HDMI_BOOT_INFO("Non CEA video timing:%d\n", u32DispFmt);
            /* 4k2k && vesa */
            enVideoTimingMode = HDMI_VIDEO_TIMING_UNKNOWN;
            break;
    }

#elif defined(CONFIG_HDMI_BVT_SDK)
    switch (u32Fmt)
    {
	    case VO_OUTPUT_PAL:
			enVideoTimingMode = HDMI_VIDEO_TIMING_720X576I_50000;
	        break;
	    case VO_OUTPUT_NTSC:
			enVideoTimingMode = HDMI_VIDEO_TIMING_720X480I_60000;
	        break;
	    
	    case VO_OUTPUT_1080P24:
			enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_24000;
	        break;
	    case VO_OUTPUT_1080P25:
			enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_25000;
	        break;
	    case VO_OUTPUT_1080P30:
			enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_30000;
	        break;
	    
	    case VO_OUTPUT_720P50: 
			enVideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_50000;
	        break;
	    case VO_OUTPUT_720P60: 
			enVideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_60000;
	        break;
	    case VO_OUTPUT_1080I50:
			enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_50000;
	        break;
	    case VO_OUTPUT_1080I60:
			enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_60000;
	        break;
	    case VO_OUTPUT_1080P50:
			enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_50000;
	        break;
	    case VO_OUTPUT_1080P60:
			enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_60000;
	        break;

	    case VO_OUTPUT_576P50:
			enVideoTimingMode = HDMI_VIDEO_TIMING_720X576P_50000;
	        break;
	    case VO_OUTPUT_480P60:
			enVideoTimingMode = HDMI_VIDEO_TIMING_720X480P_60000;
	        break;

	    case VO_OUTPUT_640x480_60:            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
			enVideoTimingMode = HDMI_VIDEO_TIMING_640X480P_60000;
	        break;
        case VO_OUTPUT_3840x2160_30:          /* 3840x2160_30 */
            enVideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_30000;
            break;

        case VO_OUTPUT_3840x2160_60:          /* 3840x2160_60 */
            enVideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_60000;
            break;
            
        case VO_OUTPUT_3840x2160_25:          /* 3840x2160_25 */
                 enVideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_25000;
                 break;
 
        case VO_OUTPUT_3840x2160_50:          /* 3840x2160_50 */
                 enVideoTimingMode = HDMI_VIDEO_TIMING_3840X2160P_50000;
                 break;

	    case VO_OUTPUT_800x600_60:            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
	    case VO_OUTPUT_1024x768_60:           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
	    case VO_OUTPUT_1280x1024_60:          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
	    case VO_OUTPUT_1366x768_60:           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
	    case VO_OUTPUT_1440x900_60:           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
	    case VO_OUTPUT_1280x800_60:           /* 1280*800@60Hz VGA@60Hz*/    
	    case VO_OUTPUT_1680x1050_60:          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
	    case VO_OUTPUT_1920x2160_30:          /* 1920x2160_30 */
	    case VO_OUTPUT_1600x1200_60:          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
	    case VO_OUTPUT_1920x1200_60:          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/ 
	    case VO_OUTPUT_2560x1440_30:          /* 2560x1440_30 */
            case VO_OUTPUT_2560x1440_60:          /* 2560x1440_60 */
	    case VO_OUTPUT_2560x1600_60:          /* 2560x1600_60 */
	    case VO_OUTPUT_960H_PAL:              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
	    case VO_OUTPUT_960H_NTSC:             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/

	    case VO_OUTPUT_USER:
	    case VO_OUTPUT_BUTT:


#if 0	
        case HI_HDMI_VIDEO_FMT_1080P_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_60000;
            break;
        case HI_HDMI_VIDEO_FMT_1080P_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_50000;
            break;
        case HI_HDMI_VIDEO_FMT_1080P_30:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_30000;
            break;
        case HI_HDMI_VIDEO_FMT_1080P_25:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_25000;
            break;
        case HI_HDMI_VIDEO_FMT_1080P_24:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080P_24000;
            break;
        case HI_HDMI_VIDEO_FMT_1080i_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_60000;
            break;
        case HI_HDMI_VIDEO_FMT_1080i_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1920X1080I_50000;
            break;
        case HI_HDMI_VIDEO_FMT_720P_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_60000;
            break;
        case HI_HDMI_VIDEO_FMT_720P_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_1280X720P_50000;
            break;
        case HI_HDMI_VIDEO_FMT_576P_50:
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X576P_50000;
            break;
        case HI_HDMI_VIDEO_FMT_480P_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X480P_60000;
            break;

        case HI_HDMI_VIDEO_FMT_PAL:
        case HI_HDMI_VIDEO_FMT_PAL_N:
        case HI_HDMI_VIDEO_FMT_PAL_Nc:
        case HI_HDMI_VIDEO_FMT_SECAM_SIN:
        case HI_HDMI_VIDEO_FMT_SECAM_COS:
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X576I_50000;
            break;
        case HI_HDMI_VIDEO_FMT_NTSC:
        case HI_HDMI_VIDEO_FMT_NTSC_J:
        case HI_HDMI_VIDEO_FMT_NTSC_PAL_M:
            enVideoTimingMode = HDMI_VIDEO_TIMING_720X480I_60000;
            break;
        case HI_HDMI_VIDEO_FMT_861D_640X480_60:
            enVideoTimingMode = HDMI_VIDEO_TIMING_640X480P_60000;
            break;
#endif

        default:
            HDMI_BOOT_INFO("Non CEA video timing:%d\n", u32Fmt);
            /* 4k2k && vesa */
            enVideoTimingMode = HDMI_VIDEO_TIMING_UNKNOWN;
            break;

    }
#endif
    return enVideoTimingMode;
}

static HI_VOID DispFmt2VOAttr(HI_U32 u32DispFmt, HDMI_VO_ATTR_S *pstVideoAttr)
{
    pstVideoAttr->enVideoTiming = DispFmt2HdmiTiming(u32DispFmt);
    pstVideoAttr->u32ClkFs      = Hdmi_PixelFreqSearch(u32DispFmt);

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

    if (pstVideoAttr->enVideoTiming != HDMI_VIDEO_TIMING_UNKNOWN)
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_YCbCr444;
    }
    else
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_RGB;
    }
    
    pstVideoAttr->enStereoMode    = HDMI_3D_BUTT;
    pstVideoAttr->enInBitDepth    = HDMI_VIDEO_BITDEPTH_10;
    pstVideoAttr->enActiveAspect  = HDMI_ACTIVE_ASPECT_PICTURE;
   
}

static HI_S32 HdmiDeviceInit(HDMI_DEVICE_S * pstHdmiDev)
{
    HDMI_AO_ATTR_S*          pstAudioAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_APP_ATTR_S*         pstAppAttr;
    CHECK_POINTER(pstHdmiDev);
    
    pstAudioAttr = &pstHdmiDev->stAttr.stAOAttr;
    pstVideoAttr = &pstHdmiDev->stAttr.stVOAttr;
    pstAppAttr   = &pstHdmiDev->stAttr.stAppAttr;

    pstAppAttr->bEnableHdmi = HI_TRUE;
    pstHdmiDev->enTmdsMode  = HDMI_TMDS_MODE_HDMI_1_4;
    
    /* video attribute init*/
    pstVideoAttr->enVideoTiming = HDMI_VIDEO_TIMING_1280X720P_50000;
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
    
    pstVideoAttr->u32PixelRepeat  = 1;
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

    if (pstVideoAttr->enVideoTiming != HDMI_VIDEO_TIMING_UNKNOWN)
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_YCbCr444;
    }
    else
    {
        pstVideoAttr->enInColorSpace = HDMI_COLORSPACE_RGB;
    }
    
    pstVideoAttr->enStereoMode    = HDMI_3D_BUTT;
    pstVideoAttr->enInBitDepth    = HDMI_VIDEO_BITDEPTH_10;
    pstVideoAttr->enActiveAspect  = HDMI_ACTIVE_ASPECT_PICTURE; 
    
    /* audio attribute init*/
    pstAudioAttr->enSoundIntf   = HDMI_AUDIO_INTERFACE__I2S;
    pstAudioAttr->enSampleFs    = HDMI_SAMPLE_RATE_48K;
    pstAudioAttr->enSampleDepth = HDMI_AUDIO_BIT_DEPTH_16;
    pstAudioAttr->enChanels     = HDMI_AUDIO_FORMAT_2CH;
    
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_Open(HDMI_DEVICE_S* pstHdmiDev)
{
    HI_S32 s32Ret = HI_FAILURE;
    CHECK_POINTER(pstHdmiDev);

    /* open hdmi hal module*/
    s32Ret = HAL_HDMI_Open(HI_NULL, &pstHdmiDev->pstHdmiHal);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }
    HdmiDeviceInit(pstHdmiDev);
    pstHdmiDev->pstHdmiHal->stHalCtx.hHdmiDev = pstHdmiDev;
    pstHdmiDev->pstHdmiHal->stHalCtx.u32HdmiID = pstHdmiDev->u32HdmiDevId;
    
    pstHdmiDev->pstHdmiHal->HAL_HDMI_HardwareInit(pstHdmiDev->pstHdmiHal);
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_AVMuteSet(HDMI_DEVICE_S* pstHdmiDev, HI_BOOL bAvMute)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_AvMuteSet(pstHdmiDev->pstHdmiHal, bAvMute);
    
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_Start(HDMI_DEVICE_S* pstHdmiDev)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
   
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyPowerEnableSet(pstHdmiDev->pstHdmiHal, HI_TRUE);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_TmdsModeSet(pstHdmiDev->pstHdmiHal, pstHdmiDev->enTmdsMode);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyOutputEnableSet(pstHdmiDev->pstHdmiHal, HI_TRUE);
    DRV_HDMI_AVMuteSet(pstHdmiDev, HI_FALSE);

    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_Stop(HDMI_DEVICE_S* pstHdmiDev)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    DRV_HDMI_AVMuteSet(pstHdmiDev, HI_TRUE);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyOutputEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_PhyPowerEnableSet(pstHdmiDev->pstHdmiHal, HI_FALSE);
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_Close(HDMI_DEVICE_S* pstHdmiDev)
{
    CHECK_POINTER(pstHdmiDev);
 
    DRV_HDMI_Stop(pstHdmiDev);
    HAL_HDMI_Close(pstHdmiDev->pstHdmiHal);
    return HI_SUCCESS;   
}



#if 0
static HI_S32 CheckAudioAttr(HDMI_AO_ATTR_S *pstAOAttr)
{
    if (HDMI_AUDIO_INTERFACE__I2S != pstAOAttr->enSoundIntf
       && HDMI_AUDIO_INTERFACE__SPDIF != pstAOAttr->enSoundIntf
       && HDMI_AUDIO_INTERFACE__HBRA != pstAOAttr->enSoundIntf)
    {
        HDMI_COM_ERR("The Audio interface(%d) is invalid\n",pstAOAttr->enSoundIntf);
        return HI_FAILURE;
    }

    if (pstAOAttr->enChanels < HDMI_AUDIO_FORMAT_2CH || pstAOAttr->enChanels > HDMI_AUDIO_FORMAT_8CH)
    {
        HDMI_COM_ERR("The Audio chanel number(%d) is invalid\n",pstAOAttr->enChanels);
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
            HDMI_COM_ERR("The input Audio Frequency(%d) is invalid\n",pstAOAttr->enSampleFs);
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
    pstHdmiDev->pstHdmiHal->HAL_HDMI_AudioPathSet(pstHdmiDev->pstHdmiHal, &stAudioCfg);

    return HI_SUCCESS;
    
}



HI_S32 DRV_HDMI_AOAttrSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_AO_ATTR_S *pstAOAttr)
{
    HDMI_AO_ATTR_S*          pstAudioAttr;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstAOAttr);
  
    if (HI_SUCCESS != CheckAudioAttr(pstAOAttr))
    {
        HDMI_COM_ERR("HDMI_CheckAudioAttr fail\n");
        return HI_FAILURE;
    }

    pstAudioAttr = &pstHdmiDev->stAttr.stAOAttr;
#if 0    
    if (!memcmp(pstAOAttr, pstAudioAttr, sizeof(HDMI_AO_ATTR_S)))
    {
        HDMI_COM_INFO("The Audio Attr is not change\n");
        return HI_SUCCESS;    
    }
#endif
    HdmiAudioPathSet(pstHdmiDev, pstAOAttr);
    memcpy(pstAudioAttr, pstAOAttr, sizeof(HDMI_AO_ATTR_S));

    HDMI_AudioInfoFrameSet(&pstHdmiDev->stInfoFrame);

    return HI_SUCCESS;
}

#endif


static HI_S32 CheckVideoAttr(HDMI_VO_ATTR_S* pstVOAttr)
{
    CHECK_POINTER(pstVOAttr);
    
    if (pstVOAttr->enVideoTiming >= HDMI_VIDEO_TIMING_BUTT ||
        pstVOAttr->enVideoTiming < 0)
    {
        HDMI_BOOT_INFO("Hdmi video timing(%d) is wrong\n", pstVOAttr->enVideoTiming);
        return HI_FAILURE;
    }

    
    if (pstVOAttr->enHvSyncPol > HDMI_HV_SYNC_POL_HNVN ||
        pstVOAttr->enHvSyncPol < 0)
    {
        HDMI_BOOT_INFO("Hdmi video HvSyncPol(%d) is wrong\n", pstVOAttr->enHvSyncPol);
        return HI_FAILURE;
    }

    if (pstVOAttr->enInColorSpace >= HDMI_COLORSPACE_BUTT ||
        pstVOAttr->enInColorSpace < 0)
    {
        HDMI_BOOT_INFO("Hdmi video InColorSpace(%d) is wrong\n", pstVOAttr->enInColorSpace);
        return HI_FAILURE;
    }

    if (pstVOAttr->enColorimetry <= HDMI_COLORIMETRY__NONE ||
        pstVOAttr->enColorimetry > HDMI_COLORIMETRY__EXTENDED)
    {
        HDMI_BOOT_INFO("Hdmi video Colorimetry(%d) is wrong\n", pstVOAttr->enColorimetry);
        return HI_FAILURE;
    }

    if (pstVOAttr->enRGBQuantization < HDMI_QUANTIZATION_RANGE_DEFAULT ||
        pstVOAttr->enRGBQuantization > HDMI_QUANTIZATION_RANGE_FULL)
    {
        HDMI_BOOT_INFO("Hdmi video RGBQuantization(%d) is wrong\n", pstVOAttr->enRGBQuantization);
        return HI_FAILURE;
    }

    if (pstVOAttr->enPictureAspect < HDMI_PICTURE_ASPECT_4_3 ||
        pstVOAttr->enPictureAspect > HDMI_PICTURE_ASPECT_16_9)
    {
        HDMI_BOOT_INFO("Hdmi video PictureAspect(%d) is wrong\n", pstVOAttr->enPictureAspect);
        return HI_FAILURE;
    }

    if (pstVOAttr->enInBitDepth < HDMI_VIDEO_BITDEPTH_8 ||
        pstVOAttr->enInBitDepth > HDMI_VIDEO_BITDEPTH_12)
    {
        HDMI_BOOT_INFO("Hdmi video InBitDepth(%d) is wrong\n", pstVOAttr->enInBitDepth);
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

    stVideoCfg.enInBitDepth     = pstVOAttr->enInBitDepth;
    stVideoCfg.enHvSyncPol      = pstVOAttr->enHvSyncPol;
    stVideoCfg.enQuantization   = pstVOAttr->enRGBQuantization;
    stVideoCfg.enInColorSpace   = pstVOAttr->enInColorSpace;
    stVideoCfg.u32PixelClk      = pstVOAttr->u32ClkFs;
    stVideoCfg.enDeepColor      = pstAppAttr->enDeepColorMode;
    stVideoCfg.enOutColorSpace  = pstAppAttr->enOutColorSpace;

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
            break;      
    }
    /* Video will mute and unmute in this function call*/
    pstHdmiDev->pstHdmiHal->HAL_HDMI_VideoPathSet(pstHdmiDev->pstHdmiHal, &stVideoCfg);
    return HI_SUCCESS;
}

#if 0 
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
    pstHdmiDev->pstHdmiHal->HAL_HDMI_AudioPathSet(pstHdmiDev->pstHdmiHal, &stAudioCfg);

    return HI_SUCCESS;
    
}

HI_S32 DRV_HDMI_VOAttrSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_VO_ATTR_S* pstVOAttr)
{
    HDMI_AO_ATTR_S*          pstAudioAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_APP_ATTR_S*         pstAppAttr;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstVOAttr);
    
    if (HI_SUCCESS != CheckVideoAttr(pstVOAttr))
    {
        HDMI_COM_ERR("HDMI_CheckVideoAttr fail\n");
        return HI_FAILURE;
    }
    
    pstAudioAttr = &pstHdmiDev->stAttr.stAOAttr;
    pstVideoAttr = &pstHdmiDev->stAttr.stVOAttr;
    pstAppAttr   = &pstHdmiDev->stAttr.stAppAttr;
    
    memcpy(pstVideoAttr, pstVOAttr, sizeof(HDMI_VO_ATTR_S));  
    
    if (pstAppAttr->bEnableHdmi && pstVideoAttr->enVideoTiming >= HDMI_VIDEO_TIMING_3840X2160P_50000)
    {
        if (pstAppAttr->enOutColorSpace != HDMI_COLORSPACE_YCbCr420)
        {
            pstHdmiDev->enTmdsMode = HDMI_TMDS_MODE_HDMI_2_0;
        }
        else
        {
            pstHdmiDev->enTmdsMode = HDMI_TMDS_MODE_HDMI_1_4;
        }
    }   
     
    HdmiVideoPathSet(pstHdmiDev, pstVOAttr);
    DRV_HDMI_AVIInfoFrameSet(&pstHdmiDev->stInfoFrame);
#if 0
    HdmiAudioPathSet(pstHdmiDev, pstAudioAttr);
    HDMI_AudioInfoFrameSet(&pstHdmiDev->stInfoFrame);
#endif 
    DRV_HDMI_VendorInfoFrameSet(&pstHdmiDev->stInfoFrame);
   
    //DRV_HDMI_Start(pstHdmiDev);
    return HI_SUCCESS;
}
#endif

HI_S32 DRV_HDMI_AttrGet(HDMI_DEVICE_S* pstHdmiDev, HDMI_ATTR_S *pstAttr)
{
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstAttr);
    
    memcpy(pstAttr, &pstHdmiDev->stAttr, sizeof(HDMI_ATTR_S));
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_AttrSet(HDMI_DEVICE_S* pstHdmiDev, HDMI_ATTR_S *pstAttr)
{
   // HDMI_AO_ATTR_S*          pstAudioAttr;
    HDMI_VO_ATTR_S*          pstVideoAttr;
    HDMI_APP_ATTR_S*         pstAppAttr;
    HDMI_TX_CAPABILITY_E     enTxCapability;
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstAttr);

   // pstAudioAttr = &pstAttr->stAOAttr;
    pstVideoAttr = &pstAttr->stVOAttr;
    pstAppAttr   = &pstAttr->stAppAttr;
    CheckVideoAttr(pstVideoAttr);
    memcpy(&pstHdmiDev->stAttr, pstAttr, sizeof(HDMI_ATTR_S)); 
    
    pstHdmiDev->pstHdmiHal->HAL_HDMI_TxCapabilityGet(pstHdmiDev->pstHdmiHal, &enTxCapability);
    if ((pstAppAttr->bEnableHdmi) && (pstVideoAttr->enVideoTiming >= HDMI_VIDEO_TIMING_3840X2160P_50000))
    {
        if (pstAppAttr->enOutColorSpace != HDMI_COLORSPACE_YCbCr420)
        {
            if (enTxCapability & HDMI_TX_SUPPORT_HDMI2_0)
            {
                pstHdmiDev->enTmdsMode = HDMI_TMDS_MODE_HDMI_2_0;
            }
            else
            {
                HDMI_BOOT_INFO("HDMI not support HDMI2.0\n");
                return HI_FAILURE;
            }
        }
        else
        {
            if (enTxCapability & HDMI_TX_SUPPORT_YUV420)
            {
                pstHdmiDev->enTmdsMode = HDMI_TMDS_MODE_HDMI_1_4;
            }
            else
            {
                HDMI_BOOT_INFO("HDMI not support YUV420\n");
                return HI_FAILURE;
            }
        }
    }
    
    HdmiVideoPathSet(pstHdmiDev, pstVideoAttr); 

    DRV_HDMI_AVIInfoFrameSend(&pstHdmiDev->stInfoFrame, pstAppAttr->bEnableHdmi);
#if 0
    HdmiAudioPathSet(pstHdmiDev, pstAudioAttr);
    DRV_HDMI_AudioInfoFrameSet(&pstHdmiDev->stInfoFrame);
#endif

	DRV_HDMI_VendorInfoFrameSend(&pstHdmiDev->stInfoFrame, pstAppAttr->bEnableHdmi); 
    return HI_SUCCESS;
}
#if 0
static HI_S32 Disp2HdmiVoAttr(HDMI_VO_ATTR_S *pstDestAttr, HDMI_VIDEO_ATTR_S *pstSrcAttr)
{
    if (pstSrcAttr == HI_NULL || pstDestAttr == HI_NULL)
    {
        return HI_FAILURE;
    }

    pstDestAttr->enColorimetry   = pstSrcAttr->enColorimetry;
    if (pstSrcAttr->enColorimetry > HDMI_COLORIMETRY_EXTENDED)
    {
        pstDestAttr->enColorimetry = HDMI_COLORIMETRY__EXTENDED;
        pstDestAttr->enExtendedColorimetry = pstSrcAttr->enColorimetry - HDMI_COLORIMETRY_EXTENDED - 1;
    }
    pstDestAttr->enPictureAspect   = pstSrcAttr->enPictureAspect;
    pstDestAttr->enActiveAspect    = pstSrcAttr->enActiveAspect;    
    pstDestAttr->enHvSyncPol       = pstSrcAttr->enHvSyncPol;
    pstDestAttr->enInBitDepth      = pstSrcAttr->enInBitDepth;
    pstDestAttr->enInColorSpace    = pstSrcAttr->enInColorSpace;
    pstDestAttr->enPictureScaling  = pstSrcAttr->enPictureScaling;
    pstDestAttr->enRGBQuantization = pstSrcAttr->enRGBQuantization;
    pstDestAttr->enYCCQuantization = pstSrcAttr->enYCCQuantization;
    pstDestAttr->enStereoMode      = pstSrcAttr->enStereoMode;
    pstDestAttr->enVideoTiming     = DispFmt2HdmiTiming(pstSrcAttr->enVideoFmt);
    pstDestAttr->u32ClkFs          = pstSrcAttr->u32ClkFs;
    pstDestAttr->u32PixelRepeat    = pstSrcAttr->u32PixelRepeat;

    return HI_SUCCESS;
}
#endif
HI_S32 HI_DRV_HDMI_Init(HI_VOID)
{
   return HI_SUCCESS;
}

HI_S32 HI_DRV_HDMI_Open(HI_UNF_HDMI_ID_E enHdmiID)
{
    memset(&s_stHdmiDev[enHdmiID], 0, sizeof(HDMI_DEVICE_S));
    s_stHdmiDev[enHdmiID].u32HdmiDevId = enHdmiID;
    return DRV_HDMI_Open(&s_stHdmiDev[enHdmiID]);
}
#if 0
HI_S32 HI_DRV_HDMI_SetFormat(HI_UNF_HDMI_ID_E enHdmiID, HDMI_VIDEO_ATTR_S *pstVideoAttr)
{
    HDMI_VO_ATTR_S  stVOAttr;
    //DispFmt2VOAttr(u32DispFmt, &stVOAttr);
    Disp2HdmiVoAttr(&stVOAttr, pstVideoAttr);
    return DRV_HDMI_VOAttrSet(&s_stHdmiDev[enHdmiID], &stVOAttr);
}
#endif
HI_S32 HI_DRV_HDMI_SetAttr(HI_UNF_HDMI_ID_E enHdmiID, HI_DRV_HDMI_ATTR_S *pstAttr)
{
    HDMI_ATTR_S stAttr;
    memset(&stAttr, 0, sizeof(HDMI_ATTR_S));
    DRV_HDMI_AttrGet(&s_stHdmiDev[enHdmiID], &stAttr);
    DispFmt2VOAttr(pstAttr->u32DispFmt, &stAttr.stVOAttr);
    stAttr.stAppAttr.bEnableHdmi     = pstAttr->bEnableHdmi;
    //stAttr.stAppAttr.bEnableAudio    = pstAttr->bEnableAudio;
    //stAttr.stAppAttr.bEnableVideo    = pstAttr->bEnableVideo;
    stAttr.stAppAttr.enOutColorSpace = pstAttr->enVidOutMode;
    stAttr.stVOAttr.enInColorSpace   = pstAttr->enVidInMode;
    return DRV_HDMI_AttrSet(&s_stHdmiDev[enHdmiID], &stAttr);
}

int hdmi_display(unsigned int vosync, unsigned int input, unsigned int output)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_DRV_HDMI_ATTR_S stAttr;
    
    s32Ret = HI_DRV_HDMI_Init();
    CHECK_FAILURE_RET(s32Ret);

    s32Ret = HI_DRV_HDMI_Open(HI_HDMI_ID_0);
    CHECK_FAILURE_RET(s32Ret);
    
    stAttr.bEnableHdmi = HI_TRUE;
    stAttr.u32DispFmt = vosync; 
    stAttr.enVidOutMode = output;   
    stAttr.enVidInMode = input;    
    s32Ret = HI_DRV_HDMI_SetAttr(HI_HDMI_ID_0, &stAttr);
    CHECK_FAILURE_RET(s32Ret);

    s32Ret = HI_DRV_HDMI_Start(HI_HDMI_ID_0 );
    CHECK_FAILURE_RET(s32Ret);

    return s32Ret;
}

void hdmi_stop(void)
{
    HI_DRV_HDMI_Close( HI_HDMI_ID_0);
    HI_DRV_HDMI_DeInit();
    return ;
}


#if 0
HI_S32 HI_DRV_HDMI_GetAttr(HI_UNF_HDMI_ID_E enHdmiID, HDMI_ATTR_S *pstAttr)
{
    return DRV_HDMI_AttrGet(&s_stHdmiDev[enHdmiID], pstAttr);
}
#endif
HI_S32 HI_DRV_HDMI_Start(HI_UNF_HDMI_ID_E enHdmiID)
{
    return DRV_HDMI_Start(&s_stHdmiDev[enHdmiID]);
}

HI_S32 HI_DRV_HDMI_Close(HI_UNF_HDMI_ID_E enHdmiID)
{
    return DRV_HDMI_Close(&s_stHdmiDev[enHdmiID]);
}

HI_S32 HI_DRV_HDMI_DeInit(HI_VOID)
{
   return HI_SUCCESS;
}

HI_S32 HI_UNF_HDMI_Init(void)
{
    return HI_DRV_HDMI_Init();
}

HI_S32 HI_UNF_HDMI_DeInit(void)
{
    return HI_DRV_HDMI_DeInit();
}


HI_S32 HI_UNF_HDMI_Open(HI_UNF_HDMI_ID_E enHdmi)
{
    return HI_DRV_HDMI_Open(enHdmi);
}
#if 0
HI_S32 HI_UNF_HDMI_SetFormat(HI_UNF_HDMI_ID_E enHdmi, HI_U32 u32DispFmt)
{
    return HI_DRV_HDMI_SetFormat(enHdmi, u32DispFmt);
}
#endif
HI_S32 HI_UNF_HDMI_SetAttr(HI_UNF_HDMI_ID_E enHdmi, HI_UNF_HDMI_ATTR_S *pstAttr)
{
    return HI_DRV_HDMI_SetAttr(enHdmi, (HI_DRV_HDMI_ATTR_S*)pstAttr);
}
#if 0
HI_S32 HI_UNF_HDMI_GetAttr(HI_UNF_HDMI_ID_E enHdmi, HI_UNF_HDMI_ATTR_S *pstAttr)
{
    return HI_DRV_HDMI_GetAttr(enHdmi, pstAttr);
}
#endif
HI_S32 HI_UNF_HDMI_Start(HI_UNF_HDMI_ID_E enHdmi)
{
    return HI_DRV_HDMI_Start(enHdmi);
}


HI_S32 HI_UNF_HDMI_Close(HI_UNF_HDMI_ID_E enHdmi)
{
    return HI_DRV_HDMI_Close(enHdmi);
}




