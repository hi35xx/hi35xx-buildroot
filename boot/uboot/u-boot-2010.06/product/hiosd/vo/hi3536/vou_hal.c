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

#define HI_INVALID_LAYER (-1)

/***************************************/
#define CRG_REGS_ADDR   0x12040000
#define CRG_REGS_SIZE   0X10000

#define CRG_PERCTL2_ADDR (0x0008 + CRG_REGS_ADDR)
#define CRG_PERCTL3_ADDR (0x000C + CRG_REGS_ADDR)
#define CRG_PERCTL4_ADDR (0x0010 + CRG_REGS_ADDR)
#define CRG_PERCTL5_ADDR (0x0014 + CRG_REGS_ADDR)
#define CRG_PERCTL6_ADDR (0x0018 + CRG_REGS_ADDR)
#define CRG_PERCTL7_ADDR (0x001c + CRG_REGS_ADDR)


#define CRG_PERCTL13_ADDR (0x0034 + CRG_REGS_ADDR)
#define CRG_PERCTL15_ADDR (0x003C + CRG_REGS_ADDR)

#define HDMI_REGS_ADDR  0x201f0000L
#define HDMI_REGS_SIZE  0X10000

#if 0
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
#endif

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

HI_S32 SYS_HAL_DDRConifg(HI_VOID)
{
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120130), 0x53356633);//  ## JPGE-VPSS1-VDH1-TDE-MDU-JPGD-VPSS0-VDH0
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120134), 0x05251554);  //## VICAP-PGD-AVC-IVE-AIO-VGS0-GZIP
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120138), 0x56555006);  //## USB2-SDIO0-NFC-DMA1-DMA0-VDP1-VDP0-GPU
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x1212013c), 0x37444666);  //## A7-A17-USB3-PCIE-BPD-SHA-SDIO1-CIPHER
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120140), 0x33411116);  //## VPSS2-VDH2-SFC-XOR-SATA-  -TOE-VDMA
    
//######### media0 timeout ctrl #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120034), 0x8020801c);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120038), 0x00000000);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x1212003c), 0x00008370);
//######### media0 pri #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120040), 0x00050066);
//######### media1 timeout ctrl #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120044), 0x8020801c);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120048), 0x00000000);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x1212004c), 0x00008170);
//######### media1 pri #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120050), 0x00050066);
//######### media2 timeout ctrl #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120054), 0x00000000);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120058), 0x00008032);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x1212005c), 0x8020801c);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120060), 0x00000000);
//######### media2 pri #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120064), 0x00560600);
//######### media3 timeout ctrl #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120144), 0x00000000);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120148), 0x00000000);
//######### media3 pri #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x1212014c), 0x00000000);
//######### hp_peri timeout ctrl #######
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x12120068), 0x81708032);
    HAL_WriteReg((HI_U32*)IO_ADDRESS( 0x1212006c), 0x00000000);
    
//###############################
//## mddrc0 pri&timeout setting #
//###############################
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110200),  0x00370000);  //# ports0 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110210),  0x00370000);  //# ports1
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110220),  0x00370000);  //# ports2 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110230),  0x00370000);  //# ports3 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110240),  0x00370000);  //# ports4 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110250),  0x00370000);  //# ports5 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110260),  0x00370000);  //# ports6 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110270),  0x00370000);  //# ports7 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110280),  0x00370000);  //# ports8 
//##DDRC AXI pri ports0 - 6
//############## WR pri ##############
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110204),  0x76543210);  //# ports0         
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110214),  0x76543210);  //# ports1         
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110224),  0x76543210);  //# ports2
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110234),  0x76543210);  //# ports3
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110244),  0x76543210);  //# ports4
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110254),  0x76543210);  //# ports5
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110264),  0x76543210);  //# ports6   
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110274),  0x76543210);  //# ports7
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110284),  0x76543210);  //# ports8
//############## RD pri ##############
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110208),  0x76543210);  //# ports0         
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110218),  0x76543210);  //# ports1         
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110228),  0x76543210);  //# ports2
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110238),  0x76543210);  //# ports3
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110248),  0x76543210);  //# ports4
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110258),  0x76543210);  //# ports5
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110268),  0x76543210);  //# ports6
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110278),  0x76543210);  //# ports7
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12110288),  0x76543210);  //# ports8
//##############  qosbuf #############
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12114000),  0x00000002);   //#qosb_push_ctrl
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x1211410c),  0x0000000a);   //#qosb_dmc_lvl
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12114110),  0x0000000a);   //#qosb_dmc_lvl
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x1211408c),  0xb3032010);   //#qosb_wbuf_ctrl
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x12114090),  0xb3032010);   //#qosb_wbuf_ctrl
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140f4),  0x00000033);   //#row-hit enable
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140ec),  0x00000055);   //#row-hit 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140f0),  0x00003333);   //#row-hit
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121141f4),  0x00000000);   //#qosb_wbuf_pri_ctrl
    
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121141f0),  0x00000001);   //#enable qosbuf timeout,through prilvl to remap timeout level
//############## WR timeout ###########
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x1211409c),  0x0000000a);  //# wr_tout3 ~wr_tout0         
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140a0),  0x00000000);  //# wr_tout7 ~wr_tout4         
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140a4),  0x00000000);  //# wr_tout11~wr_tout8
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140a8),  0x00000000);  //# wr_tout15~wr_tout12
    
//############## RD timeout ###########
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140ac),  0x0000000a);  //# rd_tout3 ~rd_tout0          
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140b0),  0x00000000);  //# rd_tout7 ~rd_tout4          
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140b4),  0x00000000);  //# rd_tout11~rd_tout8 
    HAL_WriteReg((HI_U32*)IO_ADDRESS(  0x121140b8),  0x00000000);  //# rd_tout15~rd_tout12
    
    return 0;
}

HI_S32 SYS_HAL_SelVoVgaPinConifg(HI_VOID)
{
    HI_RegWrite32(0x0, 0xffffffff, IO_ADDRESS(0x120f01E4));
    HI_RegWrite32(0x0, 0xffffffff, IO_ADDRESS(0x120f01E8));
    return 0;
}

HI_S32 SYS_HAL_SelVoHdmiPinConifg(HI_VOID)
{

    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x120f0110));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x120f0114));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x120f0118));
    HI_RegWrite32(0x1, 0xffffffff, IO_ADDRESS(0x120f011C));
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

