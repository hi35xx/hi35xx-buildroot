#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <mach/hardware.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/delay.h>

#include "hdmi_hal.h"
#include "drv_hdmi_edid.h"
#include "drv_hdmi_event.h"
#include "drv_hdmi_common.h"
#include "drv_hdmi_platform.h"
#include "drv_hdmi_intf.h"
#include "drv_hdmi_ioctl.h"

#if defined(CONFIG_HDMI_STB_SDK)
#include "drv_hdmi_ext.h"
#include "hi_drv_hdmi.h"

#define 	HDMI_FEILD_RATE_DEVIATION	50
#define 	HDMI_PIXL_FREQ_DEVIATION	100
#define 	HDMI_VIC_PC_BASE 			((HDMI_VIC_VIRTUAL_BASE)+0x100)

		
typedef struct 
{
	HI_U32				u32EtablishCode;
	HI_U32				u32Format;
}HDMI_ESTAB_FORMAT_S;


typedef struct 
{
	HI_U32				u32Vic; 
	HI_U32				u32PixlFreq;				/* KHz */
	HI_U32				u32HorActive;	
	HI_U32				u32VerActive;
	HI_U32				u32FieldRate;				/* 0.01Hz */
	HI_U32				u32Format;
}HDMI_FMT_PARAM_S;


static const HDMI_ESTAB_FORMAT_S s_g_stFmtEstabTable[]=
{
	{ HDMI_EDID_ESTABTIMG_VESA_800X600_60		,	HI_UNF_ENC_FMT_VESA_800X600_60	},
	{ HDMI_EDID_ESTABTIMG_VESA_800X600_56		,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_VESA_640X480_75		,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_VESA_640X480_72		,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_IBM_VGA_640X480_67	,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_IBM_XGA_640X480_60	,	HI_UNF_ENC_FMT_VESA_800X600_60	},
	{ HDMI_EDID_ESTABTIMG_IBM_XGA2_720X400_88	,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_IBM_XGA_720X400_70	,	HI_UNF_ENC_FMT_BUTT 			},

	{ HDMI_EDID_ESTABTIMG_VESA_1280X1024_75 	,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_VESA_1024X768_75		,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_VESA_1024X768_70		,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_VESA_1024X768_60		,	HI_UNF_ENC_FMT_VESA_1024X768_60 },
	{ HDMI_EDID_ESTABTIMG_IBM_1024X768_87		,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_APPLE_MACII_832X624_75,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_VESA_800X600_75		,	HI_UNF_ENC_FMT_BUTT 			},
	{ HDMI_EDID_ESTABTIMG_VESA_800X600_72		,	HI_UNF_ENC_FMT_BUTT 			},

	{ HDMI_EDID_ESTABTIMG_VESA_1152X870_75		,	HI_UNF_ENC_FMT_BUTT 			},

};


