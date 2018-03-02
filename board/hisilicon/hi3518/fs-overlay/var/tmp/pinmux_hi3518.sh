#!/bin/sh

# This is a sample, you should rewrite it according to your chip #

# You can configure your pinmux for the application here!

pinmux_vo_select=mii       #mii, rmii, bt1120
pinmux_i2c_select=i2c      #i2c, gpio_i2c


#SENSOR default setting is Sensor Clk
sensor_clock_select()
{
	devmem  0x200f0008 32 0x00000001 # 0:GPIO1_2   /1:SENSOR_CLK
}

#VICAP default setting is VIU
vicap_pin_mux_select()
{
	devmem  0x200f0140 32 0x00000000 # 0:VIU_CLK   / 1:GPIO11_6
	devmem  0x200f0144 32 0x00000000 # 0:VIU_VS    / 1:GPIO11_5
	devmem  0x200f0148 32 0x00000000 # 0:VIU_HS    / 1:GPIO11_4
	devmem  0x200f014c 32 0x00000000 # 0:VIU_DAT11 / 1:GPIO11_3
	devmem  0x200f0150 32 0x00000000 # 0:VIU_DAT10 / 1:GPIO11_2
	devmem  0x200f0154 32 0x00000000 # 0:VIU_DAT9  / 1:GPIO11_1
	devmem  0x200f0158 32 0x00000000 # 0:VIU_DAT8  / 1:GPIO11_0
	devmem  0x200f015c 32 0x00000000 # 0:VIU_DAT7  / 1:GPIO10_7
	devmem  0x200f0160 32 0x00000000 # 0:VIU_DAT6  / 1:GPIO10_6
	devmem  0x200f0164 32 0x00000000 # 0:VIU_DAT5  / 1:GPIO10_5
	devmem  0x200f0168 32 0x00000000 # 0:VIU_DAT4  / 1:GPIO10_4
	devmem  0x200f016c 32 0x00000000 # 0:VIU_DAT3  / 1:GPIO10_3
	devmem  0x200f0170 32 0x00000000 # 0:VIU_DAT2  / 1:GPIO10_2
	devmem  0x200f0174 32 0x00000000 # 0:VIU_DAT1  / 1:GPIO10_1
	devmem  0x200f0178 32 0x00000000 # 0:VIU_DAT0  / 1:GPIO10_0    
}

#I2C default setting is I2C
i2c_type_select()
{
	devmem  0x200f0018 32 0x00000001 # 0:GPIO2_0   / 1:I2C_SDA
	devmem  0x200f001c 32 0x00000001 # 0:GPIO2_1   / 1:I2C_SCL
}

#I2C default setting is I2C
gpio_i2c_type_select()
{
	devmem  0x200f0018 32 0x00000000 # 0:GPIO2_0   / 1:I2C_SDA
	devmem  0x200f001c 32 0x00000000 # 0:GPIO2_1   / 1:I2C_SCL
}

#MII
net_mii_mode()
{
	devmem  0x200f0030 32  0x1
	devmem  0x200f0034 32  0x1
	devmem  0x200f0038 32  0x1
	devmem  0x200f003C 32  0x1
	devmem  0x200f0040 32  0x1
	devmem  0x200f0044 32  0x1
	devmem  0x200f0048 32  0x1
	devmem  0x200f004C 32  0x1
	devmem  0x200f0050 32  0x1
	devmem  0x200f0054 32  0x1
	devmem  0x200f0058 32  0x1
	devmem  0x200f005C 32  0x1
	devmem  0x200f0060 32  0x1
	devmem  0x200f0064 32  0x1
	devmem  0x200f0068 32  0x1
	devmem  0x200f006C 32  0x1
	devmem  0x200f0070 32  0x1
	devmem  0x200f0074 32  0x1
	devmem  0x200f0078 32  0x1      
}

