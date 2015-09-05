#ifdef CONFIG_DDR_TRAINING
#ifndef _CMD_DDRTRAINING_H
#define _CMD_DDRTRAINING_H
/*
 * _ddrtraining_flag and _ddrtraining_flag_offset be defined
 * at arch/arm/cpu/{...}/start.S
 */
extern unsigned int _ddrtraining_flag;
extern unsigned int _ddrtraining_flag_offset;
extern unsigned int ssn_vpss_ddr_base;
extern unsigned int ssn_vdh_ddr_base;
extern unsigned int _ddroption_flag;
extern unsigned int _ddroption_flag_offset;

extern int do_ddr(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif
#endif
