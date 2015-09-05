#/******************************************************************************
# *    Copyright (c) 2009-2012 by Hisi.
# *    All rights reserved.
# *
#******************************************************************************/

SRC := hi3535

all: $(addsuffix .build,$(SRC))

clean: $(addsuffix .clean,$(SRC))

%.build: %
	make -C $(@:.build=) TOPDIR=$(TOPDIR) CROSS_COMPILE=$(CROSS_COMPILE)

%.clean: %
	make -C $(@:.clean=) TOPDIR=$(TOPDIR) CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

.PHONY: all clean
