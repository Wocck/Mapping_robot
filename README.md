# PZSP2 2024L Robot Project


# The goal of project
The goal of this project is to develop a system that enables automatic room mapping using a mobile robot and dedicated visualization software on a portable computer. The project focuses on extending the functionality of the existing robot by adding a Lidar to obtain more accurate information about the distance to obstacles. The entire system will communicate via a bluetooth module to enable data collection from the robot and visualization.


### Project Members
* Wojciech Sekuła
* Bartosz Pawlak
* Michał Lachowicz
* Kacper Tomczykowski

# Building software

* Download RIOT OS code - `git clone https://github.com/RIOT-OS/RIOT`
* Download robot software
* Update in robot software `Makefile` RIOT OS directory - update `RIOTBASE` variable
* Frome the robot software main directory execue command `make`

# Flashing nucleo boards without additional programmer

Most Nucleo boards contains ST-LINK programmer. To program board you could 
simple copy hex file into flash.

In RIOT to do this automatically use command:

`make BOARD=.... PROGRAMMER=cpy2remed flash`

# Firmware flashing procedure

* Turn off robot - use switch between battery-pack and nucleo board
* Move jumper JP5 from external power to power from ST-Link USB connection
 (JP5 to U5V - pins 1 and 2)
* Connect nucleo board to computer using USB
* Connect device to virtual machine - use Menu \ Devices \ USB \ STMicroelectronics STM32 STLink
* Wait for removable media
* Use command `make flash` - full version of command `make BOARD=nucleo-f334r8 PROGRAMMER=cpy2remed flash`
* Disconect USB cable
* Move jumper JP5 from power from ST-Link USB connection to external power
 (JP5 to E5V - pins 2 and 3)
* Turn external power on - use switch between battery-pack and nucleo board



