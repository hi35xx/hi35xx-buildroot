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

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
#define HAL_PRINT printf
#define IO_ADDRESS(x) (x)

/***************************************/
#define CRG_REGS_ADDR   0x20030000
#define CRG_REGS_SIZE   0X10000

#define CRG_PERCTL2_ADDR (0x0008 + CRG_REGS_ADDR)
#define CRG_PERCTL3_ADDR (0x000C + CRG_REGS_ADDR)

#define CRG_PERCTL13_ADDR (0x0034 + CRG_REGS_ADDR)
#define CRG_PERCTL15_ADDR (0x003C + CRG_REGS_ADDR)

#define HDMI_REGS_ADDR  0x201f0000L
#define HDMI_REGS_SIZE  0X10000

#define HDMI_DE_DLY_ADDR        (0x32*4 + HDMI_REGS_ADDR)
#define HDMI_DE_CTRL_ADDR       (0x33*4 + HDMI_REGS_ADDR)
#define HDMI_DE_TOP_ADDR        (0x34*4 + HDMI_REGS_ADDR)
#define HDMI_DE_CNTH_ADDR       (0x37*4 + HDMI_REGS_ADDR)
#define HDMI_DE_CNTL_ADDR       (0x36*4 + HDMI_REGS_ADDR)
#define HDMI_DE_LINH_ADDR       (0x39*4 + HDMI_REGS_ADDR)
#define HDMI_DE_LINL_ADDR       (0x38*4 + HDMI_REGS_ADDR)
#define HDMI_HBIT_2HSYNC1_ADDR  (0x40*4 + HDMI_REGS_ADDR)
#define HDMI_HBIT_2HSYNC2_ADDR  (0x41*4 + HDMI_REGS_ADDR)
#define HDMI_FLD2_HS_OFSTH_ADDR (0x43*4 + HDMI_REGS_ADDR)
#define HDMI_FLD2_HS_OFSTL_ADDR (0x42*4 + HDMI_REGS_ADDR)
#define HDMI_HWIDTH1_ADDR       (0x44*4 + HDMI_REGS_ADDR)
#define HDMI_HWIDTH2_ADDR       (0x45*4 + HDMI_REGS_ADDR)
#define HDMI_VBIT_TO_VSYNC_ADDR (0x46*4 + HDMI_REGS_ADDR)
#define HDMI_VWIDTH_ADDR        (0x47*4 + HDMI_REGS_ADDR)
#define HDMI_VID_MODE_ADDR      (0x4a*4 + HDMI_REGS_ADDR)

/****************************************************************************
 * EXTERN VARIABLES                                                         *
 ****************************************************************************/

/****************************************************************************
 * GLOBAL VARIABLES                                                         *
 ****************************************************************************/
 volatile VOU_REGS_S *pVoReg = NULL;

/****************************************************************************
 * EXTERN FUNCTION DECLARATION                                              *
 ****************************************************************************/


/****************************************************************************
 * INTERNAL FUNCTION DECLARATION                                            *
 ****************************************************************************/

/****************************************************************************
 * VOU FUNCTION                                                             *
 *            ---- Assistant Function Area                                  *
 ****************************************************************************/


/*****************************************************************************
 Prototype         : Hal initial
 Description     :

*****************************************************************************/    
HI_VOID HAL_VOU_Init(HI_VOID)
{
#if 1
    pVoReg = (volatile VOU_REGS_S *)IO_ADDRESS(VO_BASE_ADDR);
#else
    pVoReg = (volatile VOU_REGS_S *)ioremap_nocache(VO_BASE_ADDR,sizeof(VOU_REGS_S));
#endif
}

HI_VOID HAL_VOU_Exit(HI_VOID)
{
#if 0
    iounmap(pVoReg);
#endif
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

HI_S32 HDMI_AV_Config_sep(HI_VOID)
{    
    HI_U8 u8DeDly = 0;
    HI_U8 u8DeTop = 0;
    HI_U8 u8DeCnt = 0;
    HI_U8 u8DeLin = 0;
    HI_U8 u8DeGen = 0;
    HI_U8 u8Hbit2Hsync = 0;
    HI_U8 u8Field2Ofst = 0;
    HI_U8 u8HWidth     = 0;
    HI_U8 u8Vbit2Vsync = 0;
    HI_U8 u8VWidth     = 0;
    HI_U8 u8Syncext    = 0;
    HI_U8 u8Demux      = 0;
    HI_U8 u8Upsmp      = 0;
    HI_U8 u8Csc        = 1;
    HI_RegWrite32(0x28, 0xffffffff, IO_ADDRESS(0x205d1804));
    HI_RegWrite32(0x60, 0xffffffff, IO_ADDRESS(0x205d1820));
    HI_RegWrite32(0xa9, 0xffffffff, IO_ADDRESS(0x205d1808));
    HI_RegWrite32(0x40, 0xffffffff, IO_ADDRESS(0x205d180c));

    HI_RegWrite32(u8DeDly & 0xff, 0xffffffff, IO_ADDRESS(HDMI_DE_DLY_ADDR));
    HI_RegWrite32((u8DeGen << 6) | ((u8DeDly >> 8) & 0xff), 0xffffffff, IO_ADDRESS(HDMI_DE_CTRL_ADDR));
    HI_RegWrite32(u8DeTop, 0xffffffff, IO_ADDRESS(HDMI_DE_TOP_ADDR));
    HI_RegWrite32(u8DeCnt >> 8, 0xffffffff, IO_ADDRESS(HDMI_DE_CNTH_ADDR));
    HI_RegWrite32(u8DeCnt & 0xff, 0xffffffff, IO_ADDRESS(HDMI_DE_CNTL_ADDR));
    HI_RegWrite32(u8DeLin >> 8, 0xffffffff, IO_ADDRESS(HDMI_DE_LINH_ADDR));
    HI_RegWrite32(u8DeLin & 0xff, 0xffffffff, IO_ADDRESS(HDMI_DE_LINL_ADDR));
    HI_RegWrite32(u8Hbit2Hsync & 0xff, 0xffffffff, IO_ADDRESS(HDMI_HBIT_2HSYNC1_ADDR));
    HI_RegWrite32(u8Hbit2Hsync >> 8, 0xffffffff, IO_ADDRESS(HDMI_HBIT_2HSYNC2_ADDR));
    HI_RegWrite32(u8Field2Ofst >> 8, 0xffffffff, IO_ADDRESS(HDMI_FLD2_HS_OFSTH_ADDR));
    HI_RegWrite32(u8Field2Ofst, 0xff, IO_ADDRESS(HDMI_FLD2_HS_OFSTL_ADDR));
    HI_RegWrite32(u8HWidth & 0xff, 0xffffffff, IO_ADDRESS(HDMI_HWIDTH1_ADDR));
    HI_RegWrite32(u8HWidth >> 8, 0xffffffff, IO_ADDRESS(HDMI_HWIDTH2_ADDR));
    HI_RegWrite32(u8Vbit2Vsync, 0xffffffff, IO_ADDRESS(HDMI_VBIT_TO_VSYNC_ADDR));
    HI_RegWrite32(u8VWidth, 0xffffffff, IO_ADDRESS(HDMI_VWIDTH_ADDR));
    HI_RegWrite32((u8Csc << 3) | (u8Upsmp << 2) | (u8Demux << 1) | u8Syncext,
        0xffffffff, IO_ADDRESS(HDMI_VID_MODE_ADDR));

    HI_RegWrite32(0x21, 0xffffffff, IO_ADDRESS(0x205d04bc));
    HI_RegWrite32(0x35, 0xffffffff, IO_ADDRESS(0x205d0020));

    return 0;
}


HI_S32 SYS_HAL_SelVoBt1120PinConifg(HI_VOID)
{
#if HICHIP == HI3521_V100
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f0098));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f009c));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00a0));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00a4));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00a8));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00ac));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00b0));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00b4));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00b8));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00bc));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00c0));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00c4));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00c8));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00cc));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00d0));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00d4));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f00d8));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f0130));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f0134));
#elif HICHIP == HI3520A_V100
    
#else
#endif

    return 0;
}

HI_S32 SYS_HAL_SelVoVgaPinConifg(HI_VOID)
{
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f000c));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f0010));
    return 0;
}

HI_S32 SYS_HAL_SelVoHdmiPinConifg(HI_VOID)
{

    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f0078));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f007c));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f0080));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x200f0084));
    return 0;
}

HI_S32 SYS_HAL_SetVoHdmiReset(HI_BOOL bReset)
{
    if (bReset)
    {
        HI_RegWrite32(0x01, 0xffffffff, IO_ADDRESS(CRG_PERCTL15_ADDR));
    }
    else
    {
        HI_RegWrite32(0x0c, 0xffffffff, IO_ADDRESS(CRG_PERCTL15_ADDR));
    }
    return 0;
}

HI_S32 SYS_HAL_SelVoBt1120Clk(HI_U32 u32ClkSel)
{
    //HI_RegSetBit(u32ClkSel, 27, IO_ADDRESS(CRG_PERCTL14_ADDR));

    return 0;
}
HI_S32 SYS_HAL_SelVoHdDacClk(HI_U32 u32ClkSel)
{
    //HI_RegWrite32(u32ClkSel << 25, 0x3 << 25, IO_ADDRESS(CRG_PERCTL14_ADDR));

    return 0;
}
HI_S32 SYS_HAL_SelVoHddateClk(HI_U32 u32ClkSel)
{
    //HI_RegSetBit(u32ClkSel, 24, IO_ADDRESS(CRG_PERCTL14_ADDR));

    return 0;
}
HI_S32 SYS_HAL_SelVoHDMIClk(HI_U32 u32ClkSel)
{
    //HI_RegSetBit(u32ClkSel, 22, IO_ADDRESS(CRG_PERCTL14_ADDR));

    return 0;
}
HI_S32 SYS_HAL_SelVoHdClk(HI_S32 VoDev, HI_U32 u32Pll)
{
    return 0;
}

HI_S32 SYS_HAL_SetVoPllFrac(HI_S32 s32Pll, HI_U32 u32BitsSet)
{
    if (0 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 0, 0xffffff << 0, IO_ADDRESS(CRG_PERCTL2_ADDR));
    }
    else
    {
        return -1;
    }
    return 0;
}
HI_S32 SYS_HAL_SetVoPllPostdiv1(HI_S32 s32Pll, HI_U32 u32BitsSet)
{
    if (0 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 24, 0x7 << 24, IO_ADDRESS(CRG_PERCTL2_ADDR));
    }
    else
    {
        return -1;
    }
    return 0;
}
HI_S32 SYS_HAL_SetVoPllPostdiv2(HI_S32 s32Pll, HI_U32 u32BitsSet)
{
    if (0 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 27, 0x7 << 27, IO_ADDRESS(CRG_PERCTL2_ADDR));
    }
    else
    {
        return -1;
    }
    return 0;
}
HI_S32 SYS_HAL_SetVoPllRefdiv(HI_S32 s32Pll, HI_U32 u32BitsSet)
{
    if (0 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 12, 0x3f << 12, IO_ADDRESS(CRG_PERCTL3_ADDR));
    }
    else
    {
        return -1;
    }
    return 0;
}
HI_S32 SYS_HAL_SetVoPllFbdiv(HI_S32 s32Pll, HI_U32 u32BitsSet)
{
    if (0 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 0, 0xfff << 0, IO_ADDRESS(CRG_PERCTL3_ADDR));
    }
    else
    {
        return -1;
    }
    return 0;
}

