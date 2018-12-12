/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_hal.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/06/28
  Description   :
  History       :
  1.Date        : 2011/06/28
    Author      : n00168968    
    Modification: Created file
  2.Date        : 2012/06/19
    Author      : l00181524    
    Modification: Modify file

******************************************************************************/
#ifdef __LINUX_OS__
#include <linux/kernel.h>       /* printk */
#include <asm/hardware.h>       /* IO_ADDRESS */
#include <asm/io.h>             /* ioremap_nocache */
#include <linux/delay.h>        /* for ndelay(ns) */
#else
#include <common.h>
#include <command.h>
#include <malloc.h>
//#include <devices.h>
#include <version.h>
#include <net.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include <asm/sizes.h>
#endif

#include "vou_hal.h"
#include "hi3559_vo.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
#define HAL_PRINT printf
#define HI_PRINT printf
#define IO_ADDRESS(x) (x)

#define HI_INVALID_LAYER (-1)

/***************************************/
#define CRG_REGS_ADDR   0x12010000
#define CRG_REGS_SIZE   0X10000


#define CRG_PERCTL17_ADDR (0x0044 + CRG_REGS_ADDR)
#define CRG_PERCTL18_ADDR (0x0048 + CRG_REGS_ADDR)

 volatile VOU_REGS_S *pVoReg = NULL;

  
HI_VOID HAL_VOU_Init(HI_VOID)
{
    pVoReg = (volatile VOU_REGS_S *)IO_ADDRESS(VO_BASE_ADDR);

}



static inline void HI_RegSetBit(unsigned long value, unsigned long offset,
    unsigned long addr)
{
	unsigned long t, mask;

	mask = 1 << offset;
	t = readl(addr);
	t &= ~mask;
	t |= (value << offset) & mask;
	writel(t, addr);
}

static inline void HI_RegWrite32(unsigned long value, unsigned long mask,
    unsigned long addr)
{
	unsigned long t;

	t = readl(addr);
	t &= ~mask;
	t |= value & mask;
	writel(t, addr);
}

HI_VOID HAL_WriteReg(HI_U32 *pAddress, HI_U32 Value)
{
    *(volatile HI_U32*)pAddress = Value;
    return;
}

HI_U32 HAL_ReadReg(HI_U32 *pAddress)
{
    return *(volatile HI_U32*)(pAddress);
}


HI_S32 SYS_HAL_VouBusResetSel(HI_BOOL bReset)
{
    volatile HI_U32 u32Tmp = (bReset == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 0, IO_ADDRESS(CRG_PERCTL17_ADDR));

    return 0;
}




