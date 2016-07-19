
#ifndef HDMI_BUILD_IN_BOOT
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/stddef.h>
#include "drv_hdmi_intf.h"
#else
#include "boot_hdmi_intf.h"
#endif
#include "drv_hdmi_infoframe.h"
#include "drv_hdmi_common.h"

#ifdef HDMI_BUILD_IN_BOOT
#include "boot_hdmi_intf.h"
#endif

#define CHECK_POINTER(p)

typedef enum 
{
    HDMI_VIDEO_SAMPLE_TYPE_UNKNOWN,                 
    HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE,             
    HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,                
    HDMI_VIDEO_SAMPLE_TYPE_BUTT
}HDMI_VIDEO_SAMPLE_TYPE_E;

typedef struct
{
    HDMI_VIDEO_CODE_E           enVideoCode;
    HDMI_VIDEO_TIMING_E         enTiming;
    HDMI_VIDEO_SAMPLE_TYPE_E    enScanType;
    HDMI_PICTURE_ASPECT         enAspectRatio;
}HDMI_VIDEO_IDENTIFICATION_S;

static HDMI_VIDEO_IDENTIFICATION_S s_stCEA861FVideoCodes[] = 
{
    /* {Video Identification Code, Timing Mode, Scan Type, Aspect Ratio} */
    {0,                       HDMI_VIDEO_TIMING_UNKNOWN,           HDMI_VIDEO_SAMPLE_TYPE_UNKNOWN,     HDMI_PICTURE_ASPECT_NONE},
    //{HDMI_640X480P60_4_3,     HDMI_VIDEO_TIMING_640X480P_59940,    HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3},
    {HDMI_640X480P60_4_3,     HDMI_VIDEO_TIMING_640X480P_60000,    HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {HDMI_720X480P60_4_3,     HDMI_VIDEO_TIMING_720X480P_60000,    HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3},
    {HDMI_720X480P60_16_9,    HDMI_VIDEO_TIMING_720X480P_60000,    HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9},
    {HDMI_1280X720P60_4_3,    HDMI_VIDEO_TIMING_1280X720P_60000,   HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {HDMI_1280X720P60_16_9,   HDMI_VIDEO_TIMING_1280X720P_60000,   HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_1280X720P50_4_3,    HDMI_VIDEO_TIMING_1280X720P_50000,   HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {HDMI_1280X720P50_16_9,   HDMI_VIDEO_TIMING_1280X720P_50000,   HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_720X576P50_4_3,     HDMI_VIDEO_TIMING_720X576P_50000,    HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {HDMI_720X576P50_16_9,    HDMI_VIDEO_TIMING_720X576P_50000,    HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_1920X1080P24_16_9,  HDMI_VIDEO_TIMING_1920X1080P_24000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_1920X1080P25_16_9,  HDMI_VIDEO_TIMING_1920X1080P_25000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_1920X1080P30_16_9,  HDMI_VIDEO_TIMING_1920X1080P_30000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_1920X1080P50_16_9,  HDMI_VIDEO_TIMING_1920X1080P_50000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_1920X1080P60_16_9,  HDMI_VIDEO_TIMING_1920X1080P_60000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {0,                       HDMI_VIDEO_TIMING_3840X2160P_24000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {0,                       HDMI_VIDEO_TIMING_3840X2160P_24000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {0,                       HDMI_VIDEO_TIMING_3840X2160P_25000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {0,                       HDMI_VIDEO_TIMING_3840X2160P_25000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {0,                       HDMI_VIDEO_TIMING_3840X2160P_30000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {0,                       HDMI_VIDEO_TIMING_3840X2160P_30000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_3840X2160P50_4_3,   HDMI_VIDEO_TIMING_3840X2160P_50000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {HDMI_3840X2160P50_16_9,  HDMI_VIDEO_TIMING_3840X2160P_50000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_3840X2160P60_4_3,   HDMI_VIDEO_TIMING_3840X2160P_60000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_4_3}, 
    {HDMI_3840X2160P60_16_9,  HDMI_VIDEO_TIMING_3840X2160P_60000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {HDMI_1440X480I60_4_3,    HDMI_VIDEO_TIMING_720X480I_60000,    HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_4_3},
    {HDMI_1440X480I60_16_9,   HDMI_VIDEO_TIMING_720X480I_60000,    HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_16_9},
    {HDMI_1440X576I50_4_3,    HDMI_VIDEO_TIMING_720X576I_50000,    HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_4_3},
    {HDMI_1440X576I50_16_9,   HDMI_VIDEO_TIMING_720X576I_50000,    HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_16_9},
    //{HDMI_1920X1080I50_4_3,   HDMI_VIDEO_TIMING_1920X1080I_50000,  HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_4_3},
    {HDMI_1920X1080I50_16_9,  HDMI_VIDEO_TIMING_1920X1080I_50000,  HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_16_9},
    //{HDMI_1920X1080I60_4_3,   HDMI_VIDEO_TIMING_1920X1080I_60000,  HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_4_3},
    {HDMI_1920X1080I60_16_9,  HDMI_VIDEO_TIMING_1920X1080I_60000,  HDMI_VIDEO_SAMPLE_TYPE_INTERLACE,   HDMI_PICTURE_ASPECT_16_9},

};

static HDMI_VIDEO_IDENTIFICATION_S s_stH14bHdmiVideoCodes[4] = 
{
    {4,  HDMI_VIDEO_TIMING_4096X2160P_24000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {3,  HDMI_VIDEO_TIMING_3840X2160P_24000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {2,  HDMI_VIDEO_TIMING_3840X2160P_25000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 
    {1,  HDMI_VIDEO_TIMING_3840X2160P_30000,  HDMI_VIDEO_SAMPLE_TYPE_PROGRESSIVE, HDMI_PICTURE_ASPECT_16_9}, 

};

HDMI_VIDEO_TIMING_E DRV_HDMI_VideoTimingLookup(HDMI_VIDEO_CODE_E enVic, HDMI_PICTURE_ASPECT enAspect, HI_BOOL bH14bVic)
{
    HI_U32 u32Cnt,u32VICSize;
    HDMI_VIDEO_TIMING_E enVideoTiming = 0;
    
    if (!bH14bVic)  
    {
        u32VICSize = sizeof(s_stCEA861FVideoCodes)/sizeof(s_stCEA861FVideoCodes[0]);
        for (u32Cnt = 0; u32Cnt < u32VICSize; u32Cnt++)
        {
            if (s_stCEA861FVideoCodes[u32Cnt].enVideoCode == enVic &&
                s_stCEA861FVideoCodes[u32Cnt].enAspectRatio == enAspect)
            {
                enVideoTiming = s_stCEA861FVideoCodes[u32Cnt].enTiming;
                break;
            }
        }
    }
    else
    {
        for (u32Cnt = 0; u32Cnt < 4; u32Cnt++)
        {
            if (s_stH14bHdmiVideoCodes[u32Cnt].enVideoCode == enVic)
            {
                enVideoTiming = s_stH14bHdmiVideoCodes[u32Cnt].enTiming;
                break;
            }
        }  
    }

    return enVideoTiming;
    
}

static HI_VOID hdmi_infoframe_checksum(HI_VOID *buffer, HI_U32 size)
{
	HI_U8 *ptr = buffer;
	HI_U8 csum = 0;
	HI_U32 i;

	/* compute checksum */
	for (i = 0; i < size; i++)
	{
		csum += ptr[i];
    }
    if (0 == csum)
    {
        ptr[3] = 0xff;
    }
    else if ((0 < csum) && (csum <= 255))
    {
        ptr[3] = 256 - csum;
    }
    else
    {
        ptr[3] = 0;
    }
}
#if 0
/**
 * hdmi_avi_infoframe_init() - initialize an HDMI AVI infoframe
 * @frame: HDMI AVI infoframe
 *
 * Returns 0 on success or a negative error code on failure.
 */
static HI_S32 hdmi_avi_infoframe_init(struct hdmi_avi_infoframe *frame)
{
	memset(frame, 0, sizeof(*frame));

	frame->type = HDMI_INFOFRAME_TYPE_AVI;
	frame->version = 2;
	frame->length = 13;

	return 0;
}
#endif

/**
 * hdmi_avi_infoframe_pack() - write HDMI AVI infoframe to binary buffer
 * @frame: HDMI AVI infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 *
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 *
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
static HI_S32 hdmi_avi_infoframe_pack(struct hdmi_avi_infoframe *frame, HI_VOID *buffer,
				HI_U32 size)
{
	HI_U8 *ptr = buffer;
	HI_U32 length;
    HI_U32 u32Cnt,u32VICSize;
    enum hdmi_video_code video_code = 0;
    
	length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

	if (size < length)
		return -1;

    u32VICSize = sizeof(s_stCEA861FVideoCodes)/sizeof(s_stCEA861FVideoCodes[0]);
    for (u32Cnt = 0; u32Cnt < u32VICSize; u32Cnt++)
    {
        if (s_stCEA861FVideoCodes[u32Cnt].enTiming == frame->video_timing&&
            s_stCEA861FVideoCodes[u32Cnt].enAspectRatio == frame->picture_aspect)
        {
            video_code = s_stCEA861FVideoCodes[u32Cnt].enVideoCode;
            break;
        }
    }

	memset(buffer, 0, length);

	ptr[0] = frame->type;
	ptr[1] = frame->version;
	ptr[2] = frame->length;
	ptr[3] = 0; /* checksum */

	/* start infoframe payload */
	ptr += HDMI_INFOFRAME_HEADER_SIZE;

	ptr[0] = ((frame->colorspace & 0x3) << 5) | (frame->scan_mode & 0x3);

	if (frame->active_info_valid)
		ptr[0] |= SET_BIT(4);

	if (frame->horizontal_bar_valid)
		ptr[0] |= SET_BIT(3);

	if (frame->vertical_bar_valid)
		ptr[0] |= SET_BIT(2);

	ptr[1] = ((frame->colorimetry & 0x3) << 6) |
		 ((frame->picture_aspect & 0x3) << 4) |
		 (frame->active_aspect & 0xf);

	ptr[2] = ((frame->extended_colorimetry & 0x7) << 4) |
		 ((frame->quantization_range & 0x3) << 2) |
		 (frame->nups & 0x3);

	if (frame->itc)
		ptr[2] |= SET_BIT(7);

	ptr[3] = /*frame->video_code*/video_code & 0x7f;

	ptr[4] = ((frame->ycc_quantization_range & 0x3) << 6) |
		 ((frame->content_type & 0x3) << 4) |
		 ((frame->pixel_repeat - 1) & 0xf);

	ptr[5] = frame->top_bar & 0xff;
	ptr[6] = (frame->top_bar >> 8) & 0xff;
	ptr[7] = frame->bottom_bar & 0xff;
	ptr[8] = (frame->bottom_bar >> 8) & 0xff;
	ptr[9] = frame->left_bar & 0xff;
	ptr[10] = (frame->left_bar >> 8) & 0xff;
	ptr[11] = frame->right_bar & 0xff;
	ptr[12] = (frame->right_bar >> 8) & 0xff;

	hdmi_infoframe_checksum(buffer, length);

	return length;
}

#if 0
/**
 * hdmi_audio_infoframe_init() - initialize an HDMI audio infoframe
 * @frame: HDMI audio infoframe
 *
 * Returns 0 on success or a negative error code on failure.
 */
static HI_S32 hdmi_audio_infoframe_init(struct hdmi_audio_infoframe *frame)
{
	memset(frame, 0, sizeof(*frame));

	frame->type = HDMI_INFOFRAME_TYPE_AUDIO;
	frame->version = 1;
	frame->length = 10;

	return 0;
}
#endif

/**
 * hdmi_audio_infoframe_pack() - write HDMI audio infoframe to binary buffer
 * @frame: HDMI audio infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 *
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 *
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
static HI_S32 hdmi_audio_infoframe_pack(struct hdmi_audio_infoframe *frame,
				  HI_VOID *buffer, HI_U32 size)
{
	//HI_U8 channels;
	HI_U8 *ptr = buffer;
	HI_U32 length;
    

	length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

	if (size < length)
		return -1;

	memset(buffer, 0, length);

	//if (frame->channels >= 2)
	//	channels = frame->channels - 1;
	//else
	//	channels = 0;

	ptr[0] = frame->type;
	ptr[1] = frame->version;
	ptr[2] = frame->length;
	ptr[3] = 0; /* checksum */

	/* start infoframe payload */
	ptr += HDMI_INFOFRAME_HEADER_SIZE;

	ptr[0] = ((frame->coding_type & 0xf) << 4) | (frame->channels & 0x7);
	ptr[1] = ((frame->sample_frequency & 0x7) << 2) |
		 (frame->sample_size & 0x3);
	ptr[2] = frame->coding_type_ext & 0x1f;
	ptr[3] = frame->channel_allocation;
	ptr[4] = (frame->level_shift_value & 0xf) << 3;
	ptr[4] |= frame->lfe_playback_level & 0x3;

	if (frame->downmix_inhibit)
		ptr[4] |= SET_BIT(7);

	hdmi_infoframe_checksum(buffer, length);

	return length;
}
#if 0
static HI_S32 hdmi_vendor_specific_infoframe_init(struct hdmi_vendor_specific_infoframe *frame)
{
	memset(frame, 0, sizeof(*frame));

	frame->type = HDMI_INFOFRAME_TYPE_VENDOR;
	frame->version = 1;
	frame->length = 7;

	return 0;
}
#endif
/**
 * hdmi_vendor_infoframe_pack() - write a HDMI vendor infoframe to binary
 *                                buffer
 * @frame: HDMI vendor infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 *
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 *
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
static HI_S32 hdmi_vendor_specific_infoframe_pack(struct hdmi_vendor_specific_infoframe *frame,
				   HI_VOID *buffer, HI_U32 size)
{
	HI_U8 *ptr = buffer;
	HI_U32 length;

	length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

	if (size < length)
		return -1;

	memset(buffer, 0, length);

	ptr[0] = frame->type;
	ptr[1] = frame->version;
	ptr[2] = frame->length;
	ptr[3] = 0; /* checksum */

    if (frame->vsif_content.h14_vsif_content.ieee == HDMI14_IEEE_OUI)//for HDMI 1.4
    {
        hdmi_14_vsif_content *vsif_content = &frame->vsif_content.h14_vsif_content;

        /* IEEE OUI*/
        ptr[4] = HDMI14_IEEE_OUI & 0xff;
        ptr[5] = (HDMI14_IEEE_OUI >> 8) & 0xff;
        ptr[6] = (HDMI14_IEEE_OUI >> 16) & 0xff;

        /* hdmi video format*/
        ptr[7] = (vsif_content->format & 0x7) << 5;
        
        if (vsif_content->format == HDMI_VIDEO_FORMAT_4K)
        {
           ptr[8] = vsif_content->vic;  
        }
        else if (vsif_content->format == HDMI_VIDEO_FORMAT_3D)
        {
           ptr[8] = (vsif_content->_3d_structure & 0xf) << 4;
        }
    }
    else if (frame->vsif_content.hf_vsif_content.ieee == HDMI20_IEEE_OUI)//for HDMI 2.0,to be define
    {

    }

	hdmi_infoframe_checksum(buffer, length);

	return length;
}

static HI_S32 hdmi_gamut_metadata_pack(HDMI_GDB_PACK_S *pstGdbPack,  HI_VOID *buffer,HI_U32 size )
{
    if (size < sizeof(HDMI_GDB_PACK_S))
    {
        return HI_FAILURE;
    }

    memcpy(buffer, pstGdbPack, sizeof(HDMI_GDB_PACK_S));

    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_InfoFrameSend(HDMI_INFOFRAME_S* pstInfoFrame, HDMI_INFOFRAME_ID_E enInfoFrameId, HDMI_INFOFRAME_U* uInfoframe)
{
    HI_U8 buffer[32];
    //HI_S32 len;
    HDMI_DEVICE_S*  pstHdmiDev;
    CHECK_POINTER(pstInfoFrame);
    
    pstHdmiDev = container_of(pstInfoFrame, typeof(*pstHdmiDev), stInfoFrame); 
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);
    
    memset(buffer, 0, sizeof(buffer));

    /* pack infoframe*/
    switch (enInfoFrameId)
    {
        case HDMI_INFOFRAME_TYPE_VENDOR:
        {
        	uInfoframe->stVendorInfoFrame.type    = HDMI_INFOFRAME_TYPE_VENDOR;
        	uInfoframe->stVendorInfoFrame.version = 1;
        	uInfoframe->stVendorInfoFrame.length  = 7;            
            hdmi_vendor_specific_infoframe_pack(&uInfoframe->stVendorInfoFrame, buffer, 32);
            break;
        }
        case HDMI_INFOFRAME_TYPE_AVI:
        {
            uInfoframe->stAVIInfoFrame.type      = HDMI_INFOFRAME_TYPE_AVI;
	        uInfoframe->stAVIInfoFrame.version   = 2;
	        uInfoframe->stAVIInfoFrame.length    = 13;
            hdmi_avi_infoframe_pack(&uInfoframe->stAVIInfoFrame, buffer, 32);
            break;
        }
        case HDMI_INFOFRAME_TYPE_AUDIO:
        {
            uInfoframe->stAudioInfoFrame.type    = HDMI_INFOFRAME_TYPE_AUDIO;
        	uInfoframe->stAudioInfoFrame.version = 1;
        	uInfoframe->stAudioInfoFrame.length  = 10; 
            hdmi_audio_infoframe_pack(&uInfoframe->stAudioInfoFrame, buffer, 32);
            break;
        }

        case HDMI_INFOFRAME_TYPE_GBD:
        {   	            
            hdmi_gamut_metadata_pack(&uInfoframe->stGDBPack, buffer, 32);
            break;
        }
        
        default:
            return HI_FAILURE;           
    }
    
    DRV_HDMI_InfoFrameEnableSet(pstInfoFrame, enInfoFrameId, HI_FALSE);
    pstHdmiDev->pstHdmiHal->HAL_HDMI_InfoframeSet(pstHdmiDev->pstHdmiHal, enInfoFrameId, buffer);
    DRV_HDMI_InfoFrameEnableSet(pstInfoFrame, enInfoFrameId, HI_TRUE); 

    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_InfoFrameEnableSet(HDMI_INFOFRAME_S* pstInfoFrame, HDMI_INFOFRAME_ID_E enInfoFrameId, HI_BOOL bEnable)
{
    HDMI_DEVICE_S*  pstHdmiDev;
    CHECK_POINTER(pstInfoFrame);
    
    pstHdmiDev = container_of(pstInfoFrame, typeof(*pstHdmiDev), stInfoFrame); 
    CHECK_POINTER(pstHdmiDev);
    CHECK_POINTER(pstHdmiDev->pstHdmiHal);

    pstHdmiDev->pstHdmiHal->HAL_HDMI_InfoframeEnableSet(pstHdmiDev->pstHdmiHal, enInfoFrameId, bEnable);
    return HI_SUCCESS;
}


HI_S32 DRV_HDMI_AVIInfoFrameSend(HDMI_INFOFRAME_S* pstInfoFrame, HI_BOOL bEnable)
{
    HDMI_INFOFRAME_U         uInfoframe;
    HDMI_AVI_INFOFRAME_S*    pstAVIInfoframe;
    HDMI_VO_ATTR_S*          pstVOAttr;
    HDMI_APP_ATTR_S*         pstAppAttr;
    HDMI_DEVICE_S*           pstHdmiDev;
    //HI_U32                   u32Cnt,u32VICSize;
    CHECK_POINTER(pstInfoFrame);
    
    if (!bEnable)
    {
        DRV_HDMI_InfoFrameEnableSet(pstInfoFrame, HDMI_INFOFRAME_TYPE_AVI, HI_FALSE); 
        return HI_SUCCESS;
    }
    
    pstHdmiDev = container_of(pstInfoFrame, typeof(*pstHdmiDev), stInfoFrame);  
    CHECK_POINTER(pstHdmiDev);

    pstVOAttr  = &pstHdmiDev->stAttr.stVOAttr;
    pstAppAttr = &pstHdmiDev->stAttr.stAppAttr;
    pstAVIInfoframe = &pstInfoFrame->stAVIInfoFrame;
    memset(pstAVIInfoframe, 0, sizeof(HDMI_AVI_INFOFRAME_S));
    //hdmi_avi_infoframe_init(pstAVIInfoframe);
    pstAVIInfoframe->disp_fmt               = pstVOAttr->u32DispFmt;
    pstAVIInfoframe->video_timing           = pstVOAttr->enVideoTiming;
    pstAVIInfoframe->colorimetry            = pstAppAttr->bxvYCCMode ? HDMI_COLORIMETRY__EXTENDED : pstVOAttr->enColorimetry; 

    pstAVIInfoframe->extended_colorimetry   = pstVOAttr->enExtendedColorimetry;
    pstAVIInfoframe->picture_aspect         = pstVOAttr->enPictureAspect;
    pstAVIInfoframe->active_aspect          = pstVOAttr->enActiveAspect;
    pstAVIInfoframe->quantization_range     = pstVOAttr->enRGBQuantization;
    pstAVIInfoframe->ycc_quantization_range = pstVOAttr->enYCCQuantization;
    pstAVIInfoframe->nups                   = pstVOAttr->enPictureScaling;
    pstAVIInfoframe->pixel_repeat           = pstVOAttr->u32PixelRepeat;
    pstAVIInfoframe->colorspace             = pstAppAttr->bYUV420Enable ? HDMI_COLORSPACE_YCbCr420 : pstAppAttr->enOutColorSpace;
    pstAVIInfoframe->active_info_valid      = HI_TRUE;
    pstAVIInfoframe->horizontal_bar_valid   = HI_FALSE;
    pstAVIInfoframe->vertical_bar_valid     = HI_FALSE;
    pstAVIInfoframe->scan_mode              = HDMI_SCAN_MODE_NONE;    
    pstAVIInfoframe->itc                    = HI_FALSE;
    pstAVIInfoframe->content_type           = HDMI_CONTENT_TYPE_NONE;    
    pstAVIInfoframe->top_bar                = 0;
    pstAVIInfoframe->bottom_bar             = 0;
    pstAVIInfoframe->left_bar               = 0;
    pstAVIInfoframe->right_bar              = 0;
   
#if 0  
    HI_U32                   u32Cnt,u32VICSize;
    u32VICSize = sizeof(s_stH14bHdmiVideoCodes)/sizeof(s_stH14bHdmiVideoCodes[0]);
    for (u32Cnt = 0; u32Cnt < u32VICSize; u32Cnt++)
    {
        if (s_stH14bHdmiVideoCodes[u32Cnt].enTiming == pstVOAttr->enVideoTiming &&
            s_stH14bHdmiVideoCodes[u32Cnt].enAspectRatio == pstVOAttr->enPictureAspect)
        {
            pstAVIInfoframe->video_code = s_stH14bHdmiVideoCodes[u32Cnt].enVideoCode;
            break;
        }
    }
#endif

    memcpy(&uInfoframe.stAVIInfoFrame, pstAVIInfoframe, sizeof(HDMI_AVI_INFOFRAME_S));
    DRV_HDMI_InfoFrameSend(pstInfoFrame, HDMI_INFOFRAME_TYPE_AVI, &uInfoframe);
    
    return HI_SUCCESS;

}

HI_S32 DRV_HDMI_VendorInfoFrameSend(HDMI_INFOFRAME_S* pstInfoFrame, HI_BOOL bEnable)
{
    HDMI_INFOFRAME_U         uInfoframe;
    HDMI_VENDOR_INFOFRAME_S* pstVendorInfoframe;
    hdmi_14_vsif_content*    pstH14VsifContent;
    hdmi_forum_vsif_content* pstHFVsifContent;
    HDMI_VO_ATTR_S*          pstVOAttr;
    HI_BOOL                  bVendorInfoFrameEnable = HI_FALSE;    
    HDMI_DEVICE_S*           pstHdmiDev;
    CHECK_POINTER(pstInfoFrame);

    if (!bEnable)
    {
        DRV_HDMI_InfoFrameEnableSet(pstInfoFrame, HDMI_INFOFRAME_TYPE_VENDOR, HI_FALSE); 
        return HI_SUCCESS;
    }
    
    pstHdmiDev = container_of(pstInfoFrame, typeof(*pstHdmiDev), stInfoFrame);   
    CHECK_POINTER(pstHdmiDev);
    
    pstVOAttr  = &pstHdmiDev->stAttr.stVOAttr;
    pstVendorInfoframe = &pstInfoFrame->stVendorInfoFrame;
    memset(pstVendorInfoframe, 0, sizeof(HDMI_VENDOR_INFOFRAME_S));
    //hdmi_vendor_specific_infoframe_init(pstVendorInfoframe);
    
    pstH14VsifContent = &pstVendorInfoframe->vsif_content.h14_vsif_content;
    pstHFVsifContent  = &pstVendorInfoframe->vsif_content.hf_vsif_content; 
    
    if ((pstVOAttr->enVideoTiming == HDMI_VIDEO_TIMING_3840X2160P_24000 ||
        pstVOAttr->enVideoTiming == HDMI_VIDEO_TIMING_3840X2160P_25000 ||
        pstVOAttr->enVideoTiming == HDMI_VIDEO_TIMING_3840X2160P_30000 ||
        pstVOAttr->enVideoTiming == HDMI_VIDEO_TIMING_4096X2160P_24000) &&
        pstVOAttr->enStereoMode == HDMI_3D_BUTT)//4K2K24Hz¡¢25Hz¡¢30Hz
    {    
        HI_U32  u32Cnt;
        pstH14VsifContent->ieee   = HDMI14_IEEE_OUI;
        pstH14VsifContent->format = HDMI_VIDEO_FORMAT_4K;
        pstH14VsifContent->vic    = 0;
        for (u32Cnt = 0; u32Cnt < 4; u32Cnt++)
        {
            if (s_stH14bHdmiVideoCodes[u32Cnt].enTiming == pstVOAttr->enVideoTiming)
            {
                pstH14VsifContent->vic = s_stH14bHdmiVideoCodes[u32Cnt].enVideoCode;
                break;
            }
        }        
        bVendorInfoFrameEnable = HI_TRUE;
    }
    else if (pstVOAttr->enStereoMode != HDMI_3D_BUTT)//common 3D
    {
        pstH14VsifContent->ieee = HDMI14_IEEE_OUI;
        pstH14VsifContent->format = HDMI_VIDEO_FORMAT_3D;      
        pstH14VsifContent->_3d_structure = pstVOAttr->enStereoMode;
        bVendorInfoFrameEnable = HI_TRUE;
    }
    else if (0)//3D Dual-view,3D OSD
    {
        pstHFVsifContent->ieee = HDMI20_IEEE_OUI;
        //pstHFVsifContent->format = HDMI_VIDEO_FORMAT_3D; 
        pstHFVsifContent->version = 1;
        /***********************set**************************/

        /****************************************************/
        bVendorInfoFrameEnable = HI_TRUE;
    }
    else 
    {
        DRV_HDMI_InfoFrameEnableSet(pstInfoFrame, HDMI_INFOFRAME_TYPE_VENDOR, HI_FALSE);
    }

    if (bVendorInfoFrameEnable == HI_TRUE)
    {
        memcpy(&uInfoframe.stVendorInfoFrame, pstVendorInfoframe, sizeof(HDMI_VENDOR_INFOFRAME_S));
        DRV_HDMI_InfoFrameSend(pstInfoFrame, HDMI_INFOFRAME_TYPE_VENDOR, &uInfoframe);
    }

    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_AudioInfoFrameSend(HDMI_INFOFRAME_S* pstInfoFrame, HI_BOOL bEnable)
{
    HDMI_INFOFRAME_U         uInfoframe;
    HDMI_AUDIO_INFOFRAME_S*  pstAudioInfoframe;
    HDMI_AO_ATTR_S*          pstAOAttr;
    HDMI_DEVICE_S*           pstHdmiDev;
    CHECK_POINTER(pstInfoFrame);

    if (!bEnable)
    {
        DRV_HDMI_InfoFrameEnableSet(pstInfoFrame, HDMI_INFOFRAME_TYPE_AUDIO, HI_FALSE); 
        return HI_SUCCESS;
    }
    
    pstHdmiDev = container_of(pstInfoFrame, typeof(*pstHdmiDev), stInfoFrame); 
    CHECK_POINTER(pstHdmiDev);
   
    pstAOAttr  = &pstHdmiDev->stAttr.stAOAttr;
    pstAudioInfoframe = &pstInfoFrame->stAudioInfoFrame;
    memset(pstAudioInfoframe, 0, sizeof(HDMI_AUDIO_INFOFRAME_S));
    //hdmi_audio_infoframe_init(pstAudioInfoframe);   
    
    if(pstAOAttr->enSoundIntf == HDMI_AUDIO_INTERFACE__SPDIF || 
       pstAOAttr->enSoundIntf == HDMI_AUDIO_INTERFACE__HBRA)
    {
        HDMI_INFO("Audio channel refer to stream \n");
        pstAudioInfoframe->channels      = 0;// refer from stream head
    }
    else
    {
        HDMI_INFO("Audio channel %d \n",pstAOAttr->enChanels);
        pstAudioInfoframe->channels      = (pstAOAttr->enChanels >= 2) ? (pstAOAttr->enChanels-1) : 0 ; //PCM maybe 2 or 8;
    }

    if(pstAOAttr->enAudioCode == HDMI_AUDIO_CODING_TYPE_AC3 ||
       pstAOAttr->enAudioCode == HDMI_AUDIO_CODING_TYPE_DTS || 
       pstAOAttr->enAudioCode == HDMI_AUDIO_CODING_TYPE_EAC3 || 
       pstAOAttr->enAudioCode == HDMI_AUDIO_CODING_TYPE_DTS_HD)
    {
        HDMI_INFO("Aduio Code : %d \n",pstAOAttr->enAudioCode);
        pstAudioInfoframe->coding_type     = pstAOAttr->enAudioCode;
    }
    else
    {
        HDMI_INFO("Aduio Code : %d \n",pstAOAttr->enAudioCode);
        pstAudioInfoframe->coding_type     = HDMI_AUDIO_CODING_TYPE_STREAM;
    }

    switch(pstAOAttr->enChanels)     //HDMI channel map
    {
        case HDMI_AUDIO_FORMAT_3CH:
            pstAudioInfoframe->channel_allocation = 0x01;
            break;
        case HDMI_AUDIO_FORMAT_6CH:
            pstAudioInfoframe->channel_allocation = 0x0b;
            break;
        case HDMI_AUDIO_FORMAT_8CH:
            pstAudioInfoframe->channel_allocation = 0x13;
            break;
        default:
            pstAudioInfoframe->channel_allocation = 0x00;
            break;
    }
    
    pstAudioInfoframe->sample_size        = 0x0;// Refer to Stream Header
    pstAudioInfoframe->sample_frequency   = 0x0;// Refer to Stream Header
    pstAudioInfoframe->level_shift_value  = 0;
    pstAudioInfoframe->downmix_inhibit    = HI_FALSE;

    memcpy(&uInfoframe.stAudioInfoFrame, pstAudioInfoframe, sizeof(HDMI_AUDIO_INFOFRAME_S));
    DRV_HDMI_InfoFrameSend(pstInfoFrame, HDMI_INFOFRAME_TYPE_AUDIO, &uInfoframe);
    return HI_SUCCESS;
}

HI_S32 DRV_HDMI_GBDPackSend(HDMI_INFOFRAME_S* pstInfoFrame, HI_BOOL bEnable)
{
    HDMI_INFOFRAME_U         uInfoframe;
    HDMI_GDB_PACK_S*         pstGdbPack;
    HDMI_VO_ATTR_S*          pstVOAttr;
    HDMI_DEVICE_S*           pstHdmiDev;
    CHECK_POINTER(pstInfoFrame);
    
    if (!bEnable)
    {
        DRV_HDMI_InfoFrameEnableSet(pstInfoFrame, HDMI_INFOFRAME_TYPE_GBD, HI_FALSE); 
        return HI_SUCCESS;
    }
    
    pstHdmiDev = container_of(pstInfoFrame, typeof(*pstHdmiDev), stInfoFrame);  
    CHECK_POINTER(pstHdmiDev);

    pstVOAttr  = &pstHdmiDev->stAttr.stVOAttr;
    pstGdbPack = &pstInfoFrame->stGDBPack;
    memset(pstGdbPack, 0, sizeof(HDMI_GDB_PACK_S));

    pstGdbPack->u8HB0                                   = 0X0A;
  
    pstGdbPack->u8HB1.bits.bz_Affected_Gamut_Seq_Num    = 1;
    pstGdbPack->u8HB1.bits.bz_GBD_profile               = 0;
    pstGdbPack->u8HB1.bits.bz_Next_Field                = 1;

    pstGdbPack->u8HB2.bits.bz_Current_Gamut_Seq_Num     = 1;
    pstGdbPack->u8HB2.bits.bz_Packet_Seq                = 3;
    pstGdbPack->u8HB2.bits.bz_Reseved                   = 0;
    pstGdbPack->u8HB2.bits.bz_No_Crnt_GBD               = 0;

    pstGdbPack->u8GDB0.bits.bz_GBD_Color_Space          = (pstVOAttr->enColorimetry == HDMI_COLORIMETRY__ITU_709) ? 2 : 1;
    pstGdbPack->u8GDB0.bits.bz_GBD_Color_Precision      = 2;
    pstGdbPack->u8GDB0.bits.bz_Reseved0                 = 0;
    pstGdbPack->u8GDB0.bits.bz_Reseved1                 = 0;
    pstGdbPack->u8GDB0.bits.bz_Format_Flag              = 1;

    pstGdbPack->u8Min_Red_Data_H                        = 0x9b;
    pstGdbPack->u8Mid_Red_Data_M                        = 0x52;
    pstGdbPack->u8Max_Red_Data_L                        = 0xf4;

    pstGdbPack->u8Min_Green_Data_H                      = 0x8d;
    pstGdbPack->u8Mid_Green_Data_M                      = 0x72;
    pstGdbPack->u8Max_Green_Data_L                      = 0x96;
    
    pstGdbPack->u8Min_Blue_Data_H                       = 0x8c;
    pstGdbPack->u8Mid_Blue_Data_M                       = 0xc2;
    pstGdbPack->u8Max_Blue_Data_L                       = 0x92;
    
    memcpy(&uInfoframe.stGDBPack, pstGdbPack, sizeof(HDMI_GDB_PACK_S));
    DRV_HDMI_InfoFrameSend(pstInfoFrame, HDMI_INFOFRAME_TYPE_GBD, &uInfoframe);

    return HI_SUCCESS;
}



