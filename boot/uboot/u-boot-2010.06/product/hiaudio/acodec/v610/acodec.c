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

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
#include "hi3519_ao.h"
#endif

#if (defined CONFIG_ARCH_HI3516AV200)
#include "hi3516av200_ao.h"
#endif

#if (defined CONFIG_ARCH_HI3559 || defined CONFIG_ARCH_HI3556)
#include "hi3559_ao.h"
#endif

#include "acodec_def.h"
#include "acodec.h"

#define IO_ADDRESS(x) (x)

void *g_acodec_crg_reg = NULL;

static unsigned int u32AcodecBase;

unsigned int ACODEC_HAL_ReadReg(unsigned int u32Offset)
{
    return (*(volatile unsigned int*)((unsigned int)u32AcodecBase + (unsigned int)u32Offset));
}

void ACODEC_HAL_WriteReg(unsigned int u32Offset, unsigned int u32Value)
{
    *(volatile unsigned int*)((unsigned int)(u32AcodecBase) + (unsigned int)(u32Offset)) = u32Value;
}

void ACODEC_HAL_DUMP(unsigned int u32Offset)
{
    printf("addr: %8x  value: %x\n",((unsigned int)u32AcodecBase + (unsigned int)u32Offset),\
        ACODEC_HAL_ReadReg( u32Offset));
}

//static ACODEC_REGS_S *g_pstAcodecReg = NULL;

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
    ACODEC_DIGCTRL0_U   acodec_digctrl0; 
    ACODEC_DIGCTRL1_U   acodec_digctrl1;
    ACODEC_DIGCTRL2_U   acodec_digctrl2;
    ACODEC_DIGCTRL3_U   acodec_digctrl3;

    ACODEC_ANAREG0_U   acodec_anareg0;
    ACODEC_ANAREG1_U   acodec_anareg1;
    
    if (0 == u32AcodecBase) {
        printf("Haven't ioremap acodec regs!");
        return -1;
    }

    /* 左右声道上电，保持mute、popfree*/
    acodec_anareg0.ul32 = 0x6400e402;//0x6405e402;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    /* 输入增益20db*/
    acodec_anareg1.ul32 = 0x11404004;//ACODEC_ANAREG1_DEFAULT
    ACODEC_HAL_WriteReg(ACODEC_ANAREG1_ADDR, acodec_anareg1.ul32);
  
    /* 左右声道使能,*/
    acodec_digctrl0.ul32   = 0xff035144;
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL0_ADDR, acodec_digctrl0.ul32);

    /*也可0x00000001,即不开启soft mute,不更新音量，不设置去加重滤波器*/
    acodec_digctrl1.ul32 = 0x08000001;
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL1_ADDR, acodec_digctrl1.ul32);
    
    /*设置最小音量*/
    acodec_digctrl2.ul32 = 0x1b1b2424;   /*音量最小*/ 
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL2_ADDR, acodec_digctrl2.ul32);
    
    /*也可0x1E1E0001，即不开启高通滤波器*/
    acodec_digctrl3.ul32 = 0x1e1ec001;
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL3_ADDR, acodec_digctrl3.ul32);


    return 0;

}
int acodec_i2s_set(AUDIO_SAMPLE_RATE_E enSample)
{
    unsigned int i2s_fs_sel = 0;
    ACODEC_DIGCTRL0_U    unDigctrl0;

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

    unDigctrl0.ul32 = ACODEC_HAL_ReadReg(ACODEC_DIGCTRL0_ADDR);
    unDigctrl0.bits.i2s1_fs_sel = i2s_fs_sel;
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL0_ADDR, unDigctrl0.ul32);

    return 0;
}

