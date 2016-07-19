/******************************************************************************
 *    COPYRIGHT (C) 2013 by hisi. Hisilicon
 *    All rights reserved.
 *
******************************************************************************/
#include <asm/arch/platform.h>
#include <config.h>
#include "ddr_dataeye_training.h"

#define  REG_BASE_MDDRC   0x20111000
#define  REG_BASE_SYSCTRL 0x20050000

#define  REG_READ(addr)        (*(volatile unsigned int *)(addr))
#define  REG_WRITE(val, addr)  ((*(volatile unsigned *) (addr)) = (val))

extern void DDRPHY_WRLV_TRAIN_ROUTE(void);
extern int DDRPHY_GATE_TRAIN_ROUTE(void);
extern int DDRPHY_DATA_TRAIN_ROUTE(unsigned int);

/******************************************************************************/
static void update_ddrphy_timing(void)
{
	unsigned int tmp;
	tmp = REG_READ(REG_BASE_DDRPHY + 0x70);
	tmp |= 0x100000;
	REG_WRITE(tmp, REG_BASE_DDRPHY + 0x70);
	tmp &= 0xffefffff;
	REG_WRITE(tmp, REG_BASE_DDRPHY + 0x70);

	tmp = REG_READ(REG_BASE_DDRPHY + 0x04);
	tmp |= (1<<15);
	REG_WRITE(tmp, REG_BASE_DDRPHY + 0x04);
	tmp &= 0xffff7fff;
	REG_WRITE(tmp, REG_BASE_DDRPHY + 0x04);

}

static void bdl_add(unsigned int *raw, unsigned int val)
{
	if (((*raw) + val) > 0x1f)
		*raw = 0x1f;
	else
		*raw += val;
}

static void bdl_sub(unsigned int *raw, unsigned int val)
{
	if ((*raw) > val)
		*raw -= val;
	else
		*raw = 0;
}

