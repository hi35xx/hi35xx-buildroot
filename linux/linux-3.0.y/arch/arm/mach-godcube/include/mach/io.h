#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

#define __io(a)         __typesafe_io(a)
#define __mem_pci(a)    (a)

#define GODCUBE_IOCH1_PHYS 0x10000000 /* 0x1000_0000 ~ 0x1020_0000 */
#define GODCUBE_IOCH2_PHYS 0x20000000 /* 0x2000_0000 ~ 0x2020_0000 */
#define GODCUBE_IOCH1_SIZE 0x200000
#define GODCUBE_IOCH2_SIZE 0x200000

#define GODCUBE_IOCH1_VIRT 0xFE000000
#define GODCUBE_IOCH2_VIRT (GODCUBE_IOCH1_VIRT + GODCUBE_IOCH1_SIZE)

/*
 *         physical addr     ----         virtual addr
 * 0x1000_0000 ~ 0x1020_0000 <---> 0xFE00_0000 ~ 0xFE20_0000
 * 0x2000_0000 ~ 0x2020_0000 <---> 0xFE20_0000 ~ 0xFE40_0000
 */
#define IO_IOCH1_OFFSET         (GODCUBE_IOCH1_VIRT - GODCUBE_IOCH1_PHYS)
#define IO_IOCH2_OFFSET         (GODCUBE_IOCH2_VIRT - GODCUBE_IOCH2_PHYS)

#define IO_ADDRESS(x) ((x) >= GODCUBE_IOCH2_PHYS ? (x) + IO_IOCH2_OFFSET\
						: (x) + IO_IOCH1_OFFSET)
#endif