int acodec_device_init(void)
{
    ACODEC_DIGCTRL0_U   acodec_digctrl0; 
    ACODEC_DIGCTRL1_U   acodec_digctrl1;
    ACODEC_DIGCTRL2_U   acodec_digctrl2;
    
    ACODEC_ANAREG0_U   acodec_anareg0;
    /*ACODEC_ANAREG1_U   acodec_anareg1;*/
    ACODEC_ANAREG2_U   acodec_anareg2;
    ACODEC_ANAREG3_U   acodec_anareg3;

    unsigned int aiao_crg = 0;
    unsigned int aiao_cfg = 0;
    
    u32AcodecBase = (unsigned int)IO_ADDRESS(ACODEC_REGS_BASE);
    if ( 0 == u32AcodecBase) {
        printf("could not ioremap acodec regs!");
        return -1;
    }
    ACODEC_HAL_WriteReg(ACODEC_ANAREG56_ADDR, 0xD);
    /* 低功耗策略，在加载acodec时撤销复位 */
    g_acodec_crg_reg = (void *)IO_ADDRESS(ACODEC_REGS_CRG);
    if (NULL == g_acodec_crg_reg) {
        printf("could not ioremap acodec regs!");
        return -1;
    }

    acodec_regsetbit(0 , 0, (unsigned long)g_acodec_crg_reg);

    acodec_digctrl2.ul32 = 0x7f7f2424;   /*音量最小*/ 
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL2_ADDR, acodec_digctrl2.ul32);
    
    aiao_crg = (unsigned int)IO_ADDRESS(AIAO_CLK_TX0_CRG);
    *(volatile unsigned int*)((unsigned int)(aiao_crg)) = 0x00100000;
    aiao_cfg = (unsigned int)IO_ADDRESS(AIAO_CLK_TX0_CFG);
    *(volatile unsigned int*)((unsigned int)(aiao_cfg)) = 0x00000115;
    udelay(50*1000);

    acodec_anareg0.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG0_ADDR);
    /*acodec_anareg1.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG1_ADDR);*/
    acodec_anareg2.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG2_ADDR);
    acodec_anareg3.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG3_ADDR);

    /* 左右声道使能,*/
    acodec_digctrl0.ul32   = 0xff035144;
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL0_ADDR, acodec_digctrl0.ul32);

    /*也可0x00000001,即不开启soft mute,不更新音量，不设置去加重滤波器*/
    acodec_digctrl1.ul32 = 0x08000001;
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL1_ADDR, acodec_digctrl1.ul32);

    acodec_digctrl2.ul32 = 0x7e7e2424;
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL2_ADDR, acodec_digctrl2.ul32);

    /*POP_RES_SEL<1:0>=01，POP_RAMPCLK_SEL<1：0>=01*/
    acodec_anareg3.bits.acodec_popres_sel = 0x1;
    acodec_anareg3.bits.acodec_poprampclk_sel = 3;    
    ACODEC_HAL_WriteReg(ACODEC_ANAREG3_ADDR, acodec_anareg3.ul32);

    acodec_anareg2.bits.acodec_vref_sel = 0x0;//0x4
    acodec_anareg2.bits.acodec_sel_clk_chop_adc_ph = 0x0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG2_ADDR, acodec_anareg2.ul32);

    /* mute popfree */
    acodec_anareg0.bits.acodec_mute_dacr = 1;
    acodec_anareg0.bits.acodec_mute_dacl = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);    

    /* direct 0 */
    acodec_anareg3.bits.acodec_dacl_pop_direct = 0;
    acodec_anareg3.bits.acodec_dacr_pop_direct = 0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG3_ADDR, acodec_anareg3.ul32);
    
    /* power on: vref, micbias, ctcm */
    acodec_anareg0.bits.acodec_pd_micbias1 = 0;
    acodec_anareg0.bits.acodec_pdb_ctcm_ibias = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    acodec_anareg2.bits.acodec_pd_ctcm = 0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG2_ADDR, acodec_anareg2.ul32);

    acodec_anareg0.bits.acodec_popfreer = 1;//0;
    acodec_anareg0.bits.acodec_popfreel = 1;//0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    acodec_anareg0.bits.acodec_pd_vref = 0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    /* direct 1 */
    acodec_anareg3.bits.acodec_dacl_pop_direct = 1;
    acodec_anareg3.bits.acodec_dacr_pop_direct = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG3_ADDR, acodec_anareg3.ul32);

    udelay(1000*1000);//

    /* dac power on */
    acodec_anareg0.bits.acodec_pd_dacl = 0;
    acodec_anareg0.bits.acodec_pd_dacr = 0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    acodec_anareg0.bits.acodec_mute_dacl = 0;
    acodec_anareg0.bits.acodec_mute_dacr = 0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    acodec_anareg0.bits.acodec_popfreer = 0;//1;
    acodec_anareg0.bits.acodec_popfreel = 0;//1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    acodec_soft_reset();

    printf("acodec inited!\n");
    
    return 0;
}       
       
