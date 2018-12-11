#include <linux/ioctl.h>

#ifndef __HI_REG_USER_H__
#define __HI_REG_USER_H__

struct hi_reg_handle {
	unsigned int phys_addr;
	unsigned int size;
	void *data;
	int flags;
};


#define HI_REG_BASE  'R'

#define HI_REG_READ  \
	_IOW(HI_REG_BASE, 1, struct hi_reg_handle)
#define HI_REG_WRITE  \
	_IOW(HI_REG_BASE, 2, struct hi_reg_handle)

#endif   /* __HI_REG_USER_H__ */
