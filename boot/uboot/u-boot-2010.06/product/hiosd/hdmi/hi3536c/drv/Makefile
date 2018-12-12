include $(TOPDIR)/config.mk

LIB := ../libhdmi.a
CHIP_TYPE := hi3536c
CTRL := hisiv100
PHY  := hisiv100
CTRL_DIR := hal/ctrl/$(CTRL)
PHY_DIR  := hal/phy/$(PHY)
PRODUCT  := hi3536c
PRODUCT_DIR := product/$(PRODUCT)
HAL_DIR  := hal/

EXTRA_CFLAGS	 += -I$(CTRL_DIR)
EXTRA_CFLAGS	 += -I$(CTRL_DIR)/regs
EXTRA_CFLAGS	 += -I$(PHY_DIR)
EXTRA_CFLAGS	 += -I$(PHY_DIR)/regs
EXTRA_CFLAGS	 += -I$(PRODUCT_DIR)
EXTRA_CFLAGS	 += -I$(PRODUCT_DIR)/regs
EXTRA_CFLAGS     += -I./hal/emi

EXTRA_CFLAGS	 += -I$(TOPDIR)/product/hiosd/hdmi/$(CHIP_TYPE)
EXTRA_CFLAGS	 += -I$(TOPDIR)/product/hiosd/hdmi/$(CHIP_TYPE)/hal
EXTRA_CFLAGS     += -I$(TOPDIR)/include

EXTRA_CFLAGS += -I. -I./hal -I../ -I$(TOPDIR)/include

EXTRA_CFLAGS += -DCONFIG_HDMI_BVT_SDK
EXTRA_CFLAGS += -DHDMI_BUILD_IN_BOOT

EXTRA_CFLAGS += -DHDMI_SUPPORT_LOGIC_HISIV100
EXTRA_CFLAGS += -DCONFIG_HDMI_BVT_SDK -DDVI_SUPPORT
EXTRA_CFLAGS += -DHDMI_SCDC_SUPPORT
EXTRA_CFLAGS += -DCONFIG_HDMI_BVT_SDK -DDVI_SUPPORT

CFLAGS     += $(EXTRA_CFLAGS)
HOSTCFLAGS += $(EXTRA_CFLAGS)
CPPFLAGS   += $(EXTRA_CFLAGS)


ifneq ($(findstring $(HIARCH), hi3536c),)
EXTRA_CFLAGS += -DCHIP_HI3536C
endif

ifeq ($(FPGA),y)
EXTRA_CFLAGS += -DHDMI_FPGA_SUPPORT
endif

#HDMI_EMI_ENABLE

OBJ    := ../boot_hdmi_intf.o
OBJ    += drv_hdmi_common.o
OBJ    += drv_hdmi_infoframe.o
OBJ    += $(CTRL_DIR)/hdmi_hal_intf.o

OBJ    +=  $(CTRL_DIR)/regs/hdmi_reg_aon.o \
		$(CTRL_DIR)/regs/hdmi_reg_audio_path.o \
		$(CTRL_DIR)/regs/hdmi_reg_cec.o \
		$(CTRL_DIR)/regs/hdmi_reg_ctrl.o \
		$(CTRL_DIR)/regs/hdmi_reg_hdcp.o \
		$(CTRL_DIR)/regs/hdmi_reg_mcu.o \
		$(CTRL_DIR)/regs/hdmi_reg_tx.o \
		$(CTRL_DIR)/regs/hdmi_reg_video_path.o

OBJ    +=  $(CTRL_DIR)/hdmi_hal_intf.o \
		$(CTRL_DIR)/hdmi_hal_ddc.o \
		$(CTRL_DIR)/hdmi_hal_ncts.o \
		$(CTRL_DIR)/hdmi_hal_ctrl.o \
		$(CTRL_DIR)/hdmi_hal_scdc.o
		#$(CTRL_DIR)/hdmi_hal_hdcp14.o \
		#$(CTRL_DIR)/hdmi_hal_hdcp22.o \
		#$(CTRL_DIR)/hdmi_hal_cec.o \
		#$(CTRL_DIR)/hdmi_hal_machine.o

OBJ +=  $(PHY_DIR)/hdmi_hal_phy.o \
		$(PHY_DIR)/regs/hdmi_reg_dphy.o

OBJ += $(PRODUCT_DIR)/hdmi_product_define.o
OBJ += $(PRODUCT_DIR)/regs/hdmi_reg_crg.o


COBJS-y += $(OBJ)


COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

.PHONY: all clean

all: $(LIB)


clean:
	$(AT)rm -rf $(OBJS)

$(LIB):	$(OBJS)
	#@echo =$(OBJS)==
	$(AR) $(ARFLAGS) $@ $(OBJS)


#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