int acodec_device_exit(void)
{  
    ACODEC_DIGCTRL2_U  acodec_digctrl2;

    ACODEC_ANAREG0_U   acodec_anareg0;
    ACODEC_ANAREG1_U   acodec_anareg1;
    ACODEC_ANAREG2_U   acodec_anareg2;
    ACODEC_ANAREG3_U   acodec_anareg3;

    unsigned int aiao_crg = 0;
    unsigned int aiao_cfg = 0;
     
    //VALG_TimerDelete(&g_AcodecTimer);
    
    acodec_regsetbit(0 , 0, (unsigned long)g_acodec_crg_reg);
    acodec_digctrl2.ul32 = 0x7f7f2424;   /*音量最小*/ 
    ACODEC_HAL_WriteReg(ACODEC_DIGCTRL2_ADDR, acodec_digctrl2.ul32);

    aiao_crg = (unsigned int)IO_ADDRESS(AIAO_CLK_TX0_CRG);
    *(volatile unsigned int*)((unsigned int)(aiao_crg)) = 0x00100000;//0x00200000;//0x001D6666;//0x00155555;//0x00100000;
    aiao_cfg = (unsigned int)IO_ADDRESS(AIAO_CLK_TX0_CFG);
    *(volatile unsigned int*)((unsigned int)(aiao_cfg)) = 0x00000115;
    udelay(50*1000);

    acodec_anareg0.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG0_ADDR);
    acodec_anareg1.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG1_ADDR);
    acodec_anareg2.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG2_ADDR);
    acodec_anareg3.ul32 = ACODEC_HAL_ReadReg(ACODEC_ANAREG3_ADDR);

    /*POP_RES_SEL<1:0>=01，POP_RAMPCLK_SEL<1：0>=01*/
    acodec_anareg3.bits.acodec_popres_sel = 0x3;
    acodec_anareg3.bits.acodec_poprampclk_sel = 0x3;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG3_ADDR, acodec_anareg3.ul32);
    
    /* 保持direct 1*/
    acodec_anareg3.bits.acodec_dacl_pop_direct = 1;
    acodec_anareg3.bits.acodec_dacr_pop_direct = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG3_ADDR, acodec_anareg3.ul32);
    
    /* mute popfree on*/
    acodec_anareg0.bits.acodec_mute_dacr = 1;
    acodec_anareg0.bits.acodec_mute_dacl = 1;
    acodec_anareg0.bits.acodec_popfreer = 1;
    acodec_anareg0.bits.acodec_popfreel = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);
    
    acodec_anareg0.bits.acodec_pd_dacl = 1;
    acodec_anareg0.bits.acodec_pd_dacr = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);
    
    /* 保持direct 0*/
    acodec_anareg3.bits.acodec_dacl_pop_direct = 0;
    acodec_anareg3.bits.acodec_dacr_pop_direct = 0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG3_ADDR, acodec_anareg3.ul32);

    /* power on: vref, micbias, ctcm */  
    acodec_anareg0.bits.acodec_pd_vref = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    acodec_anareg2.bits.acodec_pd_ctcm = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG2_ADDR, acodec_anareg2.ul32);

    acodec_anareg0.bits.acodec_pd_micbias1 = 1;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG0_ADDR, acodec_anareg0.ul32);

    //input mod reset - 消除跑业务后初次上电的pop
    acodec_anareg1.bits.acodec_linein_l_sel = 0;
    acodec_anareg1.bits.acodec_linein_r_sel = 0;
    ACODEC_HAL_WriteReg(ACODEC_ANAREG1_ADDR, acodec_anareg1.ul32);

    printf("acodec exited!\n");
    return 0;
}


