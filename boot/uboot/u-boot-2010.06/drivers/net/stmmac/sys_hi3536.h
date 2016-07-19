#define SYS_CTRL_BASE           0x12040000

#define TOE_MAC_INTERFACE       0x000000ec
#define TOE_RST                 0x000000c8

#define MUXCTL_BASE             0x120f0000
#define MUXCTL_REG16_OFFSET     0x040
#define MUXCTL_REG34_OFFSET     0x088
#define MUXCTL_PHY_MASK         0x3

#define TOE_CLK_EXT_PHY0_RST_BIT                13
#define TOE_CLK_EXT_PHY1_RST_BIT                14

#define REG_GPIO_DIR            0x400

#define TOE_CLK_DEF_250M	(1 << 10)
#define TOE_DEFAULT_CLK_250M	250000000
#define TOE_DEFAULT_CLK_150M	150000000

unsigned long get_tnkclk(unsigned long busclk);
void reset_mac_interface_single(int val);
void reset_mac_interface_dual(int val);
void stmmac_external_phy_reset(int port_id);
