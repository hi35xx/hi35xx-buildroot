
extern int fastboot_pm_call_umh(char *event_str, unsigned long event);

extern void __exit fastboot_hibernation_misc_exit(void);
extern int __init fastboot_hibernation_misc_init(void);

extern int fastboot_hibernate_fs_suspend(void);
extern int fastboot_hibernate_fs_resume(void);
