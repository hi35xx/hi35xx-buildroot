/*
 * Hisilicon BVT I2C Controller Driver
 *
 * Copyright (c) 2016 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#ifdef CONFIG_HI_DMAC
#include <linux/hidmac.h>
#include <mach/io.h>
#endif

/*
 * I2C Registers offsets
 */
#define HII2C_CR		0x00
#define HII2C_TAR		0x04
#define I2C_DATA_CMD_REG	0x010
#define HII2C_SCL_H		0x1C
#define HII2C_SCL_L		0x20
#define HII2C_IM		0x30
#define HII2C_RI		0x34
#define HII2C_IC		0x40
#define HII2C_EN		0x6C
#define HII2C_SR		0x70
#define HII2C_SDA_HOLD		0x7C
#define HII2C_TX_ABRT		0x80
#define I2C_DMA_CTRL_REG	0x088
#define I2C_DMA_TDLR		0x08C
#define I2C_DMA_RDLR		0x090
#define HII2C_LOCK		0xAC
#define HII2C_CR1		0xB0
#define HII2C_DATA		0xB4
#define HII2C_SEQU_CMD0		0xB8
#define I2C_DMA_CMD0		0x0B8
#define I2C_DMA_CMD1		0x0BC
#define I2C_DMA_CMD2		0x0C0

/*
 * I2C Control Register -- HII2C_CR
 */
#define CR_RESTART_EN		(0x1 << 5)

/*
 * I2C Slave Address Register -- HII2C_TAR
 */
#define TAR_10BIT_MODE		(0x1 << 12)
#define TAR_SPECIAL		(0x1 << 11)
#define TAR_ADDR		(0x3ff << 0)
#define TAR_ADDR_SHIFT		0

/*
 * I2C Interrupt status and mask Register -- HII2C_IM
 */
#define INT_TX_ABRT            (1 << 6)
#define MASK_ALL_INT		0xffffffff

/*
 * I2C Status Register -- HII2C_SR
 */
#define SR_BUSY		(0x1 << 0)


/*
 * I2C Lock Register -- HII2C_LOCK
 */
#define UNLOCK_VALUE	0x1ACCE551

/*
 * I2C Single Control Register -- HII2C_CR1
 */
#define CR1_SINGLE_MODE_EN	(0x1 << 31)
#define	CR1_READ_MODE		(0x1 << 30)
#define CR1_H_WIDE		(0x1 << 29)
#define CR1_L_WIDE		(0x1 << 28)
#define	CR1_STATUS_CLR		(0xf << 24)
#define	CR1_TX_ABRT		(0x1 << 23)
#define	CR1_TFNF		(0x1 << 21)
#define	CR1_TFE			(0x1 << 20)
#define	CR1_RFF			(0x1 << 9)
#define	CR1_RFNE		(0x1 << 8)

/*
 * I2C Single CMD Register -- HII2C_DATA
 */
#define DATA_H		(0xffff << 16)
#define DATA_L		0xffff
#define DATA_H_SHIFT	16
#define DATA_L_SHIFT	0

/*
 * I2C Sequence CMD0 Register -- HII2C_SEQU_CMD0
 */
#define SEQU_MODE_EN		(0x1 << 31)
#define SEQU_MODE_WRITE		(0x1 << 30)
#define SEQU_ADDR_8BIT		(0x0 << 28)
#define SEQU_ADDR_16BIT		(0x1 << 28)

#define HII2C_TIMEOUT		0x10000

typedef enum i2c_mode {
	I2C_MODE_SINGLE,
	I2C_MODE_DMA,
} i2c_mode_t;

struct hi_i2c {
	struct platform_device	*pdev;
	struct i2c_adapter	*adap;
	struct device		*dev;
	resource_size_t		phybase;
	void __iomem		*virtbase;
	struct clk		*clk;

	struct i2c_msg		*msg;

	unsigned int		frequency;

	__u16			last_slave_addr;
	i2c_mode_t		last_mode;
};

static inline void hi_i2c_disable(struct hi_i2c *i2c)
{
	writel(0x0, i2c->virtbase + HII2C_EN);
}

static inline void hi_i2c_enable(struct hi_i2c *i2c)
{
	writel(0x1, i2c->virtbase + HII2C_EN);
}

static inline void  hi_i2c_disable_irq(struct hi_i2c *i2c, unsigned int flag)
{
	unsigned int val;

	val = readl(i2c->virtbase + HII2C_IM);
	val |= flag;
	writel(val, i2c->virtbase + HII2C_IM);
}

