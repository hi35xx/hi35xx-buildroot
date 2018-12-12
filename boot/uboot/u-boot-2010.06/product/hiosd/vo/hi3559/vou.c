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
static HI_BOOL g_bInited = HI_FALSE;


int set_vobg(unsigned int dev, unsigned int rgb)
{
	if (dev < 3) 
	{
		g_au32BgColor[dev] = rgb;
	}
	return 0;
}

int start_vo(unsigned int dev, unsigned int type, unsigned int sync)
{

    if (g_bInited == HI_FALSE)
    {    
        VOU_DRV_BoardInit();      
        SYS_HAL_VouBusResetSel(HI_FALSE);

        SYS_HAL_VouBusClkEn(HI_TRUE);
        SYS_HAL_VouCFGClkEn(dev, HI_TRUE);
        SYS_HAL_VouPPCClkEn(dev, HI_TRUE);
        SYS_HAL_VouAPBClkEn(dev, HI_TRUE);

        g_bInited = HI_TRUE;
    }
    
    SYS_HAL_VouDevClkEn(0, HI_TRUE);
    VOU_DRV_DefaultSetting();

    VOU_DRV_SetDevIntfType(dev, type);
    VOU_DRV_SetDevOutSync(dev, sync);
    VOU_DRV_SetDevBkGrd(dev, g_au32BgColor[dev]);

    VOU_DRV_SetDevClk(dev);
    VOU_DRV_Open(dev); 
    
    return 0;
}

int stop_vo(unsigned int dev)
{
    
    VOU_DRV_Close(dev);
    SYS_HAL_VouDevClkEn(dev, HI_FALSE);

    return 0;
}



int start_gx(unsigned int addr, unsigned int x, unsigned int y)
{
    unsigned int layer = 1;
    OSD_LOGO_T stScrollImageLogo = {0};
    VO_INTF_TYPE_E enIntfType;

    LoadBMP((HI_U8*)addr, &stScrollImageLogo);
    
    RECT_S stDispRect = {x, y, stScrollImageLogo.width, stScrollImageLogo.height};
    VO_CSC_S stGfxCsc;

    stGfxCsc.enCscMatrix = VO_CSC_MATRIX_RGB_TO_BT601_PC;
    stGfxCsc.u32Luma = 50;
    stGfxCsc.u32Contrast = 50;
    stGfxCsc.u32Hue = 50;
    stGfxCsc.u32Satuature = 50; 
    GRAPHIC_DRV_SetCscCoef(layer, &stGfxCsc);

    GRAPHIC_DRV_GetIntfType(0, &enIntfType);

    if (VO_INTF_LCD == enIntfType || VO_INTF_LCD_6BIT == enIntfType
        || VO_INTF_LCD_8BIT == enIntfType || VO_INTF_LCD_16BIT == enIntfType)
    {
        HAL_LAYER_SetCscEn(layer, HI_FALSE);
    }
    else
    {
        HAL_LAYER_SetCscEn(layer, HI_TRUE);
    }

    HAL_GRAPHIC_SetGfxExt(layer, HAL_GFX_BITEXTEND_3RD);
    HAL_GRAPHIC_SetGfxPalpha(layer, HI_TRUE, HI_TRUE, 0xff, 0xff);
    HAL_LAYER_SetLayerGalpha(layer, 0xff);
    
    HAL_GRAPHIC_SetGfxPreMult(layer, HI_FALSE);

    HAL_GRAPHIC_SetGfxAddr(layer, (HI_U32)stScrollImageLogo.pRGBBuffer);
    HAL_GRAPHIC_SetGfxStride(layer, (stScrollImageLogo.stride)>>4);
    HAL_LAYER_SetLayerInRect(layer, &stDispRect);
    HAL_VIDEO_SetLayerDispRect(layer, &stDispRect);
    HAL_VIDEO_SetLayerVideoRect(layer, &stDispRect);

    HAL_LAYER_SetLayerDataFmt(layer, HAL_INPUTFMT_ARGB_1555);
    
    HAL_LAYER_EnableLayer(layer, HI_TRUE);
    HAL_LAYER_SetRegUp(layer);

    return 0;
}

int stop_gx(void)
{
    VOU_LAYER_E enLayer = HAL_DISP_LAYER_GFX2 ;  
    
    HAL_LAYER_SetRegUp(enLayer);
    HAL_LAYER_EnableLayer(enLayer, HI_FALSE);    
    HAL_LAYER_SetRegUp(enLayer);
    
    return 0;
}

