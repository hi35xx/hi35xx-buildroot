#ifndef __MPI_HDMI_H__
#define __MPI_HDMI_H__

#define _HI_UNF_HDMI_DRV 0

#define MAX_SIZE 0x3000L

#define HDMI_MALLOC malloc

extern void WriteDefaultConfigToEEPROM( void);
extern void TX_SetHDMIMode(HI_U8 Enabled);
void DelayMS( HI_U16 count);

extern void SetHdmiAudio(HI_U8 Enabled);
extern void SetHdmiVideo(HI_U8 Enabled);
extern void BlockWriteEEPROM(HI_U8 NBytes, HI_U16 Addr, HI_U8 * Data);
extern HI_U8 ReadByteEEPROM(HI_U8 RegAddr);
extern void WriteByteEEPROM(HI_U8 RegAddr, HI_U8 RegData);

#endif