HI_S32 SYS_HAL_VouHdOutPctrl(HI_S32 s32Dev, HI_BOOL pbClkReverse)
{
    HI_U32 u32Tmp = (pbClkReverse == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 2, IO_ADDRESS(CRG_PERCTL17_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouDacPctrl(HI_S32 s32Dev, HI_BOOL pbClkReverse)
{
    HI_U32 u32Tmp = (pbClkReverse == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 3, IO_ADDRESS(CRG_PERCTL17_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouCFGClkEn(HI_S32 s32Dev, HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0; 
    
    if (0 == s32Dev)
    {   
        HI_RegSetBit(u32Tmp, 4, IO_ADDRESS(CRG_PERCTL17_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}


HI_S32 SYS_HAL_VouPPCClkEn(HI_S32 s32Dev, HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0; 
    
    if (0 == s32Dev)
    {   
        HI_RegSetBit(u32Tmp, 5, IO_ADDRESS(CRG_PERCTL17_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}

HI_S32 SYS_HAL_VouAPBClkEn(HI_S32 s32Dev, HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0; 
    
    if (0 == s32Dev)
    {   
        HI_RegSetBit(u32Tmp, 6, IO_ADDRESS(CRG_PERCTL17_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}


HI_S32 SYS_HAL_VouBusClkEn(HI_BOOL pbClkEn)
{
    volatile HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 7, IO_ADDRESS(CRG_PERCTL17_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouOutClkEn(HI_S32 s32Dev, HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0; 
    
    if (0 == s32Dev)
    {   
        HI_RegSetBit(u32Tmp, 8, IO_ADDRESS(CRG_PERCTL17_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}

HI_S32 SYS_HAL_VouDevClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;  

    if (0 == s32VoDev)
    {
        HI_RegSetBit(u32Tmp, 9, IO_ADDRESS(CRG_PERCTL17_ADDR));
        HI_RegSetBit(u32Tmp, 10, IO_ADDRESS(CRG_PERCTL17_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}


HI_S32 SYS_HAL_VouSdDacClkEn(HI_BOOL pbClkEn)
{
    //todo
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 11, IO_ADDRESS(CRG_PERCTL17_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouHdClkSel(HI_S32 s32Dev, HI_U32 u32ClkSel)
{
    if (0 == s32Dev)
    {   
        HI_RegWrite32(u32ClkSel << 12, 0x3 << 12, IO_ADDRESS(CRG_PERCTL17_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}


HI_S32 SYS_HAL_VouOutClkSel(HI_S32 s32Dev, HI_U32 u32ClkSel)
{
    if (0 == s32Dev)
    {   
        HI_RegWrite32(u32ClkSel << 14, 0x7 << 14, IO_ADDRESS(CRG_PERCTL17_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}


HI_S32 SYS_HAL_VouLcdClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;  

    HI_RegSetBit(u32Tmp, 27, IO_ADDRESS(CRG_PERCTL18_ADDR));

    return 0;
}

HI_S32 SYS_HAL_LcdMclkDiv(HI_U32 u32MclkDiv)
{
    HI_RegWrite32(u32MclkDiv, 0x7ffffff, IO_ADDRESS(CRG_PERCTL18_ADDR));

    return 0;
}


HI_U32 inline HAL_GetXDC_BUMA(HI_S32 s32Value)
{
    HI_U32 u32AbsValue = s32Value << 2;

    if(s32Value >= 0)
    {
        return u32AbsValue & 0x7ff;
    }

    else
    {
        u32AbsValue = (~((-1)* u32AbsValue) +1);
        //return ( (((~u32AbsValue)+1)& 0xFF) | 0x100 );
        u32AbsValue = ((u32AbsValue & 0x7ff) | 0x400);
        return u32AbsValue;
    }
}

HI_U32 inline HAL_Conver_CscCoef(HI_S32 s32Value)
{
    HI_S32 s32Result = ((s32Value << 10)/ 1000 );

    if (s32Result < 0)
    //if (s32Value < 0)
    {
        s32Result = (~((-1)*s32Result) + 1);

        /* 5.8 format */
        s32Result = ((s32Result & 0x7fff) | 0x4000);
    }
    else
    {
        s32Result = s32Result & 0x7fff;
    }
    
    return s32Result;
}


HI_U32 Vou_GetAbsAddr(HAL_DISP_LAYER_E enLayer, HI_U32 pReg)
{
    HI_U32 RegAbsAddr;
    switch(enLayer)
    {
        case HAL_DISP_LAYER_VHD1:
        {

            RegAbsAddr = (pReg) + (enLayer + 1) * VHD_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_GFX2:    /* G2 */
        {
			/* hi3516a v1/g2 mixer2 */
            //RegAbsAddr = ((pReg)) + (enLayer - HAL_DISP_LAYER_GFX0) * GFX_REG_LEN;
            RegAbsAddr = ((pReg)) + (enLayer + 1) * GFX_REG_LEN;
            break;
        }
         default:
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return 0;
        }
    }

    return RegAbsAddr;
}


HI_U32 Vou_GetChnAbsAddr(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 pReg)
{
    volatile HI_U32 RegAbsAddr;
    switch(enChan)
    {
        case HAL_DISP_CHANNEL_DHD1:            
        {

			RegAbsAddr =pReg + (HAL_DISP_CHANNEL_DHD1 + 1) * DHD_REG_LEN;
            break;
        }      
        default:
        {  
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return 0;
        }
    }
     return RegAbsAddr;
}

 //for get Intf abs addr
HI_U32 Vou_GetIntfAbsAddr(HAL_DISP_INTF_E enIntf, HI_U32 pReg)
{
    volatile HI_U32 RegAbsAddr;
    switch(enIntf)
    {
        case HAL_DISP_INTF_HDMI:
        {
            RegAbsAddr =pReg ;
            break;
        }      
        case HAL_DISP_INTF_VGA: 
        {
            RegAbsAddr =pReg + INTF_REG_LEN;
            break;
        }
        case HAL_DISP_INTF_BT656:
        case HAL_DISP_INTF_BT1120: 
        {
            RegAbsAddr =pReg + 2 * INTF_REG_LEN;
            break;
        }
        case HAL_DISP_INTF_CVBS: 
        {
            RegAbsAddr =pReg + 3 * INTF_REG_LEN;
            break;
        }
	case HAL_DISP_INTF_LCD:
	case HAL_DISP_INTF_LCD_6BIT:
	case HAL_DISP_INTF_LCD_8BIT:
	case HAL_DISP_INTF_LCD_16BIT:
	{
	    RegAbsAddr = pReg + 4 * INTF_REG_LEN;
	    break;
	}
        default:
        {  
            HAL_PRINT("Error intf id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return 0;
        }
    }
     return RegAbsAddr;
}


HI_U32 HAL_GetBkg_BUMA(HI_U8 u8Bkg_YUV)
{
    HI_U32 u32Value = 0;
    return (u32Value|u8Bkg_YUV) << 2;
}
HI_VOID HAL_SYS_SetOutstanding(HI_VOID)
{
    volatile U_VOAXICTRL VOAXICTRL;

    VOAXICTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOAXICTRL.u32));
    VOAXICTRL.bits.m0_wr_ostd = 3;
    VOAXICTRL.bits.m0_outstd_rid0  = 3; 
    HAL_WriteReg((HI_U32*)&(pVoReg->VOAXICTRL.u32), VOAXICTRL.u32); 

    return ;
}


HI_VOID HAL_DISP_SetDateCoeffByIdx(HI_U32 u32Idx, HI_U32 u32Data)
{
    switch (u32Idx)
    {
        case 22:
        {
            pVoReg->DATE_COEFF22.u32 = u32Data;
            break;
        }
        case 24:
        {
            pVoReg->DATE_COEFF24.u32 = u32Data;
            break;
        }        
        case 50:
        {
            pVoReg->DATE_COEFF50.u32 = u32Data;
            break;
        }
        case 51:
        {
            pVoReg->DATE_COEFF51.u32 = u32Data;
            break;
        }
        case 52:
        {
            pVoReg->DATE_COEFF52.u32 = u32Data;
            break;
        }
        case 53:
        {
            pVoReg->DATE_COEFF53.u32 = u32Data;
            break;
        }
        case 54:
        {
            pVoReg->DATE_COEFF54.u32 = u32Data;
            break;
        }
        case 55:
        {
            pVoReg->DATE_COEFF55.u32 = u32Data;
            break;
        }
        case 57:
        {
            pVoReg->DATE_COEFF57.u32 = u32Data;
            break;
        }
        case 60:
        {
            pVoReg->DATE_COEFF60.u32 = u32Data;
            break;
        }
        case 61:
        {
            pVoReg->DATE_COEFF61.u32 = u32Data;
            break;
        }
        case 73:
        {
            pVoReg->DATE_COEFF73.u32 = u32Data;
            break;
        }

        default:
            break;
    }
}
HI_BOOL HAL_DISP_SetIntfEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL enIntf)
{
    volatile U_DHD0_CTRL DHD0_CTRL;     
    volatile HI_U32 addr_REG;
    if(HAL_DISP_CHANNEL_DHD1 == enChan)
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_CTRL.u32));
        DHD0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_CTRL.bits.intf_en = enIntf;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_CTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntfSync(HAL_DISP_OUTPUTCHANNEL_E enChan,
                                     HAL_DISP_SYNCINFO_S *pstSyncInfo)
{   
    volatile U_DHD0_CTRL DHD0_CTRL;
    volatile U_DHD0_VSYNC DHD0_VSYNC;    
    volatile U_DHD0_HSYNC1 DHD0_HSYNC1;
    volatile U_DHD0_HSYNC2 DHD0_HSYNC2; 
    volatile U_DHD0_VPLUS DHD0_VPLUS;
    volatile U_DHD0_PWR DHD0_PWR;
    volatile HI_U32 addr_REG;
    if(HAL_DISP_CHANNEL_DHD1 == enChan)
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_CTRL.u32));
        DHD0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);        
        DHD0_CTRL.bits.iop   = pstSyncInfo->bIop;        
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_CTRL.u32);   
        
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_HSYNC1.u32));
        DHD0_HSYNC1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_HSYNC1.bits.hact = pstSyncInfo->u16Hact -1;
        DHD0_HSYNC1.bits.hbb  = (pstSyncInfo->u16Hbb)-1;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_HSYNC1.u32); 

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_HSYNC2.u32));
        DHD0_HSYNC2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_HSYNC2.bits.hmid = (pstSyncInfo->u16Hmid == 0) ? 0 : pstSyncInfo->u16Hmid -1;
        DHD0_HSYNC2.bits.hfb  = (pstSyncInfo->u16Hfb)-1;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_HSYNC2.u32); 
        
       
        //Config VHD interface veritical timming
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_VSYNC.u32));
        DHD0_VSYNC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_VSYNC.bits.vact = pstSyncInfo->u16Vact  -1;
        DHD0_VSYNC.bits.vbb = pstSyncInfo->u16Vbb - 1;
        DHD0_VSYNC.bits.vfb =  pstSyncInfo->u16Vfb - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_VSYNC.u32); 
        
        //Config VHD interface veritical bottom timming,no use in progressive mode
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_VPLUS.u32));
        DHD0_VPLUS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_VPLUS.bits.bvact = pstSyncInfo->u16Bvact - 1;
        DHD0_VPLUS.bits.bvbb = pstSyncInfo->u16Bvbb - 1;
        DHD0_VPLUS.bits.bvfb =  pstSyncInfo->u16Bvfb - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_VPLUS.u32); 

        //Config VHD interface veritical bottom timming, 
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_PWR.u32));
        DHD0_PWR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_PWR.bits.hpw = pstSyncInfo->u16Hpw - 1;
        DHD0_PWR.bits.vpw = pstSyncInfo->u16Vpw - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_PWR.u32); 
    }   
    else
    {
         HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
         return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetIntfSyncInv(HAL_DISP_INTF_E enIntf, HAL_DISP_SYNCINV_S *pstInv)
{
    volatile U_INTF_SYNC_INV INTF_SYNC_INV;    
    volatile HI_U32 addr_REG;
    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_SYNC_INV.u32));
    INTF_SYNC_INV.u32 = HAL_ReadReg((HI_U32*)addr_REG);
    INTF_SYNC_INV.bits.f_inv = pstInv->u32f_inv;
    INTF_SYNC_INV.bits.vs_inv = pstInv->u32vs_inv;     
    INTF_SYNC_INV.bits.hs_inv = pstInv->u32hs_inv;
    INTF_SYNC_INV.bits.dv_inv = pstInv->u32dv_inv; 
    HAL_WriteReg((HI_U32*)addr_REG, INTF_SYNC_INV.u32); 
    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetIntfCtrl(HAL_DISP_INTF_E enIntf, HI_U32 *pu32CtrlInfo)
{
    U_INTF_CTRL LCD_CTRL;
    volatile HI_U32 addr_REG;
    U_INTF_CTRL *pstLCDCtrl;
     if(VO_INTF_LCD == enIntf
      || VO_INTF_LCD_6BIT == enIntf
      || VO_INTF_LCD_8BIT == enIntf
      || VO_INTF_LCD_16BIT == enIntf)
    {
        pstLCDCtrl = (U_INTF_CTRL*)pu32CtrlInfo;
        addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CTRL.u32));
        LCD_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        LCD_CTRL.bits.lcd_serial_mode = pstLCDCtrl->bits.lcd_serial_mode;
        LCD_CTRL.bits.lcd_serial_perd = pstLCDCtrl->bits.lcd_serial_perd;
        LCD_CTRL.bits.lcd_parallel_order = pstLCDCtrl->bits.lcd_parallel_order;
        LCD_CTRL.bits.lcd_data_inv = pstLCDCtrl->bits.lcd_data_inv;
        HAL_WriteReg((HI_U32*)addr_REG, LCD_CTRL.u32);
    }
    else
    {
        HAL_PRINT("Error interface type found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;

}


HI_BOOL HAL_DISP_SetIntfDacChnEnable(HAL_DISP_INTF_E enIntf, HI_BOOL bDacChnEnable)
{  
    volatile U_VO_DAC_R_CTRL VO_DAC_R_CTRL;    
    
    switch(enIntf)
    {
        case HAL_DISP_INTF_CVBS:
            VO_DAC_R_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32));
            VO_DAC_R_CTRL.bits.dac0en = bDacChnEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32), VO_DAC_R_CTRL.u32);

            break;
        default:
            break;
    }
    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetIntfDacEnable(HAL_DISP_INTF_E enIntf, HI_BOOL bDacEnable)
{
    volatile U_VO_DAC_CTRL VO_DAC_CTRL;          
    
    switch(enIntf)
    {
        case HAL_DISP_INTF_CVBS:
            VO_DAC_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_CTRL.u32));
            VO_DAC_CTRL.bits.envbg = bDacEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_CTRL.u32), VO_DAC_CTRL.u32);
            
            break;
        default:
            break;
    }
    return HI_TRUE;
}


