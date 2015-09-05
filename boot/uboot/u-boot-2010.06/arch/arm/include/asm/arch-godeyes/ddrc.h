#ifndef DDRC_H
#define DDRC_H
#if 0
#define REG_DDRC_STATUS     0x00
#define REG_DDRC_CTRL       0x04
#define REG_DDRC_EMRS01     0x08
#define REG_DDRC_EMRS23     0x0C
#define REG_DDRC_CONFIG     0x10
#define REG_DDRC_TIMING0    0x20
#define REG_DDRC_TIMING1    0x24
#define REG_DDRC_TIMING2    0x28
#define REG_DDRC_TIMING3    0x2C
#define REG_DDRC_ODT_CONFIG 0x40
#define REG_DDRC_PHY_CONFIG 0x60
#define REG_DDRC_DLL_STATUS 0x78
#define REG_DDRC_DLL_CONFIG 0x7C
#define REG_DDRC_AHB_STATUS 0x90
#define REG_DDRC_QOS0       0x98
#define REG_DDRC_QOS1       0x9C
#define REG_DDRC_QOS2   0xA0
#define REG_DDRC_QOS3   0xA4
#define REG_DDRC_QOS4   0xA8
#define REG_DDRC_QOS5   0xAC
#define REG_DDRC_QOS6   0xB0
#define REG_DDRC_QOS7   0xB4

#define DDRC_BUSWITH_32BITS 1
#define DDRC_BUSWITH_16BITS 0

#define DDRC_CHIPCAP_64Mb   0
#define DDRC_CHIPCAP_128Mb  1
#define DDRC_CHIPCAP_256Mb  2
#define DDRC_CHIPCAP_512Mb  3

#define DDRC_CHIP_8BITS     0
#define DDRC_CHIP_16BITS    1
#define DDRC_CHIP_32BITS    2

#define DDRC_CHIP_4BANK     0

#define DDRC_PHYREADDELAY_1 0
#define DDRC_PHYREADDELAY_1_5   1
#define DDRC_PHYREADDELAY_2 2





#define DDRC_READDELAY_3 3
#define DDRC_READDELAY_4 4
#define DDRC_READDELAY_5 5
#define DDRC_READDELAY_6 6

#define CFG_DDRC_AUTO_REFRESH  0x8
#define DDR_ONE_16BIT  	 4
#define DDR_TWO_16BIT 	 6
#define DDR_FIR_ROW_MAP	 0
#define DDR_FIR_BANK_MAP 1
#define DDR_BANK_4  	 0
#define DDR_BANK_8  	 1
#define DDR_ROW_11       0
#define DDR_ROW_12		1
#define DDR_ROW_13		2
#define DDR_ROW_14		3
#define DDR_ROW_15		4
#define DDR_ROW_16		5

#define DDR_COL_8		0
#define DDR_COL_9		1	
#define DDR_COL_10		2
#define DDR_COL_11		3
#define DDR_COL_12		4

#define CFG_DDRC_WR   0x3
#if (CFG_DDRC_WR == 2)
    #define CFG_DDR_MRS_WR          0x1
#elif (CFG_DDRC_WR == 3)
    #define CFG_DDR_MRS_WR          0x2
#elif (CFG_DDRC_WR == 4)
    #define CFG_DDR_MRS_WR          0x3
#elif (CFG_DDRC_WR == 5)
    #define CFG_DDR_MRS_WR          0x4
#elif (CFG_DDRC_WR == 6)
    #define CFG_DDR_MRS_WR          0x5
#endif
#endif
#endif
