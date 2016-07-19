#ifndef _LINUX_UNALIGNED_ACCESS_OK_H
#define _LINUX_UNALIGNED_ACCESS_OK_H

#include <asm/byteorder.h>

static inline u16 get_unaligned_le16(const void *p)
{
#ifndef CONFIG_SNAPSHOT_BOOT
	return le16_to_cpup((__le16 *)p);
#else
	const unsigned char * const uc = p;
	const u16 us = (uc[1] << 8) | uc[0];
	return le16_to_cpup((__le16 *)&us);
#endif
}

static inline u32 get_unaligned_le32(const void *p)
{
#ifndef CONFIG_SNAPSHOT_BOOT
	return le32_to_cpup((__le32 *)p);
#else
	const unsigned char * const uc = p;
	const u32 ui = (uc[3] << 24) | (uc[2] << 16) | (uc[1] << 8) | uc[0];
	return le32_to_cpup((__le32 *)&ui);
#endif
}

static inline u64 get_unaligned_le64(const void *p)
{
#ifndef CONFIG_SNAPSHOT_BOOT
	return le64_to_cpup((__le64 *)p);
#else
	const unsigned char * const uc = p;
	const u64 ui_l = (uc[3] << 24) | (uc[2] << 16) | (uc[1] << 8) | uc[0];
	const u64 ui_h = (uc[7] << 24) | (uc[6] << 16) | (uc[5] << 8) | uc[4];
	const u64 ui = (ui_h << 32) | ui_l;
	return le64_to_cpup((__le64 *)&ui);
#endif
}

static inline u16 get_unaligned_be16(const void *p)
{
	return be16_to_cpup((__be16 *)p);
}

static inline u32 get_unaligned_be32(const void *p)
{
	return be32_to_cpup((__be32 *)p);
}

static inline u64 get_unaligned_be64(const void *p)
{
	return be64_to_cpup((__be64 *)p);
}

static inline void put_unaligned_le16(u16 val, void *p)
{
#ifndef CONFIG_SNAPSHOT_BOOT
	*((__le16 *)p) = cpu_to_le16(val);
#else
	unsigned char * const uc = p;
	uc[0] = val & 0xff;
	uc[1] = (val >> 8) & 0xff;
#endif
}

static inline void put_unaligned_le32(u32 val, void *p)
{
#ifndef CONFIG_SNAPSHOT_BOOT
	*((__le32 *)p) = cpu_to_le32(val);
#else
	unsigned char * const uc = p;
	uc[0] = val & 0xff;
	uc[1] = (val >> 8) & 0xff;
	uc[2] = (val >> 16) & 0xff;
	uc[3] = (val >> 24) & 0xff;
#endif
}

static inline void put_unaligned_le64(u64 val, void *p)
{
#ifndef CONFIG_SNAPSHOT_BOOT
	*((__le64 *)p) = cpu_to_le64(val);
#else
	unsigned char * const uc = p;
	uc[0] = val & 0xff;
	uc[1] = (val >> 8) & 0xff;
	uc[2] = (val >> 16) & 0xff;
	uc[3] = (val >> 24) & 0xff;
	uc[4] = (val >> 32) & 0xff;
	uc[5] = (val >> 40) & 0xff;
	uc[6] = (val >> 48) & 0xff;
	uc[7] = (val >> 56) & 0xff;
#endif
}

static inline void put_unaligned_be16(u16 val, void *p)
{
	*((__be16 *)p) = cpu_to_be16(val);
}

static inline void put_unaligned_be32(u32 val, void *p)
{
	*((__be32 *)p) = cpu_to_be32(val);
}

static inline void put_unaligned_be64(u64 val, void *p)
{
	*((__be64 *)p) = cpu_to_be64(val);
}

#endif /* _LINUX_UNALIGNED_ACCESS_OK_H */
