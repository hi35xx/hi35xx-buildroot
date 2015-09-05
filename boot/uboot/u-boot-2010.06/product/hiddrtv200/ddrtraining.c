#include <asm/arch/platform.h>
#include <linux/types.h>
#include <linux/string.h>
#include <common.h>

#include "ddrtraining.h"

static int ddrt_struct_init(ddrt_struct *pddrt,
		unsigned int reg1, unsigned int reg2);

static void ddrt_config_ddr(ddrt_struct *pddrt);

static void ddrt_byte_set_dq(unsigned int *dq,
		unsigned int val, unsigned int bytenum);

static void ddrt_wl_init(ddrt_struct *pddrt);
static void ddrt_start_ddrt(void);
static void ddrt_config_ddrt(void);
static unsigned int mutibyte_check_multi_ssn(unsigned int bit_num);
static unsigned int ddrt_dq_overflowing(unsigned int bit_num,
		unsigned int byte_num);
static unsigned int ddrt_dll_repair2(unsigned int dll2);
static int ddrt_get_bit_best_para(p_ddrt_struct pddrt,
		unsigned int byte_nr, unsigned int wr_rd);
static int ddrt_set_bit_best_para(p_ddrt_struct pddrt,
		unsigned int byte_nr, unsigned int wr_rd);

/*static int ddrt_2bit_training(p_ddrt_struct pddrt);*/
static int ddrt_bit_set_dqs(unsigned int dqs, unsigned int val,
		unsigned int bytenum, unsigned int bytewidth);
static int ddrt_read_ddr_config(p_ddrt_struct pddrt);
static int ddrt_bit_travel_wrdq(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte);
static int ddrt_bit_travel_rddq(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte);
static int ddrt_bit_set_dq(unsigned int *dq, unsigned int val,
		unsigned int bitnum, unsigned int bytenum);
static int ddrt_bit_wr_level(unsigned int dqs_level, unsigned int dq_level);
static int ddrt_print_bit_result(p_ddrt_struct pddrt);

static int ddrt_bit_travel_wrdqs(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte);
static int ddrt_bit_travel_rddqs(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte);
static int ddrt_bit_calculate_para(unsigned int size, unsigned int val);
static int ddrt_level_spilt(unsigned int level, unsigned int *dqs_level,
		unsigned int *dq_level);

static struct ddrt_config_ddrt_reg g_ddrt_config;


/**
 *  ddrt_entry ddr training entry point
 *  @blank_zone_offset
 *
 *  @return
 */
int ddrt_entry(unsigned int blank_zone_offset)
{
	unsigned int reg1 = 0, reg2 = 0;
	ddrt_struct *pddrt = (ddrt_struct *)DDRT_STRUCT_ADDR;

	reg1 = ddrt_read(DDRT_OPTION_REG1);
	reg2 = ddrt_read(DDRT_OPTION_REG2);

	if (DDRT_SUCCESS != ddrt_struct_init(pddrt, reg1, reg2))
		return -DDRT_EFAIL;

	ddrt_read_ddr_config(pddrt);

	if (DDRT_SUCCESS != ddrt_2bit_training(pddrt))
		return -DDRT_EFAIL;

	return DDRT_SUCCESS;
}

/**
 *  ddrt_2bit_training
 *  @pddrt
 *
 *  @return
 */
