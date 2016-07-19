#ifdef HDMI_CEC_SUPPORT
#include "si_datatypes.h"
#include "si_drv_cpi_api.h"
//#include "si_cec_enums.h"
#include "si_lib_malloc_api.h"
#include "si_lib_obj_api.h"
#include "si_lib_log_api.h"
#include "si_lib_time_api.h"
#include "sii_time.h"
#include "hal_cec.h"

static HI_VOID SiiCecCallBack(SiiInst_t inst, HI_VOID* data);

static HI_CHAR l_cecTxOsdNameString [] = "HISI";
static HI_U8 CecVendorID[3] = {'H','I','S'}; //should be HISI vendor ID
static SiiInst_t sCpiInst = HI_NULL;



static HI_U8 l_devTypes [16] =
{
	CEC_LOGADDR_TV,
	CEC_LOGADDR_RECDEV1,
	CEC_LOGADDR_RECDEV1,
	CEC_LOGADDR_TUNER1,
	CEC_LOGADDR_PLAYBACK1,
	CEC_LOGADDR_AUDSYS,
	CEC_LOGADDR_PURE_SWITCH,
	CEC_LOGADDR_VIDEO_PROCESSOR,
	CEC_LOGADDR_PLAYBACK1,
	CEC_LOGADDR_RECDEV1,
	CEC_LOGADDR_TUNER1,
	CEC_LOGADDR_PLAYBACK1,
	CEC_LOGADDR_RECDEV2,
	CEC_LOGADDR_RECDEV2,
	CEC_LOGADDR_TV,
	CEC_LOGADDR_TV
};

static HI_U8 cecValidate [128] =
{
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1,

	//0x0E - 0x0F Reserved
	0, 0,

	1, 1,

	//0x12 - 0x1C Reserved
	0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0,

	1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1,

	//0x2D - 0x2E Reserved
	0, 0,

	1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1,

	//0x39 - 0x3F Reserved
	0, 0, 0,
	0, 0, 0, 0,

	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1,

	//0x4D - 0x4F Reserved
	0, 0, 0,

	1, 1, 1, 1,
	1, 1, 1, 1,

	//0x58 - 0x5F Reserved
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1,

	//0x6E - 0x70 Reserved
	0, 0,
	0,

	1, 1, 1,
	1, 1, 1,

	// 0x77 - 0x7F Reserved
	0,
	0, 0, 0, 0,
	0, 0, 0, 0
};





HI_VOID SiiCecEnable(HDMI_HAL_CEC_S* pstCec, HI_BOOL bEnable)
{
	if (bEnable)
    {
       pstCec->bWakeUpFlag = HI_FALSE;
       SiiDrvCpiEnable(sCpiInst);
    }
    else
    {
        pstCec->bWakeUpFlag = HI_TRUE;
        wake_up(&pstCec->msgQueue.msgWaitQueue);
        SiiDrvCpiDisable(sCpiInst);
    }
 
	pstCec->bEnable = bEnable;
    pstCec->isActiveSource = bEnable;
}

static void SiiCecReset( HDMI_HAL_CEC_S* pstCec )
{
	HI_U16 i = 0;

	pstCec->logicalAddr = CEC_LOGADDR_PLAYBACK1;
	pstCec->powerState = CEC_POWERSTATUS_STANDBY;
	pstCec->sourcePowerStatus = CEC_POWERSTATUS_STANDBY;

	for ( i = 0; i <= CEC_LOGADDR_UNREGORBC; i++ )
	{
		pstCec->logicalDeviceInfo[i].devPA = 0;       
		pstCec->logicalDeviceInfo[i].devLA = 0;
		pstCec->logicalDeviceInfo[i].deviceType = 0;
	}
}

static HI_U16 SiiCecSendReportPhysicalAddress (HDMI_HAL_CEC_S* pstCec)
{
	SiiCpiData_t cecFrame;

	cecFrame.opcode         = CECOP_REPORT_PHYSICAL_ADDRESS;
	cecFrame.srcDestAddr    = MAKE_SRCDEST( pstCec->logicalAddr, CEC_LOGADDR_UNREGORBC );
	cecFrame.args[0]        = pstCec->physicalAddr >> 8;           // [Physical Address] High
	cecFrame.args[1]        = pstCec->physicalAddr & 0xFF;         // [Physical Address] Low
	cecFrame.args[2]        = l_devTypes[pstCec->logicalAddr];    // [Device Type]
	cecFrame.argCount       = 3;

	return( SiiDrvCpiWrite( sCpiInst, &cecFrame ));
}