static inline void hi_i2c_set_slave_addr(struct hi_i2c *i2c, __u16 addr,
		unsigned char ten_bit_flag)
{
	unsigned int val;

	hi_i2c_disable(i2c);

	val = readl(i2c->virtbase + HII2C_TAR);
	val &= ~TAR_ADDR;
	val |= (addr << TAR_ADDR_SHIFT) & TAR_ADDR;

	if (ten_bit_flag)
		val |= TAR_10BIT_MODE;
	else
		val &= ~TAR_10BIT_MODE;

	writel(val, i2c->virtbase + HII2C_TAR);

	i2c->last_slave_addr = addr;

	hi_i2c_enable(i2c);
}

static inline void hi_i2c_set_mode(struct hi_i2c *i2c, i2c_mode_t mode)
{
	unsigned int val;

	hi_i2c_disable(i2c);

	if (mode == I2C_MODE_SINGLE) {
		val = readl(i2c->virtbase + HII2C_SEQU_CMD0);
		val &= ~SEQU_MODE_EN;
		writel(val, i2c->virtbase + HII2C_SEQU_CMD0);

		val = readl(i2c->virtbase + HII2C_CR1);
		val |= CR1_SINGLE_MODE_EN;
		writel(val, i2c->virtbase + HII2C_CR1);
	} else {
		val = readl(i2c->virtbase + HII2C_CR1);
		val &= ~CR1_SINGLE_MODE_EN;
		writel(val, i2c->virtbase + HII2C_CR1);
	}

	i2c->last_mode = mode;

	hi_i2c_enable(i2c);
}

static inline int hi_i2c_clr_status(struct hi_i2c *i2c)
{
	unsigned int val;

	val = readl(i2c->virtbase + HII2C_CR1);
	val |= CR1_STATUS_CLR;
	writel(val & (~CR1_SINGLE_MODE_EN), i2c->virtbase + HII2C_CR1);
	writel(val, i2c->virtbase + HII2C_CR1);

	writel(0x1, i2c->virtbase + HII2C_IC);

	hi_i2c_disable(i2c);

	hi_i2c_enable(i2c);

	return 0;
}

static u32 hi_i2c_get_delay_val(struct hi_i2c *i2c)
{
	unsigned int clk_rate;
	unsigned int scl_l;
	u32 val;

	clk_rate = clk_get_rate(i2c->clk);

	scl_l = readl(i2c->virtbase + HII2C_SCL_L);

	/* delay_val = scl_l * 2.5 * 1000 * 1000 / clk_rate   */
	val = (25 * 100000 / clk_rate) + 1;// +1 is to make the time plenty more
	val = val * scl_l;

	return val;
}

static int hi_i2c_wait_idle(struct hi_i2c *i2c)
{
	unsigned int time_cnt;
	unsigned int val;
	struct device *dev = i2c->dev;

	time_cnt = 0;
	do {
		val = readl(i2c->virtbase + HII2C_RI);
		if (val & INT_TX_ABRT) {
			dev_err(dev, "%s: wait last fifo is empyt abort!",
					__func__);
			dev_err(dev, "RI: %#x\n", val);
			val = readl(i2c->virtbase + HII2C_TX_ABRT);
			dev_err(i2c->dev, "TX_ABRT:0x%x\n", val);
			return -EIO;
		}

		val = readl(i2c->virtbase + HII2C_CR1);
		if (val & CR1_RFNE)
			readl(i2c->virtbase + HII2C_DATA);

		if (val & CR1_TFE & (~CR1_RFNE))
			break;

		if (time_cnt >= HII2C_TIMEOUT) {
			dev_err(dev, "%s: wit last fifo is empty timeout!",
					__func__);
			dev_err(dev, "CR1:0x%x\n", val);
			return -EBUSY;
		}
		time_cnt++;
		udelay(50);
	} while (1);

	udelay(10);

	time_cnt = 0;
	do {
		val = readl(i2c->virtbase + HII2C_RI);
		if (val & INT_TX_ABRT) {
			dev_err(dev, "%s: wait last i2c is idle abort!",
					__func__);
			dev_err(dev, "RI: %#x\n", val);
			val = readl(i2c->virtbase + HII2C_TX_ABRT);
			dev_err(i2c->dev, "TX_ABRT:0x%x\n", val);
			return -EIO;
		}

		val = readl(i2c->virtbase + HII2C_SR);
		if (!(val & SR_BUSY))
			break;

		if (time_cnt >= HII2C_TIMEOUT) {
			dev_err(dev, "%s: wait last i2c is idle timeout!",
					__func__);
			dev_err(dev, "CR1:0x%x\n", val);
			return -EBUSY;
		}
		time_cnt++;
		udelay(50);
	} while (1);

	return 0;
}

