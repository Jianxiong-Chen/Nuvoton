#!/bin/bash 
make ARCH=arm CROSS_COMPILE=/home/cjx/tools/toolchainNUC980/host/usr/bin/arm-none-linux-gnueabi- distclean 
make ARCH=arm CROSS_COMPILE=/home/cjx/tools/toolchainNUC980/host/usr/bin/arm-none-linux-gnueabi-  nuc980_cjx_defconfig
make ARCH=arm CROSS_COMPILE=/home/cjx/tools/toolchainNUC980/host/usr/bin/arm-none-linux-gnueabi-  menuconfig
make V=1 ARCH=arm CROSS_COMPILE=/home/cjx/tools/toolchainNUC980/host/usr/bin/arm-none-linux-gnueabi- -j16