HI_S32 SiiCecAutoPing(HDMI_HAL_CEC_S* pstCec)
{
    HI_U16 mapStatus;
    HI_U8  index;
    
    if (!SiiDrvCpiAutoPing(sCpiInst, &mapStatus))
    {
        //CEC_ERR("SiiDrvCpiAutoPing fail\n");
        return HI_FAILURE;
    }
  
    //CEC Network
    for(index = 0; index <= CEC_LOGADDR_UNREGORBC; index ++)
    {
        if((mapStatus & (1 << index)) != 0)
        {
            pstCec->network[index] = 1;
            SII_LIB_LOG_DEBUG2("cec ping logAddr[0x%02x] Ack \n",index);
        }
        else
        {
            SII_LIB_LOG_DEBUG2("cec ping logAddr[0x%02x] No Ack \n",index);
        }
    }
    
    //Logical Address
    if(pstCec->network[CEC_LOGADDR_TUNER1])//bit3
    {
        if(pstCec->network[CEC_LOGADDR_PURE_SWITCH])//bit6
        {
            if(pstCec->network[CEC_LOGADDR_VIDEO_PROCESSOR])//bit7
            {
                if(pstCec->network[CEC_LOGADDR_TUNER4])//bit10
                    pstCec->logicalAddr = CEC_LOGADDR_UNREGORBC;//Brocast Address!
                else
                    pstCec->logicalAddr = CEC_LOGADDR_TUNER4;
            }
            else
            {
                pstCec->logicalAddr = CEC_LOGADDR_VIDEO_PROCESSOR;
            }
        }
        else
        {
            pstCec->logicalAddr = CEC_LOGADDR_PURE_SWITCH;
        }
    }
    else
    {
        pstCec->logicalAddr = CEC_LOGADDR_TUNER1;
    }
    SII_LIB_LOG_DEBUG2("set logicalAddr %d\n",pstCec->logicalAddr);
    SiiDrvCpiSetLogicalAddr(sCpiInst, pstCec->logicalAddr);
    SiiCecSendReportPhysicalAddress(pstCec);
    return HI_SUCCESS;
}

HI_S32 SiiCecMsgSend(HDMI_HAL_CEC_S* pstCec, SiiCpiData_t* cecFrame)
{
	return( SiiDrvCpiWrite( sCpiInst, cecFrame ));
}

HI_S32 SiiCecMsgRead(HDMI_HAL_CEC_S* pstCec, HDMI_CEC_MSG_S* msg)
{
    HI_S32 s32Ret;
    
    s32Ret = wait_event_interruptible(pstCec->msgQueue.msgWaitQueue, (pstCec->msgQueue.msgValidNum > 0 || pstCec->bWakeUpFlag));
    if((s32Ret <= 0) && (0 == pstCec->msgQueue.msgValidNum))
    {
        return HI_FAILURE;;
    }
 
    if (down_interruptible(&pstCec->msgQueue.msgMutex))
       return -ERESTARTSYS;

    if (pstCec->msgQueue.msgValidNum > 0)
    {
        memcpy(msg, &pstCec->msgQueue.cecMsgPool[pstCec->msgQueue.msgReadPtr++], sizeof(HDMI_CEC_MSG_S));
        pstCec->msgQueue.msgReadPtr = (pstCec->msgQueue.msgReadPtr == CEC_MSG_POOL_SIZE) ? 0 : pstCec->msgQueue.msgReadPtr;

        if (pstCec->msgQueue.msgWritePtr == pstCec->msgQueue.msgReadPtr)
        {
            pstCec->msgQueue.msgValidNum = 0;
        }
        else if (pstCec->msgQueue.msgWritePtr > pstCec->msgQueue.msgReadPtr)
        {
            pstCec->msgQueue.msgValidNum = pstCec->msgQueue.msgWritePtr - pstCec->msgQueue.msgReadPtr;
        }
        else 
        {
            pstCec->msgQueue.msgValidNum = CEC_MSG_POOL_SIZE - (pstCec->msgQueue.msgReadPtr - pstCec->msgQueue.msgWritePtr);
        }

    }

    up(&pstCec->msgQueue.msgMutex);
  
    return HI_SUCCESS;
}

