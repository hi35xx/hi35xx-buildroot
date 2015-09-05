/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include "typedefs.h"
#include "globdefs.h"
#include "hlviic.h"
#include "defstx.h"

#ifndef __HDMI_TX_H__
#define __HDMI_TX_H__

void HW_ResetHDMITX( void );
void WakeUpHDMITX(void);
void SetIClk(HI_U8);
void SW_ResetHDMITX(void);
void AssertHDMITX_SWReset( HI_U8 );
void ReleaseHDMITX_SWReset( HI_U8 );

#define ReadByteHDMITXP0(ADDR) ReadByte_8BA(BUS_1, TX_SLV0, ADDR)
#define WriteByteHDMITXP0(ADDR, DATA) WriteByte_8BA(BUS_1, TX_SLV0, ADDR, DATA)
#define ReadWordHDMITXP0(ADDR) ReadWord_8BA(BUS_1, TX_SLV0, ADDR)
#define WriteWordHDMITXP0(ADDR, DATA) WriteWord_8BA(BUS_1, TX_SLV0, ADDR, DATA)

#define ReadByteHDMITXP1(ADDR) ReadByte_8BA(BUS_1, TX_SLV1, ADDR)
#define WriteByteHDMITXP1(ADDR, DATA) WriteByte_8BA(BUS_1, TX_SLV1, ADDR, DATA)
#ifdef _9032_WRITEBLOCKP0_
#define WriteWordHDMITXP0(ADDR, DATA) WriteWord_8BA(BUS_1, TX_SLV0, ADDR, DATA)
#endif
#define WriteWordHDMITXP1(ADDR, DATA) WriteWord_8BA(BUS_1, TX_SLV1, ADDR, DATA)
#define ReadWordHDMITXP1(ADDR) ReadWord_8BA(BUS_1, TX_SLV1, (HI_U8) ADDR)

#define GetSysStat() ReadByteHDMITXP0(TX_STAT_ADDR)
#define GetRevId() ReadByteHDMITXP0(0x04)
#define ReadCTSValue() ((HI_U32)ReadWordHDMITXP1(HWCTS_VAL_ADDR))|((HI_U32)(ReadByteHDMITXP1(HWCTS_VAL_ADDR+2)& 0x0F)<<16)
#define WritePixPerSPDIF( PPSPDIF )   WriteByte_8BA(BUS_1, TX_SLV1, SPDIF_SSMPL_ADDR, PPSPDIF)

#define CLR_BITS_1_0	0xFC

#define WAIT_FOR_PIXEL_CK	0x01
#define P_STABLE_BIT		0x01
#define WAIT_V_SYNC			0x02


#define HDMI_REG_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define HDMI_REG_READ(addr) (*(volatile unsigned int *)(addr))


#endif

