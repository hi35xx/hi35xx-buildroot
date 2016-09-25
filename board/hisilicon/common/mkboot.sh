#!/bin/sh

me=$(basename $0)
usage="
Usage: $me [OPTIONS] <BOOTLOADER> <CONFIG1> [CONFIG2]

OPTIONS:
  -h,--help	            display this help and exit
  -f,--offset=OFFSET        offset of the configuration header to be write
                            to the bootloader, currently it should be 64.
  -s,--chsize               configuration header size [default:4096]
                            for hi3520d, this value should be 2400.
  -o,--output=OUTPUT        output file to be generated

BOOTLOADER
  The bootloader to be signed.

CONFIG1,CONFIG2
  The Configuration Header file, for example reg_info_hi3518e.bin
"

fatal() {
  echo "$*" >&2
  exit 1
}

offset=64
chsize=4096

while [ $# -gt 0 ]; do
  case $1 in
  -h|--help)
    echo "$usage" ; exit ;;
  -f)
    shift ; offset=$1 ; shift ;;
  --offset=*)
    offset=$(expr "X$1" : '[^=]*=\(.*\)') ; shift ;;
  -s)
    shift ; chsize=$1 ; shift ;;
  --chsize=*)
    chsize=$(expr "X$1" : '[^=]*=\(.*\)') ; shift ;;
  -o)
    shift ; output=$1 ; shift ;;
  --output=*)
    output=$(expr "X$1" : '[^=]*=\(.*\)') ; shift ;;
  -*)
    fatal "Invalid option '$1'" ;;
  *) # Stop option processing
    break ;;
  esac
done

[ $# -ge 2 ] || { echo "$usage"; exit; }

blfile=$1 ; shift
[ -r "$blfile" ] || {
  fatal "Please check whether if \"$blfile\" has read permission."
}

blsize=$(stat -c "%s" $blfile)
[ "$blsize" -gt $((chsize+offset)) ] || {
    fatal "\"$blfile\" is too small, is it a valid bootloader?"
}

if [ -n "$output" ]; then
  cp -f "$blfile" "$output" || {
    fatal "Generating \"$output\" failed."
  }
else
  output="$blfile"
  [ -w "$output" ] || fatal "Bootloader \"$output\" has no write permission."
fi

while [ $# -gt 0 ]; do
  chfile=$1 ; shift
  chfsize=$(stat -c "%s" "$chfile")
  [ "$chfsize" -lt $chsize ] || {
      fatal "\"$chfile\" is too large, is it a configuration header?"
  }

  echo "adding header '$(basename $chfile)' at ${offset}-$((offset+chsize))"

  dd if=$chfile of=$output \
     conv=notrunc,sync oflag=seek_bytes \
     bs=$chsize count=1 seek=$offset

  offset=$((offset+chsize))

done

