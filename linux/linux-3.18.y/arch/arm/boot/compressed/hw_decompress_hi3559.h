#define CRG_REG_BASE 0x12010000
#define PERI_CRG33 0x84
#define GZIP_CLKEN (1<<1)

static void disable_decompress_clock(void)
{
	unsigned int regval;

	regval = readl((void *)(CRG_REG_BASE + PERI_CRG33));
	regval &= ~GZIP_CLKEN;
	writel(regval, (void *)(CRG_REG_BASE + PERI_CRG33));
}

static void enable_decompress_clock(void)
{
	unsigned int regval;

	regval = readl((void *)(CRG_REG_BASE + PERI_CRG33));
	regval |= GZIP_CLKEN;
	writel(regval, (void *)(CRG_REG_BASE + PERI_CRG33));
}
