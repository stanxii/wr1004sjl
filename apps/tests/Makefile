ifeq "$(ROOTDIR)" "" 
export ROOTDIR=$(shell while true; do if [ -f .source ]; then pwd;exit; else cd ..;fi;done;)
endif

CROSS_COMPILE = arm-linux-
CC		= $(CROSS_COMPILE)gcc
STRIP	= $(CROSS_COMPILE)strip

DSDTROOT = $(ROOTDIR)/apps/DSDT_3.0
DSDTSWDIR = $(DSDTROOT)/switch
DBSDIR = $(ROOTDIR)/apps/dbs

IFLAGS = -I../include -I../boardapi
DBS_INC = -I$(DBSDIR)/include
DSDT_INC = -I$(DSDTSWDIR)/Include \
           -I$(DSDTSWDIR)/sample/Include \
	-I$(DSDTSWDIR)/Include/h/msApi
	
LDFLAGS += -L../dbs/client/lib
DBS_LIB = -ldbs
BOARDAPI_LD_FLAGS = -L../boardapi -lboardapi

QD_LIB = $(DSDTSWDIR)/Library/qdDrv.o

CFLAGS += -g -O2 -D__KERNEL__

ROOTFSDIR = ${ROOTDIR}/apps/rootfs
BIN_PATH = $(ROOTFSDIR)/usr/mnt/app

#FLASHFSDIR = ${ROOTDIR}/apps/flashfs
#BIN_PATH = $(FLASHFSDIR)/app

.PHONY: all clean distclean

all:build install

build:
	$(CC) $(IFLAGS) -c smi.c $(CFLAGS)
	$(CC) $(IFLAGS) $(DBS_INC) -c dbsTester.c $(CFLAGS)
	$(CC) $(IFLAGS) $(DBS_INC) -c mmeTester.c $(CFLAGS)
	$(CC) $(IFLAGS) $(DBS_INC) -c tmTester.c $(CFLAGS)
	$(CC) $(IFLAGS) $(DBS_INC) $(DSDT_INC) -c dsdTester.c $(CFLAGS)
	$(CC) smi.o -o smi
	$(CC) dbsTester.o -o dbsTester $(LDFLAGS) $(DBS_LIB)
	$(CC) mmeTester.o -o mmeTester $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(CC) tmTester.o -o tmTester $(LDFLAGS) $(DBS_LIB) $(BOARDAPI_LD_FLAGS)
	$(CC) dsdTester.o $(QD_LIB) -o dsdTester $(LDFLAGS) $(DBS_LIB)

install:
	$(STRIP) smi dbsTester dsdTester mmeTester tmTester
	cp -f smi $(BIN_PATH)/
	cp -f mmeTester $(BIN_PATH)/
	cp -f dbsTester $(BIN_PATH)/
	cp -f dsdTester $(BIN_PATH)/
	cp -f tmTester $(BIN_PATH)/

clean:
	rm -f *.o smi dbsTester dsdTester mmeTester tmTester

distclean:
	rm -f $(BIN_PATH)/smi
	rm -f $(BIN_PATH)/mmeTester
	rm -f $(BIN_PATH)/dbsTester
	rm -f $(BIN_PATH)/dsdTester
	rm -f $(BIN_PATH)/tmTester
