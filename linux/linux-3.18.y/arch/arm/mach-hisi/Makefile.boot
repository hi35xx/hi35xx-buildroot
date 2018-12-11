ifdef CONFIG_ARCH_HI3559
zreladdr-y      := 0x88008000
else
ifdef CONFIG_ARCH_HI3556
zreladdr-y      := 0x83008000
else
zreladdr-y      := 0x80008000
endif
endif

params_phys-y   := 0x00000100
initrd_phys-y   := 0x00800000
#LOADADDR       := 0x88000000
