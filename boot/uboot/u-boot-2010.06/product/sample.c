#include <common.h>
#include <boot/customer.h>
#include <linux/string.h>

#include "hiosd/hdmi/hi3521a/hi_hdmi.h"

#ifdef CONFIG_DDR_TRAINING_V200
#define ddrt_write(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define ddrt_read(addr) (*(volatile unsigned int *)(addr))
#endif

extern int ddr_training_second_step(void);

const int product_control(void)
{
#ifdef CONFIG_DDR_TRAINING
	ddr_training_second_step();
#endif

#ifdef CONFIG_DDR_TRAINING_V200
	unsigned long value = 0;
	char *ddrt_print = getenv("ddrt_print");
	if (ddrt_read(CUR_RDDQS_REG) != INIT_RDDQS_VALUE) {
		ddrt_write(CUR_RDDQS_REG, INIT_RDDQS_VALUE);
		ddrt_write(CUR_RDDQS_TRY, 0);
	}
	if (!ddrt_print)
		goto out;
	value = simple_strtoul(ddrt_print, 0, 10);
	if (value != 0)
		ddr_training_second_step();
out:
#endif
	return 0;
}


#if defined(CONFIG_HI3521A) || defined(CONFIG_HI3531A)
HI_U32 g_u32HDMI_update_status_flag = 0;

void update_hdmi_status(void)
{
    static HI_U32 hdmi_old_stat = 1;
    static HI_U32 hdmi_new_stat = 1;

    if(g_u32HDMI_update_status_flag == 1)
    {
        hdmi_new_stat = HI_DRV_HDMI_UpdateStatus();
        //printf("---update_hdmi_status== old:%d new:%d ---\n", hdmi_old_stat, hdmi_new_stat);
        if(hdmi_old_stat != hdmi_new_stat) 
        {
            hdmi_old_stat = hdmi_new_stat;

            if(hdmi_new_stat == 1)
	            printf("\nHDMI has been Connected.\n\n");
            else if(hdmi_new_stat == 0)
	            printf("\nHDMI has been Disconnected.\n\n");
        }
    }
  
}
#endif

