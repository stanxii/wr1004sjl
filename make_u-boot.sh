#!/bin/sh
cd u-boot-1.3.4
make clean
make distclean
make at91sam9g20ek_config
make at91sam9g20ek_nandflash_config
make

