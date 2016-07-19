/*****************************************************************************
 *    Copyright (c) 2014 by Hisilicon
 *    All rights reserved.
 *****************************************************************************/

/*****************************************************************************/
/*
  ESMT spi nand don't support QE enable.
*/
static int spi_nand_esmt_qe_enable(struct hisnfc_op *spi)
{
	return 0;
}

