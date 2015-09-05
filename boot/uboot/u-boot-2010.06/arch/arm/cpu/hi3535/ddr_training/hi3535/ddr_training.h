#ifndef DDR_TRAINING
#define DDR_TRAINING

#ifndef __ASSEMBLY__

#include "ddr_training_common.h"

/*** register operations ***/
#define io_read(addr)                (*(volatile unsigned int *)(addr))
#define io_write(addr, val)          (*(volatile unsigned int *)(addr) = (val))

/*** for calculate best level definations ***/
#define TRAINING_WR_CNT                32
#define TRAINING_RD_CNT                32

#define SSN_DDRT

#define WRDQ_LEVEL                     32
#define WRDQS_LEVEL                    32
#define RDDQ_LEVEL                     32
#define RDDQS_LEVEL                    128
#define ACDQ_LEVEL                     32

/*** for ddr training item definations ***/
#define WRITE_DESKEW                   1
#define READ_DESKEW                    1

/*** for ddr phy layout definations ***/
#define BYTEWIDTH                      4
#define BITWIDTH                       32

#define WR_COARSE_STEP                 4
#define RD_COARSE_STEP                 8
#define AC_COARSE_STEP                 4
#define FINE_STEP                      1

/*** S40 regsister ***/
/*** DDRC PHY regsister offest address ***/
#define REG_DDRC_BASE                  0x20118000

/*WR DQ0-DQ3 BIT 4:0,12:8,20:16, 28:24 */
#define DDRC_DXNWDQBDL0(n)             (0x210 + ((n) << 7))
/*WR DQ4-DQ7 BIT 4:0,12:8,20:16, 28:24*/
#define DDRC_DXNWDQBDL1(n)             (0x214 + ((n) << 7))
/*WR DM */
#define DDRC_DXNWDQBDL2(n)             (0x218 + ((n) << 7))
/*RD DQ4-DQ7 BIT 4:0,12:8,20:16, 28:24*/
#define DDRC_DXNRDQBDL0(n)             (0x21C + ((n) << 7))
/*RD DQ4-DQ7 BIT 4:0,12:8,20:16, 28:24*/
#define DDRC_DXNRDQBDL1(n)             (0x220 + ((n) << 7))
#define DDRC_DXNRDQBDL2(n)             (0x224 + ((n) << 7))
/* rdqs_bdl 6:0 rdqs_cyc 22:16 */
#define DDRC_DXNRDQSDLY(n)             (0x22C + ((n) << 7))
#define DDRC_DXWDQSDLY(n)              (0x230 + ((n) << 7))
/*WR DQ phase BIT 12:8 */
#define DDRC_DXNWDQDLY(n)              (0x234 + ((n) << 7))
/*rddqs gating*/
#define DDRC_DXNDQSGDLY(n)             (0x23c + ((n) << 7))
/*read boundary  right 0:6 left 22:16 */
#define DDRC_DXNRDBOUND(n)             (0x250 + ((n) << 7))
/*write boundary  right 0:6 left 22:16 */
#define DDRC_DXNWDBOUND(n)             (0x254 + ((n) << 7))
#define DDRC_MISC                      0x70

#define DDRC_PHYINITCTRL               0x004

#define DDRC_ACCMDBDL0                 0x120 /*ODT0 4:0   ODT1 20:16*/
#define DDRC_ACCMDBDL1                 0x124 /*ODT2 4:0   ODT3 20:16*/
#define DDRC_ACCMDBDL2                 0x128 /*CS0 4:0   CS1 20:16*/
#define DDRC_ACCMDBDL3                 0x12c /*CS2 4:0   CS3 20:16*/
#define DDRC_ACCMDBDL4                 0x130 /*CKE0 4:0   CKE1 20:16*/
#define DDRC_ACCMDBDL5                 0x134 /*CKE2 4:0   CKE3 20:16*/
#define DDRC_ACCMDBDL6                 0x138 /*WE 4:0   CAS 20:16*/
#define DDRC_ACCMDBDL7                 0x13c /*RAS 4:0   RESET 20:16*/
#define DDRC_ACADDRBDL0                0x140 /*A0 4:0   A1 20:16*/
#define DDRC_ACADDRBDL1                0x144 /*A2 4:0   A3 20:16*/
#define DDRC_ACADDRBDL2                0x148 /*A4 4:0   A5 20:16*/
#define DDRC_ACADDRBDL3                0x14c /*A6 4:0   A7 20:16*/
#define DDRC_ACADDRBDL4                0x150 /*A8 4:0   A9 20:16*/
#define DDRC_ACADDRBDL5                0x154 /*A10 4:0   A11 20:16*/
#define DDRC_ACADDRBDL6                0x158 /*A12 4:0   A13 20:16*/
#define DDRC_ACADDRBDL7                0x15c /*A14 4:0   A15 20:16*/
#define DDRC_ACADDRBDL8                0x160 /*BA0 4:0   BA1 20:16*/
#define DDRC_ACADDRBDL9                0x164 /*BA2 4:0 */
#define DDRC_ACCLKBDL                  0x168 /*CLK*/

