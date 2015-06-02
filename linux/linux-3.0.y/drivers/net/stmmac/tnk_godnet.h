#define SYSCTL_BASE		0x20030000
#define TOE_CLK_SRST		0x000000cc

#define TOE_CLK_DEF_100M	0x00000010

#define TOE_MAX_CON_NUM		0x000000ff
#define TOE_DEFAULT_CLK		0x00100000

#define HW_REG(a) (*(unsigned long *)(a))
#define A9_AXI_SCALE_REG   IO_ADDRESS(SYSCTL_BASE + 0x28)
#define TXIN_OSC_FRE       24000000
#define REG_CRG0_OFFSET    IO_ADDRESS(SYSCTL_BASE + 0x0)
#define REG_CRG1_OFFSET    IO_ADDRESS(SYSCTL_BASE + 0x4)
#define get_bus_clk()({\
	unsigned long fbdiv, pstdiv1, pstdiv2, refdiv;\
	unsigned long tmp_reg, foutvco, busclk;\
	tmp_reg = HW_REG(REG_CRG0_OFFSET);\
	pstdiv1 = (tmp_reg >> 24) & 0x7;\
	pstdiv2 = (tmp_reg >> 27) & 0x7;\
	tmp_reg = HW_REG(REG_CRG1_OFFSET);\
	refdiv = (tmp_reg >> 12) & 0x3f;\
	fbdiv = tmp_reg & 0xfff;\
	foutvco = TXIN_OSC_FRE/refdiv;\
	foutvco *= fbdiv;\
	tmp_reg = HW_REG(A9_AXI_SCALE_REG);\
	if ((tmp_reg & 0x3) == 0) {\
		busclk = foutvco/4;\
	} else if ((tmp_reg & 0x3) == 3) {\
		busclk = foutvco/6;\
	} else {\
		busclk = 0;\
		printk(KERN_ERR "system bus clk is0!\n");\
	} \
	busclk;\
})

unsigned long tnk_clk_init(void);
unsigned long mdio_clk_init(void);
int get_clk_csr(unsigned long tnkclk);
void reset_mac_interface_dual(int id, void *addr, u32 val);
void reset_mac_interface_single(void *addr, u32 val);
