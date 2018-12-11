#ifndef __HI_CHIP_REGS_H__
#define __HI_CHIP_REGS_H__

#if (defined CONFIG_ARCH_HI3519 || defined CONFIG_ARCH_HI3519V101)
#include <mach/hi3519_platform.h>
#endif

#if defined(CONFIG_ARCH_HI3559) || defined(CONFIG_ARCH_HI3556)
#include <mach/hi3559_platform.h>
#endif

#ifdef CONFIG_ARCH_HI3516AV200
#include <mach/hi3516av200_platform.h>
#endif

#ifdef CONFIG_ARCH_HI3536C
#include <mach/hi3536c_platform.h>
#endif

#endif /* End of __HI_CHIP_REGS_H__ */
