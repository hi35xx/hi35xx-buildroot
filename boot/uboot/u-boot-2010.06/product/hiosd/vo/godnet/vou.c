#include <common.h>
#include <command.h>
#include <version.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include <asm/sizes.h>
#include <config.h>

#include "vou_drv.h"
#include "vou_hal.h"

static HI_U32 g_au32BgColor[4] = {0xFF00, 0xFF00, 0xFF00, 0xFF00};	 /* default value is GREEN */
static HI_BOOL g_abHdStart[2] = {0};
static HI_U32 g_au32HdType[2] = {0};
static HI_BOOL g_bInited = HI_FALSE;

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
    return (layer + VOU_LAYER_G0);
}

int set_vobg(unsigned int dev, unsigned int rgb)
{
	if (dev < 4) 
	{
		g_au32BgColor[dev] = rgb;
	}
	return 0;
}

int start_vo(unsigned int dev, unsigned int type, unsigned int sync)
{
    int i;

    if (g_bInited == HI_FALSE)
    {
        SYS_HAL_SelVoBt1120PinConifg();
        SYS_HAL_SelVoVgaPinConifg();
        SYS_HAL_SelVoHdmiPinConifg();
        
        SYS_HAL_VouBusResetSel(HI_FALSE);
        SYS_HAL_VouBusClkEn(HI_TRUE);
        SYS_HAL_VouSdResetSel(HI_FALSE);
        for (i=0; i<2; i++)
        {
            SYS_HAL_VouHdResetSel(i, HI_FALSE);
        }
        for (i=0; i<2; i++)
        {
            SYS_HAL_VouDevClkEn(i, HI_TRUE);
        }
        
        VOU_DRV_BoardInit();
        HAL_SYS_Control();
        VOU_DRV_DefaultSetting();
        
        HAL_CBM_SetCbmAttr(HAL_DISP_LAYER_GFX4, HAL_DISP_CHANNEL_DHD0);
        HAL_CBM_SetCbmAttr(HAL_DISP_LAYER_HC0, HAL_DISP_CHANNEL_DHD0);
        HAL_CBM_SetCbmAttr(HAL_DISP_LAYER_HC1, HAL_DISP_CHANNEL_DSD0);

        g_bInited = HI_TRUE;
    }

    if (type & VO_INTF_HDMI)
    {
        SYS_HAL_SetVoHdmiReset(HI_FALSE);        
    }

    if ((dev == 2) || (dev == 3))
    {
        SYS_HAL_VouDevClkEn(dev, HI_TRUE);
    }

    VOU_DRV_SetDevIntfType(dev, type);
    VOU_DRV_SetDevOutSync(dev, sync);
    VOU_DRV_SetDevBkGrd(dev, Rgb2Yuv(g_au32BgColor[dev]));

    VOU_DRV_SetDevClk(dev);
    
    VOU_DRV_Open(dev);
    
    return 0;
}

int stop_vo(unsigned int dev)
{
    int i;
    
    VOU_DRV_Close(dev);
    //SYS_HAL_VouDevClkEn(dev, HI_FALSE);

    //VOU_DRV_IntDisableAll();
    //VOU_DRV_IntClear(VOU_INTREPORT_ALL);

    return 0;
}



int start_gx(unsigned int layer, unsigned addr, unsigned int strd,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    HI_RECT_S stDispRect = {x, y, w, h};
    VOU_LAYER_E enLayer;

    enLayer = GfxConvertLayer(layer);

    HAL_GRAPHIC_SetGfxExt(enLayer, HAL_GFX_BITEXTEND_3RD);
    HAL_GRAPHIC_SetGfxPalpha(enLayer, HI_TRUE, HI_TRUE, 0xff, 0xff);
    HAL_LAYER_SetLayerGalpha(enLayer, 0xff);
    HAL_GRAPHIC_SetGfxPreMult(enLayer, HI_FALSE);
    VOU_DRV_GraphicsSetCscCoef(enLayer);
    HAL_LAYER_SetCscEn(enLayer, HI_TRUE);

    HAL_GRAPHIC_SetGfxAddr(enLayer, addr);
    HAL_GRAPHIC_SetGfxStride(enLayer, strd);
    HAL_LAYER_SetLayerInRect(enLayer, &stDispRect);
    HAL_LAYER_SetLayerOutRect(enLayer, &stDispRect);    
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