HI_BOOL  HAL_DISP_SetIntfMuxSel(HAL_DISP_OUTPUTCHANNEL_E enChan,HAL_DISP_INTF_E enIntf)
{
    volatile U_VO_MUX VO_MUX;
    if(enChan > HAL_DISP_CHANNEL_DHD1)
     {
         HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
         return HI_FALSE;
     }

    enChan = HAL_DISP_CHANNEL_DHD1 + 1;

    VO_MUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_MUX.u32));    
    switch(enIntf)
    {
        case HAL_DISP_INTF_BT1120:
        {            
            VO_MUX.bits.digital_sel = 0;
            break;
        }        
        case HAL_DISP_INTF_BT656:
        {   
            VO_MUX.bits.digital_sel = 1;
            /* VO_MUX.bits.bt1120_sel = 1; */
            break;
        }
	case HAL_DISP_INTF_LCD:
	case HAL_DISP_INTF_LCD_6BIT:
	case HAL_DISP_INTF_LCD_8BIT:
	case HAL_DISP_INTF_LCD_16BIT:
	{
	    VO_MUX.bits.digital_sel = 2;
	    VO_MUX.bits.bt1120_sel = 1; 
	    break;
	}
        
        default:
        {
            HI_PRINT("Error! DP_DISP_SetIntfMuxSel Wrong Select\n");
            return HI_FALSE;
        }
    }
    
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_MUX.u32), VO_MUX.u32); 
    return HI_TRUE;
}


