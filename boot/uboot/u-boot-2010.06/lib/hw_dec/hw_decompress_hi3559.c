#include <asm/io.h>
#include <config.h>

#define PERI_CRG33 0x84
#define GZIP_CLKEN (1<<1)

static void disable_decompress_clock(void)
{
    unsigned int regval;

    regval = readl(CRG_REG_BASE + PERI_CRG33);
    regval &= ~GZIP_CLKEN;
    writel(regval, CRG_REG_BASE + PERI_CRG33);
}

static void enable_decompress_clock(void)
{
    unsigned int regval;

    regval = readl(CRG_REG_BASE + PERI_CRG33);
    regval |= GZIP_CLKEN;
    writel(regval, CRG_REG_BASE + PERI_CRG33);
}