HI_S32 SYS_HAL_VouHdDacClkEn(HI_BOOL pbClkEn)
{
    volatile HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 6, IO_ADDRESS(CRG_PERCTL13_ADDR));


    return 0;
}
HI_S32 SYS_HAL_SelVoHdDacClk(HI_U32 u32ClkSel)
{
    HI_RegSetBit(u32ClkSel, 18, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouBt1120ClkSel(HI_U32 u32ClkSel)
{
    HI_RegSetBit(u32ClkSel, 19, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouBt1120ClkEn(HI_BOOL bClkEn)
{
    HI_U32 u32Tmp = (bClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 20, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_SelVoHDMIClk(HI_U32 u32ClkSel)
{
    HI_RegSetBit(u32ClkSel, 19, IO_ADDRESS(CRG_PERCTL15_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouSdDacClkEn(HI_BOOL pbClkEn)
{
    volatile HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 7, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_SelVoClk(HI_U32 u32ClkSel)
{
    HI_RegWrite32(u32ClkSel << 14, 0x3 << 14, IO_ADDRESS(CRG_PERCTL13_ADDR));
    return 0;
}

HI_S32 SYS_HAL_VouSdClkSel(HI_U32 u32ClkSel)
{
    HI_RegWrite32(u32ClkSel << 16, 0x3 << 16, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_SelVoSdClkDiv(HI_U32 u32ClkSel)
{
    HI_RegSetBit(u32ClkSel, 13, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouBusResetSel(HI_BOOL bReset)
{
    volatile HI_U32 u32Tmp = (bReset == HI_TRUE) ? 1 : 0;

    HI_RegSetBit(u32Tmp, 0, IO_ADDRESS(CRG_PERCTL13_ADDR));

    return 0;
}

HI_S32 SYS_HAL_VouDevClkEn(HI_S32 s32VoDev, HI_BOOL pbClkEn)
{
    volatile HI_U32 u32Tmp = (pbClkEn == HI_TRUE) ? 1 : 0;  
    volatile HI_U32 u32Bit;

    if (s32VoDev >= 0 && s32VoDev <= 2)
    {
        u32Bit = 9 + s32VoDev;  
        HI_RegSetBit(u32Tmp, u32Bit, IO_ADDRESS(CRG_PERCTL13_ADDR));
    }
    else
    {
        return -1;
    }

    return 0;
}

HI_S32 SYS_HAL_VouHdOutClkSel(HI_U32 u32ClkSel)
{
    HI_RegSetBit(u32ClkSel, 12, IO_ADDRESS(CRG_PERCTL13_ADDR));
    return 0;
}

HI_S32 SYS_HAL_SetVoPllFrac(HI_S32 s32Pll, HI_U32 u32BitsSet)
{
    if (0 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 0, 0xffffff << 0, IO_ADDRESS(CRG_PERCTL4_ADDR));        
        
        if (0 == u32BitsSet)
        {
            HI_RegSetBit(1, 24, IO_ADDRESS(CRG_PERCTL5_ADDR));
        }
        else
        {
            HI_RegSetBit(0, 24, IO_ADDRESS(CRG_PERCTL5_ADDR));
        }
    }
    else if(1 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 0, 0xffffff << 0, IO_ADDRESS(CRG_PERCTL6_ADDR));        
        
        if (0 == u32BitsSet)
        {
            HI_RegSetBit(1, 24, IO_ADDRESS(CRG_PERCTL7_ADDR));
        }
        else
        {
            HI_RegSetBit(0, 24, IO_ADDRESS(CRG_PERCTL7_ADDR));
        }
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
        HI_RegWrite32(u32BitsSet << 24, 0x7 << 24, IO_ADDRESS(CRG_PERCTL4_ADDR));
    }
    else if(1 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 24, 0x7 << 24, IO_ADDRESS(CRG_PERCTL6_ADDR));
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
        HI_RegWrite32(u32BitsSet << 28, 0x7 << 28, IO_ADDRESS(CRG_PERCTL4_ADDR));
    }
    else if(1 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 28, 0x7 << 28, IO_ADDRESS(CRG_PERCTL6_ADDR));
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
        HI_RegWrite32(u32BitsSet << 12, 0x3f << 12, IO_ADDRESS(CRG_PERCTL5_ADDR));
    }
    else if(1 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 12, 0x3f << 12, IO_ADDRESS(CRG_PERCTL7_ADDR));
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
        HI_RegWrite32(u32BitsSet << 0, 0xfff << 0, IO_ADDRESS(CRG_PERCTL5_ADDR));
    }
    else if(1 == s32Pll)
    {
        HI_RegWrite32(u32BitsSet << 0, 0xfff << 0, IO_ADDRESS(CRG_PERCTL7_ADDR));
    }
    else
    {
        return -1;
    }
    return 0;
}

HI_VOID HAL_SYS_Control(HI_VOID)
{
    volatile U_VOCTRL VOCTRL;

    /* outstand */
    VOCTRL.u32 = pVoReg->VOCTRL.u32;
    VOCTRL.u32 = 0x80000000;
    pVoReg->VOCTRL.u32 = VOCTRL.u32;
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
        case HAL_DISP_LAYER_VHD0:
        {
            RegAbsAddr = (pReg) + enLayer * VHD_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_VHD1:
        {
            RegAbsAddr = (pReg) + (enLayer + 2) * VHD_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_VP:
        {
            RegAbsAddr = (pReg) + (enLayer - 1) * VHD_REG_LEN;
            break;
        }        
        case HAL_DISP_LAYER_VSD0:
        {
            RegAbsAddr = (pReg) + (enLayer + 1) * VHD_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_GFX0:   /* G0 */
        case HAL_DISP_LAYER_GFX1:   /* G1 */
        case HAL_DISP_LAYER_HC0:    /* G2 */
        case HAL_DISP_LAYER_HC1:    /* G3 */
        case HAL_DISP_LAYER_GFX4:   /* G4 */
        {
            RegAbsAddr = ((pReg)) + (enLayer - HAL_DISP_LAYER_GFX0) * GFX_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_WBC_G0:
        case HAL_DISP_LAYER_WBC_G4:
        {
            RegAbsAddr = ((pReg)) + (enLayer - HAL_DISP_LAYER_WBC_G0) * WBC_REG_LEN;
            break;
        }
        case HAL_DISP_LAYER_WBC_D:
        {
            RegAbsAddr = (pReg) + (enLayer - HAL_DISP_LAYER_WBC_D) * WBC_REG_LEN;
            break;
        }
         default:
        {
            HAL_PRINT("Error channel id found in %s: L%d enLayer %d\n",__FUNCTION__, __LINE__, enLayer);
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
        case HAL_DISP_CHANNEL_DHD1:            
        case HAL_DISP_CHANNEL_DSD0:
        {
            RegAbsAddr =pReg + enChan * DHD_REG_LEN;
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
    VOAXICTRL.bits.m0_id_sel = 1;
    VOAXICTRL.bits.m0_wr_ostd = 8;
    VOAXICTRL.bits.m0_outstd_rid0  = 8;    
    VOAXICTRL.bits.m0_outstd_rid1  = 8;
    VOAXICTRL.bits.m1_id_sel = 1;
    VOAXICTRL.bits.m1_wr_ostd = 8;
    VOAXICTRL.bits.m1_outstd_rid0  = 8;    
    VOAXICTRL.bits.m1_outstd_rid1  = 8;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOAXICTRL.u32), VOAXICTRL.u32); 

    return ;
}

/* AXI Master select */
HI_VOID HAL_SYS_SetAxiMaster(HI_VOID)
{
    volatile U_VOAXISEL VOAXISEL;

    VOAXISEL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VOAXISEL.u32));
    VOAXISEL.bits.v0_axi_sel = 0;
    VOAXISEL.bits.v1_axi_sel = 0;
    VOAXISEL.bits.v3_axi_sel = 0;
    VOAXISEL.bits.v4_axi_sel = 0;
    VOAXISEL.bits.g0_axi_sel = 1;
    VOAXISEL.bits.g1_axi_sel = 1;
    VOAXISEL.bits.g2_axi_sel = 1;
    VOAXISEL.bits.g3_axi_sel = 1;
    VOAXISEL.bits.g4_axi_sel = 1;
    VOAXISEL.bits.wbc_dhd_axi_sel = 0;
    VOAXISEL.bits.wbc_g0_axi_sel = 1;
    VOAXISEL.bits.wbc_g4_axi_sel = 1;
    VOAXISEL.bits.para_axi_sel = 0;
    HAL_WriteReg((HI_U32*)&(pVoReg->VOAXISEL.u32), VOAXISEL.u32);
    
    return;
}

HI_VOID HAL_SYS_SetRdBusId(HI_U32 bMode)
{
    
    return ;
}

HI_BOOL HAL_DISP_SetIntfEnable(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL enIntf)
{
    volatile U_DHD0_CTRL DHD0_CTRL;     
    volatile HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
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
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
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

HI_BOOL  HAL_DISP_SetIntfMuxDefaultSel(HI_VOID)
{
    volatile U_VO_MUX VO_MUX;
    
    VO_MUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_MUX.u32));
    VO_MUX.bits.bt1120_sel = 2; 
    VO_MUX.bits.hdmi_sel = 2;
    VO_MUX.bits.vga_sel = 2;
    VO_MUX.bits.sddate_sel = 4;

    HAL_WriteReg((HI_U32*)&(pVoReg->VO_MUX.u32), VO_MUX.u32);
    
    return HI_TRUE;
}

HI_BOOL  HAL_DISP_SetIntfMuxSel(HAL_DISP_OUTPUTCHANNEL_E enChan,HAL_DISP_INTF_E enIntf)
{
    volatile U_VO_MUX VO_MUX;
    if(enChan > HAL_DISP_CHANNEL_DSD0)
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    VO_MUX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_MUX.u32));    
    switch(enIntf)
    {
        case HAL_DISP_INTF_BT1120:
        {
            VO_MUX.bits.bt1120_sel = enChan; 
            break;
        }        
        case HAL_DISP_INTF_HDMI:
        {
            VO_MUX.bits.hdmi_sel = enChan;
            break;
        }
        case HAL_DISP_INTF_VGA:
        {
            VO_MUX.bits.vga_sel = enChan;
            break;
        }
        case HAL_DISP_INTF_CVBS:
        {
            VO_MUX.bits.sddate_sel = 4;
            break;
        }

        default:
        {
            HAL_PRINT("Error! DP_DISP_SetIntfMuxSel Wrong Select\n");
            return HI_FALSE;
        }
    }
    VO_MUX.bits.digital_sel = 2; 
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_MUX.u32), VO_MUX.u32); 
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntfDacEnable(HAL_DISP_INTF_E enIntf, HI_BOOL bDacEnable)
{
    volatile U_VO_DAC_CTRL VO_DAC_CTRL;    
    volatile U_VO_DAC_C_CTRL VO_DAC_C_CTRL;    
    volatile U_VO_DAC_R_CTRL VO_DAC_R_CTRL;    
    volatile U_VO_DAC_G_CTRL VO_DAC_G_CTRL; 
    volatile U_VO_DAC_B_CTRL VO_DAC_B_CTRL;
    
    switch(enIntf)
    {
        case(HAL_DISP_INTF_VGA):
            VO_DAC_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_CTRL.u32));
            VO_DAC_CTRL.bits.envbg = bDacEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_CTRL.u32), VO_DAC_CTRL.u32);
            
            VO_DAC_R_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32));
            VO_DAC_R_CTRL.bits.dac_en = bDacEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32), VO_DAC_R_CTRL.u32);
            
            VO_DAC_G_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_G_CTRL.u32));
            VO_DAC_G_CTRL.bits.dac_en = bDacEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_G_CTRL.u32), VO_DAC_G_CTRL.u32);
            
            VO_DAC_B_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_B_CTRL.u32));
            VO_DAC_B_CTRL.bits.dac_en = bDacEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_B_CTRL.u32), VO_DAC_B_CTRL.u32);

            break;
        case(HAL_DISP_INTF_CVBS):
            VO_DAC_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_CTRL.u32));
            VO_DAC_CTRL.bits.envbg = bDacEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_CTRL.u32), VO_DAC_CTRL.u32);
            
            VO_DAC_C_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_C_CTRL.u32));
            VO_DAC_C_CTRL.bits.dac_en = bDacEnable;
            HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_C_CTRL.u32), VO_DAC_C_CTRL.u32);

            break;
        default:
            break;
    }
    return HI_TRUE;
}

