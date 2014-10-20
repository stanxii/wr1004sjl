####################################################################################################################
@readme
####################################################################################################################
@Design Constraints:
	The first 5 bloacks of the nand flash can not be bad!
####################################################################################################################
@How to compile apps:
	1) cd apps;
	2) make;
	if you want to debug from nfs, do follow step:
	3) make nfs;
	clean src compile targets:
	4) make clean;
	distclean all src compile targets and rootfs target:
	5) make distclean;

@How to build kernel:
	1) cd linux-3.4.6;
	2) make distclean;
	3) cp arch/arm/configs/wec9720ek_defconfig ./
	4) make wec9720ek_defconfig
	5) make uImage

@How to build u-boot:
	1) cd u-boot-1.3.4;
	2) make clean;
	3) make distclean;
	4) make at91sam9g20ek_config
	5) make at91sam9g20ek_nandflash_config
	6) make
##############################################################################################
@NEW Makefile Support:

	/*How to compile and clean all project*/
	1) make {wec9720ek_c22|wec9720ek_s220|wec9720ek_xd25|wr1004sjl};
	2) make clean;
	3) make distclean;

	/*How to build and clean apps only*/
	1) make {wec9720ek_c22_config|wec9720ek_s220_config|wec9720ek_xd25_config|wr1004sjl_config};
	2) make apps;
	3) make apps_clean;
	4) make distclean;

	/*How to build and clean kernel only*/
	1) make kernel;
	2) make clean;
	3) make distclean;

	/*How to build and clean u-boot only*/
	1) make uboot;
	2) make clean;
	3) make distclean;
####################################################################################################################
@nand flash mtd map before v1.2.5.x:

	|+++++++++++++++++++++++|
	|  usrfs     (6M)       |
	|+++++++++++++++++++++++|-->offset:[0x800000]
	|  rootfs    (0x560000) |
	|+++++++++++++++++++++++|-->offset:[0x2A0000]
	|  kernel    (2M)       |
	|+++++++++++++++++++++++|-->offset:[0xA0000]
	|  nvm       (0x40000)  |
	|+++++++++++++++++++++++|-->offset:[0x60000]
	|  uboot     (0x40000)  |
	|+++++++++++++++++++++++|-->offset:[0x20000]
	|  bootstrip (0x20000)  |
	|+++++++++++++++++++++++|

##############################################################################################
@how to flash images from console:

	/*how to flash uboot*/
	1) nand erase 0x20000 0x40000;tftp 0x21100000 u-boot.bin
	2) nand write.jffs2 0x21100000 0x20000 0x40000
	
	/*how to flash nvm*/
	1) nand erase 0x60000 0x40000;tftp 0x21100000 env.bin
	2) nand write.jffs2 0x21100000 0x60000 0x40000
	
	/*how to flash kernel*/
	1) nand erase 0xA0000 0x200000;tftp 0x22200000 uImage
	2) nand write.jffs2 0x22200000 0xA0000 0x200000

	/*how to flash rootfs*/
	1) nand erase 0x2A0000 0x560000;tftp 0x21100000 rootfs.bin
	2) nand write.jffs2 0x21100000 0x2A0000 0x560000

	/*how to flash userfs*/
	1) nand erase 0x800000 0x600000;tftp 0x21100000 userfs.bin
	2) nand write.jffs2 0x21100000 0x800000 0x600000

##############################################################################################
@how to set u-boot env in nand flash

	/*start system from nand flash*/
	/* note: if watchdog env is not set, wdt will disabled by default */
	1) setenv ethaddr 30:71:B2:00:00:00
	2) setenv filesize 1B4142
	3) setenv fileaddr 20000000
	4) setenv ipaddr 192.168.223.1
	5) setenv serverip 192.168.223.254
	6) setenv bootargs root=/dev/mtdblock4 rootfstype=cramfs console=ttyS0,115200 init=/sbin/init noinitrd mem=64M
	7) setenv bootcmd nand read.jffs2 0x22200000 0xA0000 0x200000 \;bootm 0x22200000
	8) setenv devmodel 22
	9) setenv watchdog on	
	
	/*start from tftp and nfs for debug*/
	1) setenv ethaddr 30:71:B2:00:00:00
	2) setenv filesize 1B4142
	3) setenv fileaddr 20000000
	4) setenv ipaddr 192.168.1.150
	5) setenv serverip 192.168.1.249
	6) setenv bootargs mem=64M console=ttyS0 115200 root=/dev/nfs rw nfsroot=192.168.1.249:/home/nfs ip=192.168.1.150:192.168.1.249:192.168.1.1:255.255.255.0::eth0:off
	7) setenv bootcmd tftp 22200000 uImage \;bootm 0x22200000

