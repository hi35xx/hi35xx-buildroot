/*
 *
 */
#include <config.h>
#include "ddr_dataeye_training.h"

#ifdef CONFIG_DDR_DATAEYE_TRAINING_STARTUP

extern void ddr_training_print(int type, int num);

/*****************************************************************************/
unsigned int ddrt_read(unsigned int addr)
{
#define DDRT_READ_TIMEOUT	(20)
	int times = 0;
	unsigned int data0, data1, data2;
	do {
		data0 = io_read(addr);
		data1 = io_read(addr);
		data2 = io_read(addr);
		times++;
	} while (((data0 != data1) || (data1 != data2))
			&& (times < DDRT_READ_TIMEOUT));

	if (times >= DDRT_READ_TIMEOUT)
		io_write(DDR_TRAINING_STAT_REG, io_read(DDR_TRAINING_STAT_REG)
			| (1 << DDR_TRAINING_STAT_BITS_RD));

	return data0;
}
/*****************************************************************************/

static void dqs_bdl_enable(void)
{
	unsigned int tmp;

	tmp = io_read(REG_BASE_DDRPHY + DDRC_MISC);
	tmp = tmp | (1 << 20);
	io_write(REG_BASE_DDRPHY + DDRC_MISC, tmp);
	tmp = tmp & 0xFFEFFFFF;
	io_write(REG_BASE_DDRPHY + DDRC_MISC, tmp);
	tmp = io_read(REG_BASE_DDRPHY + DDRC_PHYINITCTRL);
	tmp = tmp | (1 << 15);
	io_write(REG_BASE_DDRPHY + DDRC_PHYINITCTRL, tmp);
	tmp = tmp & 0xffff7fff;
	io_write(REG_BASE_DDRPHY + DDRC_PHYINITCTRL, tmp);
}
/*****************************************************************************/

static void config_ddr_dq(struct training_data *training,
		unsigned int deskew_value, unsigned int bytenum,
		unsigned int dqindex, unsigned int mode)
{
	unsigned int val;
	unsigned int ddrc_offest0, ddrc_offest1;
	dqindex &= 0x07;
	if (mode == WR_MODE) { /*read*/
		ddrc_offest0 = DDRC_DXNWDQBDL0(bytenum);
		ddrc_offest1 = DDRC_DXNWDQBDL1(bytenum);
	} else {
		ddrc_offest0 = DDRC_DXNRDQBDL0(bytenum);
		ddrc_offest1 = DDRC_DXNRDQBDL1(bytenum);
	}

	if (dqindex < 4) {
		val = io_read(REG_BASE_DDRPHY + ddrc_offest0);
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_BASE_DDRPHY + ddrc_offest0, val);
	} else {
		dqindex -= 4;
		val = io_read(REG_BASE_DDRPHY + ddrc_offest1);
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_BASE_DDRPHY + ddrc_offest1, val);
	}

	dqs_bdl_enable();
}
/************************* DDRT init *****************************************/
static int ssn_ddrt_init(int mode, int code_stream, unsigned int reversed_bit,
			unsigned int dqindex, unsigned int bytenum)
{
#define DDRT_WAIT_TIMEOUT	(400)
	int times = 0;
	unsigned int regval;
	unsigned int ddrt_dq_err_ovfl;
	unsigned int ddrt_dq_err_cnt;
	unsigned int dq_num, dq_tmp;

	dq_num = (bytenum << 3) + dqindex;

	if (dqindex > 3)
		dq_tmp = (dqindex-4)<<3;
	else
		dq_tmp = dqindex<<3;

	io_write(REG_DDRT_BASE_CRG + DDRT_REVERSED_DQ, reversed_bit);
	io_write(REG_DDRT_BASE_CRG + DDRT_OP,
			mode | code_stream | DDRT_START);
	io_write(REG_DDRT_BASE_CRG + DDRT_STATUS, 0);
	do {
		regval = io_read(REG_DDRT_BASE_CRG + DDRT_STATUS);
		times++;
	} while ((!(regval & 0x01)) && (times < DDRT_WAIT_TIMEOUT));

	if (times >= DDRT_WAIT_TIMEOUT) {
		io_write(DDR_TRAINING_STAT_REG, io_read(DDR_TRAINING_STAT_REG)
			| (1<<DDR_TRAINING_STAT_BITS_WAIT));
		return -1;
	}

	if (!(regval & 0x02)) {
		ddrt_dq_err_ovfl = io_read(REG_DDRT_BASE_CRG
				+ DDRT_DQ_ERR_OVFL) & (1<<dq_num);
		if (ddrt_dq_err_ovfl)
			return -1;
		ddrt_dq_err_cnt = ddrt_read(REG_DDRT_BASE_CRG
				+ DDRT_DQ_ERR_CNT((bytenum<<1) + (dqindex>>2)));
		ddrt_dq_err_cnt = ddrt_dq_err_cnt & (0xff<<dq_tmp);
		if (ddrt_dq_err_cnt)
			return -1;
	}
	return 0;

}

