#include <common.h>
#include <spi_flash.h>
#include <config.h>
#include "ddrtraining.h"
#include <nand.h>
#include <configs/godarm.h>
#include <asm/io.h>
static int print_result(struct hi_training *);
static int var_init(struct hi_training *);
static int save_args(struct hi_training *);
static int train(struct hi_training *);
static int system_init(struct hi_training *);
static int delay_soc(unsigned int);
static int config_ddr(struct hi_training *);
static int travel_wrdq(struct hi_training *, unsigned int *, unsigned int);
static int travel_wrdqs(struct hi_training *, unsigned int *, unsigned int);
static int travel_wrdqs_dq_disable(struct hi_training *);
static int travel_rddq(struct hi_training *, unsigned int *, unsigned int);
static int travel_rddqs(struct hi_training *, unsigned int *, unsigned int);
static int travel_rddqs_dq_disable(struct hi_training *);
static int get_best_para(struct hi_training *);
static int check_multi_ssn(struct hi_training *, unsigned int bit);
static int check_multi_pressure(unsigned int *, unsigned int *,
		unsigned int *, unsigned int);
static int check_ssn_code(unsigned int , unsigned int *);
static int ddr_training_ssn_save(unsigned int);
/*bit training add begin*/

static int perbit_calculate_para(unsigned int size, unsigned int val);
static int prebit_train(struct hi_training *);
static int prebit_travel_rddqs(struct hi_training *,
		unsigned int, unsigned int);
static int prebit_travel_rddq(struct hi_training *,
		unsigned int, unsigned int);
/*static int prebit_travel_wrdm(struct hi_training *, unsigned int);*/
static int prebit_travel_wrdqs(struct hi_training *,
		unsigned int, unsigned int);
static int prebit_travel_wrdq(struct hi_training *,
		unsigned int, unsigned int);
/*static int prebit_travel_dmdqs(struct hi_training *, unsigned int);*/
static int prebit_travel_disable_dqs(struct hi_training *training,
		unsigned int bit, unsigned int byte);
static int prebit_travel_disable_dq(struct hi_training *training,
		unsigned int bit, unsigned int byte);
static int get_bestprebit_para(struct hi_training *training);
static int read_config_ddr(struct hi_training *);
static int bitset_dq(unsigned int *dq, unsigned int val,
		unsigned int bitnum, unsigned int bytenum);
static int set_dqs(unsigned int dqs, unsigned int val,
		unsigned int bytenum, unsigned int bytewidth);

static int wrlevel_spilt(unsigned int level, unsigned int *dqs_level,
		unsigned int *dq_level);
static int ddr_wrlevel(unsigned int dqs_level, unsigned int dq_level);
static int ddr_rdlevel(unsigned int dqs_level, unsigned int dq_level);
static int level_spilt(unsigned int level, unsigned int *dqs_level,
		unsigned int *dq_level);
static int print_prebit_result(struct hi_training *training);
static int replace_args(struct hi_training *training);

/*bit training add end*/
struct S_CAS_REGS_TYPE;

unsigned int ssn_error_num;
unsigned int ssn_error_code;
unsigned int ssn_error_check;
unsigned int ssn_cycnum;

#undef DDRT_UPLOAD_ENABLE
#undef DDRT_WRITE_FLASH_ENABLE

int ddr_training_entry(unsigned int blank_zone_offset)
{
	struct hi_training *training_ddr0 =
		(struct hi_training *)TRAINING_ADDR;

	unsigned int option = readl(DDRT_OPTION_REG);
	unsigned int tmp = readl(DDRT_OPTION_REG);
	tmp = tmp >> 16;
	option &= 0xffff;
	training_ddr0->blank_zone_offset = blank_zone_offset;

	system_init(training_ddr0);
	training_ddr0->reg_ddr_base = REG_DDR0_BASE;
	training_ddr0->ssn_ddr_base = SSN_DDR0_BASE;
	if (tmp == DDRT_OPTION_CMD) {
		switch (option) {
		case DDRT_BYTE_W:
		case DDRT_BYTE_R:
		case DDRT_BYTE_WR:
			train(training_ddr0);
			break;
		case DDRT_BIT_W:
		case DDRT_BIT_R:
		case DDRT_BIT_WR:
			train(training_ddr0);
			prebit_train(training_ddr0);
			break;
		default:
			return -1;
		}

		return 0;
	} else {
		writel(DDRT_MODE_WR, DDRT_OPTION_REG);
		train(training_ddr0);
		prebit_train(training_ddr0);
	}
	return 0;
}

/****************************************************************************/
int ddr_training_second_step(void)
{
	unsigned int option = readl(DDRT_OPTION_REG);
	unsigned int tmp = readl(DDRT_OPTION_REG);
	tmp = tmp >> 16;
	option = option & 0xffff;
	struct hi_training *ddr0 = (struct hi_training *)TRAINING_ADDR;
	if (tmp == DDRT_OPTION_CMD) {
		var_init(ddr0);
		ddr0->reg_ddr_base = REG_DDR0_BASE;
		if (option >= DDRT_BYTE_R && option <= DDRT_BYTE_WR)
			print_result(ddr0);
		if (option >= DDRT_BIT_R && option <= DDRT_BIT_WR)
			print_prebit_result(ddr0);
		save_args(ddr0);
		writel(0x0, DDRT_OPTION_REG);
		return 0;
	}

	if (_ddrtraining_flag != TRAINING_CHECK_VALUE) {
		printf("DDR0 Training:\n");
		var_init(ddr0);
		ddr0->reg_ddr_base = REG_DDR0_BASE;
		print_result(ddr0);
		print_prebit_result(ddr0);
		save_args(ddr0);
		printf("---------------------------------------\n");
		udelay(10000);
	}
#if 0
	else {
		/*disable read nandflash used AHB*/
		if (ddr0->boot_type == BOOT_FROM_NAND) {
			tmp = io_read(NANDC_REG_BASE);
			tmp |= 0x1;
			io_write(NANDC_REG_BASE, tmp);
		}
	}
#endif
	return 0;
}

static int var_init(struct hi_training *training)
{
	training->wr_data[0] = DQS_OFF_DQ_7;
	training->wr_data[1] = DQS_OFF_DQ_6;
	training->wr_data[2] = DQS_OFF_DQ_5;
	training->wr_data[3] = DQS_OFF_DQ_4;
	training->wr_data[4] = DQS_OFF_DQ_3;
	training->wr_data[5] = DQS_OFF_DQ_2;
	training->wr_data[6] = DQS_OFF_DQ_1;
	training->wr_data[7] = DQS_0_DQ_7;
	training->wr_data[8] = DQS_0_DQ_6;
	training->wr_data[9] = DQS_0_DQ_5;
	training->wr_data[10] = DQS_0_DQ_4;
	training->wr_data[11] = DQS_0_DQ_3;
	training->wr_data[12] = DQS_0_DQ_2;
	training->wr_data[13] = DQS_0_DQ_1;
	training->wr_data[14] = DQS_0_DQ_0;
	training->wr_data[15] = DQS_1_DQ_0;
	training->wr_data[16] = DQS_2_DQ_0;
	training->wr_data[17] = DQS_3_DQ_0;
	training->wr_data[18] = DQS_4_DQ_0;
	training->wr_data[19] = DQS_5_DQ_0;
	training->wr_data[20] = DQS_6_DQ_0;
	training->wr_data[21] = DQS_7_DQ_0;
	training->wr_data[22] = DQS_1_DQ_OFF;
	training->wr_data[23] = DQS_2_DQ_OFF;
	training->wr_data[24] = DQS_3_DQ_OFF;
	training->wr_data[25] = DQS_4_DQ_OFF;
	training->wr_data[26] = DQS_5_DQ_OFF;
	training->wr_data[27] = DQS_6_DQ_OFF;
	training->wr_data[28] = DQS_7_DQ_OFF;

	training->rd_data[0] = DQS_0_DQ_7;
	training->rd_data[1] = DQS_0_DQ_6;
	training->rd_data[2] = DQS_0_DQ_5;
	training->rd_data[3] = DQS_0_DQ_4;
	training->rd_data[4] = DQS_0_DQ_3;
	training->rd_data[5] = DQS_0_DQ_2;
	training->rd_data[6] = DQS_0_DQ_1;
	training->rd_data[7] = DQS_0_DQ_0;
	training->rd_data[8] = DQS_1_DQ_0;
	training->rd_data[9] = DQS_2_DQ_0;
	training->rd_data[10] = DQS_3_DQ_0;
	training->rd_data[11] = DQS_4_DQ_0;
	training->rd_data[12] = DQS_5_DQ_0;
	training->rd_data[13] = DQS_6_DQ_0;
	training->rd_data[14] = DQS_7_DQ_0;

	training->dq_data[0] = DQ_DATA_LEVEL0;
	training->dq_data[1] = DQ_DATA_LEVEL1;
	training->dq_data[2] = DQ_DATA_LEVEL2;
	training->dq_data[3] = DQ_DATA_LEVEL3;
	training->dq_data[4] = DQ_DATA_LEVEL4;
	training->dq_data[5] = DQ_DATA_LEVEL5;
	training->dq_data[6] = DQ_DATA_LEVEL6;
	training->dq_data[7] = DQ_DATA_LEVEL7;

	training->dqs_data[0] = DQS_DATA_LEVEL0;
	training->dqs_data[1] = DQS_DATA_LEVEL1;
	training->dqs_data[2] = DQS_DATA_LEVEL2;
	training->dqs_data[3] = DQS_DATA_LEVEL3;
	training->dqs_data[4] = DQS_DATA_LEVEL4;
	training->dqs_data[5] = DQS_DATA_LEVEL5;
	training->dqs_data[6] = DQS_DATA_LEVEL6;
	training->dqs_data[7] = DQS_DATA_LEVEL7;
	return 0;
}

