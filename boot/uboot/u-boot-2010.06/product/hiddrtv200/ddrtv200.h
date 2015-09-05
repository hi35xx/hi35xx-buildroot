#ifndef _HI_DDRTV200_H_H_
#define _HI_DDRTV200_H_H_

/* DDRT regs */
#define DDRT_OP 0x000
#define DDRT_STATUS 0x004
#define DDRT_BURST_CONFIG 0x008
#define DDRT_MEM_CONFIG 0x00c
#define DDRT_BURST_NUM 0x010
#define DDRT_ADDR_NUM 0x014
#define DDRT_LOOP_NUM 0x018
#define DDRT_DDR_BASE_ADDR 0x01c
#define DDRT_ADDR 0x020
#define DDRT_WADDR_OFFSET0 0x024
#define DDRT_WADDR_OFFSET1 0x028
#define DDRT_REVERSED_BIT_LOWER 0x030
#define DDRT_REVERSED_BIT_HIGHER 0x034
#define DDRT_SEED 0x038
#define DDRT_KDATA 0x03c
#define DDRT_DATA0 0x040
#define DDRT_DATA1 0x044
#define DDRT_DATA2 0x048
#define DDRT_DATA3 0x04c
#define DDRT_ERR_NUM 0x050
#define DDRT_INTS 0x054
#define DDRT_INTEN 0x058
#define DDRT_INTCLR 0x05c
#define DDRT_DQ_ERR_CNT0 0x060
#define DDRT_DQ_ERR_CNT1 0x064
#define DDRT_DQ_ERR_CNT2 0x068
#define DDRT_DQ_ERR_CNT3 0x06c
#define DDRT_DQ_ERR_CNT4 0x070
#define DDRT_DQ_ERR_CNT5 0x074
#define DDRT_DQ_ERR_CNT6 0x078
#define DDRT_DQ_ERR_CNT7 0x07c
#define DDRT_DQ_ERR_OVFL 0x080
#define DDRT_BIT_ERR_CNT0 0x090
#define DDRT_BIT_ERR_CNT1 0x094
#define DDRT_BIT_ERR_OVFL 0x0a0

#define DDRT_LOOP_NUM_NR 0x00
#define DDRT_DONE (0x1 << 0)
#define DDRT_PASS (0x1 << 1)

enum e_ddrt_pattern {
	E_DDRT_PRBS9 = 0,
	E_DDRT_PRBS7,
	E_DDRT_PRBS11,
	E_DDRT_K28_5,
	E_DDRT_P_END
};

enum e_ddrt_test_mode {
	E_DDRT_WR_RD = 0,
	E_DDRT_WR,
	E_DDRT_RD,
	E_DDRT_T_END
};

struct ddrt_config_ddrt_reg {
	/* DDRT SDRAM config register */
	unsigned int ddrt_mem_cfg;
	/* ddrt data patter */
	enum e_ddrt_pattern pattern;
	/* ddrt test mode w&d/write/read */
	enum e_ddrt_test_mode test_mode;
	/* ddrt reversed bit indicator for lower 32bit */
	unsigned int reversed_lower;
	/* ddrt reversed bit indicator for higher 32bit */
	unsigned int reversed_higher;
	/* burst num in each loop */
	unsigned int burst_num;
	/* ddr starting addr */
	unsigned int start_addr;
	/* address offset, at least 128bit align */
	unsigned int offset_addr;
	/* loop number in each test */
	unsigned int loop_num;
	unsigned int k_data;
	unsigned int data0;
	unsigned int data1;
	unsigned int data2;
	unsigned int data3;
};

#endif