static int hi_i2c_wait_tx_nofull(struct hi_i2c *i2c)
{
	unsigned int val;
	unsigned int time_cnt;
	struct device *dev = i2c->dev;

	time_cnt = 0;
	do {
		val = readl(i2c->virtbase + HII2C_RI);
		if (val & INT_TX_ABRT) {
			dev_err(dev, "wait tx no full abort, last RI: %#x\n",
					val);
			val = readl(i2c->virtbase + HII2C_TX_ABRT);
			dev_err(i2c->dev, "TX_ABRT:0x%x\n", val);
			return -EIO;
		}

		val = readl(i2c->virtbase + HII2C_CR1);
		if (val & CR1_RFNE)
			readl(i2c->virtbase + HII2C_DATA);

		if (val & CR1_TFNF)
			break;

		if (time_cnt >= HII2C_TIMEOUT) {
			dev_err(dev, "wait tx no full timeout, last CR1: %#x\n",
					val);
			return -EBUSY;
		}
		time_cnt++;
		udelay(50);
	} while (1);

	return 0;
}

static int hi_i2c_wait_rx_noempty(struct hi_i2c *i2c)
{
	unsigned int val;
	unsigned int time_cnt;
	struct device *dev = i2c->dev;

	time_cnt = 0;
	do {
		val = readl(i2c->virtbase + HII2C_RI);
		if (val & INT_TX_ABRT) {
			dev_err(dev, "wait rx no empty abort, RI: %#x\n", val);
			val = readl(i2c->virtbase + HII2C_TX_ABRT);
			dev_err(i2c->dev, "TX_ABRT:0x%x\n", val);
			return -EIO;
		}

		val = readl(i2c->virtbase + HII2C_CR1);
		if (val & CR1_RFNE)
			break;

		if (time_cnt >= HII2C_TIMEOUT) {
			dev_err(dev, "\nwait rx no empty timeout, CR1: %#x\n",
					val);
			return -EBUSY;
		}
		time_cnt++;
		udelay(50);
	} while (1);

	return 0;
}

#ifdef CONFIG_HI_DMAC
#define REVERT_HL_BYTE(value)   ((value >> 8) | ((value & 0xFF) << 8))

void hi_i2c_dma_start(struct hi_i2c *i2c, unsigned int dir)
{
	writel((1 << dir), i2c->virtbase + I2C_DMA_CTRL_REG);
}

void hi_i2c_dmac_config(struct hi_i2c *i2c, unsigned int dir)
{
	/* 1. enable RX(0) or TX(1) in DMA mode */
	hi_i2c_dma_start(i2c, dir);

	/* 2. set dma fifo */
	writel(4, i2c->virtbase + I2C_DMA_TDLR);
	writel(4, i2c->virtbase + I2C_DMA_RDLR);
}

void hi_i2c_start_rx(struct hi_i2c *i2c, unsigned int reg_addr,
			unsigned int length)
{
	unsigned int reg;

	writel(reg_addr, i2c->virtbase + I2C_DMA_CMD1);
	writel(length, i2c->virtbase + I2C_DMA_CMD2);

	reg = readl(i2c->virtbase + I2C_DMA_CMD0);

	/*start dma rx*/
	reg |= SEQU_MODE_EN;
	reg &= ~SEQU_MODE_WRITE;
	writel(reg, i2c->virtbase + I2C_DMA_CMD0);
}

void hi_i2c_start_tx(struct hi_i2c *i2c, unsigned int reg_addr,
			unsigned int length)
{
	unsigned int reg;

	writel(reg_addr, i2c->virtbase + I2C_DMA_CMD1);
	writel(length, i2c->virtbase + I2C_DMA_CMD2);

	reg = readl(i2c->virtbase + I2C_DMA_CMD0);

	/*start dma tx*/
	reg |= SEQU_MODE_EN;
	reg |= SEQU_MODE_WRITE;
	writel(reg, i2c->virtbase + I2C_DMA_CMD0);
}

int dma_to_i2c(unsigned int src, unsigned int dst, unsigned int length)
{
	int chan;

	chan = do_dma_m2p(src, dst, length);
	if (chan == -1)
		pr_err("dma_to_i2c error\n");

	return chan;
}


int i2c_to_dma(unsigned int src, unsigned int dst, unsigned int length)
{
	int chan;

	chan = do_dma_p2m(dst, src, length);
	if (chan == -1)
		pr_err("dma_p2m error...\n");

	return chan;
}

