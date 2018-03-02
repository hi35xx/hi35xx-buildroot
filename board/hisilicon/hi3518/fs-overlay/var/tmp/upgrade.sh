#!/bin/sh

RAM_ROOT=/tmp/root

[ -x /usr/bin/ldd ] || ldd() { LD_TRACE_LOADED_OBJECTS=1 $*; }
libs() { ldd $* 2>/dev/null | sed -r 's/(.* => )?(.*) .*/\2/'; }

install_file() { # <file> [ <file> ... ]
	for file in "$@"; do
		dest="$RAM_ROOT/$file"
		[ -f $file -a ! -f $dest ] && {
			dir="$(dirname $dest)"
			mkdir -p "$dir"
			cp $file $dest
		}
	done
}

install_bin() { # <file> [ <symlink> ... ]
	src=$1
	files=$1
	[ -x "$src" ] && files="$src $(libs $src)"
	install_file $files
	shift
	for link in "$@"; do {
		dest="$RAM_ROOT/$link"
		dir="$(dirname $dest)"
		mkdir -p "$dir"
		[ -f "$dest" ] || ln -s $src $dest
	}; done
}

supivot() { # <new_root> <old_root>
	/bin/mount | grep "on $1 type" 2>&- 1>&- || /bin/mount -o bind $1 $1
	mkdir -p $1$2 $1/proc $1/sys $1/dev $1/tmp $1/run $1/overlay $1/var && \
	/bin/mount -o noatime,move /proc $1/proc && \
	pivot_root $1 $1$2 || {
		/bin/umount -l $1 $1
		return 1
	}

	/bin/mount -o noatime,move $2/sys /sys
	/bin/mount -o noatime,move $2/dev /dev
	/bin/mount -o noatime,move $2/tmp /tmp
	/bin/mount -o noatime,move $2/run /run
	/bin/mount -o noatime,move $2/overlay /overlay 2>&-
	return 0
}

run_ramfs() { # <command> [...]
	install_bin /bin/busybox /bin/ash /bin/sh /bin/mount /bin/umount	\
		/sbin/pivot_root /sbin/reboot /bin/sync /bin/dd	/bin/grep       \
		/bin/cp /bin/mv /bin/tar /usr/bin/md5sum "/usr/bin/[" /bin/dd	\
		/bin/vi /bin/ls /bin/cat /usr/bin/awk /usr/bin/hexdump		\
		/bin/sleep /bin/zcat /usr/bin/bzcat /usr/bin/printf /usr/bin/wc \
		/bin/cut /usr/bin/printf /bin/sync /bin/mkdir /bin/rmdir	\
		/bin/rm /usr/bin/basename /bin/kill /bin/chmod /sbin/getty

	for file in $RAMFS_COPY_BIN; do
		install_bin ${file//:/ }
	done

	install_file /etc/inittab /etc/init.d/rc[SK]
	install_file /etc/passwd /etc/group /etc/shadow
	install_file /etc/fw_env.config /etc/resolv.conf
	install_file /lib/*/*.sh $RAMFS_COPY_DATA

	for file in /var/cache /var/lock /var/log /var/run /var/tmp; do
	    cp -d $file $RAM_ROOT$file
	done

	[ -L "/lib32" ] && ln -s /lib $RAM_ROOT/lib32
	[ -L "/usr/lib32" ] && ln -s /usr/lib $RAM_ROOT/usr/lib32

	supivot $RAM_ROOT /mnt || {
		echo "Failed to switch over to ramfs. Please reboot."
		return 1
	}

	/bin/mount -o remount,ro /mnt
	/bin/umount -l /mnt

	grep /overlay /proc/mounts > /dev/null && {
		/bin/mount -o noatime,remount,ro /overlay
		/bin/umount -l /overlay
	}

	if [ -x /bin/busybox ]; then
		mkdir -p /bin /sbin /usr/bin /usr/sbin
		/bin/busybox --install -s
	fi

	kill -HUP 1

	# spawn a new shell from ramdisk to reduce the probability of cache issues
	exec /bin/busybox ash -c "$*"
}

kill_remaining() { # [ <signal> ]
	local sig="${1:-TERM}"
	echo -n "Sending $sig to remaining processes ... "

	local my_pid=$$
	local my_ppid=$(cut -d' ' -f4  /proc/$my_pid/stat)

	local stat
	for stat in /proc/[0-9]*/stat; do
		[ -f "$stat" ] || continue

		local pid name state ppid rest
		read pid name state ppid rest < $stat
		name="${name#(}"; name="${name%)}"

		local cmdline
		read cmdline < /proc/$pid/cmdline

		# Skip kernel threads
		[ -n "$cmdline" ] || continue

		if [ $$ -eq 1 ] || [ $my_ppid -eq 1 ]; then
			# Running as init process, kill everything except me
			if [ $pid -ne $$ ] && [ $pid -ne $my_ppid ]; then
				echo -n "$name "
				kill -$sig $pid 2>/dev/null
			fi
		else 
			case "$name" in
				# Skip essential services
				*ash*|*init*|*watchdog*|*getty*|*login*|*dbus-daemon*|*httpd*|*nginx*|*hiawatha*|*cgiwrap*|*hostapd*|*wpa_supplicant*|*dnsmasq*|*udhcpc*|*udhcpd*|*wifi-config*)
					: ;;

				# Killable process
				*)
					if [ $pid -ne $$ ] && [ $ppid -ne $$ ]; then
						echo -n "$name "
						kill -$sig $pid 2>/dev/null
					fi
				;;
			esac
		fi
	done
	echo ""
}

