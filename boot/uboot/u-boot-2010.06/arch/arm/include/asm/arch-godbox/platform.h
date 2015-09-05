
#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define REG_BASE_SCTL          0x101E0000
#define REG_BASE_TIMER01	0x101E2000
#define REG_BASE_GPIO5		0x101E4000
#define REG_BASE_TIMER67	0x101E5000
#define REG_BASE_GPIO0		0x101E6000	
#define REG_BASE_GPIO1		0x101E7000	
#define REG_BASE_GPIO2		0x101E8000
#define REG_BASE_GPIO3		0x101E9000
#define REG_BASE_GPIO4		0x101EA000
#define REG_BASE_TIMER45	0x101EB000
#define REG_BASE_GPIO6		0x101EC000	
#define REG_BASE_GPIO7		0x101ED000
#define REG_BASE_GPIO8		0x101EE000
#define REG_BASE_GPIO9		0x101EF000
#define REG_BASE_GPIO10		0x101F0000
#define REG_BASE_GPIO11		0x101F1000
#define REG_BASE_GPIO12		0x101F2000
#define REG_BASE_CRG		0x101F5000
#define REG_BASE_UART0		0x101FB000
#define REG_BASE_UART1      0x101FC000
#define REG_BASE_UART3		0x101FE000
#define REG_BASE_PERI_CTRL	0x10200000
#define REG_BASE_WDG		0x10201000
#define REG_BASE_TIMER23	0x10202000
#define REG_BASE_IO_CONFIG 	0x10203000
#define REG_BASE_A9_PERI    0x10220000
#define REG_BASE_CSSYS		0x10330000					
#define REG_BASE_DNR		0x10450000					
#define	MEM_BASE_NAND		0x24000000
#define MEM_BASE_SPI		0x26000000
#define REG_BASE_NANDC		0x60010000
#define REG_BASE_SFC		0x60020000
#define REG_BASE_DMAC		0x60030000
#define REG_BASE_OHCI		0X60070000
#define REG_BASE_EHCI		0X60080000
#define REG_BASE_SF			0X600D0000
#define REG_BASE_GMAC		0x60140000
#define REG_BASE_MCI		0x601C0000
#define REG_BASE_LANSWITCH	0X63000000
 
#define REG_SC_CTRL                         0x0000
#define REG_SC_SYSRES                       0x0004
#define REG_SC_APLLFREQCTRL0		     0x0018
#define REG_SC_APLLFREQCTRL1                0x001C
#define REG_SC_LOW_POWER_CTRL               0x0040
#define REG_SC_IO_REUSE_SEL                 0x0044
#define REG_SC_SRST_REQ_CTRL                0x0048
#define REG_SC_CA_RST_CTRL                  0x004C
#define REG_SC_WDG_RST_CTRL                 0x0050
#define REG_SC_DDRC_DFI_RST_CTRL 			0x0054
#define REG_SC_PLLLOCK_STAT					0x0070
#define REG_SC_GEN0                         0x0080
#define REG_SC_GEN1                         0x0084
#define REG_SC_GEN2                         0x0088
#define REG_SC_GEN3                         0x008C
#define REG_SC_GEN4                         0x0090
#define REG_SC_GEN5                         0x0094
#define REG_SC_GEN6                         0x0098
#define REG_SC_GEN7                         0x009C
#define REG_SC_GEN8                         0x00A0
#define REG_SC_GEN9                         0x00A4
#define REG_SC_GEN10                        0x00A8
#define REG_SC_GEN11                        0x00AC
#define REG_SC_GEN12                        0x00B0
#define REG_SC_GEN13                        0x00B4
#define REG_SC_GEN14                        0x00B8
#define REG_SC_GEN15                        0x00BC
#define REG_SC_LOCKEN                       0x020C

#define REG_PERI_CRG46		(0x00F8)
#define SDIO_CLK_PCTRL		(0x01 << 17)
#define SDIO_CLK_50M		(0x01 << 16)
#define SDIO_HCLKEN		(0x01 << 9)
#define SDIO_CKEN		(0x01 << 8)
#define SDIO_SRST_REQ		(0x01 << 0)

#define REG_START_MODE		   0x000
#define REG_PERI_STAT          0x0004
#define REG_PERI_CTRL          0x0008
#define REG_PERI_CRG26         0x00A8
#define SCTL_REMAP_STAT             (1<<9)

#define REG_TIMER_RELOAD	0x000
#define REG_TIMER_VALUE		0x004
#define REG_TIMER_CONTROL	0x008

#define MEM_BASE_BOOTRAM	0x00000000  // (yes)
#define MEM_SIZE_BOOTRAM	0x00001000	//(yes)

#define MEM_CONF_ITCM_SIZE	3
#define MEM_BASE_DDR   0x80000000


#define DEFAULT_UART_CLK 54000000
#define BOOT_FROM_EMMC  3
#define BOOT_FROM_DDR   2
#define BOOT_FROM_NAND  1
#define BOOT_FROM_SPI   0
#define REG_START_FLAG         (REG_BASE_SCTL + REG_SC_GEN12)
#define CONFIG_START_MAGIC     (0x444f574e)
#define RAM_START_ADRS		0x10c00

#endif /*End of __HI_BOARD_H__ */
