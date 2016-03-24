#!/bin/sh

me=$(basename $0)
usage="
Usage: $me [OPTIONS] <CONFIG> <BOOTLOADER>

OPTIONS:
  -h,--help	            display this help and exit
  -o,--output=OUTPUT        file name to be generated

CONFIG
  The Configuration Header file, for example reg_info_hi3518e.bin

BOOTLOADER
  The bootloader to be signed.
"

fatal() {
  echo "$*" >&2
  exit 1
}

while [ $# -gt 0 ]; do
  case $1 in
  -h|--help)
    echo "$usage" ; exit ;;
  -o)
    shift ; output=$1 ; shift ;;
  --output=*)
    output=$(expr "X$1" : '[^=]*=\(.*\)') ; shift ;;
  -*)
    fatal "Invalid option '$1'" ;;
  *)
    [ -z "$ch_file" ] && ch_file=$1 || {
      [ -z "$bl_file" ] && bl_file=$1 || {
        echo "invalid argument $1"
        echo "$usage" ; exit
      }
    }
    shift ;;
  esac
done

[ -n "$ch_file" -a -n "$bl_file" ] || { echo "$usage"; exit; }

[ -f $ch_file -a -r $ch_file ] || {
  fatal "Please check if \"$ch_file\" exists and has read permission."
}
[ -f $bl_file -a -r $ch_file ] || {
  fatal "Please check if \"$bl_file\" exists and has read permission."
}

if [ -n "$output" ]; then
  cp -f $bl_file $output || {
    fatal "Generating \"$output\" failed."
  }
else
  output=$bl_file
  [ -w $output ] || fatal "Bootloader \"$output\" has no write permission."
fi

ch_size=$(stat -c "%s" $ch_file)
[ "$ch_size" -lt 4096 ] || {
    fatal "\"$ch_file\" is too large, is it a valid configuration header?"
}

bl_size=$(stat -c "%s" $bl_file)
[ $bl_size -gt $((4096+64)) ] || {
    fatal "\"$bl_file\" is too small, is it a valid bootloader?"
}

dd if=$ch_file of=$output \
   conv=notrunc,sync oflag=seek_bytes \
   bs=4096 count=1 seek=64