/*****************************************************************************/
/** success return non-negative, fail return negative. **/
static unsigned int check_multi_ssn(const struct training_data *training,
				    unsigned int index, unsigned int bytenum)
{

#ifdef SSN_DDRT

	if (ssn_ddrt_init(RW_COMPRARE_MODE, DDRT_PATTERM_PRBS9,
				0, index, bytenum))
		return -1;

	if (ssn_ddrt_init(RW_COMPRARE_MODE, DDRT_PATTERM_PRBS11,
				training->ddrt_reversed_data, index, bytenum))
		return -1;

#endif /* SSN_DDRT */


	return 0;
}
/*********************************************************************/

static int perbit_calculate_para(unsigned int size, unsigned int val)
{
	unsigned int i;
	unsigned int flag;
	unsigned int tmp_cnt;
	unsigned int record_first;
	unsigned int record_cnt;
	unsigned int tmp_first;
	unsigned int tmp_last;

	record_first = record_cnt = tmp_first = tmp_last = tmp_cnt =
		flag = i = 0;

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

	if (record_cnt) {
		int rel = (record_cnt << 16);
		/* if record_cnt is even number, the result choose big number */
		rel = ((rel & 0xffff0000) | ((record_cnt >> 1) + record_first));
		return rel;
	}

	return 0;
}

/*****************************************************************************/

static int ddr_bit_deskew_max(struct training_data *training,
		unsigned int bytenum, unsigned int dqindex, unsigned int mode)
{
	unsigned int dq_num;
	unsigned int half_bdl;
	unsigned int max_level;
	unsigned int ix;
	unsigned int left_bound;
	unsigned int interrupt_fl;
	unsigned int default_val;

	dq_num = (bytenum << 3) + dqindex;
	if (mode == WR_MODE) {
		ix = training->wrdq_val[dq_num];
		default_val = training->wrdq_val[dq_num];
	} else {
		ix = training->rddq_val[dq_num];
		default_val = training->rddq_val[dq_num];
	}

	if (!check_multi_ssn(training, dqindex, bytenum)) {
		interrupt_fl = 1;
		half_bdl = ((DQ_LEVEL - ix) >> 1) + ix;
		config_ddr_dq(training, half_bdl, bytenum, dqindex, mode);
		/* ddrt test is ERROR */
		if (check_multi_ssn(training, dqindex, bytenum))
			max_level = half_bdl + 1;
		else { /* ddrt tset is OK */
			max_level = DQ_LEVEL;
			ix = half_bdl;
		}
	} else {
		max_level = DQ_LEVEL;
		interrupt_fl = 0;
		training->ddr_bit_result[dq_num] &= (~(1<<ix));
	}

	left_bound = ix;
	ix = ix + 1;
	while (ix < max_level) {
		config_ddr_dq(training, ix, bytenum, dqindex, mode);
		/* ddrt test is OK */
		if (!check_multi_ssn(training, dqindex, bytenum))
			left_bound = ix;
		else {/* ddrt tset is ERROR */
			if (interrupt_fl)
				break;
			training->ddr_bit_result[dq_num] &= (~(1<<ix));
		}
		ix++;
	}

	if (interrupt_fl)
		training->ddr_bit_result[dq_num]
			&= (0xffffffff >> (DQ_LEVEL-left_bound - 1));

	config_ddr_dq(training, default_val, bytenum, dqindex, mode);

	return 0;
}

