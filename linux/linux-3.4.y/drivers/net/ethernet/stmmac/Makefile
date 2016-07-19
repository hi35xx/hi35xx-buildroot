obj-$(CONFIG_STMMAC_ETH_TNK) += stmmac.o
stmmac-$(CONFIG_STMMAC_TIMER) += stmmac_timer.o
stmmac-objs:= stmmac_main.o stmmac_ethtool.o stmmac_mdio.o	\
	      dwmac_lib.o dwmac1000_core.o  dwmac1000_dma.o	\
	      dwmac100_core.o dwmac100_dma.o enh_desc.o  norm_desc.o $(stmmac-y)

ifdef CONFIG_ARCH_HI3535
stmmac-y += tnk_hi3535.o
endif

stmmac-y += tnkct.o tnk_proc.o tnktcp.o tnksysctl.o tnklock.o tnkmem.o

stmmac-y += tnkhw.o

all:
	make $(KERNEL_MAKE) M=$(PWD) modules

clean:
	make $(KERNEL_MAKE) M=$(PWD) clean
