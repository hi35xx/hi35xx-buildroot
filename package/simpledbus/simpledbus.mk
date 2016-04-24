################################################################################
#
# simpledbus
#
################################################################################

SIMPLEDBUS_VERSION = master
SIMPLEDBUS_SITE = $(call github,xuhuashan,simpledbus,$(SIMPLEDBUS_VERSION))
SIMPLEDBUS_DEPENDENCIES = luainterpreter dbus
SIMPLEDBUS_LICENSE = LGPL
SIMPLEDBUS_LICENSE_FILES = COPYING COPYING.LESSER

define SIMPLEDBUS_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
	LUA=$(BR2_PACKAGE_PROVIDES_LUAINTERPRETER) \
	LUA_ABIVER=$(LUAINTERPRETER_ABIVER) \
	$(MAKE) -C $(@D)
endef

define SIMPLEDBUS_INSTALL_TARGET_CMDS
	$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
	LUA=$(BR2_PACKAGE_PROVIDES_LUAINTERPRETER) \
	LUA_ABIVER=$(LUAINTERPRETER_ABIVER) \
	$(MAKE) -C $(@D) PREFIX=/usr DESTDIR=$(TARGET_DIR) install
endef

$(eval $(generic-package))