static int system_init(struct hi_training *training)
{
	unsigned int tmp;

	var_init(training);

	tmp = io_read(REG_BASE_SCTL  + REG_SYSSTAT);
	tmp >>= 4;
	tmp &= 0x3;
	training->boot_type = tmp;
	return 0;
}

/* save the result of training at flash */
static int replace_args(struct hi_training *training)
{
	unsigned int i;
#ifdef DDRT_UPLOAD_ENABLE
	char argv_tmp[128] = {0};
#endif
	struct regentry *reg = (struct regentry *)(
			TRAINING_TMP_BASE + training->blank_zone_offset);
	for (i = 0; ; i++) {
		if ((!reg[i].reg_addr) && (!reg[i].value)
				&& (!reg[i].delay) && (!reg[i].attr))
			break;
		switch (reg[i].reg_addr) {
		case (REG_DDRC_BASE0 + DDRC_LVLSKEW):
			reg[i].value = training->lvlskew_val;
			break;
		case (REG_DDRC_BASE0 + DDRC_WRDQS_SKEW):
			reg[i].value = training->wrdqs_val;
			break;
		case (REG_DDRC_BASE0 + DDRC_RDDQS_SKEW):
			reg[i].value = training->rddqs_val;
			break;
		case (REG_DDRC_BASE0 + DDRC_WRDQSKEW(0)):
			reg[i].value = training->wrdq_val[0];
			break;
		case (REG_DDRC_BASE0 + DDRC_WRDQSKEW(1)):
			reg[i].value = training->wrdq_val[1];
			break;
		case (REG_DDRC_BASE0 + DDRC_WRDQSKEW(2)):
			reg[i].value = training->wrdq_val[2];
			break;
		case (REG_DDRC_BASE0 + DDRC_WRDQSKEW(3)):
			reg[i].value = training->wrdq_val[3];
			break;
		case (REG_DDRC_BASE0 + DDRC_RDDQSKEW(0)):
			reg[i].value = training->rddq_val[0];
			break;
		case (REG_DDRC_BASE0 + DDRC_RDDQSKEW(1)):
			reg[i].value = training->rddq_val[1];
			break;
		case (REG_DDRC_BASE0 + DDRC_RDDQSKEW(2)):
			reg[i].value = training->rddq_val[2];
			break;
		case (REG_DDRC_BASE0 + DDRC_RDDQSKEW(3)):
			reg[i].value = training->rddq_val[3];
			break;
		default:
			break;
		}
	}
#ifdef DDRT_UPLOAD_ENABLE
	if (NULL == getenv(DDRT_UPLOAD_NAME)) {
		sprintf(argv_tmp, "%s 0x%x %s 0x%x", "tftp",
				TRAINING_TMP_BASE + training->blank_zone_offset,
				DDRT_UPLOAD_DEFNAME, DDRT_UPLOAD_DEFLEN);
	} else {
		sprintf(argv_tmp, "%s 0x%x %s 0x%x", "tftp",
				TRAINING_TMP_BASE + training->blank_zone_offset,
				getenv(DDRT_UPLOAD_NAME), DDRT_UPLOAD_DEFLEN);
	}
	run_command(argv_tmp, 0);
#endif
	return 0;
}

static int save_args(struct hi_training *training)
{
	size_t length;
	static struct spi_flash *flash;
	nand_erase_options_t opts;
	memset(&opts, 0, sizeof(struct nand_erase_options));

	length = 0x80000;
	opts.offset = 0;
	opts.length = length;

	if (training->boot_type == BOOT_FROM_SPI) {
		flash = spi_flash_probe(0, 0, 0, 0);
		spi_flash_read(flash, 0x0, length, (void *)TRAINING_TMP_BASE);
		#ifdef DDRT_WRITE_FLASH_ENABLE
		spi_flash_erase(flash, 0x0, length);
		#endif
		io_write(TRAINING_TMP_BASE + _ddrtraining_flag_offset,
				TRAINING_CHECK_VALUE);
		replace_args(training);
		#ifdef DDRT_WRITE_FLASH_ENABLE
		spi_flash_write(flash, 0x0, length,
				(const void *)TRAINING_TMP_BASE);
		#endif
	}
#ifdef BOOT_FROM_NAND
	else if (training->boot_type == BOOT_FROM_NAND) {
		nand_read_skip_bad(&nand_info[nand_curr_device],
				0x0, &length, (u_char *)TRAINING_TMP_BASE);
		io_write(TRAINING_TMP_BASE + _ddrtraining_flag_offset,
				TRAINING_CHECK_VALUE);
		replace_args(training);
		#ifdef DDRT_WRITE_FLASH_ENABLE
		nand_erase_opts(&nand_info[nand_curr_device], &opts);
		nand_write_skip_bad(&nand_info[nand_curr_device],
				0x0, &length, (u_char *)TRAINING_TMP_BASE);
		#endif
	}
#endif

	return 0;
}

/* ddr config */
static int config_ddr(struct hi_training *training)
{
	unsigned int reg_ddr_base;
	reg_ddr_base = training->reg_ddr_base;

	io_write(reg_ddr_base + DDRC_LVLSKEW, training->lvlskew_val);
	io_write(reg_ddr_base + DDRC_WRDQS_SKEW, training->wrdqs_val);
	io_write(reg_ddr_base + DDRC_RDDQS_SKEW, training->rddqs_val);
	io_write(reg_ddr_base + DDRC_WRDQSKEW(0), training->wrdq_val[0]);
	io_write(reg_ddr_base + DDRC_WRDQSKEW(1), training->wrdq_val[1]);
	io_write(reg_ddr_base + DDRC_WRDQSKEW(2), training->wrdq_val[2]);
	io_write(reg_ddr_base + DDRC_WRDQSKEW(3), training->wrdq_val[3]);
	io_write(reg_ddr_base + DDRC_RDDQSKEW(0), training->rddq_val[0]);
	io_write(reg_ddr_base + DDRC_RDDQSKEW(1), training->rddq_val[1]);
	io_write(reg_ddr_base + DDRC_RDDQSKEW(2), training->rddq_val[2]);
	io_write(reg_ddr_base + DDRC_RDDQSKEW(3), training->rddq_val[3]);
	return 0;
}

