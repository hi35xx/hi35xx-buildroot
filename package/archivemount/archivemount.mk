################################################################################
#
# archivemount
#
################################################################################

ARCHIVEMOUNT_VERSION = 0.8.5
ARCHIVEMOUNT_SITE = http://www.cybernoia.de/software/archivemount/
ARCHIVEMOUNT_DEPENDENCIES = libfuse libarchive
ARCHIVEMOUNT_LICENSE = LGPL
ARCHIVEMOUNT_LICENSE_FILES = COPYING

$(eval $(autotools-package))
