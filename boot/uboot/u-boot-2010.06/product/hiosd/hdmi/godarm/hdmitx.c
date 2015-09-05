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

//---------------------------------------------------------------------

//-------------------------------------------------------------------
static void hdmiPhyInit(void)
{
    HI_U32 u32Value = 0;
    /* hdmi PHY */
    HDMI_REG_WRITE(0x205d1804, 0x00000028);
    u32Value = HDMI_REG_READ(0x205d1804);
    HDMIPrint("TMDS_CTL2 set to:0x%x\n", u32Value);
    HDMI_REG_WRITE(0x205d1820, 0x00000060);
    HDMI_REG_WRITE(0x205d1808, 0x000000a9);
    HDMI_REG_WRITE(0x205d180c, 0x00000040);
    HDMI_REG_WRITE(0x205d1818, 0x00000010);
}

void HW_ResetHDMITX(void)
{
    HDMIPrint("--> HW_ResetHDMITX.\n");
    HdmiHardwareReset(1);
    DelayMS(18);
    HdmiHardwareReset(0);
    DelayMS(50);
    
    hdmiPhyInit();
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
    HI_U8 TimeOut = 255;
    HI_U8 RegVal;

    HDMIPrint("--> SW_ResetHDMITX.\n");


    //power down hdmi
    RegVal = ReadByteHDMITXP0(TX_SYS_CTRL1_ADDR);
    RegVal &= ~ BIT_TX_PD;
    WriteByteHDMITXP0 (TX_SYS_CTRL1_ADDR, RegVal);         // 0x35->0x37 GVG
    
    while ( !siiIsTClockStable() && TimeOut-- )
    {
        DelayMS(20);         // wait for input pixel clock to stabilze
    }
        
    if (TimeOut)
    {
        AssertHDMITX_SWReset(BIT_TX_SW_RST | BIT_TX_FIFO_RST);
        DelayMS(1);
        ReleaseHDMITX_SWReset(BIT_TX_SW_RST | BIT_TX_FIFO_RST);
        DelayMS(1);          // allow TCLK (sent to Rx across the HDMS link) to stabilize
    }

    DelayMS(1);
}
//-------------------------------------------------------------------
void WakeUpHDMITX(void)
{
    HI_U8 RegVal;

    RegVal = ReadByteHDMITXP0(TX_SYS_CTRL1_ADDR) | BIT_TX_PD;
    WriteByteHDMITXP0 (TX_SYS_CTRL1_ADDR, RegVal);         // 0x35->0x37 GVG
    WriteByteHDMITXP0 (INT_CNTRL_ADDR, 0x00);       // Interrupt pin control defined per board

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
    //usleep(count*1000);
     HI_U32 i = 0;

     while(i<count * 1000)
     {
          i++;
     }    
}

