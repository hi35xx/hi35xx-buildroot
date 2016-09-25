#!/bin/sh
# Useage: ./load [ -r|-i|-a ][type]
#         -r : rmmod all modules
#         -i : insmod all modules
#    default : rmmod all moules and then insmod them
#        type: 8d1,8960h,4720p

SDK_TYPE=8d1

if [ $# -ge 2 ]; then
    SDK_TYPE=$2
fi


if [ $SDK_TYPE = "8d1" ]; then
    echo "SDK_TYPE type is $SDK_TYPE"
elif [ $SDK_TYPE = "8960h" ]; then
    echo "SDK_TYPE type is $SDK_TYPE"
elif [ $SDK_TYPE = "4720p" ]; then
    echo "SDK_TYPE type is $SDK_TYPE"
else
    SDK_TYPE=8d1
    echo "SDK_TYPE type is $SDK_TYPE"
fi

report_error()
{
    echo "******* Error!! Shell exit for error *****"
    exit 1
}

insert_audio()
{
    insmod hi3520D_sio.ko
    insmod hi3520D_ai.ko
    insmod hi3520D_ao.ko
    insmod hi3520D_aenc.ko
    insmod hi3520D_adec.ko
}

remove_audio()
{
    rmmod hi3520D_adec
    rmmod hi3520D_aenc
    rmmod hi3520D_ao
    rmmod hi3520D_ai
    rmmod hi3520D_sio     
}

mmz_mem_info()
{
    BASEADDR=$((0x80000000))
    TOTALMEM=$((0x10000000))
    local osmem=$(($(cat /proc/cmdline 2>/dev/null | \
                  sed -nr 's/mem=([^[:space:]]+).*/\1/p' | \
                  sed -r  's/[kK]$/\*1024/;s/[mM]$/\*1024\*1024/')))
    local maddr=$(printf "0x%x" $((BASEADDR+osmem)))
    while [ $osmem -ge $TOTALMEM ]; do TOTALMEM=$((TOTALMEM<<1)); done
    local msize=$(printf "0x%x" $((TOTALMEM-osmem)))

    printf '0x%x,0x%x' ${maddr} $((msize-6*1024*1024))
}

insert_ko()
{
    sh ./pinmux_8D1_hi3520D.sh > /dev/null
    devmem 0x200f0004 32 0x00000003 > /dev/null
    devmem 0x200f0008 32 0x00000003 > /dev/null
    sh ./crgctrl_8D1_hi3520D.sh > /dev/null 
   
    #insmod mmz.ko mmz=anonymous,0,0x84000000,187M:jpeg,0,0x8fb00000,5M anony=1 || report_error
    insmod mmz.ko mmz=anonymous,0,$(mmz_mem_info) anony=1 || report_error
    insmod hiuser.ko
    insmod hi3520D_base.ko
    insmod hi3520D_sys.ko
    insmod hi3520D_tde.ko g_u32TdeTmpBufW=800 g_u32TdeTmpBufH=600
    insmod hi3520D_dsu.ko
    insmod hi3520D_venc.ko
    insmod hi3520D_group.ko
    insmod hi3520D_chnl.ko
    insmod hi3520D_h264e.ko
    insmod hi3520D_rc.ko
    insmod hi3520D_jpege.ko
    insmod hi3520D_viu.ko
    insmod hi3520D_vou.ko
    insmod hi3520D_vpss.ko
    insmod hi3520D_ive.ko
    insmod hi3520D_vda.ko
    insmod hi3520D_region.ko
    insmod vcmp.ko
    insmod hifb.ko video="hifb:vram0_size:8100,vram1_size:1620,vram2_size:1620,vram3_size:32" softcursor="off" u32VcmpBufNum=2
    insmod hi3520D_vdec.ko
    insmod hi3520D_vfmw.ko
    insmod hi3520D_hdmi.ko
    insmod hi3520D_jpegd.ko
    
    insmod extdrv/gpioi2c.ko   
    insmod extdrv/cx26828.ko  > /dev/null
    #insmod extdrv/nvp6114_ex.ko vdec_mode=1 outmode=3
    insmod extdrv/nvp1918.ko
    insert_audio

    if [ $SDK_TYPE = "4720p" ]; then
        sh ./sysctl_hi3520D_4x720P.sh > /dev/null
    else
        sh ./sysctl_hi3520D.sh > /dev/null
    fi    
}

remove_ko()
{
    remove_audio
    #rmmod nvp6114_ex
    rmmod nvp1918
    rmmod cx26828
    rmmod gpioi2c
    rmmod hifb
    rmmod vcmp
    rmmod hi3520D_region
    rmmod hi3520D_ive
    rmmod hi3520D_vda
    rmmod hi3520D_vpss
    rmmod hi3520D_vou
    rmmod hi3520D_viu
    rmmod hi3520D_jpegd.ko
    rmmod hi3520D_hdmi.ko
    rmmod hi3520D_vfmw.ko
    rmmod hi3520D_vdec.ko   
    rmmod hi3520D_jpege
    rmmod hi3520D_rc
    rmmod hi3520D_h264e
    rmmod hi3520D_chnl
    rmmod hi3520D_group
    rmmod hi3520D_venc
    rmmod hi3520D_dsu
    rmmod hi3520D_tde
    rmmod hi3520D_sys
    rmmod hi3520D_base
    rmmod hiuser
    rmmod mmz
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

if [ $# -eq 0 ] || [ "$1" = "-a" ]
then
    remove_ko
    insert_ko
fi
