//*********************************************************************
//
// Copyright(c)2011,Huawei Technologies Co.,Ltd
// All rights reserved.
//
// File Name   :    Aio_hal.h
// Author       :     c00191088
// Email       :      chenwei71@huawei.com
// Data        :    2012/10/14
// Version       :    v1.0
// Abstract    :    head of aio driver common function
//**********************************************************************
#ifndef __AIAO_REG_H__
#define __AIAO_REG_H__


#include <asm/io.h>
//******************************************************************************


#define BUFFER_ADDR_ALIGN          128  /*缓存基址128字节对齐*/
#define BUFFER_ADDR_SIZE_BASE      128  /*缓存大小基数128字节*/

/******************************************************************************/
/*                                   AIO REG DEFINE                          */
/******************************************************************************/

#define AIAO_MAX_REG_SIZE          (64*1024)
#define AIAO_REG_BASE              (0x20650000)
#define MISC_REG_BASE              (0x20120000)

#define AIAO_INT_ENA_REG           (0x0)             
#define AIAO_INT_STATUS_REG        (0x4)               
#define AIAO_INT_RAW_REG           (0x8)              

#define AIAO_I2S00_REG_CFG0        (0x0100)         
#define AIAO_I2S00_REG_CFG1        (0x0104)           
#define AIAO_I2S08_REG_CFG0        (0x0140)           
#define AIAO_I2S08_REG_CFG1        (0x0144)           
#define AIAO_I2S09_REG_CFG0        (0x0148)           
#define AIAO_I2S09_REG_CFG1        (0x014c)           

#define AIP_I2S_REG_CFG0(n)        (0x0100+8*n)
#define AIP_I2S_REG_CFG1(n)        (0x0104+8*n)
#define AOP_I2S_REG_CFG0(n)        (0x0140+8*n)
#define AOP_I2S_REG_CFG1(n)        (0x0144+8*n)

#define AIP_SWITCH_RX_BCLK         (0x0028)           
#define AOP_SWITCH_TX_BCLK         (0x002c)           
#define AIO_SOFT_RESET_STATUS      (0x0030)           

/* n = 0*/
#define AIP_INF_ATTRI_REG(n)      (0x1000+0x100*n)   
#define AIP_CTRL_REG(n)           (0x1004+0x100*n)    
#define AIP_BUFF_SADDR_REG(n)     (0x1080+0x100*n)   
#define AIP_BUFF_SIZE_REG(n)      (0x1084+0x100*n)   
#define AIP_BUFF_WPTR_REG(n)      (0x1088+0x100*n)   
#define AIP_BUFF_RPTR_REG(n)      (0x108c+0x100*n)   
#define AIP_BUFF_ALFULL_TH_REG(n) (0x1090+0x100*n)   
#define AIP_TRANS_SIZE_REG(n)     (0x1094+0x100*n)    
#define AIP_WPTR_TMP_REG(n)       (0x1098+0x100*n)    

#define AIP_INT_ENA_REG(n)        (0x10A0+0x100*n)    
#define AIP_INT_RAW_REG(n)        (0x10A4+0x100*n)   
#define AIP_INT_STATUS_REG(n)     (0x10A8+0x100*n)   
#define AIP_INT_CLR_REG(n)        (0x10AC+0x100*n)    

#define AOP_INF_ATTRI_REG(m)      (0x2000+0x100*m)   
#define AOP_CTRL_REG(m)           (0x2004+0x100*m)    

#define AOP_BUFF_SADDR_REG(m)     (0x2080+0x100*m)   
#define AOP_BUFF_SIZE_REG(m)      (0x2084+0x100*m)    
#define AOP_BUFF_WPTR_REG(m)      (0x2088+0x100*m)    
#define AOP_BUFF_RPTR_REG(m)      (0x208C+0x100*m)    
#define AOP_BUFF_ALEMPTY_TH_REG(m)(0x2090+0x100*m)    
#define AOP_TRANS_SIZE_REG(m)     (0x2094+0x100*m)   
#define AOP_RPTR_TMP_REG(m)       (0x2098+0x100*m)    


