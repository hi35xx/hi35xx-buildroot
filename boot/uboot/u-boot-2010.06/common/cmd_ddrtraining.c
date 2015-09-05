#include <common.h>
#include <command.h>
#include <serial.h>
#include <nand.h>
#include <spi_flash.h>
#include <asm/io.h>
#include "cmd_ddrtraining.h"
#include "../product/ddrtraining.h"

#ifdef CONFIG_DDR_TRAINING
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
	size_t length;
	char *cmd;
	int total;
	int option = 0, tmp = 0, tmp2 = 0;
	total = 0;

	length = 0x80000;

	if (argc < 3)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "training") == 0) {
		writel(0x0, DDRT_OPTION_REG);
		tmp = atoi(argv[2]);
		tmp2 = DDRT_OPTION_CMD;
		tmp2 <<= 16;
		tmp |= tmp2;
		writel(tmp, DDRT_OPTION_REG);
		udelay(10000);
		printf("DDR trainning starting......\n");
		printf("please wait and wait!!!\n");
		udelay(10000);
		reset_cpu(0);
		udelay(10000);
		return 0;
	} else if (strcmp(cmd, "pressure") == 0) {
		if (argc == 4) {
			total = atoi(argv[3]);
			option = atoi(argv[2]);
			if (option == 2)
				ddr_training_ssn_pressure(total,
						DDRT_MODE_READ);
			else if (option == 1)
				ddr_training_ssn_pressure(total,
						DDRT_MODE_WRITE);
			else if (option == 3)
				ddr_training_ssn_pressure(total, DDRT_MODE_WR);
			else
				goto usage;
		} else
			goto usage;

		return 0;

	} else if (strcmp(cmd, "cycpressure") == 0) {
		if (argc == 4) {
			total = atoi(argv[3]);
			ddr_cyc_pressure(total, atoi(argv[2]));
		}
		return 0;

	} else if (strcmp(cmd, "result") == 0) {
		ddr_result_printf();

		return 0;

	} else
		goto usage;

usage:
	cmd_usage(cmdtp);
	return 0;
}

U_BOOT_CMD(
		ddr, CONFIG_SYS_MAXARGS, 0, do_ddr,
		"DDR training tools,usage like this:\n"
		"ddr training [option], eg. ddr training 6\n"
		"\t1. byte read\n"
		"\t2. byte write\n"
		"\t3. byte write & read\n"
		"\t4. perbit read\n"
		"\t5. perbit write\n"
		"\t6. perbit write & read\n"
		"ddr pressure [option] [times], eg. ddr pressure 3 5\n"
		"\t1. write\n"
		"\t2. read\n"
		"\t3. write & read\n",
		"training - Get the best configuration of DQS and DQ\n"
		"ddr pressure - SSN pressure one time test\n"
		"ddr result - SSN result print\n"
	  );

#endif
