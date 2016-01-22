#!/bin/sh
# Useage: ./load [ -r|-i|-a ][type]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them
#        type: 16d1,8960h,2hd,4720p,1hd7d1
#

[ $# -ge 2 ] && SDK_TYPE=$2 || SDK_TYPE=16d1
echo "SDK_TYPE is ${SDK_TYPE}"

report_error()
{
    echo "ERROR: $*"
    exit 1
}

insert_audio()
{
    insmod hidmac.ko
    #insmod extdrv/tlv_320aic31.ko > /dev/null
    #insmod extdrv/acodec.ko
    insmod hi3520A_sio.ko
    insmod hi3520A_ai.ko
    insmod hi3520A_ao.ko
    insmod hi3520A_aenc.ko
    insmod hi3520A_adec.ko
}

remove_audio()
{
    rmmod hi3520A_adec
    rmmod hi3520A_aenc
    rmmod hi3520A_ao
    rmmod hi3520A_ai
    rmmod hi3520A_sio
    #rmmod acodec
    #rmmod tlv_320aic31
    rmmod hidmac
}

mmzinfo()
{
    BASEADDR=$((0x80000000))
    TOTALMEM=$((0x10000000))
    JPEGSIZE=$((0x00600000))

    local osmem=$(($(cat /proc/cmdline 2>/dev/null | \
                  sed -nr 's/mem=([^[:space:]]+).*/\1/p' | \
                  sed -r  's/[kK]$/\*1024/;s/[mM]$/\*1024\*1024/')))

    while [ $((osmem+JPEGSIZE)) -ge $TOTALMEM ]; do
        TOTALMEM=$((TOTALMEM<<1));
    done

    local anonaddr=$(printf "0x%x" $((BASEADDR+osmem)))
    local anonsize=$(printf "0x%x" $((TOTALMEM-osmem-JPEGSIZE)))
    local jpegaddr=$(printf "0x%x" $((anonaddr+anonsize)))
    local jpegsize=$(printf "0x%x" $JPEGSIZE)

    printf 'anonymous,0,0x%x,0x%x:jpeg,0,0x%x,0x%x' \
           $anonaddr $anonsize $jpegaddr $jpegsize
}

insert_ko()
{
    case $SDK_TYPE in
    16d1)
        sh ./pinmux_hi3520A_16D1.sh > /dev/null
        devmem 0x20110168 32 0x03ff2
        ;;
    8960h)
        sh ./pinmux_hi3520A_16D1.sh > /dev/null
        ;;
    2hd)
        sh ./pinmux_hi3520A_2X1080P.sh > /dev/null
        ;;
    4720p)
        sh ./pinmux_hi3520A_4X720P.sh > /dev/null
        ;;
    1hd7d1)
        sh ./pinmux_hi3520A_1XHD_7XD1.sh > /dev/null
        ;;
    *)
        echo "Invalid SDK_TYPE [$SDK_TYPE]"
        exit 1
        ;;
    esac

    sh ./clkcfg_hi3520A.sh > /dev/null
    
    # The ddr priority must be set before inserting all modules,
    # otherwise vou will be low bandwidth(or flicker) when the machine
    # is starting up.
    sh ./sysctl_hi3520A.sh > /dev/null

    insmod mmz.ko mmz=$(mmzinfo) anony=1 || report_error "error loading mmz.ko"
    insmod hi3520A_base.ko
    insmod hi3520A_sys.ko
    insmod hi3520A_tde.ko g_u32TdeTmpBufW=0 g_u32TdeTmpBufH=0
    insmod hi3520A_dsu.ko
    insmod hi3520A_venc.ko
    insmod hi3520A_group.ko
    insmod hi3520A_chnl.ko
    insmod hi3520A_h264e.ko
    insmod hi3520A_rc.ko
    insmod hi3520A_jpege.ko
    insmod hi3520A_viu.ko
    insmod hi3520A_vou.ko
    insmod hi3520A_vpss.ko
    insmod hi3520A_ive.ko
    insmod hi3520A_vda.ko
    insmod hi3520A_region.ko
    insmod vcmp.ko
    insmod hifb.ko video="hifb:vram0_size:8100,vram1_size:1620,vram2_size:1620,vram3_size:32" softcursor="off" u32VcmpBufNum=2
    insmod hi3520A_vdec.ko
    insmod hi3520A_vfmw.ko
    insmod hi3520A_hdmi.ko
    insmod jpeg.ko
    insmod extdrv/gpioi2c.ko
    insmod extdrv/nvp1918.ko
    insmod extdrv/ssp.ko
    insert_audio
}

remove_ko()
{
    remove_audio
    rmmod ssp
    rmmod nvp1918
    rmmod gpioi2c
    rmmod hifb
    rmmod vcmp
    rmmod hi3520A_region
    rmmod hi3520A_ive
    rmmod hi3520A_vda
    rmmod hi3520A_vpss
    rmmod hi3520A_vou
    rmmod hi3520A_viu
    rmmod jpeg
    rmmod hi3520A_hdmi.ko
    rmmod hi3520A_vfmw.ko
    rmmod hi3520A_vdec.ko   
    rmmod hi3520A_jpege
    rmmod hi3520A_rc
    rmmod hi3520A_h264e
    #rmmod hi3526_mpeg4e
    rmmod hi3520A_chnl
    rmmod hi3520A_group
    rmmod hi3520A_venc
    rmmod hi3520A_dsu
    rmmod hi3520A_tde
    rmmod hi3520A_sys
    rmmod hi3520A_base
    rmmod mmz
}

case "$1" in
    -i) insert_ko ;;
    -r) remove_ko ;;
    *)  remove_ko; insert_ko ;;
esac

