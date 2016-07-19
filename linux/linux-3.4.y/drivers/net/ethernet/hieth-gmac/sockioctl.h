#ifndef _SOCKIOCTL_H_
#define _SOCKIOCTL_H_

#include <linux/sockios.h>

#define SIOCGETMODE	(SIOCDEVPRIVATE)     /* get work mode */
#define SIOCSETMODE	(SIOCDEVPRIVATE + 1) /* set work mode */
#define SIOCGETFWD	(SIOCDEVPRIVATE + 2) /* get forcing forward config */
#define SIOCSETFWD	(SIOCDEVPRIVATE + 3) /* set forcing forward config */
#define SIOCSETPM	(SIOCDEVPRIVATE + 4) /* set pmt wake up config */
#define SIOCSETSUSPEND	(SIOCDEVPRIVATE + 5) /* call dev->suspend, debug only */
#define SIOCSETRESUME	(SIOCDEVPRIVATE + 6) /* call dev->resume, debug only */

int higmac_ioctl(struct net_device *net_dev, struct ifreq *rq, int cmd);

#endif
