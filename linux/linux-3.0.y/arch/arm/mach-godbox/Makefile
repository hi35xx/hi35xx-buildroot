#
# Makefile for the linux kernel.
#


KBUILD_AFLAGS	:=$(KBUILD_AFLAGS:-msoft-float=-Wa,-mfpu=softvfp+vfp)
LDFLAGS		+=--no-warn-mismatch

obj-y					:= core.o clock.o
obj-$(CONFIG_HW_PERF_EVENTS)		+= pmu.o
obj-$(CONFIG_PM)			+= hi_pm_sleep.o hipm.o