static int hi_i2c_do_dma_write(struct hi_i2c *i2c,
		unsigned int reg_addr, unsigned int reg_addr_num,
		unsigned int dma_buf, unsigned int len)
{
	int chan;
	struct i2c_msg *msg = i2c->msg;
	unsigned int temp_reg = reg_addr;

	if ((msg->addr != i2c->last_slave_addr)
			|| (i2c->last_mode != I2C_MODE_DMA)) {
		hi_i2c_set_slave_addr(i2c, msg->addr, msg->flags & I2C_M_TEN);
		hi_i2c_set_mode(i2c, I2C_MODE_DMA);
	}

	if (2 == reg_addr_num) {
		/* switch high byte and low byte */
		temp_reg = REVERT_HL_BYTE(reg_addr);
		writel(SEQU_ADDR_16BIT, i2c->virtbase + I2C_DMA_CMD0);
	} else {
		writel(SEQU_ADDR_8BIT, i2c->virtbase + I2C_DMA_CMD0);
	}

	/* 2. config i2c into DMA mode */
	hi_i2c_dmac_config(i2c, 0x1);

	/* 3. start i2c logic to write */
	hi_i2c_start_tx(i2c, temp_reg, len - 1);

	/* 4. transmit DATA from DMAC to I2C in DMA mode */
	chan = dma_to_i2c(dma_buf, (i2c->phybase + I2C_DATA_CMD_REG),
				len);
	if (chan == -1) {
		return -1;
	}

	if (dmac_wait(chan) != DMAC_CHN_SUCCESS) {
		dev_err(i2c->dev, "dma wait failed\n");
		hi_i2c_clr_status(i2c);
		dmac_channel_free(chan);
		return -1;
	}

	hi_i2c_wait_idle(i2c);
	hi_i2c_clr_status(i2c);

	dmac_channel_free(chan);

	return 0;
}

static int hi_i2c_do_dma_read(struct hi_i2c *i2c,
		unsigned int reg_addr, unsigned int reg_addr_num,
		unsigned int dma_buf, unsigned int len)
{
	int chan;
	struct i2c_msg *msg = i2c->msg;
	unsigned int temp_reg = reg_addr;

	if ((msg->addr != i2c->last_slave_addr)
			|| (I2C_MODE_DMA != i2c->last_mode)) {
		hi_i2c_set_slave_addr(i2c, msg->addr, msg->flags & I2C_M_TEN);
		hi_i2c_set_mode(i2c, I2C_MODE_DMA);
	}

	if (2 == reg_addr_num) {
		/* switch high byte and low byte */
		temp_reg = REVERT_HL_BYTE(reg_addr);
		writel(SEQU_ADDR_16BIT, i2c->virtbase + I2C_DMA_CMD0);
	} else {
		writel(SEQU_ADDR_8BIT, i2c->virtbase + I2C_DMA_CMD0);
	}

	/* 2. config i2c into DMA mode */
	hi_i2c_dmac_config(i2c, 0x0);

	/* 3. transmit DATA from I2C to DMAC in DMA mode */
	chan = i2c_to_dma((i2c->phybase + I2C_DATA_CMD_REG),
				dma_buf, len);
	if (chan == -1) {
		return -1;
	}

	/* 4. start i2c logic to read */
	hi_i2c_start_rx(i2c, temp_reg, len - 1);

	if (dmac_wait(chan) != DMAC_CHN_SUCCESS) {
		dev_err(i2c->dev, "dma wait failed\n");
		hi_i2c_clr_status(i2c);
		dmac_channel_free(chan);
		return -1;
	}

	hi_i2c_wait_idle(i2c);
	hi_i2c_clr_status(i2c);

	dmac_channel_free(chan);

	return 0;
}
#else
static int hi_i2c_do_dma_write(struct hi_i2c *i2c,
		unsigned int reg_addr, unsigned int reg_addr_num,
		unsigned int dma_buf, unsigned int len)
{
	dev_err(i2c->dev, "DMA is not enabled!");
	return -1;
}

static int hi_i2c_do_dma_read(struct hi_i2c *i2c,
		unsigned int reg_addr, unsigned int reg_addr_num,
		unsigned int dma_buf, unsigned int len)
{
	dev_err(i2c->dev, "DMA is not enabled!");
	return -1;
}
#endif

/**
 * i2c_trylock_adapter - Try to get exclusive access to an I2C bus segment
 * @adapter: Target I2C bus segment
 */
