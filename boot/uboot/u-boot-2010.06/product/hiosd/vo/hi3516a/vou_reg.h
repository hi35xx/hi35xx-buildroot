//******************************************************************************
//  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd.
//
//******************************************************************************
// File name     : c_union_define.h
// Version       : 2.0
// Author        : xxx
// Created       : 2013-04-27
// Last Modified : 
// Description   :  The C union definition file for the module VDP
// Function List : 
// History       : 
// 1 Date        : 
// Author        : d00224772
// Modification  : Create file
//******************************************************************************

#ifndef __VOU_REG_H__
#define __VOU_REG_H__

#include "hi_type.h"
	
#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
// Define the union U_VOCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    m0_arb_mode           : 4   ; // [3..0] 
        unsigned int    Reserved_0            : 27  ; // [30..4] 
        unsigned int    vo_ck_gt_en           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOCTRL;

// Define the union U_VOINTSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dhd0vtthd1_int        : 1   ; // [0] 
        unsigned int    dhd0vtthd2_int        : 1   ; // [1] 
        unsigned int    dhd0vtthd3_int        : 1   ; // [2] 
        unsigned int    dhd0uf_int            : 1   ; // [3] 
        unsigned int    dhd1vtthd1_int        : 1   ; // [4] 
        unsigned int    dhd1vtthd2_int        : 1   ; // [5] 
        unsigned int    dhd1vtthd3_int        : 1   ; // [6] 
        unsigned int    dhd1uf_int            : 1   ; // [7] 
        unsigned int    Reserved_5            : 1   ; // [8] 
        unsigned int    dwbc0_vte_int         : 1   ; // [9] 
        unsigned int    g0wbc_vte_int         : 1   ; // [10] 
        unsigned int    g4wbc_vte_int         : 1   ; // [11] 
        unsigned int    vdac0_unload_int      : 1   ; // [12]
        unsigned int    vdac1_unload_int      : 1   ; // [13]
        unsigned int    vdac2_unload_int      : 1   ; // [14]
        unsigned int    vdac3_unload_int      : 1   ; // [15]
        unsigned int    dsd0vtthd1_int        : 1   ; // [16] 
        unsigned int    dsd0uf_int            : 1   ; // [17] 
        unsigned int    Reserved_4            : 1   ; // [18] 
        unsigned int    v3rr_int              : 1   ; // [19] 
        unsigned int    v4rr_int              : 1   ; // [20] 
        unsigned int    Reserved_3            : 1   ; // [21] 
        unsigned int    g0rr_int              : 1   ; // [22] 
        unsigned int    g1rr_int              : 1   ; // [23] 
        unsigned int    g2rr_int              : 1   ; // [24] 
        unsigned int    Reserved_2            : 1   ; // [25] 
        unsigned int    g4rr_int              : 1   ; // [26]
        unsigned int    Reserved_1            : 3   ; // [29..27] 
        unsigned int    m0_be_int             : 1   ; // [30] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOINTSTA;

// Define the union U_VOMSKINTSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dhd0vtthd1_clr        : 1   ; // [0] 
        unsigned int    dhd0vtthd2_clr        : 1   ; // [1] 
        unsigned int    dhd0vtthd3_clr        : 1   ; // [2] 
        unsigned int    dhd0uf_clr            : 1   ; // [3] 
        unsigned int    dhd1vtthd1_clr        : 1   ; // [4] 
        unsigned int    dhd1vtthd2_clr        : 1   ; // [5] 
        unsigned int    dhd1vtthd3_clr        : 1   ; // [6] 
        unsigned int    dhd1uf_clr            : 1   ; // [7] 
        unsigned int    Reserved_5            : 1   ; // [8] 
        unsigned int    dwbc0_vte_clr         : 1   ; // [9] 
        unsigned int    g0wbc_vte_clr         : 1   ; // [10] 
        unsigned int    g4wbc_vte_clr         : 1   ; // [11] 
        unsigned int    vdac0_unload_int      : 1   ; // [12]
        unsigned int    vdac1_unload_int      : 1   ; // [13]
        unsigned int    vdac2_unload_int      : 1   ; // [14]    
        unsigned int    vdac3_unload_int      : 1   ; // [15] 
        unsigned int    dsd0vtthd1_clr        : 1   ; // [16] 
        unsigned int    dsd0uf_clr            : 1   ; // [17] 
        unsigned int    Reserved_4            : 1   ; // [18] 
        unsigned int    v3rr_clr              : 1   ; // [19] 
        unsigned int    v4rr_clr              : 1   ; // [20] 
        unsigned int    Reserved_3            : 1   ; // [21] 
        unsigned int    g0rr_clr              : 1   ; // [22] 
        unsigned int    g1rr_clr              : 1   ; // [23] 
        unsigned int    g2rr_clr              : 1   ; // [24] 
        unsigned int    Reserved_2            : 1   ; // [25] 
        unsigned int    g4rr_clr              : 1   ; // [26] 
        unsigned int    Reserved_1            : 3   ; // [29..27] 
        unsigned int    m0_be_clr             : 1   ; // [30] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOMSKINTSTA;

// Define the union U_VOINTMSK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dhd0vtthd1_intmsk     : 1   ; // [0] 
        unsigned int    dhd0vtthd2_intmsk     : 1   ; // [1] 
        unsigned int    dhd0vtthd3_intmsk     : 1   ; // [2] 
        unsigned int    dhd0uf_intmsk         : 1   ; // [3] 
        unsigned int    dhd1vtthd1_intmsk     : 1   ; // [4] 
        unsigned int    dhd1vtthd2_intmsk     : 1   ; // [5] 
        unsigned int    dhd1vtthd3_intmsk     : 1   ; // [6] 
        unsigned int    dhd1uf_intmsk         : 1   ; // [7] 
        unsigned int    Reserved_5            : 1   ; // [8] 
        unsigned int    dwbc0_vte_intmsk      : 1   ; // [9] 
        unsigned int    g0wbc_vte_intmsk      : 1   ; // [10] 
        unsigned int    g4wbc_vte_intmsk      : 1   ; // [11] 
        unsigned int    vdac0_unload_intmsk   : 1   ; // [12]
        unsigned int    vdac1_unload_intmsk   : 1   ; // [13]
        unsigned int    vdac2_unload_intmsk   : 1   ; // [14]
        unsigned int    vdac3_unload_intmsk   : 1   ; // [15]
        unsigned int    dsd0vtthd1_intmsk     : 1   ; // [16] 
        unsigned int    dsd0uf_intmsk         : 1   ; // [17] 
        unsigned int    Reserved_4            : 1   ; // [18] 
        unsigned int    v3rr_intmsk           : 1   ; // [19] 
        unsigned int    v4rr_intmsk           : 1   ; // [20] 
        unsigned int    Reserved_3            : 1   ; // [21] 
        unsigned int    g0rr_intmsk           : 1   ; // [22] 
        unsigned int    g1rr_intmsk           : 1   ; // [23] 
        unsigned int    g2rr_intmsk           : 1   ; // [24] 
        unsigned int    Reserved_2            : 1   ; // [25] 
        unsigned int    g4rr_intmsk           : 1   ; // [26]
        unsigned int    Reserved_1            : 3   ; // [29..27] 
        unsigned int    m0_be_intmsk          : 1   ; // [30] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOINTMSK;

// Define the union U_VDPVERSION1
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int vdpversion1             : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_VDPVERSION1;
// Define the union U_VDPVERSION2
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int vdpversion2             : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_VDPVERSION2;
// Define the union U_VODEBUG

// Define the union U_VOAXICTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    m0_outstd_rid0        : 4   ; // [3..0] 
        unsigned int    Reserved_1            : 4   ; // [7..4] 
        unsigned int    m0_wr_ostd            : 4   ; // [11..8] 
        unsigned int    Reserved_0            : 20  ; // [31..12] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOAXICTRL;

// Define the union U_VOWBCARB0

// Define the union U_VOUFSTA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v0_uf_sta             : 1   ; // [0] 
        unsigned int    v1_uf_sta             : 1   ; // [1] 
        unsigned int    Reserved_3            : 1   ; // [2] 
        unsigned int    v3_uf_sta             : 1   ; // [3] 
        unsigned int    Reserved_2            : 4   ; // [7..4] 
        unsigned int    g0_uf_sta             : 1   ; // [8] 
        unsigned int    g1_uf_sta             : 1   ; // [9] 
        unsigned int    g2_uf_sta             : 1   ; // [10] 
        unsigned int    Reserved_1            : 1   ; // [11] 
        unsigned int    g4_uf_sta             : 1   ; // [12] 
        unsigned int    Reserved_0            : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOUFSTA;

// Define the union U_VOUFCLR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v0_uf_clr             : 1   ; // [0] 
        unsigned int    v1_uf_clr             : 1   ; // [1] 
        unsigned int    Reserved_3            : 1   ; // [2] 
        unsigned int    v3_uf_clr             : 1   ; // [3] 
        unsigned int    Reserved_2            : 4   ; // [7..4] 
        unsigned int    g0_uf_clr             : 1   ; // [8] 
        unsigned int    g1_uf_clr             : 1   ; // [9] 
        unsigned int    g2_uf_clr             : 1   ; // [10] 
        unsigned int    Reserved_1            : 1   ; // [11] 
        unsigned int    g4_uf_clr             : 1   ; // [12] 
        unsigned int    Reserved_0            : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOUFCLR;

// Define the union U_VOINTPROC_TIM
#if 0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vointproc_time        : 24  ; // [23..0] 
        unsigned int    Reserved_20           : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VOINTPROC_TIM;
#endif

// Define the union U_VO_MUX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1           : 20  ; // [19..0] 
        unsigned int    bt1120_sel            : 4   ; // [23..20] 
        unsigned int    Reserved_0           : 3   ; // [26..24]
        unsigned int    ye_sel                    : 1   ;//[27]
        unsigned int    digital_sel           : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_MUX;

// Define the union U_VO_MUX_DAC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac0_sel              : 4   ; // [3..0] 
        unsigned int    dac1_sel              : 4   ; // [7..4] 
        unsigned int    dac2_sel              : 4   ; // [11..8] 
        unsigned int    dac3_sel              : 4   ; // [15..12] 
        unsigned int    Reserved_0            : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_MUX_DAC;

// Define the union U_VO_MUX_TESTSYNC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    test_dv               : 1   ; // [0] 
        unsigned int    test_hsync            : 1   ; // [1] 
        unsigned int    test_vsync            : 1   ; // [2] 
        unsigned int    test_field            : 1   ; // [3] 
        unsigned int    Reserved_0            : 27  ; // [30..4] 
        unsigned int    vo_test_en            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_MUX_TESTSYNC;

// Define the union U_VO_MUX_TESTDATA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    test_data             : 30  ; // [29..0] 
        unsigned int    Reserved_0            : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_MUX_TESTDATA;

// Define the union U_VO_DAC_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 16  ; // [15..0] 
        unsigned int    enctr                 : 3   ; // [18..16]  
        unsigned int    Reserved_0            : 11  ; // [29..19] 
        unsigned int    enextref              : 1   ; // [30] 
        unsigned int    envbg                 : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_DAC_CTRL;

// Define the union U_VO_DAC_C_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 15  ; // [14..0] 
        unsigned int    dacgc                 : 6   ; // [20..15] 
        unsigned int    Reserved_0            : 10  ; // [30..21] 
        unsigned int    endac                 : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_DAC_C_CTRL;

// Define the union U_VO_DAC_R_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 15  ; // [14..0] 
        unsigned int    dacgc                 : 6   ; // [20..15] 
        unsigned int    Reserved_0            : 10  ; // [30..21] 
        unsigned int    endac                 : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_DAC_R_CTRL;

// Define the union U_VO_DAC_G_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 15  ; // [14..0] 
        unsigned int    dacgc                 : 6   ; // [20..15] 
        unsigned int    Reserved_0            : 10  ; // [30..21] 
        unsigned int    endac                 : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_DAC_G_CTRL;