HI_BOOL HAL_DISP_CvbsDacEn(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL bEn)
{
    volatile U_VO_MUX_DAC VO_MUX_DAC;

    VO_MUX_DAC.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_MUX_DAC.u32));


    if (HAL_DISP_CHANNEL_DHD1 == enChan)
    {        
        VO_MUX_DAC.bits.dac0_sel = 0;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_MUX_DAC.u32), VO_MUX_DAC.u32);
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCvbsGc(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Value)
{
    volatile U_VO_DAC_R_CTRL VO_DAC_R_CTRL;

    VO_DAC_R_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32));
    if (HAL_DISP_CHANNEL_DHD1 == enChan)
    {        
        VO_DAC_R_CTRL.bits.dac0gc = u32Value;
    }    
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32), VO_DAC_R_CTRL.u32);
    
    return HI_TRUE;
}

HI_BOOL  HAL_DISP_SetIntfClip(HAL_DISP_INTF_E enIntf,
                                        HI_BOOL enClip,
                                        HAL_DISP_CLIP_S *pstClipData)
{   
    volatile U_INTF_CLIP0_L    INTF_CLIP0_L;
    volatile U_INTF_CLIP0_H    INTF_CLIP0_H;    

    volatile  HI_U32 addr_REG;
    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CLIP0_L.u32));
    INTF_CLIP0_L.u32 = HAL_ReadReg((HI_U32*)addr_REG);
    INTF_CLIP0_L.bits.clip_cl2 = pstClipData->u16ClipLow_y;
    INTF_CLIP0_L.bits.clip_cl1 = pstClipData->u16ClipLow_cb;
    INTF_CLIP0_L.bits.clip_cl0 = pstClipData->u16ClipLow_cr;        
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CLIP0_L.u32); 

    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CLIP0_H.u32));
    INTF_CLIP0_H.u32 = HAL_ReadReg((HI_U32*)addr_REG);
    INTF_CLIP0_H.bits.clip_ch2 = pstClipData->u16ClipHigh_y;
    INTF_CLIP0_H.bits.clip_ch1 = pstClipData->u16ClipHigh_cb;
    INTF_CLIP0_H.bits.clip_ch0 = pstClipData->u16ClipHigh_cr;        
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CLIP0_H.u32); 
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVtThdMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 uFieldMode)
{    
    volatile U_DHD0_VTTHD  DHD0_VTTHD;
    volatile  HI_U32 addr_REG;
    if(HAL_DISP_CHANNEL_DHD1 == enChan)
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_VTTHD.u32));
        DHD0_VTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_VTTHD.bits.thd1_mode = uFieldMode;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_VTTHD.u32); 
    }   
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerUpMode(HAL_DISP_LAYER_E enLayer, HI_U32 bUpMode)
{
    U_V0_CTRL V0_CTRL;
    volatile  HI_U32 addr_REG; 
    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CTRL.u32));
        V0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CTRL.bits.vup_mode = bUpMode;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;        
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetClkGateEnable(HI_U32 u32Data)
{
    volatile U_VOCTRL VOCTRL;
    
    VOCTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOCTRL.u32));
    VOCTRL.bits.vo_ck_gt_en = u32Data;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOCTRL.u32), VOCTRL.u32);
    return HI_TRUE;
}

HI_VOID HAL_DISP_DATE_OutCtrl(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32OutCtrl)
{

    if (HAL_DISP_CHANNEL_DHD1 == enChan)
    {
        U_DATE_COEFF21 DateOutCtrl, tmp;

        tmp.u32 = u32OutCtrl;
        DateOutCtrl.u32 = pVoReg->DATE_COEFF21.u32;
        DateOutCtrl.bits.dac0_in_sel = tmp.bits.dac0_in_sel;
        DateOutCtrl.bits.dac1_in_sel = tmp.bits.dac1_in_sel;
        DateOutCtrl.bits.dac2_in_sel = tmp.bits.dac2_in_sel;
        DateOutCtrl.bits.dac3_in_sel = tmp.bits.dac3_in_sel;
        DateOutCtrl.bits.dac4_in_sel = tmp.bits.dac4_in_sel;
        DateOutCtrl.bits.dac5_in_sel = tmp.bits.dac5_in_sel;
        pVoReg->DATE_COEFF21.u32 = DateOutCtrl.u32;
    }    
    return;
}