static HI_S32 CecMsgWrite(HDMI_HAL_CEC_S* pstCec, SiiCpiData_t* cecFrame)
{
    HDMI_CEC_MSG_S msg;
    
    memset(&msg, 0, sizeof(HDMI_CEC_MSG_S));
    msg.srcDestAddr = cecFrame->srcDestAddr;
    msg.opcode      = cecFrame->opcode;
    msg.argCount    = cecFrame->argCount;
    memcpy(msg.args, cecFrame->args, msg.argCount);
    
    if (down_interruptible(&pstCec->msgQueue.msgMutex))
       return -ERESTARTSYS;

    memcpy(&pstCec->msgQueue.cecMsgPool[pstCec->msgQueue.msgWritePtr++], &msg, sizeof(HDMI_CEC_MSG_S));

    pstCec->msgQueue.msgWritePtr = (pstCec->msgQueue.msgWritePtr == CEC_MSG_POOL_SIZE) ? 0 : pstCec->msgQueue.msgWritePtr;

    if (pstCec->msgQueue.msgWritePtr > pstCec->msgQueue.msgReadPtr)
    {
        pstCec->msgQueue.msgValidNum = pstCec->msgQueue.msgWritePtr - pstCec->msgQueue.msgReadPtr;
    }
    else if (pstCec->msgQueue.msgWritePtr < pstCec->msgQueue.msgReadPtr)
    {
        pstCec->msgQueue.msgValidNum = CEC_MSG_POOL_SIZE - (pstCec->msgQueue.msgReadPtr - pstCec->msgQueue.msgWritePtr);
    }
    else
    {
        pstCec->msgQueue.msgValidNum = CEC_MSG_POOL_SIZE;
    }
    
    up(&pstCec->msgQueue.msgMutex);

    if (pstCec->msgQueue.msgValidNum > 0)
    {
        wake_up(&pstCec->msgQueue.msgWaitQueue);
    }
   
    return HI_SUCCESS;
}

HDMI_HAL_CEC_S* SiiCecDeviceCreate(SiiInst_t inst, HI_U16 cpiBaseAddr )
{
    HDMI_HAL_CEC_S* pstCec = HI_NULL;
	pstCec = (HDMI_HAL_CEC_S *) SiiLibMallocCreate(sizeof(HDMI_HAL_CEC_S));
    if (pstCec == HI_NULL)
    {
         return HI_NULL;
    }
    
    HDMI_INIT_WAIT_QUEUE(pstCec->msgQueue.msgWaitQueue);
    HDMI_INIT_MUTEX(pstCec->msgQueue.msgMutex);
    
	SiiCecReset (pstCec);

	sCpiInst = SiiDrvCpiCreate(inst, cpiBaseAddr, SiiCecCallBack, pstCec);
    if (sCpiInst == HI_NULL)
    {
        SiiLibMallocDelete(pstCec);
        return HI_NULL;
    }

    return pstCec;
}

void SiiCecDeviceDelete(HDMI_HAL_CEC_S* pstCec)
{
    if (pstCec)
    {
        pstCec->bWakeUpFlag = HI_TRUE;
        wake_up(&pstCec->msgQueue.msgWaitQueue);
    	msleep(200);
    	SiiDrvCpiDelete(sCpiInst);
    	SiiLibMallocDelete(pstCec);
    }

}


//------------------------------------------------------------------------------
static HI_U16 SiiCecSendReportPowerStatus (HDMI_HAL_CEC_S* pstCec, SiiCecLogicalAddresses_t destLa )
{
	SiiCpiData_t cecFrame;

	cecFrame.opcode        = CECOP_REPORT_POWER_STATUS;
	cecFrame.srcDestAddr   = MAKE_SRCDEST( pstCec->logicalAddr, destLa );
	cecFrame.args[0]       = pstCec->powerState;
	cecFrame.argCount      = 1;

	return( SiiDrvCpiWrite( sCpiInst, &cecFrame ));
}

