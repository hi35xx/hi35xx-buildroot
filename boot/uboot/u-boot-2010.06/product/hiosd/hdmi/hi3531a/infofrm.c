/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include <common.h>
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

static Bool GetRestOfInfoFrameHeader( HeaderType * Header )
{
    Bool Result = TRUE;

    if( Header->Type == AVI_TYPE)
    {
        Header->Version = 2 ;
        Header->Length = 13;
    }
    else if( Header->Type == SPD_TYPE)
    {
        Header->Version = 1;
        Header->Length = 25;
    }
    else if( Header->Type == AUD_TYPE)
    {
        Header->Version = 1 ;
        Header->Length = 10;
    }
    else if( Header->Type == MPEG_TYPE)
    {
        Header->Version = 1;
        Header->Length = 10;
    }
    else if( Header->Type == VENDORSPEC_TYPE)
    {
        Header->Version = 1;
        Header->Length = 9;// will be change
    }
    else
        Result = FALSE;
    return Result;
}

static Bool WaitBuffReady(HI_U16 CtrInfoBits)
{
    HI_U16 RegVal, RptBitMsk, EnBitMask;
    HI_U8 TimeOutCount = 60;
    Bool Result = TRUE;

    RptBitMsk = (CtrInfoBits >> 1) & CtrInfoBits;
    EnBitMask = (CtrInfoBits << 1) & CtrInfoBits;
    RegVal = ReadWordHDMITXP1( INF_CTRL1);
    if ( RegVal & CtrInfoBits )
    {
        if ( RegVal & RptBitMsk )
        {
            WriteWordHDMITXP1( INF_CTRL1, RegVal & (~RptBitMsk)); // Clear reapet bit
            udelay(1);
        }
        while(TimeOutCount--)
        {
            if( !(ReadWordHDMITXP1( INF_CTRL1) & EnBitMask))
                break;
            //DelayMS(1);
        }      
        if(TimeOutCount)
            Result = TRUE; // ready to rewrite data buffer
        else
        {
#if 0//lc
            Result = FALSE; // repeat bit is stuck, not ready to write into the buffer
#else
	printf("force to set Infoframe buffer ready\n");
	Result = TRUE;
#endif   
        }
    }
    return Result;
}

void SendInfoFramex(void)
{
    HI_U8 ErrNo = 0;
    HI_U8 index;
    InfoMapType InfoMap;
    HeaderType IFHeader;
    HI_U8 TXAddr;

    HI_U16 RegVal;

    HI_U8 VendorBody[20], length = 0;
    HI_U8 bIFType = VENDORSPEC_TYPE;
    HI_U8 U8HDMI_Video_Format = 0x01, u8HDMI_VIC = 0x01;
    HI_U8 * InfoFramePacket = (HI_U8 * )&VendorBody;
    //IEEE
    VendorBody[length++] = 0x03;
    VendorBody[length++] = 0x0c;
    VendorBody[length++] = 0x00;

    VendorBody[length++] = (U8HDMI_Video_Format & 0x3) << 5;
    VendorBody[length++] = u8HDMI_VIC;
    
    memset(&IFHeader, 0, sizeof(HeaderType));
    if( GetInfoFrameMapAddr( bIFType, &InfoMap) )
    {
        IFHeader.Type = bIFType;
        GetRestOfInfoFrameHeader(  &IFHeader );
        IFHeader.Length = length;
        if ( !siiIsTClockStable() )
            ErrNo = 7;
        else if ( !WaitBuffReady(InfoMap.CtrlBitsAddr ) )
            ErrNo = 3;

        CalcCheckSumIFPacket( &IFHeader, InfoFramePacket );

        TXAddr = InfoMap.DestAddr;
        /* Fill header */
        WriteByteHDMITXP1(TXAddr++, IFHeader.Type);
        WriteByteHDMITXP1(TXAddr++, IFHeader.Version);
        WriteByteHDMITXP1(TXAddr++, IFHeader.Length);
        WriteByteHDMITXP1(TXAddr++, IFHeader.CheckSum);
        /* Fill Body */
        for(index = 0; index < IFHeader.Length; index ++)
        {
            WriteByteHDMITXP1(TXAddr++, InfoFramePacket[index]);
        }
        
        TXAddr = InfoMap.DestAddr;
        HDMIPrint("Debug Info frame output length:%d\n", (IFHeader.Length + 4));
        for(index = 0; index < (IFHeader.Length + 4); index++ )
        {
            RegVal = ReadByteHDMITXP1( TXAddr++);
            HDMIPrint("0x%02x\n", RegVal);
        }
        HDMIPrint("End of Info frame\n");
            
        if ( !ErrNo )
        {
            RegVal = ReadWordHDMITXP1( INF_CTRL1);
            WriteWordHDMITXP1( INF_CTRL1, RegVal | InfoMap.CtrlBitsAddr );
            udelay(1);//lc add
        }
    }

}


