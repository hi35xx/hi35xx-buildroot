#include <common.h>
#include <command.h>
#include <version.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include <asm/sizes.h>
#include <config.h>

#include "vou_drv.h"
#include "vou_hal.h"

static HI_U32 g_au32BgColor[4] = {0x00FF, 0xFF00, 0xFF00, 0xFF00};//{0xFF00, 0xFF00, 0xFF00, 0xFF00};	 /* default value is GREEN */
//static HI_BOOL g_abHdStart[2] = {0};
//static HI_U32 g_au32HdType[2] = {0};
static HI_BOOL g_bInited = HI_FALSE;
static RECT_S stMaxRect = {0};

#define VOU_ALAIN(u32Val, u32Align)	((u32Val +u32Align -1)&(~(u32Align-1)))

static inline HI_U32 Rgb2Yuv(HI_U32 u32Rgb)
{
    HI_U8 r,g,b;
    HI_U8 y,u,v;

    r = RGB_RED(u32Rgb);
    g = RGB_GREEN(u32Rgb);
    b = RGB_BLUE(u32Rgb);

    /* Y */
    y = (HI_U8)((r*66+g*129+b*25)/256 + 16);

    /* Cb */
    u = (HI_U8)(((b*112-r*38)-g*74)/256 + 128);

    /* Cr */
    v = (HI_U8)(((r*112-g*94)-b*18)/256 + 128);

    return YUV(y,u,v);
}

static inline unsigned int GfxConvertLayer(unsigned int layer)
{
    unsigned int uGfxLayer;

    /* hard cursor is not supported. */
    switch (layer)
    {
        case 0:
            uGfxLayer = VOU_LAYER_G4;
            break;
        case 1:
            uGfxLayer = VOU_LAYER_G1;
            break;
        default :
            uGfxLayer = VOU_LAYER_G4;
            break;
        
    }
    return uGfxLayer;
}

static inline VOU_LAYER_E VideoLayerConvert(unsigned int layer)
{
    VOU_LAYER_E enLayer;

    /* hard cursor is not supported. */
    switch (layer)
    {
        case 0:
            enLayer = VOU_LAYER_VHD1;
            break;
        case 2:
            enLayer = VOU_LAYER_VSD0;
            break;
        default :
            enLayer = VOU_LAYER_VHD1;
            break;
        
    }
    return enLayer;
}

int set_vobg(unsigned int dev, unsigned int rgb)
{
	if (dev < 3) 
	{
		g_au32BgColor[dev] = rgb;
	}
	return 0;
}

