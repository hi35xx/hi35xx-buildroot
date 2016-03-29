################################################################################
#
# rtl8188eu
#
################################################################################

RTL8188EU_VERSION = af12a3a198546c0e558464f75e0d36808f2c1851
RTL8188EU_SITE = $(call github,hi35xx,rtl8188eu,$(RTL8188EU_VERSION))
RTL8188EU_LICENSE = GPLv2
RTL8188EU_LICENSE_FILES = COPYING
RTL8188EU_MODULE_MAKE_OPTS = CONFIG_RTL8188EU=m
RTL8188EU_MODULE_MAKE_OPTS += "USER_EXTRA_CFLAGS = -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DCONFIG_P2P_IPS"

$(eval $(kernel-module))
$(eval $(generic-package))
