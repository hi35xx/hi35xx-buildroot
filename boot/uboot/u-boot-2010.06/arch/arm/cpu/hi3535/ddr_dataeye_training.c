/*
 *  this file only for hi3535
 */
#include <config.h>
#include "ddr_dataeye_training.h"

#ifdef CONFIG_DDR_DATAEYE_TRAINING_STARTUP

#undef DDR_TRAINING_DEBUG
#ifdef DDR_TRAINING_DEBUG
/*
 *  Save the result to 8051 Local RAM, Just for Debug!!!
 *  DDR_TRAINING_DEBUG should not defined in release version!!!
 */
#define DEFINE_RESULT(X) static struct ddrtr_result_t *(X) = 0x88000000;
#endif

/*****************************************************************************/
#ifdef DDR_TRAINING_DEBUG
static void save_training_result(void)
{
	DEFINE_RESULT(result);
	result->count = 16;
	result->reg[0].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL0(0));
	result->reg[1].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL1(0));
	result->reg[2].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL0(1));
	result->reg[3].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL1(1));
	result->reg[4].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL0(2));
	result->reg[5].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL1(2));
	result->reg[6].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL0(3));
	result->reg[7].reg = (REG_DDRC_BASE + DDRC_DXNWDQBDL1(3));
	result->reg[8].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL0(0));
	result->reg[9].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL1(0));
	result->reg[10].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL0(1));
	result->reg[11].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL1(1));
	result->reg[12].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL0(2));
	result->reg[13].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL1(2));
	result->reg[14].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL0(3));
	result->reg[15].reg = (REG_DDRC_BASE + DDRC_DXNRDQBDL1(3));
}

/*****************************************************************************/

static void memcpy_soc(unsigned int *aim, const unsigned int *src,
		       unsigned int size)
{
	unsigned int i;
	for (i = 0; i < (size >> 2); i++)
		aim[i] = src[i];
}
#endif /* DDR_TRAINING_DEBUG */
/*****************************************************************************/

static void dqs_bdl_enable(void)
{
	unsigned int tmp;

	tmp = io_read(REG_DDRC_BASE + DDRC_MISC);
	tmp = tmp | (1 << 20);
	io_write(REG_DDRC_BASE + DDRC_MISC, tmp);
	tmp = tmp & 0xFFEFFFFF;
	io_write(REG_DDRC_BASE + DDRC_MISC, tmp);
}
/*****************************************************************************/

static void config_ddr_dq_wr(struct training_data *training,
			     unsigned int deskew_value,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int val;

	dqindex &= 0x07;

	if (dqindex < 4) {
		val = io_read(REG_DDRC_BASE + DDRC_DXNWDQBDL0(bytenum));
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_DDRC_BASE + DDRC_DXNWDQBDL0(bytenum), val);
	} else {
		dqindex -= 4;
		val = io_read(REG_DDRC_BASE + DDRC_DXNWDQBDL1(bytenum));
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_DDRC_BASE + DDRC_DXNWDQBDL1(bytenum), val);
	}

	dqs_bdl_enable();
}
/*****************************************************************************/

static void config_ddr_dq_rd(struct training_data *training,
			     unsigned int deskew_value,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int val;

	dqindex &= 0x07;

	if (dqindex < 4) {
		val = io_read(REG_DDRC_BASE + DDRC_DXNRDQBDL0(bytenum));
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_DDRC_BASE + DDRC_DXNRDQBDL0(bytenum), val);
	} else {
		dqindex -= 4;
		val = io_read(REG_DDRC_BASE + DDRC_DXNRDQBDL1(bytenum));
		val &= ~(0xFF << (dqindex << 3));
		val |= ((0x1F & deskew_value) << (dqindex << 3));
		io_write(REG_DDRC_BASE + DDRC_DXNRDQBDL1(bytenum), val);
	}

	dqs_bdl_enable();
}
/************************* DDRT init *****************************************/

