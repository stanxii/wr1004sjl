#!/bin/sh
cd linux-3.4.6
make distclean
cp arch/arm/configs/wec9720ek_defconfig ./
make wec9720ek_defconfig
make uImage