HI_BOOL HAL_DISP_SetDateCoeff(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    U_DATE_COEFF0 DATE_COEFF0;


    if (HAL_DISP_CHANNEL_DHD1 == enChan)
    {
        DATE_COEFF0.u32 = pVoReg->DATE_COEFF0.u32;
        DATE_COEFF0.u32 = u32Data;
        pVoReg->DATE_COEFF0.u32 = DATE_COEFF0.u32;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_VOID HAL_DISP_SetRegUp (HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    volatile U_DHD0_CTRL DHD0_CTRL;    
    volatile  HI_U32 addr_REG; 
    if(enChan > HAL_DISP_CHANNEL_DHD1)
    {
        HI_PRINT("Error,VDP_DISP_SetIntfEnable Select Wrong CHANNEL ID\n");
        return ;
    }
    addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_CTRL.u32));
    DHD0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
    DHD0_CTRL.bits.regup = 0x1;
    HAL_WriteReg((HI_U32*)addr_REG, DHD0_CTRL.u32); 
    return;
}




HI_BOOL HAL_VIDEO_SetLayerDispRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect)
{
    volatile U_V0_DFPOS V0_DFPOS;
    volatile U_V0_DLPOS V0_DLPOS;    
    
    volatile U_G0_DFPOS G0_DFPOS;
    volatile U_G0_DLPOS G0_DLPOS;    
    volatile  HI_U32 addr_REG;
    
    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_DFPOS.u32));
        V0_DFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_DFPOS.bits.disp_xfpos = pstRect->s32X;
        V0_DFPOS.bits.disp_yfpos = pstRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, V0_DFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_DLPOS.u32));
        V0_DLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_DLPOS.bits.disp_xlpos = pstRect->s32X +pstRect->u32Width -1;
        V0_DLPOS.bits.disp_ylpos = pstRect->s32Y + pstRect->u32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_DLPOS.u32);         
    }    
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_DFPOS.u32));
        G0_DFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_DFPOS.bits.disp_xfpos = pstRect->s32X;
        G0_DFPOS.bits.disp_yfpos = pstRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, G0_DFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_DLPOS.u32));
        G0_DLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_DLPOS.bits.disp_xlpos = pstRect->s32X +pstRect->u32Width -1;
        G0_DLPOS.bits.disp_ylpos = pstRect->s32Y + pstRect->u32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, G0_DLPOS.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}


HI_BOOL HAL_VIDEO_SetLayerVideoRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect)
{
    volatile U_V0_VFPOS V0_VFPOS;    
    volatile U_V0_VLPOS V0_VLPOS;
    volatile U_V0_CPOS V0_CPOS;
    
    volatile U_G0_VFPOS G0_VFPOS;    
    volatile U_G0_VLPOS G0_VLPOS;
    volatile  HI_U32 addr_REG;
    
    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VFPOS.u32));
        V0_VFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_VFPOS.bits.video_xfpos = pstRect->s32X;
        V0_VFPOS.bits.video_yfpos = pstRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, V0_VFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VLPOS.u32));
        V0_VLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_VLPOS.bits.video_xlpos = pstRect->s32X +pstRect->u32Width -1;
        V0_VLPOS.bits.video_ylpos = pstRect->s32Y + pstRect->u32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_VLPOS.u32);

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CPOS.u32));
        V0_CPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CPOS.bits.src_xfpos = pstRect->s32X;
        V0_CPOS.bits.src_xlpos = pstRect->s32X + pstRect->u32Width - 1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CPOS.u32);
    }    
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_VFPOS.u32));
        G0_VFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_VFPOS.bits.video_xfpos = pstRect->s32X;
        G0_VFPOS.bits.video_yfpos = pstRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, G0_VFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_VLPOS.u32));
        G0_VLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_VLPOS.bits.video_xlpos = pstRect->s32X +pstRect->u32Width -1;
        G0_VLPOS.bits.video_ylpos = pstRect->s32Y + pstRect->u32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, G0_VLPOS.u32);   

    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}




HI_BOOL HAL_LAYER_EnableLayer(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable)
                                                           