#define AOP_INT_ENA_REG(m)        (0x20A0+0x100*m)   
#define AOP_INT_RAW_REG(m)        (0x20A4+0x100*m)    
#define AOP_INT_STATUS_REG(m)     (0x20A8+0x100*m)   
#define AOP_INT_CLR_REG(m)        (0x20AC+0x100*m)    

#define AIO_BUS_WRITE_REQUEST      0xFF00            
#define AIO_BUS_READ_REQUEST       0xFF04
#define AIO_BUS_READ_DATA          0xFF08
#define AIO_BUS_WRITE_DATA         0xFF0C
#define AIO_BUS_BVALID             0xFF10

#define AUDIO_CODEC_CTRL0_REG_ADDR 0x201200c8

/******************************************************************************/
/*                 AIO register bit field define                              */
/******************************************************************************/




/************************************************************************/
#define  AIAO_SPDIF_SUPPORT
#define  AIAO_TX_DSP_SUPPORT

/***************************** Macro Definition ******************************/

// Define the union U_AIAO_SYS_CRG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int aiao_cken             : 1; // [0]
        unsigned int Reserved_0            : 3; // [3..1]
        unsigned int aiao_srst_req         : 1; // [4]
        unsigned int Reserved_1            : 3; // [7..5]
        unsigned int aiao_clk_sel          : 1; // [8]
        unsigned int Reserved_2            : 3; // [11..9]
        unsigned int aiaoclk_skipcfg       : 5; // [16..12]
        unsigned int aiaoclk_loaden        : 1; // [17]
        unsigned int Reserved_3            : 14;// [31..18]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_S40_AIAO_SYS_CRG;

// Define the union U_AIAO_INT_ENA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_ch0_int_ena        : 1; // [0]
        unsigned int rx_ch1_int_ena        : 1; // [1]
        unsigned int rx_ch2_int_ena        : 1; // [2]
        unsigned int rx_ch3_int_ena        : 1; // [3]
        unsigned int rx_ch4_int_ena        : 1; // [4]
        unsigned int rx_ch5_int_ena        : 1; // [5]
        unsigned int rx_ch6_int_ena        : 1; // [6]
        unsigned int rx_ch7_int_ena        : 1; // [7]
        unsigned int Reserved_1            : 8; // [15..8]
        unsigned int tx_ch0_int_ena        : 1; // [16]
        unsigned int tx_ch1_int_ena        : 1; // [17]
        unsigned int tx_ch2_int_ena        : 1; // [18]
        unsigned int tx_ch3_int_ena        : 1; // [19]
        unsigned int tx_ch4_int_ena        : 1; // [20]
        unsigned int tx_ch5_int_ena        : 1; // [21]
        unsigned int tx_ch6_int_ena        : 1; // [22]
        unsigned int tx_ch7_int_ena        : 1; // [23]
        unsigned int spdiftx_ch0_int_ena   : 1; // [24]
        unsigned int spdiftx_ch1_int_ena   : 1; // [25]
        unsigned int spdiftx_ch2_int_ena   : 1; // [26]
        unsigned int spdiftx_ch3_int_ena   : 1; // [27]
        unsigned int Reserved_0            : 4; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_AIAO_INT_ENA;

// Define the union U_AIAO_INT_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_ch0_int_status     : 1; // [0]
        unsigned int rx_ch1_int_status     : 1; // [1]
        unsigned int rx_ch2_int_status     : 1; // [2]
        unsigned int rx_ch3_int_status     : 1; // [3]
        unsigned int rx_ch4_int_status     : 1; // [4]
        unsigned int rx_ch5_int_status     : 1; // [5]
        unsigned int rx_ch6_int_status     : 1; // [6]
        unsigned int rx_ch7_int_status     : 1; // [7]
        unsigned int Reserved_1            : 8; // [15..8]
        unsigned int tx_ch0_int_status     : 1; // [16]
        unsigned int tx_ch1_int_status     : 1; // [17]
        unsigned int tx_ch2_int_status     : 1; // [18]
        unsigned int tx_ch3_int_status     : 1; // [19]
        unsigned int tx_ch4_int_status     : 1; // [20]
        unsigned int tx_ch5_int_status     : 1; // [21]
        unsigned int tx_ch6_int_status     : 1; // [22]
        unsigned int tx_ch7_int_status     : 1; // [23]
        unsigned int spdiftx_ch0_int_status  : 1; // [24]
        unsigned int spdiftx_ch1_int_status  : 1; // [25]
        unsigned int spdiftx_ch2_int_status  : 1; // [26]
        unsigned int spdiftx_ch3_int_status  : 1; // [27]
        unsigned int Reserved_0            : 4; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_AIAO_INT_STATUS;

