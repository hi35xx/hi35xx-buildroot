
obj-$(CONFIG_HIPCIE) += hipcie.o

hipcie-objs	:= pcie.o

ifeq ($(CONFIG_PCI_DEBUG),y)
	        EXTRA_CFLAGS += -DPCIE_DEBUG
endif
