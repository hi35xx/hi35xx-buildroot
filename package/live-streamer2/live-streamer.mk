################################################################################
#
# live-streamer
#
################################################################################

LIVE_STREAMER2_VERSION = 06a3515c0fab773fcc10c5edce451a5b16392bf7
LIVE_STREAMER2_SITE = $(call github,hi35xx,live-streamer,$(LIVE_STREAMER2_VERSION))
LIVE_STREAMER2_DEPENDENCIES = host-pkgconf libev dbus-cpp live555
LIVE_STREAMER2_DEPENDENCIES += fontconfig sdl_ttf sdl_image
LIVE_STREAMER2_INSTALL_STAGING = YES
LIVE_STREAMER2_LICENSE = GPLv2+
LIVE_STREAMER2_LICENSE_FILES = COPYING

LIVE_STREAMER2_CONF_OPTS = --with-live555

ifeq ($(BR2_PACKAGE_HIMPP_HI3518V100),y)
LIVE_STREAMER2_DEPENDENCIES += himpp-hi3518v100
LIVE_STREAMER2_CONF_OPTS += --with-hi3518v100
endif

ifeq ($(BR2_PACKAGE_HIMPP_HI3518V200),y)
LIVE_STREAMER2_DEPENDENCIES += himpp-hi3518v200
LIVE_STREAMER2_CONF_OPTS += --with-hi3518v200
endif

ifeq ($(BR2_PACKAGE_HIMPP_HI3520V100),y)
LIVE_STREAMER2_DEPENDENCIES += himpp-hi3520v100
LIVE_STREAMER2_CONF_OPTS += --with-hi3520v100
endif

ifeq ($(BR2_PACKAGE_HIMPP_HI3520DV200),y)
LIVE_STREAMER2_DEPENDENCIES += himpp-hi3520dv200
LIVE_STREAMER2_CONF_OPTS += --with-hi3520dv200
endif

#define LIVE_STREAMER_INSTALL_INIT_SYSV
#	$(INSTALL) -m 0755 -D package/live-streamer/S80streamer \
#		$(TARGET_DIR)/etc/init.d/S80streamer
#endef

$(eval $(autotools-package))
