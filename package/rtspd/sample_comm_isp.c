/******************************************************************************
  Some simple Hisilicon HI3516 isp functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "sample_comm.h"

static pthread_t gs_IspPid;

#ifdef ISP_V1
static HI_U32 u32IspExtBase = 0;

#define  MAX_STRLEN		128
#define  MAX_REGLEN		32
#define  REAL_REGLEN	strlen("0x00000000:0x00000000")
#define ISP_REG_SIZE_CFG	0x1014
#define ISP_EXT_SIZE_CFG	0x0264

#define CHECKREGADDR(regaddr)		\
do{\
	if ((regaddr < (unsigned int)ISP_REG_BASE)){return HI_FAILURE;}\
	else if(regaddr > (unsigned int)(u32IspExtBase + ISP_EXT_SIZE_CFG)){return HI_FAILURE;}\
	else if ((regaddr > (unsigned int)(ISP_REG_BASE + ISP_REG_SIZE_CFG)) && (regaddr < (unsigned int)u32IspExtBase) ){return HI_FAILURE;}\
	else{;};\
}while(0)


HI_VOID SAMPLE_COMM_ISP_CircleWriteReg(HI_U32 u32RegAddr,  FILE* fd)
{
	HI_S32 s32Ret = 0;
	HI_CHAR acRegFmt[MAX_REGLEN];
	HI_U32 u32RegVal = 0;

	s32Ret = HI_MPI_ISP_GetCfg(u32RegAddr, &u32RegVal);
	if (s32Ret != HI_SUCCESS)
	{
		printf("HI_MPI_ISP_SetCfg ERR(%x) Reg%x: 0x%x\n", s32Ret, u32RegAddr, u32RegVal);
	}

	snprintf(acRegFmt, sizeof(acRegFmt), "0x%08x:0x%08x\r\n", u32RegAddr, u32RegVal);
	if (1 != fwrite(acRegFmt, strlen(acRegFmt), 1, fd))
	{
		printf("fwrite ERR regaddr %8x : regval%8x \n", u32RegAddr, u32RegVal);
		return ;
	}
	return ;

}

HI_S32  SAMPLE_COMM_ISP_GetLine(HI_CHAR * pcSrcBuf,HI_S32 s32SrcLen)
{
    HI_S32 i = 0;
    HI_S32 s32LineLen = 0;

    if((pcSrcBuf == NULL)||(s32SrcLen <= 0))
    {
        printf("the buf is nul or len is invalid:%d\n",s32SrcLen);
        return -1;
    }

	i = 0;
	while(i < s32SrcLen)
    {
        if((pcSrcBuf[i] != '\r')&&(pcSrcBuf[i] != '\n'))
        {
            i++;
            s32LineLen ++;
        }
        else if(pcSrcBuf[i] == '\r')
        {
            i++;
            s32LineLen ++;
            if(pcSrcBuf[i] == '\n')
            {
                i++;
                s32LineLen ++;

            }
            break;
        }
        else /*pcSrcBuf[i] == '\n'**/
        {
            i++;
            s32LineLen ++;
            break;
        }
    }
    return s32LineLen;

}