int ddrt_2bit_training(p_ddrt_struct pddrt)
{
	unsigned int tmp = 0;
	int i = 0, j = 0;

	for (j = 0; j < 8; ++j) {
		tmp = pddrt->dq_dqs_level[0];
		/* bit 0~7 */
		for (i = 0; i < DDRT_BYTE_NR; ++i)
			pddrt->wrdqs_val = ddrt_bit_set_dqs(
					pddrt->wrdqs_val, tmp, i, 0);
		ddrt_bit_travel_wrdq(pddrt, j, DDRT_BYTE0);

		tmp = pddrt->dq_dqs_level[0];
		for (i = 0; i < DDRT_BYTE_NR; ++i)
			ddrt_bit_set_dq(pddrt->wrdq_val, tmp, j, i);

		ddrt_bit_travel_wrdqs(pddrt, j, DDRT_BYTE0);

		tmp = pddrt->dq_dqs_level[0];
		ddrt_byte_set_dq(pddrt->wrdq_val, tmp, 0);
		ddrt_byte_set_dq(pddrt->wrdq_val, tmp, 1);
	}

	for (i = 0; i < DDRT_BYTE_NR; ++i) {
		ddrt_get_bit_best_para(pddrt, i, DDRT_TRAINING_WR);
		ddrt_set_bit_best_para(pddrt, i, DDRT_TRAINING_WR);
	}
	ddrt_config_ddr(pddrt);

	for (j = 0; j < 8; ++j) {
		tmp = pddrt->dq_dqs_level[0];
		/* bit 0~7 */
		for (i = 0; i < DDRT_BYTE_NR; ++i)
			pddrt->rddqs_val = ddrt_bit_set_dqs(
					pddrt->rddqs_val, tmp, i, 0);
		ddrt_bit_travel_rddq(pddrt, j, DDRT_BYTE0);

		tmp = pddrt->dq_dqs_level[0];
		for (i = 0; i < DDRT_BYTE_NR; ++i)
			ddrt_bit_set_dq(pddrt->rddq_val, tmp, j, i);

		ddrt_bit_travel_rddqs(pddrt, j, DDRT_BYTE0);
		tmp = pddrt->dq_dqs_level[0];
		ddrt_byte_set_dq(pddrt->rddq_val, tmp, 0);
		ddrt_byte_set_dq(pddrt->rddq_val, tmp, 1);
	}

	for (i = 0; i < DDRT_BIT_NR; i++) {
		pddrt->bit_rd_result_dll2[i] =
			pddrt->bit_rd_result[i];
		pddrt->bit_rd_result[i] =
			ddrt_dll_repair2(pddrt->bit_rd_result_dll2[i]);
	}

	for (i = 0; i < DDRT_BYTE_NR; ++i) {
		ddrt_get_bit_best_para(pddrt, i, DDRT_TRAINING_RD);
		ddrt_set_bit_best_para(pddrt, i, DDRT_TRAINING_RD);
	}

	ddrt_config_ddr(pddrt);

	return DDRT_SUCCESS;
}

static unsigned int ddrt_dll_repair2(unsigned int dll2)
{
	unsigned int output = 0;
	unsigned int i = 0, bitnum = 0;
	unsigned int tmp = 0;

	for (i = 8; i < 16; i++) {
		tmp = dll2 >> i;
		tmp = tmp & 1;
		if (tmp == 0) {
			bitnum = i - 1;
			break;
		}
	}

	tmp = 0;
	for (i = 0; i < bitnum; i++)
		tmp |= 1 << i;

	output = dll2 & tmp;
	return output;
}

static int ddrt_read_ddr_config(p_ddrt_struct pddrt)
{
	unsigned int i = 0;
	for (i = 0; i < DDRC_PHY_DQ_NR; ++i) {
		pddrt->wrdq_val[i] =
			ddrt_read(DDRC0_REG_BASE + DDRC_WRDQSKEW(i));
		pddrt->rddq_val[i] =
			ddrt_read(DDRC0_REG_BASE + DDRC_RDDQSKEW(i));
	}

	pddrt->wrdqs_val = ddrt_read(DDRC0_REG_BASE + DDRC_WRDQSSKEW);
	pddrt->rddqs_val = ddrt_read(DDRC0_REG_BASE + DDRC_RDDQSSKEW);

	return DDRT_SUCCESS;
}

int ddrt_config_init(void)
{
	return ddrt_set_config(E_DDRT_WR_RD,
			DDRT_DEFAULT_PATTERN,
			DDRT_DEFAULT_MEM_CFG_VAL,
			0, 0,
			DDRT_DEFAULT_LOOP_NUM,
			DDRT_DEFAULT_BURST_NR,
			DDRT_DEFAULT_DDR_START_ADDR,
			DDRT_DEFAULT_DDR_OFFSET,
			DDRT_DEFAULT_KDATA,
			DDRT_DEFAULT_DATA0,
			DDRT_DEFAULT_DATA1,
			DDRT_DEFAULT_DATA2,
			DDRT_DEFAULT_DATA3);
}

/**
 * ddrt_set_config
 *  @mode write & read/write/read
 *  @pattern PRBS9/PRBS7 etc
 */