HI_S32 SYS_HAL_VouSdResetSel(HI_BOOL bReset)
{
    HI_U32 u32Tmp = (bReset == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 2, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouHdResetSel(HI_S32 s32VoDev, HI_BOOL bReset)
{
    HI_U32 u32Tmp = (bReset == HI_TRUE) ? 1 : 0;
    
    if (0 == s32VoDev)
    {
        HI_RegSetBit(u32Tmp, 1, IO_ADDRESS(CRG_PERCTL13_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}

HI_S32 SYS_HAL_VouHdDateResetSel(HI_BOOL bReset)
{
    return 0;
}

HI_S32 SYS_HAL_VouBusResetSel(HI_BOOL bReset)
{
    HI_U32 u32Tmp = (bReset == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 0, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouDevClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;  
    HI_U32 u32Bit;

    if (0 == s32VoDev)
    {
        HI_RegSetBit(u32Tmp, 9, IO_ADDRESS(CRG_PERCTL13_ADDR));
    }
    else if (s32VoDev >= 1 && s32VoDev <= 2)
    {        
        u32Bit = 10 + (s32VoDev - 1);  
        HI_RegSetBit(u32Tmp, u32Bit, IO_ADDRESS(CRG_PERCTL13_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}

HI_S32 SYS_HAL_VouHdOutClkSel(HI_S32 s32Dev, HI_U32 u32ClkSel)
{
    if (0 == s32Dev)
    {        
        HI_RegSetBit(u32ClkSel, 12, IO_ADDRESS(CRG_PERCTL13_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}

HI_S32 SYS_HAL_VouSdDateClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn)
{
#if 0
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;  
    HI_U32 u32Bit;

    if (s32VoDev >= 1 && s32VoDev <= 2)
    {        
        u32Bit = 14 - (s32VoDev - 1);
        HI_RegSetBit(u32Tmp, u32Bit, IO_ADDRESS(CRG_PERCTL13_ADDR));
    }
    else
    {
        return -1;
    }
#endif
    return 0;
}


HI_S32 SYS_HAL_VouHdDateClkEn(HI_BOOL pbClkEn)
{
    return 0;
}

HI_S32 SYS_HAL_VouBusClkEn(HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 8, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}


HI_S32 SYS_HAL_VouSdDacClkEn(HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 7, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}


HI_S32 SYS_HAL_VouHdDacClkEn(HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 6, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}


HI_S32 SYS_HAL_HdmiResetSel(HI_BOOL bReset)
{
    HI_U32 u32Tmp = (bReset == HI_TRUE) ? 1 : 0;
     
    HI_RegSetBit(u32Tmp, 0, IO_ADDRESS(CRG_PERCTL15_ADDR));
        
    return 0;
}

HI_S32 SYS_HAL_HdmiPixelClkEn(HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;
          
    HI_RegSetBit(u32Tmp, 3, IO_ADDRESS(CRG_PERCTL15_ADDR));
        
    return 0;
}

HI_S32 SYS_HAL_HdmiBusClkEn(HI_BOOL pbClkEn)
{
    HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;
          
    HI_RegSetBit(u32Tmp, 2, IO_ADDRESS(CRG_PERCTL15_ADDR));
        
    return 0;
}

HI_VOID HAL_SYS_Control(HI_VOID)
{
    U_VOCTRL VOCTRL;

    /* outstand */
    VOCTRL.u32 = pVoReg->VOCTRL.u32;
    VOCTRL.u32 = 0x80021220;
    pVoReg->VOCTRL.u32 = VOCTRL.u32;
}


HI_U32 inline HAL_GetXDC_BUMA(HI_S32 s32Value)
{
    HI_U32 u32AbsValue = 0;

    if(s32Value >= 0)
    {
        return s32Value;
    }
    /*0~8bit有效，第8bit为符号位*/
    else
    {
        u32AbsValue = (-1)*s32Value;
        //return ( (((~u32AbsValue)+1)& 0xFF) | 0x100 );
        return ( ((~u32AbsValue)+1)& 0x1FF);
    }
}

HI_U32 inline HAL_Conver_CscCoef(HI_S32 s32Value)
{
    HI_S32 s32Result = ((s32Value << 8) / 1000);

    /* 保留问题! 当CSC矩阵计算出来的结果s32Value是-1、-2等值时，
    *   s32Result将是0，换句话说0.001转换成二进制是0x0000 0000 01
    *   按注释掉的代码计算，配寄存器的值是0x1000，这个值会导致图像为绿色(50 2 50 50)
    */
    if (s32Result < 0)
    //if (s32Value < 0)
    {
        s32Result = (~((-1)*s32Result) + 1);

        /* 5.8 format */
        s32Result = (s32Result & 0x1fff) | 0x1000;
    }
    else
    {
        s32Result = s32Result & 0x1fff;
    }
    
    return s32Result;
}

HI_U32 Vou_GetAbsAddr(HAL_DISP_LAYER_E enLayer, HI_U32 pReg)
{
    HI_U32 RegAbsAddr;
    switch(enLayer)
    {
        case HAL_DISP_LAYER_VHD0:
        {
            RegAbsAddr = (pReg) + enLayer * VHD_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_VSD0:
        {
            RegAbsAddr = (pReg) + (enLayer + 2) * VHD_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_VSD1:
        {
            RegAbsAddr = (pReg) + (enLayer - HAL_DISP_LAYER_VSD0) * VHD_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_GFX0:
        {
            RegAbsAddr = ((pReg)) + (enLayer - HAL_DISP_LAYER_GFX0) * GFX_REG_LEN;
            break;
        }
        //case HAL_DISP_LAYER_GFX1:
        case HAL_DISP_LAYER_GFX2:
        case HAL_DISP_LAYER_GFX3:
        {
            RegAbsAddr = ((pReg)) + (enLayer - HAL_DISP_LAYER_GFX0 + 1) * GFX_REG_LEN;
            break;
        }
        //case HAL_DISP_LAYER_GFX4:
        case HAL_DISP_LAYER_HC0:
        //case HAL_DISP_LAYER_HC1:
        {
            RegAbsAddr = ((pReg)) + (enLayer - HAL_DISP_LAYER_GFX0 + 2) * GFX_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_WBC:
        {
            RegAbsAddr = (pReg) + (enLayer - HAL_DISP_LAYER_WBC) * WBC_REG_LEN;
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
        case HAL_DISP_CHANNEL_DHD0:
        //case HAL_DISP_CHANNEL_DHD1:
        {
            RegAbsAddr =pReg + enChan * DHD_REG_LEN;
            break;
        }
        case HAL_DISP_CHANNEL_DSD0:
        case HAL_DISP_CHANNEL_DSD1:
        //case HAL_DISP_CHANNEL_DSD2:
        //case HAL_DISP_CHANNEL_DSD3:
        //case HAL_DISP_CHANNEL_DSD4:
        //case HAL_DISP_CHANNEL_DSD5:
        {
            RegAbsAddr =pReg + (enChan - CHANNEL_DSD_START) * DSD_REG_LEN;
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


HI_VOID HAL_SYS_SetArbMode(HI_U32 bMode)
{
    volatile U_VOCTRL VOCTRL;

    VOCTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOCTRL.u32));
    VOCTRL.bits.arb_mode = bMode;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOCTRL.u32), VOCTRL.u32); 

    return ;
}

HI_VOID HAL_SYS_SetRdBusId(HI_U32 bMode)
{
    U_VOCTRL VOCTRL;

    VOCTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOCTRL.u32));
    VOCTRL.bits.vo_id_sel = bMode;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOCTRL.u32), VOCTRL.u32); 
    
    return ;
}

HI_BOOL HAL_DISP_SetIntfEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL enIntf)
{
    volatile U_DHDCTRL DHDCTRL;
    volatile U_DSDCTRL DSDCTRL;
    volatile HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCTRL.bits.intf_en = enIntf;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCTRL.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCTRL.u32));
        DSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCTRL.bits.intf_en = enIntf;
        HAL_WriteReg((HI_U32*)addr_REG, DSDCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_GetIntfEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL *pbIntfEn)
{
    volatile U_DHDCTRL DHDCTRL;
    volatile U_DSDCTRL DSDCTRL;
    
    volatile HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pbIntfEn = DHDCTRL.bits.intf_en;
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCTRL.u32));
        DSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pbIntfEn = DSDCTRL.bits.intf_en;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;    
}

HI_BOOL HAL_DISP_GetIntState(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL *pbBottom)
{
    volatile U_DHDSTATE DHDSTATE;
    volatile U_DSDSTATE DSDSTATE;
    
    volatile HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDSTATE.u32));
        DHDSTATE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pbBottom = DHDSTATE.bits.bottom_field;
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDSTATE.u32));
        DSDSTATE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pbBottom = DSDSTATE.bits.bottom_field;
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
    volatile U_DHDCTRL DHDCTRL;
    volatile U_DHDVSYNC DHDVSYNC;
    volatile U_DHDVPLUS DHDVPLUS;
    volatile U_DHDPWR DHDPWR;
    volatile U_DSDCTRL DSDCTRL;
    volatile U_DSDVSYNC DSDVSYNC;
    volatile U_DSDVPLUS DSDVPLUS;
    volatile U_DSDPWR DSDPWR;
    volatile HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCTRL.bits.iop   = pstSyncInfo->bIop;
        DHDCTRL.bits.intfb = pstSyncInfo->u8Intfb;
        DHDCTRL.bits.synm  = pstSyncInfo->bSynm;
        DHDCTRL.bits.idv   = pstSyncInfo->bIdv;
        DHDCTRL.bits.ihs   = pstSyncInfo->bIhs;
        DHDCTRL.bits.ivs   = pstSyncInfo->bIvs;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCTRL.u32); 


        if (DHDCTRL.bits.intfb == 0)
        {
            volatile U_DHDHSYNC1 DHDHSYNC1;
            volatile U_DHDHSYNC2 DHDHSYNC2;
            
            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDHSYNC1.u32));
            DHDHSYNC1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DHDHSYNC1.bits.hact = pstSyncInfo->u16Hact -1;
            DHDHSYNC1.bits.hbb  = (pstSyncInfo->u16Hbb)*2-1;
            HAL_WriteReg((HI_U32*)addr_REG, DHDHSYNC1.u32); 

            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDHSYNC2.u32));
            DHDHSYNC2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DHDHSYNC2.bits.hmid = (pstSyncInfo->u16Hmid)-1;
            DHDHSYNC2.bits.hfb  = (pstSyncInfo->u16Hfb)*2-1;
            HAL_WriteReg((HI_U32*)addr_REG, DHDHSYNC2.u32); 
        }
        else
        {
            volatile U_DHDHSYNC1 DHDHSYNC1;
            volatile U_DHDHSYNC2 DHDHSYNC2;
            
            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDHSYNC1.u32));
            DHDHSYNC1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DHDHSYNC1.bits.hact = pstSyncInfo->u16Hact -1;
            DHDHSYNC1.bits.hbb  = pstSyncInfo->u16Hbb -1;
            HAL_WriteReg((HI_U32*)addr_REG, DHDHSYNC1.u32); 

            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDHSYNC2.u32));
            DHDHSYNC2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DHDHSYNC2.bits.hmid = (pstSyncInfo->u16Hmid)-1;
            DHDHSYNC2.bits.hfb  = pstSyncInfo->u16Hfb -1;
            HAL_WriteReg((HI_U32*)addr_REG, DHDHSYNC2.u32); 
        }

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDVSYNC.u32));
        DHDVSYNC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDVSYNC.bits.vact = pstSyncInfo->u16Vact  -1;
        DHDVSYNC.bits.vbb = pstSyncInfo->u16Vbb - 1;
        DHDVSYNC.bits.vfb =  pstSyncInfo->u16Vfb - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DHDVSYNC.u32); 
        
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDVPLUS.u32));
        DHDVPLUS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDVPLUS.bits.bvact = pstSyncInfo->u16Bvact - 1;
        DHDVPLUS.bits.bvbb = pstSyncInfo->u16Bvbb - 1;
        DHDVPLUS.bits.bvfb =  pstSyncInfo->u16Bvfb - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DHDVPLUS.u32); 

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDPWR.u32));
        DHDPWR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDPWR.bits.hpw = pstSyncInfo->u16Hpw - 1;
        DHDPWR.bits.vpw = pstSyncInfo->u16Vpw - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DHDPWR.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCTRL.u32));
        DSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCTRL.bits.iop   = pstSyncInfo->bIop;
        DSDCTRL.bits.intfb = pstSyncInfo->u8Intfb;
        DSDCTRL.bits.synm  = pstSyncInfo->bSynm;
        DSDCTRL.bits.idv   = pstSyncInfo->bIdv;
        DSDCTRL.bits.ihs   = pstSyncInfo->bIhs;
        DSDCTRL.bits.ivs   = pstSyncInfo->bIvs;
        HAL_WriteReg((HI_U32*)addr_REG, DSDCTRL.u32); 


        if (DSDCTRL.bits.intfb == 0)
        {
            volatile U_DSDHSYNC1 DSDHSYNC1;
            volatile U_DSDHSYNC2 DSDHSYNC2;
            
            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDHSYNC1.u32));
            DSDHSYNC1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DSDHSYNC1.bits.hact = pstSyncInfo->u16Hact -1;
            DSDHSYNC1.bits.hbb  = (pstSyncInfo->u16Hbb)*2-1;
            HAL_WriteReg((HI_U32*)addr_REG, DSDHSYNC1.u32); 

            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDHSYNC2.u32));
            DSDHSYNC2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DSDHSYNC2.bits.hfb  = (pstSyncInfo->u16Hfb)*2-1;
            HAL_WriteReg((HI_U32*)addr_REG, DSDHSYNC2.u32); 
        }
        else
        {
            volatile U_DSDHSYNC1 DSDHSYNC1;
            volatile U_DSDHSYNC2 DSDHSYNC2;
            
            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDHSYNC1.u32));
            DSDHSYNC1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DSDHSYNC1.bits.hact = pstSyncInfo->u16Hact -1;
            DSDHSYNC1.bits.hbb  = pstSyncInfo->u16Hbb -1;
            HAL_WriteReg((HI_U32*)addr_REG, DSDHSYNC1.u32); 

            addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDHSYNC2.u32));
            DSDHSYNC2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            DSDHSYNC2.bits.hfb  = pstSyncInfo->u16Hfb -1;
            HAL_WriteReg((HI_U32*)addr_REG, DSDHSYNC2.u32); 
        }

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDVSYNC.u32));
        DSDVSYNC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDVSYNC.bits.vact = pstSyncInfo->u16Vact  -1;
        DSDVSYNC.bits.vbb = pstSyncInfo->u16Vbb - 1;
        DSDVSYNC.bits.vfb =  pstSyncInfo->u16Vfb - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DSDVSYNC.u32); 
        
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDVPLUS.u32));
        DSDVPLUS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDVPLUS.bits.bvact = pstSyncInfo->u16Bvact - 1;
        DSDVPLUS.bits.bvbb = pstSyncInfo->u16Bvbb - 1;
        DSDVPLUS.bits.bvfb =  pstSyncInfo->u16Bvfb - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DSDVPLUS.u32); 

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDPWR.u32));
        DSDPWR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDPWR.bits.hpw = pstSyncInfo->u16Hpw - 1;
        DSDPWR.bits.vpw = pstSyncInfo->u16Vpw - 1;
        HAL_WriteReg((HI_U32*)addr_REG, DSDPWR.u32); 
    }
    else
    {
         HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
         return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL  HAL_DISP_GetDispIoP(HAL_DISP_OUTPUTCHANNEL_E enChan,HI_U32 *pu32iop)
{

    U_DHDCTRL DHDCTRL;
    U_DSDCTRL DSDCTRL;
    
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32iop = DHDCTRL.bits.iop;
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCTRL.u32));
        DSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32iop = DSDCTRL.bits.iop;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntfDataFmt(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_DISP_INTFDATAFMT_E stIntfDataFmt)
{
    volatile U_DHDCTRL DHDCTRL;

    volatile U_DSDCTRL DSDCTRL;

    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCTRL.bits.intfdm = stIntfDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCTRL.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCTRL.u32));
        DSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCTRL.bits.intfdm = stIntfDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, DSDCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetHdmiSel(HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    U_VOMUX VOMUX;

    switch(enChan)
    {
        case HAL_DISP_CHANNEL_DHD0:
        {
            VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
            VOMUX.bits.hdmi_vid_sel = 1;
            HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 

            break;
        }
        #if 0
        case HAL_DISP_CHANNEL_DHD1:
        {
            VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
            VOMUX.bits.hdmi_vid_sel = 0;
            HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 

            break;
        }
        #endif
        default:
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetHdmiVid(HI_U32 u32Value)
{
    U_VOMUX VOMUX;
    
    VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
    VOMUX.bits.hdmi_vid = u32Value;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 

    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetHddateInSel(HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVgaInSel(HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetBt1120Sel(HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    return  HI_TRUE;
}


HI_BOOL HAL_DISP_SetBt656MuxHdSel(HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    volatile U_VOMUX VOMUX;
    switch(enChan)
    {
        case HAL_DISP_CHANNEL_DHD0:
        {
            VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
            VOMUX.bits.bt656_mux_hd = 0;//高清选择BT1120输出
            HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 

            break;
        }
        case HAL_DISP_CHANNEL_DSD0:
        case HAL_DISP_CHANNEL_DSD1:
        {
            VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
            VOMUX.bits.bt656_mux_hd = 1;//BT1120管脚选择输出标清BT656信号
            HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 

            break;
        }
        default:
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    return  HI_TRUE;
}

/* 高清BT1120管脚作为标清BT656管脚输出标清数据时，选择标清设备 */
HI_BOOL HAL_DISP_SetBt656SelYc(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL bBt656H)
{
    volatile U_VOMUX VOMUX;

    if (HI_TRUE == bBt656H)
    {
        switch(enChan)
        {
            case HAL_DISP_CHANNEL_DSD0:
            {
                VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
                
                VOMUX.bits.bt656_sel_yc = 0;//高清BT1120高8位输出标清的bt656数据，0表连接到sd0
                HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 
        
                break;
            }
            case HAL_DISP_CHANNEL_DSD1:
            {
                VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
                VOMUX.bits.bt656_sel_yc = 1;//高清BT1120高8位输出标清的bt656数据， 1表连接到sd1
                HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 
        
                break;
            }
            default:
            {
                HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
    }
    else
    {
        switch(enChan)
        {
            case HAL_DISP_CHANNEL_DSD0:
            {
                VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
                
                VOMUX.bits.bt656_sel_yc = 1;//高清BT1120低8位输出标清的bt656数据，1表连接到sd0
                HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 
        
                break;
            }
            case HAL_DISP_CHANNEL_DSD1:
            {
                VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
                VOMUX.bits.bt656_sel_yc = 0;//高清BT1120低8位输出标清的bt656数据， 0表连接到sd1
                HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 
        
                break;
            }
            default:
            {
                HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }

    }
    
    return  HI_TRUE;
}

/* 标清BT656选择输出sd0/sd1 */
HI_BOOL HAL_DISP_SetBt656MuxSdSel(HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    volatile U_VOMUX VOMUX;
    switch(enChan)
    {
        case HAL_DISP_CHANNEL_DSD0:
        {
            VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
            VOMUX.bits.bt656_mux_sd = 0;
            HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 

            break;
        }
        case HAL_DISP_CHANNEL_DSD1:
        {
            VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
            VOMUX.bits.bt656_mux_sd = 1;
            HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32); 

            break;
        }
        default:
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    return  HI_TRUE;
}
HI_BOOL HAL_DISP_SetDac1Sel(HAL_DAC_SEL_E DacSel)
{
    #if 0
    volatile U_VOMUX VOMUX;

    VOMUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMUX.u32));
    VOMUX.bits.dac1_sel = DacSel;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOMUX.u32), VOMUX.u32);
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_DISP_VgaDacEn(HI_BOOL bEn)
{
#if 0
    volatile U_DACCTRL3_5 DACCTRL3_5;

    DACCTRL3_5.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->DACCTRL3_5.u32));
    DACCTRL3_5.bits.endac3 = bEn;
    DACCTRL3_5.bits.endac4 = bEn;
    DACCTRL3_5.bits.endac5 = bEn;
    DACCTRL3_5.bits.envbg = bEn;
    HAL_WriteReg((HI_U32*)&(pVoReg->DACCTRL3_5.u32), DACCTRL3_5.u32);
#endif    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_CvbsDacEn(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL bEn)
{
    volatile U_DACCTRL0_2 DACCTRL0_2;

    DACCTRL0_2.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->DACCTRL0_2.u32));
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {        
        DACCTRL0_2.bits.endac0 = bEn;
        DACCTRL0_2.bits.envbg = bEn;
    }
    else if (HAL_DISP_CHANNEL_DSD1 == enChan)
    {
        DACCTRL0_2.bits.endac1 = bEn;
        DACCTRL0_2.bits.envbg = bEn;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    HAL_WriteReg((HI_U32*)&(pVoReg->DACCTRL0_2.u32), DACCTRL0_2.u32);
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVgaGc(HI_U32 u32Value)
{
    volatile U_DACCTRL3_5 DACCTRL3_5;

    DACCTRL3_5.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->DACCTRL3_5.u32));
    DACCTRL3_5.bits.dac3gc = u32Value;
    DACCTRL3_5.bits.dac4gc = u32Value;
    DACCTRL3_5.bits.dac5gc = u32Value;
    DACCTRL3_5.bits.envbg = 1;
    HAL_WriteReg((HI_U32*)&(pVoReg->DACCTRL3_5.u32), DACCTRL3_5.u32);
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCvbsGc(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Value)
{
    volatile U_DACCTRL0_2 DACCTRL0_2;

    DACCTRL0_2.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->DACCTRL0_2.u32));
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {        
        DACCTRL0_2.bits.dac0gc = u32Value;
        DACCTRL0_2.bits.envbg = 1;
    }
    else if (HAL_DISP_CHANNEL_DSD1 == enChan)
    {
        DACCTRL0_2.bits.dac1gc = u32Value;
        DACCTRL0_2.bits.envbg = 1;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    HAL_WriteReg((HI_U32*)&(pVoReg->DACCTRL0_2.u32), DACCTRL0_2.u32);
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVgaCSCEn(HI_BOOL bCscEn)
{   
    U_VGACSCIDC VGACSCIDC;

    VGACSCIDC.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCIDC.u32);
    VGACSCIDC.bits.csc_en = bCscEn;
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCIDC.u32, VGACSCIDC.u32); 
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVgaCscCoef(CscCoef_S *pstCoef)
{    
    U_VGACSCIDC VGACSCIDC;
    U_VGACSCODC VGACSCODC;
    U_VGACSCP0  VGACSCP0;
    U_VGACSCP1  VGACSCP1;
    U_VGACSCP2  VGACSCP2;
    U_VGACSCP3  VGACSCP3;
    U_VGACSCP4  VGACSCP4;
    
    VGACSCIDC.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCIDC.u32);
    VGACSCIDC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc2);
    VGACSCIDC.bits.cscidc1 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc1);
    VGACSCIDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc0);
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCIDC.u32, VGACSCIDC.u32); 

    VGACSCODC.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCODC.u32);
    VGACSCODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc2);
    VGACSCODC.bits.cscodc1 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc1);
    VGACSCODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc0);
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCODC.u32, VGACSCODC.u32); 

    VGACSCP0.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCP0.u32);
    VGACSCP0.bits.cscp00 = HAL_Conver_CscCoef(pstCoef->csc_coef00);
    VGACSCP0.bits.cscp01 = HAL_Conver_CscCoef(pstCoef->csc_coef01);
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCP0.u32, VGACSCP0.u32); 

    VGACSCP1.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCP1.u32);
    VGACSCP1.bits.cscp02 = HAL_Conver_CscCoef(pstCoef->csc_coef02);
    VGACSCP1.bits.cscp10 = HAL_Conver_CscCoef(pstCoef->csc_coef10);
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCP1.u32, VGACSCP1.u32); 

    VGACSCP2.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCP2.u32);
    VGACSCP2.bits.cscp11 = HAL_Conver_CscCoef(pstCoef->csc_coef11);
    VGACSCP2.bits.cscp12 = HAL_Conver_CscCoef(pstCoef->csc_coef12);
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCP2.u32, VGACSCP2.u32); 

    VGACSCP3.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCP3.u32);
    VGACSCP3.bits.cscp20 = HAL_Conver_CscCoef(pstCoef->csc_coef20);
    VGACSCP3.bits.cscp21 = HAL_Conver_CscCoef(pstCoef->csc_coef21);
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCP3.u32, VGACSCP3.u32); 

    VGACSCP4.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCP4.u32);
    VGACSCP4.bits.cscp22 = HAL_Conver_CscCoef(pstCoef->csc_coef22);
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCP4.u32, VGACSCP4.u32); 

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVgaCscMode(HI_BOOL bIsVgaModeBy709)
{
    U_VGACSCIDC VGACSCIDC;
    
    VGACSCIDC.u32 = HAL_ReadReg((HI_U32*)&pVoReg->VGACSCIDC.u32);
    VGACSCIDC.bits.csc_mode = bIsVgaModeBy709;    
    HAL_WriteReg((HI_U32*)&pVoReg->VGACSCIDC.u32, VGACSCIDC.u32);
    return HI_TRUE;
}

