#!/bin/sh
# Useage: ./load3518e [ -r|-i|-a ] [ sensor ]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them
#


# imx222 9m034 ar0230 ar0130 sc2035 sc1135 sc2135 mn34222


####################Variables Definition##########################

SNS_TYPE=sc2035             # sensor type

mem_total=64;               # 64M, total mem
mem_start=0x80000000;       # phy mem start

os_mem_size=32;             # 32M, os mem
mmz_start=0x82000000;       # mmz start addr
mmz_size=32M;               # 32M, mmz size


##################################################################

report_error()
{
	echo "******* Error: There's something wrong, please check! *****"
	exit 1
}

insert_audio()
{
	insmod acodec.ko
	insmod hi3518e_aio.ko
	insmod hi3518e_ai.ko
	insmod hi3518e_ao.ko
	insmod hi3518e_aenc.ko
	insmod hi3518e_adec.ko
	#insmod extdrv/tlv_320aic31.ko
	echo "insert audio"
}

remove_audio()
{
	#rmmod tlv_320aic31.ko
	rmmod hi3518e_adec
	rmmod hi3518e_aenc
	rmmod hi3518e_ao
	rmmod hi3518e_ai
	rmmod hi3518e_aio
	rmmod acodec
	echo "remove audio"
}

insert_sns()
{
	case $SNS_TYPE in
		ar0130|9m034)
			devmem 0x200f0040 32 0x2;	# I2C0_SCL
			devmem 0x200f0044 32 0x2;	# I2C0_SDA

			#cmos pinmux
			devmem 0x200f007c 32 0x1;	# VI_DATA13
			devmem 0x200f0080 32 0x1;	# VI_DATA10
			devmem 0x200f0084 32 0x1;	# VI_DATA12
			devmem 0x200f0088 32 0x1;	# VI_DATA11
			devmem 0x200f008c 32 0x2;	# VI_VS
			devmem 0x200f0090 32 0x2;	# VI_HS
			devmem 0x200f0094 32 0x1;	# VI_DATA9

			devmem 0x2003002c 32 0xb4001;	# sensor unreset, clk 27MHz, VI 99MHz
			;;
		ar0230)
			devmem 0x200f0040 32 0x2;	# I2C0_SCL
			devmem 0x200f0044 32 0x2;	# I2C0_SDA

			devmem 0x2003002c 32 0xb4005;	# sensor unreset, clk 27MHz, VI 148.5MHz
			devmem 0x20030104 32 0x1;	# vpss 148.5MHz
			;;
		imx222)
			devmem 0x200f0040 32 0x1	# SPI0_SCLK
			devmem 0x200f0044 32 0x1	# SPI0_SDO
			devmem 0x200f0048 32 0x1	# SPI0_SDI
			devmem 0x200f004c 32 0x1	# SPI0_CSN

			#cmos pinmux
			devmem 0x200f007c 32 0x1;	# VI_DATA13
			devmem 0x200f0080 32 0x1;	# VI_DATA10
			devmem 0x200f0084 32 0x1;	# VI_DATA12
			devmem 0x200f0088 32 0x1;	# VI_DATA11
			devmem 0x200f008c 32 0x2;	# VI_VS
			devmem 0x200f0090 32 0x2;	# VI_HS
			devmem 0x200f0094 32 0x1;	# VI_DATA9

			devmem 0x2003002c 32 0x94001;	# sensor unreset, clk 37.125MHz, VI 99MHz

			insmod extdrv/sensor_spi.ko;
			;;
		mn34220)
			devmem 0x200f0040 32 0x2;	# I2C0_SCL
			devmem 0x200f0044 32 0x2;	# I2C0_SDA

			devmem 0x2003002c 32 0xc4001;	# sensor unreset, clk 24MHz, VI 99MHz
			;;
		mn34222)
			devmem 0x200f0040 32 0x2;	# I2C0_SCL
			devmem 0x200f0044 32 0x2;	# I2C0_SDA

			devmem 0x2003002c 32 0x94001;	# sensor unreset, clk 37.125MHz, VI 99MHz
			;;

		ov4682)
			devmem 0x200f0040 32 0x2;	# I2C0_SCL
			devmem 0x200f0044 32 0x2;	# I2C0_SDA

			devmem 0x2003002c 32 0xc4001;	# sensor unreset, clk 24MHz, VI 99MHz
			;;
		sc1045|sc2035|sc2045|sc1135|sc2135)
			devmem 0x200f0040 32 0x2;	# I2C0_SCL
			devmem 0x200f0044 32 0x2;	# I2C0_SDA

			#cmos pinmux
			devmem 0x200f007c 32 0x1;	# VI_DATA13
			devmem 0x200f0080 32 0x1;	# VI_DATA10
			devmem 0x200f0084 32 0x1;	# VI_DATA12
			devmem 0x200f0088 32 0x1;	# VI_DATA11
			devmem 0x200f008c 32 0x2;	# VI_VS
			devmem 0x200f0090 32 0x2;	# VI_HS
			devmem 0x200f0094 32 0x1;	# VI_DATA9

			devmem 0x2003002c 32 0xb4001;	# sensor unreset, clk 27MHz, VI 99MHz
			;;
		bt1120)
			devmem 0x200f0008 32 0x4;	# VI_VS
			devmem 0x200f000c 32 0x4;	# VI_HS
			devmem 0x200f007c 32 0x1;	# VI_DATA13
			devmem 0x200f0080 32 0x1;	# VI_DATA10
			devmem 0x200f0084 32 0x1;	# VI_DATA12
			devmem 0x200f0088 32 0x1;	# VI_DATA11
			devmem 0x200f008c 32 0x1;	# VI_DATA15
			devmem 0x200f0090 32 0x1;	# VI_DATA14
			devmem 0x200f0094 32 0x1;	# VI_DATA9

			devmem 0x2003002c 32 0x94003;	# sensor unreset, clk 24MHz, VI 99MHz
			;;

		*)
			echo "xxxx Invalid sensor type $SNS_TYPE xxxx"
			report_error;;
	esac
}

