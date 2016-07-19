include $(TOPDIR)/config.mk

LIB := ../libhdmi.a
HAL := hi3536
HAL_DIR := hal/$(HAL)

EXTRA_CFLAGS	 += -I$(HAL_DIR)
EXTRA_CFLAGS	 += -I$(HAL_DIR)/driver
EXTRA_CFLAGS	 += -I$(HAL_DIR)/driver/common
EXTRA_CFLAGS	 += -I$(HAL_DIR)/driver/cra
EXTRA_CFLAGS	 += -I$(HAL_DIR)/driver/tx
EXTRA_CFLAGS	 += -I$(HAL_DIR)/driver/tx/tx_hdmi
EXTRA_CFLAGS	 += -I$(HAL_DIR)/driver/tx/videopath
EXTRA_CFLAGS	 += -I$(HAL_DIR)/library/log
EXTRA_CFLAGS	 += -I$(HAL_DIR)/library/malloc
EXTRA_CFLAGS	 += -I$(HAL_DIR)/library/obj
EXTRA_CFLAGS	 += -I$(HAL_DIR)/library/time
EXTRA_CFLAGS	 += -I$(HAL_DIR)/platform


EXTRA_CFLAGS	 += -I$(TOPDIR)/product/hiosd/hdmi/hi3536
EXTRA_CFLAGS	 += -I$(TOPDIR)/product/hiosd/hdmi/hi3536/hal
EXTRA_CFLAGS     += -I$(TOPDIR)/include

EXTRA_CFLAGS += -I. -I./hal -I../ -I$(TOPDIR)/include \
	-DCONFIG_HDMI_BVT_SDK  -DHDMI_BUILD_IN_BOOT

CFLAGS += $(EXTRA_CFLAGS)
HOSTCFLAGS += $(EXTRA_CFLAGS)
CPPFLAGS += $(EXTRA_CFLAGS)

OBJ    := ../boot_hdmi_intf.o
OBJ    += drv_hdmi_infoframe.o
OBJ    += $(HAL_DIR)/hdmi_hal.o
OBJ    += $(HAL_DIR)/driver/common/si_drv_common.o \
		$(HAL_DIR)/driver/tx/si_drv_tx.o \
		$(HAL_DIR)/driver/tx/tx_hdmi/si_mod_tx_hdmi.o \
		$(HAL_DIR)/driver/tx/videopath/si_mod_videopath.o   \
		$(HAL_DIR)/driver/tx/videopath/si_mod_videopath_cra.o \
		$(HAL_DIR)/driver/cra/si_drv_cra.o	\
		$(HAL_DIR)/library/log/si_lib_log.o \
		$(HAL_DIR)/library/malloc/si_lib_malloc.o \
		$(HAL_DIR)/library/obj/si_lib_obj.o \
		$(HAL_DIR)/library/time/si_lib_time.o \
		$(HAL_DIR)/platform/platform.o \
		$(HAL_DIR)/platform/sii_time.o 									

COBJS-y += $(OBJ)


COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

.PHONY: all clean 

all: $(LIB) 

#prepare:	
#	$(AT)cp -rf $(TOPDIR)/../../../../../mpp/component/hdmi/mkp/hal .
#	$(AT)cp -rf $(TOPDIR)/../../../../../mpp/component/hdmi/mkp/drv_hdmi_infoframe.c .
#	$(AT)cp -rf $(TOPDIR)/../../../../../mpp/component/hdmi/mkp/drv_hdmi_infoframe.h .
#	$(AT)cp -rf $(TOPDIR)/../../../../../mpp/component/hdmi/mkp/drv_hdmi_common.h .
#	$(AT)find hal -name "*.o" |xargs rm -rf {}
clean:
	$(AT)rm -rf $(OBJS)
#	$(AT)rm -rf hal 
#	$(AT)rm -rf drv_hdmi_infoframe.c 
#	$(AT)rm -rf drv_hdmi_infoframe.h 
#	$(AT)rm -rf drv_hdmi_common.h 
	
$(LIB):	$(OBJS)
	@echo =$(OBJS)==	
	$(AR) $(ARFLAGS) $@ $(OBJS)

	
#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