HI_BOOL  HAL_DISP_SetIntfDfirEn(HAL_DISP_OUTPUTCHANNEL_E enChan,HI_U32 dfir_en)
{   
    volatile U_DHDCLIPL DHDCLIPL;
    volatile U_DSDCLIPL DSDCLIPL;

    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCLIPL.u32));
        DHDCLIPL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCLIPL.bits.dfir_en  = dfir_en;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCLIPL.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCLIPL.u32));
        DSDCLIPL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCLIPL.bits.dfir_en  = dfir_en;
        HAL_WriteReg((HI_U32*)addr_REG, DSDCLIPL.u32);        
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    
    
    return HI_TRUE;
}
HI_BOOL  HAL_DISP_SetIntfClip(HAL_DISP_OUTPUTCHANNEL_E enChan,
                                        HI_BOOL enClip,
                                        HAL_DISP_CLIP_S *pstClipData)
{   
    volatile U_DHDCLIPL DHDCLIPL;
    volatile U_DHDCLIPH DHDCLIPH;
    volatile U_DSDCLIPL DSDCLIPL;
    volatile U_DSDCLIPH DSDCLIPH;

    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCLIPL.u32));
        DHDCLIPL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCLIPL.bits.clipen  = enClip;
        DHDCLIPL.bits.clipcl2 = pstClipData->u16ClipLow_y;
        DHDCLIPL.bits.clipcl1 = pstClipData->u16ClipLow_cb;
        DHDCLIPL.bits.clipcl0 = pstClipData->u16ClipLow_cr;        
        HAL_WriteReg((HI_U32*)addr_REG, DHDCLIPL.u32); 

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCLIPH.u32));
        DHDCLIPH.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCLIPH.bits.clipch2 = pstClipData->u16ClipHigh_y;
        DHDCLIPH.bits.clipch1 = pstClipData->u16ClipHigh_cb;
        DHDCLIPH.bits.clipch0 = pstClipData->u16ClipHigh_cr;        
        HAL_WriteReg((HI_U32*)addr_REG, DHDCLIPH.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCLIPL.u32));
        DSDCLIPL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCLIPL.bits.clipen  = enClip;
        DSDCLIPL.bits.clipcl2 = pstClipData->u16ClipLow_y;
        DSDCLIPL.bits.clipcl1 = pstClipData->u16ClipLow_cb;
        DSDCLIPL.bits.clipcl0 = pstClipData->u16ClipLow_cr;        
        HAL_WriteReg((HI_U32*)addr_REG, DSDCLIPL.u32); 

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCLIPH.u32));
        DSDCLIPH.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCLIPH.bits.clipch2 = pstClipData->u16ClipHigh_y;
        DSDCLIPH.bits.clipch1 = pstClipData->u16ClipHigh_cb;
        DSDCLIPH.bits.clipch0 = pstClipData->u16ClipHigh_cr;        
        HAL_WriteReg((HI_U32*)addr_REG, DSDCLIPH.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVtThdMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 uFieldMode)
{
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        U_DHDVTTHD DHDVTTHD;

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDVTTHD.u32));
        DHDVTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDVTTHD.bits.thd1_mode = uFieldMode;
        HAL_WriteReg((HI_U32*)addr_REG, DHDVTTHD.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        U_DSDVTTHD DSDVTTHD;

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDVTTHD.u32));
        DSDVTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDVTTHD.bits.thd1_mode = uFieldMode;
        HAL_WriteReg((HI_U32*)addr_REG, DSDVTTHD.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_GetVtThdMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 *pu32FieldMode)
{
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        U_DHDVTTHD DHDVTTHD;

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDVTTHD.u32));
        DHDVTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32FieldMode = DHDVTTHD.bits.thd1_mode;
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        U_DSDVTTHD DSDVTTHD;

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDVTTHD.u32));
        DSDVTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32FieldMode = DSDVTTHD.bits.thd1_mode;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVtThd(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 vtthd)
{
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        volatile U_DHDVTTHD DHDVTTHD;

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDVTTHD.u32));
        DHDVTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDVTTHD.bits.vtmgthd1 = vtthd;
        HAL_WriteReg((HI_U32*)addr_REG, DHDVTTHD.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        volatile U_DSDVTTHD DSDVTTHD;

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDVTTHD.u32));
        DSDVTTHD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDVTTHD.bits.vtmgthd1 = vtthd;
        HAL_WriteReg((HI_U32*)addr_REG, DSDVTTHD.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE; 
    }
    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetGammaEnable(HAL_DISP_OUTPUTCHANNEL_E enChn, HI_U32 uGmmEn)
{
    U_DHDCTRL DHDCTRL;
    
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChn) && (enChn <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChn,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCTRL.bits.gmmen = uGmmEn;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetGammaCoef(HAL_DISP_OUTPUTCHANNEL_E enChn, HI_U32 *upTable)
{
    U_DHDGAMMAN DHDGAMMAN[33];
    //U_DHD1GAMMAN DHD1GAMMAN[33];
    HI_U32 ii = 0;

    if(enChn == HAL_DISP_CHANNEL_DHD0)
    {
        for (ii = 0; ii < 33; ii++)
        {
            DHDGAMMAN[ii].bits.gamma_datarn = upTable[ii*3 + 33*0];
            DHDGAMMAN[ii].bits.gamma_datagn = upTable[ii*3 + 33*1];
            DHDGAMMAN[ii].bits.gamma_databn = upTable[ii*3 + 33*2];
            HAL_WriteReg((HI_U32 *)&(pVoReg->DHDGAMMAN[ii].u32), DHDGAMMAN[ii].u32); 
        }
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetGammaAddr(HAL_DISP_OUTPUTCHANNEL_E enChn, HI_U32 uGmmAddr)
{
    #if 0
    U_DHDGAMMAAD DHDGAMMAAD;


    if(enChn == HAL_DISP_CHANNEL_DHD0)
    {
        // Set Vou Dhd Channel Gamma Correct Enable
        DHDGAMMAAD.u32 = uGmmAddr;
        HAL_WriteReg((HI_U32 *)&(pVoReg->DHDGAMMAAD.u32), DHDGAMMAAD.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif

    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetDispParaUpd(HAL_DISP_OUTPUTCHANNEL_E enChan,
                                  HAL_DISP_COEFMODE_E enMode)
{
    #if 0
    U_VOPARAUP VOPARAUP;

    if(enChan == HAL_DISP_CHANNEL_DHD0)
    {
        VOPARAUP.u32 = HAL_ReadReg((HI_U32 *)&(pVoReg->VOPARAUP.u32));
        if (enMode == HAL_DISP_COEFMODE_ACC || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.video_acc_upd = 0x1;
        }

        if (enMode == HAL_DISP_COEFMODE_GAM || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.dhd_gamma_upd = 0x1;
        }
        HAL_WriteReg((HI_U32 *)&(pVoReg->VOPARAUP.u32), VOPARAUP.u32); 
    }
    else
    {
        HAL_PRINT("Error, Wrong coef update layer select\n");
        return HI_FALSE;
    }

    pVoReg->VOPARAUP.bits.video_acc_upd = 0x0;
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntMask(HI_U32 u32MaskEn)
{
    U_VOINTMSK VOINTMSK;

    VOINTMSK.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOINTMSK.u32));
    VOINTMSK.u32 = VOINTMSK.u32 | u32MaskEn;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOINTMSK.u32), VOINTMSK.u32); 

    return HI_TRUE;
}

HI_BOOL  HAL_DISP_ClrIntMask(HI_U32 u32MaskEn)
{
    U_VOINTMSK VOINTMSK;

    VOINTMSK.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOINTMSK.u32));
    VOINTMSK.u32 = VOINTMSK.u32 & (~u32MaskEn);
    HAL_WriteReg((HI_U32*)&(pVoReg->VOINTMSK.u32), VOINTMSK.u32);

    return HI_TRUE;
}

HI_U32 HAL_DISP_GetIntStatus(HI_U32 u32IntMsk)
{
    volatile U_VOMSKINTSTA VOMSKINTSTA;
    
    VOMSKINTSTA.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOMSKINTSTA.u32));
    
    return (VOMSKINTSTA.u32 & u32IntMsk);
}

HI_BOOL HAL_DISP_ClearIntStatus(HI_U32 u32IntMsk)
{
    HAL_WriteReg((HI_U32*)&(pVoReg->VOMSKINTSTA.u32), u32IntMsk);
    return HI_TRUE;
}

HI_BOOL  HAL_DISP_SetIntfClipEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bEnable)
{
    U_DHDCLIPL DHDCLIPL;
    U_DSDCLIPL DSDCLIPL;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCLIPL.u32));
        DHDCLIPL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCLIPL.bits.clipen = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCLIPL.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCLIPL.u32));
        DSDCLIPL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCLIPL.bits.clipen = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, DSDCLIPL.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVSync(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 VFB, HI_U32 VBB, HI_U32 VACT)
{
    U_DHDVSYNC DHDVSYNC;
    U_DSDVSYNC DSDVSYNC;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDVSYNC.u32));
        DHDVSYNC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDVSYNC.bits.vact = VACT;
        DHDVSYNC.bits.vbb = VBB;
        DHDVSYNC.bits.vfb = VFB;
        HAL_WriteReg((HI_U32*)addr_REG, DHDVSYNC.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDVSYNC.u32));
        DSDVSYNC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDVSYNC.bits.vact = VACT;
        DSDVSYNC.bits.vbb = VBB;
        DSDVSYNC.bits.vfb = VFB;
        HAL_WriteReg((HI_U32*)addr_REG, DSDVSYNC.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVSyncPlus(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 BVFB, HI_U32 BVBB, HI_U32 BVACT)
{
    U_DHDVPLUS DHDVPLUS;
    U_DSDVPLUS DSDVPLUS;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDVPLUS.u32));
        DHDVPLUS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDVPLUS.bits.bvact = BVACT;
        DHDVPLUS.bits.bvbb = BVBB;
        DHDVPLUS.bits.bvfb = BVFB;
        HAL_WriteReg((HI_U32*)addr_REG, DHDVPLUS.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDVPLUS.u32));
        DSDVPLUS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDVPLUS.bits.bvact = BVACT;
        DSDVPLUS.bits.bvbb = BVBB;
        DSDVPLUS.bits.bvfb = BVFB;
        HAL_WriteReg((HI_U32*)addr_REG, DSDVPLUS.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetHSync(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 HFB, HI_U32 HBB, HI_U32 HACT)
{
    U_DHDHSYNC1 DHDHSYNC1;
    U_DHDHSYNC2 DHDHSYNC2;
    U_DSDHSYNC1 DSDHSYNC1;
    U_DSDHSYNC2 DSDHSYNC2;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDHSYNC1.u32));
        DHDHSYNC1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDHSYNC1.bits.hact = HACT;
        DHDHSYNC1.bits.hbb = HBB;
        HAL_WriteReg((HI_U32*)addr_REG, DHDHSYNC1.u32); 

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDHSYNC2.u32));
        DHDHSYNC2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDHSYNC2.bits.hfb = HFB;
        HAL_WriteReg((HI_U32*)addr_REG, DHDHSYNC2.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDHSYNC1.u32));
        DSDHSYNC1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDHSYNC1.bits.hact = HACT;
        DSDHSYNC1.bits.hbb = HBB;
        HAL_WriteReg((HI_U32*)addr_REG, DSDHSYNC1.u32); 

        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDHSYNC2.u32));
        DSDHSYNC2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDHSYNC2.bits.hfb = HFB;
        HAL_WriteReg((HI_U32*)addr_REG, DSDHSYNC2.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetPlusWidth(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 HPW, HI_U32 VPW)
{
    U_DHDPWR DHDPWR;
    U_DSDPWR DSDPWR;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDPWR.u32));
        DHDPWR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDPWR.bits.hpw = HPW;
        DHDPWR.bits.vpw = VPW;
        HAL_WriteReg((HI_U32*)addr_REG, DHDPWR.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDPWR.u32));
        DSDPWR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDPWR.bits.hpw = HPW;
        DSDPWR.bits.vpw = VPW;
        HAL_WriteReg((HI_U32*)addr_REG, DSDPWR.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetPlusPhase(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 IHS, HI_U32 IVS, HI_U32 IDV)
{
    U_DHDCTRL DHDCTRL;
    U_DSDCTRL DSDCTRL;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCTRL.bits.ihs = IHS;
        DHDCTRL.bits.ivs = IVS;
        DHDCTRL.bits.idv = IDV;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCTRL.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCTRL.u32));
        DSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCTRL.bits.ihs = IHS;
        DSDCTRL.bits.ivs = IVS;
        DSDCTRL.bits.idv = IDV;
        HAL_WriteReg((HI_U32*)addr_REG, DSDCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetClkGateEnable(HI_U32 u32Data)
{
    U_VOCTRL VOCTRL;
    
    VOCTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOCTRL.u32));
    VOCTRL.bits.vo_ck_gt_en = u32Data;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOCTRL.u32), VOCTRL.u32);
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntfSyncMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_DISP_SYNC_MODE_E Mode)
{
    U_DHDCTRL DHDCTRL;
    U_DSDCTRL DSDCTRL;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCTRL.bits.synm   = Mode;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCTRL.u32); 
    }
    else if((CHANNEL_DSD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DSDCTRL.u32));
        DSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DSDCTRL.bits.synm   = Mode;
        HAL_WriteReg((HI_U32*)addr_REG, DSDCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntfBitWidth(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_DISP_BIT_WIDTH_E Width)
{
    U_DHDCTRL DHDCTRL;
    volatile  HI_U32 addr_REG;

    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DHD_END))
    {
        addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHDCTRL.u32));
        DHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        DHDCTRL.bits.intfb  = Width;
        HAL_WriteReg((HI_U32*)addr_REG, DHDCTRL.u32);
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_VOID HAL_DISP_DATE_OutCtrl(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32OutCtrl)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
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
    else if (HAL_DISP_CHANNEL_DSD1 == enChan)
    {
        U_DATE1_COEFF21 DateOutCtrl, tmp;

        tmp.u32 = u32OutCtrl;
        DateOutCtrl.u32 = pVoReg->DATE1_COEFF21.u32;
        DateOutCtrl.bits.dac0_in_sel = tmp.bits.dac0_in_sel;
        DateOutCtrl.bits.dac1_in_sel = tmp.bits.dac1_in_sel;
        DateOutCtrl.bits.dac2_in_sel = tmp.bits.dac2_in_sel;
        DateOutCtrl.bits.dac3_in_sel = tmp.bits.dac3_in_sel;
        DateOutCtrl.bits.dac4_in_sel = tmp.bits.dac4_in_sel;
        DateOutCtrl.bits.dac5_in_sel = tmp.bits.dac5_in_sel;
        pVoReg->DATE1_COEFF21.u32 = DateOutCtrl.u32;
    }
    else if (HAL_DISP_CHANNEL_DHD0 <= enChan)
    {

    }

    return;
}

HI_BOOL HAL_DISP_SetDateScart(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bScartEna)
{
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        U_DATE_COEFF0 DATE_COEFF0;

        DATE_COEFF0.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->DATE_COEFF0.u32));
        DATE_COEFF0.bits.sync_mode_scart = bScartEna;        
        HAL_WriteReg((HI_U32*)&(pVoReg->DATE_COEFF0.u32), DATE_COEFF0.u32); 
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
  
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCgain(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 Cgain)
{
    U_DATE_COEFF1 DATE_COEFF1;

    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
    {
        DATE_COEFF1.u32 = pVoReg->DATE_COEFF1.u32;
        DATE_COEFF1.bits.c_gain   = Cgain;
        pVoReg->DATE_COEFF1.u32 = DATE_COEFF1.u32;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_GetDateCoeff(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 index, HI_U32 *pu32DateCoeff)
{
    volatile HI_U32 *ptrDateCoeff;

    if (HAL_DISP_CHANNEL_DSD0 <= enChan)
    {
        ptrDateCoeff = &(pVoReg->DATE_COEFF0.u32);
    }
    #if 0
    else if (HAL_DISP_CHANNEL_DHD0 <= enChan)
    {
        ptrDateCoeff = &(pVoReg->HDATE_VERSION.u32);
    }
    #endif
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    *pu32DateCoeff = (HI_U32)(ptrDateCoeff[index]);
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    U_DATE_COEFF0 DATE_COEFF0;
    U_DATE_COEFF1 DATE1_COEFF0;

    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        DATE_COEFF0.u32 = pVoReg->DATE_COEFF0.u32;
        DATE_COEFF0.u32 = u32Data;
        pVoReg->DATE_COEFF0.u32 = DATE_COEFF0.u32;
    }
    else if (HAL_DISP_CHANNEL_DSD1 == enChan)
    {
        DATE1_COEFF0.u32 = pVoReg->DATE1_COEFF0.u32;
        DATE1_COEFF0.u32 = u32Data;
        pVoReg->DATE1_COEFF0.u32 = DATE1_COEFF0.u32;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCcdMulti32Mode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 b32Rgn)
{

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCcdSlaveMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bSlave)
{

    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetCcdEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bEnable)
{

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCcdOutImgId(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_CCD_IMGID_E enImgId)
{

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCcdOutImgMode(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 ImgMode)
{

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCcdOutAreaEnable (HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 AreaNum, HI_U32 bEnable)
{

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCcdOutAllAreaDisable (HAL_DISP_OUTPUTCHANNEL_E enChan)
{

    return HI_TRUE;
}

HI_BOOL HAL_DISP_GetCcdInImgId(HAL_DISP_OUTPUTCHANNEL_E enChan, HAL_CCD_IMGID_E *penImgId)
{

    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetCcdImgRight(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 bEnable)
{
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetAllCcdAreaDisable (HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerAddr(HAL_DISP_LAYER_E enLayer,
                                         HI_U32 u32LAddr,HI_U32 u32CAddr, 
                                         HI_U16 u16LStr, HI_U16 u16CStr)
{
    volatile U_VHDCADDR VHDCADDR;  
    volatile U_VHDCCADDR VHDCCADDR;
    volatile U_VHDSTRIDE VHDSTRIDE;
    volatile U_VSDCADDR VSDCADDR;  
    volatile U_VSDCCADDR VSDCCADDR;
    volatile U_VSDSTRIDE VSDSTRIDE;


    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCADDR.u32));
        VHDCADDR.u32 = u32LAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCCADDR.u32));
        VHDCCADDR.u32 = u32CAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCCADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDSTRIDE.u32));
        VHDSTRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDSTRIDE.bits.surface_stride = u16LStr;
        VHDSTRIDE.bits.surface_cstride = u16CStr;
        HAL_WriteReg((HI_U32*)addr_REG, VHDSTRIDE.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCADDR.u32));
        VSDCADDR.u32 = u32LAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCCADDR.u32));
        VSDCCADDR.u32 = u32CAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCCADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDSTRIDE.u32));
        VSDSTRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDSTRIDE.bits.surface_stride = u16LStr;
        VSDSTRIDE.bits.surface_cstride = u16CStr;
        HAL_WriteReg((HI_U32*)addr_REG, VSDSTRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerUpMode(HAL_DISP_LAYER_E enLayer, HI_U32 bUpMode)
{
    U_VHDCTRL VHDCTRL;
    U_VSDCTRL VSDCTRL;

    volatile  HI_U32 addr_REG;   

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTRL.u32));
        VHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCTRL.bits.vup_mode = bUpMode;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCTRL.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCTRL.u32));
        VSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCTRL.bits.vup_mode = bUpMode;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetReadMode(HAL_DISP_LAYER_E enLayer,
                                        HAL_DISP_DATARMODE_E enLRMode,
                                        HAL_DISP_DATARMODE_E enCRMode)
{

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_GetReadMode(HAL_DISP_LAYER_E enLayer, HI_U32 *pu32ReadMode)
{

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetIfirMode(HAL_DISP_LAYER_E enLayer, HAL_IFIRMODE_E enMode)
{
    volatile U_VHDCTRL VHDCTRL;
    volatile U_VSDCTRL VSDCTRL;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTRL.u32));
        VHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCTRL.bits.ifir_mode = enMode;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCTRL.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCTRL.u32));
        VSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCTRL.bits.ifir_mode = enMode;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetIfirCoef(HAL_DISP_LAYER_E enLayer, HI_S32 * s32Coef)
{
    U_VHDIFIRCOEF01 VHDIFIRCOEF01;
    U_VHDIFIRCOEF23 VHDIFIRCOEF23;
    U_VHDIFIRCOEF45 VHDIFIRCOEF45;
    U_VHDIFIRCOEF67 VHDIFIRCOEF67;
    U_VSDIFIRCOEF01 VSDIFIRCOEF01;
    U_VSDIFIRCOEF23 VSDIFIRCOEF23;
    U_VSDIFIRCOEF45 VSDIFIRCOEF45;
    U_VSDIFIRCOEF67 VSDIFIRCOEF67;
    
    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDIFIRCOEF01.u32));
        VHDIFIRCOEF01.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDIFIRCOEF01.bits.coef0 = s32Coef[0];
        VHDIFIRCOEF01.bits.coef1 = s32Coef[1];
        HAL_WriteReg((HI_U32*)addr_REG, VHDIFIRCOEF01.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDIFIRCOEF23.u32));
        VHDIFIRCOEF23.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDIFIRCOEF23.bits.coef2 = s32Coef[2];
        VHDIFIRCOEF23.bits.coef3 = s32Coef[3];
        HAL_WriteReg((HI_U32*)addr_REG, VHDIFIRCOEF23.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDIFIRCOEF45.u32));
        VHDIFIRCOEF45.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDIFIRCOEF45.bits.coef4 = s32Coef[4];
        VHDIFIRCOEF45.bits.coef5 = s32Coef[5];
        HAL_WriteReg((HI_U32*)addr_REG, VHDIFIRCOEF45.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDIFIRCOEF67.u32));
        VHDIFIRCOEF67.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDIFIRCOEF67.bits.coef6 = s32Coef[6];
        VHDIFIRCOEF67.bits.coef7 = s32Coef[7];
        HAL_WriteReg((HI_U32*)addr_REG, VHDIFIRCOEF67.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDIFIRCOEF01.u32));
        VSDIFIRCOEF01.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDIFIRCOEF01.bits.coef0 = s32Coef[0];
        VSDIFIRCOEF01.bits.coef1 = s32Coef[1];
        HAL_WriteReg((HI_U32*)addr_REG, VSDIFIRCOEF01.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDIFIRCOEF23.u32));
        VSDIFIRCOEF23.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDIFIRCOEF23.bits.coef2 = s32Coef[2];
        VSDIFIRCOEF23.bits.coef3 = s32Coef[3];
        HAL_WriteReg((HI_U32*)addr_REG, VSDIFIRCOEF23.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDIFIRCOEF45.u32));
        VSDIFIRCOEF45.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDIFIRCOEF45.bits.coef4 = s32Coef[4];
        VSDIFIRCOEF45.bits.coef5 = s32Coef[5];
        HAL_WriteReg((HI_U32*)addr_REG, VSDIFIRCOEF45.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDIFIRCOEF67.u32));
        VSDIFIRCOEF67.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDIFIRCOEF67.bits.coef6 = s32Coef[6];
        VSDIFIRCOEF67.bits.coef7 = s32Coef[7];
        HAL_WriteReg((HI_U32*)addr_REG, VSDIFIRCOEF67.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerDispRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect)
{
    U_VHDDFPOS VHDDFPOS;
    U_VHDDLPOS VHDDLPOS;
    U_VSDDFPOS VSDDFPOS;
    U_VSDDLPOS VSDDLPOS;
    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDDFPOS.u32));
        VHDDFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDDFPOS.bits.disp_xfpos = pstRect->s32X;
        VHDDFPOS.bits.disp_yfpos = pstRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, VHDDFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDDLPOS.u32));
        VHDDLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDDLPOS.bits.disp_xlpos = pstRect->s32X +pstRect->s32Width -1;
        VHDDLPOS.bits.disp_ylpos = pstRect->s32Y + pstRect->s32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDDLPOS.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDDFPOS.u32));
        VSDDFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDDFPOS.bits.disp_xfpos = pstRect->s32X;
        VSDDFPOS.bits.disp_yfpos = pstRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, VSDDFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDDLPOS.u32));
        VSDDLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDDLPOS.bits.disp_xlpos = pstRect->s32X +pstRect->s32Width -1;
        VSDDLPOS.bits.disp_ylpos = pstRect->s32Y + pstRect->s32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, VSDDLPOS.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerBkg(HAL_DISP_LAYER_E enLayer,HAL_DISP_BKCOLOR_S *pstBkg)
{
    U_VHDBK VHDBK;
    U_VSDBK VSDBK;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDBK.u32));
        VHDBK.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        //VHDBK.bits.vbk_alpha = pstBkg->u8Bkg_a;
        VHDBK.bits.vbk_y = pstBkg->u8Bkg_y;
        VHDBK.bits.vbk_cb = pstBkg->u8Bkg_cb;
        VHDBK.bits.vbk_cr = pstBkg->u8Bkg_cr;
        HAL_WriteReg((HI_U32*)addr_REG, VHDBK.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDBK.u32));
        VSDBK.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        //VSDBK.bits.vbk_alpha = pstBkg->u8Bkg_a;
        VSDBK.bits.vbk_y = pstBkg->u8Bkg_y;
        VSDBK.bits.vbk_cb = pstBkg->u8Bkg_cb;
        VSDBK.bits.vbk_cr = pstBkg->u8Bkg_cr;
        HAL_WriteReg((HI_U32*)addr_REG, VSDBK.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetFieldOrder(HAL_DISP_LAYER_E enLayer, HAL_VHD_FOD_E uFieldOrder)
{

    return HI_TRUE;
}

HI_BOOL HAL_VIEDO_SetAcmEnable(HAL_DISP_LAYER_E enLayer, HAL_ACMBLK_ID_E enAcmId, HI_U32 bAcmEn)
{
    #if 0
    U_VHDACM0 VHDACM0;
    volatile  HI_U32 addr_REG;

    VHDACM0.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VHDACM0.u32));

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM0.u32));
        VHDACM0.u32 = HAL_ReadReg((HI_U32*)addr_REG);

        switch(enAcmId)
        {
            case HAL_ACMBLK_ID0 :
            {
                VHDACM0.bits.acm0_en = bAcmEn;
                break;
            }
            case HAL_ACMBLK_ID1 :
            {
                VHDACM0.bits.acm1_en = bAcmEn;
                break;
            }
            case HAL_ACMBLK_ID2 :
            {
                VHDACM0.bits.acm2_en = bAcmEn;
                break;
            }
            case HAL_ACMBLK_ID3 :
            {
                VHDACM0.bits.acm3_en = bAcmEn;
                break;
            }
            case HAL_ACMBLK_ALL :
            {
                VHDACM0.bits.acm0_en = bAcmEn;
                VHDACM0.bits.acm1_en = bAcmEn;
                VHDACM0.bits.acm2_en = bAcmEn;
                VHDACM0.bits.acm3_en = bAcmEn;
                break;
            }
            default:
            {
                HAL_PRINT("Error ACM block id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
    
        HAL_WriteReg((HI_U32*)addr_REG, VHDACM0.u32);
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_VIEDO_SetAcmCoef(HAL_DISP_LAYER_E enLayer, HAL_ACMBLK_ID_E enAcmId, HAL_DISP_ACMBLKINFO_S *pstCoef)
{
    #if 0
    U_VHDACM0 VHDACM0;
    U_VHDACM1 VHDACM1;
    U_VHDACM2 VHDACM2;
    U_VHDACM3 VHDACM3;
    U_VHDACM4 VHDACM4;
    U_VHDACM5 VHDACM5;
    U_VHDACM6 VHDACM6;
    U_VHDACM7 VHDACM7;
    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        if((enAcmId == HAL_ACMBLK_ID0)||(enAcmId == HAL_ACMBLK_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM0.u32));
            VHDACM0.u32 = HAL_ReadReg((HI_U32*)addr_REG);

            VHDACM0.bits.acm_fir_blk = pstCoef->u8UIndex;
            VHDACM0.bits.acm_sec_blk = pstCoef->u8VIndex;

            VHDACM0.bits.acm_a_u_off = (pstCoef->s8UOffsetA);
            VHDACM0.bits.acm_b_u_off = (pstCoef->s8UOffsetB);
            VHDACM0.bits.acm_c_u_off = (pstCoef->s8UOffsetC);
            VHDACM0.bits.acm_d_u_off = (pstCoef->s8UOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM0.u32);

            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM1.u32));
            VHDACM1.u32 = HAL_ReadReg((HI_U32*)addr_REG);    
            VHDACM1.bits.acm_a_v_off = (pstCoef->s8VOffsetA);
            VHDACM1.bits.acm_b_v_off = (pstCoef->s8VOffsetB);
            VHDACM1.bits.acm_c_v_off = (pstCoef->s8VOffsetC);
            VHDACM1.bits.acm_d_v_off = (pstCoef->s8VOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM1.u32);  
        }

        if((enAcmId == HAL_ACMBLK_ID1)||(enAcmId == HAL_ACMBLK_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM2.u32));
            VHDACM2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
     
            VHDACM2.bits.acm_fir_blk = pstCoef->u8UIndex;
            VHDACM2.bits.acm_sec_blk = pstCoef->u8VIndex;

            VHDACM2.bits.acm_a_u_off = (pstCoef->s8UOffsetA);
            VHDACM2.bits.acm_b_u_off = (pstCoef->s8UOffsetB);
            VHDACM2.bits.acm_c_u_off = (pstCoef->s8UOffsetC);
            VHDACM2.bits.acm_d_u_off = (pstCoef->s8UOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM2.u32);	
            
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM3.u32));
            VHDACM3.u32 = HAL_ReadReg((HI_U32*)addr_REG);                                    
            VHDACM3.bits.acm_a_v_off = (pstCoef->s8VOffsetA);
            VHDACM3.bits.acm_b_v_off = (pstCoef->s8VOffsetB);
            VHDACM3.bits.acm_c_v_off = (pstCoef->s8VOffsetC);
            VHDACM3.bits.acm_d_v_off = (pstCoef->s8VOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM3.u32);
 
        }

        if((enAcmId == HAL_ACMBLK_ID2)||(enAcmId == HAL_ACMBLK_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM4.u32));
            VHDACM4.u32 = HAL_ReadReg((HI_U32*)addr_REG);

            VHDACM4.bits.acm_fir_blk = pstCoef->u8UIndex;
            VHDACM4.bits.acm_sec_blk = pstCoef->u8VIndex;
     
            VHDACM4.bits.acm_a_u_off = (pstCoef->s8UOffsetA);
            VHDACM4.bits.acm_b_u_off = (pstCoef->s8UOffsetB);
            VHDACM4.bits.acm_c_u_off = (pstCoef->s8UOffsetC);
            VHDACM4.bits.acm_d_u_off = (pstCoef->s8UOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM4.u32);

            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM5.u32));
            VHDACM5.u32 = HAL_ReadReg((HI_U32*)addr_REG);
                          
            VHDACM5.bits.acm_a_v_off = (pstCoef->s8VOffsetA);
            VHDACM5.bits.acm_b_v_off = (pstCoef->s8VOffsetB);
            VHDACM5.bits.acm_c_v_off = (pstCoef->s8VOffsetC);
            VHDACM5.bits.acm_d_v_off = (pstCoef->s8VOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM5.u32);
    
        }
    
        if((enAcmId == HAL_ACMBLK_ID3)||(enAcmId == HAL_ACMBLK_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM6.u32));
            VHDACM6.u32 = HAL_ReadReg((HI_U32*)addr_REG);
  
            VHDACM6.bits.acm_fir_blk = pstCoef->u8UIndex;
            VHDACM6.bits.acm_sec_blk = pstCoef->u8VIndex;
 
            VHDACM6.bits.acm_a_u_off = (pstCoef->s8UOffsetA);
            VHDACM6.bits.acm_b_u_off = (pstCoef->s8UOffsetB);
            VHDACM6.bits.acm_c_u_off = (pstCoef->s8UOffsetC);
            VHDACM6.bits.acm_d_u_off = (pstCoef->s8UOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM6.u32);

            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM7.u32));
            VHDACM7.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDACM7.bits.acm_a_v_off = (pstCoef->s8VOffsetA);
            VHDACM7.bits.acm_b_v_off = (pstCoef->s8VOffsetB);
            VHDACM7.bits.acm_c_v_off = (pstCoef->s8VOffsetC);
            VHDACM7.bits.acm_d_v_off = (pstCoef->s8VOffsetD);
            HAL_WriteReg((HI_U32*)addr_REG, VHDACM7.u32);
    
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;    
}

HI_BOOL HAL_VIEDO_SetAcmTestEnable(HAL_DISP_LAYER_E enLayer, HI_U32 bAcmEn)
{
    #if 0
    U_VHDACM1 VHDACM1;
    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACM1.u32));
        VHDACM1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDACM1.bits.acm_test_en = bAcmEn;
        HAL_WriteReg((HI_U32*)addr_REG, VHDACM1.u32);
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_FALSE;
}

HI_BOOL HAL_VIDEO_SetAccParaUpd(HAL_DISP_COEFMODE_E enMode)
{
    #if 0
    U_VOPARAUP VOPARAUP;
    
    VOPARAUP.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOPARAUP.u32));
    if (enMode == HAL_DISP_COEFMODE_ACC || enMode == HAL_DISP_COEFMODE_ALL)
    {
        VOPARAUP.bits.video_acc_upd = 0x1;
    }
    HAL_WriteReg((HI_U32*)&(pVoReg->VOPARAUP.u32), VOPARAUP.u32); 

    pVoReg->VOPARAUP.bits.video_acc_upd = 0x0;
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerAccThd(HAL_DISP_LAYER_E enLayer, ACCTHD_S *pstAccThd)
{
    #if 0
    U_VHDACCTHD1 VHDACCTHD1;
    U_VHDACCTHD2 VHDACCTHD2;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCTHD1.u32));
        VHDACCTHD1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDACCTHD1.bits.thd_med_low  = pstAccThd->thd_med_low ;
        VHDACCTHD1.bits.thd_high     = pstAccThd->thd_high    ;
        VHDACCTHD1.bits.thd_low      = pstAccThd->thd_low     ;
        HAL_WriteReg((HI_U32*)addr_REG, VHDACCTHD1.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCTHD2.u32));
        VHDACCTHD2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDACCTHD2.bits.acc_multiple = pstAccThd->acc_multi   ;
        VHDACCTHD2.bits.thd_med_high = pstAccThd->thd_med_high;
        HAL_WriteReg((HI_U32*)addr_REG, VHDACCTHD2.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerAccTab(HAL_DISP_LAYER_E enLayer, HI_U32 *upTable)
{
    #if 0
    U_VHDACCLOWN  VHDACCLOW[3];
    U_VHDACCMEDN  VHDACCMED[3];
    U_VHDACCHIGHN VHDACCHIGH[3];
    U_VHDACCMLN     VHDACCML[3];
    U_VHDACCMHN    VHDACCMH[3];
    
    HI_U32 i = 0;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        for (i = 0; i < 3; i++)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCLOWN[i].u32));
            VHDACCLOW[i].bits.table_data1n = upTable[0 + i*3];
            VHDACCLOW[i].bits.table_data2n = upTable[1 + i*3];
            VHDACCLOW[i].bits.table_data3n = upTable[2 + i*3];
            HAL_WriteReg((HI_U32*)addr_REG, VHDACCLOW[i].u32); 
        }

        for (i = 0; i < 3; i++)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCMEDN[i].u32));
            VHDACCMED[i].bits.table_data1n = upTable[9 + 0 + i*3];
            VHDACCMED[i].bits.table_data2n = upTable[9 + 1 + i*3];
            VHDACCMED[i].bits.table_data3n = upTable[9 + 2 + i*3];
            HAL_WriteReg((HI_U32*)addr_REG, VHDACCMED[i].u32); 
        }

        for (i = 0; i < 3; i++)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCHIGHN[i].u32));
            VHDACCHIGH[i].bits.table_data1n = upTable[18 + 0 + i*3];
            VHDACCHIGH[i].bits.table_data2n = upTable[18 + 1 + i*3];
            VHDACCHIGH[i].bits.table_data3n = upTable[18 + 2 + i*3];
            HAL_WriteReg((HI_U32*)addr_REG, VHDACCHIGH[i].u32); 
        }

        for (i = 0; i < 3; i++)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCMLN[i].u32));
            VHDACCML[i].bits.table_data1n = upTable[27 + 0 + i*3];
            VHDACCML[i].bits.table_data2n = upTable[27 + 1 + i*3];
            VHDACCML[i].bits.table_data3n = upTable[27 + 2 + i*3];
            HAL_WriteReg((HI_U32*)addr_REG, VHDACCML[i].u32); 
        }

        for (i = 0; i < 3; i++)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCMHN[i].u32));
            VHDACCMH[i].bits.table_data1n = upTable[36 + 0 + i*3];
            VHDACCMH[i].bits.table_data2n = upTable[36 + 1 + i*3];
            VHDACCMH[i].bits.table_data3n = upTable[36 + 2 + i*3];
            HAL_WriteReg((HI_U32*)addr_REG, VHDACCMH[i].u32); 
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerAccCtrl(HAL_DISP_LAYER_E enLayer, HI_U32 uAccEn, HI_U32 uAccMode)
{
    #if 0
    U_VHDACCTHD1 VHDACCTHD1;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCTHD1.u32));
        VHDACCTHD1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDACCTHD1.bits.acc_en   = uAccEn  ;
        VHDACCTHD1.bits.acc_mode = uAccMode;
        HAL_WriteReg((HI_U32*)addr_REG, VHDACCTHD1.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetAccWeightAddr(HI_U32 uAccAddr)
{
    #if 0
    U_ACCAD ACCAD;
    
    ACCAD.bits.coef_addr = uAccAddr;
    HAL_WriteReg((HI_U32*)&(pVoReg->ACCAD.u32), ACCAD.u32);
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerAccRst(HAL_DISP_LAYER_E enLayer, HI_U32 uAccRst)
{
    #if 0
    U_VHDACCTHD2 VHDACCTHD2;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDACCTHD2.u32));
        VHDACCTHD2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDACCTHD2.bits.acc_rst   = uAccRst  ;
        HAL_WriteReg((HI_U32*)addr_REG, VHDACCTHD2.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetMultiAreaLAddr  (HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 u32LAddr, HI_U16 stride)
{
    volatile U_VHDP0LADDR VHDP0LADDR;
    volatile U_VSDP0LADDR VSDP0LADDR;

    volatile U_VHDP0STRIDE VHDP0STRIDE;
    volatile U_VSDP0STRIDE VSDP0STRIDE;

    volatile  HI_U32 addr_REG;
    HI_U32 u32_pxaddr_addr = 0;
    HI_U32 u32_pxstride_addr = 0;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        u32_pxaddr_addr   = (HI_U32)&(pVoReg->VHDP0LADDR.u32) + area_num * MULTI_AREA_CFG_LEN;
        u32_pxstride_addr = (HI_U32)&(pVoReg->VHDP0STRIDE.u32) + area_num * MULTI_AREA_CFG_LEN;

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxaddr_addr);
        VHDP0LADDR.u32   = u32LAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VHDP0LADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxstride_addr);
        VHDP0STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDP0STRIDE.bits.surface_stride = stride;
        HAL_WriteReg((HI_U32*)addr_REG, VHDP0STRIDE.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDP0LADDR.u32));
        VSDP0LADDR.u32   = u32LAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VSDP0LADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDP0STRIDE.u32));
        VSDP0STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDP0STRIDE.bits.surface_stride = stride;
        HAL_WriteReg((HI_U32*)addr_REG, VSDP0STRIDE.u32);
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetMultiAreaCAddr  (HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 u32CAddr, HI_U16 stride)
{
    U_VHDP0CADDR VHDP0CADDR;
    U_VSDP0CADDR VSDP0CADDR;
    U_VHDP0STRIDE VHDP0STRIDE;
    U_VSDP0STRIDE VSDP0STRIDE;

    volatile  HI_U32 addr_REG;
    HI_U32 u32_pxaddr_addr = 0;
    HI_U32 u32_pxstride_addr = 0;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        u32_pxaddr_addr   = (HI_U32)&(pVoReg->VHDP0CADDR.u32) + area_num * MULTI_AREA_CFG_LEN;
        u32_pxstride_addr = (HI_U32)&(pVoReg->VHDP0STRIDE.u32) + area_num * MULTI_AREA_CFG_LEN;

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxaddr_addr);
        VHDP0CADDR.u32   = u32CAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VHDP0CADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxstride_addr);
        VHDP0STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDP0STRIDE.bits.surface_cstride = stride;
        HAL_WriteReg((HI_U32*)addr_REG, VHDP0STRIDE.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDP0CADDR.u32));
        VSDP0CADDR.u32   = u32CAddr;
        HAL_WriteReg((HI_U32*)addr_REG, VSDP0CADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDP0STRIDE.u32));
        VSDP0STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDP0STRIDE.bits.surface_cstride = stride;
        HAL_WriteReg((HI_U32*)addr_REG, VSDP0STRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_VIDEO_SetMultiAreaEnable(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 bEnable)
{
    volatile U_VHD16REGIONENL  VHD16REGIONENL;
    //volatile U_VHD64REGIONENH  VHD64REGIONENH;
    volatile U_VSD1REGIONEN   VSD1REGIONEN;
    volatile HI_U32    regionen_value;

    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        if(area_num <32)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHD16REGIONENL.u32));

            VHD16REGIONENL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            if(bEnable)
            {
                regionen_value = ((bEnable << area_num ) | VHD16REGIONENL.u32);
            }
            else
            {
                regionen_value = ~(1 << area_num ) & VHD16REGIONENL.u32;
            }
            HAL_WriteReg((HI_U32*)addr_REG, regionen_value); 
        }
        #if 0
        else if(area_num <64)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHD64REGIONENH.u32));

            VHD64REGIONENH.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            if(bEnable)
            {
                regionen_value = (bEnable << (area_num - 32)) | VHD64REGIONENH.u32;
            }
            else
            {
                regionen_value = ~(1 << (area_num - 32)) & VHD64REGIONENH.u32;
            }
            HAL_WriteReg((HI_U32*)addr_REG, regionen_value); 
        }
        #endif
        else
        {
            HAL_PRINT("Error area num found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSD1REGIONEN.u32));
        VSD1REGIONEN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        if(bEnable)
        {
            regionen_value = (bEnable | VSD1REGIONEN.u32);
        }
        else
        {
            regionen_value = ~(1) & VSD1REGIONEN.u32;
        }
        HAL_WriteReg((HI_U32*)addr_REG, regionen_value);
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}


HI_BOOL HAL_VIDEO_GetMultiAreaEnable(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_U32 *pu32Enable)
{
    volatile U_VHD16REGIONENL  VHD16REGIONENL;
    //volatile U_VHD64REGIONENH  VHD64REGIONENH;
    volatile U_VSD1REGIONEN   VSD1REGIONEN;

    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        if(area_num <32)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHD16REGIONENL.u32));

            VHD16REGIONENL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Enable = (1 << area_num ) & VHD16REGIONENL.u32;
        }
        #if 0
        else if(area_num <64)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHD64REGIONENH.u32));

            VHD64REGIONENH.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Enable = (1 << (area_num - 32)) & VHD64REGIONENH.u32;
        }
        #endif
        else
        {
            HAL_PRINT("Error area num found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSD1REGIONEN.u32));
        VSD1REGIONEN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Enable = (1 << area_num ) & VSD1REGIONEN.u32;
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}


//disable all area
HI_BOOL HAL_VIDEO_SetAllAreaDisable  (HAL_DISP_LAYER_E enLayer)
{
    //U_VHD64REGIONENL  VHD64REGIONENL;
    //U_VHD64REGIONENH  VHD64REGIONENH;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHD16REGIONENL.u32));
        HAL_WriteReg((HI_U32*)addr_REG, 0x0); 
        /* 3521无此项*/
        #if 0
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHD64REGIONENH.u32));
        HAL_WriteReg((HI_U32*)addr_REG, 0x0); 
        #endif
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSD1REGIONEN.u32));
        HAL_WriteReg((HI_U32*)addr_REG, 0x0); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_VIDEO_SetMultiAreaReso(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,
                                              HI_U32 u32Width, HI_U32 u32Height)
{
    U_VHDP0RESO  VHDP0RESO;
    U_VSDP0RESO  VSDP0RESO;
    HI_U32 pstReso;

    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        pstReso  = (HI_U32)&(pVoReg->VHDP0RESO.u32)  + area_num * MULTI_AREA_CFG_LEN;

        addr_REG = Vou_GetAbsAddr(enLayer,pstReso);
        VHDP0RESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDP0RESO.bits.w = u32Width -1;
        VHDP0RESO.bits.h = u32Height -1;

        HAL_WriteReg((HI_U32*)addr_REG, VHDP0RESO.u32); 

    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        pstReso  = (HI_U32)&(pVoReg->VSDP0RESO.u32);
        
        addr_REG = Vou_GetAbsAddr(enLayer,pstReso);
        VSDP0RESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDP0RESO.bits.w = u32Width -1;
        VSDP0RESO.bits.h = u32Height -1;

        HAL_WriteReg((HI_U32*)addr_REG, VSDP0RESO.u32); 
    }    
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_VIDEO_SetMultiAreaRect(HAL_DISP_LAYER_E enLayer,HI_U32 area_num,HI_RECT_S *pstVideoAreaRect)
{
    U_VHDP0VFPOS VHDP0VFPOS;
    U_VHDP0VLPOS VHDP0VLPOS;
    U_VSDP0VFPOS VSDP0VFPOS;
    U_VSDP0VLPOS VSDP0VLPOS;

    HI_U32 pstVfpos;
    HI_U32 pstVlpos;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        pstVfpos = (HI_U32)&(pVoReg->VHDP0VFPOS.u32) + area_num * MULTI_AREA_CFG_LEN;
        pstVlpos = (HI_U32)&(pVoReg->VHDP0VLPOS.u32) + area_num * MULTI_AREA_CFG_LEN;

        addr_REG = Vou_GetAbsAddr(enLayer,pstVfpos);
        VHDP0VFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDP0VFPOS.bits.video_xfpos = pstVideoAreaRect->s32X;
        VHDP0VFPOS.bits.video_yfpos = pstVideoAreaRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, VHDP0VFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,pstVlpos);
        VHDP0VLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDP0VLPOS.bits.video_xlpos = pstVideoAreaRect->s32X + pstVideoAreaRect->s32Width -1;
        VHDP0VLPOS.bits.video_ylpos = pstVideoAreaRect->s32Y + pstVideoAreaRect->s32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDP0VLPOS.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        pstVfpos = (HI_U32)&(pVoReg->VSDP0VFPOS.u32);
        pstVlpos = (HI_U32)&(pVoReg->VSDP0VLPOS.u32);

        addr_REG = Vou_GetAbsAddr(enLayer,pstVfpos);
        VSDP0VFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDP0VFPOS.bits.video_xfpos = pstVideoAreaRect->s32X;
        VSDP0VFPOS.bits.video_yfpos = pstVideoAreaRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, VSDP0VFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,pstVlpos);
        VSDP0VLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDP0VLPOS.bits.video_xlpos = pstVideoAreaRect->s32X + pstVideoAreaRect->s32Width -1;;
        VSDP0VLPOS.bits.video_ylpos = pstVideoAreaRect->s32Y + pstVideoAreaRect->s32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, VSDP0VLPOS.u32);
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetVouDdrSel(HAL_DISP_LAYER_E enLayer, HAL_LAYER_DDR_SEL_E DdrNo)
{
    #if 0
    volatile U_VODDRSEL VODDRSEL;
    switch(enLayer)
    {
        case HAL_DISP_LAYER_VHD0:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vhd0_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_VHD1:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vhd1_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_VHD2:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vhd2_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_VSD0:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vsd0_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_VSD1:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vsd1_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_VSD2:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vsd2_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_VSD3:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vsd3_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_VSD4:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.vsd4_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_GFX0:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.g0_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_GFX1:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.g1_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_GFX2:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.g2_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_GFX3:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.g3_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_GFX4:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.g4_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_HC0:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.hc0_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_HC1:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.hc1_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        case HAL_DISP_LAYER_WBC2:
        {
            VODDRSEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VODDRSEL.u32));
            VODDRSEL.bits.wbc1_ddr_sel = DdrNo;
            HAL_WriteReg((HI_U32*)&(pVoReg->VODDRSEL.u32), VODDRSEL.u32); 
            break;
        }
        default:
        {
            HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    #endif
    
    return HI_TRUE;
}


HI_BOOL HAL_LAYER_EnableLayer(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable)
{
    volatile U_VHDCTRL VHDCTRL;
    volatile U_VSDCTRL VSDCTRL;
    volatile U_G0CTRL G0CTRL;
    
    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTRL.u32));
        VHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCTRL.bits.surface_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCTRL.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCTRL.u32));
        VSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCTRL.bits.surface_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCTRL.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));
        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CTRL.bits.surface_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, G0CTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_GetLayerEnable(HAL_DISP_LAYER_E enLayer,HI_U32 *pu32Enable)
{
    U_VHDCTRL VHDCTRL;
    U_VSDCTRL VSDCTRL;
    U_G0CTRL  G0CTRL;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTRL.u32));
        VHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Enable = VHDCTRL.bits.surface_en;
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCTRL.u32));
        VSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Enable = VSDCTRL.bits.surface_en;
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));
        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Enable = G0CTRL.bits.surface_en;
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
    volatile U_VHDCTRL VHDCTRL;
    volatile U_VSDCTRL VSDCTRL;
    volatile U_G0CTRL G0CTRL;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTRL.u32));
        VHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCTRL.bits.ifmt = enDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCTRL.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCTRL.u32));
        VSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCTRL.bits.ifmt = enDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCTRL.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));
        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CTRL.bits.ifmt = enDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, G0CTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_GetLayerDataFmt(HAL_DISP_LAYER_E enLayer,HI_U32 *pu32Fmt)
{
    U_VHDCTRL VHDCTRL;
    U_VSDCTRL VSDCTRL;
    U_G0CTRL  G0CTRL;

    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTRL.u32));
        VHDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Fmt = VHDCTRL.bits.ifmt;
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCTRL.u32));
        VSDCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Fmt = VSDCTRL.bits.ifmt;
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));
        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Fmt = G0CTRL.bits.ifmt;
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
    U_G0CSCIDC G0CSCIDC;
    U_G0CSCODC G0CSCODC;
    U_VHDCSCIDC VHDCSCIDC;
    U_VHDCSCODC VHDCSCODC;
    U_VSDCSCIDC VSDCSCIDC;
    U_VSDCSCODC VSDCSCODC;
    
    U_VHDCSCP0 VHDCSCP0;
    U_VHDCSCP1 VHDCSCP1;
    U_VHDCSCP2 VHDCSCP2;
    U_VHDCSCP3 VHDCSCP3;
    U_VHDCSCP4 VHDCSCP4;

    U_VSDCSCP0 VSDCSCP0;
    U_VSDCSCP1 VSDCSCP1;
    U_VSDCSCP2 VSDCSCP2;
    U_VSDCSCP3 VSDCSCP3;
    U_VSDCSCP4 VSDCSCP4;

    U_G0CSCP0 G0CSCP0;
    U_G0CSCP1 G0CSCP1;
    U_G0CSCP2 G0CSCP2;
    U_G0CSCP3 G0CSCP3;
    U_G0CSCP4 G0CSCP4;
    
    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCIDC.u32));
        VHDCSCIDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCSCIDC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc2);
        VHDCSCIDC.bits.cscidc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc1);
        VHDCSCIDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCIDC.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCODC.u32));
        VHDCSCODC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCSCODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc2);
        VHDCSCODC.bits.cscodc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc1);
        VHDCSCODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCODC.u32);
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCP0.u32));
        VHDCSCP0.u32 = HAL_ReadReg((HI_U32*)addr_REG);        
        VHDCSCP0.bits.cscp00 = HAL_Conver_CscCoef(pstCscCoef->csc_coef00);
        VHDCSCP0.bits.cscp01 = HAL_Conver_CscCoef(pstCscCoef->csc_coef01);
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCP0.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCP1.u32));
        VHDCSCP1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCSCP1.bits.cscp02 = HAL_Conver_CscCoef(pstCscCoef->csc_coef02);
        VHDCSCP1.bits.cscp10 = HAL_Conver_CscCoef(pstCscCoef->csc_coef10);
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCP1.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCP2.u32));
        VHDCSCP2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCSCP2.bits.cscp11 = HAL_Conver_CscCoef(pstCscCoef->csc_coef11);
        VHDCSCP2.bits.cscp12 = HAL_Conver_CscCoef(pstCscCoef->csc_coef12);
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCP2.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCP3.u32));
        VHDCSCP3.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCSCP3.bits.cscp20 = HAL_Conver_CscCoef(pstCscCoef->csc_coef20);
        VHDCSCP3.bits.cscp21 = HAL_Conver_CscCoef(pstCscCoef->csc_coef21);
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCP3.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCP4.u32));
        VHDCSCP4.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCSCP4.bits.cscp22 = HAL_Conver_CscCoef(pstCscCoef->csc_coef22);
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCP4.u32); 
    }
    else if((LAYER_VSD_START <= enLayer) && (enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCIDC.u32));
        VSDCSCIDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCIDC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc2);
        VSDCSCIDC.bits.cscidc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc1);
        VSDCSCIDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCIDC.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCODC.u32));
        VSDCSCODC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc2);
        VSDCSCODC.bits.cscodc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc1);
        VSDCSCODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCODC.u32); 
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCP0.u32));
        VSDCSCP0.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCP0.bits.cscp00 = HAL_Conver_CscCoef(pstCscCoef->csc_coef00);
        VSDCSCP0.bits.cscp01 = HAL_Conver_CscCoef(pstCscCoef->csc_coef01);
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCP0.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCP1.u32));
        VSDCSCP1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCP1.bits.cscp02 = HAL_Conver_CscCoef(pstCscCoef->csc_coef02);
        VSDCSCP1.bits.cscp10 = HAL_Conver_CscCoef(pstCscCoef->csc_coef10);
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCP1.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCP2.u32));
        VSDCSCP2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCP2.bits.cscp11 = HAL_Conver_CscCoef(pstCscCoef->csc_coef11);
        VSDCSCP2.bits.cscp12 = HAL_Conver_CscCoef(pstCscCoef->csc_coef12);
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCP2.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCP3.u32));
        VSDCSCP3.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCP3.bits.cscp20 = HAL_Conver_CscCoef(pstCscCoef->csc_coef20);
        VSDCSCP3.bits.cscp21 = HAL_Conver_CscCoef(pstCscCoef->csc_coef21);
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCP3.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCP4.u32));
        VSDCSCP4.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCP4.bits.cscp22 = HAL_Conver_CscCoef(pstCscCoef->csc_coef22);
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCP4.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCIDC.u32));
        G0CSCIDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCIDC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc2);
        G0CSCIDC.bits.cscidc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc1);
        G0CSCIDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_in_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCIDC.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCODC.u32));
        G0CSCODC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc2);
        G0CSCODC.bits.cscodc1 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc1);
        G0CSCODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCscCoef->csc_out_dc0);
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCODC.u32);
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCP0.u32));
        G0CSCP0.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCP0.bits.cscp00 = HAL_Conver_CscCoef(pstCscCoef->csc_coef00);
        G0CSCP0.bits.cscp01 = HAL_Conver_CscCoef(pstCscCoef->csc_coef01);
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCP0.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCP1.u32));
        G0CSCP1.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCP1.bits.cscp02 = HAL_Conver_CscCoef(pstCscCoef->csc_coef02);
        G0CSCP1.bits.cscp10 = HAL_Conver_CscCoef(pstCscCoef->csc_coef10);
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCP1.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCP2.u32));
        G0CSCP2.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCP2.bits.cscp11 = HAL_Conver_CscCoef(pstCscCoef->csc_coef11);
        G0CSCP2.bits.cscp12 = HAL_Conver_CscCoef(pstCscCoef->csc_coef12);
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCP2.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCP3.u32));
        G0CSCP3.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCP3.bits.cscp20 = HAL_Conver_CscCoef(pstCscCoef->csc_coef20);
        G0CSCP3.bits.cscp21 = HAL_Conver_CscCoef(pstCscCoef->csc_coef21);
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCP3.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCP4.u32));
        G0CSCP4.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCP4.bits.cscp22 = HAL_Conver_CscCoef(pstCscCoef->csc_coef22);
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCP4.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetCscEn(HAL_DISP_LAYER_E enLayer, HI_BOOL bCscEn)
{
    U_VHDCSCIDC VHDCSCIDC;
    U_VSDCSCIDC VSDCSCIDC;
    U_G0CSCIDC G0CSCIDC;
    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCSCIDC.u32));
        VHDCSCIDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCSCIDC.bits.csc_en = bCscEn;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCSCIDC.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCSCIDC.u32));
        VSDCSCIDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCSCIDC.bits.csc_en = bCscEn;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCSCIDC.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CSCIDC.u32));
        G0CSCIDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CSCIDC.bits.csc_en = bCscEn;
        HAL_WriteReg((HI_U32*)addr_REG, G0CSCIDC.u32); 
    }
    else
    {
        HAL_PRINT("Error, Wrong layer ID!\n");
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetLayerInRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect)
{
    U_VHDIRESO VHDIRESO;
    U_VSDIRESO VSDIRESO;
    U_G0IRESO G0IRESO;
    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDIRESO.u32));
        VHDIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDIRESO.bits.iw = pstRect->s32Width- 1;
        VHDIRESO.bits.ih = pstRect->s32Height- 1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDIRESO.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDIRESO.u32));
        VSDIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDIRESO.bits.iw = pstRect->s32Width- 1;
        VSDIRESO.bits.ih = pstRect->s32Height- 1;
        HAL_WriteReg((HI_U32*)addr_REG, VSDIRESO.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0IRESO.u32));
        G0IRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0IRESO.bits.iw = pstRect->s32Width- 1;
        G0IRESO.bits.ih = pstRect->s32Height- 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0IRESO.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetLayerOutRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect)
{
    U_G0ORESO G0ORESO;
    U_G0DFPOS G0DFPOS;
    U_G0DLPOS G0DLPOS;
    U_WBCORESO WBCORESO;

    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0ORESO.u32));
        G0ORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0ORESO.bits.ow = pstRect->s32Width - 1;
        G0ORESO.bits.oh = pstRect->s32Height - 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0ORESO.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DFPOS.u32));
        G0DFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DFPOS.bits.disp_xfpos = pstRect->s32X;
        G0DFPOS.bits.disp_yfpos = pstRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, G0DFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DLPOS.u32));
        G0DLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DLPOS.bits.disp_xlpos = pstRect->s32X + pstRect->s32Width - 1;
        G0DLPOS.bits.disp_ylpos = pstRect->s32Y + pstRect->s32Height - 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0DLPOS.u32); 
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCORESO.u32));
        WBCORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCORESO.bits.ow = pstRect->s32Width - 1;
        WBCORESO.bits.oh = pstRect->s32Height - 1;
        HAL_WriteReg((HI_U32*)addr_REG, WBCORESO.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_FALSE;
}

