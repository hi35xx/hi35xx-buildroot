#include <common.h>

#include "hi3535_ddr_training.h"

static int print_prebit_result(struct training_data *training);

#define	PRINT_AC_VAL(name, wodows_width, center_val) \
printf("%-8s    %16-d%-12d\n", (name), (wodows_width), (center_val))
/*****************************************************************************/
int hi3535_ddr_training_result(unsigned int TRAINING_ADDR)
{
	struct training_data *ddr = (struct training_data *)TRAINING_ADDR;

	if (ddr->flag == DDR_DQSTRAIN)
		print_prebit_result(ddr);

	return 0;
}
/*****************************************************************************/
#define PRINT_PREBIT(level, bit, result, dqs, dq, count) do {\
	printf("%-4d", bit);\
	for (k = 0; k < level; k++) {\
		if ((result >> k) & 0x1) {\
				printf("%-3s", "-");\
		} else {\
				printf("%-3s", "X");\
		} \
	} \
	printf(" 0x%-12x%-4d%-4d%-4d\n", result, dqs, dq, count);\
} while (0)

#define PRINT_PREBIT1(level, bit, result, best, clk, addr, count) do {\
	printf("%-4d", bit);\
	for (k = 0; k < level; k++) {\
		if ((result >> k) & 0x1) {\
				printf("%-3s", "-");\
		} else {\
				printf("%-3s", "X");\
		} \
	} \
	printf(" 0x%-12x%-5d%-4d%-4d%-4d\n", result, best, clk, addr, count);\
} while (0)

#define PRINT_PREBIT_TMP2(level) do {\
	printf("%-4s", "DQ");\
	for (k = 0; k < level; k++) {\
			printf("%-3d", k);\
	} \
	printf(" %-12s  %-4s%-4s%-4s\n", "HEX_VALUE", "DQS", "DQ", "WIN");\
} while (0)

#define PRINT_PREBIT_TMP1(level) do {\
	printf("%-4s", "DQ");\
	for (k = 0; k < level; k++) {\
			printf("%-3d", k);\
	} \
	printf(" %-12s  %-4s%-4s%-4s\n", "HEX_VALUE", "DQph", "DQ", "WIN");\
} while (0)

#define PRINT_PREBIT_TMP3(level) do {\
	printf("%-4s", "DQ");\
	for (k = 0; k < level; k++) {\
			printf("%-3d", k);\
	} \
	printf(" %-12s  %-5s%-4s%-4s%-4s\n", "HEX_VALUE", \
			"BEST", "CLK", "ADDR", "WIN");\
} while (0)


/*****************************************************************************/
static int print_prebit_result(struct training_data *training)
{
	unsigned int i, k, j;
	unsigned int cnt_tmp, dq_tmp, dqs_tmp;
	unsigned int dq_num;

	unsigned int tmp = io_read(REG_DDRC_BASE + DDRC_PHYINITCTRL);

#ifdef WRITE_DESKEW
	if (tmp & 0x200) {	/*	Pub write enable	*/
		unsigned int wdqs_phase_left, wdqs_phase_right;
		printf("Pub Training result.The write window of prebit-deskew\n");
		printf("--------------------------------------------------------------------------------\n");
		printf("DQS         LEFT_BOUNDARY    RIGHT_BOUNDARY    WODOWS_WIDTH    CENTER_VAL       \n");
		for (i = 0; i < BYTEWIDTH; i++) {
			tmp = io_read(REG_DDRC_BASE + DDRC_DXNWDBOUND(i));
			wdqs_phase_left = (tmp>>16) & 0x1f;
			wdqs_phase_right = tmp & 0x1f;
			printf("%-16d%-17d%-18d%-16d%-13d\n", i,
				wdqs_phase_left, wdqs_phase_right,
				(wdqs_phase_right - wdqs_phase_left)>>2,
				((wdqs_phase_right - wdqs_phase_left)>>2)
				+ wdqs_phase_left);
		}
		printf("--------------------------------------------------------------------------------\n\n");
	}

	printf("Boot Training result. The write window of prebit-deskew\n");


	printf("--------------------------------------------------------------------------------\n");
		PRINT_PREBIT_TMP1(WRDQ_LEVEL);
		for (j = 0; j < BYTEWIDTH; j++) {
			dqs_tmp = training->wrdqs_val[j];
			for (i = 0; i < 8; i++) {

				dq_num = j*8 + i;
				cnt_tmp = training->wr_bit_best[dq_num]>>16;
				dq_tmp =  training->wr_bit_best[dq_num]
					& 0xffff;
				PRINT_PREBIT(WRDQ_LEVEL, dq_num,
					training->wr_bit_result[dq_num],
					dqs_tmp, dq_tmp, cnt_tmp);
			}
		}
	printf("--------------------------------------------------------------------------------\n\n");

	printf("--------------------------------------------------------------------------------\n");
	for (i = 0; i < BITWIDTH; i += 8) {
		int j;
		printf("DQ%-2d~DQ%-2d    ", i, i + 7);
		for (j = 0; j < 8; j++)
			printf("0x%-2d    ", training->wrdq_val[i + j]);
		printf("\n");
	}
	printf("--------------------------------------------------------------------------------\n\n");
#endif /* WRITE_DESKEW */

#ifdef READ_DESKEW
	if (tmp & 0x100) {	/*	Pub read enable	*/
		unsigned int rdqsqdl_left, rdqsqdl_right;
		printf("Pub Training result.The read window of prebit-deskew\n");
		printf("--------------------------------------------------------------------------------\n");
		printf("DQS         LEFT_BOUNDARY    RIGHT_BOUNDARY    WODOWS_WIDTH    CENTER_VAL       \n");
		for (i = 0; i < BYTEWIDTH; i++) {
			tmp = io_read(REG_DDRC_BASE + DDRC_DXNRDBOUND(i));
			rdqsqdl_left = (tmp>>16) & 0x7f;
			rdqsqdl_right = tmp & 0x7f;
			printf("%-16d%-17d%-18d%-16d%-13d\n", i, rdqsqdl_left,
					rdqsqdl_right,
					(rdqsqdl_right - rdqsqdl_left)>>2,
					((rdqsqdl_right - rdqsqdl_left)>>2)
					+ rdqsqdl_left);
		}
		printf("--------------------------------------------------------------------------------\n\n");
	}

	printf("Boot Training result.The read window of prebit-deskew\n");
	printf("--------------------------------------------------------------------------------\n");
		PRINT_PREBIT_TMP2(RDDQ_LEVEL);
		for (j = 0; j < BYTEWIDTH; j++) {
			dqs_tmp = training->rddqs_val[j];
			for (i = 0; i < 8; i++) {
				dq_num = j*8 + i;
				cnt_tmp = training->rd_bit_best[dq_num]>>16;
				dq_tmp = training->rd_bit_best[dq_num]
					& 0xffff;
				PRINT_PREBIT(RDDQ_LEVEL, dq_num,
						training->rd_bit_result[dq_num],
						dqs_tmp, dq_tmp, cnt_tmp);
			}
		}
	printf("--------------------------------------------------------------------------------\n\n");

	printf("--------------------------------------------------------------------------------\n");
	for (i = 0; i < BITWIDTH; i += 8) {
		int j;
		printf("DQ%-2d~DQ%-2d    ", i, i + 7);
		for (j = 0; j < 8; j++)
			printf("0x%-2d    ", training->rddq_val[i + j]);
		printf("\n");
	}
	printf("--------------------------------------------------------------------------------\n\n");
#endif /* READ_DESKEW */

	return 0;
}
/*****************************************************************************/
