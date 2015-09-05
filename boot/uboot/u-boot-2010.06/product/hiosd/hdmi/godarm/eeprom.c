/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include "typedefs.h"
#include "hlviic.h"
#include "globdefs.h"
#include "eedefs.h"
#include "video.h"
#include "delay.h"

HI_U8 DefaultAVIInfoFrame[0xd] = {
    0, 0, 0, 0,  // Color Space YC24 (YCbCr 4:2:2),
    0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00
};

extern HI_U8 DefaultTXVPath[4];
extern HI_U8 DefaultTXDVDAudio[4];

/* g_au8EepromMem is virual share memory block for HDMI Control chip to get parameter */
static HI_U8            g_au8EepromMem[EE_MAX_SIZE] = {0};

#define HI_HDMI_EEPROM_LOCK()
#define HI_HDMI_EEPROM_UNLOCK()

//-----------------------------------------------------------
// Read block of DATA from EEPROM

HI_VOID BlockReadEEPROM(HI_U8 NBytes, HI_U16 Addr, HI_U8 * Data)
{
    HI_U8 i;
    HI_HDMI_EEPROM_LOCK();
    for (i = 0; i < NBytes; i++)
    {
        Data[i] = g_au8EepromMem[Addr+i];
    }
    HI_HDMI_EEPROM_UNLOCK();
    return;
}

//-----------------------------------------------------------
void BlockWriteEEPROM(HI_U8 NBytes, HI_U16 Addr,  HI_U8 * Data)
{
    HI_U8 i;
    
    HI_HDMI_EEPROM_LOCK();
    
    for (i = 0; i < NBytes; i++)
    {
        g_au8EepromMem[Addr+i] = Data[i];
    }
    
    HI_HDMI_EEPROM_UNLOCK();
    return;
}

//------------------------------------------------------------------------
HI_U8 ReadByteEEPROM(HI_U8 RegAddr)
{
    return g_au8EepromMem[RegAddr];
}

//------------------------------------------------------------------------
void WriteByteEEPROM(HI_U8 RegAddr, HI_U8 RegData)
{
    HI_HDMI_EEPROM_LOCK();

    g_au8EepromMem[RegAddr] = RegData;    

    HI_HDMI_EEPROM_UNLOCK();
}

HI_U16 ReadWordEEPROM(HI_U8 RegAddr)
{
    HI_U16 data = 0;
    
    HI_HDMI_EEPROM_LOCK();

    data = g_au8EepromMem[RegAddr];
    data = data << 8;
    data |= g_au8EepromMem[RegAddr+1];    

    HI_HDMI_EEPROM_UNLOCK();     

    return data;
}

//------------------------------------------------------------------------
void WriteWordEEPROM(HI_U8 RegAddr, HI_U16 RegData)
{
    HI_HDMI_EEPROM_LOCK();

    g_au8EepromMem[RegAddr] =   (RegData >> 8) & 0xff;    
    g_au8EepromMem[RegAddr+1] = RegData & 0xff;    

    HI_HDMI_EEPROM_UNLOCK();    
}
//------------------------------------------------------------------------

HI_U8 CheckVersionID(void) 
{
    HI_U16 RegVal;
    RegVal = ReadWordEEPROM(VER_ID_ADDR);
    if(RegVal == SiI_ID_EEPROM_VER)
        return 1;
    return 0;
}

//-------------------------------------------------------------------------
void WriteDefaultConfigToEEPROM( void)
{
    WriteWordEEPROM( VER_ID_ADDR, SiI_ID_EEPROM_VER);
    
    WriteByteEEPROM(EE_AVIINFO_ENABLE, 0);
    WriteByteEEPROM(EE_SPDINFO_ENABLE, 0);
    WriteByteEEPROM(EE_AUDINFO_ENABLE, 0);

    WriteByteEEPROM(EE_EDID_PARSER_ADDR, 0); // use LONG parser

    WriteByteEEPROM(EE_TX_VIDEO_MODE_INDEX_ADDR, 1);   // 1==480p // 2==720p

    WriteByteEEPROM(EE_TX_ICLK_ADDR, 0); //ixel data is not replicated 0b01 =
    
    WriteByteEEPROM(EE_TX_DE_ENABLED_ADDR, 0);         // DE disable and force HDMI mode


    WriteByteEEPROM(EE_TX_MCLK_INDEX_ADDR, 0x01);      //256Fs

    WriteByteEEPROM(EE_TX_FPGA, 0xFF);
    WriteByteEEPROM(EE_TX_HDCP, 0/*0xFF*/);      /* hdcp set to default disable */

    BlockWriteEEPROM( 13, EE_TXAVIINFO_ADDR, DefaultAVIInfoFrame);
    BlockWriteEEPROM( 4, EE_TX_VIDEOPATH_ADDR , DefaultTXVPath);
    BlockWriteEEPROM( 4, EE_TX_AUDIOPATH_ADDR , DefaultTXDVDAudio);

}
