#!/bin/sh
# Useage: ./load3518e [ -r|-i|-a ] [ sensor ]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them
#

####################Variables Definition##########################
SNS_TYPE=ar0130
mem_start=0x80000000
totmem_size=64M
osmem_size=$(cat /proc/cmdline | sed -nre 's/mem=([^[:space:]]+).*/\1/p')
osmem_size=${osmem_size:=40M}

usage="\
Usage: $0 [OPTIONS]
OPTIONS:
    -i                      insert modules
    -r                      remove modules
    -a                      remove modules, then insert modules
    -h                      display this help message
    -sensor=SENSOR          config sensor type [default: $SNS_TYPE]
    -osmem=SIZE             config OS memory size [default: $osmem_size]
    -totalmem=SIZE          config total memory size [default: $total_size]
    -online                 VI/VPSS online mode
    -offline                VI/VPSS offline mode
    -restore                restore hardware
SENSOR:
    9m034 ar0130 ar0230 imx222 ov9712 ov9732 ov9752
    ov9750 ov2718 mn34222 sc1135 sc1145 sc2135 
EXAMPLES:
    online mode:      $0 -a -osmem 40M -totalmem=64M -online
    offline mode:     $0 -a -osmem 40M -totalmem=64M -offline
"

##################################################################

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

		devmem 0x2003002c 32 0xb4001;	# clk 27MHz, VI 99MHz
		;;
	ar0230)
		devmem 0x200f0040 32 0x2;	# I2C0_SCL
		devmem 0x200f0044 32 0x2;	# I2C0_SDA

		devmem 0x2003002c 32 0xb4005;	# clk 27MHz, VI 148.5MHz
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

		devmem 0x2003002c 32 0x94001;	# clk 37.125MHz, VI 99MHz

		insmod extdrv/sensor_spi.ko;
		;;
	ov9712)
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

		devmem 0x2003002c 32 0xc4001;	# clk 24MHz, VI 99MHz
		;;
	ov9732)
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

		devmem 0x2003002c 32 0xc4001;	# clk 24MHz, VI 99MHz
		;;

	ov9752)
		devmem 0x200f0040 32 0x2;	# I2C0_SCL
		devmem 0x200f0044 32 0x2;	# I2C0_SDA

		devmem 0x2003002c 32 0xc4001;	# clk 24MHz, VI 99MHz
		;;
	ov9750)
		devmem 0x200f0040 32 0x2;	# I2C0_SCL
		devmem 0x200f0044 32 0x2;	# I2C0_SDA

		devmem 0x2003002c 32 0xc4001;	# clk 24MHz, VI 99MHz
		;;

	ov2718)
		devmem 0x200f0040 32 0x2;	# I2C0_SCL
		devmem 0x200f0044 32 0x2;	# I2C0_SDA

		devmem 0x2003002c 32 0xc4001;	# clk 24MHz, VI 99MHz
		;;

	mn34222)
		devmem 0x200f0040 32 0x2;	# I2C0_SCL
		devmem 0x200f0044 32 0x2;	# I2C0_SDA

		devmem 0x2003002c 32 0x94001;	# clk 37.125MHz, VI 99MHz
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

		devmem 0x2003002c 32 0x94003;	# clk 24MHz, VI 99MHz
		;;

	sc1135|sc1145|sc2135)
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

		devmem 0x2003002c 32 0xb4001;	# clk 27MHz, VI 99MHz
		;;

	*)
		echo "Invalid sensor type $SNS_TYPE"
		exit 1;;
	esac
}

remove_sns()
{
	rmmod sensor_spi &> /dev/null
	rmmod sensor_i2c &> /dev/null
}

insert_isp()
{
    case $SNS_TYPE in
        ov9750)
            insmod hi3518e_isp.ko update_pos=1;
            ;;
        *)
            insmod hi3518e_isp.ko update_pos=0;
            ;;
    esac
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
	sh sysctl_hi3518e.sh $online_mode > /dev/null
}

insert_ko()
{
	# sys config
	sys_config;

	# driver load
	local totmem=$((${totmem_size/M/*0x100000}))
	local osmem=$((${osmem_size/M/*0x100000}))
	local mmz_start=$(printf "0x%08x" $((mem_start + osmem)))
	local mmz_size=$(((totmem - osmem)/0x100000))M
	insmod mmz.ko mmz=anonymous,0,$mmz_start,$mmz_size anony=1 || exit 1
	insmod hi_media.ko
	insmod hi3518e_base.ko

	insmod hi3518e_sys.ko vi_vpss_online=$online_mode sensor=$SNS_TYPE
	if [[ $? -ne 0 ]]; then
		exit;
	fi

	insmod hi3518e_tde.ko
	insmod hi3518e_region.ko
	insmod hi3518e_vgs.ko

	insert_isp;
	insmod hi3518e_viu.ko detect_err_frame=10;
	insmod hi3518e_vpss.ko rfr_frame_comp=1;
	#insmod hi3518e_vou.ko
	#insmod hi3518e_vou.ko transparentTransmit=1 #enable transparentTransmit
	#insmod hifb.ko video="hifb:vram0_size:1620"     # default pal

	insmod hi3518e_rc.ko
	insmod hi3518e_venc.ko
	insmod hi3518e_chnl.ko ChnlLowPower=1
	insmod hi3518e_h264e.ko
	insmod hi3518e_jpege.ko
	insmod hi3518e_ive.ko save_power=0;
	#insmod hi3518e_ive.ko
	insmod extdrv/sensor_i2c.ko
	insmod extdrv/pwm.ko
	insmod extdrv/piris.ko

	#insert_sns > /dev/null
	insert_sns
	insert_audio

	insmod hi_mipi.ko
	echo "Sensor TYPE: $SNS_TYPE"
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

	#rmmod hifb
	#rmmod hi3518e_vou
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
	sysctl_hi3518e.sh $online_mode > /dev/null
	insert_sns;
}

######################parse arg###################################
f_insmod=no
f_rmmod=no
online_mode=1
f_restore=no

longopts="-lsensor: -losmem: -ltotalmem: -lonline -loffline -lrestore"
shortopts="irah"
args=$(getopt -a $longopts -- $shortopts "$@") || exit 1
eval set -- "$args"

while true; do
	case "$1" in
	-a)			f_rmmod=yes; f_insmod=yes; shift;;
	-i)			f_insmod=yes; shift;;
	-r)			f_rmmod=yes; shift;;
	--restore)	f_restore=1; shift;;
	--online)   online_mode=1; shift;;
	--offline)	online_mode=0; shift;;
	--sensor)	SNS_TYPE=$2; shift 2;;
	--osmem)	osmem_size=$2; shift 2;;
	--totalmem)	totmem_size=$2; shift 2;;
	-h|--help)	echo "$usage"; exit 0;;
	--)			shift; break;;
	*)			echo "Invalid argument $1"; exit 1;;
	esac
done

[ $((${osmem_size/M/*0x100000})) -lt $((${totmem_size/M/*0x100000})) ] || {
	echo "osmem size is greater than total memory [$osmem_size/$totmem_size]"
	exit;
}

if [ x$f_rmmod = "xyes" ];   then remove_ko; fi
if [ x$f_insmod = "xyes" ];  then insert_ko; fi
if [ x$f_restore = "xyes" ]; then sys_restore; fi

# vi: ts=4 sw=4:
