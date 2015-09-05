/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_reg.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/10/08
  Description   :
  History       :
  1.Date        : 2012/10/08
    Author      : c00182286/d00224772
    Modification: Created file for 3520D
******************************************************************************/

#ifndef __VOU_REG_H__
#define __VOU_REG_H__

#include "hi_type.h"
	

// Define the union U_VHDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 4   ; // [3..0]
        HI_U32    time_out              : 4   ; // [7..4]
        HI_U32    Reserved_2            : 9   ; // [8..16]
        HI_U32    vup_mode              : 1   ; // [17]
        HI_U32    ifir_mode             : 2   ; // [19..18]
        HI_U32    Reserved_1            : 11  ; // [30..20]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCTRL;

// Define the union U_VHDUPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_3            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDUPD;

// Define the union U_VHDLADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_laddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDLADDR;
// Define the union U_VHDLCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_lcaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDLCADDR;
// Define the union U_VHDCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_caddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDCADDR;
// Define the union U_VHDCCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_ccaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDCCADDR;
// Define the union U_VHDNADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_naddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDNADDR;
// Define the union U_VHDNCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_ncaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDNCADDR;
// Define the union U_VHDSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDSTRIDE;

// Define the union U_VHDIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_5            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDIRESO;

// Define the union U_VHDLKEY
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_low              : 8   ; // [7..0]
        HI_U32    luma_high             : 8   ; // [15..8]
        HI_U32    luma_mask             : 8   ; // [23..16]
        HI_U32    Reserved_6            : 7   ; // [30..24]
        HI_U32    luma_key_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDLKEY;

// Define the union U_VHDCBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    Reserved_8            : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCBMPARA;

// Define the union U_VHDDIEADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dieaddr                 : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDDIEADDR;
// Define the union U_VHDWBC1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 wbc1addr                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDWBC1ADDR;
// Define the union U_VHDWBC1STRD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wbc1strd              : 16  ; // [15..0]
        HI_U32    req_interval          : 10  ; // [25..16]
        HI_U32    Reserved_9            : 4   ; // [29..26]
        HI_U32    wbc1_dft              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDWBC1STRD;

// Define the union U_VHDDIESTADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 diestaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDDIESTADDR;
// Define the union U_VHDCTI_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_10           : 8   ; // [7..0]
        HI_U32    cmixing_ratio         : 7   ; // [14..8]
        HI_U32    Reserved_9            : 1   ; // [15]
        HI_U32    cgain_ratio           : 12  ; // [27..16]
        HI_U32    Reserved_8            : 3   ; // [30..28]
        HI_U32    cti_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCTI_CTRL;

// Define the union U_VHDCHPASS_COEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    chpass_coef0          : 8   ; // [7..0]
        HI_U32    chpass_coef1          : 8   ; // [15..8]
        HI_U32    chpass_coef2          : 8   ; // [23..16]
        HI_U32    Reserved_12           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCHPASS_COEF;

// Define the union U_VHDCTI_THRE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cswing_thre           : 16  ; // [15..0]
        HI_U32    ccoring_thre          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCTI_THRE;

// Define the union U_VHDLGAIN_COEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lgain_coef0           : 8   ; // [7..0]
        HI_U32    lgain_coef1           : 8   ; // [15..8]
        HI_U32    lgain_coef2           : 8   ; // [23..16]
        HI_U32    Reserved_13           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDLGAIN_COEF;

// Define the union U_VHDDFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_10           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDFPOS;

// Define the union U_VHDDLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_12           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDLPOS;

// Define the union U_VHDVFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_13           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVFPOS;

// Define the union U_VHDVLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_14           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVLPOS;

// Define the union U_VHDBK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbk_cr                : 8   ; // [7..0]
        HI_U32    vbk_cb                : 8   ; // [15..8]
        HI_U32    vbk_y                 : 8   ; // [23..16]
        HI_U32    vbk_alpha             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDBK;

// Define the union U_VHDCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc2               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc0               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    csc_mode              : 1   ; // [28]
        HI_U32    Reserved_15           : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCSCIDC;

// Define the union U_VHDCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc2               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc0               : 9   ; // [26..18]
        HI_U32    Reserved_17           : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCSCODC;

// Define the union U_VHDCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_19           : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_18           : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCSCP0;

// Define the union U_VHDCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_21           : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_20           : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCSCP1;

// Define the union U_VHDCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_23           : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_22           : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCSCP2;

// Define the union U_VHDCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_25           : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_24           : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCSCP3;

// Define the union U_VHDCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_26           : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCSCP4;

// Define the union U_VHDACM0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    acm0_en               : 1   ; // [28]
        HI_U32    acm1_en               : 1   ; // [29]
        HI_U32    acm2_en               : 1   ; // [30]
        HI_U32    acm3_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM0;

// Define the union U_VHDACM1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    acm_test_en           : 1   ; // [20]
        HI_U32    Reserved_28           : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM1;

// Define the union U_VHDACM2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_29           : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM2;

// Define the union U_VHDACM3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_30           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM3;

// Define the union U_VHDACM4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_31           : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM4;

// Define the union U_VHDACM5
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_32           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM5;

// Define the union U_VHDACM6
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_33           : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM6;

// Define the union U_VHDACM7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_34           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACM7;

// Define the union U_VHDHSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 24  ; // [23..0]
        HI_U32    Reserved_37           : 1   ; // [24]        
        HI_U32    hchfir_en             : 1   ; // [25
        HI_U32    hlfir_en              : 1   ; // [26]
        HI_U32    Reserved_36           : 1   ; // [27]
        HI_U32    hchmid_en             : 1   ; // [28]     /* added */
        HI_U32    hlmid_en              : 1   ; // [29]     /* added */
        HI_U32    hchmsc_en             : 1   ; // [30]
        HI_U32    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDHSP;

// Define the union U_VHDHLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_loffset           : 28  ; // [27..0]
        HI_U32    Reserved_38           : 4  ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDHLOFFSET;

// Define the union U_VHDHCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 28  ; // [27..0]
        HI_U32    Reserved_39           : 4  ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDHCOFFSET;

// Define the union U_VHDVSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_40           : 19  ; // [18..0]
        HI_U32    zme_in_fmt            : 2   ; // [20..19]
        HI_U32    zme_out_fmt           : 2   ; // [22..21]       
        HI_U32    vchfir_en             : 1   ; // [23]
        HI_U32    vlfir_en              : 1   ; // [24]
        HI_U32    Reserved_chroma_type  : 1   ; // [25]
        HI_U32    vsc_chroma_tap        : 1   ; // [26]
        HI_U32    Reserved_vsc_luma_tap : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]       
        HI_U32    vchmsc_en             : 1   ; // [30]
        HI_U32    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVSP;

// Define the union U_VHDVSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_42           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVSR;

// Define the union U_VHDVOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vchroma_offset        : 16  ; // [15..0]
        HI_U32    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVOFFSET;

// Define the union U_VHDZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_43           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDZMEORESO;

// Define the union U_VHDZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_44           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDZMEIRESO;

// Define the union U_VHDZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    need_rand_range       : 8   ; // [7..0]
        HI_U32    rdy_rand_range        : 8   ; // [15..8]
        HI_U32    need_mode             : 1   ; // [16]
        HI_U32    rdy_mode              : 1   ; // [17]
        HI_U32    Reserved_45           : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDZMEDBG;

// Define the union U_VHDCOEF00
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_46           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCOEF00;

// Define the union U_VHDCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_47           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCOEF01;

// Define the union U_VHDCOEF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_48           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCOEF10;

// Define the union U_VHDCOEF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_49           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCOEF11;

// Define the union U_VHDLTI_CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lhpass_coef4          : 8   ; // [7..0]
        HI_U32    lmixing_ratio         : 7   ; // [14..8]
        HI_U32    Reserved_50           : 1   ; // [15]
        HI_U32    lgain_ratio           : 12  ; // [27..16]
        HI_U32    Reserved_49           : 3   ; // [30..28]
        HI_U32    lti_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDLTI_CTRL;

// Define the union U_VHDLHPASS_COEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lhpass_coef0          : 8   ; // [7..0]
        HI_U32    lhpass_coef1          : 8   ; // [15..8]
        HI_U32    lhpass_coef2          : 8   ; // [23..16]
        HI_U32    lhpass_coef3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDLHPASS_COEF;

// Define the union U_VHDLTI_THRE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lswing_thre           : 16  ; // [15..0]
        HI_U32    lcoring_thre          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDLTI_THRE;

// Define the union U_VHDLHFREQ_THRE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lhfreq_thre0          : 16  ; // [15..0]
        HI_U32    lhfreq_thre1          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDLHFREQ_THRE;

// Define the union U_VHDACCTHD1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_low               : 10  ; // [9..0]
        HI_U32    thd_high              : 10  ; // [19..10]
        HI_U32    thd_med_low           : 10  ; // [29..20]
        HI_U32    acc_mode              : 1   ; // [30]
        HI_U32    acc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCTHD1;

// Define the union U_VHDACCTHD2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_med_high          : 10  ; // [9..0]
        HI_U32    acc_multiple          : 8   ; // [17..10]
        HI_U32    acc_rst               : 1   ; // [18]
        HI_U32    acc_reso_mode         : 1   ; // [19]
        HI_U32    Reserved_50           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCTHD2;

// Define the union U_VHDACCLOWN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_51           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCLOWN;

// Define the union U_VHDACCMEDN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_53           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCMEDN;

// Define the union U_VHDACCHIGHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_55           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCHIGHN;

// Define the union U_VHDACCMLN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_57           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCMLN;

// Define the union U_VHDACCMHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_59           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCMHN;

// Define the union U_VHDACC3LOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_low              : 21  ; // [20..0]
        HI_U32    Reserved_61           : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACC3LOW;

// Define the union U_VHDACC3MED
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_med              : 21  ; // [20..0]
        HI_U32    Reserved_63           : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACC3MED;

// Define the union U_VHDACC3HIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_high             : 21  ; // [20..0]
        HI_U32    Reserved_64           : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACC3HIGH;

// Define the union U_VHDACC8MLOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_low          : 21  ; // [20..0]
        HI_U32    Reserved_65           : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACC8MLOW;

// Define the union U_VHDACC8MHIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_high         : 21  ; // [20..0]
        HI_U32    Reserved_66           : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACC8MHIGH;

// Define the union U_VHDACCTOTAL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acc_pix_total         : 21  ; // [20..0]
        HI_U32    Reserved_67           : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDACCTOTAL;

// Define the union U_VHDIFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef0                 : 10  ; // [9..0]
        HI_U32    Reserved_69           : 6   ; // [15..10]
        HI_U32    coef1                 : 10  ; // [25..16]
        HI_U32    Reserved_68           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDIFIRCOEF01;

// Define the union U_VHDIFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef2                 : 10  ; // [9..0]
        HI_U32    Reserved_72           : 6   ; // [15..10]
        HI_U32    coef3                 : 10  ; // [25..16]
        HI_U32    Reserved_71           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDIFIRCOEF23;

// Define the union U_VHDIFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef4                 : 10  ; // [9..0]
        HI_U32    Reserved_74           : 6   ; // [15..10]
        HI_U32    coef5                 : 10  ; // [25..16]
        HI_U32    Reserved_73           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDIFIRCOEF45;

// Define the union U_VHDIFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef6                 : 10  ; // [9..0]
        HI_U32    Reserved_76           : 6   ; // [15..10]
        HI_U32    coef7                 : 10  ; // [25..16]
        HI_U32    Reserved_75           : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDIFIRCOEF67;

// Define the union U_VHDDIECTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_78           : 16  ; // [15..0]
        HI_U32    stinfo_stop           : 1   ; // [16]
        HI_U32    die_rst               : 1   ; // [17]
        HI_U32    Reserved_77           : 6   ; // [23..18]
        HI_U32    die_chmmode           : 2   ; // [25..24]
        HI_U32    die_lmmode            : 2   ; // [27..26]
        HI_U32    die_out_sel_c         : 1   ; // [28]
        HI_U32    die_out_sel_l         : 1   ; // [29]
        HI_U32    die_chroma_en         : 1   ; // [30]
        HI_U32    die_luma_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIECTRL;

// Define the union U_VHDDIELMA0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_qtbl1            : 8   ; // [7..0]
        HI_U32    luma_qtbl2            : 8   ; // [15..8]
        HI_U32    luma_qtbl3            : 8   ; // [23..16]
        HI_U32    scale_ratio_ppd       : 6   ; // [29..24]
        HI_U32    Reserved_80           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIELMA0;

// Define the union U_VHDDIELMA1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_reqtbl0          : 8   ; // [7..0]
        HI_U32    luma_reqtbl1          : 8   ; // [15..8]
        HI_U32    luma_reqtbl2          : 8   ; // [23..16]
        HI_U32    luma_reqtbl3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIELMA1;

// Define the union U_VHDDIELMA2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_win_size         : 2   ; // [1..0]
        HI_U32    luma_scesdf_max       : 1   ; // [2]
        HI_U32    die_st_qrst_en        : 1   ; // [3]
        HI_U32    die_st_n_en           : 1   ; // [4]
        HI_U32    die_st_m_en           : 1   ; // [5]
        HI_U32    die_st_l_en           : 1   ; // [6]
        HI_U32    die_st_k_en           : 1   ; // [7]
        HI_U32    luma_qrst_max         : 1   ; // [8]
        HI_U32    luma_mf_max           : 1   ; // [9]
        HI_U32    chroma_mf_max         : 1   ; // [10]
        HI_U32    Reserved_82           : 18  ; // [28..11]
        HI_U32    die_st_mode           : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIELMA2;

// Define the union U_VHDDIEINTEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir_inten             : 4   ; // [3..0]
        HI_U32    Reserved_84           : 3   ; // [6..4]
        HI_U32    dir_ck_enh            : 1   ; // [7]
        HI_U32    dir_inten_ver         : 4   ; // [11..8]
        HI_U32    Reserved_83           : 4   ; // [15..12]
        HI_U32    ver_min_inten         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIEINTEN;

// Define the union U_VHDDIESCALE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    range_scale           : 8   ; // [7..0]
        HI_U32    Reserved_85           : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIESCALE;

// Define the union U_VHDDIECHECK1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ck_max_range          : 6   ; // [5..0]
        HI_U32    Reserved_88           : 2   ; // [7..6]
        HI_U32    ck_range_gain         : 4   ; // [11..8]
        HI_U32    Reserved_87           : 4   ; // [15..12]
        HI_U32    ck_gain               : 4   ; // [19..16]
        HI_U32    Reserved_86           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIECHECK1;

// Define the union U_VHDDIECHECK2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ck_max_range          : 6   ; // [5..0]
        HI_U32    Reserved_91           : 2   ; // [7..6]
        HI_U32    ck_range_gain         : 4   ; // [11..8]
        HI_U32    Reserved_90           : 4   ; // [15..12]
        HI_U32    ck_gain               : 4   ; // [19..16]
        HI_U32    Reserved_89           : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIECHECK2;

// Define the union U_VHDDIEDIR0_3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir0_mult             : 6   ; // [5..0]
        HI_U32    Reserved_95           : 2   ; // [7..6]
        HI_U32    dir1_mult             : 6   ; // [13..8]
        HI_U32    Reserved_94           : 2   ; // [15..14]
        HI_U32    dir2_mult             : 6   ; // [21..16]
        HI_U32    Reserved_93           : 2   ; // [23..22]
        HI_U32    dir3_mult             : 6   ; // [29..24]
        HI_U32    Reserved_92           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIEDIR0_3;

// Define the union U_VHDDIEDIR4_7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir4_mult             : 6   ; // [5..0]
        HI_U32    Reserved_99           : 2   ; // [7..6]
        HI_U32    dir5_mult             : 6   ; // [13..8]
        HI_U32    Reserved_98           : 2   ; // [15..14]
        HI_U32    dir6_mult             : 6   ; // [21..16]
        HI_U32    Reserved_97           : 2   ; // [23..22]
        HI_U32    dir7_mult             : 6   ; // [29..24]
        HI_U32    Reserved_96           : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIEDIR4_7;

// Define the union U_VHDDIEDIR8_11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir8_mult             : 6   ; // [5..0]
        HI_U32    Reserved_103          : 2   ; // [7..6]
        HI_U32    dir9_mult             : 6   ; // [13..8]
        HI_U32    Reserved_102          : 2   ; // [15..14]
        HI_U32    dir10_mult            : 6   ; // [21..16]
        HI_U32    Reserved_101          : 2   ; // [23..22]
        HI_U32    dir11_mult            : 6   ; // [29..24]
        HI_U32    Reserved_100          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIEDIR8_11;

// Define the union U_VHDDIEDIR12_14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir12_mult            : 6   ; // [5..0]
        HI_U32    Reserved_106          : 2   ; // [7..6]
        HI_U32    dir13_mult            : 6   ; // [13..8]
        HI_U32    Reserved_105          : 2   ; // [15..14]
        HI_U32    dir14_mult            : 6   ; // [21..16]
        HI_U32    Reserved_104          : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIEDIR12_14;

// Define the union U_VHDDIESTA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    die_ref_field         : 1   ; // [0]
        HI_U32    Reserved_107          : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDDIESTA;

// Define the union U_VHDDIESTKADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_k_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDDIESTKADDR;
// Define the union U_VHDDIESTLADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_l_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDDIESTLADDR;
// Define the union U_VHDDIESTMADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_m_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDDIESTMADDR;
// Define the union U_VHDDIESTNADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_n_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDDIESTNADDR;
// Define the union U_VHDDIESTSQTRADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_sqtr_addr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDDIESTSQTRADDR;
// Define the union U_VHDCCRSCLR0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    no_ccr_detect_thd     : 8   ; // [7..0]
        HI_U32    no_ccr_detect_max     : 8   ; // [15..8]
        HI_U32    chroma_ma_offset      : 8   ; // [23..16]
        HI_U32    chroma_ccr_en         : 1   ; // [24]
        HI_U32    Reserved_110          : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCCRSCLR0;

// Define the union U_VHDCCRSCLR1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    max_xchroma           : 8   ; // [7..0]
        HI_U32    no_ccr_detect_blend   : 4   ; // [11..8]
        HI_U32    Reserved_111          : 4   ; // [15..12]
        HI_U32    similar_thd           : 8   ; // [23..16]
        HI_U32    similar_max           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDCCRSCLR1;

// Define the union U_VHDPDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_113          : 20  ; // [19..0]
        HI_U32    edge_smooth_ratio     : 8   ; // [27..20]
        HI_U32    Reserved_112          : 1   ; // [28]
        HI_U32    dir_mch_c             : 1   ; // [29]
        HI_U32    edge_smooth_en        : 1   ; // [30]
        HI_U32    dir_mch_l             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDCTRL;

// Define the union U_VHDPDBLKPOS0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    blk_x                 : 12  ; // [11..0]
        HI_U32    blk_y                 : 12  ; // [23..12]
        HI_U32    Reserved_115          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDBLKPOS0;

// Define the union U_VHDPDBLKPOS1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    blk_x                 : 12  ; // [11..0]
        HI_U32    blk_y                 : 12  ; // [23..12]
        HI_U32    Reserved_116          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDBLKPOS1;

// Define the union U_VHDPDBLKTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    stillblk_thd          : 8   ; // [7..0]
        HI_U32    diff_movblk_thd       : 8   ; // [15..8]
        HI_U32    Reserved_117          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDBLKTHD;

// Define the union U_VHDPDHISTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hist_thd0             : 8   ; // [7..0]
        HI_U32    hist_thd1             : 8   ; // [15..8]
        HI_U32    hist_thd2             : 8   ; // [23..16]
        HI_U32    hist_thd3             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDHISTTHD;

// Define the union U_VHDPDUMTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    um_thd0               : 8   ; // [7..0]
        HI_U32    um_thd1               : 8   ; // [15..8]
        HI_U32    um_thd2               : 8   ; // [23..16]
        HI_U32    Reserved_118          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDUMTHD;

// Define the union U_VHDPDPCCCORING
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coring_tkr            : 8   ; // [7..0]
        HI_U32    coring_norm           : 8   ; // [15..8]
        HI_U32    coring_blk            : 8   ; // [23..16]
        HI_U32    Reserved_119          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDPCCCORING;

// Define the union U_VHDPDPCCHTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    pcc_hthd              : 8   ; // [7..0]
        HI_U32    Reserved_120          : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDPCCHTHD;

// Define the union U_VHDPDPCCVTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    pcc_vthd0             : 8   ; // [7..0]
        HI_U32    pcc_vthd1             : 8   ; // [15..8]
        HI_U32    pcc_vthd2             : 8   ; // [23..16]
        HI_U32    pcc_vthd3             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDPCCVTHD;

// Define the union U_VHDPDITDIFFVTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    itdiff_vthd0          : 8   ; // [7..0]
        HI_U32    itdiff_vthd1          : 8   ; // [15..8]
        HI_U32    itdiff_vthd2          : 8   ; // [23..16]
        HI_U32    itdiff_vthd3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDITDIFFVTHD;

// Define the union U_VHDPDLASITHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lasi_thd              : 8   ; // [7..0]
        HI_U32    edge_thd              : 8   ; // [15..8]
        HI_U32    Reserved_121          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDPDLASITHD;

// Define the union U_VHDPDFRMITDIFF
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 frm_it_diff             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDFRMITDIFF;
// Define the union U_VHDPDSTLBLKSAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 stlblk_sad              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDSTLBLKSAD;
// Define the union U_VHDPDHISTBIN
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 hsit_bin                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDHISTBIN;
// Define the union U_VHDPDUMMATCH0
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 match_um                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDUMMATCH0;
// Define the union U_VHDPDUMNOMATCH0
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 nomatch_um              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDUMNOMATCH0;
// Define the union U_VHDPDUMMATCH1
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 match_um                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDUMMATCH1;
// Define the union U_VHDPDUMNOMATCH1
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 nomatch_um              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDUMNOMATCH1;
// Define the union U_VHDPDPCCFFWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_ffwd                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCFFWD;
// Define the union U_VHDPDPCCFWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_fwd                 : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCFWD;
// Define the union U_VHDPDPCCBWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_bwd                 : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCBWD;
// Define the union U_VHDPDPCCCRSS
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_crss                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCCRSS;
// Define the union U_VHDPDPCCPW
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_pw                  : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCPW;
// Define the union U_VHDPDPCCFWDTKR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_fwd_tkr             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCFWDTKR;
// Define the union U_VHDPDPCCBWDTKR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_bwd_tkr             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCBWDTKR;
// Define the union U_VHDPDPCCBLKFWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_blk_fwd             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCBLKFWD;
// Define the union U_VHDPDPCCBLKBWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_blk_bwd             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDPCCBLKBWD;
// Define the union U_VHDPDLASICNT14
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 lasi_cnt                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDLASICNT14;
// Define the union U_VHDPDLASICNT32
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 lasi_cnt                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDLASICNT32;
// Define the union U_VHDPDLASICNT34
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 lasi_cnt                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDPDLASICNT34;
// Define the union U_VHDP0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_126          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP0RESO;

// Define the union U_VHDP0LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP0LADDR;
// Define the union U_VHDP0CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP0CADDR;
// Define the union U_VHDP0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP0STRIDE;

// Define the union U_VHDP0VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_127          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP0VFPOS;

// Define the union U_VHDP0VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_128          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP0VLPOS;

// Define the union U_VHDP1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_129          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP1RESO;

// Define the union U_VHDP1LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP1LADDR;
// Define the union U_VHDP1CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP1CADDR;
// Define the union U_VHDP1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP1STRIDE;

// Define the union U_VHDP1VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_131          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP1VFPOS;

// Define the union U_VHDP1VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_132          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP1VLPOS;

// Define the union U_VHDP2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_133          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP2RESO;

// Define the union U_VHDP2LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP2LADDR;
// Define the union U_VHDP2CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP2CADDR;
// Define the union U_VHDP2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP2STRIDE;

// Define the union U_VHDP2VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_135          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP2VFPOS;

// Define the union U_VHDP2VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_136          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP2VLPOS;

// Define the union U_VHDP3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_137          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP3RESO;

// Define the union U_VHDP3LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP3LADDR;
// Define the union U_VHDP3CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP3CADDR;
// Define the union U_VHDP3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP3STRIDE;

// Define the union U_VHDP3VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_139          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP3VFPOS;

// Define the union U_VHDP3VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_140          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP3VLPOS;

// Define the union U_VHDP4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_141          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP4RESO;

// Define the union U_VHDP4LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP4LADDR;
// Define the union U_VHDP4CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP4CADDR;
// Define the union U_VHDP4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP4STRIDE;

// Define the union U_VHDP4VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_143          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP4VFPOS;

// Define the union U_VHDP4VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_144          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP4VLPOS;

// Define the union U_VHDP5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_145          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP5RESO;

// Define the union U_VHDP5LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP5LADDR;
// Define the union U_VHDP5CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP5CADDR;
// Define the union U_VHDP5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP5STRIDE;

// Define the union U_VHDP5VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_147          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP5VFPOS;

// Define the union U_VHDP5VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_148          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP5VLPOS;

// Define the union U_VHDP6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_149          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP6RESO;

// Define the union U_VHDP6LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP6LADDR;
// Define the union U_VHDP6CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP6CADDR;
// Define the union U_VHDP6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP6STRIDE;

// Define the union U_VHDP6VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_151          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP6VFPOS;

// Define the union U_VHDP6VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_152          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP6VLPOS;

// Define the union U_VHDP7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_153          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP7RESO;

// Define the union U_VHDP7LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP7LADDR;
// Define the union U_VHDP7CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP7CADDR;
// Define the union U_VHDP7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP7STRIDE;

// Define the union U_VHDP7VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_155          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP7VFPOS;

// Define the union U_VHDP7VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_156          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP7VLPOS;

// Define the union U_VHDP8RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_157          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP8RESO;

// Define the union U_VHDP8LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP8LADDR;
// Define the union U_VHDP8CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP8CADDR;
// Define the union U_VHDP8STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP8STRIDE;

// Define the union U_VHDP8VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_159          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP8VFPOS;

// Define the union U_VHDP8VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_160          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP8VLPOS;

// Define the union U_VHDP9RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_161          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP9RESO;

// Define the union U_VHDP9LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP9LADDR;
// Define the union U_VHDP9CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP9CADDR;
// Define the union U_VHDP9STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP9STRIDE;

// Define the union U_VHDP9VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_163          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP9VFPOS;

// Define the union U_VHDP9VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_164          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP9VLPOS;

// Define the union U_VHDP10RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_165          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP10RESO;

// Define the union U_VHDP10LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP10LADDR;
// Define the union U_VHDP10CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP10CADDR;
// Define the union U_VHDP10STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP10STRIDE;

// Define the union U_VHDP10VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_167          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP10VFPOS;

// Define the union U_VHDP10VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_168          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP10VLPOS;

// Define the union U_VHDP11RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_169          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP11RESO;

// Define the union U_VHDP11LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP11LADDR;
// Define the union U_VHDP11CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP11CADDR;
// Define the union U_VHDP11STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP11STRIDE;

// Define the union U_VHDP11VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_171          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP11VFPOS;

// Define the union U_VHDP11VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_172          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP11VLPOS;

// Define the union U_VHDP12RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_173          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP12RESO;

// Define the union U_VHDP12LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP12LADDR;
// Define the union U_VHDP12CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP12CADDR;
// Define the union U_VHDP12STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP12STRIDE;

// Define the union U_VHDP12VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_175          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP12VFPOS;

// Define the union U_VHDP12VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_176          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP12VLPOS;

// Define the union U_VHDP13RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_177          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP13RESO;

// Define the union U_VHDP13LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP13LADDR;
// Define the union U_VHDP13CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP13CADDR;
// Define the union U_VHDP13STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP13STRIDE;

// Define the union U_VHDP13VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_179          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP13VFPOS;

// Define the union U_VHDP13VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_180          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP13VLPOS;

// Define the union U_VHDP14RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_181          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP14RESO;

// Define the union U_VHDP14LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP14LADDR;
// Define the union U_VHDP14CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP14CADDR;
// Define the union U_VHDP14STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP14STRIDE;

// Define the union U_VHDP14VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_183          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP14VFPOS;

// Define the union U_VHDP14VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_184          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP14VLPOS;

// Define the union U_VHDP15RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_185          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP15RESO;

// Define the union U_VHDP15LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP15LADDR;
// Define the union U_VHDP15CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP15CADDR;
// Define the union U_VHDP15STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP15STRIDE;

// Define the union U_VHDP15VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_187          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP15VFPOS;

// Define the union U_VHDP15VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_188          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP15VLPOS;

// Define the union U_VHDP16RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_189          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP16RESO;

// Define the union U_VHDP16LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP16LADDR;
// Define the union U_VHDP16CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP16CADDR;
// Define the union U_VHDP16STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP16STRIDE;

// Define the union U_VHDP16VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_191          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP16VFPOS;

// Define the union U_VHDP16VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_192          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP16VLPOS;

// Define the union U_VHDP17RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_193          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP17RESO;

// Define the union U_VHDP17LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP17LADDR;
// Define the union U_VHDP17CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP17CADDR;
// Define the union U_VHDP17STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP17STRIDE;

// Define the union U_VHDP17VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_195          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP17VFPOS;

// Define the union U_VHDP17VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_196          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP17VLPOS;

// Define the union U_VHDP18RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_197          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP18RESO;

// Define the union U_VHDP18LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP18LADDR;
// Define the union U_VHDP18CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP18CADDR;
// Define the union U_VHDP18STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP18STRIDE;

// Define the union U_VHDP18VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_199          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP18VFPOS;

// Define the union U_VHDP18VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_200          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP18VLPOS;

// Define the union U_VHDP19RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_201          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP19RESO;

// Define the union U_VHDP19LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP19LADDR;
// Define the union U_VHDP19CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP19CADDR;
// Define the union U_VHDP19STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP19STRIDE;

// Define the union U_VHDP19VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_203          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP19VFPOS;

// Define the union U_VHDP19VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_204          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP19VLPOS;

// Define the union U_VHDP20RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_205          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP20RESO;

// Define the union U_VHDP20LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP20LADDR;
// Define the union U_VHDP20CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP20CADDR;
// Define the union U_VHDP20STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP20STRIDE;

// Define the union U_VHDP20VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_207          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP20VFPOS;

// Define the union U_VHDP20VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_208          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP20VLPOS;

// Define the union U_VHDP21RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_209          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP21RESO;

// Define the union U_VHDP21LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP21LADDR;
// Define the union U_VHDP21CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP21CADDR;
// Define the union U_VHDP21STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP21STRIDE;

// Define the union U_VHDP21VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_211          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP21VFPOS;

// Define the union U_VHDP21VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_212          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP21VLPOS;

// Define the union U_VHDP22RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_213          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP22RESO;

// Define the union U_VHDP22LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP22LADDR;
// Define the union U_VHDP22CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP22CADDR;
// Define the union U_VHDP22STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP22STRIDE;

// Define the union U_VHDP22VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_215          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP22VFPOS;

// Define the union U_VHDP22VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_216          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP22VLPOS;

// Define the union U_VHDP23RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_217          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP23RESO;

// Define the union U_VHDP23LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP23LADDR;
// Define the union U_VHDP23CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP23CADDR;
// Define the union U_VHDP23STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP23STRIDE;

// Define the union U_VHDP23VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_219          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP23VFPOS;

// Define the union U_VHDP23VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_220          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP23VLPOS;

// Define the union U_VHDP24RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_221          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP24RESO;

// Define the union U_VHDP24LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP24LADDR;
// Define the union U_VHDP24CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP24CADDR;
// Define the union U_VHDP24STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP24STRIDE;

// Define the union U_VHDP24VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_223          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP24VFPOS;

// Define the union U_VHDP24VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_224          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP24VLPOS;

// Define the union U_VHDP25RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_225          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP25RESO;

// Define the union U_VHDP25LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP25LADDR;
// Define the union U_VHDP25CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP25CADDR;
// Define the union U_VHDP25STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP25STRIDE;

// Define the union U_VHDP25VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_227          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP25VFPOS;

// Define the union U_VHDP25VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_228          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP25VLPOS;

// Define the union U_VHDP26RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_229          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP26RESO;

// Define the union U_VHDP26LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP26LADDR;
// Define the union U_VHDP26CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP26CADDR;
// Define the union U_VHDP26STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP26STRIDE;

// Define the union U_VHDP26VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_231          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP26VFPOS;

// Define the union U_VHDP26VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_232          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP26VLPOS;

// Define the union U_VHDP27RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_233          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP27RESO;

// Define the union U_VHDP27LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP27LADDR;
// Define the union U_VHDP27CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP27CADDR;
// Define the union U_VHDP27STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP27STRIDE;

// Define the union U_VHDP27VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_235          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP27VFPOS;

// Define the union U_VHDP27VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_236          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP27VLPOS;

// Define the union U_VHDP28RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_237          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP28RESO;

// Define the union U_VHDP28LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP28LADDR;
// Define the union U_VHDP28CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP28CADDR;
// Define the union U_VHDP28STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP28STRIDE;

// Define the union U_VHDP28VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_239          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP28VFPOS;

// Define the union U_VHDP28VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_240          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP28VLPOS;

// Define the union U_VHDP29RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_241          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP29RESO;

// Define the union U_VHDP29LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP29LADDR;
// Define the union U_VHDP29CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP29CADDR;
// Define the union U_VHDP29STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP29STRIDE;

// Define the union U_VHDP29VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_243          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP29VFPOS;

// Define the union U_VHDP29VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_244          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP29VLPOS;

// Define the union U_VHDP30RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_245          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP30RESO;

// Define the union U_VHDP30LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP30LADDR;
// Define the union U_VHDP30CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP30CADDR;
// Define the union U_VHDP30STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP30STRIDE;

// Define the union U_VHDP30VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_247          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP30VFPOS;

// Define the union U_VHDP30VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_248          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP30VLPOS;

// Define the union U_VHDP31RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_249          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP31RESO;

// Define the union U_VHDP31LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP31LADDR;
// Define the union U_VHDP31CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP31CADDR;
// Define the union U_VHDP31STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP31STRIDE;

// Define the union U_VHDP31VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_251          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP31VFPOS;

// Define the union U_VHDP31VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_252          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP31VLPOS;

// Define the union U_VHDP32RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_253          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP32RESO;

// Define the union U_VHDP32LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP32LADDR;
// Define the union U_VHDP32CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP32CADDR;
// Define the union U_VHDP32STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP32STRIDE;

// Define the union U_VHDP32VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_255          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP32VFPOS;

// Define the union U_VHDP32VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_256          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP32VLPOS;

// Define the union U_VHDP33RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_257          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP33RESO;

// Define the union U_VHDP33LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP33LADDR;
// Define the union U_VHDP33CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP33CADDR;
// Define the union U_VHDP33STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP33STRIDE;

// Define the union U_VHDP33VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_259          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP33VFPOS;

// Define the union U_VHDP33VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_260          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP33VLPOS;

// Define the union U_VHDP34RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_261          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP34RESO;

// Define the union U_VHDP34LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP34LADDR;
// Define the union U_VHDP34CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP34CADDR;
// Define the union U_VHDP34STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP34STRIDE;

// Define the union U_VHDP34VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_263          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP34VFPOS;

// Define the union U_VHDP34VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_264          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP34VLPOS;

// Define the union U_VHDP35RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_265          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP35RESO;

// Define the union U_VHDP35LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP35LADDR;
// Define the union U_VHDP35CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP35CADDR;
// Define the union U_VHDP35STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP35STRIDE;

// Define the union U_VHDP35VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_267          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP35VFPOS;

// Define the union U_VHDP35VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_268          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP35VLPOS;

// Define the union U_VHDP36RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_269          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP36RESO;

// Define the union U_VHDP36LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP36LADDR;
// Define the union U_VHDP36CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP36CADDR;
// Define the union U_VHDP36STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP36STRIDE;

// Define the union U_VHDP36VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_271          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP36VFPOS;

// Define the union U_VHDP36VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_272          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP36VLPOS;

// Define the union U_VHDP37RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_273          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP37RESO;

// Define the union U_VHDP37LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP37LADDR;
// Define the union U_VHDP37CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP37CADDR;
// Define the union U_VHDP37STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP37STRIDE;

// Define the union U_VHDP37VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_275          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP37VFPOS;

// Define the union U_VHDP37VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_276          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP37VLPOS;

// Define the union U_VHDP38RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_277          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP38RESO;

// Define the union U_VHDP38LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP38LADDR;
// Define the union U_VHDP38CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP38CADDR;
// Define the union U_VHDP38STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP38STRIDE;

// Define the union U_VHDP38VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_279          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP38VFPOS;

// Define the union U_VHDP38VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_280          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP38VLPOS;

// Define the union U_VHDP39RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_281          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP39RESO;

// Define the union U_VHDP39LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP39LADDR;
// Define the union U_VHDP39CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP39CADDR;
// Define the union U_VHDP39STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP39STRIDE;

// Define the union U_VHDP39VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_283          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP39VFPOS;

// Define the union U_VHDP39VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_284          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP39VLPOS;

// Define the union U_VHDP40RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_285          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP40RESO;

// Define the union U_VHDP40LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP40LADDR;
// Define the union U_VHDP40CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP40CADDR;
// Define the union U_VHDP40STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP40STRIDE;

// Define the union U_VHDP40VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_287          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP40VFPOS;

// Define the union U_VHDP40VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_288          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP40VLPOS;

// Define the union U_VHDP41RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_289          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP41RESO;

// Define the union U_VHDP41LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP41LADDR;
// Define the union U_VHDP41CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP41CADDR;
// Define the union U_VHDP41STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP41STRIDE;

// Define the union U_VHDP41VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_291          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP41VFPOS;

// Define the union U_VHDP41VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_292          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP41VLPOS;

// Define the union U_VHDP42RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_293          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP42RESO;

// Define the union U_VHDP42LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP42LADDR;
// Define the union U_VHDP42CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP42CADDR;
// Define the union U_VHDP42STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP42STRIDE;

// Define the union U_VHDP42VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_295          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP42VFPOS;

// Define the union U_VHDP42VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_296          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP42VLPOS;

// Define the union U_VHDP43RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_297          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP43RESO;

// Define the union U_VHDP43LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP43LADDR;
// Define the union U_VHDP43CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP43CADDR;
// Define the union U_VHDP43STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP43STRIDE;

// Define the union U_VHDP43VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_299          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP43VFPOS;

// Define the union U_VHDP43VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_300          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP43VLPOS;

// Define the union U_VHDP44RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_301          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP44RESO;

// Define the union U_VHDP44LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP44LADDR;
// Define the union U_VHDP44CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP44CADDR;
// Define the union U_VHDP44STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP44STRIDE;

// Define the union U_VHDP44VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_303          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP44VFPOS;

// Define the union U_VHDP44VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_304          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP44VLPOS;

// Define the union U_VHDP45RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_305          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP45RESO;

// Define the union U_VHDP45LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP45LADDR;
// Define the union U_VHDP45CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP45CADDR;
// Define the union U_VHDP45STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP45STRIDE;

// Define the union U_VHDP45VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_307          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP45VFPOS;

// Define the union U_VHDP45VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_308          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP45VLPOS;

// Define the union U_VHDP46RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_309          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP46RESO;

// Define the union U_VHDP46LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP46LADDR;
// Define the union U_VHDP46CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP46CADDR;
// Define the union U_VHDP46STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP46STRIDE;

// Define the union U_VHDP46VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_311          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP46VFPOS;

// Define the union U_VHDP46VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_312          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP46VLPOS;

// Define the union U_VHDP47RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_313          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP47RESO;

// Define the union U_VHDP47LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP47LADDR;
// Define the union U_VHDP47CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP47CADDR;
// Define the union U_VHDP47STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP47STRIDE;

// Define the union U_VHDP47VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_315          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP47VFPOS;

// Define the union U_VHDP47VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_316          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP47VLPOS;

// Define the union U_VHDP48RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_317          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP48RESO;

// Define the union U_VHDP48LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP48LADDR;
// Define the union U_VHDP48CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP48CADDR;
// Define the union U_VHDP48STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP48STRIDE;

// Define the union U_VHDP48VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_319          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP48VFPOS;

// Define the union U_VHDP48VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_320          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP48VLPOS;

// Define the union U_VHDP49RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_321          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP49RESO;

// Define the union U_VHDP49LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP49LADDR;
// Define the union U_VHDP49CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP49CADDR;
// Define the union U_VHDP49STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP49STRIDE;

// Define the union U_VHDP49VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_323          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP49VFPOS;

// Define the union U_VHDP49VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_324          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP49VLPOS;

// Define the union U_VHDP50RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_325          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP50RESO;

// Define the union U_VHDP50LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP50LADDR;
// Define the union U_VHDP50CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP50CADDR;
// Define the union U_VHDP50STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP50STRIDE;

// Define the union U_VHDP50VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_327          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP50VFPOS;

// Define the union U_VHDP50VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_328          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP50VLPOS;

// Define the union U_VHDP51RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_329          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP51RESO;

// Define the union U_VHDP51LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP51LADDR;
// Define the union U_VHDP51CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP51CADDR;
// Define the union U_VHDP51STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP51STRIDE;

// Define the union U_VHDP51VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_331          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP51VFPOS;

// Define the union U_VHDP51VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_332          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP51VLPOS;

// Define the union U_VHDP52RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_333          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP52RESO;

// Define the union U_VHDP52LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP52LADDR;
// Define the union U_VHDP52CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP52CADDR;
// Define the union U_VHDP52STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP52STRIDE;

// Define the union U_VHDP52VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_335          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP52VFPOS;

// Define the union U_VHDP52VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_336          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP52VLPOS;

// Define the union U_VHDP53RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_337          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP53RESO;

// Define the union U_VHDP53LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP53LADDR;
// Define the union U_VHDP53CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP53CADDR;
// Define the union U_VHDP53STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP53STRIDE;

// Define the union U_VHDP53VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_339          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP53VFPOS;

// Define the union U_VHDP53VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_340          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP53VLPOS;

// Define the union U_VHDP54RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_341          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP54RESO;

// Define the union U_VHDP54LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP54LADDR;
// Define the union U_VHDP54CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP54CADDR;
// Define the union U_VHDP54STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP54STRIDE;

// Define the union U_VHDP54VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_343          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP54VFPOS;

// Define the union U_VHDP54VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_344          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP54VLPOS;

// Define the union U_VHDP55RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_345          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP55RESO;

// Define the union U_VHDP55LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP55LADDR;
// Define the union U_VHDP55CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP55CADDR;
// Define the union U_VHDP55STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP55STRIDE;

// Define the union U_VHDP55VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_347          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP55VFPOS;

// Define the union U_VHDP55VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_348          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP55VLPOS;

// Define the union U_VHDP56RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_349          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP56RESO;

// Define the union U_VHDP56LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP56LADDR;
// Define the union U_VHDP56CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP56CADDR;
// Define the union U_VHDP56STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP56STRIDE;

// Define the union U_VHDP56VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_351          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP56VFPOS;

// Define the union U_VHDP56VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_352          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP56VLPOS;

// Define the union U_VHDP57RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_353          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP57RESO;

// Define the union U_VHDP57LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP57LADDR;
// Define the union U_VHDP57CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP57CADDR;
// Define the union U_VHDP57STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP57STRIDE;

// Define the union U_VHDP57VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_355          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP57VFPOS;

// Define the union U_VHDP57VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_356          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP57VLPOS;

// Define the union U_VHDP58RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_357          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP58RESO;

// Define the union U_VHDP58LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP58LADDR;
// Define the union U_VHDP58CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP58CADDR;
// Define the union U_VHDP58STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP58STRIDE;

// Define the union U_VHDP58VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_359          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP58VFPOS;

// Define the union U_VHDP58VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_360          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP58VLPOS;

// Define the union U_VHDP59RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_361          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP59RESO;

// Define the union U_VHDP59LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP59LADDR;
// Define the union U_VHDP59CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP59CADDR;
// Define the union U_VHDP59STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP59STRIDE;

// Define the union U_VHDP59VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_363          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP59VFPOS;

// Define the union U_VHDP59VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_364          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP59VLPOS;

// Define the union U_VHDP60RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_365          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP60RESO;

// Define the union U_VHDP60LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP60LADDR;
// Define the union U_VHDP60CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP60CADDR;
// Define the union U_VHDP60STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP60STRIDE;

// Define the union U_VHDP60VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_367          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP60VFPOS;

// Define the union U_VHDP60VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_368          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP60VLPOS;

// Define the union U_VHDP61RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_369          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP61RESO;

// Define the union U_VHDP61LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP61LADDR;
// Define the union U_VHDP61CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP61CADDR;
// Define the union U_VHDP61STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP61STRIDE;

// Define the union U_VHDP61VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_371          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP61VFPOS;

// Define the union U_VHDP61VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_372          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP61VLPOS;

// Define the union U_VHDP62RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_373          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP62RESO;

// Define the union U_VHDP62LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP62LADDR;
// Define the union U_VHDP62CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP62CADDR;
// Define the union U_VHDP62STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP62STRIDE;

// Define the union U_VHDP62VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_375          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP62VFPOS;

// Define the union U_VHDP62VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_376          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP62VLPOS;

// Define the union U_VHDP63RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_377          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP63RESO;

// Define the union U_VHDP63LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP63LADDR;
// Define the union U_VHDP63CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDP63CADDR;
// Define the union U_VHDP63STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP63STRIDE;

// Define the union U_VHDP63VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_379          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP63VFPOS;

// Define the union U_VHDP63VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_380          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDP63VLPOS;

// Define the union U_VHD64REGIONENL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    p1_en                 : 1   ; // [1]
        HI_U32    p2_en                 : 1   ; // [2]
        HI_U32    p3_en                 : 1   ; // [3]
        HI_U32    p4_en                 : 1   ; // [4]
        HI_U32    p5_en                 : 1   ; // [5]
        HI_U32    p6_en                 : 1   ; // [6]
        HI_U32    p7_en                 : 1   ; // [7]
        HI_U32    p8_en                 : 1   ; // [8]
        HI_U32    p9_en                 : 1   ; // [9]
        HI_U32    p10_en                : 1   ; // [10]
        HI_U32    p11_en                : 1   ; // [11]
        HI_U32    p12_en                : 1   ; // [12]
        HI_U32    p13_en                : 1   ; // [13]
        HI_U32    p14_en                : 1   ; // [14]
        HI_U32    p15_en                : 1   ; // [15]
        HI_U32    Reserved              : 16   ; // [31..16]
       
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD16REGIONENL;

// Define the union U_VHD2CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 4   ; // [3..0]
        HI_U32    time_out              : 4   ; // [7..4]
        HI_U32    Reserved_767          : 4   ; // [11..8]
        HI_U32    chm_rmode             : 2   ; // [13..12]
        HI_U32    lm_rmode              : 2   ; // [15..14]
        HI_U32    bfield_first          : 1   ; // [16]
        HI_U32    vup_mode              : 1   ; // [17]
        HI_U32    ifir_mode             : 2   ; // [19..18]
        HI_U32    Reserved_766          : 3   ; // [22..20]
        HI_U32    vhd_sta_wr_en         : 1   ; // [23]
        HI_U32    ofl_btm               : 1   ; // [24]
        HI_U32    ofl_inter             : 1   ; // [25]
        HI_U32    wbc1_int_en           : 1   ; // [26]
        HI_U32    wbc1_en               : 1   ; // [27]
        HI_U32    mute_en               : 1   ; // [28]
        HI_U32    Reserved_765          : 1   ; // [29]
        HI_U32    resource_sel          : 1   ; // [30]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CTRL;

// Define the union U_VHD2UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_769          : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2UPD;

// Define the union U_VHD2LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_laddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2LADDR;
// Define the union U_VHD2LCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_lcaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2LCADDR;
// Define the union U_VHD2CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_caddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2CADDR;
// Define the union U_VHD2CCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_ccaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2CCADDR;
// Define the union U_VHD2NADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_naddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2NADDR;
// Define the union U_VHD2NCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_ncaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2NCADDR;
// Define the union U_VHD2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2STRIDE;

// Define the union U_VHD2IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_771          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2IRESO;

// Define the union U_VHD2LKEY
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_low              : 8   ; // [7..0]
        HI_U32    luma_high             : 8   ; // [15..8]
        HI_U32    luma_mask             : 8   ; // [23..16]
        HI_U32    Reserved_772          : 7   ; // [30..24]
        HI_U32    luma_key_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2LKEY;

// Define the union U_VHD2CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    Reserved_774          : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CBMPARA;

// Define the union U_VHD2DIEADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dieaddr                 : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2DIEADDR;
// Define the union U_VHD2WBC1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 wbc1addr                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2WBC1ADDR;
// Define the union U_VHD2WBC1STRD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wbc1strd              : 16  ; // [15..0]
        HI_U32    req_interval          : 10  ; // [25..16]
        HI_U32    Reserved_775          : 4   ; // [29..26]
        HI_U32    wbc1_dft              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2WBC1STRD;

// Define the union U_VHD2DIESTADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 diestaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2DIESTADDR;
// Define the union U_VHD2DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_776          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DFPOS;

// Define the union U_VHD2DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_778          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DLPOS;

// Define the union U_VHD2VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_779          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2VFPOS;

// Define the union U_VHD2VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_780          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2VLPOS;

// Define the union U_VHD2BK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbk_cr                : 8   ; // [7..0]
        HI_U32    vbk_cb                : 8   ; // [15..8]
        HI_U32    vbk_y                 : 8   ; // [23..16]
        HI_U32    vbk_alpha             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2BK;

// Define the union U_VHD2CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_781          : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CSCIDC;

// Define the union U_VHD2CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_783          : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CSCODC;

// Define the union U_VHD2CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_785          : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_784          : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CSCP0;

// Define the union U_VHD2CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_787          : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_786          : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CSCP1;

// Define the union U_VHD2CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_789          : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_788          : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CSCP2;

// Define the union U_VHD2CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_791          : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_790          : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CSCP3;

// Define the union U_VHD2CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_792          : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CSCP4;

// Define the union U_VHD2ACM0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    acm0_en               : 1   ; // [28]
        HI_U32    acm1_en               : 1   ; // [29]
        HI_U32    acm2_en               : 1   ; // [30]
        HI_U32    acm3_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM0;

// Define the union U_VHD2ACM1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    acm_test_en           : 1   ; // [20]
        HI_U32    Reserved_794          : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM1;

// Define the union U_VHD2ACM2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_795          : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM2;

// Define the union U_VHD2ACM3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_796          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM3;

// Define the union U_VHD2ACM4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_797          : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM4;

// Define the union U_VHD2ACM5
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_798          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM5;

// Define the union U_VHD2ACM6
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_799          : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM6;

// Define the union U_VHD2ACM7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_800          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACM7;

// Define the union U_VHD2HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_803          : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_802          : 3   ; // [22..20]
        HI_U32    hchfir_en             : 1   ; // [23]
        HI_U32    hlfir_en              : 1   ; // [24]
        HI_U32    Reserved_801          : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hchmsc_en             : 1   ; // [30]
        HI_U32    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2HSP;

// Define the union U_VHD2HLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_loffset           : 17  ; // [16..0]
        HI_U32    Reserved_804          : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2HLOFFSET;

// Define the union U_VHD2HCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 17  ; // [16..0]
        HI_U32    Reserved_805          : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2HCOFFSET;

// Define the union U_VHD2VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_806          : 23  ; // [22..0]
        HI_U32    vchfir_en             : 1   ; // [23]
        HI_U32    vlfir_en              : 1   ; // [24]
        HI_U32    chroma_type           : 1   ; // [25]
        HI_U32    vsc_chroma_tap        : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vchmsc_en             : 1   ; // [30]
        HI_U32    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2VSP;

// Define the union U_VHD2VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_808          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2VSR;

// Define the union U_VHD2VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vchroma_offset        : 16  ; // [15..0]
        HI_U32    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2VOFFSET;

// Define the union U_VHD2ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_809          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ZMEORESO;

// Define the union U_VHD2ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_810          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ZMEIRESO;

// Define the union U_VHD2ZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    need_rand_range       : 8   ; // [7..0]
        HI_U32    rdy_rand_range        : 8   ; // [15..8]
        HI_U32    need_mode             : 1   ; // [16]
        HI_U32    rdy_mode              : 1   ; // [17]
        HI_U32    Reserved_811          : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ZMEDBG;

// Define the union U_VHD2COEF00
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_812          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2COEF00;

// Define the union U_VHD2COEF01
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_813          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2COEF01;

// Define the union U_VHD2COEF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_814          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2COEF10;

// Define the union U_VHD2COEF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef00                : 10  ; // [9..0]
        HI_U32    coef01                : 10  ; // [19..10]
        HI_U32    Reserved_815          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2COEF11;

// Define the union U_VHD2ACCTHD1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_low               : 10  ; // [9..0]
        HI_U32    thd_high              : 10  ; // [19..10]
        HI_U32    thd_med_low           : 10  ; // [29..20]
        HI_U32    acc_mode              : 1   ; // [30]
        HI_U32    acc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCTHD1;

// Define the union U_VHD2ACCTHD2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_med_high          : 10  ; // [9..0]
        HI_U32    acc_multiple          : 8   ; // [17..10]
        HI_U32    acc_rst               : 1   ; // [18]
        HI_U32    Reserved_816          : 13  ; // [31..19]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCTHD2;

// Define the union U_VHD2ACCLOWN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_817          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCLOWN;

// Define the union U_VHD2ACCMEDN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_819          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCMEDN;

// Define the union U_VHD2ACCHIGHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_821          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCHIGHN;

// Define the union U_VHD2ACCMLN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_823          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCMLN;

// Define the union U_VHD2ACCMHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_825          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCMHN;

// Define the union U_VHD2ACC3LOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_low              : 21  ; // [20..0]
        HI_U32    Reserved_827          : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACC3LOW;

// Define the union U_VHD2ACC3MED
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_med              : 21  ; // [20..0]
        HI_U32    Reserved_829          : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACC3MED;

// Define the union U_VHD2ACC3HIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_high             : 21  ; // [20..0]
        HI_U32    Reserved_830          : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACC3HIGH;

// Define the union U_VHD2ACC8MLOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_low          : 21  ; // [20..0]
        HI_U32    Reserved_831          : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACC8MLOW;

// Define the union U_VHD2ACC8MHIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_high         : 21  ; // [20..0]
        HI_U32    Reserved_832          : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACC8MHIGH;

// Define the union U_VHD2ACCTOTAL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acc_pix_total         : 21  ; // [20..0]
        HI_U32    Reserved_833          : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2ACCTOTAL;

// Define the union U_VHD2IFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef0                 : 10  ; // [9..0]
        HI_U32    Reserved_835          : 6   ; // [15..10]
        HI_U32    coef1                 : 10  ; // [25..16]
        HI_U32    Reserved_834          : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2IFIRCOEF01;

// Define the union U_VHD2IFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef2                 : 10  ; // [9..0]
        HI_U32    Reserved_838          : 6   ; // [15..10]
        HI_U32    coef3                 : 10  ; // [25..16]
        HI_U32    Reserved_837          : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2IFIRCOEF23;

// Define the union U_VHD2IFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef4                 : 10  ; // [9..0]
        HI_U32    Reserved_840          : 6   ; // [15..10]
        HI_U32    coef5                 : 10  ; // [25..16]
        HI_U32    Reserved_839          : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2IFIRCOEF45;

// Define the union U_VHD2IFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef6                 : 10  ; // [9..0]
        HI_U32    Reserved_842          : 6   ; // [15..10]
        HI_U32    coef7                 : 10  ; // [25..16]
        HI_U32    Reserved_841          : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2IFIRCOEF67;

// Define the union U_VHD2DIECTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_844          : 16  ; // [15..0]
        HI_U32    stinfo_stop           : 1   ; // [16]
        HI_U32    die_rst               : 1   ; // [17]
        HI_U32    Reserved_843          : 6   ; // [23..18]
        HI_U32    die_chmmode           : 2   ; // [25..24]
        HI_U32    die_lmmode            : 2   ; // [27..26]
        HI_U32    die_out_sel_c         : 1   ; // [28]
        HI_U32    die_out_sel_l         : 1   ; // [29]
        HI_U32    die_chroma_en         : 1   ; // [30]
        HI_U32    die_luma_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIECTRL;

// Define the union U_VHD2DIELMA0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_qtbl1            : 8   ; // [7..0]
        HI_U32    luma_qtbl2            : 8   ; // [15..8]
        HI_U32    luma_qtbl3            : 8   ; // [23..16]
        HI_U32    scale_ratio_ppd       : 6   ; // [29..24]
        HI_U32    Reserved_846          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIELMA0;

// Define the union U_VHD2DIELMA1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_reqtbl0          : 8   ; // [7..0]
        HI_U32    luma_reqtbl1          : 8   ; // [15..8]
        HI_U32    luma_reqtbl2          : 8   ; // [23..16]
        HI_U32    luma_reqtbl3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIELMA1;

// Define the union U_VHD2DIELMA2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_win_size         : 2   ; // [1..0]
        HI_U32    luma_scesdf_max       : 1   ; // [2]
        HI_U32    die_st_qrst_en        : 1   ; // [3]
        HI_U32    die_st_n_en           : 1   ; // [4]
        HI_U32    die_st_m_en           : 1   ; // [5]
        HI_U32    die_st_l_en           : 1   ; // [6]
        HI_U32    die_st_k_en           : 1   ; // [7]
        HI_U32    luma_qrst_max         : 1   ; // [8]
        HI_U32    luma_mf_max           : 1   ; // [9]
        HI_U32    chroma_mf_max         : 1   ; // [10]
        HI_U32    Reserved_848          : 18  ; // [28..11]
        HI_U32    die_st_mode           : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIELMA2;

// Define the union U_VHD2DIEINTEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir_inten             : 4   ; // [3..0]
        HI_U32    Reserved_850          : 3   ; // [6..4]
        HI_U32    dir_ck_enh            : 1   ; // [7]
        HI_U32    dir_inten_ver         : 4   ; // [11..8]
        HI_U32    Reserved_849          : 4   ; // [15..12]
        HI_U32    ver_min_inten         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIEINTEN;

// Define the union U_VHD2DIESCALE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    range_scale           : 8   ; // [7..0]
        HI_U32    Reserved_851          : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIESCALE;

// Define the union U_VHD2DIECHECK1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ck_max_range          : 6   ; // [5..0]
        HI_U32    Reserved_854          : 2   ; // [7..6]
        HI_U32    ck_range_gain         : 4   ; // [11..8]
        HI_U32    Reserved_853          : 4   ; // [15..12]
        HI_U32    ck_gain               : 4   ; // [19..16]
        HI_U32    Reserved_852          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIECHECK1;

// Define the union U_VHD2DIECHECK2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ck_max_range          : 6   ; // [5..0]
        HI_U32    Reserved_857          : 2   ; // [7..6]
        HI_U32    ck_range_gain         : 4   ; // [11..8]
        HI_U32    Reserved_856          : 4   ; // [15..12]
        HI_U32    ck_gain               : 4   ; // [19..16]
        HI_U32    Reserved_855          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIECHECK2;

// Define the union U_VHD2DIEDIR0_3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir0_mult             : 6   ; // [5..0]
        HI_U32    Reserved_861          : 2   ; // [7..6]
        HI_U32    dir1_mult             : 6   ; // [13..8]
        HI_U32    Reserved_860          : 2   ; // [15..14]
        HI_U32    dir2_mult             : 6   ; // [21..16]
        HI_U32    Reserved_859          : 2   ; // [23..22]
        HI_U32    dir3_mult             : 6   ; // [29..24]
        HI_U32    Reserved_858          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIEDIR0_3;

// Define the union U_VHD2DIEDIR4_7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir4_mult             : 6   ; // [5..0]
        HI_U32    Reserved_865          : 2   ; // [7..6]
        HI_U32    dir5_mult             : 6   ; // [13..8]
        HI_U32    Reserved_864          : 2   ; // [15..14]
        HI_U32    dir6_mult             : 6   ; // [21..16]
        HI_U32    Reserved_863          : 2   ; // [23..22]
        HI_U32    dir7_mult             : 6   ; // [29..24]
        HI_U32    Reserved_862          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIEDIR4_7;

// Define the union U_VHD2DIEDIR8_11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir8_mult             : 6   ; // [5..0]
        HI_U32    Reserved_869          : 2   ; // [7..6]
        HI_U32    dir9_mult             : 6   ; // [13..8]
        HI_U32    Reserved_868          : 2   ; // [15..14]
        HI_U32    dir10_mult            : 6   ; // [21..16]
        HI_U32    Reserved_867          : 2   ; // [23..22]
        HI_U32    dir11_mult            : 6   ; // [29..24]
        HI_U32    Reserved_866          : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIEDIR8_11;

// Define the union U_VHD2DIEDIR12_14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dir12_mult            : 6   ; // [5..0]
        HI_U32    Reserved_872          : 2   ; // [7..6]
        HI_U32    dir13_mult            : 6   ; // [13..8]
        HI_U32    Reserved_871          : 2   ; // [15..14]
        HI_U32    dir14_mult            : 6   ; // [21..16]
        HI_U32    Reserved_870          : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIEDIR12_14;

// Define the union U_VHD2DIESTA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    die_ref_field         : 1   ; // [0]
        HI_U32    Reserved_873          : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2DIESTA;

// Define the union U_VHD2DIESTKADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_k_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2DIESTKADDR;
// Define the union U_VHD2DIESTLADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_l_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2DIESTLADDR;
// Define the union U_VHD2DIESTMADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_m_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2DIESTMADDR;
// Define the union U_VHD2DIESTNADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_n_addr              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2DIESTNADDR;
// Define the union U_VHD2DIESTSQTRADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 die_sqtr_addr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2DIESTSQTRADDR;
// Define the union U_VHD2CCRSCLR0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    no_ccr_detect_thd     : 8   ; // [7..0]
        HI_U32    no_ccr_detect_max     : 8   ; // [15..8]
        HI_U32    chroma_ma_offset      : 8   ; // [23..16]
        HI_U32    chroma_ccr_en         : 1   ; // [24]
        HI_U32    Reserved_876          : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CCRSCLR0;

// Define the union U_VHD2CCRSCLR1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    max_xchroma           : 8   ; // [7..0]
        HI_U32    no_ccr_detect_blend   : 4   ; // [11..8]
        HI_U32    Reserved_877          : 4   ; // [15..12]
        HI_U32    similar_thd           : 8   ; // [23..16]
        HI_U32    similar_max           : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2CCRSCLR1;

// Define the union U_VHD2PDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_879          : 20  ; // [19..0]
        HI_U32    edge_smooth_ratio     : 8   ; // [27..20]
        HI_U32    Reserved_878          : 1   ; // [28]
        HI_U32    dir_mch_c             : 1   ; // [29]
        HI_U32    edge_smooth_en        : 1   ; // [30]
        HI_U32    dir_mch_l             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDCTRL;

// Define the union U_VHD2PDBLKPOS0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    blk_x                 : 12  ; // [11..0]
        HI_U32    blk_y                 : 12  ; // [23..12]
        HI_U32    Reserved_881          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDBLKPOS0;

// Define the union U_VHD2PDBLKPOS1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    blk_x                 : 12  ; // [11..0]
        HI_U32    blk_y                 : 12  ; // [23..12]
        HI_U32    Reserved_882          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDBLKPOS1;

// Define the union U_VHD2PDBLKTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    stillblk_thd          : 8   ; // [7..0]
        HI_U32    diff_movblk_thd       : 8   ; // [15..8]
        HI_U32    Reserved_883          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDBLKTHD;

// Define the union U_VHD2PDHISTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hist_thd0             : 8   ; // [7..0]
        HI_U32    hist_thd1             : 8   ; // [15..8]
        HI_U32    hist_thd2             : 8   ; // [23..16]
        HI_U32    hist_thd3             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDHISTTHD;

// Define the union U_VHD2PDUMTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    um_thd0               : 8   ; // [7..0]
        HI_U32    um_thd1               : 8   ; // [15..8]
        HI_U32    um_thd2               : 8   ; // [23..16]
        HI_U32    Reserved_884          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDUMTHD;

// Define the union U_VHD2PDPCCCORING
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coring_tkr            : 8   ; // [7..0]
        HI_U32    coring_norm           : 8   ; // [15..8]
        HI_U32    coring_blk            : 8   ; // [23..16]
        HI_U32    Reserved_885          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDPCCCORING;

// Define the union U_VHD2PDPCCHTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    pcc_hthd              : 8   ; // [7..0]
        HI_U32    Reserved_886          : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDPCCHTHD;

// Define the union U_VHD2PDPCCVTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    pcc_vthd0             : 8   ; // [7..0]
        HI_U32    pcc_vthd1             : 8   ; // [15..8]
        HI_U32    pcc_vthd2             : 8   ; // [23..16]
        HI_U32    pcc_vthd3             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDPCCVTHD;

// Define the union U_VHD2PDITDIFFVTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    itdiff_vthd0          : 8   ; // [7..0]
        HI_U32    itdiff_vthd1          : 8   ; // [15..8]
        HI_U32    itdiff_vthd2          : 8   ; // [23..16]
        HI_U32    itdiff_vthd3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDITDIFFVTHD;

// Define the union U_VHD2PDLASITHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lasi_thd              : 8   ; // [7..0]
        HI_U32    edge_thd              : 8   ; // [15..8]
        HI_U32    Reserved_887          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2PDLASITHD;

// Define the union U_VHD2PDFRMITDIFF
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 frm_it_diff             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDFRMITDIFF;
// Define the union U_VHD2PDSTLBLKSAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 stlblk_sad              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDSTLBLKSAD;
// Define the union U_VHD2PDHISTBIN
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 hsit_bin                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDHISTBIN;
// Define the union U_VHD2PDUMMATCH0
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 match_um                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDUMMATCH0;
// Define the union U_VHD2PDUMNOMATCH0
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 nomatch_um              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDUMNOMATCH0;
// Define the union U_VHD2PDUMMATCH1
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 match_um                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDUMMATCH1;
// Define the union U_VHD2PDUMNOMATCH1
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 nomatch_um              : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDUMNOMATCH1;
// Define the union U_VHD2PDPCCFFWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_ffwd                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCFFWD;
// Define the union U_VHD2PDPCCFWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_fwd                 : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCFWD;
// Define the union U_VHD2PDPCCBWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_bwd                 : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCBWD;
// Define the union U_VHD2PDPCCCRSS
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_crss                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCCRSS;
// Define the union U_VHD2PDPCCPW
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_pw                  : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCPW;
// Define the union U_VHD2PDPCCFWDTKR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_fwd_tkr             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCFWDTKR;
// Define the union U_VHD2PDPCCBWDTKR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_bwd_tkr             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCBWDTKR;
// Define the union U_VHD2PDPCCBLKFWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_blk_fwd             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCBLKFWD;
// Define the union U_VHD2PDPCCBLKBWD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 pcc_blk_bwd             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDPCCBLKBWD;
// Define the union U_VHD2PDLASICNT14
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 lasi_cnt                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDLASICNT14;
// Define the union U_VHD2PDLASICNT32
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 lasi_cnt                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDLASICNT32;
// Define the union U_VHD2PDLASICNT34
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 lasi_cnt                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2PDLASICNT34;
// Define the union U_VHD2P0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_892          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P0RESO;

// Define the union U_VHD2P0LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P0LADDR;
// Define the union U_VHD2P0CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P0CADDR;
// Define the union U_VHD2P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P0STRIDE;

// Define the union U_VHD2P0VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_893          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P0VFPOS;

// Define the union U_VHD2P0VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_894          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P0VLPOS;

// Define the union U_VHD2P1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_895          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P1RESO;

// Define the union U_VHD2P1LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P1LADDR;
// Define the union U_VHD2P1CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P1CADDR;
// Define the union U_VHD2P1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P1STRIDE;

// Define the union U_VHD2P1VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_897          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P1VFPOS;

// Define the union U_VHD2P1VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_898          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P1VLPOS;

// Define the union U_VHD2P2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_899          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P2RESO;

// Define the union U_VHD2P2LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P2LADDR;
// Define the union U_VHD2P2CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P2CADDR;
// Define the union U_VHD2P2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P2STRIDE;

// Define the union U_VHD2P2VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_901          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P2VFPOS;

// Define the union U_VHD2P2VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_902          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P2VLPOS;

// Define the union U_VHD2P3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_903          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P3RESO;

// Define the union U_VHD2P3LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P3LADDR;
// Define the union U_VHD2P3CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P3CADDR;
// Define the union U_VHD2P3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P3STRIDE;

// Define the union U_VHD2P3VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_905          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P3VFPOS;

// Define the union U_VHD2P3VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_906          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P3VLPOS;

// Define the union U_VHD2P4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_907          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P4RESO;

// Define the union U_VHD2P4LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P4LADDR;
// Define the union U_VHD2P4CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P4CADDR;
// Define the union U_VHD2P4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P4STRIDE;

// Define the union U_VHD2P4VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_909          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P4VFPOS;

// Define the union U_VHD2P4VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_910          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P4VLPOS;

// Define the union U_VHD2P5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_911          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P5RESO;

// Define the union U_VHD2P5LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P5LADDR;
// Define the union U_VHD2P5CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P5CADDR;
// Define the union U_VHD2P5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P5STRIDE;

// Define the union U_VHD2P5VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_913          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P5VFPOS;

// Define the union U_VHD2P5VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_914          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P5VLPOS;

// Define the union U_VHD2P6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_915          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P6RESO;

// Define the union U_VHD2P6LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P6LADDR;
// Define the union U_VHD2P6CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P6CADDR;
// Define the union U_VHD2P6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P6STRIDE;

// Define the union U_VHD2P6VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_917          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P6VFPOS;

// Define the union U_VHD2P6VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_918          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P6VLPOS;

// Define the union U_VHD2P7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_919          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P7RESO;

// Define the union U_VHD2P7LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P7LADDR;
// Define the union U_VHD2P7CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P7CADDR;
// Define the union U_VHD2P7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P7STRIDE;

// Define the union U_VHD2P7VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_921          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P7VFPOS;

// Define the union U_VHD2P7VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_922          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P7VLPOS;

// Define the union U_VHD2P8RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_923          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P8RESO;

// Define the union U_VHD2P8LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P8LADDR;
// Define the union U_VHD2P8CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P8CADDR;
// Define the union U_VHD2P8STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P8STRIDE;

// Define the union U_VHD2P8VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_925          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P8VFPOS;

// Define the union U_VHD2P8VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_926          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P8VLPOS;

// Define the union U_VHD2P9RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_927          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P9RESO;

// Define the union U_VHD2P9LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P9LADDR;
// Define the union U_VHD2P9CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P9CADDR;
// Define the union U_VHD2P9STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P9STRIDE;

// Define the union U_VHD2P9VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_929          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P9VFPOS;

// Define the union U_VHD2P9VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_930          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P9VLPOS;

// Define the union U_VHD2P10RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_931          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P10RESO;

// Define the union U_VHD2P10LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P10LADDR;
// Define the union U_VHD2P10CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P10CADDR;
// Define the union U_VHD2P10STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P10STRIDE;

// Define the union U_VHD2P10VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_933          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P10VFPOS;

// Define the union U_VHD2P10VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_934          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P10VLPOS;

// Define the union U_VHD2P11RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_935          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P11RESO;

// Define the union U_VHD2P11LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P11LADDR;
// Define the union U_VHD2P11CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P11CADDR;
// Define the union U_VHD2P11STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P11STRIDE;

// Define the union U_VHD2P11VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_937          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P11VFPOS;

// Define the union U_VHD2P11VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_938          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P11VLPOS;

// Define the union U_VHD2P12RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_939          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P12RESO;

// Define the union U_VHD2P12LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P12LADDR;
// Define the union U_VHD2P12CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P12CADDR;
// Define the union U_VHD2P12STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P12STRIDE;

// Define the union U_VHD2P12VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_941          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P12VFPOS;

// Define the union U_VHD2P12VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_942          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P12VLPOS;

// Define the union U_VHD2P13RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_943          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P13RESO;

// Define the union U_VHD2P13LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P13LADDR;
// Define the union U_VHD2P13CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P13CADDR;
// Define the union U_VHD2P13STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P13STRIDE;

// Define the union U_VHD2P13VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_945          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P13VFPOS;

// Define the union U_VHD2P13VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_946          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P13VLPOS;

// Define the union U_VHD2P14RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_947          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P14RESO;

// Define the union U_VHD2P14LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P14LADDR;
// Define the union U_VHD2P14CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P14CADDR;
// Define the union U_VHD2P14STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P14STRIDE;

// Define the union U_VHD2P14VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_949          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P14VFPOS;

// Define the union U_VHD2P14VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_950          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P14VLPOS;

// Define the union U_VHD2P15RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_951          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P15RESO;

// Define the union U_VHD2P15LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P15LADDR;
// Define the union U_VHD2P15CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P15CADDR;
// Define the union U_VHD2P15STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P15STRIDE;

// Define the union U_VHD2P15VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_953          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P15VFPOS;

// Define the union U_VHD2P15VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_954          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P15VLPOS;

// Define the union U_VHD2P16RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_955          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P16RESO;

// Define the union U_VHD2P16LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P16LADDR;
// Define the union U_VHD2P16CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P16CADDR;
// Define the union U_VHD2P16STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P16STRIDE;

// Define the union U_VHD2P16VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_957          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P16VFPOS;

// Define the union U_VHD2P16VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_958          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P16VLPOS;

// Define the union U_VHD2P17RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_959          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P17RESO;

// Define the union U_VHD2P17LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P17LADDR;
// Define the union U_VHD2P17CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P17CADDR;
// Define the union U_VHD2P17STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P17STRIDE;

// Define the union U_VHD2P17VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_961          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P17VFPOS;

// Define the union U_VHD2P17VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_962          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P17VLPOS;

// Define the union U_VHD2P18RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_963          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P18RESO;

// Define the union U_VHD2P18LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P18LADDR;
// Define the union U_VHD2P18CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P18CADDR;
// Define the union U_VHD2P18STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P18STRIDE;

// Define the union U_VHD2P18VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_965          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P18VFPOS;

// Define the union U_VHD2P18VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_966          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P18VLPOS;

// Define the union U_VHD2P19RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_967          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P19RESO;

// Define the union U_VHD2P19LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P19LADDR;
// Define the union U_VHD2P19CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P19CADDR;
// Define the union U_VHD2P19STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P19STRIDE;

// Define the union U_VHD2P19VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_969          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P19VFPOS;

// Define the union U_VHD2P19VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_970          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P19VLPOS;

// Define the union U_VHD2P20RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_971          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P20RESO;

// Define the union U_VHD2P20LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P20LADDR;
// Define the union U_VHD2P20CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P20CADDR;
// Define the union U_VHD2P20STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P20STRIDE;

// Define the union U_VHD2P20VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_973          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P20VFPOS;

// Define the union U_VHD2P20VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_974          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P20VLPOS;

// Define the union U_VHD2P21RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_975          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P21RESO;

// Define the union U_VHD2P21LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P21LADDR;
// Define the union U_VHD2P21CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P21CADDR;
// Define the union U_VHD2P21STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P21STRIDE;

// Define the union U_VHD2P21VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_977          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P21VFPOS;

// Define the union U_VHD2P21VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_978          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P21VLPOS;

// Define the union U_VHD2P22RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_979          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P22RESO;

// Define the union U_VHD2P22LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P22LADDR;
// Define the union U_VHD2P22CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P22CADDR;
// Define the union U_VHD2P22STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P22STRIDE;

// Define the union U_VHD2P22VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_981          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P22VFPOS;

// Define the union U_VHD2P22VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_982          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P22VLPOS;

// Define the union U_VHD2P23RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_983          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P23RESO;

// Define the union U_VHD2P23LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P23LADDR;
// Define the union U_VHD2P23CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P23CADDR;
// Define the union U_VHD2P23STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P23STRIDE;

// Define the union U_VHD2P23VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_985          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P23VFPOS;

// Define the union U_VHD2P23VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_986          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P23VLPOS;

// Define the union U_VHD2P24RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_987          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P24RESO;

// Define the union U_VHD2P24LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P24LADDR;
// Define the union U_VHD2P24CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P24CADDR;
// Define the union U_VHD2P24STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P24STRIDE;

// Define the union U_VHD2P24VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_989          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P24VFPOS;

// Define the union U_VHD2P24VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_990          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P24VLPOS;

// Define the union U_VHD2P25RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_991          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P25RESO;

// Define the union U_VHD2P25LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P25LADDR;
// Define the union U_VHD2P25CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P25CADDR;
// Define the union U_VHD2P25STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P25STRIDE;

// Define the union U_VHD2P25VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_993          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P25VFPOS;

// Define the union U_VHD2P25VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_994          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P25VLPOS;

// Define the union U_VHD2P26RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_995          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P26RESO;

// Define the union U_VHD2P26LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P26LADDR;
// Define the union U_VHD2P26CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P26CADDR;
// Define the union U_VHD2P26STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P26STRIDE;

// Define the union U_VHD2P26VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_997          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P26VFPOS;

// Define the union U_VHD2P26VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_998          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P26VLPOS;

// Define the union U_VHD2P27RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_999          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P27RESO;

// Define the union U_VHD2P27LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P27LADDR;
// Define the union U_VHD2P27CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P27CADDR;
// Define the union U_VHD2P27STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P27STRIDE;

// Define the union U_VHD2P27VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1001         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P27VFPOS;

// Define the union U_VHD2P27VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1002         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P27VLPOS;

// Define the union U_VHD2P28RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1003         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P28RESO;

// Define the union U_VHD2P28LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P28LADDR;
// Define the union U_VHD2P28CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P28CADDR;
// Define the union U_VHD2P28STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P28STRIDE;

// Define the union U_VHD2P28VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1005         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P28VFPOS;

// Define the union U_VHD2P28VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1006         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P28VLPOS;

// Define the union U_VHD2P29RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1007         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P29RESO;

// Define the union U_VHD2P29LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P29LADDR;
// Define the union U_VHD2P29CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P29CADDR;
// Define the union U_VHD2P29STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P29STRIDE;

// Define the union U_VHD2P29VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1009         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P29VFPOS;

// Define the union U_VHD2P29VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1010         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P29VLPOS;

// Define the union U_VHD2P30RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1011         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P30RESO;

// Define the union U_VHD2P30LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P30LADDR;
// Define the union U_VHD2P30CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P30CADDR;
// Define the union U_VHD2P30STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P30STRIDE;

// Define the union U_VHD2P30VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1013         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P30VFPOS;

// Define the union U_VHD2P30VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1014         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P30VLPOS;

// Define the union U_VHD2P31RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1015         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P31RESO;

// Define the union U_VHD2P31LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P31LADDR;
// Define the union U_VHD2P31CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P31CADDR;
// Define the union U_VHD2P31STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P31STRIDE;

// Define the union U_VHD2P31VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1017         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P31VFPOS;

// Define the union U_VHD2P31VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1018         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P31VLPOS;

// Define the union U_VHD2P32RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1019         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P32RESO;

// Define the union U_VHD2P32LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P32LADDR;
// Define the union U_VHD2P32CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P32CADDR;
// Define the union U_VHD2P32STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P32STRIDE;

// Define the union U_VHD2P32VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1021         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P32VFPOS;

// Define the union U_VHD2P32VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1022         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P32VLPOS;

// Define the union U_VHD2P33RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1023         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P33RESO;

// Define the union U_VHD2P33LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P33LADDR;
// Define the union U_VHD2P33CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P33CADDR;
// Define the union U_VHD2P33STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P33STRIDE;

// Define the union U_VHD2P33VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1025         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P33VFPOS;

// Define the union U_VHD2P33VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1026         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P33VLPOS;

// Define the union U_VHD2P34RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1027         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P34RESO;

// Define the union U_VHD2P34LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P34LADDR;
// Define the union U_VHD2P34CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P34CADDR;
// Define the union U_VHD2P34STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P34STRIDE;

// Define the union U_VHD2P34VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1029         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P34VFPOS;

// Define the union U_VHD2P34VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1030         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P34VLPOS;

// Define the union U_VHD2P35RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1031         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P35RESO;

// Define the union U_VHD2P35LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P35LADDR;
// Define the union U_VHD2P35CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P35CADDR;
// Define the union U_VHD2P35STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P35STRIDE;

// Define the union U_VHD2P35VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1033         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P35VFPOS;

// Define the union U_VHD2P35VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1034         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P35VLPOS;

// Define the union U_VHD2P36RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1035         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P36RESO;

// Define the union U_VHD2P36LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P36LADDR;
// Define the union U_VHD2P36CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P36CADDR;
// Define the union U_VHD2P36STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P36STRIDE;

// Define the union U_VHD2P36VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1037         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P36VFPOS;

// Define the union U_VHD2P36VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1038         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P36VLPOS;

// Define the union U_VHD2P37RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1039         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P37RESO;

// Define the union U_VHD2P37LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P37LADDR;
// Define the union U_VHD2P37CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P37CADDR;
// Define the union U_VHD2P37STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P37STRIDE;

// Define the union U_VHD2P37VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1041         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P37VFPOS;

// Define the union U_VHD2P37VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1042         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P37VLPOS;

// Define the union U_VHD2P38RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1043         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P38RESO;

// Define the union U_VHD2P38LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P38LADDR;
// Define the union U_VHD2P38CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P38CADDR;
// Define the union U_VHD2P38STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P38STRIDE;

// Define the union U_VHD2P38VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1045         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P38VFPOS;

// Define the union U_VHD2P38VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1046         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P38VLPOS;

// Define the union U_VHD2P39RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1047         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P39RESO;

// Define the union U_VHD2P39LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P39LADDR;
// Define the union U_VHD2P39CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P39CADDR;
// Define the union U_VHD2P39STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P39STRIDE;

// Define the union U_VHD2P39VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1049         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P39VFPOS;

// Define the union U_VHD2P39VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1050         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P39VLPOS;

// Define the union U_VHD2P40RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1051         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P40RESO;

// Define the union U_VHD2P40LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P40LADDR;
// Define the union U_VHD2P40CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P40CADDR;
// Define the union U_VHD2P40STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P40STRIDE;

// Define the union U_VHD2P40VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1053         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P40VFPOS;

// Define the union U_VHD2P40VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1054         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P40VLPOS;

// Define the union U_VHD2P41RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1055         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P41RESO;

// Define the union U_VHD2P41LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P41LADDR;
// Define the union U_VHD2P41CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P41CADDR;
// Define the union U_VHD2P41STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P41STRIDE;

// Define the union U_VHD2P41VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1057         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P41VFPOS;

// Define the union U_VHD2P41VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1058         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P41VLPOS;

// Define the union U_VHD2P42RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1059         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P42RESO;

// Define the union U_VHD2P42LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P42LADDR;
// Define the union U_VHD2P42CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P42CADDR;
// Define the union U_VHD2P42STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P42STRIDE;

// Define the union U_VHD2P42VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1061         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P42VFPOS;

// Define the union U_VHD2P42VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1062         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P42VLPOS;

// Define the union U_VHD2P43RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1063         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P43RESO;

// Define the union U_VHD2P43LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P43LADDR;
// Define the union U_VHD2P43CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P43CADDR;
// Define the union U_VHD2P43STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P43STRIDE;

// Define the union U_VHD2P43VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1065         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P43VFPOS;

// Define the union U_VHD2P43VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1066         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P43VLPOS;

// Define the union U_VHD2P44RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1067         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P44RESO;

// Define the union U_VHD2P44LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P44LADDR;
// Define the union U_VHD2P44CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P44CADDR;
// Define the union U_VHD2P44STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P44STRIDE;

// Define the union U_VHD2P44VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1069         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P44VFPOS;

// Define the union U_VHD2P44VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1070         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P44VLPOS;

// Define the union U_VHD2P45RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1071         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P45RESO;

// Define the union U_VHD2P45LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P45LADDR;
// Define the union U_VHD2P45CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P45CADDR;
// Define the union U_VHD2P45STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P45STRIDE;

// Define the union U_VHD2P45VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1073         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P45VFPOS;

// Define the union U_VHD2P45VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1074         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P45VLPOS;

// Define the union U_VHD2P46RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1075         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P46RESO;

// Define the union U_VHD2P46LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P46LADDR;
// Define the union U_VHD2P46CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P46CADDR;
// Define the union U_VHD2P46STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P46STRIDE;

// Define the union U_VHD2P46VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1077         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P46VFPOS;

// Define the union U_VHD2P46VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1078         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P46VLPOS;

// Define the union U_VHD2P47RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1079         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P47RESO;

// Define the union U_VHD2P47LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P47LADDR;
// Define the union U_VHD2P47CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P47CADDR;
// Define the union U_VHD2P47STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P47STRIDE;

// Define the union U_VHD2P47VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1081         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P47VFPOS;

// Define the union U_VHD2P47VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1082         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P47VLPOS;

// Define the union U_VHD2P48RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1083         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P48RESO;

// Define the union U_VHD2P48LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P48LADDR;
// Define the union U_VHD2P48CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P48CADDR;
// Define the union U_VHD2P48STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P48STRIDE;

// Define the union U_VHD2P48VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1085         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P48VFPOS;

// Define the union U_VHD2P48VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1086         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P48VLPOS;

// Define the union U_VHD2P49RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1087         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P49RESO;

// Define the union U_VHD2P49LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P49LADDR;
// Define the union U_VHD2P49CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P49CADDR;
// Define the union U_VHD2P49STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P49STRIDE;

// Define the union U_VHD2P49VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1089         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P49VFPOS;

// Define the union U_VHD2P49VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1090         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P49VLPOS;

// Define the union U_VHD2P50RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1091         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P50RESO;

// Define the union U_VHD2P50LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P50LADDR;
// Define the union U_VHD2P50CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P50CADDR;
// Define the union U_VHD2P50STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P50STRIDE;

// Define the union U_VHD2P50VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1093         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P50VFPOS;

// Define the union U_VHD2P50VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1094         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P50VLPOS;

// Define the union U_VHD2P51RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1095         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P51RESO;

// Define the union U_VHD2P51LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P51LADDR;
// Define the union U_VHD2P51CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P51CADDR;
// Define the union U_VHD2P51STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P51STRIDE;

// Define the union U_VHD2P51VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1097         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P51VFPOS;

// Define the union U_VHD2P51VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1098         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P51VLPOS;

// Define the union U_VHD2P52RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1099         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P52RESO;

// Define the union U_VHD2P52LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P52LADDR;
// Define the union U_VHD2P52CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P52CADDR;
// Define the union U_VHD2P52STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P52STRIDE;

// Define the union U_VHD2P52VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1101         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P52VFPOS;

// Define the union U_VHD2P52VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1102         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P52VLPOS;

// Define the union U_VHD2P53RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1103         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P53RESO;

// Define the union U_VHD2P53LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P53LADDR;
// Define the union U_VHD2P53CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P53CADDR;
// Define the union U_VHD2P53STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P53STRIDE;

// Define the union U_VHD2P53VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1105         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P53VFPOS;

// Define the union U_VHD2P53VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1106         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P53VLPOS;

// Define the union U_VHD2P54RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1107         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P54RESO;

// Define the union U_VHD2P54LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P54LADDR;
// Define the union U_VHD2P54CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P54CADDR;
// Define the union U_VHD2P54STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P54STRIDE;

// Define the union U_VHD2P54VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1109         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P54VFPOS;

// Define the union U_VHD2P54VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1110         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P54VLPOS;

// Define the union U_VHD2P55RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1111         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P55RESO;

// Define the union U_VHD2P55LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P55LADDR;
// Define the union U_VHD2P55CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P55CADDR;
// Define the union U_VHD2P55STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P55STRIDE;

// Define the union U_VHD2P55VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1113         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P55VFPOS;

// Define the union U_VHD2P55VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1114         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P55VLPOS;

// Define the union U_VHD2P56RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1115         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P56RESO;

// Define the union U_VHD2P56LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P56LADDR;
// Define the union U_VHD2P56CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P56CADDR;
// Define the union U_VHD2P56STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P56STRIDE;

// Define the union U_VHD2P56VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1117         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P56VFPOS;

// Define the union U_VHD2P56VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1118         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P56VLPOS;

// Define the union U_VHD2P57RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1119         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P57RESO;

// Define the union U_VHD2P57LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P57LADDR;
// Define the union U_VHD2P57CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P57CADDR;
// Define the union U_VHD2P57STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P57STRIDE;

// Define the union U_VHD2P57VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1121         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P57VFPOS;

// Define the union U_VHD2P57VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1122         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P57VLPOS;

// Define the union U_VHD2P58RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1123         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P58RESO;

// Define the union U_VHD2P58LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P58LADDR;
// Define the union U_VHD2P58CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P58CADDR;
// Define the union U_VHD2P58STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P58STRIDE;

// Define the union U_VHD2P58VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1125         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P58VFPOS;

// Define the union U_VHD2P58VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1126         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P58VLPOS;

// Define the union U_VHD2P59RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1127         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P59RESO;

// Define the union U_VHD2P59LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P59LADDR;
// Define the union U_VHD2P59CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P59CADDR;
// Define the union U_VHD2P59STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P59STRIDE;

// Define the union U_VHD2P59VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1129         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P59VFPOS;

// Define the union U_VHD2P59VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1130         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P59VLPOS;

// Define the union U_VHD2P60RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1131         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P60RESO;

// Define the union U_VHD2P60LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P60LADDR;
// Define the union U_VHD2P60CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P60CADDR;
// Define the union U_VHD2P60STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P60STRIDE;

// Define the union U_VHD2P60VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1133         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P60VFPOS;

// Define the union U_VHD2P60VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1134         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P60VLPOS;

// Define the union U_VHD2P61RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1135         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P61RESO;

// Define the union U_VHD2P61LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P61LADDR;
// Define the union U_VHD2P61CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P61CADDR;
// Define the union U_VHD2P61STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P61STRIDE;

// Define the union U_VHD2P61VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1137         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P61VFPOS;

// Define the union U_VHD2P61VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1138         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P61VLPOS;

// Define the union U_VHD2P62RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1139         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P62RESO;

// Define the union U_VHD2P62LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P62LADDR;
// Define the union U_VHD2P62CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P62CADDR;
// Define the union U_VHD2P62STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P62STRIDE;

// Define the union U_VHD2P62VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1141         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P62VFPOS;

// Define the union U_VHD2P62VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1142         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P62VLPOS;

// Define the union U_VHD2P63RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1143         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P63RESO;

// Define the union U_VHD2P63LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P63LADDR;
// Define the union U_VHD2P63CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHD2P63CADDR;
// Define the union U_VHD2P63STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P63STRIDE;

// Define the union U_VHD2P63VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1145         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P63VFPOS;

// Define the union U_VHD2P63VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1146         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD2P63VLPOS;

// Define the union U_VHD264REGIONENH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p32_en                : 1   ; // [0]
        HI_U32    p33_en                : 1   ; // [1]
        HI_U32    p34_en                : 1   ; // [2]
        HI_U32    p35_en                : 1   ; // [3]
        HI_U32    p36_en                : 1   ; // [4]
        HI_U32    p37_en                : 1   ; // [5]
        HI_U32    p38_en                : 1   ; // [6]
        HI_U32    p39_en                : 1   ; // [7]
        HI_U32    p40_en                : 1   ; // [8]
        HI_U32    p41_en                : 1   ; // [9]
        HI_U32    p42_en                : 1   ; // [10]
        HI_U32    p43_en                : 1   ; // [11]
        HI_U32    p44_en                : 1   ; // [12]
        HI_U32    p45_en                : 1   ; // [13]
        HI_U32    p46_en                : 1   ; // [14]
        HI_U32    p47_en                : 1   ; // [15]
        HI_U32    p48_en                : 1   ; // [16]
        HI_U32    p49_en                : 1   ; // [17]
        HI_U32    p50_en                : 1   ; // [18]
        HI_U32    p51_en                : 1   ; // [19]
        HI_U32    p52_en                : 1   ; // [20]
        HI_U32    p53_en                : 1   ; // [21]
        HI_U32    p54_en                : 1   ; // [22]
        HI_U32    p55_en                : 1   ; // [23]
        HI_U32    p56_en                : 1   ; // [24]
        HI_U32    p57_en                : 1   ; // [25]
        HI_U32    p58_en                : 1   ; // [26]
        HI_U32    p59_en                : 1   ; // [27]
        HI_U32    p60_en                : 1   ; // [28]
        HI_U32    p61_en                : 1   ; // [29]
        HI_U32    p62_en                : 1   ; // [30]
        HI_U32    p63_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD264REGIONENH;

// Define the union U_VHD264REGIONENL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    p1_en                 : 1   ; // [1]
        HI_U32    p2_en                 : 1   ; // [2]
        HI_U32    p3_en                 : 1   ; // [3]
        HI_U32    p4_en                 : 1   ; // [4]
        HI_U32    p5_en                 : 1   ; // [5]
        HI_U32    p6_en                 : 1   ; // [6]
        HI_U32    p7_en                 : 1   ; // [7]
        HI_U32    p8_en                 : 1   ; // [8]
        HI_U32    p9_en                 : 1   ; // [9]
        HI_U32    p10_en                : 1   ; // [10]
        HI_U32    p11_en                : 1   ; // [11]
        HI_U32    p12_en                : 1   ; // [12]
        HI_U32    p13_en                : 1   ; // [13]
        HI_U32    p14_en                : 1   ; // [14]
        HI_U32    p15_en                : 1   ; // [15]
        HI_U32    p16_en                : 1   ; // [16]
        HI_U32    p17_en                : 1   ; // [17]
        HI_U32    p18_en                : 1   ; // [18]
        HI_U32    p19_en                : 1   ; // [19]
        HI_U32    p20_en                : 1   ; // [20]
        HI_U32    p21_en                : 1   ; // [21]
        HI_U32    p22_en                : 1   ; // [22]
        HI_U32    p23_en                : 1   ; // [23]
        HI_U32    p24_en                : 1   ; // [24]
        HI_U32    p25_en                : 1   ; // [25]
        HI_U32    p26_en                : 1   ; // [26]
        HI_U32    p27_en                : 1   ; // [27]
        HI_U32    p28_en                : 1   ; // [28]
        HI_U32    p29_en                : 1   ; // [29]
        HI_U32    p30_en                : 1   ; // [30]
        HI_U32    p31_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD264REGIONENL;

// Define the union U_VSDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 4   ; // [3..0]
        HI_U32    time_out              : 4   ; // [7..4]
        HI_U32    Reserved_1150         : 9   ; // [16..8]
        HI_U32    vup_mode              : 1   ; // [17]
        HI_U32    ifir_mode             : 2   ; // [19..18]
        HI_U32    Reserved_1149         : 11  ; // [30..20]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCTRL;

// Define the union U_VSDUPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_1152         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDUPD;

// Define the union U_VSDCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_caddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDCADDR;
// Define the union U_VSDCCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_ccaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDCCADDR;
// Define the union U_VSDSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDSTRIDE;

// Define the union U_VSDIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_1155         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDIRESO;

// Define the union U_VSDLKEY
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_low              : 8   ; // [7..0]
        HI_U32    luma_high             : 8   ; // [15..8]
        HI_U32    luma_mask             : 8   ; // [23..16]
        HI_U32    Reserved_1156         : 7   ; // [30..24]
        HI_U32    luma_key_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDLKEY;

// Define the union U_VSDCBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    Reserved_1158         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCBMPARA;

// Define the union U_VSDDFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_1159         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDDFPOS;

// Define the union U_VSDDLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_1161         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDDLPOS;

// Define the union U_VSDVFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1162         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDVFPOS;

// Define the union U_VSDVLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1163         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDVLPOS;

// Define the union U_VSDBK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbk_cr                : 8   ; // [7..0]
        HI_U32    vbk_cb                : 8   ; // [15..8]
        HI_U32    vbk_y                 : 8   ; // [23..16]
        HI_U32    vbk_alpha             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDBK;

// Define the union U_VSDCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc2               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc0               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_1164         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCSCIDC;

// Define the union U_VSDCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc2               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc0               : 9   ; // [26..18]
        HI_U32    Reserved_1166         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCSCODC;

// Define the union U_VSDCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_1168         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_1167         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCSCP0;

// Define the union U_VSDCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_1170         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_1169         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCSCP1;

// Define the union U_VSDCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_1172         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_1171         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCSCP2;

// Define the union U_VSDCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_1174         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_1173         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCSCP3;

// Define the union U_VSDCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_1175         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDCSCP4;

// Define the union U_VSDACM0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    acm0_en               : 1   ; // [28]
        HI_U32    acm1_en               : 1   ; // [29]
        HI_U32    acm2_en               : 1   ; // [30]
        HI_U32    acm3_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM0;

// Define the union U_VSDACM1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    acm_test_en           : 1   ; // [20]
        HI_U32    Reserved_1177         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM1;

// Define the union U_VSDACM2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_1178         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM2;

// Define the union U_VSDACM3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_1179         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM3;

// Define the union U_VSDACM4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_1180         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM4;

// Define the union U_VSDACM5
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_1181         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM5;

// Define the union U_VSDACM6
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_1182         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM6;

// Define the union U_VSDACM7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_1183         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACM7;

// Define the union U_VSDHSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_1186         : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_1185         : 3   ; // [22..20]
        HI_U32    hchfir_en             : 1   ; // [23]
        HI_U32    hlfir_en              : 1   ; // [24]
        HI_U32    Reserved_1184         : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hchmsc_en             : 1   ; // [30]
        HI_U32    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDHSP;

// Define the union U_VSDHLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_loffset           : 17  ; // [16..0]
        HI_U32    Reserved_1187         : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDHLOFFSET;

// Define the union U_VSDHCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 17  ; // [16..0]
        HI_U32    Reserved_1188         : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDHCOFFSET;

// Define the union U_VSDVSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_1189         : 23  ; // [22..0]
        HI_U32    vchfir_en             : 1   ; // [23]
        HI_U32    vlfir_en              : 1   ; // [24]
        HI_U32    chroma_type           : 1   ; // [25]
        HI_U32    vsc_chroma_tap        : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vchmsc_en             : 1   ; // [30]
        HI_U32    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDVSP;

// Define the union U_VSDVSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_1191         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDVSR;

// Define the union U_VSDVOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vchroma_offset        : 16  ; // [15..0]
        HI_U32    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDVOFFSET;

// Define the union U_VSDZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_1192         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDZMEORESO;

// Define the union U_VSDZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_1193         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDZMEIRESO;

// Define the union U_VSDZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    need_rand_range       : 8   ; // [7..0]
        HI_U32    rdy_rand_range        : 8   ; // [15..8]
        HI_U32    need_mode             : 1   ; // [16]
        HI_U32    rdy_mode              : 1   ; // [17]
        HI_U32    Reserved_1194         : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDZMEDBG;

// Define the union U_VSDMOSAIC
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_1195           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDMOSAIC;
// Define the union U_VSDACCTHD1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_low               : 10  ; // [9..0]
        HI_U32    thd_high              : 10  ; // [19..10]
        HI_U32    thd_med_low           : 10  ; // [29..20]
        HI_U32    acc_mode              : 1   ; // [30]
        HI_U32    acc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCTHD1;

// Define the union U_VSDACCTHD2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_med_high          : 10  ; // [9..0]
        HI_U32    acc_multiple          : 8   ; // [17..10]
        HI_U32    acc_rst               : 1   ; // [18]
        HI_U32    Reserved_1197         : 13  ; // [31..19]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCTHD2;

// Define the union U_VSDACCLOWN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1198         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCLOWN;

// Define the union U_VSDACCMEDN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1200         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCMEDN;

// Define the union U_VSDACCHIGHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1202         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCHIGHN;

// Define the union U_VSDACCMLN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1204         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCMLN;

// Define the union U_VSDACCMHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1206         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCMHN;

// Define the union U_VSDACC3LOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_low              : 21  ; // [20..0]
        HI_U32    Reserved_1208         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACC3LOW;

// Define the union U_VSDACC3MED
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_med              : 21  ; // [20..0]
        HI_U32    Reserved_1210         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACC3MED;

// Define the union U_VSDACC3HIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_high             : 21  ; // [20..0]
        HI_U32    Reserved_1211         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACC3HIGH;

// Define the union U_VSDACC8MLOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_low          : 21  ; // [20..0]
        HI_U32    Reserved_1212         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACC8MLOW;

// Define the union U_VSDACC8MHIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_high         : 21  ; // [20..0]
        HI_U32    Reserved_1213         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACC8MHIGH;

// Define the union U_VSDACCTOTAL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acc_pix_total         : 21  ; // [20..0]
        HI_U32    Reserved_1214         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDACCTOTAL;

// Define the union U_VSDIFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef0                 : 10  ; // [9..0]
        HI_U32    Reserved_1216         : 6   ; // [15..10]
        HI_U32    coef1                 : 10  ; // [25..16]
        HI_U32    Reserved_1215         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDIFIRCOEF01;

// Define the union U_VSDIFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef2                 : 10  ; // [9..0]
        HI_U32    Reserved_1219         : 6   ; // [15..10]
        HI_U32    coef3                 : 10  ; // [25..16]
        HI_U32    Reserved_1218         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDIFIRCOEF23;

// Define the union U_VSDIFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef4                 : 10  ; // [9..0]
        HI_U32    Reserved_1221         : 6   ; // [15..10]
        HI_U32    coef5                 : 10  ; // [25..16]
        HI_U32    Reserved_1220         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDIFIRCOEF45;

// Define the union U_VSDIFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef6                 : 10  ; // [9..0]
        HI_U32    Reserved_1223         : 6   ; // [15..10]
        HI_U32    coef7                 : 10  ; // [25..16]
        HI_U32    Reserved_1222         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDIFIRCOEF67;

// Define the union U_VSDP0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1224         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP0RESO;

// Define the union U_VSDP0LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP0LADDR;
// Define the union U_VSDP0CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP0CADDR;
// Define the union U_VSDP0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP0STRIDE;

// Define the union U_VSDP0VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1226         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP0VFPOS;

// Define the union U_VSDP0VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1227         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP0VLPOS;

// Define the union U_VSDP1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1228         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP1RESO;

// Define the union U_VSDP1LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP1LADDR;
// Define the union U_VSDP1CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP1CADDR;
// Define the union U_VSDP1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP1STRIDE;

// Define the union U_VSDP1VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1230         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP1VFPOS;

// Define the union U_VSDP1VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1231         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP1VLPOS;

// Define the union U_VSDP2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1232         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP2RESO;

// Define the union U_VSDP2LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP2LADDR;
// Define the union U_VSDP2CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP2CADDR;
// Define the union U_VSDP2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP2STRIDE;

// Define the union U_VSDP2VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1234         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP2VFPOS;

// Define the union U_VSDP2VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1235         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP2VLPOS;

// Define the union U_VSDP3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1236         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP3RESO;

// Define the union U_VSDP3LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP3LADDR;
// Define the union U_VSDP3CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP3CADDR;
// Define the union U_VSDP3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP3STRIDE;

// Define the union U_VSDP3VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1238         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP3VFPOS;

// Define the union U_VSDP3VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1239         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP3VLPOS;

// Define the union U_VSDP4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1240         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP4RESO;

// Define the union U_VSDP4LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP4LADDR;
// Define the union U_VSDP4CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP4CADDR;
// Define the union U_VSDP4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP4STRIDE;

// Define the union U_VSDP4VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1242         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP4VFPOS;

// Define the union U_VSDP4VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1243         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP4VLPOS;

// Define the union U_VSDP5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1244         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP5RESO;

// Define the union U_VSDP5LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP5LADDR;
// Define the union U_VSDP5CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP5CADDR;
// Define the union U_VSDP5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP5STRIDE;

// Define the union U_VSDP5VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1246         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP5VFPOS;

// Define the union U_VSDP5VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1247         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP5VLPOS;

// Define the union U_VSDP6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1248         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP6RESO;

// Define the union U_VSDP6LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP6LADDR;
// Define the union U_VSDP6CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP6CADDR;
// Define the union U_VSDP6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP6STRIDE;

// Define the union U_VSDP6VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1250         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP6VFPOS;

// Define the union U_VSDP6VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1251         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP6VLPOS;

// Define the union U_VSDP7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1252         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP7RESO;

// Define the union U_VSDP7LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP7LADDR;
// Define the union U_VSDP7CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDP7CADDR;
// Define the union U_VSDP7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP7STRIDE;

// Define the union U_VSDP7VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1254         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP7VFPOS;

// Define the union U_VSDP7VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1255         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDP7VLPOS;

// Define the union U_VSD16REGIONEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]        
        HI_U32    Reserved_1288         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1REGIONEN;

// Define the union U_VSD1CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 4   ; // [3..0]
        HI_U32    time_out              : 4   ; // [7..4]
        HI_U32    Reserved_1292         : 9   ; // [16..8]
        HI_U32    vup_mode              : 1   ; // [17]
        HI_U32    ifir_mode             : 2   ; // [19..18]
        HI_U32    Reserved_1291         : 11  ; // [30..20]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CTRL;

// Define the union U_VSD1UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_1294         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1UPD;

// Define the union U_VSD1CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_caddr           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSD1CADDR;
// Define the union U_VSD1CCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_ccaddr          : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSD1CCADDR;
// Define the union U_VSD1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1STRIDE;

// Define the union U_VSD1IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_1297         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1IRESO;

// Define the union U_VSD1LKEY
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    luma_low              : 8   ; // [7..0]
        HI_U32    luma_high             : 8   ; // [15..8]
        HI_U32    luma_mask             : 8   ; // [23..16]
        HI_U32    Reserved_1298         : 7   ; // [30..24]
        HI_U32    luma_key_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1LKEY;

// Define the union U_VSD1CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    Reserved_1300         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CBMPARA;

// Define the union U_VSD1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_1301         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1DFPOS;

// Define the union U_VSD1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_1303         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1DLPOS;

// Define the union U_VSD1VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1304         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1VFPOS;

// Define the union U_VSD1VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1305         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1VLPOS;

// Define the union U_VSD1BK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbk_cr                : 8   ; // [7..0]
        HI_U32    vbk_cb                : 8   ; // [15..8]
        HI_U32    vbk_y                 : 8   ; // [23..16]
        HI_U32    vbk_alpha             : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1BK;

// Define the union U_VSD1CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc2               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc0               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_1306         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CSCIDC;

// Define the union U_VSD1CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc2               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc0               : 9   ; // [26..18]
        HI_U32    Reserved_1308         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CSCODC;

// Define the union U_VSD1CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_1310         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_1309         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CSCP0;

// Define the union U_VSD1CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_1312         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_1311         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CSCP1;

// Define the union U_VSD1CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_1314         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_1313         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CSCP2;

// Define the union U_VSD1CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_1316         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_1315         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CSCP3;

// Define the union U_VSD1CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_1317         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1CSCP4;

// Define the union U_VSD1ACM0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    acm0_en               : 1   ; // [28]
        HI_U32    acm1_en               : 1   ; // [29]
        HI_U32    acm2_en               : 1   ; // [30]
        HI_U32    acm3_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM0;

// Define the union U_VSD1ACM1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    acm_test_en           : 1   ; // [20]
        HI_U32    Reserved_1319         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM1;

// Define the union U_VSD1ACM2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_1320         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM2;

// Define the union U_VSD1ACM3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_1321         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM3;

// Define the union U_VSD1ACM4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_1322         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM4;

// Define the union U_VSD1ACM5
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_1323         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM5;

// Define the union U_VSD1ACM6
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_u_off           : 5   ; // [4..0]
        HI_U32    acm_b_u_off           : 5   ; // [9..5]
        HI_U32    acm_c_u_off           : 5   ; // [14..10]
        HI_U32    acm_d_u_off           : 5   ; // [19..15]
        HI_U32    acm_fir_blk           : 4   ; // [23..20]
        HI_U32    acm_sec_blk           : 4   ; // [27..24]
        HI_U32    Reserved_1324         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM6;

// Define the union U_VSD1ACM7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acm_a_v_off           : 5   ; // [4..0]
        HI_U32    acm_b_v_off           : 5   ; // [9..5]
        HI_U32    acm_c_v_off           : 5   ; // [14..10]
        HI_U32    acm_d_v_off           : 5   ; // [19..15]
        HI_U32    Reserved_1325         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACM7;

// Define the union U_VSD1HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_1328         : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_1327         : 3   ; // [22..20]
        HI_U32    hchfir_en             : 1   ; // [23]
        HI_U32    hlfir_en              : 1   ; // [24]
        HI_U32    Reserved_1326         : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hchmsc_en             : 1   ; // [30]
        HI_U32    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1HSP;

// Define the union U_VSD1HLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_loffset           : 17  ; // [16..0]
        HI_U32    Reserved_1329         : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1HLOFFSET;

// Define the union U_VSD1HCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 17  ; // [16..0]
        HI_U32    Reserved_1330         : 15  ; // [31..17]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1HCOFFSET;

// Define the union U_VSD1VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_1331         : 23  ; // [22..0]
        HI_U32    vchfir_en             : 1   ; // [23]
        HI_U32    vlfir_en              : 1   ; // [24]
        HI_U32    chroma_type           : 1   ; // [25]
        HI_U32    vsc_chroma_tap        : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vchmsc_en             : 1   ; // [30]
        HI_U32    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1VSP;

// Define the union U_VSD1VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_1333         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1VSR;

// Define the union U_VSD1VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vchroma_offset        : 16  ; // [15..0]
        HI_U32    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1VOFFSET;

// Define the union U_VSD1ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_1334         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ZMEORESO;

// Define the union U_VSD1ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_1335         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ZMEIRESO;

// Define the union U_VSD1ZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    need_rand_range       : 8   ; // [7..0]
        HI_U32    rdy_rand_range        : 8   ; // [15..8]
        HI_U32    need_mode             : 1   ; // [16]
        HI_U32    rdy_mode              : 1   ; // [17]
        HI_U32    Reserved_1336         : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ZMEDBG;

// Define the union U_VSD1MOSAIC
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_1337           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSD1MOSAIC;
// Define the union U_VSD1ACCTHD1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_low               : 10  ; // [9..0]
        HI_U32    thd_high              : 10  ; // [19..10]
        HI_U32    thd_med_low           : 10  ; // [29..20]
        HI_U32    acc_mode              : 1   ; // [30]
        HI_U32    acc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCTHD1;

// Define the union U_VSD1ACCTHD2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    thd_med_high          : 10  ; // [9..0]
        HI_U32    acc_multiple          : 8   ; // [17..10]
        HI_U32    acc_rst               : 1   ; // [18]
        HI_U32    Reserved_1339         : 13  ; // [31..19]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCTHD2;

// Define the union U_VSD1ACCLOWN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1340         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCLOWN;

// Define the union U_VSD1ACCMEDN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1342         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCMEDN;

// Define the union U_VSD1ACCHIGHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1344         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCHIGHN;

// Define the union U_VSD1ACCMLN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1346         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCMLN;

// Define the union U_VSD1ACCMHN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    table_data1n          : 10  ; // [9..0]
        HI_U32    table_data2n          : 10  ; // [19..10]
        HI_U32    table_data3n          : 10  ; // [29..20]
        HI_U32    Reserved_1348         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCMHN;

// Define the union U_VSD1ACC3LOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_low              : 21  ; // [20..0]
        HI_U32    Reserved_1350         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACC3LOW;

// Define the union U_VSD1ACC3MED
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_med              : 21  ; // [20..0]
        HI_U32    Reserved_1352         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACC3MED;

// Define the union U_VSD1ACC3HIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt3_high             : 21  ; // [20..0]
        HI_U32    Reserved_1353         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACC3HIGH;

// Define the union U_VSD1ACC8MLOW
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_low          : 21  ; // [20..0]
        HI_U32    Reserved_1354         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACC8MLOW;

// Define the union U_VSD1ACC8MHIGH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cnt8_med_high         : 21  ; // [20..0]
        HI_U32    Reserved_1355         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACC8MHIGH;

// Define the union U_VSD1ACCTOTAL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    acc_pix_total         : 21  ; // [20..0]
        HI_U32    Reserved_1356         : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1ACCTOTAL;

// Define the union U_VSD1IFIRCOEF01
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef0                 : 10  ; // [9..0]
        HI_U32    Reserved_1358         : 6   ; // [15..10]
        HI_U32    coef1                 : 10  ; // [25..16]
        HI_U32    Reserved_1357         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1IFIRCOEF01;

// Define the union U_VSD1IFIRCOEF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef2                 : 10  ; // [9..0]
        HI_U32    Reserved_1361         : 6   ; // [15..10]
        HI_U32    coef3                 : 10  ; // [25..16]
        HI_U32    Reserved_1360         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1IFIRCOEF23;

// Define the union U_VSD1IFIRCOEF45
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef4                 : 10  ; // [9..0]
        HI_U32    Reserved_1363         : 6   ; // [15..10]
        HI_U32    coef5                 : 10  ; // [25..16]
        HI_U32    Reserved_1362         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1IFIRCOEF45;

// Define the union U_VSD1IFIRCOEF67
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    coef6                 : 10  ; // [9..0]
        HI_U32    Reserved_1365         : 6   ; // [15..10]
        HI_U32    coef7                 : 10  ; // [25..16]
        HI_U32    Reserved_1364         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1IFIRCOEF67;

// Define the union U_VSD1P0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_1366         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1P0RESO;

// Define the union U_VSD1P0LADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSD1P0LADDR;
// Define the union U_VSD1P0CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSD1P0CADDR;
// Define the union U_VSD1P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    surface_cstride       : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1P0STRIDE;

// Define the union U_VSD1P0VFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xfpos           : 12  ; // [11..0]
        HI_U32    video_yfpos           : 12  ; // [23..12]
        HI_U32    Reserved_1368         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1P0VFPOS;

// Define the union U_VSD1P0VLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_xlpos           : 12  ; // [11..0]
        HI_U32    video_ylpos           : 12  ; // [23..12]
        HI_U32    Reserved_1369         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD1P0VLPOS;

// Define the union U_VSD116REGIONEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    Reserved_1430         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSD11REGIONEN;

// Define the union U_WBC2CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_1879         : 12  ; // [11..0]
        HI_U32    wbc0_dft              : 2   ; // [13..12]
        HI_U32    Reserved_1878         : 10  ; // [23..14]
        HI_U32    dfp_sel               : 3   ; // [26..24]
        HI_U32    Reserved_1877         : 1   ; // [27]
        HI_U32    wb_stp_en             : 1   ; // [28]
        HI_U32    Reserved_1876         : 2   ; // [30...29]
        HI_U32    wbc0_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCCTRL;

// Define the union U_WBC2UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_1881         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCUPD;

// Define the union U_WBC2ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 wbcaddr                 : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_WBCADDR;
// Define the union U_WBC2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wbc0stride            : 16  ; // [15..0]
        HI_U32    Reserved_1882         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCSTRIDE;

// Define the union U_WBC2ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_1883         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCORESO;

// Define the union U_WBC2CADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 wbccaddr                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_WBCCADDR;
// Define the union U_WBC2CSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wbc0cstride           : 16  ; // [15..0]
        HI_U32    Reserved_1885         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCCSTRIDE;

// Define the union U_WBC2HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 24  ; // [23..0]
        HI_U32    Reserved_1887         : 1   ; // [24]
        HI_U32    hchfir_en             : 1   ; // [25]
        HI_U32    hlfir_en              : 1   ; // [26]
        HI_U32    Reserved_1886         : 1   ; // [27]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hchmsc_en             : 1   ; // [30]
        HI_U32    hlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCHSP;

// Define the union U_WBC2HLOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_loffset           : 28  ; // [27..0]
        HI_U32    Reserved_1890         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCHLOFFSET;

// Define the union U_WBC2HCOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 28  ; // [27..0]
        HI_U32    Reserved_1891         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCHCOFFSET;

// Define the union U_WBC2VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_1892         : 19  ; // [18..0]
        HI_U32    zme_in_fmt            : 2   ; // [20..19]
        HI_U32    zme_out_fmt           : 2   ; // [22..21]
        HI_U32    vchfir_en             : 1   ; // [23]
        HI_U32    vlfir_en              : 1   ; // [24]
        HI_U32    chroma_type           : 1   ; // [25]
        HI_U32    vsc_chroma_tap        : 1   ; // [26]
        HI_U32    Reserved_1891         : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vchmsc_en             : 1   ; // [30]
        HI_U32    vlmsc_en              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCVSP;

// Define the union U_WBC2VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_1894         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCVSR;

// Define the union U_WBC2VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vchroma_offset        : 16  ; // [15..0]
        HI_U32    vluma_offset          : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCVOFFSET;

// Define the union U_WBC2ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_1895         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCZMEORESO;

// Define the union U_WBC2ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_1896         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCZMEIRESO;

// Define the union U_WBC2ZMEDBG
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    need_rand_range       : 8   ; // [7..0]
        HI_U32    rdy_rand_range        : 8   ; // [15..8]
        HI_U32    need_mode             : 1   ; // [16]
        HI_U32    rdy_mode              : 1   ; // [17]
        HI_U32    Reserved_1897         : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCZMEDBG;

// Define the union U_G0CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 8   ; // [7..0]
        HI_U32    bitext                : 2   ; // [9..8]
        HI_U32    Reserved_1976         : 4  ; // [13..10]
        HI_U32    key_en                 : 1  ; //[14]
        HI_U32    key_mode                :1  ;//[15]
        HI_U32    Reserved_4000         : 8  ;// [23..16]
        HI_U32    dcmp_inter              :1  ;//[24]
        HI_U32    dcmp_mode             : 1   ; // [25]
        HI_U32    read_mode             : 1   ; // [26]
        HI_U32    upd_mode              : 1   ; // [27]
        HI_U32    mute_en               : 1   ; // [28]
        HI_U32    gmm_en                : 1   ; // [29]
        HI_U32    Reserved_1975         : 1   ; // [30]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CTRL;

// Define the union U_G0UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_1978         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0UPD;

// Define the union U_G0ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0ADDR;
// Define the union U_G0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_1979         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0STRIDE;

// Define the union U_G0CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    palpha_range          : 1   ; // [8]
        HI_U32    Reserved_1981         : 1   ; // [9]
        HI_U32    vedge_p               : 1   ; // [10]
        HI_U32    hedge_p               : 1   ; // [11]
        HI_U32    palpha_en             : 1   ; // [12]
        HI_U32    premult_en            : 1   ; // [13]
        HI_U32    key_en                : 1   ; // [14]
        HI_U32    key_mode              : 1   ; // [15]
        HI_U32    Reserved_1980         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CBMPARA;

// Define the union U_G0CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_max              : 8   ; // [7..0]
        HI_U32    keyg_max              : 8   ; // [15..8]
        HI_U32    keyr_max              : 8   ; // [23..16]
        HI_U32    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CKEYMAX;

// Define the union U_G0CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_min              : 8   ; // [7..0]
        HI_U32    keyg_min              : 8   ; // [15..8]
        HI_U32    keyr_min              : 8   ; // [23..16]
        HI_U32    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CKEYMIN;

// Define the union U_G0CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    kmsk_b                : 8   ; // [7..0]
        HI_U32    kmsk_g                : 8   ; // [15..8]
        HI_U32    kmsk_r                : 8   ; // [23..16]
        HI_U32    Reserved_1982         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CMASK;

// Define the union U_G0IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_1983         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0IRESO;

// Define the union U_G0ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_1984         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0ORESO;

// Define the union U_G0SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    src_xfpos             : 7   ; // [6..0]
        HI_U32    Reserved_1985         : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0SFPOS;

// Define the union U_G0DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_1986         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0DFPOS;

// Define the union U_G0DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_1987         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0DLPOS;

// Define the union U_G0HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_1990         : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_1989         : 3   ; // [22..20]
        HI_U32    hafir_en              : 1   ; // [23]
        HI_U32    hfir_en               : 1   ; // [24]
        HI_U32    Reserved_1988         : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hamid_en              : 1   ; // [30]
        HI_U32    hsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0HSP;

// Define the union U_G0HOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 16  ; // [15..0]
        HI_U32    hor_loffset           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0HOFFSET;

// Define the union U_G0VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_1994         : 23  ; // [22..0]
        HI_U32    vafir_en              : 1   ; // [23]
        HI_U32    vfir_en               : 1   ; // [24]
        HI_U32    Reserved_1993         : 1   ; // [25]
        HI_U32    Reserved_1992         : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vamid_en              : 1   ; // [30]
        HI_U32    vsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0VSP;

// Define the union U_G0VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_1995         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0VSR;

// Define the union U_G0VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbtm_offset           : 16  ; // [15..0]
        HI_U32    vtp_offset            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0VOFFSET;

// Define the union U_G0ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_1996         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0ZMEORESO;

// Define the union U_G0ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_1997         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0ZMEIRESO;

// Define the union U_G0CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc2               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc0               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]       
        HI_U32    Reserved_1998         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CSCIDC;

// Define the union U_G0CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc2               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc0               : 9   ; // [26..18]
        HI_U32    Reserved_2000         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CSCODC;

// Define the union U_G0CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2002         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2001         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CSCP0;

// Define the union U_G0CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2004         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2003         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CSCP1;

// Define the union U_G0CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2006         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2005         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CSCP2;

// Define the union U_G0CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2008         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2007         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CSCP3;

// Define the union U_G0CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2009         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0CSCP4;

// Define the union U_G0P0ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P0ADDR;
// Define the union U_G0P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2011         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P0STRIDE;

// Define the union U_G0P0DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2012         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P0DFPOS;

// Define the union U_G0P0DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2013         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P0DLPOS;

// Define the union U_G0P0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2014         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P0RESO;

// Define the union U_G0P1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P1ADDR;
// Define the union U_G0P1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2015         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P1STRIDE;

// Define the union U_G0P1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2016         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P1DFPOS;

// Define the union U_G0P1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2017         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P1DLPOS;

// Define the union U_G0P1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2018         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P1RESO;

// Define the union U_G0P2ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P2ADDR;
// Define the union U_G0P2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2019         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P2STRIDE;

// Define the union U_G0P2DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2020         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P2DFPOS;

// Define the union U_G0P2DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2021         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P2DLPOS;

// Define the union U_G0P2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2022         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P2RESO;

// Define the union U_G0P3ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P3ADDR;
// Define the union U_G0P3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2023         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P3STRIDE;

// Define the union U_G0P3DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2024         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P3DFPOS;

// Define the union U_G0P3DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2025         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P3DLPOS;

// Define the union U_G0P3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2026         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P3RESO;

// Define the union U_G0P4ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P4ADDR;
// Define the union U_G0P4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2027         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P4STRIDE;

// Define the union U_G0P4DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2028         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P4DFPOS;

// Define the union U_G0P4DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2029         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P4DLPOS;

// Define the union U_G0P4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2030         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P4RESO;

// Define the union U_G0P5ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P5ADDR;
// Define the union U_G0P5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2031         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P5STRIDE;

// Define the union U_G0P5DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2032         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P5DFPOS;

// Define the union U_G0P5DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2033         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P5DLPOS;

// Define the union U_G0P5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2034         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P5RESO;

// Define the union U_G0P6ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P6ADDR;
// Define the union U_G0P6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2035         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P6STRIDE;

// Define the union U_G0P6DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2036         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P6DFPOS;

// Define the union U_G0P6DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2037         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P6DLPOS;

// Define the union U_G0P6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2038         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P6RESO;

// Define the union U_G0P7ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0P7ADDR;
// Define the union U_G0P7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2039         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P7STRIDE;

// Define the union U_G0P7DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2040         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P7DFPOS;

// Define the union U_G0P7DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2041         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P7DLPOS;

// Define the union U_G0P7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2042         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0P7RESO;

// Define the union U_G08SLICEEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    p1_en                 : 1   ; // [1]
        HI_U32    p2_en                 : 1   ; // [2]
        HI_U32    p3_en                 : 1   ; // [3]
        HI_U32    p4_en                 : 1   ; // [4]
        HI_U32    p5_en                 : 1   ; // [5]
        HI_U32    p6_en                 : 1   ; // [6]
        HI_U32    p7_en                 : 1   ; // [7]
        HI_U32    Reserved_2043         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G08SLICEEN;

// Define the union U_G0DCMPMODE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2045         : 4   ; // [3..0]
        HI_U32    line_len_idx          : 2   ; // [5..4]
        HI_U32    line_type             : 2   ; // [7..6]
        HI_U32    Reserved_2044         : 23  ; // [30..8]
        HI_U32    dcmp_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0DCMPMODE;

// Define the union U_G0DCMPBANKWIDTH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_bankwidth        : 12  ; // [11..0]
        HI_U32    Reserved_2047         : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0DCMPBANKWIDTH;

// Define the union U_G0DCMPSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_stride           : 16  ; // [15..0]
        HI_U32    Reserved_2048         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0DCMPSTRIDE;

// Define the union U_G0DCMPAADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_a             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0DCMPAADDR;
// Define the union U_G0DCMPRADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_r             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0DCMPRADDR;
// Define the union U_G0DCMPGADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_g             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0DCMPGADDR;
// Define the union U_G0DCMPBADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_b             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0DCMPBADDR;
// Define the union U_G1CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 8   ; // [7..0]
        HI_U32    bitext                : 2   ; // [9..8]
        HI_U32    Reserved_2050         : 15  ; // [24..10]
        HI_U32    dcmp_mode             : 1   ; // [25]
        HI_U32    read_mode             : 1   ; // [26]
        HI_U32    upd_mode              : 1   ; // [27]
        HI_U32    mute_en               : 1   ; // [28]
        HI_U32    gmm_en                : 1   ; // [29]
        HI_U32    Reserved_2049         : 1   ; // [30]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CTRL;

// Define the union U_G1UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_2052         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1UPD;

// Define the union U_G1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1ADDR;
// Define the union U_G1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2053         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1STRIDE;

// Define the union U_G1CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    palpha_range          : 1   ; // [8]
        HI_U32    Reserved_2055         : 1   ; // [9]
        HI_U32    vedge_p               : 1   ; // [10]
        HI_U32    hedge_p               : 1   ; // [11]
        HI_U32    palpha_en             : 1   ; // [12]
        HI_U32    premult_en            : 1   ; // [13]
        HI_U32    key_en                : 1   ; // [14]
        HI_U32    key_mode              : 1   ; // [15]
        HI_U32    Reserved_2054         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CBMPARA;

// Define the union U_G1CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_max              : 8   ; // [7..0]
        HI_U32    keyg_max              : 8   ; // [15..8]
        HI_U32    keyr_max              : 8   ; // [23..16]
        HI_U32    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CKEYMAX;

// Define the union U_G1CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_min              : 8   ; // [7..0]
        HI_U32    keyg_min              : 8   ; // [15..8]
        HI_U32    keyr_min              : 8   ; // [23..16]
        HI_U32    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CKEYMIN;

// Define the union U_G1CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    kmsk_b                : 8   ; // [7..0]
        HI_U32    kmsk_g                : 8   ; // [15..8]
        HI_U32    kmsk_r                : 8   ; // [23..16]
        HI_U32    Reserved_2056         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CMASK;

// Define the union U_G1IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2057         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1IRESO;

// Define the union U_G1ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2058         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1ORESO;

// Define the union U_G1SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    src_xfpos             : 7   ; // [6..0]
        HI_U32    Reserved_2059         : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1SFPOS;

// Define the union U_G1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_2060         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1DFPOS;

// Define the union U_G1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_2061         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1DLPOS;

// Define the union U_G1HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_2064         : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_2063         : 3   ; // [22..20]
        HI_U32    hafir_en              : 1   ; // [23]
        HI_U32    hfir_en               : 1   ; // [24]
        HI_U32    Reserved_2062         : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hamid_en              : 1   ; // [30]
        HI_U32    hsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1HSP;

// Define the union U_G1HOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 16  ; // [15..0]
        HI_U32    hor_loffset           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1HOFFSET;

// Define the union U_G1VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2068         : 23  ; // [22..0]
        HI_U32    vafir_en              : 1   ; // [23]
        HI_U32    vfir_en               : 1   ; // [24]
        HI_U32    Reserved_2067         : 1   ; // [25]
        HI_U32    Reserved_2066         : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vamid_en              : 1   ; // [30]
        HI_U32    vsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1VSP;

// Define the union U_G1VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_2069         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1VSR;

// Define the union U_G1VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbtm_offset           : 16  ; // [15..0]
        HI_U32    vtp_offset            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1VOFFSET;

// Define the union U_G1ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2070         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1ZMEORESO;

// Define the union U_G1ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2071         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1ZMEIRESO;

// Define the union U_G1CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_2072         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CSCIDC;

// Define the union U_G1CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_2074         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CSCODC;

// Define the union U_G1CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2076         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2075         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CSCP0;

// Define the union U_G1CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2078         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2077         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CSCP1;

// Define the union U_G1CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2080         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2079         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CSCP2;

// Define the union U_G1CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2082         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2081         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CSCP3;

// Define the union U_G1CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2083         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1CSCP4;

// Define the union U_G1P0ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P0ADDR;
// Define the union U_G1P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2085         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P0STRIDE;

// Define the union U_G1P0DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2086         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P0DFPOS;

// Define the union U_G1P0DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2087         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P0DLPOS;

// Define the union U_G1P0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2088         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P0RESO;

// Define the union U_G1P1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P1ADDR;
// Define the union U_G1P1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2089         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P1STRIDE;

// Define the union U_G1P1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2090         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P1DFPOS;

// Define the union U_G1P1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2091         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P1DLPOS;

// Define the union U_G1P1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2092         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P1RESO;

// Define the union U_G1P2ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P2ADDR;
// Define the union U_G1P2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2093         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P2STRIDE;

// Define the union U_G1P2DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2094         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P2DFPOS;

// Define the union U_G1P2DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2095         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P2DLPOS;

// Define the union U_G1P2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2096         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P2RESO;

// Define the union U_G1P3ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P3ADDR;
// Define the union U_G1P3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2097         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P3STRIDE;

// Define the union U_G1P3DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2098         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P3DFPOS;

// Define the union U_G1P3DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2099         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P3DLPOS;

// Define the union U_G1P3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2100         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P3RESO;

// Define the union U_G1P4ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P4ADDR;
// Define the union U_G1P4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2101         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P4STRIDE;

// Define the union U_G1P4DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2102         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P4DFPOS;

// Define the union U_G1P4DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2103         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P4DLPOS;

// Define the union U_G1P4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2104         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P4RESO;

// Define the union U_G1P5ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P5ADDR;
// Define the union U_G1P5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2105         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P5STRIDE;

// Define the union U_G1P5DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2106         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P5DFPOS;

// Define the union U_G1P5DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2107         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P5DLPOS;

// Define the union U_G1P5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2108         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P5RESO;

// Define the union U_G1P6ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P6ADDR;
// Define the union U_G1P6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2109         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P6STRIDE;

// Define the union U_G1P6DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2110         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P6DFPOS;

// Define the union U_G1P6DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2111         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P6DLPOS;

// Define the union U_G1P6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2112         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P6RESO;

// Define the union U_G1P7ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1P7ADDR;
// Define the union U_G1P7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2113         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P7STRIDE;

// Define the union U_G1P7DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2114         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P7DFPOS;

// Define the union U_G1P7DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2115         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P7DLPOS;

// Define the union U_G1P7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2116         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1P7RESO;

// Define the union U_G18SLICEEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    p1_en                 : 1   ; // [1]
        HI_U32    p2_en                 : 1   ; // [2]
        HI_U32    p3_en                 : 1   ; // [3]
        HI_U32    p4_en                 : 1   ; // [4]
        HI_U32    p5_en                 : 1   ; // [5]
        HI_U32    p6_en                 : 1   ; // [6]
        HI_U32    p7_en                 : 1   ; // [7]
        HI_U32    Reserved_2117         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G18SLICEEN;

// Define the union U_G1DCMPMODE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2119         : 4   ; // [3..0]
        HI_U32    line_len_idx          : 2   ; // [5..4]
        HI_U32    line_type             : 2   ; // [7..6]
        HI_U32    Reserved_2118         : 23  ; // [30..8]
        HI_U32    dcmp_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1DCMPMODE;

// Define the union U_G1DCMPBANKWIDTH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_bankwidth        : 12  ; // [11..0]
        HI_U32    Reserved_2121         : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1DCMPBANKWIDTH;

// Define the union U_G1DCMPSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_stride           : 16  ; // [15..0]
        HI_U32    Reserved_2122         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1DCMPSTRIDE;

// Define the union U_G1DCMPAADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_a             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1DCMPAADDR;
// Define the union U_G1DCMPRADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_r             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1DCMPRADDR;
// Define the union U_G1DCMPGADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_g             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1DCMPGADDR;
// Define the union U_G1DCMPBADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_b             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1DCMPBADDR;
// Define the union U_G2CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 8   ; // [7..0]
        HI_U32    bitext                : 2   ; // [9..8]
        HI_U32    Reserved_2123         : 4  ; // [13..10]
        HI_U32    key_en                 : 1  ; //[14]
        HI_U32    key_mode                :1  ;//[15]
        HI_U32    Reserved_4001         : 8  ;// [23..16]
        HI_U32    dcmp_inter              :1  ;//[24]
        HI_U32    dcmp_mode             : 1   ; // [25]
        HI_U32    read_mode             : 1   ; // [26]
        HI_U32    upd_mode              : 1   ; // [27]
        HI_U32    mute_en               : 1   ; // [28]
        HI_U32    gmm_en                : 1   ; // [29]
        HI_U32    Reserved_2124         : 1   ; // [30]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CTRL;

// Define the union U_G2UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_2126         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2UPD;

// Define the union U_G2ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2ADDR;
// Define the union U_G2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2127         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2STRIDE;

// Define the union U_G2CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    palpha_range          : 1   ; // [8]
        HI_U32    Reserved_2129         : 1   ; // [9]
        HI_U32    vedge_p               : 1   ; // [10]
        HI_U32    hedge_p               : 1   ; // [11]
        HI_U32    palpha_en             : 1   ; // [12]
        HI_U32    premult_en            : 1   ; // [13]
        HI_U32    key_en                : 1   ; // [14]
        HI_U32    key_mode              : 1   ; // [15]
        HI_U32    Reserved_2128         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CBMPARA;

// Define the union U_G2CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_max              : 8   ; // [7..0]
        HI_U32    keyg_max              : 8   ; // [15..8]
        HI_U32    keyr_max              : 8   ; // [23..16]
        HI_U32    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CKEYMAX;

// Define the union U_G2CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_min              : 8   ; // [7..0]
        HI_U32    keyg_min              : 8   ; // [15..8]
        HI_U32    keyr_min              : 8   ; // [23..16]
        HI_U32    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CKEYMIN;

// Define the union U_G2CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    kmsk_b                : 8   ; // [7..0]
        HI_U32    kmsk_g                : 8   ; // [15..8]
        HI_U32    kmsk_r                : 8   ; // [23..16]
        HI_U32    Reserved_2130         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CMASK;

// Define the union U_G2IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2131         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2IRESO;

// Define the union U_G2ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2132         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2ORESO;

// Define the union U_G2SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    src_xfpos             : 7   ; // [6..0]
        HI_U32    Reserved_2133         : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2SFPOS;

// Define the union U_G2DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_2134         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2DFPOS;

// Define the union U_G2DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_2135         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2DLPOS;

// Define the union U_G2HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_2138         : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_2137         : 3   ; // [22..20]
        HI_U32    hafir_en              : 1   ; // [23]
        HI_U32    hfir_en               : 1   ; // [24]
        HI_U32    Reserved_2136         : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hamid_en              : 1   ; // [30]
        HI_U32    hsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2HSP;

// Define the union U_G2HOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 16  ; // [15..0]
        HI_U32    hor_loffset           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2HOFFSET;

// Define the union U_G2VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2142         : 23  ; // [22..0]
        HI_U32    vafir_en              : 1   ; // [23]
        HI_U32    vfir_en               : 1   ; // [24]
        HI_U32    Reserved_2141         : 1   ; // [25]
        HI_U32    Reserved_2140         : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vamid_en              : 1   ; // [30]
        HI_U32    vsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2VSP;

// Define the union U_G2VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_2143         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2VSR;

// Define the union U_G2VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbtm_offset           : 16  ; // [15..0]
        HI_U32    vtp_offset            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2VOFFSET;

// Define the union U_G2ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2144         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2ZMEORESO;

// Define the union U_G2ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2145         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2ZMEIRESO;

// Define the union U_G2CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_2146         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CSCIDC;

// Define the union U_G2CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc2               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc0               : 9   ; // [26..18]
        HI_U32    Reserved_2148         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CSCODC;

// Define the union U_G2CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2150         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2149         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CSCP0;

// Define the union U_G2CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2152         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2151         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CSCP1;

// Define the union U_G2CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2154         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2153         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CSCP2;

// Define the union U_G2CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2156         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2155         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CSCP3;

// Define the union U_G2CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2157         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2CSCP4;

// Define the union U_G2P0ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P0ADDR;
// Define the union U_G2P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2159         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P0STRIDE;

// Define the union U_G2P0DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2160         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P0DFPOS;

// Define the union U_G2P0DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2161         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P0DLPOS;

// Define the union U_G2P0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2162         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P0RESO;

// Define the union U_G2P1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P1ADDR;
// Define the union U_G2P1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2163         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P1STRIDE;

// Define the union U_G2P1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2164         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P1DFPOS;

// Define the union U_G2P1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2165         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P1DLPOS;

// Define the union U_G2P1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2166         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P1RESO;

// Define the union U_G2P2ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P2ADDR;
// Define the union U_G2P2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2167         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P2STRIDE;

// Define the union U_G2P2DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2168         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P2DFPOS;

// Define the union U_G2P2DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2169         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P2DLPOS;

// Define the union U_G2P2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2170         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P2RESO;

// Define the union U_G2P3ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P3ADDR;
// Define the union U_G2P3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2171         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P3STRIDE;

// Define the union U_G2P3DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2172         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P3DFPOS;

// Define the union U_G2P3DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2173         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P3DLPOS;

// Define the union U_G2P3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2174         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P3RESO;

// Define the union U_G2P4ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P4ADDR;
// Define the union U_G2P4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2175         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P4STRIDE;

// Define the union U_G2P4DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2176         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P4DFPOS;

// Define the union U_G2P4DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2177         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P4DLPOS;

// Define the union U_G2P4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2178         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P4RESO;

// Define the union U_G2P5ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P5ADDR;
// Define the union U_G2P5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2179         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P5STRIDE;

// Define the union U_G2P5DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2180         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P5DFPOS;

// Define the union U_G2P5DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2181         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P5DLPOS;

// Define the union U_G2P5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2182         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P5RESO;

// Define the union U_G2P6ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P6ADDR;
// Define the union U_G2P6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2183         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P6STRIDE;

// Define the union U_G2P6DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2184         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P6DFPOS;

// Define the union U_G2P6DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2185         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P6DLPOS;

// Define the union U_G2P6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2186         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P6RESO;

// Define the union U_G2P7ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2P7ADDR;
// Define the union U_G2P7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2187         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P7STRIDE;

// Define the union U_G2P7DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2188         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P7DFPOS;

// Define the union U_G2P7DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2189         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P7DLPOS;

// Define the union U_G2P7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2190         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2P7RESO;

// Define the union U_G28SLICEEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    p1_en                 : 1   ; // [1]
        HI_U32    p2_en                 : 1   ; // [2]
        HI_U32    p3_en                 : 1   ; // [3]
        HI_U32    p4_en                 : 1   ; // [4]
        HI_U32    p5_en                 : 1   ; // [5]
        HI_U32    p6_en                 : 1   ; // [6]
        HI_U32    p7_en                 : 1   ; // [7]
        HI_U32    Reserved_2191         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G28SLICEEN;

// Define the union U_G2DCMPMODE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2193         : 4   ; // [3..0]
        HI_U32    line_len_idx          : 2   ; // [5..4]
        HI_U32    line_type             : 2   ; // [7..6]
        HI_U32    Reserved_2192         : 23  ; // [30..8]
        HI_U32    dcmp_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2DCMPMODE;

// Define the union U_G2DCMPBANKWIDTH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_bankwidth        : 12  ; // [11..0]
        HI_U32    Reserved_2195         : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2DCMPBANKWIDTH;

// Define the union U_G2DCMPSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_stride           : 16  ; // [15..0]
        HI_U32    Reserved_2196         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G2DCMPSTRIDE;

// Define the union U_G2DCMPAADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_a             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2DCMPAADDR;
// Define the union U_G2DCMPRADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_r             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2DCMPRADDR;
// Define the union U_G2DCMPGADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_g             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2DCMPGADDR;
// Define the union U_G2DCMPBADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_b             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G2DCMPBADDR;
// Define the union U_G3CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 8   ; // [7..0]
        HI_U32    bitext                : 2   ; // [9..8]
        HI_U32    Reserved_2197         : 4  ; // [13..10]
        HI_U32    key_en                 : 1  ; //[14]
        HI_U32    key_mode                :1  ;//[15]
        HI_U32    Reserved_4002         : 8  ;// [23..16]
        HI_U32    dcmp_inter              :1  ;//[24]
        HI_U32    dcmp_mode             : 1   ; // [25]
        HI_U32    read_mode             : 1   ; // [26]
        HI_U32    upd_mode              : 1   ; // [27]
        HI_U32    mute_en               : 1   ; // [28]
        HI_U32    gmm_en                : 1   ; // [29]
        HI_U32    Reserved_2198         : 1   ; // [30]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CTRL;

// Define the union U_G3UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_2200         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3UPD;

// Define the union U_G3ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3ADDR;
// Define the union U_G3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2201         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3STRIDE;

// Define the union U_G3CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    palpha_range          : 1   ; // [8]
        HI_U32    Reserved_2203         : 1   ; // [9]
        HI_U32    vedge_p               : 1   ; // [10]
        HI_U32    hedge_p               : 1   ; // [11]
        HI_U32    palpha_en             : 1   ; // [12]
        HI_U32    premult_en            : 1   ; // [13]
        HI_U32    key_en                : 1   ; // [14]
        HI_U32    key_mode              : 1   ; // [15]
        HI_U32    Reserved_2202         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CBMPARA;

// Define the union U_G3CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_max              : 8   ; // [7..0]
        HI_U32    keyg_max              : 8   ; // [15..8]
        HI_U32    keyr_max              : 8   ; // [23..16]
        HI_U32    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CKEYMAX;

// Define the union U_G3CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_min              : 8   ; // [7..0]
        HI_U32    keyg_min              : 8   ; // [15..8]
        HI_U32    keyr_min              : 8   ; // [23..16]
        HI_U32    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CKEYMIN;

// Define the union U_G3CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    kmsk_b                : 8   ; // [7..0]
        HI_U32    kmsk_g                : 8   ; // [15..8]
        HI_U32    kmsk_r                : 8   ; // [23..16]
        HI_U32    Reserved_2204         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CMASK;

// Define the union U_G3IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2205         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3IRESO;

// Define the union U_G3ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2206         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3ORESO;

// Define the union U_G3SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    src_xfpos             : 7   ; // [6..0]
        HI_U32    Reserved_2207         : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3SFPOS;

// Define the union U_G3DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_2208         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3DFPOS;

// Define the union U_G3DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_2209         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3DLPOS;

// Define the union U_G3HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_2212         : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_2211         : 3   ; // [22..20]
        HI_U32    hafir_en              : 1   ; // [23]
        HI_U32    hfir_en               : 1   ; // [24]
        HI_U32    Reserved_2210         : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hamid_en              : 1   ; // [30]
        HI_U32    hsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3HSP;

// Define the union U_G3HOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 16  ; // [15..0]
        HI_U32    hor_loffset           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3HOFFSET;

// Define the union U_G3VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2216         : 23  ; // [22..0]
        HI_U32    vafir_en              : 1   ; // [23]
        HI_U32    vfir_en               : 1   ; // [24]
        HI_U32    Reserved_2215         : 1   ; // [25]
        HI_U32    Reserved_2214         : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vamid_en              : 1   ; // [30]
        HI_U32    vsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3VSP;

// Define the union U_G3VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_2217         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3VSR;

// Define the union U_G3VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbtm_offset           : 16  ; // [15..0]
        HI_U32    vtp_offset            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3VOFFSET;

// Define the union U_G3ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2218         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3ZMEORESO;

// Define the union U_G3ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2219         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3ZMEIRESO;

// Define the union U_G3CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_2220         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CSCIDC;

// Define the union U_G3CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_2222         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CSCODC;

// Define the union U_G3CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2224         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2223         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CSCP0;

// Define the union U_G3CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2226         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2225         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CSCP1;

// Define the union U_G3CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2228         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2227         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CSCP2;

// Define the union U_G3CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2230         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2229         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CSCP3;

// Define the union U_G3CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2231         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3CSCP4;

// Define the union U_G3P0ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P0ADDR;
// Define the union U_G3P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2233         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P0STRIDE;

// Define the union U_G3P0DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2234         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P0DFPOS;

// Define the union U_G3P0DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2235         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P0DLPOS;

// Define the union U_G3P0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2236         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P0RESO;

// Define the union U_G3P1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P1ADDR;
// Define the union U_G3P1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2237         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P1STRIDE;

// Define the union U_G3P1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2238         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P1DFPOS;

// Define the union U_G3P1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2239         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P1DLPOS;

// Define the union U_G3P1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2240         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P1RESO;

// Define the union U_G3P2ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P2ADDR;
// Define the union U_G3P2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2241         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P2STRIDE;

// Define the union U_G3P2DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2242         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P2DFPOS;

// Define the union U_G3P2DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2243         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P2DLPOS;

// Define the union U_G3P2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2244         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P2RESO;

// Define the union U_G3P3ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P3ADDR;
// Define the union U_G3P3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2245         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P3STRIDE;

// Define the union U_G3P3DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2246         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P3DFPOS;

// Define the union U_G3P3DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2247         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P3DLPOS;

// Define the union U_G3P3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2248         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P3RESO;

// Define the union U_G3P4ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P4ADDR;
// Define the union U_G3P4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2249         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P4STRIDE;

// Define the union U_G3P4DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2250         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P4DFPOS;

// Define the union U_G3P4DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2251         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P4DLPOS;

// Define the union U_G3P4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2252         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P4RESO;

// Define the union U_G3P5ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P5ADDR;
// Define the union U_G3P5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2253         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P5STRIDE;

// Define the union U_G3P5DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2254         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P5DFPOS;

// Define the union U_G3P5DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2255         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P5DLPOS;

// Define the union U_G3P5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2256         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P5RESO;

// Define the union U_G3P6ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P6ADDR;
// Define the union U_G3P6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2257         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P6STRIDE;

// Define the union U_G3P6DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2258         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P6DFPOS;

// Define the union U_G3P6DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2259         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P6DLPOS;

// Define the union U_G3P6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2260         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P6RESO;

// Define the union U_G3P7ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3P7ADDR;
// Define the union U_G3P7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2261         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P7STRIDE;

// Define the union U_G3P7DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2262         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P7DFPOS;

// Define the union U_G3P7DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2263         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P7DLPOS;

// Define the union U_G3P7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2264         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3P7RESO;

// Define the union U_G38SLICEEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    p1_en                 : 1   ; // [1]
        HI_U32    p2_en                 : 1   ; // [2]
        HI_U32    p3_en                 : 1   ; // [3]
        HI_U32    p4_en                 : 1   ; // [4]
        HI_U32    p5_en                 : 1   ; // [5]
        HI_U32    p6_en                 : 1   ; // [6]
        HI_U32    p7_en                 : 1   ; // [7]
        HI_U32    Reserved_2265         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G38SLICEEN;

// Define the union U_G3DCMPMODE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2267         : 4   ; // [3..0]
        HI_U32    line_len_idx          : 2   ; // [5..4]
        HI_U32    line_type             : 2   ; // [7..6]
        HI_U32    Reserved_2266         : 23  ; // [30..8]
        HI_U32    dcmp_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3DCMPMODE;

// Define the union U_G3DCMPBANKWIDTH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_bankwidth        : 12  ; // [11..0]
        HI_U32    Reserved_2269         : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3DCMPBANKWIDTH;

// Define the union U_G3DCMPSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_stride           : 16  ; // [15..0]
        HI_U32    Reserved_2270         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G3DCMPSTRIDE;

// Define the union U_G3DCMPAADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_a             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3DCMPAADDR;
// Define the union U_G3DCMPRADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_r             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3DCMPRADDR;
// Define the union U_G3DCMPGADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_g             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3DCMPGADDR;
// Define the union U_G3DCMPBADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_b             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G3DCMPBADDR;
// Define the union U_G4CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 8   ; // [7..0]
        HI_U32    bitext                : 2   ; // [9..8]
        HI_U32    Reserved_2272         : 15  ; // [24..10]
        HI_U32    dcmp_mode             : 1   ; // [25]
        HI_U32    read_mode             : 1   ; // [26]
        HI_U32    upd_mode              : 1   ; // [27]
        HI_U32    mute_en               : 1   ; // [28]
        HI_U32    gmm_en                : 1   ; // [29]
        HI_U32    Reserved_2271         : 1   ; // [30]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CTRL;

// Define the union U_G4UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_2274         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4UPD;

// Define the union U_G4ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4ADDR;
// Define the union U_G4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2275         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4STRIDE;

// Define the union U_G4CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    palpha_range          : 1   ; // [8]
        HI_U32    Reserved_2277         : 1   ; // [9]
        HI_U32    vedge_p               : 1   ; // [10]
        HI_U32    hedge_p               : 1   ; // [11]
        HI_U32    palpha_en             : 1   ; // [12]
        HI_U32    premult_en            : 1   ; // [13]
        HI_U32    key_en                : 1   ; // [14]
        HI_U32    key_mode              : 1   ; // [15]
        HI_U32    Reserved_2276         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CBMPARA;

// Define the union U_G4CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_max              : 8   ; // [7..0]
        HI_U32    keyg_max              : 8   ; // [15..8]
        HI_U32    keyr_max              : 8   ; // [23..16]
        HI_U32    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CKEYMAX;

// Define the union U_G4CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_min              : 8   ; // [7..0]
        HI_U32    keyg_min              : 8   ; // [15..8]
        HI_U32    keyr_min              : 8   ; // [23..16]
        HI_U32    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CKEYMIN;

// Define the union U_G4CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    kmsk_b                : 8   ; // [7..0]
        HI_U32    kmsk_g                : 8   ; // [15..8]
        HI_U32    kmsk_r                : 8   ; // [23..16]
        HI_U32    Reserved_2278         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CMASK;

// Define the union U_G4IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2279         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4IRESO;

// Define the union U_G4ORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2280         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4ORESO;

// Define the union U_G4SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    src_xfpos             : 7   ; // [6..0]
        HI_U32    Reserved_2281         : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4SFPOS;

// Define the union U_G4DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_2282         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4DFPOS;

// Define the union U_G4DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_2283         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4DLPOS;

// Define the union U_G4HSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hratio                : 16  ; // [15..0]
        HI_U32    Reserved_2286         : 3   ; // [18..16]
        HI_U32    hfir_order            : 1   ; // [19]
        HI_U32    Reserved_2285         : 3   ; // [22..20]
        HI_U32    hafir_en              : 1   ; // [23]
        HI_U32    hfir_en               : 1   ; // [24]
        HI_U32    Reserved_2284         : 3   ; // [27..25]
        HI_U32    hchmid_en             : 1   ; // [28]
        HI_U32    hlmid_en              : 1   ; // [29]
        HI_U32    hamid_en              : 1   ; // [30]
        HI_U32    hsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4HSP;

// Define the union U_G4HOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hor_coffset           : 16  ; // [15..0]
        HI_U32    hor_loffset           : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4HOFFSET;

// Define the union U_G4VSP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2290         : 23  ; // [22..0]
        HI_U32    vafir_en              : 1   ; // [23]
        HI_U32    vfir_en               : 1   ; // [24]
        HI_U32    Reserved_2289         : 1   ; // [25]
        HI_U32    Reserved_2288         : 1   ; // [26]
        HI_U32    vsc_luma_tap          : 1   ; // [27]
        HI_U32    vchmid_en             : 1   ; // [28]
        HI_U32    vlmid_en              : 1   ; // [29]
        HI_U32    vamid_en              : 1   ; // [30]
        HI_U32    vsc_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4VSP;

// Define the union U_G4VSR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vratio                : 16  ; // [15..0]
        HI_U32    Reserved_2291         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4VSR;

// Define the union U_G4VOFFSET
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vbtm_offset           : 16  ; // [15..0]
        HI_U32    vtp_offset            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4VOFFSET;

// Define the union U_G4ZMEORESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ow                    : 12  ; // [11..0]
        HI_U32    oh                    : 12  ; // [23..12]
        HI_U32    Reserved_2292         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4ZMEORESO;

// Define the union U_G4ZMEIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2293         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4ZMEIRESO;

// Define the union U_G4CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_2294         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CSCIDC;

// Define the union U_G4CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_2296         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CSCODC;

// Define the union U_G4CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2298         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2297         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CSCP0;

// Define the union U_G4CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2300         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2299         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CSCP1;

// Define the union U_G4CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2302         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2301         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CSCP2;

// Define the union U_G4CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2304         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2303         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CSCP3;

// Define the union U_G4CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2305         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4CSCP4;

// Define the union U_G4P0ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P0ADDR;
// Define the union U_G4P0STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2307         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P0STRIDE;

// Define the union U_G4P0DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2308         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P0DFPOS;

// Define the union U_G4P0DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2309         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P0DLPOS;

// Define the union U_G4P0RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2310         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P0RESO;

// Define the union U_G4P1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P1ADDR;
// Define the union U_G4P1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2311         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P1STRIDE;

// Define the union U_G4P1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2312         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P1DFPOS;

// Define the union U_G4P1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2313         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P1DLPOS;

// Define the union U_G4P1RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2314         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P1RESO;

// Define the union U_G4P2ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P2ADDR;
// Define the union U_G4P2STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2315         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P2STRIDE;

// Define the union U_G4P2DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2316         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P2DFPOS;

// Define the union U_G4P2DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2317         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P2DLPOS;

// Define the union U_G4P2RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2318         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P2RESO;

// Define the union U_G4P3ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P3ADDR;
// Define the union U_G4P3STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2319         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P3STRIDE;

// Define the union U_G4P3DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2320         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P3DFPOS;

// Define the union U_G4P3DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2321         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P3DLPOS;

// Define the union U_G4P3RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2322         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P3RESO;

// Define the union U_G4P4ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P4ADDR;
// Define the union U_G4P4STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2323         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P4STRIDE;

// Define the union U_G4P4DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2324         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P4DFPOS;

// Define the union U_G4P4DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2325         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P4DLPOS;

// Define the union U_G4P4RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2326         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P4RESO;

// Define the union U_G4P5ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P5ADDR;
// Define the union U_G4P5STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2327         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P5STRIDE;

// Define the union U_G4P5DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2328         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P5DFPOS;

// Define the union U_G4P5DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2329         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P5DLPOS;

// Define the union U_G4P5RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2330         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P5RESO;

// Define the union U_G4P6ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P6ADDR;
// Define the union U_G4P6STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2331         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P6STRIDE;

// Define the union U_G4P6DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2332         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P6DFPOS;

// Define the union U_G4P6DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2333         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P6DLPOS;

// Define the union U_G4P6RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2334         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P6RESO;

// Define the union U_G4P7ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4P7ADDR;
// Define the union U_G4P7STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2335         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P7STRIDE;

// Define the union U_G4P7DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xfpos                 : 12  ; // [11..0]
        HI_U32    yfpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2336         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P7DFPOS;

// Define the union U_G4P7DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    xlpos                 : 12  ; // [11..0]
        HI_U32    ylpos                 : 12  ; // [23..12]
        HI_U32    Reserved_2337         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P7DLPOS;

// Define the union U_G4P7RESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    w                     : 12  ; // [11..0]
        HI_U32    h                     : 12  ; // [23..12]
        HI_U32    Reserved_2338         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4P7RESO;

// Define the union U_G48SLICEEN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    p0_en                 : 1   ; // [0]
        HI_U32    p1_en                 : 1   ; // [1]
        HI_U32    p2_en                 : 1   ; // [2]
        HI_U32    p3_en                 : 1   ; // [3]
        HI_U32    p4_en                 : 1   ; // [4]
        HI_U32    p5_en                 : 1   ; // [5]
        HI_U32    p6_en                 : 1   ; // [6]
        HI_U32    p7_en                 : 1   ; // [7]
        HI_U32    Reserved_2339         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G48SLICEEN;

// Define the union U_G4DCMPMODE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_2341         : 4   ; // [3..0]
        HI_U32    line_len_idx          : 2   ; // [5..4]
        HI_U32    line_type             : 2   ; // [7..6]
        HI_U32    Reserved_2340         : 23  ; // [30..8]
        HI_U32    dcmp_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4DCMPMODE;

// Define the union U_G4DCMPBANKWIDTH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_bankwidth        : 12  ; // [11..0]
        HI_U32    Reserved_2343         : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4DCMPBANKWIDTH;

// Define the union U_G4DCMPSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dcmp_stride           : 16  ; // [15..0]
        HI_U32    Reserved_2344         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G4DCMPSTRIDE;

// Define the union U_G4DCMPAADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_a             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4DCMPAADDR;
// Define the union U_G4DCMPRADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_r             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4DCMPRADDR;
// Define the union U_G4DCMPGADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_g             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4DCMPGADDR;
// Define the union U_G4DCMPBADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 dcmp_addr_b             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G4DCMPBADDR;
// Define the union U_HCCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 8   ; // [7..0]
        HI_U32    bitext                : 2   ; // [9..8]
        HI_U32    Reserved_2346         : 16  ; // [25..10]
        HI_U32    read_mode             : 1   ; // [26]
        HI_U32    upd_mode              : 1   ; // [27]
        HI_U32    Reserved_2345         : 3   ; // [30..28]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCTRL;

// Define the union U_HCUPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_2348         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCUPD;

// Define the union U_HCADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_HCADDR;
// Define the union U_HCSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2349         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCSTRIDE;

// Define the union U_HCCBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    palpha_range          : 1   ; // [8]
        HI_U32    Reserved_2352         : 3   ; // [11..9]
        HI_U32    palpha_en             : 1   ; // [12]
        HI_U32    Reserved_2351         : 1   ; // [13]
        HI_U32    key_en                : 1   ; // [14]
        HI_U32    key_mode              : 1   ; // [15]
        HI_U32    Reserved_2350         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCBMPARA;

// Define the union U_HCCKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_max              : 8   ; // [7..0]
        HI_U32    keyg_max              : 8   ; // [15..8]
        HI_U32    keyr_max              : 8   ; // [23..16]
        HI_U32    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCKEYMAX;

// Define the union U_HCCKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_min              : 8   ; // [7..0]
        HI_U32    keyg_min              : 8   ; // [15..8]
        HI_U32    keyr_min              : 8   ; // [23..16]
        HI_U32    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCKEYMIN;

// Define the union U_HCCMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    kmsk_b                : 8   ; // [7..0]
        HI_U32    kmsk_g                : 8   ; // [15..8]
        HI_U32    kmsk_r                : 8   ; // [23..16]
        HI_U32    Reserved_2353         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCMASK;

// Define the union U_HCIRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2354         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCIRESO;

// Define the union U_HCSFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    src_xfpos             : 7   ; // [6..0]
        HI_U32    Reserved_2355         : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCSFPOS;

// Define the union U_HCDFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_2357         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCDFPOS;

// Define the union U_HCDLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_2358         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCDLPOS;

// Define the union U_HCCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    csc_mode              : 1   ; // [28]     /* 鼠标层有csc_mode，它是通过G0CSCIDC操作的，故在这加上这成员 */
        HI_U32    Reserved_2359         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCSCIDC;

// Define the union U_HCCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_2361         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCSCODC;

// Define the union U_HCCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2363         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2362         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCSCP0;

// Define the union U_HCCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2365         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2364         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCSCP1;

// Define the union U_HCCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2367         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2366         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCSCP2;

// Define the union U_HCCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2369         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2368         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCSCP3;

// Define the union U_HCCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2370         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HCCSCP4;

// Define the union U_HC1CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    ifmt                  : 8   ; // [7..0]
        HI_U32    bitext                : 2   ; // [9..8]
        HI_U32    Reserved_2372         : 16  ; // [25..10]
        HI_U32    read_mode             : 1   ; // [26]
        HI_U32    upd_mode              : 1   ; // [27]
        HI_U32    Reserved_2371         : 3   ; // [30..28]
        HI_U32    surface_en            : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CTRL;

// Define the union U_HC1UPD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    regup                 : 1   ; // [0]
        HI_U32    Reserved_2374         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1UPD;

// Define the union U_HC1ADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 surface_addr            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_HC1ADDR;
// Define the union U_HC1STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    surface_stride        : 16  ; // [15..0]
        HI_U32    Reserved_2375         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1STRIDE;

// Define the union U_HC1CBMPARA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    galpha                : 8   ; // [7..0]
        HI_U32    palpha_range          : 1   ; // [8]
        HI_U32    Reserved_2378         : 3   ; // [11..9]
        HI_U32    palpha_en             : 1   ; // [12]
        HI_U32    Reserved_2377         : 1   ; // [13]
        HI_U32    key_en                : 1   ; // [14]
        HI_U32    key_mode              : 1   ; // [15]
        HI_U32    Reserved_2376         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CBMPARA;

// Define the union U_HC1CKEYMAX
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_max              : 8   ; // [7..0]
        HI_U32    keyg_max              : 8   ; // [15..8]
        HI_U32    keyr_max              : 8   ; // [23..16]
        HI_U32    va0                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CKEYMAX;

// Define the union U_HC1CKEYMIN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    keyb_min              : 8   ; // [7..0]
        HI_U32    keyg_min              : 8   ; // [15..8]
        HI_U32    keyr_min              : 8   ; // [23..16]
        HI_U32    va1                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CKEYMIN;

// Define the union U_HC1CMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    kmsk_b                : 8   ; // [7..0]
        HI_U32    kmsk_g                : 8   ; // [15..8]
        HI_U32    kmsk_r                : 8   ; // [23..16]
        HI_U32    Reserved_2379         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CMASK;

// Define the union U_HC1IRESO
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    iw                    : 12  ; // [11..0]
        HI_U32    ih                    : 12  ; // [23..12]
        HI_U32    Reserved_2380         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1IRESO;

// Define the union U_HC1SFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    src_xfpos             : 7   ; // [6..0]
        HI_U32    Reserved_2381         : 25  ; // [31..7]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1SFPOS;

// Define the union U_HC1DFPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xfpos            : 12  ; // [11..0]
        HI_U32    disp_yfpos            : 12  ; // [23..12]
        HI_U32    Reserved_2383         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1DFPOS;

// Define the union U_HC1DLPOS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    disp_xlpos            : 12  ; // [11..0]
        HI_U32    disp_ylpos            : 12  ; // [23..12]
        HI_U32    Reserved_2384         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1DLPOS;

// Define the union U_HC1CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_2385         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CSCIDC;

// Define the union U_HC1CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_2387         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CSCODC;

// Define the union U_HC1CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2389         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2388         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CSCP0;

// Define the union U_HC1CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2391         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2390         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CSCP1;

// Define the union U_HC1CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2393         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2392         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CSCP2;

// Define the union U_HC1CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2395         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2394         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CSCP3;

// Define the union U_HC1CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2396         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_HC1CSCP4;

// Define the union U_CBMBKG1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cbm_bkgcr             : 8   ; // [7..0]
        HI_U32    cbm_bkgcb             : 8   ; // [15..8]
        HI_U32    cbm_bkgy              : 8   ; // [23..16]
        HI_U32    Reserved_2397         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_CBMBKG1;

// Define the union U_CBMBKG3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cbm_bkgcr             : 8   ; // [7..0]
        HI_U32    cbm_bkgcb             : 8   ; // [15..8]
        HI_U32    cbm_bkgy              : 8   ; // [23..16]
        HI_U32    Reserved_2400         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_CBMBKG3;

// Define the union U_CBMBKG4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cbm_bkgcr             : 8   ; // [7..0]
        HI_U32    cbm_bkgcb             : 8   ; // [15..8]
        HI_U32    cbm_bkgy              : 8   ; // [23..16]
        HI_U32    Reserved_2401         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_CBMBKG4;

// Define the union U_CBMATTR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    sur_attr0             : 4   ; // [3..0]
        HI_U32    sur_attr1             : 4   ; // [7..4]
        HI_U32    Reserved_2402         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_CBMATTR;

// Define the union U_CBMMIX1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    mixer_prio0           : 4   ; // [3..0]
        HI_U32    mixer_prio1           : 4   ; // [7..4]
        HI_U32    mixer_prio2           : 4   ; // [11..8]
        HI_U32    mixer_prio3           : 4   ; // [15..12]
        HI_U32    Reserved_2403         : 16   ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_CBMMIX1;

// Define the union U_CBMMIX3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    mixer_prio0           : 4   ; // [3..0]
        HI_U32    mixer_prio1           : 4   ; // [7..4]
        HI_U32    mixer_prio2           : 4   ; // [11..8]
        HI_U32    Reserved_2405         : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_CBMMIX3;

// Define the union U_CBMMIX4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    mixer_prio0           : 4   ; // [3..0]
        HI_U32    mixer_prio1           : 4   ; // [7..4]
        HI_U32    mixer_prio2           : 4   ; // [11..8]
        HI_U32    Reserved_2406         : 20  ; // [31..12]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_CBMMIX4;

// Define the union U_DHDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    intfdm                : 4   ; // [3..0]
        HI_U32    intfb                 : 2   ; // [5..4]
        HI_U32    synm                  : 1   ; // [6]
        HI_U32    iop                   : 1   ; // [7]
        HI_U32    ivs                   : 1   ; // [8]
        HI_U32    ihs                   : 1   ; // [9]
        HI_U32    idv                   : 1   ; // [10]
        HI_U32    Reserved_2409         : 2   ; // [12..11]
        HI_U32    gmmmode               : 1   ; // [13]
        HI_U32    gmmen                 : 1   ; // [14]
        HI_U32    Reserved_2408         : 1   ; // [15]
        HI_U32    clipen                : 1   ; // [16]
        HI_U32    Reserved_2407         : 3   ; // [19..17]
        HI_U32    fpga_lmt_width        : 7   ; // [26..20]
        HI_U32    fpga_lmt_en           : 1   ; // [27]
        HI_U32    Reserved_2410         : 3   ; // [28..30]
        HI_U32    intf_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDCTRL;

// Define the union U_DHDVSYNC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vact                  : 12  ; // [11..0]
        HI_U32    vbb                   : 8   ; // [19..12]
        HI_U32    vfb                   : 8   ; // [27..20]
        HI_U32    Reserved_2411         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDVSYNC;

// Define the union U_DHDHSYNC1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hact                  : 16  ; // [15..0]
        HI_U32    hbb                   : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDHSYNC1;

// Define the union U_DHDHSYNC2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hfb                   : 16  ; // [15..0]
        HI_U32    hmid                  : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDHSYNC2;

// Define the union U_DHDVPLUS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    bvact                 : 12  ; // [11..0]
        HI_U32    bvbb                  : 8   ; // [19..12]
        HI_U32    bvfb                  : 8   ; // [27..20]
        HI_U32    Reserved_2412         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDVPLUS;

// Define the union U_DHDPWR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hpw                   : 16  ; // [15..0]
        HI_U32    vpw                   : 8   ; // [23..16]
        HI_U32    Reserved_2413         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDPWR;

// Define the union U_DHDVTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vtmgthd1              : 13  ; // [12..0]
        HI_U32    Reserved_2417         : 2   ; // [14..13]
        HI_U32    thd1_mode             : 1   ; // [15]
        HI_U32    Reserved_2416         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDVTTHD;

// Define the union U_DHDCLIPL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    clipcl0               : 10  ; // [9..0]
        HI_U32    clipcl1               : 10  ; // [19..10]
        HI_U32    clipcl2               : 10  ; // [29..20]
        HI_U32    dfir_en               : 1   ; // [30]
        HI_U32    clipen                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDCLIPL;

// Define the union U_DHDCLIPH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    clipch0               : 10  ; // [9..0]
        HI_U32    clipch1               : 10  ; // [19..10]
        HI_U32    clipch2               : 10  ; // [29..20]
        HI_U32    Reserved_2430         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDCLIPH;


// Define the union U_DHDVGACLIPL
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clipcl0          : 10   ; // [9..0]
        unsigned int    clipcl1          : 10   ; // [19..10]
        unsigned int    clipcl2          : 10   ; // [29..20]
        unsigned int    reserved         : 1   ; // [30]
        unsigned int    clipen           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDVGACLIPL;


// Define the union U_DHDVGACLIPH
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    clipch0          : 10   ; // [9..0]
        unsigned int    clipch1          : 10   ; // [19..10]
        unsigned int    clipch2          : 10   ; // [29..20]
        unsigned int    reserved         : 2   ; // [31..30]
          
     } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDVGACLIPH;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf0_tmp0          : 8   ; // [7..0]
        unsigned int    hsp_hf0_tmp1          : 8   ; // [15..8]
        unsigned int    hsp_hf0_tmp2          : 8   ; // [23..16]
        unsigned int    hsp_hf0_tmp3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG0;

// Define the union U_DHDHSPCFG1
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf0_coring        : 8   ; // [7..0]
        unsigned int    Reserved_483          : 23  ; // [30..8]
        unsigned int    hsp_en                 : 1  ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG1;

// Define the union U_DHDHSPCFG5
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf0_gainpos       : 11  ; // [10..0]
        unsigned int    Reserved_485          : 5   ; // [15..11]
        unsigned int    hsp_hf0_gainneg       : 11  ; // [26..16]
        unsigned int    Reserved_484          : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG5;

// Define the union U_DHDHSPCFG6
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf0_overth        : 8   ; // [7..0]
        unsigned int    hsp_hf0_underth       : 8   ; // [15..8]
        unsigned int    hsp_hf0_mixratio      : 8   ; // [23..16]
        unsigned int    Reserved_487          : 4   ; // [27..24]
        unsigned int    hsp_hf0_winsize       : 3   ; // [30..28]
        unsigned int    hsp_hf0_adpshoot_en   : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG6;

// Define the union U_DHDHSPCFG7
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf1_tmp0          : 8   ; // [7..0]
        unsigned int    hsp_hf1_tmp1          : 8   ; // [15..8]
        unsigned int    hsp_hf1_tmp2          : 8   ; // [23..16]
        unsigned int    hsp_hf1_tmp3          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG7;

// Define the union U_DHDHSPCFG8
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf1_coring        : 8   ; // [7..0]
        unsigned int    Reserved_488          : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG8;

// Define the union U_DHDHSPCFG12
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf1_gainpos       : 11  ; // [10..0]
        unsigned int    Reserved_490          : 5   ; // [15..11]
        unsigned int    hsp_hf1_gainneg       : 11  ; // [26..16]
        unsigned int    Reserved_489          : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG12;

// Define the union U_DHDHSPCFG13
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_hf1_overth        : 8   ; // [7..0]
        unsigned int    hsp_hf1_underth       : 8   ; // [15..8]
        unsigned int    hsp_hf1_mixratio      : 8   ; // [23..16]
        unsigned int    Reserved_492          : 4   ; // [27..24]
        unsigned int    hsp_hf1_winsize       : 3   ; // [30..28]
        unsigned int    hsp_hf1_adpshoot_en   : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG13;

// Define the union U_DHDHSPCFG14
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_cdti_gain         : 8   ; // [7..0]
        unsigned int    hsp_ldti_gain         : 8   ; // [15..8]
        unsigned int    hsp_lti_ratio         : 8   ; // [23..16]
        unsigned int    hsp_hf_shootdiv       : 3   ; // [26..24]
        unsigned int    Reserved_493          : 1   ; // [27]
        unsigned int    hsp_ctih_en           : 1   ; // [28]
        unsigned int    hsp_ltih_en           : 1   ; // [29]
        unsigned int    hsp_h1_en             : 1   ; // [30]
        unsigned int    hsp_h0_en             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG14;

// Define the union U_DHDHSPCFG15
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    hsp_glb_underth       : 9   ; // [8..0]
        unsigned int    Reserved_496          : 1   ; // [9]
        unsigned int    hsp_glb_overth        : 9   ; // [18..10]
        unsigned int    Reserved_495          : 1   ; // [19]
        unsigned int    hsp_peak_ratio        : 8   ; // [27..20]
        unsigned int    Reserved_494          : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_DHDHSPCFG15;


// Define the union U_DHDSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vback_blank           : 1   ; // [0]
        HI_U32    vblank                : 1   ; // [1]
        HI_U32    bottom_field          : 1   ; // [2]
        HI_U32    Reserved_2436         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDSTATE;

// Define the union U_DSDCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    intfdm                : 4   ; // [3..0]
        HI_U32    intfb                 : 2   ; // [5..4]
        HI_U32    synm                  : 1   ; // [6]
        HI_U32    iop                   : 1   ; // [7]
        HI_U32    ivs                   : 1   ; // [8]
        HI_U32    ihs                   : 1   ; // [9]
        HI_U32    idv                   : 1   ; // [10]
        HI_U32    Reserved_2727         : 20  ; // [30..11]
        HI_U32    intf_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCTRL;

// Define the union U_DSDVSYNC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vact                  : 12  ; // [11..0]
        HI_U32    vbb                   : 8   ; // [19..12]
        HI_U32    vfb                   : 8   ; // [27..20]
        HI_U32    Reserved_2729         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDVSYNC;

// Define the union U_DSDHSYNC1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hact                  : 16  ; // [15..0]
        HI_U32    hbb                   : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDHSYNC1;

// Define the union U_DSDHSYNC2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hfb                   : 16  ; // [15..0]
        HI_U32    Reserved_2730         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDHSYNC2;

// Define the union U_DSDVPLUS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    bvact                 : 12  ; // [11..0]
        HI_U32    bvbb                  : 8   ; // [19..12]
        HI_U32    bvfb                  : 8   ; // [27..20]
        HI_U32    Reserved_2731         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDVPLUS;

// Define the union U_DSDPWR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hpw                   : 16  ; // [15..0]
        HI_U32    vpw                   : 8   ; // [23..16]
        HI_U32    Reserved_2732         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDPWR;

// Define the union U_DSDVTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vtmgthd1              : 13  ; // [12..0]
        HI_U32    Reserved_2736         : 2   ; // [14..13]
        HI_U32    thd1_mode             : 1   ; // [15]
        HI_U32    Reserved_2735         : 16   ; // [30..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDVTTHD;

// Define the union U_DSDCSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_2737         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCSCIDC;

// Define the union U_DSDCSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_2738         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCSCODC;

// Define the union U_DSDCSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2740         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2739         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCSCP0;

// Define the union U_DSDCSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2742         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2741         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCSCP1;

// Define the union U_DSDCSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2744         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2743         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCSCP2;

// Define the union U_DSDCSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2746         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2745         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCSCP3;

// Define the union U_DSDCSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2747         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCSCP4;

// Define the union U_DSDCLIPL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    clipcl0               : 10  ; // [9..0]
        HI_U32    clipcl1               : 10  ; // [19..10]
        HI_U32    clipcl2               : 10  ; // [29..20]
        HI_U32    dfir_en               : 1   ; // [30]
        HI_U32    clipen                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCLIPL;

// Define the union U_DSDCLIPH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    clipch0               : 10  ; // [9..0]
        HI_U32    clipch1               : 10  ; // [19..10]
        HI_U32    clipch2               : 10  ; // [29..20]
        HI_U32    Reserved_2749         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDCLIPH;

// Define the union U_DSDSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vback_blank           : 1   ; // [0]
        HI_U32    vblank                : 1   ; // [1]
        HI_U32    bottom_field          : 1   ; // [2]
        HI_U32    Reserved_2750         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSDSTATE;

// Define the union U_DSD1CTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    intfdm                : 4   ; // [3..0]
        HI_U32    intfb                 : 2   ; // [5..4]
        HI_U32    synm                  : 1   ; // [6]
        HI_U32    iop                   : 1   ; // [7]
        HI_U32    ivs                   : 1   ; // [8]
        HI_U32    ihs                   : 1   ; // [9]
        HI_U32    idv                   : 1   ; // [10]
        HI_U32    Reserved_2752         : 20  ; // [30..11]
        HI_U32    intf_en               : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CTRL;

// Define the union U_DSD1VSYNC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vact                  : 12  ; // [11..0]
        HI_U32    vbb                   : 8   ; // [19..12]
        HI_U32    vfb                   : 8   ; // [27..20]
        HI_U32    Reserved_2754         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1VSYNC;

// Define the union U_DSD1HSYNC1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hact                  : 16  ; // [15..0]
        HI_U32    hbb                   : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1HSYNC1;

// Define the union U_DSD1HSYNC2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hfb                   : 16  ; // [15..0]
        HI_U32    Reserved_2755         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1HSYNC2;

// Define the union U_DSD1VPLUS
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    bvact                 : 12  ; // [11..0]
        HI_U32    bvbb                  : 8   ; // [19..12]
        HI_U32    bvfb                  : 8   ; // [27..20]
        HI_U32    Reserved_2756         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1VPLUS;

// Define the union U_DSD1PWR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hpw                   : 16  ; // [15..0]
        HI_U32    vpw                   : 8   ; // [23..16]
        HI_U32    Reserved_2757         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1PWR;

// Define the union U_DSD1VTTHD
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vtmgthd1              : 13  ; // [12..0]
        HI_U32    Reserved_2761         : 2   ; // [14..13]
        HI_U32    thd1_mode             : 1   ; // [15]
        HI_U32    Reserved_2760         : 16   ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1VTTHD;

// Define the union U_DSD1CSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc0               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc2               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    Reserved_2762         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CSCIDC;

// Define the union U_DSD1CSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_2763         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CSCODC;

// Define the union U_DSD1CSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_2765         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_2764         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CSCP0;

// Define the union U_DSD1CSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_2767         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_2766         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CSCP1;

// Define the union U_DSD1CSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_2769         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_2768         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CSCP2;

// Define the union U_DSD1CSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_2771         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_2770         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CSCP3;

// Define the union U_DSD1CSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_2772         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CSCP4;

// Define the union U_DSD1CLIPL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    clipcl0               : 10  ; // [9..0]
        HI_U32    clipcl1               : 10  ; // [19..10]
        HI_U32    clipcl2               : 10  ; // [29..20]
        HI_U32    dfir_en               : 1   ; // [30]
        HI_U32    clipen                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CLIPL;

// Define the union U_DSD1CLIPH
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    clipch0               : 10  ; // [9..0]
        HI_U32    clipch1               : 10  ; // [19..10]
        HI_U32    clipch2               : 10  ; // [29..20]
        HI_U32    Reserved_2774         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1CLIPH;

// Define the union U_DSD1STATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vback_blank           : 1   ; // [0]
        HI_U32    vblank                : 1   ; // [1]
        HI_U32    bottom_field          : 1   ; // [2]
        HI_U32    Reserved_2775         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DSD1STATE;

// Define the union U_DATE_COEFF0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_seq                : 1   ; // [0]
        HI_U32    chgain_en             : 1   ; // [1]
        HI_U32    sylp_en               : 1   ; // [2]
        HI_U32    chlp_en               : 1   ; // [3]
        HI_U32    oversam2_en           : 1   ; // [4]
        HI_U32    lunt_en               : 1   ; // [5]
        HI_U32    oversam_en            : 2   ; // [7..6]
        HI_U32    Reserved_2952         : 1   ; // [8]
        HI_U32    luma_dl               : 4   ; // [12..9]
        HI_U32    agc_amp_sel           : 1   ; // [13]
        HI_U32    length_sel            : 1   ; // [14]
        HI_U32    sync_mode_scart       : 1   ; // [15]
        HI_U32    sync_mode_sel         : 2   ; // [17..16]
        HI_U32    style_sel             : 4   ; // [21..18]
        HI_U32    fm_sel                : 1   ; // [22]
        HI_U32    vbi_lpf_en            : 1   ; // [23]
        HI_U32    rgb_en                : 1   ; // [24]
        HI_U32    scanline              : 1   ; // [25]
        HI_U32    Reserved_2951         : 3   ; // [28..26]
        HI_U32    dis_ire               : 1   ; // [29]
        HI_U32    clpf_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF0;

// Define the union U_DATE_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac_test              : 10  ; // [9..0]
        HI_U32    date_test_mode        : 2   ; // [11..10]
        HI_U32    date_test_en          : 1   ; // [12]
        HI_U32    amp_outside           : 10  ; // [22..13]
        HI_U32    c_limit_en            : 1   ; // [23]
        HI_U32    cc_seq                : 1   ; // [24]
        HI_U32    cgms_seq              : 1   ; // [25]
        HI_U32    vps_seq               : 1   ; // [26]
        HI_U32    wss_seq               : 1   ; // [27]
        HI_U32    cvbs_limit_en         : 1   ; // [28]
        HI_U32    c_gain                : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF1;

// Define the union U_DATE_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt07_enf2             : 1   ; // [0]
        HI_U32    tt08_enf2             : 1   ; // [1]
        HI_U32    tt09_enf2             : 1   ; // [2]
        HI_U32    tt10_enf2             : 1   ; // [3]
        HI_U32    tt11_enf2             : 1   ; // [4]
        HI_U32    tt12_enf2             : 1   ; // [5]
        HI_U32    tt13_enf2             : 1   ; // [6]
        HI_U32    tt14_enf2             : 1   ; // [7]
        HI_U32    tt15_enf2             : 1   ; // [8]
        HI_U32    tt16_enf2             : 1   ; // [9]
        HI_U32    tt17_enf2             : 1   ; // [10]
        HI_U32    tt18_enf2             : 1   ; // [11]
        HI_U32    tt19_enf2             : 1   ; // [12]
        HI_U32    tt20_enf2             : 1   ; // [13]
        HI_U32    tt21_enf2             : 1   ; // [14]
        HI_U32    tt22_enf2             : 1   ; // [15]
        HI_U32    tt07_enf1             : 1   ; // [16]
        HI_U32    tt08_enf1             : 1   ; // [17]
        HI_U32    tt09_enf1             : 1   ; // [18]
        HI_U32    tt10_enf1             : 1   ; // [19]
        HI_U32    tt11_enf1             : 1   ; // [20]
        HI_U32    tt12_enf1             : 1   ; // [21]
        HI_U32    tt13_enf1             : 1   ; // [22]
        HI_U32    tt14_enf1             : 1   ; // [23]
        HI_U32    tt15_enf1             : 1   ; // [24]
        HI_U32    tt16_enf1             : 1   ; // [25]
        HI_U32    tt17_enf1             : 1   ; // [26]
        HI_U32    tt18_enf1             : 1   ; // [27]
        HI_U32    tt19_enf1             : 1   ; // [28]
        HI_U32    tt20_enf1             : 1   ; // [29]
        HI_U32    tt21_enf1             : 1   ; // [30]
        HI_U32    tt22_enf1             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF7;

// Define the union U_DATE_COEFF8
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_staddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE_COEFF8;
// Define the union U_DATE_COEFF9
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_edaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE_COEFF9;
// Define the union U_DATE_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_pktoff             : 8   ; // [7..0]
        HI_U32    tt_mode               : 2   ; // [9..8]
        HI_U32    tt_highest            : 1   ; // [10]
        HI_U32    full_page             : 1   ; // [11]
        HI_U32    nabts_100ire          : 1   ; // [12]
        HI_U32    Reserved_2955         : 18  ; // [30..13]
        HI_U32    tt_ready              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF10;

// Define the union U_DATE_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    date_clf2             : 10  ; // [9..0]
        HI_U32    date_clf1             : 10  ; // [19..10]
        HI_U32    cc_enf2               : 1   ; // [20]
        HI_U32    cc_enf1               : 1   ; // [21]
        HI_U32    Reserved_2956         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF11;

// Define the union U_DATE_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cc_f2data             : 16  ; // [15..0]
        HI_U32    cc_f1data             : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF12;

// Define the union U_DATE_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f1data             : 20  ; // [19..0]
        HI_U32    cg_enf2               : 1   ; // [20]
        HI_U32    cg_enf1               : 1   ; // [21]
        HI_U32    Reserved_2957         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF13;

// Define the union U_DATE_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f2data             : 20  ; // [19..0]
        HI_U32    Reserved_2958         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF14;

// Define the union U_DATE_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wss_data              : 14  ; // [13..0]
        HI_U32    wss_en                : 1   ; // [14]
        HI_U32    Reserved_2959         : 17  ; // [31..15]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF15;

// Define the union U_DATE_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 24  ; // [23..0]
        HI_U32    vps_en                : 1   ; // [24]
        HI_U32    Reserved_2960         : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF16;

// Define the union U_DATE_COEFF17
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE_COEFF17;
// Define the union U_DATE_COEFF18
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE_COEFF18;
// Define the union U_DATE_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 16  ; // [15..0]
        HI_U32    Reserved_2961         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF19;

// Define the union U_DATE_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt05_enf2             : 1   ; // [0]
        HI_U32    tt06_enf2             : 1   ; // [1]
        HI_U32    tt06_enf1             : 1   ; // [2]
        HI_U32    Reserved_2962         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF20;

// Define the union U_DATE_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_in_sel           : 3   ; // [2..0]
        HI_U32    Reserved_2968         : 1   ; // [3]
        HI_U32    dac1_in_sel           : 3   ; // [6..4]
        HI_U32    Reserved_2967         : 1   ; // [7]
        HI_U32    dac2_in_sel           : 3   ; // [10..8]
        HI_U32    Reserved_2966         : 1   ; // [11]
        HI_U32    dac3_in_sel           : 3   ; // [14..12]
        HI_U32    Reserved_2965         : 1   ; // [15]
        HI_U32    dac4_in_sel           : 3   ; // [18..16]
        HI_U32    Reserved_2964         : 1   ; // [19]
        HI_U32    dac5_in_sel           : 3   ; // [22..20]
        HI_U32    Reserved_2963         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF21;

// Define the union U_DATE_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_phase_delta     : 11  ; // [10..0]
        HI_U32    Reserved_2969         : 21  ; // [31..11]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF22;

// Define the union U_DATE_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_out_dly          : 3   ; // [2..0]
        HI_U32    Reserved_2975         : 1   ; // [3]
        HI_U32    dac1_out_dly          : 3   ; // [6..4]
        HI_U32    Reserved_2974         : 1   ; // [7]
        HI_U32    dac2_out_dly          : 3   ; // [10..8]
        HI_U32    Reserved_2973         : 1   ; // [11]
        HI_U32    dac3_out_dly          : 3   ; // [14..12]
        HI_U32    Reserved_2972         : 1   ; // [15]
        HI_U32    dac4_out_dly          : 3   ; // [18..16]
        HI_U32    Reserved_2971         : 1   ; // [19]
        HI_U32    dac5_out_dly          : 3   ; // [22..20]
        HI_U32    Reserved_2970         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_COEFF23;

// Define the union U_DATE_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_mask               : 1   ; // [0]
        HI_U32    Reserved_2976         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_ISRMASK;

// Define the union U_DATE_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_status             : 1   ; // [0]
        HI_U32    Reserved_2978         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_ISRSTATE;

// Define the union U_DATE_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_int                : 1   ; // [0]
        HI_U32    Reserved_2979         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE_ISR;

// Define the union U_DATE_VERSION
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_2980           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE_VERSION;
// Define the union U_DATE1_COEFF0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_seq                : 1   ; // [0]
        HI_U32    chgain_en             : 1   ; // [1]
        HI_U32    sylp_en               : 1   ; // [2]
        HI_U32    chlp_en               : 1   ; // [3]
        HI_U32    oversam2_en           : 1   ; // [4]
        HI_U32    lunt_en               : 1   ; // [5]
        HI_U32    oversam_en            : 2   ; // [7..6]
        HI_U32    Reserved_2983         : 1   ; // [8]
        HI_U32    luma_dl               : 4   ; // [12..9]
        HI_U32    agc_amp_sel           : 1   ; // [13]
        HI_U32    length_sel            : 1   ; // [14]
        HI_U32    sync_mode_scart       : 1   ; // [15]
        HI_U32    sync_mode_sel         : 2   ; // [17..16]
        HI_U32    style_sel             : 4   ; // [21..18]
        HI_U32    fm_sel                : 1   ; // [22]
        HI_U32    vbi_lpf_en            : 1   ; // [23]
        HI_U32    rgb_en                : 1   ; // [24]
        HI_U32    scanline              : 1   ; // [25]
        HI_U32    Reserved_2982         : 3   ; // [28..26]
        HI_U32    dis_ire               : 1   ; // [29]
        HI_U32    clpf_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF0;

// Define the union U_DATE1_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac_test              : 10  ; // [9..0]
        HI_U32    date_test_mode        : 2   ; // [11..10]
        HI_U32    date_test_en          : 1   ; // [12]
        HI_U32    amp_outside           : 10  ; // [22..13]
        HI_U32    c_limit_en            : 1   ; // [23]
        HI_U32    cc_seq                : 1   ; // [24]
        HI_U32    cgms_seq              : 1   ; // [25]
        HI_U32    vps_seq               : 1   ; // [26]
        HI_U32    wss_seq               : 1   ; // [27]
        HI_U32    cvbs_limit_en         : 1   ; // [28]
        HI_U32    c_gain                : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF1;

// Define the union U_DATE1_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt07_enf2             : 1   ; // [0]
        HI_U32    tt08_enf2             : 1   ; // [1]
        HI_U32    tt09_enf2             : 1   ; // [2]
        HI_U32    tt10_enf2             : 1   ; // [3]
        HI_U32    tt11_enf2             : 1   ; // [4]
        HI_U32    tt12_enf2             : 1   ; // [5]
        HI_U32    tt13_enf2             : 1   ; // [6]
        HI_U32    tt14_enf2             : 1   ; // [7]
        HI_U32    tt15_enf2             : 1   ; // [8]
        HI_U32    tt16_enf2             : 1   ; // [9]
        HI_U32    tt17_enf2             : 1   ; // [10]
        HI_U32    tt18_enf2             : 1   ; // [11]
        HI_U32    tt19_enf2             : 1   ; // [12]
        HI_U32    tt20_enf2             : 1   ; // [13]
        HI_U32    tt21_enf2             : 1   ; // [14]
        HI_U32    tt22_enf2             : 1   ; // [15]
        HI_U32    tt07_enf1             : 1   ; // [16]
        HI_U32    tt08_enf1             : 1   ; // [17]
        HI_U32    tt09_enf1             : 1   ; // [18]
        HI_U32    tt10_enf1             : 1   ; // [19]
        HI_U32    tt11_enf1             : 1   ; // [20]
        HI_U32    tt12_enf1             : 1   ; // [21]
        HI_U32    tt13_enf1             : 1   ; // [22]
        HI_U32    tt14_enf1             : 1   ; // [23]
        HI_U32    tt15_enf1             : 1   ; // [24]
        HI_U32    tt16_enf1             : 1   ; // [25]
        HI_U32    tt17_enf1             : 1   ; // [26]
        HI_U32    tt18_enf1             : 1   ; // [27]
        HI_U32    tt19_enf1             : 1   ; // [28]
        HI_U32    tt20_enf1             : 1   ; // [29]
        HI_U32    tt21_enf1             : 1   ; // [30]
        HI_U32    tt22_enf1             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF7;

// Define the union U_DATE1_COEFF8
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_staddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE1_COEFF8;
// Define the union U_DATE1_COEFF9
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_edaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE1_COEFF9;
// Define the union U_DATE1_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_pktoff             : 8   ; // [7..0]
        HI_U32    tt_mode               : 2   ; // [9..8]
        HI_U32    tt_highest            : 1   ; // [10]
        HI_U32    full_page             : 1   ; // [11]
        HI_U32    nabts_100ire          : 1   ; // [12]
        HI_U32    Reserved_2986         : 18  ; // [30..13]
        HI_U32    tt_ready              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF10;

// Define the union U_DATE1_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    date_clf2             : 10  ; // [9..0]
        HI_U32    date_clf1             : 10  ; // [19..10]
        HI_U32    cc_enf2               : 1   ; // [20]
        HI_U32    cc_enf1               : 1   ; // [21]
        HI_U32    Reserved_2987         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF11;

// Define the union U_DATE1_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cc_f2data             : 16  ; // [15..0]
        HI_U32    cc_f1data             : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF12;

// Define the union U_DATE1_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f1data             : 20  ; // [19..0]
        HI_U32    cg_enf2               : 1   ; // [20]
        HI_U32    cg_enf1               : 1   ; // [21]
        HI_U32    Reserved_2988         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF13;

// Define the union U_DATE1_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f2data             : 20  ; // [19..0]
        HI_U32    Reserved_2989         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF14;

// Define the union U_DATE1_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wss_data              : 14  ; // [13..0]
        HI_U32    wss_en                : 1   ; // [14]
        HI_U32    Reserved_2990         : 17  ; // [31..15]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF15;

// Define the union U_DATE1_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 24  ; // [23..0]
        HI_U32    vps_en                : 1   ; // [24]
        HI_U32    Reserved_2991         : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF16;

// Define the union U_DATE1_COEFF17
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE1_COEFF17;
// Define the union U_DATE1_COEFF18
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE1_COEFF18;
// Define the union U_DATE1_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 16  ; // [15..0]
        HI_U32    Reserved_2992         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF19;

// Define the union U_DATE1_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt05_enf2             : 1   ; // [0]
        HI_U32    tt06_enf2             : 1   ; // [1]
        HI_U32    tt06_enf1             : 1   ; // [2]
        HI_U32    Reserved_2993         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF20;

// Define the union U_DATE1_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_in_sel           : 3   ; // [2..0]
        HI_U32    Reserved_2999         : 1   ; // [3]
        HI_U32    dac1_in_sel           : 3   ; // [6..4]
        HI_U32    Reserved_2998         : 1   ; // [7]
        HI_U32    dac2_in_sel           : 3   ; // [10..8]
        HI_U32    Reserved_2997         : 1   ; // [11]
        HI_U32    dac3_in_sel           : 3   ; // [14..12]
        HI_U32    Reserved_2996         : 1   ; // [15]
        HI_U32    dac4_in_sel           : 3   ; // [18..16]
        HI_U32    Reserved_2995         : 1   ; // [19]
        HI_U32    dac5_in_sel           : 3   ; // [22..20]
        HI_U32    Reserved_2994         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF21;

// Define the union U_DATE1_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_phase_delta     : 11  ; // [10..0]
        HI_U32    Reserved_3000         : 21  ; // [31..11]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF22;

// Define the union U_DATE1_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_out_dly          : 3   ; // [2..0]
        HI_U32    Reserved_3006         : 1   ; // [3]
        HI_U32    dac1_out_dly          : 3   ; // [6..4]
        HI_U32    Reserved_3005         : 1   ; // [7]
        HI_U32    dac2_out_dly          : 3   ; // [10..8]
        HI_U32    Reserved_3004         : 1   ; // [11]
        HI_U32    dac3_out_dly          : 3   ; // [14..12]
        HI_U32    Reserved_3003         : 1   ; // [15]
        HI_U32    dac4_out_dly          : 3   ; // [18..16]
        HI_U32    Reserved_3002         : 1   ; // [19]
        HI_U32    dac5_out_dly          : 3   ; // [22..20]
        HI_U32    Reserved_3001         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_COEFF23;

// Define the union U_DATE1_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_mask               : 1   ; // [0]
        HI_U32    Reserved_3007         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_ISRMASK;

// Define the union U_DATE1_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_status             : 1   ; // [0]
        HI_U32    Reserved_3009         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_ISRSTATE;

// Define the union U_DATE1_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_int                : 1   ; // [0]
        HI_U32    Reserved_3010         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE1_ISR;

// Define the union U_DATE1_VERSION
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_3011           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE1_VERSION;
// Define the union U_DATE2_COEFF0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_seq                : 1   ; // [0]
        HI_U32    chgain_en             : 1   ; // [1]
        HI_U32    sylp_en               : 1   ; // [2]
        HI_U32    chlp_en               : 1   ; // [3]
        HI_U32    oversam2_en           : 1   ; // [4]
        HI_U32    lunt_en               : 1   ; // [5]
        HI_U32    oversam_en            : 2   ; // [7..6]
        HI_U32    Reserved_3014         : 1   ; // [8]
        HI_U32    luma_dl               : 4   ; // [12..9]
        HI_U32    agc_amp_sel           : 1   ; // [13]
        HI_U32    length_sel            : 1   ; // [14]
        HI_U32    sync_mode_scart       : 1   ; // [15]
        HI_U32    sync_mode_sel         : 2   ; // [17..16]
        HI_U32    style_sel             : 4   ; // [21..18]
        HI_U32    fm_sel                : 1   ; // [22]
        HI_U32    vbi_lpf_en            : 1   ; // [23]
        HI_U32    rgb_en                : 1   ; // [24]
        HI_U32    scanline              : 1   ; // [25]
        HI_U32    Reserved_3013         : 3   ; // [28..26]
        HI_U32    dis_ire               : 1   ; // [29]
        HI_U32    clpf_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF0;

// Define the union U_DATE2_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac_test              : 10  ; // [9..0]
        HI_U32    date_test_mode        : 2   ; // [11..10]
        HI_U32    date_test_en          : 1   ; // [12]
        HI_U32    amp_outside           : 10  ; // [22..13]
        HI_U32    c_limit_en            : 1   ; // [23]
        HI_U32    cc_seq                : 1   ; // [24]
        HI_U32    cgms_seq              : 1   ; // [25]
        HI_U32    vps_seq               : 1   ; // [26]
        HI_U32    wss_seq               : 1   ; // [27]
        HI_U32    cvbs_limit_en         : 1   ; // [28]
        HI_U32    c_gain                : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF1;

// Define the union U_DATE2_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt07_enf2             : 1   ; // [0]
        HI_U32    tt08_enf2             : 1   ; // [1]
        HI_U32    tt09_enf2             : 1   ; // [2]
        HI_U32    tt10_enf2             : 1   ; // [3]
        HI_U32    tt11_enf2             : 1   ; // [4]
        HI_U32    tt12_enf2             : 1   ; // [5]
        HI_U32    tt13_enf2             : 1   ; // [6]
        HI_U32    tt14_enf2             : 1   ; // [7]
        HI_U32    tt15_enf2             : 1   ; // [8]
        HI_U32    tt16_enf2             : 1   ; // [9]
        HI_U32    tt17_enf2             : 1   ; // [10]
        HI_U32    tt18_enf2             : 1   ; // [11]
        HI_U32    tt19_enf2             : 1   ; // [12]
        HI_U32    tt20_enf2             : 1   ; // [13]
        HI_U32    tt21_enf2             : 1   ; // [14]
        HI_U32    tt22_enf2             : 1   ; // [15]
        HI_U32    tt07_enf1             : 1   ; // [16]
        HI_U32    tt08_enf1             : 1   ; // [17]
        HI_U32    tt09_enf1             : 1   ; // [18]
        HI_U32    tt10_enf1             : 1   ; // [19]
        HI_U32    tt11_enf1             : 1   ; // [20]
        HI_U32    tt12_enf1             : 1   ; // [21]
        HI_U32    tt13_enf1             : 1   ; // [22]
        HI_U32    tt14_enf1             : 1   ; // [23]
        HI_U32    tt15_enf1             : 1   ; // [24]
        HI_U32    tt16_enf1             : 1   ; // [25]
        HI_U32    tt17_enf1             : 1   ; // [26]
        HI_U32    tt18_enf1             : 1   ; // [27]
        HI_U32    tt19_enf1             : 1   ; // [28]
        HI_U32    tt20_enf1             : 1   ; // [29]
        HI_U32    tt21_enf1             : 1   ; // [30]
        HI_U32    tt22_enf1             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF7;

// Define the union U_DATE2_COEFF8
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_staddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE2_COEFF8;
// Define the union U_DATE2_COEFF9
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_edaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE2_COEFF9;
// Define the union U_DATE2_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_pktoff             : 8   ; // [7..0]
        HI_U32    tt_mode               : 2   ; // [9..8]
        HI_U32    tt_highest            : 1   ; // [10]
        HI_U32    full_page             : 1   ; // [11]
        HI_U32    nabts_100ire          : 1   ; // [12]
        HI_U32    Reserved_3017         : 18  ; // [30..13]
        HI_U32    tt_ready              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF10;

// Define the union U_DATE2_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    date_clf2             : 10  ; // [9..0]
        HI_U32    date_clf1             : 10  ; // [19..10]
        HI_U32    cc_enf2               : 1   ; // [20]
        HI_U32    cc_enf1               : 1   ; // [21]
        HI_U32    Reserved_3018         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF11;

// Define the union U_DATE2_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cc_f2data             : 16  ; // [15..0]
        HI_U32    cc_f1data             : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF12;

// Define the union U_DATE2_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f1data             : 20  ; // [19..0]
        HI_U32    cg_enf2               : 1   ; // [20]
        HI_U32    cg_enf1               : 1   ; // [21]
        HI_U32    Reserved_3019         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF13;

// Define the union U_DATE2_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f2data             : 20  ; // [19..0]
        HI_U32    Reserved_3020         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF14;

// Define the union U_DATE2_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wss_data              : 14  ; // [13..0]
        HI_U32    wss_en                : 1   ; // [14]
        HI_U32    Reserved_3021         : 17  ; // [31..15]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF15;

// Define the union U_DATE2_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 24  ; // [23..0]
        HI_U32    vps_en                : 1   ; // [24]
        HI_U32    Reserved_3022         : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF16;

// Define the union U_DATE2_COEFF17
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE2_COEFF17;
// Define the union U_DATE2_COEFF18
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE2_COEFF18;
// Define the union U_DATE2_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 16  ; // [15..0]
        HI_U32    Reserved_3023         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF19;

// Define the union U_DATE2_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt05_enf2             : 1   ; // [0]
        HI_U32    tt06_enf2             : 1   ; // [1]
        HI_U32    tt06_enf1             : 1   ; // [2]
        HI_U32    Reserved_3024         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF20;

// Define the union U_DATE2_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_in_sel           : 3   ; // [2..0]
        HI_U32    Reserved_3030         : 1   ; // [3]
        HI_U32    dac1_in_sel           : 3   ; // [6..4]
        HI_U32    Reserved_3029         : 1   ; // [7]
        HI_U32    dac2_in_sel           : 3   ; // [10..8]
        HI_U32    Reserved_3028         : 1   ; // [11]
        HI_U32    dac3_in_sel           : 3   ; // [14..12]
        HI_U32    Reserved_3027         : 1   ; // [15]
        HI_U32    dac4_in_sel           : 3   ; // [18..16]
        HI_U32    Reserved_3026         : 1   ; // [19]
        HI_U32    dac5_in_sel           : 3   ; // [22..20]
        HI_U32    Reserved_3025         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF21;

// Define the union U_DATE2_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_phase_delta     : 11  ; // [10..0]
        HI_U32    Reserved_3031         : 21  ; // [31..11]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF22;

// Define the union U_DATE2_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_out_dly          : 3   ; // [2..0]
        HI_U32    Reserved_3037         : 1   ; // [3]
        HI_U32    dac1_out_dly          : 3   ; // [6..4]
        HI_U32    Reserved_3036         : 1   ; // [7]
        HI_U32    dac2_out_dly          : 3   ; // [10..8]
        HI_U32    Reserved_3035         : 1   ; // [11]
        HI_U32    dac3_out_dly          : 3   ; // [14..12]
        HI_U32    Reserved_3034         : 1   ; // [15]
        HI_U32    dac4_out_dly          : 3   ; // [18..16]
        HI_U32    Reserved_3033         : 1   ; // [19]
        HI_U32    dac5_out_dly          : 3   ; // [22..20]
        HI_U32    Reserved_3032         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_COEFF23;

// Define the union U_DATE2_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_mask               : 1   ; // [0]
        HI_U32    Reserved_3038         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_ISRMASK;

// Define the union U_DATE2_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_status             : 1   ; // [0]
        HI_U32    Reserved_3040         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_ISRSTATE;

// Define the union U_DATE2_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_int                : 1   ; // [0]
        HI_U32    Reserved_3041         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE2_ISR;

// Define the union U_DATE2_VERSION
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_3042           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE2_VERSION;
// Define the union U_DATE3_COEFF0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_seq                : 1   ; // [0]
        HI_U32    chgain_en             : 1   ; // [1]
        HI_U32    sylp_en               : 1   ; // [2]
        HI_U32    chlp_en               : 1   ; // [3]
        HI_U32    oversam2_en           : 1   ; // [4]
        HI_U32    lunt_en               : 1   ; // [5]
        HI_U32    oversam_en            : 2   ; // [7..6]
        HI_U32    Reserved_3045         : 1   ; // [8]
        HI_U32    luma_dl               : 4   ; // [12..9]
        HI_U32    agc_amp_sel           : 1   ; // [13]
        HI_U32    length_sel            : 1   ; // [14]
        HI_U32    sync_mode_scart       : 1   ; // [15]
        HI_U32    sync_mode_sel         : 2   ; // [17..16]
        HI_U32    style_sel             : 4   ; // [21..18]
        HI_U32    fm_sel                : 1   ; // [22]
        HI_U32    vbi_lpf_en            : 1   ; // [23]
        HI_U32    rgb_en                : 1   ; // [24]
        HI_U32    scanline              : 1   ; // [25]
        HI_U32    Reserved_3044         : 3   ; // [28..26]
        HI_U32    dis_ire               : 1   ; // [29]
        HI_U32    clpf_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF0;

// Define the union U_DATE3_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac_test              : 10  ; // [9..0]
        HI_U32    date_test_mode        : 2   ; // [11..10]
        HI_U32    date_test_en          : 1   ; // [12]
        HI_U32    amp_outside           : 10  ; // [22..13]
        HI_U32    c_limit_en            : 1   ; // [23]
        HI_U32    cc_seq                : 1   ; // [24]
        HI_U32    cgms_seq              : 1   ; // [25]
        HI_U32    vps_seq               : 1   ; // [26]
        HI_U32    wss_seq               : 1   ; // [27]
        HI_U32    cvbs_limit_en         : 1   ; // [28]
        HI_U32    c_gain                : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF1;

// Define the union U_DATE3_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt07_enf2             : 1   ; // [0]
        HI_U32    tt08_enf2             : 1   ; // [1]
        HI_U32    tt09_enf2             : 1   ; // [2]
        HI_U32    tt10_enf2             : 1   ; // [3]
        HI_U32    tt11_enf2             : 1   ; // [4]
        HI_U32    tt12_enf2             : 1   ; // [5]
        HI_U32    tt13_enf2             : 1   ; // [6]
        HI_U32    tt14_enf2             : 1   ; // [7]
        HI_U32    tt15_enf2             : 1   ; // [8]
        HI_U32    tt16_enf2             : 1   ; // [9]
        HI_U32    tt17_enf2             : 1   ; // [10]
        HI_U32    tt18_enf2             : 1   ; // [11]
        HI_U32    tt19_enf2             : 1   ; // [12]
        HI_U32    tt20_enf2             : 1   ; // [13]
        HI_U32    tt21_enf2             : 1   ; // [14]
        HI_U32    tt22_enf2             : 1   ; // [15]
        HI_U32    tt07_enf1             : 1   ; // [16]
        HI_U32    tt08_enf1             : 1   ; // [17]
        HI_U32    tt09_enf1             : 1   ; // [18]
        HI_U32    tt10_enf1             : 1   ; // [19]
        HI_U32    tt11_enf1             : 1   ; // [20]
        HI_U32    tt12_enf1             : 1   ; // [21]
        HI_U32    tt13_enf1             : 1   ; // [22]
        HI_U32    tt14_enf1             : 1   ; // [23]
        HI_U32    tt15_enf1             : 1   ; // [24]
        HI_U32    tt16_enf1             : 1   ; // [25]
        HI_U32    tt17_enf1             : 1   ; // [26]
        HI_U32    tt18_enf1             : 1   ; // [27]
        HI_U32    tt19_enf1             : 1   ; // [28]
        HI_U32    tt20_enf1             : 1   ; // [29]
        HI_U32    tt21_enf1             : 1   ; // [30]
        HI_U32    tt22_enf1             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF7;

// Define the union U_DATE3_COEFF8
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_staddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE3_COEFF8;
// Define the union U_DATE3_COEFF9
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_edaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE3_COEFF9;
// Define the union U_DATE3_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_pktoff             : 8   ; // [7..0]
        HI_U32    tt_mode               : 2   ; // [9..8]
        HI_U32    tt_highest            : 1   ; // [10]
        HI_U32    full_page             : 1   ; // [11]
        HI_U32    nabts_100ire          : 1   ; // [12]
        HI_U32    Reserved_3048         : 18  ; // [30..13]
        HI_U32    tt_ready              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF10;

// Define the union U_DATE3_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    date_clf2             : 10  ; // [9..0]
        HI_U32    date_clf1             : 10  ; // [19..10]
        HI_U32    cc_enf2               : 1   ; // [20]
        HI_U32    cc_enf1               : 1   ; // [21]
        HI_U32    Reserved_3049         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF11;

// Define the union U_DATE3_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cc_f2data             : 16  ; // [15..0]
        HI_U32    cc_f1data             : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF12;

// Define the union U_DATE3_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f1data             : 20  ; // [19..0]
        HI_U32    cg_enf2               : 1   ; // [20]
        HI_U32    cg_enf1               : 1   ; // [21]
        HI_U32    Reserved_3050         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF13;

// Define the union U_DATE3_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f2data             : 20  ; // [19..0]
        HI_U32    Reserved_3051         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF14;

// Define the union U_DATE3_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wss_data              : 14  ; // [13..0]
        HI_U32    wss_en                : 1   ; // [14]
        HI_U32    Reserved_3052         : 17  ; // [31..15]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF15;

// Define the union U_DATE3_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 24  ; // [23..0]
        HI_U32    vps_en                : 1   ; // [24]
        HI_U32    Reserved_3053         : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF16;

// Define the union U_DATE3_COEFF17
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE3_COEFF17;
// Define the union U_DATE3_COEFF18
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE3_COEFF18;
// Define the union U_DATE3_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 16  ; // [15..0]
        HI_U32    Reserved_3054         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF19;

// Define the union U_DATE3_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt05_enf2             : 1   ; // [0]
        HI_U32    tt06_enf2             : 1   ; // [1]
        HI_U32    tt06_enf1             : 1   ; // [2]
        HI_U32    Reserved_3055         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF20;

// Define the union U_DATE3_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_in_sel           : 3   ; // [2..0]
        HI_U32    Reserved_3061         : 1   ; // [3]
        HI_U32    dac1_in_sel           : 3   ; // [6..4]
        HI_U32    Reserved_3060         : 1   ; // [7]
        HI_U32    dac2_in_sel           : 3   ; // [10..8]
        HI_U32    Reserved_3059         : 1   ; // [11]
        HI_U32    dac3_in_sel           : 3   ; // [14..12]
        HI_U32    Reserved_3058         : 1   ; // [15]
        HI_U32    dac4_in_sel           : 3   ; // [18..16]
        HI_U32    Reserved_3057         : 1   ; // [19]
        HI_U32    dac5_in_sel           : 3   ; // [22..20]
        HI_U32    Reserved_3056         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF21;

// Define the union U_DATE3_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_phase_delta     : 11  ; // [10..0]
        HI_U32    Reserved_3062         : 21  ; // [31..11]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF22;

// Define the union U_DATE3_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_out_dly          : 3   ; // [2..0]
        HI_U32    Reserved_3068         : 1   ; // [3]
        HI_U32    dac1_out_dly          : 3   ; // [6..4]
        HI_U32    Reserved_3067         : 1   ; // [7]
        HI_U32    dac2_out_dly          : 3   ; // [10..8]
        HI_U32    Reserved_3066         : 1   ; // [11]
        HI_U32    dac3_out_dly          : 3   ; // [14..12]
        HI_U32    Reserved_3065         : 1   ; // [15]
        HI_U32    dac4_out_dly          : 3   ; // [18..16]
        HI_U32    Reserved_3064         : 1   ; // [19]
        HI_U32    dac5_out_dly          : 3   ; // [22..20]
        HI_U32    Reserved_3063         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_COEFF23;

// Define the union U_DATE3_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_mask               : 1   ; // [0]
        HI_U32    Reserved_3069         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_ISRMASK;

// Define the union U_DATE3_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_status             : 1   ; // [0]
        HI_U32    Reserved_3071         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_ISRSTATE;

// Define the union U_DATE3_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_int                : 1   ; // [0]
        HI_U32    Reserved_3072         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE3_ISR;

// Define the union U_DATE3_VERSION
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_3073           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE3_VERSION;
// Define the union U_DATE4_COEFF0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_seq                : 1   ; // [0]
        HI_U32    chgain_en             : 1   ; // [1]
        HI_U32    sylp_en               : 1   ; // [2]
        HI_U32    chlp_en               : 1   ; // [3]
        HI_U32    oversam2_en           : 1   ; // [4]
        HI_U32    lunt_en               : 1   ; // [5]
        HI_U32    oversam_en            : 2   ; // [7..6]
        HI_U32    Reserved_3076         : 1   ; // [8]
        HI_U32    luma_dl               : 4   ; // [12..9]
        HI_U32    agc_amp_sel           : 1   ; // [13]
        HI_U32    length_sel            : 1   ; // [14]
        HI_U32    sync_mode_scart       : 1   ; // [15]
        HI_U32    sync_mode_sel         : 2   ; // [17..16]
        HI_U32    style_sel             : 4   ; // [21..18]
        HI_U32    fm_sel                : 1   ; // [22]
        HI_U32    vbi_lpf_en            : 1   ; // [23]
        HI_U32    rgb_en                : 1   ; // [24]
        HI_U32    scanline              : 1   ; // [25]
        HI_U32    Reserved_3075         : 3   ; // [28..26]
        HI_U32    dis_ire               : 1   ; // [29]
        HI_U32    clpf_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF0;

// Define the union U_DATE4_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac_test              : 10  ; // [9..0]
        HI_U32    date_test_mode        : 2   ; // [11..10]
        HI_U32    date_test_en          : 1   ; // [12]
        HI_U32    amp_outside           : 10  ; // [22..13]
        HI_U32    c_limit_en            : 1   ; // [23]
        HI_U32    cc_seq                : 1   ; // [24]
        HI_U32    cgms_seq              : 1   ; // [25]
        HI_U32    vps_seq               : 1   ; // [26]
        HI_U32    wss_seq               : 1   ; // [27]
        HI_U32    cvbs_limit_en         : 1   ; // [28]
        HI_U32    c_gain                : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF1;

// Define the union U_DATE4_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt07_enf2             : 1   ; // [0]
        HI_U32    tt08_enf2             : 1   ; // [1]
        HI_U32    tt09_enf2             : 1   ; // [2]
        HI_U32    tt10_enf2             : 1   ; // [3]
        HI_U32    tt11_enf2             : 1   ; // [4]
        HI_U32    tt12_enf2             : 1   ; // [5]
        HI_U32    tt13_enf2             : 1   ; // [6]
        HI_U32    tt14_enf2             : 1   ; // [7]
        HI_U32    tt15_enf2             : 1   ; // [8]
        HI_U32    tt16_enf2             : 1   ; // [9]
        HI_U32    tt17_enf2             : 1   ; // [10]
        HI_U32    tt18_enf2             : 1   ; // [11]
        HI_U32    tt19_enf2             : 1   ; // [12]
        HI_U32    tt20_enf2             : 1   ; // [13]
        HI_U32    tt21_enf2             : 1   ; // [14]
        HI_U32    tt22_enf2             : 1   ; // [15]
        HI_U32    tt07_enf1             : 1   ; // [16]
        HI_U32    tt08_enf1             : 1   ; // [17]
        HI_U32    tt09_enf1             : 1   ; // [18]
        HI_U32    tt10_enf1             : 1   ; // [19]
        HI_U32    tt11_enf1             : 1   ; // [20]
        HI_U32    tt12_enf1             : 1   ; // [21]
        HI_U32    tt13_enf1             : 1   ; // [22]
        HI_U32    tt14_enf1             : 1   ; // [23]
        HI_U32    tt15_enf1             : 1   ; // [24]
        HI_U32    tt16_enf1             : 1   ; // [25]
        HI_U32    tt17_enf1             : 1   ; // [26]
        HI_U32    tt18_enf1             : 1   ; // [27]
        HI_U32    tt19_enf1             : 1   ; // [28]
        HI_U32    tt20_enf1             : 1   ; // [29]
        HI_U32    tt21_enf1             : 1   ; // [30]
        HI_U32    tt22_enf1             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF7;

// Define the union U_DATE4_COEFF8
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_staddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE4_COEFF8;
// Define the union U_DATE4_COEFF9
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_edaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE4_COEFF9;
// Define the union U_DATE4_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_pktoff             : 8   ; // [7..0]
        HI_U32    tt_mode               : 2   ; // [9..8]
        HI_U32    tt_highest            : 1   ; // [10]
        HI_U32    full_page             : 1   ; // [11]
        HI_U32    nabts_100ire          : 1   ; // [12]
        HI_U32    Reserved_3079         : 18  ; // [30..13]
        HI_U32    tt_ready              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF10;

// Define the union U_DATE4_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    date_clf2             : 10  ; // [9..0]
        HI_U32    date_clf1             : 10  ; // [19..10]
        HI_U32    cc_enf2               : 1   ; // [20]
        HI_U32    cc_enf1               : 1   ; // [21]
        HI_U32    Reserved_3080         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF11;

// Define the union U_DATE4_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cc_f2data             : 16  ; // [15..0]
        HI_U32    cc_f1data             : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF12;

// Define the union U_DATE4_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f1data             : 20  ; // [19..0]
        HI_U32    cg_enf2               : 1   ; // [20]
        HI_U32    cg_enf1               : 1   ; // [21]
        HI_U32    Reserved_3081         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF13;

// Define the union U_DATE4_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f2data             : 20  ; // [19..0]
        HI_U32    Reserved_3082         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF14;

// Define the union U_DATE4_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wss_data              : 14  ; // [13..0]
        HI_U32    wss_en                : 1   ; // [14]
        HI_U32    Reserved_3083         : 17  ; // [31..15]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF15;

// Define the union U_DATE4_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 24  ; // [23..0]
        HI_U32    vps_en                : 1   ; // [24]
        HI_U32    Reserved_3084         : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF16;

// Define the union U_DATE4_COEFF17
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE4_COEFF17;
// Define the union U_DATE4_COEFF18
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE4_COEFF18;
// Define the union U_DATE4_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 16  ; // [15..0]
        HI_U32    Reserved_3085         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF19;

// Define the union U_DATE4_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt05_enf2             : 1   ; // [0]
        HI_U32    tt06_enf2             : 1   ; // [1]
        HI_U32    tt06_enf1             : 1   ; // [2]
        HI_U32    Reserved_3086         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF20;

// Define the union U_DATE4_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_in_sel           : 3   ; // [2..0]
        HI_U32    Reserved_3092         : 1   ; // [3]
        HI_U32    dac1_in_sel           : 3   ; // [6..4]
        HI_U32    Reserved_3091         : 1   ; // [7]
        HI_U32    dac2_in_sel           : 3   ; // [10..8]
        HI_U32    Reserved_3090         : 1   ; // [11]
        HI_U32    dac3_in_sel           : 3   ; // [14..12]
        HI_U32    Reserved_3089         : 1   ; // [15]
        HI_U32    dac4_in_sel           : 3   ; // [18..16]
        HI_U32    Reserved_3088         : 1   ; // [19]
        HI_U32    dac5_in_sel           : 3   ; // [22..20]
        HI_U32    Reserved_3087         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF21;

// Define the union U_DATE4_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_phase_delta     : 11  ; // [10..0]
        HI_U32    Reserved_3093         : 21  ; // [31..11]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF22;

// Define the union U_DATE4_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_out_dly          : 3   ; // [2..0]
        HI_U32    Reserved_3099         : 1   ; // [3]
        HI_U32    dac1_out_dly          : 3   ; // [6..4]
        HI_U32    Reserved_3098         : 1   ; // [7]
        HI_U32    dac2_out_dly          : 3   ; // [10..8]
        HI_U32    Reserved_3097         : 1   ; // [11]
        HI_U32    dac3_out_dly          : 3   ; // [14..12]
        HI_U32    Reserved_3096         : 1   ; // [15]
        HI_U32    dac4_out_dly          : 3   ; // [18..16]
        HI_U32    Reserved_3095         : 1   ; // [19]
        HI_U32    dac5_out_dly          : 3   ; // [22..20]
        HI_U32    Reserved_3094         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_COEFF23;

// Define the union U_DATE4_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_mask               : 1   ; // [0]
        HI_U32    Reserved_3100         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_ISRMASK;

// Define the union U_DATE4_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_status             : 1   ; // [0]
        HI_U32    Reserved_3102         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_ISRSTATE;

// Define the union U_DATE4_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_int                : 1   ; // [0]
        HI_U32    Reserved_3103         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE4_ISR;

// Define the union U_DATE4_VERSION
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_3104           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE4_VERSION;
// Define the union U_DATE5_COEFF0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_seq                : 1   ; // [0]
        HI_U32    chgain_en             : 1   ; // [1]
        HI_U32    sylp_en               : 1   ; // [2]
        HI_U32    chlp_en               : 1   ; // [3]
        HI_U32    oversam2_en           : 1   ; // [4]
        HI_U32    lunt_en               : 1   ; // [5]
        HI_U32    oversam_en            : 2   ; // [7..6]
        HI_U32    Reserved_3107         : 1   ; // [8]
        HI_U32    luma_dl               : 4   ; // [12..9]
        HI_U32    agc_amp_sel           : 1   ; // [13]
        HI_U32    length_sel            : 1   ; // [14]
        HI_U32    sync_mode_scart       : 1   ; // [15]
        HI_U32    sync_mode_sel         : 2   ; // [17..16]
        HI_U32    style_sel             : 4   ; // [21..18]
        HI_U32    fm_sel                : 1   ; // [22]
        HI_U32    vbi_lpf_en            : 1   ; // [23]
        HI_U32    rgb_en                : 1   ; // [24]
        HI_U32    scanline              : 1   ; // [25]
        HI_U32    Reserved_3106         : 3   ; // [28..26]
        HI_U32    dis_ire               : 1   ; // [29]
        HI_U32    clpf_sel              : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF0;

// Define the union U_DATE5_COEFF1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac_test              : 10  ; // [9..0]
        HI_U32    date_test_mode        : 2   ; // [11..10]
        HI_U32    date_test_en          : 1   ; // [12]
        HI_U32    amp_outside           : 10  ; // [22..13]
        HI_U32    c_limit_en            : 1   ; // [23]
        HI_U32    cc_seq                : 1   ; // [24]
        HI_U32    cgms_seq              : 1   ; // [25]
        HI_U32    vps_seq               : 1   ; // [26]
        HI_U32    wss_seq               : 1   ; // [27]
        HI_U32    cvbs_limit_en         : 1   ; // [28]
        HI_U32    c_gain                : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF1;

// Define the union U_DATE5_COEFF7
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt07_enf2             : 1   ; // [0]
        HI_U32    tt08_enf2             : 1   ; // [1]
        HI_U32    tt09_enf2             : 1   ; // [2]
        HI_U32    tt10_enf2             : 1   ; // [3]
        HI_U32    tt11_enf2             : 1   ; // [4]
        HI_U32    tt12_enf2             : 1   ; // [5]
        HI_U32    tt13_enf2             : 1   ; // [6]
        HI_U32    tt14_enf2             : 1   ; // [7]
        HI_U32    tt15_enf2             : 1   ; // [8]
        HI_U32    tt16_enf2             : 1   ; // [9]
        HI_U32    tt17_enf2             : 1   ; // [10]
        HI_U32    tt18_enf2             : 1   ; // [11]
        HI_U32    tt19_enf2             : 1   ; // [12]
        HI_U32    tt20_enf2             : 1   ; // [13]
        HI_U32    tt21_enf2             : 1   ; // [14]
        HI_U32    tt22_enf2             : 1   ; // [15]
        HI_U32    tt07_enf1             : 1   ; // [16]
        HI_U32    tt08_enf1             : 1   ; // [17]
        HI_U32    tt09_enf1             : 1   ; // [18]
        HI_U32    tt10_enf1             : 1   ; // [19]
        HI_U32    tt11_enf1             : 1   ; // [20]
        HI_U32    tt12_enf1             : 1   ; // [21]
        HI_U32    tt13_enf1             : 1   ; // [22]
        HI_U32    tt14_enf1             : 1   ; // [23]
        HI_U32    tt15_enf1             : 1   ; // [24]
        HI_U32    tt16_enf1             : 1   ; // [25]
        HI_U32    tt17_enf1             : 1   ; // [26]
        HI_U32    tt18_enf1             : 1   ; // [27]
        HI_U32    tt19_enf1             : 1   ; // [28]
        HI_U32    tt20_enf1             : 1   ; // [29]
        HI_U32    tt21_enf1             : 1   ; // [30]
        HI_U32    tt22_enf1             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF7;

// Define the union U_DATE5_COEFF8
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_staddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE5_COEFF8;
// Define the union U_DATE5_COEFF9
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 tt_edaddr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE5_COEFF9;
// Define the union U_DATE5_COEFF10
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_pktoff             : 8   ; // [7..0]
        HI_U32    tt_mode               : 2   ; // [9..8]
        HI_U32    tt_highest            : 1   ; // [10]
        HI_U32    full_page             : 1   ; // [11]
        HI_U32    nabts_100ire          : 1   ; // [12]
        HI_U32    Reserved_3110         : 18  ; // [30..13]
        HI_U32    tt_ready              : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF10;

// Define the union U_DATE5_COEFF11
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    date_clf2             : 10  ; // [9..0]
        HI_U32    date_clf1             : 10  ; // [19..10]
        HI_U32    cc_enf2               : 1   ; // [20]
        HI_U32    cc_enf1               : 1   ; // [21]
        HI_U32    Reserved_3111         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF11;

// Define the union U_DATE5_COEFF12
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cc_f2data             : 16  ; // [15..0]
        HI_U32    cc_f1data             : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF12;

// Define the union U_DATE5_COEFF13
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f1data             : 20  ; // [19..0]
        HI_U32    cg_enf2               : 1   ; // [20]
        HI_U32    cg_enf1               : 1   ; // [21]
        HI_U32    Reserved_3112         : 10  ; // [31..22]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF13;

// Define the union U_DATE5_COEFF14
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cg_f2data             : 20  ; // [19..0]
        HI_U32    Reserved_3113         : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF14;

// Define the union U_DATE5_COEFF15
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    wss_data              : 14  ; // [13..0]
        HI_U32    wss_en                : 1   ; // [14]
        HI_U32    Reserved_3114         : 17  ; // [31..15]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF15;

// Define the union U_DATE5_COEFF16
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 24  ; // [23..0]
        HI_U32    vps_en                : 1   ; // [24]
        HI_U32    Reserved_3115         : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF16;

// Define the union U_DATE5_COEFF17
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE5_COEFF17;
// Define the union U_DATE5_COEFF18
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vps_data                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE5_COEFF18;
// Define the union U_DATE5_COEFF19
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vps_data              : 16  ; // [15..0]
        HI_U32    Reserved_3116         : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF19;

// Define the union U_DATE5_COEFF20
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt05_enf2             : 1   ; // [0]
        HI_U32    tt06_enf2             : 1   ; // [1]
        HI_U32    tt06_enf1             : 1   ; // [2]
        HI_U32    Reserved_3117         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF20;

// Define the union U_DATE5_COEFF21
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_in_sel           : 3   ; // [2..0]
        HI_U32    Reserved_3123         : 1   ; // [3]
        HI_U32    dac1_in_sel           : 3   ; // [6..4]
        HI_U32    Reserved_3122         : 1   ; // [7]
        HI_U32    dac2_in_sel           : 3   ; // [10..8]
        HI_U32    Reserved_3121         : 1   ; // [11]
        HI_U32    dac3_in_sel           : 3   ; // [14..12]
        HI_U32    Reserved_3120         : 1   ; // [15]
        HI_U32    dac4_in_sel           : 3   ; // [18..16]
        HI_U32    Reserved_3119         : 1   ; // [19]
        HI_U32    dac5_in_sel           : 3   ; // [22..20]
        HI_U32    Reserved_3118         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF21;

// Define the union U_DATE5_COEFF22
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    video_phase_delta     : 11  ; // [10..0]
        HI_U32    Reserved_3124         : 21  ; // [31..11]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF22;

// Define the union U_DATE5_COEFF23
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0_out_dly          : 3   ; // [2..0]
        HI_U32    Reserved_3130         : 1   ; // [3]
        HI_U32    dac1_out_dly          : 3   ; // [6..4]
        HI_U32    Reserved_3129         : 1   ; // [7]
        HI_U32    dac2_out_dly          : 3   ; // [10..8]
        HI_U32    Reserved_3128         : 1   ; // [11]
        HI_U32    dac3_out_dly          : 3   ; // [14..12]
        HI_U32    Reserved_3127         : 1   ; // [15]
        HI_U32    dac4_out_dly          : 3   ; // [18..16]
        HI_U32    Reserved_3126         : 1   ; // [19]
        HI_U32    dac5_out_dly          : 3   ; // [22..20]
        HI_U32    Reserved_3125         : 9   ; // [31..23]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_COEFF23;

// Define the union U_DATE5_ISRMASK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_mask               : 1   ; // [0]
        HI_U32    Reserved_3131         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_ISRMASK;

// Define the union U_DATE5_ISRSTATE
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_status             : 1   ; // [0]
        HI_U32    Reserved_3133         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_ISRSTATE;

// Define the union U_DATE5_ISR
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    tt_int                : 1   ; // [0]
        HI_U32    Reserved_3134         : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DATE5_ISR;

// Define the union U_DATE5_VERSION
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 Reserved_3135           : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DATE5_VERSION;
// Define the union U_VOLNKLSTCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    lnk_fst_num           : 8   ; // [7..0]
        HI_U32    lnk_fst_node_num      : 10  ; // [17..8]
        HI_U32    Reserved_3137         : 13  ; // [30..18]
        HI_U32    lnk_en                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOLNKLSTCTRL;

// Define the union U_VOLNKLSTADDR
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 lnk_addr                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VOLNKLSTADDR;
// Define the union U_VOLNKLSTSTRT
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    Reserved_3139         : 31  ; // [30..0]
        HI_U32    lnk_start             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOLNKLSTSTRT;

// Define the union U_VOLNKLSTPENDINGSTA
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 addr_pendig             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VOLNKLSTPENDINGSTA;
// Define the union U_VOLNKLSTWORKINGSTA
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 addr_working            : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VOLNKLSTWORKINGSTA;
// Define the union U_VOLNKLSTENDSTA
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 addr_end                : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VOLNKLSTENDSTA;
// Define the union U_VOLNKLSTNUMSTA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    node_num_pending      : 8   ; // [7..0]
        HI_U32    node_num_working      : 8   ; // [15..8]
        HI_U32    node_num_end          : 8   ; // [23..16]
        HI_U32    Reserved_3140         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOLNKLSTNUMSTA;

// Define the union U_VOCTRL
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    arb_mode              : 4   ; // [3..0]
        HI_U32    outstd_rid           : 4   ; // [7..4]
        HI_U32    arid_cfg0           : 4   ; // [11..8]
        HI_U32    arid_cfg1             : 4   ; // [15..12]
        HI_U32    outstd_wid            : 4   ; // [19..16]
        HI_U32    bus_dbg_en            : 2   ; // [21..20]
        HI_U32    Reserved_3141         : 2   ; // [23..22]
        HI_U32    awid_cfg                : 4  ; //[27..24]
        HI_U32    Reserved_3142           : 2 ; //[29..28]
        HI_U32    vo_id_sel               : 1;  //[30]
        HI_U32    vo_ck_gt_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOCTRL;

// Define the union U_VOINTSTA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dsdvtthd0_int         : 1   ; // [0]
        HI_U32    dsdvtthd1_int         : 1   ; // [1]
        HI_U32    Reserved_3151         : 1   ; // [2]
        HI_U32    dsduf_int             : 1   ; // [3]
        HI_U32    dhdvtthd0_int         : 1   ; // [4]
        HI_U32    Reserved_3150         : 2   ; // [6..5]
        HI_U32    dhduf_int             : 1   ; // [7]
        HI_U32    Reserved_3149         : 3   ; // [10..8]
        HI_U32    wbc_te_int            : 1   ; // [11]
        HI_U32    Reserved_3148         : 2   ; // [13..12]
        HI_U32    dsd1uf_int            : 1   ; // [14]
        HI_U32    Reserved_3147         : 5   ; // [19..15]
        HI_U32    vsdrr_int             : 1   ; // [20]
        HI_U32    Reserved_3146         : 1   ; // [21]
        HI_U32    vhdrr_int             : 1   ; // [22]
        HI_U32    g0rr_int              : 1   ; // [23]
        HI_U32    Reserved_3145         : 4   ; // [27..24]
        HI_U32    vhd_regup_err_int     : 1   ; // [28]
        HI_U32    Reserved_3144         : 1   ; // [29]
        HI_U32    ut_end_int            : 1   ; // [30]
        HI_U32    be_int                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOINTSTA;

// Define the union U_VOMSKINTSTA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dsdvtthd0_int         : 1   ; // [0]
        HI_U32    dsdvtthd1_int         : 1   ; // [1]
        HI_U32    Reserved_3153         : 1   ; // [2]
        HI_U32    dsduf_int             : 1   ; // [3]
        HI_U32    dhdvtthd1_int         : 1   ; // [4]
        HI_U32    Reserved_3152         : 2   ; // [6..5]
        HI_U32    dhduf_int             : 1   ; // [7]
        HI_U32    Reserved_3151         : 3   ; // [10..8]
        HI_U32    wbc_te_int            : 1   ; // [11]
        HI_U32    Reserved_3150         : 2   ; // [13..12]
        HI_U32    dsd1uf_int            : 1   ; // [14]
        HI_U32    Reserved_3149         : 5   ; // [19..15]
        HI_U32    vsdrr_int             : 1   ; // [20]
        HI_U32    vadrr_int             : 1   ; // [21]
        HI_U32    vhdrr_int             : 1   ; // [22]
        HI_U32    g0rr_int              : 1   ; // [23]
        HI_U32    Reserved_3148         : 4   ; // [27..24]
        HI_U32    vhd_regup_err_int     : 1   ; // [28]
        HI_U32    Reserved_3147         : 1   ; // [29]
        HI_U32    ut_end_int            : 1   ; // [30]
        HI_U32    be_int                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOMSKINTSTA;

// Define the union U_VOINTMSK
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dsdvtthd0_intmsk      : 1   ; // [0]
        HI_U32    dsdvtthd1_intmsk      : 1   ; // [1]
        HI_U32    Reserved_3157         : 1   ; // [2]
        HI_U32    dsduf_intmsk          : 1   ; // [3]
        HI_U32    dhdvtthd1_intmsk      : 1   ; // [4]
        HI_U32    Reserved_3156         : 2   ; // [6..5]
        HI_U32    dhduf_intmsk          : 1   ; // [7]
        HI_U32    Reserved_3155         : 3   ; // [10..8]
        HI_U32    wbc_te_intmsk         : 1   ; // [11]
        HI_U32    Reserved_3154         : 2   ; // [13..12]
        HI_U32    dsd1uf_intmsk         : 1   ; // [14]
        HI_U32    Reserved_3153         : 5   ; // [19..15]
        HI_U32    vsdrr_intmsk          : 1   ; // [20]
        HI_U32    Reserved_3152         : 1   ; // [21]
        HI_U32    vhdrr_intmsk          : 1   ; // [22]
        HI_U32    g0rr_intmsk           : 1   ; // [23]
        HI_U32    Reserved_3151         : 4   ; // [27..24]
        HI_U32    vhd_regup_err_intmsk  : 1   ; // [28]
        HI_U32    Reserved_3150         : 1   ; // [29]
        HI_U32    ut_end_intmsk         : 1   ; // [30]
        HI_U32    be_intmsk             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOINTMSK;

// Define the union U_VOUVERSION1
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vouversion0             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VOUVERSION1;
// Define the union U_VOUVERSION2
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 vouversion1             : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VOUVERSION2;
// Define the union U_VOMUXDATA
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    pin_test_data         : 24  ; // [23..0]
        HI_U32    dv_value              : 1   ; // [24]
        HI_U32    hsync_value           : 1   ; // [25]
        HI_U32    vsync_value           : 1   ; // [26]
        HI_U32    pin_test_mode         : 4   ; // [30..27]
        HI_U32    pin_test_en           : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOMUXDATA;

// Define the union U_VOMUX
typedef union
{
    // Define the struct bits
    struct
    {

        HI_U32    Reserved_3154         : 16  ; // [15..0]        
        HI_U32    hdmi_vid_sel          : 1   ; // [16]
        HI_U32    hdmi_vid              : 6   ; // [22..17]
        HI_U32    bt1120_sel_yc         : 1   ; //[23]         
        HI_U32    bt656_sel_yc          : 1   ; //[24]
        HI_U32    bt656_sel_sd          : 1   ; //[25]
        HI_U32    bt656_mux_sd          : 1   ; //[26]
        HI_U32    bt656_mux_hd          : 1   ; //[27]
        HI_U32    Reserved_3153         : 4   ; // [31..28]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOMUX;

// Define the union U_VODEBUG
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    rm_en_chn             : 4   ; // [3..0]
        HI_U32    dhd_ff_info           : 2   ; // [5..4]
        HI_U32    dsd_ff_info           : 2   ; // [7..6]
        HI_U32    dhd1_ff_info          : 2   ; // [9..8]
        HI_U32    dsd1_ff_info          : 2   ; // [11..10]
        HI_U32    dsd2_ff_info          : 2   ; // [13..12]
        HI_U32    dsd3_ff_info          : 2   ; // [15..14]
        HI_U32    dsd4_ff_info          : 2   ; // [17..16]
        HI_U32    dsd5_ff_info          : 2   ; // [19..18]
        HI_U32    wbc0_ff_info          : 2   ; // [21..20]
        HI_U32    wbc2_ff_info          : 2   ; // [23..22]
        HI_U32    wbc_mode              : 2   ; // [25..24]
        HI_U32    node_num              : 2   ; // [27..26]
        HI_U32    Reserved_3155         : 2   ; // [29..28]
        HI_U32    multi_data_en         : 1   ; // [30]
        HI_U32    Reserved_3154         : 1   ; // [31]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VODEBUG;

// Define the union U_VGACSCIDC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscidc2               : 9   ; // [8..0]
        HI_U32    cscidc1               : 9   ; // [17..9]
        HI_U32    cscidc0               : 9   ; // [26..18]
        HI_U32    csc_en                : 1   ; // [27]
        HI_U32    csc_mode              : 1   ; // [28]
        HI_U32    Reserved_3156         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VGACSCIDC;


// Define the union U_VGACSCODC
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscodc0               : 9   ; // [8..0]
        HI_U32    cscodc1               : 9   ; // [17..9]
        HI_U32    cscodc2               : 9   ; // [26..18]
        HI_U32    Reserved_3157         : 5   ; // [31..27]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VGACSCODC;

// Define the union U_VGACSCP0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp00                : 13  ; // [12..0]
        HI_U32    Reserved_3159         : 3   ; // [15..13]
        HI_U32    cscp01                : 13  ; // [28..16]
        HI_U32    Reserved_3158         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VGACSCP0;

// Define the union U_VGACSCP1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp02                : 13  ; // [12..0]
        HI_U32    Reserved_3161         : 3   ; // [15..13]
        HI_U32    cscp10                : 13  ; // [28..16]
        HI_U32    Reserved_3160         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VGACSCP1;

// Define the union U_VGACSCP2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp11                : 13  ; // [12..0]
        HI_U32    Reserved_3163         : 3   ; // [15..13]
        HI_U32    cscp12                : 13  ; // [28..16]
        HI_U32    Reserved_3162         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VGACSCP2;

// Define the union U_VGACSCP3
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp20                : 13  ; // [12..0]
        HI_U32    Reserved_3165         : 3   ; // [15..13]
        HI_U32    cscp21                : 13  ; // [28..16]
        HI_U32    Reserved_3164         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VGACSCP3;

// Define the union U_VGACSCP4
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cscp22                : 13  ; // [12..0]
        HI_U32    Reserved_3166         : 19  ; // [31..13]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VGACSCP4;

// Define the union U_VOPARAUP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vhd_hcoef_upd                 : 1   ; // [0]
        HI_U32    vhd_vcoef_upd                 : 1   ; // [1]
        HI_U32    Reserved_3168                 : 2   ; // [3..2]
        HI_U32    Reserved_video_acc_upd        : 1   ; // [4]
        HI_U32    Reserved_dhd_gamma_upd        : 1   ; // [5]
        HI_U32    wbc_hcoef_upd                 : 1   ; // [6]
        HI_U32    wbc_vcoef_upd                 : 1   ; // [7]
        HI_U32    Reserved_3167                 : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOPARAUP;

// Define the union U_VHDHCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDHCOEFAD;
// Define the union U_VHDVCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VHDVCOEFAD;
// Define the union U_VADHCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VADHCOEFAD;
// Define the union U_VADVCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VADVCOEFAD;
// Define the union U_ACCAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_ACCAD;

// Define the union U_WBCHCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_WBCHCOEFAD;
// Define the union U_WBCVCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_WBCVCOEFAD;

// Define the union U_VSDHCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDHCOEFAD;
// Define the union U_VSDVCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_VSDVCOEFAD;
// Define the union U_G0CLUTAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0CLUTAD;
// Define the union U_G1CLUTAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1CLUTAD;
// Define the union U_G0HCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0HCOEFAD;
// Define the union U_G0VCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G0VCOEFAD;
// Define the union U_G1HCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1HCOEFAD;
// Define the union U_G1VCOEFAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_G1VCOEFAD;
// Define the union U_DHDGAMMAAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DHDGAMMAAD;
// Define the union U_DHD1GAMMAAD
typedef union
{
    // Define the struct bits 
    struct
    {
        HI_U32 coef_addr               : 32  ; // [31..0]
    } bits;
 
    // Define an unsigned member
    HI_U32    u32;
 
} U_DHD1GAMMAAD;

// Define the union U_DACCTRL0_2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac0gc                : 6   ; // [5..0]
        HI_U32    dac1gc                : 6   ; // [11..6]
        HI_U32    dac2gc                : 6   ; // [17..12]
        HI_U32    ensc0                 : 1   ; // [18]
        HI_U32    ensc1                 : 1   ; // [19]
        HI_U32    ensc2                 : 1   ; // [20]
        HI_U32    enctr0                : 1   ; // [21]
        HI_U32    enctr1                : 1   ; // [22]
        HI_U32    enctr2                : 1   ; // [23]
        HI_U32    endac0                : 1   ; // [24]
        HI_U32    endac1                : 1   ; // [25]
        HI_U32    endac2                : 1   ; // [26]
        HI_U32    envbg                 : 1   ; // [27]
        HI_U32    enextref              : 1   ; // [28]
        HI_U32    Reserved_3169         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DACCTRL0_2;

// Define the union U_DACCTRL3_5
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    dac3gc                : 6   ; // [5..0]
        HI_U32    dac4gc                : 6   ; // [11..6]
        HI_U32    dac5gc                : 6   ; // [17..12]
        HI_U32    ensc3                 : 1   ; // [18]
        HI_U32    ensc4                 : 1   ; // [19]
        HI_U32    ensc5                 : 1   ; // [20]
        HI_U32    enctr3                : 1   ; // [21]
        HI_U32    enctr4                : 1   ; // [22]
        HI_U32    enctr5                : 1   ; // [23]
        HI_U32    endac3                : 1   ; // [24]
        HI_U32    endac4                : 1   ; // [25]
        HI_U32    endac5                : 1   ; // [26]
        HI_U32    envbg                 : 1   ; // [27]
        HI_U32    enextref              : 1   ; // [28]
        HI_U32    Reserved_3171         : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DACCTRL3_5;

// Define the union U_DACSTAT0_2
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cableout0             : 1   ; // [0]
        HI_U32    cableout1             : 1   ; // [1]
        HI_U32    cableout2             : 1   ; // [2]
        HI_U32    Reserved_3172         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DACSTAT0_2;

// Define the union U_DACSTAT3_5
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    cableout3             : 1   ; // [0]
        HI_U32    cableout4             : 1   ; // [1]
        HI_U32    cableout5             : 1   ; // [2]
        HI_U32    Reserved_3173         : 29  ; // [31..3]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DACSTAT3_5;

// Define the union U_DACBANDGAP
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    bandgap_test          : 4   ; // [3..0]
        HI_U32    bandgap_en            : 1   ; // [4]
        HI_U32    bandgap_bgtrim        : 3   ; // [7..5]
        HI_U32    Reserved_3174         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DACBANDGAP;

// Define the union U_VOLPTIMER
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    timer                 : 8   ; // [7..0]
        HI_U32    Reserved_3175         : 24  ; // [31..8]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOLPTIMER;

// Define the union U_VOLPMODE0
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vhd0_lp_mode          : 2   ; // [1..0]
        HI_U32    Reserved_3178         : 4   ; // [5..2]
        HI_U32    vsd_lp_mode           : 2   ; // [7..6]
        HI_U32    vsd1_lp_mode          : 2   ; // [9..8]
        HI_U32    Reserved_3177         : 6   ; // [15..10]
        HI_U32    dhd_lp_mode           : 2   ; // [17..16]
        HI_U32    Reserved_3176         : 2   ; // [19..18]
        HI_U32    dsd_lp_mode           : 2   ; // [21..20]
        HI_U32    dsd1_lp_mode          : 2   ; // [23..22]
        HI_U32    Reserved_3175         : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOLPMODE0;

// Define the union U_VOLPMODE1
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    g0_lp_mode            : 2   ; // [1..0]
        HI_U32    Reserved_3179         : 2   ; // [3..2]
        HI_U32    g2_lp_mode            : 2   ; // [5..4]
        HI_U32    g3_lp_mode            : 2   ; // [7..6]
        HI_U32    Reserved_3178         : 2   ; // [9..8]
        HI_U32    hc_lp_mode            : 2   ; // [11..10]
        HI_U32    Reserved_3177         : 2   ; // [13..12]
        HI_U32    para_lp_mode          : 2   ; // [15..14]
        HI_U32    wbc_lp_mode          : 2   ; // [17..16]
        HI_U32    Reserved_3176         : 14  ; // [31..18]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VOLPMODE1;

// Define the union U_VHDHLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3178         : 6   ; // [15..10]
        HI_U32    hlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3177         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDHLCOEF;

// Define the union U_VHDHCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3181         : 6   ; // [15..10]
        HI_U32    hccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3180         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDHCCOEF;

// Define the union U_VHDVLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3184         : 6   ; // [15..10]
        HI_U32    vlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3183         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVLCOEF;

// Define the union U_VHDVCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3187         : 6   ; // [15..10]
        HI_U32    vccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3186         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVCCOEF;

// Define the union U_VHD1HLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3190         : 6   ; // [15..10]
        HI_U32    hlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3189         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD1HLCOEF;

// Define the union U_VHD1HCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3193         : 6   ; // [15..10]
        HI_U32    hccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3192         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD1HCCOEF;

// Define the union U_VHD1VLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3196         : 6   ; // [15..10]
        HI_U32    vlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3195         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD1VLCOEF;

// Define the union U_VHD1VCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3199         : 6   ; // [15..10]
        HI_U32    vccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3198         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHD1VCCOEF;

// Define the union U_WBCHLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3202         : 6   ; // [15..10]
        HI_U32    hlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3201         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCHLCOEF;

// Define the union U_WBCHCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3205         : 6   ; // [15..10]
        HI_U32    hccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3204         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCHCCOEF;

// Define the union U_WBCVLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3208         : 6   ; // [15..10]
        HI_U32    vlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3207         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCVLCOEF;

// Define the union U_WBCVCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3211         : 6   ; // [15..10]
        HI_U32    vccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3210         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_WBCVCCOEF;

// Define the union U_DHDGAMMAN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    gamma_databn          : 10  ; // [9..0]
        HI_U32    gamma_datagn          : 10  ; // [19..10]
        HI_U32    gamma_datarn          : 10  ; // [29..20]
        HI_U32    Reserved_3213         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHDGAMMAN;

// Define the union U_DHD1GAMMAN
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    gamma_databn          : 10  ; // [9..0]
        HI_U32    gamma_datagn          : 10  ; // [19..10]
        HI_U32    gamma_datarn          : 10  ; // [29..20]
        HI_U32    Reserved_3215         : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_DHD1GAMMAN;

// Define the union U_VADHLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3218         : 6   ; // [15..10]
        HI_U32    hlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3217         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VADHLCOEF;

// Define the union U_VADHCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3221         : 6   ; // [15..10]
        HI_U32    hccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3220         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VADHCCOEF;

// Define the union U_VADVLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3224         : 6   ; // [15..10]
        HI_U32    vlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3223         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VADVLCOEF;

// Define the union U_VADVCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3227         : 6   ; // [15..10]
        HI_U32    vccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3226         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VADVCCOEF;

// Define the union U_VSDHLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3230         : 6   ; // [15..10]
        HI_U32    hlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3229         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDHLCOEF;

// Define the union U_VSDHCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3233         : 6   ; // [15..10]
        HI_U32    hccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3232         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDHCCOEF;

// Define the union U_VSDVLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3236         : 6   ; // [15..10]
        HI_U32    vlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3235         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDVLCOEF;

// Define the union U_VSDVCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3239         : 6   ; // [15..10]
        HI_U32    vccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3238         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VSDVCCOEF;

// Define the union U_G0HLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3242         : 6   ; // [15..10]
        HI_U32    hlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3241         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0HLCOEF;

// Define the union U_G0HCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3245         : 6   ; // [15..10]
        HI_U32    hccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3244         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0HCCOEF;

// Define the union U_G0VLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3248         : 6   ; // [15..10]
        HI_U32    vlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3247         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0VLCOEF;

// Define the union U_G0VCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3251         : 6   ; // [15..10]
        HI_U32    vccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3250         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G0VCCOEF;

// Define the union U_G1HLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3254         : 6   ; // [15..10]
        HI_U32    hlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3253         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1HLCOEF;

// Define the union U_G1HCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3257         : 6   ; // [15..10]
        HI_U32    hccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3256         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1HCCOEF;

// Define the union U_G1VLCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlcoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3260         : 6   ; // [15..10]
        HI_U32    vlcoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3259         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1VLCOEF;

// Define the union U_G1VCCOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vccoefn1              : 10  ; // [9..0]
        HI_U32    Reserved_3263         : 6   ; // [15..10]
        HI_U32    vccoefn2              : 10  ; // [25..16]
        HI_U32    Reserved_3262         : 6   ; // [31..26]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_G1VCCOEF;

// Define the union U_VHDHLTICOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hlticoefn1            : 10  ; // [9..0]
        HI_U32    hlticoefn2            : 10  ; // [19..10]
        HI_U32    Reserved_671          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDHLTICOEF;

// Define the union U_VHDHCTICOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    hcticoefn1            : 10  ; // [9..0]
        HI_U32    hcticoefn2            : 10  ; // [19..10]
        HI_U32    Reserved_673          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDHCTICOEF;

// Define the union U_VHDVLTICOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vlticoefn1            : 10  ; // [9..0]
        HI_U32    vlticoefn2            : 10  ; // [19..10]
        HI_U32    Reserved_675          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVLTICOEF;

// Define the union U_VHDVCTICOEF
typedef union
{
    // Define the struct bits
    struct
    {
        HI_U32    vcticoefn1            : 10  ; // [9..0]
        HI_U32    vcticoefn2            : 10  ; // [19..10]
        HI_U32    Reserved_677          : 12  ; // [31..20]
    } bits;

    // Define an unsigned member
    HI_U32    u32;

} U_VHDVCTICOEF;

//==============================================================================
// Define the global struct
typedef struct
{
    U_VHDCTRL            VHDCTRL;
    U_VHDUPD             VHDUPD;
    HI_U32         Reserved_VHDLADDR;
    HI_U32         Reserved_VHDLCADDR;
    U_VHDCADDR           VHDCADDR;
    U_VHDCCADDR          VHDCCADDR;
    HI_U32         Reserved_VHDNADDR;
    HI_U32         Reserved_VHDNCADDR;
    HI_U32         Reserved_0;
    U_VHDSTRIDE          VHDSTRIDE;             //0x0024
    U_VHDIRESO           VHDIRESO;              //0x0028
    HI_U32         Reserved_1;
    HI_U32         Reserved_VHDLKEY;
    U_VHDCBMPARA         VHDCBMPARA;            //0x0034
    #if 0
    HI_U32         Reserved_VHDDIEADDR;
    HI_U32         Reserved_VHDWBC1ADDR;
    HI_U32         Reserved_VHDWBC1STRD;
    HI_U32         Reserved_VHDDIESTADDR;
    HI_U32         Reserved_2[2];
    U_VHDCTI_CTRL        VHDCTI_CTRL;           /* no */
    U_VHDCHPASS_COEF     VHDCHPASS_COEF;        /* no */
    U_VHDCTI_THRE        VHDCTI_THRE;           /* no */
    U_VHDLGAIN_COEF      VHDLGAIN_COEF;         /* no */
    #else
    HI_U32         Reserved_2[10];
    #endif
    U_VHDDFPOS           VHDDFPOS;              //0x0060
    U_VHDDLPOS           VHDDLPOS;              //0x0064
    HI_U32         Reserved_VHDVFPOS;
    HI_U32         Reserved_VHDVLPOS;
    U_VHDBK              VHDBK;                 //0x0070
    HI_U32         Reserved_3[3];
    U_VHDCSCIDC          VHDCSCIDC;             //0x0080
    U_VHDCSCODC          VHDCSCODC;             //0x0084
    U_VHDCSCP0           VHDCSCP0;              //0x0088
    U_VHDCSCP1           VHDCSCP1;
    U_VHDCSCP2           VHDCSCP2;
    U_VHDCSCP3           VHDCSCP3;
    U_VHDCSCP4           VHDCSCP4;              //0x0098
    #if 0
    HI_U32         Reserved_4;
    U_VHDACM0            VHDACM0;    /* no */
    U_VHDACM1            VHDACM1;    /* no */
    U_VHDACM2            VHDACM2;    /* no */
    U_VHDACM3            VHDACM3;    /* no */
    U_VHDACM4            VHDACM4;    /* no */
    U_VHDACM5            VHDACM5;    /* no */
    U_VHDACM6            VHDACM6;    /* no */
    U_VHDACM7            VHDACM7;    /* no */
    #else
    HI_U32         Reserved_4[9];
    #endif
    U_VHDHSP             VHDHSP;                //0x00C0
    U_VHDHLOFFSET        VHDHLOFFSET;
    U_VHDHCOFFSET        VHDHCOFFSET;
    /* HI_U32         Reserved_5[3];       no */
    U_VHDVSP             VHDVSP;
    U_VHDVSR             VHDVSR;
    U_VHDVOFFSET         VHDVOFFSET;
    U_VHDZMEORESO        VHDZMEORESO;
    U_VHDZMEIRESO        VHDZMEIRESO;   /* 0x00DC */
    #if 0
    U_VHDZMEDBG          VHDZMEDBG;  /* no */
    //U_VHDCOEF00          VHDCOEF00;
    //U_VHDCOEF01          VHDCOEF01;
    //U_VHDCOEF10          VHDCOEF10;
    //U_VHDCOEF11          VHDCOEF11;
    U_VHDLTI_CTRL        VHDLTI_CTRL;       /* no */
    U_VHDLHPASS_COEF     VHDLHPASS_COEF;    /* no */
    U_VHDLTI_THRE        VHDLTI_THRE;       /* no */
    U_VHDLHFREQ_THRE     VHDLHFREQ_THRE;    /* no */
    U_VHDACCTHD1         VHDACCTHD1;        /* no */
    U_VHDACCTHD2         VHDACCTHD2;        /* no */
    HI_U32         Reserved_6[2];           /* no */
    U_VHDACCLOWN         VHDACCLOWN[3];     /* no */
    HI_U32         Reserved_7;              /* no */
    U_VHDACCMEDN         VHDACCMEDN[3];     /* no */
    HI_U32         Reserved_8;              /* no */
    U_VHDACCHIGHN        VHDACCHIGHN[3];    /* no */
    HI_U32         Reserved_9;              /* no */
    U_VHDACCMLN          VHDACCMLN[3];      /* no */
    HI_U32         Reserved_10;             /* no */
    U_VHDACCMHN          VHDACCMHN[3];      /* no */
    HI_U32         Reserved_11;             /* no */
    U_VHDACC3LOW         VHDACC3LOW;        /* no */
    U_VHDACC3MED         VHDACC3MED;        /* no */
    U_VHDACC3HIGH        VHDACC3HIGH;       /* no */
    U_VHDACC8MLOW        VHDACC8MLOW;       /* no */
    U_VHDACC8MHIGH       VHDACC8MHIGH;      /* no */
    U_VHDACCTOTAL        VHDACCTOTAL;       /* no */
    HI_U32         Reserved_12[2];          /* no */
    #else
    HI_U32         Reserved_12[40];  
    #endif
    U_VHDIFIRCOEF01      VHDIFIRCOEF01;   /* 0x0180 */
    U_VHDIFIRCOEF23      VHDIFIRCOEF23;
    U_VHDIFIRCOEF45      VHDIFIRCOEF45;
    U_VHDIFIRCOEF67      VHDIFIRCOEF67;     /* 0x018C */
    #if 0
    HI_U32         Reserved_13[28];
    HI_U32         Reserved_VHDDIECTRL;
    HI_U32         Reserved_14;
    HI_U32         Reserved_VHDDIELMA0;
    HI_U32         Reserved_VHDDIELMA1;
    HI_U32         Reserved_VHDDIELMA2;
    HI_U32         Reserved_VHDDIEINTEN;
    HI_U32         Reserved_VHDDIESCALE;
    HI_U32         Reserved_VHDDIECHECK1;
    HI_U32         Reserved_VHDDIECHECK2;
    HI_U32         Reserved_VHDDIEDIR0_3;
    HI_U32         Reserved_VHDDIEDIR4_7;
    HI_U32         Reserved_VHDDIEDIR8_11;
    HI_U32         Reserved_VHDDIEDIR12_14;
    HI_U32         Reserved_15;
    HI_U32         Reserved_VHDDIESTA;
    HI_U32         Reserved_16[26];
    HI_U32         Reserved_VHDDIESTKADDR;
    HI_U32         Reserved_VHDDIESTLADDR;
    HI_U32         Reserved_VHDDIESTMADDR;
    HI_U32         Reserved_VHDDIESTNADDR;
    HI_U32         Reserved_VHDDIESTSQTRADDR;
    HI_U32         Reserved_VHDCCRSCLR0;
    HI_U32         Reserved_VHDCCRSCLR1;
    HI_U32         Reserved_17[16];
    HI_U32         Reserved_VHDPDCTRL;
    HI_U32         Reserved_VHDPDBLKPOS0;
    HI_U32         Reserved_VHDPDBLKPOS1;
    HI_U32         Reserved_VHDPDBLKTHD;
    HI_U32         Reserved_VHDPDHISTTHD;
    HI_U32         Reserved_VHDPDUMTHD;
    HI_U32         Reserved_VHDPDPCCCORING;
    HI_U32         Reserved_VHDPDPCCHTHD;
    HI_U32         Reserved_VHDPDPCCVTHD;
    HI_U32         Reserved_VHDPDITDIFFVTHD;
    HI_U32         Reserved_VHDPDLASITHD;
    HI_U32         Reserved_VHDPDFRMITDIFF;
    HI_U32         Reserved_VHDPDSTLBLKSAD[16];
    HI_U32         Reserved_VHDPDHISTBIN[4];
    HI_U32         Reserved_VHDPDUMMATCH0;
    HI_U32         Reserved_VHDPDUMNOMATCH0;
    HI_U32         Reserved_VHDPDUMMATCH1;
    HI_U32         Reserved_VHDPDUMNOMATCH1;
    HI_U32         Reserved_VHDPDPCCFFWD;
    HI_U32         Reserved_VHDPDPCCFWD;
    HI_U32         Reserved_VHDPDPCCBWD;
    HI_U32         Reserved_VHDPDPCCCRSS;
    HI_U32         Reserved_VHDPDPCCPW;
    HI_U32         Reserved_VHDPDPCCFWDTKR;
    HI_U32         Reserved_VHDPDPCCBWDTKR;
    HI_U32         Reserved_VHDPDPCCBLKFWD[9];
    HI_U32         Reserved_VHDPDPCCBLKBWD[9];
    HI_U32         Reserved_VHDPDLASICNT14;
    HI_U32         Reserved_VHDPDLASICNT32;
    HI_U32         Reserved_VHDPDLASICNT34;
    #endif
    HI_U32         Reserved_17[156];
    U_VHDP0RESO          VHDP0RESO;             /* 0x0400 */
    U_VHDP0LADDR         VHDP0LADDR;
    U_VHDP0CADDR         VHDP0CADDR;
    U_VHDP0STRIDE        VHDP0STRIDE;
    U_VHDP0VFPOS         VHDP0VFPOS;
    U_VHDP0VLPOS         VHDP0VLPOS;
    HI_U32         Reserved_18[2];
    U_VHDP1RESO          VHDP1RESO;
    U_VHDP1LADDR         VHDP1LADDR;
    U_VHDP1CADDR         VHDP1CADDR;
    U_VHDP1STRIDE        VHDP1STRIDE;
    U_VHDP1VFPOS         VHDP1VFPOS;
    U_VHDP1VLPOS         VHDP1VLPOS;
    HI_U32         Reserved_19[2];
    U_VHDP2RESO          VHDP2RESO;
    U_VHDP2LADDR         VHDP2LADDR;
    U_VHDP2CADDR         VHDP2CADDR;
    U_VHDP2STRIDE        VHDP2STRIDE;
    U_VHDP2VFPOS         VHDP2VFPOS;
    U_VHDP2VLPOS         VHDP2VLPOS;
    HI_U32         Reserved_20[2];
    U_VHDP3RESO          VHDP3RESO;
    U_VHDP3LADDR         VHDP3LADDR;
    U_VHDP3CADDR         VHDP3CADDR;
    U_VHDP3STRIDE        VHDP3STRIDE;
    U_VHDP3VFPOS         VHDP3VFPOS;
    U_VHDP3VLPOS         VHDP3VLPOS;
    HI_U32         Reserved_21[2];
    U_VHDP4RESO          VHDP4RESO;
    U_VHDP4LADDR         VHDP4LADDR;
    U_VHDP4CADDR         VHDP4CADDR;
    U_VHDP4STRIDE        VHDP4STRIDE;
    U_VHDP4VFPOS         VHDP4VFPOS;
    U_VHDP4VLPOS         VHDP4VLPOS;
    HI_U32         Reserved_22[2];
    U_VHDP5RESO          VHDP5RESO;
    U_VHDP5LADDR         VHDP5LADDR;
    U_VHDP5CADDR         VHDP5CADDR;
    U_VHDP5STRIDE        VHDP5STRIDE;
    U_VHDP5VFPOS         VHDP5VFPOS;
    U_VHDP5VLPOS         VHDP5VLPOS;
    HI_U32         Reserved_23[2];
    U_VHDP6RESO          VHDP6RESO;
    U_VHDP6LADDR         VHDP6LADDR;
    U_VHDP6CADDR         VHDP6CADDR;
    U_VHDP6STRIDE        VHDP6STRIDE;
    U_VHDP6VFPOS         VHDP6VFPOS;
    U_VHDP6VLPOS         VHDP6VLPOS;
    HI_U32         Reserved_24[2];
    U_VHDP7RESO          VHDP7RESO;
    U_VHDP7LADDR         VHDP7LADDR;
    U_VHDP7CADDR         VHDP7CADDR;
    U_VHDP7STRIDE        VHDP7STRIDE;
    U_VHDP7VFPOS         VHDP7VFPOS;
    U_VHDP7VLPOS         VHDP7VLPOS;
    HI_U32         Reserved_25[2];
    U_VHDP8RESO          VHDP8RESO;
    U_VHDP8LADDR         VHDP8LADDR;
    U_VHDP8CADDR         VHDP8CADDR;
    U_VHDP8STRIDE        VHDP8STRIDE;
    U_VHDP8VFPOS         VHDP8VFPOS;
    U_VHDP8VLPOS         VHDP8VLPOS;
    HI_U32         Reserved_26[2];
    U_VHDP9RESO          VHDP9RESO;
    U_VHDP9LADDR         VHDP9LADDR;
    U_VHDP9CADDR         VHDP9CADDR;
    U_VHDP9STRIDE        VHDP9STRIDE;
    U_VHDP9VFPOS         VHDP9VFPOS;
    U_VHDP9VLPOS         VHDP9VLPOS;
    HI_U32         Reserved_27[2];
    U_VHDP10RESO         VHDP10RESO;
    U_VHDP10LADDR        VHDP10LADDR;
    U_VHDP10CADDR        VHDP10CADDR;
    U_VHDP10STRIDE       VHDP10STRIDE;
    U_VHDP10VFPOS        VHDP10VFPOS;
    U_VHDP10VLPOS        VHDP10VLPOS;
    HI_U32         Reserved_28[2];
    U_VHDP11RESO         VHDP11RESO;
    U_VHDP11LADDR        VHDP11LADDR;
    U_VHDP11CADDR        VHDP11CADDR;
    U_VHDP11STRIDE       VHDP11STRIDE;
    U_VHDP11VFPOS        VHDP11VFPOS;
    U_VHDP11VLPOS        VHDP11VLPOS;
    HI_U32         Reserved_29[2];
    U_VHDP12RESO         VHDP12RESO;
    U_VHDP12LADDR        VHDP12LADDR;
    U_VHDP12CADDR        VHDP12CADDR;
    U_VHDP12STRIDE       VHDP12STRIDE;
    U_VHDP12VFPOS        VHDP12VFPOS;
    U_VHDP12VLPOS        VHDP12VLPOS;
    HI_U32         Reserved_30[2];
    U_VHDP13RESO         VHDP13RESO;
    U_VHDP13LADDR        VHDP13LADDR;
    U_VHDP13CADDR        VHDP13CADDR;
    U_VHDP13STRIDE       VHDP13STRIDE;
    U_VHDP13VFPOS        VHDP13VFPOS;
    U_VHDP13VLPOS        VHDP13VLPOS;
    HI_U32         Reserved_31[2];
    U_VHDP14RESO         VHDP14RESO;
    U_VHDP14LADDR        VHDP14LADDR;
    U_VHDP14CADDR        VHDP14CADDR;
    U_VHDP14STRIDE       VHDP14STRIDE;
    U_VHDP14VFPOS        VHDP14VFPOS;
    U_VHDP14VLPOS        VHDP14VLPOS;
    HI_U32         Reserved_32[2];
    U_VHDP15RESO         VHDP15RESO;
    U_VHDP15LADDR        VHDP15LADDR;
    U_VHDP15CADDR        VHDP15CADDR;
    U_VHDP15STRIDE       VHDP15STRIDE;
    U_VHDP15VFPOS        VHDP15VFPOS;
    U_VHDP15VLPOS        VHDP15VLPOS;       //0x05F4
    #if 0
    HI_U32         Reserved_33[2];          /*no*/
    U_VHDP16RESO         VHDP16RESO;
    U_VHDP16LADDR        VHDP16LADDR;
    U_VHDP16CADDR        VHDP16CADDR;
    U_VHDP16STRIDE       VHDP16STRIDE;
    U_VHDP16VFPOS        VHDP16VFPOS;
    U_VHDP16VLPOS        VHDP16VLPOS;
    HI_U32         Reserved_34[2];
    U_VHDP17RESO         VHDP17RESO;
    U_VHDP17LADDR        VHDP17LADDR;
    U_VHDP17CADDR        VHDP17CADDR;
    U_VHDP17STRIDE       VHDP17STRIDE;
    U_VHDP17VFPOS        VHDP17VFPOS;
    U_VHDP17VLPOS        VHDP17VLPOS;
    HI_U32         Reserved_35[2];
    U_VHDP18RESO         VHDP18RESO;
    U_VHDP18LADDR        VHDP18LADDR;
    U_VHDP18CADDR        VHDP18CADDR;
    U_VHDP18STRIDE       VHDP18STRIDE;
    U_VHDP18VFPOS        VHDP18VFPOS;
    U_VHDP18VLPOS        VHDP18VLPOS;
    HI_U32         Reserved_36[2];
    U_VHDP19RESO         VHDP19RESO;
    U_VHDP19LADDR        VHDP19LADDR;
    U_VHDP19CADDR        VHDP19CADDR;
    U_VHDP19STRIDE       VHDP19STRIDE;
    U_VHDP19VFPOS        VHDP19VFPOS;
    U_VHDP19VLPOS        VHDP19VLPOS;
    HI_U32         Reserved_37[2];
    U_VHDP20RESO         VHDP20RESO;
    U_VHDP20LADDR        VHDP20LADDR;
    U_VHDP20CADDR        VHDP20CADDR;
    U_VHDP20STRIDE       VHDP20STRIDE;
    U_VHDP20VFPOS        VHDP20VFPOS;
    U_VHDP20VLPOS        VHDP20VLPOS;
    HI_U32         Reserved_38[2];
    U_VHDP21RESO         VHDP21RESO;
    U_VHDP21LADDR        VHDP21LADDR;
    U_VHDP21CADDR        VHDP21CADDR;
    U_VHDP21STRIDE       VHDP21STRIDE;
    U_VHDP21VFPOS        VHDP21VFPOS;
    U_VHDP21VLPOS        VHDP21VLPOS;
    HI_U32         Reserved_39[2];
    U_VHDP22RESO         VHDP22RESO;
    U_VHDP22LADDR        VHDP22LADDR;
    U_VHDP22CADDR        VHDP22CADDR;
    U_VHDP22STRIDE       VHDP22STRIDE;
    U_VHDP22VFPOS        VHDP22VFPOS;
    U_VHDP22VLPOS        VHDP22VLPOS;
    HI_U32         Reserved_40[2];
    U_VHDP23RESO         VHDP23RESO;
    U_VHDP23LADDR        VHDP23LADDR;
    U_VHDP23CADDR        VHDP23CADDR;
    U_VHDP23STRIDE       VHDP23STRIDE;
    U_VHDP23VFPOS        VHDP23VFPOS;
    U_VHDP23VLPOS        VHDP23VLPOS;
    HI_U32         Reserved_41[2];
    U_VHDP24RESO         VHDP24RESO;
    U_VHDP24LADDR        VHDP24LADDR;
    U_VHDP24CADDR        VHDP24CADDR;
    U_VHDP24STRIDE       VHDP24STRIDE;
    U_VHDP24VFPOS        VHDP24VFPOS;
    U_VHDP24VLPOS        VHDP24VLPOS;
    HI_U32         Reserved_42[2];
    U_VHDP25RESO         VHDP25RESO;
    U_VHDP25LADDR        VHDP25LADDR;
    U_VHDP25CADDR        VHDP25CADDR;
    U_VHDP25STRIDE       VHDP25STRIDE;
    U_VHDP25VFPOS        VHDP25VFPOS;
    U_VHDP25VLPOS        VHDP25VLPOS;
    HI_U32         Reserved_43[2];
    U_VHDP26RESO         VHDP26RESO;
    U_VHDP26LADDR        VHDP26LADDR;
    U_VHDP26CADDR        VHDP26CADDR;
    U_VHDP26STRIDE       VHDP26STRIDE;
    U_VHDP26VFPOS        VHDP26VFPOS;
    U_VHDP26VLPOS        VHDP26VLPOS;
    HI_U32         Reserved_44[2];
    U_VHDP27RESO         VHDP27RESO;
    U_VHDP27LADDR        VHDP27LADDR;
    U_VHDP27CADDR        VHDP27CADDR;
    U_VHDP27STRIDE       VHDP27STRIDE;
    U_VHDP27VFPOS        VHDP27VFPOS;
    U_VHDP27VLPOS        VHDP27VLPOS;
    HI_U32         Reserved_45[2];
    U_VHDP28RESO         VHDP28RESO;
    U_VHDP28LADDR        VHDP28LADDR;
    U_VHDP28CADDR        VHDP28CADDR;
    U_VHDP28STRIDE       VHDP28STRIDE;
    U_VHDP28VFPOS        VHDP28VFPOS;
    U_VHDP28VLPOS        VHDP28VLPOS;
    HI_U32         Reserved_46[2];
    U_VHDP29RESO         VHDP29RESO;
    U_VHDP29LADDR        VHDP29LADDR;
    U_VHDP29CADDR        VHDP29CADDR;
    U_VHDP29STRIDE       VHDP29STRIDE;
    U_VHDP29VFPOS        VHDP29VFPOS;
    U_VHDP29VLPOS        VHDP29VLPOS;
    HI_U32         Reserved_47[2];
    U_VHDP30RESO         VHDP30RESO;
    U_VHDP30LADDR        VHDP30LADDR;
    U_VHDP30CADDR        VHDP30CADDR;
    U_VHDP30STRIDE       VHDP30STRIDE;
    U_VHDP30VFPOS        VHDP30VFPOS;
    U_VHDP30VLPOS        VHDP30VLPOS;
    HI_U32         Reserved_48[2];
    U_VHDP31RESO         VHDP31RESO;
    U_VHDP31LADDR        VHDP31LADDR;
    U_VHDP31CADDR        VHDP31CADDR;
    U_VHDP31STRIDE       VHDP31STRIDE;
    U_VHDP31VFPOS        VHDP31VFPOS;
    U_VHDP31VLPOS        VHDP31VLPOS;           /*no*/
    HI_U32         Reserved_49[2];
    HI_U32         Reserved_VHDP32RESO;
    HI_U32         Reserved_VHDP32LADDR;
    HI_U32         Reserved_VHDP32CADDR;
    HI_U32         Reserved_VHDP32STRIDE;
    HI_U32         Reserved_VHDP32VFPOS;
    HI_U32         Reserved_VHDP32VLPOS;
    HI_U32         Reserved_50[2];
    HI_U32         Reserved_VHDP33RESO;
    HI_U32         Reserved_VHDP33LADDR;
    HI_U32         Reserved_VHDP33CADDR;
    HI_U32         Reserved_VHDP33STRIDE;
    HI_U32         Reserved_VHDP33VFPOS;
    HI_U32         Reserved_VHDP33VLPOS;
    HI_U32         Reserved_51[2];
    HI_U32         Reserved_VHDP34RESO;
    HI_U32         Reserved_VHDP34LADDR;
    HI_U32         Reserved_VHDP34CADDR;
    HI_U32         Reserved_VHDP34STRIDE;
    HI_U32         Reserved_VHDP34VFPOS;
    HI_U32         Reserved_VHDP34VLPOS;
    HI_U32         Reserved_52[2];
    HI_U32         Reserved_VHDP35RESO;
    HI_U32         Reserved_VHDP35LADDR;
    HI_U32         Reserved_VHDP35CADDR;
    HI_U32         Reserved_VHDP35STRIDE;
    HI_U32         Reserved_VHDP35VFPOS;
    HI_U32         Reserved_VHDP35VLPOS;
    HI_U32         Reserved_53[2];
    HI_U32         Reserved_VHDP36RESO;
    HI_U32         Reserved_VHDP36LADDR;
    HI_U32         Reserved_VHDP36CADDR;
    HI_U32         Reserved_VHDP36STRIDE;
    HI_U32         Reserved_VHDP36VFPOS;
    HI_U32         Reserved_VHDP36VLPOS;
    HI_U32         Reserved_54[2];
    HI_U32         Reserved_VHDP37RESO;
    HI_U32         Reserved_VHDP37LADDR;
    HI_U32         Reserved_VHDP37CADDR;
    HI_U32         Reserved_VHDP37STRIDE;
    HI_U32         Reserved_VHDP37VFPOS;
    HI_U32         Reserved_VHDP37VLPOS;
    HI_U32         Reserved_55[2];
    HI_U32         Reserved_VHDP38RESO;
    HI_U32         Reserved_VHDP38LADDR;
    HI_U32         Reserved_VHDP38CADDR;
    HI_U32         Reserved_VHDP38STRIDE;
    HI_U32         Reserved_VHDP38VFPOS;
    HI_U32         Reserved_VHDP38VLPOS;
    HI_U32         Reserved_56[2];
    HI_U32         Reserved_VHDP39RESO;
    HI_U32         Reserved_VHDP39LADDR;
    HI_U32         Reserved_VHDP39CADDR;
    HI_U32         Reserved_VHDP39STRIDE;
    HI_U32         Reserved_VHDP39VFPOS;
    HI_U32         Reserved_VHDP39VLPOS;
    HI_U32         Reserved_57[2];
    HI_U32         Reserved_VHDP40RESO;
    HI_U32         Reserved_VHDP40LADDR;
    HI_U32         Reserved_VHDP40CADDR;
    HI_U32         Reserved_VHDP40STRIDE;
    HI_U32         Reserved_VHDP40VFPOS;
    HI_U32         Reserved_VHDP40VLPOS;
    HI_U32         Reserved_58[2];
    HI_U32         Reserved_VHDP41RESO;
    HI_U32         Reserved_VHDP41LADDR;
    HI_U32         Reserved_VHDP41CADDR;
    HI_U32         Reserved_VHDP41STRIDE;
    HI_U32         Reserved_VHDP41VFPOS;
    HI_U32         Reserved_VHDP41VLPOS;
    HI_U32         Reserved_59[2];
    HI_U32         Reserved_VHDP42RESO;
    HI_U32         Reserved_VHDP42LADDR;
    HI_U32         Reserved_VHDP42CADDR;
    HI_U32         Reserved_VHDP42STRIDE;
    HI_U32         Reserved_VHDP42VFPOS;
    HI_U32         Reserved_VHDP42VLPOS;
    HI_U32         Reserved_60[2];
    HI_U32         Reserved_VHDP43RESO;
    HI_U32         Reserved_VHDP43LADDR;
    HI_U32         Reserved_VHDP43CADDR;
    HI_U32         Reserved_VHDP43STRIDE;
    HI_U32         Reserved_VHDP43VFPOS;
    HI_U32         Reserved_VHDP43VLPOS;
    HI_U32         Reserved_61[2];
    HI_U32         Reserved_VHDP44RESO;
    HI_U32         Reserved_VHDP44LADDR;
    HI_U32         Reserved_VHDP44CADDR;
    HI_U32         Reserved_VHDP44STRIDE;
    HI_U32         Reserved_VHDP44VFPOS;
    HI_U32         Reserved_VHDP44VLPOS;
    HI_U32         Reserved_62[2];
    HI_U32         Reserved_VHDP45RESO;
    HI_U32         Reserved_VHDP45LADDR;
    HI_U32         Reserved_VHDP45CADDR;
    HI_U32         Reserved_VHDP45STRIDE;
    HI_U32         Reserved_VHDP45VFPOS;
    HI_U32         Reserved_VHDP45VLPOS;
    HI_U32         Reserved_63[2];
    HI_U32         Reserved_VHDP46RESO;
    HI_U32         Reserved_VHDP46LADDR;
    HI_U32         Reserved_VHDP46CADDR;
    HI_U32         Reserved_VHDP46STRIDE;
    HI_U32         Reserved_VHDP46VFPOS;
    HI_U32         Reserved_VHDP46VLPOS;
    HI_U32         Reserved_64[2];
    HI_U32         Reserved_VHDP47RESO;
    HI_U32         Reserved_VHDP47LADDR;
    HI_U32         Reserved_VHDP47CADDR;
    HI_U32         Reserved_VHDP47STRIDE;
    HI_U32         Reserved_VHDP47VFPOS;
    HI_U32         Reserved_VHDP47VLPOS;
    HI_U32         Reserved_65[2];
    HI_U32         Reserved_VHDP48RESO;
    HI_U32         Reserved_VHDP48LADDR;
    HI_U32         Reserved_VHDP48CADDR;
    HI_U32         Reserved_VHDP48STRIDE;
    HI_U32         Reserved_VHDP48VFPOS;
    HI_U32         Reserved_VHDP48VLPOS;
    HI_U32         Reserved_66[2];
    HI_U32         Reserved_VHDP49RESO;
    HI_U32         Reserved_VHDP49LADDR;
    HI_U32         Reserved_VHDP49CADDR;
    HI_U32         Reserved_VHDP49STRIDE;
    HI_U32         Reserved_VHDP49VFPOS;
    HI_U32         Reserved_VHDP49VLPOS;
    HI_U32         Reserved_67[2];
    HI_U32         Reserved_VHDP50RESO;
    HI_U32         Reserved_VHDP50LADDR;
    HI_U32         Reserved_VHDP50CADDR;
    HI_U32         Reserved_VHDP50STRIDE;
    HI_U32         Reserved_VHDP50VFPOS;
    HI_U32         Reserved_VHDP50VLPOS;
    HI_U32         Reserved_68[2];
    HI_U32         Reserved_VHDP51RESO;
    HI_U32         Reserved_VHDP51LADDR;
    HI_U32         Reserved_VHDP51CADDR;
    HI_U32         Reserved_VHDP51STRIDE;
    HI_U32         Reserved_VHDP51VFPOS;
    HI_U32         Reserved_VHDP51VLPOS;
    HI_U32         Reserved_69[2];
    HI_U32         Reserved_VHDP52RESO;
    HI_U32         Reserved_VHDP52LADDR;
    HI_U32         Reserved_VHDP52CADDR;
    HI_U32         Reserved_VHDP52STRIDE;
    HI_U32         Reserved_VHDP52VFPOS;
    HI_U32         Reserved_VHDP52VLPOS;
    HI_U32         Reserved_70[2];
    HI_U32         Reserved_VHDP53RESO;
    HI_U32         Reserved_VHDP53LADDR;
    HI_U32         Reserved_VHDP53CADDR;
    HI_U32         Reserved_VHDP53STRIDE;
    HI_U32         Reserved_VHDP53VFPOS;
    HI_U32         Reserved_VHDP53VLPOS;
    HI_U32         Reserved_71[2];
    HI_U32         Reserved_VHDP54RESO;
    HI_U32         Reserved_VHDP54LADDR;
    HI_U32         Reserved_VHDP54CADDR;
    HI_U32         Reserved_VHDP54STRIDE;
    HI_U32         Reserved_VHDP54VFPOS;
    HI_U32         Reserved_VHDP54VLPOS;
    HI_U32         Reserved_72[2];
    HI_U32         Reserved_VHDP55RESO;
    HI_U32         Reserved_VHDP55LADDR;
    HI_U32         Reserved_VHDP55CADDR;
    HI_U32         Reserved_VHDP55STRIDE;
    HI_U32         Reserved_VHDP55VFPOS;
    HI_U32         Reserved_VHDP55VLPOS;
    HI_U32         Reserved_73[2];
    HI_U32         Reserved_VHDP56RESO;
    HI_U32         Reserved_VHDP56LADDR;
    HI_U32         Reserved_VHDP56CADDR;
    HI_U32         Reserved_VHDP56STRIDE;
    HI_U32         Reserved_VHDP56VFPOS;
    HI_U32         Reserved_VHDP56VLPOS;
    HI_U32         Reserved_74[2];
    HI_U32         Reserved_VHDP57RESO;
    HI_U32         Reserved_VHDP57LADDR;
    HI_U32         Reserved_VHDP57CADDR;
    HI_U32         Reserved_VHDP57STRIDE;
    HI_U32         Reserved_VHDP57VFPOS;
    HI_U32         Reserved_VHDP57VLPOS;
    HI_U32         Reserved_75[2];
    HI_U32         Reserved_VHDP58RESO;
    HI_U32         Reserved_VHDP58LADDR;
    HI_U32         Reserved_VHDP58CADDR;
    HI_U32         Reserved_VHDP58STRIDE;
    HI_U32         Reserved_VHDP58VFPOS;
    HI_U32         Reserved_VHDP58VLPOS;
    HI_U32         Reserved_76[2];
    HI_U32         Reserved_VHDP59RESO;
    HI_U32         Reserved_VHDP59LADDR;
    HI_U32         Reserved_VHDP59CADDR;
    HI_U32         Reserved_VHDP59STRIDE;
    HI_U32         Reserved_VHDP59VFPOS;
    HI_U32         Reserved_VHDP59VLPOS;
    HI_U32         Reserved_77[2];
    HI_U32         Reserved_VHDP60RESO;
    HI_U32         Reserved_VHDP60LADDR;
    HI_U32         Reserved_VHDP60CADDR;
    HI_U32         Reserved_VHDP60STRIDE;
    HI_U32         Reserved_VHDP60VFPOS;
    HI_U32         Reserved_VHDP60VLPOS;
    HI_U32         Reserved_78[2];
    HI_U32         Reserved_VHDP61RESO;
    HI_U32         Reserved_VHDP61LADDR;
    HI_U32         Reserved_VHDP61CADDR;
    HI_U32         Reserved_VHDP61STRIDE;
    HI_U32         Reserved_VHDP61VFPOS;
    HI_U32         Reserved_VHDP61VLPOS;
    HI_U32         Reserved_79[2];
    HI_U32         Reserved_VHDP62RESO;
    HI_U32         Reserved_VHDP62LADDR;
    HI_U32         Reserved_VHDP62CADDR;
    HI_U32         Reserved_VHDP62STRIDE;
    HI_U32         Reserved_VHDP62VFPOS;
    HI_U32         Reserved_VHDP62VLPOS;
    HI_U32         Reserved_80[2];
    HI_U32         Reserved_VHDP63RESO;
    HI_U32         Reserved_VHDP63LADDR;
    HI_U32         Reserved_VHDP63CADDR;
    HI_U32         Reserved_VHDP63STRIDE;
    HI_U32         Reserved_VHDP63VFPOS;
    HI_U32         Reserved_VHDP63VLPOS;
    HI_U32         Reserved_81[2];
    HI_U32         Reserved_VHD64REGIONENH;
    #else
    HI_U32         Reserved_81[387];
    #endif
    U_VHD16REGIONENL     VHD16REGIONENL;    //0x0C04
#if 0
    HI_U32         Reserved_82[254];
    HI_U32         Reserved_VHD1CTRL;
    HI_U32         Reserved_VHD1UPD;
    HI_U32         Reserved_VHD1LADDR;
    HI_U32         Reserved_VHD1LCADDR;
    HI_U32         Reserved_VHD1CADDR;
    HI_U32         Reserved_VHD1CCADDR;
    HI_U32         Reserved_VHD1NADDR;
    HI_U32         Reserved_VHD1NCADDR;
    HI_U32         Reserved_83;
    HI_U32         Reserved_VHD1STRIDE;
    HI_U32         Reserved_VHD1IRESO;
    HI_U32         Reserved_84;
    HI_U32         Reserved_VHD1LKEY;
    HI_U32         Reserved_VHD1CBMPARA;
    HI_U32         Reserved_VHD1DIEADDR;
    HI_U32         Reserved_VHD1WBC1ADDR;
    HI_U32         Reserved_VHD1WBC1STRD;
    HI_U32         Reserved_VHD1DIESTADDR;
    HI_U32         Reserved_85[6];
    HI_U32         Reserved_VHD1DFPOS;
    HI_U32         Reserved_VHD1DLPOS;
    HI_U32         Reserved_VHD1VFPOS;
    HI_U32         Reserved_VHD1VLPOS;
    HI_U32         Reserved_VHD1BK;
    HI_U32         Reserved_86[3];
    HI_U32         Reserved_VHD1CSCIDC;
    HI_U32         Reserved_VHD1CSCODC;
    HI_U32         Reserved_VHD1CSCP0;
    HI_U32         Reserved_VHD1CSCP1;
    HI_U32         Reserved_VHD1CSCP2;
    HI_U32         Reserved_VHD1CSCP3;
    HI_U32         Reserved_VHD1CSCP4;
    HI_U32         Reserved_87;
    HI_U32         Reserved_VHD1ACM0;
    HI_U32         Reserved_VHD1ACM1;
    HI_U32         Reserved_VHD1ACM2;
    HI_U32         Reserved_VHD1ACM3;
    HI_U32         Reserved_VHD1ACM4;
    HI_U32         Reserved_VHD1ACM5;
    HI_U32         Reserved_VHD1ACM6;
    HI_U32         Reserved_VHD1ACM7;
    HI_U32         Reserved_VHD1HSP;
    HI_U32         Reserved_VHD1HLOFFSET;
    HI_U32         Reserved_VHD1HCOFFSET;
    HI_U32         Reserved_88[3];
    HI_U32         Reserved_VHD1VSP;
    HI_U32         Reserved_VHD1VSR;
    HI_U32         Reserved_VHD1VOFFSET;
    HI_U32         Reserved_VHD1ZMEORESO;
    HI_U32         Reserved_VHD1ZMEIRESO;
    HI_U32         Reserved_VHD1ZMEDBG;
    HI_U32         Reserved_VHD1COEF00;
    HI_U32         Reserved_VHD1COEF01;
    HI_U32         Reserved_VHD1COEF10;
    HI_U32         Reserved_VHD1COEF11;
    HI_U32         Reserved_VHD1ACCTHD1;
    HI_U32         Reserved_VHD1ACCTHD2;
    HI_U32         Reserved_89[2];
    HI_U32         Reserved_VHD1ACCLOWN[3];
    HI_U32         Reserved_90;
    HI_U32         Reserved_VHD1ACCMEDN[3];
    HI_U32         Reserved_91;
    HI_U32         Reserved_VHD1ACCHIGHN[3];
    HI_U32         Reserved_92;
    HI_U32         Reserved_VHD1ACCMLN[3];
    HI_U32         Reserved_93;
    HI_U32         Reserved_VHD1ACCMHN[3];
    HI_U32         Reserved_94;
    HI_U32         Reserved_VHD1ACC3LOW;
    HI_U32         Reserved_VHD1ACC3MED;
    HI_U32         Reserved_VHD1ACC3HIGH;
    HI_U32         Reserved_VHD1ACC8MLOW;
    HI_U32         Reserved_VHD1ACC8MHIGH;
    HI_U32         Reserved_VHD1ACCTOTAL;
    HI_U32         Reserved_95[2];
    HI_U32         Reserved_VHD1IFIRCOEF01;
    HI_U32         Reserved_VHD1IFIRCOEF23;
    HI_U32         Reserved_VHD1IFIRCOEF45;
    HI_U32         Reserved_VHD1IFIRCOEF67;
    HI_U32         Reserved_96[28];
    HI_U32         Reserved_VHD1DIECTRL;
    HI_U32         Reserved_97;
    HI_U32         Reserved_VHD1DIELMA0;
    HI_U32         Reserved_VHD1DIELMA1;
    HI_U32         Reserved_VHD1DIELMA2;
    HI_U32         Reserved_VHD1DIEINTEN;
    HI_U32         Reserved_VHD1DIESCALE;
    HI_U32         Reserved_VHD1DIECHECK1;
    HI_U32         Reserved_VHD1DIECHECK2;
    HI_U32         Reserved_VHD1DIEDIR0_3;
    HI_U32         Reserved_VHD1DIEDIR4_7;
    HI_U32         Reserved_VHD1DIEDIR8_11;
    HI_U32         Reserved_VHD1DIEDIR12_14;
    HI_U32         Reserved_ed_98;
    HI_U32         Reserved_VHD1DIESTA;
    HI_U32         Reserved_ed_99[26];
    HI_U32         Reserved_VHD1DIESTKADDR;
    HI_U32         Reserved_VHD1DIESTLADDR;
    HI_U32         Reserved_VHD1DIESTMADDR;
    HI_U32         Reserved_VHD1DIESTNADDR;
    HI_U32         Reserved_VHD1DIESTSQTRADDR;
    HI_U32         Reserved_VHD1CCRSCLR0;
    HI_U32         Reserved_VHD1CCRSCLR1;
    HI_U32         Reserved_ed_100[16];
    HI_U32         Reserved_VHD1PDCTRL;
    HI_U32         Reserved_VHD1PDBLKPOS0;
    HI_U32         Reserved_VHD1PDBLKPOS1;
    HI_U32         Reserved_VHD1PDBLKTHD;
    HI_U32         Reserved_VHD1PDHISTTHD;
    HI_U32         Reserved_VHD1PDUMTHD;
    HI_U32         Reserved_VHD1PDPCCCORING;
    HI_U32         Reserved_VHD1PDPCCHTHD;
    HI_U32         Reserved_VHD1PDPCCVTHD;
    HI_U32         Reserved_VHD1PDITDIFFVTHD;
    HI_U32         Reserved_VHD1PDLASITHD;
    HI_U32         Reserved_VHD1PDFRMITDIFF;
    HI_U32         Reserved_VHD1PDSTLBLKSAD[16];
    HI_U32         Reserved_VHD1PDHISTBIN[4];
    HI_U32         Reserved_VHD1PDUMMATCH0;
    HI_U32         Reserved_VHD1PDUMNOMATCH0;
    HI_U32         Reserved_VHD1PDUMMATCH1;
    HI_U32         Reserved_VHD1PDUMNOMATCH1;
    HI_U32         Reserved_VHD1PDPCCFFWD;
    HI_U32         Reserved_VHD1PDPCCFWD;
    HI_U32         Reserved_VHD1PDPCCBWD;
    HI_U32         Reserved_VHD1PDPCCCRSS;
    HI_U32         Reserved_VHD1PDPCCPW;
    HI_U32         Reserved_VHD1PDPCCFWDTKR;
    HI_U32         Reserved_VHD1PDPCCBWDTKR;
    HI_U32         Reserved_VHD1PDPCCBLKFWD[9];
    HI_U32         Reserved_VHD1PDPCCBLKBWD[9];
    HI_U32         Reserved_VHD1PDLASICNT14;
    HI_U32         Reserved_VHD1PDLASICNT32;
    HI_U32         Reserved_VHD1PDLASICNT34;
    HI_U32         Reserved_VHD1P0RESO;
    HI_U32         Reserved_VHD1P0LADDR;
    HI_U32         Reserved_VHD1P0CADDR;
    HI_U32         Reserved_VHD1P0STRIDE;
    HI_U32         Reserved_VHD1P0VFPOS;
    HI_U32         Reserved_VHD1P0VLPOS;
    HI_U32         Reserved_ed_101[2];
    HI_U32         Reserved_VHD1P1RESO;
    HI_U32         Reserved_VHD1P1LADDR;
    HI_U32         Reserved_VHD1P1CADDR;
    HI_U32         Reserved_VHD1P1STRIDE;
    HI_U32         Reserved_VHD1P1VFPOS;
    HI_U32         Reserved_VHD1P1VLPOS;
    HI_U32         Reserved_ed_102[2];
    HI_U32         Reserved_VHD1P2RESO;
    HI_U32         Reserved_VHD1P2LADDR;
    HI_U32         Reserved_VHD1P2CADDR;
    HI_U32         Reserved_VHD1P2STRIDE;
    HI_U32         Reserved_VHD1P2VFPOS;
    HI_U32         Reserved_VHD1P2VLPOS;
    HI_U32         Reserved_103[2];
    HI_U32         Reserved_VHD1P3RESO;
    HI_U32         Reserved_VHD1P3LADDR;
    HI_U32         Reserved_VHD1P3CADDR;
    HI_U32         Reserved_VHD1P3STRIDE;
    HI_U32         Reserved_VHD1P3VFPOS;
    HI_U32         Reserved_VHD1P3VLPOS;
    HI_U32         Reserved_104[2];
    HI_U32         Reserved_VHD1P4RESO;
    HI_U32         Reserved_VHD1P4LADDR;
    HI_U32         Reserved_VHD1P4CADDR;
    HI_U32         Reserved_VHD1P4STRIDE;
    HI_U32         Reserved_VHD1P4VFPOS;
    HI_U32         Reserved_VHD1P4VLPOS;
    HI_U32         Reserved_105[2];
    HI_U32         Reserved_VHD1P5RESO;
    HI_U32         Reserved_VHD1P5LADDR;
    HI_U32         Reserved_VHD1P5CADDR;
    HI_U32         Reserved_VHD1P5STRIDE;
    HI_U32         Reserved_VHD1P5VFPOS;
    HI_U32         Reserved_VHD1P5VLPOS;
    HI_U32         Reserved_106[2];
    HI_U32         Reserved_VHD1P6RESO;
    HI_U32         Reserved_VHD1P6LADDR;
    HI_U32         Reserved_VHD1P6CADDR;
    HI_U32         Reserved_VHD1P6STRIDE;
    HI_U32         Reserved_VHD1P6VFPOS;
    HI_U32         Reserved_VHD1P6VLPOS;
    HI_U32         Reserved_107[2];
    HI_U32         Reserved_VHD1P7RESO;
    HI_U32         Reserved_VHD1P7LADDR;
    HI_U32         Reserved_VHD1P7CADDR;
    HI_U32         Reserved_VHD1P7STRIDE;
    HI_U32         Reserved_VHD1P7VFPOS;
    HI_U32         Reserved_VHD1P7VLPOS;
    HI_U32         Reserved_108[2];
    HI_U32         Reserved_VHD1P8RESO;
    HI_U32         Reserved_VHD1P8LADDR;
    HI_U32         Reserved_VHD1P8CADDR;
    HI_U32         Reserved_VHD1P8STRIDE;
    HI_U32         Reserved_VHD1P8VFPOS;
    HI_U32         Reserved_VHD1P8VLPOS;
    HI_U32         Reserved_109[2];
    HI_U32         Reserved_VHD1P9RESO;
    HI_U32         Reserved_VHD1P9LADDR;
    HI_U32         Reserved_VHD1P9CADDR;
    HI_U32         Reserved_VHD1P9STRIDE;
    HI_U32         Reserved_VHD1P9VFPOS;
    HI_U32         Reserved_VHD1P9VLPOS;
    HI_U32         Reserved_110[2];
    HI_U32         Reserved_VHD1P10RESO;
    HI_U32         Reserved_VHD1P10LADDR;
    HI_U32         Reserved_VHD1P10CADDR;
    HI_U32         Reserved_VHD1P10STRIDE;
    HI_U32         Reserved_VHD1P10VFPOS;
    HI_U32         Reserved_VHD1P10VLPOS;
    HI_U32         Reserved_111[2];
    HI_U32         Reserved_VHD1P11RESO;
    HI_U32         Reserved_VHD1P11LADDR;
    HI_U32         Reserved_VHD1P11CADDR;
    HI_U32         Reserved_VHD1P11STRIDE;
    HI_U32         Reserved_VHD1P11VFPOS;
    HI_U32         Reserved_VHD1P11VLPOS;
    HI_U32         Reserved_112[2];
    HI_U32         Reserved_VHD1P12RESO;
    HI_U32         Reserved_VHD1P12LADDR;
    HI_U32         Reserved_VHD1P12CADDR;
    HI_U32         Reserved_VHD1P12STRIDE;
    HI_U32         Reserved_VHD1P12VFPOS;
    HI_U32         Reserved_VHD1P12VLPOS;
    HI_U32         Reserved_113[2];
    HI_U32         Reserved_VHD1P13RESO;
    HI_U32         Reserved_VHD1P13LADDR;
    HI_U32         Reserved_VHD1P13CADDR;
    HI_U32         Reserved_VHD1P13STRIDE;
    HI_U32         Reserved_VHD1P13VFPOS;
    HI_U32         Reserved_VHD1P13VLPOS;
    HI_U32         Reserved_114[2];
    HI_U32         Reserved_VHD1P14RESO;
    HI_U32         Reserved_VHD1P14LADDR;
    HI_U32         Reserved_VHD1P14CADDR;
    HI_U32         Reserved_VHD1P14STRIDE;
    HI_U32         Reserved_VHD1P14VFPOS;
    HI_U32         Reserved_VHD1P14VLPOS;
    HI_U32         Reserved_115[2];
    HI_U32         Reserved_VHD1P15RESO;
    HI_U32         Reserved_VHD1P15LADDR;
    HI_U32         Reserved_VHD1P15CADDR;
    HI_U32         Reserved_VHD1P15STRIDE;
    HI_U32         Reserved_VHD1P15VFPOS;
    HI_U32         Reserved_VHD1P15VLPOS;
    HI_U32         Reserved_116[2];
    HI_U32         Reserved_VHD1P16RESO;
    HI_U32         Reserved_VHD1P16LADDR;
    HI_U32         Reserved_VHD1P16CADDR;
    HI_U32         Reserved_VHD1P16STRIDE;
    HI_U32         Reserved_VHD1P16VFPOS;
    HI_U32         Reserved_VHD1P16VLPOS;
    HI_U32         Reserved_117[2];
    HI_U32         Reserved_VHD1P17RESO;
    HI_U32         Reserved_VHD1P17LADDR;
    HI_U32         Reserved_VHD1P17CADDR;
    HI_U32         Reserved_VHD1P17STRIDE;
    HI_U32         Reserved_VHD1P17VFPOS;
    HI_U32         Reserved_VHD1P17VLPOS;
    HI_U32         Reserved_118[2];
    HI_U32         Reserved_VHD1P18RESO;
    HI_U32         Reserved_VHD1P18LADDR;
    HI_U32         Reserved_VHD1P18CADDR;
    HI_U32         Reserved_VHD1P18STRIDE;
    HI_U32         Reserved_VHD1P18VFPOS;
    HI_U32         Reserved_VHD1P18VLPOS;
    HI_U32         Reserved_119[2];
    HI_U32         Reserved_VHD1P19RESO;
    HI_U32         Reserved_VHD1P19LADDR;
    HI_U32         Reserved_VHD1P19CADDR;
    HI_U32         Reserved_VHD1P19STRIDE;
    HI_U32         Reserved_VHD1P19VFPOS;
    HI_U32         Reserved_VHD1P19VLPOS;
    HI_U32         Reserved_120[2];
    HI_U32         Reserved_VHD1P20RESO;
    HI_U32         Reserved_VHD1P20LADDR;
    HI_U32         Reserved_VHD1P20CADDR;
    HI_U32         Reserved_VHD1P20STRIDE;
    HI_U32         Reserved_VHD1P20VFPOS;
    HI_U32         Reserved_VHD1P20VLPOS;
    HI_U32         Reserved_121[2];
    HI_U32         Reserved_VHD1P21RESO;
    HI_U32         Reserved_VHD1P21LADDR;
    HI_U32         Reserved_VHD1P21CADDR;
    HI_U32         Reserved_VHD1P21STRIDE;
    HI_U32         Reserved_VHD1P21VFPOS;
    HI_U32         Reserved_VHD1P21VLPOS;
    HI_U32         Reserved_122[2];
    HI_U32         Reserved_VHD1P22RESO;
    HI_U32         Reserved_VHD1P22LADDR;
    HI_U32         Reserved_VHD1P22CADDR;
    HI_U32         Reserved_VHD1P22STRIDE;
    HI_U32         Reserved_VHD1P22VFPOS;
    HI_U32         Reserved_VHD1P22VLPOS;
    HI_U32         Reserved_123[2];
    HI_U32         Reserved_VHD1P23RESO;
    HI_U32         Reserved_VHD1P23LADDR;
    HI_U32         Reserved_VHD1P23CADDR;
    HI_U32         Reserved_VHD1P23STRIDE;
    HI_U32         Reserved_VHD1P23VFPOS;
    HI_U32         Reserved_VHD1P23VLPOS;
    HI_U32         Reserved_124[2];
    HI_U32         Reserved_VHD1P24RESO;
    HI_U32         Reserved_VHD1P24LADDR;
    HI_U32         Reserved_VHD1P24CADDR;
    HI_U32         Reserved_VHD1P24STRIDE;
    HI_U32         Reserved_VHD1P24VFPOS;
    HI_U32         Reserved_VHD1P24VLPOS;
    HI_U32         Reserved_125[2];
    HI_U32         Reserved_VHD1P25RESO;
    HI_U32         Reserved_VHD1P25LADDR;
    HI_U32         Reserved_VHD1P25CADDR;
    HI_U32         Reserved_VHD1P25STRIDE;
    HI_U32         Reserved_VHD1P25VFPOS;
    HI_U32         Reserved_VHD1P25VLPOS;
    HI_U32         Reserved_126[2];
    HI_U32         Reserved_VHD1P26RESO;
    HI_U32         Reserved_VHD1P26LADDR;
    HI_U32         Reserved_VHD1P26CADDR;
    HI_U32         Reserved_VHD1P26STRIDE;
    HI_U32         Reserved_VHD1P26VFPOS;
    HI_U32         Reserved_VHD1P26VLPOS;
    HI_U32         Reserved_127[2];
    HI_U32         Reserved_VHD1P27RESO;
    HI_U32         Reserved_VHD1P27LADDR;
    HI_U32         Reserved_VHD1P27CADDR;
    HI_U32         Reserved_VHD1P27STRIDE;
    HI_U32         Reserved_VHD1P27VFPOS;
    HI_U32         Reserved_VHD1P27VLPOS;
    HI_U32         Reserved_128[2];
    HI_U32         Reserved_VHD1P28RESO;
    HI_U32         Reserved_VHD1P28LADDR;
    HI_U32         Reserved_VHD1P28CADDR;
    HI_U32         Reserved_VHD1P28STRIDE;
    HI_U32         Reserved_VHD1P28VFPOS;
    HI_U32         Reserved_VHD1P28VLPOS;
    HI_U32         Reserved_129[2];
    HI_U32         Reserved_VHD1P29RESO;
    HI_U32         Reserved_VHD1P29LADDR;
    HI_U32         Reserved_VHD1P29CADDR;
    HI_U32         Reserved_VHD1P29STRIDE;
    HI_U32         Reserved_VHD1P29VFPOS;
    HI_U32         Reserved_VHD1P29VLPOS;
    HI_U32         Reserved_130[2];
    HI_U32         Reserved_VHD1P30RESO;
    HI_U32         Reserved_VHD1P30LADDR;
    HI_U32         Reserved_VHD1P30CADDR;
    HI_U32         Reserved_VHD1P30STRIDE;
    HI_U32         Reserved_VHD1P30VFPOS;
    HI_U32         Reserved_VHD1P30VLPOS;
    HI_U32         Reserved_131[2];
    HI_U32         Reserved_VHD1P31RESO;
    HI_U32         Reserved_VHD1P31LADDR;
    HI_U32         Reserved_VHD1P31CADDR;
    HI_U32         Reserved_VHD1P31STRIDE;
    HI_U32         Reserved_VHD1P31VFPOS;
    HI_U32         Reserved_VHD1P31VLPOS;
    HI_U32         Reserved_132[2];
    HI_U32         Reserved_VHD1P32RESO;
    HI_U32         Reserved_VHD1P32LADDR;
    HI_U32         Reserved_VHD1P32CADDR;
    HI_U32         Reserved_VHD1P32STRIDE;
    HI_U32         Reserved_VHD1P32VFPOS;
    HI_U32         Reserved_VHD1P32VLPOS;
    HI_U32         Reserved_133[2];
    HI_U32         Reserved_VHD1P33RESO;
    HI_U32         Reserved_VHD1P33LADDR;
    HI_U32         Reserved_VHD1P33CADDR;
    HI_U32         Reserved_VHD1P33STRIDE;
    HI_U32         Reserved_VHD1P33VFPOS;
    HI_U32         Reserved_VHD1P33VLPOS;
    HI_U32         Reserved_134[2];
    HI_U32         Reserved_VHD1P34RESO;
    HI_U32         Reserved_VHD1P34LADDR;
    HI_U32         Reserved_VHD1P34CADDR;
    HI_U32         Reserved_VHD1P34STRIDE;
    HI_U32         Reserved_VHD1P34VFPOS;
    HI_U32         Reserved_VHD1P34VLPOS;
    HI_U32         Reserved_135[2];
    HI_U32         Reserved_VHD1P35RESO;
    HI_U32         Reserved_VHD1P35LADDR;
    HI_U32         Reserved_VHD1P35CADDR;
    HI_U32         Reserved_VHD1P35STRIDE;
    HI_U32         Reserved_VHD1P35VFPOS;
    HI_U32         Reserved_VHD1P35VLPOS;
    HI_U32         Reserved_136[2];
    HI_U32         Reserved_VHD1P36RESO;
    HI_U32         Reserved_VHD1P36LADDR;
    HI_U32         Reserved_VHD1P36CADDR;
    HI_U32         Reserved_VHD1P36STRIDE;
    HI_U32         Reserved_VHD1P36VFPOS;
    HI_U32         Reserved_VHD1P36VLPOS;
    HI_U32         Reserved_137[2];
    HI_U32         Reserved_VHD1P37RESO;
    HI_U32         Reserved_VHD1P37LADDR;
    HI_U32         Reserved_VHD1P37CADDR;
    HI_U32         Reserved_VHD1P37STRIDE;
    HI_U32         Reserved_VHD1P37VFPOS;
    HI_U32         Reserved_VHD1P37VLPOS;
    HI_U32         Reserved_138[2];
    HI_U32         Reserved_VHD1P38RESO;
    HI_U32         Reserved_VHD1P38LADDR;
    HI_U32         Reserved_VHD1P38CADDR;
    HI_U32         Reserved_VHD1P38STRIDE;
    HI_U32         Reserved_VHD1P38VFPOS;
    HI_U32         Reserved_VHD1P38VLPOS;
    HI_U32         Reserved_139[2];
    HI_U32         Reserved_VHD1P39RESO;
    HI_U32         Reserved_VHD1P39LADDR;
    HI_U32         Reserved_VHD1P39CADDR;
    HI_U32         Reserved_VHD1P39STRIDE;
    HI_U32         Reserved_VHD1P39VFPOS;
    HI_U32         Reserved_VHD1P39VLPOS;
    HI_U32         Reserved_140[2];
    HI_U32         Reserved_VHD1P40RESO;
    HI_U32         Reserved_VHD1P40LADDR;
    HI_U32         Reserved_VHD1P40CADDR;
    HI_U32         Reserved_VHD1P40STRIDE;
    HI_U32         Reserved_VHD1P40VFPOS;
    HI_U32         Reserved_VHD1P40VLPOS;
    HI_U32         Reserved_141[2];
    HI_U32         Reserved_VHD1P41RESO;
    HI_U32         Reserved_VHD1P41LADDR;
    HI_U32         Reserved_VHD1P41CADDR;
    HI_U32         Reserved_VHD1P41STRIDE;
    HI_U32         Reserved_VHD1P41VFPOS;
    HI_U32         Reserved_VHD1P41VLPOS;
    HI_U32         Reserved_142[2];
    HI_U32         Reserved_VHD1P42RESO;
    HI_U32         Reserved_VHD1P42LADDR;
    HI_U32         Reserved_VHD1P42CADDR;
    HI_U32         Reserved_VHD1P42STRIDE;
    HI_U32         Reserved_VHD1P42VFPOS;
    HI_U32         Reserved_VHD1P42VLPOS;
    HI_U32         Reserved_143[2];
    HI_U32         Reserved_VHD1P43RESO;
    HI_U32         Reserved_VHD1P43LADDR;
    HI_U32         Reserved_VHD1P43CADDR;
    HI_U32         Reserved_VHD1P43STRIDE;
    HI_U32         Reserved_VHD1P43VFPOS;
    HI_U32         Reserved_VHD1P43VLPOS;
    HI_U32         Reserved_144[2];
    HI_U32         Reserved_VHD1P44RESO;
    HI_U32         Reserved_VHD1P44LADDR;
    HI_U32         Reserved_VHD1P44CADDR;
    HI_U32         Reserved_VHD1P44STRIDE;
    HI_U32         Reserved_VHD1P44VFPOS;
    HI_U32         Reserved_VHD1P44VLPOS;
    HI_U32         Reserved_145[2];
    HI_U32         Reserved_VHD1P45RESO;
    HI_U32         Reserved_VHD1P45LADDR;
    HI_U32         Reserved_VHD1P45CADDR;
    HI_U32         Reserved_VHD1P45STRIDE;
    HI_U32         Reserved_VHD1P45VFPOS;
    HI_U32         Reserved_VHD1P45VLPOS;
    HI_U32         Reserved_146[2];
    HI_U32         Reserved_VHD1P46RESO;
    HI_U32         Reserved_VHD1P46LADDR;
    HI_U32         Reserved_VHD1P46CADDR;
    HI_U32         Reserved_VHD1P46STRIDE;
    HI_U32         Reserved_VHD1P46VFPOS;
    HI_U32         Reserved_VHD1P46VLPOS;
    HI_U32         Reserved_147[2];
    HI_U32         Reserved_VHD1P47RESO;
    HI_U32         Reserved_VHD1P47LADDR;
    HI_U32         Reserved_VHD1P47CADDR;
    HI_U32         Reserved_VHD1P47STRIDE;
    HI_U32         Reserved_VHD1P47VFPOS;
    HI_U32         Reserved_VHD1P47VLPOS;
    HI_U32         Reserved_148[2];
    HI_U32         Reserved_VHD1P48RESO;
    HI_U32         Reserved_VHD1P48LADDR;
    HI_U32         Reserved_VHD1P48CADDR;
    HI_U32         Reserved_VHD1P48STRIDE;
    HI_U32         Reserved_VHD1P48VFPOS;
    HI_U32         Reserved_VHD1P48VLPOS;
    HI_U32         Reserved_149[2];
    HI_U32         Reserved_VHD1P49RESO;
    HI_U32         Reserved_VHD1P49LADDR;
    HI_U32         Reserved_VHD1P49CADDR;
    HI_U32         Reserved_VHD1P49STRIDE;
    HI_U32         Reserved_VHD1P49VFPOS;
    HI_U32         Reserved_VHD1P49VLPOS;
    HI_U32         Reserved_150[2];
    HI_U32         Reserved_VHD1P50RESO;
    HI_U32         Reserved_VHD1P50LADDR;
    HI_U32         Reserved_VHD1P50CADDR;
    HI_U32         Reserved_VHD1P50STRIDE;
    HI_U32         Reserved_VHD1P50VFPOS;
    HI_U32         Reserved_VHD1P50VLPOS;
    HI_U32         Reserved_151[2];
    HI_U32         Reserved_VHD1P51RESO;
    HI_U32         Reserved_VHD1P51LADDR;
    HI_U32         Reserved_VHD1P51CADDR;
    HI_U32         Reserved_VHD1P51STRIDE;
    HI_U32         Reserved_VHD1P51VFPOS;
    HI_U32         Reserved_VHD1P51VLPOS;
    HI_U32         Reserved_152[2];
    HI_U32         Reserved_VHD1P52RESO;
    HI_U32         Reserved_VHD1P52LADDR;
    HI_U32         Reserved_VHD1P52CADDR;
    HI_U32         Reserved_VHD1P52STRIDE;
    HI_U32         Reserved_VHD1P52VFPOS;
    HI_U32         Reserved_VHD1P52VLPOS;
    HI_U32         Reserved_153[2];
    HI_U32         Reserved_VHD1P53RESO;
    HI_U32         Reserved_VHD1P53LADDR;
    HI_U32         Reserved_VHD1P53CADDR;
    HI_U32         Reserved_VHD1P53STRIDE;
    HI_U32         Reserved_VHD1P53VFPOS;
    HI_U32         Reserved_VHD1P53VLPOS;
    HI_U32         Reserved_154[2];
    HI_U32         Reserved_VHD1P54RESO;
    HI_U32         Reserved_VHD1P54LADDR;
    HI_U32         Reserved_VHD1P54CADDR;
    HI_U32         Reserved_VHD1P54STRIDE;
    HI_U32         Reserved_VHD1P54VFPOS;
    HI_U32         Reserved_VHD1P54VLPOS;
    HI_U32         Reserved_155[2];
    HI_U32         Reserved_VHD1P55RESO;
    HI_U32         Reserved_VHD1P55LADDR;
    HI_U32         Reserved_VHD1P55CADDR;
    HI_U32         Reserved_VHD1P55STRIDE;
    HI_U32         Reserved_VHD1P55VFPOS;
    HI_U32         Reserved_VHD1P55VLPOS;
    HI_U32         Reserved_156[2];
    HI_U32         Reserved_VHD1P56RESO;
    HI_U32         Reserved_VHD1P56LADDR;
    HI_U32         Reserved_VHD1P56CADDR;
    HI_U32         Reserved_VHD1P56STRIDE;
    HI_U32         Reserved_VHD1P56VFPOS;
    HI_U32         Reserved_VHD1P56VLPOS;
    HI_U32         Reserved_157[2];
    HI_U32         Reserved_VHD1P57RESO;
    HI_U32         Reserved_VHD1P57LADDR;
    HI_U32         Reserved_VHD1P57CADDR;
    HI_U32         Reserved_VHD1P57STRIDE;
    HI_U32         Reserved_VHD1P57VFPOS;
    HI_U32         Reserved_VHD1P57VLPOS;
    HI_U32         Reserved_158[2];
    HI_U32         Reserved_VHD1P58RESO;
    HI_U32         Reserved_VHD1P58LADDR;
    HI_U32         Reserved_VHD1P58CADDR;
    HI_U32         Reserved_VHD1P58STRIDE;
    HI_U32         Reserved_VHD1P58VFPOS;
    HI_U32         Reserved_VHD1P58VLPOS;
    HI_U32         Reserved_159[2];
    HI_U32         Reserved_VHD1P59RESO;
    HI_U32         Reserved_VHD1P59LADDR;
    HI_U32         Reserved_VHD1P59CADDR;
    HI_U32         Reserved_VHD1P59STRIDE;
    HI_U32         Reserved_VHD1P59VFPOS;
    HI_U32         Reserved_VHD1P59VLPOS;
    HI_U32         Reserved_160[2];
    HI_U32         Reserved_VHD1P60RESO;
    HI_U32         Reserved_VHD1P60LADDR;
    HI_U32         Reserved_VHD1P60CADDR;
    HI_U32         Reserved_VHD1P60STRIDE;
    HI_U32         Reserved_VHD1P60VFPOS;
    HI_U32         Reserved_VHD1P60VLPOS;
    HI_U32         Reserved_161[2];
    HI_U32         Reserved_VHD1P61RESO;
    HI_U32         Reserved_VHD1P61LADDR;
    HI_U32         Reserved_VHD1P61CADDR;
    HI_U32         Reserved_VHD1P61STRIDE;
    HI_U32         Reserved_VHD1P61VFPOS;
    HI_U32         Reserved_VHD1P61VLPOS;
    HI_U32         Reserved_162[2];
    HI_U32         Reserved_VHD1P62RESO;
    HI_U32         Reserved_VHD1P62LADDR;
    HI_U32         Reserved_VHD1P62CADDR;
    HI_U32         Reserved_VHD1P62STRIDE;
    HI_U32         Reserved_VHD1P62VFPOS;
    HI_U32         Reserved_VHD1P62VLPOS;
    HI_U32         Reserved_163[2];
    HI_U32         Reserved_HD1P63RESO;
    HI_U32         Reserved_HD1P63LADDR;
    HI_U32         Reserved_HD1P63CADDR;
    HI_U32         Reserved_HD1P63STRIDE;
    HI_U32         Reserved_HD1P63VFPOS;
    HI_U32         Reserved_HD1P63VLPOS;
    HI_U32         Reserved_164[2];
    HI_U32         Reserved_VHD164REGIONENH;
    HI_U32         Reserved_VHD164REGIONENL;
    HI_U32         Reserved_165[254];
    HI_U32         Reserved_VHD2CTRL;
    HI_U32         Reserved_VHD2UPD;
    HI_U32         Reserved_VHD2LADDR;
    HI_U32         Reserved_VHD2LCADDR;
    HI_U32         Reserved_VHD2CADDR;
    HI_U32         Reserved_VHD2CCADDR;
    HI_U32         Reserved_VHD2NADDR;
    HI_U32         Reserved_VHD2NCADDR;
    HI_U32         Reserved_ed_166;
    HI_U32         Reserved_VHD2STRIDE;
    HI_U32         Reserved_VHD2IRESO;
    HI_U32         Reserved_ed_167;
    HI_U32         Reserved_VHD2LKEY;
    HI_U32         Reserved_VHD2CBMPARA;
    HI_U32         Reserved_VHD2DIEADDR;
    HI_U32         Reserved_VHD2WBC1ADDR;
    HI_U32         Reserved_VHD2WBC1STRD;
    HI_U32         Reserved_VHD2DIESTADDR;
    HI_U32         Reserved_168[6];
    HI_U32         Reserved_VHD2DFPOS;
    HI_U32         Reserved_VHD2DLPOS;
    HI_U32         Reserved_VHD2VFPOS;
    HI_U32         Reserved_VHD2VLPOS;
    HI_U32         Reserved_VHD2BK;
    HI_U32         Reserved_ed_169[3];
    HI_U32         Reserved_VHD2CSCIDC;
    HI_U32         Reserved_VHD2CSCODC;
    HI_U32         Reserved_VHD2CSCP0;
    HI_U32         Reserved_VHD2CSCP1;
    HI_U32         Reserved_VHD2CSCP2;
    HI_U32         Reserved_VHD2CSCP3;
    HI_U32         Reserved_VHD2CSCP4;
    HI_U32         Reserved_ed_170;
    HI_U32         Reserved_VHD2ACM0;
    HI_U32         Reserved_VHD2ACM1;
    HI_U32         Reserved_VHD2ACM2;
    HI_U32         Reserved_VHD2ACM3;
    HI_U32         Reserved_VHD2ACM4;
    HI_U32         Reserved_VHD2ACM5;
    HI_U32         Reserved_VHD2ACM6;
    HI_U32         Reserved_VHD2ACM7;
    HI_U32         Reserved_VHD2HSP;
    HI_U32         Reserved_VHD2HLOFFSET;
    HI_U32         Reserved_VHD2HCOFFSET;
    HI_U32         Reserved_ed_171[3];
    HI_U32         Reserved_VHD2VSP;
    HI_U32         Reserved_VHD2VSR;
    HI_U32         Reserved_VHD2VOFFSET;
    HI_U32         Reserved_VHD2ZMEORESO;
    HI_U32         Reserved_VHD2ZMEIRESO;
    HI_U32         Reserved_VHD2ZMEDBG;
    HI_U32         Reserved_VHD2COEF00;
    HI_U32         Reserved_VHD2COEF01;
    HI_U32         Reserved_VHD2COEF10;
    HI_U32         Reserved_VHD2COEF11;
    HI_U32         Reserved_VHD2ACCTHD1;
    HI_U32         Reserved_VHD2ACCTHD2;
    HI_U32         Reserved_ed_172[2];
    HI_U32         Reserved_VHD2ACCLOWN[3];
    HI_U32         Reserved_ed_173;
    HI_U32         Reserved_VHD2ACCMEDN[3];
    HI_U32         Reserved_174;
    HI_U32         Reserved_VHD2ACCHIGHN[3];
    HI_U32         Reserved_ed_175;
    HI_U32         Reserved_VHD2ACCMLN[3];
    HI_U32         Reserved_ed_176;
    HI_U32         Reserved_VHD2ACCMHN[3];
    HI_U32         Reserved_ed_177;
    HI_U32         Reserved_VHD2ACC3LOW;
    HI_U32         Reserved_VHD2ACC3MED;
    HI_U32         Reserved_VHD2ACC3HIGH;
    HI_U32         Reserved_VHD2ACC8MLOW;
    HI_U32         Reserved_VHD2ACC8MHIGH;
    HI_U32         Reserved_VHD2ACCTOTAL;
    HI_U32         Reserved_178[2];
    HI_U32         Reserved_VHD2IFIRCOEF01;
    HI_U32         Reserved_VHD2IFIRCOEF23;
    HI_U32         Reserved_VHD2IFIRCOEF45;
    HI_U32         Reserved_VHD2IFIRCOEF67;
    HI_U32         Reserved_ed_179[28];
    HI_U32         Reserved_VHD2DIECTRL;
    HI_U32         Reserved_180;
    HI_U32         Reserved_VHD2DIELMA0;
    HI_U32         Reserved_VHD2DIELMA1;
    HI_U32         Reserved_VHD2DIELMA2;
    HI_U32         Reserved_VHD2DIEINTEN;
    HI_U32         Reserved_VHD2DIESCALE;
    HI_U32         Reserved_VHD2DIECHECK1;
    HI_U32         Reserved_VHD2DIECHECK2;
    HI_U32         Reserved_VHD2DIEDIR0_3;
    HI_U32         Reserved_VHD2DIEDIR4_7;
    HI_U32         Reserved_VHD2DIEDIR8_11;
    HI_U32         Reserved_VHD2DIEDIR12_14;
    HI_U32         Reserved_ed_181;
    HI_U32         Reserved_VHD2DIESTA;
    HI_U32         Reserved_ed_182[26];
    HI_U32         Reserved_VHD2DIESTKADDR;
    HI_U32         Reserved_VHD2DIESTLADDR;
    HI_U32         Reserved_VHD2DIESTMADDR;
    HI_U32         Reserved_VHD2DIESTNADDR;
    HI_U32         Reserved_VHD2DIESTSQTRADDR;
    HI_U32         Reserved_VHD2CCRSCLR0;
    HI_U32         Reserved_VHD2CCRSCLR1;
    HI_U32         Reserved_ed_183[16];
    HI_U32         Reserved_VHD2PDCTRL;
    HI_U32         Reserved_VHD2PDBLKPOS0;
    HI_U32         Reserved_VHD2PDBLKPOS1;
    HI_U32         Reserved_VHD2PDBLKTHD;
    HI_U32         Reserved_VHD2PDHISTTHD;
    HI_U32         Reserved_VHD2PDUMTHD;
    HI_U32         Reserved_VHD2PDPCCCORING;
    HI_U32         Reserved_VHD2PDPCCHTHD;
    HI_U32         Reserved_VHD2PDPCCVTHD;
    HI_U32         Reserved_VHD2PDITDIFFVTHD;
    HI_U32         Reserved_VHD2PDLASITHD;
    HI_U32         Reserved_VHD2PDFRMITDIFF;
    HI_U32         Reserved_VHD2PDSTLBLKSAD[16];
    HI_U32         Reserved_VHD2PDHISTBIN[4];
    HI_U32         Reserved_VHD2PDUMMATCH0;
    HI_U32         Reserved_VHD2PDUMNOMATCH0;
    HI_U32         Reserved_VHD2PDUMMATCH1;
    HI_U32         Reserved_VHD2PDUMNOMATCH1;
    HI_U32         Reserved_VHD2PDPCCFFWD;
    HI_U32         Reserved_VHD2PDPCCFWD;
    HI_U32         Reserved_VHD2PDPCCBWD;
    HI_U32         Reserved_VHD2PDPCCCRSS;
    HI_U32         Reserved_VHD2PDPCCPW;
    HI_U32         Reserved_VHD2PDPCCFWDTKR;
    HI_U32         Reserved_VHD2PDPCCBWDTKR;
    HI_U32         Reserved_VHD2PDPCCBLKFWD[9];
    HI_U32         Reserved_VHD2PDPCCBLKBWD[9];
    HI_U32         Reserved_VHD2PDLASICNT14;
    HI_U32         Reserved_VHD2PDLASICNT32;
    HI_U32         Reserved_VHD2PDLASICNT34;
    HI_U32         Reserved_VHD2P0RESO;
    HI_U32         Reserved_VHD2P0LADDR;
    HI_U32         Reserved_VHD2P0CADDR;
    HI_U32         Reserved_VHD2P0STRIDE;
    HI_U32         Reserved_VHD2P0VFPOS;
    HI_U32         Reserved_VHD2P0VLPOS;
    HI_U32         Reserved_ed_184[2];
    HI_U32         Reserved_VHD2P1RESO;
    HI_U32         Reserved_VHD2P1LADDR;
    HI_U32         Reserved_VHD2P1CADDR;
    HI_U32         Reserved_VHD2P1STRIDE;
    HI_U32         Reserved_VHD2P1VFPOS;
    HI_U32         Reserved_VHD2P1VLPOS;
    HI_U32         Reserved_ed_185[2];
    HI_U32         Reserved_VHD2P2RESO;
    HI_U32         Reserved_VHD2P2LADDR;
    HI_U32         Reserved_VHD2P2CADDR;
    HI_U32         Reserved_VHD2P2STRIDE;
    HI_U32         Reserved_VHD2P2VFPOS;
    HI_U32         Reserved_VHD2P2VLPOS;
    HI_U32         Reserved_186[2];
    HI_U32         Reserved_VHD2P3RESO;
    HI_U32         Reserved_VHD2P3LADDR;
    HI_U32         Reserved_VHD2P3CADDR;
    HI_U32         Reserved_VHD2P3STRIDE;
    HI_U32         Reserved_VHD2P3VFPOS;
    HI_U32         Reserved_VHD2P3VLPOS;
    HI_U32         Reserved_187[2];
    HI_U32         Reserved_VHD2P4RESO;
    HI_U32         Reserved_VHD2P4LADDR;
    HI_U32         Reserved_VHD2P4CADDR;
    HI_U32         Reserved_VHD2P4STRIDE;
    HI_U32         Reserved_VHD2P4VFPOS;
    HI_U32         Reserved_VHD2P4VLPOS;
    HI_U32         Reserved_188[2];
    HI_U32         Reserved_VHD2P5RESO;
    HI_U32         Reserved_VHD2P5LADDR;
    HI_U32         Reserved_VHD2P5CADDR;
    HI_U32         Reserved_VHD2P5STRIDE;
    HI_U32         Reserved_VHD2P5VFPOS;
    HI_U32         Reserved_VHD2P5VLPOS;
    HI_U32         Reserved_189[2];
    HI_U32         Reserved_VHD2P6RESO;
    HI_U32         Reserved_VHD2P6LADDR;
    HI_U32         Reserved_VHD2P6CADDR;
    HI_U32         Reserved_VHD2P6STRIDE;
    HI_U32         Reserved_VHD2P6VFPOS;
    HI_U32         Reserved_VHD2P6VLPOS;
    HI_U32         Reserved_190[2];
    HI_U32         Reserved_VHD2P7RESO;
    HI_U32         Reserved_VHD2P7LADDR;
    HI_U32         Reserved_VHD2P7CADDR;
    HI_U32         Reserved_VHD2P7STRIDE;
    HI_U32         Reserved_VHD2P7VFPOS;
    HI_U32         Reserved_VHD2P7VLPOS;
    HI_U32         Reserved_191[2];
    HI_U32         Reserved_HD2P8RESO;
    HI_U32         Reserved_HD2P8LADDR;
    HI_U32         Reserved_HD2P8CADDR;
    HI_U32         Reserved_HD2P8STRIDE;
    HI_U32         Reserved_HD2P8VFPOS;
    HI_U32         Reserved_HD2P8VLPOS;
    HI_U32         Reserved_192[2];
    HI_U32         Reserved_VHD2P9RESO;
    HI_U32         Reserved_VHD2P9LADDR;
    HI_U32         Reserved_VHD2P9CADDR;
    HI_U32         Reserved_VHD2P9STRIDE;
    HI_U32         Reserved_VHD2P9VFPOS;
    HI_U32         Reserved_VHD2P9VLPOS;
    HI_U32         Reserved_193[2];
    HI_U32         Reserved_VHD2P10RESO;
    HI_U32         Reserved_VHD2P10LADDR;
    HI_U32         Reserved_VHD2P10CADDR;
    HI_U32         Reserved_VHD2P10STRIDE;
    HI_U32         Reserved_VHD2P10VFPOS;
    HI_U32         Reserved_VHD2P10VLPOS;
    HI_U32         Reserved_194[2];
    HI_U32         Reserved_VHD2P11RESO;
    HI_U32         Reserved_VHD2P11LADDR;
    HI_U32         Reserved_VHD2P11CADDR;
    HI_U32         Reserved_VHD2P11STRIDE;
    HI_U32         Reserved_VHD2P11VFPOS;
    HI_U32         Reserved_VHD2P11VLPOS;
    HI_U32         Reserved_195[2];
    HI_U32         Reserved_VHD2P12RESO;
    HI_U32         Reserved_VHD2P12LADDR;
    HI_U32         Reserved_VHD2P12CADDR;
    HI_U32         Reserved_VHD2P12STRIDE;
    HI_U32         Reserved_VHD2P12VFPOS;
    HI_U32         Reserved_VHD2P12VLPOS;
    HI_U32         Reserved_196[2];
    HI_U32         Reserved_VHD2P13RESO;
    HI_U32         Reserved_VHD2P13LADDR;
    HI_U32         Reserved_VHD2P13CADDR;
    HI_U32         Reserved_VHD2P13STRIDE;
    HI_U32         Reserved_VHD2P13VFPOS;
    HI_U32         Reserved_VHD2P13VLPOS;
    HI_U32         Reserved_197[2];
    HI_U32         Reserved_VHD2P14RESO;
    HI_U32         Reserved_VHD2P14LADDR;
    HI_U32         Reserved_VHD2P14CADDR;
    HI_U32         Reserved_VHD2P14STRIDE;
    HI_U32         Reserved_VHD2P14VFPOS;
    HI_U32         Reserved_VHD2P14VLPOS;
    HI_U32         Reserved_198[2];
    HI_U32         Reserved_VHD2P15RESO;
    HI_U32         Reserved_VHD2P15LADDR;
    HI_U32         Reserved_VHD2P15CADDR;
    HI_U32         Reserved_VHD2P15STRIDE;
    HI_U32         Reserved_VHD2P15VFPOS;
    HI_U32         Reserved_VHD2P15VLPOS;
    HI_U32         Reserved_199[2];
    HI_U32         Reserved_VHD2P16RESO;
    HI_U32         Reserved_VHD2P16LADDR;
    HI_U32         Reserved_VHD2P16CADDR;
    HI_U32         Reserved_VHD2P16STRIDE;
    HI_U32         Reserved_VHD2P16VFPOS;
    HI_U32         Reserved_VHD2P16VLPOS;
    HI_U32         Reserved_200[2];
    HI_U32         Reserved_VHD2P17RESO;
    HI_U32         Reserved_VHD2P17LADDR;
    HI_U32         Reserved_VHD2P17CADDR;
    HI_U32         Reserved_VHD2P17STRIDE;
    HI_U32         Reserved_VHD2P17VFPOS;
    HI_U32         Reserved_VHD2P17VLPOS;
    HI_U32         Reserved_201[2];
    HI_U32         Reserved_VHD2P18RESO;
    HI_U32         Reserved_VHD2P18LADDR;
    HI_U32         Reserved_VHD2P18CADDR;
    HI_U32         Reserved_VHD2P18STRIDE;
    HI_U32         Reserved_VHD2P18VFPOS;
    HI_U32         Reserved_VHD2P18VLPOS;
    HI_U32         Reserved_202[2];
    HI_U32         Reserved_VHD2P19RESO;
    HI_U32         Reserved_VHD2P19LADDR;
    HI_U32         Reserved_VHD2P19CADDR;
    HI_U32         Reserved_VHD2P19STRIDE;
    HI_U32         Reserved_VHD2P19VFPOS;
    HI_U32         Reserved_VHD2P19VLPOS;
    HI_U32         Reserved_203[2];
    HI_U32         Reserved_VHD2P20RESO;
    HI_U32         Reserved_VHD2P20LADDR;
    HI_U32         Reserved_VHD2P20CADDR;
    HI_U32         Reserved_VHD2P20STRIDE;
    HI_U32         Reserved_VHD2P20VFPOS;
    HI_U32         Reserved_VHD2P20VLPOS;
    HI_U32         Reserved_204[2];
    HI_U32         Reserved_VHD2P21RESO;
    HI_U32         Reserved_VHD2P21LADDR;
    HI_U32         Reserved_VHD2P21CADDR;
    HI_U32         Reserved_VHD2P21STRIDE;
    HI_U32         Reserved_VHD2P21VFPOS;
    HI_U32         Reserved_VHD2P21VLPOS;
    HI_U32         Reserved_205[2];
    HI_U32         Reserved_VHD2P22RESO;
    HI_U32         Reserved_VHD2P22LADDR;
    HI_U32         Reserved_VHD2P22CADDR;
    HI_U32         Reserved_VHD2P22STRIDE;
    HI_U32         Reserved_VHD2P22VFPOS;
    HI_U32         Reserved_VHD2P22VLPOS;
    HI_U32         Reserved_206[2];
    HI_U32         Reserved_VHD2P23RESO;
    HI_U32         Reserved_VHD2P23LADDR;
    HI_U32         Reserved_VHD2P23CADDR;
    HI_U32         Reserved_VHD2P23STRIDE;
    HI_U32         Reserved_VHD2P23VFPOS;
    HI_U32         Reserved_VHD2P23VLPOS;
    HI_U32         Reserved_207[2];
    HI_U32         Reserved_VHD2P24RESO;
    HI_U32         Reserved_VHD2P24LADDR;
    HI_U32         Reserved_VHD2P24CADDR;
    HI_U32         Reserved_VHD2P24STRIDE;
    HI_U32         Reserved_VHD2P24VFPOS;
    HI_U32         Reserved_VHD2P24VLPOS;
    HI_U32         Reserved_208[2];
    HI_U32         Reserved_VHD2P25RESO;
    HI_U32         Reserved_VHD2P25LADDR;
    HI_U32         Reserved_VHD2P25CADDR;
    HI_U32         Reserved_VHD2P25STRIDE;
    HI_U32         Reserved_VHD2P25VFPOS;
    HI_U32         Reserved_VHD2P25VLPOS;
    HI_U32         Reserved_209[2];
    HI_U32         Reserved_VHD2P26RESO;
    HI_U32         Reserved_VHD2P26LADDR;
    HI_U32         Reserved_VHD2P26CADDR;
    HI_U32         Reserved_VHD2P26STRIDE;
    HI_U32         Reserved_VHD2P26VFPOS;
    HI_U32         Reserved_VHD2P26VLPOS;
    HI_U32         Reserved_210[2];
    HI_U32         Reserved_VHD2P27RESO;
    HI_U32         Reserved_VHD2P27LADDR;
    HI_U32         Reserved_VHD2P27CADDR;
    HI_U32         Reserved_VHD2P27STRIDE;
    HI_U32         Reserved_VHD2P27VFPOS;
    HI_U32         Reserved_VHD2P27VLPOS;
    HI_U32         Reserved_211[2];
    HI_U32         Reserved_VHD2P28RESO;
    HI_U32         Reserved_VHD2P28LADDR;
    HI_U32         Reserved_VHD2P28CADDR;
    HI_U32         Reserved_VHD2P28STRIDE;
    HI_U32         Reserved_VHD2P28VFPOS;
    HI_U32         Reserved_VHD2P28VLPOS;
    HI_U32         Reserved_212[2];
    HI_U32         Reserved_VHD2P29RESO;
    HI_U32         Reserved_VHD2P29LADDR;
    HI_U32         Reserved_VHD2P29CADDR;
    HI_U32         Reserved_VHD2P29STRIDE;
    HI_U32         Reserved_VHD2P29VFPOS;
    HI_U32         Reserved_VHD2P29VLPOS;
    HI_U32         Reserved_213[2];
    HI_U32         Reserved_VHD2P30RESO;
    HI_U32         Reserved_VHD2P30LADDR;
    HI_U32         Reserved_VHD2P30CADDR;
    HI_U32         Reserved_VHD2P30STRIDE;
    HI_U32         Reserved_VHD2P30VFPOS;
    HI_U32         Reserved_VHD2P30VLPOS;
    HI_U32         Reserved_214[2];
    HI_U32         Reserved_VHD2P31RESO;
    HI_U32         Reserved_VHD2P31LADDR;
    HI_U32         Reserved_VHD2P31CADDR;
    HI_U32         Reserved_VHD2P31STRIDE;
    HI_U32         Reserved_VHD2P31VFPOS;
    HI_U32         Reserved_VHD2P31VLPOS;
    HI_U32         Reserved_215[2];
    HI_U32         Reserved_VHD2P32RESO;
    HI_U32         Reserved_VHD2P32LADDR;
    HI_U32         Reserved_VHD2P32CADDR;
    HI_U32         Reserved_VHD2P32STRIDE;
    HI_U32         Reserved_VHD2P32VFPOS;
    HI_U32         Reserved_VHD2P32VLPOS;
    HI_U32         Reserved_216[2];
    HI_U32         Reserved_VHD2P33RESO;
    HI_U32         Reserved_VHD2P33LADDR;
    HI_U32         Reserved_VHD2P33CADDR;
    HI_U32         Reserved_VHD2P33STRIDE;
    HI_U32         Reserved_VHD2P33VFPOS;
    HI_U32         Reserved_VHD2P33VLPOS;
    HI_U32         Reserved_217[2];
    HI_U32         Reserved_VHD2P34RESO;
    HI_U32         Reserved_VHD2P34LADDR;
    HI_U32         Reserved_VHD2P34CADDR;
    HI_U32         Reserved_VHD2P34STRIDE;
    HI_U32         Reserved_VHD2P34VFPOS;
    HI_U32         Reserved_VHD2P34VLPOS;
    HI_U32         Reserved_218[2];
    HI_U32         Reserved_VHD2P35RESO;
    HI_U32         Reserved_VHD2P35LADDR;
    HI_U32         Reserved_VHD2P35CADDR;
    HI_U32         Reserved_VHD2P35STRIDE;
    HI_U32         Reserved_VHD2P35VFPOS;
    HI_U32         Reserved_VHD2P35VLPOS;
    HI_U32         Reserved_219[2];
    HI_U32         Reserved_VHD2P36RESO;
    HI_U32         Reserved_VHD2P36LADDR;
    HI_U32         Reserved_VHD2P36CADDR;
    HI_U32         Reserved_VHD2P36STRIDE;
    HI_U32         Reserved_VHD2P36VFPOS;
    HI_U32         Reserved_VHD2P36VLPOS;
    HI_U32         Reserved_220[2];
    HI_U32         Reserved_VHD2P37RESO;
    HI_U32         Reserved_VHD2P37LADDR;
    HI_U32         Reserved_VHD2P37CADDR;
    HI_U32         Reserved_VHD2P37STRIDE;
    HI_U32         Reserved_VHD2P37VFPOS;
    HI_U32         Reserved_VHD2P37VLPOS;
    HI_U32         Reserved_221[2];
    HI_U32         Reserved_VHD2P38RESO;
    HI_U32         Reserved_VHD2P38LADDR;
    HI_U32         Reserved_VHD2P38CADDR;
    HI_U32         Reserved_VHD2P38STRIDE;
    HI_U32         Reserved_VHD2P38VFPOS;
    HI_U32         Reserved_VHD2P38VLPOS;
    HI_U32         Reserved_222[2];
    HI_U32         Reserved_VHD2P39RESO;
    HI_U32         Reserved_VHD2P39LADDR;
    HI_U32         Reserved_VHD2P39CADDR;
    HI_U32         Reserved_VHD2P39STRIDE;
    HI_U32         Reserved_VHD2P39VFPOS;
    HI_U32         Reserved_VHD2P39VLPOS;
    HI_U32         Reserved_223[2];
    HI_U32         Reserved_VHD2P40RESO;
    HI_U32         Reserved_VHD2P40LADDR;
    HI_U32         Reserved_VHD2P40CADDR;
    HI_U32         Reserved_VHD2P40STRIDE;
    HI_U32         Reserved_VHD2P40VFPOS;
    HI_U32         Reserved_VHD2P40VLPOS;
    HI_U32         Reserved_224[2];
    HI_U32         Reserved_VHD2P41RESO;
    HI_U32         Reserved_VHD2P41LADDR;
    HI_U32         Reserved_VHD2P41CADDR;
    HI_U32         Reserved_VHD2P41STRIDE;
    HI_U32         Reserved_VHD2P41VFPOS;
    HI_U32         Reserved_VHD2P41VLPOS;
    HI_U32         Reserved_225[2];
    HI_U32         Reserved_VHD2P42RESO;
    HI_U32         Reserved_VHD2P42LADDR;
    HI_U32         Reserved_VHD2P42CADDR;
    HI_U32         Reserved_VHD2P42STRIDE;
    HI_U32         Reserved_VHD2P42VFPOS;
    HI_U32         Reserved_VHD2P42VLPOS;
    HI_U32         Reserved_226[2];
    HI_U32         Reserved_VHD2P43RESO;
    HI_U32         Reserved_VHD2P43LADDR;
    HI_U32         Reserved_VHD2P43CADDR;
    HI_U32         Reserved_VHD2P43STRIDE;
    HI_U32         Reserved_VHD2P43VFPOS;
    HI_U32         Reserved_VHD2P43VLPOS;
    HI_U32         Reserved_227[2];
    HI_U32         Reserved_VHD2P44RESO;
    HI_U32         Reserved_VHD2P44LADDR;
    HI_U32         Reserved_VHD2P44CADDR;
    HI_U32         Reserved_VHD2P44STRIDE;
    HI_U32         Reserved_VHD2P44VFPOS;
    HI_U32         Reserved_VHD2P44VLPOS;
    HI_U32         Reserved_228[2];
    HI_U32         Reserved_VHD2P45RESO;
    HI_U32         Reserved_VHD2P45LADDR;
    HI_U32         Reserved_VHD2P45CADDR;
    HI_U32         Reserved_VHD2P45STRIDE;
    HI_U32         Reserved_VHD2P45VFPOS;
    HI_U32         Reserved_VHD2P45VLPOS;
    HI_U32         Reserved_229[2];
    HI_U32         Reserved_VHD2P46RESO;
    HI_U32         Reserved_VHD2P46LADDR;
    HI_U32         Reserved_VHD2P46CADDR;
    HI_U32         Reserved_VHD2P46STRIDE;
    HI_U32         Reserved_VHD2P46VFPOS;
    HI_U32         Reserved_VHD2P46VLPOS;
    HI_U32         Reserved_230[2];
    HI_U32         Reserved_VHD2P47RESO;
    HI_U32         Reserved_VHD2P47LADDR;
    HI_U32         Reserved_VHD2P47CADDR;
    HI_U32         Reserved_VHD2P47STRIDE;
    HI_U32         Reserved_VHD2P47VFPOS;
    HI_U32         Reserved_VHD2P47VLPOS;
    HI_U32         Reserved_231[2];
    HI_U32         Reserved_VHD2P48RESO;
    HI_U32         Reserved_VHD2P48LADDR;
    HI_U32         Reserved_VHD2P48CADDR;
    HI_U32         Reserved_VHD2P48STRIDE;
    HI_U32         Reserved_VHD2P48VFPOS;
    HI_U32         Reserved_VHD2P48VLPOS;
    HI_U32         Reserved_232[2];
    HI_U32         Reserved_VHD2P49RESO;
    HI_U32         Reserved_VHD2P49LADDR;
    HI_U32         Reserved_VHD2P49CADDR;
    HI_U32         Reserved_VHD2P49STRIDE;
    HI_U32         Reserved_VHD2P49VFPOS;
    HI_U32         Reserved_VHD2P49VLPOS;
    HI_U32         Reserved_233[2];
    HI_U32         Reserved_VHD2P50RESO;
    HI_U32         Reserved_VHD2P50LADDR;
    HI_U32         Reserved_VHD2P50CADDR;
    HI_U32         Reserved_VHD2P50STRIDE;
    HI_U32         Reserved_VHD2P50VFPOS;
    HI_U32         Reserved_VHD2P50VLPOS;
    HI_U32         Reserved_234[2];
    HI_U32         Reserved_VHD2P51RESO;
    HI_U32         Reserved_VHD2P51LADDR;
    HI_U32         Reserved_VHD2P51CADDR;
    HI_U32         Reserved_VHD2P51STRIDE;
    HI_U32         Reserved_VHD2P51VFPOS;
    HI_U32         Reserved_VHD2P51VLPOS;
    HI_U32         Reserved_235[2];
    HI_U32         Reserved_VHD2P52RESO;
    HI_U32         Reserved_VHD2P52LADDR;
    HI_U32         Reserved_VHD2P52CADDR;
    HI_U32         Reserved_VHD2P52STRIDE;
    HI_U32         Reserved_VHD2P52VFPOS;
    HI_U32         Reserved_VHD2P52VLPOS;
    HI_U32         Reserved_236[2];
    HI_U32         Reserved_VHD2P53RESO;
    HI_U32         Reserved_VHD2P53LADDR;
    HI_U32         Reserved_VHD2P53CADDR;
    HI_U32         Reserved_VHD2P53STRIDE;
    HI_U32         Reserved_VHD2P53VFPOS;
    HI_U32         Reserved_VHD2P53VLPOS;
    HI_U32         Reserved_237[2];
    HI_U32         Reserved_VHD2P54RESO;
    HI_U32         Reserved_VHD2P54LADDR;
    HI_U32         Reserved_VHD2P54CADDR;
    HI_U32         Reserved_VHD2P54STRIDE;
    HI_U32         Reserved_VHD2P54VFPOS;
    HI_U32         Reserved_VHD2P54VLPOS;
    HI_U32         Reserved_238[2];
    HI_U32         Reserved_VHD2P55RESO;
    HI_U32         Reserved_VHD2P55LADDR;
    HI_U32         Reserved_VHD2P55CADDR;
    HI_U32         Reserved_VHD2P55STRIDE;
    HI_U32         Reserved_VHD2P55VFPOS;
    HI_U32         Reserved_VHD2P55VLPOS;
    HI_U32         Reserved_239[2];
    HI_U32         Reserved_VHD2P56RESO;
    HI_U32         Reserved_VHD2P56LADDR;
    HI_U32         Reserved_VHD2P56CADDR;
    HI_U32         Reserved_VHD2P56STRIDE;
    HI_U32         Reserved_VHD2P56VFPOS;
    HI_U32         Reserved_VHD2P56VLPOS;
    HI_U32         Reserved_240[2];
    HI_U32         Reserved_VHD2P57RESO;
    HI_U32         Reserved_VHD2P57LADDR;
    HI_U32         Reserved_VHD2P57CADDR;
    HI_U32         Reserved_VHD2P57STRIDE;
    HI_U32         Reserved_VHD2P57VFPOS;
    HI_U32         Reserved_VHD2P57VLPOS;
    HI_U32         Reserved_241[2];
    HI_U32         Reserved_VHD2P58RESO;
    HI_U32         Reserved_VHD2P58LADDR;
    HI_U32         Reserved_VHD2P58CADDR;
    HI_U32         Reserved_VHD2P58STRIDE;
    HI_U32         Reserved_VHD2P58VFPOS;
    HI_U32         Reserved_VHD2P58VLPOS;
    HI_U32         Reserved_242[2];
    HI_U32         Reserved_VHD2P59RESO;
    HI_U32         Reserved_VHD2P59LADDR;
    HI_U32         Reserved_VHD2P59CADDR;
    HI_U32         Reserved_VHD2P59STRIDE;
    HI_U32         Reserved_VHD2P59VFPOS;
    HI_U32         Reserved_VHD2P59VLPOS;
    HI_U32         Reserved_243[2];
    HI_U32         Reserved_VHD2P60RESO;
    HI_U32         Reserved_VHD2P60LADDR;
    HI_U32         Reserved_VHD2P60CADDR;
    HI_U32         Reserved_VHD2P60STRIDE;
    HI_U32         Reserved_VHD2P60VFPOS;
    HI_U32         Reserved_VHD2P60VLPOS;
    HI_U32         Reserved_244[2];
    HI_U32         Reserved_VHD2P61RESO;
    HI_U32         Reserved_VHD2P61LADDR;
    HI_U32         Reserved_VHD2P61CADDR;
    HI_U32         Reserved_VHD2P61STRIDE;
    HI_U32         Reserved_VHD2P61VFPOS;
    HI_U32         Reserved_VHD2P61VLPOS;
    HI_U32         Reserved_245[2];
    HI_U32         Reserved_VHD2P62RESO;
    HI_U32         Reserved_VHD2P62LADDR;
    HI_U32         Reserved_VHD2P62CADDR;
    HI_U32         Reserved_VHD2P62STRIDE;
    HI_U32         Reserved_VHD2P62VFPOS;
    HI_U32         Reserved_VHD2P62VLPOS;
    HI_U32         Reserved_246[2];
    HI_U32         Reserved_VHD2P63RESO;
    HI_U32         Reserved_VHD2P63LADDR;
    HI_U32         Reserved_VHD2P63CADDR;
    HI_U32         Reserved_VHD2P63STRIDE;
    HI_U32         Reserved_VHD2P63VFPOS;
    HI_U32         Reserved_VHD2P63VLPOS;
    HI_U32         Reserved_247[2];
    HI_U32         Reserved_VHD264REGIONENH;
    HI_U32         Reserved_VHD264REGIONENL;
    HI_U32         Reserved_248[254];
#endif
    HI_U32         Reserved_248[2302];

    U_VSDCTRL            VSDCTRL;       //0x3000
    U_VSDUPD             VSDUPD;
    HI_U32         Reserved_249[2];
    U_VSDCADDR           VSDCADDR;
    U_VSDCCADDR          VSDCCADDR;
    HI_U32         Reserved_250[3];
    U_VSDSTRIDE          VSDSTRIDE;
    U_VSDIRESO           VSDIRESO;
    HI_U32         Reserved_251;
    HI_U32         Reserved_VSDLKEY;
    U_VSDCBMPARA         VSDCBMPARA;    //0x3034
    HI_U32         Reserved_252[10];
    U_VSDDFPOS           VSDDFPOS;      //0x3060
    U_VSDDLPOS           VSDDLPOS;
    HI_U32         Reserved_VSDVFPOS;
    HI_U32         Reserved_VSDVLPOS;
    U_VSDBK              VSDBK;
    HI_U32         Reserved_253[3];
    U_VSDCSCIDC          VSDCSCIDC;
    U_VSDCSCODC          VSDCSCODC;
    U_VSDCSCP0           VSDCSCP0;
    U_VSDCSCP1           VSDCSCP1;
    U_VSDCSCP2           VSDCSCP2;
    U_VSDCSCP3           VSDCSCP3;
    U_VSDCSCP4           VSDCSCP4;      //0x3098
    HI_U32         Reserved_254;
    U_VSDACM0            VSDACM0;       /*no*/
    U_VSDACM1            VSDACM1;
    U_VSDACM2            VSDACM2;
    U_VSDACM3            VSDACM3;
    U_VSDACM4            VSDACM4;
    U_VSDACM5            VSDACM5;
    U_VSDACM6            VSDACM6;
    U_VSDACM7            VSDACM7;
    HI_U32         Reserved_VSDHSP;
    HI_U32         Reserved_VSDHLOFFSET;
    HI_U32         Reserved_VSDHCOFFSET;
    HI_U32         Reserved_255[3];
    HI_U32         Reserved_VSDVSP;
    HI_U32         Reserved_VSDVSR;
    HI_U32         Reserved_VSDVOFFSET;
    HI_U32         Reserved_VSDZMEORESO;
    HI_U32         Reserved_VSDZMEIRESO;
    HI_U32         Reserved_VSDZMEDBG;
    HI_U32         Reserved_VSDMOSAIC[4];
    U_VSDACCTHD1         VSDACCTHD1;
    U_VSDACCTHD2         VSDACCTHD2;
    HI_U32         Reserved_256[2];
    U_VSDACCLOWN         VSDACCLOWN[3];
    HI_U32         Reserved_257;
    U_VSDACCMEDN         VSDACCMEDN[3];
    HI_U32         Reserved_258;
    U_VSDACCHIGHN        VSDACCHIGHN[3];
    HI_U32         Reserved_259;
    U_VSDACCMLN          VSDACCMLN[3];
    HI_U32         Reserved_260;
    U_VSDACCMHN          VSDACCMHN[3];
    HI_U32         Reserved_261;
    U_VSDACC3LOW         VSDACC3LOW;
    U_VSDACC3MED         VSDACC3MED;
    U_VSDACC3HIGH        VSDACC3HIGH;
    U_VSDACC8MLOW        VSDACC8MLOW;
    U_VSDACC8MHIGH       VSDACC8MHIGH;
    U_VSDACCTOTAL        VSDACCTOTAL;
    HI_U32         Reserved_262[2];     /*no*/
    U_VSDIFIRCOEF01      VSDIFIRCOEF01;     //0x3180
    U_VSDIFIRCOEF23      VSDIFIRCOEF23;
    U_VSDIFIRCOEF45      VSDIFIRCOEF45;
    U_VSDIFIRCOEF67      VSDIFIRCOEF67;
    HI_U32         Reserved_263[156];
    U_VSDP0RESO          VSDP0RESO;       //0x3400
    U_VSDP0LADDR         VSDP0LADDR;
    U_VSDP0CADDR         VSDP0CADDR;
    U_VSDP0STRIDE        VSDP0STRIDE;
    U_VSDP0VFPOS         VSDP0VFPOS;
    U_VSDP0VLPOS         VSDP0VLPOS;    //0x3414
    #if 0
    HI_U32         Reserved_264[2];     /*no*/
    U_VSDP1RESO          VSDP1RESO;
    U_VSDP1LADDR         VSDP1LADDR;
    U_VSDP1CADDR         VSDP1CADDR;
    U_VSDP1STRIDE        VSDP1STRIDE;
    U_VSDP1VFPOS         VSDP1VFPOS;
    U_VSDP1VLPOS         VSDP1VLPOS;
    HI_U32         Reserved_265[2];
    U_VSDP2RESO          VSDP2RESO;
    U_VSDP2LADDR         VSDP2LADDR;
    U_VSDP2CADDR         VSDP2CADDR;
    U_VSDP2STRIDE        VSDP2STRIDE;
    U_VSDP2VFPOS         VSDP2VFPOS;
    U_VSDP2VLPOS         VSDP2VLPOS;
    HI_U32         Reserved_266[2];
    U_VSDP3RESO          VSDP3RESO;
    U_VSDP3LADDR         VSDP3LADDR;
    U_VSDP3CADDR         VSDP3CADDR;
    U_VSDP3STRIDE        VSDP3STRIDE;
    U_VSDP3VFPOS         VSDP3VFPOS;
    U_VSDP3VLPOS         VSDP3VLPOS;
    HI_U32         Reserved_267[2];
    U_VSDP4RESO          VSDP4RESO;
    U_VSDP4LADDR         VSDP4LADDR;
    U_VSDP4CADDR         VSDP4CADDR;
    U_VSDP4STRIDE        VSDP4STRIDE;
    U_VSDP4VFPOS         VSDP4VFPOS;
    U_VSDP4VLPOS         VSDP4VLPOS;
    HI_U32         Reserved_268[2];
    U_VSDP5RESO          VSDP5RESO;
    U_VSDP5LADDR         VSDP5LADDR;
    U_VSDP5CADDR         VSDP5CADDR;
    U_VSDP5STRIDE        VSDP5STRIDE;
    U_VSDP5VFPOS         VSDP5VFPOS;
    U_VSDP5VLPOS         VSDP5VLPOS;
    HI_U32         Reserved_269[2];
    U_VSDP6RESO          VSDP6RESO;
    U_VSDP6LADDR         VSDP6LADDR;
    U_VSDP6CADDR         VSDP6CADDR;
    U_VSDP6STRIDE        VSDP6STRIDE;
    U_VSDP6VFPOS         VSDP6VFPOS;
    U_VSDP6VLPOS         VSDP6VLPOS;
    HI_U32         Reserved_270[2];
    U_VSDP7RESO          VSDP7RESO;
    U_VSDP7LADDR         VSDP7LADDR;
    U_VSDP7CADDR         VSDP7CADDR;
    U_VSDP7STRIDE        VSDP7STRIDE;
    U_VSDP7VFPOS         VSDP7VFPOS;
    U_VSDP7VLPOS         VSDP7VLPOS;
    HI_U32         Reserved_271[2];
    HI_U32         Reserved_VSDP8RESO;
    HI_U32         Reserved_VSDP8LADDR;
    HI_U32         Reserved_VSDP8CADDR;
    HI_U32         Reserved_VSDP8STRIDE;
    HI_U32         Reserved_VSDP8VFPOS;
    HI_U32         Reserved_VSDP8VLPOS;
    HI_U32         Reserved_272[2];
    HI_U32         Reserved_VSDP9RESO;
    HI_U32         Reserved_VSDP9LADDR;
    HI_U32         Reserved_VSDP9CADDR;
    HI_U32         Reserved_VSDP9STRIDE;
    HI_U32         Reserved_VSDP9VFPOS;
    HI_U32         Reserved_VSDP9VLPOS;
    HI_U32         Reserved_273[2];
    HI_U32         Reserved_VSDP10RESO;
    HI_U32         Reserved_VSDP10LADDR;
    HI_U32         Reserved_VSDP10CADDR;
    HI_U32         Reserved_VSDP10STRIDE;
    HI_U32         Reserved_VSDP10VFPOS;
    HI_U32         Reserved_VSDP10VLPOS;
    HI_U32         Reserved_274[2];
    HI_U32         Reserved_VSDP11RESO;
    HI_U32         Reserved_VSDP11LADDR;
    HI_U32         Reserved_VSDP11CADDR;
    HI_U32         Reserved_VSDP11STRIDE;
    HI_U32         Reserved_VSDP11VFPOS;
    HI_U32         Reserved_VSDP11VLPOS;
    HI_U32         Reserved_275[2];
    HI_U32         Reserved_VSDP12RESO;
    HI_U32         Reserved_VSDP12LADDR;
    HI_U32         Reserved_VSDP12CADDR;
    HI_U32         Reserved_VSDP12STRIDE;
    HI_U32         Reserved_VSDP12VFPOS;
    HI_U32         Reserved_VSDP12VLPOS;
    HI_U32         Reserved_276[2];
    HI_U32         Reserved_VSDP13RESO;
    HI_U32         Reserved_VSDP13LADDR;
    HI_U32         Reserved_VSDP13CADDR;
    HI_U32         Reserved_VSDP13STRIDE;
    HI_U32         Reserved_VSDP13VFPOS;
    HI_U32         Reserved_VSDP13VLPOS;
    HI_U32         Reserved_277[2];
    HI_U32         Reserved_VSDP14RESO;
    HI_U32         Reserved_VSDP14LADDR;
    HI_U32         Reserved_VSDP14CADDR;
    HI_U32         Reserved_VSDP14STRIDE;
    HI_U32         Reserved_VSDP14VFPOS;
    HI_U32         Reserved_VSDP14VLPOS;
    HI_U32         Reserved_278[2];
    HI_U32         Reserved_VSDP15RESO;
    HI_U32         Reserved_VSDP15LADDR;
    HI_U32         Reserved_VSDP15CADDR;
    HI_U32         Reserved_VSDP15STRIDE;
    HI_U32         Reserved_VSDP15VFPOS;
    HI_U32         Reserved_VSDP15VLPOS;
    HI_U32         Reserved_279[387];
    #endif
    HI_U32         Reserved_279[507];
    U_VSD1REGIONEN      VSD1REGIONEN;     //0x3C04
    HI_U32         Reserved_280[254];
    U_VSD1CTRL           VSD1CTRL;         //0x4000
    U_VSD1UPD            VSD1UPD;
    HI_U32         Reserved_281[2];
    U_VSD1CADDR          VSD1CADDR;
    U_VSD1CCADDR         VSD1CCADDR;
    HI_U32         Reserved_282[3];
    U_VSD1STRIDE         VSD1STRIDE;
    U_VSD1IRESO          VSD1IRESO;
    HI_U32         Reserved_283;
    HI_U32         Reserved_VSD1LKEY;
    U_VSD1CBMPARA        VSD1CBMPARA;       //0x4034
    HI_U32         Reserved_284[10];
    U_VSD1DFPOS          VSD1DFPOS;         //0x4060
    U_VSD1DLPOS          VSD1DLPOS;
    HI_U32         Reserved_VSD1VFPOS;
    HI_U32         Reserved_VSD1VLPOS;
    U_VSD1BK             VSD1BK;            /*no*/
    HI_U32         Reserved_285[3];
    U_VSD1CSCIDC         VSD1CSCIDC;
    U_VSD1CSCODC         VSD1CSCODC;
    U_VSD1CSCP0          VSD1CSCP0;
    U_VSD1CSCP1          VSD1CSCP1;
    U_VSD1CSCP2          VSD1CSCP2;
    U_VSD1CSCP3          VSD1CSCP3;
    U_VSD1CSCP4          VSD1CSCP4;     //0x4098
    #if 0
    HI_U32         Reserved_286;
    HI_U32         Reserved_VSD1ACM0;
    HI_U32         Reserved_VSD1ACM1;
    HI_U32         Reserved_VSD1ACM2;
    HI_U32         Reserved_VSD1ACM3;
    HI_U32         Reserved_VSD1ACM4;
    HI_U32         Reserved_VSD1ACM5;
    HI_U32         Reserved_VSD1ACM6;
    HI_U32         Reserved_VSD1ACM7;
    HI_U32         Reserved_VSD1HSP;
    HI_U32         Reserved_VSD1HLOFFSET;
    HI_U32         Reserved_VSD1HCOFFSET;
    HI_U32         Reserved_ed_287[3];
    HI_U32         Reserved_VSD1VSP;
    HI_U32         Reserved_VSD1VSR;
    HI_U32         Reserved_VSD1VOFFSET;
    HI_U32         Reserved_VSD1ZMEORESO;
    HI_U32         Reserved_VSD1ZMEIRESO;
    HI_U32         Reserved_VSD1ZMEDBG;
    HI_U32         Reserved_VSD1MOSAIC[4];
    HI_U32         Reserved_VSD1ACCTHD1;
    HI_U32         Reserved_VSD1ACCTHD2;
    HI_U32         Reserved_ed_288[2];
    HI_U32         Reserved_VSD1ACCLOWN[3];
    HI_U32         Reserved_ed_289;
    HI_U32         Reserved_VSD1ACCMEDN[3];
    HI_U32         Reserved_ed_290;
    HI_U32         Reserved_VSD1ACCHIGHN[3];
    HI_U32         Reserved_ed_291;
    HI_U32         Reserved_VSD1ACCMLN[3];
    HI_U32         Reserved_292;
    HI_U32         Reserved_VSD1ACCMHN[3];
    HI_U32         Reserved_ed_293;
    HI_U32         Reserved_VSD1ACC3LOW;
    HI_U32         Reserved_VSD1ACC3MED;
    HI_U32         Reserved_VSD1ACC3HIGH;
    HI_U32         Reserved_VSD1ACC8MLOW;
    HI_U32         Reserved_VSD1ACC8MHIGH;
    HI_U32         Reserved_VSD1ACCTOTAL;
    HI_U32         Reserved_294[2];
    #endif
    HI_U32         Reserved_294[57];
    U_VSD1IFIRCOEF01     VSD1IFIRCOEF01;    //0x4180
    U_VSD1IFIRCOEF23     VSD1IFIRCOEF23;
    U_VSD1IFIRCOEF45     VSD1IFIRCOEF45;
    U_VSD1IFIRCOEF67     VSD1IFIRCOEF67;    //0x418C
    HI_U32         Reserved_295[156];
    U_VSD1P0RESO         VSD1P0RESO;        //0x4400
    U_VSD1P0LADDR        VSD1P0LADDR;
    U_VSD1P0CADDR        VSD1P0CADDR;
    U_VSD1P0STRIDE       VSD1P0STRIDE;
    U_VSD1P0VFPOS        VSD1P0VFPOS;
    U_VSD1P0VLPOS        VSD1P0VLPOS;       //0x4414
    #if 0
    HI_U32         Reserved_296[2];
    HI_U32         Reserved_VSD1P1RESO;
    HI_U32         Reserved_VSD1P1LADDR;
    HI_U32         Reserved_VSD1P1CADDR;
    HI_U32         Reserved_VSD1P1STRIDE;
    HI_U32         Reserved_VSD1P1VFPOS;
    HI_U32         Reserved_VSD1P1VLPOS;
    HI_U32         Reserved_297[2];
    HI_U32         Reserved_VSD1P2RESO;
    HI_U32         Reserved_VSD1P2LADDR;
    HI_U32         Reserved_VSD1P2CADDR;
    HI_U32         Reserved_VSD1P2STRIDE;
    HI_U32         Reserved_VSD1P2VFPOS;
    HI_U32         Reserved_VSD1P2VLPOS;
    HI_U32         Reserved_298[2];
    HI_U32         Reserved_VSD1P3RESO;
    HI_U32         Reserved_VSD1P3LADDR;
    HI_U32         Reserved_VSD1P3CADDR;
    HI_U32         Reserved_VSD1P3STRIDE;
    HI_U32         Reserved_VSD1P3VFPOS;
    HI_U32         Reserved_VSD1P3VLPOS;
    HI_U32         Reserved_299[2];
    HI_U32         Reserved_VSD1P4RESO;
    HI_U32         Reserved_VSD1P4LADDR;
    HI_U32         Reserved_VSD1P4CADDR;
    HI_U32         Reserved_VSD1P4STRIDE;
    HI_U32         Reserved_VSD1P4VFPOS;
    HI_U32         Reserved_VSD1P4VLPOS;
    HI_U32         Reserved_300[2];
    HI_U32         Reserved_VSD1P5RESO;
    HI_U32         Reserved_VSD1P5LADDR;
    HI_U32         Reserved_VSD1P5CADDR;
    HI_U32         Reserved_VSD1P5STRIDE;
    HI_U32         Reserved_VSD1P5VFPOS;
    HI_U32         Reserved_VSD1P5VLPOS;
    HI_U32         Reserved_301[2];
    HI_U32         Reserved_VSD1P6RESO;
    HI_U32         Reserved_VSD1P6LADDR;
    HI_U32         Reserved_VSD1P6CADDR;
    HI_U32         Reserved_VSD1P6STRIDE;
    HI_U32         Reserved_VSD1P6VFPOS;
    HI_U32         Reserved_VSD1P6VLPOS;
    HI_U32         Reserved_302[2];
    HI_U32         Reserved_VSD1P7RESO;
    HI_U32         Reserved_VSD1P7LADDR;
    HI_U32         Reserved_VSD1P7CADDR;
    HI_U32         Reserved_VSD1P7STRIDE;
    HI_U32         Reserved_VSD1P7VFPOS;
    HI_U32         Reserved_VSD1P7VLPOS;
    HI_U32         Reserved_303[2];
    HI_U32         Reserved_VSD1P8RESO;
    HI_U32         Reserved_VSD1P8LADDR;
    HI_U32         Reserved_VSD1P8CADDR;
    HI_U32         Reserved_VSD1P8STRIDE;
    HI_U32         Reserved_VSD1P8VFPOS;
    HI_U32         Reserved_VSD1P8VLPOS;
    HI_U32         Reserved_304[2];
    HI_U32         Reserved_VSD1P9RESO;
    HI_U32         Reserved_VSD1P9LADDR;
    HI_U32         Reserved_VSD1P9CADDR;
    HI_U32         Reserved_VSD1P9STRIDE;
    HI_U32         Reserved_VSD1P9VFPOS;
    HI_U32         Reserved_VSD1P9VLPOS;
    HI_U32         Reserved_305[2];
    HI_U32         Reserved_VSD1P10RESO;
    HI_U32         Reserved_VSD1P10LADDR;
    HI_U32         Reserved_VSD1P10CADDR;
    HI_U32         Reserved_VSD1P10STRIDE;
    HI_U32         Reserved_VSD1P10VFPOS;
    HI_U32         Reserved_VSD1P10VLPOS;
    HI_U32         Reserved_306[2];
    HI_U32         Reserved_VSD1P11RESO;
    HI_U32         Reserved_VSD1P11LADDR;
    HI_U32         Reserved_VSD1P11CADDR;
    HI_U32         Reserved_VSD1P11STRIDE;
    HI_U32         Reserved_VSD1P11VFPOS;
    HI_U32         Reserved_VSD1P11VLPOS;
    HI_U32         Reserved_307[2];
    HI_U32         Reserved_VSD1P12RESO;
    HI_U32         Reserved_VSD1P12LADDR;
    HI_U32         Reserved_VSD1P12CADDR;
    HI_U32         Reserved_VSD1P12STRIDE;
    HI_U32         Reserved_VSD1P12VFPOS;
    HI_U32         Reserved_VSD1P12VLPOS;
    HI_U32         Reserved_308[2];
    HI_U32         Reserved_VSD1P13RESO;
    HI_U32         Reserved_VSD1P13LADDR;
    HI_U32         Reserved_VSD1P13CADDR;
    HI_U32         Reserved_VSD1P13STRIDE;
    HI_U32         Reserved_VSD1P13VFPOS;
    HI_U32         Reserved_VSD1P13VLPOS;
    HI_U32         Reserved_309[2];
    HI_U32         Reserved_VSD1P14RESO;
    HI_U32         Reserved_VSD1P14LADDR;
    HI_U32         Reserved_VSD1P14CADDR;
    HI_U32         Reserved_VSD1P14STRIDE;
    HI_U32         Reserved_VSD1P14VFPOS;
    HI_U32         Reserved_VSD1P14VLPOS;
    HI_U32         Reserved_310[2];
    HI_U32         Reserved_VSD1P15RESO;
    HI_U32         Reserved_VSD1P15LADDR;
    HI_U32         Reserved_VSD1P15CADDR;
    HI_U32         Reserved_VSD1P15STRIDE;
    HI_U32         Reserved_VSD1P15VFPOS;
    HI_U32         Reserved_VSD1P15VLPOS;
    HI_U32         Reserved_311[387];
    #endif
    HI_U32         Reserved_311[507];
    U_VSD11REGIONEN     VSD11REGIONEN;    //0x4C04
    #if 0
    HI_U32         Reserved_312[254];
    HI_U32         Reserved_VSD2CTRL;
    HI_U32         Reserved_VSD2UPD;
    HI_U32         Reserved_313[2];
    HI_U32         Reserved_VSD2CADDR;
    HI_U32         Reserved_VSD2CCADDR;
    HI_U32         Reserved_314[3];
    HI_U32         Reserved_VSD2STRIDE;
    HI_U32         Reserved_VSD2IRESO;
    HI_U32         Reserved_ed_315;
    HI_U32         Reserved_VSD2LKEY;
    HI_U32         Reserved_VSD2CBMPARA;
    HI_U32         Reserved_ed_316[10];
    HI_U32         Reserved_VSD2DFPOS;
    HI_U32         Reserved_VSD2DLPOS;
    HI_U32         Reserved_VSD2VFPOS;
    HI_U32         Reserved_VSD2VLPOS;
    HI_U32         Reserved_VSD2BK;
    HI_U32         Reserved_ed_317[3];
    HI_U32         Reserved_VSD2CSCIDC;
    HI_U32         Reserved_VSD2CSCODC;
    HI_U32         Reserved_VSD2CSCP0;
    HI_U32         Reserved_VSD2CSCP1;
    HI_U32         Reserved_VSD2CSCP2;
    HI_U32         Reserved_VSD2CSCP3;
    HI_U32         Reserved_VSD2CSCP4;
    HI_U32         Reserved_ed_318;
    HI_U32         Reserved_VSD2ACM0;
    HI_U32         Reserved_VSD2ACM1;
    HI_U32         Reserved_VSD2ACM2;
    HI_U32         Reserved_VSD2ACM3;
    HI_U32         Reserved_VSD2ACM4;
    HI_U32         Reserved_VSD2ACM5;
    HI_U32         Reserved_VSD2ACM6;
    HI_U32         Reserved_VSD2ACM7;
    HI_U32         Reserved_VSD2HSP;
    HI_U32         Reserved_VSD2HLOFFSET;
    HI_U32         Reserved_VSD2HCOFFSET;
    HI_U32         Reserved_ed_319[3];
    HI_U32         Reserved_VSD2VSP;
    HI_U32         Reserved_VSD2VSR;
    HI_U32         Reserved_VSD2VOFFSET;
    HI_U32         Reserved_VSD2ZMEORESO;
    HI_U32         Reserved_VSD2ZMEIRESO;
    HI_U32         Reserved_VSD2ZMEDBG;
    HI_U32         Reserved_VSD2MOSAIC[4];
    HI_U32         Reserved_VSD2ACCTHD1;
    HI_U32         Reserved_VSD2ACCTHD2;
    HI_U32         Reserved_ed_320[2];
    HI_U32         Reserved_VSD2ACCLOWN[3];
    HI_U32         Reserved_ed_321;
    HI_U32         Reserved_VSD2ACCMEDN[3];
    HI_U32         Reserved_ed_322;
    HI_U32         Reserved_VSD2ACCHIGHN[3];
    HI_U32         Reserved_ed_323;
    HI_U32         Reserved_VSD2ACCMLN[3];
    HI_U32         Reserved_ed_324;
    HI_U32         Reserved_VSD2ACCMHN[3];
    HI_U32         Reserved_ed_325;
    HI_U32         Reserved_VSD2ACC3LOW;
    HI_U32         Reserved_VSD2ACC3MED;
    HI_U32         Reserved_VSD2ACC3HIGH;
    HI_U32         Reserved_VSD2ACC8MLOW;
    HI_U32         Reserved_VSD2ACC8MHIGH;
    HI_U32         Reserved_VSD2ACCTOTAL;
    HI_U32         Reserved_ed_326[2];
    HI_U32         Reserved_VSD2IFIRCOEF01;
    HI_U32         Reserved_VSD2IFIRCOEF23;
    HI_U32         Reserved_VSD2IFIRCOEF45;
    HI_U32         Reserved_VSD2IFIRCOEF67;
    HI_U32         Reserved_ed_327[156];
    HI_U32         Reserved_VSD2P0RESO;
    HI_U32         Reserved_VSD2P0LADDR;
    HI_U32         Reserved_VSD2P0CADDR;
    HI_U32         Reserved_VSD2P0STRIDE;
    HI_U32         Reserved_VSD2P0VFPOS;
    HI_U32         Reserved_VSD2P0VLPOS;
    HI_U32         Reserved_ed_328[2];
    HI_U32         Reserved_VSD2P1RESO;
    HI_U32         Reserved_VSD2P1LADDR;
    HI_U32         Reserved_VSD2P1CADDR;
    HI_U32         Reserved_VSD2P1STRIDE;
    HI_U32         Reserved_VSD2P1VFPOS;
    HI_U32         Reserved_VSD2P1VLPOS;
    HI_U32         Reserved_ed_329[2];
    HI_U32         Reserved_VSD2P2RESO;
    HI_U32         Reserved_VSD2P2LADDR;
    HI_U32         Reserved_VSD2P2CADDR;
    HI_U32         Reserved_VSD2P2STRIDE;
    HI_U32         Reserved_VSD2P2VFPOS;
    HI_U32         Reserved_VSD2P2VLPOS;
    HI_U32         Reserved_330[2];
    HI_U32         Reserved_VSD2P3RESO;
    HI_U32         Reserved_VSD2P3LADDR;
    HI_U32         Reserved_VSD2P3CADDR;
    HI_U32         Reserved_VSD2P3STRIDE;
    HI_U32         Reserved_VSD2P3VFPOS;
    HI_U32         Reserved_VSD2P3VLPOS;
    HI_U32         Reserved_331[2];
    HI_U32         Reserved_VSD2P4RESO;
    HI_U32         Reserved_VSD2P4LADDR;
    HI_U32         Reserved_VSD2P4CADDR;
    HI_U32         Reserved_VSD2P4STRIDE;
    HI_U32         Reserved_VSD2P4VFPOS;
    HI_U32         Reserved_VSD2P4VLPOS;
    HI_U32         Reserved_332[2];
    HI_U32         Reserved_VSD2P5RESO;
    HI_U32         Reserved_VSD2P5LADDR;
    HI_U32         Reserved_VSD2P5CADDR;
    HI_U32         Reserved_VSD2P5STRIDE;
    HI_U32         Reserved_VSD2P5VFPOS;
    HI_U32         Reserved_VSD2P5VLPOS;
    HI_U32         Reserved_333[2];
    HI_U32         Reserved_VSD2P6RESO;
    HI_U32         Reserved_VSD2P6LADDR;
    HI_U32         Reserved_VSD2P6CADDR;
    HI_U32         Reserved_VSD2P6STRIDE;
    HI_U32         Reserved_VSD2P6VFPOS;
    HI_U32         Reserved_VSD2P6VLPOS;
    HI_U32         Reserved_334[2];
    HI_U32         Reserved_VSD2P7RESO;
    HI_U32         Reserved_VSD2P7LADDR;
    HI_U32         Reserved_VSD2P7CADDR;
    HI_U32         Reserved_VSD2P7STRIDE;
    HI_U32         Reserved_VSD2P7VFPOS;
    HI_U32         Reserved_VSD2P7VLPOS;
    HI_U32         Reserved_335[2];
    HI_U32         Reserved_VSD2P8RESO;
    HI_U32         Reserved_VSD2P8LADDR;
    HI_U32         Reserved_VSD2P8CADDR;
    HI_U32         Reserved_VSD2P8STRIDE;
    HI_U32         Reserved_VSD2P8VFPOS;
    HI_U32         Reserved_VSD2P8VLPOS;
    HI_U32         Reserved_336[2];
    HI_U32         Reserved_VSD2P9RESO;
    HI_U32         Reserved_VSD2P9LADDR;
    HI_U32         Reserved_VSD2P9CADDR;
    HI_U32         Reserved_VSD2P9STRIDE;
    HI_U32         Reserved_VSD2P9VFPOS;
    HI_U32         Reserved_VSD2P9VLPOS;
    HI_U32         Reserved_337[2];
    HI_U32         Reserved_VSD2P10RESO;
    HI_U32         Reserved_VSD2P10LADDR;
    HI_U32         Reserved_VSD2P10CADDR;
    HI_U32         Reserved_VSD2P10STRIDE;
    HI_U32         Reserved_VSD2P10VFPOS;
    HI_U32         Reserved_VSD2P10VLPOS;
    HI_U32         Reserved_338[2];
    HI_U32         Reserved_VSD2P11RESO;
    HI_U32         Reserved_VSD2P11LADDR;
    HI_U32         Reserved_VSD2P11CADDR;
    HI_U32         Reserved_VSD2P11STRIDE;
    HI_U32         Reserved_VSD2P11VFPOS;
    HI_U32         Reserved_VSD2P11VLPOS;
    HI_U32         Reserved_339[2];
    HI_U32         Reserved_VSD2P12RESO;
    HI_U32         Reserved_VSD2P12LADDR;
    HI_U32         Reserved_VSD2P12CADDR;
    HI_U32         Reserved_VSD2P12STRIDE;
    HI_U32         Reserved_VSD2P12VFPOS;
    HI_U32         Reserved_VSD2P12VLPOS;
    HI_U32         Reserved_340[2];
    HI_U32         Reserved_VSD2P13RESO;
    HI_U32         Reserved_VSD2P13LADDR;
    HI_U32         Reserved_VSD2P13CADDR;
    HI_U32         Reserved_VSD2P13STRIDE;
    HI_U32         Reserved_VSD2P13VFPOS;
    HI_U32         Reserved_VSD2P13VLPOS;
    HI_U32         Reserved_341[2];
    HI_U32         Reserved_VSD2P14RESO;
    HI_U32         Reserved_VSD2P14LADDR;
    HI_U32         Reserved_VSD2P14CADDR;
    HI_U32         Reserved_VSD2P14STRIDE;
    HI_U32         Reserved_VSD2P14VFPOS;
    HI_U32         Reserved_VSD2P14VLPOS;
    HI_U32         Reserved_342[2];
    HI_U32         Reserved_VSD2P15RESO;
    HI_U32         Reserved_VSD2P15LADDR;
    HI_U32         Reserved_VSD2P15CADDR;
    HI_U32         Reserved_VSD2P15STRIDE;
    HI_U32         Reserved_VSD2P15VFPOS;
    HI_U32         Reserved_VSD2P15VLPOS;
    HI_U32         Reserved_343[387];
    HI_U32         Reserved_VSD216REGIONEN;
    HI_U32         Reserved_ed_344[254];
    HI_U32         Reserved_VSD3CTRL;
    HI_U32         Reserved_VSD3UPD;
    HI_U32         Reserved_ed_345[2];
    HI_U32         Reserved_VSD3CADDR;
    HI_U32         Reserved_VSD3CCADDR;
    HI_U32         Reserved_ed_346[3];
    HI_U32         Reserved_VSD3STRIDE;
    HI_U32         Reserved_VSD3IRESO;
    HI_U32         Reserved_ed_347;
    HI_U32         Reserved_VSD3LKEY;
    HI_U32         Reserved_VSD3CBMPARA;
    HI_U32         Reserved_ed_348[10];
    HI_U32         Reserved_VSD3DFPOS;
    HI_U32         Reserved_VSD3DLPOS;
    HI_U32         Reserved_VSD3VFPOS;
    HI_U32         Reserved_VSD3VLPOS;
    HI_U32         Reserved_VSD3BK;
    HI_U32         Reserved_ed_349[3];
    HI_U32         Reserved_VSD3CSCIDC;
    HI_U32         Reserved_VSD3CSCODC;
    HI_U32         Reserved_VSD3CSCP0;
    HI_U32         Reserved_VSD3CSCP1;
    HI_U32         Reserved_VSD3CSCP2;
    HI_U32         Reserved_VSD3CSCP3;
    HI_U32         Reserved_VSD3CSCP4;
    HI_U32         Reserved_ed_350;
    HI_U32         Reserved_VSD3ACM0;
    HI_U32         Reserved_VSD3ACM1;
    HI_U32         Reserved_VSD3ACM2;
    HI_U32         Reserved_VSD3ACM3;
    HI_U32         Reserved_VSD3ACM4;
    HI_U32         Reserved_VSD3ACM5;
    HI_U32         Reserved_VSD3ACM6;
    HI_U32         Reserved_VSD3ACM7;
    HI_U32         Reserved_VSD3HSP;
    HI_U32         Reserved_VSD3HLOFFSET;
    HI_U32         Reserved_VSD3HCOFFSET;
    HI_U32         Reserved_ed_351[3];
    HI_U32         Reserved_VSD3VSP;
    HI_U32         Reserved_VSD3VSR;
    HI_U32         Reserved_VSD3VOFFSET;
    HI_U32         Reserved_VSD3ZMEORESO;
    HI_U32         Reserved_VSD3ZMEIRESO;
    HI_U32         Reserved_VSD3ZMEDBG;
    HI_U32         Reserved_VSD3MOSAIC[4];
    HI_U32         Reserved_VSD3ACCTHD1;
    HI_U32         Reserved_VSD3ACCTHD2;
    HI_U32         Reserved_ed_352[2];
    HI_U32         Reserved_VSD3ACCLOWN[3];
    HI_U32         Reserved_ed_353;
    HI_U32         Reserved_VSD3ACCMEDN[3];
    HI_U32         Reserved_ed_354;
    HI_U32         Reserved_VSD3ACCHIGHN[3];
    HI_U32         Reserved_ed_355;
    HI_U32         Reserved_VSD3ACCMLN[3];
    HI_U32         Reserved_ed_356;
    HI_U32         Reserved_VSD3ACCMHN[3];
    HI_U32         Reserved_ed_357;
    HI_U32         Reserved_VSD3ACC3LOW;
    HI_U32         Reserved_VSD3ACC3MED;
    HI_U32         Reserved_VSD3ACC3HIGH;
    HI_U32         Reserved_VSD3ACC8MLOW;
    HI_U32         Reserved_VSD3ACC8MHIGH;
    HI_U32         Reserved_VSD3ACCTOTAL;
    HI_U32         Reserved_358[2];
    HI_U32         Reserved_VSD3IFIRCOEF01;
    HI_U32         Reserved_VSD3IFIRCOEF23;
    HI_U32         Reserved_VSD3IFIRCOEF45;
    HI_U32         Reserved_VSD3IFIRCOEF67;
    HI_U32         Reserved_ed_359[156];
    HI_U32         Reserved_VSD3P0RESO;
    HI_U32         Reserved_VSD3P0LADDR;
    HI_U32         Reserved_VSD3P0CADDR;
    HI_U32         Reserved_VSD3P0STRIDE;
    HI_U32         Reserved_VSD3P0VFPOS;
    HI_U32         Reserved_VSD3P0VLPOS;
    HI_U32         Reserved_ed_360[2];
    HI_U32         Reserved_VSD3P1RESO;
    HI_U32         Reserved_VSD3P1LADDR;
    HI_U32         Reserved_VSD3P1CADDR;
    HI_U32         Reserved_VSD3P1STRIDE;
    HI_U32         Reserved_VSD3P1VFPOS;
    HI_U32         Reserved_VSD3P1VLPOS;
    HI_U32         Reserved_361[2];
    HI_U32         Reserved_VSD3P2RESO;
    HI_U32         Reserved_VSD3P2LADDR;
    HI_U32         Reserved_VSD3P2CADDR;
    HI_U32         Reserved_VSD3P2STRIDE;
    HI_U32         Reserved_VSD3P2VFPOS;
    HI_U32         Reserved_VSD3P2VLPOS;
    HI_U32         Reserved_362[2];
    HI_U32         Reserved_VSD3P3RESO;
    HI_U32         Reserved_VSD3P3LADDR;
    HI_U32         Reserved_VSD3P3CADDR;
    HI_U32         Reserved_VSD3P3STRIDE;
    HI_U32         Reserved_VSD3P3VFPOS;
    HI_U32         Reserved_VSD3P3VLPOS;
    HI_U32         Reserved_363[2];
    HI_U32         Reserved_VSD3P4RESO;
    HI_U32         Reserved_VSD3P4LADDR;
    HI_U32         Reserved_VSD3P4CADDR;
    HI_U32         Reserved_VSD3P4STRIDE;
    HI_U32         Reserved_VSD3P4VFPOS;
    HI_U32         Reserved_VSD3P4VLPOS;
    HI_U32         Reserved_364[2];
    HI_U32         Reserved_VSD3P5RESO;
    HI_U32         Reserved_VSD3P5LADDR;
    HI_U32         Reserved_VSD3P5CADDR;
    HI_U32         Reserved_VSD3P5STRIDE;
    HI_U32         Reserved_VSD3P5VFPOS;
    HI_U32         Reserved_VSD3P5VLPOS;
    HI_U32         Reserved_365[2];
    HI_U32         Reserved_VSD3P6RESO;
    HI_U32         Reserved_VSD3P6LADDR;
    HI_U32         Reserved_VSD3P6CADDR;
    HI_U32         Reserved_VSD3P6STRIDE;
    HI_U32         Reserved_VSD3P6VFPOS;
    HI_U32         Reserved_VSD3P6VLPOS;
    HI_U32         Reserved_366[2];
    HI_U32         Reserved_VSD3P7RESO;
    HI_U32         Reserved_VSD3P7LADDR;
    HI_U32         Reserved_VSD3P7CADDR;
    HI_U32         Reserved_VSD3P7STRIDE;
    HI_U32         Reserved_VSD3P7VFPOS;
    HI_U32         Reserved_VSD3P7VLPOS;
    HI_U32         Reserved_367[2];
    HI_U32         Reserved_VSD3P8RESO;
    HI_U32         Reserved_VSD3P8LADDR;
    HI_U32         Reserved_VSD3P8CADDR;
    HI_U32         Reserved_VSD3P8STRIDE;
    HI_U32         Reserved_VSD3P8VFPOS;
    HI_U32         Reserved_VSD3P8VLPOS;
    HI_U32         Reserved_368[2];
    HI_U32         Reserved_VSD3P9RESO;
    HI_U32         Reserved_VSD3P9LADDR;
    HI_U32         Reserved_VSD3P9CADDR;
    HI_U32         Reserved_VSD3P9STRIDE;
    HI_U32         Reserved_VSD3P9VFPOS;
    HI_U32         Reserved_VSD3P9VLPOS;
    HI_U32         Reserved_369[2];
    HI_U32         Reserved_VSD3P10RESO;
    HI_U32         Reserved_VSD3P10LADDR;
    HI_U32         Reserved_VSD3P10CADDR;
    HI_U32         Reserved_VSD3P10STRIDE;
    HI_U32         Reserved_VSD3P10VFPOS;
    HI_U32         Reserved_VSD3P10VLPOS;
    HI_U32         Reserved_370[2];
    HI_U32         Reserved_VSD3P11RESO;
    HI_U32         Reserved_VSD3P11LADDR;
    HI_U32         Reserved_VSD3P11CADDR;
    HI_U32         Reserved_VSD3P11STRIDE;
    HI_U32         Reserved_VSD3P11VFPOS;
    HI_U32         Reserved_VSD3P11VLPOS;
    HI_U32         Reserved_371[2];
    HI_U32         Reserved_VSD3P12RESO;
    HI_U32         Reserved_VSD3P12LADDR;
    HI_U32         Reserved_VSD3P12CADDR;
    HI_U32         Reserved_VSD3P12STRIDE;
    HI_U32         Reserved_VSD3P12VFPOS;
    HI_U32         Reserved_VSD3P12VLPOS;
    HI_U32         Reserved_372[2];
    HI_U32         Reserved_VSD3P13RESO;
    HI_U32         Reserved_VSD3P13LADDR;
    HI_U32         Reserved_VSD3P13CADDR;
    HI_U32         Reserved_VSD3P13STRIDE;
    HI_U32         Reserved_VSD3P13VFPOS;
    HI_U32         Reserved_VSD3P13VLPOS;
    HI_U32         Reserved_373[2];
    HI_U32         Reserved_VSD3P14RESO;
    HI_U32         Reserved_VSD3P14LADDR;
    HI_U32         Reserved_VSD3P14CADDR;
    HI_U32         Reserved_VSD3P14STRIDE;
    HI_U32         Reserved_VSD3P14VFPOS;
    HI_U32         Reserved_VSD3P14VLPOS;
    HI_U32         Reserved_374[2];
    HI_U32         Reserved_VSD3P15RESO;
    HI_U32         Reserved_VSD3P15LADDR;
    HI_U32         Reserved_VSD3P15CADDR;
    HI_U32         Reserved_VSD3P15STRIDE;
    HI_U32         Reserved_VSD3P15VFPOS;
    HI_U32         Reserved_VSD3P15VLPOS;
    HI_U32         Reserved_375[387];
    HI_U32         Reserved_VSD316REGIONEN;
    HI_U32         Reserved_ed_376[254];
    HI_U32         Reserved_VSD4CTRL;
    HI_U32         Reserved_VSD4UPD;
    HI_U32         Reserved_ed_377[2];
    HI_U32         Reserved_VSD4CADDR;
    HI_U32         Reserved_VSD4CCADDR;
    HI_U32         Reserved_ed_378[3];
    HI_U32         Reserved_VSD4STRIDE;
    HI_U32         Reserved_VSD4IRESO;
    HI_U32         Reserved_ed_379;
    HI_U32         Reserved_VSD4LKEY;
    HI_U32         Reserved_VSD4CBMPARA;
    HI_U32         Reserved_ed_380[10];
    HI_U32         Reserved_VSD4DFPOS;
    HI_U32         Reserved_VSD4DLPOS;
    HI_U32         Reserved_VSD4VFPOS;
    HI_U32         Reserved_VSD4VLPOS;
    HI_U32         Reserved_VSD4BK;
    HI_U32         Reserved_ed_381[3];
    HI_U32         Reserved_VSD4CSCIDC;
    HI_U32         Reserved_VSD4CSCODC;
    HI_U32         Reserved_VSD4CSCP0;
    HI_U32         Reserved_VSD4CSCP1;
    HI_U32         Reserved_VSD4CSCP2;
    HI_U32         Reserved_VSD4CSCP3;
    HI_U32         Reserved_VSD4CSCP4;
    HI_U32         Reserved_ed_382;
    HI_U32         Reserved_VSD4ACM0;
    HI_U32         Reserved_VSD4ACM1;
    HI_U32         Reserved_VSD4ACM2;
    HI_U32         Reserved_VSD4ACM3;
    HI_U32         Reserved_VSD4ACM4;
    HI_U32         Reserved_VSD4ACM5;
    HI_U32         Reserved_VSD4ACM6;
    HI_U32         Reserved_VSD4ACM7;
    HI_U32         Reserved_VSD4HSP;
    HI_U32         Reserved_VSD4HLOFFSET;
    HI_U32         Reserved_VSD4HCOFFSET;
    HI_U32         Reserved_ed_383[3];
    HI_U32         Reserved_VSD4VSP;
    HI_U32         Reserved_VSD4VSR;
    HI_U32         Reserved_VSD4VOFFSET;
    HI_U32         Reserved_VSD4ZMEORESO;
    HI_U32         Reserved_VSD4ZMEIRESO;
    HI_U32         Reserved_VSD4ZMEDBG;
    HI_U32         Reserved_VSD4MOSAIC[4];
    HI_U32         Reserved_VSD4ACCTHD1;
    HI_U32         Reserved_VSD4ACCTHD2;
    HI_U32         Reserved_ed_384[2];
    HI_U32         Reserved_VSD4ACCLOWN[3];
    HI_U32         Reserved_ed_385;
    HI_U32         Reserved_VSD4ACCMEDN[3];
    HI_U32         Reserved_ed_386;
    HI_U32         Reserved_VSD4ACCHIGHN[3];
    HI_U32         Reserved_ed_387;
    HI_U32         Reserved_VSD4ACCMLN[3];
    HI_U32         Reserved_ed_388;
    HI_U32         Reserved_VSD4ACCMHN[3];
    HI_U32         Reserved_ed_389;
    HI_U32         Reserved_VSD4ACC3LOW;
    HI_U32         Reserved_VSD4ACC3MED;
    HI_U32         Reserved_VSD4ACC3HIGH;
    HI_U32         Reserved_VSD4ACC8MLOW;
    HI_U32         Reserved_VSD4ACC8MHIGH;
    HI_U32         Reserved_VSD4ACCTOTAL;
    HI_U32         Reserved_390[2];
    HI_U32         Reserved_VSD4IFIRCOEF01;
    HI_U32         Reserved_VSD4IFIRCOEF23;
    HI_U32         Reserved_VSD4IFIRCOEF45;
    HI_U32         Reserved_VSD4IFIRCOEF67;
    HI_U32         Reserved_ed_391[156];
    HI_U32         Reserved_VSD4P0RESO;
    HI_U32         Reserved_VSD4P0LADDR;
    HI_U32         Reserved_VSD4P0CADDR;
    HI_U32         Reserved_VSD4P0STRIDE;
    HI_U32         Reserved_VSD4P0VFPOS;
    HI_U32         Reserved_VSD4P0VLPOS;
    HI_U32         Reserved_ed_392[2];
    HI_U32         Reserved_VSD4P1RESO;
    HI_U32         Reserved_VSD4P1LADDR;
    HI_U32         Reserved_VSD4P1CADDR;
    HI_U32         Reserved_VSD4P1STRIDE;
    HI_U32         Reserved_VSD4P1VFPOS;
    HI_U32         Reserved_VSD4P1VLPOS;
    HI_U32         Reserved_393[2];
    HI_U32         Reserved_VSD4P2RESO;
    HI_U32         Reserved_VSD4P2LADDR;
    HI_U32         Reserved_VSD4P2CADDR;
    HI_U32         Reserved_VSD4P2STRIDE;
    HI_U32         Reserved_VSD4P2VFPOS;
    HI_U32         Reserved_VSD4P2VLPOS;
    HI_U32         Reserved_394[2];
    HI_U32         Reserved_VSD4P3RESO;
    HI_U32         Reserved_VSD4P3LADDR;
    HI_U32         Reserved_VSD4P3CADDR;
    HI_U32         Reserved_VSD4P3STRIDE;
    HI_U32         Reserved_VSD4P3VFPOS;
    HI_U32         Reserved_VSD4P3VLPOS;
    HI_U32         Reserved_395[2];
    HI_U32         Reserved_VSD4P4RESO;
    HI_U32         Reserved_VSD4P4LADDR;
    HI_U32         Reserved_VSD4P4CADDR;
    HI_U32         Reserved_VSD4P4STRIDE;
    HI_U32         Reserved_VSD4P4VFPOS;
    HI_U32         Reserved_VSD4P4VLPOS;
    HI_U32         Reserved_396[2];
    HI_U32         Reserved_VSD4P5RESO;
    HI_U32         Reserved_VSD4P5LADDR;
    HI_U32         Reserved_VSD4P5CADDR;
    HI_U32         Reserved_VSD4P5STRIDE;
    HI_U32         Reserved_VSD4P5VFPOS;
    HI_U32         Reserved_VSD4P5VLPOS;
    HI_U32         Reserved_397[2];
    HI_U32         Reserved_VSD4P6RESO;
    HI_U32         Reserved_VSD4P6LADDR;
    HI_U32         Reserved_VSD4P6CADDR;
    HI_U32         Reserved_VSD4P6STRIDE;
    HI_U32         Reserved_VSD4P6VFPOS;
    HI_U32         Reserved_VSD4P6VLPOS;
    HI_U32         Reserved_398[2];
    HI_U32         Reserved_VSD4P7RESO;
    HI_U32         Reserved_VSD4P7LADDR;
    HI_U32         Reserved_VSD4P7CADDR;
    HI_U32         Reserved_VSD4P7STRIDE;
    HI_U32         Reserved_VSD4P7VFPOS;
    HI_U32         Reserved_VSD4P7VLPOS;
    HI_U32         Reserved_399[2];
    HI_U32         Reserved_VSD4P8RESO;
    HI_U32         Reserved_VSD4P8LADDR;
    HI_U32         Reserved_VSD4P8CADDR;
    HI_U32         Reserved_VSD4P8STRIDE;
    HI_U32         Reserved_VSD4P8VFPOS;
    HI_U32         Reserved_VSD4P8VLPOS;
    HI_U32         Reserved_400[2];
    HI_U32         Reserved_VSD4P9RESO;
    HI_U32         Reserved_VSD4P9LADDR;
    HI_U32         Reserved_VSD4P9CADDR;
    HI_U32         Reserved_VSD4P9STRIDE;
    HI_U32         Reserved_VSD4P9VFPOS;
    HI_U32         Reserved_VSD4P9VLPOS;
    HI_U32         Reserved_401[2];
    HI_U32         Reserved_VSD4P10RESO;
    HI_U32         Reserved_VSD4P10LADDR;
    HI_U32         Reserved_VSD4P10CADDR;
    HI_U32         Reserved_VSD4P10STRIDE;
    HI_U32         Reserved_VSD4P10VFPOS;
    HI_U32         Reserved_VSD4P10VLPOS;
    HI_U32         Reserved_402[2];
    HI_U32         Reserved_VSD4P11RESO;
    HI_U32         Reserved_VSD4P11LADDR;
    HI_U32         Reserved_VSD4P11CADDR;
    HI_U32         Reserved_VSD4P11STRIDE;
    HI_U32         Reserved_VSD4P11VFPOS;
    HI_U32         Reserved_VSD4P11VLPOS;
    HI_U32         Reserved_403[2];
    HI_U32         Reserved_VSD4P12RESO;
    HI_U32         Reserved_VSD4P12LADDR;
    HI_U32         Reserved_VSD4P12CADDR;
    HI_U32         Reserved_VSD4P12STRIDE;
    HI_U32         Reserved_VSD4P12VFPOS;
    HI_U32         Reserved_VSD4P12VLPOS;
    HI_U32         Reserved_404[2];
    HI_U32         Reserved_VSD4P13RESO;
    HI_U32         Reserved_VSD4P13LADDR;
    HI_U32         Reserved_VSD4P13CADDR;
    HI_U32         Reserved_VSD4P13STRIDE;
    HI_U32         Reserved_VSD4P13VFPOS;
    HI_U32         Reserved_VSD4P13VLPOS;
    HI_U32         Reserved_405[2];
    HI_U32         Reserved_VSD4P14RESO;
    HI_U32         Reserved_VSD4P14LADDR;
    HI_U32         Reserved_VSD4P14CADDR;
    HI_U32         Reserved_VSD4P14STRIDE;
    HI_U32         Reserved_VSD4P14VFPOS;
    HI_U32         Reserved_VSD4P14VLPOS;
    HI_U32         Reserved_406[2];
    HI_U32         Reserved_VSD4P15RESO;
    HI_U32         Reserved_VSD4P15LADDR;
    HI_U32         Reserved_VSD4P15CADDR;
    HI_U32         Reserved_VSD4P15STRIDE;
    HI_U32         Reserved_VSD4P15VFPOS;
    HI_U32         Reserved_VSD4P15VLPOS;
    HI_U32         Reserved_407[387];
    HI_U32         Reserved_VSD416REGIONEN;
    HI_U32         Reserved_ed_408[254];    
    HI_U32         Reserved_WBC0CTRL;
    HI_U32         Reserved_WBC0UPD;
    HI_U32         Reserved_WBC0ADDR;
    HI_U32         Reserved_WBC0STRIDE;
    HI_U32         Reserved_WBC0ORESO;
    HI_U32         Reserved_ed_409[3];
    HI_U32         Reserved_WBC0CADDR;
    HI_U32         Reserved_WBC0CSTRIDE;
    HI_U32         Reserved_ed_410[38];
    HI_U32         Reserved_WBC0HSP;
    HI_U32         Reserved_WBC0HLOFFSET;
    HI_U32         Reserved_WBC0HCOFFSET;
    HI_U32         Reserved_ed_411[3];
    HI_U32         Reserved_WBC0VSP;
    HI_U32         Reserved_WBC0VSR;
    HI_U32         Reserved_WBC0VOFFSET;
    HI_U32         Reserved_WBC0ZMEORESO;
    HI_U32         Reserved_WBC0ZMEIRESO;
    HI_U32         Reserved_WBC0ZMEDBG;
    HI_U32         Reserved_412[68];
    #endif
    HI_U32         Reserved_412[3454];
    U_WBCCTRL           WBCCTRL;        //0x8200
    U_WBCUPD            WBCUPD;
    U_WBCADDR           WBCADDR;
    U_WBCSTRIDE         WBCSTRIDE;
    U_WBCORESO          WBCORESO;
    HI_U32         Reserved_413[3];
    U_WBCCADDR          WBCCADDR;
    U_WBCCSTRIDE        WBCCSTRIDE;
    HI_U32         Reserved_414[38];
    U_WBCHSP            WBCHSP;
    U_WBCHLOFFSET       WBCHLOFFSET;
    U_WBCHCOFFSET       WBCHCOFFSET;
    /* HI_U32         Reserved_415[3];   no */
    U_WBCVSP            WBCVSP;
    U_WBCVSR            WBCVSR;
    U_WBCVOFFSET        WBCVOFFSET;
    U_WBCZMEORESO       WBCZMEORESO;
    U_WBCZMEIRESO       WBCZMEIRESO;    //0x82DC
    
#if 0
    U_WBCZMEDBG         WBCZMEDBG;      
    HI_U32         Reserved_416[324];
    HI_U32         Reserved_VADCTRL;
    HI_U32         Reserved_VADUPD;
    HI_U32         Reserved_417[2];
    HI_U32         Reserved_VADCADDR;
    HI_U32         Reserved_VADCCADDR;
    HI_U32         Reserved_418[3];
    HI_U32         Reserved_VADSTRIDE;
    HI_U32         Reserved_VADIRESO;
    HI_U32         Reserved_ed_419;
    HI_U32         Reserved_VADLKEY;
    HI_U32         Reserved_VADCBMPARA;
    HI_U32         Reserved_ed_420[10];
    HI_U32         Reserved_VADDFPOS;
    HI_U32         Reserved_VADDLPOS;
    HI_U32         Reserved_VADVFPOS;
    HI_U32         Reserved_VADVLPOS;
    HI_U32         Reserved_VADBK;
    HI_U32         Reserved_ed_421[3];
    HI_U32         Reserved_VADCSCIDC;
    HI_U32         Reserved_VADCSCODC;
    HI_U32         Reserved_VADCSCP0;
    HI_U32         Reserved_VADCSCP1;
    HI_U32         Reserved_VADCSCP2;
    HI_U32         Reserved_VADCSCP3;
    HI_U32         Reserved_VADCSCP4;
    HI_U32         Reserved_ed_422;
    HI_U32         Reserved_VADACM0;
    HI_U32         Reserved_VADACM1;
    HI_U32         Reserved_VADACM2;
    HI_U32         Reserved_VADACM3;
    HI_U32         Reserved_VADACM4;
    HI_U32         Reserved_VADACM5;
    HI_U32         Reserved_VADACM6;
    HI_U32         Reserved_VADACM7;
    HI_U32         Reserved_VADHSP;
    HI_U32         Reserved_VADHLOFFSET;
    HI_U32         Reserved_VADHCOFFSET;
    HI_U32         Reserved_ed_423[3];
    HI_U32         Reserved_VADVSP;
    HI_U32         Reserved_VADVSR;
    HI_U32         Reserved_VADVOFFSET;
    HI_U32         Reserved_VADZMEORESO;
    HI_U32         Reserved_VADZMEIRESO;
    HI_U32         Reserved_VADZMEDBG;
    HI_U32         Reserved_VADMOSAIC[4];
    HI_U32         Reserved_VADACCTHD1;
    HI_U32         Reserved_VADACCTHD2;
    HI_U32         Reserved_ed_424[2];
    HI_U32         Reserved_VADACCLOWN[3];
    HI_U32         Reserved_ed_425;
    HI_U32         Reserved_VADACCMEDN[3];
    HI_U32         Reserved_ed_426;
    HI_U32         Reserved_VADACCHIGHN[3];
    HI_U32         Reserved_ed_427;
    HI_U32         Reserved_VADACCMLN[3];
    HI_U32         Reserved_ed_428;
    HI_U32         Reserved_VADACCMHN[3];
    HI_U32         Reserved_ed_429;
    HI_U32         Reserved_VADACC3LOW;
    HI_U32         Reserved_VADACC3MED;
    HI_U32         Reserved_VADACC3HIGH;
    HI_U32         Reserved_VADACC8MLOW;
    HI_U32         Reserved_VADACC8MHIGH;
    HI_U32         Reserved_VADACCTOTAL;
    HI_U32         Reserved_ed_430[2];
    HI_U32         Reserved_VADIFIRCOEF01;
    HI_U32         Reserved_VADIFIRCOEF23;
    HI_U32         Reserved_VADIFIRCOEF45;
    HI_U32         Reserved_VADIFIRCOEF67;
    HI_U32         Reserved_431[412];
    #endif
    HI_U32         Reserved_431[840];
    U_G0CTRL             G0CTRL;    //0x9000
    U_G0UPD              G0UPD;
    U_G0ADDR             G0ADDR;
    U_G0STRIDE           G0STRIDE;
    U_G0CBMPARA          G0CBMPARA;
    U_G0CKEYMAX          G0CKEYMAX;
    U_G0CKEYMIN          G0CKEYMIN;
    U_G0CMASK            G0CMASK;
    U_G0IRESO            G0IRESO;
    U_G0ORESO            G0ORESO;
    HI_U32         Reserved_G0SFPOS;
    U_G0DFPOS            G0DFPOS;
    U_G0DLPOS            G0DLPOS;
    HI_U32         Reserved_432[3];
    HI_U32         Reserved_G0HSP;
    HI_U32         Reserved_G0HOFFSET;
    HI_U32         Reserved_G0VSP;
    HI_U32         Reserved_G0VSR;
    HI_U32         Reserved_G0VOFFSET;
    HI_U32         Reserved_G0ZMEORESO;
    HI_U32         Reserved_G0ZMEIRESO;
    HI_U32         Reserved_433[17];
    U_G0CSCIDC           G0CSCIDC;
    U_G0CSCODC           G0CSCODC;
    U_G0CSCP0            G0CSCP0;
    U_G0CSCP1            G0CSCP1;
    U_G0CSCP2            G0CSCP2;
    U_G0CSCP3            G0CSCP3;
    U_G0CSCP4            G0CSCP4;
    HI_U32         Reserved_434[17];
    HI_U32         Reserved_G0P0ADDR;
    HI_U32         Reserved_G0P0STRIDE;
    HI_U32         Reserved_G0P0DFPOS;
    HI_U32         Reserved_G0P0DLPOS;
    HI_U32         Reserved_G0P0RESO;
    HI_U32         Reserved_G0P1ADDR;
    HI_U32         Reserved_G0P1STRIDE;
    HI_U32         Reserved_G0P1DFPOS;
    HI_U32         Reserved_G0P1DLPOS;
    HI_U32         Reserved_G0P1RESO;
    HI_U32         Reserved_G0P2ADDR;
    HI_U32         Reserved_G0P2STRIDE;
    HI_U32         Reserved_G0P2DFPOS;
    HI_U32         Reserved_G0P2DLPOS;
    HI_U32         Reserved_G0P2RESO;
    HI_U32         Reserved_G0P3ADDR;
    HI_U32         Reserved_G0P3STRIDE;
    HI_U32         Reserved_G0P3DFPOS;
    HI_U32         Reserved_G0P3DLPOS;
    HI_U32         Reserved_G0P3RESO;
    HI_U32         Reserved_G0P4ADDR;
    HI_U32         Reserved_G0P4STRIDE;
    HI_U32         Reserved_G0P4DFPOS;
    HI_U32         Reserved_G0P4DLPOS;
    HI_U32         Reserved_G0P4RESO;
    HI_U32         Reserved_G0P5ADDR;
    HI_U32         Reserved_G0P5STRIDE;
    HI_U32         Reserved_G0P5DFPOS;
    HI_U32         Reserved_G0P5DLPOS;
    HI_U32         Reserved_G0P5RESO;
    HI_U32         Reserved_G0P6ADDR;
    HI_U32         Reserved_G0P6STRIDE;
    HI_U32         Reserved_G0P6DFPOS;
    HI_U32         Reserved_G0P6DLPOS;
    HI_U32         Reserved_G0P6RESO;
    HI_U32         Reserved_G0P7ADDR;
    HI_U32         Reserved_G0P7STRIDE;
    HI_U32         Reserved_G0P7DFPOS;
    HI_U32         Reserved_G0P7DLPOS;
    HI_U32         Reserved_G0P7RESO;
    HI_U32         Reserved_G08SLICEEN;
    HI_U32         Reserved_ed_435[3];
    HI_U32         Reserved_G0DCMPMODE;
    U_G0DCMPBANKWIDTH    G0DCMPBANKWIDTH;
    U_G0DCMPSTRIDE       G0DCMPSTRIDE;
    U_G0DCMPAADDR        G0DCMPAADDR;
    U_G0DCMPRADDR        G0DCMPRADDR;
    U_G0DCMPGADDR        G0DCMPGADDR;
    U_G0DCMPBADDR        G0DCMPBADDR;
    HI_U32         Reserved_436[13];
    HI_U32         Reserved_G1CTRL;
    HI_U32         Reserved_G1UPD;
    HI_U32         Reserved_G1ADDR;
    HI_U32         Reserved_G1STRIDE;
    HI_U32         Reserved_G1CBMPARA;
    HI_U32         Reserved_G1CKEYMAX;
    HI_U32         Reserved_G1CKEYMIN;
    HI_U32         Reserved_G1CMASK;
    HI_U32         Reserved_G1IRESO;
    HI_U32         Reserved_G1ORESO;
    HI_U32         Reserved_G1SFPOS;
    HI_U32         Reserved_G1DFPOS;
    HI_U32         Reserved_G1DLPOS;
    HI_U32         Reserved_ed_437[3];
    HI_U32         Reserved_G1HSP;
    HI_U32         Reserved_G1HOFFSET;
    HI_U32         Reserved_G1VSP;
    HI_U32         Reserved_G1VSR;
    HI_U32         Reserved_G1VOFFSET;
    HI_U32         Reserved_G1ZMEORESO;
    HI_U32         Reserved_G1ZMEIRESO;
    HI_U32         Reserved_ed_438[17];
    HI_U32         Reserved_G1CSCIDC;
    HI_U32         Reserved_G1CSCODC;
    HI_U32         Reserved_G1CSCP0;
    HI_U32         Reserved_G1CSCP1;
    HI_U32         Reserved_G1CSCP2;
    HI_U32         Reserved_G1CSCP3;
    HI_U32         Reserved_G1CSCP4;
    HI_U32         Reserved_ed_439[17];
    HI_U32         Reserved_G1P0ADDR;
    HI_U32         Reserved_G1P0STRIDE;
    HI_U32         Reserved_G1P0DFPOS;
    HI_U32         Reserved_G1P0DLPOS;
    HI_U32         Reserved_G1P0RESO;
    HI_U32         Reserved_G1P1ADDR;
    HI_U32         Reserved_G1P1STRIDE;
    HI_U32         Reserved_G1P1DFPOS;
    HI_U32         Reserved_G1P1DLPOS;
    HI_U32         Reserved_G1P1RESO;
    HI_U32         Reserved_G1P2ADDR;
    HI_U32         Reserved_G1P2STRIDE;
    HI_U32         Reserved_G1P2DFPOS;
    HI_U32         Reserved_G1P2DLPOS;
    HI_U32         Reserved_G1P2RESO;
    HI_U32         Reserved_G1P3ADDR;
    HI_U32         Reserved_G1P3STRIDE;
    HI_U32         Reserved_G1P3DFPOS;
    HI_U32         Reserved_G1P3DLPOS;
    HI_U32         Reserved_G1P3RESO;
    HI_U32         Reserved_G1P4ADDR;
    HI_U32         Reserved_G1P4STRIDE;
    HI_U32         Reserved_G1P4DFPOS;
    HI_U32         Reserved_G1P4DLPOS;
    HI_U32         Reserved_G1P4RESO;
    HI_U32         Reserved_G1P5ADDR;
    HI_U32         Reserved_G1P5STRIDE;
    HI_U32         Reserved_G1P5DFPOS;
    HI_U32         Reserved_G1P5DLPOS;
    HI_U32         Reserved_G1P5RESO;
    HI_U32         Reserved_G1P6ADDR;
    HI_U32         Reserved_G1P6STRIDE;
    HI_U32         Reserved_G1P6DFPOS;
    HI_U32         Reserved_G1P6DLPOS;
    HI_U32         Reserved_G1P6RESO;
    HI_U32         Reserved_G1P7ADDR;
    HI_U32         Reserved_G1P7STRIDE;
    HI_U32         Reserved_G1P7DFPOS;
    HI_U32         Reserved_G1P7DLPOS;
    HI_U32         Reserved_G1P7RESO;
    HI_U32         Reserved_G18SLICEEN;
    HI_U32         Reserved_ed_440[3];
    HI_U32         Reserved_G1DCMPMODE;
    HI_U32         Reserved_G1DCMPBANKWIDTH;
    HI_U32         Reserved_G1DCMPSTRIDE;
    HI_U32         Reserved_G1DCMPAADDR;
    HI_U32         Reserved_G1DCMPRADDR;
    HI_U32         Reserved_G1DCMPGADDR;
    HI_U32         Reserved_G1DCMPBADDR;
    HI_U32         Reserved_441[13];
    U_G2CTRL             G2CTRL;
    U_G2UPD              G2UPD;
    U_G2ADDR             G2ADDR;
    U_G2STRIDE           G2STRIDE;
    U_G2CBMPARA          G2CBMPARA;
    U_G2CKEYMAX          G2CKEYMAX;
    U_G2CKEYMIN          G2CKEYMIN;
    U_G2CMASK            G2CMASK;
    U_G2IRESO            G2IRESO;
    U_G2ORESO            G2ORESO;
    HI_U32         Reserved_G2SFPOS;
    U_G2DFPOS            G2DFPOS;
    U_G2DLPOS            G2DLPOS;
    HI_U32         Reserved_442[3];
    HI_U32         Reserved_G2HSP;
    HI_U32         Reserved_G2HOFFSET;
    HI_U32         Reserved_G2VSP;
    HI_U32         Reserved_G2VSR;
    HI_U32         Reserved_G2VOFFSET;
    HI_U32         Reserved_G2ZMEORESO;
    HI_U32         Reserved_G2ZMEIRESO;
    HI_U32         Reserved_443[17];
    U_G2CSCIDC           G2CSCIDC;
    U_G2CSCODC           G2CSCODC;
    U_G2CSCP0            G2CSCP0;
    U_G2CSCP1            G2CSCP1;
    U_G2CSCP2            G2CSCP2;
    U_G2CSCP3            G2CSCP3;
    U_G2CSCP4            G2CSCP4;
    HI_U32         Reserved_444[17];
    HI_U32         Reserved_G2P0ADDR;
    HI_U32         Reserved_G2P0STRIDE;
    HI_U32         Reserved_G2P0DFPOS;
    HI_U32         Reserved_G2P0DLPOS;
    HI_U32         Reserved_G2P0RESO;
    HI_U32         Reserved_G2P1ADDR;
    HI_U32         Reserved_G2P1STRIDE;
    HI_U32         Reserved_G2P1DFPOS;
    HI_U32         Reserved_G2P1DLPOS;
    HI_U32         Reserved_G2P1RESO;
    HI_U32         Reserved_G2P2ADDR;
    HI_U32         Reserved_G2P2STRIDE;
    HI_U32         Reserved_G2P2DFPOS;
    HI_U32         Reserved_G2P2DLPOS;
    HI_U32         Reserved_G2P2RESO;
    HI_U32         Reserved_G2P3ADDR;
    HI_U32         Reserved_G2P3STRIDE;
    HI_U32         Reserved_G2P3DFPOS;
    HI_U32         Reserved_G2P3DLPOS;
    HI_U32         Reserved_G2P3RESO;
    HI_U32         Reserved_G2P4ADDR;
    HI_U32         Reserved_G2P4STRIDE;
    HI_U32         Reserved_G2P4DFPOS;
    HI_U32         Reserved_G2P4DLPOS;
    HI_U32         Reserved_G2P4RESO;
    HI_U32         Reserved_G2P5ADDR;
    HI_U32         Reserved_G2P5STRIDE;
    HI_U32         Reserved_G2P5DFPOS;
    HI_U32         Reserved_G2P5DLPOS;
    HI_U32         Reserved_G2P5RESO;
    HI_U32         Reserved_G2P6ADDR;
    HI_U32         Reserved_G2P6STRIDE;
    HI_U32         Reserved_G2P6DFPOS;
    HI_U32         Reserved_G2P6DLPOS;
    HI_U32         Reserved_G2P6RESO;
    HI_U32         Reserved_G2P7ADDR;
    HI_U32         Reserved_G2P7STRIDE;
    HI_U32         Reserved_G2P7DFPOS;
    HI_U32         Reserved_G2P7DLPOS;
    HI_U32         Reserved_G2P7RESO;
    HI_U32         Reserved_G28SLICEEN;
    HI_U32         Reserved_445[3];
    HI_U32         Reserved_G2DCMPMODE;
    HI_U32         Reserved_G2DCMPBANKWIDTH;
    HI_U32         Reserved_G2DCMPSTRIDE;
    HI_U32         Reserved_G2DCMPAADDR;
    HI_U32         Reserved_G2DCMPRADDR;
    HI_U32         Reserved_G2DCMPGADDR;
    HI_U32         Reserved_G2DCMPBADDR;
    HI_U32         Reserved_446[13];
    U_G3CTRL             G3CTRL;
    U_G3UPD              G3UPD;
    U_G3ADDR             G3ADDR;
    U_G3STRIDE           G3STRIDE;
    U_G3CBMPARA          G3CBMPARA;
    U_G3CKEYMAX          G3CKEYMAX;
    U_G3CKEYMIN          G3CKEYMIN;
    U_G3CMASK            G3CMASK;
    U_G3IRESO            G3IRESO;
    U_G3ORESO            G3ORESO;
    HI_U32         Reserved_G3SFPOS;
    U_G3DFPOS            G3DFPOS;
    U_G3DLPOS            G3DLPOS;
    HI_U32         Reserved_447[3];
    HI_U32         Reserved_G3HSP;
    HI_U32         Reserved_G3HOFFSET;
    HI_U32         Reserved_G3VSP;
    HI_U32         Reserved_G3VSR;
    HI_U32         Reserved_G3VOFFSET;
    HI_U32         Reserved_G3ZMEORESO;
    HI_U32         Reserved_G3ZMEIRESO;
    HI_U32         Reserved_448[17];
    U_G3CSCIDC           G3CSCIDC;
    U_G3CSCODC           G3CSCODC;
    U_G3CSCP0            G3CSCP0;
    U_G3CSCP1            G3CSCP1;
    U_G3CSCP2            G3CSCP2;
    U_G3CSCP3            G3CSCP3;
    U_G3CSCP4            G3CSCP4;
    HI_U32         Reserved_449[17];
    HI_U32         Reserved_G3P0ADDR;
    HI_U32         Reserved_G3P0STRIDE;
    HI_U32         Reserved_G3P0DFPOS;
    HI_U32         Reserved_G3P0DLPOS;
    HI_U32         Reserved_G3P0RESO;
    HI_U32         Reserved_G3P1ADDR;
    HI_U32         Reserved_G3P1STRIDE;
    HI_U32         Reserved_G3P1DFPOS;
    HI_U32         Reserved_G3P1DLPOS;
    HI_U32         Reserved_G3P1RESO;
    HI_U32         Reserved_G3P2ADDR;
    HI_U32         Reserved_G3P2STRIDE;
    HI_U32         Reserved_G3P2DFPOS;
    HI_U32         Reserved_G3P2DLPOS;
    HI_U32         Reserved_G3P2RESO;
    HI_U32         Reserved_G3P3ADDR;
    HI_U32         Reserved_G3P3STRIDE;
    HI_U32         Reserved_G3P3DFPOS;
    HI_U32         Reserved_G3P3DLPOS;
    HI_U32         Reserved_G3P3RESO;
    HI_U32         Reserved_G3P4ADDR;
    HI_U32         Reserved_G3P4STRIDE;
    HI_U32         Reserved_G3P4DFPOS;
    HI_U32         Reserved_G3P4DLPOS;
    HI_U32         Reserved_G3P4RESO;
    HI_U32         Reserved_G3P5ADDR;
    HI_U32         Reserved_G3P5STRIDE;
    HI_U32         Reserved_G3P5DFPOS;
    HI_U32         Reserved_G3P5DLPOS;
    HI_U32         Reserved_G3P5RESO;
    HI_U32         Reserved_G3P6ADDR;
    HI_U32         Reserved_G3P6STRIDE;
    HI_U32         Reserved_G3P6DFPOS;
    HI_U32         Reserved_G3P6DLPOS;
    HI_U32         Reserved_G3P6RESO;
    HI_U32         Reserved_G3P7ADDR;
    HI_U32         Reserved_G3P7STRIDE;
    HI_U32         Reserved_G3P7DFPOS;
    HI_U32         Reserved_G3P7DLPOS;
    HI_U32         Reserved_G3P7RESO;
    HI_U32         Reserved_G38SLICEEN;
    HI_U32         Reserved_ed_450[3];
    HI_U32         Reserved_G3DCMPMODE;
    HI_U32         Reserved_G3DCMPBANKWIDTH;
    HI_U32         Reserved_G3DCMPSTRIDE;
    HI_U32         Reserved_G3DCMPAADDR;
    HI_U32         Reserved_G3DCMPRADDR;
    HI_U32         Reserved_G3DCMPGADDR;
    HI_U32         Reserved_G3DCMPBADDR;
    HI_U32         Reserved_451[13];
    HI_U32         Reserved_G4CTRL;
    HI_U32         Reserved_G4UPD;
    HI_U32         Reserved_G4ADDR;
    HI_U32         Reserved_G4STRIDE;
    HI_U32         Reserved_G4CBMPARA;
    HI_U32         Reserved_G4CKEYMAX;
    HI_U32         Reserved_G4CKEYMIN;
    HI_U32         Reserved_G4CMASK;
    HI_U32         Reserved_G4IRESO;
    HI_U32         Reserved_G4ORESO;
    HI_U32         Reserved_G4SFPOS;
    HI_U32         Reserved_G4DFPOS;
    HI_U32         Reserved_G4DLPOS;
    HI_U32         Reserved_ed_452[3];
    HI_U32         Reserved_G4HSP;
    HI_U32         Reserved_G4HOFFSET;
    HI_U32         Reserved_G4VSP;
    HI_U32         Reserved_G4VSR;
    HI_U32         Reserved_G4VOFFSET;
    HI_U32         Reserved_G4ZMEORESO;
    HI_U32         Reserved_G4ZMEIRESO;
    HI_U32         Reserved_ed_453[17];
    HI_U32         Reserved_G4CSCIDC;
    HI_U32         Reserved_G4CSCODC;
    HI_U32         Reserved_G4CSCP0;
    HI_U32         Reserved_G4CSCP1;
    HI_U32         Reserved_G4CSCP2;
    HI_U32         Reserved_G4CSCP3;
    HI_U32         Reserved_G4CSCP4;
    HI_U32         Reserved_454[17];
    HI_U32         Reserved_G4P0ADDR;
    HI_U32         Reserved_G4P0STRIDE;
    HI_U32         Reserved_G4P0DFPOS;
    HI_U32         Reserved_G4P0DLPOS;
    HI_U32         Reserved_G4P0RESO;
    HI_U32         Reserved_G4P1ADDR;
    HI_U32         Reserved_G4P1STRIDE;
    HI_U32         Reserved_G4P1DFPOS;
    HI_U32         Reserved_G4P1DLPOS;
    HI_U32         Reserved_G4P1RESO;
    HI_U32         Reserved_G4P2ADDR;
    HI_U32         Reserved_G4P2STRIDE;
    HI_U32         Reserved_G4P2DFPOS;
    HI_U32         Reserved_G4P2DLPOS;
    HI_U32         Reserved_G4P2RESO;
    HI_U32         Reserved_G4P3ADDR;
    HI_U32         Reserved_G4P3STRIDE;
    HI_U32         Reserved_G4P3DFPOS;
    HI_U32         Reserved_G4P3DLPOS;
    HI_U32         Reserved_G4P3RESO;
    HI_U32         Reserved_G4P4ADDR;
    HI_U32         Reserved_G4P4STRIDE;
    HI_U32         Reserved_G4P4DFPOS;
    HI_U32         Reserved_G4P4DLPOS;
    HI_U32         Reserved_G4P4RESO;
    HI_U32         Reserved_G4P5ADDR;
    HI_U32         Reserved_G4P5STRIDE;
    HI_U32         Reserved_G4P5DFPOS;
    HI_U32         Reserved_G4P5DLPOS;
    HI_U32         Reserved_G4P5RESO;
    HI_U32         Reserved_G4P6ADDR;
    HI_U32         Reserved_G4P6STRIDE;
    HI_U32         Reserved_G4P6DFPOS;
    HI_U32         Reserved_G4P6DLPOS;
    HI_U32         Reserved_G4P6RESO;
    HI_U32         Reserved_G4P7ADDR;
    HI_U32         Reserved_G4P7STRIDE;
    HI_U32         Reserved_G4P7DFPOS;
    HI_U32         Reserved_G4P7DLPOS;
    HI_U32         Reserved_G4P7RESO;
    HI_U32         Reserved_G48SLICEEN;
    HI_U32         Reserved_ed_455[3];
    HI_U32         Reserved_G4DCMPMODE;
    HI_U32         Reserved_G4DCMPBANKWIDTH;
    HI_U32         Reserved_G4DCMPSTRIDE;
    HI_U32         Reserved_G4DCMPAADDR;
    HI_U32         Reserved_G4DCMPRADDR;
    HI_U32         Reserved_G4DCMPGADDR;
    HI_U32         Reserved_G4DCMPBADDR;
    HI_U32         Reserved_456[13];
    U_HCCTRL             HCCTRL;
    U_HCUPD              HCUPD;
    U_HCADDR             HCADDR;
    U_HCSTRIDE           HCSTRIDE;
    U_HCCBMPARA          HCCBMPARA;
    U_HCCKEYMAX          HCCKEYMAX;
    U_HCCKEYMIN          HCCKEYMIN;
    U_HCCMASK            HCCMASK;
    U_HCIRESO            HCIRESO;
    HI_U32         Reserved_457;
    HI_U32         Reserved_HCSFPOS;
    U_HCDFPOS            HCDFPOS;
    U_HCDLPOS            HCDLPOS;
    HI_U32         Reserved_458[27];
    U_HCCSCIDC           HCCSCIDC;
    U_HCCSCODC           HCCSCODC;
    U_HCCSCP0            HCCSCP0;
    U_HCCSCP1            HCCSCP1;
    U_HCCSCP2            HCCSCP2;
    U_HCCSCP3            HCCSCP3;
    U_HCCSCP4            HCCSCP4;       /* 0x9AB8 */
    #if 0
    HI_U32         Reserved_459[81];
    HI_U32         Reserved_HC1CTRL;
    HI_U32         Reserved_HC1UPD;
    HI_U32         Reserved_HC1ADDR;
    HI_U32         Reserved_HC1STRIDE;
    HI_U32         Reserved_HC1CBMPARA;
    HI_U32         Reserved_HC1CKEYMAX;
    HI_U32         Reserved_HC1CKEYMIN;
    HI_U32         Reserved_HC1CMASK;
    HI_U32         Reserved_HC1IRESO;
    HI_U32         Reserved_ed_460;
    HI_U32         Reserved_HC1SFPOS;
    HI_U32         Reserved_HC1DFPOS;
    HI_U32         Reserved_HC1DLPOS;
    HI_U32         Reserved_ed_461[27];
    HI_U32         Reserved_HC1CSCIDC;
    HI_U32         Reserved_HC1CSCODC;
    HI_U32         Reserved_HC1CSCP0;
    HI_U32         Reserved_HC1CSCP1;
    HI_U32         Reserved_HC1CSCP2;
    HI_U32         Reserved_HC1CSCP3;
    HI_U32         Reserved_HC1CSCP4;
    HI_U32         Reserved_462[81];
    #else 
    HI_U32         Reserved_462[209];
    #endif
    
    U_CBMBKG1            CBMBKG1;     /* 0x9E00 */
    HI_U32         Reserved_CBMBKG2;
    U_CBMBKG3            CBMBKG3;
    U_CBMBKG4            CBMBKG4;
    U_CBMATTR            CBMATTR;
    U_CBMMIX1            CBMMIX1;
    HI_U32         Reserved_CBMMIX2;                   //9E18
    U_CBMMIX3            CBMMIX3;                   //9E1C
    U_CBMMIX4            CBMMIX4;                   //9E20
    HI_U32         Reserved_463[119];
    U_DHDCTRL            DHDCTRL;           /* 0xA000 */
    U_DHDVSYNC           DHDVSYNC;
    U_DHDHSYNC1          DHDHSYNC1;
    U_DHDHSYNC2          DHDHSYNC2;
    U_DHDVPLUS           DHDVPLUS;
    U_DHDPWR             DHDPWR;
    HI_U32         Reserved_DHDVTTHD3;
    U_DHDVTTHD           DHDVTTHD;      /* 0xA01C */
    HI_U32         Reserved_DHDCSCIDC;
    HI_U32         Reserved_DHDCSCODC;
    HI_U32         Reserved_DHDCSCP0;
    HI_U32         Reserved_DHDCSCP1;
    HI_U32         Reserved_DHDCSCP2;
    HI_U32         Reserved_DHDCSCP3;
    HI_U32         Reserved_DHDCSCP4;
    HI_U32         Reserved_464;
    U_DHDCLIPL           DHDCLIPL;      /* 0xA040 */
    U_DHDCLIPH           DHDCLIPH;
    HI_U32         Reserved_465[6];
    U_DHDVGACLIPL  DHDVGACLIPL;         /* 0xA060 */
    U_DHDVGACLIPH  DHDVGACLIPH;
    HI_U32         Reserved_466[6];
    U_DHDHSPCFG0   DHDHSPCFG0;
    U_DHDHSPCFG1   DHDHSPCFG1;
    HI_U32         Reserved_DHDHSPCFG2;
    HI_U32         Reserved_DHDHSPCFG3;
    HI_U32         Reserved_DHDHSPCFG4;
    U_DHDHSPCFG5   DHDHSPCFG5;
    U_DHDHSPCFG6   DHDHSPCFG6;
    U_DHDHSPCFG7   DHDHSPCFG7;
    U_DHDHSPCFG8   DHDHSPCFG8;
    HI_U32         Reserved_DHDHSPCFG9;
    HI_U32         Reserved_DHDHSPCFG10;
    HI_U32         Reserved_DHDHSPCFG11;
    U_DHDHSPCFG12  DHDHSPCFG12;
    U_DHDHSPCFG13  DHDHSPCFG13;
    U_DHDHSPCFG14  DHDHSPCFG14;
    U_DHDHSPCFG15  DHDHSPCFG15;       /* 0xA0BC */
    HI_U32         Reserved_ed_467[12];
    U_DHDSTATE               DHDSTATE;      /* 0xA0F0 */
    HI_U32         Reserved_ed_468[3];
    HI_U32         Reserved_DHDCCDP0FPOS;
    HI_U32         Reserved_DHDCCDP0LPOS;
    HI_U32         Reserved_DHDCCDP1FPOS;
    HI_U32         Reserved_DHDCCDP1LPOS;
    HI_U32         Reserved_DHDCCDP2FPOS;
    HI_U32         Reserved_DHDCCDP2LPOS;
    HI_U32         Reserved_DHDCCDP3FPOS;
    HI_U32         Reserved_DHDCCDP3LPOS;
    HI_U32         Reserved_DHDCCDP4FPOS;
    HI_U32         Reserved_DHDCCDP4LPOS;
    HI_U32         Reserved_DHDCCDP5FPOS;
    HI_U32         Reserved_DHDCCDP5LPOS;
    HI_U32         Reserved_DHDCCDP6FPOS;
    HI_U32         Reserved_DHDCCDP6LPOS;
    HI_U32         Reserved_DHDCCDP7FPOS;
    HI_U32         Reserved_DHDCCDP7LPOS;
    HI_U32         Reserved_DHDCCDP8FPOS;
    HI_U32         Reserved_DHDCCDP8LPOS;
    HI_U32         Reserved_DHDCCDP9FPOS;
    HI_U32         Reserved_DHDCCDP9LPOS;
    HI_U32         Reserved_DHDCCDP10FPOS;
    HI_U32         Reserved_DHDCCDP10LPOS;
    HI_U32         Reserved_DHDCCDP11FPOS;
    HI_U32         Reserved_DHDCCDP11LPOS;
    HI_U32         Reserved_DHDCCDP12FPOS;
    HI_U32         Reserved_DHDCCDP12LPOS;
    HI_U32         Reserved_DHDCCDP13FPOS;
    HI_U32         Reserved_DHDCCDP13LPOS;
    HI_U32         Reserved_DHDCCDP14FPOS;
    HI_U32         Reserved_DHDCCDP14LPOS;
    HI_U32         Reserved_DHDCCDP15FPOS;
    HI_U32         Reserved_DHDCCDP15LPOS;
    HI_U32         Reserved_DHDCCDP16FPOS;
    HI_U32         Reserved_DHDCCDP16LPOS;
    HI_U32         Reserved_DHDCCDP17FPOS;
    HI_U32         Reserved_DHDCCDP17LPOS;
    HI_U32         Reserved_DHDCCDP18FPOS;
    HI_U32         Reserved_DHDCCDP18LPOS;
    HI_U32         Reserved_DHDCCDP19FPOS;
    HI_U32         Reserved_DHDCCDP19LPOS;
    HI_U32         Reserved_DHDCCDP20FPOS;
    HI_U32         Reserved_DHDCCDP20LPOS;
    HI_U32         Reserved_DHDCCDP21FPOS;
    HI_U32         Reserved_DHDCCDP21LPOS;
    HI_U32         Reserved_DHDCCDP22FPOS;
    HI_U32         Reserved_DHDCCDP22LPOS;
    HI_U32         Reserved_DHDCCDP23FPOS;
    HI_U32         Reserved_DHDCCDP23LPOS;
    HI_U32         Reserved_DHDCCDP24FPOS;
    HI_U32         Reserved_DHDCCDP24LPOS;
    HI_U32         Reserved_DHDCCDP25FPOS;
    HI_U32         Reserved_DHDCCDP25LPOS;
    HI_U32         Reserved_DHDCCDP26FPOS;
    HI_U32         Reserved_DHDCCDP26LPOS;
    HI_U32         Reserved_DHDCCDP27FPOS;
    HI_U32         Reserved_DHDCCDP27LPOS;
    HI_U32         Reserved_DHDCCDP28FPOS;
    HI_U32         Reserved_DHDCCDP28LPOS;
    HI_U32         Reserved_DHDCCDP29FPOS;
    HI_U32         Reserved_DHDCCDP29LPOS;
    HI_U32         Reserved_DHDCCDP30FPOS;
    HI_U32         Reserved_DHDCCDP30LPOS;
    HI_U32         Reserved_DHDCCDP31FPOS;
    HI_U32         Reserved_DHDCCDP31LPOS;
    HI_U32         Reserved_DHDCCDP32FPOS;
    HI_U32         Reserved_DHDCCDP32LPOS;
    HI_U32         Reserved_DHDCCDP33FPOS;
    HI_U32         Reserved_DHDCCDP33LPOS;
    HI_U32         Reserved_DHDCCDP34FPOS;
    HI_U32         Reserved_DHDCCDP34LPOS;
    HI_U32         Reserved_DHDCCDP35FPOS;
    HI_U32         Reserved_DHDCCDP35LPOS;
    HI_U32         Reserved_DHDCCDP36FPOS;
    HI_U32         Reserved_DHDCCDP36LPOS;
    HI_U32         Reserved_DHDCCDP37FPOS;
    HI_U32         Reserved_DHDCCDP37LPOS;
    HI_U32         Reserved_DHDCCDP38FPOS;
    HI_U32         Reserved_DHDCCDP38LPOS;
    HI_U32         Reserved_DHDCCDP39FPOS;
    HI_U32         Reserved_DHDCCDP39LPOS;
    HI_U32         Reserved_DHDCCDP40FPOS;
    HI_U32         Reserved_DHDCCDP40LPOS;
    HI_U32         Reserved_DHDCCDP41FPOS;
    HI_U32         Reserved_DHDCCDP41LPOS;
    HI_U32         Reserved_DHDCCDP42FPOS;
    HI_U32         Reserved_DHDCCDP42LPOS;
    HI_U32         Reserved_DHDCCDP43FPOS;
    HI_U32         Reserved_DHDCCDP43LPOS;
    HI_U32         Reserved_DHDCCDP44FPOS;
    HI_U32         Reserved_DHDCCDP44LPOS;
    HI_U32         Reserved_DHDCCDP45FPOS;
    HI_U32         Reserved_DHDCCDP45LPOS;
    HI_U32         Reserved_DHDCCDP46FPOS;
    HI_U32         Reserved_DHDCCDP46LPOS;
    HI_U32         Reserved_DHDCCDP47FPOS;
    HI_U32         Reserved_DHDCCDP47LPOS;
    HI_U32         Reserved_DHDCCDP48FPOS;
    HI_U32         Reserved_DHDCCDP48LPOS;
    HI_U32         Reserved_DHDCCDP49FPOS;
    HI_U32         Reserved_DHDCCDP49LPOS;
    HI_U32         Reserved_DHDCCDP50FPOS;
    HI_U32         Reserved_DHDCCDP50LPOS;
    HI_U32         Reserved_DHDCCDP51FPOS;
    HI_U32         Reserved_DHDCCDP51LPOS;
    HI_U32         Reserved_DHDCCDP52FPOS;
    HI_U32         Reserved_DHDCCDP52LPOS;
    HI_U32         Reserved_DHDCCDP53FPOS;
    HI_U32         Reserved_DHDCCDP53LPOS;
    HI_U32         Reserved_DHDCCDP54FPOS;
    HI_U32         Reserved_DHDCCDP54LPOS;
    HI_U32         Reserved_DHDCCDP55FPOS;
    HI_U32         Reserved_DHDCCDP55LPOS;
    HI_U32         Reserved_DHDCCDP56FPOS;
    HI_U32         Reserved_DHDCCDP56LPOS;
    HI_U32         Reserved_DHDCCDP57FPOS;
    HI_U32         Reserved_DHDCCDP57LPOS;
    HI_U32         Reserved_DHDCCDP58FPOS;
    HI_U32         Reserved_DHDCCDP58LPOS;
    HI_U32         Reserved_DHDCCDP59FPOS;
    HI_U32         Reserved_DHDCCDP59LPOS;
    HI_U32         Reserved_DHDCCDP60FPOS;
    HI_U32         Reserved_DHDCCDP60LPOS;
    HI_U32         Reserved_DHDCCDP61FPOS;
    HI_U32         Reserved_DHDCCDP61LPOS;
    HI_U32         Reserved_DHDCCDP62FPOS;
    HI_U32         Reserved_DHDCCDP62LPOS;
    HI_U32         Reserved_DHDCCDP63FPOS;
    HI_U32         Reserved_DHDCCDP63LPOS;
    HI_U32         Reserved_469[64];
    HI_U32         Reserved_DHD1CTRL;
    HI_U32         Reserved_DHD1VSYNC;
    HI_U32         Reserved_DHD1HSYNC1;
    HI_U32         Reserved_DHD1HSYNC2;
    HI_U32         Reserved_DHD1VPLUS;
    HI_U32         Reserved_DHD1PWR;
    HI_U32         Reserved_DHD1VTTHD3;
    HI_U32         Reserved_DHD1VTTHD;
    HI_U32         Reserved_DHD1CSCIDC;
    HI_U32         Reserved_DHD1CSCODC;
    HI_U32         Reserved_DHD1CSCP0;
    HI_U32         Reserved_DHD1CSCP1;
    HI_U32         Reserved_DHD1CSCP2;
    HI_U32         Reserved_DHD1CSCP3;
    HI_U32         Reserved_DHD1CSCP4;
    HI_U32         Reserved_ed_470;
    HI_U32         Reserved_DHD1CLIPL;
    HI_U32         Reserved_DHD1CLIPH;
    HI_U32         Reserved_471[14];
    HI_U32         Reserved_DHD1CCDOIMGMOD;
    HI_U32         Reserved_DHD1CCDOPOSMSKH;
    HI_U32         Reserved_DHD1CCDOPOSMSKL;
    HI_U32         Reserved_ed_472;
    HI_U32         Reserved_DHD1CCDIIMGMOD;
    HI_U32         Reserved_DHD1CCDIPOSMSKH;
    HI_U32         Reserved_DHD1CCDIPOSMSKL;
    HI_U32         Reserved_ed_473[21];
    HI_U32         Reserved_DHD1STATE;
    HI_U32         Reserved_ed_474[3];
    HI_U32         Reserved_DHD1CCDP0FPOS;
    HI_U32         Reserved_DHD1CCDP0LPOS;
    HI_U32         Reserved_DHD1CCDP1FPOS;
    HI_U32         Reserved_DHD1CCDP1LPOS;
    HI_U32         Reserved_DHD1CCDP2FPOS;
    HI_U32         Reserved_DHD1CCDP2LPOS;
    HI_U32         Reserved_DHD1CCDP3FPOS;
    HI_U32         Reserved_DHD1CCDP3LPOS;
    HI_U32         Reserved_DHD1CCDP4FPOS;
    HI_U32         Reserved_DHD1CCDP4LPOS;
    HI_U32         Reserved_DHD1CCDP5FPOS;
    HI_U32         Reserved_DHD1CCDP5LPOS;
    HI_U32         Reserved_DHD1CCDP6FPOS;
    HI_U32         Reserved_DHD1CCDP6LPOS;
    HI_U32         Reserved_DHD1CCDP7FPOS;
    HI_U32         Reserved_DHD1CCDP7LPOS;
    HI_U32         Reserved_DHD1CCDP8FPOS;
    HI_U32         Reserved_DHD1CCDP8LPOS;
    HI_U32         Reserved_DHD1CCDP9FPOS;
    HI_U32         Reserved_DHD1CCDP9LPOS;
    HI_U32         Reserved_DHD1CCDP10FPOS;
    HI_U32         Reserved_DHD1CCDP10LPOS;
    HI_U32         Reserved_DHD1CCDP11FPOS;
    HI_U32         Reserved_DHD1CCDP11LPOS;
    HI_U32         Reserved_DHD1CCDP12FPOS;
    HI_U32         Reserved_DHD1CCDP12LPOS;
    HI_U32         Reserved_DHD1CCDP13FPOS;
    HI_U32         Reserved_DHD1CCDP13LPOS;
    HI_U32         Reserved_DHD1CCDP14FPOS;
    HI_U32         Reserved_DHD1CCDP14LPOS;
    HI_U32         Reserved_DHD1CCDP15FPOS;
    HI_U32         Reserved_DHD1CCDP15LPOS;
    HI_U32         Reserved_DHD1CCDP16FPOS;
    HI_U32         Reserved_DHD1CCDP16LPOS;
    HI_U32         Reserved_DHD1CCDP17FPOS;
    HI_U32         Reserved_DHD1CCDP17LPOS;
    HI_U32         Reserved_DHD1CCDP18FPOS;
    HI_U32         Reserved_DHD1CCDP18LPOS;
    HI_U32         Reserved_DHD1CCDP19FPOS;
    HI_U32         Reserved_DHD1CCDP19LPOS;
    HI_U32         Reserved_DHD1CCDP20FPOS;
    HI_U32         Reserved_DHD1CCDP20LPOS;
    HI_U32         Reserved_DHD1CCDP21FPOS;
    HI_U32         Reserved_DHD1CCDP21LPOS;
    HI_U32         Reserved_DHD1CCDP22FPOS;
    HI_U32         Reserved_DHD1CCDP22LPOS;
    HI_U32         Reserved_DHD1CCDP23FPOS;
    HI_U32         Reserved_DHD1CCDP23LPOS;
    HI_U32         Reserved_DHD1CCDP24FPOS;
    HI_U32         Reserved_DHD1CCDP24LPOS;
    HI_U32         Reserved_DHD1CCDP25FPOS;
    HI_U32         Reserved_DHD1CCDP25LPOS;
    HI_U32         Reserved_DHD1CCDP26FPOS;
    HI_U32         Reserved_DHD1CCDP26LPOS;
    HI_U32         Reserved_DHD1CCDP27FPOS;
    HI_U32         Reserved_DHD1CCDP27LPOS;
    HI_U32         Reserved_DHD1CCDP28FPOS;
    HI_U32         Reserved_DHD1CCDP28LPOS;
    HI_U32         Reserved_DHD1CCDP29FPOS;
    HI_U32         Reserved_DHD1CCDP29LPOS;
    HI_U32         Reserved_DHD1CCDP30FPOS;
    HI_U32         Reserved_DHD1CCDP30LPOS;
    HI_U32         Reserved_DHD1CCDP31FPOS;
    HI_U32         Reserved_DHD1CCDP31LPOS;
    HI_U32         Reserved_DHD1CCDP32FPOS;
    HI_U32         Reserved_DHD1CCDP32LPOS;
    HI_U32         Reserved_DHD1CCDP33FPOS;
    HI_U32         Reserved_DHD1CCDP33LPOS;
    HI_U32         Reserved_DHD1CCDP34FPOS;
    HI_U32         Reserved_DHD1CCDP34LPOS;
    HI_U32         Reserved_DHD1CCDP35FPOS;
    HI_U32         Reserved_DHD1CCDP35LPOS;
    HI_U32         Reserved_DHD1CCDP36FPOS;
    HI_U32         Reserved_DHD1CCDP36LPOS;
    HI_U32         Reserved_DHD1CCDP37FPOS;
    HI_U32         Reserved_DHD1CCDP37LPOS;
    HI_U32         Reserved_DHD1CCDP38FPOS;
    HI_U32         Reserved_DHD1CCDP38LPOS;
    HI_U32         Reserved_DHD1CCDP39FPOS;
    HI_U32         Reserved_DHD1CCDP39LPOS;
    HI_U32         Reserved_DHD1CCDP40FPOS;
    HI_U32         Reserved_DHD1CCDP40LPOS;
    HI_U32         Reserved_DHD1CCDP41FPOS;
    HI_U32         Reserved_DHD1CCDP41LPOS;
    HI_U32         Reserved_DHD1CCDP42FPOS;
    HI_U32         Reserved_DHD1CCDP42LPOS;
    HI_U32         Reserved_DHD1CCDP43FPOS;
    HI_U32         Reserved_DHD1CCDP43LPOS;
    HI_U32         Reserved_DHD1CCDP44FPOS;
    HI_U32         Reserved_DHD1CCDP44LPOS;
    HI_U32         Reserved_DHD1CCDP45FPOS;
    HI_U32         Reserved_DHD1CCDP45LPOS;
    HI_U32         Reserved_DHD1CCDP46FPOS;
    HI_U32         Reserved_DHD1CCDP46LPOS;
    HI_U32         Reserved_DHD1CCDP47FPOS;
    HI_U32         Reserved_DHD1CCDP47LPOS;
    HI_U32         Reserved_DHD1CCDP48FPOS;
    HI_U32         Reserved_DHD1CCDP48LPOS;
    HI_U32         Reserved_DHD1CCDP49FPOS;
    HI_U32         Reserved_DHD1CCDP49LPOS;
    HI_U32         Reserved_DHD1CCDP50FPOS;
    HI_U32         Reserved_DHD1CCDP50LPOS;
    HI_U32         Reserved_DHD1CCDP51FPOS;
    HI_U32         Reserved_DHD1CCDP51LPOS;
    HI_U32         Reserved_DHD1CCDP52FPOS;
    HI_U32         Reserved_DHD1CCDP52LPOS;
    HI_U32         Reserved_DHD1CCDP53FPOS;
    HI_U32         Reserved_DHD1CCDP53LPOS;
    HI_U32         Reserved_DHD1CCDP54FPOS;
    HI_U32         Reserved_DHD1CCDP54LPOS;
    HI_U32         Reserved_DHD1CCDP55FPOS;
    HI_U32         Reserved_DHD1CCDP55LPOS;
    HI_U32         Reserved_DHD1CCDP56FPOS;
    HI_U32         Reserved_DHD1CCDP56LPOS;
    HI_U32         Reserved_DHD1CCDP57FPOS;
    HI_U32         Reserved_DHD1CCDP57LPOS;
    HI_U32         Reserved_DHD1CCDP58FPOS;
    HI_U32         Reserved_DHD1CCDP58LPOS;
    HI_U32         Reserved_DHD1CCDP59FPOS;
    HI_U32         Reserved_DHD1CCDP59LPOS;
    HI_U32         Reserved_DHD1CCDP60FPOS;
    HI_U32         Reserved_DHD1CCDP60LPOS;
    HI_U32         Reserved_DHD1CCDP61FPOS;
    HI_U32         Reserved_DHD1CCDP61LPOS;
    HI_U32         Reserved_DHD1CCDP62FPOS;
    HI_U32         Reserved_DHD1CCDP62LPOS;
    HI_U32         Reserved_DHD1CCDP63FPOS;
    HI_U32         Reserved_DHD1CCDP63LPOS;
    HI_U32         Reserved_475[64];
    U_DSDCTRL            DSDCTRL;       /* 0xA800 */
    U_DSDVSYNC           DSDVSYNC;
    U_DSDHSYNC1          DSDHSYNC1;
    U_DSDHSYNC2          DSDHSYNC2;
    U_DSDVPLUS           DSDVPLUS;
    U_DSDPWR             DSDPWR;
    HI_U32         Reserved_DSDVTTHD3;
    U_DSDVTTHD           DSDVTTHD;
    HI_U32         Reserved_DSDCSCIDC;
    HI_U32         Reserved_DSDCSCODC;
    HI_U32         Reserved_DSDCSCP0;
    HI_U32         Reserved_DSDCSCP1;
    HI_U32         Reserved_DSDCSCP2;
    HI_U32         Reserved_DSDCSCP3;
    HI_U32         Reserved_DSDCSCP4;
    HI_U32         Reserved_476;
    U_DSDCLIPL           DSDCLIPL;
    U_DSDCLIPH           DSDCLIPH;      //0xA844
    HI_U32         Reserved_477[42];
    U_DSDSTATE           DSDSTATE;      //0xA8F0
    HI_U32         Reserved_478[195];
    U_DSD1CTRL           DSD1CTRL;      //0xAC00
    U_DSD1VSYNC          DSD1VSYNC;
    U_DSD1HSYNC1         DSD1HSYNC1;
    U_DSD1HSYNC2         DSD1HSYNC2;
    U_DSD1VPLUS          DSD1VPLUS;
    U_DSD1PWR            DSD1PWR;
    HI_U32         Reserved_DSD1VTTHD3;
    U_DSD1VTTHD          DSD1VTTHD;     //0xAC1C
    HI_U32         Reserved_DSD1CSCIDC;
    HI_U32         Reserved_DSD1CSCODC;
    HI_U32         Reserved_DSD1CSCP0;
    HI_U32         Reserved_DSD1CSCP1;
    HI_U32         Reserved_DSD1CSCP2;
    HI_U32         Reserved_DSD1CSCP3;
    HI_U32         Reserved_DSD1CSCP4;
    HI_U32         Reserved_479;
    U_DSD1CLIPL          DSD1CLIPL;     //0xAC40
    U_DSD1CLIPH          DSD1CLIPH;
    HI_U32         Reserved_480[42];
    U_DSD1STATE          DSD1STATE;     //0xACF0
    HI_U32         Reserved_481[195];
    HI_U32         Reserved_DSD2CTRL;
    HI_U32         Reserved_DSD2VSYNC;
    HI_U32         Reserved_DSD2HSYNC1;
    HI_U32         Reserved_DSD2HSYNC2;
    HI_U32         Reserved_DSD2VPLUS;
    HI_U32         Reserved_DSD2PWR;
    HI_U32         Reserved_DSD2VTTHD3;
    HI_U32         Reserved_DSD2VTTHD;
    HI_U32         Reserved_DSD2CSCIDC;
    HI_U32         Reserved_DSD2CSCODC;
    HI_U32         Reserved_DSD2CSCP0;
    HI_U32         Reserved_DSD2CSCP1;
    HI_U32         Reserved_DSD2CSCP2;
    HI_U32         Reserved_DSD2CSCP3;
    HI_U32         Reserved_DSD2CSCP4;
    HI_U32         Reserved_482;
    HI_U32         Reserved_DSD2CLIPL;
    HI_U32         Reserved_DSD2CLIPH;
    HI_U32         Reserved_483[42];
    HI_U32         Reserved_DSD2STATE;
    HI_U32         Reserved_ed_484[195];
    HI_U32         Reserved_DSD3CTRL;
    HI_U32         Reserved_DSD3VSYNC;
    HI_U32         Reserved_DSD3HSYNC1;
    HI_U32         Reserved_DSD3HSYNC2;
    HI_U32         Reserved_DSD3VPLUS;
    HI_U32         Reserved_DSD3PWR;
    HI_U32         Reserved_DSD3VTTHD3;
    HI_U32         Reserved_DSD3VTTHD;
    HI_U32         Reserved_DSD3CSCIDC;
    HI_U32         Reserved_DSD3CSCODC;
    HI_U32         Reserved_DSD3CSCP0;
    HI_U32         Reserved_DSD3CSCP1;
    HI_U32         Reserved_DSD3CSCP2;
    HI_U32         Reserved_DSD3CSCP3;
    HI_U32         Reserved_DSD3CSCP4;
    HI_U32         Reserved_ed_485;
    HI_U32         Reserved_DSD3CLIPL;
    HI_U32         Reserved_DSD3CLIPH;
    HI_U32         Reserved_ed_486[42];
    HI_U32         Reserved_DSD3STATE;
    HI_U32         Reserved_487[195];
    HI_U32         Reserved_DSD4CTRL;
    HI_U32         Reserved_DSD4VSYNC;
    HI_U32         Reserved_DSD4HSYNC1;
    HI_U32         Reserved_DSD4HSYNC2;
    HI_U32         Reserved_DSD4VPLUS;
    HI_U32         Reserved_DSD4PWR;
    HI_U32         Reserved_DSD4VTTHD3;
    HI_U32         Reserved_DSD4VTTHD;
    HI_U32         Reserved_DSD4CSCIDC;
    HI_U32         Reserved_DSD4CSCODC;
    HI_U32         Reserved_DSD4CSCP0;
    HI_U32         Reserved_DSD4CSCP1;
    HI_U32         Reserved_DSD4CSCP2;
    HI_U32         Reserved_DSD4CSCP3;
    HI_U32         Reserved_DSD4CSCP4;
    HI_U32         Reserved_ed_488;
    HI_U32         Reserved_DSD4CLIPL;
    HI_U32         Reserved_DSD4CLIPH;
    HI_U32         Reserved_ed_489[42];
    HI_U32         Reserved_DSD4STATE;
    HI_U32         Reserved_490[195];
    HI_U32         Reserved_DSD5CTRL;
    HI_U32         Reserved_DSD5VSYNC;
    HI_U32         Reserved_DSD5HSYNC1;
    HI_U32         Reserved_DSD5HSYNC2;
    HI_U32         Reserved_DSD5VPLUS;
    HI_U32         Reserved_DSD5PWR;
    HI_U32         Reserved_DSD5VTTHD3;
    HI_U32         Reserved_DSD5VTTHD;
    HI_U32         Reserved_DSD5CSCIDC;
    HI_U32         Reserved_DSD5CSCODC;
    HI_U32         Reserved_DSD5CSCP0;
    HI_U32         Reserved_DSD5CSCP1;
    HI_U32         Reserved_DSD5CSCP2;
    HI_U32         Reserved_DSD5CSCP3;
    HI_U32         Reserved_DSD5CSCP4;
    HI_U32         Reserved_ed_491;
    HI_U32         Reserved_DSD5CLIPL;
    HI_U32         Reserved_DSD5CLIPH;
    HI_U32         Reserved_ed_492[42];
    HI_U32         Reserved_DSD5STATE;
    HI_U32         Reserved_493[195];
    HI_U32         Reserved_HDATE_VERSION;
    HI_U32         Reserved_HDATE_EN;
    HI_U32         Reserved_HDATE_POLA_CTRL;
    HI_U32         Reserved_HDATE_VIDEO_FORMAT;
    HI_U32         Reserved_HDATE_STATE;
    HI_U32         Reserved_HDATE_OUT_CTRL;
    HI_U32         Reserved_HDATE_SRC_13_COEF1;
    HI_U32         Reserved_HDATE_SRC_13_COEF2;
    HI_U32         Reserved_HDATE_SRC_13_COEF3;
    HI_U32         Reserved_HDATE_SRC_13_COEF4;
    HI_U32         Reserved_HDATE_SRC_13_COEF5;
    HI_U32         Reserved_HDATE_SRC_13_COEF6;
    HI_U32         Reserved_HDATE_SRC_13_COEF7;
    HI_U32         Reserved_HDATE_SRC_13_COEF8;
    HI_U32         Reserved_HDATE_SRC_13_COEF9;
    HI_U32         Reserved_HDATE_SRC_13_COEF10;
    HI_U32         Reserved_HDATE_SRC_13_COEF11;
    HI_U32         Reserved_HDATE_SRC_13_COEF12;
    HI_U32         Reserved_HDATE_SRC_13_COEF13;
    HI_U32         Reserved_HDATE_SRC_24_COEF1;
    HI_U32         Reserved_HDATE_SRC_24_COEF2;
    HI_U32         Reserved_HDATE_SRC_24_COEF3;
    HI_U32         Reserved_HDATE_SRC_24_COEF4;
    HI_U32         Reserved_HDATE_SRC_24_COEF5;
    HI_U32         Reserved_HDATE_SRC_24_COEF6;
    HI_U32         Reserved_HDATE_SRC_24_COEF7;
    HI_U32         Reserved_HDATE_SRC_24_COEF8;
    HI_U32         Reserved_HDATE_SRC_24_COEF9;
    HI_U32         Reserved_HDATE_SRC_24_COEF10;
    HI_U32         Reserved_HDATE_SRC_24_COEF11;
    HI_U32         Reserved_HDATE_SRC_24_COEF12;
    HI_U32         Reserved_HDATE_SRC_24_COEF13;
    HI_U32         Reserved_HDATE_CSC_COEF1;
    HI_U32         Reserved_HDATE_CSC_COEF2;
    HI_U32         Reserved_HDATE_CSC_COEF3;
    HI_U32         Reserved_HDATE_CSC_COEF4;
    HI_U32         Reserved_HDATE_CSC_COEF5;
    HI_U32         Reserved_ed_494[3];
    HI_U32         Reserved_HDATE_TEST;
    HI_U32         Reserved_HDATE_VBI_CTRL;
    HI_U32         Reserved_HDATE_CGMSA_DATA;
    HI_U32         Reserved_HDATE_CGMSB_H;
    HI_U32         Reserved_HDATE_CGMSB_DATA1;
    HI_U32         Reserved_HDATE_CGMSB_DATA2;
    HI_U32         Reserved_HDATE_CGMSB_DATA3;
    HI_U32         Reserved_HDATE_CGMSB_DATA4;
    HI_U32         Reserved_495[80];
    U_DATE_COEFF0        DATE_COEFF0;   //0xC200
    U_DATE_COEFF1        DATE_COEFF1;
    HI_U32         Reserved_496[5];
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
    U_DATE_COEFF23       DATE_COEFF23;  //0xC25C
    HI_U32         Reserved_497[8];
    U_DATE_ISRMASK       DATE_ISRMASK;  //0xC280
    U_DATE_ISRSTATE      DATE_ISRSTATE;
    U_DATE_ISR           DATE_ISR;
    HI_U32         Reserved_498;
    U_DATE_VERSION       DATE_VERSION;  //0xC290
    HI_U32         Reserved_499[91];
    U_DATE1_COEFF0       DATE1_COEFF0;  //0xC400
    U_DATE1_COEFF1       DATE1_COEFF1;
    HI_U32         Reserved_500[5];
    U_DATE1_COEFF7       DATE1_COEFF7;
    U_DATE1_COEFF8       DATE1_COEFF8;
    U_DATE1_COEFF9       DATE1_COEFF9;
    U_DATE1_COEFF10      DATE1_COEFF10;
    U_DATE1_COEFF11      DATE1_COEFF11;
    U_DATE1_COEFF12      DATE1_COEFF12;
    U_DATE1_COEFF13      DATE1_COEFF13;
    U_DATE1_COEFF14      DATE1_COEFF14;
    U_DATE1_COEFF15      DATE1_COEFF15;
    U_DATE1_COEFF16      DATE1_COEFF16;
    U_DATE1_COEFF17      DATE1_COEFF17;
    U_DATE1_COEFF18      DATE1_COEFF18;
    U_DATE1_COEFF19      DATE1_COEFF19;
    U_DATE1_COEFF20      DATE1_COEFF20;
    U_DATE1_COEFF21      DATE1_COEFF21;
    U_DATE1_COEFF22      DATE1_COEFF22;
    U_DATE1_COEFF23      DATE1_COEFF23;
    HI_U32         Reserved_501[8];
    U_DATE1_ISRMASK      DATE1_ISRMASK;
    U_DATE1_ISRSTATE     DATE1_ISRSTATE;
    U_DATE1_ISR          DATE1_ISR;
    HI_U32         Reserved_502;
    U_DATE1_VERSION      DATE1_VERSION; //0xC490
    HI_U32         Reserved_503[91];
    HI_U32         Reserved_DATE2_COEFF0;
    HI_U32         Reserved_DATE2_COEFF1;
    HI_U32         Reserved_504[5];
    HI_U32         Reserved_DATE2_COEFF7;
    HI_U32         Reserved_DATE2_COEFF8;
    HI_U32         Reserved_DATE2_COEFF9;
    HI_U32         Reserved_DATE2_COEFF10;
    HI_U32         Reserved_DATE2_COEFF11;
    HI_U32         Reserved_DATE2_COEFF12;
    HI_U32         Reserved_DATE2_COEFF13;
    HI_U32         Reserved_DATE2_COEFF14;
    HI_U32         Reserved_DATE2_COEFF15;
    HI_U32         Reserved_DATE2_COEFF16;
    HI_U32         Reserved_DATE2_COEFF17;
    HI_U32         Reserved_DATE2_COEFF18;
    HI_U32         Reserved_DATE2_COEFF19;
    HI_U32         Reserved_DATE2_COEFF20;
    HI_U32         Reserved_DATE2_COEFF21;
    HI_U32         Reserved_DATE2_COEFF22;
    HI_U32         Reserved_DATE2_COEFF23;
    HI_U32         Reserved_ed_505[8];
    HI_U32         Reserved_DATE2_ISRMASK;
    HI_U32         Reserved_DATE2_ISRSTATE;
    HI_U32         Reserved_DATE2_ISR;
    HI_U32         Reserved_506;
    HI_U32         Reserved_DATE2_VERSION;
    HI_U32         Reserved_507[91];
    HI_U32         Reserved_DATE3_COEFF0;
    HI_U32         Reserved_DATE3_COEFF1;
    HI_U32         Reserved_ed_508[5];
    HI_U32         Reserved_DATE3_COEFF7;
    HI_U32         Reserved_DATE3_COEFF8;
    HI_U32         Reserved_DATE3_COEFF9;
    HI_U32         Reserved_DATE3_COEFF10;
    HI_U32         Reserved_DATE3_COEFF11;
    HI_U32         Reserved_DATE3_COEFF12;
    HI_U32         Reserved_DATE3_COEFF13;
    HI_U32         Reserved_DATE3_COEFF14;
    HI_U32         Reserved_DATE3_COEFF15;
    HI_U32         Reserved_DATE3_COEFF16;
    HI_U32         Reserved_DATE3_COEFF17;
    HI_U32         Reserved_DATE3_COEFF18;
    HI_U32         Reserved_DATE3_COEFF19;
    HI_U32         Reserved_DATE3_COEFF20;
    HI_U32         Reserved_DATE3_COEFF21;
    HI_U32         Reserved_DATE3_COEFF22;
    HI_U32         Reserved_DATE3_COEFF23;
    HI_U32         Reserved_ed_509[8];
    HI_U32         Reserved_DATE3_ISRMASK;
    HI_U32         Reserved_DATE3_ISRSTATE;
    HI_U32         Reserved_DATE3_ISR;
    HI_U32         Reserved_ed_510;
    HI_U32         Reserved_DATE3_VERSION;
    HI_U32         Reserved_511[91];
    HI_U32         Reserved_DATE4_COEFF0;
    HI_U32         Reserved_DATE4_COEFF1;
    HI_U32         Reserved_ed_512[5];
    HI_U32         Reserved_DATE4_COEFF7;
    HI_U32         Reserved_DATE4_COEFF8;
    HI_U32         Reserved_DATE4_COEFF9;
    HI_U32         Reserved_DATE4_COEFF10;
    HI_U32         Reserved_DATE4_COEFF11;
    HI_U32         Reserved_DATE4_COEFF12;
    HI_U32         Reserved_DATE4_COEFF13;
    HI_U32         Reserved_DATE4_COEFF14;
    HI_U32         Reserved_DATE4_COEFF15;
    HI_U32         Reserved_DATE4_COEFF16;
    HI_U32         Reserved_DATE4_COEFF17;
    HI_U32         Reserved_DATE4_COEFF18;
    HI_U32         Reserved_DATE4_COEFF19;
    HI_U32         Reserved_DATE4_COEFF20;
    HI_U32         Reserved_DATE4_COEFF21;
    HI_U32         Reserved_DATE4_COEFF22;
    HI_U32         Reserved_DATE4_COEFF23;
    HI_U32         Reserved_ed_513[8];
    HI_U32         Reserved_DATE4_ISRMASK;
    HI_U32         Reserved_DATE4_ISRSTATE;
    HI_U32         Reserved_DATE4_ISR;
    HI_U32         Reserved_ed_514;
    HI_U32         Reserved_DATE4_VERSION;
    HI_U32         Reserved_515[91];
    HI_U32         Reserved_DATE5_COEFF0;
    HI_U32         Reserved_DATE5_COEFF1;
    HI_U32         Reserved_ed_516[5];
    HI_U32         Reserved_DATE5_COEFF7;
    HI_U32         Reserved_DATE5_COEFF8;
    HI_U32         Reserved_DATE5_COEFF9;
    HI_U32         Reserved_DATE5_COEFF10;
    HI_U32         Reserved_DATE5_COEFF11;
    HI_U32         Reserved_DATE5_COEFF12;
    HI_U32         Reserved_DATE5_COEFF13;
    HI_U32         Reserved_DATE5_COEFF14;
    HI_U32         Reserved_DATE5_COEFF15;
    HI_U32         Reserved_DATE5_COEFF16;
    HI_U32         Reserved_DATE5_COEFF17;
    HI_U32         Reserved_DATE5_COEFF18;
    HI_U32         Reserved_DATE5_COEFF19;
    HI_U32         Reserved_DATE5_COEFF20;
    HI_U32         Reserved_DATE5_COEFF21;
    HI_U32         Reserved_DATE5_COEFF22;
    HI_U32         Reserved_DATE5_COEFF23;
    HI_U32         Reserved_ed_517[8];
    HI_U32         Reserved_DATE5_ISRMASK;
    HI_U32         Reserved_DATE5_ISRSTATE;
    HI_U32         Reserved_DATE5_ISR;
    HI_U32         Reserved_ed_518;
    HI_U32         Reserved_DATE5_VERSION;
    HI_U32         Reserved_519[36];
    HI_U32         Reserved_VOLNKLSTCTRL;
    HI_U32         Reserved_VOLNKLSTADDR;
    HI_U32         Reserved_VOLNKLSTSTRT;
    HI_U32         Reserved_VOLNKLSTPENDINGSTA;
    HI_U32         Reserved_VOLNKLSTWORKINGSTA;
    HI_U32         Reserved_VOLNKLSTENDSTA;
    HI_U32         Reserved_VOLNKLSTNUMSTA;
    HI_U32         Reserved_520[48];
    U_VOCTRL             VOCTRL;        //0xCE00
    U_VOINTSTA           VOINTSTA;
    U_VOMSKINTSTA        VOMSKINTSTA;
    U_VOINTMSK           VOINTMSK;
    U_VOUVERSION1        VOUVERSION1;
    U_VOUVERSION2        VOUVERSION2;
    U_VOMUXDATA          VOMUXDATA;
    U_VOMUX              VOMUX;
    U_VODEBUG            VODEBUG;
    U_VGACSCIDC          VGACSCIDC;
    U_VGACSCODC          VGACSCODC;
    U_VGACSCP0           VGACSCP0;
    U_VGACSCP1           VGACSCP1;
    U_VGACSCP2           VGACSCP2;
    U_VGACSCP3           VGACSCP3;
    U_VGACSCP4           VGACSCP4;
    U_VOPARAUP           VOPARAUP;
    U_VHDHCOEFAD         VHDHCOEFAD;
    U_VHDVCOEFAD         VHDVCOEFAD;
    HI_U32               Reserved_VADHCOEFAD;
    HI_U32               Reserved_VADVCOEFAD;
    HI_U32               Reserved_ACCAD;   
    U_WBCHCOEFAD         WBCHCOEFAD;
    U_WBCVCOEFAD         WBCVCOEFAD;    //0xCE5C
    HI_U32         Reserved_G0CLUTAD;
    HI_U32         Reserved_G1CLUTAD;
    HI_U32         Reserved_G0HCOEFAD;
    HI_U32         Reserved_G0VCOEFAD;
    HI_U32         Reserved_G1HCOEFAD;
    HI_U32         Reserved_G1VCOEFAD;
    HI_U32         Reserved_DHDGAMMAAD; 
    HI_U32         Reserved_DHD1GAMMAAD;
    HI_U32         Reserved_VODDRSEL;
    HI_U32         Reserved_521[23];
    U_DACCTRL0_2         DACCTRL0_2;    /*no*/
    U_DACCTRL3_5         DACCTRL3_5;
    U_DACSTAT0_2         DACSTAT0_2;
    U_DACSTAT3_5         DACSTAT3_5;
    U_DACBANDGAP         DACBANDGAP;
    U_VOLPTIMER          VOLPTIMER;
    U_VOLPMODE0          VOLPMODE0;
    U_VOLPMODE1          VOLPMODE1;
    HI_U32         Reserved_522[64];
    U_VHDHLCOEF          VHDHLCOEF[68];     /* 0xD000 */
    HI_U32         Reserved_523[60];
    U_VHDHCCOEF          VHDHCCOEF[34];
    HI_U32         Reserved_524[94];
    U_VHDVLCOEF          VHDVLCOEF[68];
    HI_U32         Reserved_525[60];
    U_VHDVCCOEF          VHDVCCOEF[34];
    HI_U32         Reserved_526[94];
    HI_U32         Reserved_VHD1HLCOEF[68];
    HI_U32         Reserved_527[60];
    HI_U32         Reserved_VHD1HCCOEF[34];
    HI_U32         Reserved_528[94];
    HI_U32         Reserved_VHD1VLCOEF[68];
    HI_U32         Reserved_529[60];
    HI_U32         Reserved_VHD1VCCOEF[98];
    HI_U32         Reserved_530[30];
    U_WBCHLCOEF          WBCHLCOEF[68];
    HI_U32         Reserved_531[60];
    U_WBCHCCOEF          WBCHCCOEF[34];
    HI_U32         Reserved_532[94];
    U_WBCVLCOEF          WBCVLCOEF[68];
    HI_U32         Reserved_533[60];
    U_WBCVCCOEF          WBCVCCOEF[34];
    HI_U32         Reserved_534[94];
    U_DHDGAMMAN          DHDGAMMAN[33];
    HI_U32         Reserved_535[31];
    HI_U32         Reserved_DHD1GAMMAN[33];
    HI_U32         Reserved_536[415];
    HI_U32         Reserved_VADHLCOEF[68];
    HI_U32         Reserved_537[60];
    HI_U32         Reserved_VADHCCOEF[34];
    HI_U32         Reserved_538[30];
    HI_U32         Reserved_VADVLCOEF[34];
    HI_U32         Reserved_539[30];
    HI_U32         Reserved_VADVCCOEF[34];
    HI_U32         Reserved_540[30];
    HI_U32         Reserved_VSDHLCOEF[4];
    HI_U32         Reserved_541[60];
    HI_U32         Reserved_VSDHCCOEF[34];
    HI_U32         Reserved_542[30];
    HI_U32         Reserved_VSDVLCOEF[34];
    HI_U32         Reserved_543[30];
    HI_U32         Reserved_VSDVCCOEF[34];
    HI_U32         Reserved_544[30];
    HI_U32         Reserved_G0HLCOEF[32];
    HI_U32         Reserved_545[32];
    HI_U32         Reserved_G0HCCOEF[32];
    HI_U32         Reserved_546[32];
    HI_U32         Reserved_G0VLCOEF[32];
    HI_U32         Reserved_547[32];
    HI_U32         Reserved_G0VCCOEF[32];
    HI_U32         Reserved_548[32];
    HI_U32         Reserved_G1HLCOEF[32];
    HI_U32         Reserved_549[32];
    HI_U32         Reserved_G1HCCOEF[32];
    HI_U32         Reserved_550[32];
    HI_U32         Reserved_G1VLCOEF[32];
    HI_U32         Reserved_G1VCCOEF[32];
} VOU_REGS_S;



#endif // __C_UNION_DEFINE_H__
