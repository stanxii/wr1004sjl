# TODO: set this appropriately for your local toolchain
ifndef ERASE_FCT
ERASE_FCT=rm -f
endif
ifndef CROSS_COMPILE
CROSS_COMPILE=arm-elf-
endif

TOOLCHAIN=gcc

BOOTSTRAP_PATH=../../..

# NandFlashBoot Configuration for AT91SAM9XEEK

# Target name (case sensitive!!!)
TARGET=AT91SAM9XE
# Board name (case sensitive!!!)
BOARD=at91sam9xeek
# Link Address and Top_of_Memory
LINK_ADDR=0x200000
MEMORY_BASE=0x300000
TOP_OF_MEMORY=0x301000 # suitable for all SAM9XE chips 128/256/512/..
# Name of current directory
PROJECT=nandflash

ifndef BOOT_NAME
BOOT_NAME=$(PROJECT)_$(BOARD)
endif

INCL=./$(BOOTSTRAP_PATH)/board/$(BOARD)/$(PROJECT)

ifeq ($(TOOLCHAIN), gcc)

AS=$(CROSS_COMPILE)gcc
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
NM= $(CROSS_COMPILE)nm
SIZE=$(CROSS_COMPILE)size
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
CCFLAGS=-g -mcpu=arm9 -Os -Wall -D$(TARGET) -I$(INCL)
ASFLAGS=-g -mcpu=arm9 -c -Os -Wall -D$(TARGET) -I$(INCL) -DTOP_OF_MEM=$(TOP_OF_MEMORY)

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to map file
LDFLAGS+=-nostartfiles -nostdlib -Wl,-Map=$(BOOT_NAME).map,--cref
LDFLAGS+=-T $(BOOTSTRAP_PATH)/elf32-littlearm.lds -Ttext $(LINK_ADDR)
ifdef MEMORY_BASE
LDFLAGS+=-Tdata $(MEMORY_BASE)
endif
OBJS=crt0_gnu.o

endif

OBJS+=\
	$(BOARD).o \
	main.o \
	gpio.o \
	pmc.o \
	debug.o \
	sdramc.o \
	nandflash.o \
	_udivsi3.o \
	_umodsi3.o \
	div0.o \
	udiv.o \
	string.o

rebuild: clean all

all: 	$(BOOT_NAME)

ifeq ($(TOOLCHAIN), gcc)
$(BOOT_NAME): $(OBJS)
	$(LD) $(LDFLAGS) -n -o $(BOOT_NAME).elf $(OBJS)
	$(OBJCOPY) --strip-debug --strip-unneeded $(BOOT_NAME).elf -O binary $(BOOT_NAME).bin
endif


$(BOARD).o: $(BOOTSTRAP_PATH)/board/$(BOARD)/$(BOARD).c
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/board/$(BOARD)/$(BOARD).c -o $(BOARD).o

main.o: $(BOOTSTRAP_PATH)/main.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/main.c -o main.o

gpio.o: $(BOOTSTRAP_PATH)/driver/gpio.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/driver/gpio.c -o gpio.o

pmc.o:  $(BOOTSTRAP_PATH)/driver/pmc.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/driver/pmc.c -o pmc.o

debug.o: $(BOOTSTRAP_PATH)/driver/debug.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/driver/debug.c -o debug.o

sdramc.o:  $(BOOTSTRAP_PATH)/driver/sdramc.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/driver/sdramc.c -o sdramc.o

dataflash.o:  $(BOOTSTRAP_PATH)/driver/dataflash.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/driver/dataflash.c -o dataflash.o

nandflash.o:  $(BOOTSTRAP_PATH)/driver/nandflash.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/driver/nandflash.c -o nandflash.o

crt0_gnu.o:  $(BOOTSTRAP_PATH)/crt0_gnu.S
	$(AS) $(ASFLAGS) $(BOOTSTRAP_PATH)/crt0_gnu.S -o crt0_gnu.o

div0.o:  $(BOOTSTRAP_PATH)/lib/div0.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/lib/div0.c -o div0.o

string.o:  $(BOOTSTRAP_PATH)/lib/string.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/lib/string.c -o string.o

udiv.o:  $(BOOTSTRAP_PATH)/lib/udiv.c 
	$(CC) -c $(CCFLAGS) $(BOOTSTRAP_PATH)/lib/udiv.c -o udiv.o

_udivsi3.o:  $(BOOTSTRAP_PATH)/lib/_udivsi3.S
	$(AS) $(ASFLAGS) $(BOOTSTRAP_PATH)/lib/_udivsi3.S -o _udivsi3.o

_umodsi3.o:  $(BOOTSTRAP_PATH)/lib/_umodsi3.S
	$(AS) $(ASFLAGS) $(BOOTSTRAP_PATH)/lib/_umodsi3.S -o _umodsi3.o
	
clean:
	$(ERASE_FCT) *.o *.bin *.elf *.map
