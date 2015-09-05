#ifndef DDR_TRAINING_H
#define DDR_TRAINING_H

#define io_read(addr) (*(volatile unsigned int *)(addr))
#define io_write(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define LOOP_NUM 0xff
#define DDRT_BASE_ADDR 0x206d0000
#define TRAINING_WR_CNT 29
#define TRAINING_RD_CNT 15
#define ALL_ERROR 0xff

#define DDRT_BASE_CLK 0x200300d8
#define DDRT_INTSTAT 0x0000
#define DDRT_INTEN 0x0004
#define DDRT_RAWINT 0x0008
#define DDRT_START 0x0010
#define DDRT_MODE 0x0014
#define DDRT_BLKSIZE 0x0018
#define DDRT_LOOP_NUM 0x001c
#define DDRT_WADDR 0x0020
#define DDRT_RADDR 0x0024
#define DDRT_STRIDE 0x0028
#define DDRT_INTCLR 0xc
/* we have 4 databufs, first is 0x130,others are 0x130 + n * 4 */
#define DDRT_DATABUF 0x0130

#define BURST_LENGTH 128
#define SSN_DMA 0x1
#define SSN_VPSS 0x2
#define REG_DMAC_BASE 0x100d0000
#define SSN_SAVE_BASE 0x84000000
#define SSN_PNODE_BASE 0x85000000
#define SSN_DMA_DDR_BASE 0x83000000

#define TRAINING_DMAC_CTL 0x0f492008
#define DMAC_CX_SRC_ADDR(x) (0x100 + (x)*0x20)
#define DMAC_CX_DEST_ADDR(x) (0x104 + (x)*0x20)
#define DMAC_CX_LLI(x) (0x108 + (x)*0x20)
#define DMAC_CX_CONTROL(x) (0x10c + (x)*0x20)
#define DMAC_CX_CONFIG(x)               (0x110 + (x)*0x20)

#define REG_VDH0_BASE 0x20630000
#define VDH_AXI_TEST_MODE 0x0304
#define VDH_AXI_TEST_ST 0x0300
#define VDH_AXI_TEST_ADDR 0x308
#define VDH_AXI_TEST_CMD 0x30c
#define VDH_AXI_TEST_LOOP 0x310
#define VDH_AXI_TEST_STA 0x314
#define VDH_AXI_TEST_RAM 0xb000

#define DDRT_UPLOAD_ENABLE
#define DDRT_WRITE_FLASH_ENABLE
#define DDRT_UPLOAD_NAME "ddrt"
#define DDRT_UPLOAD_DEFNAME "ddrt-reg-4k.bin"
#define DDRT_UPLOAD_DEFLEN  0x1000
#define DDRT_UPLOAD_DEFOFF  0x82000040

/*DDRC reg*/
#define REG_DDRC_BASE0 0x20110000
#define REG_DDRC_BASE1 0x20120000

#define DDRC_LVLSKEW 0x470
#define DDRC_WRDMSKEW 0x508
#define DDRC_WRDQS_SKEW 0x500
#define	DDRC_RDDQS_SKEW 0x504
#define	DDRC_WRDQSKEW(x) (0x510 + (x) * 0x4)
#define DDRC_RDDQSKEW(x) (0x540 + (x) * 0x4)

#define DDRT_MODE_READ 0x0
#define DDRT_MODE_WRITE 0x1
#define DDRT_MODE_WR 0x2

#define DDRT_BYTE_R 1
#define DDRT_BYTE_W 2
#define DDRT_BYTE_WR 3
#define DDRT_BIT_R 4
#define DDRT_BIT_W 5
#define DDRT_BIT_WR 6

#define DDRT_READ_LEVEL 15
#define DDRT_WRITE_LEVEL 29

#define DDR_OPTION_SWITCH(option) do {\
	switch (option) {\
	case DDRT_BYTE_R:\
			 option = DDRT_MODE_READ;\
	break;\
	case DDRT_BYTE_W:\
			 option = DDRT_MODE_WRITE;\
	break;\
	case DDRT_BYTE_WR:\
			  option = DDRT_MODE_WR;\
	break;\
	case DDRT_BIT_R:\
			option = DDRT_MODE_READ;\
	break;\
	case DDRT_BIT_W:\
			option = DDRT_MODE_WRITE;\
	break;\
	case DDRT_BIT_WR:\
			 option = DDRT_MODE_WR;\
	break;\
	default:\
		option = DDRT_MODE_WR;\
	} \
} while (0)

/*#define DDRT_OPTION_REG 0x20050038*/
/*#define DDRT_OPTION_REG 0x20050034*/
#define DDRT_OPTION_REG 0x20050020