// Define the union U_AIAO_INT_RAW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_ch0_int_raw        : 1; // [0]
        unsigned int rx_ch1_int_raw        : 1; // [1]
        unsigned int rx_ch2_int_raw        : 1; // [2]
        unsigned int rx_ch3_int_raw        : 1; // [3]
        unsigned int rx_ch4_int_raw        : 1; // [4]
        unsigned int rx_ch5_int_raw        : 1; // [5]
        unsigned int rx_ch6_int_raw        : 1; // [6]
        unsigned int rx_ch7_int_raw        : 1; // [7]
        unsigned int Reserved_1            : 8; // [15..8]
        unsigned int tx_ch0_int_raw        : 1; // [16]
        unsigned int tx_ch1_int_raw        : 1; // [17]
        unsigned int tx_ch2_int_raw        : 1; // [18]
        unsigned int tx_ch3_int_raw        : 1; // [19]
        unsigned int tx_ch4_int_raw        : 1; // [20]
        unsigned int tx_ch5_int_raw        : 1; // [21]
        unsigned int tx_ch6_int_raw        : 1; // [22]
        unsigned int tx_ch7_int_raw        : 1; // [23]
        unsigned int spdiftx_ch0_int_raw   : 1; // [24]
        unsigned int spdiftx_ch1_int_raw   : 1; // [25]
        unsigned int spdiftx_ch2_int_raw   : 1; // [26]
        unsigned int spdiftx_ch3_int_raw   : 1; // [27]
        unsigned int Reserved_0            : 4; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_AIAO_INT_RAW;

// Define the union U_HW_CAPABILITY
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_ch0_cap            : 1; // [0]
        unsigned int rx_ch1_cap            : 1; // [1]
        unsigned int rx_ch2_cap            : 1; // [2]
        unsigned int rx_ch3_cap            : 1; // [3]
        unsigned int rx_ch4_cap            : 1; // [4]
        unsigned int rx_ch5_cap            : 1; // [5]
        unsigned int rx_ch6_cap            : 1; // [6]
        unsigned int rx_ch7_cap            : 1; // [7]
        unsigned int Reserved_1            : 8; // [15..8]
        unsigned int tx_ch0_cap            : 1; // [16]
        unsigned int tx_ch1_cap            : 1; // [17]
        unsigned int tx_ch2_cap            : 1; // [18]
        unsigned int tx_ch3_cap            : 1; // [19]
        unsigned int tx_ch4_cap            : 1; // [20]
        unsigned int tx_ch5_cap            : 1; // [21]
        unsigned int tx_ch6_cap            : 1; // [22]
        unsigned int tx_ch7_cap            : 1; // [23]
        unsigned int spdiftx_ch0_cap       : 1; // [24]
        unsigned int spdiftx_ch1_cap       : 1; // [25]
        unsigned int spdiftx_ch2_cap       : 1; // [26]
        unsigned int spdiftx_ch3_cap       : 1; // [27]
        unsigned int Reserved_0            : 4; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_HW_CAPABILITY;

// Define the union U_SPDIF_TX_MUX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int spdif_tx_0_port_sel   : 3; // [2..0]
        unsigned int spdif_tx_0_port_en    : 1; // [3]
        unsigned int Reserved_3            : 4; // [7..4]
        unsigned int spdif_tx_1_port_sel   : 3; // [10..8]
        unsigned int spdif_tx_1_port_en    : 1; // [11]
        unsigned int Reserved_2            : 4; // [15..12]
        unsigned int spdif_tx_2_port_sel   : 3; // [18..16]
        unsigned int spdif_tx_2_port_en    : 1; // [19]
        unsigned int Reserved_1            : 4; // [23..20]
        unsigned int spdif_tx_3_port_sel   : 3; // [26..24]
        unsigned int spdif_tx_3_port_en    : 1; // [27]
        unsigned int Reserved_0            : 4; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_TX_MUX;

