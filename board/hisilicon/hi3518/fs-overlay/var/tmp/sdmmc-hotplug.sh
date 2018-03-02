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
      [ -d "$mnt" ] || mkdir -p "$mnt"
      fstype=$(blkid "/dev/${MDEV}" | sed -nr 's/.*TYPE=\"([^\"]*)\".*/\1/p')
      mount -t "$fstype" "/dev/${MDEV}" "${mnt}" || exit 1
      [ -x /usr/bin/snapshotd ] && {
        start-stop-daemon -S -q -m -b -p $SNAPSHOTD_PID \
            -x /usr/bin/snapshotd -- -s -l57 -p "$mnt/photo/" "$SNAPSHOT_URI"
      }
    ) &
    ;;
  remove)
    (
      [ -x /usr/bin/snapshotd ] && {
        start-stop-daemon -K -q -p $SNAPSHOTD_PID
      }
      mnt=$(mount | awk "(\$1 ~ /\/dev\/$MDEV/) { print \$3 }")
      [ -n "$mnt" ] && umount "$mnt" && rm -r "$mnt"
    ) &
    ;;
esac