static void set_dq(unsigned int *dq, unsigned val)
{
	dq[0] = val;
	dq[1] = val;
	dq[2] = val;
	dq[3] = val;
}

static void set_best_skew(struct hi_training *training)
{
	unsigned int tmp;
	unsigned int best_data;

	/* Set best write skew */
	training->lvlskew_val = ENABLE_WR_DM | ENABLE_WR_DQS;
	if (training->wr_best == ALL_ERROR) {
		training->wrdqs_val = training->dqs_data[0];
		tmp = training->dq_data[0];
		set_dq(training->wrdq_val, tmp);

	} else {

		/*
		 * Write DQS and DQ of the best skew are all opened
		 */
		best_data = training->wr_data[training->wr_best];
		training->wrdqs_val = training->dqs_data[best_data >> 16];
		tmp = training->dq_data[best_data & 0xffff];
		set_dq(training->wrdq_val, tmp);

		/*
		 * Write DQS of the best skew is closed
		 */
		if ((best_data >> 16) == 8)
			training->lvlskew_val ^= ENABLE_WR_DQS;

		/*
		 * Write DQ of the best skew is closed
		 */
		if ((best_data & 0xffff) == 8) {
			training->lvlskew_val ^= ENABLE_WR_DM;
			tmp = 0;
			set_dq(training->wrdq_val, tmp);
		}
	}

	/*
	 * Set best read skew
	 */
	if (training->rd_best == ALL_ERROR) {
		training->rddqs_val = training->dqs_data[0];
		tmp = training->dq_data[0];
		set_dq(training->rddq_val, tmp);
	} else {
		best_data = training->rd_data[training->rd_best];
		training->rddqs_val = training->dqs_data[best_data >> 16];
		tmp = training->dq_data[best_data & 0xffff];
		set_dq(training->rddq_val, tmp);
	}
}

/*****************************************************************************/
static int train(struct hi_training *training)
{
	unsigned int i, j, k;
	unsigned int tmp;

	for (i = 0; i < 9; i++) {
		training->wr_result[i] = 0;
		training->rd_result[i] = 0;
	}
	training->skew_cnt = 0;

	/*Training write DQS and DQ*/
	for (i = 0; i < 8; i++) {
		/*Enable Write DQS and DM*/
		training->lvlskew_val = ENABLE_WR_DM | ENABLE_WR_DQS;
		/*Read-DQS be setted as level 'i'*/
		training->rddqs_val = training->dqs_data[i];
		for (j = 0; j < 8; j++) {
			/*Read-DQ be setted as level 'j'*/
			tmp = training->dq_data[j];
			set_dq(training->rddq_val, tmp);
			/*
			 * Write-DQS be setted as level '0' and travel write-DQ.
			 */
			training->wrdqs_val = training->dqs_data[0];
			travel_wrdq(training, training->dq_data, 0);
			/*
			 * Enable Write DQ and Write-DQ be setted as level '0'.
			 * And travel write-DQS.
			 */
			tmp = training->dq_data[0];
			set_dq(training->wrdq_val, tmp);
			travel_wrdqs(training, training->dqs_data, 0);
			/*
			 * Disable Write-DQ and Write-DM,
			 * enable DQS to travel write-DQS.
			 */
			set_dq(training->wrdq_val, DISABLE_WR_DQ);
			training->lvlskew_val = ENABLE_WR_DQS;
			travel_wrdqs(training, training->dqs_data, 8);

			/*
			 * Disable Write-DQS, enable Write-DM,
			 * to travel write-dq
			 */
			training->lvlskew_val = ENABLE_WR_DM;
			travel_wrdq(training, training->dq_data, 8);

			/* If all is failure, continue the circle.*/
			for (k = 0; k < 9; k++)
				if (training->wr_result[k] != 0)
					goto wr_training_end;
		}
	}
wr_training_end:

	/* get the best write parameter */
	get_best_para(training);
	set_best_skew(training);
	/*
	 * Training Read DQS and DQ
	 * Read-DQS be setted as level '0'.
	 */
	training->rddqs_val = training->dqs_data[0];
	travel_rddq(training, training->dq_data, 0);

	/*
	 * Enable DQ and read-DQ be setted as level '0'.
	 * And travel read-DQS.
	 */
	tmp = training->dq_data[0];
	set_dq(training->rddq_val, tmp);
	travel_rddqs(training, training->dqs_data, 0);
	/*Disable DQ and DQS.*/
	set_dq(training->rddq_val, DISABLE_RD_DQ_DQS);
	travel_rddqs_dq_disable(training);

	/* get the best read parameter */
	get_best_para(training);
	set_best_skew(training);
	config_ddr(training);
	return 0;
}

/*****************************************************************************/
static int travel_wrdq(struct hi_training *training,
		unsigned int *dat, unsigned int level)
{
	unsigned int i;
	for (i = 0; i < 8; i++) {
		set_dq(training->wrdq_val, dat[i]);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->wr_result[level] |= (1 << i);
	}
	return 0;
}

static int travel_wrdqs(struct hi_training *training,
		unsigned int *dat, unsigned int level)
{
	unsigned int i;
	for (i = 0; i < 8; i++) {
		training->wrdqs_val = dat[i];
		config_ddr(training);

		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->wr_result[i] |= (1 << level);

	}

	return 0;
}

/*****************************************************************************/
static int travel_rddq(struct hi_training *training,
		unsigned int *dat, unsigned int level)
{
	unsigned int i;
	for (i = 0; i < 8; i++) {
		set_dq(training->rddq_val, dat[i]);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->rd_result[level] |= (1 << i);

	}
	return 0;
}

/*****************************************************************************/
static int travel_rddqs(struct hi_training *training,
		unsigned int *dat, unsigned int level)
{
	unsigned int i;
	for (i = 0; i < 8; i++) {
		training->rddqs_val = dat[i];
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->rd_result[i] |= (1 << level);

	}
	return 0;
}

/*****************************************************************************/
static int travel_wrdqs_dq_disable(struct hi_training *training)
{
	config_ddr(training);
	if (!check_multi_ssn(training, DDRTRAININT_BYTE))
		training->wr_result[8] |= 1 << 8;

	return 0;
}

/*****************************************************************************/
static int travel_rddqs_dq_disable(struct hi_training *training)
{
	config_ddr(training);
	if (!check_multi_ssn(training, DDRTRAININT_BYTE))
		training->rd_result[8] |= 1 << 8;

	return 0;
}

static int delay_soc(unsigned int time)
{
	unsigned int tmp;
	while (time--) {
		tmp = 0x10;
		while (tmp--)
			;
	}
	return 0;
}

static int prs_rebuilt(unsigned char input,
		unsigned int *output, unsigned int *data)
{
	int n;
	unsigned int tmp = input;
	for (n = 6; n >= 0; n -= 2) {
		output[n] = data[tmp & 0x3];
		output[n + 1] = data[tmp & 0x3];
		tmp >>= 2;
	}
	return 0;
}

void memcpy_soc(unsigned int *aim, unsigned int *src, unsigned int size)
{
	unsigned int i;
	unsigned int sizetemp;
	sizetemp = (size >> 2);
	for (i = 0; i < sizetemp; i++)
		*(unsigned int *)(aim + i) = *(unsigned int *)(src + i);

}


static int check_prebit_tmp(const unsigned int code, unsigned int word,
		unsigned int bit_num)
{
	unsigned int bit1, bit2;
	bit1 = bit_num;
	bit2 = bit_num < 8 ? (bit_num + 8) : (bit_num - 8);

	if ((((word >> bit1) & 0x1) == ((code >> bit1) & 0x1))
		&& (((word >> bit2) & 0x1) == ((code >> bit2) & 0x1)))
		return 0;
	else
		return -1;
}