int ddrt_set_config(unsigned int mode, unsigned int pattern,
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
		unsigned int data3
		)
{

	g_ddrt_config.test_mode = mode;
	g_ddrt_config.pattern = pattern;

	g_ddrt_config.ddrt_mem_cfg = mem_cfg;

	g_ddrt_config.reversed_lower = reversed_lower;
	g_ddrt_config.reversed_higher = reversed_higher;

	g_ddrt_config.loop_num = loop_num;
	g_ddrt_config.burst_num = burst_num;
	g_ddrt_config.start_addr = start_addr;
	g_ddrt_config.offset_addr = offset_addr;

	g_ddrt_config.k_data = k_data;
	g_ddrt_config.data0 = data0;
	g_ddrt_config.data1 = data1;
	g_ddrt_config.data2 = data2;
	g_ddrt_config.data3 = data3;

	return DDRT_SUCCESS;
}

static int ddrt_struct_init(ddrt_struct *pddrt,
		unsigned int reg1, unsigned int reg2)
{
	unsigned int wr_rd = 0;
	if (!pddrt)
		return -DDRT_ENULL;

	memset(pddrt, 0x0, sizeof(ddrt_struct));
	ddrt_wl_init(pddrt);

	ddrt_config_init();
	ddrt_option(reg1, wr_rd);
	g_ddrt_config.test_mode = (enum e_ddrt_test_mode)wr_rd;

	return DDRT_SUCCESS;
}

static void ddrt_wl_init(ddrt_struct *pddrt)
{
	pddrt->dq_dqs_level[0] = DQ_DQS_DATA_LEVEL0;
	pddrt->dq_dqs_level[1] = DQ_DQS_DATA_LEVEL1;
	pddrt->dq_dqs_level[2] = DQ_DQS_DATA_LEVEL2;
	pddrt->dq_dqs_level[3] = DQ_DQS_DATA_LEVEL3;
	pddrt->dq_dqs_level[4] = DQ_DQS_DATA_LEVEL4;
	pddrt->dq_dqs_level[5] = DQ_DQS_DATA_LEVEL5;
	pddrt->dq_dqs_level[6] = DQ_DQS_DATA_LEVEL6;
	pddrt->dq_dqs_level[7] = DQ_DQS_DATA_LEVEL7;

	return;
}

static void ddrt_config_ddr(ddrt_struct *pddrt)
{
	unsigned int i = 0;

	for (i = 0; i < DDRC_DQ_DQS_LEVEL; ++i) {
		ddrt_write(DDRC0_REG_BASE + DDRC_WRDQSKEW(i),
				pddrt->wrdq_val[i]);
		ddrt_write(DDRC0_REG_BASE + DDRC_RDDQSKEW(i),
				pddrt->rddq_val[i]);
	}

	ddrt_write(DDRC0_REG_BASE + DDRC_WRDQSSKEW, pddrt->wrdqs_val);
	ddrt_write(DDRC0_REG_BASE + DDRC_RDDQSSKEW, pddrt->rddqs_val);

	return;
}

static void ddrt_byte_set_dq(unsigned int *dq,
		unsigned int val, unsigned int byte_nr)
{
	dq[byte_nr * 4 + 0] = val;
	dq[byte_nr * 4 + 1] = val;
	dq[byte_nr * 4 + 2] = val;
	dq[byte_nr * 4 + 3] = val;
}

static void ddrt_start_ddrt(void)
{
	unsigned int tmp = 0;
	tmp = ddrt_read(DDRT_BASE_ADDR + DDRT_OP);
	tmp |= 0x1;
	ddrt_write(DDRT_BASE_ADDR + DDRT_OP, tmp);
	return;
}

void ddrt_reset_ddrt(void)
{
	unsigned int val = 0;

	val = ddrt_read(DDRT_SYS_BASE_ADDR);
	val |= DDRT_ENABLE_SYS;
	ddrt_write(DDRT_SYS_BASE_ADDR, val);

	val = ddrt_read(DDRT_CLK_BASE_ADDR);
	val &= (~DDRT_RESET);
	ddrt_write(DDRT_CLK_BASE_ADDR, val);

	val = ddrt_read(DDRT_CLK_BASE_ADDR);
	val |= (DDRT_CLK);
	ddrt_write(DDRT_CLK_BASE_ADDR, val);

	return;
}

