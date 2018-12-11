/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

void dwc_usb3_task_schedule(struct tasklet_struct *tasklet)
{
}

/**
 * Helper routine for dwc_wait_pme_thread()
 */
static void dwc_wait_for_link_up(dwc_usb3_pcd_t *pcd)
{
}

int dwc_wait_pme_thread(void *data)
{
	return 0;
}

int dwc_usb3_handle_pme_intr(dwc_usb3_device_t *dev)
{
	return 0;
}

void dwc_usb3_power_ctl(dwc_usb3_device_t *dev, int on)
{
}
