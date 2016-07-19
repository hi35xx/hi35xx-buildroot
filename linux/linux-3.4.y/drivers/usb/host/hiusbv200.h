#ifndef __HI_USB3_H__
#define __HI_USB3_H__

struct hiusb_plat_data {
	void (*start_hcd)(void __iomem *base);
	void (*stop_hcd)(void);
};
#endif
