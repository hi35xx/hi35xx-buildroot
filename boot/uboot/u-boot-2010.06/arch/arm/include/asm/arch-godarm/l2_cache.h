#ifndef _L2_H
#define _L2_H
#include <config.h>
#ifndef CONFIG_L2_OFF

extern void l2cache_enable(void);
extern void l2cache_disable(void);

#ifndef REG_BASE_L2CACEH
#define REG_BASE_L2CACEH		0x16800000
#endif

#define REG_L2_CACHE_CTRL		0X000
#define REG_L2_CACHE_AUX_CTRL		0X004
#define REG_L2_CACHE_STATUS		0X008
#define REG_L2_CACHE_INTMASK		0X100
#define REG_L2_CACHE_INTMASK_STATUS	0X104
#define REG_L2_CACHE_RAW_INT		0X108
#define REG_L2_CACHE_INTCLR		0X10C
#define REG_L2_CACHE_SYNC		0X200
#define REG_L2_CACHE_INVALID		0X204
#define REG_L2_CACHE_CLEAN		0X208
#define REG_L2_CACHE_MAINT_AUTO		0X20C
#define REG_L2_CACHE_DLOCKWAY		0X300
#define REG_L2_CACHE_ILOCKWAY		0X304
#define REG_L2_CACHE_TESTMODE		0X400
#define REG_L2_CACHE_INTTEST		0X404
#define REG_L2_CACHE_EVENTTEST		0X408

/*BIT OF REG L2_CTRL */
#define BIT_L2_CTRL_CACHE_ENABLE                0

/*BIT OF REG L2_AUCTRL */
#define BIT_L2_AUCTRL_NON_SECURE_INT_CON        20
#define BIT_L2_AUCTRL_NON_SECURE_LOCK_EN        19
#define BIT_L2_AUCTRL_OVERRIDE_SECRUE_CHECK     18
#define BIT_L2_AUCTRL_FORCE_WRITE_ALLOCATE      16
#define BIT_L2_AUCTRL_SHARED_ATTRIBUTE_OVER_EN  15
#define BIT_L2_AUCTRL_PARITY_EN                 14
#define BIT_L2_AUCTRL_EVENT_MON_BUS_EN          13
#define BIT_L2_AUCTRL_EVENT_CNT_EN      12
#define BIT_L2_AUCTRL_EXCLUSIVE_CACHE_OPER      11
#define BIT_L2_AUCTRL_CYCLE_OF_DIRTY_RAM        9
#define BIT_L2_AUCTRL_CYCLE_OF_TAG_RAM          6
#define BIT_L2_AUCTRL_CYCLE_OF_RAM_READ         3
#define BIT_L2_AUCTRL_CYCLE_OF_RAM_WRITE        0

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


#define L2_REG_WRITE(data, reg)			\
		\(*(volatile unsigned int *) \
		\(reg + REG_BASE_L2CACEH) = data)
#define L2_REG_READ(reg)			\
		\(*(volatile unsigned int *) \
		\(reg + REG_BASE_L2CACEH))



#define L2_SIZE              0x20000
#define L2_LINE_SIZE         0x20
#define L2_WAY_NUM           0x8
#define L2_WAY_SIZE          (L2_SIZE/L2_WAY_NUM)
#define L2_LINE_NUM          (L2_WAY_SIZE/L2_LINE_SIZE)

/* #define L2_DATA_LOCK         0X0 */
/* #define L2_INSTRUCT_LOCK     0X1 */
#endif
#endif