static void ddrt_config_ddrt(void)
{
	unsigned int tmp = 0;
	unsigned int tmp2 = 0;

	/* set ddrt pattern */
	tmp = ddrt_read(DDRT_BASE_ADDR + DDRT_OP);
	tmp2 = g_ddrt_config.pattern;
	tmp &= ~(0x3 << 12);
	tmp |= (tmp2 << 12);
	ddrt_write(DDRT_BASE_ADDR + DDRT_OP, tmp);

	ddrt_write(DDRT_BASE_ADDR + DDRT_MEM_CONFIG,
			g_ddrt_config.ddrt_mem_cfg);

	ddrt_write(DDRT_BASE_ADDR + DDRT_BURST_NUM,
			g_ddrt_config.burst_num);

	ddrt_write(DDRT_BASE_ADDR + DDRT_LOOP_NUM,
			g_ddrt_config.loop_num);

	ddrt_write(DDRT_BASE_ADDR + DDRT_ADDR,
			g_ddrt_config.start_addr);

	ddrt_write(DDRT_BASE_ADDR + DDRT_WADDR_OFFSET0,
			g_ddrt_config.offset_addr);

	ddrt_write(DDRT_BASE_ADDR + DDRT_REVERSED_BIT_LOWER,
			g_ddrt_config.reversed_lower);

	ddrt_write(DDRT_BASE_ADDR + DDRT_REVERSED_BIT_HIGHER,
			g_ddrt_config.reversed_higher);

	ddrt_write(DDRT_BASE_ADDR + DDRT_KDATA,
			g_ddrt_config.k_data);

	ddrt_write(DDRT_BASE_ADDR + DDRT_DATA0,
			g_ddrt_config.data0);

	ddrt_write(DDRT_BASE_ADDR + DDRT_DATA1,
			g_ddrt_config.data1);

	ddrt_write(DDRT_BASE_ADDR + DDRT_DATA2,
			g_ddrt_config.data2);

	ddrt_write(DDRT_BASE_ADDR + DDRT_DATA3,
			g_ddrt_config.data3);

	return;
}

int ddr_training_second_step(void)
{
	ddrt_struct *pddrt = (ddrt_struct *)DDRT_STRUCT_ADDR;
	ddrt_print_bit_result(pddrt);
	return 0;
}

static unsigned int ddrt_dq_overflowing(unsigned int bit_num,
		unsigned int byte_num)
{
	unsigned int tmp_overflowing = 0;
	unsigned int overflowing_result = 1;
	tmp_overflowing = ddrt_read(DDRT_BASE_ADDR + DDRT_DQ_ERR_OVFL);
	if (0 == (tmp_overflowing & (0x1 << (bit_num + 8 * byte_num)))) {
		if (bit_num < 4) {
			tmp_overflowing = ddrt_read(DDRT_BASE_ADDR +
					DDRT_DQ_ERR_CNT0 + 0x8 * byte_num);
		} else {
			tmp_overflowing = ddrt_read(DDRT_BASE_ADDR +
					DDRT_DQ_ERR_CNT1 + 0x8 * byte_num);
		}
		if (0 == tmp_overflowing)
			overflowing_result = 0;
	}

	return overflowing_result;
}

#define ddrt_tmp_check() do {\
	ddrt_config_ddrt();\
	ddrt_start_ddrt();\
	do {\
		tmp_check1 = ddrt_read(DDRT_BASE_ADDR + DDRT_STATUS);\
	} while (0 == (DDRT_DONE & tmp_check1));\
	if (0 == (DDRT_PASS & tmp_check1)) {\
		if (ddrt_dq_overflowing(bit_num, DDRT_BYTE0))\
			word_cmp_result[0] = 1;\
		if (ddrt_dq_overflowing(bit_num, DDRT_BYTE1))\
			word_cmp_result[1] = 1;\
	} \
} while (0)

