#include "sockioctl.h"

/* debug code */
static int set_suspend(int eth_n)
{
	return 0;
}

/* debug code */
static int set_resume(int eth_n)
{
	/* higmac_dev_driver.resume(&higmac_platform_device); */
	return 0;
}

static int hw_states_read(struct seq_file *m, void *v)
{
	return 0;
}

static struct proc_dir_entry *higmac_proc_root;

#define proc_open(name)	\
static int proc_open_##name(struct inode *inode, struct file *file) \
{ \
	return single_open(file, name, PDE_DATA(inode)); \
} \

proc_open(hw_states_read);

static struct proc_file {
	char *name;
	const struct file_operations ops;

} proc_file[] = {
	{
		.name = "hw_stats",
		.ops = {
			.open           = proc_open_hw_states_read,
			.read           = seq_read,
			.llseek         = seq_lseek,
			.release        = single_release,
		},
	}
};

/* /proc/higmac/
 *	|---hw_stats
 *	|---skb_pools
 */
void higmac_proc_create(void)
{
	int i;

	higmac_proc_root = proc_mkdir("higmac", NULL);
	if (!higmac_proc_root)
		return;

	for (i = 0; i < ARRAY_SIZE(proc_file); i++) {
		struct proc_dir_entry *entry;

		entry = proc_create(proc_file[i].name, 0, higmac_proc_root,
				    &proc_file[i].ops);
		if (!entry)
			pr_err("failed to create %s\n", proc_file[i].name);
	}
}

void higmac_proc_destroy(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(proc_file); i++)
		remove_proc_entry(proc_file[i].name, higmac_proc_root);

	remove_proc_entry("higmac", NULL);
}

int higmac_ioctl(struct net_device *ndev, struct ifreq *rq, int cmd)
{
	struct higmac_netdev_local *priv = netdev_priv(ndev);
	struct pm_config pm_config;
	int val = 0;

	switch (cmd) {
	case SIOCSETPM:
		if (copy_from_user(&pm_config, rq->ifr_data, sizeof(pm_config)))
			return -EFAULT;
		return pmt_config(ndev, &pm_config);

	case SIOCSETSUSPEND:
		if (copy_from_user(&val, rq->ifr_data, sizeof(val)))
			return -EFAULT;
		return set_suspend(val);

	case SIOCSETRESUME:
		if (copy_from_user(&val, rq->ifr_data, sizeof(val)))
			return -EFAULT;
		return set_resume(val);

	default:
		if (!netif_running(ndev))
			return -EINVAL;

		if (!priv->phy)
			return -EINVAL;

		return phy_mii_ioctl(priv->phy, rq, cmd);
	}
	return 0;
}
