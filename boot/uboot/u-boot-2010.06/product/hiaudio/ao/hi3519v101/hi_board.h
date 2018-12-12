/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_board.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/03/11
  Description   : 
  History       :
  
******************************************************************************/

#ifndef __HI_BOARD_H__
#define __HI_BOARD_H__


/***************************************/
#define CRG_REGS_ADDR       0x12010000
#define CRG_REGS_SIZE       0X10000

/***************************************/
#define SYS_REGS_ADDR       0x20050000
#define SYS_REGS_SIZE       0x10000




#define SYS_PERCTL0_ADDR (0x1C + SYS_REGS_ADDR)
#define SYS_PERCTL1_ADDR (0x20 + SYS_REGS_ADDR)
#define SYS_PERCTL2_ADDR (0x34 + SYS_REGS_ADDR)
#define SYS_PERCTL3_ADDR (0x38 + SYS_REGS_ADDR)
#define SYS_PERCTL4_ADDR (0x3C + SYS_REGS_ADDR)

#define SYS_PERCTL5_ADDR (0x40 + SYS_REGS_ADDR)
#define SYS_PERCTL6_ADDR (0x48 + SYS_REGS_ADDR)
#define SYS_PERCTL7_ADDR (0x4c + SYS_REGS_ADDR)
#define SYS_PERCTL8_ADDR (0x50 + SYS_REGS_ADDR)
#define SYS_PERCTL9_ADDR (0x54 + SYS_REGS_ADDR)

#define SYS_PERCTL10_ADDR (0x58 + SYS_REGS_ADDR)
#define SYS_PERCTL11_ADDR (0x5c + SYS_REGS_ADDR)
#define SYS_PERCTL12_ADDR (0x60 + SYS_REGS_ADDR)
#define SYS_PERCTL13_ADDR (0x64 + SYS_REGS_ADDR)
#define SYS_PERCTL14_ADDR (0x68 + SYS_REGS_ADDR)
#define SYS_PERCTL15_ADDR (0x6c + SYS_REGS_ADDR)
#define SYS_PERCTL16_ADDR (0x70 + SYS_REGS_ADDR)
#define SYS_PERCTL17_ADDR (0x74 + SYS_REGS_ADDR)
#define SYS_PERCTL18_ADDR (0x78 + SYS_REGS_ADDR)
#define SYS_PERCTL19_ADDR (0x7c + SYS_REGS_ADDR)
#define SYS_PERCTL22_ADDR (0x88 + SYS_REGS_ADDR)
#define SYS_PERCTL23_ADDR (0x8c + SYS_REGS_ADDR)

