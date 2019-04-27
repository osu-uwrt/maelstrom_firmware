# Coprocessor firmware
The coprocessor is programmed over USB using micropython. Micropython is an implementation of python specifically stripped down enough to fit on embedded devices. Below are instructions on getting up and running with the micropython firmware for our device.

## Compiling and flashing micropython
**1. Clone the micropython repo**
```
git clone https://github.com/micropython/micropython.git
cd micropython/ports/stm32
git submodule update --init
```
**2. Copy the board configuration files**
```
cp ~/riptide_firmware/UWRT_COPRO/ ~/micropython/ports/stm32/boards
```
**3. Build the firmware**
```
make BOARD=UWRT_COPRO MICROPY_PY_WIZNET5K=5500
```
this step is necessary to be able to include the modules needed for ethernet on the device
**4. Flash to the device** - hook up the ST Link hardware debugger to the coprocessor board. TODO: insert pictures below to show how the ST Link is connected
  i. Install the texane stlink tools from github.com/texane/stlink. Either install from a repo, download a release, or build it yourself. Make sure that the st-flash utility is in the system path
  ii. Flash the firmware
  ```
  cd build-UWRT_COPRO
  st-flash --format ihex write firmware.hex
  ```
  iii. The board should now be flashed. Test the installation by plugging in the USB cable. The coprocessor's internal flash storage memory should show up in the filesystem with the files `main.py` and `boot.py`
  
## Executing the firmware
To run the firmware, copy all of the files in the Copro/ dirctory over to the coprocessor. Note: the dependency uasyncio is required. TODO: add the required deps to a folder and include it with this repo.
