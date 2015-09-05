/*******************************************************************************
 *              Copyright 2005 - 2006, Hisilicon Tech. Co., Ltd.
 *                           ALL RIGHTS RESERVED
 * FileName:    hi_hdmi.h
 * Description: This is internal include file for irda
 *
 * History:
 * Version      Date         Author       DefectNum    Description
 *  0.01     2010-08-06      luochen      NULL       Create this file
 ******************************************************************************/

#include "hi_hdmi.h"
#include "mpi_hdmi.h"
#include "txvidp.h"

extern void BlockWriteEEPROM(HI_U8 NBytes, HI_U16 Addr,  HI_U8 * Data);

static inline HI_HDMI_VIDEO_FMT_E VoSyncToHdmiFmt(unsigned int vosync)
{
    switch (vosync)
    {
        case 2:
            return HI_HDMI_VIDEO_FMT_1080P_24;
        case 3:
            return HI_HDMI_VIDEO_FMT_1080P_25;
        case 4:
            return HI_HDMI_VIDEO_FMT_1080P_30;
        case 5:
            return HI_HDMI_VIDEO_FMT_720P_50;
        case 6:
            return HI_HDMI_VIDEO_FMT_720P_60;
        case 7:
            return HI_HDMI_VIDEO_FMT_1080i_50;
        case 8:
            return HI_HDMI_VIDEO_FMT_1080i_60;
        case 9:
            return HI_HDMI_VIDEO_FMT_1080P_50;
        case 10:
            return HI_HDMI_VIDEO_FMT_1080P_60;
        case 11:
            return HI_HDMI_VIDEO_FMT_576P_50;
        case 12:
            return HI_HDMI_VIDEO_FMT_480P_60;
        case 13:
            return HI_HDMI_VIDEO_FMT_VESA_800X600_60;
        case 14:
            return HI_HDMI_VIDEO_FMT_VESA_1024X768_60;
        case 15:
            return HI_HDMI_VIDEO_FMT_VESA_1280X1024_60;
        case 16:
            return HI_HDMI_VIDEO_FMT_VESA_1366X768_60;
        case 17:
            return HI_HDMI_VIDEO_FMT_VESA_1440X900_60;
        case 18:
            return HI_HDMI_VIDEO_FMT_VESA_1280X800_60;
        default:
            return HI_HDMI_VIDEO_FMT_1080P_60;            
    }
}

