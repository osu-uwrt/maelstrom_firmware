# riptide_firmware README

This holds two main things for right now, all of the firmware running on the Arduinos in *Riptide* and all of the firmware running on the main co-processing board on *Riptide*

## Arduino Specific

-You need to get the ros libs from team Github

-You need to get the sensor libs from Blue Robotics Docs

## STM32 Specific
### Makefile specific:
To compile you need to edit the Makefile and change the bin path in line 127 to point to wherever:

`STM32/gcc-arm-none-eabi-6-2017-q2-update/bin`

is stored. 
