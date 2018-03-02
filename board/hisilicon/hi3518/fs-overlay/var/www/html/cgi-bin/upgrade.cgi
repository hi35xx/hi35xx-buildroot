#!/bin/sh

get_form_field()
{
  httpd -d $(echo "$1" | sed -nr "s/.*$2=([^&]*).*/\1/p")
}

_query_string=$(dd count=$CONTENT_LENGTH bs=1 2>/dev/null)

printf "Content-type: text/json\n\n"

action=$(get_form_field "$_query_string" action)

case "$action" in
switchmode)
  RAMFS_COPY_DATA="/var/www/html/* /var/www/html/*/*"
  . /lib/scripts/upgrade.sh
  kill_remaining 2>&1 >/dev/null
  if [ $(rootfs_type) != "tmpfs" ]; then
    run_ramfs "echo '{ \"status\":\"OK\" }'"
    echo "{ \"status\":\"FAIL\" }"
  else
    echo "{ \"status\":\"OK\" }"
  fi
  ;;
upgrade)
  . /lib/scripts/upgrade.sh
  file=$(get_form_field "$_query_string" file)
  errmsg=$(default_do_upgrade "$file")
  if [ $? -eq 0 ]; then
    echo "{ \"status\":\"OK\" }"
    # ugrade complete, now restarting
    (reboot -f -d 2 &) 0>&- 1>&- 2>&-
  else
    echo "{ \"status\":\"FAIL\", \"errmsg\":\"$errmsg\" }"
    rm -f "$file"
    # start telnetd for debug
    telnetd
  fi
  ;;
*)
  echo "{ \"status\":\"FAIL\", \"errmsg\":\"invalid action\" }"
  ;;
*)
  ;;
esac
