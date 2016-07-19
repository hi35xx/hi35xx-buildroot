#ifndef _ACODEC_H_
#define _ACODEC_H_

typedef enum hiACODEC_FS_E {
	ACODEC_FS_48000 =   0x1a,
	ACODEC_FS_24000 =   0x19,
	ACODEC_FS_12000 =   0x18,

	ACODEC_FS_44100 =   0x1a,
	ACODEC_FS_22050 =   0x19,
	ACODEC_FS_11025 =   0x18,

	ACODEC_FS_32000 =   0x1a,
	ACODEC_FS_16000 =   0x19,
	ACODEC_FS_8000  =   0x18,

	ACODEC_FS_BUTT = 0x1b,
} ACODEC_FS_E;

#endif /* End of #ifndef _ACODEC_H_ */