remove_sns()
{
	rmmod sensor_spi &> /dev/null
	rmmod sensor_i2c &> /dev/null
}

sys_config()
{
	# pinmux configuration
	#sh ./pinmux_hi3518e.sh -vo BT656 > /dev/null
	#sh ./pinmux_hi3518e.sh -vo LCD > /dev/null
	sh ./pinmux_hi3518e.sh -net > /dev/null

	# clock configuration
	sh clkcfg_hi3518e.sh > /dev/null

	# system configuration
	sh sysctl_hi3518e.sh $b_arg_online  > /dev/null
}

insert_ko()
{
	# sys config
	sys_config;

	# driver load
	insmod mmz.ko mmz=anonymous,0,$mmz_start,$mmz_size anony=1 || report_error
	insmod hi_media.ko
	insmod hi3518e_base.ko

	insmod hi3518e_sys.ko vi_vpss_online=$b_arg_online sensor=$SNS_TYPE

	insmod hi3518e_tde.ko
	insmod hi3518e_region.ko
	insmod hi3518e_vgs.ko

	insmod hi3518e_isp.ko
	insmod hi3518e_viu.ko detect_err_frame=10;
	insmod hi3518e_vpss.ko rfr_frame_comp=1;
	insmod hi3518e_vou.ko
	#insmod hi3518e_vou.ko transparentTransmit=1 #enable transparentTransmit
	insmod hifb.ko video="hifb:vram0_size:1620"     # default pal

	insmod hi3518e_rc.ko
	insmod hi3518e_venc.ko
	insmod hi3518e_chnl.ko ChnlLowPower=1
	insmod hi3518e_h264e.ko
	insmod hi3518e_jpege.ko
	insmod hi3518e_ive.ko save_power=0;
#	insmod hi3518e_ive.ko
	insmod extdrv/sensor_i2c.ko
	echo "==== Your input Sensor type is $SNS_TYPE ===="
	insmod extdrv/pwm.ko
	insmod extdrv/piris.ko

	#insert_sns > /dev/null
	insert_sns
	insert_audio

	insmod hi_mipi.ko
	echo "==== Your input Sensor type is $SNS_TYPE ===="
}