/************************************************************/
static int check_multi_ssn(struct hi_training *training, unsigned int bit)
{

	unsigned int i, j, n, k;
	unsigned int ad_cnt = LOOP_NUM;
	unsigned int ssn_code[BURST_LENGTH];
	unsigned int ddrt_ddr_base = training->ssn_ddr_base;
	unsigned int tmp1, tmp2;
	int ret = 0;
	unsigned int option = readl(DDRT_OPTION_REG);
	option = option & 0xffff;
	DDR_OPTION_SWITCH(option);

	for (n = 0; n < 4; n++) {
		check_ssn_code(n, ssn_code);
		io_write(DDRT_BASE_CLK, 0xa);
		io_write(DDRT_BASE_ADDR + DDRT_INTEN, 1);
		io_write(DDRT_BASE_ADDR + DDRT_MODE, option);
		/*writel(option, 0x20050038);*/
		io_write(DDRT_BASE_ADDR + DDRT_BLKSIZE, 0x001f);
		io_write(DDRT_BASE_ADDR + DDRT_LOOP_NUM, ad_cnt - 1);
		io_write(DDRT_BASE_ADDR + DDRT_WADDR, ddrt_ddr_base);
		io_write(DDRT_BASE_ADDR + DDRT_RADDR, ddrt_ddr_base);
		io_write(DDRT_BASE_ADDR + DDRT_STRIDE, 0X100);

		if (option == DDRT_MODE_WR || option == DDRT_MODE_WRITE) {
			for (j = 0; j < BURST_LENGTH; j++)
				io_write(DDRT_BASE_ADDR + DDRT_DATABUF + j * 4, ssn_code[j]);

			io_write(DDRT_BASE_ADDR + DDRT_INTCLR, 1);
			io_write(DDRT_BASE_ADDR + DDRT_START, 1);
			while (!(io_read(DDRT_BASE_ADDR)))
				;
			io_write(DDRT_BASE_ADDR + DDRT_INTCLR, 1);
			for (k = 0; k < ad_cnt; k++) {
				for (i = 0; i < BURST_LENGTH;) {
					if (bit == DDRTRAININT_BYTE) {
						if (io_read(ddrt_ddr_base + k * 0x200 + i * 4) != ssn_code[i])
							return -1;
					} else {
						tmp1 = io_read(ddrt_ddr_base + k * 0x200 + i * 4);
						if (0 == (bit / 16))
							ret = check_prebit_tmp(ssn_code[i], tmp1, bit);
						else {
							tmp2 = io_read(ddrt_ddr_base + k * 0x200 + (i + 1) * 4);
							ret = check_prebit_tmp(ssn_code[i + 1], tmp2, bit / 16);
							++i;
						}
						if (0 != ret)
							return -1;
					}
					++i;
				}
			}
		} else if (option == DDRT_MODE_READ) {
			for (n = 0; n < ad_cnt; ++n)
				memcpy_soc((unsigned int *)((char *)ddrt_ddr_base + n * BURST_LENGTH * 4),
						ssn_code, BURST_LENGTH * 4);

			for (k = 0; k < ad_cnt; k++) {
					io_write(DDRT_BASE_ADDR + DDRT_INTCLR, 1);
					io_write(DDRT_BASE_ADDR + DDRT_START, 1);
					while (!(io_read(DDRT_BASE_ADDR)))
						;
					io_write(DDRT_BASE_ADDR + DDRT_INTCLR, 1);
					if (bit == DDRTRAININT_BYTE) {
						for (i = 0; i < BURST_LENGTH; i++)
							if (io_read(DDRT_BASE_ADDR + DDRT_DATABUF + i * 4) != ssn_code[i])
								return -1;
					} else {
						for (i = 0; i < BURST_LENGTH; i++)
							if (io_read(DDRT_BASE_ADDR + DDRT_DATABUF + i * 4) != ssn_code[i])
								return -1;
					}
			}
		} else
			return -1;
	}
	return 0;
}

static int check_multi_pressure(unsigned int *ssn_num,
		unsigned int *ssn_code_temp, unsigned int *ssn_check_temp,
		unsigned int option)
{
	unsigned int i = 0, j, n;
	unsigned int ad_cnt = LOOP_NUM + 0x1;
	unsigned int ssn_code[BURST_LENGTH] = {0};
	unsigned int ddr_addr = 0x82000000;
	unsigned int code_tmp;
	const unsigned int loop_num_r = 0x1f;
	for (n = 0; n < 4; n++)
		check_ssn_code(n, ssn_code);
	io_write(DDRT_BASE_CLK, 0xa);
	io_write(DDRT_BASE_ADDR + DDRT_INTEN, 1);
	io_write(DDRT_BASE_ADDR + DDRT_MODE, option);
	io_write(DDRT_BASE_ADDR + DDRT_BLKSIZE, 0x001f);
	io_write(DDRT_BASE_ADDR + DDRT_LOOP_NUM, 0xffe);
	io_write(DDRT_BASE_ADDR + DDRT_WADDR, SSN_DMA_DDR_BASE);
	io_write(DDRT_BASE_ADDR + DDRT_RADDR, ddr_addr);
	io_write(DDRT_BASE_ADDR + DDRT_STRIDE, 0X100);

	if (option == DDRT_MODE_WR || option == DDRT_MODE_WRITE) {
		for (j = 0; j < BURST_LENGTH; j++)
			io_write(DDRT_BASE_ADDR + DDRT_DATABUF + j * 4, ssn_code[j]);

		io_write(DDRT_BASE_ADDR + DDRT_INTCLR, 1);
		io_write(DDRT_BASE_ADDR + DDRT_START, 1);
		while (!(io_read(DDRT_BASE_ADDR)))
			;

		io_write(DDRT_BASE_ADDR + 0xc, 1);
		for (n = 0; n < ad_cnt; n++)
			for (i = 0; i < BURST_LENGTH; i++) {
				code_tmp = io_read(SSN_DMA_DDR_BASE + n * 0x200 + i * 4);
				if (code_tmp !=  ssn_code[i]) {
					printf("ddr[%d]=0x%x, ssn[%d]=0x%x\n", i, code_tmp, i, ssn_code[i]);
					return -1;
				}
			}
	} else if (option == DDRT_MODE_READ) {
		io_write(DDRT_BASE_ADDR + DDRT_LOOP_NUM, loop_num_r - 1);
		for (n = 0; n < loop_num_r; ++n)
			memcpy_soc((unsigned int *)((char *)ddr_addr + n * BURST_LENGTH * 4),
					ssn_code, BURST_LENGTH * 4);
		for (n = 0; n < ad_cnt; n++) {
			io_write(DDRT_BASE_ADDR + DDRT_INTCLR, 1);
			io_write(DDRT_BASE_ADDR + DDRT_START, 1);
			while (!(io_read(DDRT_BASE_ADDR)))
				;
			io_write(DDRT_BASE_ADDR + 0xc, 1);
			for (i = 0; i < BURST_LENGTH; i++) {
				code_tmp = io_read(DDRT_BASE_ADDR + DDRT_DATABUF + i * 4);
				if (code_tmp != ssn_code[i]) {
					printf("ddr[%d]=0x%x, ssn[%d]=0x%x\n", i, code_tmp, i, ssn_code[i]);
					return -1;
				}
			}
		}
	} else
		return -1;
	return 0;
}

int ddr_training_ssn_pressure(int total, unsigned int option)
{
	unsigned int  ssn_cnt = 0;
	unsigned int  ssn_num;
	unsigned int  ssn_code_temp;
	unsigned int  ssn_check_temp;

	delay_soc(1);

	printf("----------ssn multi test start!!!!----------\n");

	for (ssn_cnt = 0; ssn_cnt < total;) {
		if (check_multi_pressure(&ssn_num, &ssn_code_temp,
					&ssn_check_temp, option) != 0) {
			ssn_error_num = ssn_num;
			ssn_error_code = ssn_code_temp;
			ssn_error_check = ssn_check_temp;
			printf("DDR multi channel pressure, test is error!\n");
			ssn_cycnum = ssn_cnt;
			return -1;
		} else {
			++ssn_cnt;
			printf("-----------ssn multi");
			printf("test %d times OK-----------\n", ssn_cnt);

		}
	}
	printf("DDR multi channel pressure test is OK!!!!!!!\n");
	return 0;
}

