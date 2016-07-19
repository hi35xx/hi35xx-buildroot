/*
 *  this file only for hi3516a
 */
#include "ddr_training.h"

extern void uart_early_puts(const char *s);
extern void uart_early_put_hex(int hex);

static struct ddrtr_result_t result;

/*****************************************************************************/

static void save_training_result(void)
{
	result.count = 16;

	result.reg[0].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(0));
	result.reg[1].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(0));
	result.reg[2].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(1));
	result.reg[3].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(1));
	result.reg[4].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(2));
	result.reg[5].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(2));
	result.reg[6].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(3));
	result.reg[7].reg = (REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(3));
	result.reg[8].reg = (REG_BASE_DDRPHY + DDRC_DXNDQSGDLY(0));
	result.reg[9].reg = (REG_BASE_DDRPHY + DDRC_DXNDQSGDLY(1));
	result.reg[10].reg = (REG_BASE_DDRPHY + DDRC_DXNDQSGDLY(2));
	result.reg[11].reg = (REG_BASE_DDRPHY + DDRC_DXNDQSGDLY(3));
	result.reg[12].reg = (REG_BASE_DDRPHY + DDRC_DXWDQSDLY(0));
	result.reg[13].reg = (REG_BASE_DDRPHY + DDRC_DXWDQSDLY(1));
	result.reg[14].reg = (REG_BASE_DDRPHY + DDRC_DXWDQSDLY(2));
	result.reg[15].reg = (REG_BASE_DDRPHY + DDRC_DXWDQSDLY(3));
}
/*****************************************************************************/

static void dqs_bdl_enable(void)
{
	unsigned int tmp;

	tmp = io_read(REG_BASE_DDRPHY + DDRC_MISC);
	tmp = tmp | (1<<20);
	io_write(REG_BASE_DDRPHY + DDRC_MISC, tmp);
	tmp = tmp & 0xFFEFFFFF;
	io_write(REG_BASE_DDRPHY + DDRC_MISC, tmp);
}
/*****************************************************************************/

static void config_ddr_dq_wr(struct training_data *training,
			     unsigned int deskew_value,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int val;
	dqindex &= 0x07;

	if (dqindex < 4) {
		val = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(bytenum));
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(bytenum), val);
	} else {
		dqindex -= 4;
		val = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(bytenum));
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(bytenum), val);
	}
	dqs_bdl_enable();
}
/*****************************************************************************/

void memcpy_soc(unsigned int *aim, const unsigned int *src, unsigned int size)
{
	unsigned int i;
	for (i = 0; i < (size >> 2); i++)
		aim[i] = src[i];
}
/*****************************************************************************/

void memset_soc(void *s, char c, int count)
{
	char *xs = (char *) s;

	while (count--)
		*xs++ = c;
}
/*****************************************************************************/
unsigned int  ddrt_read(unsigned int addr)
{
#define DDRT_READ_TIMEOUT	(20)
	int times = 0;
	unsigned int tmp0, tmp1, tmp2;
	do {
		tmp0 = io_read(addr);
		tmp1 = io_read(addr);
		tmp2 = io_read(addr);
		times++;
	} while (((tmp0 != tmp1) || (tmp1 != tmp2))
			&& (times < DDRT_READ_TIMEOUT));
	return tmp0;
}