/******************************************************************************
* funciton : isp register to cfg file
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Reg2File(HI_CHAR* pcCfgFile)
{
    FILE* fdcfg = NULL;
    HI_CHAR* pstr = NULL;
    HI_U32 u32RegVal = 0;
    HI_U32 u32RegAddr = 0;
    HI_CHAR strReg[9];
    HI_S32 s32Ret = 0;
    HI_S32 s32NewFlag = 0;
    HI_CHAR RegFmt[18];
    HI_S32 i = 0;
    HI_S32 s32FileLen = 0;
    struct stat filestat;
    HI_CHAR* pbuf = NULL;

    if ((pcCfgFile == NULL) || (strlen(pcCfgFile) == 0))
    {
        printf("%s: Invalid Filename \n", __FUNCTION__);
        return HI_FAILURE;
    }

    printf("pcCfgFile is %s!\n", pcCfgFile);

    if (0 == u32IspExtBase)
    {
		if (HI_MPI_ISP_GetExtRegAddr(&u32IspExtBase))
		{
		    printf("Get ISP extent address failed!\n");
		}
		else
		{
		    printf("Get ISP extent address: %#x\n", u32IspExtBase);
		}
    }

    if (access(pcCfgFile, F_OK) == 0)
    {
        if (stat(pcCfgFile, &filestat) != 0)
        {
            printf("%s: stat err!\n", __FUNCTION__);
            return HI_FAILURE;
        }
        fdcfg = fopen(pcCfgFile, "r+");
        s32NewFlag = 0;
        printf("isp configure file exist !\n");
    }
    else
    {
        fdcfg = fopen(pcCfgFile, "w+");
        s32NewFlag = 1;
        printf("create new isp configure file!!\n");
    }

    if (s32NewFlag)
    {
        u32RegAddr = ISP_REG_BASE;
        for (i = 0; i < (int)(ISP_REG_SIZE_CFG); i+=4)
        {
            SAMPLE_COMM_ISP_CircleWriteReg(u32RegAddr, fdcfg);
            u32RegAddr +=4;
        }

        u32RegAddr = u32IspExtBase;
        for (i = 0; i <= (int)ISP_EXT_SIZE_CFG; i+=4)
        {
            SAMPLE_COMM_ISP_CircleWriteReg(u32RegAddr, fdcfg);
            u32RegAddr +=4;
        }

        fclose(fdcfg);
        chmod(pcCfgFile, 0666);
        return HI_SUCCESS;
    }

    pbuf = (char*)malloc(filestat.st_size);
    if (pbuf == NULL)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        fclose(fdcfg);
        return HI_FAILURE;
    }

    if (fread(pbuf, filestat.st_size, 1, fdcfg) != 1)
    {
        printf("%s: fread failed!\n", __FUNCTION__);
        free(pbuf);
        fclose(fdcfg);
        return HI_FAILURE;
    }

    s32FileLen = 0;
    fseek(fdcfg, 0, SEEK_SET);
    while(s32FileLen < filestat.st_size)
    {
        int s32RdLen = 0;
        char  curbuf[MAX_STRLEN];

        memcpy(curbuf, pbuf+s32FileLen, MAX_STRLEN);
        s32RdLen = SAMPLE_COMM_ISP_GetLine(curbuf, MAX_STRLEN);
        if(s32RdLen<= 0)
        {
            printf("%s: the file is not the register value!\n", __FUNCTION__);
            free(pbuf);
            return HI_FAILURE;
        }
        curbuf[s32RdLen] ='\0';
        s32FileLen +=s32RdLen;

        if (curbuf[0] == '#')
        {
            fwrite(curbuf, s32RdLen, 1, fdcfg);
            continue;
        }
        else if ((pstr = strchr(curbuf, ':')) != NULL )
        {
            strncpy(strReg, pstr+1, sizeof(strReg));
            strReg[9] = '\0';

            u32RegVal = strtoll(strReg, NULL, 16);

            u32RegAddr = strtoll(curbuf, NULL, 16);

            CHECKREGADDR(u32RegAddr);

            s32Ret = HI_MPI_ISP_GetCfg(u32RegAddr, &u32RegVal);
            if (s32Ret != HI_SUCCESS)
            {
                printf("%s: HI_MPI_ISP_SetCfg Reg%x: 0x%x failed with %#x!\n", __FUNCTION__, u32RegAddr, u32RegVal, s32Ret);
                goto END;
            }

            snprintf(RegFmt, sizeof(RegFmt), "0x%08x:0x%08x", u32RegAddr, u32RegVal);

            memcpy(&curbuf[0], RegFmt, strlen(RegFmt));
            s32Ret = fwrite(curbuf, s32RdLen, 1, fdcfg);
            if (1 != s32Ret)
            {
                printf("%s: fwrite err \n", __FUNCTION__);
                goto END;
            }
            continue;
        }
        else
        {
            fwrite(curbuf, s32RdLen, 1, fdcfg);
            continue;
        }
    }
    s32Ret = HI_SUCCESS;
END:
    free(pbuf);
    fclose(fdcfg);
    return s32Ret;
}

/******************************************************************************
* funciton : isp cfg file to register
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_File2Reg(HI_CHAR* pCfgFile)
{
    FILE* fdcfg = NULL;
    HI_CHAR* pstr = NULL;
    HI_U32 u32RegVal = 0;
    HI_U32 u32RegAddr = 0;
    HI_CHAR strReg[REAL_REGLEN+1];
    HI_S32 s32Ret = 0;
    HI_S32 s32FileLen = 0;
    struct stat filestat;
    HI_CHAR* pbuf = NULL;

    if ((pCfgFile == NULL) || (strlen(pCfgFile) == 0))
    {
        printf("%s: Invalid Filename!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    if (0 == u32IspExtBase)
    {
		if (HI_MPI_ISP_GetExtRegAddr(&u32IspExtBase))
		{
		    printf("Get ISP extent address failed!\n");
		}
		else
		{
		    printf("Get ISP extent address: %#x\n", u32IspExtBase);
		}
    }

    if (stat(pCfgFile, &filestat) != 0)
    {
        printf("%s: stat error!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    fdcfg = fopen(pCfgFile, "r");
    if (fdcfg == NULL)
    {
        printf("%s: fopen failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    pbuf = (char*)malloc(filestat.st_size);
    if (pbuf == NULL)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        fclose(fdcfg);
        return HI_FAILURE;
    }

    if (fread(pbuf, filestat.st_size, 1, fdcfg) != 1)
    {
        printf("%s: fread failed!\n", __FUNCTION__);
        free(pbuf);
        fclose(fdcfg);
        return HI_FAILURE;
    }
    fclose(fdcfg);

    s32FileLen = 0;
    while(s32FileLen < filestat.st_size)
    {
        HI_S32 s32RdLen = 0;
        HI_CHAR curbuf[MAX_STRLEN];

        memcpy(curbuf, pbuf+s32FileLen, MAX_STRLEN);
        s32RdLen = SAMPLE_COMM_ISP_GetLine(curbuf, MAX_STRLEN);
        if(s32RdLen<= 0)
        {
            printf("%s: the file is not the register value\n", __FUNCTION__);
            free(pbuf);
            return HI_FAILURE;
        }
        curbuf[s32RdLen] ='\0';
        s32FileLen +=s32RdLen;

        if (curbuf[0] == '#')
        {
            continue;
        }
        else if ((curbuf[0] == '\n') ||(curbuf[1] == '\n'))
        {
            continue;
        }
        else if((pstr=strchr(curbuf, ':') )!= NULL)
        {
            strncpy(strReg, pstr+1, sizeof(strReg));
            strReg[REAL_REGLEN] = '\0';

            u32RegVal = strtol(strReg, NULL, 16);
            u32RegAddr = strtoll(curbuf, NULL, 16);

            CHECKREGADDR(u32RegAddr);

            s32Ret = HI_MPI_ISP_SetCfg(u32RegAddr, u32RegVal);
            if (s32Ret != HI_SUCCESS)
            {
                printf("%s: HI_MPI_ISP_SetCfg Reg%x: 0x%x failed with %#x!\n", __FUNCTION__, u32RegAddr, u32RegVal, s32Ret);
                free(pbuf);
                return HI_FAILURE;
            }
        }
        else
        {
            continue;
        }
    }

    free(pbuf);

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : load isp regist define file
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_LoadRegFile(SAMPLE_CFG_OPT_E enCfgOpt, HI_CHAR *pcIspCfgFile)
{
    if (enCfgOpt == CFG_OPT_LOAD)
    {
        if (HI_SUCCESS != SAMPLE_COMM_ISP_File2Reg(pcIspCfgFile))
        {
            printf("%s: Load isp configure file failed!\n", __FUNCTION__);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Export isp regist define file
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_SaveRegFile(SAMPLE_CFG_OPT_E enCfgOpt, HI_CHAR *pcIspCfgFile)
{
    if (enCfgOpt == CFG_OPT_SAVE)
    {
        if (HI_SUCCESS != SAMPLE_COMM_ISP_Reg2File(pcIspCfgFile))
        {
            printf("%s: Export isp configure file failed!\n", __FUNCTION__);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}
#endif

/******************************************************************************
* funciton : Sensor init
* note : different sensor corresponding different lib. So,  you can change
		  SENSOR_TYPE in Makefile.para, instead of modify program code.
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_SensorInit(void)
{
    HI_S32 s32Ret;
#ifdef ISP_V1
    /* 1. sensor init */
    sensor_init();

    /* 0: linear mode, 1: WDR mode */
    sensor_mode_set(0);
