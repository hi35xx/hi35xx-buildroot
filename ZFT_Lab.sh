#!/bin/sh

# make list-defconfigs        # просмотр доступных архитектур
# make hi3518v100_defconfig   # скопировать конфиг по-умолчанию для выбранной архитектуры
# make menuconfig             # настроить конфиг для сборки

clear
make clean && make -j 7