HI_BOOL HAL_LAYER_GetLayerOutRect(HAL_DISP_LAYER_E enLayer, HI_RECT_S *pstRect)
{
    U_G0ORESO G0ORESO;
    U_G0DFPOS G0DFPOS;

    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0ORESO.u32));
        G0ORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        pstRect->s32Width = G0ORESO.bits.ow + 1;
        pstRect->s32Height = G0ORESO.bits.oh + 1;

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DFPOS.u32));
        G0DFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        pstRect->s32X = G0DFPOS.bits.disp_xfpos;
        pstRect->s32Y = G0DFPOS.bits.disp_yfpos;
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_LAYER_SetLayerRect(HAL_DISP_LAYER_E enLayer,
                                         HAL_DISP_RECT_S  stVideoRect)
{
    U_VHDIRESO VHDIRESO;
    U_VHDDFPOS VHDDFPOS;
    U_VHDDLPOS VHDDLPOS;
    U_VSDIRESO VSDIRESO;
    U_VSDDFPOS VSDDFPOS;
    U_VSDDLPOS VSDDLPOS;
    U_G0IRESO G0IRESO;
    U_G0ORESO G0ORESO;
    U_G0DFPOS G0DFPOS;
    U_G0DLPOS G0DLPOS;

    HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDIRESO.u32));
        VHDIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDIRESO.bits.iw = stVideoRect.u32InWidth - 1;
        VHDIRESO.bits.ih = stVideoRect.u32InHeight - 1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDIRESO.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDDFPOS.u32));
        VHDDFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDDFPOS.bits.disp_xfpos = stVideoRect.s32DXS;
        VHDDFPOS.bits.disp_yfpos = stVideoRect.s32DYS;
        HAL_WriteReg((HI_U32*)addr_REG, VHDDFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDDLPOS.u32));
        VHDDLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDDLPOS.bits.disp_xlpos = stVideoRect.s32DXL-1;
        VHDDLPOS.bits.disp_ylpos = stVideoRect.s32DYL-1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDDLPOS.u32); 
    }
    else if((LAYER_VSD_START <= enLayer) && (enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDIRESO.u32));
        VSDIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDIRESO.bits.iw = stVideoRect.u32InWidth - 1;
        VSDIRESO.bits.ih = stVideoRect.u32InHeight - 1;
        HAL_WriteReg((HI_U32*)addr_REG, VSDIRESO.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDDFPOS.u32));
        VSDDFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDDFPOS.bits.disp_xfpos = stVideoRect.s32DXS;
        VSDDFPOS.bits.disp_yfpos = stVideoRect.s32DYS;
        HAL_WriteReg((HI_U32*)addr_REG, VSDDFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDDLPOS.u32));
        VSDDLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDDLPOS.bits.disp_xlpos = stVideoRect.s32DXL-1;
        VSDDLPOS.bits.disp_ylpos = stVideoRect.s32DYL-1;
        HAL_WriteReg((HI_U32*)addr_REG, VSDDLPOS.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0IRESO.u32));
        G0IRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0IRESO.bits.iw = stVideoRect.u32InWidth - 1;
        G0IRESO.bits.ih = stVideoRect.u32InHeight - 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0IRESO.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0ORESO.u32));
        G0ORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0ORESO.bits.ow = stVideoRect.u32OutWidth - 1;
        G0ORESO.bits.oh = stVideoRect.u32OutHeight - 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0ORESO.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DFPOS.u32));
        G0DFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DFPOS.bits.disp_xfpos = stVideoRect.s32DXS;
        G0DFPOS.bits.disp_yfpos = stVideoRect.s32DYS;
        HAL_WriteReg((HI_U32*)addr_REG, G0DFPOS.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DLPOS.u32));
        G0DLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DLPOS.bits.disp_xlpos = stVideoRect.s32DXL - 1;
        G0DLPOS.bits.disp_ylpos = stVideoRect.s32DYL - 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0DLPOS.u32); 
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
    volatile U_VHDCBMPARA VHDCBMPARA;
    volatile U_VSDCBMPARA VSDCBMPARA;
    volatile U_G0CBMPARA G0CBMPARA;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCBMPARA.u32));
        VHDCBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDCBMPARA.bits.galpha = u8Alpha0;
        HAL_WriteReg((HI_U32*)addr_REG, VHDCBMPARA.u32); 
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCBMPARA.u32));
        VSDCBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDCBMPARA.bits.galpha = u8Alpha0;
        HAL_WriteReg((HI_U32*)addr_REG, VSDCBMPARA.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        G0CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CBMPARA.bits.galpha = u8Alpha0;
        HAL_WriteReg((HI_U32*)addr_REG, G0CBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_LAYER_GetLayerGalpha(HAL_DISP_LAYER_E enLayer,HI_U8 *pu8Alpha0)
{
    U_VHDCBMPARA VHDCBMPARA;
    U_VSDCBMPARA VSDCBMPARA;
    U_G0CBMPARA  G0CBMPARA;
    
    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCBMPARA.u32));
        VHDCBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu8Alpha0 = VHDCBMPARA.bits.galpha;
    }
    else if((LAYER_VSD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDCBMPARA.u32));
        VSDCBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu8Alpha0 = VSDCBMPARA.bits.galpha;
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        G0CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu8Alpha0 = G0CBMPARA.bits.galpha;
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_GetCoefAddr(HAL_DISP_LAYER_E enLayer,
                                   HAL_DISP_COEFMODE_E enMode,
                                   HI_U32 *pu32Addr)
{
    U_VHDHCOEFAD VHDHCOEFAD;
    U_VHDVCOEFAD VHDVCOEFAD;
    //U_VHD1HCOEFAD VHD1HCOEFAD;
    //U_VHD1VCOEFAD VHD1VCOEFAD;
    U_WBCHCOEFAD WBCHCOEFAD;
    U_WBCVCOEFAD WBCVCOEFAD;

    volatile  HI_U32 addr_REG;

    if(enLayer == HAL_DISP_LAYER_VHD0)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHCOEFAD.u32));
            VHDHCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Addr = VHDHCOEFAD.bits.coef_addr;
        }
        else if(enMode == HAL_DISP_COEFMODE_VER)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVCOEFAD.u32));
            VHDVCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Addr = VHDVCOEFAD.bits.coef_addr;
        }
        else
        {
            HAL_PRINT("Error coef found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    #if 0
    else if(enLayer == HAL_DISP_LAYER_VHD1)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->VHD1HCOEFAD.u32));
            VHD1HCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Addr = VHD1HCOEFAD.bits.coef_addr;
        }
        else if(enMode == HAL_DISP_COEFMODE_VER)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->VHD1VCOEFAD.u32));
            VHD1VCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Addr = VHD1VCOEFAD.bits.coef_addr;
        }
        else
        {
            HAL_PRINT("Error coef found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    #endif
    else if(enLayer == HAL_DISP_LAYER_WBC)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->WBCHCOEFAD.u32));
            WBCHCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Addr = WBCHCOEFAD.bits.coef_addr;
        }
        else if(enMode == HAL_DISP_COEFMODE_VER)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->WBCVCOEFAD.u32));
            WBCVCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            *pu32Addr = WBCVCOEFAD.bits.coef_addr;
        }
        else
        {
            HAL_PRINT("Error coef found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_FALSE;
}


HI_BOOL HAL_LAYER_SetCoefAddr(HAL_DISP_LAYER_E enLayer,
                                   HAL_DISP_COEFMODE_E enMode,
                                   HI_U32 u32Addr)
{
    U_VHDHCOEFAD VHDHCOEFAD;
    U_VHDVCOEFAD VHDVCOEFAD;
    //U_VHD1HCOEFAD VHD1HCOEFAD;
    //U_VHD1VCOEFAD VHD1VCOEFAD;
    U_WBCHCOEFAD WBCHCOEFAD;
    U_WBCVCOEFAD WBCVCOEFAD;

    volatile  HI_U32 addr_REG;

    if(enLayer == HAL_DISP_LAYER_VHD0)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHCOEFAD.u32));
            VHDHCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHCOEFAD.bits.coef_addr = u32Addr;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHCOEFAD.u32); 
        }
        else if(enMode == HAL_DISP_COEFMODE_VER)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVCOEFAD.u32));
            VHDVCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVCOEFAD.bits.coef_addr = u32Addr;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVCOEFAD.u32); 
        }
        else
        {
            HAL_PRINT("Error coef found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    #if 0
    else if(enLayer == HAL_DISP_LAYER_VHD1)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->VHD1HCOEFAD.u32));
            VHD1HCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHD1HCOEFAD.bits.coef_addr = u32Addr;
            HAL_WriteReg((HI_U32*)addr_REG, VHD1HCOEFAD.u32); 
        }
        else if(enMode == HAL_DISP_COEFMODE_VER)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->VHD1VCOEFAD.u32));
            VHD1VCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHD1VCOEFAD.bits.coef_addr = u32Addr;
            HAL_WriteReg((HI_U32*)addr_REG, VHD1VCOEFAD.u32); 
        }
        else
        {
            HAL_PRINT("Error coef found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    #endif
    else if(enLayer == HAL_DISP_LAYER_WBC)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->WBCHCOEFAD.u32));
            WBCHCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHCOEFAD.bits.coef_addr = u32Addr;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHCOEFAD.u32); 
        }
        else if(enMode == HAL_DISP_COEFMODE_VER)
        {
            addr_REG = Vou_GetAbsAddr(0,(HI_U32)&(pVoReg->WBCVCOEFAD.u32));
            WBCVCOEFAD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVCOEFAD.bits.coef_addr = u32Addr;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVCOEFAD.u32); 
        }
        else
        {
            HAL_PRINT("Error coef found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_FALSE;
}

HI_BOOL HAL_LAYER_SetLayerParaUpd(HAL_DISP_LAYER_E enLayer,
                                  HAL_DISP_COEFMODE_E enMode)
{
    U_VOPARAUP VOPARAUP;

    VOPARAUP.u32 = HAL_ReadReg((HI_U32 *)&(pVoReg->VOPARAUP.u32));

    if(enLayer == HAL_DISP_LAYER_VHD0)
    {

        if(enMode == HAL_DISP_COEFMODE_HOR || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.vhd_hcoef_upd = 0x1;
        }

        if(enMode == HAL_DISP_COEFMODE_VER || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.vhd_vcoef_upd = 0x1;
        }
    }
    #if 0
    else if(enLayer == HAL_DISP_LAYER_VHD1)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.vhd1_hcoef_upd = 0x1;
        }

        if(enMode == HAL_DISP_COEFMODE_VER || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.vhd1_vcoef_upd = 0x1;
        }
    }
    #endif
    #if 0
    /* hi3521的图形层没有缩放能力 */
    else if(enLayer == HAL_DISP_LAYER_GFX0)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.g0_hcoef_upd = 0x1;
        }

        if(enMode == HAL_DISP_COEFMODE_VER || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.g0_vcoef_upd = 0x1;
        }
    }
    #endif
    #if 0
    else if(enLayer == HAL_DISP_LAYER_GFX1)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.g1_hcoef_upd = 0x1;
        }

        if(enMode == HAL_DISP_COEFMODE_VER || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.g1_vcoef_upd = 0x1;
        }
    }
    #endif
    else if(enLayer == HAL_DISP_LAYER_WBC)
    {
        if(enMode == HAL_DISP_COEFMODE_HOR || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.wbc_hcoef_upd = 0x1;
        }

        if(enMode == HAL_DISP_COEFMODE_VER || enMode == HAL_DISP_COEFMODE_ALL)
        {
            VOPARAUP.bits.wbc_vcoef_upd = 0x1;
        }

    }
    else
    {
        HAL_PRINT("Error, Wrong coef update layer select\n");
        return HI_FALSE;
    }
    
	HAL_WriteReg((HI_U32*)&(pVoReg->VOPARAUP.u32), VOPARAUP.u32); 

    return HI_TRUE;
}      

