#ifndef __HI_I2C_H__
#define __HI_I2C_H__

#define I2C_CON_REG		0x000
#define I2C_TAR_REG		0x004
#define I2C_DATA_CMD_REG	0x010
#define I2C_SCL_H_REG		0x01C
#define I2C_SCL_L_REG		0x020
#define I2C_INTR_STAT_REG	0x02C
#define I2C_INTR_MASK_REG	0x030
#define I2C_INTR_RAW_REG	0x034
#define I2C_RX_TL_REG		0x038
#define I2C_TX_TL_REG		0x03C
#define I2C_CLR_INTR_REG	0x040
#define I2C_CLR_RX_OVER_REG	0x048
#define I2C_CLR_TX_OVER_REG	0x04C
#define I2C_ENABLE_REG		0x06C
#define I2C_STATUS_REG		0x070
#define I2C_TXFLR_REG		0x074
#define I2C_RXFLR_REG		0x078
#define I2C_SDA_HOLD_REG	0x07C
#define I2C_TX_ABRT_SRC		0x080
#define I2C_DMA_CTRL_REG	0x088
#define I2C_DMA_TDLR		0x08C
#define I2C_DMA_RDLR		0x090
#define I2C_LPIF_STATE		0x0A8
#define I2C_LOCK_REG		0x0AC
#define I2C_AUTO_REG		0x0B0
#define I2C_TX_RX_REG		0x0B4
#define I2C_DMA_CMD0		0x0B8
#define I2C_DMA_CMD1		0x0BC
#define I2C_DMA_CMD2		0x0C0
#define I2C_ENABLE_STATUS_REG	0x09C

#define HI_I2C_FAST_MODE	0x65

#define HI_I2C_UNLOCK_VALUE	0x1ACCE551

#define HI_I2C_ENABLE		(1 << 0)

#define HI_I2C_AUTO_MODE_OFF	0x0f000000

#define HI_I2C_WRITE		0x80000000
#define HI_I2C_READ		0xc0000000

#define READ_OPERATION     (1)
#define WRITE_OPERATION    0xfe

#define CMD_I2C_WRITE      0x01
#define CMD_I2C_READ       0x03

/* I2C_COM_REG */
#define I2C_SEND_ACK (~(1 << 4))
#define I2C_START        (1 << 3)
#define I2C_READ         (1 << 2)
#define I2C_WRITE        (1 << 1)
#define I2C_STOP (1 << 0)

/* I2C_ENABLE_REG */
#define I2C_ENABLE                  (1 << 0)

#define I2C_RAW_TX_ABORT            (1 << 6)

/*I2C_INTR_STAT_REG */
#define I2C_AUTO_RX_FIFO_NOT_EMPTY  (1 << 8)
#define I2C_AUTO_TX_FIFO_EMPTRY     (1 << 20)
#define I2c_AUTO_TX_FIFO_NOT_FULL   (1 << 21)
#define I2C_TX_ABRT                 (1 << 23)
#define I2C_AUTO_DATA               (1 << 28)
#define I2C_AUTO_ADDR               (1 << 29)

/* I2C_STATUS */
#define I2C_STATUS_WORKING          (1 << 0)

#define IS_TX_FIFO_EMPTY(status)        (((status) &\
			I2C_AUTO_TX_FIFO_EMPTRY) == I2C_AUTO_TX_FIFO_EMPTRY)
#define IS_RX_FIFO_EMPTY(status)        (((status) &\
			I2C_AUTO_RX_FIFO_NOT_EMPTY) == 0)
#define IS_FIFO_EMPTY(status)           (IS_RX_FIFO_EMPTY(status) &&\
			IS_TX_FIFO_EMPTY(status))
#define IS_I2C_IDLE(status)             (((status) & I2C_STATUS_WORKING) == 0)

#define REG_SHIFT		16
#define DATA_16BIT_MASK		0xFFFF
#define DATA_8BIT_MASK		0xFFFF

#define REVERT_HL_BYTE(value)   ((value >> 8) | ((value & 0xFF) << 8))

/*
 * I2C Interrupt related Macros
 */
#define DEFAULT_I2C_REG_IMSC		0x0UL
#define DISABLE_ALL_INTERRUPTS		((~DEFAULT_I2C_REG_IMSC) & 0xfff)
#define ENABLE_ALL_INTERRUPTS		DEFAULT_I2C_REG_IMSC

typedef enum i2c_mode_e {
	I2C_MODE_AUTO,
	I2C_MODE_DMA,
	I2C_MODE_NONE,
} i2c_mode_e;

struct hi_platform_i2c {
	int	clk_limit;
	unsigned int i2c_class;
	unsigned int clk_rate;
};

#endif
