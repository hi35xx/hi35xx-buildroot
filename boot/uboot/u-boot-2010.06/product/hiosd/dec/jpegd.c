#include <common.h>
#include <command.h>
#include <malloc.h>
//#include <devices.h>
#include <version.h>
#include <net.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include <asm/sizes.h>
#include <config.h>

unsigned long jpeg_size=0;

typedef unsigned int UINT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;

extern UINT32  LoadJpegFile(void *pImg);
extern unsigned long ImgWidth, ImgHeight;

const char hilogo_magic_str[] = "HISILICON LOGO MAGIC";
unsigned char hilogo[786*1024];
unsigned long VIDEO_DATA_BASE=0;

extern void dcache_enable(void);
extern void dcache_disable(void);
extern int dcache_status(void);
extern void enable_mmu(void);
extern void stop_mmu(void);

int jpeg_decode(void)
{
    enable_mmu();
    dcache_enable();
    printf("mmu_enable\n");

    LoadJpegFile((void *)VIDEO_DATA_BASE);
    
    dcache_disable();
    stop_mmu();

    return 0;
}

int load_jpeg(void)
{
    char *s=NULL;
    char *t=NULL;
    long u=0;

    printf("jpeg decoding ...\n");

#if 0
    run_command("setenv vobuf 0xcf000000",0);
    run_command("setenv jpeg_size 0x20000",0);
    run_command("setenv jpeg_addr 0x81f00000",0);
#endif

    s = getenv("vobuf");
    if(s)
    {
    	VIDEO_DATA_BASE = simple_strtol(s,NULL,0);
    }
    else
    {
    	printf("Invalid vobuf address!\n");
    	return 1;
    }

    if(VIDEO_DATA_BASE == 0)
    {
        printf("Invalid VOU buffer address 0x%08lX\n",VIDEO_DATA_BASE);
        return -1;
    }

    jpeg_size=simple_strtol(getenv("jpeg_size"),NULL,0);

    t = getenv("jpeg_addr");

    memset(hilogo,0x0,786*1024);

    if(t)
    {
    	u=  simple_strtol(t,NULL,0);
        printf("<<addr=%#lx, size=%#lx, vobuf=%#lx>>\n", u,jpeg_size,VIDEO_DATA_BASE);
    	memcpy(hilogo, (char*)u,jpeg_size);
    	if (*(volatile unsigned char *)hilogo != 0xFF || *(volatile unsigned char *)(hilogo+1) != 0xD8)
    	{
    		printf("addr:%#x,size:%ld,logoaddr:%#lx,:%2x,%2x\n",(unsigned int)(void *)hilogo,jpeg_size,u
    			,*(volatile unsigned char *)hilogo,*(volatile unsigned char *)(hilogo+1));
    	    return -1;
    	}
    	return 0;
    }
    else
    {
    	printf("you should set jpeg picture's address!\n");
    	return -1;
    }
}