// Define the union U_VO_DAC_B_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 15  ; // [14..0] 
        unsigned int    dacgc                 : 6   ; // [20..15] 
        unsigned int    Reserved_0            : 10  ; // [30..21] 
        unsigned int    endac                 : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_DAC_B_CTRL;

// Define the union U_VO_DAC_STAT0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 16  ; // [15..0]
        unsigned int    cableout0             : 1   ; // [16]
        unsigned int    cableout1             : 1   ; // [17]
        unsigned int    cableout2             : 1   ; // [18]
        unsigned int    cableout3             : 1   ; // [19]
        unsigned int    Reserved_0            : 12  ; // [31..20] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_DAC_STAT0;

// Define the union U_VO_DAC_STAT1

// Define the union U_COEF_DATA
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int coef_data               : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_COEF_DATA;

// Define the union U_V0_PARARD

// Define the union U_V0_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 4   ; // [3..0] 
        unsigned int    Reserved_5            : 1   ; // [4] 
        unsigned int    time_out              : 3   ; // [7..5] 
        unsigned int    dcmp_en               : 1   ; // [8]  
        unsigned int    Reserved_4            : 3   ; // [11..9] 
        unsigned int    chm_rmode             : 2   ; // [13..12] 
        unsigned int    lm_rmode              : 2   ; // [15..14] 
        unsigned int    Reserved_3            : 1   ; // [16] 
        unsigned int    vup_mode              : 1   ; // [17] 
        unsigned int    ifir_mode             : 2   ; // [19..18]
        unsigned int    precharge_mode        : 1   ; // [20] 
        unsigned int    precharge_en          : 1   ; // [21]
        unsigned int    Reserved_2            : 1   ; // [22]
        unsigned int    crop_en               : 1   ; // [23]
        unsigned int    Reserved_1            : 3   ; // [26..24] 
        unsigned int    mute_en               : 1   ; // [27] 
        unsigned int    Reserved_0            : 3   ; // [30..28] 
        unsigned int    surface_en            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CTRL;

// Define the union U_V0_UPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0] 
        unsigned int    Reserved_0            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_UPD;

// Define the union U_V0_PRERD
#if 0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_69           : 31  ; // [30..0] 
        unsigned int    pre_rd_en             : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_PRERD;
#endif

// Define the union U_V0_IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0] 
        unsigned int    ih                    : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_IRESO;

// Define the union U_V0_ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ow                    : 12  ; // [11..0] 
        unsigned int    oh                    : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_ORESO;

// Define the union U_V0_CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    galpha                : 8   ; // [7..0] 
        unsigned int    Reserved_0            : 24  ; // [31..8] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CBMPARA;


// Define the union U_V0_CPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    src_xfpos             : 12  ; // [11..0] 
        unsigned int    src_xlpos             : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CPOS;

// Define the union U_V0_DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0] 
        unsigned int    disp_yfpos            : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_DFPOS;

// Define the union U_V0_DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0] 
        unsigned int    disp_ylpos            : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_DLPOS;

// Define the union U_V0_VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xfpos           : 12  ; // [11..0] 
        unsigned int    video_yfpos           : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_VFPOS;

// Define the union U_V0_VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xlpos           : 12  ; // [11..0] 
        unsigned int    video_ylpos           : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_VLPOS;

// Define the union U_V0_BK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbk_cr                : 10  ; // [9..0] 
        unsigned int    vbk_cb                : 10  ; // [19..10] 
        unsigned int    vbk_y                 : 10  ; // [29..20] 
        unsigned int    Reserved_0            : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_BK;

// Define the union U_V0_ALPHA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbk_alpha             : 8   ; // [7..0] 
        unsigned int    Reserved_0            : 24  ; // [31..8] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_ALPHA;

// Define the union U_V0_CSC_IDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 11  ; // [10..0] 
        unsigned int    cscidc1               : 11  ; // [21..11] 
        unsigned int    csc_en                : 1   ; // [22] 
        unsigned int    Reserved_0            : 9   ; // [31..23] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_IDC;

// Define the union U_V0_CSC_ODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 11  ; // [10..0] 
        unsigned int    cscodc1               : 11  ; // [21..11] 
        unsigned int    csc_sign_mode         : 1   ; // [22] 
        unsigned int    Reserved_0            : 9   ; // [31..23] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_ODC;

// Define the union U_V0_CSC_IODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc2               : 11  ; // [10..0] 
        unsigned int    cscodc2               : 11  ; // [21..11] 
        unsigned int    Reserved_0            : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_IODC;

// Define the union U_V0_CSC_P0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp01                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_P0;

// Define the union U_V0_CSC_P1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp10                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_P1;

// Define the union U_V0_CSC_P2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp12                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_P2;

// Define the union U_V0_CSC_P3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp21                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_P3;

// Define the union U_V0_CSC_P4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 15  ; // [14..0] 
        unsigned int    Reserved_0            : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_CSC_P4;

// Define the union U_V0_HSP

// Define the union U_V0_IFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef0                 : 10  ; // [9..0] 
        unsigned int    Reserved_1            : 6   ; // [15..10] 
        unsigned int    coef1                 : 10  ; // [25..16] 
        unsigned int    Reserved_0            : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_IFIRCOEF01;

// Define the union U_V0_IFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef2                 : 10  ; // [9..0] 
        unsigned int    Reserved_1            : 6   ; // [15..10] 
        unsigned int    coef3                 : 10  ; // [25..16] 
        unsigned int    Reserved_0            : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_IFIRCOEF23;

// Define the union U_V0_IFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef4                 : 10  ; // [9..0] 
        unsigned int    Reserved_1            : 6   ; // [15..10] 
        unsigned int    coef5                 : 10  ; // [25..16] 
        unsigned int    Reserved_0            : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_IFIRCOEF45;

// Define the union U_V0_IFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef6                 : 10  ; // [9..0] 
        unsigned int    Reserved_1            : 6   ; // [15..10] 
        unsigned int    coef7                 : 10  ; // [25..16] 
        unsigned int    Reserved_0            : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_IFIRCOEF67;



// Define the union U_V0_P0LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int surface_addr            : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_P0LADDR;
// Define the union U_V0_P0CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int surface_addr            : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_P0CADDR;
// Define the union U_V0_P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0] 
        unsigned int    surface_cstride       : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_P0STRIDE;

// Define the union U_V0_P0VFPOS

// Define the union U_V0_LADDROFFSET
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int laddr_offset            : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_LADDROFFSET;
// Define the union U_V0_CADDROFFSET
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int caddr_offset            : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_CADDROFFSET;


// Define the union U_V0_DCMP_LSTATE0
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int dcmp_l_state0           : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_DCMP_LSTATE0;
// Define the union U_V0_DCMP_LSTATE1
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int dcmp_l_state1           : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_DCMP_LSTATE1;
// Define the union U_V0_DCMP_CSTATE0
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int dcmp_c_state0           : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_DCMP_CSTATE0;
// Define the union U_V0_DCMP_CSTATE1
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int dcmp_c_state1           : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_V0_DCMP_CSTATE1;
// Define the union U_VO_DCMPERRCLR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dcmp_l_errclr         : 1   ; // [0] 
        unsigned int    dcmp_c_errclr         : 1   ; // [1] 
        unsigned int    Reserved_190          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_VO_DCMPERRCLR;

// Define the union U_V0_DCMP_ERR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dcmp_l_wrong          : 1   ; // [0] 
        unsigned int    dcmp_c_wrong          : 1   ; // [1] 
        unsigned int    Reserved_191          : 30  ; // [31..2] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_V0_DCMP_ERR;


// Define the union U_G0_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ifmt                  : 8   ; // [7..0]
        unsigned int    bitext                : 2   ; // [9..8]
        unsigned int    Reserved_2            : 10  ; // [19..10] 
        unsigned int    precharge_mode        : 1   ; // [20] 
        unsigned int    precharge_en          : 1   ; // [21] 
        unsigned int    Reserved_1            : 4   ; // [25..22] 
        unsigned int    read_mode             : 1   ; // [26] 
        unsigned int    upd_mode              : 1   ; // [27]  
        unsigned int    Reserved_0            : 3   ; // [30..28] 
        unsigned int    surface_en            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CTRL;

// Define the union U_G0_UPD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0] 
        unsigned int    Reserved_0            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_UPD;

// Define the union U_G0_ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int surface_addr            : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_G0_ADDR;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    surface_stride        : 16  ; // [15..0] 
        unsigned int    Reserved_0            : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_STRIDE;

// Define the union U_G0_IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    iw                    : 12  ; // [11..0] 
        unsigned int    ih                    : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_IRESO;

// Define the union U_G0_SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    src_xfpos             : 7   ; // [6..0] 
        unsigned int    Reserved_0            : 25  ; // [31..7] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_SFPOS;