/**************************************************/
#if 1
int ddr_cyc_pressure(int total, unsigned int option)
{
	unsigned int cyc_cnt = 0;
	unsigned int all_num = 0;
	unsigned int wr_block_num = 0;
	unsigned int save_addr;
	unsigned int nand_offest_addr = 0;
	size_t length;
	nand_erase_options_t opts;

	/******************clear the SSN_TEST_FLAG **********************/
	length = 0x800000;   /*16M*/
	opts.offset = 0x400000;
	opts.length = length;
	io_write(SSN_SAVE_BASE, 0x6f6b6f6b);
	io_write(SSN_SAVE_BASE + 0x4, 0x0);
	nand_erase_opts(&nand_info[nand_curr_device], &opts);
	opts.offset = 0x400000 + 0x800000;
	nand_erase_opts(&nand_info[nand_curr_device], &opts);
	length = 0x2000 ;
	nand_write_skip_bad(&nand_info[nand_curr_device],
			0x400000, &length, (u_char *)SSN_SAVE_BASE);

	/******************** start pressure ****************************/
	while (wr_block_num < 2048) {
		if (cyc_cnt < 512) {

			if (ddr_training_ssn_pressure(total, option) != 0) {
				save_addr = SSN_SAVE_BASE
					+ 0x100000 +
					wr_block_num * 0x2000
					+ cyc_cnt*0x10;

				all_num = all_num + cyc_cnt;
				io_write(SSN_SAVE_BASE, 0x72726f72);
				io_write(SSN_SAVE_BASE + 0x4, wr_block_num);
				io_write(save_addr, ssn_cycnum);
				io_write(save_addr + 0x4, ssn_error_num);
				io_write(save_addr + 0x8, ssn_error_code);
				io_write(save_addr + 0xc, ssn_error_check);
			}
			cyc_cnt++;
			if (cyc_cnt == 1) {
				memset(&opts, 0, sizeof(struct nand_erase_options));

				opts.offset = 0x400000;
				opts.length = 0x80000;

				nand_erase_opts(&nand_info[nand_curr_device], &opts);
				nand_write_skip_bad(&nand_info[nand_curr_device],
					 0x400000, &length, (u_char *)SSN_SAVE_BASE);
			}
		} else {
			/************save to flash************************/
			printf("%0x,%0x\n", ssn_error_code, ssn_error_check);
			nand_offest_addr = wr_block_num * 0x2000;
			ddr_training_ssn_save(nand_offest_addr);
			all_num = all_num + 1;
			wr_block_num  = wr_block_num + 1;
			cyc_cnt = 0;
		}
	}

	printf("the test it too long to save\n");
	return 0;
}

