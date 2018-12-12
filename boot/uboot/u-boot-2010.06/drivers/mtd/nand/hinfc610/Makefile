#
# The Flash Memory Controller v100 Device Driver for hisilicon
#
# Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
#
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

include $(TOPDIR)/config.mk

LIB	:= $(obj)libhinfcv610.a

COBJS-$(CONFIG_NAND_FLASH_HINFC610)  += hinfc610.o hinfc610_gen.o \
	hinfc610_spl_ids.o \
	hinfc610_read_retry_hynix_bg_cdie.o \
	hinfc610_read_retry_hynix_bg_bdie.o \
	hinfc610_read_retry_hynix_cg_adie.o \
	hinfc610_read_retry_micron.o  \
	hinfc610_read_retry_samsung.o \
	hinfc610_read_retry_toshiba.o \
	hinfc620_gen.o

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