static int i2c_trylock_adapter(struct i2c_adapter *adapter)
{
	struct i2c_adapter *parent = i2c_parent_is_i2c_adapter(adapter);

	if (parent)
		return i2c_trylock_adapter(parent);
	else
		return rt_mutex_trylock(&adapter->bus_lock);
}

int hi_i2c_dma_write(const struct i2c_client *client, unsigned int dma_buf,
		unsigned int reg_addr, unsigned int reg_addr_num,
		unsigned int len)
{
	struct i2c_adapter *adap = client->adapter;
	struct hi_i2c *i2c = i2c_get_adapdata(adap);
	struct i2c_msg msg;
	int status;

	if (in_atomic() || irqs_disabled()) {
		if (!i2c_trylock_adapter(adap))
			/* I2C activity is ongoing. */
			return -EAGAIN;
	} else {
		i2c_lock_adapter(adap);
	}

	memset(&msg, 0x0, sizeof(struct i2c_msg));
	msg.addr = client->addr;
	msg.flags = client->flags;
	msg.len = len;

	i2c->msg = &msg;

	status = hi_i2c_do_dma_write(i2c, reg_addr, reg_addr_num, dma_buf, len);

	i2c_unlock_adapter(adap);

	return status;
}
EXPORT_SYMBOL(hi_i2c_dma_write);

int hi_i2c_dma_read(const struct i2c_client *client, unsigned int dma_buf,
		unsigned int reg_addr, unsigned int reg_addr_num,
		unsigned int len)
{
	struct i2c_adapter *adap = client->adapter;
	struct hi_i2c *i2c = i2c_get_adapdata(adap);
	struct i2c_msg msg;
	int status;

	if (in_atomic() || irqs_disabled()) {
		if (!i2c_trylock_adapter(adap))
			/* I2C activity is ongoing. */
			return -EAGAIN;
	} else {
		i2c_lock_adapter(adap);
	}

	memset(&msg, 0x0, sizeof(struct i2c_msg));
	msg.addr = client->addr;
	msg.flags = client->flags;
	msg.flags |= I2C_M_RD;
	msg.len = len;

	i2c->msg = &msg;

	status = hi_i2c_do_dma_read(i2c, reg_addr, reg_addr_num, dma_buf, len);

	i2c_unlock_adapter(adap);

	return status;
}
EXPORT_SYMBOL(hi_i2c_dma_read);

static void hi_i2c_set_rate(struct hi_i2c *i2c)
{
	unsigned int max_frequency, freq;
	unsigned int clk_rate;
	unsigned int scl_h, scl_l, sda_hold;

	freq = i2c->frequency;
	clk_rate = clk_get_rate(i2c->clk);
	max_frequency = clk_rate >> 1;

	if (freq > max_frequency) {
		i2c->frequency = max_frequency;
		freq = i2c->frequency;
	}

	if (!freq) {
		pr_err("freq can not be zero...\n");
		return;
	}
	
	/* set SCLH and SCLL depend on clk_rate and freq */
	if (freq <= 100000) {
		/* in normal mode		F_scl: freq
		   i2c_scl_hcnt = (F_i2c / F_scl) * 0.5
		   i2c_scl_hcnt = (F_i2c / F_scl) * 0.5
		*/
		scl_h = clk_rate / (freq * 2);
		scl_l = scl_h;
	} else {
		/* in fast mode		F_scl: freq
		   i2c_scl_hcnt = (F_i2c / F_scl) * 0.36
		   i2c_scl_hcnt = (F_i2c / F_scl) * 0.64
		*/
		scl_h = ((clk_rate / 100) * 36) / freq;
		scl_l = ((clk_rate / 100) * 64) / freq;
	}

	writel(scl_h, i2c->virtbase + HII2C_SCL_H);
	writel(scl_l, i2c->virtbase + HII2C_SCL_L);

	sda_hold = scl_h / 2;
	writel(sda_hold, i2c->virtbase + HII2C_SDA_HOLD);
}

static void hi_i2c_hw_init(struct hi_i2c *i2c)
{
	unsigned int val;

	/* unlock hi_i2c controller to access */
	writel(UNLOCK_VALUE, i2c->virtbase + HII2C_LOCK);

	hi_i2c_disable(i2c);

	val = readl(i2c->virtbase + HII2C_CR);
	val |= CR_RESTART_EN;
	writel(val, i2c->virtbase + HII2C_CR);

	hi_i2c_set_rate(i2c);

	hi_i2c_disable_irq(i2c, MASK_ALL_INT);

	writel(0x0, i2c->virtbase + HII2C_TAR);
	hi_i2c_set_slave_addr(i2c, 0, 0);
	hi_i2c_set_mode(i2c, I2C_MODE_SINGLE);
	i2c->last_slave_addr = 0;
	i2c->last_mode = I2C_MODE_SINGLE;

	hi_i2c_enable(i2c);
}

