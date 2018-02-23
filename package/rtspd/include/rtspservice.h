#ifndef _RTSP_H
#define _RTSP_H
#include "rtsputils.h"

#define RTSP_DEBUG 1
#define RTP_DEFAULT_PORT 5004

void CallBackNotifyRtspExit(char s8IsExit);
void *ThreadRtsp(void *pArgs);
int rtsp_server(RTSP_buffer *rtsp);
void IntHandl(int i);
void UpdateSps(unsigned char *data,int len);
void UpdatePps(unsigned char *data,int len);


#endif /* _RTSP_H */
