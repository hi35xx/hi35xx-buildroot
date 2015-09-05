#define SYS_CTRL_BASE           0x20030000

#define TOE_CLK_SRST            0x000000cc
#define TOE_MAC_INTERFACE       0x000000ec
#define TOE_RST                 0x000000c8

#define TOE_CLK_DEF_100M        0x00000010
#define TOE_DEFAULT_CLK         0x00100000

unsigned long get_tnkclk(unsigned long busclk);
void reset_mac_interface_dual(int val);