/*****************************************************************************/
static int ddr_training_ssn_save(unsigned int nand_offest_addr)
{
	size_t length;
	nand_erase_options_t opts;

	length = 0x2000;
	opts.offset = 0x400000 + nand_offest_addr;
	opts.length = length;
	loff_t nand_offest = nand_offest_addr;
	nand_write_skip_bad(&nand_info[nand_curr_device],
	0x500000 + nand_offest, &length,
	(u_char *)(SSN_SAVE_BASE + 0x100000 + nand_offest_addr));
	 return 0;
}
/*****************************************************************************/
int ddr_result_printf(void)
{
	size_t length;
	nand_erase_options_t opts;

	length = 0x2000;
	opts.offset = 0x400000;
	opts.length = length;
	unsigned int nand_offest ;
	unsigned int i, n;
	unsigned int bank, cycnum, codenum;
	unsigned int ssn_flag;
	unsigned int ssn_bank_num;
	unsigned int save_addr;
	nand_read_skip_bad(&nand_info[nand_curr_device],
			0x400000, &length, (u_char *)SSN_SAVE_BASE);

	ssn_flag = io_read(SSN_SAVE_BASE);
	ssn_bank_num = io_read(SSN_SAVE_BASE + 0x4);

	if (ssn_flag == 0x6f6b6f6b) {
		printf("The last DDR pressure test is OK !!!!!!\n");
		return 0;
	} else if (ssn_flag == 0x72726f72) {
		if (ssn_bank_num == 0) {
			printf("The error data not enough 8k");
			printf("and not save !!\n");
			return 0;
		} else {
			printf("The last DDR pressure test is");
			printf("ERROR !!!!!!\n");
			printf("The total error number");
			printf("is  %d\n" , ssn_bank_num);
			printf("The error information.");
			printf("....................\n");
			printf("SEQ BURST CODE      SSN_CODE");
			printf("CHECK_CODE\n");
		}
	} else {
		printf("Please start a test, input: ddr cycpressure\n");
		return 0;
	}

	/*************************prepare for printf*********************************/
	for (n = 0; n < ssn_bank_num; n++) {
		nand_offest =  0x100000 + n*0x2000;
		nand_read_skip_bad(&nand_info[nand_curr_device],
				0x400000 + nand_offest,
				&length,
				(u_char *)(SSN_SAVE_BASE + nand_offest));
		for (i = 0; i < 512; i++) {
			save_addr = SSN_SAVE_BASE + nand_offest + i * 0x10;
			ssn_cycnum = io_read(save_addr);
			ssn_error_num = io_read(save_addr + 0x4);
			ssn_error_code = io_read(save_addr + 0x8);
			ssn_error_check = io_read(save_addr + 0xc);
			bank = ssn_error_num & 0xf;
			cycnum = (ssn_error_num & 0xfff0)>>4;
			codenum = (ssn_error_num & 0xff0000)>>16;
			printf(" %d,  %d,   %d,     0x%x,      0x%x\n",
					i + n * 512, bank, codenum,
					ssn_error_code, ssn_error_check);
		}
	}
	return 0;
}
#endif
/*****************************************************************************/
static int check_ssn_code(unsigned int input, unsigned int *ssn_code)
{

	unsigned int i, j;
	unsigned int ssn_code_tmp[8];

	unsigned int prbs_sole0[16] = {
		0x73, 0x65, 0x6B, 0xEF, 0x0C, 0x45, 0x33, 0xD5,
		0x1F, 0xC8, 0x16, 0x0E, 0x84, 0xE3, 0x49, 0x76
	};
	unsigned int prbs_sole1[16] = {
		0xF0, 0x0F, 0x87, 0x78, 0xE1, 0x1E, 0xD2, 0x2D,
		0xC3, 0x3C, 0xB4, 0x4B, 0xA5, 0x5A, 0x96, 0x69
	};
	unsigned int prs_data0[4] = {
		0x00000000, 0xff00ff00, 0x00ff00ff, 0xffffffff
	};
	unsigned int prs_data1[4] = {
		0x10101010, 0xef10ef10, 0x10ef10ef, 0xefefefef
	};
	unsigned int prs_data2[4] = {
		0x00000000, 0xef00ef00, 0x00ef00ef, 0xefefefef
	};
	unsigned int prs_data3[4] = {
		0x10101010, 0xff10ff10, 0x10ff10ff, 0xffffffff
	};
	unsigned int prs_data4[4] = {
		0x00000000, 0x10001000, 0x00100010, 0x10101010
	};
	switch (input) {
	case 0:
		for (i = 0; i < 16; i++) {
			prs_rebuilt(prbs_sole0[i], ssn_code_tmp, prs_data0);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;
	case 1:
		for (i = 0; i < 16; i++) {
			prs_rebuilt(prbs_sole0[i],
					ssn_code_tmp, prs_data1);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;
	case 2:
		for (i = 0; i < 16; i++) {
			prs_rebuilt(prbs_sole1[i],
					ssn_code_tmp, prs_data0);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;
	case 3:
		for (i = 0; i < 16; i++) {
			prs_rebuilt(prbs_sole1[i],
					ssn_code_tmp, prs_data1);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;
	case 4:
		for (i = 0; i < 8; i++) {
			prs_rebuilt(prbs_sole1[i],
					ssn_code_tmp, prs_data0);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;
	case 5:
		for (i = 0; i < 8; i++) {
			prs_rebuilt(prbs_sole1[i],
					ssn_code_tmp, prs_data1);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;
	case 6:
		for (i = 0; i < 8; i++) {
			prs_rebuilt(prbs_sole1[i],
					ssn_code_tmp, prs_data2);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];

		}
		break;

	case 7:
		for (i = 0; i < 8; i++) {
			prs_rebuilt(prbs_sole1[i],
					ssn_code_tmp, prs_data3);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;
	case 8:
		for (i = 0; i < 8; i++) {
			prs_rebuilt(prbs_sole0[i], ssn_code_tmp,
					prs_data4);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;

	case 9:
		for (i = 0; i < 8; i++) {
			prs_rebuilt(prbs_sole1[i], ssn_code_tmp,
					prs_data4);
			for (j = 0; j < 8; j++)
				ssn_code[i * 8 + j] = ssn_code_tmp[j];
		}
		break;

	default:
		break;
	}
	return 0;
}
/****************************************************************************/
static int calculate_para(unsigned int size, unsigned int val,
		struct hi_training *training)
{
	unsigned int record_first = 0;
	unsigned int best_para = 0;
	unsigned int record_last = 0;
	unsigned int record_cnt = 0;
	unsigned int tmp_first = 0;
	unsigned int tmp_last = 0;
	unsigned int tmp_cnt = 0;
	unsigned int flag = 0;
	unsigned int i = 0;
	while (1) {
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
					record_last = tmp_last;
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
				record_last = tmp_last;
				record_cnt = tmp_cnt;
			}
			break;
		}
	}
	if (record_cnt == 0) {
		training->skew_cnt = 0;
		return ALL_ERROR;
	} else {
		training->skew_cnt = record_cnt;
		best_para = record_cnt / 2 + record_first;
		return best_para;
	}
}

/*****************************************************************************/
static int get_best_para(struct hi_training *training)
{
	unsigned int i;
	unsigned int wr_para_tmp;
	unsigned int rd_para_tmp;
	unsigned int tmp;

	wr_para_tmp = 0;
	rd_para_tmp = 0;
	for (i = 0; i < TRAINING_WR_CNT; i++) {
		tmp = training->wr_data[i];
		tmp = training->wr_result[tmp >> 16] & (1 << (tmp & 0xffff));
		wr_para_tmp |= tmp ? (1 << i) : 0;
	}
	for (i = 0; i < TRAINING_RD_CNT; i++) {
		tmp = training->rd_data[i];
		tmp = training->rd_result[tmp >> 16] & (1 << (tmp & 0xffff));
		rd_para_tmp |= tmp ? (1 << i) : 0;
	}
	training->wr_para = wr_para_tmp;
	training->rd_para = rd_para_tmp;

	/*
	 * get the best parameters of write-skew
	 */
	training->wr_best = calculate_para(TRAINING_WR_CNT,
			training->wr_para, training);
	training->wr_skew_cnt = training->skew_cnt;

	/*
	 * get the best parameters of read-skew
	 */
	training->rd_best = calculate_para(TRAINING_RD_CNT,
			training->rd_para, training);
	training->rd_skew_cnt = training->skew_cnt;
	return 0;
}

/*****************************************************************************/
static void print_risk(int cnt)
{
	if (cnt <= 1)
		printf("No Skew!\n");
	else if ((cnt > 1) && (cnt <= 3))
		printf("High Risk!\n");
	else if ((cnt > 3) && (cnt <= 5))
		printf("Potential Risk!\n");
	else
		printf("Low Risk!\n");

}
static int print_result(struct hi_training *training)
{
	unsigned int i;

	printf("----------------------------------------------------------\n");
	printf("The window of deskew\n");
	printf("----------------------------------------------------------\n");
	printf("Write DQS  DQ  Result\n");
	for (i = 0; i < 29; i++)
		printf("      %d    %d   %s\n",
				training->wr_data[i] >> 16,
				training->wr_data[i] & 0xffff,
				training->wr_para & (1 << i) ? "OK" : "ERROR");

	printf("Read  DQS  DQ  Result\n");
	for (i = 0; i < 15; i++)
		printf("      %d    %d   %s\n",
				training->rd_data[i] >> 16,
				training->rd_data[i] & 0xffff,
				training->rd_para&(1 << i) ? "OK" : "ERROR");
	printf("----------------------------------------------------------\n");
	printf("The best deskew of DQS/DQ\n"
			"Write DQS:%d   DQ:%d\n"
			"Read  DQS:%d   DQ:%d\n",
			training->wr_data[training->wr_best] >> 16,
			training->wr_data[training->wr_best] & 0xffff,
			training->rd_data[training->rd_best] >> 16,
			training->rd_data[training->rd_best] & 0xffff);
	printf("----------------------------------------------------------\n");
	printf("Risk Assessment:\n");
	printf("Write:");
	print_risk(training->wr_skew_cnt);
	printf("Read: ");
	print_risk(training->rd_skew_cnt);
	return 0;
}

static int prebit_train(struct hi_training *training)
{
	unsigned int i, j;
	unsigned int tmp;
	unsigned int dqsresult_tmp;
	unsigned int dqresult_tmp;
	unsigned int dqs_disable_tmp;

	for (i = 0; i < 32; i++) {
		training->prebit_wr_result[i] =  0;
		training->prebit_rd_result[i] =  0;
	}

	for (i = 0; i < 4; i++)
		training->prebit_dm_result[i] = 0;

	read_config_ddr(training);
	for (i = 0; i < 4; i++) {
		dqsresult_tmp = training->wrdqs_val;
		dqresult_tmp = training->wrdq_val[i];
		/*every DQS  have 8 DQ ; j  is bits 0-7*/
		for (j = 0; j < 8; j++) {
			/*DQS write enable DM enable and  DQS set 0,
			  training write DQ*/
			training->lvlskew_val = ENABLE_WR_DM | ENABLE_WR_DQS;
			tmp = training->dqs_data[0];
			training->wrdqs_val = set_dqs(training->wrdqs_val,
					tmp, i, 0);
			prebit_travel_wrdq(training, j, i);

			training->wrdqs_val = dqsresult_tmp;
			training->wrdq_val[i] = dqresult_tmp;

			/*write DQ set 0, training write DQS*/
			prebit_travel_wrdqs(training, j, i);

			training->wrdqs_val  = dqsresult_tmp;
			training->wrdq_val[i] = dqresult_tmp;

			prebit_travel_disable_dq(training, j, i);

			training->wrdq_val[i] = dqresult_tmp;
			training->wrdqs_val = dqsresult_tmp;
			training->lvlskew_val = 0x000f000f;

			dqs_disable_tmp = ~(1 << i);
			training->lvlskew_val = 0x000f000f & dqs_disable_tmp;
			prebit_travel_disable_dqs(training, j, i);

			training->lvlskew_val = 0x000f000f;
			training->wrdq_val[i] = dqresult_tmp;
			training->wrdqs_val  = dqsresult_tmp;
		}
	}

	training->lvlskew_val = 0x000f000f;
	/* read training , wr dqs , dq, unchange */
	for (i = 0; i < 4; i++) {
		dqsresult_tmp = training->rddqs_val;
		dqresult_tmp = training->rddq_val[i];
		/*every DQS  have 8 DQ ; j  is bits 0-7*/
		for (j = 0;  j < 8; j++) {
			/*read DQS set 0  , training  read DQ*/

			tmp = training->dqs_data[0];
			training->rddqs_val = set_dqs(training->rddqs_val,
					tmp, i, 0);
			prebit_travel_rddq(training, j, i);

			/*read DQ set 0  , training  read DQS*/
			prebit_travel_rddqs(training, j, i);

			training->rddq_val[i] = dqresult_tmp;
		}
		training->rddqs_val = dqsresult_tmp;
	}

	get_bestprebit_para(training);
	config_ddr(training);
	return 0;
}

static int prebit_travel_rddq(struct hi_training *training,
		unsigned int bit, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int bit_num;
	unsigned int level;
	bit_num = byte * 8 + bit;
	for (i = 0; i < 8; i++) {
		tmp =  training->dq_data[i];
		bitset_dq(training->rddq_val, tmp, bit, byte);
		level = ddr_rdlevel(0, i);
		config_ddr(training);
		if (!check_multi_ssn(training, bit_num))
			training->prebit_rd_result[bit_num] |= (1 << level);
	}
	return 0;
}

static int prebit_travel_rddqs(struct hi_training *training,
		unsigned int bit, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int bit_num;
	unsigned int level;
	bit_num = byte * 8 + bit;
	for (i = 1; i < 8; i++) {
		training->rddq_val[byte] = training->dq_data[i];
		tmp =  training->dqs_data[i];
		training->rddqs_val = set_dqs(training->rddqs_val, tmp, byte, 0);
		tmp = training->dq_data[0];
		bitset_dq(training->rddq_val, tmp, bit, byte);

		level = ddr_rdlevel(i, 0);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->prebit_rd_result[bit_num] |= (1 << level);
	}
	return 0;
}

static int ddr_rdlevel(unsigned int dqs_level, unsigned int dq_level)
{
	unsigned int level;
	if (dqs_level == 0)
		level = 7 - dq_level;
	else
		level = 7 + dqs_level;
	return level;
}

static int read_config_ddr(struct hi_training *training)
{
	unsigned int ddrc_base;
	ddrc_base = training->reg_ddr_base;

	training->wrdq_val[0] = io_read(ddrc_base + DDRC_WRDQSKEW(0));
	training->wrdq_val[1] = io_read(ddrc_base + DDRC_WRDQSKEW(1));
	training->wrdq_val[2] = io_read(ddrc_base + DDRC_WRDQSKEW(2));
	training->wrdq_val[3] = io_read(ddrc_base + DDRC_WRDQSKEW(3));

	training->rddq_val[0] = io_read(ddrc_base + DDRC_RDDQSKEW(0));
	training->rddq_val[1] = io_read(ddrc_base + DDRC_RDDQSKEW(1));
	training->rddq_val[2] = io_read(ddrc_base + DDRC_RDDQSKEW(2));
	training->rddq_val[3] = io_read(ddrc_base + DDRC_RDDQSKEW(3));

	training->wrdqs_val = io_read(ddrc_base + DDRC_WRDQS_SKEW);
	training->rddqs_val = io_read(ddrc_base + DDRC_RDDQS_SKEW);

	training->wrdm_val = io_read(ddrc_base + DDRC_WRDMSKEW);
	training->lvlskew_val = io_read(ddrc_base + DDRC_LVLSKEW);

	return 0;
}

static int prebit_travel_disable_dqs(struct hi_training *training,
		unsigned int bit, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int bit_num;
	unsigned int level;

	bit_num = byte * 8 + bit;
	for (i = 1; i < 8; i++) {
		tmp =  training->dq_data[i];
		bitset_dq(training->wrdq_val, tmp, bit, byte);
		level = ddr_wrlevel(8, i);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->prebit_wr_result[bit_num] |= (1 << level);
	}
	return 0;
}

static int prebit_travel_disable_dq(struct hi_training *training,
		unsigned int bit, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int bit_num;
	unsigned int level;
	bit_num = byte * 8 + bit;

	for (i = 1; i < 8; i++) {
		tmp =  training->dqs_data[i];
		training->wrdqs_val = set_dqs(training->wrdqs_val,
				tmp, byte, 0);
		training->wrdq_val[byte] = training->dq_data[i];
		tmp = ~(1 << (24 + bit));
		training->wrdq_val[byte] =
				training->wrdq_val[byte] & tmp;

		level = ddr_wrlevel(i, 7);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->prebit_wr_result[bit_num] |= (1 << level);
	}
	return 0;
}

static int ddr_wrlevel(unsigned int dqs_level, unsigned int dq_level)
{
	unsigned int level;
	if (dqs_level == 0)
		level = 14 - dq_level;
	else if (dqs_level == 8)
		level = 7 - dq_level;
	else
		level = 14 + dq_level + dqs_level;
	return level;
}

static int bitset_dq(unsigned int *dq, unsigned int val,
		unsigned int bitnum, unsigned int bytenum)
{
	switch (bitnum) {
	case 0:
		dq[bytenum] = (dq[bytenum] & 0xfffffff8) | (val & 0x7);
		break;
	case 1:
		dq[bytenum] = (dq[bytenum] & 0xffffffc7) | (val & 0x38);
		break;
	case 2:
		dq[bytenum] = (dq[bytenum] & 0xfffffe3f) |
			(val & 0x1c0);
		break;
	case 3:
		dq[bytenum] = (dq[bytenum] & 0xfffff1ff) |
			(val & 0xe00);
		break;
	case 4:
		dq[bytenum] = (dq[bytenum] & 0xffff8fff) |
			(val & 0x7000);
		break;
	case 5:
		dq[bytenum] = (dq[bytenum] & 0xfffc7fff) |
			(val & 0x38000);
		break;
	case 6:
		dq[bytenum] = (dq[bytenum] & 0xffe3ffff) |
			(val & 0x1c0000);
		break;
	case 7:
		dq[bytenum] = (dq[bytenum] & 0xff1fffff) |
			(val & 0xe00000);
		break;
	default:
		break;
	}
	return 0;
}

static int prebit_travel_wrdq(struct hi_training *training,
		unsigned int bit, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int bit_num;
	unsigned int level;

	bit_num = byte * 8 + bit;

	for (i = 0; i < 8; i++) {
		tmp =  training->dq_data[i];
		bitset_dq(training->wrdq_val, tmp, bit, byte);
		level = ddr_wrlevel(0, i);
		config_ddr(training);

		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->prebit_wr_result[bit_num] |= (1 << level);
	}
	return 0;
}

static int prebit_travel_wrdqs(struct hi_training *training,
		unsigned int bit, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int bit_num;
	unsigned int level;
	bit_num = byte * 8 + bit;

	for (i = 1; i < 8; i++) {
		tmp =  training->dqs_data[i];
		training->wrdq_val[byte] = training->dq_data[i];
		training->wrdqs_val = set_dqs(training->wrdqs_val, tmp,
				byte, 0);

		tmp = training->dq_data[0];
		bitset_dq(training->wrdq_val, tmp, bit, byte);

		level = ddr_wrlevel(i, 0);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->prebit_wr_result[bit_num] |= (1 << level);
	}
	return 0;
}

static int set_dqs(unsigned int dqs, unsigned int val,
		unsigned int bytenum, unsigned int bytewidth)
{
	if (bytewidth == 0) {
		switch (bytenum) {
		case 0:
			dqs = (dqs & 0xfffffff8) | (val & 0x7);
			break;
		case 1:
			dqs = (dqs & 0xffffffc7) | (val & 0x38);
			break;
		case 2:
			dqs = (dqs & 0xfffffe3f) | (val & 0x1c0);
			break;
		case 3:
			dqs = (dqs & 0xfffff1ff) | (val & 0xe00);
			break;
		default:
			break;
		}
	} else
		dqs = val;
	return dqs;
}

static int get_bestprebit_para(struct hi_training *training)
{
	unsigned int i, j;
	unsigned int max_result;
	unsigned int max_dqs, max_dq;
	unsigned int dqs, dq;
	unsigned int tmp;

	for (i = 0; i < 32; i++) {
		training->prebit_wr_best[i] =
			perbit_calculate_para(TRAINING_WR_CNT,
					training->prebit_wr_result[i]);
		training->prebit_rd_best[i] =
			perbit_calculate_para(TRAINING_RD_CNT,
					training->prebit_rd_result[i]);
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 8; j++) {
			if (j == 0)
				max_result =
					training->prebit_wr_best[i * 8 + j] &
					0xffff;
			else
				if (max_result <= (training->prebit_wr_best[i * 8 + j] & 0xffff))
					max_result = training->prebit_wr_best[i * 8 + j] & 0xffff;
		}
		/*
		   if (max_result <= training->prebit_dm_best[i])
			   max_result = training->prebit_dm_best[i];
		   if (max_result <= training->prebit_dm_best[1])
			   max_result = training->prebit_dm_best[1];
		 */

		/*find byte wr max_dqs*/
		wrlevel_spilt(max_result, &max_dqs, &max_dq);
		/*set  max_dqs  in wr_dqsval*/
		training->wrdqs_val = set_dqs(training->wrdqs_val, training->dqs_data[max_dqs], i, 0);

		/*rebuilt DQ level*/
		for (j = 0; j < 8; j++) {
			tmp = training->prebit_wr_best[i * 8 + j] & 0xffff;
			wrlevel_spilt(tmp, &dqs, &dq);
			if (max_dqs != dqs) {
				dq = dq + (max_result - tmp);
				if (dq > 7)
					dq = 7;
			}
			/*set bit best  dq*/
			bitset_dq(training->wrdq_val, training->dq_data[dq], j, i);
		}

	}

	/*find rd max_result for byte*/
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 8; j++) {
			if (j == 0)
				max_result = training->prebit_rd_best[i * 8 + j] & 0xffff;
			else
				if (max_result <= (training->prebit_rd_best[i * 8 + j] & 0xffff))
					max_result = training->prebit_rd_best[i * 8 + j] & 0xffff;
		}

		level_spilt(max_result, &max_dqs, &max_dq);  /*find byte rd max_dqs*/
		training->rddqs_val = set_dqs(training->rddqs_val, max_dqs, i, 0); /*set  max_dqs  in rd_dqsval*/
		for (j = 0; j < 8; j++) {
			tmp = training->prebit_rd_best[i * 8 + j] & 0xffff;
			level_spilt(tmp, &dqs, &dq);
			if (max_dqs != dqs) {
				dq = dq + (max_result - tmp);
				if (dq > 7)
					dq = 7;
			}
			/*set prebit  best  dq*/
			bitset_dq(training->rddq_val, training->dq_data[dq], j, i);
		}
	}
	return 0;
}

static int level_spilt(unsigned int level, unsigned int *dqs_level,
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

#if 0
static int prebit_travel_wrdm(struct hi_training *training, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int level;

	for (i = 0; i < 8; i++) {
		tmp = training->dqs_data[i];
		set_dm(training->wrdm_val, tmp,  byte, 0);
		level = ddr_rdlevel(0, i);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->prebit_dm_result[byte] |= (1 << level);
	}
	return 0;
}

static int prebit_travel_dmdqs(struct hi_training *training, unsigned int byte)
{
	unsigned int i;
	unsigned int tmp;
	unsigned int level;
	for (i = 0; i < 8; i++) {
		tmp =  training->dqs_data[i];
		set_dqs(training, tmp, byte, 0);
		level = ddr_rdlevel(i, 0);
		config_ddr(training);
		if (!check_multi_ssn(training, DDRTRAININT_BYTE))
			training->prebit_dm_result[byte] |= (1 << level);
	}

	return 0;
}
#endif

static int perbit_calculate_para(unsigned int size, unsigned int val)
{
	unsigned int record_first = 0;
	unsigned int record_last = 0;
	unsigned int record_cnt = 0;
	unsigned int tmp_first = 0;
	unsigned int tmp_last = 0;
	unsigned int tmp_cnt = 0;
	unsigned int flag = 0;
	unsigned int i = 0;
	unsigned int result;
	while (1) {
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
					record_last = tmp_last;
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
				record_last = tmp_last;
				record_cnt = tmp_cnt;
			}
			break;
		}
	}
	if (record_cnt == 0)
		return ALL_ERROR;
	else {
		result = record_cnt << 16;
		result = (result & 0xffff0000) | (record_cnt / 2 + record_first);
		return  result;
	}
}

static int wrlevel_spilt(unsigned int level, unsigned int *dqs_level, unsigned int *dq_level)
{
	if (level < 7) {
		*dqs_level = 8;
		*dq_level = 7 - level;
	} else if ((7 <= level) && (level < 15)) {
		*dqs_level = 0;
		*dq_level = (~(level - 7)) & 0x7;
	} else if ((15 <= level) && (level < 22)) {
		*dqs_level = level - 14;
		*dq_level = 0;
	} else {
		*dqs_level = level - 21;
		*dq_level = 8;
	}
	return 0;
}

#if 0
static void set_dm(unsigned int dm, unsigned int val, unsigned int bytenum,
		unsigned int bytewidth)
{
	if (bytewidth == 0) {
		switch (bytenum) {
		case 0:
			dm = (dm & 0xfffffff8) | (val & 0x7);
			break;
		case 1:
			dm = (dm & 0xffffffc7) | (val & 0x38);
			break;
		case 2:
			dm = (dm & 0xfffffe3f) | (val & 0x1c0);
			break;
		case 3:
			dm = (dm & 0xfffff1ff) | (val & 0xe00);
			break;
		default:
			break;
		}
	} else
		dm = val;
}
#endif

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
	printf(" %-12s  %-5s%-4s%-4s%-4s\n", "HEX_RESULT", "BEST", "DQS", "DQ", "WIN");\
} while (0)