// Define the union U_G0_CBMPARA
typedef union
{
    // Define the struct bits
    struct    
	{
		unsigned int	galpha				  : 8	; // [7..0] 
		unsigned int	palpha_range		  : 1	; // [8] 
		unsigned int	Reserved_1			  : 1	; // [9] 
		unsigned int	vedge_p 			  : 1	; // [10] 
		unsigned int	hedge_p 			  : 1	; // [11] 
		unsigned int	palpha_en			  : 1	; // [12] 
		unsigned int	premult_en			  : 1	; // [13] 
		unsigned int	key_en				  : 1	; // [14] 
		unsigned int	key_mode			  : 1	; // [15] 
		unsigned int	Reserved_0			  : 16	; // [31..16] 
	} bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CBMPARA;

// Define the union U_G0_CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_max              : 8   ; // [7..0] 
        unsigned int    keyg_max              : 8   ; // [15..8] 
        unsigned int    keyr_max              : 8   ; // [23..16] 
        unsigned int    va0                   : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CKEYMAX;

// Define the union U_G0_CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    keyb_min              : 8   ; // [7..0] 
        unsigned int    keyg_min              : 8   ; // [15..8] 
        unsigned int    keyr_min              : 8   ; // [23..16] 
        unsigned int    va1                   : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CKEYMIN;

// Define the union U_G0_CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    kmsk_b                : 8   ; // [7..0] 
        unsigned int    kmsk_g                : 8   ; // [15..8] 
        unsigned int    kmsk_r                : 8   ; // [23..16] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CMASK;

// Define the union U_G0_PARAADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int surface_addr            : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_G0_PARAADDR;
// Define the union U_G0_PARAUP
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    gdc_paraup            : 1   ; // [0] 
        unsigned int    Reserved_0            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_PARAUP;


// Define the union U_G0_DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xfpos            : 12  ; // [11..0] 
        unsigned int    disp_yfpos            : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_DFPOS;

// Define the union U_G0_DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    disp_xlpos            : 12  ; // [11..0] 
        unsigned int    disp_ylpos            : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_DLPOS;

// Define the union U_G0_VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xfpos           : 12  ; // [11..0] 
        unsigned int    video_yfpos           : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_VFPOS;

// Define the union U_G0_VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_xlpos           : 12  ; // [11..0] 
        unsigned int    video_ylpos           : 12  ; // [23..12] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_VLPOS;

// Define the union U_G0_BK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbk_cr                : 10  ; // [9..0] 
        unsigned int    vbk_cb                : 10  ; // [19..10] 
        unsigned int    vbk_y                 : 10  ; // [29..20] 
        unsigned int    Reserved_0            : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_BK;

// Define the union U_G0_ALPHA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vbk_alpha             : 8   ; // [7..0] 
        unsigned int    Reserved_0            : 24  ; // [31..8] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_ALPHA;



// Define the union U_G0_CSC_IDC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc0               : 11  ; // [10..0] 
        unsigned int    cscidc1               : 11  ; // [21..11] 
        unsigned int    csc_en                : 1   ; // [22] 
        unsigned int    csc_mode              : 3   ; // [25..23] 
        unsigned int    Reserved_0            : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_IDC;

// Define the union U_G0_CSC_ODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscodc0               : 11  ; // [10..0] 
        unsigned int    cscodc1               : 11  ; // [21..11] 
        unsigned int    csc_sign_mode         : 1   ; // [22] 
        unsigned int    Reserved_0            : 9   ; // [31..23] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_ODC;

// Define the union U_G0_CSC_IODC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscidc2               : 11  ; // [10..0] 
        unsigned int    cscodc2               : 11  ; // [21..11] 
        unsigned int    Reserved_0            : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_IODC;

// Define the union U_G0_CSC_P0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp00                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp01                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_P0;

// Define the union U_G0_CSC_P1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp02                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp10                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_P1;

// Define the union U_G0_CSC_P2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp11                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp12                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_P2;

// Define the union U_G0_CSC_P3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp20                : 15  ; // [14..0] 
        unsigned int    Reserved_1            : 1   ; // [15] 
        unsigned int    cscp21                : 15  ; // [30..16] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_P3;

// Define the union U_G0_CSC_P4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cscp22                : 15  ; // [14..0] 
        unsigned int    Reserved_0            : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_G0_CSC_P4;


// Define the union U_CBM_BKG2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cbm_bkgcr2            : 10  ; // [9..0] 
        unsigned int    cbm_bkgcb2            : 10  ; // [19..10] 
        unsigned int    cbm_bkgy2             : 10  ; // [29..20] 
        unsigned int    Reserved_0            : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBM_BKG2;

// Define the union U_CBM_MIX2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    mixer_prio0           : 4   ; // [3..0] 
        unsigned int    mixer_prio1           : 4   ; // [7..4] 
        unsigned int    mixer_prio2           : 4   ; // [11..8] 
        unsigned int    mixer_prio3           : 4   ; // [15..12] 
        unsigned int    Reserved_0            : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_CBM_MIX2;


// Define the union U_DHD0_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    regup                 : 1   ; // [0] 
        unsigned int    Reserved_3            : 3   ; // [3..1] 
        unsigned int    iop                   : 1   ; // [4]
        unsigned int    Reserved_2            : 4   ; // [8..5]
        unsigned int    precharge_mode        : 1   ; // [9]
        unsigned int    precharge_en          : 1   ; // [10]
        unsigned int    Reserved_1            : 9   ; // [19..11] 
        unsigned int    fpga_lmt_width        : 7   ; // [26..20] 
        unsigned int    fpga_lmt_en           : 1   ; // [27] 
        unsigned int    Reserved_0            : 1   ; // [28] 
        unsigned int    cbar_sel              : 1   ; // [29] 
        unsigned int    cbar_en               : 1   ; // [30] 
        unsigned int    intf_en               : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_CTRL;

// Define the union U_DHD0_VSYNC
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vact                  : 12  ; // [11..0] 
        unsigned int    vbb                   : 8   ; // [19..12] 
        unsigned int    vfb                   : 8   ; // [27..20] 
        unsigned int    Reserved_0            : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_VSYNC;

// Define the union U_DHD0_HSYNC1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hact                  : 16  ; // [15..0] 
        unsigned int    hbb                   : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_HSYNC1;

// Define the union U_DHD0_HSYNC2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hfb                   : 16  ; // [15..0] 
        unsigned int    hmid                  : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_HSYNC2;

// Define the union U_DHD0_VPLUS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    bvact                 : 12  ; // [11..0] 
        unsigned int    bvbb                  : 8   ; // [19..12] 
        unsigned int    bvfb                  : 8   ; // [27..20] 
        unsigned int    Reserved_0            : 4   ; // [31..28] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_VPLUS;

// Define the union U_DHD0_PWR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hpw                   : 16  ; // [15..0] 
        unsigned int    vpw                   : 8   ; // [23..16] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_PWR;

// Define the union U_DHD0_VTTHD3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vtmgthd3              : 13  ; // [12..0] 
        unsigned int    Reserved_1            : 2   ; // [14..13] 
        unsigned int    thd3_mode             : 1   ; // [15] 
        unsigned int    vtmgthd4              : 13  ; // [28..16] 
        unsigned int    Reserved_0            : 2   ; // [30..29] 
        unsigned int    thd4_mode             : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_VTTHD3;

// Define the union U_DHD0_VTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vtmgthd1              : 13  ; // [12..0] 
        unsigned int    Reserved_1            : 2   ; // [14..13] 
        unsigned int    thd1_mode             : 1   ; // [15] 
        unsigned int    vtmgthd2              : 13  ; // [28..16] 
        unsigned int    Reserved_0            : 2   ; // [30..29] 
        unsigned int    thd2_mode             : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_VTTHD;



// Define the union U_DHD0_AFFTHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dhd_aff_thd           : 6    ; // [5..0]
        unsigned int    Reserved_0            : 26   ; // [31..6] 
    }bits;

    // Define an unsigned member
    unsigned int    u32;
    
}U_DHD0_AFFTHD;



typedef union
{
    // Define the struct bits
    struct
    {       
        unsigned int    vdac_det_high         : 10  ; // [9..0]         
        unsigned int    Reserved_1            : 6   ; // [15..10] 
        unsigned int    det_line              : 10  ; // [25..16] 
        unsigned int    Reserved_0            : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_VGA_DACDET1;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    det_pixel_sta         : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11]         
        unsigned int    det_pixel_wid         : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 4   ; // [30..27] 
        unsigned int    vdac_det_en           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_VGA_DACDET2;


// Define the union U_DHD0_PARATHD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    para_thd              : 6   ; // [5..0] 
        unsigned int    Reserved_0            : 25  ; // [30..6] 
        unsigned int    dfs_en                : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_PARATHD;

// Define the union U_DHD0_START_POS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    start_pos             : 8   ; // [7..0] 
        unsigned int    timing_start_pos      : 8   ; // [15..8] 
        unsigned int    Reserved_0            : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_START_POS;



// Define the union U_DHD0_STATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vback_blank           : 1   ; // [0] 
        unsigned int    vblank                : 1   ; // [1] 
        unsigned int    bottom_field          : 1   ; // [2] 
        unsigned int    vcnt                  : 13  ; // [15..3] 
        unsigned int    count_int             : 8   ; // [23..16] 
        unsigned int    Reserved_0            : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_STATE;

// Define the union U_DHD0_DEBUG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pix_h                 : 16  ; // [15..0] 
        unsigned int    pix_v                 : 12  ; // [27..16] 
        unsigned int    pix_src               : 3   ; // [30..28] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_DEBUG;

// Define the union U_DHD0_DEBUG_STATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pixel_value           : 30  ; // [29..0] 
        unsigned int    Reserved_0            : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHD0_DEBUG_STATE;

// Define the union U_BT_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_1            : 30  ; // [29..0] 
        unsigned int    bt1120_bypass         : 1   ; // [30] 
        unsigned int    Reserved_0            : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_BT_CTRL;


// Define the union U_INTF_SYNC_INV
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dv_inv                : 1   ; // [0] 
        unsigned int    hs_inv                : 1   ; // [1] 
        unsigned int    vs_inv                : 1   ; // [2] 
        unsigned int    f_inv                 : 1   ; // [3] 
        unsigned int    Reserved_0            : 28  ; // [31..4] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_INTF_SYNC_INV;

// Define the union U_INTF_CLIP0_L
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_cl0              : 10  ; // [9..0] 
        unsigned int    clip_cl1              : 10  ; // [19..10] 
        unsigned int    clip_cl2              : 10  ; // [29..20] 
        unsigned int    Reserved_0            : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_INTF_CLIP0_L;

// Define the union U_INTF_CLIP0_H
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clip_ch0              : 10  ; // [9..0] 
        unsigned int    clip_ch1              : 10  ; // [19..10] 
        unsigned int    clip_ch2              : 10  ; // [29..20] 
        unsigned int    Reserved_0            : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_INTF_CLIP0_H;



// Define the union U_INTF_DITHER0_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0            : 29  ; // [28..0] 
        unsigned int    dither_md             : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_INTF_DITHER0_CTRL;

// Define the union U_INTF_DITHER0_COEF0
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dither_coef0          : 8   ; // [7..0] 
        unsigned int    dither_coef1          : 8   ; // [15..8] 
        unsigned int    dither_coef2          : 8   ; // [23..16] 
        unsigned int    dither_coef3          : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_INTF_DITHER0_COEF0;

// Define the union U_INTF_DITHER0_COEF1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dither_coef4          : 8   ; // [7..0] 
        unsigned int    dither_coef5          : 8   ; // [15..8] 
        unsigned int    dither_coef6          : 8   ; // [23..16] 
        unsigned int    dither_coef7          : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_INTF_DITHER0_COEF1;


typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_seq                : 1   ; // [0] 
        unsigned int    chgain_en             : 1   ; // [1] 
        unsigned int    sylp_en               : 1   ; // [2] 
        unsigned int    chlp_en               : 1   ; // [3] 
        unsigned int    oversam2_en           : 1   ; // [4] 
        unsigned int    lunt_en               : 1   ; // [5] 
        unsigned int    oversam_en            : 2   ; // [7..6] 
        unsigned int    Reserved_1            : 1   ; // [8] 
        unsigned int    luma_dl               : 4   ; // [12..9] 
        unsigned int    agc_amp_sel           : 1   ; // [13] 
        unsigned int    length_sel            : 1   ; // [14] 
        unsigned int    sync_mode_scart       : 1   ; // [15] 
        unsigned int    sync_mode_sel         : 2   ; // [17..16] 
        unsigned int    style_sel             : 4   ; // [21..18] 
        unsigned int    fm_sel                : 1   ; // [22] 
        unsigned int    vbi_lpf_en            : 1   ; // [23] 
        unsigned int    rgb_en                : 1   ; // [24] 
        unsigned int    scanline              : 1   ; // [25] 
        unsigned int    pbpr_lpf_en           : 1   ; // [26] 
        unsigned int    pal_half_en           : 1   ; // [27] 
        unsigned int    Reserved_0            : 1   ; // [28] 
        unsigned int    dis_ire               : 1   ; // [29] 
        unsigned int    clpf_sel              : 2   ; // [31..30] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF0;

// Define the union U_DATE_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac_test              : 10  ; // [9..0] 
        unsigned int    date_test_mode        : 2   ; // [11..10] 
        unsigned int    date_test_en          : 1   ; // [12] 
        unsigned int    amp_outside           : 10  ; // [22..13] 
        unsigned int    c_limit_en            : 1   ; // [23] 
        unsigned int    cc_seq                : 1   ; // [24] 
        unsigned int    cgms_seq              : 1   ; // [25] 
        unsigned int    vps_seq               : 1   ; // [26] 
        unsigned int    wss_seq               : 1   ; // [27] 
        unsigned int    cvbs_limit_en         : 1   ; // [28] 
        unsigned int    c_gain                : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF1;

// Define the union U_DATE_COEFF2
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int    coef02      : 32   ; // [31..0]         
     } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_DATE_COEFF2;
// Define the union U_DATE_COEFF3
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef03        : 26   ; // [25..0] 
        unsigned int    Reserved_0    : 6    ; // [31..26]         
     } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF3;

// Define the union U_DATE_COEFF4
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef04       : 30   ; // [29..0] 
        unsigned int    Reserved_0   : 2    ; // [31..30]         
     } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF4;

// Define the union U_DATE_COEFF5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef05       : 29   ; // [28..0] 
        unsigned int    Reserved_0   : 3    ; // [31..29]
     }  bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF5;

// Define the union U_DATE_COEFF6
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef06_1          : 23  ; // [22..0] 
        unsigned int    Reserved_0        : 8   ; // [30..23]         
        unsigned int    coef06_0          : 1   ; // [31]
     } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF6;

