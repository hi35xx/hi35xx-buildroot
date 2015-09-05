/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include "linux/string.h"
#include "globdefs.h"
#include "defstx.h"
#include "hdmitx.h"
#include "defstx.h"
#include "delay.h"
#include "de.h"
#include "eeprom.h"
#include "txvidp.h"
#include "infofrm.h"
#include "vmode.h"
#include "siitxapidefs.h"
#include "mpi_hdmi.h"
#include <common.h>


//---------------------------------------------------------------------

//-------------------------------------------------------------------
static void hdmiPhyInit(void)
{
    HI_U32 u32Value = 0;
    /* hdmi PHY */
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x18, 0x89);
    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR + 0x18);
    HDMIPrint("TMDS_CTL2 set to:0x%x\n", u32Value);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x38, 0x00);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x1c, 0x81);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x24, 0x1a);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x28, 0x07);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x2c, 0x51);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x08, 0x24);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x14, 0x12);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x20, 0x40);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x34, 0x00);  
}

HI_S32 SI_TX_PHY_PowerDown(HI_BOOL bPwdown)
{
    HI_U32 u32Value = 0;

    //新phy
    /* hdmi PHY */
    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR + 0x14);
    if(bPwdown)
    {
        u32Value &= ~0x10;
    }
    else
    {
        u32Value |= 0x10;
    }
    HDMIPrint("writing phy 0x%x PowerDown\n",u32Value);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x14,u32Value);

   return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_SetDeepColor(HI_U8 bDeepColor)
{
    HI_U32 u32Value = 0;
 
    //新phy
    /* Config kudu IP for DeepColor*/
    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR + 0x08);

    HDMIPrint("PLL_CTRL  old walue:0x%x\n", u32Value);
    
    if (SiI_DeepColor_30bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x01;
        HDMIPrint("SiI_DeepColor_30bit\n");
    }
    else if (SiI_DeepColor_36bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x02;
        HDMIPrint("SiI_DeepColor_36bit\n");
    }
    else if (SiI_DeepColor_24bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x00;
        HDMIPrint("SiI_DeepColor_24bit(normal)\n");
    }
    else
    {
        u32Value =  (u32Value & ~0x03) | 0x00;
        HDMIPrint("SiI_DeepColor_Off\n");
    } 
    
    HDMIPrint("writing phy 0x%x SetDeepColor\n",u32Value);
    
    HDMI_REG_WRITE(0x20101808,u32Value);
    u32Value = 0;
    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR + 0x08);
    HDMIPrint("PLL_CTRL  new walue:0x%x\n", u32Value);

    return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_EnableHdmiOutput(void)
{
    HI_U32 u32Reg = 0;
    //新phy
    u32Reg = HDMI_REG_READ(HDMI_TX_PHY_ADDR + 0x14);
    HDMIPrint("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    u32Reg |= 0x20;
    HDMIPrint("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x14, u32Reg);

    return HI_SUCCESS;
}

void HW_ResetHDMITX(unsigned int dev)
{
    volatile unsigned int i;
#if 0    
    HDMIPrint("--> HW_ResetHDMITX.\n");
    HdmiHardwareReset(1);
    DelayMS(18);
    HdmiHardwareReset(0);
    DelayMS(50);
#else
    HDMI_REG_WRITE(HDMI_HARDWARE_RESET_ADDR, 0x3);/* enable hdmi clk */
    DelayMS(5);
    #if 0
    HDMI_REG_WRITE(HDMI_HARDWARE_RESET_ADDR, 0x3fc);/* enable hdmi clk */
    #else
    RegSetBit(0, 0, HDMI_HARDWARE_RESET_ADDR);
    RegSetBit(0, 1, HDMI_HARDWARE_RESET_ADDR);

    for (i = 2; i < 10; i++)
    {
        RegSetBit(1, i, HDMI_HARDWARE_RESET_ADDR);
    }

    /* 如果是DHD1需要选择VPLL1时钟源 */
    if (1 == dev)
    {
        RegSetBit(1, 11, HDMI_HARDWARE_RESET_ADDR);
    }
    #endif
    DelayMS(5);
#endif
    
    hdmiPhyInit();
    DelayMS(1);

    WriteByteHDMITXP0(TX_SYS_CTRL1_ADDR, 0x35);

}

//-------------------------------------------------------------------
void AssertHDMITX_SWReset(HI_U8 SoftReset)
{
    HI_U8 RegVal;
    RegVal = ReadByteHDMITXP0( TX_SWRST_ADDR );
    RegVal |= SoftReset;
    WriteByteHDMITXP0( TX_SWRST_ADDR, SoftReset);
}
//-------------------------------------------------------------------
void ReleaseHDMITX_SWReset( HI_U8 SoftReset )
{
    HI_U8 RegVal;

    RegVal = ReadByteHDMITXP0( TX_SWRST_ADDR );
    RegVal &= (~SoftReset);
    WriteByteHDMITXP0( TX_SWRST_ADDR, 0);
}
//-------------------------------------------------------------------
void SW_ResetHDMITX(void)
{
    //HI_U8 TimeOut = 255;
    HI_U8 TimeOut = 20;
    //HI_U8 RegVal;
#if 1
    HDMIPrint("--> SW_ResetHDMITX.\n");

    //power down hdmi
    #if 0
    RegVal = ReadByteHDMITXP0(TX_SYS_CTRL1_ADDR);
    RegVal &= ~ BIT_TX_PD;
    WriteByteHDMITXP0 (TX_SYS_CTRL1_ADDR, RegVal);         // 0x35->0x37 GVG
    #endif
    
    while ( !siiIsTClockStable() && --TimeOut )
    {
        DelayMS(1);         // wait for input pixel clock to stabilze
    }
        
    if (TimeOut)
    {
        HDMIPrint("--> clk unstable time out.\n");
    }
    
    AssertHDMITX_SWReset(BIT_TX_SW_RST | BIT_TX_FIFO_RST);
    DelayMS(1);
    ReleaseHDMITX_SWReset(BIT_TX_SW_RST | BIT_TX_FIFO_RST);
    DelayMS(1);          // allow TCLK (sent to Rx across the HDMS link) to stabilize

#else
    AssertHDMITX_SWReset(BIT_TX_SW_RST | BIT_TX_FIFO_RST);
    DelayMS(1);
    ReleaseHDMITX_SWReset(BIT_TX_SW_RST | BIT_TX_FIFO_RST);
    DelayMS(1);          // allow TCLK (sent to Rx across the HDMS link) to stabilize

    DelayMS(1);
#endif
}
//-------------------------------------------------------------------
void WakeUpHDMITX(void)
{
    HI_U8 RegVal;

    RegVal = ReadByteHDMITXP0(TX_SYS_CTRL1_ADDR) | BIT_TX_PD;
    WriteByteHDMITXP0 (TX_SYS_CTRL1_ADDR, RegVal);         // 0x35->0x37 GVG
    WriteByteHDMITXP0 (INT_CNTRL_ADDR, 0x02);       // Interrupt pin control defined per board

}
//---------------------------------------------------------------------------
void SetIClk(HI_U8 IClk)
{
    HI_U8 RegVal;
    RegVal = ReadByteHDMITXP0(TX_VID_CTRL_ADDR) & CLR_BITS_1_0;
    WriteByteHDMITXP0( TX_VID_CTRL_ADDR, (RegVal | IClk) );
}

//--------------------------------------------------------------------------
void TX_SetHDMIMode(HI_U8 Enabled)
{
    HI_U8 RegVal;

    HDMIPrint("Set HDMI mode to %d.\n", Enabled);

    RegVal = ReadByteHDMITXP1(AUDP_TXCTRL_ADDR);
    if(Enabled)
    {
        WriteByteHDMITXP1(AUDP_TXCTRL_ADDR, RegVal | BIT_TXHDMI_MODE);
    }
    else
    {
        WriteByteHDMITXP1(AUDP_TXCTRL_ADDR, RegVal & (~BIT_TXHDMI_MODE));
    }
}

inline void DelayMS (HI_U16 count)
{
    udelay(count*1000);

#if 0
    volatile HI_U32 i = 0;

    while(i<count * 1000)
    {
      i++;
    }
#endif
}

