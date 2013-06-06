ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
#USERFSDIR = ${ROOTDIR}/apps/flashfs

WEBTARNAME = wec9720ek.tar.bz2
WEBIMGNAME = wec9720ek-s.web
ENVIMGNAME = env.bin
IMAGEDIR = $(ROOTDIR)/images

UBIIMGNAME = rootfs.ubifs
#USRFSIMGNAME = userfs.bin
ROOTFSIMGNAME = rootfs.bin

.PHONY: all clean distclean

all:build mkenvimg mkwebimg mkrootfs install

build:	
	make -C mkweb
	make -C mkenv

mkenvimg:
	rm -f $(ENVIMGNAME)
	mkenv/mkenv

mkwebimg:
	rm -f $(WEBIMGNAME)
	cp -a $(ROOTFSDIR)/usr/mnt/app ./
	cp -a $(ROOTFSDIR)/usr/mnt/html ./
	tar jcvf $(WEBTARNAME) app html
	rm -rf app html
	mkweb/mkweb
	rm -f $(WEBTARNAME)

#mkusrfs:
#	rm -f $(USRFSIMGNAME)
#	$(ROOTDIR)/tools/mkfs.ubifs -r $(USERFSDIR) -m 2048 -e 129024 -c 48  -o $(UBIIMGNAME)
#	$(ROOTDIR)/tools/ubinize -o $(USRFSIMGNAME) -m 2048 -p 128KiB -s 512 ./ubinize.cfg
#	rm -f $(UBIIMGNAME)

mkrootfs:
	rm -f $(ROOTFSIMGNAME)
	mkdir -p $(ROOTFSDIR)/dev
	rm -f $(ROOTFSDIR)/dev/*
	$(ROOTDIR)/tools/mkfs.ubifs -r $(ROOTFSDIR) -m 2048 -e 129024 -c 96 -D device_table  -o $(UBIIMGNAME)
	$(ROOTDIR)/tools/ubinize -o $(ROOTFSIMGNAME) -m 2048 -p 131072 -s 512 ./ubinize.cfg

install:
	mkdir -p $(IMAGEDIR)
	rm -f $(IMAGEDIR)/$(WEBIMGNAME) $(IMAGEDIR)/$(ROOTFSIMGNAME)
	rm -f $(IMAGEDIR)/$(ENVIMGNAME)
	cp $(ENVIMGNAME) $(IMAGEDIR)
	cp $(WEBIMGNAME) $(IMAGEDIR)
	cp $(ROOTFSIMGNAME) $(IMAGEDIR)
	
clean:	
	make clean -C mkweb
	make clean -C mkenv

distclean:
	rm -f $(ENVIMGNAME) $(WEBIMGNAME) $(ROOTFSIMGNAME) $(UBIIMGNAME)
	rm -f $(IMAGEDIR)/$(ROOTFSIMGNAME) $(IMAGEDIR)/$(WEBIMGNAME)
	rm -f $(IMAGEDIR)/$(ENVIMGNAME)
	rm -rf $(ROOTFSDIR)/dev