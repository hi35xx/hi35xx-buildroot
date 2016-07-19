//******************************************************************************
//  Copyright (C), 2003-2012, Huawei Technologies Co., Ltd.
//
//******************************************************************************
// File name     : jpge_reg(r004).h
// Version       : 2.0
// Author        : XXX
// Created       : 2012-05-16
// Last Modified : 
// Description   :  The C union definition file for XXX
// Function List : 
// History       : 
// 1 Date        : 
// Author        : XXX
// Modification  : Create file
//******************************************************************************

#ifndef __JPGE_REGR004_H__
#define __JPGE_REGR004_H__

// Define the union U_JPGE_INTSTAT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    VencEndOfPic          : 1   ; // [0]
        unsigned int    VencBufFull           : 1   ; // [1]
        unsigned int    JpgeTimeout           : 1   ; // [2]
        unsigned int    cfgErr                : 1   ; // [3]
        unsigned int    Reserved_0            : 28  ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_INTSTAT;

// Define the union U_JPGE_INTMASK
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    VencEndOfPicMask      : 1   ; // [0]
        unsigned int    VencBufFullMask       : 1   ; // [1]
        unsigned int    JpgeTimeoutMask       : 1   ; // [2]
        unsigned int    cfgErrMask            : 1   ; // [3]
        unsigned int    Reserved_0            : 28  ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_INTMASK;

// Define the union U_JPGE_RAWINT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    VencEndOfPic          : 1   ; // [0]
        unsigned int    VencBufFull           : 1   ; // [1]
        unsigned int    JpgeTimeout           : 1   ; // [2]
        unsigned int    cfgErr                : 1   ; // [3]
        unsigned int    Reserved_0            : 28  ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_RAWINT;

// Define the union U_JPGE_INTCLR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    VencEndOfPicClr       : 1   ; // [0]
        unsigned int    VencBufFullClr        : 1   ; // [1]
        unsigned int    JpgeTimeoutClr        : 1   ; // [2]
        unsigned int    cfgErrClr             : 1   ; // [3]
        unsigned int    Reserved_0            : 28  ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_INTCLR;

// Define the union U_JPGE_PICFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    strFmt                : 2   ; // [1..0]
        unsigned int    yuvFmt                : 2   ; // [3..2]
        unsigned int    chnId                 : 8   ; // [11..4]
        unsigned int    rotationAngle         : 2   ; // [13..12]
        unsigned int    clkGateEn             : 2   ; // [15..14]
        unsigned int    memClkGateEn          : 1   ; // [16]
        unsigned int    configlockEn          : 1   ; // [17]
        unsigned int    timeoutEn             : 1   ; // [18]
        unsigned int    roundEn               : 1   ; // [19]
        unsigned int    packageSel            : 8   ; // [27..20]
        unsigned int    jcpi_clip_en          : 1   ; // [28]
        unsigned int    col2grayEn            : 1   ; // [29]
        unsigned int    reserved2             : 2   ; // [31..30]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_PICFG;

// Define the union U_JPGE_ECSCFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ecsSplit              : 1   ; // [0]
        unsigned int    ecsSize               : 20  ; // [20..1]
        unsigned int    reserved0             : 11  ; // [31..21]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_ECSCFG;

// Define the union U_JPGE_START
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    VStart                : 1   ; // [0]
        unsigned int    Reserved_0            : 31  ; // [31..1]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_START;

// Define the union U_JPGE_VEDIMGSIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    ImgWidthInPixelsMinus1  : 13  ; // [12..0]
        unsigned int    Reserved2             : 3   ; // [15..13]
        unsigned int    ImgHeightInPixelsMinus1  : 13  ; // [28..16]
        unsigned int    Reserved1             : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_VEDIMGSIZE;

// Define the union U_JPGE_CLIP_THR
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    jcpi_luma_min         : 8   ; // [7..0]
        unsigned int    jcpi_luma_max         : 8   ; // [15..8]
        unsigned int    jcpi_chr_min          : 8   ; // [23..16]
        unsigned int    jcpi_chr_max          : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_CLIP_THR;

// Define the union U_JPGE_SSTRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    SrcYStride            : 16  ; // [15..0]
        unsigned int    SrcCStride            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_SSTRIDE;

// Define the union U_JPGE_STRMBUFLEN
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    Reserved_0            : 6   ; // [5..0]
        unsigned int    BufferLength          : 26  ; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_STRMBUFLEN;