#define DDRT_TRAINING_ADDR             0x80000000
#define REVERSED_DATA                  0xB49C863C

/* four ddr  reversal = 0xB49C863C */
/* s40v200 demo reversal=  0x8DE18DE1 */

#ifdef SSN_DDRT
#define REG_DDRT_BASE_CRG              0x206d0000
#define DDRT_OP                        0x0
#define DDRT_STATUS                    0x4
#define DDRT_BURST_TRANSFER            0x8
#define DDRT_MEM_CONFIG                0xc
#define DDRT_BURST_NUM                 0x10
#define DDRT_ADDR_NUM                  0x14
#define DDRT_LOOP_NUM                  0x18
#define DDRT_START_ADDR                0x1c
#define DDRT_ADDR                      0x20
#define DDRT_ADDR_OFFSET0              0x24
#define DDRT_REVERSED_DQ               0x30
#define DDRT_KDATA                     0x3c
#define DDRT_DATA0                     0x40
#define DDRT_DATA1                     0x44
#define DDRT_DATA2                     0x48
#define DDRT_DATA3                     0x4c
#define DDRT_TEST_CLK                  0x200300d8
#define DDRT_ENABLE                    0x20120004

#define DDRT_DQ_ERR_CNT(n)             (0x60 + ((n) << 2))
#define DDRT_DQ_ERR_OVFL               0x80

#define DDRT_START                     0x1
#define BURST_NUM                      0xff

#define RW_COMPRARE_MODE               (0<<8)
#define ONLY_WRITE_MODE                (1<<8)
#define ONLY_READ_MODE                 (2<<8)
#define RANDOM_RW_MODE                 (3<<8)

#define DDRT_PATTERM_PRBS9             (0<<12)
#define DDRT_PATTERM_PRBS7             (1<<12)
#define DDRT_PATTERM_PRBS11            (2<<12)
#define DDRT_PATTERM_K28_5             (3<<12)
#endif /* SSN_DDRT */

#define DDRT_TIME
#ifdef DDRT_TIME
#  define WATCH_DOG                    0x24000000
#  define WDG_LOAD                     0x0
#  define WDG_VALUE                    0x4
#  define WDG_CONTROL                  0x8
#  define WDG_LOCK                     0xc00
#endif /* DDRT_TIME */

#define DQ_DATA_LEVEL(n)               (n)

#define DDR_DQSTRAIN                   1
#define DDR_ADDRTRAIN                  2

/*** data of DDRC ***/
struct training_data {
	unsigned int flag;

	unsigned int wr_left_dqs_best[BYTEWIDTH];
	unsigned int wr_right_dqs_best[BYTEWIDTH];
	unsigned int wr_wodows_width[BYTEWIDTH];
	unsigned int wr_dqs_center_best[BYTEWIDTH];

	unsigned int rd_left_dqs_best[BYTEWIDTH];
	unsigned int rd_right_dqs_best[BYTEWIDTH];
	unsigned int rd_wodows_width[BYTEWIDTH];
	unsigned int rd_dqs_center_best[BYTEWIDTH];

	unsigned int rd_bit_result[BITWIDTH];
	unsigned int wr_bit_result[BITWIDTH];
	unsigned int rd_bit_best[BITWIDTH];
	unsigned int wr_bit_best[BITWIDTH];

	unsigned int wrdqs_val[BYTEWIDTH];
	unsigned int wrdq_val[BITWIDTH];
	unsigned int rddqs_val[BYTEWIDTH];
	unsigned int rddq_val[BITWIDTH];
};

#endif /* __ASSEMBLY__ */

#endif /* DDR_TRAINING */
