/**
 * ddr_training_custom.c
 *
 * Copyright (c) 2009-2014, HiSilicon Technologies Co., Ltd.
 * All rights reserved.
 *
 * DDR training custom implement.
 */

#include <ddr_interface.h>
#include "ddr_training_impl.h"

#define DDRT_TEST_CLK                  0x200300d8
#define DDRT_ENABLE                    0x20120004
/**
 * ddr_cmd_prepare_copy
 * @void
 *
 * Do some prepare before copy code from DDR to SRAM.
 * Keep empty when nothing to do.
 */
void ddr_cmd_prepare_copy(void) { return; }

/**
 * ddr_ddrt_prepare_custom
 * @void
 *
 * Open DDRT clock and enable DDRT function.
 */
void ddr_ddrt_prepare_custom(void)
{
	REG_WRITE(REG_READ(DDRT_TEST_CLK) | 0x8, DDRT_TEST_CLK);
	REG_WRITE(REG_READ(DDRT_ENABLE) | 0x4, DDRT_ENABLE);
}
/**
 * ddr_cmd_site_save
 * @void
 *
 * Save site before DDR training command execute .
 * Keep empty when nothing to do.
 */
void ddr_cmd_site_save(void) { return; }

/**
 * ddr_cmd_site_restore
 * @void
 *
 * Restore site after DDR training command execute.
 * Keep empty when nothing to do.
 */
void ddr_cmd_site_restore(void) { return; }