static unsigned int mutibyte_check_multi_ssn(unsigned int bit_num)
{
	unsigned int burst_num = 0xff;
	unsigned int loop_num = 1;
	unsigned int start_addr = 0x82000000;
	unsigned int mem_cfg = DDRT_DEFAULT_MEM_CFG_VAL;
	unsigned int offset_addr = DDRT_DEFAULT_DDR_OFFSET;
	unsigned int k_data = DDRT_DEFAULT_KDATA;
	unsigned int data0 = DDRT_DEFAULT_DATA0;
	unsigned int data1 = DDRT_DEFAULT_DATA1;
	unsigned int data2 = DDRT_DEFAULT_DATA2;
	unsigned int data3 = DDRT_DEFAULT_DATA3;

	unsigned int word_cmp_result[2] = {0};
	unsigned int final_result = 0;
	unsigned int tmp_check1 = 0;

	ddrt_reset_ddrt();

	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS9, mem_cfg, 0x0, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS9, mem_cfg, 0x4b2c, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS9, mem_cfg, 0xb4d3, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	start_addr = 0x83000000;
	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS7, mem_cfg, 0x0, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS7, mem_cfg, 0x4b2c, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	start_addr = 0x84000000;
	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS11, mem_cfg, 0x0, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS11, mem_cfg, 0x4b2c, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_K28_5, mem_cfg, 0x0, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	ddrt_tmp_check();
	if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 1))
		return final_result = 0x0;

	start_addr = 0x85000000;
	ddrt_set_config(E_DDRT_WR_RD, E_DDRT_PRBS7, mem_cfg, 0x0, 0x0,
			loop_num, burst_num, start_addr, offset_addr,
			k_data, 0x55555555, 0x8a18207f, 0x5f2b9a27, 0x55555555);
	ddrt_tmp_check();

	if ((word_cmp_result[0] == 0) && (word_cmp_result[1] == 0))
		final_result = 0x3;
	else if ((word_cmp_result[0] == 1) && (word_cmp_result[1] == 0))
		final_result = 0x2;
	else if ((word_cmp_result[0] == 0) && (word_cmp_result[1] == 1))
		final_result = 0x1;
	else
		final_result = 0x0;

	return final_result;
}


static int ddrt_bit_travel_wrdq(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte)
{
	int i = 0;
	unsigned int bit_num1 = 0, bit_num2 = 0;
	unsigned int tmp = 0;
	unsigned int level = 0;
	unsigned int check_result = 0;

	bit_num1 = byte * 8 + bit;
	bit_num2 = (byte + 1) * 8 + bit;

	for (i = 0; i < 8; ++i) {
		tmp = pddrt->dq_dqs_level[i];
		ddrt_bit_set_dq(pddrt->wrdq_val, tmp, bit, byte);
		ddrt_bit_set_dq(pddrt->wrdq_val, tmp, bit, byte + 1);
		level = ddrt_bit_wr_level(0, i);
		ddrt_config_ddr(pddrt);

		/* start DDRT 2bit */
		check_result = mutibyte_check_multi_ssn(bit_num1);
		if (check_result & 0x1)
			pddrt->bit_wr_result[bit_num1] |= (1 << level);
		if (check_result & 0x2)
			pddrt->bit_wr_result[bit_num2] |= (1 << level);
		if (check_result == 0)
			break;
	}
	return DDRT_SUCCESS;
}

static int ddrt_bit_travel_rddq(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte)
{
	int i = 0;
	unsigned int bit_num1 = 0, bit_num2 = 0;
	unsigned int tmp = 0;
	unsigned int level = 0;
	unsigned int check_result = 0;

	bit_num1 = byte * 8 + bit;
	bit_num2 = (byte + 1) * 8 + bit;

	for (i = 0; i < 8; ++i) {
		tmp = pddrt->dq_dqs_level[i];
		ddrt_bit_set_dq(pddrt->rddq_val, tmp, bit, byte);
		ddrt_bit_set_dq(pddrt->rddq_val, tmp, bit, byte + 1);
		level = ddrt_bit_wr_level(0, i);
		ddrt_config_ddr(pddrt);

		/* start DDRT bit */
		check_result = mutibyte_check_multi_ssn(bit_num1);
		if (check_result & 0x1)
			pddrt->bit_rd_result[bit_num1] |= (1 << level);
		if (check_result & 0x2)
			pddrt->bit_rd_result[bit_num2] |= (1 << level);
		if (check_result == 0)
			break;
	}
	return DDRT_SUCCESS;
}

