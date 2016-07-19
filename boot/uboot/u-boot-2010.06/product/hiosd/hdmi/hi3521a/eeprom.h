/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/
#include "typedefs.h"
#include "eedefs.h"
#include "video.h"

#ifndef __EEPROM_H_20100414_
#define __EEPROM_H_20100414_

HI_U8 CheckVersionID(void);
void WriteDefaultConfigToEEPROM( void);
HI_U8 ReadByteEEPROM(HI_U8);
void WriteByteEEPROM(HI_U8, HI_U8);
void BlockWriteEEPROM(HI_U8, HI_U16, HI_U8 *);
HI_U8 BlockReadEEPROM(HI_U8, HI_U16, HI_U8 *);
void WriteWordEEPROM(HI_U8 RegAddr, HI_U16 data);
HI_U16 ReadWordEEPROM(HI_U8 RegAddr);

#define ReadEECVMPol(OFFSET)     ReadByteEEPROM( EE_CUSTVMODE + OFFSET )
#define ReadFullEDIDParserType() ReadByteEEPROM(EE_EDID_PARSER_ADDR)
#define ReadEEHStartPos(OFFSET)  ReadWordEEPROM( EE_CUSTVMODE + OFFSET + HSTART_OFFSET )
#define ReadEEVStartPos(OFFSET)  ReadWordEEPROM( EE_CUSTVMODE + OFFSET + VSTART_OFFSET )
#define ReadEEHRes(OFFSET)       ReadWordEEPROM( EE_CUSTVMODE + OFFSET + HRES_OFFSET )
#define ReadEEVRes(OFFSET)       ReadWordEEPROM( EE_CUSTVMODE + OFFSET + VRES_OFFSET )
#define ReadEEPixClock(OFFSET)   ReadWordEEPROM( EE_CUSTVMODE + OFFSET + FPIX_OFFSET)

#endif

