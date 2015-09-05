#define SYS_CTRL_BASE           0x20030000

#define TOE_MAC_INTERFACE       0x000000ec
#define TOE_RST                 0x000000c8

#define TOE_CLK_DEF_100M	0x00000800
#define TOE_DEFAULT_CLK_100M	100000000
#define TOE_DEFAULT_CLK_150M	150000000

unsigned long get_tnkclk(unsigned long busclk);
void reset_mac_interface_single(int val);
void reset_mac_interface_dual(int val);
