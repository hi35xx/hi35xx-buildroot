#!/bin/sh
# Useage: ./load3518e.sh [ -r|-i|-a ] [ sensor ]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them
#


# ar0130 mn34031 imx104 icx692 ov9712 9m034 imx122 mt9p006 imx138 soih22 ov2710 himax1375
SNS_A=ov9712

if [ $# -ge 2 ]; then
    SNS_A=$2
fi

report_error()
{
    echo "******* Error: There's something wrong, please check! *****"
    exit 1
}

insert_audio()
{
   insmod acodec.ko
   insmod hidmac.ko
   insmod hi3518_sio.ko
   insmod hi3518_ai.ko
   insmod hi3518_ao.ko
   insmod hi3518_aenc.ko
   insmod hi3518_adec.ko
    echo "insert audio"
}

remove_audio()
{
    rmmod hi3518_adec
    rmmod hi3518_aenc
    rmmod hi3518_ao
    rmmod hi3518_ai
    rmmod hi3518_sio
    rmmod acodec
    rmmod hidmac
    echo "remove audio"
}

insert_sns()
{
    case $SNS_A in
        ar0130|9m034|po3100k)
            devmem  0x20030030 32 0x5;              #Sensor clock 27 MHz
            insmod extdrv/ssp_ad9020.ko;;
        icx692)
            devmem  0x200f000c 32 0x1;              #pinmux SPI0
            devmem  0x200f0010 32 0x1;              #pinmux SPI0
            devmem  0x200f0014 32 0x1;              #pinmux SPI0
            insmod extdrv/ssp_ad9020.ko;;
        mn34031|mn34041)
            devmem  0x200f000c 32 0x1;              #pinmux SPI0
            devmem  0x200f0010 32 0x1;              #pinmux SPI0
            devmem  0x200f0014 32 0x1;              #pinmux SPI0
            devmem  0x20030030 32 0x5;              #Sensor clock 27MHz
            insmod extdrv/ssp_pana.ko;;
        imx104|imx122|imx138)
            devmem  0x200f000c 32 0x1;              #pinmux SPI0
            devmem  0x200f0010 32 0x1;              #pinmux SPI0
            devmem  0x200f0014 32 0x1;              #pinmux SPI0
            devmem  0x20030030 32 0x6;              #Sensor clock 37.125 MHz
            insmod extdrv/ssp_sony.ko;;        
        ov9712|soih22|ov2710)
            devmem  0x20030030 32 0x1;              #Sensor clock 24 MHz
            insmod extdrv/ssp_ad9020.ko;;
        mt9p006)
            devmem  0x20030030 32 0x1;              #Sensor clock 24 MHz
            devmem  0x2003002c 32 0x6a;             #VI input associated clock phase reversed
            insmod extdrv/ssp_ad9020.ko;;
	    hm1375|ar0330)
            devmem  0x20030030 32 0x1;;             #Sensor clock 24 MHz
	    imx236)
            devmem  0x20030030 32 0x6;              #Sensor clock 37.125 MHz
            ;;
        *)
            echo "xxxx Invalid sensor type $SNS_A xxxx"
            report_error;;
    esac
}

remove_sns()
{
    rmmod hi_i2c &> /dev/null
    rmmod ssp &> /dev/null
    rmmod ssp_sony &> /dev/null
    rmmod ssp_pana &> /dev/null
    rmmod ssp_ad9020 &> /dev/null
}

insert_ko()
{
    # low power control
    source ./lowpower.sh > /dev/null

    # pinmux configuration
    source ./pinmux_hi3518.sh rmii i2c > /dev/null

    # clock configuration
    source ./clkcfg_hi3518.sh > /dev/null

    # driver load
    insmod mmz.ko mmz=anonymous,0,0x82800000,24M anony=1 || report_error   #for 3518E use
    insmod hi3518_base.ko
    insmod hi3518_sys.ko
    insmod hiuser.ko

    insmod hi3518_tde.ko
    insmod hi3518_dsu.ko

    insmod hi3518_viu.ko
    insmod hi3518_isp.ko
    insmod hi3518_vpss.ko
    insmod hi3518_vou.ko
    #insmod hi3518_vou.ko detectCycle=0 #close dac detect
    #insmod hifb.ko video="hifb:vram0_size:1620"
    
    insmod hi3518_venc.ko
    insmod hi3518_group.ko
    insmod hi3518_chnl.ko
    insmod hi3518_h264e.ko
    insmod hi3518_jpege.ko
    insmod hi3518_rc.ko
    insmod hi3518_region.ko
    
    insmod hi3518_vda.ko
    insmod hi3518_ive.ko

    insmod extdrv/hi_i2c.ko
    #insmod extdrv/gpioi2c.ko
    #insmod extdrv/gpioi2c_ex.ko
    insmod extdrv/pwm.ko
    #insmod extdrv/sil9024.ko norm=5   #720P@60fps
    
 
    
    insert_sns > /dev/null
    
    #insmod hi3518_isp.ko
    insert_audio
    echo "==== Your input Sensor type is $SNS_A ===="

    # system configuration
    source ./sysctl_hi3518.sh > /dev/null
}

remove_ko()
{
    remove_audio
    remove_sns

    rmmod sil9024 &> /dev/null
    rmmod hi_i2c.ko &> /dev/null
    rmmod pwm
    #rmmod gpioi2c

    rmmod hi3518_ive
    rmmod hi3518_vda

    rmmod hi3518_region
    rmmod hi3518_rc
    rmmod hi3518_jpege
    rmmod hi3518_h264e
    rmmod hi3518_chnl
    rmmod hi3518_group
    rmmod hi3518_venc
  
    rmmod hifb
    rmmod hi3518_vou
    rmmod hi3518_vpss
    rmmod hi3518_isp
    rmmod hi3518_viu

    rmmod hi3518_dsu
    rmmod hi3518_tde
  
    rmmod hiuser
    rmmod hi3518_sys
    rmmod hi3518_base
    rmmod mmz
}

load_usage()
{
    echo "Usage:  ./load3518 [-option] [sensor_name]"
    echo "options:"
    echo "    -i sensor_name           insert modules"
    echo "    -r                       remove modules"
    echo "    -a sensor_name           remove modules first, then insert modules"
    echo "    -h                       help information"
    echo -e "Available sensors: ar0130, imx104, icx692, ov9715, 9m034, imx122, mt9p006"
    echo -e "for example: ./load3518 -a ar0130 \n"
}

# load module.
if [ "$1" = "-i" ]
then
    insert_ko
fi

if [ "$1" = "-r" ]
then
    remove_ko
fi

if [ "$1" = "-h" ]
then
    load_usage
    exit
fi

if [ $# -eq 0 ] || [ "$1" = "-a" ]
then
    remove_ko
    insert_ko
fi
