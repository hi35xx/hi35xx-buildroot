#include <asm/io.h>
#include <config.h>

#define PERI_CRG53 0xD4
#define PERI_CRG56 0xE0
#define GZIP_CLKEN (1<<0)
#define GZIP_AP5_CLKEN (1<<2)
#define GZIP_AXI_CLKEN (1<<1)
#define GZIP_SRST_REQ (1<<7)

static void disable_decompress_clock(void)
{
    unsigned int regval;

    regval = readl(CRG_REG_BASE + PERI_CRG53);
    regval &= ~(GZIP_CLKEN | GZIP_AP5_CLKEN | GZIP_AXI_CLKEN);
    writel(regval, CRG_REG_BASE + PERI_CRG53);
}

static void config_decompress(void)
{
    unsigned int reg_val;
    reg_val = readl(MISC_REG_BASE + MISC_CTRL0);
    reg_val &= ~GZIP_DDRT_SEL_MASK;
    reg_val |= (GZIP_DDRT_SEL_GZIP << GZIP_DDRT_SEL_SHIFT);
    writel(reg_val, MISC_REG_BASE + MISC_CTRL0);
}

static void enable_decompress_clock(void)
{
    unsigned int regval;

    config_decompress();

    regval = readl(CRG_REG_BASE + PERI_CRG53);
    regval |= GZIP_CLKEN | GZIP_AP5_CLKEN | GZIP_AXI_CLKEN;
    writel(regval, CRG_REG_BASE + PERI_CRG53);

    regval = readl(CRG_REG_BASE + PERI_CRG56);
    regval |= (1 << 1);
    writel(regval, CRG_REG_BASE + PERI_CRG56);

    regval = readl(CRG_REG_BASE + PERI_CRG56);
    regval &= ~(1 << 1);
    writel(regval, CRG_REG_BASE + PERI_CRG56);
}