#define DDRTRAININT_BYTE 0xff

#define ENABLE_WR_DM 0x000f0000
#define ENABLE_WR_DQS 0x0000000f
/* DDRC WRDQSKEW REG */
#define DISABLE_WR_DQ 0x0
/* DDRC RDDQS_SKEW REG */
#define DISABLE_RD_DQ_DQS 0x0

#define	DQS_OFF_DQ_7 0x80007
#define	DQS_OFF_DQ_6 0x80006
#define	DQS_OFF_DQ_5 0x80005
#define	DQS_OFF_DQ_4 0x80004
#define	DQS_OFF_DQ_3 0x80003
#define	DQS_OFF_DQ_2 0x80002
#define	DQS_OFF_DQ_1 0x80001
#define DQS_0_DQ_7 0x00007
#define	DQS_0_DQ_6 0x00006
#define	DQS_0_DQ_5 0x00005
#define	DQS_0_DQ_4 0x00004
#define	DQS_0_DQ_3 0x00003
#define	DQS_0_DQ_2 0x00002
#define	DQS_0_DQ_1 0x00001
#define	DQS_0_DQ_0 0x00000
#define	DQS_1_DQ_0 0x10000
#define	DQS_2_DQ_0 0x20000
#define	DQS_3_DQ_0 0x30000
#define	DQS_4_DQ_0 0x40000
#define	DQS_5_DQ_0 0x50000
#define	DQS_6_DQ_0 0x60000
#define	DQS_7_DQ_0 0x70000
#define	DQS_1_DQ_OFF 0x10008
#define	DQS_2_DQ_OFF 0x20008
#define	DQS_3_DQ_OFF 0x30008
#define	DQS_4_DQ_OFF 0x40008
#define	DQS_5_DQ_OFF 0x50008
#define	DQS_6_DQ_OFF 0x60008
#define	DQS_7_DQ_OFF 0x70008

#define DQS_DATA_LEVEL0 0x000
#define DQS_DATA_LEVEL1 0x249
#define DQS_DATA_LEVEL2 0x492
#define DQS_DATA_LEVEL3 0x6db
#define DQS_DATA_LEVEL4 0x924
#define DQS_DATA_LEVEL5 0xb6d
#define DQS_DATA_LEVEL6 0xdb6
#define DQS_DATA_LEVEL7 0xfff

#define DQ_DATA_LEVEL0 0xff000000
#define DQ_DATA_LEVEL1 0xff249249
#define DQ_DATA_LEVEL2 0xff492492
#define DQ_DATA_LEVEL3 0xff6db6db
#define DQ_DATA_LEVEL4 0xff924924
#define DQ_DATA_LEVEL5 0xffb6db6d
#define DQ_DATA_LEVEL6 0xffdb6db6
#define DQ_DATA_LEVEL7 0xffffffff

extern unsigned int _ddrtraining_flag;
extern unsigned int _ddrtraining_flag_offset;
extern unsigned int _ddroption_flag;
extern unsigned int _ddroption_flag_offset;
/* data of DDRC */
struct hi_training {
	unsigned int prebit_wr_result[32];
	unsigned int prebit_rd_result[32];
	unsigned int prebit_dm_result[4];
	unsigned int prebit_rd_best[32];
	unsigned int prebit_wr_best[32];
	unsigned int prebit_dm_best[4];

	unsigned int lvlskew_val;
	unsigned int rddqs_val;
	unsigned int rddq_val[4];
	unsigned int wrdqs_val;
	unsigned int wrdq_val[4];
	unsigned int wrdm_val;
	unsigned int wr_result[9];
	unsigned int rd_result[9];
	unsigned int wr_para;
	unsigned int rd_para;
	unsigned int wr_best;
	unsigned int rd_best;
	unsigned int boot_type;
	unsigned int wr_data[29];
	unsigned int rd_data[15];
	unsigned int dq_data[8];
	unsigned int dqs_data[8];
	unsigned int blank_zone_offset;
	unsigned int skew_cnt;
	unsigned int wr_skew_cnt;
	unsigned int rd_skew_cnt;
	unsigned int reg_ddr_base;
	unsigned int ssn_ddr_base;
	unsigned int ssn_vpss_base;
};
/* data of registers */
struct regentry {
	unsigned int reg_addr;
	unsigned int value;
	unsigned int delay;
	unsigned int attr;
};
extern int ddr_training_second_step(void);
extern int ddr_training_ssn_pressure(int total, unsigned int option);
extern int ddr_training_test(unsigned int start, unsigned int size);
extern int ddr_cyc_pressure(int total, unsigned int option);
extern int ddr_result_printf(void);
#endif
