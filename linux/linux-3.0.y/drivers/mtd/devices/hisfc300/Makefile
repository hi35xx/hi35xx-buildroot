#
# drivers/devices/hisfc300/Makefile
#

obj-$(CONFIG_MTD_HISFC300)     += hisfc300_spi_ids.o hisfc300.o

ifdef CONFIG_ARCH_GODBOX
obj-$(CONFIG_MTD_HISFC300)     += hisfc300_godbox.o
endif

ifdef CONFIG_ARCH_GODEYES
obj-$(CONFIG_MTD_HISFC300)     += hisfc300_godeyes.o
endif

ifdef CONFIG_ARCH_GODNET
obj-$(CONFIG_MTD_HISFC300)     += hisfc300_godnet.o
endif

ifdef CONFIG_ARCH_GODCUBE
obj-$(CONFIG_MTD_HISFC300)     += hisfc300_godcube.o
endif