remove_ko()
{
	remove_audio
	remove_sns

	rmmod pwm

	rmmod hi3518e_ive

	rmmod hi3518e_rc
	rmmod hi3518e_jpege
	rmmod hi3518e_h264e
	rmmod hi3518e_chnl
	rmmod hi3518e_venc

	rmmod hifb
	rmmod hi3518e_vou
	rmmod hi3518e_vpss
	rmmod hi3518e_viu
	rmmod hi_mipi

	rmmod hi3518e_vgs
	rmmod hi3518e_region
	rmmod hi3518e_tde

	rmmod piris
	rmmod hi3518e_isp
	rmmod hi3518e_sys
	rmmod hi3518e_base
	rmmod hi_media
	rmmod mmz
}

sys_restore()
{
    ####################################################
	pinmux_hi3518e.sh -net > /dev/null
	clkcfg_hi3518e.sh > /dev/null

	# system configuration
	sysctl_hi3518e.sh $b_arg_online  > /dev/null
	insert_sns;
}

load_usage()
{
	echo "Usage:  ./load3518e [-option] [sensor_name]"
	echo "options:"
	echo "    -i                       insert modules"
	echo "    -r                       remove modules"
	echo "    -a                       remove modules first, then insert modules"
	echo "    -sensor sensor_name      config sensor type [default: ar0230]"
	echo "    -osmem os_mem_size       config os mem size [unit: M, default: 32]"
	echo "    -total_mem_size          config total mem size [unit: M, default: 64]"
	echo "    -offline                 vi/vpss offline"
	echo "    -h                       help information"
	echo -e "Available sensors: imx222 9m034 ov9752 ar0230 ar0130 ov9712 mn34222 sc2035 sc1135 sc2135"
	echo -e "notes: osmem option can't be used when mmz zone partition is enable\n\n"
	echo -e "for example online:   ./load3518e -a -sensor ar0230 -osmem 32 -total 64\n"
	echo -e "            offline:  ./load3518e -a -sensor ar0230 -osmem 32 -total 64 -offline\n"
}

calc_mmz_info()
{
	mmz_start=`echo "$mem_start $os_mem_size" |
	awk 'BEGIN { temp = 0; }
	{
		temp = $1/1024/1024 + $2;
	}
	END { printf("0x%x00000\n", temp); }'`

	mmz_size=`echo "$mem_total $os_mem_size" |
	awk 'BEGIN { temp = 0; }
	{
		temp = $1 - $2;
	}
	END { printf("%dM\n", temp); }'`
	echo "mmz_start: $mmz_start, mmz_size: $mmz_size"
}


######################parse arg###################################
b_arg_os_mem=0
b_arg_total_mem=0
b_arg_sensor=0
b_arg_insmod=0
b_arg_remove=0
b_arg_online=1
b_arg_restore=0

for arg in $@
do
	if [ $b_arg_total_mem -eq 1 ]; then
		b_arg_total_mem=0;
		mem_total=$arg;

		if [ -z $mem_total ]; then
			echo "[error] mem_total is null"
			exit;
		fi
	fi

	if [ $b_arg_os_mem -eq 1 ] ; then
		b_arg_os_mem=0;
		os_mem_size=$arg;

		if [ -z $os_mem_size ]; then
			echo "[error] os_mem_size is null"
			exit;
		fi
	fi

	if [ $b_arg_sensor -eq 1 ] ; then
		b_arg_sensor=0
		SNS_TYPE=$arg;
	fi

	case $arg in
		"-i")
			b_arg_insmod=1;
			;;
		"-r")
			b_arg_remove=1;
			;;
		"-a")
			b_arg_insmod=1;
			b_arg_remove=1;
			;;
		"-h")
			load_usage;
			;;
		"-sensor")
			b_arg_sensor=1;
			;;
		"-osmem")
			b_arg_os_mem=1;
			;;
		"-total")
			b_arg_total_mem=1;
			;;
		"-restore")
			b_arg_restore=1;
			;;
		"-offline")
			b_arg_online=0;
			;;
	esac
done
#######################parse arg end########################

if [ $os_mem_size -ge $mem_total ] ; then
	echo "[err] os_mem[$os_mem_size], over total_mem[$mem_total]"
	exit;
fi

calc_mmz_info;

#######################Action###############################

if [ $# -lt 1 ]; then
    load_usage;
    exit 0;
fi

if [ $b_arg_remove -eq 1 ]; then
	remove_ko;
fi

if [ $b_arg_insmod -eq 1 ]; then
	insert_ko;
fi

if [ $b_arg_restore -eq 1 ]; then
	sys_restore;
fi