int ddrphy_train_route(void)
{
	unsigned int auto_ref_timing;
	unsigned int mr1_raw;
	unsigned int ddr_byte_num;
	unsigned int wdqs_phase[4];
	unsigned int wdq_phase[4];
	unsigned int wdqs_bdl_raw[4];
	unsigned int wdqs_bdl_new[4];
	unsigned int wdq_bdl[8];
	unsigned int dqsoe_bdl[4];
	unsigned int dqoe_bdl[4];
	unsigned int wdm_bdl;
	unsigned int write_leveling_retrain;
	unsigned int dqsg_phase[4];
	unsigned int i, j, val, tmp;
	unsigned int result;

	/*
	 * this function is called just after DDR Init
	 * it will trainning the ddrphy to find the best wdqs and gate
	 */

	auto_ref_timing = REG_READ(REG_BASE_MDDRC + 0x88);
	REG_WRITE(auto_ref_timing & 0xfffff800, REG_BASE_MDDRC + 0x88);
	ddr_byte_num = (REG_READ(REG_BASE_MDDRC + 0x50) >> 4) & 0x3;
	ddr_byte_num = ddr_byte_num << 1;

	if (!(REG_READ(REG_BASE_SYSCTRL + REG_SC_DDRT0) & 0x10)) {

		mr1_raw = REG_READ(REG_BASE_DDRPHY + 0x64) >> 16;
		/* Set A7 = 1, Rank = 0, Cmd = MRS */
		tmp = ((mr1_raw | 0x80) << 16) + 0x12;
		REG_WRITE(tmp, REG_BASE_MDDRC + 0x210);
		REG_WRITE(0x1, REG_BASE_MDDRC + 0x218);
		REG_WRITE(0x1, REG_BASE_MDDRC + 0x00c);

		for (i = 0; i < ddr_byte_num; i++)
			wdqs_bdl_raw[i] =
				REG_READ(REG_BASE_DDRPHY + 0x230 + (i << 7))
				& 0x1f;

		DDRPHY_WRLV_TRAIN_ROUTE();

		write_leveling_retrain = 0;
		for (i = 0; i < ddr_byte_num; i++) {
			wdqs_bdl_new[i] = REG_READ(REG_BASE_DDRPHY
					+ 0x230 + (i << 7)) & 0x1f;
			if (wdqs_bdl_new[i] <= 4) {
				tmp = REG_READ(REG_BASE_DDRPHY + 0x230 \
						+ (i << 7));
				wdqs_phase[i] = (tmp >> 8) & 0xf;
				if (wdqs_phase[i] > 1) {
					wdqs_phase[i] = wdqs_phase[i] - 1;
					tmp = (tmp & 0x1f)
						+ (wdqs_phase[i] << 8);
					REG_WRITE(tmp, REG_BASE_DDRPHY \
							+ 0x230 + (i << 7));
					tmp = REG_READ(REG_BASE_DDRPHY \
							+ 0x234 + (i << 7));
					wdq_phase[i] = (tmp >> 8) & 0x1f;
					wdq_phase[i] = wdq_phase[i] - 1;
					tmp = wdq_phase[i] << 8;
					REG_WRITE(tmp, REG_BASE_DDRPHY \
							+ 0x234 + (i << 7));
					write_leveling_retrain = 1;
				}
			} else if (wdqs_bdl_new[i] >= 16) {
				tmp = REG_READ(REG_BASE_DDRPHY + 0x230 \
						+ (i << 7));
				wdqs_phase[i] = (tmp >> 8) & 0xf;
				if (wdqs_phase[i] < 0xd) {
					wdqs_phase[i] = wdqs_phase[i] + 2;
					tmp = (tmp & 0x1f)
						+ (wdqs_phase[i] << 8);
					REG_WRITE(tmp, REG_BASE_DDRPHY \
							+ 0x230 + (i << 7));
					tmp = REG_READ(REG_BASE_DDRPHY \
							+ 0x234 + (i << 7));
					wdq_phase[i] = (tmp >> 8) & 0x1f;
					if (wdq_phase[i] <= 0x1d) {
						wdq_phase[i] = wdq_phase[i] + 2;
						tmp = wdq_phase[i] << 8;
						REG_WRITE(tmp, REG_BASE_DDRPHY \
							+ 0x234 + (i << 7));
					}
					write_leveling_retrain = 1;
				}
			}
		}

		if (write_leveling_retrain == 1)
			DDRPHY_WRLV_TRAIN_ROUTE();

		/*  Set A7 = 0, Rank = 0, Cmd = MRS */
		tmp = ((mr1_raw & 0xff7f) << 16) + 0x12;
		REG_WRITE(tmp, REG_BASE_MDDRC + 0x210);
		REG_WRITE(0x1, REG_BASE_MDDRC + 0x218);
		REG_WRITE(0x1, REG_BASE_MDDRC + 0x00c);
		REG_WRITE(0x0, REG_BASE_MDDRC + 0x218);
		REG_WRITE(0x0, REG_BASE_MDDRC + 0x00c);

		for (i = 0; i < ddr_byte_num; i++) {
			wdqs_bdl_new[i] = REG_READ(REG_BASE_DDRPHY \
					+ 0x230 + (i << 7)) & 0x1f;
			if (wdqs_bdl_new[i] == 0x1f) {
				result = (i<<16) + 0x1;
				return result;
			}
		}

		for (i = 0; i < ddr_byte_num; i++) {
			wdqs_bdl_new[i] = REG_READ(REG_BASE_DDRPHY
					+ 0x230 + (i << 7)) & 0x1f;
			tmp = REG_READ(REG_BASE_DDRPHY + 0x210 + (i << 7));
			wdq_bdl[0] = (tmp >> 0) & 0xff;
			wdq_bdl[1] = (tmp >> 8) & 0xff;
			wdq_bdl[2] = (tmp >> 16) & 0xff;
			wdq_bdl[3] = (tmp >> 24) & 0xff;
			tmp = REG_READ(REG_BASE_DDRPHY + 0x214 + (i << 7));
			wdq_bdl[4] = (tmp >> 0) & 0xff;
			wdq_bdl[5] = (tmp >> 8) & 0xff;
			wdq_bdl[6] = (tmp >> 16) & 0xff;
			wdq_bdl[7] = (tmp >> 24) & 0xff;
			wdm_bdl = REG_READ(REG_BASE_DDRPHY
					+ 0x218 + (i << 7)) & 0x1f;
			tmp = REG_READ(REG_BASE_DDRPHY
					+ 0x258 + (i << 7));
			dqsoe_bdl[i] = (tmp >> 16) & 0x1f;
			dqoe_bdl[i] = tmp & 0x1f;

			if (wdqs_bdl_new[i] > wdqs_bdl_raw[i]) {
				val = wdqs_bdl_new[i] - wdqs_bdl_raw[i];
				for (j = 0; j < 8; j++)
					bdl_add(&wdq_bdl[j], val);

				bdl_add(&wdm_bdl, val);
				bdl_add(&dqsoe_bdl[i], val);
				bdl_add(&dqoe_bdl[i], val);
			} else if (wdqs_bdl_new[i] < wdqs_bdl_raw[i]) {
				val = wdqs_bdl_raw[i] - wdqs_bdl_new[i];
				for (j = 0; j < 8; j++)
					bdl_sub(&wdq_bdl[j], val);

				bdl_sub(&wdm_bdl, val);
				bdl_sub(&dqsoe_bdl[i], val);
				bdl_sub(&dqoe_bdl[i], val);
			}

			tmp = (wdq_bdl[3] << 24) + (wdq_bdl[2] << 16)
				+ (wdq_bdl[1] << 8) + wdq_bdl[0];
			REG_WRITE(tmp, REG_BASE_DDRPHY + 0x210 + (i << 7));
			tmp = (wdq_bdl[7] << 24) + (wdq_bdl[6] << 16)
				+ (wdq_bdl[5] << 8) + wdq_bdl[4];
			REG_WRITE(tmp, REG_BASE_DDRPHY + 0x214 + (i << 7));
			REG_WRITE(wdm_bdl, REG_BASE_DDRPHY + 0x218 + (i << 7));

			tmp = (dqsoe_bdl[i] << 16) + dqoe_bdl[i];
			REG_WRITE(tmp, REG_BASE_DDRPHY + 0x258 + (i << 7));
		}

		update_ddrphy_timing();
	} /* write leveling end */

	REG_WRITE(0x8000, REG_BASE_DDRPHY + 0x04);
	REG_WRITE(0x0000, REG_BASE_DDRPHY + 0x04);

	if (!(REG_READ(REG_BASE_SYSCTRL + REG_SC_DDRT0) & 0x100)) {
		for (i = 0; i < ddr_byte_num; i++) {
			for (dqsg_phase[i] = 60;
					dqsg_phase[i] > 16;
					dqsg_phase[i] -= 4) {
				REG_WRITE(dqsg_phase[i] << 8,
					REG_BASE_DDRPHY + 0x23c + (i << 7));
				update_ddrphy_timing();
				if (DDRPHY_DATA_TRAIN_ROUTE(0xff
						<< (i << 3)) == 0)
					break;
			}
			if (dqsg_phase[i] <= 16) {
				result = (i<<16) + 0x2;
				return result;
			} else {
				dqsg_phase[i] -= 8;
				REG_WRITE(dqsg_phase[i] << 8,
					REG_BASE_DDRPHY + 0x23c + (i << 7));
			}
		}

		update_ddrphy_timing();

		if (DDRPHY_GATE_TRAIN_ROUTE() != 0) {
			result = 0x2;
			return result;
		}

		for (i = 0; i < ddr_byte_num; i++) {
			dqsg_phase[i] -= 8;
			tmp = REG_READ(REG_BASE_DDRPHY + 0x23c + (i << 7));
			tmp &= 0xffff00ff;
			tmp |= dqsg_phase[i] << 8;
			REG_WRITE(tmp, REG_BASE_DDRPHY + 0x23c + (i << 7));
		}

		update_ddrphy_timing();
	} else { /* Gate Train End */
		if (REG_READ(REG_BASE_DDRPHY + 8) & 0x20) {
			result = 0x3;
			return result;
		}

	}
	REG_WRITE(auto_ref_timing, REG_BASE_MDDRC + 0x88);

	return 0;
}

/******************************************************************************/
