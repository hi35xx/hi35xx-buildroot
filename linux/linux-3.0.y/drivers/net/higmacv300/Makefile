obj-$(CONFIG_HIGMACV300_ETH) += higmacv300.o
higmacv300-$(CONFIG_HiGMACV300_TIMER) += higmacv300_timer.o
higmacv300-objs:= higmacv300_main.o higmacv300_ethtool.o higmacv300_mdio.o	\
	      dwmac_lib.o dwmac1000_core.o  dwmac1000_dma.o	\
	      dwmac100_core.o dwmac100_dma.o enh_desc.o  norm_desc.o higmacv300_proc.o $(higmacv300-y)

ifdef CONFIG_ARCH_GODARM
higmacv300-y += higmacv300_godarm.o
endif

all:
	make $(KERNEL_MAKE) M=$(PWD) modules

clean:
	make $(KERNEL_MAKE) M=$(PWD) clean
