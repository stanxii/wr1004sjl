ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
USERFSDIR = ${ROOTDIR}/apps/flashfs

WEBTARNAME = wec9720ek.tar.bz2
WEBIMGNAME = wec9720ek-s.web
ENVIMGNAME = env.bin
IMAGEDIR = $(ROOTDIR)/images

UBIIMGNAME = userfs.ubifs
USRFSIMGNAME = userfs.bin

ROOTFSIMGNAME = rootfs.bin

.PHONY: all clean distclean

all:build mkenvimg mkwebimg mkusrfs mkrootfs install

build:	
	make -C mkweb
	make -C mkenv

mkenvimg:
	rm -f $(ENVIMGNAME)
	mkenv/mkenv

mkwebimg:
	rm -f $(WEBIMGNAME)
	cp -a ../flashfs/app ./
	cp -a ../flashfs/html ./
	tar jcvf $(WEBTARNAME) app html
	rm -rf app html
	mkweb/mkweb
	rm -f $(WEBTARNAME)

mkusrfs:
	rm -f $(USRFSIMGNAME)
	$(ROOTDIR)/tools/mkfs.ubifs -r $(USERFSDIR) -m 2048 -e 129024 -c 48  -o $(UBIIMGNAME)
	$(ROOTDIR)/tools/ubinize -o $(USRFSIMGNAME) -m 2048 -p 128KiB -s 512 ./ubinize.cfg
	rm -f $(UBIIMGNAME)

mkrootfs:
	rm -f $(ROOTFSIMGNAME)
	mkdir -p $(ROOTFSDIR)/dev
	rm -f $(ROOTFSDIR)/dev/*
	sudo mknod -m 600 $(ROOTFSDIR)/dev/console c 5 1
	sudo mknod -m 666 $(ROOTFSDIR)/dev/null c 1 3
	mkfs.cramfs $(ROOTFSDIR) $(ROOTFSIMGNAME)

install:
	mkdir -p $(IMAGEDIR)
	rm -f $(IMAGEDIR)/$(WEBIMGNAME) $(IMAGEDIR)/$(USRFSIMGNAME) $(IMAGEDIR)/$(ROOTFSIMGNAME)
	rm -f $(IMAGEDIR)/$(ENVIMGNAME)
	cp $(ENVIMGNAME) $(IMAGEDIR)
	cp $(WEBIMGNAME) $(IMAGEDIR)
	cp $(USRFSIMGNAME) $(IMAGEDIR)
	cp $(ROOTFSIMGNAME) $(IMAGEDIR)
	
clean:	
	make clean -C mkweb
	make clean -C mkenv

distclean:
	rm -f $(ENVIMGNAME) $(WEBIMGNAME) $(USRFSIMGNAME) $(ROOTFSIMGNAME)
	rm -f $(IMAGEDIR)/$(ROOTFSIMGNAME) $(IMAGEDIR)/$(USRFSIMGNAME) $(IMAGEDIR)/$(WEBIMGNAME)
	rm -f $(IMAGEDIR)/$(ENVIMGNAME)
	rm -rf $(ROOTFSDIR)/dev
	rm -rf $(USERFSDIR)