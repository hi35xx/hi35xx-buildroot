/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : aio_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/5/5
  Description   :
  History       :
  1.Date        : 2009/5/5
    Author      : p00123320
    Modification: Created file
******************************************************************************/


#ifndef __AIAO_EXT_H__
#define __AIAO_EXT_H__

typedef struct hiAIO_DRV_DEV_CTX_S
{
    struct
    {
        HI_U32          u32BufSize;         /*循环buf的大小*/
        HI_U32          u32PhyAddr;         /*循环buf物理起始地址*/
        HI_U8*          pu8VirAddr;         /*循环buf虚拟起始地址*/

        HI_U32          u32RptrOffSet;         /*循环buf读指针，记录读指针相对于起始地址的偏移量*/
        HI_U32          u32WptrOffSet;         /*循环buf写指针，记录写指针相对于起始地址的偏移量*/

    } stCirBuf;

    AIO_ATTR_S          stAioAttr;
    HI_BOOL             bEnable;

    HI_U64              u64LastPts;         /*上帧时间戳*/
    HI_U32              u32FrmTime;         /*帧时间间隔*/
    HI_U32              u32MaxFrmTime;      /*帧之间最大时间间隔*/
    HI_U32              u32IsrTime;         /*中断处理时间*/
    HI_U32              u32MaxIsrTime;      /*中断处理历史最长时间*/
    HI_U32              u32AioFifoLen;      /*AIO FIFO 宽度,一次DMA传输的音频数据，u32AioFifoLen个字节，最小为8，最大为32*/
    HI_U32              u32FifoLenBase;     /*AIO FIFO 宽度的2的幂级，例如8是2的三次幂(用于通道分离合并算法的移位操作)*/
    HI_U32              u32FifoShift;       /*AIO FIFO中数据移位(标准PCM需要移位1位)*/
    HI_U32              u32TransLen;        /*总的dma传输长度,byte为单位(采样点个数*u32AioFifoLen) */

    HI_S32              as32ChnIndex[AIO_MAX_CHN_NUM];

    HI_U32              u32IntCnt;
    HI_U32              u32fifoIntCnt;          /*中断数目，调试用*/
    HI_U32              u32buffIntCnt;          /*中断数目，调试用*/
    AUDIO_TRACK_MODE_E  enTrackMode;
    HI_BOOL             bMute;
    AUDIO_FADE_S        stFade;
    HI_S32              s32Volume;
	HI_BOOL             bMicInl;
	HI_BOOL             bMicInr;
}AIO_DRV_DEV_CTX_S;

#endif