// Define the union U_I2S_CRG_CFG0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int aiao_mclk_div         : 27; // [26..0]
        unsigned int Reserved_0            : 5; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_I2S_CRG_CFG0;

// Define the union U_I2S_CRG_CFG1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int aiao_bclk_div         : 4; // [3..0]
        unsigned int aiao_fsclk_div        : 3; // [6..4]
        unsigned int Reserved_1            : 1; // [7]
        unsigned int aiao_cken             : 1; // [8]
        unsigned int aiao_srst_req         : 1; // [9]
        unsigned int aiao_bclk_oen         : 1; // [10]
        unsigned int aiao_bclk_sel         : 1; // [11]
        unsigned int aiao_bclkin_pctrl     : 1; // [12]
        unsigned int aiao_bclkout_pctrl    : 1; // [13]
        unsigned int aiao_bclk_en          : 1; // [14]
        unsigned int aiao_ws_en            : 1; // [15]
        unsigned int Reserved_0            : 16; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_I2S_CRG_CFG1;


// Define the union U_SPDIF_CRG_CFG0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int aiao_mclk_div_spdif   : 27; // [26..0]
        unsigned int Reserved_0            : 5; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CRG_CFG0;

// Define the union U_SPDIF_CRG_CFG1_03
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int aiao_bclk_div_spdif   : 4; // [3..0]
        unsigned int aiao_fsclk_div_spdif  : 3; // [6..4]
        unsigned int Reserved_1            : 1; // [7]
        unsigned int aiao_cken_spdif       : 1; // [8]
        unsigned int aiao_srst_req_spdif   : 1; // [9]
        unsigned int Reserved_0            : 22; // [31..10]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CRG_CFG1;

// Define the union U_AIAO_RXSWITCH_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int inner_bclk_ws_sel_rx_00  : 4; // [3..0]
        unsigned int inner_bclk_ws_sel_rx_01  : 4; // [7..4]
        unsigned int inner_bclk_ws_sel_rx_02  : 4; // [11..8]
        unsigned int inner_bclk_ws_sel_rx_03  : 4; // [15..12]
        unsigned int inner_bclk_ws_sel_rx_04  : 4; // [19..16]
        unsigned int inner_bclk_ws_sel_rx_05  : 4; // [23..20]
        unsigned int inner_bclk_ws_sel_rx_06  : 4; // [27..24]
        unsigned int inner_bclk_ws_sel_rx_07  : 4; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_AIAO_SWITCH_RX_BCLK;

// Define the union U_AIAO_TXSWITCH_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int inner_bclk_ws_sel_tx_00  : 4; // [3..0]
        unsigned int inner_bclk_ws_sel_tx_01  : 4; // [7..4]
        unsigned int inner_bclk_ws_sel_tx_02  : 4; // [11..8]
        unsigned int inner_bclk_ws_sel_tx_03  : 4; // [15..12]
        unsigned int inner_bclk_ws_sel_tx_04  : 4; // [19..16]
        unsigned int inner_bclk_ws_sel_tx_05  : 4; // [23..20]
        unsigned int inner_bclk_ws_sel_tx_06  : 4; // [27..24]
        unsigned int inner_bclk_ws_sel_tx_07  : 4; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_AIAO_SWITCH_TX_BCLK;

// Define the union U_RX_IF_ATTRI
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_mode               : 2; // [1..0]
        unsigned int rx_i2s_precision      : 2; // [3..2]
        unsigned int rx_ch_num             : 2; // [5..4]
        unsigned int Reserved_1            : 1; // [6]
        unsigned int rx_multislot_en       : 1; // [7]
        unsigned int rx_sd_offset          : 8; // [15..8]
        unsigned int rx_trackmode          : 3; // [18..16]
        unsigned int Reserved_0            : 1; // [19]
        unsigned int rx_sd_source_sel      : 4; // [23..20]
        unsigned int rx_sd0_sel            : 2; // [25..24]
        unsigned int rx_sd1_sel            : 2; // [27..26]
        unsigned int rx_sd2_sel            : 2; // [29..28]
        unsigned int rx_sd3_sel            : 2; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_IF_ATTRI;

