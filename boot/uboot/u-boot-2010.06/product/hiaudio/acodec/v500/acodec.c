/*******************************************************************
 *
 *  Copyright (C), 2001-2015, Hisilicon Tech. Co., Ltd.
 *
 * *****************************************************************
 * File Name     : acodec.c
 * Version       : Initial Draft
 * Author        : Hisilicon multimedia software group
 * Created       : 2015/01/14
 * Description   : 
 * History       :
 * 1.Date        : 2015/01/14
 * Author      : x00307967
 * Modification: Created file
 *
 * *****************************************************************
 */

#include <common.h>
#include <command.h>
#include <version.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include <asm/sizes.h>
#include <config.h>

#include "hi3516a_ao.h"
#include "acodec_def.h"
#include "acodec.h"

#define IO_ADDRESS(x) (x)

void *g_acodec_crg_reg = NULL;

static ACODEC_REGS_S *g_pstAcodecReg = NULL;

static inline void acodec_regsetbit(
		unsigned long value,
		unsigned long offset,
		unsigned long addr)
{
	unsigned long t, mask;

	mask = 1 << offset;
	t = readl(addr);
	t &= ~mask;
	t |= (value << offset) & mask;
	writel(t, addr);
}

static int acodec_soft_reset(void)
{
	if ( NULL == g_pstAcodecReg) {
		printf("Haven't ioremap acodec regs!");
		return -1;
	}

	/* umute,0db*/
    g_pstAcodecReg->acodec_ctrl0.ul32   = 0x00f22c2c;
    g_pstAcodecReg->acodec_ctrl1.ul32   = 0xff035144;
	g_pstAcodecReg->acodec_dacreg0.ul32 = 0x08000001;
	g_pstAcodecReg->acodec_dacreg1.ul32 = 0x07072424;
	g_pstAcodecReg->acodec_adcreg0.ul32 = 0x1e1ec001;
	g_pstAcodecReg->acodec_adcreg1.ul32 = 0x24242424;
	
	return 0;
}
int acodec_i2s_set(AUDIO_SAMPLE_RATE_E enSample)
{
    unsigned int i2s_fs_sel = 0;
	ACODEC_CTRL1_U      unCtrl1;

	if ((AUDIO_SAMPLE_RATE_8000 == enSample)
        || (AUDIO_SAMPLE_RATE_11025 == enSample)
        || (AUDIO_SAMPLE_RATE_12000 == enSample)) 
    {
        i2s_fs_sel = 0x18;
    } 
    else if ((AUDIO_SAMPLE_RATE_16000 == enSample)
        || (AUDIO_SAMPLE_RATE_22050 == enSample)
        || (AUDIO_SAMPLE_RATE_24000 == enSample)) 
    {
        i2s_fs_sel = 0x19;
    } 
    else if ((AUDIO_SAMPLE_RATE_32000 == enSample)
        || (AUDIO_SAMPLE_RATE_44100 == enSample)
        || (AUDIO_SAMPLE_RATE_48000 == enSample)) 
    {
        i2s_fs_sel = 0x1a;
    } 
    else 
    {
        printf("%s: not support enSample:%d\n", __FUNCTION__, enSample);
        return -1;
    }
	unCtrl1.ul32 = g_pstAcodecReg->acodec_ctrl1.ul32;
	unCtrl1.bits.i2s1_fs_sel = i2s_fs_sel;
	g_pstAcodecReg->acodec_ctrl1.ul32 = unCtrl1.ul32;
		
	return 0;
}

int acodec_device_init(void)
{
    unsigned int u32DefultCtrl0 = 0;
    
	g_pstAcodecReg = (ACODEC_REGS_S *)IO_ADDRESS(ACODEC_REGS_BASE);
	if ( NULL == g_pstAcodecReg) {
		printf("could not ioremap acodec regs!");
		return -1;
	}
	/*internal codec£¬AIO MCLK out, CODEC AIO TX MCLK */
	g_pstAcodecReg->acodec_ctrl56.ul32 = 0xd;

	g_acodec_crg_reg = (void *)IO_ADDRESS(ACODEC_REGS_CRG);
	if (NULL == g_acodec_crg_reg) {
		printf("could not ioremap acodec regs!");
		return -1;
	}
	acodec_regsetbit(0 , 15, (unsigned long)g_acodec_crg_reg);

	u32DefultCtrl0 = g_pstAcodecReg->acodec_ctrl0.ul32;
       
    g_pstAcodecReg->acodec_ctrl0.ul32 = 0xfccc0000;     //default value  
    if(0 == u32DefultCtrl0)
    {
        udelay(10000);
    }
    g_pstAcodecReg->acodec_ctrl0.ul32 = 0xfcfc0000;     // [20, 21] : [1 1], popfree on
    g_pstAcodecReg->acodec_ctrl0.ul32 = 0xf8fc0000;     // [26] : 0   vref on
	
    acodec_soft_reset();
	
	return 0;
}   	

int acodec_device_exit(void)
{ 
    g_pstAcodecReg->acodec_dacreg1.ul32 = 0x7f7f2424;  
    g_pstAcodecReg->acodec_ctrl0.ul32   = 0x00fe2c2c;   /*da mute,popfree on*/
    g_pstAcodecReg->acodec_ctrl0.ul32   = 0xf8fe2c2c;   /*da power off*/
    g_pstAcodecReg->acodec_ctrl0.ul32   = 0xfcfe2c2c;   /*verf off*/
    g_pstAcodecReg->acodec_ctrl0.ul32   = 0xfccc0000;   /*default value*/
    
	acodec_regsetbit(1 , 15, (unsigned long)g_acodec_crg_reg);
	g_acodec_crg_reg = NULL;

	return 0;
}

