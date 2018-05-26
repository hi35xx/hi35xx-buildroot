#!/bin/bash


clear
echo ""
build=no


hi3518v1()
{
  # Copy default HI3518v1 config
  make hi3518v100_defconfig
  #
  # Clean and compile
  make clean && time make -j 7
  #
  # Check time of building
  DATE=$(date +%Y%m%d)
  #
  # Check output dir and move firmware
  [ -d zft_lab ] || mkdir -p zft_lab
  cp -v output/images/uImage zft_lab/uImage-Buildroot-HI3518Xv1-XM-${DATE}.bin
  #
}


hi3518v2()
{
  # Copy default HI3518v2 config
  make hi3518v200_defconfig
  #
  # Clean and compile
  make clean && time make -j 7
  #
  # Check time of building
  DATE=$(date +%Y%m%d)
  #
  # Check output dir and move firmware
  [ -d zft_lab ] || mkdir -p zft_lab
  cp -v output/images/uImage zft_lab/uImage-Buildroot-HI3518Xv2-XM-${DATE}.bin
  #
}

hi3520dv2()
{
  # Copy default HI3518v2 config
  make hi3520dv200_defconfig
  #
  # Clean and compile
  make clean && time make -j 7
  #
  # Check time of building
  DATE=$(date +%Y%m%d)
  #
  # Check output dir and move firmware
  #[ -d zft_lab ] || mkdir -p zft_lab
  #cp -v output/images/uImage zft_lab/uImage-Buildroot-HI3518Xv2-XM-${DATE}.bin
  #
}


upload()
{
  # Upload current firmware to TFTP server
  scp output/images/uImage root@172.28.200.72:/srv/tftp/
  #
  # Upload current firmware to Desktop
  scp output/images/uImage zig@172.28.200.74:~
}


if [ $# -ge 1 ]; then
  build=$1
fi


case $build in

  all)
    echo "Start building all firmware";
    hi3518v1;
    hi3518v2;
    ;;

  hi3518v1)
    echo "Start building hi3518v1 firmware";
    hi3518v1;
    ;;

  hi3518v2)
    echo "Start building hi3518v2 firmware";
    hi3518v2;
    ;;

  hi3520dv2)
    echo "Start building hi3518v2 firmware";
    hi3520dv2;
    ;;

  upload)
    echo "Start uploading firmware";
    upload;
    ;;

  *)
    echo "Please select: all, hi3518v1, hi3518v2, hi3520dv2 or upload";
    sleep 3
    ;;

esac

