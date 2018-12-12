// ******************************************************************************
// Copyright     :  Copyright (C) 2016, Hisilicon Technologies Co. Ltd.
// File name     :  hdmi_reg_crg.c
// Project line  :  Platform And Key Technologies Development
// Department    :  CAD Development Department
// Author        :  xxx
// Version       :  1
// Date          :  2016/7/18
// Description   :  The description of xxx project
// Others        :
// History       :  xxx 2016/07/18 10:40:30 Create file
// ******************************************************************************

#include "hi_type.h"
#include "hdmi_reg_crg.h"
#include "hdmi_product_define.h"
#include "hdmi_platform.h"
#ifndef HDMI_BUILD_IN_BOOT
#include "drv_hdmi_platform.h"
#endif

/* Define the struct pointor of the module hdmi2tx_dphy_reg */
volatile HDMI_REG_CRG_S *s_pstRegCrg = NULL;


int REG_HDMI_CRG_reg_Init(void)
{
    if (NULL != s_pstRegCrg)
    {
        return HI_SUCCESS;
    }

    s_pstRegCrg  = (HDMI_REG_CRG_S*)HDMI_IO_MAP(HDMI_ADDR_CRG, sizeof(HDMI_REG_CRG_S));

    if(NULL == s_pstRegCrg)
    {
        HDMI_ERR("ioremap_nocache gophdmi2tx_dphy_regAllReg failed!!!!!!!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

int REG_HDMI_CRG_reg_DeInit(void)
{
    if(HI_NULL != s_pstRegCrg)
    {
        HDMI_IO_UNMAP(s_pstRegCrg);
        s_pstRegCrg = HI_NULL;
    }

    return HI_SUCCESS;

}

int REG_HDMI_CRG_ssc_in_cken_Set(unsigned int ussc_in_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.ssc_in_cken = ussc_in_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_ssc_in_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.ssc_in_cken;
}

int REG_HDMI_CRG_ssc_bypass_cken_Set(unsigned int ussc_bypass_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.ssc_bypass_cken = ussc_bypass_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_ssc_bypass_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.ssc_bypass_cken;
}

int REG_HDMI_CRG_hdmitx_ctrl_osc_24m_cken_Set(unsigned int uhdmitx_ctrl_osc_24m_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ctrl_osc_24m_cken = uhdmitx_ctrl_osc_24m_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ctrl_osc_24m_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ctrl_osc_24m_cken;
}

int REG_HDMI_CRG_hdmitx_ctrl_cec_cken_Set(unsigned int uhdmitx_ctrl_cec_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ctrl_cec_cken = uhdmitx_ctrl_cec_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ctrl_cec_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ctrl_cec_cken;
}

int REG_HDMI_CRG_hdmitx_ctrl_os_cken_Set(unsigned int uhdmitx_ctrl_os_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ctrl_os_cken = uhdmitx_ctrl_os_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ctrl_os_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ctrl_os_cken;
}

int REG_HDMI_CRG_hdmitx_ctrl_as_cken_Set(unsigned int uhdmitx_ctrl_as_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ctrl_as_cken = uhdmitx_ctrl_as_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ctrl_as_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ctrl_as_cken;
}

int REG_HDMI_CRG_hdmitx_ctrl_bus_srst_req_Set(unsigned int uhdmitx_ctrl_bus_srst_req)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ctrl_bus_srst_req = uhdmitx_ctrl_bus_srst_req;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ctrl_bus_srst_req_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ctrl_bus_srst_req;
}

int REG_HDMI_CRG_hdmitx_ctrl_srst_req_Set(unsigned int uhdmitx_ctrl_srst_req)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ctrl_srst_req = uhdmitx_ctrl_srst_req;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ctrl_srst_req_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ctrl_srst_req;
}

int REG_HDMI_CRG_hdmitx_ctrl_cec_srst_req_Set(unsigned int uhdmitx_ctrl_cec_srst_req)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ctrl_cec_srst_req = uhdmitx_ctrl_cec_srst_req;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ctrl_cec_srst_req_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ctrl_cec_srst_req;
}

