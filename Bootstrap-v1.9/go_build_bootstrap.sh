#!/bin/bash

AT91BOOTSTRAP_BOARD="at91sam9260ek at91sam9261ek at91sam9263ek at91sam9xeek at91sam9rlek at91sam9g20ek"
MAKE=/usr/local/bin/make-3.80
X_COMPILE=/opt/codesourcery/arm-2007q1/bin/arm-none-linux-gnueabi-

#########################################################
mkdir -p build
rm -rf build/*
> build.log

for board in ${AT91BOOTSTRAP_BOARD}; do
	echo -n "building for ${board} :"
	for media in dataflash nandflash; do
		echo -n " ${media}"
		${MAKE} -C board/${board}/${media} CROSS_COMPILE=${X_COMPILE} rebuild >> build.log 2>&1
		if [ "$?" -ne 0 ]; then
			echo " error."
			exit 1
		fi
		cp board/${board}/${media}/*.bin build/
	done
	echo "."
done