{
    volatile U_V0_CTRL V0_CTRL;
    volatile U_G0_CTRL G0_CTRL;    
    volatile  HI_U32 addr_REG;

    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CTRL.u32));
        V0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CTRL.bits.surface_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CTRL.u32); 
    }
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CTRL.u32));
        G0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CTRL.bits.surface_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, G0_CTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_LAYER_SetLayerDataFmt(HAL_DISP_LAYER_E enLayer,
                                            HAL_DISP_PIXEL_FORMAT_E  enDataFmt)
{
    volatile U_V0_CTRL V0_CTRL;
    volatile U_G0_CTRL G0_CTRL;
    volatile  HI_U32 addr_REG;
    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CTRL.u32));
        V0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CTRL.bits.ifmt = enDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CTRL.u32); 
    }   
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CTRL.u32));
        G0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CTRL.bits.ifmt = enDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, G0_CTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetCscCoef(HAL_DISP_LAYER_E enLayer, CscCoef_S *pstCscCoef)
{
    U_G0_CSC_IDC G0_CSC_IDC;
    U_G0_CSC_ODC  G0_CSC_ODC;    
    U_G0_CSC_IODC  G0_CSC_IODC;
    U_V0_CSC_IDC  V0_CSC_IDC;
    U_V0_CSC_ODC  V0_CSC_ODC;    
    U_V0_CSC_IODC  V0_CSC_IODC;
    
    volatile U_V0_CSC_P0 V0_CSC_P0;
    volatile U_V0_CSC_P1 V0_CSC_P1;
    volatile U_V0_CSC_P2 V0_CSC_P2;
    volatile U_V0_CSC_P3 V0_CSC_P3;
    volatile U_V0_CSC_P4 V0_CSC_P4;

    volatile U_G0_CSC_P0 G0_CSC_P0;
    volatile U_G0_CSC_P1 G0_CSC_P1;
    volatile U_G0_CSC_P2 G0_CSC_P2;
    volatile U_G0_CSC_P3 G0_CSC_P3;
    volatile U_G0_CSC_P4 G0_CSC_P4;
    
    volatile  HI_U32 addr_REG;

    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_IDC.u32));
        V0_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_IDC.bits.cscidc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc1);
      // V0_CSC_IDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc0);        
        V0_CSC_IDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc2);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_IDC.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_ODC.u32));
        V0_CSC_ODC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_ODC.bits.cscodc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc1);
        //V0_CSC_ODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc0);        
        V0_CSC_ODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc2);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_ODC.u32);
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_IODC.u32));
        V0_CSC_IODC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        //V0_CSC_IODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc2);
        //V0_CSC_IODC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc2);
        
        V0_CSC_IODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc0);
        V0_CSC_IODC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_IODC.u32);
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_P0.u32));
        V0_CSC_P0.u32 = HAL_ReadReg((HI_U32*)addr_REG);        
        V0_CSC_P0.bits.cscp00 = HAL_Conver_CscCoef(pstCscCoef->csc_coef00);
        V0_CSC_P0.bits.cscp01 = HAL_Conver_CscCoef(pstCscCoef->csc_coef01);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_P0.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_P1.u32));
        V0_CSC_P1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_P1.bits.cscp02 = HAL_Conver_CscCoef(pstCscCoef->csc_coef02);
        V0_CSC_P1.bits.cscp10 = HAL_Conver_CscCoef(pstCscCoef->csc_coef10);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_P1.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_P2.u32));
        V0_CSC_P2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_P2.bits.cscp11 = HAL_Conver_CscCoef(pstCscCoef->csc_coef11);
        V0_CSC_P2.bits.cscp12 = HAL_Conver_CscCoef(pstCscCoef->csc_coef12);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_P2.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_P3.u32));
        V0_CSC_P3.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_P3.bits.cscp20 = HAL_Conver_CscCoef(pstCscCoef->csc_coef20);
        V0_CSC_P3.bits.cscp21 = HAL_Conver_CscCoef(pstCscCoef->csc_coef21);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_P3.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_P4.u32));
        V0_CSC_P4.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_P4.bits.cscp22 = HAL_Conver_CscCoef(pstCscCoef->csc_coef22);
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_P4.u32); 
    }    
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_IDC.u32));
        G0_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CSC_IDC.bits.cscidc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc1);
        //G0_CSC_IDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc0);        
        G0_CSC_IDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc2);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_IDC.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_ODC.u32));
        G0_CSC_ODC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CSC_ODC.bits.cscodc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc1);
        //G0_CSC_ODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc0);        
        G0_CSC_ODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc2);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_ODC.u32);
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_IODC.u32));
        G0_CSC_IODC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        //G0_CSC_IODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc2);
        //G0_CSC_IODC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc2);
        G0_CSC_IODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc0);
        G0_CSC_IODC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_IODC.u32);
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_P0.u32));
        G0_CSC_P0.u32 = HAL_ReadReg((HI_U32*)addr_REG);        
        G0_CSC_P0.bits.cscp00 = HAL_Conver_CscCoef(pstCscCoef->csc_coef00);
        G0_CSC_P0.bits.cscp01 = HAL_Conver_CscCoef(pstCscCoef->csc_coef01);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_P0.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_P1.u32));
        G0_CSC_P1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CSC_P1.bits.cscp02 = HAL_Conver_CscCoef(pstCscCoef->csc_coef02);
        G0_CSC_P1.bits.cscp10 = HAL_Conver_CscCoef(pstCscCoef->csc_coef10);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_P1.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_P2.u32));
        G0_CSC_P2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CSC_P2.bits.cscp11 = HAL_Conver_CscCoef(pstCscCoef->csc_coef11);
        G0_CSC_P2.bits.cscp12 = HAL_Conver_CscCoef(pstCscCoef->csc_coef12);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_P2.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_P3.u32));
        G0_CSC_P3.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CSC_P3.bits.cscp20 = HAL_Conver_CscCoef(pstCscCoef->csc_coef20);
        G0_CSC_P3.bits.cscp21 = HAL_Conver_CscCoef(pstCscCoef->csc_coef21);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_P3.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_P4.u32));
        G0_CSC_P4.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CSC_P4.bits.cscp22 = HAL_Conver_CscCoef(pstCscCoef->csc_coef22);
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_P4.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetCscMode(HAL_DISP_LAYER_E enLayer, HI_BOOL bIsHCModeBy709)
{
    U_G0_CSC_IDC G0_CSC_IDC;          
    volatile  HI_U32 addr_REG;

    

    if(HAL_DISP_LAYER_GFX2 == enLayer) 
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_IDC.u32));
        G0_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        if (HI_TRUE == bIsHCModeBy709)
        {
            G0_CSC_IDC.bits.csc_mode = 0x7; /* 111£º RGB2YUV_709 */
        }
        else
        {
            G0_CSC_IDC.bits.csc_mode = 0x6; /* 110£º RGB2YUV_601 */
        }
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_IDC.u32); 
    }
    else
    {
        
    }

    return HI_TRUE;
}