#endif
    /* 2. sensor register callback */
    s32Ret = sensor_register_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Init(void)
{
    HI_S32 s32Ret;
    ISP_IMAGE_ATTR_S stImageAttr;
    ISP_INPUT_TIMING_S stInputTiming;
#ifdef ISP_V2
    ALG_LIB_S stLib;

    /* 1. register ae lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_Register(&stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AE_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 2. register awb lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_Register(&stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AWB_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 3. register af lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AF_LIB_NAME);
    s32Ret = HI_MPI_AF_Register(&stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AF_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }
#endif
    /* 4. isp init */
    s32Ret = HI_MPI_ISP_Init();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_Init failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 5. isp set image attributes */
    /* note : different sensor, different ISP_IMAGE_ATTR_S define.
              if the sensor you used is different, you can change
              ISP_IMAGE_ATTR_S definition */

    switch(SENSOR_TYPE)
    {
        case APTINA_9M034_DC_720P_30FPS:
        case APTINA_AR0130_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;

        case SONY_ICX692_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;

        case SONY_IMX104_DC_720P_30FPS:
        case SONY_IMX138_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_GBRG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;

        case OMNI_OV9712_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 25;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;
			
       case OMNI_OV2710_DC_1080P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1920;
            stImageAttr.u16Height    = 1080;
            break;

       case SOI_H22_DC_720P_30FPS:
       case PIXELPLUS_3100K_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;
			
        case HIMAX_1375_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;	
			
        case SONY_IMX122_DC_1080P_30FPS:
        case SONY_IMX236_DC_1080P_30FPS:
            stImageAttr.enBayer      = BAYER_RGGB;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1920;
            stImageAttr.u16Height    = 1080;
            break;
            
        case APTINA_MT9P006_DC_1080P_30FPS:
        case APTINA_AR0330_DC_1080P_30FPS:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1920;
            stImageAttr.u16Height    = 1080;
            break;

        default:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;
    }

    s32Ret = HI_MPI_ISP_SetImageAttr(&stImageAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetImageAttr failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 6. isp set timing */
    switch(SENSOR_TYPE)
    {
        case APTINA_9M034_DC_720P_30FPS:
        case APTINA_AR0130_DC_720P_30FPS:
        case OMNI_OV9712_DC_720P_30FPS:
        case OMNI_OV2710_DC_1080P_30FPS:
        case SOI_H22_DC_720P_30FPS:
        case PIXELPLUS_3100K_DC_720P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_NONE;
            break;

        case HIMAX_1375_DC_720P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL; //for sony or pana
            stInputTiming.u16HorWndLength = 1280;
            stInputTiming.u16HorWndStart = 0;
            stInputTiming.u16VerWndLength = 720;
            stInputTiming.u16VerWndStart = 4;
            break;
			
        case SONY_ICX692_DC_720P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL; //for sony or pana
        	stInputTiming.u16HorWndLength = 1280;
        	stInputTiming.u16HorWndStart = 200;
        	stInputTiming.u16VerWndLength = 720;
        	stInputTiming.u16VerWndStart = 2;
            break;

        case SONY_IMX104_DC_720P_30FPS:
        case SONY_IMX138_DC_720P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL;
            stInputTiming.u16HorWndStart = 72;
            stInputTiming.u16HorWndLength = 1280;
            stInputTiming.u16VerWndStart = 20;
            stInputTiming.u16VerWndLength = 720;
            break;

        case SONY_IMX122_DC_1080P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL;
            stInputTiming.u16HorWndStart = 200;
            stInputTiming.u16HorWndLength = 1920;
            stInputTiming.u16VerWndStart = 18;
            stInputTiming.u16VerWndLength = 1080;
            break;
            
        case SONY_IMX236_DC_1080P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL;
            stInputTiming.u16HorWndStart = 96;
            stInputTiming.u16HorWndLength = 1920;
            stInputTiming.u16VerWndStart = 12;
            stInputTiming.u16VerWndLength = 1080;
            break;
            
        case APTINA_MT9P006_DC_1080P_30FPS:
        case APTINA_AR0330_DC_1080P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL;
            stInputTiming.u16HorWndStart = 0;
            stInputTiming.u16HorWndLength = 1920;
            stInputTiming.u16VerWndStart = 2;
            stInputTiming.u16VerWndLength = 1080;
            break;
            
        default:
            printf("%s: SENSOR_TYPE %d not support!\n", __FUNCTION__, SENSOR_TYPE);
			stInputTiming.enWndMode = ISP_WIND_NONE;
            break;
    }
    s32Ret = HI_MPI_ISP_SetInputTiming(&stInputTiming);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetInputTiming failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Run()
{
    HI_S32 s32Ret;

    s32Ret = SAMPLE_COMM_ISP_Init();
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: ISP init failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }

#if 1
    if (0 != pthread_create(&gs_IspPid, 0, (void* (*)(void*))HI_MPI_ISP_Run, NULL))
    {
        printf("%s: create isp running thread failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }
#else
	/* configure thread priority */
	if (1)
	{
		#include <sched.h>

		pthread_attr_t attr;
		struct sched_param param;
		int newprio = 50;

		pthread_attr_init(&attr);

		if (1)
		{
			int policy = 0;
			int min, max;

			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->default thread use policy is %d --<\n", policy);

			pthread_attr_setschedpolicy(&attr, SCHED_RR);
			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->current thread use policy is %d --<\n", policy);

			switch (policy)
			{
				case SCHED_FIFO:
					printf("-->current thread use policy is SCHED_FIFO --<\n");
					break;

				case SCHED_RR:
					printf("-->current thread use policy is SCHED_RR --<\n");
					break;

				case SCHED_OTHER:
					printf("-->current thread use policy is SCHED_OTHER --<\n");
					break;

				default:
					printf("-->current thread use policy is UNKNOW --<\n");
					break;
			}

			min = sched_get_priority_min(policy);
			max = sched_get_priority_max(policy);

			printf("-->current thread policy priority range (%d ~ %d) --<\n", min, max);
		}

		pthread_attr_getschedparam(&attr, &param);

		printf("-->default isp thread priority is %d , next be %d --<\n", param.sched_priority, newprio);
		param.sched_priority = newprio;
		pthread_attr_setschedparam(&attr, &param);

		if (0 != pthread_create(&gs_IspPid, &attr, (void* (*)(void*))HI_MPI_ISP_Run, NULL))
		{
			printf("%s: create isp running thread failed!\n", __FUNCTION__);
			return HI_FAILURE;
		}

		pthread_attr_destroy(&attr);
	}
#endif

#ifdef ISP_V1
    /* load configure file if there is */
#ifdef SAMPLE_LOAD_ISPREGFILE
    s32Ret = SAMPLE_COMM_ISP_LoadRegFile(CFG_OPT_LOAD, SAMPLE_ISPCFG_FILE);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: load isp cfg file failed![%s]\n", __FUNCTION__, SAMPLE_ISPCFG_FILE);
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : change between linear and wdr mode
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_ChangeSensorMode(HI_U8 u8Mode)
{
    HI_S32 s32Ret;
#ifdef ISP_V1    
    SAMPLE_COMM_ISP_Stop();
    
    s32Ret = sensor_mode_set(u8Mode);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_mode_set failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    s32Ret = sensor_register_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    s32Ret = SAMPLE_COMM_ISP_Run();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: SAMPLE_COMM_ISP_Run failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
#endif
#ifdef ISP_V2
    ISP_WDR_ATTR_S stWdrAttr;
    stWdrAttr.enWdrMode = u8Mode;
    s32Ret = HI_MPI_ISP_SetWdrAttr(&stWdrAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetWdrAttr failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
#endif
    return HI_SUCCESS;
}


/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
HI_VOID SAMPLE_COMM_ISP_Stop()
{
    HI_MPI_ISP_Exit();
    pthread_join(gs_IspPid, 0);
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

