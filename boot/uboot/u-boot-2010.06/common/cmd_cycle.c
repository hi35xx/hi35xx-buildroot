/*
 * Command for compress.
 */

#include <common.h>


typedef unsigned char           HI_U8;
typedef unsigned char           HI_UCHAR;
typedef unsigned short          HI_U16;
typedef unsigned int            HI_U32;

typedef char                    HI_S8;
typedef short                   HI_S16;
typedef int                     HI_S32;

typedef char                    HI_CHAR;
typedef char*                   HI_PCHAR;
#define HI_NULL (0)

#define UNCOMPRESSED_SIZE_OFFSET    (0x1000)

#define CYCLE_MAGIC_HEAD            (0x6379636c)

#define CYCLE_MAGIC_ITEM_START      (0x63796373)

#define CYCLE_MAGIC_ITEM_END        (0x63796365)

typedef struct hiCYCLE_HEAD_S
{
    HI_U32 u32MagicHead;
    HI_U32 u32CycleFlashSize; //include cycle_head size
    HI_U32 u32Compress;
    HI_U32 u32AlignSize;
} HI_CYCLE_HEAD_S;


typedef struct hiCYCLE_ITEM_START_S
{
    HI_U32 u32MagicItemStart;
    HI_U32 u32ItemLen;
    HI_U32 u32ItemAllLen;
    HI_U32 u32ItemOriginLen;
} HI_CYCLE_ITEM_START_S;

#define DIVIDE (4)
#define BYTE_ALIGN ((HI_U32)16)  // needed by decompress

extern unsigned int hw_dec_type;
extern void hw_dec_init(void);
extern void hw_dec_uinit(void);
extern int hw_dec_decompress(unsigned char *dst, int *dstlen,
			unsigned char *src, int srclen,
			void *unused);\

static HI_S32 GetCycleData(unsigned long Src, unsigned long Src_Backup, HI_U32 u32Src_Len, unsigned long dest);

static int do_cycle(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    unsigned long src, src_backup,dst;
    unsigned int src_len;
    /* need four arguments */
    if (argc != 5)
        goto usage;

    src = simple_strtoul(argv[1], NULL, 16);
    src_backup = simple_strtoul(argv[2], NULL, 16);
    src_len = simple_strtoul(argv[3], NULL, 16);
    dst = simple_strtoul(argv[4], NULL, 16);

    if(src & 0XF) {
        printf("ERR:\n    src[0X%08lx] is not 16Byte-aligned!\n", src);
        return 1;
    }

    if(src_backup & 0XF) {
        printf("ERR:\n    src_backup[0X%08lx] is not 16Byte-aligned!\n", src_backup);
        return 1;
    }

    if(src_len & 0XFFFF) {
        printf("ERR:\n    src_len[0X%08x] is not 0x10000Byte-aligned!\n", src_len);
        return 1;
    }

    if(dst & 0XF) {
        printf("ERR:\n    dst[0X%08lx] is not 16Byte-aligned!\n", dst);
        return 1;
    }

    HI_S32 s32Ret = GetCycleData(src,src_backup,src_len,dst);

    return s32Ret;

usage:
    cmd_usage(cmdtp);
    return 1;
}



