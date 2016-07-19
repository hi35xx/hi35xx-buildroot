/******************************************************************************
 *    Copyright (C) 2014 Hisilicon STB Development Dept
 *    All rights reserved.
 * ***
 *    Create by Czyong
 *
******************************************************************************/
#ifndef HIMCI_ACL_H
#define HIMCI_ACL_H

int himci_acl_init(int devid);

int himci_acl_rw(int devid, int is_write, u32 ofblk, u32 nrblk);

extern char *get_blkdevparts(void);
#endif