static int ssn_ddrt_init(int mode, int code_stream, unsigned int reversed_bit)
{
	unsigned int regval;
	unsigned int value;

	io_write(REG_DDRT_BASE_CRG + DDRT_REVERSED_DQ, reversed_bit);
	io_write(REG_DDRT_BASE_CRG + DDRT_OP, mode | code_stream | DDRT_START);

	do {
		regval = io_read(REG_DDRT_BASE_CRG + DDRT_STATUS);
	} while (!(regval & 0x01));

	value = (regval & 0x02) ? 0 : -1;
	return value;
}
/*****************************************************************************/
/** success return non-negative, fail return negative. **/
static unsigned int check_multi_ssn(const struct training_data *training,
				    unsigned int index)
{


#ifdef SSN_DDRT

	if (ssn_ddrt_init(RW_COMPRARE_MODE, DDRT_PATTERM_PRBS9, 0))
		return -1;



	if (ssn_ddrt_init(RW_COMPRARE_MODE, DDRT_PATTERM_PRBS11, REVERSED_DATA))
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
static int wr_bit_deskew_max(struct training_data *training,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int dq_num;
	unsigned int half_bdl;
	unsigned int max_level;
	unsigned int ix;
	unsigned int left_bound;

	dq_num = (bytenum << 3) + dqindex;
	ix = training->wrdq_val[dq_num];

	half_bdl = ((WRDQ_LEVEL-ix)>>1) + ix;

	config_ddr_dq_wr(training, half_bdl, bytenum, dqindex);

	if (check_multi_ssn(training, dq_num)) /* ddrt tset is ERROR*/
		max_level = half_bdl + 1;
	else { /* ddrt tset is OK*/
		max_level = WRDQ_LEVEL;
		ix = half_bdl;
	}

	left_bound = ix-1;
	while (ix < max_level) {
		config_ddr_dq_wr(training, ix, bytenum, dqindex);
		if (!check_multi_ssn(training, dq_num)) { /* ddrt tset is OK*/
			left_bound = ix;
			ix++;
		} else /* ddrt tset is ERROR*/
			break;
	}
	training->wr_bit_result[dq_num] &=
		(0xffffffff >> (WRDQ_LEVEL-left_bound - 1));
	config_ddr_dq_wr(training, training->wrdq_val[dq_num],
			 bytenum, dqindex);
	return 0;
}
/*****************************************************************************/
static int wr_bit_deskew_min(struct training_data *training,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int ix;
	unsigned int dq_num;
	unsigned int right_bound, half_bdl;
	unsigned int min_level;

	dq_num = (bytenum << 3) + dqindex;
	ix = training->wrdq_val[dq_num];

	half_bdl = ix >> 1;
	config_ddr_dq_wr(training, half_bdl, bytenum, dqindex);
	if (check_multi_ssn(training, dq_num))  /* ddrt test is ERROR*/
		min_level = half_bdl;
	else {
		min_level = 0;
		ix = half_bdl ;
	}

	right_bound = ix+1;

	while (min_level <= ix) {
		config_ddr_dq_wr(training, ix, bytenum, dqindex);

		if (!check_multi_ssn(training, dq_num)) { /* ddrt test is OK*/
			right_bound = ix;
			if (ix == 0)
				break;
			ix--;
		} else
			break;
	}

	training->wr_bit_result[dq_num] &= (0xffffffff << right_bound);
	config_ddr_dq_wr(training, training->wrdq_val[dq_num],
			 bytenum, dqindex);

	return 0;
}
/*****************************************************************************/

static int rd_bit_deskew_max(struct training_data *training,
			     unsigned int bytenum, unsigned int dqindex)
{
	unsigned int dq_num;
	unsigned int half_bdl;
	unsigned int max_level;
	unsigned int ix;
	unsigned int left_bound;

	dq_num = (bytenum << 3) + dqindex;
	ix = training->rddq_val[dq_num];

	half_bdl = ((RDDQ_LEVEL-ix) >> 1) + ix;

	config_ddr_dq_rd(training, half_bdl, bytenum, dqindex);
	if (check_multi_ssn(training, dq_num)) /* ddrt tset is ERROR*/
		max_level = half_bdl + 1;
	else { /* ddrt tset is OK*/
		max_level = RDDQ_LEVEL;
		ix = half_bdl;
	}

	left_bound = ix-1;
	while (ix < max_level) {
		config_ddr_dq_rd(training, ix, bytenum, dqindex);

		if (!check_multi_ssn(training, dq_num)) { /* ddrt tset is OK*/
			left_bound = ix;
			ix++;
		} else /* ddrt tset is ERROR*/
			break;
	}
	training->rd_bit_result[dq_num] &=
		(0xffffffff >> (RDDQ_LEVEL-left_bound - 1));
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

	dq_num = (bytenum << 3) + dqindex;
	ix = training->rddq_val[dq_num];

	half_bdl = ix >> 1;
	config_ddr_dq_rd(training, half_bdl, bytenum, dqindex);

	if (check_multi_ssn(training, dq_num)) /* ddrt test is ERROR*/
		min_level = half_bdl;
	else {
		min_level = 0;
		ix = half_bdl ;
	}

	right_bound = ix+1;

	while (min_level <= ix) {

		config_ddr_dq_rd(training, ix, bytenum, dqindex);
		if (!check_multi_ssn(training, dq_num)) { /* ddrt test is OK*/
			right_bound = ix;
			if (ix == 0)
				break;
			ix--;
		} else
			break;
	}
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

	for (i = 0; i < BYTEWIDTH; i++) {

		off = (i << 3);
		regval = io_read(REG_DDRC_BASE + DDRC_DXNWDQBDL0(i));
		training->wrdq_val[off + 0] = (regval >> 0) & 0xff;
		training->wrdq_val[off + 1] = (regval >> 8) & 0xff;
		training->wrdq_val[off + 2] = (regval >> 16) & 0xff;
		training->wrdq_val[off + 3] = (regval >> 24) & 0xff;

		regval = io_read(REG_DDRC_BASE + DDRC_DXNWDQBDL1(i));
		training->wrdq_val[off + 4] = (regval >> 0) & 0xff;
		training->wrdq_val[off + 5] = (regval >> 8) & 0xff;
		training->wrdq_val[off + 6] = (regval >> 16) & 0xff;
		training->wrdq_val[off + 7] = (regval >> 24) & 0xff;

		regval = io_read(REG_DDRC_BASE + DDRC_DXNRDQBDL0(i));
		training->rddq_val[off + 0] = (regval >> 0) & 0xff;
		training->rddq_val[off + 1] = (regval >> 8) & 0xff;
		training->rddq_val[off + 2] = (regval >> 16) & 0xff;
		training->rddq_val[off + 3] = (regval >> 24) & 0xff;

		regval = io_read(REG_DDRC_BASE + DDRC_DXNRDQBDL1(i));
		training->rddq_val[off + 4] = (regval >> 0) & 0xff;
		training->rddq_val[off + 5] = (regval >> 8) & 0xff;
		training->rddq_val[off + 6] = (regval >> 16) & 0xff;
		training->rddq_val[off + 7] = (regval >> 24) & 0xff;
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

	for (i = 0; i < 4; i++) {
		wdqs_dly = io_read(REG_DDRC_BASE + DDRC_DXWDQSDLY(i));
		wdq_phase = io_read(REG_DDRC_BASE + DDRC_DXNWDQDLY(i));
		wdqs_dly = wdqs_dly & 0x1f;
		adj_value = wdqs_dly >> 2;
		wdq_phase = (wdq_phase >> 8) + adj_value;

		if (wdq_phase > 0x1f)
			wdq_phase = 0x1f;

		wdq_phase = wdq_phase << 8;
		io_write(REG_DDRC_BASE + DDRC_DXNWDQDLY(i), wdq_phase);
		dm_value = io_read(REG_DDRC_BASE + DDRC_DXNWDQBDL2(i));
		dm_value -= adj_value << 2;
		io_write(REG_DDRC_BASE + DDRC_DXNWDQBDL2(i), dm_value);

		for (j = 0; j < 8; j++) {
			training->wrdq_val[(i << 3) + j] -= adj_value << 2;
			config_ddr_dq_wr(training,
					 training->wrdq_val[(i << 3) + j], i,
					 j);
		}

		dqs_bdl_enable();

	}
}

/*****************************************************************************/

static void ddr_training_entry(void)
{
	unsigned int i, j;
	unsigned int dq_num;
	unsigned int tmp;
	unsigned int cnt_num;
	unsigned int dm_sum;
	struct training_data tmp_result;
	struct training_data *training;

	/*Open DDRT clock and enable DDRT function */
	io_write(DDRT_TEST_CLK, io_read(DDRT_TEST_CLK) | 0x8);
	io_write(DDRT_ENABLE, io_read(DDRT_ENABLE) | 0x4);

	io_write(REG_DDRT_BASE_CRG + DDRT_MEM_CONFIG, 0x01142);
	io_write(REG_DDRT_BASE_CRG + DDRT_START_ADDR, 0x80000000);
	io_write(REG_DDRT_BASE_CRG + DDRT_BURST_TRANSFER, 0x4f);
	io_write(REG_DDRT_BASE_CRG + DDRT_ADDR_NUM, 0xffffffff);

	io_write(REG_DDRT_BASE_CRG + DDRT_LOOP_NUM, 0);
	io_write(REG_DDRT_BASE_CRG + DDRT_ADDR, DDRT_TRAINING_ADDR);
	io_write(REG_DDRT_BASE_CRG + DDRT_BURST_NUM, BURST_NUM);


	io_write(REG_DDRT_BASE_CRG + DDRT_KDATA, 0xBC3EB053);
	io_write(REG_DDRT_BASE_CRG + DDRT_DATA0, 0x8A18207F);
	io_write(REG_DDRT_BASE_CRG + DDRT_DATA1, 0x5F2B9A27);
	io_write(REG_DDRT_BASE_CRG + DDRT_DATA2, 0xBDAD9238);
	io_write(REG_DDRT_BASE_CRG + DDRT_DATA3, 0xAA6774B1);

	training = &tmp_result;

	for (i = 0; i < BITWIDTH; i++) {
		training->wr_bit_result[i] = 0xffffffff;
		training->rd_bit_result[i] = 0xffffffff;
		training->wr_bit_best[i] = 0;
		training->rd_bit_best[i] = 0;
	}

	read_config_ddr(training);
	/*SWL bit4=0 enable;  */
	if (!(io_read(SW_TRAINING_REG) & SWL_TRAINING_MASK))
		ddr_wl_adj(training);
#ifdef WRITE_DESKEW
	for (i = 0; i < BYTEWIDTH; i++) {
		/*dq0-dq7*/
		dm_sum = 0;
		for (j = 0; j < 8; j++) {
			dq_num = (i << 3) + j;

			wr_bit_deskew_max(training, i, j);
			wr_bit_deskew_min(training, i, j);

			training->wr_bit_best[dq_num] =
				perbit_calculate_para(WRDQ_LEVEL,
					training->wr_bit_result[dq_num]);

			cnt_num = training->wr_bit_best[dq_num]>>16;
			if (cnt_num > 7)
				tmp = training->wr_bit_best[dq_num] & 0xffff;
			else
				tmp = training->wrdq_val[dq_num];

			config_ddr_dq_wr(training, tmp, i, j);
			dm_sum = dm_sum + tmp;
		}

		dm_sum = dm_sum >> 3;
		io_write(REG_DDRC_BASE + DDRC_DXNWDQBDL2(i), dm_sum);
		dqs_bdl_enable();
	}
#endif

#ifdef READ_DESKEW
	for (i = 0; i < BYTEWIDTH; i++) {
		/* byte */
		for (j = 0; j < 8; j++) {
			dq_num = (i << 3) + j;

			rd_bit_deskew_max(training, i, j);
			rd_bit_deskew_min(training, i, j);

			training->rd_bit_best[dq_num] =
				perbit_calculate_para(RDDQ_LEVEL,
					training->rd_bit_result[dq_num]);
			cnt_num = training->rd_bit_best[dq_num]>>16;

			if (cnt_num > 7)
				tmp = training->rd_bit_best[dq_num] & 0xffff;
			else
				tmp = training->rddq_val[dq_num];

			config_ddr_dq_rd(training, tmp, i, j);
		}
	}
#endif

#ifdef DDR_TRAINING_DEBUG
	DEFINE_RESULT(result);
	memcpy_soc((unsigned int *)result->data,
		(unsigned int *)training, sizeof(*training));
	save_training_result();
#endif
}

/*****************************************************************************/
/* this function will set value to ddr register. */
int ddr_dataeye_training(void *param)
{
	ddr_training_entry();
	return 0;
}
/*****************************************************************************/
#endif /* CONFIG_DDR_DATAEYE_TRAINING_STARTUP */