/*****************************************************************************/
static int ddr_bit_deskew_min(struct training_data *training,
		unsigned int bytenum, unsigned int dqindex, unsigned int mode)
{
	unsigned int ix;
	unsigned int dq_num;
	unsigned int right_bound, half_bdl;
	unsigned int min_level;
	unsigned int interrupt_fl;
	unsigned int default_val;

	dq_num = (bytenum << 3) + dqindex;
	if (mode == WR_MODE) {
		ix = training->wrdq_val[dq_num];
		default_val = training->wrdq_val[dq_num];
	} else {
		ix = training->rddq_val[dq_num];
		default_val = training->rddq_val[dq_num];
	}
	if (!check_multi_ssn(training, dqindex, bytenum)) {
		interrupt_fl = 1;
		half_bdl = ix >> 1;
		config_ddr_dq(training, half_bdl, bytenum, dqindex, mode);
		/* ddrt test is ERROR */
		if (check_multi_ssn(training, dqindex, bytenum))
			min_level = half_bdl;
		else {
			min_level = 0;
			ix = half_bdl;
		}
	} else {
		min_level = 0;
		interrupt_fl = 0;
		training->ddr_bit_result[dq_num] &= (~(1<<ix));
	}

	right_bound = ix;
	if (ix != 0)
		ix = ix - 1;

	while (min_level <= ix) {
		config_ddr_dq(training, ix, bytenum, dqindex, mode);
		/* ddrt test is OK */
		if (!check_multi_ssn(training, dqindex, bytenum))
			right_bound = ix;
		else {
			if (interrupt_fl)
				break;
			training->ddr_bit_result[dq_num] &= (~(1<<ix));
		}
		if (0 == ix)
				break;
		ix--;

	}
	if (interrupt_fl)
		training->ddr_bit_result[dq_num] &= (0xffffffff << right_bound);

	config_ddr_dq(training, default_val, bytenum, dqindex, mode);

	return 0;
}

/*****************************************************************************/

static void read_config_ddr(struct training_data *training)
{
	unsigned int off;
	unsigned int i, a1, a2, a3, a4, jx;

	for (i = 0; i < training->ddr_byte_num; i++) {

		off = (i << 3);

		a1 = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(i));
		a2 = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(i));
		a3 = io_read(REG_BASE_DDRPHY + DDRC_DXNRDQBDL0(i));
		a4 = io_read(REG_BASE_DDRPHY + DDRC_DXNRDQBDL1(i));

		for (jx = 0; jx < 4; jx++) {
			training->wrdq_val[off + jx] = (a1 >> (jx << 3)) & 0xff;
			training->wrdq_val[off + 4 + jx]
				= (a2 >> (jx << 3)) & 0xff;
			training->rddq_val[off + jx] = (a3 >> (jx << 3)) & 0xff;
			training->rddq_val[off + 4 + jx]
				= (a4 >> (jx << 3)) & 0xff;
		}
	}
}