// Define the union U_DATE_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt07_enf2             : 1   ; // [0] 
        unsigned int    tt08_enf2             : 1   ; // [1] 
        unsigned int    tt09_enf2             : 1   ; // [2] 
        unsigned int    tt10_enf2             : 1   ; // [3] 
        unsigned int    tt11_enf2             : 1   ; // [4] 
        unsigned int    tt12_enf2             : 1   ; // [5] 
        unsigned int    tt13_enf2             : 1   ; // [6] 
        unsigned int    tt14_enf2             : 1   ; // [7] 
        unsigned int    tt15_enf2             : 1   ; // [8] 
        unsigned int    tt16_enf2             : 1   ; // [9] 
        unsigned int    tt17_enf2             : 1   ; // [10] 
        unsigned int    tt18_enf2             : 1   ; // [11] 
        unsigned int    tt19_enf2             : 1   ; // [12] 
        unsigned int    tt20_enf2             : 1   ; // [13] 
        unsigned int    tt21_enf2             : 1   ; // [14] 
        unsigned int    tt22_enf2             : 1   ; // [15] 
        unsigned int    tt07_enf1             : 1   ; // [16] 
        unsigned int    tt08_enf1             : 1   ; // [17] 
        unsigned int    tt09_enf1             : 1   ; // [18] 
        unsigned int    tt10_enf1             : 1   ; // [19] 
        unsigned int    tt11_enf1             : 1   ; // [20] 
        unsigned int    tt12_enf1             : 1   ; // [21] 
        unsigned int    tt13_enf1             : 1   ; // [22] 
        unsigned int    tt14_enf1             : 1   ; // [23] 
        unsigned int    tt15_enf1             : 1   ; // [24] 
        unsigned int    tt16_enf1             : 1   ; // [25] 
        unsigned int    tt17_enf1             : 1   ; // [26] 
        unsigned int    tt18_enf1             : 1   ; // [27] 
        unsigned int    tt19_enf1             : 1   ; // [28] 
        unsigned int    tt20_enf1             : 1   ; // [29] 
        unsigned int    tt21_enf1             : 1   ; // [30] 
        unsigned int    tt22_enf1             : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF7;

// Define the union U_DATE_COEFF8
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int tt_staddr               : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_DATE_COEFF8;
// Define the union U_DATE_COEFF9
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int tt_edaddr               : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_DATE_COEFF9;
// Define the union U_DATE_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_pktoff             : 8   ; // [7..0] 
        unsigned int    tt_mode               : 2   ; // [9..8] 
        unsigned int    tt_highest            : 1   ; // [10] 
        unsigned int    full_page             : 1   ; // [11] 
        unsigned int    nabts_100ire          : 1   ; // [12] 
        unsigned int    Reserved_661          : 18  ; // [30..13] 
        unsigned int    tt_ready              : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF10;

// Define the union U_DATE_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    date_clf2             : 10  ; // [9..0] 
        unsigned int    date_clf1             : 10  ; // [19..10] 
        unsigned int    cc_enf2               : 1   ; // [20] 
        unsigned int    cc_enf1               : 1   ; // [21] 
        unsigned int    Reserved_0            : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF11;

// Define the union U_DATE_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cc_f2data             : 16  ; // [15..0] 
        unsigned int    cc_f1data             : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF12;

// Define the union U_DATE_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cg_f1data             : 20  ; // [19..0] 
        unsigned int    cg_enf2               : 1   ; // [20] 
        unsigned int    cg_enf1               : 1   ; // [21] 
        unsigned int    Reserved_0            : 10  ; // [31..22] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF13;

// Define the union U_DATE_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cg_f2data             : 20  ; // [19..0] 
        unsigned int    Reserved_0            : 12  ; // [31..20] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF14;

// Define the union U_DATE_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wss_data              : 14  ; // [13..0] 
        unsigned int    wss_en                : 1   ; // [14] 
        unsigned int    Reserved_0            : 17  ; // [31..15] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF15;

// Define the union U_DATE_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vps_data              : 24  ; // [23..0] 
        unsigned int    vps_en                : 1   ; // [24] 
        unsigned int    Reserved_0            : 7   ; // [31..25] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF16;

// Define the union U_DATE_COEFF17
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int vps_data                : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_DATE_COEFF17;
// Define the union U_DATE_COEFF18
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int vps_data                : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_DATE_COEFF18;
// Define the union U_DATE_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vps_data              : 16  ; // [15..0] 
        unsigned int    Reserved_0            : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF19;

// Define the union U_DATE_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt05_enf2             : 1   ; // [0] 
        unsigned int    tt06_enf2             : 1   ; // [1] 
        unsigned int    tt06_enf1             : 1   ; // [2] 
        unsigned int    Reserved_0            : 29  ; // [31..3] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF20;

// Define the union U_DATE_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac0_in_sel           : 3   ; // [2..0] 
        unsigned int    Reserved_5            : 1   ; // [3] 
        unsigned int    dac1_in_sel           : 3   ; // [6..4] 
        unsigned int    Reserved_4            : 1   ; // [7] 
        unsigned int    dac2_in_sel           : 3   ; // [10..8] 
        unsigned int    Reserved_3            : 1   ; // [11] 
        unsigned int    dac3_in_sel           : 3   ; // [14..12] 
        unsigned int    Reserved_2            : 1   ; // [15] 
        unsigned int    dac4_in_sel           : 3   ; // [18..16] 
        unsigned int    Reserved_1            : 1   ; // [19] 
        unsigned int    dac5_in_sel           : 3   ; // [22..20] 
        unsigned int    Reserved_0            : 9   ; // [31..23] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF21;

// Define the union U_DATE_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    video_phase_delta     : 11  ; // [10..0] 
        unsigned int    Reserved_0            : 21  ; // [31..11] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF22;

// Define the union U_DATE_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    dac0_out_dly          : 3   ; // [2..0] 
        unsigned int    Reserved_5            : 1   ; // [3] 
        unsigned int    dac1_out_dly          : 3   ; // [6..4] 
        unsigned int    Reserved_4            : 1   ; // [7] 
        unsigned int    dac2_out_dly          : 3   ; // [10..8] 
        unsigned int    Reserved_3            : 1   ; // [11] 
        unsigned int    dac3_out_dly          : 3   ; // [14..12] 
        unsigned int    Reserved_2            : 1   ; // [15] 
        unsigned int    dac4_out_dly          : 3   ; // [18..16] 
        unsigned int    Reserved_1            : 1   ; // [19] 
        unsigned int    dac5_out_dly          : 3   ; // [22..20] 
        unsigned int    Reserved_0            : 9   ; // [31..23] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF23;

// Define the union U_DATE_COEFF24
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int burst_start             : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_DATE_COEFF24;
// Define the union U_DATE_COEFF25
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    x_n_coef              : 13  ; // [12..0] 
        unsigned int    Reserved_1            : 3   ; // [15..13] 
        unsigned int    x_n_1_coef            : 13  ; // [28..16] 
        unsigned int    Reserved_0            : 3   ; // [31..29] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF25;

// Define the union U_DATE_COEFF26
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    x_n_1_coef            : 13  ; // [12..0] 
        unsigned int    Reserved_0            : 19  ; // [31..13] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF26;

// Define the union U_DATE_COEFF27
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    y_n_coef              : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    y_n_1_coef            : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF27;

// Define the union U_DATE_COEFF28
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pixel_begin1          : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    pixel_begin2          : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF28;

// Define the union U_DATE_COEFF29
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pixel_end             : 11  ; // [10..0] 
        unsigned int    Reserved_0            : 21  ; // [31..11] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF29;

// Define the union U_DATE_COEFF30
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    g_secam               : 7   ; // [6..0] 
        unsigned int    Reserved_0            : 25  ; // [31..7] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF30;

// Define the union U_DATE_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_mask               : 1   ; // [0] 
        unsigned int    Reserved_0            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_ISRMASK;

// Define the union U_DATE_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_status             : 1   ; // [0] 
        unsigned int    Reserved_0            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_ISRSTATE;

// Define the union U_DATE_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    tt_int                : 1   ; // [0] 
        unsigned int    Reserved_0            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_ISR;

// Define the union U_DATE_VERSION
typedef union
{
    // Define the struct bits 
    struct
    {
        unsigned int Reserved_0              : 32  ; // [31..0] 
    } bits;
 
    // Define an unsigned member
        unsigned int    u32;
 
} U_DATE_VERSION;
// Define the union U_DATE_COEFF37
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    fir_y1_coeff0         : 8   ; // [7..0] 
        unsigned int    fir_y1_coeff1         : 8   ; // [15..8] 
        unsigned int    fir_y1_coeff2         : 8   ; // [23..16] 
        unsigned int    fir_y1_coeff3         : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF37;

// Define the union U_DATE_COEFF38
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    fir_y2_coeff0         : 16  ; // [15..0] 
        unsigned int    fir_y2_coeff1         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF38;

// Define the union U_DATE_COEFF39
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    fir_y2_coeff2         : 16  ; // [15..0] 
        unsigned int    fir_y2_coeff3         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF39;

// Define the union U_DATE_COEFF40
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    fir_c1_coeff0         : 8   ; // [7..0] 
        unsigned int    fir_c1_coeff1         : 8   ; // [15..8] 
        unsigned int    fir_c1_coeff2         : 8   ; // [23..16] 
        unsigned int    fir_c1_coeff3         : 8   ; // [31..24] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF40;

// Define the union U_DATE_COEFF41
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    fir_c2_coeff0         : 16  ; // [15..0] 
        unsigned int    fir_c2_coeff1         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF41;

// Define the union U_DATE_COEFF42
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    fir_c2_coeff2         : 16  ; // [15..0] 
        unsigned int    fir_c2_coeff3         : 16  ; // [31..16] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF42;

// Define the union U_DATE_DACDET1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    vdac_det_high         : 10  ; // [9..0] 
        unsigned int    Reserved_1            : 6   ; // [15..10] 
        unsigned int    det_line              : 10  ; // [25..16] 
        unsigned int    Reserved_0            : 6   ; // [31..26] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_DACDET1;

// Define the union U_DATE_DACDET2
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    det_pixel_sta         : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    det_pixel_wid         : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 4   ; // [30..27] 
        unsigned int    vdac_det_en           : 1   ; // [31] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_DACDET2;

// Define the union U_DATE_COEFF50
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovs_coeff0            : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    ovs_coeff1            : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF50;

// Define the union U_DATE_COEFF51
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovs_coeff0            : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    ovs_coeff1            : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF51;

// Define the union U_DATE_COEFF52
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovs_coeff0            : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    ovs_coeff1            : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF52;

// Define the union U_DATE_COEFF53
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovs_coeff0            : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    ovs_coeff1            : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF53;

// Define the union U_DATE_COEFF54
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovs_coeff0            : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    ovs_coeff1            : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF54;

// Define the union U_DATE_COEFF55
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ovs_coeff0            : 11  ; // [10..0] 
        unsigned int    Reserved_1            : 5   ; // [15..11] 
        unsigned int    ovs_coeff1            : 11  ; // [26..16] 
        unsigned int    Reserved_0            : 5   ; // [31..27] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF55;

// Define the union U_DATE_COEFF56
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    oversam2_round_en     : 1   ; // [0]  
        unsigned int    Reserved_0            : 31  ; // [31..1] 
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF56;

// Define the union U_DATE_COEFF57
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    v_gain                : 8   ; // [7..0]  
        unsigned int    u_gain                : 8   ; // [15..8]
        unsigned int    ycvbs_gain            : 8   ; // [23..16]
        unsigned int    Reserved_0            : 7   ; // [30..24]
        unsigned int    cvbs_gain_en          : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF57;

// Define the union U_DATE_COEFF58
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pr_gain               : 8   ; // [7..0]  
        unsigned int    pb_gain               : 8   ; // [15..8]
        unsigned int    ycomp_gain            : 8   ; // [23..16]
        unsigned int    Reserved_0            : 7   ; // [30..24]
        unsigned int    comp_gain_en          : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF58;

// Define the union U_DATE_COEFF59
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ynotch_clip_fullrange : 1   ; // [0]  
        unsigned int    clpf_clip_fullrange   : 1   ; // [1]
        unsigned int    Reserved_4            : 2   ; // [3..2]
        unsigned int    y_os_clip_fullrange   : 1   ; // [4]
        unsigned int    Reserved_3            : 3   ; // [7..5]
        unsigned int    u_os_clip_fullrange   : 1   ; // [8]
        unsigned int    v_os_clip_fullrange   : 1   ; // [9]
        unsigned int    Reserved_2            : 2   ; // [11..10]
        unsigned int    cb_os_clip_fullrange  : 1   ; // [12]
        unsigned int    cr_os_clip_fullrange  : 1   ; // [13]
        unsigned int    Reserved_1            : 2   ; // [15..14]
        unsigned int    cb_gain_polar         : 1   ; // [16]
        unsigned int    Reserved_0            : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF59;

