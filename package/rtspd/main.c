
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>

#include "rtspservice.h"
#include "rtputils.h"
#include "ringfifo.h"
#include "sample_comm.h"

extern int g_s32Quit ;

/**************************************************************************************************
**
**
**
**************************************************************************************************/
extern void *SAMPLE_VENC_720P_CLASSIC(void *p);
int main(void)
{
	int s32MainFd,temp;
	struct timespec ts = { 2, 0 };
	pthread_t id;
	ringmalloc(720*576);
	printf("RTSP server START\n");
	PrefsInit();
	printf("listen for client connecting...\n");
	signal(SIGINT, IntHandl);
	s32MainFd = tcp_listen(SERVER_RTSP_PORT_DEFAULT);
	if (ScheduleInit() == ERR_FATAL)
	{
		fprintf(stderr,"Fatal: Can't start scheduler %s, %i \nServer is aborting.\n", __FILE__, __LINE__);
		return 0;
	}
	RTP_port_pool_init(RTP_DEFAULT_PORT);
	pthread_create(&id,NULL,SAMPLE_VENC_720P_CLASSIC,NULL);
	while (!g_s32Quit)
	{
		nanosleep(&ts, NULL);
		EventLoop(s32MainFd);
	}
	sleep(2);
	ringfree();
	printf("The Server quit!\n");

	return 0;
}