static int hi_i2c_polling_xfer_one_msg(struct hi_i2c *i2c)
{
	int status;
	unsigned val;
	struct i2c_msg *msg = i2c->msg;
	unsigned int tmp;
	unsigned int data_h = 0;
	unsigned int data_l = 0;

	dev_dbg(i2c->dev, "[%s,%d]\n", __func__, __LINE__);

	tmp = !CR1_READ_MODE;
	switch (msg->len) {
	case 2:
		tmp &= ~CR1_H_WIDE;
		tmp &= ~CR1_L_WIDE;

		data_h = msg->buf[0];
		data_l = msg->buf[1];
		break;
	case 3:
		tmp |= CR1_H_WIDE;
		tmp &= ~CR1_L_WIDE;

		data_h = msg->buf[0];
		data_h |= msg->buf[1] << 8;
		data_l = msg->buf[2];
		break;
	case 4:
		tmp |= CR1_H_WIDE;
		tmp |= CR1_L_WIDE;

		data_h = msg->buf[0];
		data_h |= msg->buf[1] << 8;
		data_l = msg->buf[2];
		data_l |=  msg->buf[3] << 8;
		break;
	default:
		dev_err(i2c->dev, "Unsupported this length: %d!\n", msg->len);
		return -EIO;
	}

	if ((msg->addr != i2c->last_slave_addr)
			|| (I2C_MODE_SINGLE != i2c->last_mode)) {
		hi_i2c_set_slave_addr(i2c, msg->addr, msg->flags & I2C_M_TEN);
		hi_i2c_set_mode(i2c, I2C_MODE_SINGLE);
	}

	val = readl(i2c->virtbase + HII2C_CR1);
	val &= ~CR1_H_WIDE & ~CR1_L_WIDE & ~CR1_READ_MODE;
	val |= tmp;
	writel(val, i2c->virtbase + HII2C_CR1);
	do {
		status = hi_i2c_wait_tx_nofull(i2c);
		if (status)
			break;

		val = (data_h << DATA_H_SHIFT) | (data_l << DATA_L_SHIFT);
		writel(val, i2c->virtbase + HII2C_DATA);
	} while (0);

	udelay(hi_i2c_get_delay_val(i2c));

	if (!status)
		status = hi_i2c_wait_idle(i2c);

	hi_i2c_clr_status(i2c);

	return status;
}

static int hi_i2c_polling_xfer_two_msg(struct hi_i2c *i2c)
{
	int status;
	unsigned val;
	struct i2c_msg *msg = i2c->msg;
	unsigned int tmp;
	unsigned int data_h = 0;
	unsigned int data_l = 0;


	dev_dbg(i2c->dev, "[%s,%d]\n", __func__, __LINE__);

	tmp = CR1_READ_MODE;
	switch (msg->len) {
	case 1:
		tmp &= ~CR1_H_WIDE;

		data_h = msg->buf[0];
		break;
	case 2:
		tmp |= CR1_H_WIDE;

		data_h = msg->buf[0];
		data_h |= msg->buf[1] << 8;
		break;
	default:
		dev_err(i2c->dev, "Unsupported this length: %d!\n", msg->len);
		return -EIO;
	}

	msg++;

	switch (msg->len) {
	case 1:
		tmp &= ~CR1_L_WIDE;
		break;
	case 2:
		tmp |= CR1_L_WIDE;
		break;
	default:
		dev_err(i2c->dev, "Unsupported this length: %d!\n", msg->len);
		return -EIO;
	}

	if ((msg->addr != i2c->last_slave_addr)
			|| (I2C_MODE_SINGLE != i2c->last_mode)) {
		hi_i2c_set_slave_addr(i2c, msg->addr, msg->flags & I2C_M_TEN);
		hi_i2c_set_mode(i2c, I2C_MODE_SINGLE);
	}

	val = readl(i2c->virtbase + HII2C_CR1);
	val &= ~CR1_H_WIDE & ~CR1_L_WIDE & ~CR1_READ_MODE;
	val |= tmp;
	writel(val, i2c->virtbase + HII2C_CR1);
	do {
		status = hi_i2c_wait_tx_nofull(i2c);
		if (status)
			break;

		val = data_h << DATA_H_SHIFT;
		writel(val, i2c->virtbase + HII2C_DATA);

		status = hi_i2c_wait_rx_noempty(i2c);
		if (status)
			break;

		data_l = readl(i2c->virtbase + HII2C_DATA) & DATA_L;

		switch (msg->len) {
		case 1:
			msg->buf[0] = data_l & 0xff;
			break;
		case 2:
			msg->buf[0] = data_l & 0xff;
			msg->buf[1] = (data_l >> 8) & 0xff;
			break;
		default:
			status = -EIO;
			dev_err(i2c->dev, "Unsupported this length: %d!\n",
					msg->len);
			break;
		}
	} while (0);

	if (!status)
		status = hi_i2c_wait_idle(i2c);

	hi_i2c_clr_status(i2c);

	return status;
}

