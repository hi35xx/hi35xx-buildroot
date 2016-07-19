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

//-------------------------------------------------------------------
#define PMC_REG_BASE 0x120e0000
#define SYS_MASK  0x3000
#define CYCLE_NUM 5

typedef enum
{
    HDMI_LEVEL_UNKNOW = 0,
    HDMI_LEVEL_SS,
    HDMI_LEVEL_TT,
    HDMI_LEVEL_FF,
    
}HDMI_LEVEL_E;

static HDMI_LEVEL_E g_emHdmiLevel = HDMI_LEVEL_UNKNOW;

static HDMI_LEVEL_E getHdmiCoreLevel(HI_VOID)
{
    HI_U32 freq_sel = 0, i;
    HI_U32 core_temp;
    
    int core_value0=0,core_value1=0,core_value2=0,core_value3=0, aver_core_vale=0;
    HDMI_LEVEL_E emHdmiLevel = HDMI_LEVEL_UNKNOW;

    freq_sel = HDMI_REG_READ(CRG_REG_BASE+0x34);
    core_temp=(freq_sel & SYS_MASK)>>12;

    switch (core_temp)
    {
    case 0:
    	printf("xtal mode cant't test hpm!\n");
    	return HDMI_LEVEL_UNKNOW;
    case 1:
        HDMI_REG_WRITE(PMC_REG_BASE+0x28,0x5000004);
    	break;
    case 2:
        HDMI_REG_WRITE(PMC_REG_BASE+0x28,0x5000003);
    	break;
    }

    //-------get HPM value -------------
    for(i=0;i<CYCLE_NUM;i++)
    {
        DelayMS(20);
        core_temp=HDMI_REG_READ(PMC_REG_BASE+0x2C);
        core_value1+=(core_temp >> 12)&0x3ff;
        core_value0+=core_temp & 0x3ff;
        core_temp=HDMI_REG_READ(PMC_REG_BASE+0x30);
        core_value3+=(core_temp >> 12)&0x3ff;
        core_value2+=core_temp & 0x3ff;
    }
    core_value0=core_value0/CYCLE_NUM;
    core_value1=core_value1/CYCLE_NUM;
    core_value2=core_value2/CYCLE_NUM;
    core_value3=core_value3/CYCLE_NUM;

    aver_core_vale = (core_value0+core_value1+core_value2+core_value3)/4;

    //printf("CORE_HPM_RESULT:HPM0=%d\t,HPM1=%d\t,HPM2=%d\t,HPM3=%d\n",core_value0,core_value1,core_value2,core_value3);
    //printf("               :aver=%d\n",aver_core_vale);

    if(aver_core_vale < 185)
    {
        emHdmiLevel = HDMI_LEVEL_SS;
    }
    else if(aver_core_vale < 215)
    {
        emHdmiLevel = HDMI_LEVEL_TT;
    }
    else
    {
        emHdmiLevel = HDMI_LEVEL_FF;
    }

    return emHdmiLevel;
}
//-------------------------------------------------------------------
static void hdmiPhyInit(void)
{
    //HI_U32 u32Value = 0;
  
    // oe
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR,0x00);

    // audio clk
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x08,0x02);

    // swing & bw ctrl
    g_emHdmiLevel = getHdmiCoreLevel();
    switch(g_emHdmiLevel)
    {
        case HDMI_LEVEL_SS:
        case HDMI_LEVEL_TT:
            HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x0C,0x40);
            break;
        case HDMI_LEVEL_FF:
            HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x0C,0x41);
            break;
        default:
            HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x0C,0x40);
            break;
    }
    
    // deep color & pixel repeat ctrl
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x10,0x09);

    // driver setting , slew rate ctrl    
    /* the tuner would been disturbed by EMI signal if the reg is setting 0x01 */
    //SI_TX_PHY_WriteRegister(PHY_DRV_ADDR,0x01);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x14, 0x40);   

    // clk invert
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x18,0x40);

    // I think power up at last will imporve stability of pll
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x04,0x01);

}