// Define the union U_RX_DSP_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int mute_en               : 1; // [0]
        unsigned int mute_fade_en          : 1; // [1]
        unsigned int pause_en              : 1; // [2]
        unsigned int pause_fade_en         : 1; // [3]
        unsigned int Reserved_3            : 4; // [7..4]
        unsigned int volume                : 7; // [14..8]
        unsigned int Reserved_2            : 1; // [15]
        unsigned int fade_in_rate          : 4; // [19..16]
        unsigned int fade_out_rate         : 4; // [23..20]
        unsigned int Reserved_1            : 3; // [26..24]
        unsigned int bypass_en             : 1; // [27]
        unsigned int rx_enable             : 1; // [28]
        unsigned int rx_disable_done       : 1; // [29]
        unsigned int Reserved_0            : 2; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_DSP_CTRL;

// Define the union U_RX_WS_CNT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int ws_count              : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_WS_CNT;

// Define the union U_RX_BCLK_CNT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int bclk_count            : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_BCLK_CNT;

// Define the union U_RX_BUFF_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_buff_size          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_BUFF_SIZE;

// Define the union U_RX_BUFF_WPTR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_buff_wptr          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_BUFF_WPTR;

// Define the union U_RX_BUFF_RPTR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_buff_rptr          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_BUFF_RPTR;

// Define the union U_RX_BUFF_ALFULL_TH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_buff_alfull_th     : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_BUFF_ALFULL_TH;

// Define the union U_RX_TRANS_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_trans_size         : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_TRANS_SIZE;

// Define the union U_RX_WPTR_TMP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_wptr_tmp           : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_WPTR_TMP;

// Define the union U_RX_INT_ENA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_trans_int_ena      : 1; // [0]
        unsigned int rx_full_int_ena       : 1; // [1]
        unsigned int rx_alfull_int_ena     : 1; // [2]
        unsigned int rx_bfifo_full_int_ena  : 1; // [3]
        unsigned int rx_ififo_full_int_ena  : 1; // [4]
        unsigned int rx_stop_int_ena       : 1; // [5]
        unsigned int Reserved_0            : 26; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_INT_ENA;

// Define the union U_RX_INT_RAW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_trans_int_raw      : 1; // [0]
        unsigned int rx_full_int_raw       : 1; // [1]
        unsigned int rx_alfull_int_raw     : 1; // [2]
        unsigned int rx_bfifo_full_int_raw  : 1; // [3]
        unsigned int rx_ififo_full_int_raw  : 1; // [4]
        unsigned int rx_stop_int_raw       : 1; // [5]
        unsigned int Reserved_0            : 26; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_INT_RAW;

// Define the union U_RX_INT_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_trans_int_status   : 1; // [0]
        unsigned int rx_full_int_status    : 1; // [1]
        unsigned int rx_alfull_int_status  : 1; // [2]
        unsigned int rx_bfifo_full_int_status  : 1; // [3] 
        unsigned int rx_ififo_full_int_status  : 1; // [4]
        unsigned int rx_stop_int_status    : 1; // [5]
        unsigned int Reserved_0            : 26; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_INT_STATUS;

// Define the union U_RX_INT_CLR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int rx_trans_int_clear    : 1; // [0]
        unsigned int rx_full_int_clear     : 1; // [1]
        unsigned int rx_alfull_int_clear   : 1; // [2]
        unsigned int rx_bfifo_full_int_clear  : 1; // [3]
        unsigned int rx_ififo_full_int_clear  : 1; // [4]
        unsigned int rx_stop_int_clear     : 1; // [5]
        unsigned int Reserved_0            : 26; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_RX_INT_CLR;

// Define the union U_TX_IF_ATTRI
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_mode               : 2; // [1..0]
        unsigned int tx_i2s_precision      : 2; // [3..2]
        unsigned int tx_ch_num             : 2; // [5..4]
        unsigned int tx_underflow_ctrl     : 1; // [6]
        unsigned int Reserved_1            : 1; // [7]
        unsigned int tx_sd_offset          : 8; // [15..8]
        unsigned int tx_trackmode          : 3; // [18..16]
        unsigned int Reserved_0            : 1; // [19]
        unsigned int tx_sd_source_sel      : 4; // [23..20]
        unsigned int tx_sd0_sel            : 2; // [25..24]
        unsigned int tx_sd1_sel            : 2; // [27..26]
        unsigned int tx_sd2_sel            : 2; // [29..28]
        unsigned int tx_sd3_sel            : 2; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_IF_ATTRI;

