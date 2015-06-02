#ifndef _L2_H
#define _L2_H

#define REG_L2_CTRL				0x0000
#define REG_L2_AUCTRL			0x0004
#define REG_L2_STATUS			0x0008
#define REG_L2_INTMASK			0x0100
#define REG_L2_MINT			0x0104
#define REG_L2_RINT			0x0108
#define REG_L2_INTCLR			0x010C
#define REG_L2_INTMASK_INT_MON		0x0110
#define REG_L2_MINT_INT_MON		0x0114
#define REG_L2_RINT_INT_MON		0x0118
#define REG_L2_INTCLR_INT_MON		0x011C
#define REG_L2_INTMASK_EXT_MON		0x0120
#define REG_L2_MINT_EXT_MON		0x0124
#define REG_L2_RINT_EXT_MON		0x0128
#define REG_L2_INTCLR_EXT_MON		0x012C
#define REG_L2_SYNC				0x0200
#define REG_L2_INVALID			0x0204
#define REG_L2_CLEAN			0x0208
#define REG_L2_MAINT_AUTO			0x020C
#define REG_L2_DLOCKWAY			0x0300
#define REG_L2_ILOCKWAY			0x0304
#define REG_L2_TESTMODE			0x0400
#define REG_L2_INTTEST			0x0404
#define REG_L2_INTTEST_INT_MON		0x0408
#define REG_L2_INTTEST_EXT_MON		0x040C
#define REG_L2_EVENTTEST			0x0410
#define REG_L2_REGION0			0x0500
#define REG_L2_REGION1			0x0504
#define REG_L2_REGION2			0x0508
#define REG_L2_REGION3			0x050C
#define REG_L2_REGION4			0x0510
#define REG_L2_INT_COUNTER0			0x0600
#define REG_L2_INT_COUNTER1			0x0604
#define REG_L2_INT_COUNTER2			0x0608
#define REG_L2_INT_COUNTER3			0x060C
#define REG_L2_INT_COUNTER4			0x0610
#define REG_L2_INT_COUNTER5			0x0614
#define REG_L2_INT_COUNTER6			0x0618
#define REG_L2_INT_COUNTER7			0x061C
#define REG_L2_INT_COUNTER8			0x0620
#define REG_L2_INT_COUNTER9			0x0624
#define REG_L2_INT_COUNTER10		0x0628
#define REG_L2_EXT_COUNTER0			0x0700
#define REG_L2_EXT_COUNTER1			0x0704
#define REG_L2_EXT_COUNTER2			0x0708
#define REG_L2_EXT_COUNTER3			0x070C
#define REG_L2_EXT_COUNTER4			0x0710
#define REG_L2_EXT_COUNTER5			0x0714
#define REG_L2_EXT_COUNTER6			0x0718
#define REG_L2_EXT_COUNTER7			0x071C
#define REG_L2_EXT_COUNTER8			0x0720
#define REG_L2_EXT_COUNTER9			0x0724
#define REG_L2_EXT_COUNTER10		0x0728
#define REG_L2_EXT_COUNTER11		0x072C
#define REG_L2_EXT_COUNTER12		0x0730
#define REG_L2_EXT_COUNTER13		0x0734
#define REG_L2_EXT_COUNTER14		0x0738
#define REG_L2_EXT_COUNTER15		0x073C
#define REG_L2_EXT_COUNTER16		0x0740
#define REG_L2_EXT_COUNTER17		0x0744
#define REG_L2_EXT_COUNTER18		0x0748
#define REG_L2_EXT_COUNTER19		0x074C
#define REG_L2_EXT_COUNTER20		0x0750
#define REG_L2_EXT_COUNTER21		0x0754
#define REG_L2_EXT_COUNTER22		0x0758
#define REG_L2_EXT_COUNTER23		0x075C
#define REG_L2_EXT_COUNTER24		0x0760
#define REG_L2_EXT_COUNTER25		0x0764
#define REG_L2_EXT_COUNTER26		0x0768
#define REG_L2_EXT_COUNTER27		0x076C
#define REG_L2_SPECIAL_CTRL		0x0800
#define REG_L2_SPECIAL_CHECK0		0x0804
#define REG_L2_SPECIAL_CHECK1		0x0808

