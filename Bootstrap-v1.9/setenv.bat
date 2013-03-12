@echo OFF
echo ##########################################################################
echo _
echo Setting up build environment for AT91BootStrap :
echo _
set CROSS_COMPILE=arm-none-eabi-
set ERASE_FCT=del /F

echo CROSS_COMPILE=arm-none-eabi-
echo ERASE_FCT=del /F
echo _
echo ##########################################################################
echo _
echo Usage :
echo change to the directory you want the AT91BootStrap to be built for
echo ie : cd board\at91sam9260ek\dataflash
echo _
echo Type the 'make' command.
echo You get a .bin file ready to be sent to your board with SAM-BA