HI_BOOL HAL_DISP_VgaDacEn(HI_BOOL bEn)
{
    volatile U_VO_MUX_DAC VO_MUX_DAC;
    if(bEn)
    {
        VO_MUX_DAC.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_MUX_DAC.u32));
        VO_MUX_DAC.bits.dac2_sel = 9;          
        VO_MUX_DAC.bits.dac1_sel = 8;              
        VO_MUX_DAC.bits.dac0_sel = 7;        
        HAL_WriteReg((HI_U32*)&(pVoReg->VO_MUX_DAC.u32), VO_MUX_DAC.u32);
    }    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_CvbsDacEn(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_BOOL bEn)
{
    volatile U_VO_MUX_DAC VO_MUX_DAC;

    VO_MUX_DAC.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_MUX_DAC.u32));
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {        
        VO_MUX_DAC.bits.dac3_sel = 0;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_MUX_DAC.u32), VO_MUX_DAC.u32);
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetVgaGc(HI_U32 u32Value)
{
    volatile U_VO_DAC_R_CTRL VO_DAC_R_CTRL;    
    volatile U_VO_DAC_G_CTRL VO_DAC_G_CTRL;
    volatile U_VO_DAC_B_CTRL VO_DAC_B_CTRL;

    VO_DAC_R_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32));
    VO_DAC_R_CTRL.bits.dacgc = u32Value;
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_R_CTRL.u32), VO_DAC_R_CTRL.u32);
    
    VO_DAC_G_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_G_CTRL.u32));
    VO_DAC_G_CTRL.bits.dacgc = u32Value;
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_G_CTRL.u32), VO_DAC_G_CTRL.u32);

    VO_DAC_B_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_B_CTRL.u32));
    VO_DAC_B_CTRL.bits.dacgc = u32Value;
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_B_CTRL.u32), VO_DAC_B_CTRL.u32);
    
    return HI_TRUE;
}