static int ddrt_bit_travel_wrdqs(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte)
{
	unsigned int i = 0, j = 0;
	unsigned int tmp = 0;
	unsigned int bit_num1 = 0, bit_num2;
	unsigned int level = 0;
	unsigned int check_result = 0;

	bit_num1 = byte * 8 + bit;
	bit_num2 = (byte + 1) * 8 + bit;

	for (i = 1; i < 8; i++) {
		tmp =  pddrt->dq_dqs_level[i];

		for (j = 0; j < DDRT_BYTE_NR; ++j) {
			ddrt_byte_set_dq(pddrt->wrdq_val, tmp, j);
			pddrt->wrdqs_val = ddrt_bit_set_dqs(pddrt->wrdqs_val,
					tmp, j, 0);
		}

		tmp = pddrt->dq_dqs_level[0];
		for (j = 0; j < DDRT_BYTE_NR; ++j)
			ddrt_bit_set_dq(pddrt->wrdq_val, tmp, bit, j);

		level = ddrt_bit_wr_level(i, 0);
		ddrt_config_ddr(pddrt);

		/* start DDRT 2bit */
		check_result = mutibyte_check_multi_ssn(bit_num1);
		if (check_result & 0x1)
			pddrt->bit_wr_result[bit_num1] |= (1 << level);
		if (check_result & 0x2)
			pddrt->bit_wr_result[bit_num2] |= (1 << level);
		if (check_result == 0)
			break;

	}
	return DDRT_SUCCESS;
}

static int ddrt_bit_travel_rddqs(p_ddrt_struct pddrt,
		unsigned int bit, unsigned int byte)
{
	unsigned int i = 0, j = 0;
	unsigned int tmp = 0;
	unsigned int bit_num1 = 0, bit_num2;
	unsigned int level = 0;
	unsigned int check_result = 0;

	bit_num1 = byte * 8 + bit;
	bit_num2 = (byte + 1) * 8 + bit;

	for (i = 1; i < 8; i++) {
		tmp =  pddrt->dq_dqs_level[i];

		for (j = 0; j < DDRT_BYTE_NR; ++j) {
			ddrt_byte_set_dq(pddrt->rddq_val, tmp, j);
			pddrt->rddqs_val = ddrt_bit_set_dqs(pddrt->rddqs_val,
					tmp, j, 0);
		}

		tmp = pddrt->dq_dqs_level[0];
		for (j = 0; j < DDRT_BYTE_NR; ++j)
			ddrt_bit_set_dq(pddrt->rddq_val, tmp, bit, j);

		level = ddrt_bit_wr_level(i, 0);
		ddrt_config_ddr(pddrt);

		/* start DDRT bit */
		check_result = mutibyte_check_multi_ssn(bit_num1);
		if (check_result & 0x1)
			pddrt->bit_rd_result[bit_num1] |= (1 << level);
		if (check_result & 0x2)
			pddrt->bit_rd_result[bit_num2] |= (1 << level);
		if (check_result == 0)
			break;

	}
	return DDRT_SUCCESS;
}

static int ddrt_bit_set_dq(unsigned int *dq, unsigned int val,
		unsigned int bitnum, unsigned int bytenum)
{
	unsigned int i = 0;
	unsigned int j = 0;
	i = bitnum & 0x1;
	j = (bytenum * 4) + (bitnum >> 1);
	if (i == 0)
		dq[j] = (dq[j] & 0xfffffff8) | (val & 0x7);
	else
		dq[j] = (dq[j] & 0xffffffc7) | (val & 0x38);

	return DDRT_SUCCESS;
}


static int ddrt_bit_set_dqs(unsigned int dqs, unsigned int val,
		unsigned int bytenum, unsigned int bytewidth)
{
	if (bytewidth)
		dqs = val;
	else {
		switch (bytenum) {
		case 0:
			dqs = (dqs & 0xfffffff8) | (val & 0x7);
			break;
		case 1:
			dqs = (dqs & 0xffffffc7) | (val & 0x38);
			break;
		default:
			dqs = (dqs & 0xfffffff8) | (val & 0x7);
			break;
		}
	}

	return dqs;
}

