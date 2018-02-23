################################################################################
#
# hiawatha
#
################################################################################

HIAWATHA_VERSION = 10.6
HIAWATHA_SITE = https://www.hiawatha-webserver.org/files
HIAWATHA_DEPENDENCIES = zlib
HIAWATHA_LICENSE = GPL-2.0
HIAWATHA_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_HIAWATHA_SSL),y)
HIAWATHA_CONF_OPTS += -DUSE_SYSTEM_MBEDTLS=ON
HIAWATHA_DEPENDENCIES += mbedtls
else
HIAWATHA_CONF_OPTS += -DENABLE_TLS=OFF
endif

HIAWATHA_CONF_OPTS += \
	-DENABLE_TOOLKIT=ON \
	-DENABLE_XSLT=OFF \
	-DCONFIG_DIR=/etc/hiawatha \
	-DLOG_DIR=/var/log/hiawatha \
	-DPID_DIR=/var/run \
	-DWEBROOT_DIR=/var/www/hiawatha \
	-DWORK_DIR=/var/lib/hiawatha

# fix rebuild (hiawatha makefile errors out if /var/lib/hiawatha is a symlink)
define HIAWATHA_REMOVE_VAR_LIB_HIAWATHA
	rm -rf $(TARGET_DIR)/var/lib/hiawatha
endef

HIAWATHA_POST_BUILD_HOOKS += HIAWATHA_REMOVE_VAR_LIB_HIAWATHA

define HIAWATHA_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D package/hiawatha/S50hiawatha \
		$(TARGET_DIR)/etc/init.d/S50hiawatha

	mkdir -p $(TARGET_DIR)/var/lib
	rm -rf $(TARGET_DIR)/var/lib/hiawatha
	ln -sf /tmp/hiawatha $(TARGET_DIR)/var/lib/hiawatha
endef

define HIAWATHA_INSTALL_INIT_SYSTEMD
	mkdir -p $(TARGET_DIR)/var/lib/hiawatha
endef

$(eval $(cmake-package))
