#define SYSCTL_BASE		0x20030000
#define TOE_CLK_SRST		0x000000c8
#define TOE_MAC_INTERFACE	0x000000ec

#define TOE_CLK_DEF_100M	0x00000800

#define TOE_MAX_CON_NUM		0x000000ff
#define TOE_DEFAULT_CLK_100M	100000000
#define TOE_DEFAULT_CLK_150M	150000000

#define HW_REG(a) (*(unsigned long *)(a))
#define A9_AXI_SCALE_REG   IO_ADDRESS(SYSCTL_BASE + 0x30)
#define REG_CRG0_OFFSET    IO_ADDRESS(SYSCTL_BASE + 0x0)
#define REG_CRG1_OFFSET    IO_ADDRESS(SYSCTL_BASE + 0x4)
#define get_bus_clk()({\
	unsigned long tmp_reg, busclk;\
	tmp_reg = HW_REG(A9_AXI_SCALE_REG);\
	if ((tmp_reg & 0x1) == 0x1) {\
		busclk = 200000000;\
	} else {\
		busclk = 250000000;\
	} \
	busclk;\
})

unsigned long tnk_clk_init(void);
unsigned long mdio_clk_init(void);
int get_clk_csr(unsigned long tnkclk);
void reset_mac_interface_dual(int id, void *addr, u32 val);