static const HDMI_FMT_PARAM_S s_g_stFmtParamTable[]=
{
	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    EncFmt************************/
	{						 1, 25175 ,  640 ,480 , 6000 ,	HI_UNF_ENC_FMT_861D_640X480_60	},		// 1. 640 x 480p @ 60 VGA
	{						 2, 27000 ,  720 ,480 , 6000 ,	HI_UNF_ENC_FMT_480P_60			},		// 2,3 720 x 480p
	{						 3, 27000 ,  720 ,480 , 6000 ,	HI_UNF_ENC_FMT_480P_60			},		// 2,3 720 x 480p
	{						 4, 74170 ,  1280,720 , 6000 ,	HI_UNF_ENC_FMT_720P_60			},		// 4   1280 x 720p
	{						 5, 74170 ,  1920,1080, 6000 ,	HI_UNF_ENC_FMT_1080i_60 		},		// 5 1920 x 1080i
	{						 6, 27000 ,  720 ,480 , 6000 ,	HI_UNF_ENC_FMT_NTSC 			},		// 6,7 720 x 480i, pix repl
	{						 7, 27000 ,  720 ,480 , 6000 ,	HI_UNF_ENC_FMT_NTSC 			},		// 6,7 720 x 480i, pix repl
	{						 8, 27000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 8,9(1) 1440 x 240p
	{						 8, 27000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 8,9(2) 1440 x 240p
	{						 9, 27000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 8,9(1) 1440 x 240p
	{						 9, 27000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 8,9(2) 1440 x 240p
	{						10, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 10,11 2880 x 480p
	{						11, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 10,11 2880 x 480p
	{						12, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 12,13(1) 2280 x 240p
	{						12, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 12,13(2) 2280 x 240p
	{						14, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 14, 15 1140 x 480p
	{						16, 148500,  1920,1080, 6000 ,	HI_UNF_ENC_FMT_1080P_60 		},		// 16 1920 x 1080p
	{						17, 27000 ,  720 ,576 , 5000 ,	HI_UNF_ENC_FMT_576P_50			},		// 17,18 720 x 576p
	{						18, 27000 ,  720 ,576 , 5000 ,	HI_UNF_ENC_FMT_576P_50			},		// 17,18 720 x 576p
	{						19, 74250 ,  1280,720 , 5000 ,	HI_UNF_ENC_FMT_720P_50			},		// 19 1280 x 720p
	{						20, 74250 ,  1920,1080, 5000 ,	HI_UNF_ENC_FMT_1080i_50 		},		// 20 1920 x 1080i
	{						21, 27000 ,  1440,576 , 5000 ,	HI_UNF_ENC_FMT_PAL				},		// 21,22 1440 x 576i
	{						22, 27000 ,  1440,576 , 5000 ,	HI_UNF_ENC_FMT_PAL				},		// 21,22 1440 x 576i
	{						23, 27000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 23,24(1) 1440 x 288p
	{						23, 27000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 23,24(2) 1440 x 288p
	{						23, 27000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 23,24(3) 1440 x 288p
	{						25, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 25, 26 2880 x 576p
	{						27, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 27,28(1) 2880 x 288p
	{						27, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 27,28(2) 2880 x 288p
	{						27, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 27,28(3) 2880 x 288p
	{						29, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 29,30 1440 x 576p

	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    EncFmt************************/
	{						31, 148500,  1440,576 , 5000 ,	HI_UNF_ENC_FMT_1080P_50 		},		// 31(1) 1920 x 1080p
	{						32, 74170 ,  1920,1080, 2400 ,	HI_UNF_ENC_FMT_1080P_24 		},		// 32(2) 1920 x 1080p
	{						33, 74250 ,  1920,1080, 2500 ,	HI_UNF_ENC_FMT_1080P_25 		},		// 33(3) 1920 x 1080p
	{						34, 74170 ,  1920,1080, 3000 ,	HI_UNF_ENC_FMT_1080P_30 		},		// 34(4) 1920 x 1080p
	{						35, 108000,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 35, 36 2880 x 480p@59.94/60Hz
	{						37, 108000,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 37, 38 2880 x 576p@50Hz
	{						39, 72000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 39 1920 x 1080i@50Hz
	{						40, 148500,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 40 1920 x 1080i@100Hz
	{						41, 148500,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 41 1280 x 720p@100Hz
	{						42, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 42, 43, 720p x 576p@100Hz
	{						44, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 44, 45, 720p x 576i@100Hz, pix repl
	{						46, 148500,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 46, 1920 x 1080i@119.88/120Hz
	{						47, 148500,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 47, 1280 x 720p@119.88/120Hz
	{						48, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 48, 49 720 x 480p@119.88/120Hz
	{						50, 54000 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 50, 51 720 x 480i@119.88/120Hz
	{						52, 10800 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 52, 53, 720 x 576p@200Hz
	{						54, 10800 ,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 54, 55, 1440 x 720i @200Hz, pix repl
	{						56, 108000,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 56, 57, 720 x 480p @239.76/240Hz
	{						58, 108000,  0	 ,0   , 0	 ,	HI_UNF_ENC_FMT_BUTT 			},		// 58, 59, 1440 x 480i @239.76/240Hz, pix repl

#if defined (DVI_SUPPORT)
	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    EncFmt************************/
	{ HDMI_VIC_PC_BASE+ 	 0, 31500 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 640x350@85.08
	{ HDMI_VIC_PC_BASE+ 	 1, 31500 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 640x400@85.08
	{ HDMI_VIC_PC_BASE+ 	 2, 27000 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 720x400@70.08
	{ HDMI_VIC_PC_BASE+ 	 3, 35000 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 720x400@85.04
	{ HDMI_VIC_PC_BASE+ 	 4, 25170 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 640x480@59.94
	{ HDMI_VIC_PC_BASE+ 	 5, 31500 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 640x480@72.80
	{ HDMI_VIC_PC_BASE+ 	 6, 31500 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 640x480@75.00
	{ HDMI_VIC_PC_BASE+ 	 7, 36000 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 640x480@85.00
	{ HDMI_VIC_PC_BASE+ 	 8, 36000 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 800x600@56.25
	{ HDMI_VIC_PC_BASE+ 	 9, 40000 , 800 ,600 ,	6000 ,	  HI_UNF_ENC_FMT_VESA_800X600_60	},	// 800x600@60.317
	{ HDMI_VIC_PC_BASE+ 	10, 50000 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 800x600@72.19
	{ HDMI_VIC_PC_BASE+ 	11, 49500 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 800x600@75
	{ HDMI_VIC_PC_BASE+ 	12, 56250 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 800x600@85.06
	{ HDMI_VIC_PC_BASE+ 	13, 65000 , 1024,768 ,	6000 ,	  HI_UNF_ENC_FMT_VESA_1024X768_60	},	// 1024x768@60
	{ HDMI_VIC_PC_BASE+ 	14, 75000 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1024x768@70.07
	{ HDMI_VIC_PC_BASE+ 	15, 78750 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1024x768@75.03
	{ HDMI_VIC_PC_BASE+ 	16, 94500 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1024x768@85
	{ HDMI_VIC_PC_BASE+ 	17, 108000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1152x864@75
	{ HDMI_VIC_PC_BASE+ 	18, 162000, 1600,1200,	6000 ,	  HI_UNF_ENC_FMT_VESA_1600X1200_60	},	// 1600x1200@60
	{ HDMI_VIC_PC_BASE+ 	19, 68250 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1280x768@59.95
	{ HDMI_VIC_PC_BASE+ 	20, 79500 , 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1280x768@59.87

	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    EncFmt************************/
	{ HDMI_VIC_PC_BASE+ 	21, 102200, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1280x768@74.89
	{ HDMI_VIC_PC_BASE+ 	22, 117500, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1280x768@85
	{ HDMI_VIC_PC_BASE+ 	23, 108000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1280x960@60
	{ HDMI_VIC_PC_BASE+ 	24, 148500, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1280x960@85
	{ HDMI_VIC_PC_BASE+ 	25, 108000, 1280,1024,	6000 ,	  HI_UNF_ENC_FMT_VESA_1280X1024_60	},	// 1280x1024@60
	{ HDMI_VIC_PC_BASE+ 	26, 135000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1280x1024@75
	{ HDMI_VIC_PC_BASE+ 	27, 157500, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 157 280x1024@85
	{ HDMI_VIC_PC_BASE+ 	28, 85500 , 1360,768 ,	6000 ,	  HI_UNF_ENC_FMT_VESA_1366X768_60	},	// 1360x768@60
	{ HDMI_VIC_PC_BASE+ 	29, 101000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1400x105@59.95
	{ HDMI_VIC_PC_BASE+ 	30, 121750, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 400x105@59.98
	{ HDMI_VIC_PC_BASE+ 	31, 156000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 400x105@74.87
	{ HDMI_VIC_PC_BASE+ 	32, 179500, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 400x105@84.96
	{ HDMI_VIC_PC_BASE+ 	33, 175500, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 600x1200@65
	{ HDMI_VIC_PC_BASE+ 	34, 189000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 600x1200@70
	{ HDMI_VIC_PC_BASE+ 	35, 202500, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 600x1200@75
	{ HDMI_VIC_PC_BASE+ 	36, 229500, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 600x1200@85
	{ HDMI_VIC_PC_BASE+ 	37, 204750, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 792x1344@60
	{ HDMI_VIC_PC_BASE+ 	38, 261000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 792x1344@74.997
	{ HDMI_VIC_PC_BASE+ 	39, 218250, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 856x1392@60
	{ HDMI_VIC_PC_BASE+ 	40, 288000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1856x1392@75
	{ HDMI_VIC_PC_BASE+ 	41, 154000, 1920,1200,	6000 ,	  HI_UNF_ENC_FMT_VESA_1920X1200_60	},	// 1920x1200@59.95
	{ HDMI_VIC_PC_BASE+ 	42, 193250, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1920x1200@59.88
	{ HDMI_VIC_PC_BASE+ 	43, 245250, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1920x1200@74.93
	{ HDMI_VIC_PC_BASE+ 	44, 281250, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1920x1200@84.93
	{ HDMI_VIC_PC_BASE+ 	45, 234000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1920x1440@60
	{ HDMI_VIC_PC_BASE+ 	46, 297000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1920x1440@75
	{ HDMI_VIC_PC_BASE+ 	47, 297000, 0	,0	 ,	0	 ,	  HI_UNF_ENC_FMT_BUTT				},	// 1920x1440@75
#endif
	/*for 2k & 4k ,we use VIRTUAL VIC*/
	/********************VIC,PixFfeq  , HACT ,VACT,FldRate,    EncFmt************************/
	{ HDMI_VIC_VIRTUAL_BASE+ 1, 297000,  3840,2160, 3000 ,	HI_UNF_ENC_FMT_3840X2160_30 		}, // 1 2160P@30
	{ HDMI_VIC_VIRTUAL_BASE+ 2, 297000,  3840,2160, 2500 ,	HI_UNF_ENC_FMT_3840X2160_25 		}, // 2 2160P@25
	{ HDMI_VIC_VIRTUAL_BASE+ 3, 297000,  3840,2160, 2400 ,	HI_UNF_ENC_FMT_3840X2160_24 		}, // 3 2160P@24
	{ HDMI_VIC_VIRTUAL_BASE+ 4, 297000,  4096,2160, 2400 ,	HI_UNF_ENC_FMT_4096X2160_24 		}, // 4 4k*2K SMTP 4096*2160@24

};


static HI_S32 HdmiExtIoctl(unsigned int cmd, void *argp);
static HI_S32 Disp2HdmiVoAttr(HDMI_DEVICE_S* pstHdmiDev, HDMI_VO_ATTR_S *pstDestAttr, HDMI_VIDEO_ATTR_S *pstSrcAttr);

HI_S32 HI_DRV_HDMI_Suspend(PM_BASEDEV_S *pdev, pm_message_t state);
HI_S32 HI_DRV_HDMI_Resume(PM_BASEDEV_S *pdev);
HI_S32 HI_DRV_HDMI_SoftResume(HDMI_VIDEO_ATTR_S *pstVideoAttr);

static HDMI_EXPORT_FUNC_S s_stHdmiExportFuncs = {
    .pfnHdmiInit = HI_DRV_HDMI_Init,
    .pfnHdmiDeinit = HI_DRV_HDMI_Deinit,
    .pfnHdmiOpen  = HI_DRV_HDMI_Open,
    .pfnHdmiClose = HI_DRV_HDMI_Close,
    .pfnHdmiGetPlayStus = HI_DRV_HDMI_GetPlayStatus,
    .pfnHdmiGetAoAttr = HI_DRV_HDMI_GetAOAttr,
    .pfnHdmiGetSinkCapability = HI_DRV_HDMI_GetSinkCapability,
    .pfnHdmiGetAudioCapability = HI_DRV_HDMI_GetAudioCapability,
    .pfnHdmiSetAudioMute = HI_DRV_HDMI_SetAudioMute,
    .pfnHdmiSetAudioUnMute = HI_DRV_HDMI_SetAudioUnMute,
    .pfnHdmiAudioChange = HI_DRV_HDMI_SetAOAttr,
    .pfnHdmiPreFormat = HI_DRV_HDMI_PreFormat,
    .pfnHdmiSetFormat = HI_DRV_HDMI_SetFormat,
    .pfnHdmiDetach = HI_DRV_HDMI_Detach,
    .pfnHdmiAttach = HI_DRV_HDMI_Attach,
    .pfnHdmiResume = HI_DRV_HDMI_Resume,
    .pfnHdmiSuspend = HI_DRV_HDMI_Suspend,
    .pfnHdmiSoftResume = HI_DRV_HDMI_SoftResume,
};

#define CHECK_HDMI_OPEN(HdmiId) \
do \
{   \
    if (GetHdmiDevice(HdmiId) == HI_NULL) \
    { \
        HDMI_WARN("The hdmi device id is wrong\n");\
        return HI_FAILURE;\
    }\
    if(GetHdmiDevice(HdmiId)->u32KernelCnt == 0 && GetHdmiDevice(HdmiId)->u32UserCnt == 0) \
    {   \
        HDMI_WARN("The hdmi device not open\n");\
        return HI_FAILURE;\
    } \
}while(0)


static HI_U32 Hdmi_Etablish2Format(HI_U32 u32EtablishCode)
{
	HI_U32 i = 0;
	HDMI_ESTAB_FORMAT_S *pstEstabTiming = HI_NULL;

	for (i=0;i<HDMI_ARRAY_SIZE(s_g_stFmtEstabTable);i++)
	{
		pstEstabTiming = (HDMI_ESTAB_FORMAT_S *)&s_g_stFmtEstabTable[i];
		if (   pstEstabTiming != HI_NULL
			&& pstEstabTiming->u32EtablishCode == u32EtablishCode)
		{
			return pstEstabTiming->u32Format;
		}
	}

	return HI_UNF_ENC_FMT_BUTT;
	
}

static HI_U32 Hdmi_Vic2Format(HI_U32 u32Vic)
{
	HI_U32 i = 0;
	HDMI_FMT_PARAM_S *pstFmtParam = HI_NULL;

	for (i=0;i<HDMI_ARRAY_SIZE(s_g_stFmtParamTable);i++)
	{
		pstFmtParam = (HDMI_FMT_PARAM_S *)&s_g_stFmtParamTable[i];
		if (   (pstFmtParam != HI_NULL)
			&& (pstFmtParam->u32Vic == u32Vic)	  )
		{
			return pstFmtParam->u32Format;
		}
	}

	return HI_UNF_ENC_FMT_BUTT;
	
}


static HI_U32 Hdmi_StdTiming2Format(HDMI_EDID_STD_TIMING_S *pstStdTiming)
{
	HI_U32 i = 0;
	HDMI_FMT_PARAM_S *pstFmtParam = HI_NULL;

	if (pstStdTiming == HI_NULL)
	{
		return HI_UNF_ENC_FMT_BUTT;
	}

	for (i=0;i<HDMI_ARRAY_SIZE(s_g_stFmtParamTable);i++)
	{
		pstFmtParam = (HDMI_FMT_PARAM_S *)&s_g_stFmtParamTable[i];
		if (   (pstFmtParam != HI_NULL) 
			&& (pstFmtParam->u32HorActive == pstStdTiming->u32HorActive)
			&& (pstFmtParam->u32VerActive == pstStdTiming->u32VerActive)
			&& (pstFmtParam->u32FieldRate >= pstStdTiming->u32RefreshRate)
			&& ((pstFmtParam->u32FieldRate-HDMI_FEILD_RATE_DEVIATION) <= pstStdTiming->u32RefreshRate)	)
		{
			return pstFmtParam->u32Format;
		}
	}

	return HI_UNF_ENC_FMT_BUTT;
	
}

static HI_U32 Hdmi_PreTiming2Format(HDMI_EDID_PRE_TIMING_S *pstPreTiming)
{
	HI_U32 i = 0;
	HDMI_FMT_PARAM_S *pstFmtParam = HI_NULL;

	if (pstPreTiming == HI_NULL)
	{
		return HI_UNF_ENC_FMT_BUTT;
	}

	for (i=0;i<HDMI_ARRAY_SIZE(s_g_stFmtParamTable);i++)
	{
		pstFmtParam = (HDMI_FMT_PARAM_S *)&s_g_stFmtParamTable[i];
		if (   (pstFmtParam != HI_NULL)
			&& (pstFmtParam->u32HorActive == pstPreTiming->u32HACT)
			&& (pstFmtParam->u32VerActive == pstPreTiming->u32VACT)
			&& (pstFmtParam->u32PixlFreq >= pstPreTiming->u32PixelClk)
			&& ((pstFmtParam->u32PixlFreq-HDMI_PIXL_FREQ_DEVIATION) <= pstPreTiming->u32PixelClk)  )
		{
			return pstFmtParam->u32Format;
		}
	}

	return HI_UNF_ENC_FMT_BUTT;
	
}


static HI_S32 Hdmi_CapabilityDrv2Ext(HI_UNF_EDID_BASE_INFO_S *pstExtCap,
									HDMI_SINK_CAPABILITY_S *pstDrvCap)
{
	HI_U32				i = 0;
	HI_UNF_ENC_FMT_E	enTmpEncFmt = HI_UNF_ENC_FMT_BUTT;

	if (HI_NULL==pstDrvCap || HI_NULL==pstExtCap)
	{
		HDMI_ERR("null pointer!\n");
		return HI_FAILURE;
	}

	pstExtCap->bSupportHdmi 	= pstDrvCap->bSupportHdmi;		  
	pstExtCap->enNativeFormat	= Hdmi_Vic2Format(pstDrvCap->u32NativeFormat);

	memset(pstExtCap->bSupportFormat,HI_FALSE,sizeof(pstExtCap->bSupportFormat));
	
	for (i=0;i<pstDrvCap->u32EstabNum;i++)
	{
		enTmpEncFmt = Hdmi_Etablish2Format(pstDrvCap->au32EstabTiming[i]);
		if (enTmpEncFmt != HI_UNF_ENC_FMT_BUTT)
		{
			pstExtCap->bSupportFormat[enTmpEncFmt] = HI_TRUE;
		}
	}

	for (i=0;i<HDMI_EDID_MAX_STDTIMNG_COUNT;i++)
	{
		enTmpEncFmt = Hdmi_StdTiming2Format(&pstDrvCap->stStdTiming[i]);
		if (enTmpEncFmt != HI_UNF_ENC_FMT_BUTT)
		{
			pstExtCap->bSupportFormat[enTmpEncFmt] = HI_TRUE;
		}
	}

	for (i=0;i<pstDrvCap->u32PerferTimingNum;i++)
	{
		enTmpEncFmt = Hdmi_PreTiming2Format(&pstDrvCap->stPerferTiming[i]);
		if (enTmpEncFmt != HI_UNF_ENC_FMT_BUTT)
		{
			pstExtCap->bSupportFormat[enTmpEncFmt] = HI_TRUE;
		}
	}

	for (i=0;i<pstDrvCap->u32SupportVICNum;i++)
	{
		enTmpEncFmt = Hdmi_Vic2Format(pstDrvCap->au32SupportFormat[i]);
		if (enTmpEncFmt != HI_UNF_ENC_FMT_BUTT)
		{
			pstExtCap->bSupportFormat[enTmpEncFmt] = HI_TRUE;
		}
	}

	memcpy(&pstExtCap->st3DInfo, &pstDrvCap->st3DInfo, sizeof(pstExtCap->st3DInfo));
	memcpy(&pstExtCap->stDeepColor, &pstDrvCap->stDeepColor, sizeof(pstExtCap->stDeepColor));		  
	memcpy(&pstExtCap->stColorMetry, &pstDrvCap->stColorMetry, sizeof(pstExtCap->stColorMetry));
	//pstExtCap->u8MDBit			= pstDrvCap->u8MDBit;
	memcpy(&pstExtCap->stColorSpace, &pstDrvCap->stColorSpace, sizeof(pstExtCap->stColorSpace));	  
	memcpy(&pstExtCap->stAudioInfo, &pstDrvCap->stAudioInfo, sizeof(pstExtCap->stAudioInfo));

	pstExtCap->u32AudioInfoNum	= pstDrvCap->u32AudioInfoNum;	  

	memcpy(&pstExtCap->bSupportAudioSpeaker, &pstDrvCap->bSupportAudioSpeaker, sizeof(pstExtCap->bSupportAudioSpeaker));

	pstExtCap->u8ExtBlockNum	= pstDrvCap->u8ExtBlockNum; 	  
	pstExtCap->u8Version		= pstDrvCap->u8Version; 		  
	pstExtCap->u8Revision		= pstDrvCap->u8Revision;		  

	memcpy(&pstExtCap->stMfrsInfo, &pstDrvCap->stMfrsInfo, sizeof(pstExtCap->stMfrsInfo));
	memcpy(&pstExtCap->stCECAddr, &pstDrvCap->stCECAddr, sizeof(pstExtCap->stCECAddr)); 		  

	pstExtCap->bSupportDVIDual	= pstDrvCap->bSupportDVIDual;	  
	pstExtCap->bSupportsAI		= pstDrvCap->bSupportsAI;			

	memcpy(&pstExtCap->stPerferTiming, &pstDrvCap->stPerferTiming[0], sizeof(pstExtCap->stPerferTiming));	
#if 0
	//pstExtCap->u32MaxTMDSClock			 = pstDrvCap->u32MaxTMDSClock;			 
	//pstExtCap->bILatencyFieldsPresent  = pstDrvCap->bILatencyFieldsPresent; 
	//pstExtCap->bLatencyFieldsPresent	 = pstDrvCap->bLatencyFieldsPresent;	
	//pstExtCap->bHDMIVideoPresent		 = pstDrvCap->bHDMIVideoPresent;		
	//pstExtCap->u8VideoLatency 		 = pstDrvCap->u8VideoLatency ;		
	//pstExtCap->u8AudioLatency 		 = pstDrvCap->u8AudioLatency;		
	//pstExtCap->u8InterlacedVideoLatency	 = pstDrvCap->u8InterlacedVideoLatency;
	//pstExtCap->u8InterlacedAudioLatency	 = pstDrvCap->u8InterlacedAudioLatency;

	//pstExtCap->bSupportHdmi2_0			 = pstDrvCap->bSupportHdmi_2_0; 	   
	//pstExtCap->bSupportHDCP2_2			 = pstDrvCap->bSupportHDCP_2_2; 	  
	//pstExtCap->bSupport3dOsdDisparity 	 = pstDrvCap->bSupport3dOsdDisparity; 
	//pstExtCap->bSupport3dDualView 		 = pstDrvCap->bSupport3dDualView;	  
	//pstExtCap->bSupport3dIndependentView = pstDrvCap->bSupport3dIndependentView;

	//memset(pstExtCap->bSupportY420Format, HI_FALSE, sizeof(pstExtCap->bSupportY420Format));
	for (i=0;i<pstDrvCap->u32SupportY420VICNum;i++)
	{
		enTmpEncFmt = Hdmi_Vic2Format(pstDrvCap->au32SupportY420Format[i]);
		if (enTmpEncFmt != HI_UNF_ENC_FMT_BUTT)
		{
			pstExtCap->bSupportY420Format[enTmpEncFmt] = HI_TRUE;
		}
	}

	memset(pstExtCap->bOnlySupportY420Format, HI_FALSE, sizeof(pstExtCap->bOnlySupportY420Format));
	for (i=0;i<pstDrvCap->u32OnlySupportY420VICNum;i++)
	{
		enTmpEncFmt = Hdmi_Vic2Format(pstDrvCap->au32OnlySupportY420Format[i]);
		if (enTmpEncFmt != HI_UNF_ENC_FMT_BUTT)
		{
			pstExtCap->bOnlySupportY420Format[enTmpEncFmt] = HI_TRUE;
		}
	}

	memcpy(&pstExtCap->stDeepColorY420 , &pstDrvCap->stDeepColorY420, sizeof(pstExtCap->stDeepColorY420));		
#endif
	return HI_SUCCESS;
}


HI_S32 HI_DRV_HDMI_Suspend(PM_BASEDEV_S *pdev, pm_message_t state)
{
    HI_S32          ret = HI_SUCCESS;  
    HDMI_DEVICE_ID_E enHdmi = HDMI_DEVICE_ID0;
    HDMI_PRINT_IN;
    while (enHdmi < HDMI_DEVICE_ID_BUTT)
    {
        CHECK_HDMI_OPEN(enHdmi);
        ret = HdmiExtIoctl(CMD_HDMI_STOP, &enHdmi);
        if (ret != HI_SUCCESS)
        {
    	    HDMI_ERR("hdmi stop  err!:0x%x\n",ret);
    	    return ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_STOP ok! \n");
        enHdmi++;
    }
    HDMI_PRINT_OUT;
    return ret;
}

HI_S32 HI_DRV_HDMI_Resume(PM_BASEDEV_S *pdev)
{
    HI_S32          ret = HI_SUCCESS;  
    HDMI_DEVICE_ID_E enHdmi = HDMI_DEVICE_ID0;
    HDMI_PRINT_IN;
    while (enHdmi < HDMI_DEVICE_ID_BUTT)
    {
        DRV_HDMI_APP_ATTR_S     stHDMIAttr;
        CHECK_HDMI_OPEN(enHdmi);

        ret = HdmiExtIoctl(CMD_HDMI_OPEN, &enHdmi);
        if (ret != HI_SUCCESS)
        {
    	    HDMI_ERR("hdmi open  err!:0x%x\n",ret);
    	    return ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_OPEN ok! \n");

        memset(&stHDMIAttr, 0, sizeof(stHDMIAttr));
        stHDMIAttr.enHdmiID = enHdmi;
        ret = HdmiExtIoctl(CMD_HDMI_GET_ATTR, &stHDMIAttr);
        if (ret != HI_SUCCESS)
        {
    	    HDMI_ERR("Get hdmi attr err!\n");
    	    return ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_GET_ATTR ok! \n");

        ret = HdmiExtIoctl(CMD_HDMI_SET_ATTR, &stHDMIAttr);
        if (ret != HI_SUCCESS)
        {
    	    HDMI_ERR("set attr err!:0x%x\n", ret);
            return ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_SET_ATTR ok! \n");

        ret = HdmiExtIoctl(CMD_HDMI_START, &enHdmi);
        if (ret != HI_SUCCESS)
        {
    	    HDMI_ERR("hdmi stop  err!:0x%x\n",ret);
    	    return ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_START ok! \n");
        enHdmi++;
    }
    HI_PRINT("HDMI Resume OK\n");
    HDMI_PRINT_OUT;
    return ret;
}

HI_S32 HI_DRV_HDMI_SoftResume(HDMI_VIDEO_ATTR_S *pstVideoAttr)
{
    HI_S32   s32Ret = HI_FAILURE;
    HDMI_DEVICE_ID_E enHdmi = HDMI_DEVICE_ID0;
    DRV_HDMI_VO_ATTR_S  stDrvVoAttr;
    HDMI_PRINT_IN;
    while (enHdmi < HDMI_DEVICE_ID_BUTT)
    {
        CHECK_HDMI_OPEN(enHdmi);

        s32Ret = HdmiExtIoctl(CMD_HDMI_OPEN, &enHdmi);
        if (s32Ret != HI_SUCCESS)
        {
    	    HDMI_ERR("hdmi open  err!:0x%x\n",s32Ret);
    	    return s32Ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_OPEN ok! \n");

        Disp2HdmiVoAttr(GetHdmiDevice(enHdmi),&stDrvVoAttr.stVOAttr, pstVideoAttr);
        stDrvVoAttr.enHdmiID = enHdmi;
        s32Ret = HdmiExtIoctl(CMD_HDMI_SET_VO_ATTR, &stDrvVoAttr);
        if (s32Ret != HI_SUCCESS)
        {
    	    HDMI_ERR("set vo attr err!:0x%x\n", s32Ret);
    	    return s32Ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_SET_VO_ATTR ok! \n");

        s32Ret = HdmiExtIoctl(CMD_HDMI_START, &enHdmi);
        if (s32Ret != HI_SUCCESS)
        {
    	    HDMI_ERR("hdmi stop  err!:0x%x\n",s32Ret);
    	    return s32Ret;
        }
        HDMI_INFO("HdmiExtIoctl CMD_HDMI_START ok! \n");        
        enHdmi++;
    }
    HI_PRINT("HDMI SoftResume OK\n");
    HDMI_PRINT_OUT;
    return s32Ret;
}
#endif

HI_S32 HI_DRV_HDMI_Init(HI_VOID)
{ 
#if defined(CONFIG_HDMI_STB_SDK)
    return HI_HDMI_MODULE_Register((HI_U32)HI_ID_HDMI, "HI_HDMI", (HI_VOID *)&s_stHdmiExportFuncs);
#elif defined(CONFIG_HDMI_BVT_SDK)
    return HI_SUCCESS;
#endif	
}

HI_VOID HI_DRV_HDMI_Deinit(HI_VOID)
{
#if defined(CONFIG_HDMI_STB_SDK)
    HI_HDMI_MODULE_UnRegister((HI_U32)HI_ID_HDMI);
#elif defined(CONFIG_HDMI_BVT_SDK)
    return ;
#endif	
}

#if defined(CONFIG_HDMI_STB_SDK)
HI_S32 HI_DRV_HDMI_Open(HI_UNF_HDMI_ID_E enHdmi)
{

    HI_S32   s32Ret = HI_FAILURE;
    HDMI_PRINT_IN;
    s32Ret = HdmiExtIoctl(CMD_HDMI_OPEN, &enHdmi);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("open hdmi err!:0x%x\n", s32Ret);
	    return s32Ret;
    }
    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}

HI_S32 HI_DRV_HDMI_Close(HI_UNF_HDMI_ID_E enHdmi)
{
    HI_S32   s32Ret = HI_FAILURE;
    HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);

    s32Ret = HdmiExtIoctl(CMD_HDMI_STOP, &enHdmi);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("stop hdmi err!:0x%x\n", s32Ret);
	    return s32Ret;
    }
    
    s32Ret = HdmiExtIoctl(CMD_HDMI_CLOSE, &enHdmi);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("close hdmi err!:0x%x\n", s32Ret);
	    return s32Ret;
    }
    HDMI_PRINT_OUT;
    return HI_SUCCESS;
}
#endif

static HI_S32 HdmiExtIoctl(unsigned int cmd, void *argp)
{
    HI_S32   s32Ret = HI_FAILURE;

    s32Ret = DRV_HDMI_CmdProcess(cmd, argp, HI_FALSE);

    return s32Ret;
}

static HI_VOID HdmiHotPlugProcess(HDMI_DEVICE_ID_E enHdmiID)
{
    HI_S32          ret = HI_SUCCESS;
    HDMI_SINK_CAPABILITY_S *pstSinkCap;
    DRV_HDMI_APP_ATTR_S  stHDMIAttr;
    HDMI_APP_ATTR_S     *pstAppAttr;
    DRV_HDMI_STATUS_S   stDrvHdmiStatus;
    HDMI_EDID_STATUS_S  stStatus;

    HDMI_INFO("\n---HDMI kernel event(no UserCallBack): HOTPLUG. --- \n");

    DRV_HDMI_DefaultActionSet(GetHdmiDevice(enHdmiID), HDMI_DEFAULT_ACTION_HDMI);
    
    ret = DRV_HDMI_EdidCapabilityGet(&(GetHdmiDevice(enHdmiID)->stEdidInfo), &pstSinkCap);
    if (ret != HDMI_EDID_DATA_VALIDSINK)
    {
        HDMI_WARN("Get sink capability err!\n");
	    //return ;
    }

    memset(&stDrvHdmiStatus,0,sizeof(HDMI_STATUS_S));
    stDrvHdmiStatus.enHdmiID = enHdmiID;
    ret = HdmiExtIoctl(CMD_HDMI_GET_STATUS, &stDrvHdmiStatus);
    if (ret != HI_SUCCESS)
    {
        HDMI_ERR("Get HDMI Status err!\n");
	    return ;
    }

    if (HI_FALSE == stDrvHdmiStatus.stHdmiStatus.bConnected)
    {
       HDMI_ERR("No Connect!\n");
       return;
    }

    HDMI_INFO("Connect !\n");


    memset(&stHDMIAttr, 0, sizeof(stHDMIAttr));
    stHDMIAttr.enHdmiID = enHdmiID;
    ret = HdmiExtIoctl(CMD_HDMI_GET_ATTR, &stHDMIAttr);
    if (ret != HI_SUCCESS)
    {
	    HDMI_ERR("Get hdmi attr err!\n");
	    return ;
    }
    HDMI_INFO("HdmiExtIoctl CMD_HDMI_GET_ATTR ok! \n");


    pstAppAttr = &stHDMIAttr.stAPPAttr;
    DRV_HDMI_EdidStatusGet(&(GetHdmiDevice(enHdmiID)->stEdidInfo), &stStatus);
    if(stStatus.bCapValid)
    {
	#if defined(CONFIG_HDMI_STB_SDK)
        pstAppAttr->enOutColorSpace = HI_UNF_HDMI_VIDEO_MODE_YCBCR444;
	#elif defined(CONFIG_HDMI_BVT_SDK)
        pstAppAttr->enOutColorSpace = HDMI_COLORSPACE_YCbCr444;
	#endif	
        if(HI_TRUE == pstSinkCap->bSupportHdmi)
        {
            pstAppAttr->bEnableHdmi = HI_TRUE;
            if(HI_TRUE != pstSinkCap->stColorSpace.bYCbCr444)
            {
            #if defined(CONFIG_HDMI_STB_SDK)
		        pstAppAttr->enOutColorSpace = HI_UNF_HDMI_VIDEO_MODE_RGB444;
            #elif defined(CONFIG_HDMI_BVT_SDK)
		        pstAppAttr->enOutColorSpace = HDMI_COLORSPACE_RGB;
            #endif	
            }
        }
        else
        {
		#if defined(CONFIG_HDMI_STB_SDK)
	        pstAppAttr->enOutColorSpace = HI_UNF_HDMI_VIDEO_MODE_RGB444;
		#elif defined(CONFIG_HDMI_BVT_SDK)
	        pstAppAttr->enOutColorSpace = HDMI_COLORSPACE_RGB;
		#endif	
            //read real edid ok && sink not support hdmi,then we run in dvi mode
            pstAppAttr->bEnableHdmi = HI_FALSE;
        }
    }
    else
    {
        if(DRV_HDMI_DefaultActionGet(GetHdmiDevice(enHdmiID)) == HDMI_DEFAULT_ACTION_HDMI)
        {
            pstAppAttr->bEnableHdmi = HI_TRUE;
        }
        else
        {
            pstAppAttr->bEnableHdmi = HI_FALSE;
        }
    }

    if(HI_TRUE == pstAppAttr->bEnableHdmi)
    {
        pstAppAttr->bEnableAudio = HI_TRUE;
        pstAppAttr->bEnableVideo = HI_TRUE;
        pstAppAttr->bEnableAudInfoFrame = HI_TRUE;
        pstAppAttr->bEnableAviInfoFrame = HI_TRUE;
    }
    else
    {
        pstAppAttr->bEnableAudio = HI_FALSE;
        pstAppAttr->bEnableVideo = HI_TRUE;
        pstAppAttr->bEnableAudInfoFrame = HI_FALSE;
        pstAppAttr->bEnableAviInfoFrame = HI_FALSE;
	#if defined(CONFIG_HDMI_STB_SDK)
        pstAppAttr->enOutColorSpace = HI_UNF_HDMI_VIDEO_MODE_RGB444;
	#elif defined(CONFIG_HDMI_BVT_SDK)
        pstAppAttr->enOutColorSpace = HDMI_COLORSPACE_RGB;
	#endif	
    }
    pstAppAttr->bHDCPEnable = HI_FALSE;
    ret = HdmiExtIoctl(CMD_HDMI_SET_ATTR, &stHDMIAttr);
    if (ret != HI_SUCCESS)
    {
	    HDMI_ERR("set attr err!:0x%x\n", ret);
    }
    HDMI_INFO("HdmiExtIoctl CMD_HDMI_SET_ATTR ok! \n");

    ret = HdmiExtIoctl(CMD_HDMI_START, &enHdmiID);
    if (ret != HI_SUCCESS)
    {
	    HDMI_ERR("hdmi start  err!:0x%x\n",ret);
	    return ;
    }
 
    HDMI_INFO("HdmiExtIoctl CMD_HDMI_START ok! \n");

    return;
}

static HI_VOID HdmiHotUnPlugProcess(HDMI_DEVICE_ID_E enHdmiID)
{
    HI_S32          ret = HI_SUCCESS;
    
    ret = HdmiExtIoctl(CMD_HDMI_STOP, &enHdmiID);
    if (ret != HI_SUCCESS)
    {
	    HDMI_ERR("hdmi stop  err!:0x%x\n",ret);
	    return ;
    }
    HDMI_INFO("HdmiExtIoctl CMD_HDMI_STOP ok! \n");
    return;
}

HI_S32 HI_DRV_HDMI_KernelEventCallback(HI_VOID* data, HDMI_EVENT_E enEvent)
{
    HDMI_DEVICE_ID_E enHdmiID = *(HDMI_DEVICE_ID_E*)data;
	HI_U32  u32NeedHotPlugProcess = HI_TRUE;

	#if defined(CONFIG_HDMI_BVT_SDK)
	HDMI_DEVICE_S *pstHdmiDev = GetHdmiDevice(enHdmiID); 
    u32NeedHotPlugProcess = (pstHdmiDev->enRunState & HDMI_RUN_STATE_START) || \
	                      (pstHdmiDev->enRunState & HDMI_RUN_STATE_STOP);
	#endif
	
	if (u32NeedHotPlugProcess)
    {
        if (enEvent == HDMI_EVENT_HOTPLUG)
	    {
	        HdmiHotPlugProcess(enHdmiID);
	    }
	    else if (enEvent == HDMI_EVENT_HOTUNPLUG)
	    {
	        HdmiHotUnPlugProcess(enHdmiID);
	    }
    }
    return HI_SUCCESS;
}

#if defined(CONFIG_HDMI_STB_SDK)
HI_S32 HI_DRV_HDMI_GetPlayStatus(HI_UNF_HDMI_ID_E enHdmi, HI_U32 *pu32Status)
{
    HI_S32   s32Ret = HI_FAILURE;
    DRV_HDMI_PLAYSTATUS_S stPlayStatus;
    //HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);
    stPlayStatus.enHdmiID = enHdmi;
    s32Ret = HdmiExtIoctl(CMD_HDMI_GET_HDMI_PLAYSTATUS, &stPlayStatus);
    if (s32Ret == HI_SUCCESS)
    {
        *pu32Status = stPlayStatus.u32Status;
    }
    //HDMI_PRINT_OUT;
    return s32Ret;
}


HI_S32 HI_DRV_HDMI_GetSinkCapability(HI_UNF_HDMI_ID_E enHdmi, HI_UNF_EDID_BASE_INFO_S *pstSinkCap)
{
	HI_S32	 s32Ret = HI_FAILURE;
	DRV_HDMI_SINK_CAPABILITY_S *pstDrvCap = HI_NULL;
	
	//HDMI_PRINT_IN;
	CHECK_HDMI_OPEN(enHdmi);

	pstDrvCap = HDMI_KMALLOC(sizeof(DRV_HDMI_SINK_CAPABILITY_S));
	if (pstDrvCap == HI_NULL)
	{
		return HI_FAILURE;
	}
	
	pstDrvCap->enHdmiID = enHdmi;
	s32Ret = HdmiExtIoctl(CMD_HDMI_GET_SINK_CAPABILITY,pstDrvCap);

	if (s32Ret == HI_SUCCESS)
	{
		s32Ret = Hdmi_CapabilityDrv2Ext(pstSinkCap,&pstDrvCap->stCap);
	}
	
	if (pstDrvCap)
	{
		HDMI_KFREE(pstDrvCap);
	}
	//HDMI_PRINT_OUT;
	return s32Ret;
}


HI_S32 HI_DRV_HDMI_GetAudioCapability(HI_UNF_HDMI_ID_E enHdmi, HI_DRV_HDMI_AUDIO_CAPABILITY_S *pstAudCap)
{
    HI_S32	 s32Ret = HI_FAILURE;
    DRV_HDMI_AUDIO_CAPABILITY_S stTmpAudioCap;   

	CHECK_HDMI_OPEN(enHdmi);

	stTmpAudioCap.enHdmiID = enHdmi;
    s32Ret = HdmiExtIoctl(CMD_HDMI_GET_AUDIO_CAPABILITY,&stTmpAudioCap);
	if (s32Ret == HI_SUCCESS)
	{
        HDMI_MEMCPY(pstAudCap,&stTmpAudioCap.stAudioCap,sizeof(HDMI_AUDIO_CAPABILITY_S));
	}
	
	return s32Ret;
}


HI_S32 HI_DRV_HDMI_SetAudioMute(HI_UNF_HDMI_ID_E enHdmi)
{
    //HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);
    DRV_HDMI_AudioOutputEnableSet(GetHdmiDevice(enHdmi), HI_FALSE);
    //HDMI_PRINT_OUT;
    return HI_SUCCESS;
}

HI_S32 HI_DRV_HDMI_SetAudioUnMute(HI_UNF_HDMI_ID_E enHdmi)
{
    //HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);
    DRV_HDMI_AudioOutputEnableSet(GetHdmiDevice(enHdmi), HI_TRUE);
    //HDMI_PRINT_OUT;
    return HI_SUCCESS;
}

HI_S32 HI_DRV_HDMI_GetAOAttr(HI_UNF_HDMI_ID_E enHdmi, HDMI_AUDIO_ATTR_S *pstAudioAttr)
{
    HI_S32   s32Ret = HI_FAILURE;
    DRV_HDMI_AO_ATTR_S stDrvAOAttr;
    // HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);
    stDrvAOAttr.enHdmiID = enHdmi;
    s32Ret = HdmiExtIoctl(CMD_HDMI_GET_AO_ATTR, &stDrvAOAttr);
    if (s32Ret == HI_SUCCESS)
    {   
        pstAudioAttr->bIsMultiChannel  = stDrvAOAttr.stAOAttr.enChanels > HDMI_AUDIO_FORMAT_2CH ? HI_TRUE : HI_FALSE;
        pstAudioAttr->enAudioCode      = stDrvAOAttr.stAOAttr.enAudioCode;
        pstAudioAttr->enBitDepth       = stDrvAOAttr.stAOAttr.enSampleDepth;
        pstAudioAttr->enSampleRate     = stDrvAOAttr.stAOAttr.enSampleFs;
        pstAudioAttr->enSoundIntf      = stDrvAOAttr.stAOAttr.enSoundIntf;
        pstAudioAttr->u32Channels      = stDrvAOAttr.stAOAttr.enChanels;
        pstAudioAttr->u8DownSampleParm = stDrvAOAttr.stAOAttr.bDownSample;
    }
    //HDMI_PRINT_OUT;
    return s32Ret;
}


HI_S32 HI_DRV_HDMI_SetAOAttr(HI_UNF_HDMI_ID_E enHdmi, HDMI_AUDIO_ATTR_S *pstAudioAttr)
{
    HI_S32   s32Ret = HI_FAILURE;
    DRV_HDMI_AO_ATTR_S stDrvAoAttr;
    //HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);

    memset(&stDrvAoAttr, 0, sizeof(DRV_HDMI_AO_ATTR_S));
    stDrvAoAttr.enHdmiID = enHdmi;
    stDrvAoAttr.stAOAttr.enAudioCode   = pstAudioAttr->enAudioCode;
    stDrvAoAttr.stAOAttr.enSampleDepth = pstAudioAttr->enBitDepth;
    stDrvAoAttr.stAOAttr.enSampleFs    = pstAudioAttr->enSampleRate;
    stDrvAoAttr.stAOAttr.enSoundIntf   = pstAudioAttr->enSoundIntf;
    stDrvAoAttr.stAOAttr.enChanels     = pstAudioAttr->u32Channels;
    stDrvAoAttr.stAOAttr.bDownSample   = pstAudioAttr->u8DownSampleParm;
    HDMI_INFO("SoundIntf:%d, AudioCode:%d, SampleRate:%d, Channels:%d, BitDepth:%d\n",pstAudioAttr->enSoundIntf,pstAudioAttr->enAudioCode,pstAudioAttr->enSampleRate,pstAudioAttr->u32Channels,pstAudioAttr->enBitDepth);
    s32Ret = HdmiExtIoctl(CMD_HDMI_SET_AO_ATTR, &stDrvAoAttr);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("set ao attr err!:0x%x\n",s32Ret);
	    return s32Ret;
    }
    //HDMI_PRINT_OUT;
    return s32Ret;
}

static HI_S32 Disp2HdmiVoAttr(HDMI_DEVICE_S* pstHdmiDev, HDMI_VO_ATTR_S *pstDestAttr, HDMI_VIDEO_ATTR_S *pstSrcAttr)
{
    if (pstSrcAttr == HI_NULL || pstDestAttr == HI_NULL)
    {
        return HI_FAILURE;
    }
    /*
    pstDestAttr->enColorimetry   = pstSrcAttr->enColorimetry;
    if (pstSrcAttr->enColorimetry > HDMI_COLORIMETRY_EXTENDED)
    {
        pstDestAttr->enColorimetry = HDMI_COLORIMETRY__EXTENDED;
        pstDestAttr->enExtendedColorimetry = pstSrcAttr->enColorimetry - HDMI_COLORIMETRY_EXTENDED - 1;
    }
    
    pstDestAttr->enPictureAspect   = pstSrcAttr->enPictureAspect;
    pstDestAttr->enActiveAspect    = pstSrcAttr->enActiveAspect;
    pstDestAttr->enInColorSpace    = pstSrcAttr->enInColorSpace;   
    pstDestAttr->enPictureScaling  = pstSrcAttr->enPictureScaling;   
    pstDestAttr->enRGBQuantization = pstSrcAttr->enRGBQuantization;
    pstDestAttr->enYCCQuantization = pstSrcAttr->enYCCQuantization;   
    */
    pstDestAttr->enHvSyncPol       = pstSrcAttr->enHvSyncPol;
    pstDestAttr->enInBitDepth      = pstSrcAttr->enInBitDepth;
    pstDestAttr->enStereoMode      = pstSrcAttr->enStereoMode;
    pstDestAttr->u32DispFmt        = (HI_U32)pstSrcAttr->enVideoFmt;
    pstDestAttr->enVideoTiming     = DispFmt2HdmiTiming(pstHdmiDev, pstSrcAttr->enVideoFmt);
    pstDestAttr->u32ClkFs          = pstSrcAttr->u32ClkFs;
    pstDestAttr->u32PixelRepeat    = pstSrcAttr->u32PixelRepeat;
    pstDestAttr->enActiveAspect    = HDMI_ACTIVE_ASPECT_PICTURE;
    
    switch (pstSrcAttr->u32ColorSpace)
    {
        case HI_DRV_CS_BT601_YUV_LIMITED:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_YCbCr444;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
            pstDestAttr->enYCCQuantization = HDMI_YCC_QUANTIZATION_RANGE_LIMITED;   
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_601;
            break;
        case HI_DRV_CS_BT601_YUV_FULL:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_YCbCr444;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
            pstDestAttr->enYCCQuantization = HDMI_YCC_QUANTIZATION_RANGE_FULL;   
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_601;
            break;
        case HI_DRV_CS_BT601_RGB_LIMITED:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_RGB;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_LIMITED;  
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_601;
            break;
        case HI_DRV_CS_BT601_RGB_FULL:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_RGB;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_FULL;  
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_601;
            break;
        case HI_DRV_CS_BT709_YUV_LIMITED:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_YCbCr444;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
            pstDestAttr->enYCCQuantization = HDMI_YCC_QUANTIZATION_RANGE_LIMITED;   
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_709;
            break;
        case HI_DRV_CS_BT709_YUV_FULL:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_YCbCr444;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_DEFAULT;
            pstDestAttr->enYCCQuantization = HDMI_YCC_QUANTIZATION_RANGE_FULL;   
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_709;
            break;
        case HI_DRV_CS_BT709_RGB_LIMITED:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_RGB;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_LIMITED;  
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_709;
            break;
        case HI_DRV_CS_BT709_RGB_FULL:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_RGB;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_FULL;  
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_709;
            break;
        default:
            pstDestAttr->enInColorSpace    = HDMI_COLORSPACE_RGB;   
            pstDestAttr->enRGBQuantization = HDMI_QUANTIZATION_RANGE_LIMITED;
            pstDestAttr->enColorimetry     = HDMI_COLORIMETRY__ITU_601;
            break;
    }
    
    if (pstDestAttr->enVideoTiming  > HDMI_VIDEO_TIMING_720X576P_50000)
    {
        pstDestAttr->enPictureAspect = HDMI_PICTURE_ASPECT_16_9;
    }
    else
    {
        pstDestAttr->enPictureAspect = HDMI_PICTURE_ASPECT_4_3;
    }
    return HI_SUCCESS;
}

HI_S32 HI_DRV_HDMI_PreFormat(HI_UNF_HDMI_ID_E enHdmi)
{
    HI_S32   s32Ret = HI_FAILURE;
    HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);

    s32Ret = HdmiExtIoctl(CMD_HDMI_STOP, &enHdmi);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("stop hdmi err!:0x%x\n",s32Ret);
	    return s32Ret;
    }
    HDMI_PRINT_OUT;
    return s32Ret;   
}

HI_S32 HI_DRV_HDMI_SetFormat(HI_UNF_HDMI_ID_E enHdmi, HDMI_VIDEO_ATTR_S *pstVideoAttr)
{
    HI_S32   s32Ret = HI_FAILURE;
    DRV_HDMI_VO_ATTR_S stDrvVoAttr;
    HDMI_AUDIO_ATTR_S  stAudioAttr;
    HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);
    memset(&stDrvVoAttr, 0, sizeof(DRV_HDMI_VO_ATTR_S));
    Disp2HdmiVoAttr(GetHdmiDevice(enHdmi),&stDrvVoAttr.stVOAttr, pstVideoAttr);
    
    HDMI_INFO("VideoFmt:%d, StereoMode:%d, ClkFs:%d, PixelRepeat:%d, ColorSpace:%d, HvSyncPol:%d\n",pstVideoAttr->enVideoFmt,pstVideoAttr->enStereoMode,
        pstVideoAttr->u32ClkFs,pstVideoAttr->u32PixelRepeat,pstVideoAttr->u32ColorSpace,pstVideoAttr->enHvSyncPol);

    stDrvVoAttr.enHdmiID = enHdmi;
    s32Ret = HdmiExtIoctl(CMD_HDMI_SET_VO_ATTR, &stDrvVoAttr);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("set vo attr err!:0x%x\n",s32Ret);
	    return s32Ret;
    }
    s32Ret = HI_DRV_HDMI_GetAOAttr(enHdmi, &stAudioAttr);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("HI_DRV_HDMI_GetAOAttr fail\n");
	    return s32Ret;
    }
    s32Ret = HI_DRV_HDMI_SetAOAttr(enHdmi, &stAudioAttr);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("HI_DRV_HDMI_SetAOAttr fail\n");
	    return s32Ret;
    }
    s32Ret = HdmiExtIoctl(CMD_HDMI_START, &enHdmi);
    HDMI_PRINT_OUT;
    return s32Ret; 
}

HI_S32 HI_DRV_HDMI_Detach(HI_UNF_HDMI_ID_E enHdmi)
{
    HI_S32   s32Ret = HI_FAILURE;
    HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);
    DRV_HDMI_ThreadStateSet(GetHdmiDevice(enHdmi), HDMI_THREAD_STATE_STOP);
    s32Ret = HI_DRV_HDMI_PreFormat(enHdmi);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("HI_DRV_HDMI_PreFormat fail\n");
	    return s32Ret;
    }
    HDMI_PRINT_OUT;
    return s32Ret; 
}

HI_S32 HI_DRV_HDMI_Attach(HI_UNF_HDMI_ID_E enHdmi, HDMI_VIDEO_ATTR_S *pstVideoAttr)
{
    HI_S32   s32Ret = HI_FAILURE;
    HDMI_PRINT_IN;
    CHECK_HDMI_OPEN(enHdmi);
    s32Ret = HI_DRV_HDMI_SetFormat(enHdmi, pstVideoAttr);
    if (s32Ret != HI_SUCCESS)
    {
	    HDMI_ERR("HI_DRV_HDMI_SetFormat fail\n");
	    return s32Ret;
    }
    DRV_HDMI_ThreadStateSet(GetHdmiDevice(enHdmi), HDMI_THREAD_STATE_STATR);
    HDMI_PRINT_OUT;
    return s32Ret;
}
#endif