HI_BOOL HAL_LAYER_SetCscEn(HAL_DISP_LAYER_E enLayer, HI_BOOL bCscEn)
{
    U_V0_CSC_IDC  V0_CSC_IDC;
    U_G0_CSC_IDC G0_CSC_IDC;
    volatile  HI_U32 addr_REG;

    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_IDC.u32));
        V0_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_IDC.bits.csc_en = bCscEn;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_IDC.u32); 
    }
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CSC_IDC.u32));
        G0_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CSC_IDC.bits.csc_en = bCscEn;
        HAL_WriteReg((HI_U32*)addr_REG, G0_CSC_IDC.u32); 
    }
    else
    {
        HAL_PRINT("Error, Wrong layer ID!\n");
        return HI_FALSE;
    }
    
    return HI_TRUE;
}




HI_BOOL HAL_LAYER_SetLayerInRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect)
{
    U_V0_IRESO V0_IRESO;
    U_G0_IRESO G0_IRESO;
    //U_G0SFPOS G0SFPOS;
    volatile  HI_U32 addr_REG;
    
    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_IRESO.u32));
        V0_IRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_IRESO.bits.iw = pstRect->u32Width- 1;
        V0_IRESO.bits.ih = pstRect->u32Height- 1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_IRESO.u32); 
    }   
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_IRESO.u32));
        G0_IRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_IRESO.bits.iw = pstRect->u32Width- 1;
        G0_IRESO.bits.ih = pstRect->u32Height- 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0_IRESO.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}



HI_BOOL HAL_LAYER_SetLayerGalpha(HAL_DISP_LAYER_E enLayer,
                                     HI_U8 u8Alpha0)
{
    volatile U_V0_CBMPARA V0_CBMPARA;
    volatile U_G0_CBMPARA G0_CBMPARA;

    volatile  HI_U32 addr_REG;
    if(HAL_DISP_LAYER_VHD1 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CBMPARA.u32));
        V0_CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CBMPARA.bits.galpha = u8Alpha0;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CBMPARA.u32); 
    }
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CBMPARA.u32));
        G0_CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CBMPARA.bits.galpha = u8Alpha0;
        HAL_WriteReg((HI_U32*)addr_REG, G0_CBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id %d found in %s: L%d\n",enLayer,__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}



HI_BOOL HAL_DISP_SetIntMask(HI_U32 u32MaskEn)
{
    volatile U_VOINTMSK VOINTMSK;

    /* Dispaly interrupt mask enable */
    VOINTMSK.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOINTMSK.u32));
    VOINTMSK.u32 = VOINTMSK.u32 | u32MaskEn;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOINTMSK.u32), VOINTMSK.u32); 

    return HI_TRUE;
}

HI_BOOL  HAL_DISP_ClrIntMask(HI_U32 u32MaskEn)
{
    volatile U_VOINTMSK VOINTMSK;

    /* Dispaly interrupt mask enable */
    VOINTMSK.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOINTMSK.u32));
    VOINTMSK.u32 = VOINTMSK.u32 & (~u32MaskEn);
    HAL_WriteReg((HI_U32*)&(pVoReg->VOINTMSK.u32), VOINTMSK.u32);

    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetVtThd(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 vtthd)
{    
    volatile U_DHD0_VTTHD  DHD0_VTTHD;
    volatile  HI_U32 addr_REG;
    if(HAL_DISP_CHANNEL_DHD1 == enChan)
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_VTTHD.u32));
        DHD0_VTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHD0_VTTHD.bits.vtmgthd1 = vtthd;
        HAL_WriteReg((HI_U32*)addr_REG, DHD0_VTTHD.u32); 
    }   
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE; 
    }
    return HI_TRUE;
}

HI_BOOL  HAL_LAYER_SetRegUp(HAL_DISP_LAYER_E enLayer)
{
    U_V0_UPD V0_UPD;
    U_G0_UPD G0_UPD;    
    volatile HI_U32 addr_REG;
    
    if(enLayer <= HAL_DISP_LAYER_VHD1)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_UPD.u32));
        V0_UPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        /* layer register update */
        V0_UPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_UPD.u32); 
    }
    else if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_UPD.u32));
        G0_UPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        /* layer register update */
        G0_UPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, G0_UPD.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}




HI_BOOL HAL_GRAPHIC_SetGfxAddr(HAL_DISP_LAYER_E enLayer, HI_U32 u32LAddr)
{
    U_G0_ADDR G0_ADDR;
    volatile  HI_U32 addr_REG;

    if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_ADDR.u32));
        G0_ADDR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_ADDR.u32 = u32LAddr;
        HAL_WriteReg((HI_U32*)addr_REG, G0_ADDR.u32); 

    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}


HI_BOOL HAL_GRAPHIC_SetGfxStride(HAL_DISP_LAYER_E enLayer, HI_U16 u16pitch)
{
    volatile U_G0_STRIDE G0_STRIDE;
    volatile  HI_U32 addr_REG;
    if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_STRIDE.u32));
        G0_STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_STRIDE.bits.surface_stride = u16pitch;
        HAL_WriteReg((HI_U32*)addr_REG, G0_STRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_SetGfxExt(HAL_DISP_LAYER_E enLayer,
                                HAL_GFX_BITEXTEND_E enMode)
{
    U_G0_CTRL G0_CTRL;

    volatile  HI_U32 addr_REG;
    if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CTRL.u32));
        G0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CTRL.bits.bitext = enMode;
        HAL_WriteReg((HI_U32*)addr_REG, G0_CTRL.u32); 

    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetGfxPreMult(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable)
{
    U_G0_CBMPARA G0_CBMPARA;
    volatile  HI_U32 addr_REG;

    if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CBMPARA.u32));
        G0_CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CBMPARA.bits.premult_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, G0_CBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;    
}