#RMII    
net_rmii_mode()
{
	devmem  0x200f0030 32  0x1
	devmem  0x200f0034 32  0x1
	devmem  0x200f0038 32  0x1
	devmem  0x200f003C 32  0x1
	devmem  0x200f0040 32  0x1
	devmem  0x200f0044 32  0x1
	devmem  0x200f0048 32  0x1
	devmem  0x200f004C 32  0x1
	devmem  0x200f0050 32  0x1
	devmem  0x200f0054 32  0x1
	devmem  0x200f0058 32  0x1
	devmem  0x200f005C 32  0x3
	devmem  0x200f0060 32  0x1
	devmem  0x200f0064 32  0x1
	devmem  0x200f0068 32  0x1  #MII_TXER 0x1,GPIO2_6 0x0
	devmem  0x200f006C 32  0x1  #MII_RXER 0x1,GPIO2_7 0x0
	devmem  0x200f0070 32  0x1
	devmem  0x200f0074 32  0x1
	devmem  0x200f0078 32  0x1
}      
      
#BT1120
vo_bt1120_mode()
{
	devmem  0x200f0030 32  0x2  #VOU1120_DATA10,MII_CRS 0x1,GPIO3_0 0x0
	devmem  0x200f0034 32  0x2  #VOU1120_DATA9,MII_COL 0x1,GPIO3_1 0x0
	devmem  0x200f0038 32  0x2  #VOU1120_DATA15,MII_RXD3 0x1,GPIO4_3 0x0  
	devmem  0x200f003C 32  0x2  #VOU1120_DATA11,MII_RXD2 0x1,GPIO4_2 0x0
	devmem  0x200f0040 32  0x2  #VOU1120_DATA8,MII_RXD1 0x1,GPIO4_1 0x0
	devmem  0x200f0044 32  0x2  #VOU1120_DATA12,MII_RXD0 0x1,GPIO4_0 0x0
	devmem  0x200f0048 32  0x2  #VOU1120_DATA3,MII_TXD3 0x1,GPIO4_7 0x0
	devmem  0x200f004C 32  0x2  #VOU1120_DATA13,MII_TXD2 0x1,GPIO4_6 0x0
	devmem  0x200f0050 32  0x2  #VOU1120_DATA0,MII_TXD1 0x1,GPIO4_5 0x0
	devmem  0x200f0054 32  0x2  #VOU1120_DATA4,MII_TXD0 0x1,GPIO4_4 0x0
	devmem  0x200f0058 32  0x2  #VOU1120_CLK,MII_RXCK 0x1,GPIO3_2 0x0
	devmem  0x200f005C 32  0x2  #VOU1120_DATA7,MII_TXCK 0x1,GPIO3_3 0x0,RMII_CK 0x3
	devmem  0x200f0060 32  0x2  #VOU1120_DATA1,MII_RXDV 0x1,GPIO3_4 0x0
	devmem  0x200f0064 32  0x2  #VOU1120_DATA5,MII_TXEN 0x1,GPIO3_5 0x0
	devmem  0x200f0070 32  0x2  #VOU1120_DATA2,EPHY_CLK 0x1,GPIO1_3 0x0
	devmem  0x200f0074 32  0x2  #VOU1120_DATA6,MDCK 0x1,GPIO3_6 0x0,BOOTSEL 0x3
	devmem  0x200f0078 32  0x2  #VOU1120_DATA14,MDIO 0x1,GPIO3_7 0x0
}

if [ $# -ge 1 ]; then
    pinmux_vo_select=$1
fi

if [ $# -ge 2 ]; then
    pinmux_i2c_select=$2
fi

case $pinmux_vo_select in
bt1120)
    echo "use bt1120";
    vo_bt1120_mode;
;;
rmii)
    echo "use rmii";
#    net_rmii_mode;
;;
*)
    echo "use mii";
#    net_mii_mode;
;;
esac

case $pinmux_i2c_select in
gpio_i2c)
    echo "use gpio_i2c";
    gpio_i2c_type_select;
;;
*)
    echo "use i2c";
    i2c_type_select;
;;
esac

sensor_clock_select;
#vicap_pin_mux_select;