static int print_prebit_result(struct hi_training *training)
{
	unsigned int i, k;
	const unsigned int count = sizeof(training->prebit_wr_result)\
				   / sizeof(unsigned int);
	unsigned int best_tmp, dqs_tmp, dq_tmp, cnt_tmp;
	printf("----------------------------------------------------------\n");
	printf("The window of deskew\n");
	printf("----------------------------------------------------------\n");
	printf("========== Write DQS  DQ  Result ==========\n");
	PRINT_PREBIT_TMP2(DDRT_WRITE_LEVEL);
	for (i = 0; i < count; i++) {
		cnt_tmp = training->prebit_wr_best[i] >> 16;
		best_tmp =  training->prebit_wr_best[i] & 0xffff;
		wrlevel_spilt(best_tmp, &dqs_tmp, &dq_tmp);
		PRINT_PREBIT(DDRT_WRITE_LEVEL, i, training->prebit_wr_result[i],
				training->prebit_wr_best[i] & 0xffff,
				dqs_tmp, dq_tmp, cnt_tmp);
	}
	printf("========== Read  DQS  DQ  Result ==========\n");
	PRINT_PREBIT_TMP2(DDRT_READ_LEVEL);
	for (i = 0; i < count; i++) {
		cnt_tmp = training->prebit_rd_best[i] >> 16;
		best_tmp =  training->prebit_rd_best[i] & 0xffff;
		level_spilt(best_tmp, &dqs_tmp, &dq_tmp);
		PRINT_PREBIT(DDRT_READ_LEVEL, i, training->prebit_rd_result[i],
				training->prebit_rd_best[i] & 0xffff,
				dqs_tmp, dq_tmp, cnt_tmp);
	}

#if 0
	printf("-------------------deskew REG config-------------------------\n");
	printf("Write DQS      0x20110500 = 0x%x\n", io_read(0x20110500));
	printf("Write DQ0-7    0x20110510 = 0x%x\n", io_read(0x20110510));
	printf("Write DQ8-15   0x20110514 = 0x%x\n", io_read(0x20110514));
	printf("Write DQ16-23  0x20110518 = 0x%x\n", io_read(0x20110518));
	printf("Write DQ24-31  0x2011051c = 0x%x\n", io_read(0x2011051c));
	printf("Read DQS       0x20110504 = 0x%x\n", io_read(0x20110504));
	printf("Read DQ0-7     0x20110540 = 0x%x\n", io_read(0x20110540));
	printf("Read DQ8-15    0x20110540 = 0x%x\n", io_read(0x20110544));
	printf("Read DQ16-23   0x20110540 = 0x%x\n", io_read(0x20110548));
	printf("Read DQ24-31   0x20110540 = 0x%x\n", io_read(0x2011054c));
#endif
	return 0;
}