static HI_S32 Cycle_Get_InitData(HI_U8* pcBuffer, HI_U32 u32SrcLen ,unsigned long *pnDest,HI_U32 *pu32Compress,HI_CYCLE_HEAD_S **pstHead)
{
    HI_CYCLE_ITEM_START_S* pstItem = HI_NULL;

    *pstHead = (HI_CYCLE_HEAD_S*)pcBuffer;

    if ((*pstHead)->u32MagicHead != CYCLE_MAGIC_HEAD || (*pstHead)->u32CycleFlashSize == 0 || (*pstHead)->u32AlignSize == 0)
    {
        printf("\nfunc: %s  line :%d pstHead->u32MagicHead: 0x%x\n",__func__,__LINE__,(*pstHead)->u32MagicHead);
        return -1;
    }


    *pu32Compress = (*pstHead)->u32Compress;

    HI_U32 u32Len = (*pstHead)->u32CycleFlashSize;

    if (u32SrcLen < (*pstHead)->u32CycleFlashSize)
    {
        printf("!ERR: srclen(%d) is too short. CycleFlashSize(%d)\n  ",u32SrcLen,(*pstHead)->u32CycleFlashSize);

        return -1;
    }
    HI_U8* pcEnd = pcBuffer + u32Len;

    pcBuffer += sizeof(HI_CYCLE_HEAD_S);

    pcBuffer = (HI_U8 *)(((HI_U32)pcBuffer + (*pstHead)->u32AlignSize - 1) & ~((*pstHead)->u32AlignSize - 1));

    pstItem = (HI_CYCLE_ITEM_START_S*)pcBuffer;

    if ((pstItem->u32ItemLen == 0) || ((pstItem->u32ItemLen % BYTE_ALIGN) != 0) )
    {
        printf("\nfunc: %s  line :%d pstItem->u32ItemLen: 0x%x\n",__func__,__LINE__,pstItem->u32ItemLen);
        return -1;
    }

    HI_U32 u32MagicEnd = *(HI_U32 *)((HI_U32)pcBuffer + (pstItem->u32ItemLen + sizeof(HI_CYCLE_ITEM_START_S)));


    if ((pstItem->u32MagicItemStart != CYCLE_MAGIC_ITEM_START)
        || (pstItem->u32ItemAllLen >= (u32Len / DIVIDE) )
        || (u32MagicEnd != CYCLE_MAGIC_ITEM_END))
    {
        printf("\nfunc: %s  line :%d [%x][%d][%x][%d] \n",__func__,__LINE__,pstItem->u32MagicItemStart,pstItem->u32ItemLen,u32MagicEnd,u32Len);
        return -1;
    }

    while (1)
    {
        *pnDest = (unsigned long)pcBuffer;

        pcBuffer += pstItem->u32ItemAllLen;

        pstItem = (HI_CYCLE_ITEM_START_S*)pcBuffer;

        if (pcBuffer >= pcEnd)
        {
            return 0;
        }
        if (pstItem->u32MagicItemStart != CYCLE_MAGIC_ITEM_START)
        {
            if (pstItem->u32MagicItemStart == 0xffffffff)
            {
                return 0;
            }
            else
            {
                printf("\nfunc: %s  line :%d pstItem->u32MagicItemStart(0x%x) wrong!\n",__func__,__LINE__,pstItem->u32MagicItemStart);
                return -1;
            }
        }

        u32MagicEnd = *(HI_U32 *)((HI_U32)pcBuffer + (pstItem->u32ItemLen + sizeof(HI_CYCLE_ITEM_START_S)));

        if ((pstItem->u32ItemLen >= (u32Len / DIVIDE) )
            || (u32MagicEnd != CYCLE_MAGIC_ITEM_END))
        {
            printf("\nerror !func: %s  line :%d \n",__func__,__LINE__);
            return -1;
        }
    }

    return -1;
}

static HI_S32 Cycle_Get_Data(unsigned long Src, HI_U32 u32Src_Len, unsigned long dest)
{
    HI_S32 s32Ret = -1;

    HI_U32 nCompressed = 0;
    unsigned long nTempAddr = 0;
    int  size_uncomparessed = 0;

    HI_CYCLE_HEAD_S *pstHead = HI_NULL;

    s32Ret = Cycle_Get_InitData((HI_U8*)Src, u32Src_Len, &nTempAddr,&nCompressed,&pstHead);

    if (s32Ret == 0 && nTempAddr != 0)
    {
        HI_CYCLE_ITEM_START_S *pstItem = (HI_CYCLE_ITEM_START_S *)nTempAddr;

        nTempAddr += sizeof(HI_CYCLE_ITEM_START_S);

        if (nCompressed)
        {
            /*use direct address mode*/
            hw_dec_type=0;
            /*init hw decompress IP*/
            hw_dec_init();
            /*start decompress*/

            size_uncomparessed = (pstItem->u32ItemOriginLen + UNCOMPRESSED_SIZE_OFFSET );

            s32Ret = hw_dec_decompress((unsigned char *)dest, &size_uncomparessed, \
                    (HI_U8 *)(nTempAddr), pstItem->u32ItemLen, NULL);

            if (s32Ret == 0 && size_uncomparessed == pstItem->u32ItemOriginLen)
            {
                printf("decompress ok!\n");
                s32Ret = 0;
            }
            else
            {
                memset((void *)dest,0,16);
                printf("ERR:\n    decompress fail! ret: 0x%x size_uncomparessed: 0x%x\n",s32Ret,size_uncomparessed);

                s32Ret = -1;
            }

            /*uinit hw decompress IP*/
            hw_dec_uinit();
        }
        else
        {
            memcpy((void *)dest,(void *)nTempAddr,pstItem->u32ItemLen);
            s32Ret = 0;
        }
    }
    else
    {
        memset((void *)dest,0,16);
        printf("Failed to get cycle data. dst: 0x%lx\n",dest);
        s32Ret = -1;
    }

    return s32Ret;
}

static HI_S32 GetCycleData(unsigned long Src, unsigned long Src_Backup, HI_U32 u32Src_Len, unsigned long dest)
{
    HI_S32 s32Ret = Cycle_Get_Data(Src, u32Src_Len, dest);

    if (s32Ret == -1 && Src_Backup != 0)
    {
        s32Ret = Cycle_Get_Data(Src_Backup, u32Src_Len, dest);
    }

    return s32Ret;
}


U_BOOT_CMD(
    cread,	5,	1,	do_cycle,
    "get valid data from cycle_data buffer. 'cycle <src> <src_backup>  <src_len> <dst>'",
    "1. src_backup can be 0.  2. if src and src_backup are wrong, dst head (16 byte) will be set to 0.  3. src and dst must be 16Byte-aligned"
);