HI_BOOL HAL_GRAPHIC_SetGfxPalpha(HAL_DISP_LAYER_E enLayer,
                                   HI_U32 bAlphaEn,HI_U32 bArange,
                                   HI_U8 u8Alpha0, HI_U8 u8Alpha1)
{
    U_G0_CBMPARA G0_CBMPARA;
    U_G0_CKEYMIN G0_CKEYMIN;
    U_G0_CKEYMAX G0_CKEYMAX;

    volatile HI_U32 addr_REG;
    if(HAL_DISP_LAYER_GFX2 == enLayer)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CBMPARA.u32));
        G0_CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CBMPARA.bits.palpha_en = bAlphaEn; 
        HAL_WriteReg((HI_U32*)addr_REG, G0_CBMPARA.u32); 

        if(HI_TRUE == bAlphaEn)
        {
	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CKEYMIN.u32));
	        G0_CKEYMIN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0_CKEYMIN.bits.va1 = u8Alpha1;
	        HAL_WriteReg((HI_U32*)addr_REG, G0_CKEYMIN.u32); 

	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CKEYMAX.u32));
	        G0_CKEYMAX.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0_CKEYMAX.bits.va0 = u8Alpha0;                            
	        HAL_WriteReg((HI_U32*)addr_REG, G0_CKEYMAX.u32); 
        }
		else
		{
	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CKEYMIN.u32));
	        G0_CKEYMIN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0_CKEYMIN.bits.va1 = 0xff;
	        HAL_WriteReg((HI_U32*)addr_REG, G0_CKEYMIN.u32); 

	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CKEYMAX.u32));
	        G0_CKEYMAX.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0_CKEYMAX.bits.va0 = 0xff;                            
	        HAL_WriteReg((HI_U32*)addr_REG, G0_CKEYMAX.u32); 
        }	
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}


HI_BOOL HAL_GRAPHIC_SetGfxPalphaRange(HAL_DISP_LAYER_E enLayer, HI_U32 bArange)
{
    U_G0_CBMPARA G0_CBMPARA;

    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_CBMPARA.u32));
        G0_CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0_CBMPARA.bits.palpha_range = bArange;            
        HAL_WriteReg((HI_U32*)addr_REG, G0_CBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}



HI_BOOL HAL_CBM_SetCbmBkg(HI_U32 bMixerId, HAL_DISP_BKCOLOR_S *pstBkg)
{
    U_CBM_BKG2 CBM_BKG2;

    if(bMixerId == HAL_CBMMIX2)
    {
        CBM_BKG2.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBM_BKG2.u32));
        CBM_BKG2.bits.cbm_bkgy2  = HAL_GetBkg_BUMA(pstBkg->u8Bkg_y);
        CBM_BKG2.bits.cbm_bkgcb2 = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cb);
        CBM_BKG2.bits.cbm_bkgcr2 = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cr);        
        HAL_WriteReg((HI_U32*)&(pVoReg->CBM_BKG2.u32), CBM_BKG2.u32); 
    }  
    else
    {
        HAL_PRINT("Error mix id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_DISP_ClearIntStatus(HI_U32 u32IntMsk)
{
    /* read interrupt status */
    HAL_WriteReg((HI_U32*)&(pVoReg->VOMSKINTSTA.u32), u32IntMsk);
    return HI_TRUE;
}

HI_BOOL HAL_CBM_SetCbmMixerPrio(HAL_DISP_LAYER_E enLayer, HI_U8 u8Prio, HI_U8 u8MixerId)
{
    //U_CBM_MIX1 CBM_MIX1;
    //U_CBMBKG2 CBMBKG2;
    U_CBM_MIX2 CBM_MIX2;
    //U_MIXDSD_MIX MIXDSD_MIX;
    HI_U8  u8LayerId = 0;
	
    //---------------------------------------------------------------
    if(u8MixerId == HAL_CBMMIX2)
    {
        //check layer availability
        switch(enLayer)
        {

            case HAL_DISP_LAYER_VHD1:
            {
                u8LayerId = 0x9;
                break;
            }
            case HAL_DISP_LAYER_GFX2:
            {
                u8LayerId = 0xa;
                break;
            }
            default:
            {
                HAL_PRINT("Error layer id %d found in %s: L%d\n",enLayer,__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        //set mixer prio
        CBM_MIX2.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBM_MIX2.u32));
        switch(u8Prio)
        {
            case 6:
            {
                //clean all priority
                CBM_MIX2.bits.mixer_prio0 = 0;
                CBM_MIX2.bits.mixer_prio1 = 0;
                CBM_MIX2.bits.mixer_prio2 = 0;                
                CBM_MIX2.bits.mixer_prio3 = 0;
                break;
            }
            case 0:
            {
                CBM_MIX2.bits.mixer_prio0 = u8LayerId;
                break;
            }
            case 1:
            {
                CBM_MIX2.bits.mixer_prio1 = u8LayerId;
                break;
            }
            case 2:
            {
                CBM_MIX2.bits.mixer_prio2 = u8LayerId;
                break;
            }            
            case 3:
            {
                CBM_MIX2.bits.mixer_prio3 = u8LayerId;
                break;
            }
            default:
            {
                HAL_PRINT("Error priority id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        HAL_WriteReg((HI_U32*)&(pVoReg->CBM_MIX2.u32), CBM_MIX2.u32); 
    }
    //---------------------------------------------------------------
    else
    {
        HAL_PRINT("Error, u8MixerId %d Vou_SetCbmMixerPrio() Set a wrong CBMMIX\n",u8MixerId);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}


#ifdef __cplusplus
#if __cplusplus
 }
#endif
#endif /* End of #ifdef __cplusplus */

