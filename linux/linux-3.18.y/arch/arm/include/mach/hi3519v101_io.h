#ifndef __HI3519_IO_H
#define __HI3519_IO_H


/*  phys_addr		virt_addr
 * 0x1000_0000 <-----> 0xFE00_0000
 */
#define HI3519_IOCH1_VIRT (0xFE000000)
#define HI3519_IOCH1_PHYS (0x10000000)
#define HI3519_IOCH1_SIZE (0x00310000)

/*  phys_addr		virt_addr
 * 0x11000000 <-----> 0xFE400000
 */
#define HI3519_IOCH2_VIRT (0xFE400000)
#define HI3519_IOCH2_PHYS (0x11000000)
#define HI3519_IOCH2_SIZE (0x004F0000)

/*  phys_addr		virt_addr
 * 0x12000000 <-----> 0xFE900000
 */
#define HI3519_IOCH3_VIRT (0xFE900000)
#define HI3519_IOCH3_PHYS (0x12000000)
#define HI3519_IOCH3_SIZE (0x00170000)


#define IO_OFFSET_LOW   (0xEC900000)
#define IO_OFFSET_MID   (0xED400000)
#define IO_OFFSET_HIGH  (0xEE000000)

#define IO_ADDRESS_LOW(x)  ((x) + IO_OFFSET_LOW)
#define IO_ADDRESS_MID(x)  ((x) + IO_OFFSET_MID)
#define IO_ADDRESS_HIGH(x) ((x) + IO_OFFSET_HIGH)

#define __IO_ADDRESS(x) ((x >= HI3519_IOCH2_PHYS) ? IO_ADDRESS_MID(x)\
		: IO_ADDRESS_HIGH(x))
#define IO_ADDRESS(x)   ((x) >= HI3519_IOCH3_PHYS ? IO_ADDRESS_LOW(x)\
		: __IO_ADDRESS(x))

#define __io_address(n) (IOMEM(IO_ADDRESS(n)))

#endif
