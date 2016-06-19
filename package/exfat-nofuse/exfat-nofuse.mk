################################################################################
#
# exfat-nofuse
#
################################################################################

EXFAT_NOFUSE_VERSION = 051167ceccbe75d9d77fc4f5356f5a39a01779e8
EXFAT_NOFUSE_SITE = $(call github,dorimanx,exfat-nofuse,$(EXFAT_NOFUSE_VERSION))
EXFAT_NOFUSE_LICENSE = GPLv2
EXFAT_NOFUSE_LICENSE_FILES = LICENSE
EXFAT_NOFUSE_MODULE_MAKE_OPTS = CONFIG_EXFAT_FS=m

$(eval $(kernel-module))
$(eval $(generic-package))
