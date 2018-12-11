#ifndef __HI3536C_IO_H
#define __HI3536C_IO_H

/*  phys_addr		virt_addr
 * 0x1000_0000 <-----> 0xFE00_0000
 * 0x1004_0000 <-----> 0xFE04_0000
 */
#define HI3536C_IOCH1_VIRT (0xFE000000)
#define HI3536C_IOCH1_PHYS (0x10000000)
#define HI3536C_IOCH1_SIZE (0x00400000)

/* phys_addr        virt_addr
 * 0x1200_0000 <-----> 0xFE4_00000
 * 0x1223_0000 <-----> 0xFE63_0000
 */
#define HI3536C_IOCH2_VIRT (0xFE400000)
#define HI3536C_IOCH2_PHYS (0x12000000)
#define HI3536C_IOCH2_SIZE (0x00230000)

/* phys_addr        virt_addr
 * 0x1300_0000 <-----> 0xFE70_0000
 * 0x1316_0000 <-----> 0xFE86_0000
 */
#define HI3536C_IOCH3_VIRT (0xFE700000)
#define HI3536C_IOCH3_PHYS (0x13000000)
#define HI3536C_IOCH3_SIZE (0x00160000)

#define IO_OFFSET_LOW		(0xEB700000)
#define IO_OFFSET_MID		(0xEC400000)
#define IO_OFFSET_HIGH		(0xEE000000)

#define IO_ADDRESS_LOW(x)	((x) + IO_OFFSET_LOW)
#define IO_ADDRESS_MID(x)	((x) + IO_OFFSET_MID)
#define IO_ADDRESS_HIGH(x)	((x) + IO_OFFSET_HIGH)

#define __IO_ADDRESS_HIGH(x) ((x >= HI3536C_IOCH2_PHYS) ? IO_ADDRESS_MID(x) \
		: IO_ADDRESS_HIGH(x))
#define IO_ADDRESS(x)   ((x) >= HI3536C_IOCH3_PHYS ? IO_ADDRESS_LOW(x) \
		: __IO_ADDRESS_HIGH(x))

#define __io_address(x) (IOMEM(IO_ADDRESS(x)))

#endif