####################################################################################################################
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
####################################################################################################################
@Nand Flash MTD MAP(v1.3.8.0):

	|+++++++++++++++++++++++|
	|  reserve              |
	|+++++++++++++++++++++++|-->offset:[0x00EA0000]
	|  rootfs    (0xC00000) |
	|+++++++++++++++++++++++|-->offset:[0x002A0000]
	|  kernel    (2M)       |
	|+++++++++++++++++++++++|-->offset:[0x000A0000]
	|  nvm       (0x40000)  |
	|+++++++++++++++++++++++|-->offset:[0x00060000]
	|  uboot     (0x40000)  |
	|+++++++++++++++++++++++|-->offset:[0x00020000]
	|  bootstrip (0x20000)  |
	|+++++++++++++++++++++++|

##############################################################################################
@how to flash images from console:

	/*how to flash uboot*/
	1) nand erase 0x20000 0x40000;tftp 0x21100000 u-boot.bin
	2) nand write.jffs2 0x21100000 0x20000 0x40000
	
	/*how to flash nvm*/
	1) nand erase 0x60000 0x40000;tftp 0x21100000 env.bin
	2) nand write.jffs2 0x21100000 0x60000 0x40000
	
	/*how to flash kernel*/
	1) nand erase 0xA0000 0x200000;tftp 0x22200000 uImage
	2) nand write.jffs2 0x22200000 0xA0000 0x200000

	/*how to flash rootfs*/
	1) nand erase 0x2A0000 0xC00000;tftp 0x21100000 rootfs.bin
	2) nand write.jffs2 0x21100000 0x2A0000 0x800000

##############################################################################################
@how to set u-boot env in nand flash

	/*start from nand flash*/
	/* note: if watchdog env is not set, wdt will disabled by default */
	1) setenv ethaddr 30:71:B2:00:00:00
	2) setenv filesize 1B4142
	3) setenv fileaddr 20000000
	4) setenv ipaddr 192.168.223.1
	5) setenv serverip 192.168.223.254
	6) setenv bootargs ubi.mtd=4 root=ubi0:rootfs rootfstype=ubifs console=ttyS0,115200 init=/sbin/init noinitrd mem=64M
	7) setenv bootcmd nand read.jffs2 0x22200000 0xA0000 0x200000 \;bootm 0x22200000
	8) setenv devmodel 22
	9) setenv watchdog on
	
####################################################################################################################
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
####################################################################################################################
@NAND Flash MTD MAP(v1.3.6.x or higher):

|+++++++++++++++++++++++|
|  reserve              |
|+++++++++++++++++++++++|-->offset:[0x01EA0000]
|  userfs    (0x6M)     |
|  rootfs    (0x10M)    |
|  safefs    (0x12M)    |
| (ubifs size: 28MiB)   |
|+++++++++++++++++++++++|-->offset:[0x002A0000]
|  kernel    (2M)       |
|+++++++++++++++++++++++|-->offset:[0x000A0000]
|  nvm       (0x40000)  |
|+++++++++++++++++++++++|-->offset:[0x00060000]
|  uboot     (0x40000)  |
|+++++++++++++++++++++++|-->offset:[0x00020000]
|  bootstrip (0x20000)  |
|+++++++++++++++++++++++|

##############################################################################################
@how to flash images from console:

	/*how to flash uboot*/
	1) nand erase 0x20000 0x40000;tftp 0x21100000 u-boot.bin
	2) nand write.jffs2 0x21100000 0x20000 0x40000
	
	/*how to flash nvm*/
	1) nand erase 0x60000 0x40000;tftp 0x21100000 env.bin
	2) nand write.jffs2 0x21100000 0x60000 0x40000
	
	/*how to flash kernel*/
	1) nand erase 0xA0000 0x200000;tftp 0x22200000 uImage
	2) nand write.jffs2 0x22200000 0xA0000 0x200000

	/*how to flash ubifs*/
	1) nand erase 0x2A0000 0x1C00000;tftp 0x20000000 ubifs.bin
	2) nand write.jffs2 0x20000000 0x2A0000 0x1C00000

##############################################################################################
@how to set u-boot env in nand flash

	/*start from nand flash*/
	/* note: if watchdog env is not set, wdt will disabled by default */
	1) setenv ethaddr 30:71:B2:00:00:00
	2) setenv filesize 1B4142
	3) setenv fileaddr 20000000
	4) setenv ipaddr 192.168.223.1
	5) setenv serverip 192.168.223.254
	6) setenv bootargs ubi.mtd=4 root=ubi0:rootfs ro rootfstype=ubifs console=ttyS0,115200 init=/sbin/init noinitrd mem=64M
	7) setenv bootcmd nand read.jffs2 0x22200000 0xA0000 0x200000 \;bootm 0x22200000
	8) setenv devmodel 22
	9) setenv watchdog on
	
	/*how to start safemode*/
	1) setenv bootargs ubi.mtd=4 root=ubi0:safefs ro rootfstype=ubifs console=ttyS0,115200 init=/sbin/init noinitrd mem=64M
####################################################################################################################