// Define the union U_TX_DSP_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mute_en               : 1   ; // [0]
        unsigned int    mute_fade_en          : 1   ; // [1]
        unsigned int    Reserved_3            : 6   ; // [7..2]
        unsigned int    volume                : 7   ; // [14..8]
        unsigned int    Reserved_2            : 1   ; // [15]
        unsigned int    fade_in_rate          : 4   ; // [19..16]
        unsigned int    fade_out_rate         : 4   ; // [23..20]
        unsigned int    Reserved_1            : 3   ; // [26..24]
        unsigned int    bypass_en             : 1   ; // [27]
        unsigned int    tx_enable             : 1   ; // [28]
        unsigned int    tx_disable_done       : 1   ; // [29]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_DSP_CTRL;

// Define the union U_TX_DSP_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mute_fade_vol         : 7   ; // [6..0]
        unsigned int    Reserved_0            : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_DSP_STATUS;

// Define the union U_TX_WS_CNT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int ws_count              : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_WS_CNT;

// Define the union U_TX_BCLK_CNT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int bclk_count            : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_BCLK_CNT;

// Define the union U_TX_BUFF_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_size          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_BUFF_SIZE;

// Define the union U_TX_BUFF_WPTR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_wptr          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_BUFF_WPTR;

// Define the union U_TX_BUFF_RPTR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_rptr          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_BUFF_RPTR;

// Define the union U_TX_BUFF_ALEMPTY_TH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_alempty_th    : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_BUFF_ALEMPTY_TH;

// Define the union U_TX_TRANS_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_trans_size         : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_TRANS_SIZE;

// Define the union U_TX_RPTR_TMP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_rptr_tmp           : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_RPTR_TMP;

// Define the union U_TX_INT_ENA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_ena      : 1   ; // [0]
        unsigned int    tx_empty_int_ena      : 1   ; // [1]
        unsigned int    tx_alempty_int_ena    : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_ena  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_ena  : 1   ; // [4]
        unsigned int    tx_stop_int_ena       : 1   ; // [5]
        unsigned int    tx_mfade_int_ena      : 1   ; // [6]
        unsigned int    tx_dat_break_int_ena  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_INT_ENA;

// Define the union U_TX_INT_RAW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_raw      : 1   ; // [0]
        unsigned int    tx_empty_int_raw      : 1   ; // [1]
        unsigned int    tx_alempty_int_raw    : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_raw  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_raw  : 1   ; // [4]
        unsigned int    tx_stop_int_raw       : 1   ; // [5]
        unsigned int    tx_mfade_int_raw      : 1   ; // [6]
        unsigned int    tx_dat_break_int_raw  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_INT_RAW;

// Define the union U_TX_INT_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_status   : 1   ; // [0]
        unsigned int    tx_empty_int_status   : 1   ; // [1]
        unsigned int    tx_alempty_int_status  : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_status  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_status  : 1   ; // [4]
        unsigned int    tx_stop_int_status    : 1   ; // [5]
        unsigned int    tx_mfade_int_status   : 1   ; // [6]
        unsigned int    tx_dat_break_int_status  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_INT_STATUS;

// Define the union U_TX_INT_CLR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_clear    : 1   ; // [0]
        unsigned int    tx_empty_int_clear    : 1   ; // [1]
        unsigned int    tx_alempty_int_clear  : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_clear  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_clear  : 1   ; // [4]
        unsigned int    tx_stop_int_clear     : 1   ; // [5]
        unsigned int    tx_mfade_int_clear    : 1   ; // [6]
        unsigned int    tx_dat_break_int_clear  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_TX_INT_CLR;

