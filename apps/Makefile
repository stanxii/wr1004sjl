ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE = arm-linux-

CC		= $(CROSS_COMPILE)gcc
STRIP	= $(CROSS_COMPILE)strip

CFLAGS += -g -O2

USERFSDIR = ${ROOTDIR}/apps/flashfs
ROOTFSDIR = ${ROOTDIR}/apps/rootfs
ROOTFSIMG = rootfs.cramfs
USERFSNAME = userfs.ubifs
USERFSIMG = userfs.img
NFS_DIR = /home/nfs
TFTPROOT = /tftpboot

apps = sqlite
apps += busybox
apps += mtd-utils
apps += boardapi
apps += nvm
apps += dbs
apps += snmp
apps += alarm
apps += cli
apps += cmm
apps += httpd
apps += mmead
apps += register
apps += template
apps += sysMonitor
apps += tests
apps += mkimges

.PHONY: all clean distclean nfs

all:app

app:
	@for i in $(apps) ; do \
	echo -e "\n======== Making $$i ========";\
	make -C $$i || exit $?; \
	echo -e "======== $$i done ========\n"; \
	done

tftp:
	rm -f $(TFTPROOT)/$(ROOTFSIMG)
	rm -f $(TFTPROOT)/$(USERFSIMG)
	cp mkimges/$(ROOTFSIMG) $(TFTPROOT)
	cp mkimges/$(USERFSIMG) $(TFTPROOT)

clean:
	-for i in $(apps) ; do [ ! -d $$i ] || make -C $$i clean; \
	done

nfs:
	sudo rm -rf $(NFS_DIR)/*
	sudo cp -a $(ROOTFSDIR)/* $(NFS_DIR)/

distclean:	
	rm -f $(TFTPROOT)/$(ROOTFSIMG)
	rm -f $(TFTPROOT)/$(USERFSIMG)
	-for i in $(apps) ; do [ ! -d $$i ] || make -C $$i distclean; \
	done

