#include <common.h>
#include <command.h>
#include <serial.h>
#include <nand.h>
#include <spi_flash.h>
#include <asm/io.h>

#include "../product/hiddrtv200/ddrtraining.h"

static int atoi(char *string)
{
	int length;
	int retval = 0;
	int i;
	int sign = 1;

	length = strlen(string);
	for (i = 0; i < length; i++) {
		if (0 == i && string[0] == '-') {
			sign = -1;
			continue;
		}
		if (string[i] > '9' || string[i] < '0')
			break;

		retval *= 10;
		retval += string[i] - '0';
	}
	retval *= sign;
	return retval;
}

int do_ddr(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc != 2)
		goto ERROR;

	unsigned int *reg1 = (unsigned int *)(DDRT_OPTION_REG1);
	unsigned int tmp = 0;
	tmp = atoi(argv[1]);
	if (tmp < 1 || tmp > 3)
		goto ERROR;
	*reg1 = tmp;
	printf("ddr trainning will start ...\n");
	printf("please wait and wait!!!\n");
	udelay(10000);
	reset_cpu(0);
	udelay(10000);
	return 0;
ERROR:
	printf("ddr training tools v200\n"
			"usage like this:\n"
			"ddrt num [num=1,2,3], eg. ddrt 1\n"
			"DESCRIPTION:\n"
			"\t1. bit write & read\n"
			"\t2. bit write\n"
			"\t3. bit read\n");
	return -1;
}

U_BOOT_CMD(ddrt, CONFIG_SYS_MAXARGS, 0, do_ddr,
		"ddr training tools v200",
		"ddr training tools v200\n"
		"usage like this:\n"
		"ddrt num [num=1,2,3], eg. ddrt 1\n"
		"DESCRIPTION:\n"
		"\t1. bit write & read"
		"\t2. bit write"
		"\t3. bit read"
	  );

static void print_usage(void)
{
	printf("ddr pressure training tools v200\n"
			"ddrp [OPTION] ...\n"
			"DESCRIPTION:\n"
			"\t-p, pattern, eg.PRBS9/PRBS7/PRBS11/K28.5, default PRBS9.\n"
			"\t-m, write and read mode, eg.WR/W/R, default WR.\n"
			"\t-b, burst number in each loop, default 0x8.\n"
			"\t-l, loop number in each test, default 0xff.\n"
			"\t-s, DDR starting address, default 0x82000000.\n"
			"\t-o, DDRT address offset, at least 128bit align, default 0x1000.\n"
			"\t-rl, DDRT reversed bit indicator for lower 32bit, default 0x0.\n"
			"\t-rh, DDRT reversed bit indicator for higher 32bit, default 0x0.\n"
			"\t-kd, DDRT K28.5 pattern data config register, default 0xbc3eb053.\n"
			"\t-d0, DDRT data config register 0, default 0x8a18207f.\n"
			"\t-d1, DDRT data config register 1, default 0x5f2b9a27.\n"
			"\t-d2, DDRT data config register 2, default 0xbdad9238.\n"
			"\t-d3, DDRT data config register 3, default 0xaa6774b1.\n"
			"\t-c, loop times, default 1.\n");

	return;
}

int do_ddr_pressure(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int mode = E_DDRT_WR_RD;
	unsigned int pattern = DDRT_DEFAULT_PATTERN;
	unsigned int mem_cfg = DDRT_DEFAULT_MEM_CFG_VAL;
	unsigned int reversed_lower = 0;
	unsigned int reversed_higher = 0;
	unsigned int loop_num = DDRT_DEFAULT_LOOP_NUM;
	unsigned int burst_num = DDRT_DEFAULT_BURST_NR;
	unsigned int start_addr = DDRT_DEFAULT_DDR_START_ADDR;
	unsigned int offset_addr = DDRT_DEFAULT_DDR_OFFSET;
	unsigned int k_data = DDRT_DEFAULT_KDATA;
	unsigned int data0 = DDRT_DEFAULT_DATA0;
	unsigned int data1 = DDRT_DEFAULT_DATA1;
	unsigned int data2 = DDRT_DEFAULT_DATA2;
	unsigned int data3 = DDRT_DEFAULT_DATA3;
	unsigned int count = 0x1;
	unsigned int success_count = 0;
	unsigned int fail_count = 0;
	unsigned int result = 0;

	int i = 0;
	int flags = 0;

	if (argc <= 1)
		goto ERROR;

	for (i = 1; i < argc; i += 2) {
		if (0 == strcmp(argv[i], "-p")) {
			if (0 == strcmp(argv[i + 1], "PRBS9"))
				pattern = E_DDRT_PRBS9;
			else if (0 == strcmp(argv[i + 1], "PRBS7"))
				pattern = E_DDRT_PRBS7;
			else if (0 == strcmp(argv[i + i], "PRBS11"))
				pattern = E_DDRT_PRBS11;
			else if (0 == strcmp(argv[i + i], "K28.5"))
				pattern = E_DDRT_K28_5;
			else
				flags = 1;
		} else if (0 == strcmp(argv[i], "-m")) {
			if (0 == strcmp(argv[i + 1], "WR"))
				mode = E_DDRT_WR_RD;
			else if (0 == strcmp(argv[i + 1], "W"))
				mode = E_DDRT_WR;
			else if (0 == strcmp(argv[i + 1], "R"))
				mode = E_DDRT_RD;
			else
				flags = 1;

		} else if (0 == strcmp("-b", argv[i]))
			burst_num = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-l", argv[i]))
			loop_num = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-s", argv[i]))
			start_addr = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-o", argv[i]))
			offset_addr = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-rl", argv[i]))
			reversed_lower = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-rh", argv[i]))
			reversed_higher = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-kd", argv[i]))
			k_data = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-d0", argv[i]))
			data0 = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-d1", argv[i]))
			data1 = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-d2", argv[i]))
			data2 = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-d3", argv[i]))
			data3 = simple_strtoul(argv[i + 1], 0, 16);
		else if (0 == strcmp("-c", argv[i]))
			count = simple_strtoul(argv[i + 1], 0, 10);
		else
			flags = 1;
		if (flags != 0)
			goto ERROR;
	}

	ddrt_reset_ddrt();
	ddrt_set_config(mode, pattern, mem_cfg,
			reversed_lower, reversed_higher,
			loop_num, burst_num,
			start_addr, offset_addr,
			k_data, data0, data1, data2, data3);
	for (i = 0; i < count; ++i) {
		result = ddrt_pressure_test();
		if (DDRT_SUCCESS == result) {
			++success_count;
			printf("ddrp success at <%d> times.\n", i + 1);
		} else {
			++fail_count;
			printf("ddrp failed at <%d> times ...\n", i + 1);
		}
	}

	printf("ddrp result: success %u times, failed %u times.\n",
			success_count, fail_count);
	return 0;
ERROR:
	print_usage();
	return -1;
}

U_BOOT_CMD(ddrp, CONFIG_SYS_MAXARGS, 0, do_ddr_pressure,
		"ddrp training tools v200", "");
