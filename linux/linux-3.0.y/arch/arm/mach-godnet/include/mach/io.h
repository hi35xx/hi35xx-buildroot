#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

#define __io(a)         __typesafe_io(a)
#define __mem_pci(a)    (a)

#define IO_OFFSET_HIGH 0xDE000000
#define IO_OFFSET_MID 0xEE900000

#define GODNET_IOCH1_VIRT (GODNET_IOCH1_PHYS + IO_OFFSET_HIGH)	/* 0xFE000000 */
#define GODNET_IOCH1_PHYS 0x20000000				/* 0x20000000
								~ 0x20820000 */
#define GODNET_IOCH1_SIZE 0x820000

#define GODNET_IOCH2_VIRT (GODNET_IOCH2_PHYS + IO_OFFSET_MID)	/* 0xFE900000 */
#define GODNET_IOCH2_PHYS 0x10000000				/* 0x10000000
								~ 0x101d0000 */
#define GODNET_IOCH2_SIZE 0x1d0000

#define IO_ADDRESS(x) ((x) >= GODNET_IOCH1_PHYS ? (x) + IO_OFFSET_HIGH \
						: (x) + IO_OFFSET_MID)

#endif