HI_BOOL HAL_DISP_SetCvbsGc(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Value)
{
    volatile U_VO_DAC_C_CTRL VO_DAC_C_CTRL;

    VO_DAC_C_CTRL.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VO_DAC_C_CTRL.u32));
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {        
        VO_DAC_C_CTRL.bits.dacgc = u32Value;
    }    
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    HAL_WriteReg((HI_U32*)&(pVoReg->VO_DAC_C_CTRL.u32), VO_DAC_C_CTRL.u32);
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntfCSCEn(HAL_DISP_INTF_E enIntf,HI_BOOL bCscEn)
{   
    U_INTF_CSC_IDC INTF_CSC_IDC;
    volatile  HI_U32 addr_REG;
    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_IDC.u32));
    INTF_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
    INTF_CSC_IDC.bits.csc_en = bCscEn;
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_IDC.u32);     
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetIntfCscCoef(HAL_DISP_INTF_E enIntf,CscCoef_S *pstCoef)
{    
    U_INTF_CSC_IDC INTF_CSC_IDC;
    U_INTF_CSC_ODC INTF_CSC_ODC;
    U_INTF_CSC_IODC INTF_CSC_IODC;
    U_INTF_CSC_P0  INTF_CSC_P0;
    U_INTF_CSC_P1  INTF_CSC_P1;
    U_INTF_CSC_P2  INTF_CSC_P2;
    U_INTF_CSC_P3  INTF_CSC_P3;
    U_INTF_CSC_P4  INTF_CSC_P4;    
    volatile  HI_U32 addr_REG;
    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_IDC.u32));    
    INTF_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
    INTF_CSC_IDC.bits.cscidc1 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc1);
//    DHD0_CSC_IDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc0);
    INTF_CSC_IDC.bits.cscidc0 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc2);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_IDC.u32); 

    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_ODC.u32));    
    INTF_CSC_ODC.bits.cscodc1 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc1);
    //DHD0_CSC_ODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc0);    
    INTF_CSC_ODC.bits.cscodc0 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc2);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_ODC.u32); 
    
    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_IODC.u32));
//    DHD0_CSC_IODC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc2);
//    DHD0_CSC_IODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc2);
    INTF_CSC_IODC.bits.cscidc2 = HAL_GetXDC_BUMA(pstCoef->csc_in_dc0);
    INTF_CSC_IODC.bits.cscodc2 = HAL_GetXDC_BUMA(pstCoef->csc_out_dc0);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_IODC.u32); 

    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_P0.u32));
    INTF_CSC_P0.bits.cscp00 = HAL_Conver_CscCoef(pstCoef->csc_coef00);
    INTF_CSC_P0.bits.cscp01 = HAL_Conver_CscCoef(pstCoef->csc_coef01);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_P0.u32); 

    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_P1.u32));
    INTF_CSC_P1.bits.cscp02 = HAL_Conver_CscCoef(pstCoef->csc_coef02);
    INTF_CSC_P1.bits.cscp10 = HAL_Conver_CscCoef(pstCoef->csc_coef10);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_P1.u32); 

    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_P2.u32));
    INTF_CSC_P2.bits.cscp11 = HAL_Conver_CscCoef(pstCoef->csc_coef11);
    INTF_CSC_P2.bits.cscp12 = HAL_Conver_CscCoef(pstCoef->csc_coef12);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_P2.u32); 

    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_P3.u32));
    INTF_CSC_P3.bits.cscp20 = HAL_Conver_CscCoef(pstCoef->csc_coef20);
    INTF_CSC_P3.bits.cscp21 = HAL_Conver_CscCoef(pstCoef->csc_coef21);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_P3.u32); 

    addr_REG = Vou_GetIntfAbsAddr(enIntf,(HI_U32)&(pVoReg->INTF_CSC_P4.u32));
    INTF_CSC_P4.bits.cscp22 = HAL_Conver_CscCoef(pstCoef->csc_coef22);
    HAL_WriteReg((HI_U32*)addr_REG, INTF_CSC_P4.u32); 

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
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
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

