#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/suspend.h>

#include "fastboot_pm.h"

/*
 * ToDo
 *  add uevent
 */

#if defined(CONFIG_PM) && defined(CONFIG_SUSPEND)

#define PM_EVENT_HELPER_LEN (32)

static char pm_umh_helper[PM_EVENT_HELPER_LEN] = { 0, };
static struct ctl_table_header *ctl;

static struct ctl_table pm_umh_ctl[] = {
	{
		.procname	= "pm_notifier",
		.data		= &pm_umh_helper,
		.maxlen		= PM_EVENT_HELPER_LEN,
		.mode		= 0644,
		.proc_handler	= proc_dostring,
	},
	{}
};

static struct ctl_path pm_umh_root[] = {
	{
		.procname	= "kernel",
	},
	{}
};

int fastboot_pm_call_umh(char *event_str, unsigned long event)
{
	char *argv[3] = {pm_umh_helper, event_str, NULL};
	char *envp[3] = {"HOME=/", NULL, NULL };
	int ret;

	if (!pm_umh_helper[0])
		return 0;

	envp[1] = "PATH=/usr/bin:/usr/sbin:/bin:/sbin";
	ret = call_usermodehelper_fns_force(argv[0], argv, envp,
			UMH_WAIT_PROC, NULL, NULL, NULL);
	if (ret < 0)
		pr_err("%s: helper failed(%d)\n", __func__, ret);

	return 0;
}
EXPORT_SYMBOL_GPL(fastboot_pm_call_umh);

static int __init fastboot_pm_umh_init(void)
{
	ctl = register_sysctl_paths(pm_umh_root, pm_umh_ctl);
	fastboot_hibernation_misc_init();
	return 0;
}


late_initcall(fastboot_pm_umh_init);
#endif /* CONFIG_PM && CONFIG_SUSPEND */