// Define the union U_JPGE_OUTSTD
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    outstdNum             : 4   ; // [3..0]
        unsigned int    Reserved_0            : 28  ; // [31..4]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OUTSTD;

// Define the union U_JPGE_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    reserved1             : 5   ; // [4..0]
        unsigned int    osdEn                 : 1   ; // [5]
        unsigned int    reserved0             : 26  ; // [31..6]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_CFG;

// Define the union U_JPGE_OSD_CFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osdRgbfmt             : 1   ; // [0]
        unsigned int    Reserved_0            : 15  ; // [15..1]
        unsigned int    osd0GlobalEn          : 1   ; // [16]
        unsigned int    osd1GlobalEn          : 1   ; // [17]
        unsigned int    osd2GlobalEn          : 1   ; // [18]
        unsigned int    osd3GlobalEn          : 1   ; // [19]
        unsigned int    osd4GlobalEn          : 1   ; // [20]
        unsigned int    osd5GlobalEn          : 1   ; // [21]
        unsigned int    osd6GlobalEn          : 1   ; // [22]
        unsigned int    osd7GlobalEn          : 1   ; // [23]
        unsigned int    osd0En                : 1   ; // [24]
        unsigned int    osd1En                : 1   ; // [25]
        unsigned int    osd2En                : 1   ; // [26]
        unsigned int    osd3En                : 1   ; // [27]
        unsigned int    osd4En                : 1   ; // [28]
        unsigned int    osd5En                : 1   ; // [29]
        unsigned int    osd6En                : 1   ; // [30]
        unsigned int    osd7En                : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_CFG;

// Define the union U_JPGE_OSD01_ALPHA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd0Alpha0            : 8   ; // [7..0]
        unsigned int    osd0Alpha1            : 8   ; // [15..8]
        unsigned int    osd1Alpha0            : 8   ; // [23..16]
        unsigned int    osd1Alpha1            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD01_ALPHA;

// Define the union U_JPGE_OSD23_ALPHA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd2Alpha0            : 8   ; // [7..0]
        unsigned int    osd2Alpha1            : 8   ; // [15..8]
        unsigned int    osd3Alpha0            : 8   ; // [23..16]
        unsigned int    osd3Alpha1            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD23_ALPHA;

// Define the union U_JPGE_OSD45_ALPHA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd4Alpha0            : 8   ; // [7..0]
        unsigned int    osd4Alpha1            : 8   ; // [15..8]
        unsigned int    osd5Alpha0            : 8   ; // [23..16]
        unsigned int    osd5Alpha1            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD45_ALPHA;

// Define the union U_JPGE_OSD67_ALPHA
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd6Alpha0            : 8   ; // [7..0]
        unsigned int    osd6Alpha1            : 8   ; // [15..8]
        unsigned int    osd7Alpha0            : 8   ; // [23..16]
        unsigned int    osd7Alpha1            : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD67_ALPHA;

// Define the union U_JPGE_OSD_ALPHA0123
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd0GlobalAlpha       : 8   ; // [7..0]
        unsigned int    osd1GlobalAlpha       : 8   ; // [15..8]
        unsigned int    osd2GlobalAlpha       : 8   ; // [23..16]
        unsigned int    osd3GlobalAlpha       : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_ALPHA0123;

// Define the union U_JPGE_OSD_ALPHA4567
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd4GlobalAlpha       : 8   ; // [7..0]
        unsigned int    osd5GlobalAlpha       : 8   ; // [15..8]
        unsigned int    osd6GlobalAlpha       : 8   ; // [23..16]
        unsigned int    osd7GlobalAlpha       : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_ALPHA4567;

// Define the union U_JPGE_OSD_POS
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osdStartX             : 13  ; // [12..0]
        unsigned int    reserved1             : 3   ; // [15..13]
        unsigned int    osdStartY             : 13  ; // [28..16]
        unsigned int    reserved0             : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_POS;

// Define the union U_JPGE_OSD_SIZE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osdW                  : 13  ; // [12..0]
        unsigned int    reserved1             : 3   ; // [15..13]
        unsigned int    osdH                  : 13  ; // [28..16]
        unsigned int    reserved0             : 3   ; // [31..29]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_SIZE;

// Define the union U_JPGE_OSD_STRIDE
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osdStride             : 16  ; // [15..0]
        unsigned int    Reserved_0            : 16  ; // [31..16]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_STRIDE;

