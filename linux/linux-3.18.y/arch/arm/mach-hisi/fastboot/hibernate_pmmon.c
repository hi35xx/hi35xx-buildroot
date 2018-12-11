#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/suspend.h>

#include "fastboot_pm.h"

#if defined(CONFIG_PM) && defined(CONFIG_SUSPEND)

static char *pm_umh_str;
static int fastboot_pm_monitor(struct notifier_block *this,
		unsigned long event, void *ptr)
{
	switch (event) {
	case PM_HIBERNATION_PREPARE: /* starting pm_suspend_to_hibernation */
		pm_umh_str = "PM_HIBERNATION_PREPARE";
		break;
	case PM_POST_HIBERNATION: /* end of pm_resume_from_hibernation */
		pm_umh_str = "PM_POST_HIBERNATION";
		break;
	case PM_SUSPEND_PREPARE: /* starting pm_suspend */
		pm_umh_str = "PM_SUSPEND_PREPARE";
		break;
	case PM_POST_SUSPEND: /* end of pm_resume */
		pm_umh_str = "PM_POST_SUSPEND";
		break;
	case PM_RESTORE_PREPARE: /* pm_suspend failure ? */
		pm_umh_str = "PM_RESTORE_PREPARE";
		break;
	case PM_POST_RESTORE: /* pm_suspend failure ? */
		pm_umh_str = "PM_POST_RESTORE";
		break;
	case PM_POST_FREEZE_PROCESS:
		pm_umh_str = "PM_POST_FREEZE_PROCESS";
		break;
	case PM_THAW_PROCESS_PREPARE:
		pm_umh_str = "PM_THAW_PROCESS_PREPARE";
		/*mark by liucan*/
        //fastboot_hibernate_fs_resume();
		break;
	case PM_POST_DEVICE_SUSPEND:
		pm_umh_str = "PM_POST_DEVICE_SUSPEND";
		break;
	case PM_RESUME_DEVICE_PREPARE:
		pm_umh_str = "PM_RESUME_DEVICE_PREPARE";
		break;
	default:
		printk("%s: unknown event, ignoring...\n", __func__);
		return 0;
	}

	/* FIXME:
	 *  The callee must be on a initrd in order to handle DEVICE events.
	 *  Or it will be blocked forever because a storage was suspended.
	 */
	if (event != PM_POST_DEVICE_SUSPEND
			&& event != PM_RESUME_DEVICE_PREPARE) {
		fastboot_pm_call_umh(pm_umh_str, event);
	}

	switch (event) {
		case PM_POST_FREEZE_PROCESS:
            // mark by liucan.
			//fastboot_hibernate_fs_suspend();
			//break;
		case PM_HIBERNATION_PREPARE:
		case PM_POST_HIBERNATION:
		case PM_SUSPEND_PREPARE:
		case PM_POST_SUSPEND:
		case PM_RESTORE_PREPARE:
		case PM_POST_RESTORE:
		case PM_THAW_PROCESS_PREPARE:
		case PM_POST_DEVICE_SUSPEND:
		case PM_RESUME_DEVICE_PREPARE:
		default:
			return 0;
	}

	return 0;
}

static struct notifier_block fastboot_pm_notifier = {
	.notifier_call = fastboot_pm_monitor,
};

static int __init fastboot_pm_monitor_init(void)
{
	register_pm_notifier(&fastboot_pm_notifier);
	return 0;
}
#if 0
#ifdef MODULE
static void __exit fastboot_pm_monitor_exit(void)
{
	printk("hahaha\n");
	unregister_pm_notifier(&fastboot_pm_notifier);
}

module_init(fastboot_pm_monitor_init);
module_exit(fastboot_pm_monitor_exit);
#else
late_initcall(fastboot_pm_monitor_init);
#endif
#endif
late_initcall(fastboot_pm_monitor_init);
#endif /* CONFIG_PM && CONFIG_SUSPEND */

MODULE_LICENSE("GPL");