HI_BOOL HAL_DISP_SetVtThd(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 vtthd)
{    
    volatile U_DHD0_VTTHD  DHD0_VTTHD;
    volatile  HI_U32 addr_REG;
    if((CHANNEL_DHD_START <= enChan) && (enChan <= CHANNEL_DSD_END))
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

HI_BOOL HAL_DISP_ClearIntStatus(HI_U32 u32IntMsk)
{
    /* read interrupt status */
    HAL_WriteReg((HI_U32*)&(pVoReg->VOMSKINTSTA.u32), u32IntMsk);
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
    return;
}

HI_BOOL HAL_DISP_SetDateCoeff(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    U_DATE_COEFF0 DATE_COEFF0;

    if (HAL_DISP_CHANNEL_DSD0 == enChan)
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

HI_BOOL HAL_DISP_SetDateCoeff22(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    volatile U_DATE_COEFF22 DATE_COEFF22;

    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        DATE_COEFF22.u32 = pVoReg->DATE_COEFF22.u32;
        DATE_COEFF22.bits.video_phase_delta = u32Data;
        pVoReg->DATE_COEFF22.u32 = DATE_COEFF22.u32;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff24(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF24.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff50(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF50.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff51(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF51.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff52(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF52.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff53(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF53.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff54(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF54.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff55(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF55.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff57(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF57.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_DISP_SetDateCoeff61(HAL_DISP_OUTPUTCHANNEL_E enChan, HI_U32 u32Data)
{
    if (HAL_DISP_CHANNEL_DSD0 == enChan)
    {
        pVoReg->DATE_COEFF61.u32 = u32Data;
    }
    else
    {
        HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_VOID HAL_DISP_SetDateCoeffByIdx(HI_U32 u32Idx, HI_U32 u32Data)
{
    switch (u32Idx)
    {
        case 60:
        {
            pVoReg->DATE_COEFF60.u32 = u32Data;
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

inline HI_U16 VOU_HAL_Conver_DateCoef(HI_S16 s16Value)
{
    HI_U32 temp = 0;

    if(s16Value >= 0)
    {
        return s16Value;
    }
    else
    {
        temp = (-1)*s16Value;
        return ( ((~temp)+1)& 0x3FF);
    }
}

HI_VOID VOU_HAL_DISP_SetDateNotchCoeff(const HI_S16 s16Coef[])
{
    HI_U16 u16Coef[12];
    int i = 0;

    for (i = 0; i < 12; i++)
    {    
        u16Coef[i] = VOU_HAL_Conver_DateCoef(s16Coef[i]);
    }
    
    pVoReg->DATE_COEFF74.bits.coef_notch_1 = u16Coef[0];
    pVoReg->DATE_COEFF74.bits.coef_notch_2 = u16Coef[1];

    pVoReg->DATE_COEFF75.bits.coef_notch_3 = u16Coef[2];
    pVoReg->DATE_COEFF75.bits.coef_notch_4 = u16Coef[3];

    pVoReg->DATE_COEFF76.bits.coef_notch_5 = u16Coef[4];
    pVoReg->DATE_COEFF76.bits.coef_notch_6 = u16Coef[5];

    pVoReg->DATE_COEFF77.bits.coef_notch_7 = u16Coef[6];
    pVoReg->DATE_COEFF77.bits.coef_notch_8 = u16Coef[7];

    pVoReg->DATE_COEFF78.bits.coef_notch_9 = u16Coef[8];
    pVoReg->DATE_COEFF78.bits.coef_notch_10 = u16Coef[9];

    pVoReg->DATE_COEFF79.bits.coef_notch_11 = u16Coef[10];
    pVoReg->DATE_COEFF79.bits.coef_notch_12 = u16Coef[11];
}

HI_VOID HAL_DISP_SetRegUp (HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    volatile U_DHD0_CTRL DHD0_CTRL;    
    volatile  HI_U32 addr_REG; 
    if(enChan > HAL_DISP_CHANNEL_DSD0)
    {
        HAL_PRINT("Error,VDP_DISP_SetIntfEnable Select Wrong CHANNEL ID\n");
        return ;
    }
    addr_REG = Vou_GetChnAbsAddr(enChan,(HI_U32)&(pVoReg->DHD0_CTRL.u32));
    DHD0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
    DHD0_CTRL.bits.regup = 0x1;
    HAL_WriteReg((HI_U32*)addr_REG, DHD0_CTRL.u32); 
    return;
}

HI_BOOL HAL_VIDEO_SetLayerUpMode(HAL_DISP_LAYER_E enLayer, HI_U32 bUpMode)
{
    U_V0_CTRL V0_CTRL;
    volatile  HI_U32 addr_REG;   
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
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

HI_BOOL HAL_VIDEO_SetIfirMode(HAL_DISP_LAYER_E enLayer, HAL_IFIRMODE_E enMode)
{
    volatile U_V0_CTRL V0_CTRL;
    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CTRL.u32));
        V0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CTRL.bits.ifir_mode = enMode;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CTRL.u32); 
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
    volatile U_V0_IFIRCOEF01 V0_IFIRCOEF01;
    volatile U_V0_IFIRCOEF23 V0_IFIRCOEF23;
    volatile U_V0_IFIRCOEF45 V0_IFIRCOEF45;
    volatile U_V0_IFIRCOEF67 V0_IFIRCOEF67;   
    
    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_IFIRCOEF01.u32));
        V0_IFIRCOEF01.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_IFIRCOEF01.bits.coef0 = s32Coef[0];
        V0_IFIRCOEF01.bits.coef1 = s32Coef[1];
        HAL_WriteReg((HI_U32*)addr_REG, V0_IFIRCOEF01.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_IFIRCOEF23.u32));
        V0_IFIRCOEF23.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_IFIRCOEF23.bits.coef2 = s32Coef[2];
        V0_IFIRCOEF23.bits.coef3 = s32Coef[3];
        HAL_WriteReg((HI_U32*)addr_REG, V0_IFIRCOEF23.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_IFIRCOEF45.u32));
        V0_IFIRCOEF45.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_IFIRCOEF45.bits.coef4 = s32Coef[4];
        V0_IFIRCOEF45.bits.coef5 = s32Coef[5];
        HAL_WriteReg((HI_U32*)addr_REG, V0_IFIRCOEF45.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_IFIRCOEF67.u32));
        V0_IFIRCOEF67.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_IFIRCOEF67.bits.coef6 = s32Coef[6];
        V0_IFIRCOEF67.bits.coef7 = s32Coef[7];
        HAL_WriteReg((HI_U32*)addr_REG, V0_IFIRCOEF67.u32);
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }


    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetLayerDispRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect)
{
    volatile U_V0_DFPOS V0_DFPOS;
    volatile U_V0_DLPOS V0_DLPOS;    
    
    volatile U_G0_DFPOS G0_DFPOS;
    volatile U_G0_DLPOS G0_DLPOS;    
    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
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
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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
    
    volatile U_G0_VFPOS G0_VFPOS;    
    volatile U_G0_VLPOS G0_VLPOS;
    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
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

static HI_U32 HAL_LAYER_GetLayerMaxAreaNum(HAL_DISP_LAYER_E enLayer)
{
    HI_U32 u32MaxAreaNum;
        
    switch (enLayer)
    {
        case HAL_DISP_LAYER_VHD0 :
            u32MaxAreaNum = 64;
            break;
        case HAL_DISP_LAYER_VHD1 :
            u32MaxAreaNum = 32;
            break;
        case HAL_DISP_LAYER_VP :
            u32MaxAreaNum = 4;
            break;
        case HAL_DISP_LAYER_VSD0 :
            u32MaxAreaNum = 1;
            break;
        default:
            u32MaxAreaNum = 0;
    }

    return u32MaxAreaNum;
}

HI_BOOL HAL_VIDEO_SetMultiAreaLAddr  (HAL_DISP_LAYER_E enLayer,HI_U32 u32AreaNum,HI_U32 u32LAddr, HI_U16 stride)
{
    volatile U_V0_P0LADDR V0_P0LADDR;
    volatile U_V0_P0STRIDE V0_P0STRIDE;
    volatile  HI_U32 addr_REG;
    HI_U32 u32_pxaddr_addr = 0;
    HI_U32 u32_pxstride_addr = 0;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        u32_pxaddr_addr   = (HI_U32)&(pVoReg->V0_P0LADDR.u32) + u32AreaNum * MULTI_AREA_CFG_LEN;
        u32_pxstride_addr = (HI_U32)&(pVoReg->V0_P0STRIDE.u32) + u32AreaNum * MULTI_AREA_CFG_LEN;

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxaddr_addr);
        V0_P0LADDR.u32   = u32LAddr;
        HAL_WriteReg((HI_U32*)addr_REG, V0_P0LADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxstride_addr);
        V0_P0STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_P0STRIDE.bits.surface_stride = stride;
        HAL_WriteReg((HI_U32*)addr_REG, V0_P0STRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetMultiAreaCAddr  (HAL_DISP_LAYER_E enLayer,HI_U32 u32AreaNum,HI_U32 u32CAddr, HI_U16 stride)
{
    volatile U_V0_P0CADDR V0_P0CADDR;
    volatile U_V0_P0STRIDE V0_P0STRIDE;
    volatile  HI_U32 addr_REG;
    HI_U32 u32_pxaddr_addr = 0;
    HI_U32 u32_pxstride_addr = 0;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        u32_pxaddr_addr   = (HI_U32)&(pVoReg->V0_P0CADDR.u32) + u32AreaNum * MULTI_AREA_CFG_LEN;
        u32_pxstride_addr = (HI_U32)&(pVoReg->V0_P0STRIDE.u32) + u32AreaNum * MULTI_AREA_CFG_LEN;

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxaddr_addr);
        V0_P0CADDR.u32 = u32CAddr;
        HAL_WriteReg((HI_U32*)addr_REG, V0_P0CADDR.u32); 

        addr_REG = Vou_GetAbsAddr(enLayer,u32_pxstride_addr);
        V0_P0STRIDE.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_P0STRIDE.bits.surface_cstride = stride;
        HAL_WriteReg((HI_U32*)addr_REG, V0_P0STRIDE.u32); 
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetMultiAreaEnable(HAL_DISP_LAYER_E enLayer,HI_U32 u32AreaNum,HI_U32 bEnable)
{
    volatile  HI_U32 addr_REG;
    volatile U_V0_P0CTRL V0_P0CTRL;
    HI_U32 pstP0CTRL;
    HI_U32 u32MaxAreaNum;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        u32MaxAreaNum = HAL_LAYER_GetLayerMaxAreaNum(enLayer);
        if(u32AreaNum < u32MaxAreaNum)
        {
            pstP0CTRL = (HI_U32)&(pVoReg->V0_P0CTRL.u32) + u32AreaNum * MULTI_AREA_CFG_LEN;
            addr_REG = Vou_GetAbsAddr(enLayer, pstP0CTRL);

            V0_P0CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_P0CTRL.bits.p0_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_P0CTRL.u32); 
        }       
        else
        {
            HAL_PRINT("Error area num found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
    }    
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    
    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetMultiAreaReso(HAL_DISP_LAYER_E enLayer,HI_U32 u32AreaNum,
                                              HI_U32 u32Width)
{
    U_V0_P0RESO  V0_P0RESO;
    HI_U32 pstReso;
    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        pstReso  = (HI_U32)&(pVoReg->V0_P0RESO.u32)  + u32AreaNum * MULTI_AREA_CFG_LEN;
        //cfg resolution
        addr_REG = Vou_GetAbsAddr(enLayer,pstReso);
        V0_P0RESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_P0RESO.bits.w = u32Width -1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_P0RESO.u32); 
    }   
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_VIDEO_SetMultiAreaRect(HAL_DISP_LAYER_E enLayer,HI_U32 u32AreaNum,RECT_S *pstVideoAreaRect)
{
    U_V0_P0VFPOS V0_P0VFPOS;
    U_V0_P0VLPOS V0_P0VLPOS;

    HI_U32 pstVfpos;
    HI_U32 pstVlpos;

    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        pstVfpos = (HI_U32)&(pVoReg->V0_P0VFPOS.u32) + u32AreaNum * MULTI_AREA_CFG_LEN;
        pstVlpos = (HI_U32)&(pVoReg->V0_P0VLPOS.u32) + u32AreaNum * MULTI_AREA_CFG_LEN;

        //cfg fpos
        addr_REG = Vou_GetAbsAddr(enLayer,pstVfpos);
        V0_P0VFPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_P0VFPOS.bits.video_xfpos = pstVideoAreaRect->s32X;
        V0_P0VFPOS.bits.video_yfpos = pstVideoAreaRect->s32Y;
        HAL_WriteReg((HI_U32*)addr_REG, V0_P0VFPOS.u32); 

        //cfg lpos
        addr_REG = Vou_GetAbsAddr(enLayer,pstVlpos);
        V0_P0VLPOS.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_P0VLPOS.bits.video_xlpos = pstVideoAreaRect->s32X + pstVideoAreaRect->u32Width -1;
        V0_P0VLPOS.bits.video_ylpos = pstVideoAreaRect->s32Y + pstVideoAreaRect->u32Height -1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_P0VLPOS.u32); 
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
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CTRL.u32));
        V0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CTRL.bits.surface_en = bEnable;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CTRL.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CTRL.u32));
        V0_CTRL.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CTRL.bits.ifmt = enDataFmt;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CTRL.u32); 
    }   
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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

    if((LAYER_VHD_START <= enLayer) && (enLayer <= LAYER_VSD_END))
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
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
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

HI_BOOL HAL_LAYER_SetCscEn(HAL_DISP_LAYER_E enLayer, HI_BOOL bCscEn)
{
    U_V0_CSC_IDC  V0_CSC_IDC;
    U_G0_CSC_IDC G0_CSC_IDC;
    volatile  HI_U32 addr_REG;

    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CSC_IDC.u32));
        V0_CSC_IDC.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CSC_IDC.bits.csc_en = bCscEn;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CSC_IDC.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_IRESO.u32));
        V0_IRESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_IRESO.bits.iw = pstRect->u32Width- 1;
        V0_IRESO.bits.ih = pstRect->u32Height- 1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_IRESO.u32); 
    }   
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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

