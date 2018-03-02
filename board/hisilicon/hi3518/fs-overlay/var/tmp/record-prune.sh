#!/bin/sh

MIN_FREESPACE=$((512 * 1024 * 1024))

get_mmc_mnt()
{
	mount | awk '( $1 ~ /\/dev\/mmcblk[0-9]p[0-9]/) { print $3 }'
}

get_fs_free_space()
{
    local blksz=$(stat -f "$1" -c "%S")
    local freeblks=$(stat -f "$1" -c "%f")
    echo $(($freeblks * $blksz))
}

mmc_mnt=$(get_mmc_mnt)
[ -n "$mmc_mnt" ] || exit 1

# delete the earliest video record to make enough free space
IFS=$'\n'
for f in $(ls "$mmc_mnt"/*/*/*.avi | sort -u); do
    freebytes=$(get_fs_free_space "$mmc_mnt")
    [ $freebytes -gt $MIN_FREESPACE ] && exit
    rm -f $f
done