// Define the union U_DATE_COEFF60
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    line_960h_en          : 1   ; // [0]  
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF60;

// Define the union U_DATE_COEFF61
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    burst_start_ovrd      : 1   ; // [0]
        unsigned int    burst_dura_ovrd       : 1   ; // [1]
        unsigned int    cb_bound_ovrd         : 1   ; // [2]
        unsigned int    pal_half_ovrd         : 1   ; // [3]
        unsigned int    rgb_acrive_ovrd       : 1   ; // [4]
        unsigned int    Reserved_0            : 27  ; // [31..5]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF61;

// Define the union U_DATE_COEFF62
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    burst_dura_coeff      : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF62;

// Define the union U_DATE_COEFF63
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cb_bound_coeff        : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF63;

// Define the union U_DATE_COEFF64
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    pal_half_coeff        : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF64;

// Define the union U_DATE_COEFF65
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    rgb_active_coeff      : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF65;

// Define the union U_DATE_COEFF66
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    htotal_ovrd          : 1   ; // [0]
        unsigned int    wid_sync_ovrd        : 1   ; // [1]
        unsigned int    wid_hfb_ovrd         : 1   ; // [2]
        unsigned int    wid_act_ovrd         : 1   ; // [3]
        unsigned int    Reserved_0           : 28  ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF66;

// Define the union U_DATE_COEFF67
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    htotal_sw            : 12  ; // [11..0]
        unsigned int    Reserved_0           : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF67;

// Define the union U_DATE_COEFF68
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wid_sync_sw          : 12  ; // [11..0]
        unsigned int    Reserved_0           : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF68;

// Define the union U_DATE_COEFF69
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wid_hfb_sw           : 12  ; // [11..0]
        unsigned int    Reserved_0           : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF69;

// Define the union U_DATE_COEFF70
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    wid_act_sw           : 12  ; // [11..0]
        unsigned int    Reserved_0           : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF70;

// Define the union U_DATE_COEFF71
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    phase_shift_ovrd     : 1   ; // [0]
        unsigned int    Reserved_0           : 31  ; // [31..11]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF71;

// Define the union U_DATE_COEFF72
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    phase_shift_coeff    : 32  ; // [31..0]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF72;

// Define the union U_DATE_COEFF73
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_notch_ovrd      : 1   ; // [0]
        unsigned int    Reserved_0           : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF73;

// Define the union U_DATE_COEFF74
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_notch_1         : 10  ; // [9..0]
        unsigned int    Reserved_1           : 6   ; // [15..10]
        unsigned int    coef_notch_2         : 10  ; // [25..16]
        unsigned int    Reserved_0           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF74;

// Define the union U_DATE_COEFF75
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_notch_3         : 10  ; // [9..0]
        unsigned int    Reserved_1           : 6   ; // [15..10]
        unsigned int    coef_notch_4         : 10  ; // [25..16]
        unsigned int    Reserved_0           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF75;

// Define the union U_DATE_COEFF76
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_notch_5         : 10  ; // [9..0]
        unsigned int    Reserved_1           : 6   ; // [15..10]
        unsigned int    coef_notch_6         : 10  ; // [25..16]
        unsigned int    Reserved_0           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF76;

// Define the union U_DATE_COEFF77
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_notch_7         : 10  ; // [9..0]
        unsigned int    Reserved_1           : 6   ; // [15..10]
        unsigned int    coef_notch_8         : 10  ; // [25..16]
        unsigned int    Reserved_0           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF77;

// Define the union U_DATE_COEFF78
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_notch_9         : 10  ; // [9..0]
        unsigned int    Reserved_1           : 6   ; // [15..10]
        unsigned int    coef_notch_10        : 10  ; // [25..16]
        unsigned int    Reserved_0           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF78;

// Define the union U_DATE_COEFF79
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_notch_11        : 10  ; // [9..0]
        unsigned int    Reserved_1           : 6   ; // [15..10]
        unsigned int    coef_notch_12        : 10  ; // [25..16]
        unsigned int    Reserved_0           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF79;

// Define the union U_DATE_COEFF80
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_chra_lpf_ovrd   : 1   ; // [0]
        unsigned int    Reserved_0           : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF80;

// Define the union U_DATE_COEFF81
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_chra_lpf_1      : 9   ; // [8..0]
        unsigned int    Reserved_1           : 7   ; // [15..9]
        unsigned int    coef_chra_lpf_2      : 9   ; // [24..16]
        unsigned int    Reserved_0           : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF81;

// Define the union U_DATE_COEFF82
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_chra_lpf_3      : 9   ; // [8..0]
        unsigned int    Reserved_1           : 7   ; // [15..9]
        unsigned int    coef_chra_lpf_4      : 9   ; // [24..16]
        unsigned int    Reserved_0           : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF82;

// Define the union U_DATE_COEFF83
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_chra_lpf_5      : 9   ; // [8..0]
        unsigned int    Reserved_1           : 7   ; // [15..9]
        unsigned int    coef_chra_lpf_6      : 9   ; // [24..16]
        unsigned int    Reserved_0           : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF83;

// Define the union U_DATE_COEFF84
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    coef_chra_lpf_7      : 9   ; // [8..0]
        unsigned int    Reserved_0           : 23  ; // [31..9]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DATE_COEFF84;