/***************************************************************************/
static void ddr_wl_adj(struct training_data *training)
{
	unsigned int i, j;
	unsigned int wdqs_dly;
	unsigned int wdq_phase;
	unsigned int adj_value;
	unsigned int dm_value;

	for (i = 0; i < training->ddr_byte_num; i++) {
		wdqs_dly = io_read(REG_BASE_DDRPHY + DDRC_DXWDQSDLY(i));
		wdq_phase = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQDLY(i));
		wdqs_dly = wdqs_dly & 0x1f;
		adj_value = wdqs_dly >> 2;
		wdq_phase = (wdq_phase >> 8) + adj_value;

		if (wdq_phase > 0x1f)
			wdq_phase = 0x1f;

		wdq_phase = wdq_phase << 8;
		io_write(REG_BASE_DDRPHY + DDRC_DXNWDQDLY(i), wdq_phase);

		dm_value = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQBDL2(i));
		if (dm_value >= (adj_value << 2))
			dm_value -= (adj_value << 2);
		else
			dm_value = 0;
		io_write(REG_BASE_DDRPHY + DDRC_DXNWDQBDL2(i), dm_value);

		for (j = 0; j < 8; j++) {
			if (training->wrdq_val[(i << 3) + j] > (adj_value << 2))
				training->wrdq_val[(i << 3) + j]
					-= (adj_value << 2);
			else
				training->wrdq_val[(i << 3) + j] = 0;
			config_ddr_dq(training,
					training->wrdq_val[(i << 3) + j],
					i, j, WR_MODE);
			}

		dqs_bdl_enable();

	}
}

/*****************************************************************************/
static void ddr_rdbdl_adj(struct training_data *training)
{
	unsigned int  i, j;
	unsigned int  rdqs_bdl;
	unsigned int  dq_index;
	unsigned int  bdl_offest;
	unsigned int dqs_bound;

	for (i = 0; i < training->ddr_byte_num; i++) {
		dqs_bound = io_read(REG_BASE_DDRPHY + DDRC_DXNRDBOUND(i));
		bdl_offest = (dqs_bound & 0x7f) - ((dqs_bound>>16) & 0x7f);
		bdl_offest = bdl_offest>>1;
		if (bdl_offest >= RDDQS_OFFEST)
			bdl_offest = bdl_offest - RDDQS_OFFEST;

		rdqs_bdl = (io_read(REG_BASE_DDRPHY
				+ DDRC_DXNRDQSDLY(i)) & 0x7f) + bdl_offest;

		if (rdqs_bdl >= RDDQS_LEVEL)
			rdqs_bdl = RDDQS_LEVEL-1;
		io_write(REG_BASE_DDRPHY + DDRC_DXNRDQSDLY(i), rdqs_bdl);

		for (j = 0; j < 8; j++) {
			dq_index = (i<<3) + j;
			training->rddq_val[dq_index]
				= training->rddq_val[dq_index] + bdl_offest;
			if (training->rddq_val[dq_index] >=  RDDQ_LEVEL)
				training->rddq_val[dq_index] = RDDQ_LEVEL - 1;
			config_ddr_dq(training, training->rddq_val[dq_index],
					i, j, RD_MODE);
		}

		dqs_bdl_enable();

	}
}
/*****************************************************************************/
static unsigned int ddr_byte_deskew(struct training_data *training,
		unsigned int byte_index, unsigned int mode)
{
	unsigned int dq_num;
	unsigned int loop_times = 0;
	unsigned int cnt_num, dm_sum;
	unsigned int tmp;
	unsigned int result = 0;
	int i;

	dm_sum = 0;
	for (i = 0; i < 8; i++) {
		dq_num = (byte_index << 3) + i;
		training->ddr_bit_result[dq_num] = 0xffffffff;
		training->ddr_bit_best[dq_num] = 0;
		ddr_bit_deskew_max(training, byte_index, i, mode);
		ddr_bit_deskew_min(training, byte_index, i, mode);
		training->ddr_bit_best[dq_num] =
			perbit_calculate_para(DQ_LEVEL,
			training->ddr_bit_result[dq_num]);
		cnt_num = training->ddr_bit_best[dq_num] >> 16;
		if (cnt_num >= 12)
			tmp = training->ddr_bit_best[dq_num] & 0xffff;
		else {
			if (loop_times < DDRT_LOOP_TIMES_LMT) {
				loop_times++;
				i--;
				continue;
			} else {
				tmp = training->ddr_bit_best[dq_num] & 0xffff;
				if (cnt_num == 0) {
					training->hw_training_flag = 1;
					result = dq_num<<16 | (0x5 + mode);
					return result;
				}
				io_write(DDR_TRAINING_STAT_REG,
					io_read(DDR_TRAINING_STAT_REG)
					| (1<<DDR_TRAINING_STAT_BITS_WS));
			}
		}
		loop_times = 0;
		config_ddr_dq(training, tmp, byte_index, i, mode);
		dm_sum = dm_sum + tmp;
	}
	if (mode == WR_MODE) {
		dm_sum = dm_sum >> 3;
		io_write(REG_BASE_DDRPHY + DDRC_DXNWDQBDL2(byte_index), dm_sum);
	}
	dqs_bdl_enable();

	return 0;
}
/*****************************************************************************/