HI_BOOL HAL_LAYER_SetZmeEnable(HAL_DISP_LAYER_E enLayer,
                                    HAL_DISP_ZMEMODE_E enMode,
                                    HI_U32 bEnable)
{
    volatile U_VHDHSP VHDHSP;
    volatile U_VHDVSP VHDVSP;
    //U_G0HSP G0HSP;
   // U_G0VSP G0VSP;
    volatile U_WBCHSP WBCHSP;
    volatile U_WBCVSP WBCVSP;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer < LAYER_VHD_END))
    {
        /* layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
            VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHSP.bits.hlmsc_en = bEnable;
            VHDHSP.bits.hlfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
            VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHSP.bits.hchmsc_en = bEnable;
            VHDHSP.bits.hchfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32);
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vlmsc_en = bEnable;
            VHDVSP.bits.vlfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vchmsc_en = bEnable;
            VHDVSP.bits.vchfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
        #if 0
        /* graphic0 layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0HSP.u32));
            G0HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            G0HSP.bits.hsc_en = bEnable;
            G0HSP.bits.hfir_en = 1;
            G0HSP.bits.hafir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, G0HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0VSP.u32));
            G0VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            G0VSP.bits.vsc_en = bEnable;
            G0VSP.bits.vfir_en = 1;
            G0VSP.bits.vafir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, G0VSP.u32); 
        }
        #endif
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        /* VSD layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
            WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHSP.bits.hlmsc_en = bEnable;
            WBCHSP.bits.hlfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
            WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHSP.bits.hchmsc_en = bEnable;
            WBCHSP.bits.hchfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vlmsc_en = bEnable;
            WBCVSP.bits.vlfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vchmsc_en = bEnable;
            WBCVSP.bits.vchfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetZmeFirEnable(HAL_DISP_LAYER_E enLayer, HAL_DISP_ZMEMODE_E enMode, HI_U32 bEnable)
{
    U_VHDHSP VHDHSP;
    U_VHDVSP VHDVSP;
    //U_G0HSP G0HSP;
    //U_G0VSP G0VSP;
    U_WBCHSP WBCHSP;
    U_WBCVSP WBCVSP;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer < LAYER_VHD_END))
    {
        /* layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
            VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHSP.bits.hlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
            VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHSP.bits.hchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        /* VSD layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
            WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHSP.bits.hlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
            WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHSP.bits.hchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetZmeMscEnable(HAL_DISP_LAYER_E enLayer, HAL_DISP_ZMEMODE_E enMode, HI_U32 bEnable)
{
    U_VHDHSP VHDHSP;
    U_VHDVSP VHDVSP;
    //U_G0HSP G0HSP;
    //U_G0VSP G0VSP;
    U_WBCHSP WBCHSP;
    U_WBCVSP WBCVSP;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer < LAYER_VHD_END))
    {
        /* layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
            VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHSP.bits.hlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
            VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHSP.bits.hchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
            WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHSP.bits.hlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
            WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHSP.bits.hchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL  HAL_LAYER_SetMidEnable(HAL_DISP_LAYER_E enLayer,
                                    HAL_DISP_ZMEMODE_E enMode,
                                    HI_U32 bEnable)
{
    return HI_TRUE;   
}

HI_BOOL HAL_LAYER_SetHfirOrder(HAL_DISP_LAYER_E enLayer, HI_U32 uHfirOrder)
{
    #if 0
    volatile U_VHDHSP VHDHSP;
    volatile U_WBCHSP WBCHSP;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
        VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDHSP.bits.hfir_order = uHfirOrder;
        HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
        WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCHSP.bits.hfir_order = uHfirOrder;
        HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}


HI_BOOL HAL_LAYER_SetZmeVerTap(HAL_DISP_LAYER_E enLayer, HAL_DISP_ZMEMODE_E enMode, HI_U32 uVerTap)
{
    #if 0
    volatile U_VHDVSP VHDVSP;
    volatile U_WBCVSP WBCVSP;
    volatile HI_U32 addr_REG;

    if (  (enMode == HAL_DISP_ZMEMODE_HORL) || (enMode == HAL_DISP_ZMEMODE_HOR) \
       || (enMode == HAL_DISP_ZMEMODE_HORC))
    {
        HAL_PRINT("Error! HAL_SetZmeVerTap enMode is error!\n");
    }


    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if ((enMode == HAL_DISP_ZMEMODE_VERL) || (enMode == HAL_DISP_ZMEMODE_VER) || (enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));

            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vsc_luma_tap = uVerTap;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }

        if ((enMode == HAL_DISP_ZMEMODE_VERC) || (enMode == HAL_DISP_ZMEMODE_VER) || (enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));

            VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSP.bits.vsc_chroma_tap = uVerTap;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
        }
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        if ((enMode == HAL_DISP_ZMEMODE_VERL) || (enMode == HAL_DISP_ZMEMODE_VER) || (enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vsc_luma_tap = uVerTap;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }


        if ((enMode == HAL_DISP_ZMEMODE_VERC) || (enMode == HAL_DISP_ZMEMODE_VER) || (enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVSP.bits.vsc_chroma_tap = uVerTap;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}


HI_BOOL HAL_LAYER_SetZmeVerType(HAL_DISP_LAYER_E enLayer, HI_U32 uVerType)
{
    U_VHDVSP VHDVSP;
    U_WBCVSP WBCVSP;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
        VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDVSP.bits.zme_in_fmt = uVerType;
        HAL_WriteReg((HI_U32*)addr_REG, VHDVSP.u32); 
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSP.u32));
        WBCVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCVSP.bits.zme_in_fmt = uVerType;
        HAL_WriteReg((HI_U32*)addr_REG, WBCVSP.u32); 
    }
    else
    {
        //当视频层为PIP时，不对寄存器进行处理        
    }

    return HI_TRUE;
}


HI_BOOL  HAL_LAYER_SetZmePhase(HAL_DISP_LAYER_E enLayer,
                                   HAL_DISP_ZMEMODE_E enMode,
                                   HI_S32 s32Phase)
{                                    
    U_VHDHLOFFSET VHDHLOFFSET;
    U_VHDHCOFFSET VHDHCOFFSET;
    U_VHDVOFFSET VHDVOFFSET;
    U_WBCHLOFFSET WBCHLOFFSET;
    U_WBCHCOFFSET WBCHCOFFSET;
    U_WBCVOFFSET  WBCVOFFSET;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((enMode == HAL_DISP_ZMEMODE_HORL) || (enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHLOFFSET.u32));
            VHDHLOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHLOFFSET.bits.hor_loffset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHLOFFSET.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC) || (enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHCOFFSET.u32));
            VHDHCOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHCOFFSET.bits.hor_coffset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, VHDHCOFFSET.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVOFFSET.u32));
            VHDVOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVOFFSET.bits.vluma_offset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVOFFSET.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVOFFSET.u32));
            VHDVOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVOFFSET.bits.vchroma_offset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, VHDVOFFSET.u32); 
        }
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        if((enMode == HAL_DISP_ZMEMODE_HORL) || (enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHLOFFSET.u32));
            WBCHLOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHLOFFSET.bits.hor_loffset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHLOFFSET.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC) || (enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHLOFFSET.u32));
            WBCHLOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCHCOFFSET.bits.hor_coffset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, WBCHLOFFSET.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVOFFSET.u32));
            WBCVOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVOFFSET.bits.vluma_offset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVOFFSET.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVOFFSET.u32));
            WBCVOFFSET.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBCVOFFSET.bits.vchroma_offset = s32Phase;
            HAL_WriteReg((HI_U32*)addr_REG, WBCVOFFSET.u32); 
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetZmeIReso(HAL_DISP_LAYER_E enLayer, HI_U32 u32Width, HI_U32 u32Height)
{
    volatile U_VHDZMEIRESO VHDZMEIRESO;
    volatile U_WBCZMEIRESO WBCZMEIRESO;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDZMEIRESO.u32));
        VHDZMEIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDZMEIRESO.bits.iw = u32Width  - 1;
        VHDZMEIRESO.bits.ih = u32Height - 1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDZMEIRESO.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCZMEIRESO.u32));
        WBCZMEIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCZMEIRESO.bits.iw = u32Width  - 1;
        WBCZMEIRESO.bits.ih = u32Height - 1;
        HAL_WriteReg((HI_U32*)addr_REG, WBCZMEIRESO.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_LAYER_GetZmeIReso(HAL_DISP_LAYER_E enLayer, 
                                         HI_U32 *pu32Width, HI_U32 *pu32Height)
{
    volatile U_VHDZMEIRESO VHDZMEIRESO;
    volatile U_WBCZMEIRESO WBCZMEIRESO;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDZMEIRESO.u32));
        VHDZMEIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Width = VHDZMEIRESO.bits.iw + 1;
        *pu32Height = VHDZMEIRESO.bits.ih + 1;
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCZMEIRESO.u32));
        WBCZMEIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Width = WBCZMEIRESO.bits.iw + 1;
        *pu32Height = WBCZMEIRESO.bits.ih + 1;

    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetZmeOReso(HAL_DISP_LAYER_E enLayer, HI_U32 u32Width, HI_U32 u32Height)
{
    volatile U_VHDZMEORESO VHDZMEORESO;  
    volatile U_WBCZMEORESO WBCZMEORESO;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDZMEORESO.u32));
        VHDZMEORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDZMEORESO.bits.ow = u32Width  - 1;
        VHDZMEORESO.bits.oh = u32Height - 1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDZMEORESO.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCZMEORESO.u32));
        WBCZMEORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCZMEORESO.bits.ow = u32Width  - 1;
        WBCZMEORESO.bits.oh = u32Height - 1;
        HAL_WriteReg((HI_U32*)addr_REG, WBCZMEORESO.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_GetZmeOReso(HAL_DISP_LAYER_E enLayer, 
                                           HI_U32 *pu32Width, HI_U32 *pu32Height)
{
    volatile U_VHDZMEORESO VHDZMEORESO;
    volatile U_WBCZMEORESO WBCZMEORESO;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDZMEORESO.u32));
        VHDZMEORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Width = VHDZMEORESO.bits.ow + 1;
        *pu32Height = VHDZMEORESO.bits.oh + 1;
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCZMEORESO.u32));
        WBCZMEORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu32Width = WBCZMEORESO.bits.ow + 1;
        *pu32Height = WBCZMEORESO.bits.oh + 1;
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiEnable(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_U32 u32En)
{
    #if 0
    U_VHDLTI_CTRL VHDLTI_CTRL;
    U_VHDCTI_CTRL VHDCTI_CTRL;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLTI_CTRL.u32));
            VHDLTI_CTRL.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLTI_CTRL.bits.lti_en = u32En;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLTI_CTRL.u32);
        }

        if((u32Mode == HAL_DISP_TIMODE_CHM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTI_CTRL.u32));
            VHDCTI_CTRL.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDCTI_CTRL.bits.cti_en = u32En;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDCTI_CTRL.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE ;
}

HI_BOOL HAL_LAYER_SetTiGainRatio(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_S32 s32Data)
{
    #if 0
    U_VHDLTI_CTRL VHDLTI_CTRL;
    U_VHDCTI_CTRL VHDCTI_CTRL;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLTI_CTRL.u32));
            VHDLTI_CTRL.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLTI_CTRL.bits.lgain_ratio = s32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLTI_CTRL.u32);
        }

        if((u32Mode == HAL_DISP_TIMODE_CHM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTI_CTRL.u32));
            VHDCTI_CTRL.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDCTI_CTRL.bits.cgain_ratio = s32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDCTI_CTRL.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiMixRatio(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_U32 u32Data)
{
    #if 0
    U_VHDLTI_CTRL VHDLTI_CTRL;
    U_VHDCTI_CTRL VHDCTI_CTRL;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLTI_CTRL.u32));
            VHDLTI_CTRL.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLTI_CTRL.bits.lmixing_ratio = u32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLTI_CTRL.u32);
        }

        if((u32Mode == HAL_DISP_TIMODE_CHM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTI_CTRL.u32));
            VHDCTI_CTRL.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDCTI_CTRL.bits.cmixing_ratio = u32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDCTI_CTRL.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiHfThd(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_U32 * u32Data)
{
    #if 0
    U_VHDLHFREQ_THRE VHDLHFREQ_THRE;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLHFREQ_THRE.u32));
            VHDLHFREQ_THRE.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLHFREQ_THRE.bits.lhfreq_thre0 = u32Data[0];
            VHDLHFREQ_THRE.bits.lhfreq_thre1 = u32Data[1];
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLHFREQ_THRE.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiHpCoef(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_S8 * s8Data)
{
    #if 0
    U_VHDLTI_CTRL VHDLTI_CTRL;
    U_VHDLHPASS_COEF VHDLHPASS_COEF;
    U_VHDCHPASS_COEF VHDCHPASS_COEF;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLHPASS_COEF.u32));
            VHDLHPASS_COEF.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLHPASS_COEF.bits.lhpass_coef0 = s8Data[0];
            VHDLHPASS_COEF.bits.lhpass_coef1 = s8Data[1];
            VHDLHPASS_COEF.bits.lhpass_coef2 = s8Data[2];
            VHDLHPASS_COEF.bits.lhpass_coef3 = s8Data[3];
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLHPASS_COEF.u32);

            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLTI_CTRL.u32));
            VHDLTI_CTRL.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLTI_CTRL.bits.lhpass_coef4    = s8Data[4];
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLTI_CTRL.u32);
        }

        if((u32Mode == HAL_DISP_TIMODE_CHM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCHPASS_COEF.u32));
            VHDCHPASS_COEF.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDCHPASS_COEF.bits.chpass_coef0 = s8Data[0];
            VHDCHPASS_COEF.bits.chpass_coef1 = s8Data[1];
            VHDCHPASS_COEF.bits.chpass_coef2 = s8Data[2];
            HAL_WriteReg((HI_U32 *)addr_REG, VHDCHPASS_COEF.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiCoringThd(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_U32 u32Data)
{
    #if 0
    U_VHDLTI_THRE VHDLTI_THRE;
    U_VHDCTI_THRE VHDCTI_THRE;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLTI_THRE.u32));
            VHDLTI_THRE.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLTI_THRE.bits.lcoring_thre = u32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLTI_THRE.u32);
        }

        if((u32Mode == HAL_DISP_TIMODE_CHM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTI_THRE.u32));
            VHDCTI_THRE.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDCTI_THRE.bits.ccoring_thre = u32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDCTI_THRE.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiSwingThd(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_U32 u32Data)
{
    #if 0
    U_VHDLTI_THRE VHDLTI_THRE;
    U_VHDCTI_THRE VHDCTI_THRE;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLTI_THRE.u32));
            VHDLTI_THRE.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLTI_THRE.bits.lswing_thre = u32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLTI_THRE.u32);
        }

        if((u32Mode == HAL_DISP_TIMODE_CHM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDCTI_THRE.u32));
            VHDCTI_THRE.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDCTI_THRE.bits.cswing_thre = u32Data;
            HAL_WriteReg((HI_U32 *)addr_REG, VHDCTI_THRE.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiGainCoef(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode, HI_U32 * u32Data)
{
    #if 0
    U_VHDLGAIN_COEF VHDLGAIN_COEF;

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if((u32Mode == HAL_DISP_TIMODE_LUM) || (u32Mode == HAL_DISP_TIMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDLGAIN_COEF.u32));
            VHDLGAIN_COEF.u32 = HAL_ReadReg((HI_U32 *)addr_REG);
            VHDLGAIN_COEF.bits.lgain_coef0 = u32Data[0];
            VHDLGAIN_COEF.bits.lgain_coef1 = u32Data[1];
            VHDLGAIN_COEF.bits.lgain_coef2 = u32Data[2];
            HAL_WriteReg((HI_U32 *)addr_REG, VHDLGAIN_COEF.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetTiDefThd(HAL_DISP_LAYER_E enLayer, HI_U32 u32Mode)
{
    #if 0
    HI_U32 u32HfThd[2]    = {20,512};
    HI_U32 u32GainCoef[3] = {152,118,108};
    HI_S8  s8LtiHpCoef[5] = {-19,-8,-2,-1,-1};
    HI_S8  s8CtiHpCoef[5] = {-21,-21,-11,0,0};

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        if(u32Mode == HAL_DISP_TIMODE_LUM)
        {
            HAL_LAYER_SetTiGainRatio(enLayer, u32Mode, 255);
            HAL_LAYER_SetTiMixRatio (enLayer, u32Mode, 128);
            HAL_LAYER_SetTiCoringThd(enLayer, u32Mode, 0);
            HAL_LAYER_SetTiSwingThd (enLayer, u32Mode, 6);
            HAL_LAYER_SetTiHfThd    (enLayer, u32Mode, u32HfThd);
            HAL_LAYER_SetTiGainCoef (enLayer, u32Mode, u32GainCoef);
            HAL_LAYER_SetTiHpCoef   (enLayer, u32Mode, s8LtiHpCoef);
        }

        if(u32Mode == HAL_DISP_TIMODE_CHM)
        {
            HAL_LAYER_SetTiGainRatio(enLayer, u32Mode, 255);
            HAL_LAYER_SetTiMixRatio (enLayer, u32Mode, 128);
            HAL_LAYER_SetTiCoringThd(enLayer, u32Mode, 4);
            HAL_LAYER_SetTiSwingThd (enLayer, u32Mode, 6);
            HAL_LAYER_SetTiHfThd    (enLayer, u32Mode, u32HfThd);
            HAL_LAYER_SetTiGainCoef (enLayer, u32Mode, u32GainCoef);
            HAL_LAYER_SetTiHpCoef   (enLayer, u32Mode, s8CtiHpCoef);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    return HI_TRUE;
}



HI_BOOL HAL_LAYER_SetVerRatio(HAL_DISP_LAYER_E enLayer, HI_U32 uRatio)
{
    volatile U_VHDVSR VHDVSR;
    volatile U_WBCVSR WBCVSR;

    volatile HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSR.u32));
        VHDVSR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDVSR.bits.vratio = uRatio;
        HAL_WriteReg((HI_U32*)addr_REG, VHDVSR.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCVSR.u32));
        WBCVSR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCVSR.bits.vratio = uRatio;
        HAL_WriteReg((HI_U32*)addr_REG, WBCVSR.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetHorRatio(HAL_DISP_LAYER_E enLayer, HI_U32 uRatio)
{
    volatile U_VHDHSP VHDHSP;
    volatile U_WBCHSP WBCHSP;

    volatile HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
        VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDHSP.bits.hratio = uRatio;
        HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCHSP.u32));
        WBCHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCHSP.bits.hratio = uRatio;
        HAL_WriteReg((HI_U32*)addr_REG, WBCHSP.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetZmeRatio(HAL_DISP_LAYER_E enLayer)
{
    U_VHDHSP VHDHSP;
    U_VHDVSP VHDVSP;
    U_VHDVSR VHDVSR;
    HI_U32 u32InWidth   = 0;
    HI_U32 u32InHeight  = 0;
    HI_U32 u32OutWidth  = 0;  
    HI_U32 u32OutHeight = 0;

    U_VHDZMEIRESO VHDZMEIRESO;
    U_VHDZMEORESO VHDZMEORESO;
    
    volatile volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer < LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
        VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSP.u32));
        VHDVSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        if( VHDHSP.bits.hlmsc_en | VHDHSP.bits.hchmsc_en \
            | VHDVSP.bits.vlmsc_en | VHDVSP.bits.vchmsc_en)
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDZMEIRESO.u32));
            VHDZMEIRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDZMEORESO.u32));
            VHDZMEORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);

            u32InWidth   = VHDZMEIRESO.bits.iw+ 1;
            u32InHeight  = VHDZMEIRESO.bits.ih+ 1;
            u32OutWidth  = VHDZMEORESO.bits.ow+ 1;
            u32OutHeight = VHDZMEORESO.bits.oh+ 1;
            //printk("22222  u32InWidth =%d %d %d %d ,",u32InWidth,u32InHeight,u32OutWidth,u32OutHeight);
                                             
            /* set VHD zoom ratio */
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDHSP.u32));
            VHDHSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDHSP.bits.hratio = (HI_U32)(u32InWidth*4096/u32OutWidth);
            HAL_WriteReg((HI_U32*)addr_REG, VHDHSP.u32);

            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDVSR.u32));
            VHDVSR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            VHDVSR.bits.vratio = (HI_U32)(u32InHeight*4096/u32OutHeight);
            HAL_WriteReg((HI_U32*)addr_REG, VHDVSR.u32);
        }
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

     return HI_TRUE;
}

HI_BOOL  HAL_LAYER_SetRegUp(HAL_DISP_LAYER_E enLayer)
{
    U_VHDUPD VHDUPD;
    U_VSDUPD VSDUPD;
    U_G0UPD G0UPD;
    U_WBCUPD WBCUPD;
    

    volatile HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer) && (enLayer <= LAYER_VHD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VHDUPD.u32));
        VHDUPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VHDUPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, VHDUPD.u32); 
    }
    else if((LAYER_VSD_START <= enLayer) && (enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->VSDUPD.u32));
        VSDUPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        VSDUPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, VSDUPD.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0UPD.u32));
        G0UPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0UPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, G0UPD.u32); 
    }
    else if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCUPD.u32));
        WBCUPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        /* G1 layer register update */
        WBCUPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, WBCUPD.u32); 
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
    U_G0ADDR G0ADDR;
    volatile  HI_U32 addr_REG;
    
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0ADDR.u32));
        G0ADDR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0ADDR.u32 = u32LAddr;
        HAL_WriteReg((HI_U32*)addr_REG, G0ADDR.u32); 

    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_GetGfxAddr(HAL_DISP_LAYER_E enLayer, HI_U32 *pu32GfxAddr)
{
    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0ADDR.u32));
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    *pu32GfxAddr = HAL_ReadReg((HI_U32*)addr_REG);
    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_SetGfxStride(HAL_DISP_LAYER_E enLayer, HI_U16 u16pitch)
{
    volatile U_G0STRIDE G0STRIDE;
    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0STRIDE.u32));
        G0STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0STRIDE.bits.surface_stride = u16pitch;
        HAL_WriteReg((HI_U32*)addr_REG, G0STRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_GetGfxStride(HAL_DISP_LAYER_E enLayer, HI_U32 *pu32GfxStride)
{
    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0STRIDE.u32));
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    *pu32GfxStride = HAL_ReadReg((HI_U32*)addr_REG);
    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_SetGfxExt(HAL_DISP_LAYER_E enLayer,
                                HAL_GFX_BITEXTEND_E enMode)
{
    U_G0CTRL G0CTRL;

    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));
        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CTRL.bits.bitext = enMode;
        HAL_WriteReg((HI_U32*)addr_REG, G0CTRL.u32); 

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
    U_G0CBMPARA G0CBMPARA;
    volatile  HI_U32 addr_REG;

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        G0CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CBMPARA.bits.premult_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, G0CBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_GetGfxPreMult(HAL_DISP_LAYER_E enLayer, HI_U32 *pbEnable)
{
    U_G0CBMPARA G0CBMPARA;
    volatile  HI_U32 addr_REG;

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        G0CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pbEnable = G0CBMPARA.bits.premult_en;
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
                                   HI_U8 u8Alpha0,HI_U8 u8Alpha1)
{
    U_G0CBMPARA G0CBMPARA;
    U_G0CKEYMIN G0CKEYMIN;
    U_G0CKEYMAX G0CKEYMAX;

    volatile HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        G0CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CBMPARA.bits.palpha_en = bAlphaEn; 
        HAL_WriteReg((HI_U32*)addr_REG, G0CBMPARA.u32); 

        if(HI_TRUE == bAlphaEn)
        {
	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMIN.u32));
	        G0CKEYMIN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0CKEYMIN.bits.va1 = u8Alpha1;
	        HAL_WriteReg((HI_U32*)addr_REG, G0CKEYMIN.u32); 

	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMAX.u32));
	        G0CKEYMAX.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0CKEYMAX.bits.va0 = u8Alpha0;                            
	        HAL_WriteReg((HI_U32*)addr_REG, G0CKEYMAX.u32); 
        }
		else
		{
	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMIN.u32));
	        G0CKEYMIN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0CKEYMIN.bits.va1 = 0xff;
	        HAL_WriteReg((HI_U32*)addr_REG, G0CKEYMIN.u32); 

	        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMAX.u32));
	        G0CKEYMAX.u32 = HAL_ReadReg((HI_U32*)addr_REG);
	        G0CKEYMAX.bits.va0 = 0xff;                            
	        HAL_WriteReg((HI_U32*)addr_REG, G0CKEYMAX.u32); 
        }	
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_GetGfxPalpha(HAL_DISP_LAYER_E enLayer, HI_U32 *pbAlphaEn, HI_U32 *pbArange,
                         HI_U8 *pu8Alpha0, HI_U8 *pu8Alpha1)
{
    U_G0CBMPARA G0CBMPARA;
    U_G0CKEYMIN G0CKEYMIN;
    U_G0CKEYMAX G0CKEYMAX;
    volatile  HI_U32 addr_REG;
    
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        G0CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pbAlphaEn = G0CBMPARA.bits.palpha_en;

        *pbArange = G0CBMPARA.bits.palpha_range;
        
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMIN.u32));
        G0CKEYMIN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu8Alpha0 = G0CKEYMIN.bits.va1;

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMAX.u32));
        G0CKEYMAX.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *pu8Alpha1 = G0CKEYMAX.bits.va0;
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
    U_G0CBMPARA G0CBMPARA;

    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        G0CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CBMPARA.bits.palpha_range = bArange;            
        HAL_WriteReg((HI_U32*)addr_REG, G0CBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetGfxKeyEn(HAL_DISP_LAYER_E enLayer, HI_U32 u32KeyEnable)
{
    U_HCCBMPARA HCCBMPARA;

    volatile  HI_U32 addr_REG;
    if((HAL_DISP_LAYER_GFX0 <= enLayer)&&(enLayer <= HAL_DISP_LAYER_HC0))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        HCCBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        HCCBMPARA.bits.key_en = u32KeyEnable;
        HAL_WriteReg((HI_U32*)addr_REG, HCCBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id %d not support colorkey in %s: L%d\n",
            (HI_S32)enLayer, __FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetGfxKeyMode(HAL_DISP_LAYER_E enLayer, HI_U32 u32KeyOut)
{
    U_HCCBMPARA HCCBMPARA;

    volatile  HI_U32 addr_REG;
    if((HAL_DISP_LAYER_GFX0 <= enLayer)&&(enLayer <= HAL_DISP_LAYER_HC0))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CBMPARA.u32));
        HCCBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        HCCBMPARA.bits.key_mode = u32KeyOut;
        HAL_WriteReg((HI_U32*)addr_REG, HCCBMPARA.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id %d not support colorkey mode in %s: L%d\n",
            (HI_S32)enLayer, __FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetColorKeyValue(HAL_DISP_LAYER_E enLayer, 
        HAL_GFX_KEY_MAX_S stKeyMax, HAL_GFX_KEY_MIN_S stKeyMin)
{
    U_HCCKEYMAX HCCKEYMAX;
    U_HCCKEYMIN HCCKEYMIN;

    volatile  HI_U32 addr_REG;
    if((HAL_DISP_LAYER_GFX0 <= enLayer)&&(enLayer <= HAL_DISP_LAYER_HC0))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMAX.u32));
        HCCKEYMAX.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        HCCKEYMAX.bits.keyr_max = stKeyMax.u8KeyMax_R;
        HCCKEYMAX.bits.keyg_max = stKeyMax.u8KeyMax_G;
        HCCKEYMAX.bits.keyb_max = stKeyMax.u8KeyMax_B;
        HAL_WriteReg((HI_U32*)addr_REG, HCCKEYMAX.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CKEYMIN.u32));
        HCCKEYMIN.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        HCCKEYMIN.bits.keyr_min = stKeyMin.u8KeyMin_R;
        HCCKEYMIN.bits.keyg_min = stKeyMin.u8KeyMin_G;
        HCCKEYMIN.bits.keyb_min = stKeyMin.u8KeyMin_B;
        HAL_WriteReg((HI_U32*)addr_REG, HCCKEYMIN.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id %d not support colorkey in %s: L%d\n",
            (HI_S32)enLayer, __FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetColorKeyMask(HAL_DISP_LAYER_E enLayer, HAL_GFX_MASK_S stMsk)
{
    U_HCCMASK HCCMASK;

    volatile  HI_U32 addr_REG;
    if((HAL_DISP_LAYER_GFX0 <= enLayer)&&(enLayer <= HAL_DISP_LAYER_HC0))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CMASK.u32));
        HCCMASK.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        HCCMASK.bits.kmsk_r = stMsk.u8Mask_r;
        HCCMASK.bits.kmsk_g = stMsk.u8Mask_g;
        HCCMASK.bits.kmsk_b = stMsk.u8Mask_b;
        HAL_WriteReg((HI_U32*)addr_REG, HCCMASK.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id %d not support colorkey mask in %s: L%d\n",
            (HI_S32)enLayer, __FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetGfxUpdMode(HAL_DISP_LAYER_E enLayer, HI_U32 u32Data)
{
    U_G0CTRL G0CTRL;

    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));
        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CTRL.bits.upd_mode = u32Data;
        HAL_WriteReg((HI_U32*)addr_REG, G0CTRL.u32); 

    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetGfxReadMode(HAL_DISP_LAYER_E enLayer, HI_U32 u32Data)
{
    U_G0CTRL G0CTRL;

    volatile  HI_U32 addr_REG;
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));
        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CTRL.bits.read_mode = u32Data;
        HAL_WriteReg((HI_U32*)addr_REG, G0CTRL.u32); 

    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }    

    return HI_TRUE;    
}

