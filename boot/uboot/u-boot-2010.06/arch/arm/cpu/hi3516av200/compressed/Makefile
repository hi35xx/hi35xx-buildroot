################################################################################
#    Create By Hisilicon
################################################################################

PWD           = $(shell pwd)
CROSS_COMPILE = arm-hisiv500-linux-
TOPDIR        =
BINIMAGE      = $(TOPDIR)/full-boot.bin

################################################################################
CC       := $(CROSS_COMPILE)gcc
AR       := $(CROSS_COMPILE)ar
LD       := $(CROSS_COMPILE)ld
OBJCOPY  := $(CROSS_COMPILE)objcopy

################################################################################
BOOT     := mini-boot
TEXTBASE := 0x88300000

CFLAGS   := -g -O2 -fno-strict-aliasing -fno-common -ffixed-r8 \
	-D__KERNEL__ -DTEXT_BASE=$(TEXTBASE) \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/drivers/ddr \
	-fno-builtin -ffreestanding \
	-fno-aggressive-loop-optimizations \
	-mno-unaligned-access \
	-pipe  -DCONFIG_ARM -D__ARM__ -marm  -mabi=aapcs-linux \
	-mno-thumb-interwork -march=armv7-a $(MKFLAGS)

################################################################################

START := start.o
COBJS := chip.o time.o timer.o\
	lowlevel_init_v300.o \
	emmc_boot.o \
	uart.o \
	ddr_training_impl.o \
	ddr_training_ctl.o \
	ddr_training_boot.o \
	ddr_training_custom.o \
	startup.o \
	image_data.o \
	div0.o _divsi3.o _udivsi3.o \
	mmu.o cache_ops.o cache-cp15.o

SSRC  := arch/arm/cpu/hi3516av200/start.S \
	arch/arm/cpu/hi3516av200/lowlevel_init.S \
	arch/arm/cpu/hi3516av200/chip.c \
	lib/time.c \
	arch/arm/cpu/hi3516av200/hi3516av200/timer.c \
        arch/arm/cpu/hi3516av200/himci_boot_hi3516av200.c \
	arch/arm/cpu/hi3516av200/lowlevel_init_v300.c \
	arch/arm/cpu/hi3516av200/emmc_boot.c \
	arch/arm/cpu/hi3516av200/himci_boot.c \
	arch/arm/cpu/hi3516av200/uart.S \
	drivers/ddr/ddr_training_impl.c \
	drivers/ddr/ddr_training_ctl.c \
	drivers/ddr/ddr_training_boot.c \
	arch/arm/cpu/hi3516av200/ddr_training_custom.c \
	drivers/ddr/ddr_training_impl.h \
	arch/arm/cpu/hi3516av200/ddr_training_custom.h \
	drivers/ddr/ddr_training_internal_config.h \
	arch/arm/lib/mmu.S \
	arch/arm/lib/div0.c \
	arch/arm/lib/_divsi3.S \
	arch/arm/lib/_udivsi3.S \
	arch/arm/lib/cache_ops.S \
	arch/arm/lib/cache-cp15.c \
	lib/unlzma.c

REG := $(wildcard $(TOPDIR)/*.reg $(TOPDIR)/.reg)
SRC := $(notdir $(SSRC))

################################################################################
.PHONY: $(BOOT).bin
$(BOOT).bin: $(BOOT).tmp regfile
	@dd if=./$(BOOT).tmp of=./tmp1 bs=1 count=64 2>/dev/null
	@dd if=$(REG) of=./tmp2 bs=4096 conv=sync 2>/dev/null
	@dd if=./$(BOOT).tmp of=./tmp3 bs=1 skip=4160 2>/dev/null
	@cat tmp1 tmp2 tmp3 > $(BOOT).bin
	@rm -f tmp1 tmp2 tmp3
	@chmod 754 $(BOOT).bin
	@cp -fv $@ $(TOPDIR)
	@echo $(BOOT).bin is Ready.

$(BOOT).tmp: $(BOOT).elf
	$(OBJCOPY) -O srec $< $(BOOT).srec
	$(OBJCOPY) -j .text -O binary $< $(BOOT).text
	$(OBJCOPY) --gap-fill=0xff -O binary $< $@

$(BOOT).elf: image_data.lzma $(SRC) $(START) $(COBJS)
	$(LD) -Bstatic -T mini-boot.lds -Ttext $(TEXTBASE) $(START) \
		$(COBJS) -Map $(BOOT).map -o $@

.PHONY: regfile
regfile:
	@if [ "$(words $(REG))" = "0" ]; then ( \
		echo '***' Need '.reg' or '*.reg' file in directory $(TOPDIR); \
		exit 1; \
	) fi
	@if [ "$(words $(REG))" != "1" ]; then ( \
		echo '***' Found multi '.reg' or '*.reg' file in directory $(TOPDIR); \
		echo '***' Files: $(notdir $(REG)); \
		exit 1; \
	) fi

################################################################################
start.o: start.S
	$(CC) -D__ASSEMBLY__ $(CFLAGS) -o $@ $< -c

image_data.lzma: $(BINIMAGE)
	lzma -fkzc -7 $< > $@

%.o: %.c
	$(CC) $(CFLAGS) -Wall -Wstrict-prototypes \
		-fno-stack-protector -o $@ $< -c

%.o: %.S
	$(CC) -D__ASSEMBLY__ $(CFLAGS) -o $@ $< -c

image_data.o: image_data.S image_data.lzma
	$(CC) -D__ASSEMBLY__ $(CFLAGS) -o $@ $< -c

#############################################################################

$(SRC):
	ln -sf ../../../../../$(filter %/$@,$(SSRC)) $@
################################################################################
TMPS := $(COBJS) start.o $(SRC) \
	$(BOOT).map $(BOOT).elf $(BOOT).srec $(BOOT).bin $(BOOT).text $(BOOT).tmp \
	image_data.lzma

distclean: clean

clean:
	-rm -f $(TMPS)

################################################################################
.PHONY: clean
################################################################################
