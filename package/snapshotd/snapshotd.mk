################################################################################
#
# snapshotd
#
################################################################################

SNAPSHOTD_DEPENDENCIES = live555
SNAPSHOTD_LICENSE = GPLv2+

define SNAPSHOTD_BUILD_CMDS
	$(TARGET_CXX) $(TARGET_CXXFLAGS) $(TARGET_LDFLAGS) \
		-I$(STAGING_DIR)/usr/include/BasicUsageEnvironment \
		-I$(STAGING_DIR)/usr/include/UsageEnvironment \
		-I$(STAGING_DIR)/usr/include/groupsock \
		-I$(STAGING_DIR)/usr/include/liveMedia \
		-lBasicUsageEnvironment -lUsageEnvironment \
		-lgroupsock -lliveMedia \
		package/snapshotd/snapshotd.cpp \
		-o $(@D)/snapshotd
endef

define SNAPSHOTD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/snapshotd $(TARGET_DIR)/usr/bin/snapshotd
endef

$(eval $(generic-package))