HI_VOID VouSetDispMaxSize(VO_DEV VoDev, VO_INTF_SYNC_E enVoOutMode)
{
    HI_U32 u32MaxWidth, u32MaxHeight;

    switch (enVoOutMode)
    {
        case VO_OUTPUT_PAL :
        case VO_OUTPUT_576P50 :
            u32MaxWidth = 720;
            u32MaxHeight = 576;
            break;
        case VO_OUTPUT_NTSC :
        case VO_OUTPUT_480P60:
            u32MaxWidth = 720;
            u32MaxHeight = 480;
            break;
        case VO_OUTPUT_960H_PAL:
            u32MaxWidth = 960;
            u32MaxHeight = 576;
            break;
        case VO_OUTPUT_960H_NTSC:
            u32MaxWidth = 960;
            u32MaxHeight = 480;
            break;
        case VO_OUTPUT_720P60 :
        case VO_OUTPUT_720P50 :
            u32MaxWidth = 1280;
            u32MaxHeight = 720;
            break;
        case VO_OUTPUT_1080I50 :
        case VO_OUTPUT_1080I60 :
        case VO_OUTPUT_1080P24 :
        case VO_OUTPUT_1080P25 :
        case VO_OUTPUT_1080P30 :            
        case VO_OUTPUT_1080P50 :
        case VO_OUTPUT_1080P60 : 
            u32MaxWidth = 1920;
            u32MaxHeight = 1080;
            break;
        case VO_OUTPUT_800x600_60 :
            u32MaxWidth = 800;
            u32MaxHeight = 600;
            break;
        case VO_OUTPUT_1024x768_60 :
            u32MaxWidth = 1024;
            u32MaxHeight = 768;
            break;
        case VO_OUTPUT_1280x1024_60 :
            u32MaxWidth = 1280;
            u32MaxHeight = 1024;
            break;
        case VO_OUTPUT_1366x768_60 :
            u32MaxWidth = 1366;
            u32MaxHeight = 768;
            break;
        case VO_OUTPUT_1440x900_60 :
            u32MaxWidth = 1440;
            u32MaxHeight = 900;
            break;
        case VO_OUTPUT_1280x800_60 :
            u32MaxWidth = 1280;
            u32MaxHeight = 800;
            break;
        case VO_OUTPUT_1600x1200_60 :
            u32MaxWidth = 1600;
            u32MaxHeight = 1200;
            break;
        case VO_OUTPUT_1680x1050_60 :
            u32MaxWidth = 1680;
            u32MaxHeight = 1050;
            break;
        case VO_OUTPUT_1920x1200_60 :
            u32MaxWidth = 1920;
            u32MaxHeight = 1200;
            break;            
        case VO_OUTPUT_640x480_60 :
            u32MaxWidth = 640;
            u32MaxHeight = 480;
            break;
        case VO_OUTPUT_1920x2160_30 :
            u32MaxWidth = 1920;
            u32MaxHeight = 2160;
            break;
        case VO_OUTPUT_2560x1440_30 :
            u32MaxWidth = 2560;
            u32MaxHeight = 1440;
            break;    
        case VO_OUTPUT_2560x1600_60 :
            u32MaxWidth = 2560;
            u32MaxHeight = 1600;
            break;
        case VO_OUTPUT_3840x2160_30 :
        case VO_OUTPUT_3840x2160_60 :
            u32MaxWidth = 3840;
            u32MaxHeight = 2160;
            break;            
        default:
            u32MaxWidth = 1920;
            u32MaxHeight = 1080;
    }

    stMaxRect.u32Width = u32MaxWidth;
    stMaxRect.u32Height = u32MaxHeight;
}


int start_vo(unsigned int dev, unsigned int type, unsigned int sync)
{
    int i;
    
    if (g_bInited == HI_FALSE)
    {    
        SYS_HAL_SelVoVgaPinConifg();
        SYS_HAL_SelVoHdmiPinConifg();
        // asic todo
        SYS_HAL_DDRConifg();
        SYS_HAL_VouBusResetSel(HI_FALSE);
        SYS_HAL_VouBusClkEn(HI_TRUE);
        SYS_HAL_SelVoClk(0x0);
        SYS_HAL_VouWorkClkEn(HI_TRUE);
        VOU_DRV_BoardInit();
        HAL_SYS_Control();
        
        g_bInited = HI_TRUE;
    }
    /*打开HD的时钟门控*/
    for (i = 0; i < 2; i++)
    {
        SYS_HAL_VouDevClkEn(i, HI_TRUE);
    }
    if (type & VO_INTF_HDMI)
    {
        SYS_HAL_HdmiResetSel(HI_FALSE);
    }
    if (dev == VOU_DEV_DSD0)
    {
        SYS_HAL_SetVouSdClkDiv(0x0);
        SYS_HAL_VouDevClkEn(dev, HI_TRUE);
    }
    
    VouSetDispMaxSize(dev, sync);
    VOU_DRV_SetDevIntfType(dev, type);
    VOU_DRV_SetDevOutSync(dev, sync);
    VOU_DRV_SetDevBkGrd(dev, Rgb2Yuv(g_au32BgColor[dev]));
	
    VOU_DRV_SetDevClk(dev);
    VOU_DRV_Open(dev);

    return 0;
}

int stop_vo(unsigned int dev)
{
    
    VOU_DRV_Close(dev);
    SYS_HAL_VouDevClkEn(dev, HI_FALSE);

    //VOU_DRV_IntDisableAll();
    //VOU_DRV_IntClear(VOU_INTREPORT_ALL);

    return 0;
}