HI_S32 SI_TX_PHY_PowerDown(HI_BOOL bPwdown)
{
    HI_U32 u32Value = 0;

    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR+0x4);
    if(bPwdown)
    {
        u32Value &= ~0x01;
    }
    else
    {
        u32Value |= 0x01;
    }
    HDMIPrint("writing phy 0x%x PowerDown\n",u32Value);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0x4,u32Value);

   return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_SetDeepColor(HI_U8 bDeepColor)
{
    HI_U32 u32Value = 0;

    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR + 0x10);

    HDMIPrint("PLL_CTRL  old value:0x%x\n", u32Value);

    if (SiI_DeepColor_30bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x00;
        HDMIPrint("SiI_DeepColor_30bit\n");
    }
    else if (SiI_DeepColor_36bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x02;
        HDMIPrint("SiI_DeepColor_36bit\n");
    }
    else if (SiI_DeepColor_24bit == bDeepColor)
    {
        u32Value =  (u32Value & ~0x03) | 0x01;
        HDMIPrint("SiI_DeepColor_24bit(normal)\n");
    }
    else
    {
        u32Value =  (u32Value & ~0x03) | 0x01;
        HDMIPrint("SiI_DeepColor_Off\n");
    }

    HDMIPrint("writing phy 0x%x SetDeepColor\n",u32Value);

    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR + 0x10,u32Value);
    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR + 0x10);
    HDMIPrint("PLL_CTRL  new walue:0x%x\n", u32Value);

    return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_SwingCtrl(HI_U32 u32TMDSClk)
{
    // taiyan phy, use encoder in ctrl
    HI_U32 u32Value = 0;

    u32Value = HDMI_REG_READ(HDMI_TX_PHY_ADDR+0xc);
    u32Value &= ~0xFF;
    if(u32TMDSClk == SWING_DOWN_165M)  //TMDS_clock <= 165M
    {
        switch(g_emHdmiLevel)
        {
            case HDMI_LEVEL_SS:
            case HDMI_LEVEL_TT:
                u32Value |= 0x40;
                break;
            case HDMI_LEVEL_FF:
                u32Value |= 0x41;
                break;
            default:
                u32Value |= 0x40;
                break;
        }
    }
    else                        //TMDS_clock > 165M
    {
        u32Value |= 0x41;
    }

    HDMIPrint("writing phy PHY_PLL1_ADDR(swing):0x%x \n",  u32Value);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR+0xc, u32Value);

    return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_EnableHdmiOutput(void)
{
    HI_U32 u32Reg = 0;
    //新phy
    u32Reg = HDMI_REG_READ(HDMI_TX_PHY_ADDR);
    HDMIPrint("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    u32Reg |= 0x01;
    HDMIPrint("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR, u32Reg);

    return HI_SUCCESS;
}

HI_S32 SI_TX_PHY_DisableHdmiOutput(void)
{
    HI_U32 u32Reg = 0;
    u32Reg = HDMI_REG_READ(HDMI_TX_PHY_ADDR);
    HDMIPrint("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    u32Reg &= ~ 0x01;
    HDMIPrint("writing phy 0x%x EnableHdmiOutput\n",u32Reg);
    HDMI_REG_WRITE(HDMI_TX_PHY_ADDR, u32Reg);

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
    HDMI_REG_WRITE(HDMI_HARDWARE_RESET_ADDR, 0x0000403f);/* enable hdmi clk */
    DelayMS(5);
    #if 0
    HDMI_REG_WRITE(HDMI_HARDWARE_RESET_ADDR, 0x3fc);/* enable hdmi clk */
    #else
    RegSetBit(0, 9, HDMI_HARDWARE_RESET_ADDR);
    RegSetBit(0, 8, HDMI_HARDWARE_RESET_ADDR);

    for (i = 0; i < 8; i++)
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
//====
HI_U32 SI_HPD_Status( void )
{
    HI_U8 ucNewStat = 0;
    
    /*  System Status Register:SYS_STAT(0x72,0x09) */
    /* Only BIT_RSEN&BIT_HPD_PIN is asserted, HPD is ok */
    ucNewStat = GetSysStat() & (BIT_HPD_PIN);
    //ucNewStat = GetSysStat() & (BIT_HPD_PIN | BIT_RSEN);
    //printk("GetSysStat:0x%x\n", GetSysStat());
    
    if (ucNewStat && BIT_HPD_PIN)
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
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