HI_BOOL HAL_LAYER_SetLayerOutRect(HAL_DISP_LAYER_E enLayer, RECT_S *pstRect)
{
    U_V0_ORESO V0_ORESO;
    volatile  HI_U32 addr_REG;
    
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_ORESO.u32));
        V0_ORESO.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_ORESO.bits.ow = pstRect->u32Width- 1;
        V0_ORESO.bits.oh = pstRect->u32Height- 1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_ORESO.u32); 
    }   
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
    {
        
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }
    return HI_FALSE;
}

HI_BOOL HAL_LAYER_SetLayerGalpha(HAL_DISP_LAYER_E enLayer,
                                     HI_U8 u8Alpha0)
{
    volatile U_V0_CBMPARA V0_CBMPARA;
    volatile U_G0_CBMPARA G0_CBMPARA;

    volatile  HI_U32 addr_REG;
    if((LAYER_VHD_START <= enLayer)&&(enLayer <= LAYER_VSD_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_CBMPARA.u32));
        V0_CBMPARA.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_CBMPARA.bits.galpha = u8Alpha0;
        HAL_WriteReg((HI_U32*)addr_REG, V0_CBMPARA.u32); 
    }
    else if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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
/* Vou zoom enable */
HI_BOOL HAL_LAYER_SetZmeEnable(HAL_DISP_LAYER_E enLayer,
                                    HAL_DISP_ZMEMODE_E enMode,
                                    HI_U32 bEnable)
{
    volatile U_V0_HSP V0_HSP;
    volatile U_V0_VSP V0_VSP;
    volatile U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;
    volatile U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;

    volatile  HI_U32 addr_REG;

    if(enLayer <= HAL_DISP_LAYER_VSD0)
    {
        /* layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_HSP.u32));
            V0_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_HSP.bits.hlmsc_en = bEnable;            
            V0_HSP.bits.hlfir_en = 0;
            HAL_WriteReg((HI_U32*)addr_REG, V0_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_HSP.u32));
            V0_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_HSP.bits.hchmsc_en = bEnable;
            V0_HSP.bits.hchfir_en = 0;
            HAL_WriteReg((HI_U32*)addr_REG, V0_HSP.u32);
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSP.u32));
            V0_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_VSP.bits.vlmsc_en = bEnable;
            V0_VSP.bits.vlfir_en = 0;
            HAL_WriteReg((HI_U32*)addr_REG, V0_VSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSP.u32));
            V0_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_VSP.bits.vchmsc_en = bEnable;
            V0_VSP.bits.vchfir_en = 0;
            HAL_WriteReg((HI_U32*)addr_REG, V0_VSP.u32); 
        }
    }
    else if(enLayer == HAL_DISP_LAYER_WBC_D)
    {
        /* VSD layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_HSP.u32));
            WBC_DHD0_ZME_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_HSP.bits.hlmsc_en = bEnable;
            WBC_DHD0_ZME_HSP.bits.hlfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_HSP.u32));
            WBC_DHD0_ZME_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_HSP.bits.hchmsc_en = bEnable;
            WBC_DHD0_ZME_HSP.bits.hchfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSP.u32));
            WBC_DHD0_ZME_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_VSP.bits.vlmsc_en = bEnable;
            WBC_DHD0_ZME_VSP.bits.vlfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSP.u32));
            WBC_DHD0_ZME_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_VSP.bits.vchmsc_en = bEnable;
            WBC_DHD0_ZME_VSP.bits.vchfir_en = 1;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSP.u32); 
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
    U_V0_HSP V0_HSP;    
    U_V0_VSP V0_VSP;
    U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;
    U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;

    volatile  HI_U32 addr_REG;
    if(enLayer <= HAL_DISP_LAYER_VSD0)
    {
        /* layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_HSP.u32));
            V0_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_HSP.bits.hlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_HSP.u32));
            V0_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_HSP.bits.hchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSP.u32));
            V0_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_VSP.bits.vlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_VSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSP.u32));
            V0_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_VSP.bits.vchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_VSP.u32); 
        }
    }    
    else if(enLayer == HAL_DISP_LAYER_WBC_D)
    {
        /* VSD layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_HSP.u32));
            WBC_DHD0_ZME_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_HSP.bits.hlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_HSP.u32));
            WBC_DHD0_ZME_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_HSP.bits.hchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSP.u32));
            WBC_DHD0_ZME_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_VSP.bits.vlfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSP.u32));
            WBC_DHD0_ZME_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_VSP.bits.vchfir_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSP.u32); 
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
    U_V0_HSP V0_HSP;    
    U_V0_VSP V0_VSP;
    U_WBC_DHD0_ZME_HSP WBC_DHD0_ZME_HSP;
    U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;

    volatile  HI_U32 addr_REG;
    if(enLayer <= HAL_DISP_LAYER_VSD0)
    {
        /* layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_HSP.u32));
            V0_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_HSP.bits.hlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_HSP.u32));
            V0_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_HSP.bits.hchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSP.u32));
            V0_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_VSP.bits.vlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_VSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSP.u32));
            V0_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            V0_VSP.bits.vchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, V0_VSP.u32); 
        }
    }  
    else if(enLayer == HAL_DISP_LAYER_WBC_D)
    {
        /* VSD layer zoom enable */
        if((enMode == HAL_DISP_ZMEMODE_HORL)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_HSP.u32));
            WBC_DHD0_ZME_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_HSP.bits.hlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_HORC)||(enMode == HAL_DISP_ZMEMODE_HOR)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_HSP.u32));
            WBC_DHD0_ZME_HSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_HSP.bits.hchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_HSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERL)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSP.u32));
            WBC_DHD0_ZME_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_VSP.bits.vlmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSP.u32); 
        }

        if((enMode == HAL_DISP_ZMEMODE_VERC)||(enMode == HAL_DISP_ZMEMODE_VER)||(enMode == HAL_DISP_ZMEMODE_ALL))
        {
            addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSP.u32));
            WBC_DHD0_ZME_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
            WBC_DHD0_ZME_VSP.bits.vchmsc_en = bEnable;
            HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSP.u32); 
        }
    }
    else
    {
        HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
        return HI_FALSE;
    }

    return HI_TRUE;
}

