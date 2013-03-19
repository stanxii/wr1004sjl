#!/bin/sh
./make_kernel.sh
#make_u-boot.sh
./c22_apps.sh
cp ./linux-3.4.6/arch/arm/boot/uImage  ./images/
