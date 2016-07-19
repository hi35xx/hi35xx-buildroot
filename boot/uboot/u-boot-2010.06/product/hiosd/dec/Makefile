include $(TOPDIR)/config.mk

LIB := libjpegd.a

COBJS-y += jpegd.o
COBJS-y += hi_JpegDecode.o
COBJS-y += jpeg_hdec_rwreg.o
COBJS-y += jpeg_hdec_table.o
COBJS-y += jpeg_hdec_setpara.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

