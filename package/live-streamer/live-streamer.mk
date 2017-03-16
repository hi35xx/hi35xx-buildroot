################################################################################
#
# live-streamer
#
################################################################################

LIVE_STREAMER_VERSION = e68ebd7e21f447eba6d89cc8a78a8f5a724fc620
LIVE_STREAMER_SITE = $(call github,hi35xx,live-streamer,$(LIVE_STREAMER_VERSION))
LIVE_STREAMER_DEPENDENCIES = host-pkgconf libev dbus-cpp live555
LIVE_STREAMER_DEPENDENCIES += fontconfig sdl_ttf sdl_image
LIVE_STREAMER_INSTALL_STAGING = YES
LIVE_STREAMER_LICENSE = GPLv2+
LIVE_STREAMER_LICENSE_FILES = COPYING

LIVE_STREAMER_CONF_OPTS = --with-live555

ifeq ($(BR2_PACKAGE_HIMPP_HI3518V100),y)
LIVE_STREAMER_DEPENDENCIES += himpp-hi3518v100
LIVE_STREAMER_CONF_OPTS += --with-hi3518v100mpp
endif

ifeq ($(BR2_PACKAGE_HIMPP_HI3518V200),y)
LIVE_STREAMER_DEPENDENCIES += himpp-hi3518v200
LIVE_STREAMER_CONF_OPTS += --with-hi3518v200mpp
endif

ifeq ($(BR2_PACKAGE_HIMPP_HI3520V100),y)
LIVE_STREAMER_DEPENDENCIES += himpp-hi3520v100
LIVE_STREAMER_CONF_OPTS += --with-hi3520v100mpp
endif

ifeq ($(BR2_PACKAGE_HIMPP_HI3520DV200),y)
LIVE_STREAMER_DEPENDENCIES += himpp-hi3520dv200
LIVE_STREAMER_CONF_OPTS += --with-hi3520dv200mpp
endif

define LIVE_STREAMER_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/live-streamer/S80streamer \
		$(TARGET_DIR)/etc/init.d/S80streamer
endef

$(eval $(autotools-package))