static HI_U16 SiiCecSendVendorId(  SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa,HI_U8 *vendorId )
{
	SiiCpiData_t cecFrame;

	cecFrame.opcode        = CECOP_DEVICE_VENDOR_ID;
	cecFrame.srcDestAddr   = MAKE_SRCDEST( srcLa, destLa );
	cecFrame.args[0]       = vendorId[0];
	cecFrame.args[1]       = vendorId[1];
	cecFrame.args[2]       = vendorId[2];
	cecFrame.argCount      = 3;

	return( SiiDrvCpiWrite( sCpiInst, &cecFrame ));

}
//------------------------------------------------------------------------------
static HI_BOOL SiiCecSetOsdName (HDMI_HAL_CEC_S* pstCec, char *pOsdName )
{
	HI_S32 nameLen = strlen( pOsdName );

	if ( nameLen < 14 )
	{
		pstCec->osdNameLen = nameLen;
		memcpy( pstCec->osdName, pOsdName, pstCec->osdNameLen );
	}

	return RESULT_CEC_SUCCESS;
}
//------------------------------------------------------------------------------
//! @brief  Send the device name string as the OSD name
//------------------------------------------------------------------------------
static void CecSendSetOsdName (HDMI_HAL_CEC_S* pstCec, HI_U8 destLogicalAddr )
{
	SiiCpiData_t cecFrame;

	cecFrame.opcode         = CECOP_SET_OSD_NAME;
	cecFrame.srcDestAddr    = MAKE_SRCDEST( pstCec->logicalAddr, destLogicalAddr );
	memcpy( &cecFrame.args, pstCec->osdName, pstCec->osdNameLen );
	cecFrame.argCount       = pstCec->osdNameLen;

	SiiDrvCpiWrite( sCpiInst, &cecFrame );
}

//-------------------------------------------------------------------------------------------------
static void SiiCecFeatureAbortSendEx (HI_U8 opCode, HI_U8 reason, SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa )
{
	SiiCpiData_t cecFrame;

	cecFrame.args[0]        = opCode;
	cecFrame.args[1]        = reason;
	cecFrame.opcode         = CECOP_FEATURE_ABORT;
	cecFrame.srcDestAddr    = MAKE_SRCDEST(srcLa, destLa );
	cecFrame.argCount       = 2;

	SiiDrvCpiWrite( sCpiInst, &cecFrame );
}

//------------------------------------------------------------------------------
//! @brief  Send an ACTIVE SOURCE message.  Does not wait for a reply.
//! @param[in]  - logicalAddr   Logical device going active
//! @param[in]  - physicalAddr  Physical address of device
//------------------------------------------------------------------------------
void SiiCecSendActiveSource ( HI_U8 logicalAddr, HI_U16 physicalAddr )
{
	SiiCpiData_t cecFrame;

	cecFrame.opcode         = CECOP_ACTIVE_SOURCE;
	cecFrame.srcDestAddr    = MAKE_SRCDEST( logicalAddr, CEC_LOGADDR_UNREGORBC );
	cecFrame.args[0]        = physicalAddr >> 8;        // [Physical Address] High
	cecFrame.args[1]        = physicalAddr & 0xFF;      // [Physical Address] Low
	cecFrame.argCount       = 2;

	SiiDrvCpiWrite( sCpiInst, &cecFrame );
}

static void SiiCecUpdateLogicalDeviceInfo (HDMI_HAL_CEC_S* pstCec, HI_U8 newLA, HI_U16 newPA, HI_BOOL isActive )
{

	pstCec->logicalDeviceInfo[ newLA ].devPA      = newPA;
	pstCec->logicalDeviceInfo[ newLA ].devLA      = newLA;
	pstCec->logicalDeviceInfo[ newLA ].deviceType = l_devTypes[ newLA ];
	pstCec->logicalDeviceInfo[ newLA ].selected = isActive? true:false;

}

//------------------------------------------------------------------------------
static HI_BOOL SiiCecValidateKeyCode ( HI_U8 keyData )
{
	HI_BOOL  validKey = false;

	// All keys 0x80 - 0xFF are invalid, use the table for the rest
	if (( cecValidate[ keyData & ~SII_BIT7]) != 0 )
	{
		validKey = true;
	}

	return( validKey );
}


