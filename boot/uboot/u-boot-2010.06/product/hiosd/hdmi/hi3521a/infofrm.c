/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include "typedefs.h"
#include "infofrm.h"
#include "hlviic.h"
#include "video.h"
#include "delay.h"
#include "defstx.h"
#include "hdmitx.h"
#include "eeprom.h"
#include "mpi_hdmi.h"


//----------------------------------------------------------------------------------
Bool GetInfoFrameMapAddr(HI_U8 InfoType, InfoMapType * InfoMap )
{
    Bool Result = TRUE;

    if( InfoType == AVI_TYPE)
    {
        InfoMap->CtrlBitsAddr = BIT_AVI_EN_REPEAT;
        InfoMap->DestAddr =  AVI_IF_ADDR;
    }
    else if( InfoType == SPD_TYPE)
    {
        InfoMap->CtrlBitsAddr = BIT_SPD_EN_REPEAT;
        InfoMap->DestAddr = SPD_IF_ADDR;
    }
    else if( InfoType == AUD_TYPE)
    {
        InfoMap->CtrlBitsAddr = BIT_AUD_EN_REPEAT;
        InfoMap->DestAddr = AUD_IF_ADDR;
    }
    else if( InfoType == MPEG_TYPE)
    {
        InfoMap->CtrlBitsAddr = BIT_MPEG_EN_REPEAT;
        InfoMap->DestAddr = MPEG_IF_ADDR;
    }
    else
        Result = FALSE;
    return Result;
}
//----------------------------------------------------------------------------------
// InfoFrame will be enable and set repeat property, InfoFrame cannot be enabled at Power Down mode
//----------------------------------------------------------------------------------
Bool EnableInfoFrame(HI_U8 InfoFrameType)
{
    InfoMapType InfoMap;
    HI_U16 RegVal;
    Bool Result = FALSE;

    RegVal = ReadWordHDMITXP1( INF_CTRL1);
    if(GetInfoFrameMapAddr( InfoFrameType, &InfoMap))
    {
        WriteWordHDMITXP1( INF_CTRL1, RegVal | InfoMap.CtrlBitsAddr );
        Result = TRUE;
    }
    return Result;

}

//----------------------------------------------------------------------------------
// This function is used to send AVI from TX

void SendAVIInfoFrame(void)
{
    HI_U8 CRC, RegVal, TXAddr, EEAddr,i;


    TXAddr = AVI_IF_ADDR;
    EEAddr  = EE_TXAVIINFO_ADDR;

    CRC = 0x82 + 0x02 + 13;

    for(i = 0; i < 13; i++ )
        CRC += ReadByteEEPROM ( EEAddr++);
    CRC = 0x100 - CRC;
    RegVal = ReadByteHDMITXP1( INF_CTRL1 );

    // To send a new AVI InfoFrame, must first clera RPT bit, then wait for EN bit to go to 0, then
    // OK to set EN and RPT bits:

    WriteByteHDMITXP1(INF_CTRL1, RegVal & (~BIT_AVI_REPEAT));	// Clear RPT bit

    DelayMS(128); // Allow EN bit to clear
    if( ReadByteHDMITXP1( INF_CTRL1) & BIT_AVI_ENABLE) //EN bit did not clear
    {
        HDMIPrint(" Send AVI Err, En not Zero! ");
        return;
    }

    WriteByteHDMITXP1( TXAddr++ , 0x82);
    WriteByteHDMITXP1( TXAddr++ , 0x02);
    WriteByteHDMITXP1( TXAddr++ , 13);     //0x0D
    WriteByteHDMITXP1( TXAddr++ , CRC);    // Send Header

    EEAddr  = EE_TXAVIINFO_ADDR;

    HDMIPrint(" SendAVIInfoFrame\n ");
    for(i = 0; i < 13; i++ )
    {
        RegVal = ReadByteEEPROM ( EEAddr++);
        WriteByteHDMITXP1( TXAddr++ , RegVal);
    }

    RegVal = ReadByteHDMITXP1( INF_CTRL1);
    WriteByteHDMITXP1(INF_CTRL1, RegVal | ( BIT_AVI_ENABLE | BIT_AVI_REPEAT));
    RegVal = ReadByteHDMITXP1( INF_CTRL1);
}