int hdmi_display(unsigned int vosync, unsigned int input, unsigned int output)    
{
    HI_U8  u8AviInfoFrameByte = 0, Index;
    HI_U8  u8VideoPath[4];
    HI_U8  pu8AviInfoFrame[32];
    HI_U32 bVideoMode = 0, VidCode = 0, bRxVideoMode = 0, bTxVideoMode = 0;
    HI_U32 retval = HI_SUCCESS, hdmi_dvi_mode = 1;
    
    HI_U32 u32PixelRepetition;
    HI_HDMI_COLORSPACE_E enColorimetry;
    HI_HDMI_ASPECT_RATIO_E enAspectRate;

    HI_HDMI_VIDEO_FMT_E enHdFmt;
    HI_HDMI_VIDEO_MODE_E einput = input;
    HI_HDMI_VIDEO_MODE_E eoutput= output;

    enHdFmt = VoSyncToHdmiFmt(vosync);
   
    /* Video Inforframe */
    enColorimetry      = HDMI_COLORIMETRY_ITU709;
    enAspectRate       = HI_HDMI_ASPECT_RATIO_16TO9;
    u32PixelRepetition = HI_FALSE;
    
    if(HI_HDMI_VIDEO_FMT_1080P_60 == enHdFmt)
    {
        VidCode = 16;
        bVideoMode = 0x0b;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_1080P_60\n");
    }
    else if(HI_HDMI_VIDEO_FMT_1080P_50 == enHdFmt)
    {
        VidCode = 31;
        bVideoMode = 0x18;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_1080P_50\n");
    }
    else if(HI_HDMI_VIDEO_FMT_1080P_30 == enHdFmt)
    {
        VidCode = 34;
        bVideoMode = 0x1b;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_1080P_30\n");
    }
    else if(HI_HDMI_VIDEO_FMT_1080P_25 == enHdFmt)
    {
        VidCode = 33;
        bVideoMode = 0x1a;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_1080P_25\n");
    }
    else if(HI_HDMI_VIDEO_FMT_1080P_24 == enHdFmt)
    {
        VidCode = 32;
        bVideoMode = 0x19;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_1080P_24\n");
    }
    else if(HI_HDMI_VIDEO_FMT_1080i_60 == enHdFmt)
    {
        VidCode = 5;
        bVideoMode = 0x03;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_1080i_60\n");
    }
    else if(HI_HDMI_VIDEO_FMT_1080i_50 == enHdFmt)
    {
        VidCode = 20;
        bVideoMode = 0x0e;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_1080i_50\n");
    }
    else if(HI_HDMI_VIDEO_FMT_720P_60 == enHdFmt)
    {
        VidCode = 4;
        bVideoMode = 0x02;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_720P_60\n");
    }
    else if(HI_HDMI_VIDEO_FMT_720P_50 == enHdFmt)
    {
        VidCode = 19;
        bVideoMode = 0x0d;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_720P_50\n");
    }
    else if(HI_HDMI_VIDEO_FMT_576P_50 == enHdFmt)
    {
        enColorimetry = HDMI_COLORIMETRY_ITU601;
        enAspectRate  = HI_HDMI_ASPECT_RATIO_4TO3;
        VidCode = 17;
        bVideoMode = 0x0c;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_576P_50\n");
    }
    else if(HI_HDMI_VIDEO_FMT_480P_60 == enHdFmt)
    {
        enColorimetry = HDMI_COLORIMETRY_ITU601;
        enAspectRate  = HI_HDMI_ASPECT_RATIO_4TO3;
        VidCode = 2;
        bVideoMode = 0x01;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_480P_60\n");
    }
    else if(HI_HDMI_VIDEO_FMT_PAL == enHdFmt)
    {
        enColorimetry = HDMI_COLORIMETRY_ITU601;
        enAspectRate  = HI_HDMI_ASPECT_RATIO_4TO3;
        u32PixelRepetition = HI_TRUE;
        VidCode = 21;
        bVideoMode = 0x0f;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_PAL\n");
    }
    else if(HI_HDMI_VIDEO_FMT_NTSC == enHdFmt)
    {
        enColorimetry = HDMI_COLORIMETRY_ITU601;
        enAspectRate  = HI_HDMI_ASPECT_RATIO_4TO3;
        u32PixelRepetition = HI_TRUE;
        VidCode = 0x06;
        bVideoMode = 0x04;
        HDMIPrint("HDMI input format is HI_HDMI_VIDEO_FMT_NTSC\n");
    }
    // for surpport VGA format bootlogo
    else if(HI_HDMI_VIDEO_FMT_861D_640X480_60 == enHdFmt)
    {
        HDMIPrint("Set 640X480P_60000 enTimingMode\n");
        enColorimetry      = HDMI_COLORIMETRY_ITU601;
        enAspectRate       = HI_HDMI_ASPECT_RATIO_4TO3;
        u32PixelRepetition = HI_FALSE;

    }
    else if ((HI_HDMI_VIDEO_FMT_VESA_800X600_60 <= enHdFmt) && (HI_HDMI_VIDEO_FMT_VESA_2048X1152_60 >= enHdFmt))
    {
#if 0   /* 3716´úÂë */
		hdmi_dvi_mode = 0;
        HDMIPrint("DVI timing mode enTimingMode\n");
        enColorimetry      = HDMI_COLORIMETRY_ITU601;
        enAspectRate       = HI_HDMI_ASPECT_RATIO_4TO3;
        u32PixelRepetition = HI_FALSE; 
#else   /* 3531´úÂë */
        enColorimetry      = HDMI_COLORIMETRY_ITU601;
        enAspectRate       = HI_HDMI_ASPECT_RATIO_4TO3;
        u32PixelRepetition = HI_FALSE; 
#endif	        
    }
    //OpenHdmiDevice();
    
    WriteDefaultConfigToEEPROM(); /* eeprom.c */
    
    HW_ResetHDMITX();
    SW_ResetHDMITX();
    
    UpdateTX_656(bVideoMode);
    SetIClk( ReadByteEEPROM(EE_TX_ICLK_ADDR) );
    
    WakeUpHDMITX();
            
    /* Set Video Path */
    HDMIPrint("einput:%d, eoutput:%d\n", einput, eoutput);
    if(HI_HDMI_VIDEO_MODE_RGB444 == einput)
    {
        bRxVideoMode = 0;  /* inRGB24[] */
        if(HI_HDMI_VIDEO_MODE_RGB444 == eoutput)
        {
            bTxVideoMode = 0;
        }
        else
        {
            HDMIPrint("Error output mode when input RGB444\n");
            bTxVideoMode = 0;
        }
    }
    else if(HI_HDMI_VIDEO_MODE_YCBCR444 == einput)
    {
        bRxVideoMode = 2; /* inYCbCr24[] */
        if(HI_HDMI_VIDEO_MODE_RGB444 == eoutput)
        {
            bTxVideoMode = 0;
        }
        else if(HI_HDMI_VIDEO_MODE_YCBCR444 == eoutput)
        {
            bTxVideoMode = 1;
        }
        else
        {
            HDMIPrint("Error output mode when input YCbCr444\n");
            bTxVideoMode = 0;
        }
    }
    else if(HI_HDMI_VIDEO_MODE_YCBCR422 == einput)
    {
        bRxVideoMode = 3; /* inYC24[] */
        if(HI_HDMI_VIDEO_MODE_RGB444 == eoutput)
        {
            bTxVideoMode = 0;
        }
        else if(HI_HDMI_VIDEO_MODE_YCBCR444 == eoutput)
        {
            bTxVideoMode = 1;
        }
        else
        {
            bTxVideoMode = 2;
        }
    }
    u8VideoPath[0] = bRxVideoMode;
    u8VideoPath[1] = bTxVideoMode;
    u8VideoPath[2] = 0x00;
    u8VideoPath[3] = 0xFF;
    
    siiSetVideoPath(bVideoMode, u8VideoPath);
   
    /* HDMI AVI Infoframe is use Version = 0x02 in HDMI1.3 */
    /* Fill Data Byte 1 */
    u8AviInfoFrameByte=0;
    /* Scan information bits 0-1:S0,S1 */
    /*
           S1 S0 Scan Information
           0   0    No Data
           0   1   overscanned
           1   0   underscanned
           1   1   Future
    */
    u8AviInfoFrameByte |= (HI_U8)0x01;
    /* Bar Information bits 2-3:B0,B1 */
    /*
         B1 B0  Bar Info
         0   0  not valid
         0   1  Vert. Bar Info valid
         1   0  Horiz.Bar Info Valid
         1   1  Vert. and Horiz. Bar Info valid
    */
    u8AviInfoFrameByte |= (HI_U8) 0x00;
    /* Active information bit 4:A0 */
    /*
         A0 Active Format Information Present
         0        No Data
         1      Active Format(R0¡­R3) Information valid
    */
    u8AviInfoFrameByte |= (HI_U8)0x10;
    /* Output Type bits 5-6:Y0,Y1 */
    /*
         Y1 Y0  RGB orYCbCr
         0  0   RGB (default)
         0  1   YCbCr 4:2:2
         1  0   YCbCr 4:4:4
         1  1    Future
    */
    switch (eoutput)
    {
        case HI_HDMI_VIDEO_MODE_RGB444 :
            u8AviInfoFrameByte |= (HI_U8)0x00;
            break;
        case HI_HDMI_VIDEO_MODE_YCBCR422 :
            u8AviInfoFrameByte |= (HI_U8)0x20;
            break;
        case HI_HDMI_VIDEO_MODE_YCBCR444 :
            u8AviInfoFrameByte |= (HI_U8)0x40;
            break;
        default :
            HDMIPrint("Error Output format *******\n");            
            retval = HI_FAILURE;
            break;
    }
    pu8AviInfoFrame[0]= (HI_U8)(u8AviInfoFrameByte&0x7F);
    
    /* Fill Data byte 2 */
    u8AviInfoFrameByte=0;
    /* Active Format aspect ratio bits 0-3:R0...R3 */
    /*
        R3 R2 R1 R0  Active Format Aspect Ratio
        1  0  0  0   Same as picture aspect ratio
        1  0  0  1   4:3 (Center)
        1  0  1  0   16:9 (Center)
        1  0  1  1   14:9 (Center)
    */
#if 0    
    switch (enAspectRate)
    {
        case HI_HDMI_ASPECT_RATIO_4TO3 :
            u8AviInfoFrameByte |= (HI_U8) 0x09;
            break;
        case HI_HDMI_ASPECT_RATIO_16TO9 :
            u8AviInfoFrameByte |= (HI_U8) 0x0A;
            break;
        case HI_HDMI_ASPECT_RATIO_14TO9:
            u8AviInfoFrameByte |= (HI_U8) 0x0B;
            break;
        default :
            u8AviInfoFrameByte |= (HI_U8) 0x08;
            break;
     }
#else
    HDMIPrint("Active Format aspect ratio  set to 0x1000:Same as picture aspect ratio\n");
    u8AviInfoFrameByte |= (HI_U8) 0x08;
#endif
    /* Picture aspect ratio bits 4-5:M0.M1 */
    /*
        M1 M0     Picture Aspect Ratio
        0  0        No Data
        0  1        4:3
        1  0        16:9
        1  1        Future
    */
    switch (enAspectRate)
    {

        case HI_HDMI_ASPECT_RATIO_4TO3 :
             u8AviInfoFrameByte |= (HI_U8) 0x10;
             break;
        case HI_HDMI_ASPECT_RATIO_16TO9 :
             u8AviInfoFrameByte |= (HI_U8) 0x20;
                break;
            default :
             u8AviInfoFrameByte |=  (HI_U8) 0x00;
             break;
     }

    /* Colorimetry bits 6-7 of data byte2:C0,C1 */
    /*
        C1 C0    Colorim
        0   0    No Data
        0   1    SMPTE 170M[1] ITU601 [5]
        1   0    ITU709 [6] 1 0 16:9
        1   1    Extended Colorimetry Information Valid (colorimetry indicated in bits EC0, EC1,
                     EC2. See Table 11)
    */
    switch (enColorimetry)
    {
        case HDMI_COLORIMETRY_ITU601 :
            u8AviInfoFrameByte |= (HI_U8)0x40;
            break;
        case HDMI_COLORIMETRY_ITU709 :
            u8AviInfoFrameByte |= (HI_U8)0x80;
            break;
        case HDMI_COLORIMETRY_XVYCC_601 :
        case HDMI_COLORIMETRY_XVYCC_709 :
        case HDMI_COLORIMETRY_EXTENDED :
            u8AviInfoFrameByte |= (HI_U8)0xC0;
            break;
        default :
            u8AviInfoFrameByte |= (HI_U8)0x00;
            break;
    }
    pu8AviInfoFrame[1] = (HI_U8)(u8AviInfoFrameByte&0XFF);

    /* Fill data Byte 3: Picture Scaling bits 0-1:SC0,SC1 */
    u8AviInfoFrameByte=0;
    /*
       SC1  SC0   Non-Uniform Picture Scaling
       0     0    No Known non-uniform Scaling
       0     1    Picture has been scaled horizontally
       1     0    Picture has been scaled vertically
       1     1    Picture has been scaled horizontally and vertically
    */
    u8AviInfoFrameByte |= (HI_U8)0x00;
    /* Fill data Byte 3: RGB quantization range bits 2-3:Q0,Q1 */
    /*
        Q1  Q0  RGB Quantization Range
        0   0   Default (depends on video format)
        0   1   Limited Range
        1   0   Full Range
        1   1   Reserved
    */
    u8AviInfoFrameByte |= (HI_U8)0x00;
    /* Fill data Byte 3: Extended colorimtery range bits 4-6:EC0,EC1,EC2 */
    /*
        EC2 EC1 EC0   Extended Colorimetry
        0   0   0      xvYCC601
        0   0   1      xvYCC709
        -   -   -      All other values reserved
    */
    /*
        xvYCC601 is based on the colorimetry defined in ITU-R BT.601. 
        xvYCC709 is based on the colorimetry defined in ITU-R BT.709.
    */
     switch (enColorimetry)
    {
        case HDMI_COLORIMETRY_XVYCC_601 :
            u8AviInfoFrameByte |= (HI_U8)0x00;
            break;
        case HDMI_COLORIMETRY_XVYCC_709 :
            u8AviInfoFrameByte |= (HI_U8)0x10;
            break;
        default:
            break;
    }
    /* Fill data Byte 3: IT content bit 7:ITC */
    /*
        ITC  IT content
        0    No data
        1    IT content
    */

    u8AviInfoFrameByte &= ~0x80;
    pu8AviInfoFrame[2] = (HI_U8)(u8AviInfoFrameByte&0XFF);
    
    /* Fill Data byte 4: Video indentification data Code, Bit0~7:VIC0 ~ VIC6 */
    u8AviInfoFrameByte=0;
    pu8AviInfoFrame[3] = (HI_U8)(VidCode & 0x7F);

    /* Fill Data byte 5: Pixel repetition, Bit0~3:PR0~PR3 */
    /*
        PR3 PR2 PR1 PR0 Pixel Repetition Factor
        0   0   0    0   No Repetition (i.e., pixel sent once)
        0   0   0    1   pixel sent 2 times (i.e., repeated once)
        0   0   1    0   pixel sent 3 times
        0   0   1    1   pixel sent 4 times
        0   1   0    0   pixel sent 5 times
        0   1   0    1   pixel sent 6 times
        0   1   1    0   pixel sent 7 times
        0   1   1    1   pixel sent 8 times
        1   0   0    0   pixel sent 9 times
        1   0   0    1   pixel sent 10 times
        0Ah-0Fh          Reserved
    */
    pu8AviInfoFrame[4]= (HI_U8)(u32PixelRepetition& 0x0F);
    
     /* Fill Data byte 6  */
    pu8AviInfoFrame[5] = 0x00;

    /* Fill Data byte 7  */
    pu8AviInfoFrame[6] = 0x00;

    /* Fill Data byte 8  */
    pu8AviInfoFrame[7] = 0x00;

    /* Fill Data byte 9  */
    pu8AviInfoFrame[8] = 0x00;

    /* Fill Data byte 10  */
    pu8AviInfoFrame[9] = 0x00;

    /* Fill Data byte 11  */
    pu8AviInfoFrame[10] = 0x00;

    /* Fill Data byte 12  */
    pu8AviInfoFrame[11] = 0x00;

    /* Fill Data byte 13  */
    pu8AviInfoFrame[12] = 0x00;

    /*
    720X480P_60000  :0x51,0x59,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    720X576P_50000  :0x51,0x59,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1280X720P_50000 :0x51,0xaa,0x00,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1280X720P_60000 :0x51,0xaa,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1920X1080i_50000:0x51,0xaa,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1920X1080i_60000:0x51,0xaa,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1920X1080P_24000:0x51,0xaa,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1920X1080P_25000:0x51,0xaa,0x00,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1920X1080P_30000:0x51,0xaa,0x00,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1920X1080P_50000:0x51,0xaa,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    1920X1080P_60000:0x51,0xaa,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    */
    
#if 1
    HDMIPrint("The Content of the AVI version2 info frame is:\n");
    for (Index=0; Index<17; Index++)
    {
        HDMIPrint("0x%02x,",pu8AviInfoFrame[Index]);
    }
    HDMIPrint("\n");
#endif   


    BlockWriteEEPROM( 13, EE_TXAVIINFO_ADDR, pu8AviInfoFrame);
    if(hdmi_dvi_mode){
	    SendAVIInfoFrame();
	    EnableInfoFrame(AVI_TYPE);
    }
    
    TX_SetHDMIMode(hdmi_dvi_mode);    //for hdmi 
    
    //SendCP_Packet(OFF);
{    
    HI_U8 RegVal;
    WriteByteHDMITXP1( CP_IF_ADDR, BIT_CP_AVI_MUTE_CLEAR);
    RegVal = ReadByteHDMITXP1( INF_CTRL2 );
    WriteByteHDMITXP1(INF_CTRL2, RegVal | BIT_CP_ENABLE | BIT_CP_REPEAT);
}    

    //SetDeepColor(0x02);
    return HI_SUCCESS;    
}

