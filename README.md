# riptide_firmware

This holds two main things for right now, all of the firmware running on the Arduinos in *Riptide* and all of the firmware running on the main co-processing board on *Riptid3*. The co-processing board is an STM32F4 based MCU that was custom made to work with the 2018 Robosub vehicle.   

## STM32 Spinup
All STM projects are built initially with STM32CubeMX software.
### Makefile spinup:

In the STM32CubeMX software, in project settings, set the IDE to **Makefile**. These projects are geared towards using MacOS or a specific flavor of Linux, Ubuntu 16.04 in our case. To compile you need to edit the Makefile and change the bin path in line 127 to point to wherever:

`gcc-arm-none-eabi-6-2017-q2-update/bin`

is stored. Type the command `make -f Makefile`, in the project directory.  The -f forces the Makefile to run. 

To be able to program, you need either a windows environment with **STLink Utility** installed or with MacOS or ubuntu follow this [STLink Utility](https://github.com/texane/stlink) to get a command line version.

#### Using STLink Command Line 
With the project compiled and built, go to the directory where the **STLink Utility** was installed. Plug in the STLink programmer and run the following command:

`./st-flash --format ihex write ~/path/to/project/build/project.hex`

And you should now have a programmed STM32 chip. The **st-flash** can take many formats so you need to specify `--formate ihex` and the `write` command writes the actual hexfile to the chip.

## Arduino Spinup

-You need to get the ros libs (messages) from team Github

-You need to get the sensor libs from Blue Robotics Docs

 