HI_BOOL HAL_LAYER_SetZmeVerType(HAL_DISP_LAYER_E enLayer, HI_U32 uVerType)
{
    U_V0_VSP V0_VSP;    
    U_WBC_DHD0_ZME_VSP WBC_DHD0_ZME_VSP;

    volatile  HI_U32 addr_REG;
    if(enLayer <= HAL_DISP_LAYER_VSD0)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSP.u32));
        V0_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_VSP.bits.zme_in_fmt = uVerType;
        HAL_WriteReg((HI_U32*)addr_REG, V0_VSP.u32); 
    }
    else if(enLayer == HAL_DISP_LAYER_WBC_D)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSP.u32));
        WBC_DHD0_ZME_VSP.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBC_DHD0_ZME_VSP.bits.zme_in_fmt = uVerType;
        HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSP.u32); 
    }

    return HI_TRUE;
}
HI_BOOL HAL_LAYER_SetVerRatio(HAL_DISP_LAYER_E enLayer, HI_U32 uRatio)
{
    volatile U_V0_VSR V0_VSR;
    volatile U_WBC_DHD0_ZME_VSR WBC_DHD0_ZME_VSR;

    volatile HI_U32 addr_REG;
    if(enLayer <= HAL_DISP_LAYER_VSD0)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_VSR.u32));
        V0_VSR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        V0_VSR.bits.vratio = uRatio;
        HAL_WriteReg((HI_U32*)addr_REG, V0_VSR.u32); 
    }
    else if(enLayer == HAL_DISP_LAYER_WBC_D)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_ZME_VSR.u32));
        WBC_DHD0_ZME_VSR.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        WBC_DHD0_ZME_VSR.bits.vratio = uRatio;
        HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_ZME_VSR.u32); 
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
    U_V0_UPD V0_UPD;
    U_G0_UPD G0_UPD;
    U_WBC_DHD0_UPD WBC_DHD0_UPD;    

    volatile HI_U32 addr_REG;
    if(enLayer <= HAL_DISP_LAYER_VSD0)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->V0_UPD.u32));
        V0_UPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        /* layer register update */
        V0_UPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, V0_UPD.u32); 
    }
    else if((LAYER_GFX_START <= enLayer) && (enLayer <= LAYER_GFX_END))
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->G0_UPD.u32));
        G0_UPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        /* layer register update */
        G0_UPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, G0_UPD.u32); 
    }
    else if(enLayer == HAL_DISP_LAYER_WBC_D)
    {
        addr_REG = Vou_GetAbsAddr(enLayer,(HI_U32)&(pVoReg->WBC_DHD0_UPD.u32));
        WBC_DHD0_UPD.u32 = HAL_ReadReg((HI_U32*)addr_REG);
        /* G1 layer register update */
        WBC_DHD0_UPD.bits.regup = 0x1;
        HAL_WriteReg((HI_U32*)addr_REG, WBC_DHD0_UPD.u32); 
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

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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

    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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
    if((LAYER_GFX_START <= enLayer)&&(enLayer <= LAYER_GFX_END))
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

HI_BOOL HAL_CBM_SetCbmAttr(HAL_DISP_LAYER_E enLayer, HAL_DISP_OUTPUTCHANNEL_E enChan)
{
    U_VHD_CORRESP VHD_CORRESP;
    U_GDC_CORRESP GDC_CORRESP;

    if(enLayer == HAL_DISP_LAYER_VP)
    {        
        VHD_CORRESP.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->VHD_CORRESP.u32));
        if(enChan == HAL_DISP_CHANNEL_DHD0)
        {
            VHD_CORRESP.bits.v1_corresp = 0x1;
        }       
        else if(enChan == HAL_DISP_CHANNEL_DHD1)
        {
            VHD_CORRESP.bits.v1_corresp = 0x2;
        }       
        else
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
        
        HAL_WriteReg((HI_U32*)&(pVoReg->VHD_CORRESP.u32), VHD_CORRESP.u32); 
    }
    else if(enLayer == HAL_DISP_LAYER_HC0)
    {        
        GDC_CORRESP.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->GDC_CORRESP.u32));
        if(enChan == HAL_DISP_CHANNEL_DHD0)
        {
            GDC_CORRESP.bits.g2_corresp = 0x2;
        }
        else if(enChan == HAL_DISP_CHANNEL_DHD1)
        {
            GDC_CORRESP.bits.g2_corresp = 0x2;
        } 
        else if(enChan == HAL_DISP_CHANNEL_DSD0)
        {
            GDC_CORRESP.bits.g2_corresp = 0x4;
        }  
        else
        {
            HAL_PRINT("Error channel id found in %s: L%d\n",__FUNCTION__, __LINE__);
            return HI_FALSE;
        }
        
        HAL_WriteReg((HI_U32*)&(pVoReg->GDC_CORRESP.u32), GDC_CORRESP.u32); 
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
    U_CBM_BKG1 CBM_BKG1;
    U_CBM_BKG2 CBM_BKG2;
    U_MIXDSD_BKG MIXDSD_BKG;

    if(bMixerId == HAL_CBMMIX1)
    {
        CBM_BKG1.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBM_BKG1.u32));
        CBM_BKG1.bits.cbm_bkgy1  = HAL_GetBkg_BUMA(pstBkg->u8Bkg_y);
        CBM_BKG1.bits.cbm_bkgcb1 = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cb);
        CBM_BKG1.bits.cbm_bkgcr1 = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cr); 
        HAL_WriteReg((HI_U32*)&(pVoReg->CBM_BKG1.u32), CBM_BKG1.u32); 
    }   
    else if(bMixerId == HAL_CBMMIX2)
    {
        CBM_BKG2.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBM_BKG2.u32));
        CBM_BKG2.bits.cbm_bkgy2  = HAL_GetBkg_BUMA(pstBkg->u8Bkg_y);
        CBM_BKG2.bits.cbm_bkgcb2 = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cb);
        CBM_BKG2.bits.cbm_bkgcr2 = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cr);        
        HAL_WriteReg((HI_U32*)&(pVoReg->CBM_BKG2.u32), CBM_BKG2.u32); 
    }  
    else if(bMixerId == HAL_CBMMIX3)
    {
        MIXDSD_BKG.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->MIXDSD_BKG.u32));
        MIXDSD_BKG.bits.mixer_bkgy  = HAL_GetBkg_BUMA(pstBkg->u8Bkg_y);
        MIXDSD_BKG.bits.mixer_bkgcb = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cb);
        MIXDSD_BKG.bits.mixer_bkgcr = HAL_GetBkg_BUMA(pstBkg->u8Bkg_cr);        
        HAL_WriteReg((HI_U32*)&(pVoReg->MIXDSD_BKG.u32), MIXDSD_BKG.u32); 
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
    U_CBM_MIX1 CBM_MIX1;
    U_CBM_MIX2 CBM_MIX2;
    U_MIXDSD_MIX MIXDSD_MIX;
    HI_U8  u8LayerId = 0;

    //---------------------------------------------------------------
    if(u8MixerId == HAL_CBMMIX1)
    {
        switch(enLayer)
        {
            case HAL_DISP_LAYER_VHD0:
            {
                u8LayerId = 0x1;
                break;
            }
            case HAL_DISP_LAYER_VP:
            {
                u8LayerId = 0x9;
                break;
            }
            case HAL_DISP_LAYER_GFX0:
            {
                u8LayerId = 0x2;
                break;
            }
            case HAL_DISP_LAYER_HC0:
            {
                u8LayerId = 0xa;
                break;
            }
            default:
            {
                HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        //set mixer prio
        CBM_MIX1.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->CBM_MIX1.u32));
        switch(u8Prio)
        {
            case 6:
            {
                //clean all priority
                CBM_MIX1.bits.mixer_prio0 = 0;
                CBM_MIX1.bits.mixer_prio1 = 0;
                CBM_MIX1.bits.mixer_prio2 = 0;
                CBM_MIX1.bits.mixer_prio3 = 0;
                CBM_MIX1.bits.mixer_prio4 = 0;
                break;
            }
            case 0:
            {
                CBM_MIX1.bits.mixer_prio0 = u8LayerId;
                break;
            }
            case 1:
            {
                CBM_MIX1.bits.mixer_prio1 = u8LayerId;
                break;
            }
            case 2:
            {
                CBM_MIX1.bits.mixer_prio2 = u8LayerId;
                break;
            }
            case 3:
            {
                CBM_MIX1.bits.mixer_prio3 = u8LayerId;
                break;
            }
            case 4:
            {
                CBM_MIX1.bits.mixer_prio4 = u8LayerId;
                break;
            }
            default:
            {
                HAL_PRINT("Error priority id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        HAL_WriteReg((HI_U32*)&(pVoReg->CBM_MIX1.u32), CBM_MIX1.u32);
    }
    //---------------------------------------------------------------
    else if(u8MixerId == HAL_CBMMIX2)
    {
        //check layer availability
        switch(enLayer)
        {
            case HAL_DISP_LAYER_VHD1:
            {
                u8LayerId = 0x3;
                break;
            }            
            case HAL_DISP_LAYER_VP:
            {
                u8LayerId = 0x9;
                break;
            }
            case HAL_DISP_LAYER_GFX4:
            {
                u8LayerId = 0x4;
                break;
            }
            case HAL_DISP_LAYER_HC0:
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
                CBM_MIX2.bits.mixer_prio4 = 0;
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
            case 4:
            {
                CBM_MIX2.bits.mixer_prio4 = u8LayerId;
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
    else if(u8MixerId == HAL_CBMMIX3)
    {
        //check layer availability
        switch(enLayer)
        {
            case HAL_DISP_LAYER_VSD0:
            {
                u8LayerId = 0x1;
                break;
            }
            case HAL_DISP_LAYER_GFX1:
            {
                u8LayerId = 0x2;
                break;
            }
            case HAL_DISP_LAYER_HC0:
            {
                u8LayerId = 0xa;
                break;
            }
            default:
            {
                HAL_PRINT("Error layer id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        //set mixer prio
        MIXDSD_MIX.u32 = HAL_ReadReg((HI_U32*)&(pVoReg->MIXDSD_MIX.u32));
        switch(u8Prio)
        {
            case 6:
            {
                //clean all priority
                MIXDSD_MIX.bits.mixer_prio0 = 0;
                MIXDSD_MIX.bits.mixer_prio1 = 0;
                MIXDSD_MIX.bits.mixer_prio2 = 0;
                MIXDSD_MIX.bits.mixer_prio3 = 0;
                break;
            }
            case 0:
            {
                MIXDSD_MIX.bits.mixer_prio0 = u8LayerId;
                break;
            }
            case 1:
            {
                MIXDSD_MIX.bits.mixer_prio1 = u8LayerId;
                break;
            }
            case 2:
            {
                MIXDSD_MIX.bits.mixer_prio2 = u8LayerId;
                break;
            }
            case 3:
            {
                MIXDSD_MIX.bits.mixer_prio3 = u8LayerId;
                break;
            }
            default:
            {
                HAL_PRINT("Error priority id found in %s: L%d\n",__FUNCTION__, __LINE__);
                return HI_FALSE;
            }
        }
        HAL_WriteReg((HI_U32*)&(pVoReg->MIXDSD_MIX.u32), MIXDSD_MIX.u32); 
    }
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