static int hi_i2c_dma_xfer_one_msg(struct hi_i2c *i2c)
{
	int status;
	struct i2c_msg *msg = i2c->msg;
	dma_addr_t dma_buf;
	unsigned int reg_addr;
	unsigned int reg_width;

	dev_dbg(i2c->dev, "[%s,%d]\n", __func__, __LINE__);

	if (msg->len < 2) {
		dev_err(i2c->dev, "Unsupported this length: %d!\n", msg->len);
		return -EIO;
	}
	reg_addr = msg->buf[0];
	reg_width = 1;

	dma_buf = dma_map_single(i2c->dev, msg->buf, msg->len, DMA_TO_DEVICE);

	if (dma_mapping_error(i2c->dev, dma_buf)) {
		dev_err(i2c->dev, "DMA mapping failed\n");
		return -EINVAL;
	}

	status = hi_i2c_do_dma_write(i2c, reg_addr, reg_width,
			dma_buf + reg_width, msg->len - reg_width);

	dma_unmap_single(i2c->dev, dma_buf, msg->len, DMA_TO_DEVICE);

	return status;
}

static int hi_i2c_dma_xfer_two_msg(struct hi_i2c *i2c)
{
	int status;
	struct i2c_msg *msg = i2c->msg;
	dma_addr_t dma_buf;
	unsigned int reg_addr;
	unsigned int reg_width;

	dev_dbg(i2c->dev, "[%s,%d]\n", __func__, __LINE__);

	switch (msg->len) {
	case 1:
		reg_addr = msg->buf[0];
		reg_width = 1;
		break;
	case 2:
		reg_addr = msg->buf[0] << 8;
		reg_addr |= msg->buf[1];
		reg_width = 2;
		break;
	default:
		dev_err(i2c->dev, "Unsupported this length: %d!\n", msg->len);
		return -EIO;
	}

	msg++;

	if (msg->len <= 0) {
		dev_err(i2c->dev, "Unsupported this length: %d!\n", msg->len);
		return -EIO;
	}

	dma_buf = dma_map_single(i2c->dev, msg->buf, msg->len, DMA_FROM_DEVICE);

	if (dma_mapping_error(i2c->dev, dma_buf)) {
		dev_err(i2c->dev, "DMA mapping failed\n");
		return -EINVAL;
	}

	status = hi_i2c_do_dma_read(i2c, reg_addr, reg_width, dma_buf,
			msg->len);

	dma_unmap_single(i2c->dev, dma_buf,  msg->len, DMA_FROM_DEVICE);

	return status;
}

static int hi_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs,
		int num)
{
	struct hi_i2c *i2c = i2c_get_adapdata(adap);
	int status;
	struct i2c_msg *msg = i2c->msg;
	unsigned int msg_idx;

	dev_dbg(i2c->dev, "msg_num=%d\n", num);

	i2c->msg = msgs;
	msg_idx = 0;

	while (msg_idx < num) {
		msg = i2c->msg;

		if (msg->flags & I2C_M_RD) {
			status = -EIO;
			dev_err(i2c->dev, "Unsupported read-only!\n");
			break;
		}

		if ((msg_idx < (num - 1)) && ((msg + 1)->flags & I2C_M_RD)) {
			if ((msg + 1)->len <= 2)
				status = hi_i2c_polling_xfer_two_msg(i2c);
			else
				status = hi_i2c_dma_xfer_two_msg(i2c);

			if (status)
				break;

			i2c->msg += 2;
			msg_idx += 2;
		} else {
			if (msg->len <= 4)
				status = hi_i2c_polling_xfer_one_msg(i2c);
			else
				status = hi_i2c_dma_xfer_one_msg(i2c);

			if (status)
				break;

			i2c->msg++;
			msg_idx++;
		}
	}

	if (!status || msg_idx > 0)
		status = msg_idx;
	else
		status = -EIO;

	return status;
}

