
include $(TOPDIR)/config.mk

LIB	:= $(obj)libddr.a

SOBJS   += ddr_cmd_loc.o
COBJS   += ddr_cmd_ctl.o ddr_training_impl.o ddr_training_ctl.o ddr_training_boot.o ddr_training_custom.o

SSRC    := ddr_training_custom.h ddr_training_custom.c

CFLAGS  += -I$(TOPDIR)/arch/$(ARCH)/cpu/$(CPU)/

SRCS	:= $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))

LINK_FILES = $(SSRC)


all: $(SSRC) $(LIB)
	@rm -f $(LINK_FILES)

ddr_training: $(LIB)

$(LIB):	$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

$(SSRC):
	ln -sf $(TOPDIR)/arch/$(ARCH)/cpu/$(CPU)/$@ $@
include $(SRCTREE)/rules.mk

#sinclude $(obj).depend

#########################################################################