// Define the union U_SPDIFTX_IF_ATTRI
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_2            : 2   ; // [1..0]
        unsigned int    tx_i2s_precision      : 2   ; // [3..2]
        unsigned int    tx_ch_num             : 2   ; // [5..4]
        unsigned int    Reserved_1            : 10  ; // [15..6]
        unsigned int    tx_trackmode          : 3   ; // [18..16]
        unsigned int    Reserved_0            : 13  ; // [31..19]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_IF_ATTRI;

// Define the union U_SPDIFTX_DSP_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mute_en               : 1   ; // [0]
        unsigned int    mute_fade_en          : 1   ; // [1]
        unsigned int    Reserved_3            : 6   ; // [7..2]
        unsigned int    volume                : 7   ; // [14..8]
        unsigned int    Reserved_2            : 1   ; // [15]
        unsigned int    fade_in_rate          : 4   ; // [19..16]
        unsigned int    fade_out_rate         : 4   ; // [23..20]
        unsigned int    Reserved_1            : 3   ; // [26..24]
        unsigned int    bypass_en             : 1   ; // [27]
        unsigned int    tx_enable             : 1   ; // [28]
        unsigned int    tx_disable_done       : 1   ; // [29]
        unsigned int    Reserved_0            : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_DSP_CTRL;

// Define the union U_SPDIFTX_DSP_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mute_fade_vol         : 7   ; // [6..0]
        unsigned int    Reserved_0            : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_DSP_STATUS;

// Define the union U_SPDIFTX_WS_CNT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int ws_count              : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_WS_CNT;

// Define the union U_SPDIFTX_BCLK_CNT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int bclk_count            : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_BCLK_CNT;

// Define the union U_SPDIFTX_BUFF_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_size          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_BUFF_SIZE;

// Define the union U_SPDIFTX_BUFF_WPTR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_wptr          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_BUFF_WPTR;

// Define the union U_SPDIFTX_BUFF_RPTR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_rptr          : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_BUFF_RPTR;

// Define the union U_SPDIFTX_BUFF_ALEMPTY_TH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_buff_alempty_th    : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_BUFF_ALEMPTY_TH;

// Define the union U_SPDIFTX_TRANS_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_trans_size         : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_TRANS_SIZE;

// Define the union U_SPDIFTX_RPTR_TMP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int tx_rptr_tmp           : 24; // [23..0]
        unsigned int Reserved_0            : 8; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_RPTR_TMP;

// Define the union U_SPDIFTX_INT_ENA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_ena      : 1   ; // [0]
        unsigned int    tx_empty_int_ena      : 1   ; // [1]
        unsigned int    tx_alempty_int_ena    : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_ena  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_ena  : 1   ; // [4]
        unsigned int    tx_stop_int_ena       : 1   ; // [5]
        unsigned int    tx_mfade_int_ena      : 1   ; // [6]
        unsigned int    tx_dat_break_int_ena  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_INT_ENA;

// Define the union U_SPDIFTX_INT_RAW
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_raw      : 1   ; // [0]
        unsigned int    tx_empty_int_raw      : 1   ; // [1]
        unsigned int    tx_alempty_int_raw    : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_raw  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_raw  : 1   ; // [4]
        unsigned int    tx_stop_int_raw       : 1   ; // [5]
        unsigned int    tx_mfade_int_raw      : 1   ; // [6]
        unsigned int    tx_dat_break_int_raw  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_INT_RAW;

// Define the union U_SPDIFTX_INT_STATUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_status   : 1   ; // [0]
        unsigned int    tx_empty_int_status   : 1   ; // [1]
        unsigned int    tx_alempty_int_status  : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_status  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_status  : 1   ; // [4]
        unsigned int    tx_stop_int_status    : 1   ; // [5]
        unsigned int    tx_mfade_int_status   : 1   ; // [6]
        unsigned int    tx_dat_break_int_status  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_INT_STATUS;

// Define the union U_SPDIFTX_INT_CLR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tx_trans_int_clear    : 1   ; // [0]
        unsigned int    tx_empty_int_clear    : 1   ; // [1]
        unsigned int    tx_alempty_int_clear  : 1   ; // [2]
        unsigned int    tx_bfifo_empty_int_clear  : 1   ; // [3]
        unsigned int    tx_ififo_empty_int_clear  : 1   ; // [4]
        unsigned int    tx_stop_int_clear     : 1   ; // [5]
        unsigned int    tx_mfade_int_clear    : 1   ; // [6]
        unsigned int    tx_dat_break_int_clear  : 1   ; // [7]
        unsigned int    Reserved_0            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIFTX_INT_CLR;

