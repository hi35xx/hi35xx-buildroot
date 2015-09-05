/******************************************************************************
 *    COPYRIGHT (C) 2012 Hisi. Hisilicon
 *    All rights reserved.
 *
******************************************************************************/

#ifndef DDR_TRAININGH
#define DDR_TRAININGH

struct regval_t {
	unsigned int reg;
	unsigned int val;
};

struct ddrtr_result_t {
	unsigned int count;
#define DDR_TRAINING_MAX_VALUE       20
	struct regval_t reg[20];
	char data[1024];
};

struct ddrtr_param_t {
#define DDRTR_PARAM_TRAINING         1
#define DDRTR_PARAM_PRESSURE         2
#define DDRTR_PARAM_ADDRTRAIN        3
	unsigned int cmd;
	union {
		struct {
			unsigned int start;
			unsigned int length;
		} train;
		struct {
			unsigned int mode;
			unsigned int codetype;
			unsigned int burstnum;
			unsigned int count;
			unsigned int changebit;
		} pressure;
	};
};

typedef struct ddrtr_result_t *(*ddrtr_t)(struct ddrtr_param_t *param);

#endif /* DDR_TRAININGH */

