# maelstrom_firmware

This repository holds the code for the embedded systems and other tools running on Maelstrom. There are two primary parts to this: the acoustics code, and the the coprocessor code/console. A brief overview of each of the systems is below. For detailed install instructions, see the acoustics and [copro](/Copro/README.md) README files. For more information about the hardware of these devices, see github.com/osu-uwrt/uwrt_electronics

## Coprocessor
The coprocessor has several primary tasks:

- read sensor values
- control thrusters
- communicate with the main computer

It does this with an onboard microcontroller, an STM32F405RGT6. This microcontroller board is loosely based on the pyboard from the micropython project. As such, the microcontroller is programmed using micropython. It is programmed using a USB connection, and talks with the main computer over the local network via websocket. Additionally, there is an operator console web app that shows current status of the coprocessor and allows the device to be controlled without having to open up the vehicle.

## Acoustics
The acoustics system is designed to be able to determine the direction and distance of an acoustic ping (between 20kHz - 40kHz frequency). This is used to navigate towards a pinger in the robot. Thus, the device has a lot of signal processing and alignment code