// Define the union U_SPDIF_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int spdif_en              : 1; // [0]
        unsigned int Reserved_0            : 31; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CTRL;

// Define the union U_SPDIF_CONFIG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int txdata_en             : 1; // [0]
        unsigned int mode                  : 2; // [2..1]
        unsigned int Reserved_0            : 29; // [31..3]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CONFIG;

// Define the union U_SPDIF_INTRUPT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int lfifo_empty_intr      : 1; // [0]
        unsigned int rfifo_empty_intr      : 1; // [1]
        unsigned int lfifo_single_intr     : 1; // [2]
        unsigned int rfifo_single_intr     : 1; // [3]
        unsigned int Reserved_0            : 28; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_INTRUPT;

// Define the union U_SPDIF_INTR_MASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int lfifo_empty_mask      : 1; // [0]
        unsigned int rfifo_empty__mask     : 1; // [1]
        unsigned int lfifo_single__mask    : 1; // [2]
        unsigned int rfifo_single__mask    : 1; // [3]
        unsigned int Reserved_0            : 28; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_INTR_MASK;

// Define the union U_SPDIF_INTR_STATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int lfifo_empty_state     : 1; // [0]
        unsigned int rfifo_empty_state     : 1; // [1]
        unsigned int lfifo_single_state    : 1; // [2]
        unsigned int rfifo_single_state    : 1; // [3]
        unsigned int Reserved_0            : 28; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_INTR_STATE;

// Define the union U_SPDIF_CH_STATUS1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int l_use_a               : 1; // [0]
        unsigned int l_use_b               : 1; // [1]
        unsigned int l_use_c               : 1; // [2]
        unsigned int l_use_d               : 3; // [5..3]
        unsigned int l_use_mode            : 2; // [7..6]
        unsigned int r_use_a               : 1; // [8]
        unsigned int r_use_b               : 1; // [9]
        unsigned int r_use_c               : 1; // [10]
        unsigned int r_use_d               : 3; // [13..11]
        unsigned int r_use_mode            : 2; // [15..14]
        unsigned int Reserved_0            : 16; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CH_STATUS1;

// Define the union U_SPDIF_CH_STATUS2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int l_category_code       : 8; // [7..0]
        unsigned int r_category_code       : 8; // [15..8]
        unsigned int Reserved_0            : 16; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CH_STATUS2;

// Define the union U_SPDIF_CH_STATUS3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int l_src_nb              : 4; // [3..0]
        unsigned int l_ch_nb               : 4; // [7..4]
        unsigned int r_src_nb              : 4; // [11..8]
        unsigned int r_ch_nb               : 4; // [15..12]
        unsigned int Reserved_0            : 16; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CH_STATUS3;

// Define the union U_SPDIF_CH_STATUS4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int l_freq                : 4; // [3..0]
        unsigned int l_clk_accuracy        : 2; // [5..4]
        unsigned int Reserved_1            : 2; // [7..6]
        unsigned int r_freq                : 4; // [11..8]
        unsigned int r_clk_accuracy        : 2; // [13..12]
        unsigned int Reserved_0            : 18; // [31..14]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CH_STATUS4;

// Define the union U_SPDIF_CH_STATUS5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int l_bit_max             : 1; // [0]
        unsigned int l_bit_width           : 3; // [3..1]
        unsigned int l_org_freq            : 4; // [7..4]
        unsigned int r_bit_max             : 1; // [8]
        unsigned int r_bit_width           : 3; // [11..9]
        unsigned int r_org_freq            : 4; // [15..12]
        unsigned int Reserved_0            : 16; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_SPDIF_CH_STATUS5;

// Define the union U_AIO_CRG35
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int aio_hrst_req          : 1; // [0]
        unsigned int aio_hcken             : 1; // [1]
        unsigned int aio_cksel             : 2; // [3..2]
        unsigned int Reserved              : 28; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int u32;
} U_AIO_CRG35;


#endif  // __AIAO_REG_H__