static u32 hi_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_10BIT_ADDR
		| I2C_FUNC_PROTOCOL_MANGLING;
}

static const struct i2c_algorithm hi_i2c_algorithm = {
	.master_xfer		= hi_i2c_xfer,
	.functionality		= hi_i2c_func,
};

/* hi_i2c_parse_dt
 *
 * Parse the device tree node.
 */
static int hi_i2c_parse_dt(struct hi_i2c *i2c)
{
	struct device *dev = i2c->dev;
	struct device_node *np = dev->of_node;

	if (!np) {
		dev_err(dev, "no dt node defined\n");
		return -ENODEV;
	}

	if (of_property_read_u32(np, "clock-frequency", &i2c->frequency)) {
		dev_dbg(dev, "of read error clock-frequency = %d\n",
				i2c->frequency);
		i2c->frequency = 4000000;
	}

	return 0;
}

static int hi_i2c_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hi_i2c *i2c;
	struct i2c_adapter *adap;
	struct resource *res;
	int status;

	dev_info(dev, "HISILICON I2C V110 bus driver\n");

	adap = devm_kzalloc(&pdev->dev,
			sizeof(struct i2c_adapter) + sizeof(struct hi_i2c),
			GFP_KERNEL);
	if (!adap)
		return -ENOMEM;

	adap->dev.parent = dev;
	adap->dev.of_node = dev->of_node;
	i2c_set_adapdata(adap,  &adap[1]);

	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_DEPRECATED;
	adap->algo = &hi_i2c_algorithm;
	strlcpy(adap->name, "hisi-i2c-v110", sizeof(adap->name));

	i2c = i2c_get_adapdata(adap);
	platform_set_drvdata(pdev, i2c);

	i2c->pdev = pdev;
	i2c->adap = adap;
	i2c->dev = dev;

	status = hi_i2c_parse_dt(i2c);
	if (status)
		goto err_ioremap;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		goto err_ioremap;
	
	i2c->virtbase = devm_ioremap_resource(dev, res);
	if (i2c->virtbase == NULL) {
		status = PTR_ERR(i2c->virtbase);
		dev_err(dev, "cannot ioremap resource\n");
		goto err_ioremap;
	}
	i2c->phybase = res->start;
	dev_info(dev, "mapped registers from 0x%x to 0x%p\n",
		res->start, i2c->virtbase);

	/* find the clock and enable it */
	i2c->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(i2c->clk)) {
		status = PTR_ERR(i2c->clk);
		dev_err(dev, "cannot get clock\n");
		goto err_clk;
	}
	status = clk_prepare_enable(i2c->clk);
	if (status) {
		dev_err(dev, "could not enable clock\n");
		goto err_clk;
	}

	hi_i2c_hw_init(i2c);

	status = i2c_add_adapter(i2c->adap);
	if (status < 0) {
		dev_err(dev, "failed to add bus to i2c core\n");
		goto err_add_adapter;
	}

	return 0;

err_add_adapter:
	clk_disable_unprepare(i2c->clk);
err_clk:
err_ioremap:

	return status;
}

/* hi_i2c_remove
 *
 * called when device is removed from the bus
 */
static int hi_i2c_remove(struct platform_device *pdev)
{
	struct hi_i2c *i2c = platform_get_drvdata(pdev);

	clk_disable_unprepare(i2c->clk);
	i2c_del_adapter(i2c->adap);

	return 0;
}

#ifdef CONFIG_PM
static int hi_i2c_runtime_suspend(struct device *dev)
{
	return 0;
}

static int hi_i2c_runtime_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct hi_i2c *i2c = platform_get_drvdata(pdev);

	hi_i2c_hw_init(i2c);
	return 0;
}
#endif

static const struct dev_pm_ops hi_i2c_dev_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(hi_i2c_runtime_suspend,
			      hi_i2c_runtime_resume)
};

static const struct of_device_id hi_i2c_match[] = {
	{ .compatible = "hisilicon,hisi-i2c-v110"},
	{},
};
MODULE_DEVICE_TABLE(of, hi_i2c_match);

static struct platform_driver hi_i2c_driver = {
	.probe		= hi_i2c_probe,
	.remove		= hi_i2c_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "hisi-i2c-v110",
		.pm	= &hi_i2c_dev_pm_ops,
		.of_match_table = of_match_ptr(hi_i2c_match),
	},
};

module_platform_driver(hi_i2c_driver);

MODULE_DESCRIPTION("HISILICON I2C V110 Bus driver");
MODULE_AUTHOR("BVT OSDRV");
MODULE_LICENSE("GPL");
