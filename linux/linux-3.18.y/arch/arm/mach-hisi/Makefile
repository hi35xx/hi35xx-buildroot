#
# Makefile for Hisilicon processors family
#

CFLAGS_platmcpm.o	:= -march=armv7-a

obj-y	+= hisilicon.o
obj-$(CONFIG_MCPM)		+= platmcpm.o
ifdef CONFIG_ARCH_HI3519
obj-$(CONFIG_PMC)		+= pmc_hi3519.o
endif
ifdef CONFIG_ARCH_HI3519V101
obj-$(CONFIG_PMC)		+= pmc_hi3519v101.o
endif
ifdef CONFIG_ARCH_HI3516AV200
obj-$(CONFIG_PMC)		+= pmc_hi3516av200.o
endif
ifdef CONFIG_ARCH_HI3559
obj-$(CONFIG_PMC)		+= pmc_hi3559.o
obj-$(CONFIG_PM)		+= pwr_hi3559.o
endif
ifdef CONFIG_ARCH_HI3556
obj-$(CONFIG_PMC)		+= pmc_hi3559.o
obj-$(CONFIG_PM)		+= pwr_hi3559.o
endif
obj-$(CONFIG_ARCH_HI3536C)	+= pmc_hi3536c.o
obj-$(CONFIG_SMP)		+= platsmp.o hotplug.o headsmp.o
obj-$(CONFIG_PM)		+= pm.o
obj-$(CONFIG_HISI_SNAPSHOT_BOOT) += fastboot/
ifdef CONFIG_ARCH_MULTI_V5
obj-$(CONFIG_HISI_SNAPSHOT_BOOT) += hibernate.o swsusp.o
else
obj-$(CONFIG_HISI_SNAPSHOT_BOOT) += hibernate.o swsusp.o cpu_helper_a7.o
endif