static int ddrt_bit_wr_level(unsigned int dqs_level,
		unsigned int dq_level)
{
	unsigned int level = 0;
	if (dqs_level == 0)
		level = 7 - dq_level;
	else
		level = 7 + dqs_level;
	return level;
}

static int ddrt_get_bit_best_para(p_ddrt_struct pddrt, unsigned int byte_nr,
		unsigned int wr_rd)
{
	unsigned int i = 0;
	unsigned int *ptr_best = NULL;
	unsigned int *ptr_result = NULL;
	unsigned int bit_num = 0;

	if (DDRT_TRAINING_WR == wr_rd) {
		ptr_best = pddrt->bit_wr_best;
		ptr_result = pddrt->bit_wr_result;
	} else if (DDRT_TRAINING_RD == wr_rd) {
		ptr_best = pddrt->bit_rd_best;
		ptr_result = pddrt->bit_rd_result;
	} else
		return -DDRT_EFAIL;

	for (i = 0; i < DDRT_BIT_PER_BYTE; ++i) {
		bit_num = byte_nr * DDRT_BIT_PER_BYTE + i;
		ptr_best[bit_num] = ddrt_bit_calculate_para(DDRT_WR_WL_NR,
				ptr_result[bit_num]);
	}

	return DDRT_SUCCESS;
}

static int ddrt_set_bit_best_para(p_ddrt_struct pddrt, unsigned int byte_nr,
		unsigned int wr_rd)
{
	unsigned int tmp = 0;
	unsigned int i = 0;
	unsigned int max_dqs = 0, max_dq = 0, max_result = 0;
	unsigned int dqs = 0, dq = 0;
	unsigned int *ptr_best = NULL;
	unsigned int *dqs_val = 0;
	unsigned int *dq_val = 0;
	unsigned int bit_num = 0;

	if (DDRT_TRAINING_WR == wr_rd) {
		ptr_best = pddrt->bit_wr_best;
		dqs_val = &pddrt->wrdqs_val;
		dq_val = pddrt->wrdq_val;
	} else if (DDRT_TRAINING_RD == wr_rd) {
		ptr_best = pddrt->bit_rd_best;
		dqs_val = &pddrt->rddqs_val;
		dq_val = pddrt->rddq_val;
	} else
		return -DDRT_EFAIL;

	for (i = 0; i < DDRT_BIT_PER_BYTE; i++) {
		bit_num = byte_nr * DDRT_BIT_PER_BYTE + i;
		if (i == 0) {
			tmp = ptr_best[bit_num] & 0xffff;
			max_result = tmp;
		} else {
			tmp = ptr_best[bit_num] & 0xffff;
			if (max_result <= tmp)
				max_result = tmp;
		}
	}
	ddrt_level_spilt(max_result, &max_dqs, &max_dq);
	*dqs_val = ddrt_bit_set_dqs(*dqs_val,
			pddrt->dq_dqs_level[max_dqs], byte_nr, 0);

	for (i = 0; i < DDRT_BIT_PER_BYTE; i++) {
		bit_num = byte_nr * DDRT_BIT_PER_BYTE + i;
		tmp = ptr_best[bit_num] & 0xffff;
		ddrt_level_spilt(tmp, &dqs, &dq);
		if (max_dqs != dqs) {
			dq = dq + (max_result - tmp);
			if (dq > 7)
				dq = 7;
		}
		/*rebuilt wr DQ level*/
		ddrt_bit_set_dq(dq_val, pddrt->dq_dqs_level[dq], i, byte_nr);
	}

	return DDRT_SUCCESS;
}