/*****************************************************************************/
static void config_ddr_dq_rd(struct training_data *training,
			     unsigned int deskew_value,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int val;

	dqindex &= 0x07;

		if (dqindex < 4) {
			val = io_read(REG_BASE_DDRPHY
					+ DDRC_DXNRDQBDL0(bytenum));
			val &= ~(0xFF << (dqindex << 3));
			val |= ((0x1F & deskew_value) << (dqindex << 3));
			io_write(REG_BASE_DDRPHY
					+ DDRC_DXNRDQBDL0(bytenum), val);
		} else {
			dqindex -= 4;
			val = io_read(REG_BASE_DDRPHY
					+ DDRC_DXNRDQBDL1(bytenum));
			val &= ~(0xFF << (dqindex << 3));
			val |= ((0x1F & deskew_value) << (dqindex << 3));
			io_write(REG_BASE_DDRPHY
					+ DDRC_DXNRDQBDL1(bytenum), val);
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

	dq_num =  (bytenum << 3) + dqindex;
	if (dqindex > 3)
		dq_tmp = ((dqindex - 4) << 3);
	else
		dq_tmp = (dqindex << 3);
	io_write(REG_DDRT_BASE_CRG + DDRT_REVERSED_DQ, reversed_bit);
	io_write(REG_DDRT_BASE_CRG + DDRT_OP,
			mode | code_stream | DDRT_START);

	io_write(REG_DDRT_BASE_CRG + DDRT_STATUS, 0);
	do {
		regval = ddrt_read(REG_DDRT_BASE_CRG + DDRT_STATUS);
		times++;
	} while ((!(regval & 0x01)) && (times < DDRT_WAIT_TIMEOUT));

	if (!(regval & 0x02)) {
		ddrt_dq_err_ovfl = ddrt_read(REG_DDRT_BASE_CRG
				+ DDRT_DQ_ERR_OVFL) & (1<<dq_num);
		if (ddrt_dq_err_ovfl)
			return -1;
		ddrt_dq_err_cnt = ddrt_read(REG_DDRT_BASE_CRG
				+ DDRT_DQ_ERR_CNT((bytenum<<1) + (dqindex>>2)));
		ddrt_dq_err_cnt = ddrt_dq_err_cnt & (0xff<<(dq_tmp));

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
#define NUM_TEST_CYCLE      0

#ifdef TRAINING_DEBUG
	uart_early_puts("\r\n Bitnum=");
	uart_early_put_hex(index);
#endif /* TRAINING_DEBUG */

#ifdef SSN_DDRT

	if (ssn_ddrt_init(RW_COMPRARE_MODE, DDRT_PATTERM_PRBS9,
				0, index, bytenum))
		return -1;

	if (ssn_ddrt_init(RW_COMPRARE_MODE, DDRT_PATTERM_PRBS11,
				training->ddrt_reversed_data, index, bytenum))
		return -1;

#endif /* SSN_DDRT */
	return 0;
#undef NUM_TEST_CYCLE
}
/*********************************************************************/

static int perbit_calculate_para(unsigned int size, unsigned int val)
{
	unsigned int i;
	unsigned int rel;
	unsigned int flag;
	unsigned int tmp_cnt;
	unsigned int record_first;
	unsigned int record_cnt;
	unsigned int tmp_first;
	unsigned int tmp_last;

	record_first = record_cnt = tmp_first = tmp_last = tmp_cnt =
		flag = i = rel = 0;

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

	if (!record_cnt) {
		rel = 0;
	} else {
		rel = record_cnt << 16;
		/* if record_cnt is even number, the result choose big number */
		rel = (rel & 0xffff0000) | (((record_cnt) >> 1) + record_first);
	}

	return rel;
}
/*****************************************************************************/

static int wr_bit_deskew_max(struct training_data *training,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int dq_num;
	unsigned int half_bdl;
	unsigned int max_level;
	unsigned int ix;
	unsigned int left_bound;
	unsigned int interrupt_fl;

	dq_num = (bytenum << 3) + dqindex;

	ix = training->wrdq_val[dq_num];
	if (!check_multi_ssn(training, dqindex, bytenum)) {
		interrupt_fl = 1;
		half_bdl = ((WRDQ_LEVEL - ix) >> 1) + ix;
		config_ddr_dq_wr(training, half_bdl, bytenum, dqindex);
		/* ddrt tset is ERROR */
		if (check_multi_ssn(training, dqindex, bytenum))
			max_level = half_bdl + 1;
		else { /* ddrt tset is OK */
			max_level = WRDQ_LEVEL;
			ix = half_bdl;
		}
	} else {
		max_level = WRDQ_LEVEL;
		interrupt_fl = 0;
		training->wr_bit_result[dq_num] &= (~(1<<ix));
	}
	left_bound = ix;
	ix = ix + 1;
	while (ix < max_level) {
		config_ddr_dq_wr(training, ix, bytenum, dqindex);
		/* ddrt tset is OK */
		if (!check_multi_ssn(training, dqindex, bytenum))
			left_bound = ix;
		else {/* ddrt tset is ERROR */
			if (interrupt_fl)
				break;
			training->wr_bit_result[dq_num] &= (~(1<<ix));
		}
		ix++;
	}
	if (interrupt_fl) {
		training->wr_bit_result[dq_num]
			&= (0xffffffff >> (WRDQ_LEVEL - left_bound - 1));

	}
	config_ddr_dq_wr(training, training->wrdq_val[dq_num],
			 bytenum, dqindex);
	return 0;
}
/*****************************************************************************/

static int wr_bit_deskew_min(struct training_data *training,
			     unsigned int bytenum,
			     unsigned int dqindex)
{
	unsigned int ix;
	unsigned int dq_num;
	unsigned int right_bound, half_bdl;
	unsigned int min_level;
	unsigned int interrupt_fl;

	dq_num = (bytenum << 3) + dqindex;
	ix = training->wrdq_val[dq_num];
	if (!check_multi_ssn(training, dqindex, bytenum)) {
		interrupt_fl = 1;
		half_bdl = ix >> 1;
		config_ddr_dq_wr(training, half_bdl, bytenum, dqindex);
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
		training->wr_bit_result[dq_num] &= (~(1<<ix));
	}

	right_bound = ix;
	if (ix != 0)
		ix = ix - 1;

	while (min_level <= ix) {
		config_ddr_dq_wr(training, ix, bytenum, dqindex);
		/* ddrt test is OK */
		if (!check_multi_ssn(training, dqindex, bytenum))
			right_bound = ix;
		else {
			if (interrupt_fl)
				break;
			training->wr_bit_result[dq_num] &= (~(1<<ix));
		}
		if (0 == ix)
			break;
		ix--;
	}
	if (interrupt_fl)
		training->wr_bit_result[dq_num] &= (0xffffffff << right_bound);
	config_ddr_dq_wr(training, training->wrdq_val[dq_num],
			 bytenum, dqindex);

	return 0;
}
/*****************************************************************************/

static int rd_bit_deskew_max(struct training_data *training,
			     unsigned int bytenum,
			     unsigned int dqindex)
{

	unsigned int dq_num;
	unsigned int half_bdl;
	unsigned int max_level;
	unsigned int ix;
	unsigned int left_bound;
	unsigned int interrupt_fl;

	dq_num = (bytenum << 3) + dqindex;
	ix = training->rddq_val[dq_num];
	if (!check_multi_ssn(training, dqindex, bytenum)) {
		interrupt_fl = 1;
		half_bdl = ((RDDQ_LEVEL - ix) >> 1) + ix;
		config_ddr_dq_rd(training, half_bdl, bytenum, dqindex);
		/* ddrt tset is ERROR */
		if (check_multi_ssn(training, dqindex, bytenum))
			max_level = half_bdl + 1;
		else { /* ddrt tset is OK */
			max_level = RDDQ_LEVEL;
			ix = half_bdl;
		}
	} else {
		max_level = RDDQ_LEVEL;
		interrupt_fl = 0;
		training->rd_bit_result[dq_num] &= (~(1<<ix));
	}

	left_bound = ix;
	ix = ix + 1;

	while (ix < max_level) {
		config_ddr_dq_rd(training, ix, bytenum, dqindex);
		/* ddrt tset is OK */
		if (!check_multi_ssn(training, dqindex, bytenum))
			left_bound = ix;

		else { /* ddrt tset is ERROR */
			if (interrupt_fl)
				break;
			training->rd_bit_result[dq_num] &= (~(1<<ix));
		}
		ix++;
	}
	if (interrupt_fl)
		training->rd_bit_result[dq_num]
			&= (0xffffffff >> (RDDQ_LEVEL - left_bound - 1));
	config_ddr_dq_rd(training, training->rddq_val[dq_num],
			 bytenum, dqindex);
	return 0;
}
/*****************************************************************************/

static int rd_bit_deskew_min(struct training_data *training,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int ix;
	unsigned int dq_num;
	unsigned int right_bound, half_bdl;
	unsigned int min_level;
	unsigned int interrupt_fl;
	dq_num = (bytenum << 3) + dqindex;
	ix = training->rddq_val[dq_num];
	if (!check_multi_ssn(training, dqindex, bytenum)) {
		interrupt_fl = 1;
		half_bdl = ix >> 1;
		config_ddr_dq_rd(training, half_bdl, bytenum, dqindex);
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
		training->rd_bit_result[dq_num] &= (~(1<<ix));
	}

	right_bound = ix;
	if (ix != 0)
		ix = ix - 1;

	while (min_level <= ix) {

		config_ddr_dq_rd(training, ix, bytenum, dqindex);
		/* ddrt test is OK */
		if (!check_multi_ssn(training, dqindex, bytenum))
			right_bound = ix;
		else {
			if (interrupt_fl)
				break;
			training->rd_bit_result[dq_num] &= (~(1<<ix));
		}
		if (0 == ix)
			break;
		ix--;

	}
	if (interrupt_fl)
		training->rd_bit_result[dq_num] &= (0xffffffff << right_bound);
	config_ddr_dq_rd(training, training->rddq_val[dq_num],
			 bytenum, dqindex);

	return 0;
}
/*****************************************************************************/
static void read_config_ddr(struct training_data *training)
{
	unsigned int off;
	unsigned int i, regval;

	for (i = 0; i < training->ddr_byte_num; i++) {

		off = (i << 3);
		regval = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQBDL0(i));
		training->wrdq_val[off + 0] = (regval >> 0) & 0xff;
		training->wrdq_val[off + 1] = (regval >> 8) & 0xff;
		training->wrdq_val[off + 2] = (regval >> 16) & 0xff;
		training->wrdq_val[off + 3] = (regval >> 24) & 0xff;

		regval = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQBDL1(i));

		training->wrdq_val[off + 4] = (regval >> 0) & 0xff;
		training->wrdq_val[off + 5] = (regval >> 8) & 0xff;
		training->wrdq_val[off + 6] = (regval >> 16) & 0xff;
		training->wrdq_val[off + 7] = (regval >> 24) & 0xff;

		regval = io_read(REG_BASE_DDRPHY + DDRC_DXNRDQBDL0(i));
		training->rddq_val[off + 0] = (regval >> 0) & 0xff;
		training->rddq_val[off + 1] = (regval >> 8) & 0xff;
		training->rddq_val[off + 2] = (regval >> 16) & 0xff;
		training->rddq_val[off + 3] = (regval >> 24) & 0xff;

		regval = io_read(REG_BASE_DDRPHY + DDRC_DXNRDQBDL1(i));
		training->rddq_val[off + 4] = (regval >> 0) & 0xff;
		training->rddq_val[off + 5] = (regval >> 8) & 0xff;
		training->rddq_val[off + 6] = (regval >> 16) & 0xff;
		training->rddq_val[off + 7] = (regval >> 24) & 0xff;
	}

	for (i = 0; i < BYTEWIDTH; i++) {
		regval = io_read(REG_BASE_DDRPHY + DDRC_DXNWDQDLY(i));
		training->wrdqs_val[i] = (regval >> 8) & 0x1f;
	}

	for (i = 0; i < BYTEWIDTH; i++) {
		regval = io_read(REG_BASE_DDRPHY + DDRC_DXNRDQSDLY(i));
		training->rddqs_val[i] = regval & 0x7f;
	}
}
/*****************************************************************************/
static void wr_byte_deskew(struct training_data *training, unsigned int byte_index)
{
	unsigned int dq_num;
	unsigned int loop_times = 0;
	unsigned int cnt_num, dm_sum;
	unsigned int tmp;
	int i;

	dm_sum = 0;
	for (i = 0; i < 8; i++) {
		dq_num = (byte_index << 3) + i;
		training->wr_bit_result[dq_num] = 0xffffffff;

		wr_bit_deskew_max(training, byte_index, i);
		wr_bit_deskew_min(training, byte_index, i);
		training->wr_bit_best[dq_num]
			= perbit_calculate_para(WRDQ_LEVEL,
					training->wr_bit_result[dq_num]);

		cnt_num = training->wr_bit_best[dq_num] >> 16;
		if (cnt_num > 9)
			tmp = training->wr_bit_best[dq_num] & 0xffff;
		else {
			if (loop_times < DDRT_LOOP_TIMES_LMT) {
				loop_times++;
				i--;
				continue;
			} else
				tmp = training->wrdq_val[dq_num];
		}


		loop_times = 0;

		config_ddr_dq_wr(training, tmp, byte_index, i);
		dm_sum = dm_sum + tmp;
	}
	dm_sum = dm_sum >> 3;
	io_write(REG_BASE_DDRPHY + DDRC_DXNWDQBDL2(byte_index), dm_sum);
	dqs_bdl_enable();
}
/*****************************************************************************/
static void rd_byte_deskew(struct training_data *training, unsigned int byte_index)
{
	unsigned int dq_num;
	unsigned int loop_times = 0;
	unsigned int cnt_num;
	unsigned int tmp;
	int j;
	for (j = 0; j < 8; j++) {
		dq_num = (byte_index << 3) + j;
		training->rd_bit_result[dq_num] = 0xffffffff;
		rd_bit_deskew_max(training, byte_index, j);
		rd_bit_deskew_min(training, byte_index, j);

		training->rd_bit_best[dq_num] =
			perbit_calculate_para(RDDQ_LEVEL,
					training->rd_bit_result[dq_num]);
		cnt_num = training->rd_bit_best[dq_num]>>16;

		if (cnt_num > 9)
			tmp = training->rd_bit_best[dq_num] & 0xffff;
		else {
			if (loop_times < DDRT_LOOP_TIMES_LMT) {
				loop_times++;
				j--;
				continue;
			} else
				tmp = training->rddq_val[dq_num];
		}

		loop_times = 0;

		config_ddr_dq_rd(training, tmp, byte_index, j);
	}

}
/*****************************************************************************/
unsigned int byte_deskew_judge(struct training_data *training,
		unsigned int byte_index, unsigned int mode)
{
	unsigned int left_bound_overnum = 0;
	unsigned int right_bound_overnum = 0;
	unsigned int i;


	for (i = (byte_index << 3); i < ((byte_index + 1) << 3); i++) {
		if (mode) {
			if (training->wr_bit_result[i] & 0x2)
				left_bound_overnum++;
			if (training->wr_bit_result[i] & 0x40000000)
				right_bound_overnum++;
		} else {
			if (training->rd_bit_result[i] & 0x2)
				left_bound_overnum++;
			if (training->rd_bit_result[i] & 0x40000000)
				right_bound_overnum++;
		}

	}
	if ((left_bound_overnum >= 4) && (right_bound_overnum < 1))
		return DQPHASE_DECREASE;
	if ((left_bound_overnum < 4) && (right_bound_overnum >= 1))
		return DQPHASE_INCREASE;
	return 0;

}
/*****************************************************************************/
static void ddr_training_entry(void)
{
	int i;
	struct training_data tmp_result;
	struct training_data *training = &tmp_result;

	/* Open DDRT clock and enable DDRT function */
	io_write(DDRT_TEST_CLK, io_read(DDRT_TEST_CLK) | 0x8);
	io_write(DDRT_ENABLE, io_read(DDRT_ENABLE) | 0x4);

	training->ddrt_reversed_data = io_read(REG_DDRT1);
	training->ddr_byte_num = (io_read(DDRC_CFG_DDRMODE) >> 4) & 0x3;

	io_write(REG_DDRT_BASE_CRG + DDRT_MEM_CONFIG,
		((training->ddr_byte_num - 1) << 12) + (0x152 & 0xfff));

	training->ddr_byte_num  = training->ddr_byte_num << 1;
	training->ddr_bit_num  = training->ddr_byte_num << 3;
	training->flag = DDR_DQSTRAIN;

	io_write(REG_DDRT_BASE_CRG + DDRT_START_ADDR, DDRT_TRAINING_ADDR);
	io_write(REG_DDRT_BASE_CRG + DDRT_BURST_TRANSFER, 0x4f);
	io_write(REG_DDRT_BASE_CRG + DDRT_ADDR_NUM, 0xffffffff);

	io_write(REG_DDRT_BASE_CRG + DDRT_LOOP_NUM, 0);
	io_write(REG_DDRT_BASE_CRG + DDRT_ADDR, DDRT_TRAINING_ADDR);
	io_write(REG_DDRT_BASE_CRG + DDRT_BURST_NUM, BURST_NUM);

	/*** init result ***/
	for (i = 0; i < training->ddr_byte_num<<3; i++) {
		training->wr_bit_result[i] = 0;
		training->rd_bit_result[i] = 0;
		training->wr_bit_best[i] = 0;
		training->rd_bit_best[i] = 0;
	}

	read_config_ddr(training); /* read default configure */

#ifdef WRITE_DESKEW
	uart_early_puts("\r\n");
	uart_early_puts("write data eye training start... ...\r\n");

	for (i = 0; i < training->ddr_byte_num; i++)
		wr_byte_deskew(training, i);


#endif

#ifdef READ_DESKEW
	uart_early_puts("\r\n");
	uart_early_puts("read data eye training start... ...\r\n");
	for (i = 0; i < training->ddr_byte_num; i++)
		rd_byte_deskew(training, i);
#endif

	uart_early_puts("\r\n");

	memcpy_soc((unsigned int *)result.data,
		(unsigned int *)training, sizeof(*training));
	save_training_result();

}

/*****************************************************************************/
/* this function will set value to ddr register. */
struct ddrtr_result_t *ddr_training(struct ddrtr_param_t *param)
{
	if (param->cmd == DDRTR_PARAM_TRAINING) {
		uart_early_puts("DDR training start ");
		ddr_training_entry();
	} else if (param->cmd == DDRTR_PARAM_PRESSURE) {
		uart_early_puts("DDR pressure no support ");
		uart_early_puts("\r\n");
	} else if (param->cmd == DDRTR_PARAM_ADDRTRAIN) {
		uart_early_puts("DDR addrtrain start ");
		uart_early_puts("\r\n");
	}

	return &result;
}
/*****************************************************************************/