/*BIT OF REG L2_CTRL */
#define BIT_L2_CTRL_CACHE_ENABLE                0

/*BIT OF REG L2_AUCTRL */
#define BIT_L2_AUCTRL_NON_SECURE_INT_CON        20
#define BIT_L2_AUCTRL_NON_SECURE_LOCK_EN        19
#define BIT_L2_AUCTRL_OVERRIDE_SECRUE_CHECK     18
#define BIT_L2_AUCTRL_FORCE_WRITE_ALLOCATE      16
#define BIT_L2_AUCTRL_SHARED_ATTRIBUTE_OVER_EN  15

#define BIT_L2_AUCTRL_MONITOR_EN		13
#define BIT_L2_AUCTRL_EVENT_BUS_EN		12
#define BIT_L2_AUCTRL_EXCLUSIVE_CACHE_OPER      11


/*BIT OF REG L2_STATUS */
#define BIT_L2_STATUS_SPNIDEN                   1
#define BIT_L2_STATUS_IDLE                      0

/*BIT OF REG L2_INTMASK */
#define BIT_L2_INTMASK_AUTO_END                 14
#define BIT_L2_INTMASK_SLVERR_RB                13
#define BIT_L2_INTMASK_DECERR_RA                12
#define BIT_L2_INTMASK_DECERR_WB                11
#define BIT_L2_INTMASK_SLVERR_WB                10
#define BIT_L2_INTMASK_DECERR_WA                9
#define BIT_L2_INTMASK_SLVERR_WA                8
#define BIT_L2_INTMASK_DECERR_EB                7
#define BIT_L2_INTMASK_SLVERR_EB                6
#define BIT_L2_INTMASK_ERRRD                    5
#define BIT_L2_INTMASK_ERRRT                    4
#define BIT_L2_INTMASK_ERRWD                    3
#define BIT_L2_INTMASK_ERRWT                    2
#define BIT_L2_INTMASK_PARRD                    1
#define BIT_L2_INTMASK_PARRT                    0

/*BIT OF REG L2_MINT*/
#define BIT_L2_MINT_AUTO_END                 14
#define BIT_L2_MINT_SLVERR_RB                13
#define BIT_L2_MINT_DECERR_RA                12
#define BIT_L2_MINT_DECERR_WB                11
#define BIT_L2_MINT_SLVERR_WB                10
#define BIT_L2_MINT_DECERR_WA                9
#define BIT_L2_MINT_SLVERR_WA                8
#define BIT_L2_MINT_DECERR_EB                7
#define BIT_L2_MINT_SLVERR_EB                6
#define BIT_L2_MINT_ERRRD                    5
#define BIT_L2_MINT_ERRRT                    4
#define BIT_L2_MINT_ERRWD                    3
#define BIT_L2_MINT_ERRWT                    2
#define BIT_L2_MINT_PARRD                    1
#define BIT_L2_MINT_PARRT                    0

/*BIT OF REG L2_RINT*/
#define BIT_L2_RINT_AUTO_END                 14
#define BIT_L2_RINT_SLVERR_RB                13
#define BIT_L2_RINT_DECERR_RA                12
#define BIT_L2_RINT_DECERR_WB                11
#define BIT_L2_RINT_SLVERR_WB                10
#define BIT_L2_RINT_DECERR_WA                9
#define BIT_L2_RINT_SLVERR_WA                8
#define BIT_L2_RINT_DECERR_EB                7
#define BIT_L2_RINT_SLVERR_EB                6
#define BIT_L2_RINT_ERRRD                    5
#define BIT_L2_RINT_ERRRT                    4
#define BIT_L2_RINT_ERRWD                    3
#define BIT_L2_RINT_ERRWT                    2
#define BIT_L2_RINT_PARRD                    1
#define BIT_L2_RINT_PARRT                    0

