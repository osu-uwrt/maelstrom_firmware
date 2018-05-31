# riptide_firmware README

This holds three main things, all of the firmware running on the Arduinos in *Riptide, all of the firmware running on the main co-processing board on *Riptide*, and the configuration fro the acoustics FPGA

## Arduino Specific

-You need to get the ros libs from team Github

-You need to get the sensor libs from Blue Robotics Docs

## STM32 Specific
### Makefile specific:
To compile you need to edit the Makefile and change the bin path in line 127 to point to wherever:

`STM32/gcc-arm-none-eabi-6-2017-q2-update/bin`

is stored. 

## ZEM5305
To compile, open in quartus, compile, and make .rbf programming file.

This configuration interacts with the ADCs on the acoustics board, loads in .5 ms of data from each hydrophone, and sends to computer.
