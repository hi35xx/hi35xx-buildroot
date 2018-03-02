#!/bin/sh

# POST upload format:
# -----------------------------29995809218093749221856446032^M
# Content-Disposition: form-data; name="file1"; filename="..."^M
# Content-Type: application/octet-stream^M
# ^M    <--------- headers end with empty line
# file contents
# file contents
# file contents
# ^M    <--------- extra empty line
# -----------------------------29995809218093749221856446032--^M

uploaddir=/tmp
file=$uploaddir/$$-$RANDOM
mkdir -p $uploaddir

CR=`printf '\r'`

# CGI output must start with at least empty line (or headers)
printf "Content-type: text/json\r\n\r\n"

saved_IFS=$IFS
IFS="$CR"
read -r delim_line
IFS=""

while read -r line; do
    if echo $line | grep -q '^Content-Disposition:'; then
        fn=$(echo $line | sed -nre 's/.*filename="(.*)".*/\1/p')
        fn=${fn##*\\}
        fn=${fn##*/}
        [ -z "$fn" ] || file=$uploaddir/$fn
    fi
    test x"$line" = x"" && break
    test x"$line" = x"$CR" && break
done

cat >"$file"

# We need to delete the tail of "\r\ndelim_line--\r\n"
tail_text=$(printf "\r\n%s--\r\n" "$delim_line")
tail_len=$((${#delim_line} + 6))

# Get and check file size
filesize=`stat -c"%s" "$file"`
[ "$filesize" -lt "$tail_len" ] && exit 1
realsize=$((filesize - tail_len))

# Check that tail is correct
if ! echo $tail_text | cmp -s $file - $realsize 0; then
    printf "{\"status\":\"FAIL\",\"errmsg\":\"Malformed file upload\"}"
    exit 1
fi

# Truncate the file
dd of="$file" seek=$realsize bs=1 count=0 >/dev/null 2>/dev/null

# Begin upgrade
IFS=$saved_IFS

#printf "Upgrade complete."
printf "{\"status\":\"OK\",\"file\":\"$file\",\"size\":$filesize}"
