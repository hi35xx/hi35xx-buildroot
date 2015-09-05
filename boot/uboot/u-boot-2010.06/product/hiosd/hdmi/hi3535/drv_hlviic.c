/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

#include "linux/string.h"
#include "typedefs.h"
#include "hlviic.h"

#include "hi_type.h"
#include "defstx.h"
#include "hdmitx.h"
#include "mpi_hdmi.h"

//-------------------------------------------------------------------
HI_U8 ReadByte_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr)
{
    HI_U8 Data = 0xff;

    if (TX_SLV0 == SlaveAddr)
    {
        Data = HDMI_REG_READ(HDMI_TX_SLV0_ADDR_BASE + RegAddr * 4);
    }
    else
    {
        Data = HDMI_REG_READ(HDMI_TX_SLV1_ADDR_BASE + RegAddr * 4);
    }
    
    return Data;
}

//-------------------------------------------------------------------
void WriteByte_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr, HI_U8 Data)
{  
    if (TX_SLV0 == SlaveAddr)
    {
        HDMI_REG_WRITE((HDMI_TX_SLV0_ADDR_BASE + RegAddr * 4), Data);
    }
    else
    {
        HDMI_REG_WRITE((HDMI_TX_SLV1_ADDR_BASE + RegAddr * 4), Data);
    }
    return;
}

//--------------------------------------------------------------------

HI_U16 ReadWord_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr)
{
    HI_U16 Data = 0xffff;
	volatile HI_U32 	*pu32VirAddr = HI_NULL;

    if(SlaveAddr==TX_SLV0)
    {
        pu32VirAddr = (volatile HI_U32 *)(HDMI_TX_SLV0_ADDR_BASE + RegAddr * 4);
        Data = (HI_U16)((pu32VirAddr[0]&0xff) | ((pu32VirAddr[1]&0xff) << 8));
    }
    else if(SlaveAddr==TX_SLV1)
    {
        pu32VirAddr = (volatile HI_U32 *)(HDMI_TX_SLV1_ADDR_BASE + RegAddr * 4);
        Data = (HI_U16)((pu32VirAddr[0]&0xff) | ((pu32VirAddr[1]&0xff) << 8));
    }
    return Data;
}

//-------------------------------------------------------------------

void WriteWord_8BA(HI_U8 Bus, HI_U8 SlaveAddr, HI_U8 RegAddr, HI_U16 Data)
{
	volatile HI_U32 	*pu32VirAddr = HI_NULL;

    if(SlaveAddr==TX_SLV0)
    {
        pu32VirAddr = (volatile HI_U32 *)(HDMI_TX_SLV0_ADDR_BASE + RegAddr * 4);
        pu32VirAddr[0] = Data&0xff;
        pu32VirAddr[1] = (Data&0xff00) >> 8;
    }
    else if(SlaveAddr==TX_SLV1)
    {
        pu32VirAddr = (volatile HI_U32 *)(HDMI_TX_SLV1_ADDR_BASE + RegAddr * 4);
        pu32VirAddr[0] = Data&0xff;
        pu32VirAddr[1] = (Data&0xff00) >> 8;
    }
}

void RegSetBit(unsigned long value, unsigned long offset,
    unsigned long addr)
{
	volatile unsigned long mask;
    volatile unsigned long *t = NULL;

	mask = 1 << offset;
	t = (volatile unsigned long*)(addr);
	*t &= ~mask;
	*t |= (value << offset) & mask;
}

//------------------------------------------------------------------------------------

void HdmiHardwareReset(int iEnable)
{
    HI_BOOL bReset;    
    if (iEnable)
    {        
        bReset = HI_TRUE;
        RegSetBit(1, 0, HDMI_HARDWARE_RESET_ADDR);  /* 撤销复位 */
        RegSetBit(1, 1, HDMI_HARDWARE_RESET_ADDR);  /* 开时钟 */
    }
    else
    {        
        bReset = HI_FALSE;
        RegSetBit(0, 0, HDMI_HARDWARE_RESET_ADDR);  /* 关时钟 */
        RegSetBit(0, 1, HDMI_HARDWARE_RESET_ADDR);
    }
    RegSetBit(1, 3, HDMI_HARDWARE_RESET_ADDR);    
    RegSetBit(1, 2, HDMI_HARDWARE_RESET_ADDR);
    
    return;  
}