unsigned int  byte_deskew_judge(struct training_data *training,
		unsigned int byte_index)
{
	unsigned int left_bound_overnum = 0;
	unsigned int right_bound_overnum = 0;
	unsigned int i;

	for (i = (byte_index<<3); i < ((byte_index+1)<<3); i++) {

		if (training->ddr_bit_result[i] & 0x2)
			left_bound_overnum++;
		if (training->ddr_bit_result[i] & 0x40000000)
			right_bound_overnum++;

	}
	if ((left_bound_overnum >= 4) && (right_bound_overnum < 1))
		return DQPHASE_DECREASE;
	if ((left_bound_overnum < 4) && (right_bound_overnum >= 1))
		return DQPHASE_INCREASE;
	return 0;
}
/*****************************************************************************/
static void hw_training(void)
{
	/* gating init */
	unsigned int result;
	io_write(REG_BASE_DDRPHY + 0x21c, 0);
	io_write(REG_BASE_DDRPHY + 0x220, 0);
	io_write(REG_BASE_DDRPHY + 0x29c, 0);
	io_write(REG_BASE_DDRPHY + 0x2a0, 0);
	io_write(REG_BASE_DDRPHY + 0x31c, 0);
	io_write(REG_BASE_DDRPHY + 0x320, 0);
	io_write(REG_BASE_DDRPHY + 0x39c, 0);
	io_write(REG_BASE_DDRPHY + 0x3a0, 0);

	io_write(REG_BASE_DDRPHY + 0x22c, 0);
	io_write(REG_BASE_DDRPHY + 0x2ac, 0);
	io_write(REG_BASE_DDRPHY + 0x32c, 0);
	io_write(REG_BASE_DDRPHY + 0x3ac, 0);
	/* hw read data eye training enable */
	io_write(REG_BASE_DDRPHY + 0x4, 0x105);

	while ((io_read(REG_BASE_DDRPHY + 0x4) & 0xffff))
		;
	if (io_read(REG_BASE_DDRPHY + 0x8) & 0x100) {
		result = 0x4;
		ddr_training_print(result, 0);
	}

}
static void ddr_training_entry(void)
{
	int i;
	unsigned int byte_deskew_flag, last_byte_deskew_flag;
	unsigned int first_deskew_flag;
	unsigned int rd_dqsbdl;
	struct training_data tmp_result;
	struct training_data *training;
	unsigned int result = 0;

	training = &tmp_result;
	/* Open DDRT clock and enable DDRT function */
	io_write(DDRT_TEST_CLK, io_read(DDRT_TEST_CLK) | 0x8);
	io_write(DDRT_ENABLE, io_read(DDRT_ENABLE) | 0x4);

	training->ddrt_reversed_data = io_read(REG_DDRT1);
	training->hw_training_flag = 0;

	training->ddr_byte_num = (io_read(DDRC_CFG_DDRMODE) >> 4) & 0x3;
	io_write(REG_DDRT_BASE_CRG + DDRT_MEM_CONFIG,
		((training->ddr_byte_num-1) << 12) + (0x152 & 0xfff));
	training->ddr_byte_num  = training->ddr_byte_num  << 1;



	io_write(REG_DDRT_BASE_CRG + DDRT_START_ADDR, DDRT_TRAINING_ADDR);
	io_write(REG_DDRT_BASE_CRG + DDRT_BURST_TRANSFER, 0x4f);
	io_write(REG_DDRT_BASE_CRG + DDRT_ADDR_NUM, 0xffffffff);
	io_write(REG_DDRT_BASE_CRG + DDRT_LOOP_NUM, 0);
	io_write(REG_DDRT_BASE_CRG + DDRT_ADDR, DDRT_TRAINING_ADDR);
	io_write(REG_DDRT_BASE_CRG + DDRT_BURST_NUM, BURST_NUM);

	read_config_ddr(training);
	/* SWL bit4=0 enable;  */
	if (!(io_read(SW_TRAINING_REG) & SWL_TRAINING_MASK))
		ddr_wl_adj(training);
	for (i = 0; i < training->ddr_byte_num; i++)
		if (!training->hw_training_flag)
			/* fine deskew */
			ddr_byte_deskew(training, i, WR_MODE);
	/* read deskew */
	for (i = 0; i < training->ddr_byte_num; i++)
		if (!training->hw_training_flag)
			ddr_byte_deskew(training, i, RD_MODE);


/***********************************************************/
	/* hardware training */
	if (training->hw_training_flag) {

		io_write(DDR_TRAINING_STAT_REG,
			io_read(DDR_TRAINING_STAT_REG)
			&(~(1<<DDR_TRAINING_STAT_BITS_WS)));

		training->hw_training_flag = 0;
		io_write(DDR_TRAINING_STAT_REG, 0);
		hw_training();
		read_config_ddr(training);
		/* write data eye training */
		for (i = 0; i < training->ddr_byte_num; i++)
			/* write deskew */
			result = ddr_byte_deskew(training, i, WR_MODE);

		/* read data eye training */
		if (!(io_read(SW_TRAINING_REG) & HW_RDDESKEW_MASK))
			ddr_rdbdl_adj(training);

		last_byte_deskew_flag = 0;
		first_deskew_flag = 0;
		for (i = 0; i < training->ddr_byte_num; i++) {
			result = ddr_byte_deskew(training, i, RD_MODE);
			byte_deskew_flag = byte_deskew_judge(training, i);

			if (((!first_deskew_flag) && (byte_deskew_flag)) ||
				((first_deskew_flag) && (byte_deskew_flag
					== last_byte_deskew_flag))) {

				last_byte_deskew_flag = byte_deskew_flag;
				first_deskew_flag = 1;
				rd_dqsbdl = io_read(REG_BASE_DDRPHY
						+ DDRC_DXNRDQSDLY(i));
				if ((byte_deskew_flag == DQPHASE_DECREASE)
					&& (rd_dqsbdl < (RDDQS_LEVEL - 4))) {
					rd_dqsbdl += 4;
					goto next;
				}
				if ((byte_deskew_flag == DQPHASE_INCREASE)
						&& (rd_dqsbdl >= 4)) {
					rd_dqsbdl -= 4;
					goto next;
				}
			}
			last_byte_deskew_flag = 0;
			first_deskew_flag = 0;
			continue;
next:
			io_write(REG_BASE_DDRPHY + DDRC_DXNRDQSDLY(i),
					rd_dqsbdl);
			dqs_bdl_enable();
			i--;
		}
	}
/***********************************************************/

	if (result)
		ddr_training_print(result & 0xffff, result >> 16);
}

/*****************************************************************************/
/* this function will set value to ddr register. */
int ddr_dataeye_training(void *param)
{
	int val;
	io_write(DDR_TRAINING_STAT_REG, 0);
	ddr_training_entry();
	val = io_read(DDR_TRAINING_STAT_REG);

	return val;
}
/*****************************************************************************/
#endif /* CONFIG_DDR_DATAEYE_TRAINING_STARTUP */