static int ddrt_bit_calculate_para(unsigned int size, unsigned int val)
{
	unsigned int record_first = 0;
	unsigned int record_cnt = 0;
	unsigned int tmp_first = 0;
	unsigned int tmp_last = 0;
	unsigned int tmp_cnt = 0;
	unsigned int flag = 0;
	unsigned int i = 0;
	unsigned int tmp_result;

	while (i < size) {
		if (flag == 0) {
			if (val & (1 << i)) {
				flag = 1;
				tmp_first = i;
			}
		} else {
			if (!(val & (1 << i))) {
				flag = 0;
				tmp_last = i;
				tmp_cnt = tmp_last - tmp_first;
				if (tmp_cnt > record_cnt) {
					record_first = tmp_first;
					record_cnt = tmp_cnt;
				}
			}
		}
		i++;
		if (i > (size - 1)) {
			if (flag == 1)
				tmp_last = size;
			tmp_cnt = tmp_last - tmp_first;
			if (tmp_cnt > record_cnt) {
				record_first = tmp_first;
				record_cnt = tmp_cnt;
			}
			break;
		}
	}
	if (record_cnt == 0) {
		tmp_result = 0;
		return 0;
	} else {
		tmp_result = record_cnt << 16;
		tmp_result = (tmp_result & 0xffff0000) |
			((record_cnt - 1) / 2 + record_first);
		return tmp_result;
	}
}

static int ddrt_level_spilt(unsigned int level, unsigned int *dqs_level,
		unsigned int *dq_level)
{
	if (level < 8) {
		*dqs_level = 0;
		*dq_level = 7 - level;
	} else {
		*dqs_level = level - 7;
		*dq_level = 0;
	}

	return 0;
}

#define PRINT_PREBIT(level, bit, result, best, dqs, dq, count) do {\
	printf("%-4d", bit);\
	for (k = 0; k < level; k++) {\
		if ((result >> k) & 0x1)\
			printf("%-2s", "-");\
		else\
			printf("%-2s", "X");\
	} \
	printf(" 0x%-12x%-5d%-4d%-4d%-4d\n", result, best, dqs, dq, count);\
} while (0)

#define PRINT_PREBIT_TMP2(level) do {\
	printf("%-4s", "BIT");\
	for (k = 0; k < level; k++)\
		printf("%-2d", k);\
	printf(" %-12s  %-5s%-4s%-4s%-4s\n", "HEX_RESULT", "BEST",\
			"DQS", "DQ", "WIN");\
} while (0)

static int ddrt_print_bit_result(p_ddrt_struct pddrt)
{
	unsigned int i, k;
	const unsigned int count = sizeof(pddrt->bit_wr_result)\
				   / sizeof(unsigned int);
	unsigned int best_tmp, dqs_tmp, dq_tmp, cnt_tmp;
	printf("----------------------------------------------------------\n");
	printf("The window of deskew\n");
	printf("----------------------------------------------------------\n");
	printf("========== Write DQS  DQ  Result ==========\n");
	PRINT_PREBIT_TMP2(DDRT_WR_WL_NR);
	for (i = 0; i < count; i++) {
		cnt_tmp = pddrt->bit_wr_best[i] >> 16;
		best_tmp =  pddrt->bit_wr_best[i] & 0xffff;
		/*ddrt_wrlevel_spilt(best_tmp, &dqs_tmp, &dq_tmp);*/
		ddrt_level_spilt(best_tmp, &dqs_tmp, &dq_tmp);
		PRINT_PREBIT(DDRT_WR_WL_NR, i, pddrt->bit_wr_result[i],
				pddrt->bit_wr_best[i] & 0xffff,
				dqs_tmp, dq_tmp, cnt_tmp);
	}
	printf("========== Read  DQS  DQ  Result ==========\n");
	PRINT_PREBIT_TMP2(DDRT_RD_WL_NR);
	for (i = 0; i < count; i++) {
		cnt_tmp = pddrt->bit_rd_best[i] >> 16;
		best_tmp =  pddrt->bit_rd_best[i] & 0xffff;
		ddrt_level_spilt(best_tmp, &dqs_tmp, &dq_tmp);
		PRINT_PREBIT(DDRT_RD_WL_NR, i, pddrt->bit_rd_result[i],
				pddrt->bit_rd_best[i] & 0xffff,
				dqs_tmp, dq_tmp, cnt_tmp);
	}

	return 0;
}

int ddrt_pressure_test(void)
{
	unsigned int result = 0;

	ddrt_config_ddrt();
	ddrt_start_ddrt();

	do {
		result = ddrt_read(DDRT_BASE_ADDR + DDRT_STATUS);
	} while (0 == (DDRT_DONE & result));

	if (0 == (DDRT_PASS & result))
		return -DDRT_EFAIL;
	else
		return DDRT_SUCCESS;
}