int REG_HDMI_CRG_hdmitx_ssc_srst_req_Set(unsigned int uhdmitx_ssc_srst_req)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_ssc_srst_req = uhdmitx_ssc_srst_req;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_ssc_srst_req_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_ssc_srst_req;
}

int REG_HDMI_CRG_ssc_clk_div_Set(unsigned int ussc_clk_div)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.ssc_clk_div = ussc_clk_div;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_ssc_clk_div_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.ssc_clk_div;
}

/*  */
int REG_HDMI_CRG_hdmitx_pxl_cken_Set(unsigned int uhdmitx_pxl_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.hdmitx_pxl_cken = uhdmitx_pxl_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_pxl_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.hdmitx_pxl_cken;
}

int REG_HDMI_CRG_ssc_bypass_clk_sel_Set(unsigned int ussc_bypass_clk_sel)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);

    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg59.bits.ssc_bypass_clk_sel = ussc_bypass_clk_sel;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg59.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_ssc_bypass_clk_sel_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG59 o_peri_crg59;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG59.u32);
    o_peri_crg59.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg59.bits.ssc_bypass_clk_sel;
}

int REG_HDMI_CRG_phy_tmds_cken_Set(unsigned int uphy_tmds_cken)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);

    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg60.bits.phy_tmds_cken = uphy_tmds_cken;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg60.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_phy_tmds_cken_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);
    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg60.bits.phy_tmds_cken;
}

int REG_HDMI_CRG_hdmitx_phy_srst_req_Set(unsigned int uhdmitx_phy_srst_req)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);

    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg60.bits.hdmitx_phy_srst_req = uhdmitx_phy_srst_req;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg60.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_hdmitx_phy_srst_req_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);
    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg60.bits.hdmitx_phy_srst_req;
}

int REG_HDMI_CRG_phy_tmds_srst_req_Set(unsigned int uphy_tmds_srst_req)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);

    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg60.bits.phy_tmds_srst_req = uphy_tmds_srst_req;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg60.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_phy_tmds_srst_req_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);
    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg60.bits.phy_tmds_srst_req;
}

int REG_HDMI_CRG_tmds_clk_div_Set(unsigned int utmds_clk_div)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);

    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg60.bits.tmds_clk_div = utmds_clk_div;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg60.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_tmds_clk_div_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG60 o_peri_crg60;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG60.u32);
    o_peri_crg60.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg60.bits.tmds_clk_div;
}

/*  */
int REG_HDMI_CRG_vo_hd0_hdmi_clk_div_Set(unsigned int uvo_hd0_hdmi_clk_div)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG61 o_peri_crg61;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG61.u32);

    o_peri_crg61.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg61.bits.vo_hd0_hdmi_clk_div = uvo_hd0_hdmi_clk_div;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg61.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_vo_hd0_hdmi_clk_div_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG61 o_peri_crg61;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG61.u32);
    o_peri_crg61.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg61.bits.vo_hd0_hdmi_clk_div;

}

int REG_HDMI_CRG_vo_hd1_hdmi_clk_div_Set(unsigned int uvo_hd1_hdmi_clk_div)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG61 o_peri_crg61;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG61.u32);

    o_peri_crg61.u32 = HDMITXRegRead(pu32RegAddr);
    o_peri_crg61.bits.vo_hd1_hdmi_clk_div = uvo_hd1_hdmi_clk_div;
    HDMITXRegWrite(pu32RegAddr, o_peri_crg61.u32);

    return HI_SUCCESS;
}

int REG_HDMI_CRG_vo_hd1_hdmi_clk_div_Get(HI_VOID)
{
    HI_U32 *pu32RegAddr = NULL;
    R_PERI_CRG61 o_peri_crg61;

    pu32RegAddr = (HI_U32*)&(s_pstRegCrg->PERI_CRG61.u32);
    o_peri_crg61.u32 = HDMITXRegRead(pu32RegAddr);
    return o_peri_crg61.bits.vo_hd1_hdmi_clk_div;
}