v() {
	[ "$VERBOSE" -ge 1 ] && echo "$@"
}

rootfs_type() {
	/bin/mount | awk '($3 ~ /^\/$/) && ($5 !~ /rootfs/) { print $5 }'
}

find_mtd_index() {
	local part="$(grep "\"$1\"" /proc/mtd | awk -F: '{print $1}')"
	echo "${part##mtd}"
}

# Flash firmware to MTD partition
#
# $(1): path to image
default_do_upgrade() {
	[ -f "$1" ] || { echo "firmware not found"; return 1; }
	tar -tf "$1" .manifest || { echo "invalid firmware format"; return 1; }
	local manifest=$(tar -Oxf "$1" .manifest)

	local savedIFS="$IFS"
	IFS=$'\n'

	# verify
	for line in $manifest; do
		# ignore empty lines and comment lines
		[ -n "$line" ] || continue
		echo "$line" | grep -q '^\s*#.*' && continue

		local part=$(echo "$line" | cut -d: -f1)
		local file=$(echo "$line" | cut -d: -f2)
		local csum=$(echo "$line" | cut -d: -f3)
		# check if file exists
		tar -tf "$1" "$file" || {
			echo "file $file not found"; IFS="$savedIFS"; return 1
		}
		local ccsum=$(tar -Oxf "$1" "$file" | sha1sum | cut -d ' ' -f1)
		# verify checksum
		[ "${ccsum// /}" = "${csum// /}" ] || {
			echo "checksum not match"; IFS="$savedIFS"; return 1
		}
		# check if partition exists
		[ x$(find_mtd_index "$part") != x ] || {
			echo "partition $part not found"; IFS="$savedIFS"; return 1
		}
	done

	# program
	local workdir=$(mktemp -d /tmp/firmware-XXXXXX)
	for line in $manifest; do
		# ignore empty lines and comment lines
		[ -n "$line" ] || continue
		echo "$line" | grep -q '^\s*#.*' && continue

		local part=$(echo "$line" | cut -d: -f1)
		local file=$(echo "$line" | cut -d: -f2)
		local mtdnr=$(find_mtd_index "$part")

		tar -xf "$1" "$file" -C "$workdir" || {
			echo "error extract file $file"
			IFS="$savedIFS"; rm -rf "$workdir"; return 2
		}
		flashcp "$workdir/$file" "/dev/mtd${mtdnr}" || {
			echo "error program $file to $part"
			IFS="$savedIFS"; rm -rf "$workdir"; return 2
		}
	done
	rm -rf "$workdir"
	IFS="$savedIFS"

	return 0
}

do_upgrade() {
	v "Performing system upgrade..."
	if type 'platform_do_upgrade' >/dev/null 2>/dev/null; then
		platform_do_upgrade "$ARGV"
	else
		default_do_upgrade "$ARGV"
	fi

	v "Upgrade completed"
	[ -n "$DELAY" ] && sleep "$DELAY"

	v "Rebooting system..."
	reboot -f
	sleep 5
	echo b 2>/dev/null >/proc/sysrq-trigger
}

# vi: sw=4 ts=4 ai