//==============================================================================
// Define the global struct
typedef struct
{
    U_VOCTRL             VOCTRL;
    U_VOINTSTA           VOINTSTA;
    U_VOMSKINTSTA        VOMSKINTSTA;
    U_VOINTMSK           VOINTMSK;
    U_VDPVERSION1        VDPVERSION1;
    U_VDPVERSION2        VDPVERSION2;
    unsigned int         Reserved_0[2];
    unsigned int         Reserved_VODEBUG;
    unsigned int         Reserved_1[4];
    U_VOAXICTRL          VOAXICTRL;
    unsigned int         Reserved_VOWBCARB0;
    unsigned int         Reserved_VOWBCARB1;
    U_VOUFSTA            VOUFSTA;
    U_VOUFCLR            VOUFCLR;
    unsigned int         Reserved_2[2];
    unsigned int         Reserved_VOINTPROC_TIM;
    unsigned int         Reserved_3[43];
    U_VO_MUX             VO_MUX;
    U_VO_MUX_DAC         VO_MUX_DAC;
    U_VO_MUX_TESTSYNC    VO_MUX_TESTSYNC;
    U_VO_MUX_TESTDATA    VO_MUX_TESTDATA;
    unsigned int         Reserved_4[4];
    U_VO_DAC_CTRL        VO_DAC_CTRL;
    unsigned int         Reserved_5[3];
    U_VO_DAC_C_CTRL      VO_DAC_C_CTRL;
    U_VO_DAC_R_CTRL      VO_DAC_R_CTRL;
    U_VO_DAC_G_CTRL      VO_DAC_G_CTRL;
    U_VO_DAC_B_CTRL      VO_DAC_B_CTRL;
    U_VO_DAC_STAT0       VO_DAC_STAT0;
    unsigned int         Reserved_VO_DAC_STAT1;
    unsigned int         Reserved_6[110];
    unsigned int         Reserved_WBC_DHD_LOCATE;
    unsigned int         Reserved_WBC_OFL_EN;
    unsigned int         Reserved_VHD_CORRESP;
    unsigned int         Reserved_GDC_CORRESP;
    unsigned int         Reserved_WBC_CORRESP;
    unsigned int         Reserved_7[59];
    U_COEF_DATA          COEF_DATA;
    unsigned int         Reserved_8[3];
    unsigned int         Reserved_V0_PARARD;
    unsigned int         Reserved_V1_PARARD;
    unsigned int         Reserved_9;
    unsigned int         Reserved_V3_PARARD;
    unsigned int         Reserved_10[8];
    unsigned int         Reserved_VP0_PARARD;
    unsigned int         Reserved_11[19];
    unsigned int         Reserved_GP0_PARARD;
    unsigned int         Reserved_GP1_PARARD;
    unsigned int         Reserved_12[10];
    unsigned int         Reserved_WBCDHD_PARARD;
    unsigned int         Reserved_13[11];
    unsigned int         Reserved_DHD0_PARARD;
    unsigned int         Reserved_DHD1_PARARD;
    unsigned int         Reserved_14[194];
    U_V0_CTRL            V0_CTRL;
    U_V0_UPD             V0_UPD;
    unsigned int         Reserved_15[6];
    unsigned int         Reserved_V0_PRERD;
    unsigned int         Reserved_16;
    U_V0_IRESO           V0_IRESO;
    U_V0_ORESO           V0_ORESO;
    unsigned int         Reserved_17[2];
    U_V0_CBMPARA         V0_CBMPARA;
    unsigned int         Reserved_18;
    unsigned int         Reserved_V0_PARAUP;
    U_V0_CPOS            V0_CPOS;
    unsigned int         Reserved_V0_DRAWMODE;
    unsigned int         Reserved_19;
    unsigned int         Reserved_V0_HLCOEFAD;
    unsigned int         Reserved_V0_HCCOEFAD;
    unsigned int         Reserved_V0_VLCOEFAD;
    unsigned int         Reserved_V0_VCCOEFAD;
    U_V0_DFPOS           V0_DFPOS;
    U_V0_DLPOS           V0_DLPOS;
    U_V0_VFPOS           V0_VFPOS;
    U_V0_VLPOS           V0_VLPOS;
    U_V0_BK              V0_BK;
    U_V0_ALPHA           V0_ALPHA;
    unsigned int         Reserved_20[2];
    U_V0_CSC_IDC         V0_CSC_IDC;
    U_V0_CSC_ODC         V0_CSC_ODC;
    U_V0_CSC_IODC        V0_CSC_IODC;
    U_V0_CSC_P0          V0_CSC_P0;
    U_V0_CSC_P1          V0_CSC_P1;
    U_V0_CSC_P2          V0_CSC_P2;
    U_V0_CSC_P3          V0_CSC_P3;
    U_V0_CSC_P4          V0_CSC_P4;
    unsigned int         Reserved_21[8];
    unsigned int         Reserved_V0_HSP;
    unsigned int         Reserved_V0_HLOFFSET;
    unsigned int         Reserved_V0_HCOFFSET;
    unsigned int         Reserved_22[3];
    unsigned int         Reserved_V0_VSP;
    unsigned int         Reserved_V0_VSR;
    unsigned int         Reserved_V0_VOFFSET;
    unsigned int         Reserved_V0_VBOFFSET;
    unsigned int         Reserved_23[38];
    U_V0_IFIRCOEF01      V0_IFIRCOEF01;
    U_V0_IFIRCOEF23      V0_IFIRCOEF23;
    U_V0_IFIRCOEF45      V0_IFIRCOEF45;
    U_V0_IFIRCOEF67      V0_IFIRCOEF67;
    unsigned int         Reserved_24[28];
    unsigned int         Reserved_V0_P0RESO;
    U_V0_P0LADDR         V0_P0LADDR;
    U_V0_P0CADDR         V0_P0CADDR;
    U_V0_P0STRIDE        V0_P0STRIDE;
    unsigned int         Reserved_V0_P0VFPOS;
    unsigned int         Reserved_V0_P0VLPOS;
    unsigned int         Reserved_25[2];
    unsigned int         Reserved_V0_P1RESO;
    unsigned int         Reserved_V0_P1LADDR;
    unsigned int         Reserved_V0_P1CADDR;
    unsigned int         Reserved_V0_P1STRIDE;
    unsigned int         Reserved_V0_P1VFPOS;
    unsigned int         Reserved_V0_P1VLPOS;
    unsigned int         Reserved_26[2];
    unsigned int         Reserved_V0_P2RESO;
    unsigned int         Reserved_V0_P2LADDR;
    unsigned int         Reserved_V0_P2CADDR;
    unsigned int         Reserved_V0_P2STRIDE;
    unsigned int         Reserved_V0_P2VFPOS;
    unsigned int         Reserved_V0_P2VLPOS;
    unsigned int         Reserved_27[2];
    unsigned int         Reserved_V0_P3RESO;
    unsigned int         Reserved_V0_P3LADDR;
    unsigned int         Reserved_V0_P3CADDR;
    unsigned int         Reserved_V0_P3STRIDE;
    unsigned int         Reserved_V0_P3VFPOS;
    unsigned int         Reserved_V0_P3VLPOS;
    unsigned int         Reserved_28[2];
    unsigned int         Reserved_V0_P4RESO;
    unsigned int         Reserved_V0_P4LADDR;
    unsigned int         Reserved_V0_P4CADDR;
    unsigned int         Reserved_V0_P4STRIDE;
    unsigned int         Reserved_V0_P4VFPOS;
    unsigned int         Reserved_V0_P4VLPOS;
    unsigned int         Reserved_29[2];
    unsigned int         Reserved_V0_P5RESO;
    unsigned int         Reserved_V0_P5LADDR;
    unsigned int         Reserved_V0_P5CADDR;
    unsigned int         Reserved_V0_P5STRIDE;
    unsigned int         Reserved_V0_P5VFPOS;
    unsigned int         Reserved_V0_P5VLPOS;
    unsigned int         Reserved_30[2];
    unsigned int         Reserved_V0_P6RESO;
    unsigned int         Reserved_V0_P6LADDR;
    unsigned int         Reserved_V0_P6CADDR;
    unsigned int         Reserved_V0_P6STRIDE;
    unsigned int         Reserved_V0_P6VFPOS;
    unsigned int         Reserved_V0_P6VLPOS;
    unsigned int         Reserved_31[2];
    unsigned int         Reserved_V0_P7RESO;
    unsigned int         Reserved_V0_P7LADDR;
    unsigned int         Reserved_V0_P7CADDR;
    unsigned int         Reserved_V0_P7STRIDE;
    unsigned int         Reserved_V0_P7VFPOS;
    unsigned int         Reserved_V0_P7VLPOS;
    unsigned int         Reserved_32[2];
    unsigned int         Reserved_V0_P8RESO;
    unsigned int         Reserved_V0_P8LADDR;
    unsigned int         Reserved_V0_P8CADDR;
    unsigned int         Reserved_V0_P8STRIDE;
    unsigned int         Reserved_V0_P8VFPOS;
    unsigned int         Reserved_V0_P8VLPOS;
    unsigned int         Reserved_33[2];
    unsigned int         Reserved_V0_P9RESO;
    unsigned int         Reserved_V0_P9LADDR;
    unsigned int         Reserved_V0_P9CADDR;
    unsigned int         Reserved_V0_P9STRIDE;
    unsigned int         Reserved_V0_P9VFPOS;
    unsigned int         Reserved_V0_P9VLPOS;
    unsigned int         Reserved_34[2];
    unsigned int         Reserved_V0_P10RESO;
    unsigned int         Reserved_V0_P10LADDR;
    unsigned int         Reserved_V0_P10CADDR;
    unsigned int         Reserved_V0_P10STRIDE;
    unsigned int         Reserved_V0_P10VFPOS;
    unsigned int         Reserved_V0_P10VLPOS;
    unsigned int         Reserved_35[2];
    unsigned int         Reserved_V0_P11RESO;
    unsigned int         Reserved_V0_P11LADDR;
    unsigned int         Reserved_V0_P11CADDR;
    unsigned int         Reserved_V0_P11STRIDE;
    unsigned int         Reserved_V0_P11VFPOS;
    unsigned int         Reserved_V0_P11VLPOS;
    unsigned int         Reserved_36[2];
    unsigned int         Reserved_V0_P12RESO;
    unsigned int         Reserved_V0_P12LADDR;
    unsigned int         Reserved_V0_P12CADDR;
    unsigned int         Reserved_V0_P12STRIDE;
    unsigned int         Reserved_V0_P12VFPOS;
    unsigned int         Reserved_V0_P12VLPOS;
    unsigned int         Reserved_37[2];
    unsigned int         Reserved_V0_P13RESO;
    unsigned int         Reserved_V0_P13LADDR;
    unsigned int         Reserved_V0_P13CADDR;
    unsigned int         Reserved_V0_P13STRIDE;
    unsigned int         Reserved_V0_P13VFPOS;
    unsigned int         Reserved_V0_P13VLPOS;
    unsigned int         Reserved_38[2];
    unsigned int         Reserved_V0_P14RESO;
    unsigned int         Reserved_V0_P14LADDR;
    unsigned int         Reserved_V0_P14CADDR;
    unsigned int         Reserved_V0_P14STRIDE;
    unsigned int         Reserved_V0_P14VFPOS;
    unsigned int         Reserved_V0_P14VLPOS;
    unsigned int         Reserved_39[2];
    unsigned int         Reserved_V0_P15RESO;
    unsigned int         Reserved_V0_P15LADDR;
    unsigned int         Reserved_V0_P15CADDR;
    unsigned int         Reserved_V0_P15STRIDE;
    unsigned int         Reserved_V0_P15VFPOS;
    unsigned int         Reserved_V0_P15VLPOS;
    unsigned int         Reserved_40[130];
    unsigned int         Reserved_V0_NADDR;
    unsigned int         Reserved_V0_NCADDR;
    unsigned int         Reserved_41[10];
    unsigned int         Reserved_V0_MULTI_MODE;
    unsigned int         Reserved_42[3];
    U_V0_LADDROFFSET     V0_LADDROFFSET;
    U_V0_CADDROFFSET     V0_CADDROFFSET;
    unsigned int         Reserved_43[46];
    unsigned int         Reserved_V0_16REGIONENL;
    unsigned int         Reserved_44;
    unsigned int         Reserved_V0_16MUTE;
    unsigned int         Reserved_45[5];
    U_V0_DCMP_LSTATE0    V0_DCMP_LSTATE0;
    U_V0_DCMP_LSTATE1    V0_DCMP_LSTATE1;
    U_V0_DCMP_CSTATE0    V0_DCMP_CSTATE0;
    U_V0_DCMP_CSTATE1    V0_DCMP_CSTATE1;
    U_VO_DCMPERRCLR      VO_DCMPERRCLR;
    U_V0_DCMP_ERR        V0_DCMP_ERR;
    unsigned int         Reserved_46[3122];
    unsigned int         Reserved_VP0_CTRL;
    unsigned int         Reserved_VP0_UPD;
    unsigned int         Reserved_47[2];
    unsigned int         Reserved_VP0_ACC_CAD;
    unsigned int         Reserved_VP0_ACM_CAD;
    unsigned int         Reserved_VP0_PARAUP;
    unsigned int         Reserved_48;
    unsigned int         Reserved_VP0_IRESO;
    unsigned int         Reserved_49[31];
    unsigned int         Reserved_VP0_DOF_CTRL;
    unsigned int         Reserved_VP0_DOF_STEP;
    unsigned int         Reserved_50[22];
    unsigned int         Reserved_VP0_ACCTHD1;
    unsigned int         Reserved_VP0_ACCTHD2;
    unsigned int         Reserved_51[2];
    unsigned int         Reserved_VP0_ACCLOWN[3];
    unsigned int         Reserved_52;
    unsigned int         Reserved_VP0_ACCMEDN[3];
    unsigned int         Reserved_53;
    unsigned int         Reserved_VP0_ACCHIGHN[3];
    unsigned int         Reserved_54;
    unsigned int         Reserved_VP0_ACCMLN[3];
    unsigned int         Reserved_55;
    unsigned int         Reserved_VP0_ACCMHN[3];
    unsigned int         Reserved_56;
    unsigned int         Reserved_VP0_ACC3LOW;
    unsigned int         Reserved_VP0_ACC3MED;
    unsigned int         Reserved_VP0_ACC3HIGH;
    unsigned int         Reserved_VP0_ACC8MLOW;
    unsigned int         Reserved_VP0_ACC8MHIGH;
    unsigned int         Reserved_VP0_ACCTOTAL;
    unsigned int         Reserved_57[6];
    unsigned int         Reserved_VP0_ACM_CTRL;
    unsigned int         Reserved_VP0_ACM_ADJ;
    unsigned int         Reserved_58[26];
    unsigned int         Reserved_VP0_DFPOS;
    unsigned int         Reserved_VP0_DLPOS;
    unsigned int         Reserved_VP0_VFPOS;
    unsigned int         Reserved_VP0_VLPOS;
    unsigned int         Reserved_VP0_BK;
    unsigned int         Reserved_VP0_ALPHA;
    unsigned int         Reserved_59[58];
    unsigned int         Reserved_VP0_CSC0_IDC;
    unsigned int         Reserved_VP0_CSC0_ODC;
    unsigned int         Reserved_VP0_CSC0_IODC;
    unsigned int         Reserved_VP0_CSC0_P0;
    unsigned int         Reserved_VP0_CSC0_P1;
    unsigned int         Reserved_VP0_CSC0_P2;
    unsigned int         Reserved_VP0_CSC0_P3;
    unsigned int         Reserved_VP0_CSC0_P4;
    unsigned int         Reserved_VP0_CSC1_IDC;
    unsigned int         Reserved_VP0_CSC1_ODC;
    unsigned int         Reserved_VP0_CSC1_IODC;
    unsigned int         Reserved_VP0_CSC1_P0;
    unsigned int         Reserved_VP0_CSC1_P1;
    unsigned int         Reserved_VP0_CSC1_P2;
    unsigned int         Reserved_VP0_CSC1_P3;
    unsigned int         Reserved_VP0_CSC1_P4;
    unsigned int         Reserved_60[1840];
    U_G0_CTRL            G0_CTRL;
    U_G0_UPD             G0_UPD;
    unsigned int         Reserved_61[2];
    U_G0_ADDR            G0_ADDR;
    unsigned int         Reserved_62;
    unsigned int         Reserved_G0_NADDR;
    U_G0_STRIDE          G0_STRIDE;
    U_G0_IRESO           G0_IRESO;
    U_G0_SFPOS           G0_SFPOS;
    unsigned int         Reserved_63[2];
    U_G0_CBMPARA         G0_CBMPARA;
    U_G0_CKEYMAX         G0_CKEYMAX;
    U_G0_CKEYMIN         G0_CKEYMIN;
    U_G0_CMASK           G0_CMASK;
    U_G0_PARAADDR        G0_PARAADDR;
    U_G0_PARAUP          G0_PARAUP;
    unsigned int         Reserved_64[2];
    unsigned int         Reserved_G0_DCMP_ADDR;
    unsigned int         Reserved_G0_DCMP_NADDR;
    unsigned int         Reserved_G0_DCMP_OFFSET;
    unsigned int         Reserved_65[9];
    U_G0_DFPOS           G0_DFPOS;
    U_G0_DLPOS           G0_DLPOS;
    U_G0_VFPOS           G0_VFPOS;
    U_G0_VLPOS           G0_VLPOS;
    U_G0_BK              G0_BK;
    U_G0_ALPHA           G0_ALPHA;
    unsigned int         Reserved_66[2];
    unsigned int         Reserved_G0_DOF_CTRL;
    unsigned int         Reserved_G0_DOF_STEP;
    unsigned int         Reserved_67[6];
    U_G0_CSC_IDC         G0_CSC_IDC;
    U_G0_CSC_ODC         G0_CSC_ODC;
    U_G0_CSC_IODC        G0_CSC_IODC;
    U_G0_CSC_P0          G0_CSC_P0;
    U_G0_CSC_P1          G0_CSC_P1;
    U_G0_CSC_P2          G0_CSC_P2;
    U_G0_CSC_P3          G0_CSC_P3;
    U_G0_CSC_P4          G0_CSC_P4;
    unsigned int         Reserved_68[3016];
    unsigned int         Reserved_GP0_CTRL;
    unsigned int         Reserved_GP0_UPD;
    unsigned int         Reserved_GP0_ORESO;
    unsigned int         Reserved_GP0_IRESO;
    unsigned int         Reserved_GP0_HCOEFAD;
    unsigned int         Reserved_GP0_VCOEFAD;
    unsigned int         Reserved_GP0_PARAUP;
    unsigned int         Reserved_69;
    unsigned int         Reserved_GP0_GALPHA;
    unsigned int         Reserved_70[55];
    unsigned int         Reserved_GP0_DFPOS;
    unsigned int         Reserved_GP0_DLPOS;
    unsigned int         Reserved_GP0_VFPOS;
    unsigned int         Reserved_GP0_VLPOS;
    unsigned int         Reserved_GP0_BK;
    unsigned int         Reserved_GP0_ALPHA;
    unsigned int         Reserved_71[2];
    unsigned int         Reserved_GP0_CSC_IDC;
    unsigned int         Reserved_GP0_CSC_ODC;
    unsigned int         Reserved_GP0_CSC_IODC;
    unsigned int         Reserved_GP0_CSC_P0;
    unsigned int         Reserved_GP0_CSC_P1;
    unsigned int         Reserved_GP0_CSC_P2;
    unsigned int         Reserved_GP0_CSC_P3;
    unsigned int         Reserved_GP0_CSC_P4;
    unsigned int         Reserved_GP0_ZME_HSP;
    unsigned int         Reserved_GP0_ZME_HOFFSET;
    unsigned int         Reserved_GP0_ZME_VSP;
    unsigned int         Reserved_GP0_ZME_VSR;
    unsigned int         Reserved_GP0_ZME_VOFFSET;
    unsigned int         Reserved_72[3];
    unsigned int         Reserved_GP0_ZME_LTICTRL;
    unsigned int         Reserved_GP0_ZME_LHPASSCOEF;
    unsigned int         Reserved_GP0_ZME_LTITHD;
    unsigned int         Reserved_73;
    unsigned int         Reserved_GP0_ZME_LHFREQTHD;
    unsigned int         Reserved_GP0_ZME_LGAINCOEF;
    unsigned int         Reserved_74[2];
    unsigned int         Reserved_GP0_ZME_CTICTRL;
    unsigned int         Reserved_GP0_ZME_CHPASSCOEF;
    unsigned int         Reserved_GP0_ZME_CTITHD;
    unsigned int         Reserved_75[925];
    unsigned int         Reserved_WBC_G0_CTRL;
    unsigned int         Reserved_WBC_G0_UPD;
    unsigned int         Reserved_WBC_G0_CMP;
    unsigned int         Reserved_76;
    unsigned int         Reserved_WBC_G0_AR_ADDR;
    unsigned int         Reserved_WBC_G0_GB_ADDR;
    unsigned int         Reserved_WBC_G0_STRIDE;
    unsigned int         Reserved_WBC_G0_OFFSET;
    unsigned int         Reserved_WBC_G0_ORESO;
    unsigned int         Reserved_WBC_G0_FCROP;
    unsigned int         Reserved_WBC_G0_LCROP;
    unsigned int         Reserved_0_77[4];
    unsigned int         Reserved_WBC_G0_STPSTACLR;
    unsigned int         Reserved_1_77[496];
    unsigned int         Reserved_WBC_GP0_CTRL;
    unsigned int         Reserved_WBC_GP0_UPD;
    unsigned int         Reserved_78[2];
    unsigned int         Reserved_WBC_GP0_YADDR;
    unsigned int         Reserved_WBC_GP0_CADDR;
    unsigned int         Reserved_WBC_GP0_STRIDE;
    unsigned int         Reserved_79;
    unsigned int         Reserved_WBC_GP0_ORESO;
    unsigned int         Reserved_WBC_GP0_FCROP;
    unsigned int         Reserved_WBC_GP0_LCROP;
    unsigned int         Reserved_80[53];
    unsigned int         Reserved_WBC_GP0_DITHER_CTRL;
    unsigned int         Reserved_WBC_GP0_DITHER_COEF0;
    unsigned int         Reserved_WBC_GP0_DITHER_COEF1;
    unsigned int         Reserved_81[189];
    unsigned int         Reserved_WBC_DHD0_CTRL;
    unsigned int         Reserved_WBC_DHD0_UPD;
    unsigned int         Reserved_82[2];
    unsigned int         Reserved_WBC_DHD0_YADDR;
    unsigned int         Reserved_WBC_DHD0_CADDR;
    unsigned int         Reserved_WBC_DHD0_STRIDE;
    unsigned int         Reserved_83;
    unsigned int         Reserved_WBC_DHD0_ORESO;
    unsigned int         Reserved_WBC_DHD0_FCROP;
    unsigned int         Reserved_WBC_DHD0_LCROP;
    unsigned int         Reserved_84;
    unsigned int         Reserved_WBC_DHD0_LOWDLYCTRL;
    unsigned int         Reserved_WBC_DHD0_TUNLADDR;
    unsigned int         Reserved_WBC_DHD0_LOWDLYSTA;
    unsigned int         Reserved_85;
    unsigned int         Reserved_WBC_DHD0_PARAUP;
    unsigned int         Reserved_86[3];
    unsigned int         Reserved_WBC_DHD0_HLCOEFAD;
    unsigned int         Reserved_WBC_DHD0_HCCOEFAD;
    unsigned int         Reserved_WBC_DHD0_VLCOEFAD;
    unsigned int         Reserved_WBC_DHD0_VCCOEFAD;
    unsigned int         Reserved_87[16];
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM0;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM1;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM2;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM3;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM4;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM5;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM6;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM7;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM8;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM9;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM10;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM11;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM12;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM13;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM14;
    unsigned int         Reserved_WBC_DHD0_HISTOGRAM15;
    unsigned int         Reserved_88[4];
    unsigned int         Reserved_WBC_DHD0_CHECKSUM_Y;
    unsigned int         Reserved_WBC_DHD0_CHECKSUM_C;
    unsigned int         Reserved_89[2];
    unsigned int         Reserved_WBC_DHD0_DITHER_CTRL;
    unsigned int         Reserved_WBC_DHD0_DITHER_COEF0;
    unsigned int         Reserved_WBC_DHD0_DITHER_COEF1;
    unsigned int         Reserved_90[65];
    unsigned int         Reserved_WBC_DHD0_HCDS;
    unsigned int         Reserved_WBC_DHD0_HCDS_COEF0;
    unsigned int         Reserved_WBC_DHD0_HCDS_COEF1;
    unsigned int         Reserved_91[41];
    unsigned int         Reserved_WBC_DHD0_ZME_HSP;
    unsigned int         Reserved_WBC_DHD0_ZME_HLOFFSET;
    unsigned int         Reserved_WBC_DHD0_ZME_HCOFFSET;
    unsigned int         Reserved_92[3];
    unsigned int         Reserved_WBC_DHD0_ZME_VSP;
    unsigned int         Reserved_WBC_DHD0_ZME_VSR;
    unsigned int         Reserved_WBC_DHD0_ZME_VOFFSET;
    unsigned int         Reserved_WBC_DHD0_ZME_VBOFFSET;
    unsigned int         Reserved_93[6];
    unsigned int         Reserved_WBC_DHD0_CSCIDC;
    unsigned int         Reserved_WBC_DHD0_CSCODC;
    unsigned int         Reserved_WBC_DHD0_CSCP0;
    unsigned int         Reserved_WBC_DHD0_CSCP1;
    unsigned int         Reserved_WBC_DHD0_CSCP2;
    unsigned int         Reserved_WBC_DHD0_CSCP3;
    unsigned int         Reserved_WBC_DHD0_CSCP4;
    unsigned int         Reserved_94[57];
    unsigned int         Reserved_MIXV0_BKG;
    unsigned int         Reserved_95;
    unsigned int         Reserved_MIXV0_MIX;
    unsigned int         Reserved_96[125];
    unsigned int         Reserved_MIXG0_BKG;
    unsigned int         Reserved_MIXG0_BKALPHA;
    unsigned int         Reserved_MIXG0_MIX;
    unsigned int         Reserved_97[125];
    unsigned int         Reserved_CBM_BKG1;
    unsigned int         Reserved_98;
    unsigned int         Reserved_CBM_MIX1;
    unsigned int         Reserved_99[5];
    U_CBM_BKG2           CBM_BKG2;
    unsigned int         Reserved_100;
    U_CBM_MIX2           CBM_MIX2;
    unsigned int         Reserved_101[5];
    unsigned int         Reserved_CBM_ATTR;
    unsigned int         Reserved_102[111];
    unsigned int         Reserved_MIXDSD_BKG;
    unsigned int         Reserved_103;
    unsigned int         Reserved_MIXDSD_MIX;
    unsigned int         Reserved_104[637];
    U_DHD0_CTRL          DHD0_CTRL;
    U_DHD0_VSYNC         DHD0_VSYNC;
    U_DHD0_HSYNC1        DHD0_HSYNC1;
    U_DHD0_HSYNC2        DHD0_HSYNC2;
    U_DHD0_VPLUS         DHD0_VPLUS;
    U_DHD0_PWR           DHD0_PWR;
    U_DHD0_VTTHD3        DHD0_VTTHD3;
    U_DHD0_VTTHD         DHD0_VTTHD;
    unsigned int         Reserved_DHD0_SYNC_INV;
    unsigned int         Reserved_105[2];
    unsigned int         Reserved_DHD0_DATA_SEL;
    U_DHD0_AFFTHD        DHD0_AFFTHD;
    unsigned int         Reserved_DHD0_ABUFTHD;
    U_DHD0_VGA_DACDET1   DHD0_VGA_DACDET1;
    U_DHD0_VGA_DACDET2   DHD0_VGA_DACDET2;
    unsigned int         Reserved_DHD0_CSC_IDC;
    unsigned int         Reserved_DHD0_CSC_ODC;
    unsigned int         Reserved_DHD0_CSC_IODC;
    unsigned int         Reserved_DHD0_CSC_P0;
    unsigned int         Reserved_DHD0_CSC_P1;
    unsigned int         Reserved_DHD0_CSC_P2;
    unsigned int         Reserved_DHD0_CSC_P3;
    unsigned int         Reserved_DHD0_CSC_P4;
    unsigned int         Reserved_DHD0_DITHER0_CTRL;
    unsigned int         Reserved_DHD0_DITHER0_COEF0;
    unsigned int         Reserved_DHD0_DITHER0_COEF1;
    unsigned int         Reserved_108;
    unsigned int         Reserved_DHD0_DITHER1_CTRL;
    unsigned int         Reserved_DHD0_DITHER1_COEF0;
    unsigned int         Reserved_DHD0_DITHER1_COEF1;
    unsigned int         Reserved_109;
    unsigned int         Reserved_DHD0_CLIP0_L;
    unsigned int         Reserved_DHD0_CLIP0_H;
    unsigned int         Reserved_DHD0_CLIP1_L;
    unsigned int         Reserved_DHD0_CLIP1_H;
    unsigned int         Reserved_DHD0_CLIP2_L;
    unsigned int         Reserved_DHD0_CLIP2_H;
    unsigned int         Reserved_DHD0_CLIP3_L;
    unsigned int         Reserved_DHD0_CLIP3_H;
    unsigned int         Reserved_DHD0_CLIP4_L;
    unsigned int         Reserved_DHD0_CLIP4_H;
    unsigned int         Reserved_110[2];
    U_DHD0_PARATHD       DHD0_PARATHD;
    unsigned int         Reserved_111[3];
    U_DHD0_START_POS     DHD0_START_POS;
    unsigned int         Reserved_112[3];
    unsigned int         Reserved_DHD0_LOCKCFG;
    unsigned int         Reserved_DHD0_LOCK_STATE1;
    unsigned int         Reserved_DHD0_LOCK_STATE2;
    unsigned int         Reserved_DHD0_LOCK_STATE3;
    unsigned int         Reserved_DHD0_GMM_COEFAD;
    unsigned int         Reserved_113[2];
    unsigned int         Reserved_DHD0_PARAUP;
    U_DHD0_STATE         DHD0_STATE;
    unsigned int         Reserved_114;
    U_DHD0_DEBUG         DHD0_DEBUG;
    U_DHD0_DEBUG_STATE   DHD0_DEBUG_STATE;
    unsigned int         Reserved_DHD0_HSPCFG0;
    unsigned int         Reserved_DHD0_HSPCFG1;
    unsigned int         Reserved_115[3];
    unsigned int         Reserved_DHD0_HSPCFG5;
    unsigned int         Reserved_DHD0_HSPCFG6;
    unsigned int         Reserved_DHD0_HSPCFG7;
    unsigned int         Reserved_DHD0_HSPCFG8;
    unsigned int         Reserved_116[3];
    unsigned int         Reserved_DHD0_HSPCFG12;
    unsigned int         Reserved_DHD0_HSPCFG13;
    unsigned int         Reserved_DHD0_HSPCFG14;
    unsigned int         Reserved_DHD0_HSPCFG15;
    unsigned int         Reserved_117[944];
    U_BT_CTRL            BT_CTRL;
    unsigned int         Reserved_INTF_UPD;
    U_INTF_SYNC_INV      INTF_SYNC_INV;
    unsigned int         Reserved_118;
    U_INTF_CLIP0_L       INTF_CLIP0_L; /* BT_CLIP0_L */
    U_INTF_CLIP0_H       INTF_CLIP0_H; /* BT_CLIP0_H */
    unsigned int         Reserved_119[2];
    unsigned int         Reserved_INTF_CSC_IDC;
    unsigned int         Reserved_INTF_CSC_ODC;
    unsigned int         Reserved_INTF_CSC_IODC;
    unsigned int         Reserved_INTF_CSC_P0;
    unsigned int         Reserved_INTF_CSC_P1;
    unsigned int         Reserved_INTF_CSC_P2;
    unsigned int         Reserved_INTF_CSC_P3;
    unsigned int         Reserved_INTF_CSC_P4;
    unsigned int         Reserved_INTF_HSPCFG0;
    unsigned int         Reserved_INTF_HSPCFG1;
    unsigned int         Reserved_120[3];
    unsigned int         Reserved_INTF_HSPCFG5;
    unsigned int         Reserved_INTF_HSPCFG6;
    unsigned int         Reserved_INTF_HSPCFG7;
    unsigned int         Reserved_INTF_HSPCFG8;
    unsigned int         Reserved_121[3];
    unsigned int         Reserved_INTF_HSPCFG12;
    unsigned int         Reserved_INTF_HSPCFG13;
    unsigned int         Reserved_INTF_HSPCFG14;
    unsigned int         Reserved_INTF_HSPCFG15;
    U_INTF_DITHER0_CTRL  INTF_DITHER0_CTRL;
    U_INTF_DITHER0_COEF0 INTF_DITHER0_COEF0;
    U_INTF_DITHER0_COEF1 INTF_DITHER0_COEF1;
    unsigned int         Reserved_122[25];
    unsigned int         Reserved_INTF_CHKSUM_Y;
    unsigned int         Reserved_INTF_CHKSUM_U;
    unsigned int         Reserved_INTF_CHKSUM_V;
    unsigned int         Reserved_123[1985];
    unsigned int         Reserved_HDATE_VERSION;
    unsigned int         Reserved_HDATE_EN;
    unsigned int         Reserved_HDATE_POLA_CTRL;
    unsigned int         Reserved_HDATE_VIDEO_FORMAT;
    unsigned int         Reserved_HDATE_STATE;
    unsigned int         Reserved_HDATE_OUT_CTRL;
    unsigned int         Reserved_HDATE_SRC_13_COEF1;
    unsigned int         Reserved_HDATE_SRC_13_COEF2;
    unsigned int         Reserved_HDATE_SRC_13_COEF3;
    unsigned int         Reserved_HDATE_SRC_13_COEF4;
    unsigned int         Reserved_HDATE_SRC_13_COEF5;
    unsigned int         Reserved_HDATE_SRC_13_COEF6;
    unsigned int         Reserved_HDATE_SRC_13_COEF7;
    unsigned int         Reserved_HDATE_SRC_13_COEF8;
    unsigned int         Reserved_HDATE_SRC_13_COEF9;
    unsigned int         Reserved_HDATE_SRC_13_COEF10;
    unsigned int         Reserved_HDATE_SRC_13_COEF11;
    unsigned int         Reserved_HDATE_SRC_13_COEF12;
    unsigned int         Reserved_HDATE_SRC_13_COEF13;
    unsigned int         Reserved_HDATE_SRC_24_COEF1;
    unsigned int         Reserved_HDATE_SRC_24_COEF2;
    unsigned int         Reserved_HDATE_SRC_24_COEF3;
    unsigned int         Reserved_HDATE_SRC_24_COEF4;
    unsigned int         Reserved_HDATE_SRC_24_COEF5;
    unsigned int         Reserved_HDATE_SRC_24_COEF6;
    unsigned int         Reserved_HDATE_SRC_24_COEF7;
    unsigned int         Reserved_HDATE_SRC_24_COEF8;
    unsigned int         Reserved_HDATE_SRC_24_COEF9;
    unsigned int         Reserved_HDATE_SRC_24_COEF10;
    unsigned int         Reserved_HDATE_SRC_24_COEF11;
    unsigned int         Reserved_HDATE_SRC_24_COEF12;
    unsigned int         Reserved_HDATE_SRC_24_COEF13;
    unsigned int         Reserved_HDATE_CSC_COEF1;
    unsigned int         Reserved_HDATE_CSC_COEF2;
    unsigned int         Reserved_HDATE_CSC_COEF3;
    unsigned int         Reserved_HDATE_CSC_COEF4;
    unsigned int         Reserved_HDATE_CSC_COEF5;
    unsigned int         Reserved_124[3];
    unsigned int         Reserved_HDATE_TEST;
    unsigned int         Reserved_HDATE_VBI_CTRL;
    unsigned int         Reserved_HDATE_CGMSA_DATA;
    unsigned int         Reserved_HDATE_CGMSB_H;
    unsigned int         Reserved_HDATE_CGMSB_DATA1;
    unsigned int         Reserved_HDATE_CGMSB_DATA2;
    unsigned int         Reserved_HDATE_CGMSB_DATA3;
    unsigned int         Reserved_HDATE_CGMSB_DATA4;
    unsigned int         Reserved_HDATE_DACDET1;
    unsigned int         Reserved_HDATE_DACDET2;
    unsigned int         Reserved_HDATE_SRC_13_COEF14;
    unsigned int         Reserved_HDATE_SRC_13_COEF15;
    unsigned int         Reserved_HDATE_SRC_13_COEF16;
    unsigned int         Reserved_HDATE_SRC_13_COEF17;
    unsigned int         Reserved_HDATE_SRC_13_COEF18;
    unsigned int         Reserved_HDATE_SRC_24_COEF14;
    unsigned int         Reserved_HDATE_SRC_24_COEF15;
    unsigned int         Reserved_HDATE_SRC_24_COEF16;
    unsigned int         Reserved_HDATE_SRC_24_COEF17;
    unsigned int         Reserved_HDATE_SRC_24_COEF18;
    unsigned int         Reserved_HDATE_CLIP;
    unsigned int         Reserved_125[67];
    U_DATE_COEFF0        DATE_COEFF0;
    U_DATE_COEFF1        DATE_COEFF1;
    U_DATE_COEFF2        DATE_COEFF2;
    U_DATE_COEFF3        DATE_COEFF3;
    U_DATE_COEFF4        DATE_COEFF4;
    U_DATE_COEFF5        DATE_COEFF5;
    U_DATE_COEFF6        DATE_COEFF6;
    U_DATE_COEFF7        DATE_COEFF7;
    U_DATE_COEFF8        DATE_COEFF8;
    U_DATE_COEFF9        DATE_COEFF9;
    U_DATE_COEFF10       DATE_COEFF10;
    U_DATE_COEFF11       DATE_COEFF11;
    U_DATE_COEFF12       DATE_COEFF12;
    U_DATE_COEFF13       DATE_COEFF13;
    U_DATE_COEFF14       DATE_COEFF14;
    U_DATE_COEFF15       DATE_COEFF15;
    U_DATE_COEFF16       DATE_COEFF16;
    U_DATE_COEFF17       DATE_COEFF17;
    U_DATE_COEFF18       DATE_COEFF18;
    U_DATE_COEFF19       DATE_COEFF19;
    U_DATE_COEFF20       DATE_COEFF20;
    U_DATE_COEFF21       DATE_COEFF21;
    U_DATE_COEFF22       DATE_COEFF22;
    U_DATE_COEFF23       DATE_COEFF23;
    U_DATE_COEFF24       DATE_COEFF24;
    U_DATE_COEFF25       DATE_COEFF25;
    U_DATE_COEFF26       DATE_COEFF26;
    U_DATE_COEFF27       DATE_COEFF27;
    U_DATE_COEFF28       DATE_COEFF28;
    U_DATE_COEFF29       DATE_COEFF29;
    U_DATE_COEFF30       DATE_COEFF30;
    unsigned int         Reserved_126;
    U_DATE_ISRMASK       DATE_ISRMASK;
    U_DATE_ISRSTATE      DATE_ISRSTATE;
    U_DATE_ISR           DATE_ISR;
    unsigned int         Reserved_127;
    U_DATE_VERSION       DATE_VERSION;
    U_DATE_COEFF37       DATE_COEFF37;
    U_DATE_COEFF38       DATE_COEFF38;
    U_DATE_COEFF39       DATE_COEFF39;
    U_DATE_COEFF40       DATE_COEFF40;
    U_DATE_COEFF41       DATE_COEFF41;
    U_DATE_COEFF42       DATE_COEFF42;
    unsigned int         Reserved_128[5];
    U_DATE_DACDET1       DATE_DACDET1;
    U_DATE_DACDET2       DATE_DACDET2;
    U_DATE_COEFF50       DATE_COEFF50;
    U_DATE_COEFF51       DATE_COEFF51;
    U_DATE_COEFF52       DATE_COEFF52;
    U_DATE_COEFF53       DATE_COEFF53;
    U_DATE_COEFF54       DATE_COEFF54;
    U_DATE_COEFF55       DATE_COEFF55;
    U_DATE_COEFF56       DATE_COEFF56;
    U_DATE_COEFF57       DATE_COEFF57;
    U_DATE_COEFF58       DATE_COEFF58;
    U_DATE_COEFF59       DATE_COEFF59;
    U_DATE_COEFF60       DATE_COEFF60;
    U_DATE_COEFF61       DATE_COEFF61;
    U_DATE_COEFF62       DATE_COEFF62;
    U_DATE_COEFF63       DATE_COEFF63;
    U_DATE_COEFF64       DATE_COEFF64;
    U_DATE_COEFF65       DATE_COEFF65;
    U_DATE_COEFF66       DATE_COEFF66;
    U_DATE_COEFF67       DATE_COEFF67;
    U_DATE_COEFF68       DATE_COEFF68;
    U_DATE_COEFF69       DATE_COEFF69;
    U_DATE_COEFF70       DATE_COEFF71;
    U_DATE_COEFF72       DATE_COEFF72;
    U_DATE_COEFF73       DATE_COEFF73;
    U_DATE_COEFF74       DATE_COEFF74;
    U_DATE_COEFF75       DATE_COEFF75;
    U_DATE_COEFF76       DATE_COEFF76;
    U_DATE_COEFF77       DATE_COEFF77;
    U_DATE_COEFF78       DATE_COEFF78;
    U_DATE_COEFF79       DATE_COEFF79;
    U_DATE_COEFF80       DATE_COEFF80;
    U_DATE_COEFF81       DATE_COEFF81;
    U_DATE_COEFF82       DATE_COEFF82;
    U_DATE_COEFF83       DATE_COEFF83;
    U_DATE_COEFF84       DATE_COEFF84;

} VOU_REGS_S;

// Declare the struct pointor of the module VDP



#endif // __C_UNION_DEFINE_H__