static HI_BOOL CecValidateMessage (HDMI_HAL_CEC_S* pstCec, SiiCpiData_t *pMsg )
{
	HI_U8 parameterCount = 0;
	HI_BOOL  countOK = true;
	HI_BOOL  isFromUnregistered = false;

	// If message is from Broadcast address, we ignore it except for
	// some specific cases.

	if (( pMsg->srcDestAddr & 0xF0 ) == 0xF0 )
	{
		switch ( pMsg->opcode )
		{
		case CECOP_STANDBY:
		case CECOP_SYSTEM_AUDIO_MODE_REQUEST:
		case CECOP_ROUTING_CHANGE:
		case CECOP_ROUTING_INFORMATION:
		case CECOP_ACTIVE_SOURCE:
		case CECOP_GIVE_PHYSICAL_ADDRESS:
		case CECOP_REPORT_PHYSICAL_ADDRESS:
		case CECOP_REQUEST_ACTIVE_SOURCE:
		case CECOP_GET_MENU_LANGUAGE:
		case CECOP_SET_STREAM_PATH:
		case CDCOP_HEADER:
			break;
		default:
			isFromUnregistered = true;          // All others should be ignored
			break;
		}
	}

	/* Determine required parameter count   */

	switch ( pMsg->opcode )
	{
	case CECOP_IMAGE_VIEW_ON:
	case CECOP_TEXT_VIEW_ON:
	case CECOP_STANDBY:
	case CECOP_GIVE_PHYSICAL_ADDRESS:
	case CECOP_GIVE_DEVICE_VENDOR_ID:
	case CECOP_GIVE_DEVICE_POWER_STATUS:
	case CECOP_GET_MENU_LANGUAGE:
	case CECOP_GET_CEC_VERSION:
	case CECOP_INITIATE_ARC:
	case CECOP_REPORT_ARC_INITIATED:
	case CECOP_REPORT_ARC_TERMINATED:
	case CECOP_REQUEST_ARC_INITIATION:
	case CECOP_REQUEST_ARC_TERMINATION:
	case CECOP_TERMINATE_ARC:
	case CECOP_ABORT:
		parameterCount = 0;
		break;
	case CECOP_REPORT_POWER_STATUS:         // power status
	case CECOP_CEC_VERSION:                 // cec version
		parameterCount = 1;
		break;
	case CECOP_INACTIVE_SOURCE:             // physical address
	case CECOP_FEATURE_ABORT:               // feature opcode / abort reason
	case CECOP_ACTIVE_SOURCE:               // physical address
		parameterCount = 2;
		break;
	case CECOP_REPORT_PHYSICAL_ADDRESS:     // physical address / device type
	case CECOP_DEVICE_VENDOR_ID:            // vendor id
		parameterCount = 3;
		break;
	case CECOP_USER_CONTROL_PRESSED:        // UI command
	case CECOP_SET_OSD_NAME:                // osd name (1-14 bytes)
	case CECOP_SET_OSD_STRING:              // 1 + x   display control / osd string (1-13 bytes)
		parameterCount = 1;                 // must have a minimum of 1 operands
		break;

	default:
		break;
	}

	/* Test for correct parameter count.    */

	if (( pMsg->argCount < parameterCount ) || isFromUnregistered )
	{
		countOK = false;
	}

	return( countOK );
}