#define CRG_PERCTL0_ADDR (0x0000 + CRG_REGS_ADDR)
#define CRG_PERCTL1_ADDR (0x0004 + CRG_REGS_ADDR)
#define CRG_PERCTL2_ADDR (0x0008 + CRG_REGS_ADDR)
#define CRG_PERCTL3_ADDR (0x000C + CRG_REGS_ADDR)
//#define CRG_PERCTL4_ADDR (0x0010 + CRG_REGS_ADDR)
//#define CRG_PERCTL5_ADDR (0x0014 + CRG_REGS_ADDR)
#define CRG_PERCTL6_ADDR (0x0018 + CRG_REGS_ADDR)
#define CRG_PERCTL7_ADDR (0x001C + CRG_REGS_ADDR)
#define CRG_PERCTL8_ADDR (0x0020 + CRG_REGS_ADDR)
#define CRG_PERCTL9_ADDR (0x0024 + CRG_REGS_ADDR)
#define CRG_PERCTL10_ADDR (0x0028 + CRG_REGS_ADDR)
#define CRG_PERCTL11_ADDR (0x002C + CRG_REGS_ADDR)
#define CRG_PERCTL12_ADDR (0x0030 + CRG_REGS_ADDR)
#define CRG_PERCTL13_ADDR (0x0034 + CRG_REGS_ADDR)
//#define CRG_PERCTL14_ADDR (0x0038 + CRG_REGS_ADDR)
#define CRG_PERCTL15_ADDR (0x003C + CRG_REGS_ADDR)
#define CRG_PERCTL16_ADDR (0x0040 + CRG_REGS_ADDR)
#define CRG_PERCTL17_ADDR (0x0044 + CRG_REGS_ADDR)
#define CRG_PERCTL18_ADDR (0x0048 + CRG_REGS_ADDR)
//#define CRG_PERCTL19_ADDR (0x004C + CRG_REGS_ADDR)
#define CRG_PERCTL20_ADDR (0x0050 + CRG_REGS_ADDR)
//#define CRG_PERCTL21_ADDR (0x0054 + CRG_REGS_ADDR)
#define CRG_PERCTL22_ADDR (0x0058 + CRG_REGS_ADDR)
#define CRG_PERCTL23_ADDR (0x005C + CRG_REGS_ADDR)
#define CRG_PERCTL24_ADDR (0x0060 + CRG_REGS_ADDR)
#define CRG_PERCTL25_ADDR (0x0064 + CRG_REGS_ADDR)
#define CRG_PERCTL26_ADDR (0x0068 + CRG_REGS_ADDR)
#define CRG_PERCTL27_ADDR (0x006C + CRG_REGS_ADDR)
#define CRG_PERCTL28_ADDR (0x0070 + CRG_REGS_ADDR)
#define CRG_PERCTL29_ADDR (0x0074 + CRG_REGS_ADDR)
//#define CRG_PERCTL30_ADDR (0x0078 + CRG_REGS_ADDR)
#define CRG_PERCTL31_ADDR (0x007C + CRG_REGS_ADDR)
#define CRG_PERCTL32_ADDR (0x0080 + CRG_REGS_ADDR)
#define CRG_PERCTL33_ADDR (0x0084 + CRG_REGS_ADDR)
#define CRG_PERCTL34_ADDR (0x0088 + CRG_REGS_ADDR)
#define CRG_PERCTL35_ADDR (0x008C + CRG_REGS_ADDR)
#define CRG_PERCTL36_ADDR (0x0090 + CRG_REGS_ADDR)
#define CRG_PERCTL37_ADDR (0x0094 + CRG_REGS_ADDR)
#define CRG_PERCTL38_ADDR (0x0098 + CRG_REGS_ADDR)
//#define CRG_PERCTL39_ADDR (0x009C + CRG_REGS_ADDR)
//#define CRG_PERCTL40_ADDR (0x00A0 + CRG_REGS_ADDR)


/* For the following function
** typedef HI_S32 FN_SYS_AioSampleClkDivSel(HI_U32 u32DivSel);
*/
#define SYS_AIO_SAMPLE_CLK16  0x0  /* 16 division */
#define SYS_AIO_SAMPLE_CLK32  0x01 /* 32 division */
#define SYS_AIO_SAMPLE_CLK48  0x02 /* 48 division */
#define SYS_AIO_SAMPLE_CLK64  0x03 /* 64 division */
#define SYS_AIO_SAMPLE_CLK128 0x04 /* 128 division */
#define SYS_AIO_SAMPLE_CLK256 0x05 /* 256 division */

/* For the following function
** typedef HI_S32 FN_SYS_AioBitStreamClkDivSel(HI_U32 u32DivSel);
*/
#define SYS_AIO_BS_CLK1   0x00 /* 1 division */
#define SYS_AIO_BS_CLK2   0x02 /* 2 division */
#define SYS_AIO_BS_CLK3   0x01 /* 3 division */
#define SYS_AIO_BS_CLK4   0x03 /* 4 division */
#define SYS_AIO_BS_CLK6   0x04 /* 6 division */
#define SYS_AIO_BS_CLK8   0x05 /* 8 division */
#define SYS_AIO_BS_CLK12  0x06 /* 12 division */
#define SYS_AIO_BS_CLK16  0x07 /* 16 division */
#define SYS_AIO_BS_CLK24  0x08 /* 24 division */
#define SYS_AIO_BS_CLK32  0x09 /* 32 division */
#define SYS_AIO_BS_CLK48  0x0a /* 48 division */
#define SYS_AIO_BS_CLK64  0x0b /* 64 division */



#endif /* __HI_BOARD_H__ */

