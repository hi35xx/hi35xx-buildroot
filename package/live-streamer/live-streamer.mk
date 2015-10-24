################################################################################
#
# live-streamer
#
################################################################################

LIVE_STREAMER_VERSION = master
LIVE_STREAMER_SITE = $(call github,hi35xx,live-streamer,$(LIVE_STREAMER_VERSION))
LIVE_STREAMER_DEPENDENCIES = host-pkgconf dbus-cpp live555
LIVE_STREAMER_INSTALL_STAGING = YES
LIVE_STREAMER_LICENSE = GPLv2+
LIVE_STREAMER_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_HIMPP_HI3518),Y)
LIVE_STREAMER_DEPENDENCIES += himpp-hi3518
endif

LIVE_STREAMER_CONF_OPTS = \
	--with-live555=$(STAGING_DIR)/usr \
	--with-himpp=$(STAGING_DIR)/opt/himpp2

define LIVE_STREAMER_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/live-streamer/S80streamer \
		$(TARGET_DIR)/etc/init.d/S80streamer
endef

$(eval $(autotools-package))