/*BIT OF REG L2_INTCLR*/
#define BIT_L2_INTCLR_AUTO_END                 14
#define BIT_L2_INTCLR_SLVERR_RB                13
#define BIT_L2_INTCLR_DECERR_RA                12
#define BIT_L2_INTCLR_DECERR_WB                11
#define BIT_L2_INTCLR_SLVERR_WB                10
#define BIT_L2_INTCLR_DECERR_WA                9
#define BIT_L2_INTCLR_SLVERR_WA                8
#define BIT_L2_INTCLR_DECERR_EB                7
#define BIT_L2_INTCLR_SLVERR_EB                6
#define BIT_L2_INTCLR_ERRRD                    5
#define BIT_L2_INTCLR_ERRRT                    4
#define BIT_L2_INTCLR_ERRWD                    3
#define BIT_L2_INTCLR_ERRWT                    2
#define BIT_L2_INTCLR_PARRD                    1
#define BIT_L2_INTCLR_PARRT                    0

/*BIT OF REG L2_SYNC*/
#define BIT_L2_SYNC_SYNC                       0

/*BIT OF REG L2_INVALID*/
#define BIT_L2_INVALID_WAYADDRESS              29
#define BIT_L2_INVALID_LINEADDRESS             5
#define BIT_L2_INVALID_BYADDRESS               1

/*BIT OF REG L2_CLEAN*/
#define BIT_L2_CLEAN_WAYADDRESS                29
#define BIT_L2_CLEAN_LINEADDRESS               5
#define BIT_L2_CLEAN_BYADDRESS                 1

/*BIT OF REG L2_MAINT_AUTO*/
#define BIT_L2_MAINT_AUTO_WAYADDRESS           2
#define BIT_L2_MAINT_AUTO_CLEAN                1
#define BIT_L2_MAINT_AUTO_START                0

/*BIT OF REG L2_DLOCKWAY*/
#define BIT_L2_DLOCKWAY_DATALOCK               0
#define L2_LOCKWAY_MASK                        0XFF

/*BIT OF REG L2_ILOCKWAY*/
#define BIT_L2_ILOCKWAY_INSTRLOCK              0

/*BIT OF REG L2_TESTMODE*/
#define BIT_L2_TESTMODE_TEST_MODE              0

/*BIT OF REG L2_INTTEST*/
#define L2_INTTEST_DECERR_WB                11
#define L2_INTTEST_SLVERR_WB                10
#define L2_INTTEST_DECERR_WA                9
#define L2_INTTEST_SLVERR_WA                8
#define L2_INTTEST_DECERR_EB                7
#define L2_INTTEST_SLVERR_EB                6
#define L2_INTTEST_ERRRD                    5
#define L2_INTTEST_ERRRT                    4
#define L2_INTTEST_ERRWD                    3
#define L2_INTTEST_ERRWT                    2
#define L2_INTTEST_PARRD                    1
#define L2_INTTEST_PARRT                    0

/*BIT OF REG L2_EVENTTEST*/
#define BIT_L2_EVENTTEST_DATAREAD_HIT       8
#define BIT_L2_EVENTTEST_DATAREAD_REQUEST   7
#define BIT_L2_EVENTTEST_DATAWRITE_HIT      6
#define BIT_L2_EVENTTEST_DATAWRITE_REQUEST  5
#define BIT_L2_EVENTTEST_INSTRUT_HIT        4
#define BIT_L2_EVENTTEST_INSTRUT_REQUEST    3
#define BIT_L2_EVENTTEST_EVICTION           2
#define BIT_L2_EVENTTEST_BUFFEREDW_REQUEST  1
#define BIT_L2_EVENTTEST_WRITEALL_REQUEST   0

#define L2_SIZE              0x40000
#define L2_LINE_SIZE         0x20
#define L2_WAY_NUM           0x8
#define L2_WAY_SIZE          (L2_SIZE/L2_WAY_NUM)
#define L2_LINE_NUM          (L2_WAY_SIZE/L2_LINE_SIZE)

#endif
