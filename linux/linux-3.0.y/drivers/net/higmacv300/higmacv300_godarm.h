#define SYSCTL_BASE		0x20030000
#define GMAC_CLK_SRST		0x000000cc

#define GMAC_CLK_DEF_100M	0x00000010

#define GMAC_MAX_CON_NUM		0x000000ff
#define GMAC_DEFAULT_CLK		0x00100000

#define HW_REG(a) (*(unsigned long *)(a))
#define TXIN_OSC_FRE       24000000
#define REG_CRG0_OFFSET    IO_ADDRESS(SYSCTL_BASE + 0x0)
#define REG_CRG1_OFFSET    IO_ADDRESS(SYSCTL_BASE + 0x4)
#define get_bus_clk()({\
	unsigned long fbdiv, pstdiv1, pstdiv2, refdiv;\
	unsigned long tmp_reg, foutvco, foutpostdiv, busclk;\
	tmp_reg = HW_REG(REG_CRG0_OFFSET);\
	pstdiv1 = (tmp_reg >> 24) & 0x7;\
	pstdiv2 = (tmp_reg >> 27) & 0x7;\
	tmp_reg = HW_REG(REG_CRG1_OFFSET);\
	refdiv = (tmp_reg >> 12) & 0x3f;\
	fbdiv = tmp_reg & 0xfff;\
	foutvco = TXIN_OSC_FRE/refdiv;\
	foutvco *= fbdiv;\
	foutpostdiv = foutvco/(pstdiv1 * pstdiv2);\
	busclk = foutpostdiv/6;\
	busclk;\
})

unsigned long gmac_clk_init(void);
unsigned long mdio_clk_init(void);
int get_clk_csr(unsigned long gmac_clk);
void reset_mac_interface_single(void *addr, u32 val);
