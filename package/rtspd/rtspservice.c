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
#include <errno.h>

#include "rtsputils.h"
#include "rtspservice.h"
#include "rtputils.h"
#include "ringfifo.h"

struct profileid_sps_pps{
	char base64profileid[10];
	char base64sps[524];
	char base64pps[524];
};

pthread_mutex_t mut; 

#define MAX_SIZE 1024*1024*200
#define SDP_EL "\r\n"
#define RTSP_RTP_AVP "RTP/AVP"


struct profileid_sps_pps psp; //存base64编码的profileid sps pps

StServPrefs stPrefs;
extern int num_conn;
int g_s32Maxfd = 0;//最大轮询id号
int g_s32DoPlay = 0;

uint32_t s_u32StartPort=RTP_DEFAULT_PORT;
uint32_t s_uPortPool[MAX_CONNECTION];//RTP端口
extern int stop_schedule;
int g_s32Quit = 0;//退出全局变量

void RTP_port_pool_init(int port);
int UpdateSpsOrPps(unsigned char *data,int frame_type,int len);
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void PrefsInit()
{
	int l;
	//设置服务器信息全局变量
	stPrefs.port = SERVER_RTSP_PORT_DEFAULT;

	gethostname(stPrefs.hostname,sizeof(stPrefs.hostname));
	l=strlen(stPrefs.hostname);
	if (getdomainname(stPrefs.hostname+l+1,sizeof(stPrefs.hostname)-l)!=0)
	{
		stPrefs.hostname[l]='.';
	}

#ifdef RTSP_DEBUG
	printf("\n");
	printf("\thostname is: %s\n", stPrefs.hostname);
	printf("\trtsp listening port is: %d\n", stPrefs.port);
	printf("\tInput rtsp://hostIP:%d/test.264 to play this\n",stPrefs.port);
	printf("\n");
#endif

}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//为缓冲分配空间
void RTSP_initserver(RTSP_buffer *rtsp, int fd)
{
    rtsp->fd = fd;
    rtsp->session_list = (RTSP_session *) calloc(1, sizeof(RTSP_session));
    rtsp->session_list->session_id = -1;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//为RTP准备两个端口
int RTP_get_port_pair(port_pair *pair)
{
    int i;

    for (i=0; i<MAX_CONNECTION; ++i)
    {
        if (s_uPortPool[i]!=0)
        {
            pair->RTP=(s_uPortPool[i]-s_u32StartPort)*2+s_u32StartPort;
            pair->RTCP=pair->RTP+1;
            s_uPortPool[i]=0;
            return ERR_NOERROR;
        }
    }
    return ERR_GENERIC;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void AddClient(RTSP_buffer **ppRtspList, int fd)
{
    RTSP_buffer *pRtsp=NULL,*pRtspNew=NULL;

#ifdef RTSP_DEBUG
    fprintf(stderr, "%s, %d\n", __FUNCTION__, __LINE__);
#endif

    //在链表头部插入第一个元素
    if (*ppRtspList==NULL)
    {
        /*分配空间*/
        if ( !(*ppRtspList=(RTSP_buffer*)calloc(1,sizeof(RTSP_buffer)) ) )
        {
            fprintf(stderr,"alloc memory error %s,%i\n", __FILE__, __LINE__);
            return;
        }
        pRtsp = *ppRtspList;
    }
    else
    {
    	//向链表中插入新的元素
        for (pRtsp=*ppRtspList; pRtsp!=NULL; pRtsp=pRtsp->next)
        {
        	pRtspNew=pRtsp;
        }
        /*在链表尾部插入*/
        if (pRtspNew!=NULL)
        {
        	if ( !(pRtspNew->next=(RTSP_buffer *)calloc(1,sizeof(RTSP_buffer)) ) )
            {
                fprintf(stderr, "error calloc %s,%i\n", __FILE__, __LINE__);
                return;
            }
            pRtsp=pRtspNew->next;
            pRtsp->next=NULL;
        }
    }

    //设置最大轮询id号
    if(g_s32Maxfd < fd)
    {
    	g_s32Maxfd = fd;
    }

    /*初始化新添加的客户端*/
    RTSP_initserver(pRtsp,fd);
    fprintf(stderr,"Incoming RTSP connection accepted on socket: %d\n",pRtsp->fd);

}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
/*根据缓冲区的内容，填充后边两个长度数据,检查缓冲区中消息的完整性
 * return -1 on ERROR
 * return RTSP_not_full (0) if a full RTSP message is NOT present in the in_buffer yet.
 * return RTSP_method_rcvd (1) if a full RTSP message is present in the in_buffer and is
 *                     ready to be handled.
 * return RTSP_interlvd_rcvd (2) if a complete RTP/RTCP interleaved packet is present.
 * terminate on really ugly cases.
 */
int RTSP_full_msg_rcvd(RTSP_buffer *rtsp, int *hdr_len, int *body_len)
{
    int eomh;    /* end of message header found */
	int mb;       /* message body exists */
    int tc;         /* terminator count */
    int ws;        /* white space */
    unsigned int ml;              /* total message length including any message body */
    int bl;                           /* message body length */
    char c;                         /* character */
    int control;
    char *p;

    /*是否存在交叉存取的二进制rtp/rtcp数据包，参考RFC2326-10.12*/
    if (rtsp->in_buffer[0] == '$')
    {
    	uint16_t *intlvd_len = (uint16_t *)&rtsp->in_buffer[2];   /*跳过通道标志符*/

        /*转化为主机字节序，因为长度是网络字节序*/
        if ( (bl = ntohs(*intlvd_len)) <= rtsp->in_size)
        {
        	fprintf(stderr,"Interleaved RTP or RTCP packet arrived (len: %hu).\n", bl);
            if (hdr_len)
                *hdr_len = 4;
            if (body_len)
                *body_len = bl;
            return RTSP_interlvd_rcvd;
        }
        else
        {
            /*缓冲区不能完全存放数据*/
            fprintf(stderr,"Non-complete Interleaved RTP or RTCP packet arrived.\n");
            return RTSP_not_full;
        }

    }


    eomh = mb = ml = bl = 0;
    while (ml <= rtsp->in_size)
    {
        /* look for eol. */
        /*计算不包含回车、换行在内的所有字符数*/
        control = strcspn(&(rtsp->in_buffer[ml]), "\r\n");
        if(control > 0)
            ml += control;
        else
            return ERR_GENERIC;

        /* haven't received the entire message yet. */
        if (ml > rtsp->in_size)
            return RTSP_not_full;


        /* 处理终结符，判读是否是消息头的结束*/
        tc = ws = 0;
        while (!eomh && ((ml + tc + ws) < rtsp->in_size))
        {
            c = rtsp->in_buffer[ml + tc + ws];
            /*统计回车换行*/
            if (c == '\r' || c == '\n')
                tc++;
            else if ((tc < 3) && ((c == ' ') || (c == '\t')))
            {
                ws++;                 /*回车、换行之间的空格或者TAB，也是可以接受的 */
            }
            else
            {
            	break;
            }
        }

        /*
         *一对回车、换行符仅仅被统计为一个行终结符
         * 双行可以被接受，并将其认为是消息头的结束标识
         * 这与RFC2068中的描述一致，参考rfc2068 19.3
         *否则，对所有的HTTP/1.1兼容协议消息元素来说，
         *回车、换行被认为是合法的行终结符
         */

        /* must be the end of the message header */
        if ((tc > 2) || ((tc == 2) && (rtsp->in_buffer[ml] == rtsp->in_buffer[ml + 1])))
            eomh = 1;
        ml += tc + ws;

        if (eomh)
        {
            ml += bl;   /* 加入消息体长度 */
            if (ml <= rtsp->in_size)
            	break;  /* all done finding the end of the message. */
        }

        if (ml >= rtsp->in_size)
            return RTSP_not_full;   /* 还没有完全接收消息 */

        /*检查每一行的第一个记号，确定是否有消息体存在 */
        if (!mb)
        {
            /* content length token not yet encountered. */
            if (!strncmp(&(rtsp->in_buffer[ml]), HDR_CONTENTLENGTH, strlen(HDR_CONTENTLENGTH)))
            {
                mb = 1;                        /* 存在消息体. */
                ml += strlen(HDR_CONTENTLENGTH);

                /*跳过:和空格，找到长度字段*/
                while (ml < rtsp->in_size)
                {
                    c = rtsp->in_buffer[ml];
                    if ((c == ':') || (c == ' '))
                        ml++;
                    else
                        break;
                }
                //Content-Length:后面是消息体长度值
                if (sscanf(&(rtsp->in_buffer[ml]), "%d", &bl) != 1)
                {
                    fprintf(stderr,"RTSP_full_msg_rcvd(): Invalid ContentLength encountered in message.\n");
                    return ERR_GENERIC;
                }
            }
        }
    }

    if (hdr_len)
        *hdr_len = ml - bl;

    if (body_len)
    {
    /*
     * go through any trailing nulls.  Some servers send null terminated strings
     * following the body part of the message.  It is probably not strictly
     * legal when the null byte is not included in the Content-Length count.
     * However, it is tolerated here.
     * 减去可能存在的\0，它没有被计算在Content-Length中
     */
        for (tc = rtsp->in_size - ml, p = &(rtsp->in_buffer[ml]); tc && (*p == '\0'); p++, bl++, tc--);
            *body_len = bl;
    }

    return RTSP_method_rcvd;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
/*
 * return	0 是客户端发送的请求
 *			1 是服务器返回的响应
 */
int RTSP_valid_response_msg(unsigned short *status, RTSP_buffer * rtsp)
{
    char ver[32], trash[15];
    unsigned int stat;
    unsigned int seq;
    int pcnt;                   /* parameter count */

    /* assuming "stat" may not be zero (probably faulty) */
    stat = 0;

    /*从消息中填充数据*/
    pcnt = sscanf(rtsp->in_buffer, " %31s %u %s %s %u\n%*255s ", ver, &stat, trash, trash, &seq);

    /* 通过起始字符，检查信息是客户端发送的请求还是服务器做出的响应*/
    /* C->S CMD rtsp://IP:port/suffix RTSP/1.0\r\n			|head
     * 		CSeq: 1 \r\n									|
     * 		Content_Length:**								|body
     * S->C RTSP/1.0 200 OK\r\n
     * 		CSeq: 1\r\n
     * 		Date:....
      */
    if (strncmp(ver, "RTSP/", 5))
        return 0;   /*不是响应消息，是客户端请求消息，返回*/

    /*确信至少存在版本、状态码、序列号*/
    if (pcnt < 3 || stat == 0)
        return 0;            /* 表示不是一个响应消息   */

    /*如果版本不兼容，在此处增加码来拒绝该消息*/

    /*检查回复消息中的序列号是否合法*/
    if (rtsp->rtsp_cseq != seq + 1)
    {
        fprintf(stderr,"Invalid sequence number returned in response.\n");
        return ERR_GENERIC;    /*序列号错误，返回*/
    }

    *status = stat;
    return 1;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//返回请求方法类型，出错返回-1
int RTSP_validate_method(RTSP_buffer * pRtsp)
{
    char method[32], hdr[16];
    char object[256];
    char ver[32];
    unsigned int seq;
    int pcnt;   /* parameter count */
    int mid = ERR_GENERIC;
	char *p; //=======增加
	char trash[255];   //===增加

    *method = *object = '\0';
    seq = 0;

	printf("");
    /*按照请求消息的格式解析消息的第一行*/
//    if ( (pcnt = sscanf(pRtsp->in_buffer, " %31s %255s %31s\n%15s", method, object, ver, hdr, &seq)) != 5){
	if ( (pcnt = sscanf(pRtsp->in_buffer, " %31s %255s %31s\n%15s", method, object, ver, hdr)) != 4){
		printf("========\n%s\n==========\n",pRtsp->in_buffer);
		printf("%s ",method); 
		printf("%s ",object);
		printf("%s ",ver);
		printf("hdr:%s\n",hdr);		
	   	return ERR_GENERIC;
	}
	

    /*如果没有头标记，则错误*/
/*	
    if ( !strstr(hdr, HDR_CSEQ) ){
		printf("no HDR_CSEQ err_generic");
	   	return ERR_GENERIC;	
	}
*/
//===========加
	if ((p = strstr(pRtsp->in_buffer, "CSeq")) == NULL) {
		return ERR_GENERIC;
	}else {
		if(sscanf(p,"%254s %d",trash,&seq)!=2){
			return ERR_GENERIC;
		}
	}
//==========

    /*根据不同的方法，返回响应的方法ID*/
    if (strcmp(method, RTSP_METHOD_DESCRIBE) == 0) {
        mid = RTSP_ID_DESCRIBE;
    }
    if (strcmp(method, RTSP_METHOD_ANNOUNCE) == 0) {
        mid = RTSP_ID_ANNOUNCE;
    }
    if (strcmp(method, RTSP_METHOD_GET_PARAMETERS) == 0) {
        mid = RTSP_ID_GET_PARAMETERS;
    }
    if (strcmp(method, RTSP_METHOD_OPTIONS) == 0) {
        mid = RTSP_ID_OPTIONS;
    }
    if (strcmp(method, RTSP_METHOD_PAUSE) == 0) {
        mid = RTSP_ID_PAUSE;
    }
    if (strcmp(method, RTSP_METHOD_PLAY) == 0) {
        mid = RTSP_ID_PLAY;
    }
    if (strcmp(method, RTSP_METHOD_RECORD) == 0) {
        mid = RTSP_ID_RECORD;
    }
    if (strcmp(method, RTSP_METHOD_REDIRECT) == 0) {
        mid = RTSP_ID_REDIRECT;
    }
    if (strcmp(method, RTSP_METHOD_SETUP) == 0) {
        mid = RTSP_ID_SETUP;
    }
    if (strcmp(method, RTSP_METHOD_SET_PARAMETER) == 0) {
        mid = RTSP_ID_SET_PARAMETER;
    }
    if (strcmp(method, RTSP_METHOD_TEARDOWN) == 0) {
        mid = RTSP_ID_TEARDOWN;
    }

    /*设置当前方法的请求序列号*/
    pRtsp->rtsp_cseq = seq;
    return mid;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//解析URL中的port端口和文件名称
int ParseUrl(const char *pUrl, char *pServer, unsigned short *port, char *pFileName, size_t FileNameLen)
{
	/* expects format [rtsp://server[:port/]]filename RTSP/1.0*/

	int s32NoValUrl;

    /*拷贝URL */
    char *pFull = (char *)malloc(strlen(pUrl) + 1);
    strcpy(pFull, pUrl);

    /*检查前缀是否正确*/
    if (strncmp(pFull, "rtsp://", 7) == 0)
    {
        char *pSuffix;

        //找到/ 它之后是文件名
        if((pSuffix = strchr(&pFull[7], '/')) != NULL)
        {
        	char *pPort;
        	char pSubPort[128];
        	//判断是否有端口
        	pPort=strchr(&pFull[7], ':');
        	if(pPort != NULL)
        	{	
				strncpy(pServer,&pFull[7],pPort-pFull-7);
				printf("server:%s\n",pServer);
        		strncpy(pSubPort, pPort+1, pSuffix-pPort-1);
        		pSubPort[pSuffix-pPort-1] = '\0';
        		*port = (unsigned short) atol(pSubPort);
				printf("port:%d\n",port);
        	}
        	else
        	{
        		*port = SERVER_RTSP_PORT_DEFAULT;
        	}
        	pSuffix++;
        	//跳过空格或者制表符
        	while(*pSuffix == ' '||*pSuffix == '\t')
        	{
        		pSuffix++;
        	}
        	//拷贝文件名
        	strcpy(pFileName, pSuffix);
        	s32NoValUrl = 0;
        }
        else
        {
        	*port = SERVER_RTSP_PORT_DEFAULT;
        	*pFileName = '\0';
        	s32NoValUrl = 1;
        }
    }else
    {
    	*pFileName = '\0';
    	s32NoValUrl = 1;
    }
    //释放空间
    free(pFull);
    return s32NoValUrl;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//把当前时间作为session号
char *GetSdpId(char *buffer)
{
	time_t t;
    buffer[0]='\0';
    t = time(NULL);
    sprintf(buffer,"%.0f",(float)t+2208988800U);    /*获得NPT时间*/
    return buffer;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void GetSdpDescr(RTSP_buffer * pRtsp, char *pDescr, char *s8Str)
{
/*/=====================================
	char const* const SdpPrefixFmt =
			"v=0\r\n"	//版本信息
			"o=- %s %s IN IP4 %s\r\n" //<用户名><会话id><版本>//<网络类型><地址类型><地址>
			"c=IN IP4 %s\r\n"		//c=<网络信息><地址信息><连接地址>对ip4为0.0.0.0  here！
			"s=RTSP Session\r\n"		//会话名session id
			"i=N/A\r\n"		//会话信息
			"t=0 0\r\n"		//<开始时间><结束时间>
			"a=recvonly\r\n"
			"m=video %s RTP/AVP 96\r\n\r\n";	//<媒体格式><端口><传送><格式列表,即媒体净荷类型> m=video 5004 RTP/AVP 96
			
	struct ifreq stIfr;
	char pSdpId[128];

	//获取本机地址
	strcpy(stIfr.ifr_name, "eth0");
	if(ioctl(pRtsp->fd, SIOCGIFADDR, &stIfr) < 0)
	{
		//printf("Failed to get host eth0 ip\n");
		strcpy(stIfr.ifr_name, "wlan0");
		if(ioctl(pRtsp->fd, SIOCGIFADDR, &stIfr) < 0)
		{
			printf("Failed to get host eth0 or wlan0 ip\n");
		}
	}

	sock_ntop_host(&stIfr.ifr_addr, sizeof(struct sockaddr), s8Str, 128);

	GetSdpId(pSdpId);

	sprintf(pDescr,  SdpPrefixFmt,  pSdpId,  pSdpId,  s8Str,  inet_ntoa(((struct sockaddr_in *)(&pRtsp->stClientAddr))->sin_addr), "5006", "H264");
			"b=RR:0\r\n"
			 //按spydroid改
			"a=rtpmap:96 %s/90000\r\n"		//a=rtpmap:<净荷类型><编码名>/<时钟速率> 	a=rtpmap:96 H264/90000
			"a=fmtp:96 packetization-mode=1;profile-level-id=1EE042;sprop-parameter-sets=QuAe2gLASRA=,zjCkgA==\r\n"
			"a=control:trackID=0\r\n";
	
#ifdef RTSP_DEBUG
//			printf("SDP:\n%s\n", pDescr);
#endif
*/
	struct ifreq stIfr;
	char pSdpId[128];
	char rtp_port[5];
	strcpy(stIfr.ifr_name, "eth0");
	if(ioctl(pRtsp->fd, SIOCGIFADDR, &stIfr) < 0)
	{
		//printf("Failed to get host eth0 ip\n");
		strcpy(stIfr.ifr_name, "wlan0");
		if(ioctl(pRtsp->fd, SIOCGIFADDR, &stIfr) < 0)
		{
			printf("Failed to get host eth0 or wlan0 ip\n");
		}
	}
	sock_ntop_host(&stIfr.ifr_addr, sizeof(struct sockaddr), s8Str, 128);

	GetSdpId(pSdpId);

	strcpy(pDescr, "v=0\r\n");	
	strcat(pDescr, "o=-");
	strcat(pDescr, pSdpId);
	strcat(pDescr," ");
	strcat(pDescr, pSdpId);
	strcat(pDescr," IN IP4 ");
	strcat(pDescr, s8Str);

	strcat(pDescr, "\r\n");
	strcat(pDescr, "s=Unnamed\r\n");
	strcat(pDescr, "i=N/A\r\n");

   	strcat(pDescr, "c=");
   	strcat(pDescr, "IN ");		/* Network type: Internet. */
   	strcat(pDescr, "IP4 ");		/* Address type: IP4. */
	//strcat(pDescr, get_address());
	strcat(pDescr, inet_ntoa(((struct sockaddr_in *)(&pRtsp->stClientAddr))->sin_addr));
	strcat(pDescr, "\r\n");
	
   	strcat(pDescr, "t=0 0\r\n");	
	strcat(pDescr, "a=recvonly\r\n");
	/**** media specific ****/
	strcat(pDescr,"m=");
	strcat(pDescr,"video ");
	sprintf(rtp_port,"%d",s_u32StartPort);
	strcat(pDescr, rtp_port);
	strcat(pDescr," RTP/AVP "); /* Use UDP */
	strcat(pDescr,"96\r\n");
	//strcat(pDescr, "\r\n");
	strcat(pDescr,"b=RR:0\r\n");
		/**** Dynamically defined payload ****/
		strcat(pDescr,"a=rtpmap:96");
		strcat(pDescr," ");	
		strcat(pDescr,"H264/90000");
		strcat(pDescr, "\r\n");
		strcat(pDescr,"a=fmtp:96 packetization-mode=1;");
		strcat(pDescr,"profile-level-id=");
		strcat(pDescr,psp.base64profileid);
		strcat(pDescr,";sprop-parameter-sets=");
		strcat(pDescr,psp.base64sps);
		strcat(pDescr,",");
		strcat(pDescr,psp.base64pps);
		strcat(pDescr,";");
		strcat(pDescr, "\r\n");
		strcat(pDescr,"a=control:trackID=0");
		strcat(pDescr, "\r\n");

printf("\n\n%s,%d===>psp.base64profileid=%s,psp.base64sps=%s,psp.base64pps=%s\n\n",__FUNCTION__,__LINE__,psp.base64profileid,psp.base64sps,psp.base64pps);

		
/*
		strcat(pDescr, "m=audio ");
		strcat(pDescr,"5004");
		strcat(pDescr," RTP/AVP 96\r\n");
		strcat(pDescr, "b=AS:128\r\n");
		strcat(pDescr, "b=RR:0\r\n");
		strcat(pDescr, "a=rtpmap:96 AMR/8000\r\n");
		strcat(pDescr, "a=fmtp:96 octrt-align=1;\r\n");
		strcat(pDescr,"a=control:trackID=1");
		strcat(pDescr, "\r\n");
*/
		//strcat(pDescr, "\r\n");
		//printf("0\r\n");
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
/*添加时间戳*/
void add_time_stamp(char *b, int crlf)
{
    struct tm *t;
    time_t now;

    /*
    * concatenates a null terminated string with a
    * time stamp in the format of "Date: 23 Jan 1997 15:35:06 GMT"
    */
    now = time(NULL);
    t = gmtime(&now);
    //输出时间格式：Date: Fri, 15 Jul 2011 09:23:26 GMT
    strftime(b + strlen(b), 38, "Date: %a, %d %b %Y %H:%M:%S GMT"RTSP_EL, t);

    //是否是消息结束，添加回车换行符
    if (crlf)
        strcat(b, "\r\n");	/* add a message header terminator (CRLF) */
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int SendDescribeReply(RTSP_buffer * rtsp, char *object, char *descr, char *s8Str)
{
    char *pMsgBuf;            /* 用于获取响应缓冲指针*/
    int s32MbLen;

    /* 分配空间，处理内部错误*/
    s32MbLen = 2048;
    pMsgBuf = (char *)malloc(s32MbLen);
    if (!pMsgBuf)
    {
        fprintf(stderr,"send_describe_reply(): unable to allocate memory\n");
        send_reply(500, 0, rtsp);    /* internal server error */
        if (pMsgBuf)
        {
            free(pMsgBuf);
        }
        return ERR_ALLOC;
    }

    /*构造describe消息串*/
    sprintf(pMsgBuf, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL"Server: %s/%s"RTSP_EL, RTSP_VER, 200, get_stat(200), rtsp->rtsp_cseq, PACKAGE, VERSION);
    add_time_stamp(pMsgBuf, 0);                 /*添加时间戳*/

	strcat(pMsgBuf, "Content-Type: application/sdp"RTSP_EL);   /*实体头，表示实体类型*/

    /*用于解析实体内相对url的 绝对url*/
    sprintf(pMsgBuf + strlen(pMsgBuf), "Content-Base: rtsp://%s/%s/"RTSP_EL, s8Str, object);
    sprintf(pMsgBuf + strlen(pMsgBuf), "Content-Length: %d"RTSP_EL, strlen(descr)); /*消息体的长度*/
    strcat(pMsgBuf, RTSP_EL);

    /*消息头结束*/

    /*加上消息体*/
    strcat(pMsgBuf, descr);    /*describe消息*/
    /*向缓冲区中填充数据*/
    bwrite(pMsgBuf, (unsigned short) strlen(pMsgBuf), rtsp);

    free(pMsgBuf);

    return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//describe处理
int RTSP_describe(RTSP_buffer * pRtsp)
{
	char object[255], trash[255];
	char *p;
	unsigned short port;
	char s8Url[255];
	char s8Descr[MAX_DESCR_LENGTH];
	char server[128];
	char s8Str[128];

	/*根据收到的请求请求消息，跳过方法名，分离出URL*/
	if (!sscanf(pRtsp->in_buffer, " %*s %254s ", s8Url))
	{
		fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
		send_reply(400, 0, pRtsp);                			/* bad request */
		printf("get URL error");
		return ERR_NOERROR;
	}

	/*验证URL */
	switch (ParseUrl(s8Url, server, &port, object, sizeof(object)))
	{
		case 1: /*请求错误*/
			fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
			send_reply(400, 0, pRtsp);
			printf("url request error");
			return ERR_NOERROR;
			break;

		case -1: /*内部错误*/
			fprintf(stderr,"url error while parsing !\n");
			send_reply(500, 0, pRtsp);
			printf("inner error");
			return ERR_NOERROR;
			break;

		default:
			break;
	}

	/*取得序列号,并且必须有这个选项*/
	if ((p = strstr(pRtsp->in_buffer, HDR_CSEQ)) == NULL)
	{
		fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
		send_reply(400, 0, pRtsp);  /* Bad Request */
		printf("get serial num error");
		return ERR_NOERROR;
	}
	else
	{
		if (sscanf(p, "%254s %d", trash, &(pRtsp->rtsp_cseq)) != 2)
		{
			fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
			send_reply(400, 0, pRtsp);   /*请求错误*/
			printf("get serial num 2 error");
			return ERR_NOERROR;
		}
	}

	//获取SDP内容
	GetSdpDescr(pRtsp, s8Descr, s8Str);
	//发送Describe响应
	//printf("----------------1\r\n");
	SendDescribeReply(pRtsp, object, s8Descr, s8Str);
	//printf("2\r\n");
	return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//发送options处理后的响应
int send_options_reply(RTSP_buffer * pRtsp, long cseq)
{
    char r[1024];
    sprintf(r, "%s %d %s"RTSP_EL"CSeq: %ld"RTSP_EL, RTSP_VER, 200, get_stat(200), cseq);
    strcat(r, "Public: OPTIONS,DESCRIBE,SETUP,PLAY,PAUSE,TEARDOWN"RTSP_EL);
    strcat(r, RTSP_EL);

    bwrite(r, (unsigned short) strlen(r), pRtsp);

#ifdef RTSP_DEBUG
//	fprintf(stderr ,"SERVER SEND Option Replay: %s\n", r);
#endif

    return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
//options处理
int RTSP_options(RTSP_buffer * pRtsp)
{
    char *p;
    char trash[255];
    unsigned int cseq;
    char method[255], url[255], ver[255];

#ifdef RTSP_DEBUG
//	trace_point();
#endif

    /*序列号*/
    if ((p = strstr(pRtsp->in_buffer, HDR_CSEQ)) == NULL)
    {
    	fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
        send_reply(400, 0, pRtsp);/* Bad Request */
		printf("serial num error");
        return ERR_NOERROR;
    }
    else
    {
        if (sscanf(p, "%254s %d", trash, &(pRtsp->rtsp_cseq)) != 2)
        {
        	fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
            send_reply(400, 0, pRtsp);/* Bad Request */
			printf("serial num 2 error");
            return ERR_NOERROR;
        }
    }

    cseq = pRtsp->rtsp_cseq;

#ifdef RTSP_DEBUG
    sscanf(pRtsp->in_buffer, " %31s %255s %31s ", method, url, ver);
    fprintf(stderr,"%s %s %s \n",method,url,ver);
#endif

    //发送option处理后的消息
    send_options_reply(pRtsp, cseq);

    return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int send_setup_reply(RTSP_buffer *pRtsp, RTSP_session *pSession, RTP_session *pRtpSes)
{
	char s8Str[1024];
	sprintf(s8Str, "%s %d %s"RTSP_EL"CSeq: %ld"RTSP_EL"Server: %s/%s"RTSP_EL, RTSP_VER,\
			200, get_stat(200), (long int)pRtsp->rtsp_cseq, PACKAGE, VERSION);
	add_time_stamp(s8Str, 0);
	sprintf(s8Str + strlen(s8Str), "Session: %d"RTSP_EL"Transport: ", (pSession->session_id));

    switch (pRtpSes->transport.type)
    {
		case RTP_rtp_avp:
			if (pRtpSes->transport.u.udp.is_multicast)
			{
//				sprintf(s8Str + strlen(s8Str), "RTP/AVP;multicast;ttl=%d;destination=%s;port=", (int)DEFAULT_TTL, descr->multicast);
			}
			else
			{
				sprintf(s8Str + strlen(s8Str), "RTP/AVP;unicast;client_port=%d-%d;destination=192.168.245.65;source=%s;server_port=", \
						pRtpSes->transport.u.udp.cli_ports.RTP, pRtpSes->transport.u.udp.cli_ports.RTCP,"192.168.245.96");
			}

			sprintf(s8Str + strlen(s8Str), "%d-%d"RTSP_EL, pRtpSes->transport.u.udp.ser_ports.RTP, pRtpSes->transport.u.udp.ser_ports.RTCP);
			break;

		case RTP_rtp_avp_tcp:
			sprintf(s8Str + strlen(s8Str), "RTP/AVP/TCP;interleaved=%d-%d"RTSP_EL, \
					pRtpSes->transport.u.tcp.interleaved.RTP, pRtpSes->transport.u.tcp.interleaved.RTCP);
			break;

		default:
			break;
    }

    strcat(s8Str, RTSP_EL);
    bwrite(s8Str, (unsigned short) strlen(s8Str), pRtsp);

     return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int RTSP_setup(RTSP_buffer * pRtsp)
{
	char s8TranStr[128], *s8Str;
	char *pStr;
	RTP_transport Transport;
	int s32SessionID=0;
	RTP_session *rtp_s, *rtp_s_prec;
	RTSP_session *rtsp_s;

	if ((s8Str = strstr(pRtsp->in_buffer, HDR_TRANSPORT)) == NULL)
	{
		fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
		send_reply(406, 0, pRtsp);     // Not Acceptable
		printf("not acceptable");
		return ERR_NOERROR;
	}

	//检查传输层子串是否正确
	if (sscanf(s8Str, "%*10s %255s", s8TranStr) != 1)
	{
		fprintf(stderr,"SETUP request malformed: Transport string is empty\n");
		send_reply(400, 0, pRtsp);       // Bad Request
		printf("check transport 400 bad request");
		return ERR_NOERROR;
	}

	fprintf(stderr,"*** transport: %s ***\n", s8TranStr);

	//如果需要增加一个会话
	if ( !pRtsp->session_list )
	{
		pRtsp->session_list = (RTSP_session *) calloc(1, sizeof(RTSP_session));
	}
	rtsp_s = pRtsp->session_list;

	//建立一个新会话，插入到链表中
	if (pRtsp->session_list->rtp_session == NULL)
	{
		pRtsp->session_list->rtp_session = (RTP_session *) calloc(1, sizeof(RTP_session));
		rtp_s = pRtsp->session_list->rtp_session;
	}
	else
	{
		for (rtp_s = rtsp_s->rtp_session; rtp_s != NULL; rtp_s = rtp_s->next)
		{
			rtp_s_prec = rtp_s;
		}
		rtp_s_prec->next = (RTP_session *) calloc(1, sizeof(RTP_session));
		rtp_s = rtp_s_prec->next;
	}

/*
	printf("\tFile Name %s\n", Object);
	if((rtp_s->file_id = open(Object, O_RDONLY)) < 0)
	{
		printf("Open File error %s, %d\n", __FILE__, __LINE__);
	}
*/

	//起始状态为暂停
	rtp_s->pause = 1;

	rtp_s->hndRtp = NULL;

	Transport.type = RTP_no_transport;

	if((pStr = strstr(s8TranStr, RTSP_RTP_AVP)))
	{
		//Transport: RTP/AVP
		pStr += strlen(RTSP_RTP_AVP);
		if ( !*pStr || (*pStr == ';') || (*pStr == ' '))
		{
			//单播
			if (strstr(s8TranStr, "unicast"))
			{
				//如果指定了客户端端口号，填充对应的两个端口号
				if( (pStr = strstr(s8TranStr, "client_port")) )
				{
					pStr = strstr(s8TranStr, "=");
					sscanf(pStr + 1, "%d", &(Transport.u.udp.cli_ports.RTP));
					pStr = strstr(s8TranStr, "-");
					sscanf(pStr + 1, "%d", &(Transport.u.udp.cli_ports.RTCP));
				}

				//服务器端口
				if (RTP_get_port_pair(&Transport.u.udp.ser_ports) != ERR_NOERROR)
				{
					fprintf(stderr, "Error %s,%d\n", __FILE__, __LINE__);
					send_reply(500, 0, pRtsp);/* Internal server error */
					return ERR_GENERIC;
				}

				//建立RTP套接字
				rtp_s->hndRtp = (struct _tagStRtpHandle*)RtpCreate((unsigned int)(((struct sockaddr_in *)(&pRtsp->stClientAddr))->sin_addr.s_addr), Transport.u.udp.cli_ports.RTP, _h264nalu);
				printf("<><><><>Creat RTP<><><><>\n");

				Transport.u.udp.is_multicast = 0;
			}
			else
			{
				printf("multicast not codeing\n");
				//multicast 多播处理....
			}
			Transport.type = RTP_rtp_avp;
		}
		else if (!strncmp(s8TranStr, "/TCP", 4))
		{
			if( (pStr = strstr(s8TranStr, "interleaved")) )
			{
				pStr = strstr(s8TranStr, "=");
				sscanf(pStr + 1, "%d", &(Transport.u.tcp.interleaved.RTP));
				if ((pStr = strstr(pStr, "-")))
					sscanf(pStr + 1, "%d", &(Transport.u.tcp.interleaved.RTCP));
				else
					Transport.u.tcp.interleaved.RTCP = Transport.u.tcp.interleaved.RTP + 1;
			}
			else
			{

			}

			Transport.rtp_fd = pRtsp->fd;
//			Transport.rtcp_fd_out = pRtsp->fd;
//			Transport.rtcp_fd_in = -1;



		}
	}
	printf("pstr=%s\n",pStr);
	if (Transport.type == RTP_no_transport)
	{
		fprintf(stderr,"AAAAAAAAAAA Unsupported Transport,%s,%d\n", __FILE__, __LINE__);
		send_reply(461, 0, pRtsp);// Bad Request
		return ERR_NOERROR;
	}

	memcpy(&rtp_s->transport, &Transport, sizeof(Transport));

	//如果有会话头，就有了一个控制集合
	if ((pStr = strstr(pRtsp->in_buffer, HDR_SESSION)) != NULL)
	{
		if (sscanf(pStr, "%*s %d", &s32SessionID) != 1)
		{
			fprintf(stderr, "Error %s,%i\n", __FILE__, __LINE__);
			send_reply(454, 0, pRtsp); // Session Not Found
			return ERR_NOERROR;
		}
	}
	else
	{
		//产生一个非0的随机的会话序号
		struct timeval stNowTmp;
		gettimeofday(&stNowTmp, 0);
		srand((stNowTmp.tv_sec * 1000) + (stNowTmp.tv_usec / 1000));
		s32SessionID = 1 + (int) (10.0 * rand() / (100000 + 1.0));
		if (s32SessionID == 0)
		{
			s32SessionID++;
		}
	}

	pRtsp->session_list->session_id = s32SessionID;
	pRtsp->session_list->rtp_session->sched_id = schedule_add(rtp_s);

	send_setup_reply(pRtsp, rtsp_s, rtp_s);

	return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int send_play_reply(RTSP_buffer * pRtsp, RTSP_session * pRtspSessn)
{
	char s8Str[1024];
	char s8Temp[30];
	sprintf(s8Str, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL"Server: %s/%s"RTSP_EL, RTSP_VER, 200,\
			get_stat(200), pRtsp->rtsp_cseq, PACKAGE, VERSION);
	add_time_stamp(s8Str, 0);

	sprintf(s8Temp, "Session: %d"RTSP_EL, pRtspSessn->session_id);
	strcat(s8Str, s8Temp);
	strcat(s8Str, RTSP_EL);

	bwrite(s8Str, (unsigned short) strlen(s8Str), pRtsp);

	return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int RTSP_play(RTSP_buffer * pRtsp)
{
	char *pStr;
	char pTrash[128];
	long int s32SessionId;
	RTSP_session *pRtspSesn;
	RTP_session *pRtpSesn;

	//获取CSeq
	if ((pStr = strstr(pRtsp->in_buffer, HDR_CSEQ)) == NULL)
	{
		send_reply(400, 0, pRtsp);   /* Bad Request */
		printf("get CSeq!!400");
		return ERR_NOERROR;
	}
	else
	{
		if (sscanf(pStr, "%254s %d", pTrash, &(pRtsp->rtsp_cseq)) != 2)
		{
			send_reply(400, 0, pRtsp);    /* Bad Request */
			printf("get CSeq!! 2 400");
			return ERR_NOERROR;
		}
	}

	//获取session
	if ((pStr = strstr(pRtsp->in_buffer, HDR_SESSION)) != NULL)
	{
		if (sscanf(pStr, "%254s %ld", pTrash, &s32SessionId) != 2)
		{
			send_reply(454, 0, pRtsp);// Session Not Found
			printf("Session Not Found");
			return ERR_NOERROR;
		}
	}
	else
	{
		send_reply(400, 0, pRtsp);// bad request
		printf("Session Not Found bad request");
		return ERR_NOERROR;
	}

	//时间参数,假设都是 0-0,不做设置
/*	if ((pStr = strstr(pRtsp->in_buffer, HDR_RANGE)) != NULL)
	{
		if((pStrTime = strstr(pRtsp->in_buffer, "npt")) != NULL)
		{
			if((pStrTime = strstr(pStrTime, "=")) == NULL)
			{
				send_reply(400, 0, pRtsp);// Bad Request
				return ERR_NOERROR;
			}

		}
		else
		{

		}
	}
*/
	//播放list指向的rtp session
	pRtspSesn = pRtsp->session_list;
	if (pRtspSesn != NULL)
	{
		if (pRtspSesn->session_id == s32SessionId)
		{
			//查找RTP session,播放list中所有的session，本例程只有一个成员.
			for (pRtpSesn = pRtspSesn->rtp_session; pRtpSesn != NULL; pRtpSesn = pRtpSesn->next)
			{
				//播放所有演示
				if (!pRtpSesn->started)
				{
					//开始新的播放
					printf("\t+++++++++++++++++++++\n");
					printf("\tstart to play %d now!\n", pRtpSesn->sched_id);
					printf("\t+++++++++++++++++++++\n");

					if (schedule_start(pRtpSesn->sched_id, NULL) == ERR_ALLOC)
					{
						return ERR_ALLOC;
					}
				}
				else
				{
					//恢复暂停，播放
					if (!pRtpSesn->pause)
					{
						//fnc_log(FNC_LOG_INFO,"PLAY: already playing\n");
					}
					else
					{
//						schedule_resume(pRtpSesn->sched_id, NULL);
					}
				}

			}
		}
		else
		{
			send_reply(454, 0, pRtsp);	// Session not found
			return ERR_NOERROR;
		}
	}
	else
	{
		send_reply(415, 0, pRtsp);  // Internal server error
		return ERR_GENERIC;
	}

	send_play_reply(pRtsp, pRtspSesn);

	return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int send_teardown_reply(RTSP_buffer * pRtsp, long SessionId, long cseq)
{
    char s8Str[1024];
    char s8Temp[30];

    // 构建回复消息
    sprintf(s8Str, "%s %d %s"RTSP_EL"CSeq: %ld"RTSP_EL"Server: %s/%s"RTSP_EL, RTSP_VER,\
    		200, get_stat(200), cseq, PACKAGE, VERSION);
    //添加时间戳
    add_time_stamp(s8Str, 0);
    //会话ID
    sprintf(s8Temp, "Session: %ld"RTSP_EL, SessionId);
    strcat(s8Str, s8Temp);

    strcat(s8Str, RTSP_EL);

    //写入缓冲区
    bwrite(s8Str, (unsigned short) strlen(s8Str), pRtsp);

    return ERR_NOERROR;
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
int RTSP_teardown(RTSP_buffer * pRtsp)
{
	char *pStr;
	char pTrash[128];
	long int s32SessionId;
	RTSP_session *pRtspSesn;
	RTP_session *pRtpSesn;

	//获取CSeq
	if ((pStr = strstr(pRtsp->in_buffer, HDR_CSEQ)) == NULL)
	{
		send_reply(400, 0, pRtsp);   // Bad Request
		printf("get CSeq error");
		return ERR_NOERROR;
	}
	else
	{
		if (sscanf(pStr, "%254s %d", pTrash, &(pRtsp->rtsp_cseq)) != 2)
		{
			send_reply(400, 0, pRtsp);    // Bad Request
			printf("get CSeq 2 error");
			return ERR_NOERROR;
		}
	}

	//获取session
	if ((pStr = strstr(pRtsp->in_buffer, HDR_SESSION)) != NULL)
	{
		if (sscanf(pStr, "%254s %ld", pTrash, &s32SessionId) != 2)
		{
			send_reply(454, 0, pRtsp);	// Session Not Found
			return ERR_NOERROR;
		}
	}
	else
	{
		s32SessionId = -1;
	}

	pRtspSesn = pRtsp->session_list;
	if (pRtspSesn == NULL)
	{
		send_reply(415, 0, pRtsp);  // Internal server error
		return ERR_GENERIC;
	}

	if (pRtspSesn->session_id != s32SessionId)
	{
		send_reply(454, 0, pRtsp);	// Session not found
		return ERR_NOERROR;
	}

	//向客户端发送响应消息
	send_teardown_reply(pRtsp, s32SessionId, pRtsp->rtsp_cseq);

	//释放所有的URI RTP会话
	RTP_session *pRtpSesnTemp;
	pRtpSesn = pRtspSesn->rtp_session;
	while (pRtpSesn != NULL)
	{
		pRtpSesnTemp = pRtpSesn;

		pRtspSesn->rtp_session = pRtpSesn->next;

		pRtpSesn = pRtpSesn->next;

		//删除RTP视频发送
		RtpDelete((unsigned int)pRtpSesnTemp->hndRtp);
		//删除schedule中对应id
		schedule_remove(pRtpSesnTemp->sched_id);
		//全局变量播放总数减一，如果为0则不播放
		g_s32DoPlay--;

	}
	if (g_s32DoPlay == 0) 
	{
		printf("no user online now resetfifo\n");
		ringreset;
		/* 重新将所有可用的RTP端口号放入到port_pool[MAX_SESSION] 中 */
		RTP_port_pool_init(RTP_DEFAULT_PORT);
	}
	//释放链表空间
	if (pRtspSesn->rtp_session == NULL)
	{
		free(pRtsp->session_list);
		pRtsp->session_list = NULL;
	}

	return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
/*rtsp状态机，服务器端*/
void RTSP_state_machine(RTSP_buffer * pRtspBuf, int method)
{

#ifdef RTSP_DEBUG
	trace_point();
#endif

    /*除了播放过程中发送的最后一个数据流，
     *所有的状态迁移都在这里被处理
     * 状态迁移位于stream_event中
     */
    char *s;
    RTSP_session *pRtspSess;
    long int session_id;
    char trash[255];
    char szDebug[255];

    /*找到会话位置*/
    if ((s = strstr(pRtspBuf->in_buffer, HDR_SESSION)) != NULL)
    {
        if (sscanf(s, "%254s %ld", trash, &session_id) != 2)
        {
            fprintf(stderr,"Invalid Session number %s,%i\n", __FILE__, __LINE__);
            send_reply(454, 0, pRtspBuf);              /* 没有此会话*/
            return;
        }
    }

    /*打开会话列表*/
    pRtspSess = pRtspBuf->session_list;
    if (pRtspSess == NULL)
    {
        return;
    }

#ifdef RTSP_DEBUG
    sprintf(szDebug,"state_machine:current state is  ");
    strcat(szDebug,((pRtspSess->cur_state==0)?"init state":((pRtspSess->cur_state==1)?"ready state":"play state")));
    printf("%s\n", szDebug);
#endif

    /*根据状态迁移规则，从当前状态开始迁移*/
    switch (pRtspSess->cur_state)
    {
        case INIT_STATE:                    /*初始态*/
        {
#ifdef RTSP_DEBUG
        	fprintf(stderr,"current method code is:  %d  \n",method);
#endif
            switch (method)
            {
                case RTSP_ID_DESCRIBE:  //状态不变
                    RTSP_describe(pRtspBuf);
					//printf("3\r\n");
                    break;

                case RTSP_ID_SETUP:                //状态变为就绪态
					//printf("4\r\n");
                  if (RTSP_setup(pRtspBuf) == ERR_NOERROR)
                    {
						//printf("5\r\n");
                    	pRtspSess->cur_state = READY_STATE;
                        fprintf(stderr,"TRANSFER TO READY STATE!\n");
                    }
                    break;

                case RTSP_ID_TEARDOWN:       //状态不变
                    RTSP_teardown(pRtspBuf);
                    break;

                case RTSP_ID_OPTIONS:
                    if (RTSP_options(pRtspBuf) == ERR_NOERROR)
                    {
                    	pRtspSess->cur_state = INIT_STATE;         //状态不变
                    }
                    break;

                case RTSP_ID_PLAY:          //method not valid this state.

                case RTSP_ID_PAUSE:
                    send_reply(455, 0, pRtspBuf);
                    break;

                default:
                    send_reply(501, 0, pRtspBuf);
                    break;
            }
        break;
        }

        case READY_STATE:
        {
#ifdef RTSP_DEBUG
            fprintf(stderr,"current method code is:%d\n",method);
#endif

            switch (method)
            {
                case RTSP_ID_PLAY:                                      //状态迁移为播放态
                   if (RTSP_play(pRtspBuf) == ERR_NOERROR)
                    {
                        fprintf(stderr,"\tStart Playing!\n");
                        pRtspSess->cur_state = PLAY_STATE;
                    }
                    break;

                case RTSP_ID_SETUP:
                    if (RTSP_setup(pRtspBuf) == ERR_NOERROR)    //状态不变
                    {
                        pRtspSess->cur_state = READY_STATE;
                    }
                    break;

                case RTSP_ID_TEARDOWN:
                    RTSP_teardown(pRtspBuf);                 //状态变为初始态 ?
                    break;

                case RTSP_ID_OPTIONS:
                    if (RTSP_options(pRtspBuf) == ERR_NOERROR)
                    {
                        pRtspSess->cur_state = INIT_STATE;          //状态不变
                    }
                    break;

                case RTSP_ID_PAUSE:         			// method not valid this state.
                    send_reply(455, 0, pRtspBuf);
                    break;

                case RTSP_ID_DESCRIBE:
                    RTSP_describe(pRtspBuf);
                    break;

                default:
                    send_reply(501, 0, pRtspBuf);
                    break;
            }

            break;
        }


        case PLAY_STATE:
        {
            switch (method)
            {
                case RTSP_ID_PLAY:
                    // Feature not supported
                    fprintf(stderr,"UNSUPPORTED: Play while playing.\n");
                    send_reply(551, 0, pRtspBuf);        // Option not supported
                    break;
/*				//不支持暂停命令
                case RTSP_ID_PAUSE:              	//状态变为就绪态
                    if (RTSP_pause(pRtspBuf) == ERR_NOERROR)
                    {
                    	pRtspSess->cur_state = READY_STATE;
                    }
                    break;
*/
                case RTSP_ID_TEARDOWN:
                    RTSP_teardown(pRtspBuf);        //状态迁移为初始态
                    break;

                case RTSP_ID_OPTIONS:
                    break;

                case RTSP_ID_DESCRIBE:
                    RTSP_describe(pRtspBuf);
                    break;

                case RTSP_ID_SETUP:
                    break;
            }

            break;
        }/* PLAY state */

        default:
            {
                /* invalid/unexpected current state. */
                fprintf(stderr,"%s State error: unknown state=%d, method code=%d\n", __FUNCTION__, pRtspSess->cur_state, method);
            }
            break;
    }/* end of current state switch */

#ifdef RTSP_DEBUG
    printf("leaving rtsp_state_machine!\n");
#endif
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void RTSP_remove_msg(int len, RTSP_buffer * rtsp)
{
    rtsp->in_size -= len;
    if (rtsp->in_size && len)
    {
        //删除指定长度的消息
        memmove(rtsp->in_buffer, &(rtsp->in_buffer[len]), RTSP_BUFFERSIZE - len);
        memset(&(rtsp->in_buffer[RTSP_BUFFERSIZE - len]), 0, len);
    }
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void RTSP_discard_msg(RTSP_buffer * rtsp)
{
    int hlen, blen;

#ifdef RTSP_DEBUG
//	trace_point();
#endif

    //找出缓冲区中首个消息的长度，然后删除
    if (RTSP_full_msg_rcvd(rtsp, &hlen, &blen) > 0)
        RTSP_remove_msg(hlen + blen, rtsp);
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int RTSP_handler(RTSP_buffer *pRtspBuf)
{
	int s32Meth;

#ifdef RTSP_DEBUG
//	trace_point();
#endif

	while(pRtspBuf->in_size)
	{
		s32Meth = RTSP_validate_method(pRtspBuf);
		if (s32Meth < 0)
		{
			//错误的请求，请求的方法不存在
			fprintf(stderr,"Bad Request %s,%d\n", __FILE__, __LINE__);
			printf("bad request, requestion not exit %d",s32Meth);
			send_reply(400, NULL, pRtspBuf);
		}
		else
		{
			//进入到状态机，处理接收的请求
			RTSP_state_machine(pRtspBuf, s32Meth);
			printf("exit Rtsp_state_machine\r\n");
		}
		//丢弃处理之后的消息
		RTSP_discard_msg(pRtspBuf);
		printf("4\r\n");
	}
	return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
int RtspServer(RTSP_buffer *rtsp)
{
	fd_set rset,wset;       /*读写I/O描述集*/
	struct timeval t;
	int size;
	static char buffer[RTSP_BUFFERSIZE+1]; /* +1 to control the final '\0'*/
	int n;
	int res;
	struct sockaddr ClientAddr;

#ifdef RTSP_DEBUG
//    fprintf(stderr, "%s, %d\n", __FUNCTION__, __LINE__);
#endif

//    memset((void *)&ClientAddr,0,sizeof(ClientAddr));

	if (rtsp == NULL)
	{
		return ERR_NOERROR;
	}

	/*变量初始化*/
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	t.tv_sec=0;				/*select 时间间隔*/
	t.tv_usec=100000;

	FD_SET(rtsp->fd,&rset);

	/*调用select等待对应描述符变化*/
	if (select(g_s32Maxfd+1,&rset,0,0,&t)<0)
	{
		fprintf(stderr,"select error %s %d\n", __FILE__, __LINE__);
		send_reply(500, NULL, rtsp);
		return ERR_GENERIC; //errore interno al server
	}

	/*有可供读进的rtsp包*/
	if (FD_ISSET(rtsp->fd,&rset))
	{
		memset(buffer,0,sizeof(buffer));
		size=sizeof(buffer)-1;  /*最后一位用于填充字符串结束标识*/

		/*读入数据到缓冲区中*/
#ifdef RTSP_DEBUG
//    fprintf(stderr, "tcp_read, %d\n", __LINE__);
#endif
		n= tcp_read(rtsp->fd, buffer, size, &ClientAddr);
		if (n==0)
		{
			return ERR_CONNECTION_CLOSE;
		}

		if (n<0)
		{
			fprintf(stderr,"read() error %s %d\n", __FILE__, __LINE__);
			send_reply(500, NULL, rtsp);                //服务器内部错误消息
			return ERR_GENERIC;
		}

		//检查读入的数据是否产生溢出
		if (rtsp->in_size+n>RTSP_BUFFERSIZE)
		{
			fprintf(stderr,"RTSP buffer overflow (input RTSP message is most likely invalid).\n");
			send_reply(500, NULL, rtsp);
			return ERR_GENERIC;//数据溢出错误
		}

#ifdef RTSP_DEBUG
		fprintf(stderr,"INPUT_BUFFER was:%s\n", buffer);
#endif

		/*填充数据*/
		memcpy(&(rtsp->in_buffer[rtsp->in_size]),buffer,n);
		rtsp->in_size+=n;
		//清空buffer
		memset(buffer, 0, n);
		//添加客户端地址信息
		memcpy(	&rtsp->stClientAddr, &ClientAddr, sizeof(ClientAddr));

		/*处理缓冲区的数据，进行rtsp处理*/
		if ((res=RTSP_handler(rtsp))==ERR_GENERIC)
		{
			fprintf(stderr,"Invalid input message.\n");
			return ERR_NOERROR;
		}
	}

	/*有发送数据*/
	if (rtsp->out_size>0)
	{
		//将数据发送出去
		n= tcp_write(rtsp->fd,rtsp->out_buffer,rtsp->out_size);
		printf("5\r\n");
		if (n<0)
		{
			fprintf(stderr,"tcp_write error %s %i\n", __FILE__, __LINE__);
			send_reply(500, NULL, rtsp);
			return ERR_GENERIC; //errore interno al server
		}

#ifdef 	RTSP_DEBUG
		//fprintf(stderr,"OUTPUT_BUFFER length %d\n%s\n", rtsp->out_size, rtsp->out_buffer);
#endif
		//清空发送缓冲区
		memset(rtsp->out_buffer, 0, rtsp->out_size);
		rtsp->out_size = 0;
	}


	//如果需要RTCP在此出加入对RTCP数据的接收，并存放在缓存中。
	//继而在schedule_do线程中对其处理。
	//rtcp控制处理,检查读入RTCP数据报


	return ERR_NOERROR;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void ScheduleConnections(RTSP_buffer **rtsp_list, int *conn_count)
{
    int res;
    RTSP_buffer *pRtsp=*rtsp_list,*pRtspN=NULL;
    RTP_session *r=NULL, *t=NULL;

#ifdef RTSP_DEBUG
//    fprintf(stderr, "%s\n", __FUNCTION__);
#endif

    while (pRtsp!=NULL)
    {
        if ((res = RtspServer(pRtsp))!=ERR_NOERROR)
        {
            if (res==ERR_CONNECTION_CLOSE || res==ERR_GENERIC)
            {
                /*连接已经关闭*/
                if (res==ERR_CONNECTION_CLOSE)
                    fprintf(stderr,"fd:%d,RTSP connection closed by client.\n",pRtsp->fd);
                else
                	fprintf(stderr,"fd:%d,RTSP connection closed by server.\n",pRtsp->fd);

                /*客户端在发送TEARDOWN 之前就截断了连接，但是会话却没有被释放*/
                if (pRtsp->session_list!=NULL)
                {
                    r=pRtsp->session_list->rtp_session;
                    /*释放所有会话*/
                    while (r!=NULL)
                    {
                        t = r->next;
                        RtpDelete((unsigned int)(r->hndRtp));
                        schedule_remove(r->sched_id);
                        r=t;
                    }

                    /*释放链表头指针*/
                    free(pRtsp->session_list);
                    pRtsp->session_list=NULL;

                    g_s32DoPlay--;
					if (g_s32DoPlay == 0) 
					{
						printf("user abort! no user online now resetfifo\n");
						ringreset;
						/* 重新将所有可用的RTP端口号放入到port_pool[MAX_SESSION] 中 */
						RTP_port_pool_init(RTP_DEFAULT_PORT);
					}
                    fprintf(stderr,"WARNING! fd:%d RTSP connection truncated before ending operations.\n",pRtsp->fd);
                }

                // wait for
                close(pRtsp->fd);
                --*conn_count;
                num_conn--;

                /*释放rtsp缓冲区*/
                if (pRtsp==*rtsp_list)
                {
                	//链表第一个元素就出错，则pRtspN为空
					printf("first error,pRtsp is null\n");
                    *rtsp_list=pRtsp->next;
                    free(pRtsp);
                    pRtsp=*rtsp_list;
                }
                else
                {
                	//不是链表中的第一个，则把当前出错任务删除，并把next任务存放在pRtspN(上一个没有出错的任务)
                	//指向的next，和当前需要处理的pRtsp中.
					printf("dell current fd:%d\n",pRtsp->fd);
                	pRtspN->next=pRtsp->next;
                    free(pRtsp);
                    pRtsp=pRtspN->next;
					printf("current next fd:%d\n",pRtsp->fd);
                }

                /*适当情况下，释放调度器本身*/
                if (pRtsp==NULL && *conn_count<0)
                {
                	fprintf(stderr,"to stop cchedule_do thread\n");
                    stop_schedule=1;
                }
            }
            else
            {	
				printf("current fd:%d\n",pRtsp->fd);
            	pRtsp = pRtsp->next;
            }
        }
        else
        {
			//printf("6\r\n");
        	//没有出错
        	//上一个处理没有出错的list存放在pRtspN中,需要处理的任务放在pRtst中
        	pRtspN = pRtsp;
            pRtsp = pRtsp->next;
        }
    }
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void RTP_port_pool_init(int port)
{
    int i;
    s_u32StartPort = port;
    for (i=0; i<MAX_CONNECTION; ++i)
    {
    	s_uPortPool[i] = i+s_u32StartPort;
    }
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void EventLoop(int s32MainFd)
{
//	static unsigned int s32ChdCnt=0;
	static int s32ConCnt = 0;//已经连接的客户端数
	int s32Fd = -1;
	static RTSP_buffer *pRtspList=NULL;
	RTSP_buffer *p=NULL;
	unsigned int u32FdFound;

//	printf("%s\n", __FUNCTION__);
	/*接收连接，创建一个新的socket*/
	if (s32ConCnt!=-1)
	{
		s32Fd= tcp_accept(s32MainFd);
	}

	/*处理新创建的连接*/
	if (s32Fd >= 0)
	{
		/*查找列表中是否存在此连接的socket*/
		for (u32FdFound=0,p=pRtspList; p!=NULL; p=p->next)
		{
			if (p->fd == s32Fd)
			{
				u32FdFound=1;
				break;
			}
		}
		if (!u32FdFound)
		{
			/*创建一个连接，增加一个客户端*/
			if (s32ConCnt<MAX_CONNECTION)
			{
				++s32ConCnt;
				AddClient(&pRtspList,s32Fd);
			}
			else
			{
				fprintf(stderr, "exceed the MAX client, ignore this connecting\n");
				return;
			}
			num_conn++;
			fprintf(stderr, "%s Connection reached: %d\n", __FUNCTION__, num_conn);
		}
	}

	/*对已有的连接进行调度*/
	//printf("7\r\n");
	ScheduleConnections(&pRtspList,&s32ConCnt);
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void IntHandl(int i)
{	
	stop_schedule = 1;
	g_s32Quit = 1;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
char * base64_encode(const unsigned char * bindata, char * base64, int binlength)
{
    int i, j;
    unsigned char current;
    char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    for(i = 0, j = 0 ; i < binlength ; i += 3)
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;

        base64[j++] = base64char[(int)current];

        current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30) ;
        if(i + 1 >= binlength)
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ((unsigned char)(bindata[i+1] >> 4)) & ((unsigned char) 0x0F);
        base64[j++] = base64char[(int)current];


        current = ((unsigned char)(bindata[i+1] << 2)) & ((unsigned char)0x3C) ;
        if(i + 2 >= binlength)
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ((unsigned char)(bindata[i+2] >> 6)) & ((unsigned char) 0x03);
        base64[j++] = base64char[(int)current];

        current = ((unsigned char)bindata[i+2]) & ((unsigned char)0x3F) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return base64;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void base64_encode2(char *in, const int in_len, char *out, int out_len)
{
	static const char *codes ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	int base64_len = 4 * ((in_len+2)/3);
	//if(out_len >= base64_len)
	//	printf("out_len >= base64_len\n");
	char *p = out;
	int times = in_len / 3;
	int i;

	for(i=0; i<times; ++i)
	{
		*p++ = codes[(in[0] >> 2) & 0x3f];
		*p++ = codes[((in[0] << 4) & 0x30) + ((in[1] >> 4) & 0xf)];
		*p++ = codes[((in[1] << 2) & 0x3c) + ((in[2] >> 6) & 0x3)];
		*p++ = codes[in[2] & 0x3f];
		in += 3;
	}
	if(times * 3 + 1 == in_len) 
	{
		*p++ = codes[(in[0] >> 2) & 0x3f];
		*p++ = codes[((in[0] << 4) & 0x30) + ((in[1] >> 4) & 0xf)];
		*p++ = '=';
		*p++ = '=';
	}
	if(times * 3 + 2 == in_len) 
	{
		*p++ = codes[(in[0] >> 2) & 0x3f];
		*p++ = codes[((in[0] << 4) & 0x30) + ((in[1] >> 4) & 0xf)];
		*p++ = codes[((in[1] << 2) & 0x3c)];
		*p++ = '=';
	}
	*p = 0;
}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void UpdateSps(unsigned char *data,int len)
{
	int i;
	if(len>21)
		return ;

	sprintf(psp.base64profileid,"%x%x%x",data[1],data[2],data[3]);//sps[0] 0x67
	#if 0
	///a=fmtp:96 packetization-mode=1;profile-level-id=4b9096;sprop-parameter-sets=8kuQlme8xyAX,+sIKBQ==;
	base64_encode((unsigned char *)data, psp.base64sps, len);///sps
	#else
	//a=fmtp:96 packetization-mode=1;profile-level-id=ee69bc;sprop-parameter-sets=xu5pvGeDzEef,qFT3dm==;
	base64_encode2(data, len, psp.base64sps, 512);
	#endif

}
/**************************************************************************************************
**
**
**
**************************************************************************************************/
void UpdatePps(unsigned char *data,int len)
{
	int i;
	if(len>21)
		return ;
	#if 0
	base64_encode((unsigned char *)data, psp.base64pps, len);//pps
	#else
	char pic1_paramBase64[512] = {0};
	base64_encode2(data, len, psp.base64pps, 512);
	#endif

}


