#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

#define __io(a)         __typesafe_io(a)
#define __mem_pci(a)    (a)

#define IO_OFFSET_HIGH 0xDE000000
#define IO_OFFSET_MID 0xEE900000

#define HI3520D_IOCH1_VIRT (HI3520D_IOCH1_PHYS + IO_OFFSET_HIGH)
#define HI3520D_IOCH1_PHYS 0x20000000

#define HI3520D_IOCH1_SIZE 0x820000

#define HI3520D_IOCH2_VIRT (HI3520D_IOCH2_PHYS + IO_OFFSET_MID)
#define HI3520D_IOCH2_PHYS 0x10000000

#define HI3520D_IOCH2_SIZE 0x1d0000

#define IO_ADDRESS(x) ((x) >= HI3520D_IOCH1_PHYS ? (x) + IO_OFFSET_HIGH \
						: (x) + IO_OFFSET_MID)

#endif