// Define the union U_JPGE_OSD_LAYERID
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd0LayerId           : 3   ; // [2..0]
        unsigned int    reserved7             : 1   ; // [3]
        unsigned int    osd1LayerId           : 3   ; // [6..4]
        unsigned int    reserved6             : 1   ; // [7]
        unsigned int    osd2LayerId           : 3   ; // [10..8]
        unsigned int    reserved5             : 1   ; // [11]
        unsigned int    osd3LayerId           : 3   ; // [14..12]
        unsigned int    reserved4             : 1   ; // [15]
        unsigned int    osd4LayerId           : 3   ; // [18..16]
        unsigned int    reserved3             : 1   ; // [19]
        unsigned int    osd5LayerId           : 3   ; // [22..20]
        unsigned int    reserved2             : 1   ; // [23]
        unsigned int    osd6LayerId           : 3   ; // [26..24]
        unsigned int    reserved1             : 1   ; // [27]
        unsigned int    osd7LayerId           : 3   ; // [30..28]
        unsigned int    reserved0             : 1   ; // [31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_LAYERID;

// Define the union U_JPGE_OSD_CINVCFG
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    osd0CinvEn            : 1   ; // [0]
        unsigned int    osd1CinvEn            : 1   ; // [1]
        unsigned int    osd2CinvEn            : 1   ; // [2]
        unsigned int    osd3CinvEn            : 1   ; // [3]
        unsigned int    osd4CinvEn            : 1   ; // [4]
        unsigned int    osd5CinvEn            : 1   ; // [5]
        unsigned int    osd6CinvEn            : 1   ; // [6]
        unsigned int    osd7CinvEn            : 1   ; // [7]
        unsigned int    osdCinvWidth          : 2   ; // [9..8]
        unsigned int    osdCinvHeight         : 2   ; // [11..10]
        unsigned int    reserved1             : 4   ; // [15..12]
        unsigned int    osdCinvThr            : 8   ; // [23..16]
        unsigned int    osdCinvMode           : 1   ; // [24]
        unsigned int    reserved0             : 7   ; // [31..25]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_OSD_CINVCFG;

// Define the union U_JPGE_QPT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    QP0                   : 8   ; // [7..0]
        unsigned int    QP1                   : 8   ; // [15..8]
        unsigned int    QP2                   : 8   ; // [23..16]
        unsigned int    QP3                   : 8   ; // [31..24]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} U_JPGE_QPT;

//==============================================================================
// Define the global struct
typedef struct
{
    volatile U_JPGE_INTSTAT         JPGE_INTSTAT;
    volatile U_JPGE_INTMASK         JPGE_INTMASK;
    volatile U_JPGE_RAWINT          JPGE_RAWINT;
    volatile U_JPGE_INTCLR          JPGE_INTCLR;
    volatile U_JPGE_PICFG           JPGE_PICFG;
    volatile U_JPGE_ECSCFG          JPGE_ECSCFG;
    volatile unsigned int           RESERVED0;
    volatile U_JPGE_START           JPGE_START;
    volatile U_JPGE_VEDIMGSIZE      JPGE_VEDIMGSIZE;
    volatile U_JPGE_CLIP_THR        JPGE_CLIP_THR;
    volatile unsigned int           RESERBED1[2];
    volatile unsigned int           JPGE_SRCYADDR;
    volatile unsigned int           JPGE_SRCCADDR;
    volatile unsigned int           JPGE_SRCVADDR;
    volatile U_JPGE_SSTRIDE         JPGE_SSTRIDE;
    volatile unsigned int           JPGE_STRMADDR;
    volatile unsigned int           JPGE_SRPTRADDR;
    volatile unsigned int           JPGE_SWPTRADDR;
    volatile U_JPGE_STRMBUFLEN      JPGE_STRMBUFLEN;
    volatile unsigned int           RESERVED3[8];
    volatile unsigned int           JPGE_PTS0;
    volatile unsigned int           JPGE_PTS1;
    volatile unsigned int           JPGE_PTS2;
    volatile unsigned int           JPGE_PTS3;
    volatile unsigned int           JPGE_TIMEOUT;
    volatile unsigned int           JPGE_TIMER;
    volatile unsigned int           RESERVED4[7];
    volatile U_JPGE_OUTSTD          JPGE_OUTSTD;
    volatile unsigned int           RESERVED4_1[21];
    volatile unsigned int           JPGE_VERSION;
    volatile unsigned int           RESERVED9[256];
    volatile U_JPGE_CFG           VPP_CFG;
    volatile unsigned int           RESERVED10[3];
    volatile U_JPGE_OSD_CFG          VPP_OSD_CFG;
    volatile U_JPGE_OSD01_ALPHA      VPP_OSD01_ALPHA;
    volatile U_JPGE_OSD23_ALPHA      VPP_OSD23_ALPHA;
    volatile U_JPGE_OSD45_ALPHA      VPP_OSD45_ALPHA;
    volatile U_JPGE_OSD67_ALPHA      VPP_OSD67_ALPHA;
    volatile U_JPGE_OSD_ALPHA0123    VPP_OSD_ALPHA0123;
    volatile U_JPGE_OSD_ALPHA4567    VPP_OSD_ALPHA4567;
    volatile unsigned int           RESERVED6;
    volatile U_JPGE_OSD_POS          VPP_OSD_POS[8];
    volatile U_JPGE_OSD_SIZE         VPP_OSD_SIZE[8];
    volatile unsigned int           VPP_OSD_ADDR[8];
    volatile U_JPGE_OSD_STRIDE       VPP_OSD_STRIDE[8];
    volatile unsigned int           RESERVED8[4];
    volatile U_JPGE_OSD_LAYERID      VPP_OSD_LAYERID;
    volatile unsigned int           RESERVED7[3];
    volatile U_JPGE_OSD_CINVCFG      VPP_OSD_CINVCFG;
    volatile unsigned int           RESERVED8_1[651];
    volatile U_JPGE_QPT             JPGE_QPT[48];
} S_JPGE_REGS_TYPE;