HI_BOOL HAL_GRAPHIC_SetGfxDcmpEnable(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable)
{
    volatile U_G0CTRL G0CTRL;
    volatile  HI_U32 addr_REG;

    if((HAL_DISP_LAYER_GFX0 <= enLayer)&&(enLayer <= HAL_DISP_LAYER_GFX3))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));

        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CTRL.bits.dcmp_mode = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, G0CTRL.u32); 
    }
    else
    {
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_SetGfxDcmpBank(HAL_DISP_LAYER_E enLayer, HI_U32 bank)
{
    volatile U_G0DCMPBANKWIDTH G0DCMPBANKWIDTH;
    volatile  HI_U32 addr_REG;

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DCMPBANKWIDTH.u32));
        G0DCMPBANKWIDTH.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DCMPBANKWIDTH.bits.dcmp_bankwidth= bank - 1;
        HAL_WriteReg((HI_U32*)addr_REG, G0DCMPBANKWIDTH.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_SetGfxDcmpStride(HAL_DISP_LAYER_E enLayer, HI_U32 stride)
{
    volatile U_G0DCMPSTRIDE G0DCMPSTRIDE;
    volatile  HI_U32 addr_REG;

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DCMPSTRIDE.u32));
        G0DCMPSTRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DCMPSTRIDE.bits.dcmp_stride= stride;
        HAL_WriteReg((HI_U32*)addr_REG, G0DCMPSTRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_SetGfxDcmpAddr(HAL_DISP_LAYER_E enLayer, HI_U32 addr_a, HI_U32 addr_r, HI_U32 addr_g, HI_U32 addr_b)
{
    volatile U_G0DCMPAADDR G0DCMPAADDR;
    volatile U_G0DCMPRADDR G0DCMPRADDR;
    volatile U_G0DCMPGADDR G0DCMPGADDR;
    volatile U_G0DCMPBADDR G0DCMPBADDR;
    volatile  HI_U32 addr_REG;

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DCMPAADDR.u32));
        G0DCMPAADDR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DCMPAADDR.bits.dcmp_addr_a= addr_a;
        HAL_WriteReg((HI_U32*)addr_REG, G0DCMPAADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DCMPRADDR.u32));
        G0DCMPRADDR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DCMPRADDR.bits.dcmp_addr_r= addr_r;
        HAL_WriteReg((HI_U32*)addr_REG, G0DCMPRADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DCMPGADDR.u32));
        G0DCMPGADDR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DCMPGADDR.bits.dcmp_addr_g= addr_g;
        HAL_WriteReg((HI_U32*)addr_REG, G0DCMPGADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0DCMPBADDR.u32));
        G0DCMPBADDR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0DCMPBADDR.bits.dcmp_addr_b= addr_b;
        HAL_WriteReg((HI_U32*)addr_REG, G0DCMPBADDR.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_GRAPHIC_SetGfxDcmpInterModeEn(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable)
{
    volatile U_G0CTRL G0CTRL;
    volatile  HI_U32 addr_REG;

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0CTRL.u32));

        G0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        G0CTRL.bits.dcmp_inter = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, G0CTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
} 


HI_BOOL HAL_CBM_SetCbmAttr(HAL_DISP_LAYER_E enLayer, HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    U_CBMATTR CBMATTR;

    CBMATTR.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMATTR.u32));
    if(enLayer == HAL_DISP_LAYER_VSD0)
    {
        if(enChan == HAL_DISP_CHANNEL_DHD0)
        {
            CBMATTR.bits.sur_attr0 = 0x0;
        }
        #if 0
        else if(enChan == HAL_DISP_CHANNEL_DHD1)
        {
            CBMATTR.bits.sur_attr0 = 0x1;
        }
        #endif
        else if(enChan == HAL_DISP_CHANNEL_DSD0)
        {
            CBMATTR.bits.sur_attr0 = 0x1;
        }
        else
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    else if(enLayer == HAL_DISP_LAYER_HC0)
    {
        if(enChan == HAL_DISP_CHANNEL_DHD0)
        {
            CBMATTR.bits.sur_attr1 = 0x0;
        }
        #if 0
        else if(enChan == HAL_DISP_CHANNEL_DHD1)
        {
            CBMATTR.bits.sur_attr1 = 0x1;
        }
        #endif
        else if(enChan == HAL_DISP_CHANNEL_DSD0)
        {
            CBMATTR.bits.sur_attr1 = 0x1;
        }
        else if(enChan == HAL_DISP_CHANNEL_DSD1)
        {
            CBMATTR.bits.sur_attr1 = 0x2;
        }
        else
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    #if 0
    else if(enLayer == HAL_DISP_LAYER_HC1)
    {
        if(enChan == HAL_DISP_CHANNEL_DHD0)
        {
            CBMATTR.bits.sur_attr2 = 0x0;
        }
        else if(enChan == HAL_DISP_CHANNEL_DHD1)
        {
            CBMATTR.bits.sur_attr2 = 0x1;
        }
        else if(enChan == HAL_DISP_CHANNEL_DSD0)
        {
            CBMATTR.bits.sur_attr2 = 0x2;
        }
        else if(enChan == HAL_DISP_CHANNEL_DSD1)
        {
            CBMATTR.bits.sur_attr2 = 0x3;
        }
        else
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    else if(enLayer == HAL_DISP_LAYER_GFX4)
    {
        if(enChan == HAL_DISP_CHANNEL_DHD0)
        {
            CBMATTR.bits.sur_attr3 = 0x0;
        }
        else if(enChan == HAL_DISP_CHANNEL_DHD1)
        {
            CBMATTR.bits.sur_attr3 = 0x1;
        }
        else if(enChan == HAL_DISP_CHANNEL_DSD0)
        {
            CBMATTR.bits.sur_attr3 = 0x2;
        }
        else if(enChan == HAL_DISP_CHANNEL_DSD1)
        {
            CBMATTR.bits.sur_attr3 = 0x3;
        }
        else
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }
    #endif
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    HAL_WriteReg((HI_U32*)&(pVoReg->CBMATTR.u32), CBMATTR.u32); 
    return HI_TRUE;
}

HI_BOOL HAL_CBM_ReleaseCbmAttr(HAL_DISP_LAYER_E enLayer)
{
    U_CBMATTR CBMATTR;

    CBMATTR.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMATTR.u32));
    if(enLayer == HAL_DISP_LAYER_VSD0)
    {
        CBMATTR.bits.sur_attr0 = 0xf;
    }
    else if(enLayer == HAL_DISP_LAYER_HC0)
    {
        CBMATTR.bits.sur_attr1 = 0xf;
    }
    #if 0
    else if(enLayer == HAL_DISP_LAYER_HC1)
    {
        CBMATTR.bits.sur_attr2 = 0xf;
    }
    else if(enLayer == HAL_DISP_LAYER_GFX4)
    {
        CBMATTR.bits.sur_attr3 = 0xf;
    }
    #endif
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    HAL_WriteReg((HI_U32*)&(pVoReg->CBMATTR.u32), CBMATTR.u32); 
    return HI_TRUE;
}


HI_BOOL HAL_CBM_SetCbmBkg(HI_U32 bMixerId, HAL_DISP_BKCOLOR_S *pstBkg)
{
    U_CBMBKG1 CBMBKG1;
    //U_CBMBKG2 CBMBKG2;
    U_CBMBKG3 CBMBKG3;
    U_CBMBKG4 CBMBKG4;


    if(bMixerId == HAL_CBMMIX1)
    {
        CBMBKG1.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMBKG1.u32));
        CBMBKG1.bits.cbm_bkgy  = pstBkg->u8Bkg_y;
        CBMBKG1.bits.cbm_bkgcb = pstBkg->u8Bkg_cb;
        CBMBKG1.bits.cbm_bkgcr = pstBkg->u8Bkg_cr; 
        HAL_WriteReg((HI_U32*)&(pVoReg->CBMBKG1.u32), CBMBKG1.u32); 
    }
    #if 0
    else if(bMixerId == HAL_CBMMIX2)
    {
        CBMBKG2.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMBKG2.u32));
        CBMBKG2.bits.cbm_bkgy  = pstBkg->u8Bkg_y;
        CBMBKG2.bits.cbm_bkgcb = pstBkg->u8Bkg_cb;
        CBMBKG2.bits.cbm_bkgcr = pstBkg->u8Bkg_cr;        
        HAL_WriteReg((HI_U32*)&(pVoReg->CBMBKG2.u32), CBMBKG2.u32); 
    }
    #endif
    else if(bMixerId == HAL_CBMMIX3)
    {
        CBMBKG3.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMBKG3.u32));
        CBMBKG3.bits.cbm_bkgy  = pstBkg->u8Bkg_y;
        CBMBKG3.bits.cbm_bkgcb = pstBkg->u8Bkg_cb;
        CBMBKG3.bits.cbm_bkgcr = pstBkg->u8Bkg_cr;        
        HAL_WriteReg((HI_U32*)&(pVoReg->CBMBKG3.u32), CBMBKG3.u32); 
    }
    else if(bMixerId == HAL_CBMMIX4)
    {
        CBMBKG4.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMBKG4.u32));
        CBMBKG4.bits.cbm_bkgy  = pstBkg->u8Bkg_y;
        CBMBKG4.bits.cbm_bkgcb = pstBkg->u8Bkg_cb;
        CBMBKG4.bits.cbm_bkgcr = pstBkg->u8Bkg_cr;        
        HAL_WriteReg((HI_U32*)&(pVoReg->CBMBKG4.u32), CBMBKG4.u32); 
    }
    else
    {
        HAL_PRINT("Error mix id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}


HI_BOOL HAL_CBM_SetCbmMixerPrio(HAL_DISP_LAYER_E enLayer, HI_U8 u8Prio, HI_U8 u8MixerId)
{
    U_CBMMIX1 CBMMIX1;
    //U_CBMMIX2 CBMMIX2;
    U_CBMMIX3 CBMMIX3;
    U_CBMMIX4 CBMMIX4;
    HI_U8  u8LayerId = 0;

    //---------------------------------------------------------------
    if(u8MixerId == HAL_CBMMIX1)
    {
        //check layer availability
        switch(enLayer)
        {
            case HAL_DISP_LAYER_VHD0:
            {
                u8LayerId = 0x1;
                break;
            }
            case HAL_DISP_LAYER_VSD0:
            {
                u8LayerId = 0x4;
                break;
            }
            case HAL_DISP_LAYER_GFX0:
            {
                u8LayerId = 0x9;
                break;
            }
            case HAL_DISP_LAYER_HC0:
            {
                u8LayerId = 0xe;
                break;
            }
            default:
            {
                u8LayerId = 0x0;
                HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        //set mixer prio
        CBMMIX1.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMMIX1.u32));
        switch(u8Prio)
        {
            case 6:
            {
                //clean all priority
                CBMMIX1.bits.mixer_prio0 = 0;
                CBMMIX1.bits.mixer_prio1 = 0;
                CBMMIX1.bits.mixer_prio2 = 0;
                CBMMIX1.bits.mixer_prio3 = 0;
                break;
            }
            case 0:
            {
                CBMMIX1.bits.mixer_prio0 = u8LayerId;
                break;
            }
            case 1:
            {
                CBMMIX1.bits.mixer_prio1 = u8LayerId;
                break;
            }
            case 2:
            {
                CBMMIX1.bits.mixer_prio2 = u8LayerId;
                break;
            }
            case 3:
            {
                CBMMIX1.bits.mixer_prio3 = u8LayerId;
                break;
            }
            default:
            {
                HAL_PRINT("Error priority id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        HAL_WriteReg((HI_U32*)&(pVoReg->CBMMIX1.u32), CBMMIX1.u32); 
    }
    //---------------------------------------------------------------
    else if(u8MixerId == HAL_CBMMIX3)
    {
        //check layer availability
        switch(enLayer)
        {
            case HAL_DISP_LAYER_VSD0:
            {
                u8LayerId = 0x4;
                break;
            }
            case HAL_DISP_LAYER_GFX2:
            {
                u8LayerId = 0xb;
                break;
            }
            case HAL_DISP_LAYER_HC0:
            {
                u8LayerId = 0xe;
                break;
            }
            default:
            {
                u8LayerId = 0x0;
                HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        //set mixer prio
        CBMMIX3.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMMIX3.u32));
        switch(u8Prio)
        {
            case 6:
            {
                //clean all priority
                CBMMIX3.bits.mixer_prio0 = 0;
                CBMMIX3.bits.mixer_prio1 = 0;
                CBMMIX3.bits.mixer_prio2 = 0;
                break;
            }
            case 0:
            {
                CBMMIX3.bits.mixer_prio0 = u8LayerId;
                break;
            }
            case 1:
            {
                CBMMIX3.bits.mixer_prio1 = u8LayerId;
                break;
            }
            case 2:
            {
                CBMMIX3.bits.mixer_prio2 = u8LayerId;
                break;
            }
            default:
            {
                HAL_PRINT("Error priority id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        HAL_WriteReg((HI_U32*)&(pVoReg->CBMMIX3.u32), CBMMIX3.u32); 
    }
    //---------------------------------------------------------------
    else if(u8MixerId == HAL_CBMMIX4)
    {
        //check layer availability
        switch(enLayer)
        {
            case HAL_DISP_LAYER_VSD1:
            {
                u8LayerId = 0x5;
                break;
            }
            case HAL_DISP_LAYER_GFX3:
            {
                u8LayerId = 0xc;
                break;
            }
            case HAL_DISP_LAYER_HC0:
            {
                u8LayerId = 0xe;
                break;
            }
            default:
            {
                u8LayerId = 0x0;
                HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        //set mixer prio
        CBMMIX4.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBMMIX4.u32));
        switch(u8Prio)
        {
            case 6:
            {
                //clean all priority
                CBMMIX4.bits.mixer_prio0 = 0;
                CBMMIX4.bits.mixer_prio1 = 0;
                CBMMIX4.bits.mixer_prio2 = 0;
                break;
            }
            case 0:
            {
                CBMMIX4.bits.mixer_prio0 = u8LayerId;
                break;
            }
            case 1:
            {
                CBMMIX4.bits.mixer_prio1 = u8LayerId;
                break;
            }
            case 2:
            {
                CBMMIX4.bits.mixer_prio2 = u8LayerId;
                break;
            }
            default:
            {
                HAL_PRINT("Error priority id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        HAL_WriteReg((HI_U32*)&(pVoReg->CBMMIX4.u32), CBMMIX4.u32); 
    }
    else
    {
        HAL_PRINT("Error, Vou_SetCbmMixerPrio() Set a wrong CBMMIX\n");
        return HI_FALSE;
    }
    
    return HI_TRUE;
}


HI_BOOL HAL_WBC_SetWbcEnable(HAL_DISP_LAYER_E enLayer, HI_U32 bEnable)
{
    volatile U_WBCCTRL WBCCTRL;
    //volatile U_VHDCTRL VHDCTRL;
    volatile HI_U32 addr_REG;

    if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCCTRL.u32));
        WBCCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCCTRL.bits.wbc0_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, WBCCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_WBC_SetWbcAddr(HAL_DISP_LAYER_E enLayer, HI_U32 u32Addr,HI_U16 u16Str)
{
    volatile U_WBCADDR WBCADDR;
    volatile U_WBCSTRIDE WBCSTRIDE;
    volatile HI_U32 addr_REG;

    if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCADDR.u32));
        WBCADDR.u32 = u32Addr;
        HAL_WriteReg((HI_U32*)addr_REG, WBCADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCSTRIDE.u32));
        WBCSTRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCSTRIDE.bits.wbc0stride = u16Str;
        HAL_WriteReg((HI_U32*)addr_REG, WBCSTRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_WBC_SetWbcCAddr(HAL_DISP_LAYER_E enLayer, HI_U32 u32CAddr,HI_U16 u16CStr)
{
    //volatile U_WBC2ADDR WBC2ADDR;
    volatile U_WBCCADDR WBCCADDR;
    volatile U_WBCCSTRIDE WBCCSTRIDE;
    volatile HI_U32 addr_REG;

    if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCCADDR.u32));
        WBCCADDR.u32 = u32CAddr;
        HAL_WriteReg((HI_U32*)addr_REG, WBCCADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCCSTRIDE.u32));
        WBCCSTRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCCSTRIDE.bits.wbc0cstride = u16CStr;
        HAL_WriteReg((HI_U32*)addr_REG, WBCCSTRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_WBC_SetWbcOReso(HI_U32 width,  HI_U32 height)
{
    volatile U_WBCORESO WBCORESO;
    WBCORESO.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->WBCORESO.u32));
    WBCORESO.bits.ow = width - 1;
    WBCORESO.bits.oh  = height - 1;
    HAL_WriteReg((HI_U32*)&(pVoReg->WBCORESO.u32), WBCORESO.u32);  

    return HI_TRUE;
}

HI_BOOL HAL_WBC_SetWbcSpd(HAL_DISP_LAYER_E enLayer, HI_U32 u16ReqSpd)
{
    /* hi3521无此项 */
    #if 0
    U_WBCCTRL WBCCTRL;
    //U_VHDWBC1STRD VHDWBC1STRD;
    HI_U32 addr_REG;

    if(u16ReqSpd >= 1024)
    {
        HAL_PRINT("Error,Vou_SetWbcSpd() Test Data out of legal range\n");
        return HI_FALSE;
    }
    
    if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        //set wbc0 write back channel data requst interval value
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCCTRL.u32));
        WBCCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCCTRL.bits.req_interval = u16ReqSpd;
        HAL_WriteReg((HI_U32*)addr_REG, WBCCTRL.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    #endif
    
    return HI_TRUE;
}

HI_BOOL HAL_WBC_SetWbcOutFmt(HAL_DISP_LAYER_E enLayer, HAL_DISP_INTFDATAFMT_E enIntfFmt)
{
    volatile U_WBCCTRL WBCCTRL;
    //volatile U_VHDWBC1STRD VHDWBC1STRD;
    volatile U_WBCVSP  WBCVSP;
    volatile HI_U32 addr_REG;

    if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCCTRL.u32));
        WBCCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBCCTRL.bits.wbc0_dft = enIntfFmt;
        HAL_WriteReg((HI_U32*)addr_REG, WBCCTRL.u32); 

        // for hi3531 WBC2 only!
        // set the zme output format(420) when set WBC output format(420)
        if(enIntfFmt == HAL_DISP_INTFDATAFMT_YCBCR422)
        {
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->WBCVSP.u32));
            WBCVSP.bits.zme_out_fmt = 0;//yuv 422 output for zme
            HAL_WriteReg((HI_U32*)&(pVoReg->WBCVSP.u32), WBCVSP.u32);
        }
        else if(enIntfFmt == HAL_DISP_INTFDATAFMT_YCBCR420)
        {
            WBCVSP.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->WBCVSP.u32));
            WBCVSP.bits.zme_out_fmt = 1;//yuv 420 output for zme
            HAL_WriteReg((HI_U32*)&(pVoReg->WBCVSP.u32), WBCVSP.u32);
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
    
}

HI_BOOL HAL_WBC_GetWbcOutFmt(HAL_DISP_LAYER_E enLayer, HAL_DISP_INTFDATAFMT_E *penIntfFmt)
{
    volatile U_WBCCTRL WBCCTRL;
    volatile HI_U32 addr_REG;

    if((LAYER_WBC_START <= enLayer) && (enLayer <= LAYER_WBC_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBCCTRL.u32));
        WBCCTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        *penIntfFmt = WBCCTRL.bits.wbc0_dft;
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
    
}


#ifdef __cplusplus
#if __cplusplus
 }
#endif
#endif /* End of #ifdef __cplusplus */

