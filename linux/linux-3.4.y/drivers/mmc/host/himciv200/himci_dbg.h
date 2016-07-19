/******************************************************************************
 *    Copyright (C) 2014 Hisilicon STB Development Dept
 *    All rights reserved.
 * ***
 *    Create by Czyong
 *
******************************************************************************/
#ifndef HIMCI_DBG_H
#define HIMCI_DBG_H

int himci_dbg_init(int devid);

int himci_dbg_rw(int devid, int is_write, u32 ofblk, u32 nrblk);

#endif
