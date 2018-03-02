#!/bin/sh

prepare() {
    [ ! -f /sys/class/gpio/export ] && exit
    [ ! -d /sys/class/gpio/gpio$1 ] && echo $1 > /sys/class/gpio/export
    [ -f /sys/class/gpio/gpio$1/direction ] && echo out > /sys/class/gpio/gpio$1/direction
}

blink() {
    [ ! -f /sys/class/gpio/gpio$1/value ] && exit
    while [ true ]; do
        echo 1 > /sys/class/gpio/gpio$1/value
        sleep 1
        echo 0 > /sys/class/gpio/gpio$1/value
        sleep 1
    done
}

if [ $# -ge 1 ]; then
    prepare $1
    blink $1
fi