// Declare the struct pointor of the module JPGE
extern volatile S_JPGE_REGS_TYPE *gopJPGEAllReg;

// Declare the functions that set the member value
int iSetJPGE_INTMASKVencEndOfPicMask(unsigned int uVencEndOfPicMask);
int iSetJPGE_INTMASKVencBufFullMask(unsigned int uVencBufFullMask);
int iSetJPGE_INTMASKJpgeTimeoutMask(unsigned int uJpgeTimeoutMask);
int iSetJPGE_INTMASKcfgErrMask(unsigned int ucfgErrMask);
int iSetJPGE_INTCLRVencEndOfPicClr(unsigned int uVencEndOfPicClr);
int iSetJPGE_INTCLRVencBufFullClr(unsigned int uVencBufFullClr);
int iSetJPGE_INTCLRJpgeTimeoutClr(unsigned int uJpgeTimeoutClr);
int iSetJPGE_INTCLRcfgErrClr(unsigned int ucfgErrClr);
int iSetJPGE_PICFGstrFmt(unsigned int ustrFmt);
int iSetJPGE_PICFGyuvFmt(unsigned int uyuvFmt);
int iSetJPGE_PICFGchnId(unsigned int uchnId);
int iSetJPGE_PICFGrotationAngle(unsigned int urotationAngle);
int iSetJPGE_PICFGclkGateEn(unsigned int uclkGateEn);
int iSetJPGE_PICFGmemClkGateEn(unsigned int umemClkGateEn);
int iSetJPGE_PICFGconfiglockEn(unsigned int uconfiglockEn);
int iSetJPGE_PICFGtimeoutEn(unsigned int utimeoutEn);
int iSetJPGE_PICFGroundEn(unsigned int uroundEn);
int iSetJPGE_PICFGpackageSel(unsigned int upackageSel);
int iSetJPGE_PICFGjcpi_clip_en(unsigned int ujcpi_clip_en);
int iSetJPGE_PICFGcol2grayEn(unsigned int ucol2grayEn);
int iSetJPGE_ECSCFGecsSplit(unsigned int uecsSplit);
int iSetJPGE_ECSCFGecsSize(unsigned int uecsSize);
int iSetJPGE_STARTVStart(unsigned int uVStart);
int iSetJPGE_VEDIMGSIZEImgWidthInPixelsMinus1(unsigned int uImgWidthInPixelsMinus1);
int iSetJPGE_VEDIMGSIZEImgHeightInPixelsMinus1(unsigned int uImgHeightInPixelsMinus1);
int iSetJPGE_CLIP_THRjcpi_luma_min(unsigned int ujcpi_luma_min);
int iSetJPGE_CLIP_THRjcpi_luma_max(unsigned int ujcpi_luma_max);
int iSetJPGE_CLIP_THRjcpi_chr_min(unsigned int ujcpi_chr_min);
int iSetJPGE_CLIP_THRjcpi_chr_max(unsigned int ujcpi_chr_max);
int iSetJPGE_SRCYADDRVeSrcYAddr(unsigned int uVeSrcYAddr);
int iSetJPGE_SRCCADDRVeSrcCAddr(unsigned int uVeSrcCAddr);
int iSetJPGE_SRCVADDRVeSrcCAddr(unsigned int uVeSrcCAddr);
int iSetJPGE_SSTRIDESrcYStride(unsigned int uSrcYStride);
int iSetJPGE_SSTRIDESrcCStride(unsigned int uSrcCStride);
int iSetJPGE_STRMADDRStreamAddr(unsigned int uStreamAddr);
int iSetJPGE_SRPTRADDRStreamRdPtrAddr(unsigned int uStreamRdPtrAddr);
int iSetJPGE_SWPTRADDRStreamWrPtrAddr(unsigned int uStreamWrPtrAddr);
int iSetJPGE_STRMBUFLENBufferLength(unsigned int uBufferLength);
int iSetJPGE_PTS0PTS0(unsigned int uPTS0);
int iSetJPGE_PTS1PTS1(unsigned int uPTS1);
int iSetJPGE_PTS2PTS2(unsigned int uPTS2);
int iSetJPGE_PTS3PTS3(unsigned int uPTS3);
int iSetJPGE_TIMEOUTjpgeTimeout(unsigned int ujpgeTimeout);
int iSetVPP_CFGosdEn(unsigned int uosdEn);
int iSetVPP_OSD_CFGosdRgbfmt(unsigned int uosdRgbfmt);
int iSetVPP_OSD_CFGosd0GlobalEn(unsigned int uosd0GlobalEn);
int iSetVPP_OSD_CFGosd1GlobalEn(unsigned int uosd1GlobalEn);
int iSetVPP_OSD_CFGosd2GlobalEn(unsigned int uosd2GlobalEn);
int iSetVPP_OSD_CFGosd3GlobalEn(unsigned int uosd3GlobalEn);
int iSetVPP_OSD_CFGosd4GlobalEn(unsigned int uosd4GlobalEn);
int iSetVPP_OSD_CFGosd5GlobalEn(unsigned int uosd5GlobalEn);
int iSetVPP_OSD_CFGosd6GlobalEn(unsigned int uosd6GlobalEn);
int iSetVPP_OSD_CFGosd7GlobalEn(unsigned int uosd7GlobalEn);
int iSetVPP_OSD_CFGosd0En(unsigned int uosd0En);
int iSetVPP_OSD_CFGosd1En(unsigned int uosd1En);
int iSetVPP_OSD_CFGosd2En(unsigned int uosd2En);
int iSetVPP_OSD_CFGosd3En(unsigned int uosd3En);
int iSetVPP_OSD_CFGosd4En(unsigned int uosd4En);
int iSetVPP_OSD_CFGosd5En(unsigned int uosd5En);
int iSetVPP_OSD_CFGosd6En(unsigned int uosd6En);
int iSetVPP_OSD_CFGosd7En(unsigned int uosd7En);
int iSetVPP_OSD01_ALPHAosd0Alpha0(unsigned int uosd0Alpha0);
int iSetVPP_OSD01_ALPHAosd0Alpha1(unsigned int uosd0Alpha1);
int iSetVPP_OSD01_ALPHAosd1Alpha0(unsigned int uosd1Alpha0);
int iSetVPP_OSD01_ALPHAosd1Alpha1(unsigned int uosd1Alpha1);
int iSetVPP_OSD23_ALPHAosd2Alpha0(unsigned int uosd2Alpha0);
int iSetVPP_OSD23_ALPHAosd2Alpha1(unsigned int uosd2Alpha1);
int iSetVPP_OSD23_ALPHAosd3Alpha0(unsigned int uosd3Alpha0);
int iSetVPP_OSD23_ALPHAosd3Alpha1(unsigned int uosd3Alpha1);
int iSetVPP_OSD45_ALPHAosd4Alpha0(unsigned int uosd4Alpha0);
int iSetVPP_OSD45_ALPHAosd4Alpha1(unsigned int uosd4Alpha1);
int iSetVPP_OSD45_ALPHAosd5Alpha0(unsigned int uosd5Alpha0);
int iSetVPP_OSD45_ALPHAosd5Alpha1(unsigned int uosd5Alpha1);
int iSetVPP_OSD67_ALPHAosd6Alpha0(unsigned int uosd6Alpha0);
int iSetVPP_OSD67_ALPHAosd6Alpha1(unsigned int uosd6Alpha1);
int iSetVPP_OSD67_ALPHAosd7Alpha0(unsigned int uosd7Alpha0);
int iSetVPP_OSD67_ALPHAosd7Alpha1(unsigned int uosd7Alpha1);
int iSetVPP_OSD_ALPHA0123osd0GlobalAlpha(unsigned int uosd0GlobalAlpha);
int iSetVPP_OSD_ALPHA0123osd1GlobalAlpha(unsigned int uosd1GlobalAlpha);
int iSetVPP_OSD_ALPHA0123osd2GlobalAlpha(unsigned int uosd2GlobalAlpha);
int iSetVPP_OSD_ALPHA0123osd3GlobalAlpha(unsigned int uosd3GlobalAlpha);
int iSetVPP_OSD_ALPHA4567osd4GlobalAlpha(unsigned int uosd4GlobalAlpha);
int iSetVPP_OSD_ALPHA4567osd5GlobalAlpha(unsigned int uosd5GlobalAlpha);
int iSetVPP_OSD_ALPHA4567osd6GlobalAlpha(unsigned int uosd6GlobalAlpha);
int iSetVPP_OSD_ALPHA4567osd7GlobalAlpha(unsigned int uosd7GlobalAlpha);
int iSetVPP_OSD_POSosdStartX(unsigned int *uposdStartX);
int iSetVPP_OSD_POSosdStartY(unsigned int *uposdStartY);
int iSetVPP_OSD_SIZEosdW(unsigned int *uposdW);
int iSetVPP_OSD_SIZEosdH(unsigned int *uposdH);
int iSetVPP_OSD_ADDRosdAddr(unsigned int *uposdAddr);
int iSetVPP_OSD_STRIDEosdStride(unsigned int *uposdStride);
int iSetVPP_OSD_LAYERIDosd0LayerId(unsigned int uosd0LayerId);
int iSetVPP_OSD_LAYERIDosd1LayerId(unsigned int uosd1LayerId);
int iSetVPP_OSD_LAYERIDosd2LayerId(unsigned int uosd2LayerId);
int iSetVPP_OSD_LAYERIDosd3LayerId(unsigned int uosd3LayerId);
int iSetVPP_OSD_LAYERIDosd4LayerId(unsigned int uosd4LayerId);
int iSetVPP_OSD_LAYERIDosd5LayerId(unsigned int uosd5LayerId);
int iSetVPP_OSD_LAYERIDosd6LayerId(unsigned int uosd6LayerId);
int iSetVPP_OSD_LAYERIDosd7LayerId(unsigned int uosd7LayerId);
int iSetVPP_OSD_CINVCFGosd0CinvEn(unsigned int uosd0CinvEn);
int iSetVPP_OSD_CINVCFGosd1CinvEn(unsigned int uosd1CinvEn);
int iSetVPP_OSD_CINVCFGosd2CinvEn(unsigned int uosd2CinvEn);
int iSetVPP_OSD_CINVCFGosd3CinvEn(unsigned int uosd3CinvEn);
int iSetVPP_OSD_CINVCFGosd4CinvEn(unsigned int uosd4CinvEn);
int iSetVPP_OSD_CINVCFGosd5CinvEn(unsigned int uosd5CinvEn);
int iSetVPP_OSD_CINVCFGosd6CinvEn(unsigned int uosd6CinvEn);
int iSetVPP_OSD_CINVCFGosd7CinvEn(unsigned int uosd7CinvEn);
int iSetVPP_OSD_CINVCFGosdCinvWidth(unsigned int uosdCinvWidth);
int iSetVPP_OSD_CINVCFGosdCinvHeight(unsigned int uosdCinvHeight);
int iSetVPP_OSD_CINVCFGosdCinvThr(unsigned int uosdCinvThr);
int iSetVPP_OSD_CINVCFGosdCinvMode(unsigned int uosdCinvMode);
int iSetJPGE_QPTQP0(unsigned int *upQP0);
int iSetJPGE_QPTQP1(unsigned int *upQP1);
int iSetJPGE_QPTQP2(unsigned int *upQP2);
int iSetJPGE_QPTQP3(unsigned int *upQP3);

#endif // __JPGE_REGR004_H__
