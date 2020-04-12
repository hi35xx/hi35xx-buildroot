#!/bin/sh

[ -n "$ACTION" -a -n "$MDEV" ] || exit 1

SNAPSHOTD_PID=/var/run/snapshotd.pid
SNAPSHOT_URI=rtsp://127.0.0.1/snapshot

case ${ACTION} in
  add)
    (
      grep -q "^/dev/$MDEV" /proc/mounts && exit 0
      usleep 100000
      grep -q "^/dev/$MDEV" /proc/mounts && exit 0
      [ -b "/dev/$MDEV" ] || exit 1
      mnt=/run/media/$MDEV
      fstype=$(blkid "/dev/${MDEV}" | sed -nr 's/.*TYPE=\"([^\"]*)\".*/\1/p')
      [ -n "$fstype" ] || exit 0
      [ -d "$mnt" ] || mkdir -p "$mnt"
      mount -t "$fstype" "/dev/${MDEV}" "${mnt}" || exit 1
      # workaround for device without RTC
      #if [ -d "$mnt/Video" ]; then
      #  fts=$(stat -c "%Y" "$mnt/Video")
      #  nts=$(date +%s)
      #  if [ "$fts" -gt "$nts" ]; then
      #    tm=$(($fts + 10))
      #    date -s $(date "+%Y.%m.%d-%H:%M:%S" -d "@$tm")
      #  fi
      #fi
      # start snapshotd
      [ -x /usr/bin/snapshotd ] && {
        start-stop-daemon -S -q -b -p $SNAPSHOTD_PID \
            -x /usr/bin/snapshotd -- -s \
	    -p $SNAPSHOTD_PID -P $mnt/Photo/ $SNAPSHOT_URI
      }
      killall -HUP ppcs-server
    )
    ;;
  remove)
    (
      [ -x /usr/bin/snapshotd ] && {
        start-stop-daemon -K -q -p $SNAPSHOTD_PID
      }
      mnt=$(mount | awk "(\$1 ~ /\/dev\/$MDEV/) { print \$3 }")
      [ -n "$mnt" ] && { umount -l "$mnt"; } && rm -r "$mnt"
      killall -HUP ppcs-server
    )
    ;;
esac
