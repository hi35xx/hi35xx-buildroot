################################################################################
#
# live-streamer
#
################################################################################

LIVE_STREAMER_VERSION = master
LIVE_STREAMER_SITE = $(call github,hi35xx,live-streamer,$(LIVE_STREAMER_VERSION))
LIVE_STREAMER_DEPENDENCIES = host-pkgconf libev dbus-cpp live555
LIVE_STREAMER_INSTALL_STAGING = YES
LIVE_STREAMER_LICENSE = GPLv2+
LIVE_STREAMER_LICENSE_FILES = COPYING

LIVE_STREAMER_CONF_OPTS = --with-live555

ifeq ($(BR2_PACKAGE_HIMPP_HI3518),y)
LIVE_STREAMER_DEPENDENCIES += himpp-hi3518
LIVE_STREAMER_CONF_OPTS += --with-hi3518mpp
endif

ifeq ($(BR2_PACKAGE_HIMPP_HI3520),y)
LIVE_STREAMER_DEPENDENCIES += himpp-hi3520
LIVE_STREAMER_CONF_OPTS += --with-hi3520mpp
endif

define LIVE_STREAMER_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/live-streamer/S80streamer \
		$(TARGET_DIR)/etc/init.d/S80streamer
endef

$(eval $(autotools-package))