static HI_BOOL CecMsgHandlerFirst (HDMI_HAL_CEC_S* pstCec, SiiCpiData_t *pMsg )
{
	HI_BOOL  usedMessage         = true;
	HI_BOOL  isDirectAddressed   = !((pMsg->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );

	// Don't process unless it is intended for the local logical address (we must check
	// in case another device has been added for virtual device support).
	switch ( pMsg->opcode )
	{
	case CECOP_STANDBY:                                             // Direct and Broadcast
		if ( pstCec->powerState != CEC_POWERSTATUS_STANDBY )
		{
			// Next time through the main loop, power will be cycled off

			pstCec->powerState    = CEC_POWERSTATUS_ON_TO_STANDBY;
			//pstCec->statusFlags   |= SiiCEC_POWERSTATE_CHANGE;        // Signal upper layer
			pstCec->isActiveSource = false;                           // Only impacts TX
		}
		break;

	case CECOP_GIVE_PHYSICAL_ADDRESS:
		if ( isDirectAddressed)                    // Ignore as broadcast message
		{

			SiiCecSendReportPhysicalAddress(pstCec );
		}
		else
			usedMessage = false;
		break;

	case CECOP_REPORT_PHYSICAL_ADDRESS:
 
		if ( !isDirectAddressed )                   // Ignore as direct message
		{
			if(pMsg->args[2] != 0)
				pMsg->srcDestAddr = (pMsg->srcDestAddr & 0x0F) | (pMsg->args[2] << 4) ;

			SiiCecUpdateLogicalDeviceInfo(pstCec,(pMsg->srcDestAddr >> 4) & 0x0F,         // broadcast logical address
				(((HI_U16)pMsg->args[0]) << 8) | pMsg->args[1],   // broadcast physical address
				false
				);
        #if 0
            // Let Enumerate task know about it.
			if ( ACTIVE_TASK.task == SiiCECTASK__ENUMERATE )
			{
				ACTIVE_TASK.cpiState = CPI_RESPONSE;
			}
        #endif    
		}
		break;

	case CECOP_GIVE_DEVICE_POWER_STATUS:
		if ( isDirectAddressed )                // Ignore as broadcast message
		{
			SiiCecSendReportPowerStatus( pstCec,(SiiCecLogicalAddresses_t)(pMsg->srcDestAddr >> 4) );
		}
		else
			usedMessage = false;
		break;

	case CECOP_REPORT_POWER_STATUS:                 // Someone sent us their power state.
		if ( isDirectAddressed )                    // Ignore as broadcast message
		{
			//CecLogicalDevice_t cld;  // Doubt 
			//SiiCecGetLogicalDevice((SiiCecLogicalAddresses_t)((pMsg->srcDestAddr & 0xF0) >> 4), &cld);
			// unrecognised device not taken care
			pstCec->sourcePowerStatus = (HDMI_CEC_POWER_STATUS_S)pMsg->args[0];
		}
		break;

	case CECOP_USER_CONTROL_PRESSED:

        if ( !isDirectAddressed )
			break;  // Ignore as broadcast message

		// If not a VALID CEC key, feature abort
		if ( !SiiCecValidateKeyCode( pMsg->args[0]))
		{
			SiiCecFeatureAbortSendEx( (HI_U8)pMsg->opcode, CECAR_INVALID_OPERAND, pstCec->logicalAddr,(SiiCecLogicalAddresses_t)GET_CEC_SRC( pMsg->srcDestAddr ));
			break;              // Used the message...
		}
   		usedMessage = false;    // Didn't use the message, we just validated the key ID 
        break;

	case CECOP_GIVE_OSD_NAME:
		SiiCecSetOsdName(pstCec,l_cecTxOsdNameString);
		usedMessage = false;
		break;

	case CECOP_GIVE_DEVICE_VENDOR_ID:
		SiiCecSendVendorId(pstCec->logicalAddr, CEC_LOGADDR_UNREGORBC, CecVendorID);
		break;

	default:
		usedMessage = false;                        // Didn't use the message
		break;
	}

	return( usedMessage );
}

//------------------------------------------------------------------------------------------
static HI_BOOL CecMsgHandler(HDMI_HAL_CEC_S* pstCec, SiiCpiData_t *pMsg)
{
	HI_BOOL  isDirectAddressed, usedMessage;

	isDirectAddressed   = !((pMsg->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );
	usedMessage         = false;

	switch ( pMsg->opcode )
	{
	case CECOP_ACTIVE_SOURCE:
		if ( !isDirectAddressed )                   // Ignore as direct message
		{
			switch ((pMsg->srcDestAddr & 0xF0) >> 4)
			{
			case CEC_LOGADDR_PLAYBACK1:
			case CEC_LOGADDR_PLAYBACK2:
			case CEC_LOGADDR_PLAYBACK3:
				//              case CEC_LOGADDR_TV:
				pstCec->isActiveSource = false;
				//            	DEBUG_PRINT(MSG_ALWAYS,"used Message in ACTIVE SOURCE\n");
				usedMessage = true;
				break;
			default:
				break;
			}
		}
		else
			usedMessage = true;
		break;

	case CECOP_REQUEST_ACTIVE_SOURCE:
		if ( !isDirectAddressed )                   // Ignore as direct message
		{
			if ( pstCec->isActiveSource )
			{
				SiiCecSendActiveSource( pstCec->logicalAddr, pstCec->physicalAddr );
			}
		}
		usedMessage = true;
		break;
	default:
		break;
	}

	return( usedMessage );
}

//------------------------------------------------------------------------------
// Function:    CecMsgHandlerLast
// Description: This is the last message handler called in the chain, and
//              parses any messages left untouched by the previous handlers.
//
// NOTE:        Messages handled here should be common to all system types.
//
//------------------------------------------------------------------------------

static HI_BOOL CecMsgHandlerLast (HDMI_HAL_CEC_S* pstCec, SiiCpiData_t *pMsg )
{
	HI_U8         srcAddr;
	HI_BOOL          isDirectAddressed;
	SiiCpiData_t    cecFrame;
    HI_BOOL          usedMessage = false;
    
	isDirectAddressed   = !((pMsg->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );
	srcAddr             = GET_CEC_SRC( pMsg->srcDestAddr );

	switch ( pMsg->opcode )
	{
	case CECOP_FEATURE_ABORT:
		if ( isDirectAddressed )                // Ignore as broadcast message
		{
			SII_LIB_LOG_DEBUG2("Feature abort received\n");
		}
		break;

		// These messages have already been handled for internal purposes
		// by CecRxMsgHandler and passed to the application level
		// and/or were ignored but not consumed by the application level.
		// Ignore them here.

	case CECOP_IMAGE_VIEW_ON:
	case CECOP_TEXT_VIEW_ON:
	case CECOP_GET_MENU_LANGUAGE:
	case CECOP_USER_CONTROL_PRESSED:
		//SII_LIB_LOG_DEBUG2(("CecMsgHandlerLast():: USER CONTROL PRESSED\n"));
		switch(pMsg->args[0])
		{
		case CEC_RC_PLAY:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: PLAY Command Received\n");
			break;
		case CEC_RC_STOP:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: STOP Command Received\n");
			break;
		case CEC_RC_PAUSE:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: PAUSE Command Received\n");
			break;
		case CEC_RC_FAST_FORWARD:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: FAST FORWARD Command Received\n");
			break;
		case CEC_RC_REWIND:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: REWIND Command Received\n");
			break;
		case CEC_RC_UP:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: UP ARROW Command Received\n");
			break;
		case CEC_RC_DOWN:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: DOWN ARROW Command Received\n");
			break;
		case CEC_RC_LEFT:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: LEFT ARROW Command Received\n");
			break;
		case CEC_RC_RIGHT:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: RIGHT ARROW Command Received\n");
			break;
		case CEC_RC_SELECT:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: SELECT Command Received\n");
			break;
		case CEC_RC_0:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 0  Received\n");
			break;
		case CEC_RC_1:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 1  Received\n");
			break;
		case CEC_RC_2 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 2  Received\n");
			break;
		case CEC_RC_3 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 3  Received\n");
			break;
		case CEC_RC_4 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 4  Received\n");
			break;
		case CEC_RC_5 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 5  Received\n");
			break;
		case CEC_RC_6 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 6  Received\n");
			break;
		case CEC_RC_7 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 7  Received\n");
			break;
		case CEC_RC_8 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 8  Received\n");
			break;
		case CEC_RC_9 :
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: Number 9  Received\n");
			break;
		default:
			break;
		}
		break;

	case CECOP_USER_CONTROL_RELEASED:
		//SII_LIB_LOG_DEBUG2(("CecMsgHandlerLast():: User Control Released received\n"));
		break;

		// Handle this here because the app level may have upgraded the version
		// and handled it before it gets here.
	case CECOP_PLAY:
		SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: PLAY Command Received\n");
		switch(pMsg->args[0])
		{
		case CEC_PLAY_FORWARD:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: PLAY FORWARD Command Received\n");
			break;
		case CEC_PLAY_STILL:
			SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: PLAY STILL Command Received\n");
			break;
		case CEC_PLAY_REVERSE:
			SII_LIB_LOG_DEBUG2( "CecMsgHandlerLast():: PLAY REVERSE Command Received\n");
			break;
		default:
			break;
		}
		break;
	case CECOP_DECK_CONTROL:
		SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: DECK CONTROL Command Received\n");
		break;
	case CECOP_VENDOR_REMOTE_BUTTON_DOWN:
		SII_LIB_LOG_DEBUG2("CecMsgHandlerLast():: VENDOR REMOTE BUTTON DOWN Command Received\n");
		break;

	case CECOP_GET_CEC_VERSION:
		if ( isDirectAddressed )                    // Ignore as broadcast message
		{
			// Respond to this request with the CEC version support.
			cecFrame.srcDestAddr   = MAKE_SRCDEST( pstCec->logicalAddr, pMsg->srcDestAddr >> 4 );
			cecFrame.opcode        = CECOP_CEC_VERSION;
			cecFrame.args[0]       = 0x05;       // Report CEC1.4
			cecFrame.argCount      = 1;
			SiiDrvCpiWrite( sCpiInst, &cecFrame );
		}
        usedMessage = true;
		break;

	case CECOP_GIVE_OSD_NAME:
		if ( isDirectAddressed )
		{
			CecSendSetOsdName( pstCec, srcAddr );
		}
        usedMessage = true;
		break;

		// Ignore these messages if unrecognized AND broadcast
		// but feature abort them if directly addressed
	case CECOP_GIVE_DEVICE_VENDOR_ID:
	case CDCOP_HEADER:
		if ( isDirectAddressed )
		{
			SiiCecFeatureAbortSendEx( (HI_U8)pMsg->opcode, CECAR_UNRECOG_OPCODE, pstCec->logicalAddr, (SiiCecLogicalAddresses_t)srcAddr );
		}
        usedMessage = true;
		break;

	case CECOP_SET_STREAM_PATH:
		if ( isDirectAddressed )                   // Ignore as direct message
		{
			if ( pstCec->isActiveSource )
			{
				SiiCecSendActiveSource( pstCec->logicalAddr, pstCec->physicalAddr );
			}
		}
        usedMessage = true;
		break;

		// Any directly addressed message that gets here is not supported by this
		// device, so feature abort it with unrecognized opcode.
		// This means that the app layer must be sure to properly handle any messages
		// that it should be able to handle.

	case CECOP_ABORT:
	//default
		if((pMsg->opcode != CECOP_ABORT) &&
			((pMsg->opcode == CECOP_ROUTING_CHANGE) || (pMsg->opcode == CECOP_ROUTING_INFORMATION) ))
			break;
		if ( isDirectAddressed )                    // Ignore as broadcast message
		{
			SiiCecFeatureAbortSendEx( (HI_U8)pMsg->opcode, CECAR_UNRECOG_OPCODE, pstCec->logicalAddr, (SiiCecLogicalAddresses_t)srcAddr );
		}
        usedMessage = true;
		break;
   default:
        break;
	}

    return usedMessage;
}


static HI_VOID SiiCecCallBack(SiiInst_t inst, HI_VOID* data)
{
	HI_BOOL          processedMsg;
	SiiCpiStatus_t  cecStatus;
	SiiCpiData_t    cecFrame;
	uint_t          frameCount;
    HDMI_HAL_CEC_S * pstCec = (HDMI_HAL_CEC_S*)data;
	//sCpiInst = inst;

	if(pstCec->bEnable)
	{
		SiiDrvCpiServiceWriteQueue(inst);               // Send any pending messages
		SiiDrvCpiHwStatusGet(inst, &cecStatus );       // Get the CEC transceiver status

		if(cecStatus.rxState)
		{
			for(;;)
			{
				frameCount = SiiDrvCpiFrameCount(inst);
				if ( frameCount == 0 )
				{
					break;
				}
				if ( !SiiDrvCpiRead(inst, &cecFrame ))
				{
					SII_LIB_LOG_DEBUG2("Error in Rx Fifo\n" );
					break;
				}
				processedMsg = false;
				if ( !CecValidateMessage(pstCec, &cecFrame ))   // If invalid message, ignore it, but treat it as handled
				{
					processedMsg = true;
               
				}
				if ( !processedMsg && CecMsgHandlerFirst(pstCec, &cecFrame ))   // Handle the common system messages
				{
					processedMsg = true;
				}
				if(!processedMsg && CecMsgHandler(pstCec, &cecFrame))
				{
					processedMsg = true;
				}
				if ( !processedMsg && CecMsgHandlerLast(pstCec, &cecFrame ) ) // 
				{
				   processedMsg = true;   
				}

                if (!processedMsg)
                {
                    CecMsgWrite(pstCec, &cecFrame);
                }
			}

		}

	}


}
#endif
