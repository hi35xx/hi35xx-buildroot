#ifndef __TW2865_H__
#define __TW2865_H__

#include <linux/ioctl.h>
//#include "hi_type.h"



#define TW2865_NTSC	        1
#define TW2865_PAL	        2
#define TW2865_AUTO	        3
#define TW2865_INVALID_MODE	4

typedef struct _tw2865_video_norm
{
    unsigned int chip;
    unsigned int mode;
} tw2865_video_norm;


#define TW2865_1D1_MODE     0
#define TW2865_2D1_MODE     1
#define TW2865_4CIF_MODE    2
#define TW2865_4D1_MODE     3

typedef struct _tw2865_work_mode
{
    unsigned int chip;
    unsigned int mode;
} tw2865_work_mode;


typedef struct _tw2865_video_loss
{
    unsigned int chip;
    unsigned int ch;
    unsigned int is_lost;
} tw2865_video_loss;


#define TW2865_SET_HUE          0x01
#define TW2865_SET_CONTRAST     0x02
#define TW2865_SET_BRIGHT       0x04
#define TW2865_SET_SATURATION   0x08

typedef struct _tw2865_image_adjust
{
    unsigned char chip;
    unsigned char chn;
	unsigned char hue;
	unsigned char contrast;
	unsigned char brightness;
	unsigned char saturation;
	unsigned long item_sel;/* use such as "TW2865_SET_HUE|TW2865_SET_CONTRAST" */
} tw2865_image_adjust;


typedef struct _tw2865_audio_format
{
	unsigned char format;   /* 0:i2s; 1:dsp */
} tw2865_audio_format;

typedef struct _tw2865_audio_output
{
	unsigned char PlaybackOrLoop;   /*0:playback; 1:loop*/
	unsigned char channel;          /*[0, 15]*/
	unsigned char reserve[6];
} tw2865_audio_output;


typedef enum _tw2865_audio_samplerate
{
	TW2865_SAMPLE_RATE_8000,
	TW2865_SAMPLE_RATE_16000,
	TW2865_SAMPLE_RATE_32000,
	TW2865_SAMPLE_RATE_44100,
	TW2865_SAMPLE_RATE_48000,
	TW2865_SAMPLE_RATE_BUTT
} tw2865_audio_samplerate;

typedef struct _tw2865_audio_playback
{
    unsigned int chip;
    unsigned int chn;
} tw2865_audio_playback;

typedef struct _tw2865_audio_da_mute
{
    unsigned int chip;
} tw2865_audio_da_mute;

typedef struct _ttw2865_audio_da_volume
{
    unsigned int chip;
    unsigned int volume;
} tw2865_audio_da_volume;

typedef enum _tw2865_audio_bitwidth
{
	TW2865_AUDIO_BITWIDTH_16 = 0,
	TW2865_AUDIO_BITWIDTH_8 = 1,
	TW2865_AUDIO_BITWIDTH_BUTT
} tw2865_audio_bitwidth;


#define TW2865_IOC_MAGIC            't'

#define TW2865CMD_READ_REG		      _IOWR(TW2865_IOC_MAGIC, 1, int) 
#define TW2865CMD_WRITE_REG	        _IOW(TW2865_IOC_MAGIC, 2, int) 

#define TW2865_GET_VIDEO_LOSS		    _IOWR(TW2865_IOC_MAGIC, 4, tw2865_video_loss) 
#define TW2865_SET_IMAGE_ADJUST	    _IOW(TW2865_IOC_MAGIC, 5, tw2865_image_adjust) 
#define TW2865_GET_IMAGE_ADJUST	    _IOWR(TW2865_IOC_MAGIC, 6, tw2865_image_adjust) 
#define TW2865_SET_VIDEO_NORM	      _IOW(TW2865_IOC_MAGIC, 7, tw2865_video_norm) 
#define TW2865_GET_VIDEO_NORM	      _IOWR(TW2865_IOC_MAGIC, 8, tw2865_video_norm) 
#define TW2865_SET_WORK_MODE	      _IOW(TW2865_IOC_MAGIC, 9, tw2865_work_mode) 

#define TW2865_SET_SAMPLE_RATE	    _IOW(TW2865_IOC_MAGIC, 10, tw2865_audio_samplerate) 
#define TW2865_SET_AUDIO_PLAYBACK   _IOW(TW2865_IOC_MAGIC, 11, tw2865_audio_playback) 
#define TW2865_SET_AUDIO_DA_MUTE    _IOW(TW2865_IOC_MAGIC, 12, tw2865_audio_da_mute)
#define TW2865_SET_AUDIO_DA_VOLUME  _IOW(TW2865_IOC_MAGIC, 13, tw2865_audio_da_volume)
#define TW2865_SET_AUDIO_FORMAT     _IOW(TW2865_IOC_MAGIC, 14, tw2865_audio_format)
#define TW2865_SET_AUDIO_BITWIDTH     _IOW(TW2865_IOC_MAGIC, 15, tw2865_audio_format)

int tw2865_set_audio_samplerate(tw2865_audio_samplerate samplerate);
int tw2865_set_audio_format(tw2865_audio_format audio_format);
#endif


