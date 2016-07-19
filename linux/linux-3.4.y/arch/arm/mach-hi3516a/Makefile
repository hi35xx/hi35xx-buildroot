#
# Makefile for the linux kernel.
#


KBUILD_AFLAGS	:=$(KBUILD_AFLAGS:-msoft-float=-Wa,-mfpu=softvfp+vfp)
LDFLAGS		+=--no-warn-mismatch

obj-y					:= core.o clock.o timer.o
obj-$(CONFIG_PM)        += pm.o
obj-$(CONFIG_HW_PERF_EVENTS)		+= pmu.o
obj-$(CONFIG_HISI_SNAPSHOT_BOOT) += fastboot/

obj-$(CONFIG_HIBERNATION)   += hibernate.o swsusp.o cpu_helper_a7.o