int start_gx(unsigned int layer, unsigned addr, unsigned int strd,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    RECT_S stDispRect = {x, y, w, h};
    VOU_LAYER_E enLayer;

    enLayer = GfxConvertLayer(layer);

    HAL_GRAPHIC_SetGfxExt(enLayer, HAL_GFX_BITEXTEND_3RD);
    HAL_GRAPHIC_SetGfxPalpha(enLayer, HI_TRUE, HI_TRUE, 0xff, 0xff);
    HAL_LAYER_SetLayerGalpha(enLayer, 0xff);
    HAL_GRAPHIC_SetGfxPreMult(enLayer, HI_FALSE);
    VOU_DRV_GraphicsSetCscCoef(enLayer);
    HAL_LAYER_SetCscEn(enLayer, HI_TRUE);

    HAL_GRAPHIC_SetGfxAddr(enLayer, addr);
    HAL_GRAPHIC_SetGfxStride(enLayer, strd>>4);
    HAL_LAYER_SetLayerInRect(enLayer, &stDispRect);

    HAL_LAYER_SetLayerOutRect(enLayer, &stDispRect);
    
    HAL_VIDEO_SetLayerDispRect(enLayer, &stDispRect);
    HAL_VIDEO_SetLayerVideoRect(enLayer, &stDispRect);

    HAL_LAYER_SetLayerDataFmt(enLayer, HAL_INPUTFMT_ARGB_1555);
    
    HAL_LAYER_EnableLayer(enLayer, HI_TRUE);
    HAL_LAYER_SetRegUp(enLayer);

    return 0;
}

int stop_gx(unsigned int layer)
{
    VOU_LAYER_E enLayer;

    enLayer = GfxConvertLayer(layer);    
    
    HAL_LAYER_SetRegUp(enLayer);
    HAL_LAYER_EnableLayer(enLayer, HI_FALSE);    
    HAL_LAYER_SetRegUp(enLayer);
    
    return 0;
}

int start_videolayer(unsigned int layer, unsigned addr, unsigned int strd,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    RECT_S stDispRect = {x, y, w, h};
    VOU_LAYER_E enLayer;

	printf("layer:%d , addr:%x , stride:%d , (%d,%d,%d,%d) \n",layer,addr,strd,x,y,w,h);

    enLayer = VideoLayerConvert(layer);
    HAL_LAYER_SetLayerDataFmt(layer, VOU_LAYER_PIXERL_FORMAT_SPYCbCr_420);

    /*复制模式*/
    HAL_VIDEO_SetIfirMode(layer, 1);
    HAL_VIDEO_SetLayerDispRect(layer, &stMaxRect);
    HAL_VIDEO_SetLayerVideoRect(layer, &stMaxRect);
    HAL_LAYER_SetLayerOutRect(layer, &stMaxRect);
    HAL_LAYER_SetLayerInRect(layer, &stMaxRect);
    HAL_LAYER_SetLayerGalpha(layer, 255);

    /*关闭视频层缩放，且默认将滤波模式全部设置为复制模式*/
    HAL_LAYER_SetZmeEnable(layer, HAL_DISP_ZMEMODE_ALL, HI_FALSE);
    /*420->422->444(复制模式)，不支持缩放 */
    HAL_LAYER_SetZmeMscEnable(layer, HAL_DISP_ZMEMODE_VERC, HI_TRUE);
    HAL_LAYER_SetVerRatio(layer, 0x1000);

    HAL_LAYER_SetZmeVerType(layer, 1);

    HAL_LAYER_SetCscEn(layer, 0);

    HAL_VIDEO_SetMultiAreaRect(layer, 0, &stDispRect);
    HAL_VIDEO_SetMultiAreaReso(layer, 0, stDispRect.u32Width);
    HAL_VIDEO_SetMultiAreaLAddr(layer, 0, addr, strd);
    HAL_VIDEO_SetMultiAreaCAddr(layer, 0, addr+strd * VOU_ALAIN(h, 16), strd);
    HAL_VIDEO_SetMultiAreaEnable(layer, 0, 1);
    HAL_LAYER_EnableLayer(enLayer, HI_TRUE);
    HAL_LAYER_SetRegUp(enLayer);

    return 0;
}

int stop_videolayer(unsigned int layer)
{
    VOU_LAYER_E enLayer;

    enLayer = VideoLayerConvert(layer);   
    
    HAL_VIDEO_SetMultiAreaEnable(layer, 0, 0);
    HAL_LAYER_EnableLayer(enLayer, HI_FALSE);    
    HAL_LAYER_SetRegUp(enLayer);
    
    return 0;
}

