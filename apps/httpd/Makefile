ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

TARGET=arm-linux
CROSS_COMPILE=arm-linux-
STRIP	= $(CROSS_COMPILE)strip
AR = $(CROSS_COMPILE)ar -r
CC = $(CROSS_COMPILE)gcc

CFLAGS +=-g -O2


# Web language select [ZH|EN]
HTTPD_GUI_LANGUAGE = ZH
#HTTPD_GUI_LANGUAGE = EN

ROOTFSDIR	=	${ROOTDIR}/apps/rootfs
SRCDIR 		=	${ROOTDIR}/apps/httpd/src

#TARGETDIR 	=	$(ROOTFSDIR)/usr/mnt
TARGETDIR 	=	$(ROOTDIR)/apps/flashfs
BIN_PATH 	=	$(TARGETDIR)/app
WEB_PATH 	=	$(TARGETDIR)/html


#HTMLDIR 	=	${ROOTDIR}/apps/httpd/html
#HTMLDIR_ZH 	=	${ROOTDIR}/apps/httpd/WEB_ZH
#HTMLDIR_EN 	=	${ROOTDIR}/apps/httpd/WEB_EN

ifeq ($(HTTPD_GUI_LANGUAGE), ZH)
	echo "HTTPD_GUI_LANGUAGE = ZH"
	HTML_SRC_DIR = ${ROOTDIR}/apps/httpd/WEB_ZH
else ifeq ($(HTTPD_GUI_LANGUAGE), EN)
	echo "HTTPD_GUI_LANGUAGE = EN"
	HTML_SRC_DIR = ${ROOTDIR}/apps/httpd/WEB_EN
else
	echo "HTTPD_GUI_LANGUAGE not defined, force ZH by default"
	HTML_SRC_DIR = ${ROOTDIR}/apps/httpd/WEB_ZH
endif

export HTTPD_GUI_LANGUAGE HTML_SRC_DIR

.PHONY: all clean distclean


all:config build install

config:
ifeq ($(HTTPD_GUI_LANGUAGE), ZH)
	echo "HTTPD_GUI_LANGUAGE = ZH"
else ifeq ($(HTTPD_GUI_LANGUAGE), EN)
	echo "HTTPD_GUI_LANGUAGE = EN"
else
	echo "HTTPD_GUI_LANGUAGE not defined, force ZH by default"
endif
	
build:
	make -C $(SRCDIR)
	$(STRIP) $(SRCDIR)/httpd

install:
	mkdir -p $(BIN_PATH)
	cp $(SRCDIR)/httpd $(BIN_PATH)/
	mkdir -p $(WEB_PATH)
	cp -r $(HTML_SRC_DIR)/html/* $(WEB_PATH)/
	
clean:
	make clean -C $(SRCDIR)
	rm -rf $(WEB_PATH)
	rm -f $(BIN_PATH)/httpd

distclean:
	rm -rf $(WEB_PATH)
	rm -f $(BIN_PATH)/httpd
