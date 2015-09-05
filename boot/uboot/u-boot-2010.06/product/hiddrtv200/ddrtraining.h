#ifndef _HI_DDR_TRAINING_V200_H_
#define _HI_DDR_TRAINING_V200_H_

#include <config.h>
#include "ddrtv200.h"

#define ddrt_write(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define ddrt_read(addr) (*(volatile unsigned int *)(addr))

/* error num */
#define DDRT_SUCCESS 0
#define DDRT_ENULL 1
#define DDRT_EFAIL 2

#define DDRT_BYTE0 0
#define DDRT_BYTE1 1
#define DDRT_BYTE2 2
#define DDRT_BYTE3 3

#define DDRT_CMD_BIT_WR_RD 1
#define DDRT_CMD_BIT_WR 2
#define DDRT_CMD_BIT_RD 3

#define DDRT_CMD_BYTE 1
#define DDRT_CMD_BIT 2

#define DDRT_CMD_WR_RD 0x0
#define DDRT_CMD_WR 0x1
#define DDRT_CMD_RD 0x2

#define ddrt_option(reg, wr_rd) do {\
	switch (reg) {\
	case DDRT_CMD_BIT_WR_RD:\
		wr_rd = DDRT_CMD_WR_RD;\
		break;\
	case DDRT_CMD_BIT_WR:\
		wr_rd = DDRT_CMD_WR;\
		break;\
	case DDRT_CMD_BIT_RD:\
		wr_rd = DDRT_CMD_RD;\
		break;\
	default:\
		wr_rd = DDRT_CMD_WR_RD;\
	} \
} while (0)

/* ddrc reg */
#define DDRC_WRDQSKEW(x) (0xAFC + x * 4)
#define DDRC_RDDQSKEW(x) (0xB2C + x * 4)
#define DDRC_WRDQSSKEW 0xB1C
#define DDRC_RDDQSSKEW 0xB4C

#define DDRT_BYTE_NR 2
#define DDRT_BIT_NR (DDRT_BYTE_NR * 8)
#define DDRT_BIT_PER_BYTE 8

#define DDRT_TRAINING_WR 0x0
#define DDRT_TRAINING_RD 0x1

#define DDRC_DQ_DQS_LEVEL 8
#define DDRC_PHY_DQ_NR 8
#define DDRC_PHY_DQS_NR 2

#define DQ_DQS_DATA_LEVEL0 0x0
#define DQ_DQS_DATA_LEVEL1 0x9
#define DQ_DQS_DATA_LEVEL2 0x12
#define DQ_DQS_DATA_LEVEL3 0x1b
#define DQ_DQS_DATA_LEVEL4 0x24
#define DQ_DQS_DATA_LEVEL5 0x2d
#define DQ_DQS_DATA_LEVEL6 0x36
#define DQ_DQS_DATA_LEVEL7 0x3f

#define DDRT_DEFAULT_BURST_NR 8
#define DDRT_DEFAULT_PATTERN E_DDRT_PRBS9
#define DDRT_DEFAULT_DDR_START_ADDR 0x82000000
#define DDRT_DEFAULT_DDR_OFFSET 0x1000
#define DDRT_DEFAULT_LOOP_NUM 0xf
#define DDRT_DEFAULT_MEM_CFG_VAL 0x0132
#define DDRT_DEFAULT_KDATA 0xbc3eb053
#define DDRT_DEFAULT_DATA0 0x8a18207f
#define DDRT_DEFAULT_DATA1 0x5f2b9a27
#define DDRT_DEFAULT_DATA2 0xbdad9238
#define DDRT_DEFAULT_DATA3 0xaa6774b1


typedef struct _hi_ddrt_struct {
	/* bit */
	unsigned int bit_wr_result[DDRT_BIT_NR];
	unsigned int bit_rd_result[DDRT_BIT_NR];
	unsigned int bit_wr_best[DDRT_BIT_NR];
	unsigned int bit_rd_best[DDRT_BIT_NR];
	unsigned int bit_rd_result_dll2[DDRT_BIT_NR];

	/* ddrc reg */
	unsigned int wrdqs_val;
	unsigned int rddqs_val;
	unsigned int rddq_val[DDRC_PHY_DQ_NR];
	unsigned int wrdq_val[DDRC_PHY_DQ_NR];

	unsigned int dq_dqs_level[DDRC_DQ_DQS_LEVEL];
	unsigned int skew_cnt;
} ddrt_struct, *p_ddrt_struct;

extern int ddrt_second_step(void);
extern int ddrt_entry(unsigned int blank_zone_offset);
extern int ddr_training_second_step(void);
extern int ddrt_show_result(void);
extern int ddrt_2bit_training(p_ddrt_struct pddrt);
extern int ddrt_config_init(void);
extern int ddrt_pressure_test(void);
extern int ddrt_set_config(unsigned int mode, unsigned int pattern,
		unsigned int mem_cfg,
		unsigned int reversed_lower,
		unsigned int reversed_higher,
		unsigned int loop_num,
		unsigned int burst_num,
		unsigned int start_addr,
		unsigned int offset_addr,
		unsigned int k_data,
		unsigned int data0,
		unsigned int data1,
		unsigned int data2,
		unsigned int data3);
extern void ddrt_reset_ddrt(void);

#endif
