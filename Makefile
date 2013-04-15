ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE = arm-linux-
CC		= $(CROSS_COMPILE)gcc
STRIP	= $(CROSS_COMPILE)strip
XECHO = echo
CFLAGS += -g -O2

DIR_APPS = ${ROOTDIR}/apps
DIR_INC	 = ${ROOTDIR}/apps/include
DIR_UBOOT = ${ROOTDIR}/u-boot-1.3.4
DIR_KERNEL = ${ROOTDIR}/linux-3.4.6

.PHONY: all clean distclean uboot kernel apps apps_clean wr1004sjl wec9720ek_c22 wec9720ek_s220 wec9720ek_xd25

all:
	@echo "please choose platform device by make:"
	@echo "	make wec9720ek_c22"
	@echo "	make wec9720ek_s220"
	@echo "	make wec9720ek_xd25"
	@echo "	make wr1004sjl"
	@echo "make execution terminated !"

wec9720ek_c22:wec9720ek_c22_config uboot kernel apps
wec9720ek_s220:wec9720ek_s220_config uboot kernel apps
wec9720ek_xd25:wec9720ek_xd25_config uboot kernel apps
wr1004sjl:wr1004sjl_config uboot kernel apps

wr1004sjl_config:
	echo "#define CFG_USE_PLATFORM_WR1004SJL 1" >$(DIR_INC)/config.h
	@echo "... use platform device wr1004sjl"

wec9720ek_c22_config:
	echo "#define CFG_USE_PLATFORM_WEC9720EK_C22 1" >$(DIR_INC)/config.h
	@echo "... use platform device wec9720ek c22"

wec9720ek_s220_config:
	echo "#define CFG_USE_PLATFORM_WEC9720EK_S220 1" >$(DIR_INC)/config.h
	@echo "... use platform device wec9720ek s220"

wec9720ek_xd25_config:
	echo "#define CFG_USE_PLATFORM_WEC9720EK_XD25 1" >$(DIR_INC)/config.h
	@echo "... use platform device wec9720ek xd25"

apps:
	make -C $(DIR_APPS)

uboot:
	make -C $(DIR_UBOOT) clean
	make -C $(DIR_UBOOT) distclean
	@rm -f $(ROOTDIR)/images/u-boot.bin
	@mkdir -p $(ROOTDIR)/images
	make -C $(DIR_UBOOT) at91sam9g20ek_config
	make -C $(DIR_UBOOT) at91sam9g20ek_nandflash_config
	make -C $(DIR_UBOOT)
	@cp $(DIR_UBOOT)/u-boot.bin  $(ROOTDIR)/images/

kernel:
	make -C $(DIR_KERNEL) distclean
	@rm -f $(ROOTDIR)/images/uImage
	@mkdir -p $(ROOTDIR)/images
	@cp $(DIR_KERNEL)/arch/arm/configs/wec9720ek_defconfig $(DIR_KERNEL)/
	make -C $(DIR_KERNEL) wec9720ek_defconfig
	make -C $(DIR_KERNEL) uImage
	@cp $(DIR_KERNEL)/arch/arm/boot/uImage  $(ROOTDIR)/images/

apps_clean:
	make -C $(DIR_APPS) clean
	
clean:
	make -C $(DIR_UBOOT) clean
	@rm -f $(ROOTDIR)/images/u-boot.bin
	make -C $(DIR_KERNEL) clean
	@rm -f $(ROOTDIR)/images/uImage
	make -C $(DIR_APPS) clean

distclean:
	make -C $(DIR_UBOOT) clean
	make -C $(DIR_UBOOT) distclean
	make -C $(DIR_KERNEL) clean
	make -C $(DIR_KERNEL) distclean
	make -C $(DIR_APPS) clean
	make -C $(DIR_APPS) distclean
	@rm -rf $(ROOTDIR)/images/
	@rm -f $(DIR_INC)/config.h
